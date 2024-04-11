using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;

public class DangerousFunctions {
    public static REFrameworkNET.PreHookResult isInsidePreHook(Span<ulong> args) {
        //Console.WriteLine("Inside pre hook (From C#) " + args.ToString());
        REFrameworkNET.API.LogInfo("isInsidePreHook");
        return REFrameworkNET.PreHookResult.Continue;
    }

    public static void isInsidePostHook(ref System.Object retval) {
        Console.WriteLine("Inside post hook (From C#)");
    }

    public static void Entry() {
        var tdb = REFrameworkNET.API.GetTDB();
        tdb.GetType("app.CameraManager")?.
            GetMethod("isInside")?.
            AddHook(false).
            AddPre(isInsidePreHook).
            AddPost(isInsidePostHook);
        
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

        var mouse = REFrameworkNET.API.GetNativeSingletonT<via.hid.Mouse>();

        mouse.set_ShowCursor(false);
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

class ObjectExplorer {
    static System.Numerics.Vector4 TYPE_COLOR = new System.Numerics.Vector4(78.0f / 255.0f, 201 / 255.0f, 176 / 255.0f, 1.0f);
    static System.Numerics.Vector4 FIELD_COLOR = new(156 / 255.0f, 220 / 255.0f, 254 / 255.0f, 1.0f);
    static System.Numerics.Vector4 METHOD_COLOR = new(220 / 255.0f, 220 / 255.0f, 170 / 255.0f, 1.0f);

    public static void DisplayColorPicker() {
        ImGui.ColorEdit4("Type Color", ref TYPE_COLOR);
        ImGui.ColorEdit4("Field Color", ref FIELD_COLOR);
        ImGui.ColorEdit4("Method Color", ref METHOD_COLOR);
    }

    public static void DisplayField(REFrameworkNET.IObject obj, REFrameworkNET.Field field) {
        var t = field.GetType();
        string tName = t != null ? t.GetFullName() : "null";

        var unified = obj != null ? obj as REFrameworkNET.UnifiedObject : null;
        ulong address = unified != null ? unified.GetAddress() : 0;

        if (field.IsStatic()) {
            address = field.GetDataRaw(obj.GetAddress(), false);
        } else if (obj != null) {
            address += field.GetOffsetFromBase();
        }

        // Make a tree node that spans the entire width of the window
        ImGui.PushID(address.ToString("X"));
        ImGui.SetNextItemOpen(false, ImGuiCond.Once);
        var made = ImGui.TreeNodeEx("", ImGuiTreeNodeFlags.SpanFullWidth);
        
        ImGui.SameLine();
        //ImGui.Text(" " + tName);
        ImGui.TextColored(TYPE_COLOR, " " + tName);

        ImGui.SameLine();

        ImGui.TextColored(FIELD_COLOR, field.GetName());

        ImGui.SameLine();

        if (field.IsStatic()) {
            // Red text
            ImGui.TextColored(new System.Numerics.Vector4(0.75f, 0.2f, 0.0f, 1.0f), "Static");
        } else {
            ImGui.Text("0x" + field.GetOffsetFromBase().ToString("X"));
        }

        if (obj == null) {
            if (made) {
                ImGui.Text("Value: null");
                ImGui.TreePop();
            }

            ImGui.PopID();
            return;
        }

        if (made) {
            if (!t.IsValueType()) {
                var objValue = obj.GetField(field.GetName()) as REFrameworkNET.IObject;

                if (objValue != null) {
                    DisplayObject(objValue);
                } else {
                    ImGui.Text("Value: null");
                }
            } else {
                switch (tName) {
                    case "System.Int32":
                        ImGui.Text("Value: " + field.GetDataT<int>(obj.GetAddress(), false).ToString());
                        break;
                    case "System.UInt32":
                        ImGui.Text("Value: " + field.GetDataT<uint>(obj.GetAddress(), false).ToString());
                        break;
                    case "System.Int64":
                        ImGui.Text("Value: " + field.GetDataT<long>(obj.GetAddress(), false).ToString());
                        break;
                    case "System.UInt64":
                        ImGui.Text("Value: " + field.GetDataT<ulong>(obj.GetAddress(), false).ToString());
                        break;
                    case "System.Single":
                        ImGui.Text("Value: " + field.GetDataT<float>(obj.GetAddress(), false).ToString());
                        break;
                    case "System.Boolean":
                        ImGui.Text("Value: " + field.GetDataT<bool>(obj.GetAddress(), false).ToString());
                        break;
                    /*case "System.String":
                        ImGui.Text("Value: " + field.GetDataT<string>(obj.GetAddress(), false));
                        break;*/
                    default:
                        ImGui.Text("Value: " + field.GetDataRaw(obj.GetAddress(), false).ToString("X"));
                        break;
                }
            }

            ImGui.TreePop();
        }

        ImGui.PopID();
    }

    public static void DisplayMethod(REFrameworkNET.IObject obj, REFrameworkNET.Method method) {
        var returnT = method.GetReturnType();
        var returnTName = returnT != null ? returnT.GetFullName() : "null";

        //ImGui.Text(" " + returnTName);
        ImGui.TextColored(TYPE_COLOR, " " + returnTName);
        ImGui.SameLine(0.0f, 0.0f);

        ImGui.TextColored(METHOD_COLOR, " " + method.GetName());


        ImGui.SameLine(0.0f, 0.0f);
        ImGui.Text("(");
        
        var ps = method.GetParameters();

        if (ps.Count > 0) {
            for (int i = 0; i < ps.Count; i++) {
                var p = ps[i];

                ImGui.SameLine(0.0f, 0.0f);
                ImGui.TextColored(TYPE_COLOR, p.Type.GetFullName());

                if (p.Name != null && p.Name.Length > 0) {
                    ImGui.SameLine(0.0f, 0.0f);
                    ImGui.Text(" " + p.Name);
                }

                if (i < ps.Count - 1) {
                    ImGui.SameLine(0.0f, 0.0f);
                    ImGui.Text(", ");
                }
                //postfix += p.Type.GetFullName() + " " + p.Name + ", ";
            }

            ImGui.SameLine(0.0f, 0.0f);
            ImGui.Text(")");

            //postfix = postfix.Substring(0, postfix.Length - 3);
        } else {
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.Text(")");
        }
    }

    public static void DisplayType(REFrameworkNET.TypeDefinition t) {
        ImGui.Text("Name: " + t.GetFullName());
        ImGui.Text("Namespace: " + t.GetNamespace());

        if (t.DeclaringType != null) {
            var made = ImGui.TreeNodeEx("Declaring Type: ", ImGuiNET.ImGuiTreeNodeFlags.SpanFullWidth);
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.TextColored(TYPE_COLOR, t.DeclaringType.GetFullName());
            if (made) {
                DisplayType(t.DeclaringType);
                ImGui.TreePop();
            }
        }

        if (t.ParentType != null) {
            var made = ImGui.TreeNodeEx("Parent Type: ", ImGuiNET.ImGuiTreeNodeFlags.SpanFullWidth);
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.TextColored(TYPE_COLOR, t.ParentType.GetFullName());
            if (made) {
                DisplayType(t.ParentType);
                ImGui.TreePop();
            }
        }
        
        var runtimeTypeRaw = t.GetRuntimeType();

        if (runtimeTypeRaw != null) {
            var runtimeType = runtimeTypeRaw.As<_System.Type>();
            var assembly = runtimeType.get_Assembly();

            if (assembly != null) {
                if (ImGui.TreeNode("Assembly: " + assembly.get_FullName().Split(',')[0])) {
                    DisplayObject(assembly as IObject);
                    ImGui.TreePop();
                }
            }

            var baseType = runtimeType.get_BaseType();

            /*if (baseType != null) {
                if (ImGui.TreeNode("Base Type (" + (baseType.get_TypeHandle() as REFrameworkNET.TypeDefinition).FullName + ")")) {
                    DisplayObject(baseType as IObject);
                    ImGui.TreePop();
                }
            }*/

            if (ImGui.TreeNode("Runtime Type")) {
                DisplayObject(runtimeType as IObject);
                ImGui.TreePop();
            }
        }
    }

    public static void DisplayObject(REFrameworkNET.IObject obj) {
        if (ImGui.TreeNode("Type Info")) {
            DisplayType(obj.GetTypeDefinition());
            ImGui.TreePop();
        }

        if (ImGui.TreeNode("Methods")) {
            var methods = obj.GetTypeDefinition().GetMethods();

            // Sort methods by name
            methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            foreach (var method in methods) {
                DisplayMethod(obj, method);
            }
        }

        if (ImGui.TreeNode("Fields")) {
            var fields = obj.GetTypeDefinition().GetFields();

            // Sort fields by name
            fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            foreach (var field in fields) {
                DisplayField(obj, field);
            }

            ImGui.TreePop();
        }
    }

    public static void RenderNativeSingletons() {
        var singletons = REFrameworkNET.API.GetNativeSingletons();

        // Sort by type name
        singletons.Sort((a, b) => a.Instance.GetTypeDefinition().GetFullName().CompareTo(b.Instance.GetTypeDefinition().GetFullName()));

        foreach (var singletonDesc in singletons) {
            var singleton = singletonDesc.Instance;
            if (singleton == null) {
                continue;
            }
            var singletonName = singleton.GetTypeDefinition().GetFullName();

            if (ImGui.TreeNode(singletonName)) {
                DisplayObject(singleton);
                ImGui.TreePop();
            }
        }
    }

    public static void RenderManagedSingletons() {
        var singletons = REFrameworkNET.API.GetManagedSingletons();

        foreach (var singletonDesc in singletons) {
            var singleton = singletonDesc.Instance;
            if (singleton == null) {
                continue;
            }
            var singletonName = singleton.GetTypeDefinition().GetFullName();

            if (ImGui.TreeNode(singletonName)) {
                DisplayObject(singleton);
                ImGui.TreePop();
            }
        }
    }

    public static void Render() {
        ImGui.SetNextItemOpen(true, ImGuiCond.Once);
        if (ImGui.TreeNode("Color Picker")) {
            DisplayColorPicker();
            ImGui.TreePop();
        }


        try {
            if (ImGui.TreeNode("Managed Singletons")) {
                RenderManagedSingletons();
                ImGui.TreePop();
            }

            if (ImGui.TreeNode("Native Singletons")) {
                RenderNativeSingletons();
            }

            DisplayType(REFrameworkNET.API.GetTDB().GetType("JsonParser.Value"));
        } catch (Exception e) {
            System.Console.WriteLine(e.ToString());
        }
    }
} // class ObjectExplorer

class REFrameworkPlugin {
    // Measure time between pre and post
    // get time
    static System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    static System.Diagnostics.Stopwatch sw2 = new System.Diagnostics.Stopwatch();

    // To be called when the AssemblyLoadContext is unloading the assembly
    public static void OnUnload() {
        REFrameworkNET.API.LogInfo("Unloading Test");
    }

    // Assigned in a callback below.
    public static void RenderImGui() {
        if (ImGui.Begin("Test Window")) {
            ImGui.SetNextItemOpen(true, ImGuiCond.Once);
            if (ImGui.TreeNode("Object Explorer")) {
                ObjectExplorer.Render();

                ImGui.TreePop();
            }

            ImGui.End();
        }
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

        REFrameworkNET.Callbacks.ImGuiRender.Pre += RenderImGui;
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