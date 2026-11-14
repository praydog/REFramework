#pragma once
#include "RenderResource.hpp"
struct ID3D12Resource;
namespace regenny::via::render {
#pragma pack(push, 1)
struct DXResource : public RenderResource {
    ID3D12Resource* dx_resource; // 0x10
    void* destructor; // 0x18
}; // Size: 0x20
#pragma pack(pop)
}
