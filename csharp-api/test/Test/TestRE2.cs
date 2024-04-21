using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;

public class TestRE2 {
    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static void Main() {
        // Get executable name
        string executableName = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;

        if (executableName.ToLower().Contains("re2")) {
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
        };
    }

    [REFrameworkNET.Attributes.PluginExitPoint]
    public static void Unload() {
    }
}