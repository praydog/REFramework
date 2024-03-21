// Import REFramework::API
using System;
using System.Dynamic;
using System.Reflection;

public interface IProxy {
    void SetInstance(dynamic instance);
}

// TODO: Put this in its own assembly, or make it part of C++/CLI?
public class Proxy<T, T2> : DispatchProxy, IProxy where T2 : class {
    public dynamic Instance { get; private set; }

    public void SetInstance(dynamic instance) {
        Instance = instance;
    }

    public static T Create(dynamic target) {
        var proxy = Create<T, Proxy<T, T2>>();
        (proxy as IProxy).SetInstance(target);
        return proxy;
    }

    protected override object Invoke(MethodInfo targetMethod, object[] args) {
        object result = null;
        dynamic obj = Instance as T2;
        obj.HandleInvokeMember_Internal(targetMethod.Name, args, ref result);

        if (targetMethod.ReturnType == typeof(REFrameworkNET.ManagedObject)) {
            return result;
        }

        if (targetMethod.ReturnType == typeof(REFrameworkNET.NativeObject)) {
            return result;
        }

        if (targetMethod.ReturnType == typeof(string)) {
            return result;
        }

        if (!targetMethod.ReturnType.IsPrimitive && targetMethod.DeclaringType.IsInterface) {
            if (result != null && result.GetType() == typeof(REFrameworkNET.ManagedObject)) {
                // See if we can do a dynamic lookup and resolve it to a local type
                var t = (result as REFrameworkNET.ManagedObject).GetTypeDefinition();
                var fullName = t.GetFullName();

                // See if we can find a local type with the same name
                var localType = typeof(via.Scene).Assembly.GetType(fullName);

                if (localType != null) {
                    var prox = Create(localType, typeof(Proxy<,>).MakeGenericType(localType, typeof(REFrameworkNET.ManagedObject)));
                    (prox as IProxy).SetInstance(result);
                    result = prox;
                    return result;
                }
            } else if (result != null && result.GetType() == typeof(REFrameworkNET.NativeObject)) {
                // See if we can do a dynamic lookup and resolve it to a local type
                var t = (result as REFrameworkNET.NativeObject).GetTypeDefinition();
                var fullName = t.GetFullName();

                // See if we can find a local type with the same name
                var localType = typeof(via.Scene).Assembly.GetType(fullName);

                if (localType != null) {
                    var prox = Create(localType, typeof(Proxy<,>).MakeGenericType(localType, typeof(REFrameworkNET.NativeObject)));
                    (prox as IProxy).SetInstance(result);
                    result = prox;
                    return result;
                }
            }
        }

        return result;
    }
}

public class ManagedProxy<T> : Proxy<T, REFrameworkNET.ManagedObject> {
    new public static T Create(dynamic target) {
        return Proxy<T, REFrameworkNET.ManagedObject>.Create(target);
    }
}

public class NativeProxy<T> : Proxy<T, REFrameworkNET.NativeObject> {
    new public static T Create(dynamic target) {
        return Proxy<T, REFrameworkNET.NativeObject>.Create(target);
    }
}

public class DangerousFunctions {
    public static void Entry() {
        // These via.SceneManager and via.Scene are
        // loaded from an external reference assembly
        // the classes are all interfaces that correspond to real in-game classes
        var sceneManager = NativeProxy<via.SceneManager>.Create(REFrameworkNET.API.GetNativeSingleton("via.SceneManager"));
        var scene = sceneManager.get_CurrentScene();

        scene.set_Pause(true);
        var view = sceneManager.get_MainView();
        var name = view.get_Name();
        var go = view.get_PrimaryCamera()?.get_GameObject()?.get_Transform()?.get_GameObject();
        
        REFrameworkNET.API.LogInfo("game object name: " + go?.get_Name().ToString());
        REFrameworkNET.API.LogInfo("Scene name: " + name);

        // Testing autocomplete for the concrete ManagedObject
        REFrameworkNET.API.LogInfo("Scene: " + scene.ToString() + ": " + (scene as REFrameworkNET.ManagedObject)?.GetTypeDefinition()?.GetFullName()?.ToString());

        // Testing dynamic invocation
        float currentTimescale = scene.get_TimeScale();
        scene.set_TimeScale(0.1f);

        REFrameworkNET.API.LogInfo("Previous timescale: " + currentTimescale.ToString());
        REFrameworkNET.API.LogInfo("Current timescale: " + scene?.get_TimeScale().ToString());
    }
}

class REFrameworkPlugin {
    // Measure time between pre and post
    // get time
    static System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    static System.Diagnostics.Stopwatch sw2 = new System.Diagnostics.Stopwatch();

    public static void Main(REFrameworkNET.API api) {
        try {
        REFrameworkNET.API.LogInfo("Testing REFrameworkAPI...");

        REFrameworkNET.Callbacks.BeginRendering.Pre += () => {
            sw.Start();
        };
        REFrameworkNET.Callbacks.BeginRendering.Post += () => {
            sw.Stop();

            if (sw.ElapsedMilliseconds >= 6) {
                Console.WriteLine("BeginRendering took " + sw.ElapsedMilliseconds + "ms");
            }

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


        DangerousFunctions.Entry();

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

        } catch (Exception e) {
            REFrameworkNET.API.LogError(e.ToString());

            var ex = e;

            while (ex.InnerException != null) {
                ex = ex.InnerException;
                REFrameworkNET.API.LogError(ex.ToString());
            }
        }
    }
};