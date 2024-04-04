// Import REFramework::API
using System;
using System.Collections;
using System.Dynamic;
using System.Reflection;
using app;

public class DangerousFunctions {
    public static REFrameworkNET.PreHookResult isInsidePreHook(System.Object args) {
        //Console.WriteLine("Inside pre hook (From C#) " + args.ToString());
        REFrameworkNET.API.LogInfo("isInsidePreHook");
        return REFrameworkNET.PreHookResult.Continue;
    }

    public static void isInsidePostHook(ref System.Object retval) {
        Console.WriteLine("Inside post hook (From C#)");
    }

    public static void Entry() {
        var tdb = REFrameworkNET.API.GetTDB();
        /*tdb.GetType("app.CameraManager")?.
            GetMethod("isInside")?.
            AddHook(false).
            AddPre(isInsidePreHook).
            AddPost(isInsidePostHook);*/
        
        // These via.SceneManager and via.Scene are
        // loaded from an external reference assembly
        // the classes are all interfaces that correspond to real in-game classes
        var sceneManager = REFrameworkNET.API.GetNativeSingletonT<via.SceneManager>();
        var scene = sceneManager.get_CurrentScene();
        var scene2 = sceneManager.get_CurrentScene();

        if (scene == scene2) {
            REFrameworkNET.API.LogInfo("Test success: Scene is the same");
        } else {
            REFrameworkNET.API.LogError("Test failure: Scene is not the same");
        }

        //scene.set_Pause(true);
        var view = sceneManager.get_MainView();
        var name = view.get_Name();
        var go = view.get_PrimaryCamera()?.get_GameObject()?.get_Transform()?.get_GameObject();

        REFrameworkNET.API.LogInfo("game object name: " + go?.get_Name().ToString());
        REFrameworkNET.API.LogInfo("Scene name: " + name);

        // Testing autocomplete for the concrete ManagedObject
        REFrameworkNET.API.LogInfo("Scene: " + scene.ToString() + ": " + (scene as REFrameworkNET.IObject).GetTypeDefinition()?.GetFullName()?.ToString());

        // Testing dynamic invocation
        /*float currentTimescale = scene.get_TimeScale();
        scene.set_TimeScale(0.1f);

        REFrameworkNET.API.LogInfo("Previous timescale: " + currentTimescale.ToString());
        REFrameworkNET.API.LogInfo("Current timescale: " + scene?.get_TimeScale().ToString());*/

        var appdomainStatics = tdb.GetType("System.AppDomain").As<_System.AppDomain>();
        var appdomain = appdomainStatics.get_CurrentDomain();
        dynamic assemblies = appdomain.GetAssemblies();

        foreach (REFrameworkNET.ManagedObject assemblyRaw in assemblies) {
            var assembly = assemblyRaw.As<_System.Reflection.Assembly>();
            REFrameworkNET.API.LogInfo("Assembly: " + assembly.get_Location()?.ToString());
        }

        via.os os = tdb.GetType("via.os").As<via.os>();
        var platform = os.getPlatform();
        var platformSubset = os.getPlatformSubset();
        var title = os.getTitle();

        REFrameworkNET.API.LogInfo("Platform: " + platform);
        REFrameworkNET.API.LogInfo("Platform Subset: " + platformSubset);
        REFrameworkNET.API.LogInfo("Title: " + title);

        var dialog = tdb.GetTypeT<via.os.dialog>();
        dialog.open("Hello from C#!");

        var devUtil = tdb.GetTypeT<via.sound.dev.DevUtil>();
        var currentDirectory = System.IO.Directory.GetCurrentDirectory();
        devUtil.writeLogFile("what the heck", currentDirectory + "\\what_the_frick.txt");
        
        var stringTest = REFrameworkNET.VM.CreateString("Hello from C#!"); // inside RE Engine VM
        var stringInDotNetVM = stringTest.ToString(); // Back in .NET

        REFrameworkNET.API.LogInfo("Managed string back in .NET: " + stringInDotNetVM);

        var devUtilT = (devUtil as REFrameworkNET.IObject).GetTypeDefinition();
        var dialogT = (dialog as REFrameworkNET.IObject).GetTypeDefinition();

        REFrameworkNET.API.LogInfo("DevUtil: " + devUtilT.GetFullName());
        REFrameworkNET.API.LogInfo("Dialog: " + dialogT.GetFullName());
    }

