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

        ImGuiRender.Pre += () => {
            if (ImGui.Begin("TestRE2")) {
                ImGui.Text("Hello, world!");
                ImGui.End();
            }
            
            if (ImGui.Begin("Test Window")) {
                ImGui.Text("RE2");
                ImGui.Separator();

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
    }

    [REFrameworkNET.Attributes.PluginExitPoint]
    public static void Unload() {
    }
}

