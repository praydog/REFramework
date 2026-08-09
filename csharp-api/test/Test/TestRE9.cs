using System;

using Hexa.NET.ImGui;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;

public class TestRE9Plugin {
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
#if RE9
        if (ImGui.Begin("TestRE9.cs")) {
            ImGui.Text("RE9 plugin loaded");

            ImGui.End();
        }
#endif
    }

    [Callback(typeof(ImGuiDrawUI), CallbackType.Pre)]
    public static void ImGuiDrawUICallback()
    {
#if RE9
        ImGui.Text("RE9 plugin ImGuiDrawUI callback");
#else
        ImGui.Text("RE9 plugin ImGuiDrawUI callback - not RE9");
#endif
    }
}