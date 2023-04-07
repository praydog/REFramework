#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Mod.hpp"

namespace vrmod {
class CameraDuplicator : public Mod::Component {
public:
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_draw_ui() override;

private:
    void clone_camera();
    void find_new_camera();
    void copy_camera_properties();

    sdk::SystemArray* get_all_cameras();

    bool m_copy_camera{true};
    bool m_called_activate{false};
    RECamera* m_old_camera{nullptr};
    RECamera* m_new_camera{nullptr};
    
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
        WantedComponent{ "via.render.ToneMapping", { } },
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
        WantedComponent{ "via.render.Fog", {} },
        WantedComponent{ "via.render.VolumetricFog", { } },
        WantedComponent{ "via.render.VolumetricFogControl", { } },
        WantedComponent{ "via.render.GodRay", { } },
        WantedComponent{ "via.render.GeometryAOControl", { } },
        WantedComponent{ "via.render.MotionBlur", { } },
        WantedComponent{ "via.render.FakeLensflare", { } },
        WantedComponent{ "via.render.CustomFilter", { } },
    };

    std::vector<std::function<void()>> m_property_jobs{};
    size_t m_property_jobs_index{0};
};
}