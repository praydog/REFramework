using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using app;
using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Attributes;
using REFrameworkNET.Callbacks;

public class DangerousFunctions {
    [MethodHook(typeof(app.CameraManager), nameof(app.CameraManager.isInside), MethodHookType.Pre)]
    public static REFrameworkNET.PreHookResult isInsidePreHook(Span<ulong> args) {
        //Console.WriteLine("Inside pre hook (From C#) " + args.ToString());
        //REFrameworkNET.API.LogInfo("isInsidePreHook");
        return REFrameworkNET.PreHookResult.Continue;
    }

    [MethodHook(typeof(app.CameraManager), nameof(app.CameraManager.isInside), MethodHookType.Post)]
    public static void isInsidePostHook(ref ulong retval) {
        if ((retval & 1) != 0) {
            //REFrameworkNET.API.LogInfo("Camera is inside");
        }
        //Console.WriteLine("Inside post hook (From C#), retval: " + (retval & 1).ToString());
    }

    [MethodHook(typeof(app.PlayerInputProcessorDetail), nameof(app.PlayerInputProcessorDetail.processNormalAttack), MethodHookType.Pre)]
    public static REFrameworkNET.PreHookResult processNormalAttackPreHook(Span<ulong> args) {
        var inputProcessor = ManagedObject.ToManagedObject(args[1]).As<app.PlayerInputProcessorDetail>();
        var asIObject = inputProcessor as REFrameworkNET.IObject;
        ulong flags = args[2];
        bool isCombo = (args[4] & 1) != 0;
        API.LogInfo("processNormalAttack: " + 
            inputProcessor.ToString() + " " + 
            asIObject.GetTypeDefinition()?.GetFullName()?.ToString() + " " +
            flags.ToString() + " " +
            isCombo.ToString());
        return PreHookResult.Continue;
    }

