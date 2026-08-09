#pragma once
#include "RenderResource.hpp"
struct ID3D12Resource;
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct DXResource : public RenderResource {
    ID3D12Resource* dx_resource; // 0x18
    void* destructor; // 0x20
    private: char pad_28[0xfd8]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
