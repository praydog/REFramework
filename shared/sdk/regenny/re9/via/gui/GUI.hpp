#pragma once
#include "..\Component.hpp"
namespace regenny::via {
struct SceneView;
}
namespace regenny::via::gui {
struct View;
}
namespace regenny::via::gui {
#pragma pack(push, 1)
struct GUI : public regenny::via::Component {
    regenny::via::SceneView* SceneView; // 0x30
    void* GUIResource; // 0x38
    void* RtTextureHandle; // 0x40
    void* GUISound; // 0x48
    bool Enabled; // 0x50
    bool FailedSetup; // 0x51
    uint8_t MaskLayerBits; // 0x52
    uint8_t MaskLayerBits2; // 0x53
    uint8_t MaskLayerBits3; // 0x54
    private: char pad_55[0x3]; public:
    uint32_t RenderOutputID; // 0x58
    float PlaySpeed; // 0x5c
    uint32_t Segment; // 0x60
    uint32_t SoftParticleDistType; // 0x64
    float SoftParticleDist; // 0x68
    private: char pad_6c[0x4]; public:
    regenny::via::gui::View* View; // 0x70
    private: char pad_78[0x258]; public:
}; // Size: 0x2d0
#pragma pack(pop)
}
