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
            ImGui.Text("Level: " + player.ContextHolder.Pl._CurrentStage);
            ImGui.Text("Network position: " + player._ContextHolder.Pl.NetworkPosition.ToString());
            ImGui.Text(player.ContextHolder.Pl._GeneralPos.ToString());
            ImGui.Text(player.ContextHolder.Pl._NetMemberInfo.IsMasterRow.ToString());
            ImGui.Text(player.ContextHolder.Pl._DistToCamera.ToString());
            if (ImGui.Button("Test")) {
                player.ContextHolder.Chara.HealthManager._Health.write(0.0f);
            }

            if (ImGui.Button("Test2")) {
                player.ContextHolder.Chara.HealthManager.addHealth(1.0f);
            }

            ImGui.End();
        }
    }

    [Callback(typeof(ImGuiDrawUI), CallbackType.Pre)]
    public static void ImGuiDrawUICallback() {
        ImGui.Text("Hello from TestMHWilds.cs");
    }
}