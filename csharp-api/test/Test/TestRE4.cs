using System;

using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using _;

public class TestRE4Plugin {
    static bool IsRunningRE2 => Environment.ProcessPath.Contains("re4", StringComparison.CurrentCultureIgnoreCase);
    
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        if (ImGui.Begin("Test Window")) {
            ImGui.Text("RE4");
            ImGui.Separator();

            
            if (ImGui.TreeNode("Player")) {
                var context = GlobalService.Chainsaw._sCharacterManagerCache.getPlayerContextRef();

                if (context == null) {
                    ImGui.Text("Player context is null");
                } else {
                    ImGui.Text($"Player context: {(context as IObject).Call("ToString", null) as string}");
                }

                ImGui.TreePop();
            }

            ImGui.End();
        }
    }
}