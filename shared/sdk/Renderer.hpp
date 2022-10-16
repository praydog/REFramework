#pragma once

#include <cstdint>
#include <tuple>
#include <optional>

#include "ReClass.hpp"
#include "RENativeArray.hpp"

class REType;

namespace sdk {
namespace renderer {
struct SceneInfo {
    Matrix4x4f view_projection_matrix;
    Matrix4x4f view_matrix;
    Matrix4x4f inverse_view_matrix;
    Matrix4x4f projection_matrix;
    Matrix4x4f inverse_projection_matrix;
    Matrix4x4f inverse_view_projection_matrix;
    Matrix4x4f old_view_projection_matrix;
};

class TargetState {
public:
    void* get_native_resource_d3d12() const;

private:
};

class RenderLayer : public REManagedObject {
public:
    RenderLayer* add_layer(::REType* layer_type, uint32_t priority, uint8_t offset = 0);
    sdk::NativeArray<RenderLayer*>& get_layers();
    RenderLayer** find_layer(::REType* layer_type);
    std::tuple<RenderLayer*, RenderLayer**> find_layer_recursive(const ::REType* layer_type); // parent, type
    std::tuple<RenderLayer*, RenderLayer**> find_layer_recursive(std::string_view type_name); // parent, type

    RenderLayer* get_parent();
    RenderLayer* find_parent(::REType* layer_type);
    RenderLayer* clone(bool recursive = false);
    void clone(RenderLayer* other, bool recursive = false);
    void clone_layers(RenderLayer* other, bool recursive = false);

    ::sdk::renderer::TargetState* get_target_state(std::string_view name);

    void* get_target_state_resource_d3d12(std::string_view name) {
        auto state = get_target_state(name);
        return state != nullptr ? state->get_native_resource_d3d12() : nullptr;
    }

#if TDB_VER >= 69
    static constexpr uint32_t DRAW_VTABLE_INDEX = 14;
#elif TDB_VER > 49
    static constexpr uint32_t DRAW_VTABLE_INDEX = 12;
#else
    static constexpr uint32_t DRAW_VTABLE_INDEX = 10;
#endif

    static constexpr uint32_t UPDATE_VTABLE_INDEX = DRAW_VTABLE_INDEX + 1;

    // only verified in RE8 and RE2.
#if TDB_VER >= 69
    static constexpr uint32_t NUM_PRIORITY_OFFSETS = 7;
#elif TDB_VER >= 66
    static constexpr uint32_t NUM_PRIORITY_OFFSETS = 6;
#else
    static constexpr uint32_t NUM_PRIORITY_OFFSETS = 0;
#endif

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

#if TDB_VER <= 49
    sdk::renderer::RenderLayer* m_parent;
    sdk::NativeArray<sdk::renderer::RenderLayer*> m_layers;
    uint32_t m_priority;
#else
    uint32_t m_priority;
    uint32_t m_priority_offsets[NUM_PRIORITY_OFFSETS];
    sdk::renderer::RenderLayer* m_parent;
    sdk::NativeArray<sdk::renderer::RenderLayer*> m_layers;
#endif

    struct {
        void* DebugInfo;
        uint32_t LockCount;
        uint32_t RecursionCount;
        void* OwningThread;
        void* LockSemaphore;
        uintptr_t SpinCount;
    } m_cs;
    uint32_t m_version;
};

#if TDB_VER <= 49
static_assert(offsetof(RenderLayer, m_priority) == 0x48, "RenderLayer::m_priority offset is wrong");
static_assert(offsetof(RenderLayer, m_layers) == 0x38, "RenderLayer::m_layers offset is wrong");
static_assert(offsetof(RenderLayer, m_parent) == 0x30, "RenderLayer::m_parent offset is wrong");
#endif

namespace layer {
class Output : public sdk::renderer::RenderLayer {
public:
    // Not only grabs the scene view, but grabs a reference to it
    // so we can modify it.
    void*& get_present_state(); // via.render.OutputTargetState
    REManagedObject*& get_scene_view();

    void* get_output_target_d3d12() {
        return get_target_state_resource_d3d12("OutputTarget");
    }
};

class Scene : public sdk::renderer::RenderLayer {
public:
    sdk::renderer::SceneInfo* get_scene_info();
    sdk::renderer::SceneInfo* get_depth_distortion_scene_info();
    sdk::renderer::SceneInfo* get_filter_scene_info();
    sdk::renderer::SceneInfo* get_jitter_disable_scene_info();
    sdk::renderer::SceneInfo* get_z_prepass_scene_info();
    void* get_depth_stencil_d3d12();

    void* get_motion_vectors_d3d12() {
        return get_target_state_resource_d3d12("VelocityTarget");
    }

    void* get_post_main_target_d3d12() {
        return get_target_state_resource_d3d12("PostMainTarget");
    }

    void* get_hdr_target_d3d12() {
        return get_target_state_resource_d3d12("HDRTarget");
    }
};

class Overlay : public sdk::renderer::RenderLayer {
};

class PostEffect : public sdk::renderer::RenderLayer {
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

std::optional<Vector2f> world_to_screen(const Vector3f& world_pos);
}
}