    public static void Entry() {
        //via.render.RayTracingManager.set_EnableLod(false);
        //via.render.RayTracingManager.set_PreferShadowCast(true);
        via.render.RayTracingManager.EnableLod = false;
        via.render.RayTracingManager.PreferShadowCast = true;

        // Print the current value
        Console.WriteLine("RayTracingManager.EnableLod: " + via.render.RayTracingManager.EnableLod.ToString());
        Console.WriteLine("RayTracingManager.PreferShadowCast: " + via.render.RayTracingManager.PreferShadowCast.ToString());

        //via.hid.Mouse.set_ShowCursor(false);
        via.hid.Mouse.ShowCursor = false;

        var tdb = REFrameworkNET.API.GetTDB();
        
        // These via.SceneManager and via.Scene are
        // loaded from an external reference assembly
        // the classes are all interfaces that correspond to real in-game classes
        //var sceneManager = API.GetNativeSingletonT<via.SceneManager>();
        //var scene = sceneManager.get_CurrentScene();
        //var scene2 = sceneManager.get_CurrentScene();
        //var scene = via.SceneManager.get_CurrentScene();
        //var scene2 = via.SceneManager.get_CurrentScene();
        var scene = via.SceneManager.CurrentScene;
        var scene2 = via.SceneManager.CurrentScene;

        if (scene == scene2) {
            REFrameworkNET.API.LogInfo("Test success: Scene is the same");
        } else {
            REFrameworkNET.API.LogError("Test failure: Scene is not the same");
        }

        //scene.set_Pause(true);
        //var view = sceneManager.get_MainView();
        var view = via.SceneManager.MainView;
        var name = view.Name;
        var go = view.PrimaryCamera?.GameObject?.Transform?.GameObject;

        REFrameworkNET.API.LogInfo("game object name: " + go?.Name.ToString());
        REFrameworkNET.API.LogInfo("Scene name: " + name);

        // Testing autocomplete for the concrete ManagedObject
        REFrameworkNET.API.LogInfo("Scene: " + scene.ToString() + ": " + (scene as REFrameworkNET.IObject).GetTypeDefinition()?.GetFullName()?.ToString());

        // Testing dynamic invocation
        /*float currentTimescale = scene.get_TimeScale();
        scene.set_TimeScale(0.1f);

        REFrameworkNET.API.LogInfo("Previous timescale: " + currentTimescale.ToString());
        REFrameworkNET.API.LogInfo("Current timescale: " + scene?.get_TimeScale().ToString());*/

        var appdomain = _System.AppDomain.CurrentDomain;
        var assemblies = appdomain.GetAssemblies();

        //foreach (REFrameworkNET.ManagedObject assemblyRaw in assemblies) {
        for (int i = 0; i < assemblies.Length; i++) {
            //var assembly = assemblyRaw.As<_System.Reflection.Assembly>();
            var assembly = assemblies[i];
            REFrameworkNET.API.LogInfo("Assembly: " + assembly.Location?.ToString());
        }

        var platform = via.os.getPlatform();
        var platformSubset = via.os.getPlatformSubset();
        var title = via.os.getTitle();

        REFrameworkNET.API.LogInfo("Platform: " + platform);
        REFrameworkNET.API.LogInfo("Platform Subset: " + platformSubset);
        REFrameworkNET.API.LogInfo("Title: " + title);

        var dialogError = via.os.dialog.open("Hello from C#!");

        REFrameworkNET.API.LogInfo("Dialog error: " + dialogError.ToString());

        var currentDirectory = System.IO.Directory.GetCurrentDirectory();
        via.sound.dev.DevUtil.writeLogFile("what the heck", currentDirectory + "\\what_the_frick.txt");
        
        var stringTest = REFrameworkNET.VM.CreateString("Hello from C#!"); // inside RE Engine VM
        var stringInDotNetVM = stringTest.ToString(); // Back in .NET

        REFrameworkNET.API.LogInfo("Managed string back in .NET: " + stringInDotNetVM);
        
        //var range = via.RangeI.REFType.CreateInstance(0).As<via.RangeI>();
        var range = REFrameworkNET.ValueType.New<via.RangeI>();
        var testVec = REFrameworkNET.ValueType.New<via.vec3>();

        System.Console.WriteLine("Test vec before: " + testVec.x + " " + testVec.y + " " + testVec.z);

        testVec.x = 1.0f;
        testVec.y = 2.0f;
        testVec.z = 3.0f;

        System.Console.WriteLine("Test vec after: " + testVec.x + " " + testVec.y + " " + testVec.z);

        var axisXStatic = via.vec3.AxisX;
        var axisYStatic = via.vec3.AxisY;
        var axisZStatic = via.vec3.AxisZ;

        System.Console.WriteLine("Axis X: " + axisXStatic.x + " " + axisXStatic.y + " " + axisXStatic.z);
        System.Console.WriteLine("Axis Y: " + axisYStatic.x + " " + axisYStatic.y + " " + axisYStatic.z);
        System.Console.WriteLine("Axis Z: " + axisZStatic.x + " " + axisZStatic.y + " " + axisZStatic.z);
        
        //testVec[0] = 1.0f;
        // print min max to test if this works
        range.setMinMax(1, 10);
        REFrameworkNET.API.LogInfo("Range min: " + range.getMin().ToString());
        REFrameworkNET.API.LogInfo("Range max: " + range.getMax().ToString());

        var meshes = via.SceneManager.MainScene.findComponents(via.render.Mesh.REFType.RuntimeType.As<_System.Type>());
        for (int i = 0; i < meshes.Length; i++) {
            var mesh = (meshes[i] as IObject).As<via.render.Mesh>();
            mesh.DrawRaytracing = true;
        }

        var characterManager = API.GetManagedSingletonT<app.CharacterManager>();
        if (characterManager.ManualPlayer != null) {
            var playergo = characterManager.ManualPlayer.GameObject;
            var transform = playergo.Transform;
            var position = transform.Position;

            REFrameworkNET.API.LogInfo("Player position: " + position.x + " " + position.y + " " + position.z);

            position.y += 5.0f;
            transform.Position = position;
        }
    }

