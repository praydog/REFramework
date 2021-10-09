#pragma once
#include "via\Object.hpp"
namespace regenny {
#pragma pack(push, 1)
struct RenderDeviceDX11 : public via::Object {
    uint32_t driver_type; // 0x8
    uint32_t feature_level; // 0xc
    void* device; // 0x10
    void* immediate_context; // 0x18
    void* deferred_context; // 0x20
    void* dxgi_device; // 0x28
    void* dxgi_factory; // 0x30
    char pad_38[0xc8];
}; // Size: 0x100
#pragma pack(pop)
}
