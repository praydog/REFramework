#pragma once
#include ".\RenderType.hpp"
#include ".\Size.hpp"
#include ".\DisplayType.hpp"
#include ".\BasisPlane.hpp"
#include ".\CameraType.hpp"
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
    regenny::via::Window* window; // 0x20
    regenny::via::Scene* scene; // 0x28
    char pad_30[0x8];
    // Metadata: utf16*
    wchar_t* name; // 0x38
    char pad_40[0x18];
    regenny::via::Size size; // 0x58
    regenny::via::Size custom_display_size; // 0x60
    regenny::via::Size present_rect; // 0x68
    bool draw_develop; // 0x70
    bool tool_light; // 0x71
    bool show_icons; // 0x72
    bool outline_fadeout_enable; // 0x73
    regenny::via::DisplayType display_type; // 0x74
    regenny::via::BasisPlane basis_plane; // 0x78
    regenny::via::CameraType camera_type; // 0x7c
    regenny::via::RenderType render_type; // 0x80
    int32_t icon_size; // 0x84
    float indicator_scale; // 0x88
    char pad_8c[0x24];
}; // Size: 0xb0
#pragma pack(pop)
}
