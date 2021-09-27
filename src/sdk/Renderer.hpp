#pragma once

#include <cstdint>

#include "ReClass.hpp"
#include "RENativeArray.hpp"

class REType;

namespace sdk {
namespace renderer {
class RenderLayer : public REManagedObject {
public:
    RenderLayer* add_layer(::REType* layer_type, uint32_t priority, uint8_t offset = 0);
    sdk::NativeArray<RenderLayer*>& get_layers();

#ifdef RE8
    static constexpr uint32_t DRAW_VTABLE_INDEX = 14;
#else
    static constexpr uint32_t DRAW_VTABLE_INDEX = 12;
#endif

public:
    uint32_t m_id;
    uint32_t m_render_output_id;
    uint32_t m_render_output_id_2;
    uint32_t m_priority;

    // more stuff below, map it later
};

namespace layer {
class Output : public sdk::renderer::RenderLayer {
public:
    // Not only grabs the scene view, but grabs a reference to it
    // so we can modify it.
    void*& get_present_state(); // via.render.OutputTargetState
    REManagedObject*& get_scene_view();
};
}

void* get_renderer();
void add_scene_view(void* scene_view);
void remove_scene_view(void* scene_view);
RenderLayer* get_root_layer();
sdk::renderer::layer::Output* get_output_layer();
}
}