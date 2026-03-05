#pragma once
#include "..\clr\ManagedObject.hpp"
namespace regenny::via::gui {
struct GUI;
}
namespace regenny::via::gui {
struct Control;
}
namespace regenny::via::gui {
#pragma pack(push, 1)
struct PlayObject : public regenny::via::clr::ManagedObject {
    // Metadata: utf16*
    wchar_t* Name; // 0x10
    regenny::via::gui::GUI* Component; // 0x18
    private: char pad_20[0x8]; public:
    regenny::via::gui::Control* Parent; // 0x28
    regenny::via::gui::PlayObject* Next; // 0x30
    regenny::via::gui::PlayObject* Prev; // 0x38
    private: char pad_40[0x2]; public:
    uint16_t Priority; // 0x42
    private: char pad_44[0x3]; public:
    uint8_t pad; // 0x47
    uint64_t Visible : 1; // 0x48
    uint64_t Visible2 : 1; // 0x48
    uint64_t HitVisible : 1; // 0x48
    uint64_t ShouldPlay : 1; // 0x48
    uint64_t IsPlaying : 1; // 0x48
    uint64_t IsInteractive : 1; // 0x48
    uint64_t HasName : 1; // 0x48
    uint64_t HasTag : 1; // 0x48
    uint64_t SequenceAsset : 1; // 0x48
    uint64_t IsPNG : 1; // 0x48
    uint64_t IsJPG : 1; // 0x48
    uint64_t LinearAlphaBlend : 1; // 0x48
    uint64_t CompOwnership : 1; // 0x48
    uint64_t DuplicateRoot : 1; // 0x48
    uint64_t DataBind : 1; // 0x48
    uint64_t DataBinding : 1; // 0x48
    uint64_t AllocExData : 1; // 0x48
    uint64_t NextStateEnable : 1; // 0x48
    uint64_t _Unk : 1; // 0x48
    uint64_t _Unk2 : 1; // 0x48
    uint64_t ResolutionAdjust : 1; // 0x48
    uint64_t UniqueVisible : 1; // 0x48
    uint64_t EffectAlphaType : 1; // 0x48
    uint64_t HasOverrideProp : 1; // 0x48
    uint64_t IsCapturable : 1; // 0x48
    uint64_t ForceInvisible : 1; // 0x48
    uint64_t ClipPlaying : 1; // 0x48
    uint64_t BlendType : 2; // 0x48
    uint64_t MaskType : 3; // 0x48
    uint64_t MaskMode : 3; // 0x48
    uint64_t ColorType : 2; // 0x48
    uint64_t ControlPoint : 4; // 0x48
    uint64_t ResolutionCond : 2; // 0x48
    uint64_t ResolutionScale : 2; // 0x48
    uint64_t UVType : 2; // 0x48
    uint64_t UnkFlags : 17; // 0x48
}; // Size: 0x50
#pragma pack(pop)
}
