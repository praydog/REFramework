#pragma once

#include <optional>
#include <cstdint>

namespace sdk::gui::renderer  {
struct PrimitiveBuffer;

class PrimitiveSystem {
public:
    static PrimitiveSystem* get();

public:
    PrimitiveBuffer* get_primitive_buffer() const;
};

struct PrimitiveBuffer {
    struct Buffer {
        uint8_t* data;
        size_t size;
        int32_t used;
        int32_t max_used;

        void resize(size_t new_size);
    };

    Buffer vertex;
    Buffer index;
    Buffer scratch;
};


static_assert(offsetof(PrimitiveBuffer, scratch) == 0x30, "PrimitiveBuffer::scratch offset is not 0x30");
}