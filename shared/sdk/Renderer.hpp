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

class RenderResource {
public:
    void add_ref() {
        _InterlockedIncrement((long*)&m_ref_count);
    }

public:
    void* m_vtable;
    int32_t m_ref_count;
    uint32_t m_render_frame;
};

class DirectXResource : public RenderResource {
public:
    void* get_native_resource() const {
    #if TDB_VER > 67
        return *(void**)((uintptr_t)this + 0x10);
    #else
        return *(void**)((uintptr_t)this + 0x18);
    #endif
    }

private:
};

class Texture : public RenderResource {
public:
    void* get_desc() {
        return (void*)((uintptr_t)this + sizeof(RenderResource) + sizeof(void*));
    }

    DirectXResource* get_d3d12_resource() {
        return *(DirectXResource**)((uintptr_t)this + s_d3d12_resource_offset);
    }

private:
#if TDB_VER >= 71
    static constexpr inline auto s_d3d12_resource_offset = 0xA0;
#elif TDB_VER == 70
    static constexpr inline auto s_d3d12_resource_offset = 0x98;
#elif TDB_VER == 69
    static constexpr inline auto s_d3d12_resource_offset = 0x98;
#else
    // TODO? might not be right offset
    static constexpr inline auto s_d3d12_resource_offset = 0x98;
#endif
};

class DepthStencilView : public RenderResource {
};

class RenderTargetView : public RenderResource {
public:
    struct Desc {
        uint32_t format;
        uint32_t dimension;

        uint8_t unk_pad[0xC];
    };

    Desc& get_desc() {
        return m_desc;
    }

    Texture* get_texture_d3d12() const {
        return *(Texture**)((uintptr_t)this + s_texture_d3d12_offset);
    }

private:
    Desc m_desc;

#if TDB_VER == 71
    static constexpr inline auto s_texture_d3d12_offset = 0x98;
#elif TDB_VER == 70
    static constexpr inline auto s_texture_d3d12_offset = 0x90;
#elif TDB_VER == 69
    static constexpr inline auto s_texture_d3d12_offset = 0x88;
#elif TDB_VER <= 67
    // TODO: 66 and below
    static constexpr inline auto s_texture_d3d12_offset = 0x88;
#endif
};

static_assert(sizeof(RenderTargetView::Desc) == 0x14);

class TargetState : public RenderResource {
public:
    struct Desc;

    Desc& get_desc() {
        return m_desc;
    }

    void* get_native_resource_d3d12() const;

    uint32_t get_rtv_count() const {
        return m_desc.num_rtv;
    }

    RenderTargetView* get_rtv(int32_t index) const {
        if (index < 0 || index >= get_rtv_count() || m_desc.rtvs == nullptr) {
            return nullptr;
        }
        
        return m_desc.rtvs[index];
    }

public:
    struct Desc {
#if TDB_VER <= 67
        void* _unk_pad;
#endif
        RenderTargetView** rtvs;
        DepthStencilView* dsv;
        uint32_t num_rtv;
        float left;
        float right;
        float top;
        float bottom;
        uint32_t flag;
    } m_desc;

    // more here but not needed... for now
};
#if TDB_VER > 67
static_assert(offsetof(TargetState, m_desc) + offsetof(TargetState::Desc, num_rtv) == 0x20);
#else
static_assert(offsetof(TargetState, m_desc) + offsetof(TargetState::Desc, num_rtv) == 0x28);
#endif

class Buffer : public RenderResource {
public:
    uint32_t m_size_in_bytes;
    uint32_t m_usage_type;
    uint32_t m_option_flags;
};

class ConstantBuffer : public Buffer {
public:
    uint32_t m_update_times;
};

class ConstantBufferDX12 : public ConstantBuffer {
public:
    void* get_desc() {
        return (void*)((uintptr_t)this + sizeof(ConstantBuffer));
    }
};

class RenderLayer : public REManagedObject {
public:
    RenderLayer* add_layer(::REType* layer_type, uint32_t priority, uint8_t offset = 0);
    void add_layer(RenderLayer* existing_layer) {
        m_layers.push_back(existing_layer);
        existing_layer->m_parent = this;
    }
    sdk::NativeArray<RenderLayer*>& get_layers();
    RenderLayer** find_layer(::REType* layer_type);
    std::tuple<RenderLayer*, RenderLayer**> find_layer_recursive(const ::REType* layer_type); // parent, type
    std::tuple<RenderLayer*, RenderLayer**> find_layer_recursive(std::string_view type_name); // parent, type
    std::vector<RenderLayer*> find_layers(::REType* layer_type);
    
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
    sdk::renderer::SceneInfo* get_jitter_disable_post_scene_info();
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

    void* get_g_buffer_target_d3d12() {
        return get_target_state_resource_d3d12("GBufferTarget");
    }
};

class PrepareOutput : public sdk::renderer::RenderLayer {
public:
    sdk::renderer::TargetState* get_output_state() {
        return *(sdk::renderer::TargetState**)((uintptr_t)this + s_output_state_offset);
    }

    void set_output_state(sdk::renderer::TargetState* state) {
        *(sdk::renderer::TargetState**)((uintptr_t)this + s_output_state_offset) = state;
    }

private:
#if TDB_VER >= 70
    static constexpr inline auto s_output_state_offset = 0xF8;
#else
    static constexpr inline auto s_output_state_offset = 0xF8; // TODO! VERIFY!
#endif
};

class Overlay : public sdk::renderer::RenderLayer {
};

class PostEffect : public sdk::renderer::RenderLayer {
};
}

class Renderer {
public:
    ConstantBuffer* get_constant_buffer(std::string_view name) const;

    ConstantBuffer* get_scene_info() const {
        return get_constant_buffer("SceneInfo");
    }

    ConstantBuffer* get_shadow_cast_info() const {
        return get_constant_buffer("ShadowCastInfo");
    }

    ConstantBuffer* get_environment_info() const {
        return get_constant_buffer("EnvironmentInfo");
    }

    ConstantBuffer* get_fog_parameter() const {
        return get_constant_buffer("FogParameter");
    }
};

Renderer* get_renderer();

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

ConstantBuffer* create_constant_buffer(void* desc);
TargetState* create_target_state(TargetState::Desc* desc);
Texture* create_texture(void* desc);
RenderTargetView* create_render_target_view(sdk::renderer::RenderResource* resource, void* desc);
}
}