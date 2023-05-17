#pragma once

#include <cstdint>

namespace sdk::renderer {
class RenderResource {
public:
    void add_ref();
    void release();

    using ReleaseFn = void(*)(RenderResource* resource);
    static ReleaseFn get_release_fn();

public:
    void* m_vtable;
    int32_t m_ref_count;
    uint32_t m_render_frame;
};
}