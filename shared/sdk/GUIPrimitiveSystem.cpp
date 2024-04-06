#include <spdlog/spdlog.h>

#include "RETypeDB.hpp"

#include "GUIPrimitiveSystem.hpp"

namespace sdk::gui::renderer {
PrimitiveSystem* PrimitiveSystem::get() {
    static auto instance = ::sdk::get_native_singleton<PrimitiveSystem>("via.gui.renderer.GUIPrimitiveSystem");
    return instance;
}

PrimitiveBuffer* PrimitiveSystem::get_primitive_buffer() const {
    static std::optional<size_t> s_primitive_buffer_offset = [this]() -> std::optional<size_t> {
        spdlog::info("[via::sdk::gui::renderer::PrimitiveSystem::get_primitive_buffer] Finding PrimitiveBuffer offset...");

        for (size_t i = sizeof(void*); i < 0x1000; i += sizeof(void*)) try {
            const auto potential_buffer = (PrimitiveBuffer*)((uintptr_t)this + i);

            // All of these have to be valid.
            if (IsBadReadPtr(potential_buffer->vertex.data, sizeof(void*)) || 
                IsBadReadPtr(potential_buffer->index.data, sizeof(void*)) || 
                IsBadReadPtr(potential_buffer->scratch.data, sizeof(void*))
               ) 
            {
                continue;
            }
            
            // Pointers have to be aligned on an 8-byte boundary.
            if (((uintptr_t)potential_buffer->vertex.data & (sizeof(void*) - 1)) != 0 || 
                ((uintptr_t)potential_buffer->index.data & (sizeof(void*) - 1)) != 0 || 
                ((uintptr_t)potential_buffer->scratch.data & (sizeof(void*) - 1)) != 0
               ) 
            {
                continue;
            }

            // All of the sizes have to be > 0.
            if (potential_buffer->vertex.size == 0 || potential_buffer->index.size == 0 || potential_buffer->scratch.size == 0) {
                continue;
            }

            // Sizes have to be aligned on an 8-byte boundary.
            if ((potential_buffer->vertex.size & (sizeof(void*) - 1)) != 0 || 
                (potential_buffer->index.size & (sizeof(void*) - 1)) != 0 || 
                (potential_buffer->scratch.size & (sizeof(void*) - 1)) != 0
               ) 
            {
                continue;
            }

            // If used is > 0, max_used has to be > 0 and >= used.
            if (potential_buffer->vertex.used > 0 && potential_buffer->vertex.max_used < potential_buffer->vertex.used) {
                continue;
            }

            if (potential_buffer->index.used > 0 && potential_buffer->index.max_used < potential_buffer->index.used) {
                continue;
            }

            if (potential_buffer->scratch.used > 0 && potential_buffer->scratch.max_used < potential_buffer->scratch.used) {
                continue;
            }

            // Used has to be <= size.
            if (potential_buffer->vertex.used > potential_buffer->vertex.size || 
                potential_buffer->index.used > potential_buffer->index.size || 
                potential_buffer->scratch.used > potential_buffer->scratch.size
               ) 
            {
                continue;
            }

            // TODO: More heuristics and potentially make it use the reflection properties
            // for all 3 buffers which hardcode the sizes.
            spdlog::info("[via::sdk::gui::renderer::PrimitiveSystem::get_primitive_buffer] Found PrimitiveBuffer offset at 0x{:x}", i);
            return i;
        } catch(...) {
            continue;
        }

        return {};
    }();

    if (!s_primitive_buffer_offset) {
        return nullptr;
    }

    return (PrimitiveBuffer*)((uintptr_t)this + *s_primitive_buffer_offset);
}

void PrimitiveBuffer::Buffer::resize(size_t new_size) {
    if (new_size == this->size) {
        return;
    }

    this->data = (uint8_t*)sdk::memory::reallocate(this->data, size, new_size);
    this->size = new_size;
}
}