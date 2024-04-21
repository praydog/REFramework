using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;

public class TestRE2Plugin {
    static bool IsRunningRE2 => Environment.ProcessPath.Contains("re2", StringComparison.CurrentCultureIgnoreCase);

    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static void Main() {
        if (IsRunningRE2) {
            Console.WriteLine("Running in RE2");
        } else {
            Console.WriteLine("Not running in RE2");
            return;
        }

        RE2HookBenchmark.InstallHook();

        ImGuiRender.Pre += () => {
            if (ImGui.Begin("Test Window")) {
                ImGui.Text("RE2");
                ImGui.Separator();

                ImGui.Text("Benchmark average: " + RE2HookBenchmark.RunningAvg.ToString("0.000") + " µs");
                ImGui.Text("Benchmark count: " + RE2HookBenchmark.MeasureCount);

                ImGui.PlotLines("Benchmark", ref RE2HookBenchmark.BenchmarkData[0], 1000, 0, "µs", 0, (float)RE2HookBenchmark.HighestMicros, new System.Numerics.Vector2(0, 80));

                if (ImGui.TreeNode("Player")) {
                    var playerManager = API.GetManagedSingletonT<app.ropeway.PlayerManager>();
                    var player = playerManager.get_CurrentPlayer();
                    if (player != null) {
                        ImGui.Text("Player is not null");
                    } else {
                        ImGui.Text("Player is null");
                    }
                    ImGui.TreePop();
                }

                ImGui.End();
            }
        };

        // Benchmarking the effects of threading on invoking game code
        for (int i = 0; i < 8; ++i) {
            threads.Add(new System.Threading.Thread(() => {
                while (!cts.Token.IsCancellationRequested) {
                    RE2HookBenchmark.Bench(BenchFnAction);
                    // We must manually call the GC in our own threads not owned by the game
                    API.LocalFrameGC();
                }
            }));
        }

        foreach (var thread in threads) {
            thread.Start();
        }
    }

    [REFrameworkNET.Attributes.PluginExitPoint]
    public static void Unload() {
        cts.Cancel();
        foreach (var thread in threads) {
            thread.Join();
        }
    }

    public static void BenchFn() {
        var playerManager = API.GetManagedSingletonT<app.ropeway.PlayerManager>();
        var player = playerManager.get_CurrentPlayer();
        if (player != null) {
            var playerController = player.getComponent(app.Collision.HitController.REFType.GetRuntimeType().As<_System.Type>());
            if (playerController != null) {
                for (int i = 0; i < 10000; ++i) {
                    get_GameObjectFn.Invoke(playerController, null);
                }
            }
        }
    }

    public static System.Action BenchFnAction = BenchFn;

    static Method get_GameObjectFn = via.Component.REFType.GetMethod("get_GameObject");

    static List<System.Threading.Thread> threads = new();

    static System.Threading.CancellationTokenSource cts = new();
}

public class RE2HookBenchmark {
    public static int MeasureCount { get; private set; }
    static int callCount = 0;
    static double totalMicros = 0.0;
    public static double RunningAvg { get; private set; }

    public static double HighestMicros { get; private set; } = 0.0;
    public static float[] BenchmarkData { get; private set; } = new float[1000];
    
    static System.Threading.ReaderWriterLockSlim rwl = new();

    public static void Bench(System.Action action) {
        var sw = _System.Diagnostics.Stopwatch.REFType.CreateInstance(0).As<_System.Diagnostics.Stopwatch>();
        //var sw2 = new System.Diagnostics.Stopwatch();
        sw.Start();

        action();

        sw.Stop();
        var elapsedTicks = (double)sw.get_ElapsedTicks();

        rwl.EnterWriteLock();

        callCount++;

        if (callCount >= 5) {
            var elapsedMicros = elapsedTicks / (double)TimeSpan.TicksPerMicrosecond;

            totalMicros += elapsedMicros;
            MeasureCount++;
            RunningAvg = totalMicros / MeasureCount;
            BenchmarkData[callCount % 1000] = (float)elapsedMicros;

            if (elapsedMicros > HighestMicros) {
                HighestMicros = elapsedMicros;
            }
        }
        
        if (MeasureCount >= 1000) {
            MeasureCount = 0;
            totalMicros = 0.0;
        }

        rwl.ExitWriteLock();
    }

    static PreHookResult Pre(System.Span<ulong> args) {
        var hitController = ManagedObject.ToManagedObject(args[1]).As<app.Collision.HitController>();

        /*Bench(() => {
            hitController.get_GameObject();
        });*/

        return PreHookResult.Continue;
    }

    static void Post(ref ulong retval) {
    }

    public static void InstallHook() {
        app.Collision.HitController.REFType
            .GetMethod("update")
            .AddHook(false)
            .AddPre(Pre)
            .AddPost(Post);
    }
}