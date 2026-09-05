#pragma once
namespace regenny::via::render {
#pragma pack(push, 1)
enum class TextureStreamingType : uint32_t {
    None = 0,
    Streaming = 1,
    HighMap = 2,
};
#pragma pack(pop)
}