    public static void TryEnableFrameGeneration() {
        var dlssInterface = via.render.UpscalingInterface.DLSSInterface;

        if (dlssInterface != null && dlssInterface.DLSSGEnable == false) {
            dlssInterface.DLSSGEnable = true;
        }

        var fsr3Interface = via.render.UpscalingInterface.FSR3Interface;

        if (fsr3Interface != null && fsr3Interface.EnableFrameGeneration == false) {
            fsr3Interface.EnableFrameGeneration = true;
        }
    }
}
class REFrameworkPlugin {
    // Measure time between pre and post
    // get time
    static System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    static System.Diagnostics.Stopwatch sw2 = new System.Diagnostics.Stopwatch();

    static bool doFullGC = false;

    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void RenderImGui() {
        if (ImGui.Begin("Test Window")) {
            // Debug info about GC state
            if (ImGui.TreeNode(".NET Debug")) {
                ImGui.Checkbox("Do Full GC", ref doFullGC);

                if (ImGui.Button("Do Gen 2 GC")) {
                    GC.Collect(2, GCCollectionMode.Forced, false);
                }

                var heapBytes = GC.GetTotalMemory(false);
                var heapKb = heapBytes / 1024;
                var heapMb = heapKb / 1024;
                var heapGb = heapMb / 1024;

                if (doFullGC) {
                    GC.Collect(0, GCCollectionMode.Forced, false);
                    GC.Collect(1, GCCollectionMode.Forced, false);
                    GC.Collect(2, GCCollectionMode.Forced, false);
                }

                ImGui.Text("GC Memory: " + GC.GetTotalMemory(false) / 1024 + " KB (" + heapMb + " MB, " + heapGb + " GB)");
                ImGui.Text("GC Collection Count (gen 0): " + GC.CollectionCount(0));
                ImGui.Text("GC Collection Count (gen 1): " + GC.CollectionCount(1));
                ImGui.Text("GC Collection Count (gen 2): " + GC.CollectionCount(2));
                ImGui.Text("GC Latency Mode: " + System.Runtime.GCSettings.LatencyMode.ToString());
                ImGui.Text("GC Is Server GC: " + System.Runtime.GCSettings.IsServerGC);
                ImGui.Text("GC Max Generation: " + GC.MaxGeneration);
                ImGui.Text("GC Force Full Collection: " + System.Runtime.GCSettings.LargeObjectHeapCompactionMode);
                
                // memory info
                var memoryInfo = GC.GetGCMemoryInfo();
                ImGui.Text("GC Is Concurrent: " + memoryInfo.Concurrent);
                ImGui.Text("GC Fragmentation: " + memoryInfo.FragmentedBytes);

                var gcHeapSize = GC.GetGCMemoryInfo().HeapSizeBytes;
                var gcHeapSizeKb = gcHeapSize / 1024;
                var gcHeapSizeMb = gcHeapSizeKb / 1024;
                var gcHeapSizeGb = gcHeapSizeMb / 1024;
                ImGui.Text("GC Heap Size During Last GC: " + memoryInfo.HeapSizeBytes + " bytes (" + gcHeapSizeKb + " KB, " + gcHeapSizeMb + " MB, " + gcHeapSizeGb + " GB)");
                ImGui.Text("GC High Memory Load Threshold: " + memoryInfo.HighMemoryLoadThresholdBytes);
                ImGui.Text("GC Memory Load: " + memoryInfo.MemoryLoadBytes);

                // Combo box for latency mode
                // Turn enum into string
                var latencyModeEnum = typeof(System.Runtime.GCLatencyMode);
                var latencyModeNames = Enum.GetNames(latencyModeEnum);

                int currentLatencyMode = (int)System.Runtime.GCSettings.LatencyMode;

                if (ImGui.BeginCombo("Latency Mode", latencyModeNames[currentLatencyMode])) {
                    for (int i = 0; i < latencyModeNames.Length; i++) {
                        bool isSelected = i == currentLatencyMode;

                        if (ImGui.Selectable(latencyModeNames[i], isSelected)) {
                            System.Runtime.GCSettings.LatencyMode = (System.Runtime.GCLatencyMode)i;
                        }

                        if (isSelected) {
                            ImGui.SetItemDefaultFocus();
                        }
                    }

                    ImGui.EndCombo();
                }

                ImGui.TreePop();
            }

            ImGui.End();
        }
    }

