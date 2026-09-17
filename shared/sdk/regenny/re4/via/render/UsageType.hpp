#pragma once
namespace regenny::via::render {
#pragma pack(push, 1)
enum class UsageType : uint32_t {
    Default = 0,
    Immutable = 1,
    Dynamic = 2,
    Staging = 3,
};
#pragma pack(pop)
}
