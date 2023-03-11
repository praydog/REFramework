#pragma once
#include "..\clr\ManagedObject.hpp"
#include "LayerArray.hpp"
#include "RTL_CRITICAL_SECTION.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderLayer : public regenny::via::clr::ManagedObject {
    uint32_t m_id; // 0x10
    uint32_t m_render_output_id; // 0x14
    uint32_t m_render_output_id_2; // 0x18
    uint32_t m_priority; // 0x1c
    uint32_t m_priority_offsets[7]; // 0x20
    char pad_3c[0x4];
    regenny::via::render::RenderLayer* m_parent; // 0x40
    regenny::via::render::LayerArray layers; // 0x48
    regenny::via::render::RTL_CRITICAL_SECTION m_mutex; // 0x58
    uint32_t version; // 0x80
    char pad_84[0x4];
}; // Size: 0x88
#pragma pack(pop)
}
