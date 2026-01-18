using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using REFrameworkNET;
using via;

class REFrameworkPluginWilds {
    // STore time
    public static System.DateTime lastTime = System.DateTime.Now;

    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        var now = System.DateTime.Now;

        if ((now - lastTime).TotalSeconds > 0.1 && !GC.GetGCMemoryInfo().Concurrent) {
            //GC.Collect(1, GCCollectionMode.Forced, false);
            //lastTime = now;
        }

        if (ImGui.Begin("TestDMC5.cs")) {
            ImGui.Text("Hello from TestDMC5.cs");

            try {
                var playerManager = REFrameworkNET.API.GetManagedSingletonT<app.PlayerManager>();

                if (playerManager == null) {
                    return;
                }
                
                var player = playerManager.manualPlayer;

                if (player == null) {
                    return;
                }

                ImGui.Text("Name: " + player.GameObject.Name);
                ImGui.Text("Network name: " + player.NetworkName);
                ImGui.Text("HP: " + player.hp);
                ImGui.Text("Max HP: " + player.maxHp);
                ImGui.Text("Position: " + player.GameObject.Transform.Position);

                if (ImGui.TreeNode("Components")) {
                    var components = player.GameObject.Components;

                    //foreach (ManagedObject componentRaw in (components as IProxy).GetInstance() as ManagedObject) {
                        //var component = componentRaw.As<via.Component>();
                    for (int i = 0; i < components.Count; i++) {
                        var component = components.Get(i);
                        if (ImGui.TreeNode("Component: " + component.ToString())) {             
                            var t = component.GetType();
                            var fields = t.GetFields();

                            for (int j = 0; j < fields.Length; j++) {
                                var field = fields.Get(j);
                                var fieldval = field.GetValue(component);

                                ImGui.Text("Field: " + field.Name + " = " + (fieldval as IObject)?.Call("ToString", []));

                                // Dispose of the field value
                                (fieldval as IDisposable)?.Dispose();
                                //(field as IDisposable)?.Dispose();
                            }

                            var props = t.GetProperties();

                            for (int j = 0; j < props.Length; j++) {
                                var prop = props.Get(j);
                                var propval = prop.GetValue(component);

                                ImGui.Text("Property: " + prop.Name + " = " + (propval as IObject)?.Call("ToString", []));

                                // Dispose of the property value
                                (propval as IDisposable)?.Dispose();
                                //(prop as IDisposable)?.Dispose();
                            }
                            
                            (fields as IDisposable)?.Dispose();
                            (props as IDisposable)?.Dispose();
                            (t as IDisposable)?.Dispose();

                            ImGui.TreePop();
                        }
                    }

                    (components as IDisposable)?.Dispose();

                    ImGui.TreePop();
                }

                if (ImGui.Button("Test")) {
                    player.hp = 0;
                }

                if (ImGui.Button("Air Hike")) {
                    var pos = player.GameObject.Transform.Position;
                    pos.y += 1;
                    player.GameObject.Transform.Position = pos;

                    player.airHike(1.0f);
                }

            } finally {
                ImGui.End();
            }
        }
    }

    [Callback(typeof(ImGuiDrawUI), CallbackType.Pre)]
    public static void ImGuiDrawUICallback() {
        ImGui.Text("Hello from TestDMC5.cs");
    }
}