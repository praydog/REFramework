using System;

using ImGuiNET;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using _;
using chainsaw;

public class TestRE4Plugin {
    static bool IsRunningRE2 => Environment.ProcessPath.Contains("re4", StringComparison.CurrentCultureIgnoreCase);
    static System.Diagnostics.Stopwatch imguiStopwatch = new();
    
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        imguiStopwatch.Restart();

        if (ImGui.Begin("Test Window")) {
            ImGui.Text("RE4");
            ImGui.Separator();

            
            if (ImGui.TreeNode("Player")) {
                var context = GlobalService.Chainsaw._sCharacterManagerCache.getPlayerContextRef();

                if (context == null) {
                    ImGui.Text("Player context is null");
                } else {
                    ImGui.Text($"Player context: {context}");

                    var body = context.BodyGameObject;
                    var head = context.HeadGameObject;

                    bool drawBody = body.DrawSelf;
                    if (ImGui.Checkbox("Draw body", ref drawBody)) {
                        body.DrawSelf = drawBody;
                    }

                    ImGui.Text($"Body: {body}");
                    ImGui.Text($"Head: {head}");

                    if (ImGui.TreeNode("Components (Body)")) {
                        var components = body.Components;

                        for (int i = 0; i < components.Count; i++) {
                            var component = components[i];
                            ImGui.Text($"Component {i}: {component}");
                        }

                        ImGui.TreePop();
                    }

                    if (ImGui.TreeNode("Components (Head)")) {
                        var components = head.Components;

                        for (int i = 0; i < components.Count; i++) {
                            var component = components[i];
                            ImGui.Text($"Component {i}: {component}");
                        }

                        ImGui.TreePop();
                    }
                }

                ImGui.TreePop();
            }

            imguiStopwatch.Stop();
            var microseconds = imguiStopwatch.ElapsedTicks / (System.Diagnostics.Stopwatch.Frequency / 1000000);
            ImGui.Text($"Test execution time: {microseconds} µs");

            ImGui.End();
        }
    }
}