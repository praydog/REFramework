#pragma once
#include "BasisPlane.hpp"
#include "CameraType.hpp"
#include "Color.hpp"
#include "DeviceOrientation.hpp"
#include "DisplayType.hpp"
#include "MobileBarState.hpp"
#include "RenderType.hpp"
#include "Size.hpp"
#include "clr\ManagedObject.hpp"
namespace regenny::via {
struct Window;
}
namespace regenny::via {
struct Scene;
}
namespace regenny::via {
#pragma pack(push, 1)
struct SceneView : public clr::ManagedObject {
    regenny::via::Window* window; // 0x10
    regenny::via::Scene* scene; // 0x18
    regenny::via::Color background_color; // 0x20
    private: char pad_24[0x4]; public:
    // Metadata: utf16*
    wchar_t* name; // 0x28
    regenny::via::Size size; // 0x30
    regenny::via::Size custom_display_size; // 0x38
    regenny::via::Size present_rect; // 0x40
    private: char pad_48[0x18]; public:
    bool draw_develop; // 0x60
    bool tool_light; // 0x61
    bool show_icons; // 0x62
    bool outline_fadeout_enable; // 0x63
    regenny::via::DisplayType display_type; // 0x64
    regenny::via::BasisPlane basis_plane; // 0x68
    regenny::via::CameraType camera_type; // 0x6c
    regenny::via::RenderType render_type; // 0x70
    int32_t icon_size; // 0x74
    float indicator_scale; // 0x78
    private: char pad_7c[0x14]; public:
    void* message_function_handle; // 0x90
    private: char pad_98[0x4]; public:
    regenny::via::DeviceOrientation orientation; // 0x9c
    regenny::via::MobileBarState mobile_status_bar_state; // 0xa0
    regenny::via::MobileBarState mobile_nav_bar_state; // 0xa4
    bool fix_mobile_nav_bar_bottom; // 0xa8
    private: char pad_a9[0x7]; public:
}; // Size: 0xb0
#pragma pack(pop)
}
