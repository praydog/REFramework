using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using System.Threading;
using app.ropeway;
using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;

public class TestRE2Plugin {
    static bool IsRunningRE2 => Environment.ProcessPath.Contains("re2", StringComparison.CurrentCultureIgnoreCase);
    static System.Diagnostics.Stopwatch imguiStopwatch = new();

    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        var deltaSeconds = imguiStopwatch.Elapsed.TotalMilliseconds / 1000.0;
        imguiStopwatch.Restart();

        if (ImGui.Begin("Test Window")) {
            ImGui.Text("RE2");
            ImGui.Separator();

            
            if (ImGui.TreeNode("Player")) {
                var playerManager = API.GetManagedSingletonT<app.ropeway.PlayerManager>();
                var player = playerManager.CurrentPlayer;

                if (player != null) {
                    ImGui.Text("Player is not null");
                } else {
                    ImGui.Text("Player is null");
                }
                ImGui.TreePop();
            }

            ImGui.End();
        }
        
        // ROund the window
        ImGui.PushStyleVar(ImGuiStyleVar.WindowRounding, 10.0f);

        ImGui.SetNextWindowSize(new System.Numerics.Vector2(500, 500), ImGuiCond.FirstUseEver);
        if (ImGui.Begin("RE2 Bench")) {
            ImGui.Text(RE2HookBenchmark.MeasureCount.ToString() + " calls");

            ImGui.PushStyleColor(ImGuiCol.Text, new System.Numerics.Vector4(0.5f, 1f, 0.4f, 1.0f));
            ImGui.PlotLines("Overall Benchmark", ref RE2HookBenchmark.BenchmarkData[0], 1000, RE2HookBenchmark.MeasureCount % 1000, RE2HookBenchmark.RunningAvg.ToString("0.000") + " µs", 0, (float)RE2HookBenchmark.RunningAvg * 2.0f, new System.Numerics.Vector2(0, 40));
            ImGui.PopStyleColor();

            System.Collections.Generic.List<long> threadRanks = new();

            foreach(var tdata in RE2HookBenchmark.threadData) {
                threadRanks.Add(tdata.Value.threadID);
            }

            // Sort by highest running average
            threadRanks.Sort((a, b) => {
                var aData = RE2HookBenchmark.threadData[a];
                var bData = RE2HookBenchmark.threadData[b];
                return aData.runningAvg.CompareTo(bData.runningAvg);
            });

            var totalThreadRanks = threadRanks.Count;

            System.Collections.Generic.List<RE2HookBenchmark.ThreadData> threadData = new();

            foreach(var tdata in RE2HookBenchmark.threadData) {
                threadData.Add(tdata.Value);
            }

            threadData.Sort((a, b) => {
                return a.lerp.CompareTo(b.lerp);
            });

            foreach(var tdata in threadData) {
                var rank = threadRanks.IndexOf(tdata.threadID) + 1;
                var rankRatio = (double)rank / (double)totalThreadRanks;
                var towards = Math.Max(80.0 * rankRatio, 20.0);
                tdata.lerp = towards * deltaSeconds + tdata.lerp * (1.0 - deltaSeconds);
                var lerpRatio = tdata.lerp / 80.0;

                var greenColor = 1.0 - lerpRatio;
                var redColor = lerpRatio;

                //ImGui.Text("Thread ID: " + tdata.Value.threadID + " Avg: " + tdata.Value.runningAvg.ToString("0.000") + " µs");
                ImGui.PushStyleColor(ImGuiCol.Text, new System.Numerics.Vector4((float)redColor, (float)greenColor, 0f, 1.0f));
                ImGui.PlotLines("Thread " + tdata.threadID, ref tdata.benchmarkData[0], 1000, tdata.callCount % 1000, tdata.runningAvg.ToString("0.000") + " µs", (float)tdata.runningAvg, (float)tdata.runningAvg * 2.0f, new System.Numerics.Vector2(0, (int)tdata.lerp));
                ImGui.PopStyleColor();
            }

            ImGui.End();
        }

