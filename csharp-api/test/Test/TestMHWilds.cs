using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using ImGuiNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using REFrameworkNET;

class REFrameworkPluginWilds {
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        if (ImGui.Begin("TestMHWilds.cs")) {
            var playerManager = API.GetManagedSingletonT<app.PlayerManager>();
            if (playerManager == null) {
                ImGui.End();
                return;
            }

            var player = playerManager.getMasterPlayer();

            if (player == null) {
                ImGui.End();
                return;
            }

            ImGui.Text("Name: " + player.ContextHolder.Pl._PlayerName);

            ImGui.Text("TestMHWilds.cs");
            ImGui.End();
        }
    }
}