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
    regenny::via::SceneView* SceneView; // 0x40
    void* GUIResource; // 0x48
    bool Enabled; // 0x50
    char pad_51[0x7];
    uint32_t RenderOutputID; // 0x58
    float PlaySpeed; // 0x5c
    bool FailedSetup; // 0x60
    uint8_t MaskLayerBits; // 0x61
    uint8_t MaskLayerBits2; // 0x62
    uint8_t MaskLayerBits3; // 0x63
    uint32_t SoftParticleDistType; // 0x64
    float SoftParticleDist; // 0x68
    char pad_6c[0x4];
    regenny::via::gui::View* View; // 0x70
    char pad_78[0x258];
}; // Size: 0x2d0
#pragma pack(pop)
}