        ImGui.PopStyleVar();
    }

    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static void Main() {
        if (IsRunningRE2) {
            Console.WriteLine("Running in RE2");
        } else {
            Console.WriteLine("Not running in RE2");
            return;
        }

        // Benchmarking the effects of threading on invoking game code
        for (int i = 0; i < 2; ++i) {
            threads.Add(new System.Threading.Thread(() => {
                /*while (!cts.Token.IsCancellationRequested) {
                    RE2HookBenchmark.Bench(BenchFnAction);
                    // We must manually call the GC in our own threads not owned by the game
                    API.LocalFrameGC();
                }*/

                API.LocalFrameGC();
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

    static System.Threading.ReaderWriterLockSlim rwl = new();
    static System.Collections.Concurrent.ConcurrentDictionary<long, bool> test = new(Environment.ProcessorCount * 2, 8192);
    public static void BenchFn() {
        var playerManager = API.GetManagedSingletonT<app.ropeway.PlayerManager>();
        var player = playerManager.CurrentPlayer;
        if (player != null) {
            via.Component playerControllerRaw = player.getComponent(app.Collision.HitController.REFType.GetRuntimeType().As<_System.Type>());
            if (playerControllerRaw != null) {
                var playerController = (playerControllerRaw as IObject).As<app.Collision.HitController>();
                for (int i = 0; i < 1; ++i) {
                    //rwl.EnterReadLock();
                    //rwl.ExitReadLock();
                    //playerController.
                    //playerController.get_DeltaTime();
                    var gameobj = playerController.GameObject;

                    if (gameobj != null) {
                        /*var backToPlayerController = gameobj.getComponent(app.Collision.HitController.REFType.GetRuntimeType().As<_System.Type>());

                        if (backToPlayerController != null) {
                            //System.Console.WriteLine("Back to player controller!!!");
                        }*/
                    }
                    //get_GameObjectFn.Invoke(playerController, null);
                    //object result = null;
                    //get_GameObjectFn.HandleInvokeMember_Internal(playerController, null, ref result);
                }

                var refCount = ((playerController.GameObject as IProxy).GetInstance() as ManagedObject).GetReferenceCount();
                System.Console.WriteLine("PlayerController ref count: " + refCount);
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

    internal class ThreadData {
        internal long threadID;
        internal double totalMicros;
        internal int callCount;
        internal double highestMicros;
        internal double runningAvg;
        internal float[] benchmarkData = new float[1000];
        internal double lerp = 40.0;
    };

    internal static System.Collections.Concurrent.ConcurrentDictionary<long, ThreadData> threadData = new(Environment.ProcessorCount * 2, 8192);

    public static void Bench(System.Action action) {
        var threadID = System.Threading.Thread.CurrentThread.ManagedThreadId;
        ThreadData? data = null;
        if (!threadData.ContainsKey(threadID)) {
            data = new ThreadData() { threadID = threadID };
            threadData.TryAdd(threadID, data);
        } else {
            threadData.TryGetValue(threadID, out data);
        }

        var sw = _System.Diagnostics.Stopwatch.REFType.CreateInstance(0).As<_System.Diagnostics.Stopwatch>();
        //var sw2 = new System.Diagnostics.Stopwatch();
        sw.Start();

        action();

        sw.Stop();
        var elapsedTicks = (double)sw.ElapsedTicks;
        var elapsedMicros = elapsedTicks / (double)TimeSpan.TicksPerMicrosecond;

        data.totalMicros += elapsedMicros;
        data.callCount++;

        if (elapsedMicros > data.highestMicros) {
            data.highestMicros = elapsedMicros;
        }
        
        data.runningAvg = data.totalMicros / (double)data.callCount;
        data.benchmarkData[data.callCount % 1000] = (float)elapsedMicros;

        if (data.callCount >= 1000) {
            data.callCount = 0;
            data.totalMicros = 0.0;
        }

        rwl.EnterWriteLock();

        callCount++;

        if (callCount >= 5) {
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
 
    [MethodHook(typeof(app.Collision.HitController), nameof(app.Collision.HitController.update), MethodHookType.Pre, false)]
    static PreHookResult Pre(Span<ulong> args) {
        var hitController = ManagedObject.ToManagedObject(args[1]).As<app.Collision.HitController>();

        Bench(() => {
            for (int i = 0; i < 10; ++i) {
                var gameobj = hitController.GameObject;
                if (gameobj != null) {
                }
            }
        });

        return PreHookResult.Continue;
    }

    [MethodHook(typeof(app.Collision.HitController), nameof(app.Collision.HitController.update), MethodHookType.Post, false)]
    static void Post(ref ulong retval) {

    }

    static double cameraFovLerp = 1;
    static double cameraFovLerpSpeed = 0.5;
    static System.Diagnostics.Stopwatch cameraFovStopwatch = new();
    static System.Diagnostics.Stopwatch cameraFovStopwatch2 = new();

    [Callback(typeof(LockScene), CallbackType.Post)]
    static void LockSceneCallback() {
        var playerManager = API.GetManagedSingletonT<app.ropeway.PlayerManager>();
        if (playerManager == null) {
            return;
        }

        var player = playerManager.CurrentPlayer;
        if (player == null) {
            return;
        }

        var transform = player.Transform;
        var headJoint = transform.getJointByName("head");

        if (headJoint == null) {
            return;
        }

        var camera = via.SceneManager.MainView.PrimaryCamera;
        camera.GameObject.Transform.Position = headJoint.Position;
        camera.GameObject.Transform.Joints[0].Position = headJoint.Position;

        // Shrink head scale
        headJoint.LocalScale = via.vec3.Zero;
    }
}