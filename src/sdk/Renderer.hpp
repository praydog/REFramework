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
    RenderLayer* find_layer(::REType* layer_type);

    RenderLayer* get_parent();
    RenderLayer* find_parent(::REType* layer_type);

#ifdef RE8
    static constexpr uint32_t DRAW_VTABLE_INDEX = 14;
#elif defined(RE7)
    static constexpr uint32_t DRAW_VTABLE_INDEX = 10;
#else
    static constexpr uint32_t DRAW_VTABLE_INDEX = 12;
#endif

    static constexpr uint32_t UPDATE_VTABLE_INDEX = DRAW_VTABLE_INDEX + 1;

    void draw(void* render_context) {
        const auto vtable = *(void(***)(void*, void*))this;
        return vtable[DRAW_VTABLE_INDEX](this, render_context);
    }

    void update() {
        const auto vtable = *(void(***)(void*))this;
        return vtable[UPDATE_VTABLE_INDEX](this);
    }

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

void wait_rendering();
void begin_update_primitive();
void update_primitive();
void end_update_primitive();

void begin_rendering();
void end_rendering();

void add_scene_view(void* scene_view);
void remove_scene_view(void* scene_view);
RenderLayer* get_root_layer();
RenderLayer* find_layer(::REType* layer_type);

sdk::renderer::layer::Output* get_output_layer();
}
}