    [Callback(typeof(BeginRendering), CallbackType.Pre)]
    static void BeginRenderingPre() {
        sw.Start();
    }

    [Callback(typeof(BeginRendering), CallbackType.Post)]
    static void BeginRenderingPost() {
        sw.Stop();

        if (sw.ElapsedMilliseconds >= 6) {
            Console.WriteLine("BeginRendering took " + sw.ElapsedMilliseconds + "ms");
        }

        /*try {
            DangerousFunctions.TryEnableFrameGeneration();
        } catch (Exception e) {
            REFrameworkNET.API.LogError(e.ToString());
        }*/

        sw.Reset();
    }

    [Callback(typeof(EndRendering), CallbackType.Post)]
    static void EndRenderingPost() {
        if (!sw2.IsRunning) {
            sw2.Start();
        }

        if (sw2.ElapsedMilliseconds >= 5000) {
            sw2.Restart();
            Console.WriteLine("EndRendering");
        }
    }

    [Callback(typeof(FinalizeRenderer), CallbackType.Pre)]
    static void FinalizeRendererPre() {
        Console.WriteLine("Finalizing Renderer");
    }

    [Callback(typeof(PrepareRendering), CallbackType.Post)]
    static void PrepareRenderingPost() {
    }

    // To be called when the AssemblyLoadContext is unloading the assembly
    [PluginExitPoint]
    public static void OnUnload() {
        REFrameworkNET.API.LogInfo("Unloading Test");
    }

    [PluginEntryPoint]
    public static void Main() { 
        try {
            MainImpl();
        } catch (Exception e) {
            REFrameworkNET.API.LogError(e.ToString());

            var ex = e;

            while (ex.InnerException != null) {
                ex = ex.InnerException;
                REFrameworkNET.API.LogError(ex.ToString());
            }
        }
    }

