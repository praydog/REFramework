using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;

class REFrameworkPlugin2 {
    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static void Main() {
        REFrameworkNET.Callbacks.ImGuiRender.Pre += () => {
            if (ImGui.Begin("Test2.cs")) {
                ImGui.Text("Test2.cs");
                ImGui.End();
            }
        };
    }
}