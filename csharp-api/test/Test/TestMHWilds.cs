#if MHWILDS
using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using Hexa.NET.ImGui;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;
using REFrameworkNET;

class REFrameworkPluginWilds {
    static System.Numerics.Vector4 TYPE_COLOR = new(78f / 255f, 201f / 255f, 176f / 255f, 1f);
    static System.Numerics.Vector4 VALUE_COLOR = new(181f / 255f, 206f / 255f, 168f / 255f, 1f);
    static System.Numerics.Vector4 WARN_COLOR = new(1f, 0.6f, 0.2f, 1f);

    static string Vec3Str(via.vec3 v) => $"({v.x:F2}, {v.y:F2}, {v.z:F2})";
    
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void ImGuiCallback() {
        if (ImGui.Begin("TestMHWilds.cs")) {
            if (ImGui.CollapsingHeader("Player Info", ImGuiTreeNodeFlags.DefaultOpen)) {
                RenderPlayerInfo();
            }
            
            if (ImGui.CollapsingHeader("Camera Info")) {
                RenderCameraInfo();
            }

            ImGui.End();
        }
    }

    static void RenderPlayerInfo() {
        try {
            var playerManager = API.GetManagedSingletonT<app.PlayerManager>();
            if (playerManager == null) {
                ImGui.TextColored(WARN_COLOR, "PlayerManager not available");
                return;
            }

            var player = playerManager.getMasterPlayer();
            if (player == null) {
                ImGui.Text("Player is null (not in game?)");
                return;
            }

            var ctx = player.ContextHolder;
            var pl = ctx.Pl;

            ImGui.Text("Name: " + pl._PlayerName);
            ImGui.Text("Level: " + pl._CurrentStage);
            ImGui.Text("Network position: " + pl.NetworkPosition.ToString());
            ImGui.Text("General pos: " + Vec3Str(pl._GeneralPos));
            ImGui.Text("Is master row: " + pl._NetMemberInfo.IsMasterRow.ToString());
            ImGui.Text("Dist to camera: " + pl._DistToCamera.ToString());

            ImGui.Separator();

            // Health controls
            var healthMgr = ctx.Chara.HealthManager;
            if (healthMgr != null) {
                float health = healthMgr._Health.read();
                float maxHealth = healthMgr._MaxHealth.read();
                ImGui.Text("Health: " + health.ToString("F1") + " / " + maxHealth.ToString("F1"));
                ImGui.ProgressBar(health / Math.Max(maxHealth, 1f), new System.Numerics.Vector2(-1, 0), health.ToString("F0") + " HP");

                if (ImGui.Button("Full Heal")) {
                    healthMgr.addHealth(maxHealth);
                }
                ImGui.SameLine();
                if (ImGui.Button("Set to 1 HP")) {
                    healthMgr._Health.write(1.0f);
                }
            }

            // get_Object() returns via.GameObject which has Transform
            try {
                var go = player.Object;
                if (go != null) {
                    var transform = go.Transform;
                    if (transform != null) {
                        ImGui.Text("World position: " + Vec3Str(transform.Position));
                    }
                }
            } catch { }
        } catch (Exception e) {
            ImGui.TextColored(WARN_COLOR, "Error: " + e.Message);
        }
    }

    static void RenderCameraInfo() {
        try {
            var camera = via.SceneManager.MainView.PrimaryCamera;
            if (camera == null) {
                ImGui.TextColored(WARN_COLOR, "No primary camera");
                return;
            }

            var camTransform = camera.GameObject.Transform;
            ImGui.Text("Camera position: " + camTransform.Position.ToString());

            var fov = camera.FOV;
            ImGui.Text("FOV: " + fov.ToString("F1"));

            var nearClip = camera.NearClipPlane;
            var farClip = camera.FarClipPlane;
            ImGui.Text("Near/Far clip: " + nearClip.ToString("F2") + " / " + farClip.ToString("F1"));
        } catch (Exception e) {
            ImGui.TextColored(WARN_COLOR, "Error: " + e.Message);
        }
    }

    [Callback(typeof(ImGuiDrawUI), CallbackType.Pre)]
    public static void ImGuiDrawUICallback() {
        ImGui.Text("Hello from TestMHWilds.cs");
    }
}
#endif