    public static void TryEnableFrameGeneration() {
        var upscalingInterface = REFrameworkNET.API.GetNativeSingletonT<via.render.UpscalingInterface>();
        var dlssInterface = upscalingInterface.get_DLSSInterface();

        if (dlssInterface != null && dlssInterface.get_DLSSGEnable() == false) {
            dlssInterface.set_DLSSGEnable(true);
        }

        var fsr3Interface = upscalingInterface.get_FSR3Interface();

        if (fsr3Interface != null && fsr3Interface.get_EnableFrameGeneration() == false) {
            fsr3Interface.set_EnableFrameGeneration(true);
        }
    }
}

class REFrameworkPlugin {
    // Measure time between pre and post
    // get time
    static System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    static System.Diagnostics.Stopwatch sw2 = new System.Diagnostics.Stopwatch();

    // To be called when the AssemblyLoadContext is unloading the assembly
    public static void OnUnload() {
        REFrameworkNET.API.LogInfo("Unloading Test");
    }

    public static void TestCallbacks() {
        REFrameworkNET.Callbacks.BeginRendering.Pre += () => {
            sw.Start();
        };
        REFrameworkNET.Callbacks.BeginRendering.Post += () => {
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
        };

        REFrameworkNET.Callbacks.EndRendering.Post += () => {
            if (!sw2.IsRunning) {
                sw2.Start();
            }

            if (sw2.ElapsedMilliseconds >= 5000) {
                sw2.Restart();
                Console.WriteLine("EndRendering");
            }
        };

        REFrameworkNET.Callbacks.FinalizeRenderer.Pre += () => {
            Console.WriteLine("Finalizing Renderer");
        };

        REFrameworkNET.Callbacks.PrepareRendering.Post += () => {
        };
    }

    [REFrameworkNET.Attributes.PluginEntryPoint]
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

        TestCallbacks();

        var tdb = REFrameworkNET.API.GetTDB();

        REFrameworkNET.API.LogInfo(tdb.GetNumTypes().ToString() + " types");

        for (uint i = 0; i < 50; i++) {
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
        }

        REFrameworkNET.API.LogInfo("Done with types");

        var singletons = REFrameworkNET.API.GetManagedSingletons();

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
        }

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
        dynamic fadeOwnerCmn = guiManager?.FadeOwnerCmn;
        dynamic optionData = guiManager?.OptionData;
        dynamic optionDataFromGet = guiManager?.getOptionData();
        bool? isDispSubtitle = optionData?._IsDispSubtitle;

        REFrameworkNET.API.LogInfo("GuiManager: " + guiManager.ToString() + " @ " + guiManagerAddress.ToString("X"));
        REFrameworkNET.API.LogInfo(" FadeOwnerCmn: " + fadeOwnerCmn.ToString());
        REFrameworkNET.API.LogInfo(" OptionData: " + optionData.ToString() + ": " + optionData?.GetTypeDefinition()?.GetFullName()?.ToString());
        REFrameworkNET.API.LogInfo(" OptionDataFromGet: " + optionDataFromGet.ToString() + ": " + optionDataFromGet?.GetTypeDefinition()?.GetFullName()?.ToString());
        REFrameworkNET.API.LogInfo(" OptionDataFromGet same: " + (optionData?.Equals(optionDataFromGet)).ToString() + (" {0} vs {1}", optionData?.GetAddress().ToString("X"), optionDataFromGet?.GetAddress().ToString("X")));
        
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