    public static void MainImpl() {
        REFrameworkNET.API.LogInfo("Testing REFrameworkAPI...");

        var tdb = REFrameworkNET.API.GetTDB();

        REFrameworkNET.API.LogInfo(tdb.GetNumTypes().ToString() + " types");

        /*for (uint i = 0; i < 50; i++) {
            var type = tdb.GetType(i);
            REFrameworkNET.API.LogInfo(type.GetFullName());

            var methods = type.GetMethods();

            foreach (var method in methods) {
                var returnT = method.GetReturnType();
                var returnTName = returnT != null ? returnT.GetFullName() : "null";

                REFrameworkNET.API.LogInfo(" " + returnTName + " " + method.GetName());
            }

            var fields = type.GetFields();

            foreach (var field in fields) {
                var t = field.GetType();
                string tName = t != null ? t.GetFullName() : "null";
                REFrameworkNET.API.LogInfo(" " + tName + " " + field.GetName() + " @ " + "0x" + field.GetOffsetFromBase().ToString("X"));
            }
        }*/

        REFrameworkNET.API.LogInfo("Done with types");

        /*var singletons = REFrameworkNET.API.GetManagedSingletons();

        foreach (var singletonDesc in singletons) {
            var singleton = singletonDesc.Instance;

            Console.WriteLine(singleton.GetTypeDefinition().GetFullName());
            var isManagedObject = REFrameworkNET.ManagedObject.IsManagedObject(singleton.GetAddress());

            Console.WriteLine(" Is managed object: " + isManagedObject.ToString());

            // Log all methods
            var td = singleton.GetTypeDefinition();
            var methods = td.GetMethods();

            foreach (var method in methods) {
                string postfix = "";
                foreach (var param in method.GetParameters()) {
                    postfix += param.Type.GetFullName() + " " + param.Name + ", ";
                }

                Console.WriteLine(" " + method.GetName() + " " + postfix);
            }

            var fields = td.GetFields();

            foreach (var field in fields) {
                Console.WriteLine(" " + field.GetName());
            }
        }*/

        try {
            DangerousFunctions.Entry();
        } catch (Exception e) {
            REFrameworkNET.API.LogError(e.ToString());
        }

        dynamic optionManager = REFrameworkNET.API.GetManagedSingleton("app.OptionManager");

        ulong optionManagerAddress = optionManager != null ? (optionManager as REFrameworkNET.ManagedObject).GetAddress() : 0;
        bool? hasAnySave = optionManager?._HasAnySave;

        REFrameworkNET.API.LogInfo("OptionManager: " + optionManager.ToString() + " @ " + optionManagerAddress.ToString("X"));
        REFrameworkNET.API.LogInfo("HasAnySave: " + hasAnySave.ToString());

        dynamic guiManager = REFrameworkNET.API.GetManagedSingleton("app.GuiManager");
        ulong guiManagerAddress = guiManager != null ? (guiManager as REFrameworkNET.ManagedObject).GetAddress() : 0;
        REFrameworkNET.API.LogInfo("GuiManager: " + guiManager.ToString() + " @ " + guiManagerAddress.ToString("X"));

        dynamic fadeOwnerCmn = guiManager?.FadeOwnerCmn;
        REFrameworkNET.API.LogInfo(" FadeOwnerCmn: " + fadeOwnerCmn.ToString());

        dynamic optionData = guiManager?.OptionData;
        REFrameworkNET.API.LogInfo(" OptionData: " + optionData.ToString() + ": " + optionData?.GetTypeDefinition()?.GetFullName()?.ToString());

        dynamic optionDataFromGet = guiManager?.getOptionData();
        REFrameworkNET.API.LogInfo(" OptionDataFromGet: " + optionDataFromGet.ToString() + ": " + optionDataFromGet?.GetTypeDefinition()?.GetFullName()?.ToString());
        REFrameworkNET.API.LogInfo(" OptionDataFromGet same: " + (optionData?.Equals(optionDataFromGet)).ToString() + (" {0} vs {1}", optionData?.GetAddress().ToString("X"), optionDataFromGet?.GetAddress().ToString("X")));

        bool? isDispSubtitle = optionData?._IsDispSubtitle;

        REFrameworkNET.API.LogInfo("  IsDispSubtitle: " + isDispSubtitle.ToString());

        if (optionData != null) {
            optionData._IsDispSubtitle = !isDispSubtitle;
            REFrameworkNET.API.LogInfo("  IsDispSubtitle: " + optionData?._IsDispSubtitle.ToString());
        }

        dynamic test = (guiManager as REFrameworkNET.ManagedObject)?.GetTypeDefinition()?.GetRuntimeType();

        if (test != null) {
            REFrameworkNET.API.LogInfo("GuiManager runtime type: " + test.ToString());

            // This is basically a System.Type, so lets get the assembly location
            REFrameworkNET.API.LogInfo("GuiManager runtime type assembly: " + test.get_Assembly());
            REFrameworkNET.API.LogInfo("GuiManager runtime type assembly name: " + test.get_Assembly().get_Location());
        }

        //IGUIManager proxyGuiManager = ManagedObjectProxy<IGUIManager>.Create(guiManager);
        //var proxyOptionData = proxyGuiManager.getOptionData();

        //REFrameworkNET.API.LogInfo("ProxyOptionData: " + proxyOptionData?.ToString() + ": " + proxyOptionData?.GetTypeDefinition()?.GetFullName()?.ToString());
        
        dynamic appdomainT = tdb.GetType("System.AppDomain");
        dynamic appdomain = appdomainT.get_CurrentDomain();
        dynamic assemblies = appdomain?.GetAssemblies();

        foreach (dynamic assembly in assemblies) {
            REFrameworkNET.API.LogInfo("Assembly: " + assembly.get_Location()?.ToString());
        }
    }
};