#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Mod.hpp"

namespace sdk {
namespace renderer {
namespace layer {
class Scene;
}
}
}

namespace vrmod {
class CameraDuplicator : public Mod::Component {
public:
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_draw_ui() override;

    RECamera* get_new_camera_counterpart(RECamera* camera) {
        std::scoped_lock _{ m_camera_mutex };

        if (auto it = m_old_to_new_camera.find(camera); it != m_old_to_new_camera.end()) {
            return it->second;
        }

        return nullptr;
    }

    RECamera* get_old_camera_counterpart(RECamera* camera) {
        std::scoped_lock _{ m_camera_mutex };

        if (auto it = m_new_to_old_camera.find(camera); it != m_new_to_old_camera.end()) {
            return it->second;
        }

        return nullptr;
    }

    std::vector<sdk::renderer::layer::Scene*> get_relevant_scene_layers();

private:
    void clone_camera();
    void hook_get_primary_camera();
    void copy_camera_properties();

    sdk::SystemArray* get_all_cameras();

    bool m_copy_camera{true};
    bool m_called_activate{false};
    bool m_hooked_get_primary_camera{false};
    bool m_identified_new_cameras{false};
    bool m_waiting_for_identify{false};
    std::recursive_mutex m_camera_mutex{};
    std::unordered_set<RECamera*> m_seen_cameras{};
    std::unordered_set<RECamera*> m_old_cameras{};
    std::unordered_set<RECamera*> m_new_cameras{};
    std::unordered_map<RECamera*, RECamera*> m_old_to_new_camera{};
    std::unordered_map<RECamera*, RECamera*> m_new_to_old_camera{};

    RECamera* m_last_primary_camera{nullptr};
    
    struct GetterSetter {
        sdk::REMethodDefinition* getter{nullptr};
        sdk::REMethodDefinition* setter{nullptr};
    };

    std::unordered_map<sdk::RETypeDefinition*, std::unordered_map<std::string, GetterSetter>> m_getter_setters{};

    struct WantedComponent {
        std::string name{};
        std::unordered_set<size_t> ignored_properties{};
        bool allowed{true}; // used for debug imgui checkbox
    };

    // Components that we want to copy from the old camera to the new camera
    std::vector<WantedComponent> m_wanted_components {
        WantedComponent{ "via.render.ToneMapping", { "set_InjectingGameObject"_fnv } },
        WantedComponent{ "via.render.LDRPostProcess", {} },
        WantedComponent{ "via.render.SoftBloom", {} },
        WantedComponent{ "via.render.SSRControl", {} },
        WantedComponent{ "via.render.SSAOControl", {} },
        WantedComponent{ "via.render.SSSSSControl", {} },
        WantedComponent{ "via.render.DepthOfField", {} },
        WantedComponent{ "via.render.LightShaftFilterControl", {} },
        WantedComponent{ "via.render.FakeLensFlare", {} },
        WantedComponent{ "via.render.MotionBlur", {} },
        WantedComponent{ "via.render.FFTBloom", {} },
        WantedComponent{ "via.render.Echo", {} },
        WantedComponent{ "via.render.OverlayFXAA", {} },
        WantedComponent{ "via.render.TessellationFactor", {} },
        WantedComponent{ "via.render.ShadowQualityControl", {} },
        WantedComponent{ "via.render.Fog", { "set_FSSunPosGameObject"_fnv } },
        WantedComponent{ "via.render.VolumetricFog", { } },
        WantedComponent{ "via.render.VolumetricFogControl", { } },
        WantedComponent{ "via.render.GodRay", { } },
        WantedComponent{ "via.render.GeometryAOControl", { } },
        WantedComponent{ "via.render.MotionBlur", { } },
        WantedComponent{ "via.render.FakeLensflare", { } },
        WantedComponent{ "via.render.RetroFilm", { } },
        //WantedComponent{ "via.render.ExperimentalRayTrace", {}}
        //WantedComponent{ "via.render.CustomFilter", { } },
    };

    std::vector<std::function<void()>> m_property_jobs{};
    size_t m_property_jobs_index{0};
};
}