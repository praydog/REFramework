using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using REFrameworkNET;

class REFrameworkPlugin2 {
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        if (ImGui.Begin("Test2.cs")) {
            ImGui.Text("Test2.cs");
            ImGui.End();
        }
    }
}