// Import REFramework::API
using System;

class REFrameworkPlugin {
    // Measure time between pre and post
    // get time
    static System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    static System.Diagnostics.Stopwatch sw2 = new System.Diagnostics.Stopwatch();

    class Scene {
        public void set_TimeScale(float timeScale) {

        }
    };
    public static void Main(REFrameworkNET.API api) {
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

        dynamic sceneManager = REFrameworkNET.API.GetNativeSingleton("via.SceneManager");
        dynamic scene = sceneManager.get_CurrentScene();

        if (scene != null) {
            var name = (scene as REFrameworkNET.ManagedObject).GetTypeDefinition().GetFullName();
            REFrameworkNET.API.LogInfo("Scene type: " + name);
            REFrameworkNET.API.LogInfo("Scene: " + scene.ToString());
            
            scene.set_TimeScale(0.1f);
        } else {
            REFrameworkNET.API.LogInfo("Scene is null");
        }
    }
};