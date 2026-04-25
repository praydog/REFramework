#pragma once

#include <cstdint>
#include <tuple>
#include <optional>

#include "ReClass.hpp"
#include "RENativeArray.hpp"
#include "renderer/RenderResource.hpp"
#include "renderer/PipelineState.hpp"
#include "intrusive_ptr.hpp"
#include "ManagedObject.hpp"

#include "GameIdentity.hpp"

class REType;
struct ID3D12Resource;

namespace sdk {
namespace renderer {
namespace layer {
class Scene;
}

struct SceneInfo {
    Matrix4x4f view_projection_matrix;
    Matrix4x4f view_matrix;
    Matrix4x4f inverse_view_matrix;
    Matrix4x4f projection_matrix;
    Matrix4x4f inverse_projection_matrix;
    Matrix4x4f inverse_view_projection_matrix;
    Matrix4x4f old_view_projection_matrix;
};

struct Rect {
    float left;
    float top;
    float right;
    float bottom;
};

class TargetState;

template<typename T>
class DirectXResource : public RenderResource {
public:
    T* get_native_resource() const {
        // sizeof(RenderResource) varies by TDB version at runtime in universal.
        // Use the runtime dispatch helper to get the correct offset.
        return *(T**)((uintptr_t)this + RenderResource::get_runtime_size());
    }

private:
};

class Texture : public RenderResource {
public:
    struct Desc {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mip;
        uint32_t arr;
        uint32_t format; // DXGI_FORMAT/via.render.TextureFormat

        // rest dont care
    };

    Texture* clone();
    Texture* clone(uint32_t new_width, uint32_t new_height) {
        // Modifying description directly as dont have full definition of the Desc struct
        auto desc = get_desc();

        const auto old_width = desc->width;
        const auto old_height = desc->height;

        desc->width = new_width;
        desc->height = new_height;

        auto new_texture = clone();

        desc->width = old_width;
        desc->height = old_height;

        return new_texture;
    }

    Desc* get_desc() {
        return (Desc*)((uintptr_t)this + get_s_desc_offset());
    }

    DirectXResource<ID3D12Resource>* get_d3d12_resource_container();

private:
    // desc sits at sizeof(RenderResource) + one void* for older games, +0x18 for TDB >= 73 / SF6.
    static inline uintptr_t get_s_desc_offset() {
        const auto& gi = sdk::GameIdentity::get();
        const auto v = gi.tdb_ver();
        if (v >= 73 || gi.is_sf6()) {
            return RenderResource::get_runtime_size() + 0x18;
        }
        return RenderResource::get_runtime_size() + sizeof(void*);
    }

    static inline uintptr_t get_s_d3d12_resource_offset() {
        const auto& gi = sdk::GameIdentity::get();
        const auto v = gi.tdb_ver();
        if (v >= 73) return 0xE0;
        if (v >= 71) {
            if (gi.is_sf6()) return 0xB8;
            if (gi.is_mhrise()) return 0x98; // WHAT THE HECK!!!
            return 0xA0;
        }
        return 0x98; // TDB 69, 70 and below (verified in DMC5)
    }
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

    sdk::intrusive_ptr<RenderTargetView> clone();
    sdk::intrusive_ptr<RenderTargetView> clone(uint32_t new_width, uint32_t new_height);

    Desc& get_desc() {
        return *reinterpret_cast<Desc*>((uintptr_t)this + RenderResource::get_runtime_size());
    }
    const Desc& get_desc() const {
        return *reinterpret_cast<const Desc*>((uintptr_t)this + RenderResource::get_runtime_size());
    }

    sdk::intrusive_ptr<Texture>& get_texture_d3d12() const;
    sdk::intrusive_ptr<TargetState>& get_target_state_d3d12() const;

private:
    Desc m_desc;
};

static_assert(sizeof(RenderTargetView::Desc) == 0x14);

class TargetState : public RenderResource {
public:
    struct Desc;

    ID3D12Resource* get_native_resource_d3d12() const;
    sdk::intrusive_ptr<TargetState> clone() const;
    sdk::intrusive_ptr<TargetState> clone(const std::vector<std::array<uint32_t, 2>>& new_dimensions) const;

    // The compiled `m_desc` member is at `sizeof(compiled RenderResource)` bytes
    // past `this`, but the real per-game RenderResource size varies (0x10 / 0x18 / 0x20).
    // `get_desc_ptr()` / `get_desc()` compute the runtime offset and return a reference
    // into the actual game memory.
    uintptr_t get_desc_base() const {
        return (uintptr_t)this + RenderResource::get_runtime_size();
    }

    Desc& get_desc() {
        return *reinterpret_cast<Desc*>(get_desc_base());
    }

    const Desc& get_desc() const {
        return *reinterpret_cast<const Desc*>(get_desc_base());
    }

    uint32_t get_rtv_count() const {
        return get_desc().num_rtv;
    }

    sdk::intrusive_ptr<RenderTargetView>* get_rtvs_ptr() const {
        return get_desc().rtvs;
    }

    sdk::intrusive_ptr<RenderTargetView> get_rtv(int32_t index) const {
        auto rtvs = get_rtvs_ptr();
        if (index < 0 || (uint32_t)index >= get_rtv_count() || rtvs == nullptr) {
            return nullptr;
        }
        return rtvs[index];
    }

    void set_rtv(int32_t index, RenderTargetView* rtv) {
        auto rtvs = get_rtvs_ptr();
        if (index < 0 || (uint32_t)index >= get_rtv_count() || rtvs == nullptr) {
            return;
        }
        rtvs[index] = rtv;
    }

public:
    struct Desc {
        sdk::intrusive_ptr<RenderTargetView>* rtvs;
        sdk::intrusive_ptr<DepthStencilView> dsv;
        uint32_t num_rtv;
        Rect rect;
        uint32_t flag;
    } m_desc;

    // more here but not needed... for now
};

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
        // TODO (universal): sizeof(ConstantBuffer) = sizeof(RenderResource) + Buffer fields + m_update_times.
        // In the universal build sizeof(RenderResource) is always 0x20 (compile-time includes both padding
        // fields), but on RE2/RE3/RE7 (TDB 70) the real in-game size is 0x18, and on DMC5 (TDB 67) it is
        // 0x10. This makes get_desc() return a pointer 8–16 bytes past the real Desc on those games.
        // Currently ConstantBufferDX12 has no call sites so this is latent, not active. If the method is
        // ever wired up, replace sizeof(ConstantBuffer) with a runtime-computed offset that accounts for
        // RenderResource::get_runtime_size() + sizeof(Buffer::m_size_in_bytes/m_usage_type/m_option_flags)
        // + sizeof(m_update_times).
        return (void*)((uintptr_t)this + sizeof(ConstantBuffer));
    }
};

class RenderLayer : public sdk::ManagedObject {
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
    std::vector<layer::Scene*> find_all_scene_layers();
    std::vector<layer::Scene*> find_fully_rendered_scene_layers();
    
    RenderLayer* get_parent();
    void set_parent(RenderLayer* layer);
    RenderLayer* find_parent(::REType* layer_type);
    RenderLayer* clone(bool recursive = false);
    void clone(RenderLayer* other, bool recursive = false);
    void clone_layers(RenderLayer* other, bool recursive = false);

    ::sdk::renderer::TargetState* get_target_state(std::string_view name);

    ID3D12Resource* get_target_state_resource_d3d12(std::string_view name) {
        auto state = get_target_state(name);
        return state != nullptr ? state->get_native_resource_d3d12() : nullptr;
    }

    static inline uint32_t get_draw_vtable_index() {
        const auto ver = sdk::GameIdentity::get().tdb_ver();
        if (ver >= 81) return 15;
        if (ver >= 69) return 14;
        if (ver > 49) return 12;
        return 10;
    }

    static inline uint32_t get_update_vtable_index() {
        return get_draw_vtable_index() + 1;
    }

    static inline uint32_t get_num_priority_offsets() {
        const auto ver = sdk::GameIdentity::get().tdb_ver();
        if (ver >= 69) return 7;
        if (ver >= 66) return 6;
        return 0;
    }

    static constexpr uint32_t MAX_PRIORITY_OFFSETS = 7;

    // Returns the actual in-game sizeof(RenderLayer) at runtime.
    // The universal binary compiles m_priority_offsets[MAX_PRIORITY_OFFSETS] (7 slots = 28 bytes),
    // but TDB 66-67 games (e.g. DMC5) only have 6 slots in their real struct (24 bytes),
    // making sizeof(RenderLayer) 4 bytes too large for those games.
    // Any code that uses sizeof(RenderLayer) to locate members of a derived class
    // (e.g. Overlay) must call this instead.
    static inline uintptr_t get_runtime_sizeof() {
        const auto actual = get_num_priority_offsets();
        if (actual < MAX_PRIORITY_OFFSETS) {
            // e.g. DMC5 (TDB 67) has 6 slots; subtract the extra 4 bytes per missing slot.
            return sizeof(RenderLayer) - (MAX_PRIORITY_OFFSETS - actual) * sizeof(uint32_t);
        }
        // TDB >= 69: 7 slots — matches the compiled-in array size.
        return sizeof(RenderLayer);
    }

    void draw(void* render_context) {
        const auto vtable = *(void(***)(void*, void*))this;
        return vtable[get_draw_vtable_index()](this, render_context);
    }

    void update() {
        const auto vtable = *(void(***)(void*))this;
        return vtable[get_update_vtable_index()](this);
    }

public:
    uint32_t m_id;
    uint32_t m_render_output_id;
    uint32_t m_render_output_id_2;

#ifdef REFRAMEWORK_UNIVERSAL
    uint32_t m_priority;
    // NOTE: In the universal build this array is always MAX_PRIORITY_OFFSETS (7) slots.
    // The TDB ≤49 member ordering (m_parent / m_layers / m_priority without an offset array)
    // is *not* compiled in universal mode — that layout was only supported in the old
    // per-game builds where TDB_VER <= 49 was set at compile time.
    // If a TDB ≤49 game (e.g. the legacy RE7 TDB49 build) is ever added to the universal
    // roster, the entire RenderLayer layout must be revisited; the current struct will be
    // silently wrong (members in the wrong order).  Add the new GameID to the list below
    // when that happens so this comment can be updated.
    // Current universal roster minimum TDB: 67 (DMC5). TDB ≤49 is not supported. ✅
    uint32_t m_priority_offsets[MAX_PRIORITY_OFFSETS];
    sdk::renderer::RenderLayer* m_parent;
    sdk::NativeArray<sdk::renderer::RenderLayer*> m_layers;
#else
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

#ifndef REFRAMEWORK_UNIVERSAL
#if TDB_VER <= 49
static_assert(offsetof(RenderLayer, m_priority) == 0x48, "RenderLayer::m_priority offset is wrong");
static_assert(offsetof(RenderLayer, m_layers) == 0x38, "RenderLayer::m_layers offset is wrong");
static_assert(offsetof(RenderLayer, m_parent) == 0x30, "RenderLayer::m_parent offset is wrong");
#endif
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

    sdk::renderer::TargetState* get_present_output_state() {
        return get_target_state("PresentState");
    }
};

class Scene : public sdk::renderer::RenderLayer {
public:
    uint32_t get_view_id() const;
    RECamera* get_camera() const;
    RECamera* get_main_camera_if_possible() const;
    REManagedObject* get_mirror() const;
    bool is_enabled() const;

    bool has_main_camera() const {
        return get_main_camera_if_possible() != nullptr;
    }

    bool is_fully_rendered() const {
        return is_enabled() && get_mirror() == nullptr && has_main_camera();
    }

    sdk::renderer::SceneInfo* get_scene_info();
    sdk::renderer::SceneInfo* get_depth_distortion_scene_info();
    sdk::renderer::SceneInfo* get_filter_scene_info();
    sdk::renderer::SceneInfo* get_jitter_disable_scene_info();
    sdk::renderer::SceneInfo* get_jitter_disable_post_scene_info();
    sdk::renderer::SceneInfo* get_z_prepass_scene_info();

    Texture* get_depth_stencil();
    TargetState* get_motion_vectors_state();
    ID3D12Resource* get_depth_stencil_d3d12();

    ID3D12Resource* get_motion_vectors_d3d12() {
        return get_target_state_resource_d3d12("VelocityTarget");
    }

    ID3D12Resource* get_post_main_target_d3d12() {
        return get_target_state_resource_d3d12("PostMainTarget");
    }

    ID3D12Resource* get_hdr_target_d3d12() {
        return get_target_state_resource_d3d12("HDRTarget");
    }

    auto get_hdr_target() {
        return get_target_state("HDRTarget");
    }

    auto get_depth_target() {
        return get_target_state("DepthTarget");
    }

    ID3D12Resource* get_g_buffer_target_d3d12() {
        return get_target_state_resource_d3d12("GBufferTarget");
    }

    void set_lod_bias(float x, float y) {
        if (sdk::GameIdentity::get().is_re4()) {
            *(float*)((uintptr_t)this + s_lod_bias_offset) = x;
            *(float*)((uintptr_t)this + s_lod_bias_offset + 4) = y;
        }
    }

private:
    constexpr static auto s_lod_bias_offset = 0x1818; // RE4 offset, only used when is_re4()
};

class PrepareOutput : public sdk::renderer::RenderLayer {
public:
    sdk::renderer::TargetState* get_output_state() {
        if (auto offset = get_output_state_offset()) {
            return *(sdk::renderer::TargetState**)((uintptr_t)this + *offset);
        }

        return nullptr;
    }

    void set_output_state(sdk::renderer::TargetState* state) {
        if (auto offset = get_output_state_offset()) {
            if (state != nullptr) {
                state->add_ref();
            }

            auto& current_state = *(sdk::renderer::TargetState**)((uintptr_t)this + *offset);
            current_state = state;
        }
    }

    std::optional<size_t> get_output_state_offset();
};

class Overlay : public sdk::renderer::RenderLayer {
public:
    sdk::intrusive_ptr<sdk::renderer::TargetState>& get_main_target_state() {
        return *(sdk::intrusive_ptr<sdk::renderer::TargetState>*)((uintptr_t)this + RenderLayer::get_runtime_sizeof() + sizeof(void*));
    }

    sdk::intrusive_ptr<sdk::renderer::TargetState>& get_main_depth_target_state() {
        return *(sdk::intrusive_ptr<sdk::renderer::TargetState>*)((uintptr_t)this + RenderLayer::get_runtime_sizeof());
    }

    sdk::renderer::Texture** get_b8g8r8a8_unorm_textures() {
        return (sdk::renderer::Texture**)((uintptr_t)this + s_b8g8r8a8_unorm_textures_offset);
    }

    sdk::renderer::Texture** get_r8g8b8a8_unorm_textures() {
        return (sdk::renderer::Texture**)((uintptr_t)this + s_r8g8b8a8_unorm_textures_offset);
    }

    sdk::renderer::TargetState** get_b8g8r8a8_unorm_target_states() {
        return (sdk::renderer::TargetState**)((uintptr_t)this + s_b8g8r8a8_unorm_target_state_offset);
    }

    sdk::renderer::TargetState** get_b8g8r8a8_unorm_target_only_states() {
        return (sdk::renderer::TargetState**)((uintptr_t)this + s_b8g8r8a8_unorm_target_only_state_offset);
    }

private:
    // Offsets verified in SF6/RE4 (TDB >= 71); other games copy these as best-guess defaults.
    // TODO (universal): these are compile-time constants but the true values vary across TDB versions.
    // All get_b8g8r8a8_unorm_*/get_r8g8b8a8_unorm_* accessors below use these offsets unconditionally
    // for every game in the universal build (DMC5, RE7, RE8, RE2, RE3, MHRISE, MHWILDS, etc.).
    // If any of these methods are exercised on a game where the offsets differ from the RE4/SF6 baseline,
    // they will read garbage or crash without warning.  Before activating these in non-RE4/SF6 code paths,
    // verify the per-TDB offsets and replace these constexprs with a runtime dispatcher
    // (see RenderOutput::get_s_scene_layers_offset() for the pattern to follow), or gate the callers
    // with is_re4() || is_sf6() until the other games' offsets are confirmed.
    static constexpr inline auto s_b8g8r8a8_unorm_textures_offset = 0x288;
    static constexpr inline auto s_r8g8b8a8_unorm_textures_offset = 0x260;
    static constexpr inline auto s_b8g8r8a8_unorm_target_state_offset = 0x1A0;
    static constexpr inline auto s_b8g8r8a8_unorm_target_only_state_offset = 0x1E0;
};

class PostEffect : public sdk::renderer::RenderLayer {
};
}

class RenderOutput {
public:
    sdk::NativeArray<sdk::renderer::layer::Scene*>& get_scene_layers() {
        return *(sdk::NativeArray<sdk::renderer::layer::Scene*>*)((uintptr_t)this + get_s_scene_layers_offset());
    }

private:
    static inline uintptr_t get_s_scene_layers_offset() {
        const auto v = sdk::GameIdentity::get().tdb_ver();
        if (v >= 71) return 0x98;
        if (v >= 70) return 0xB8;
        if (v == 69) return 0x98;
        return 0x98; // TODO! VERIFY!
    }
};

struct Fence {
    int32_t state{-2};
    uint32_t unk2{0};
    uint32_t unk3{0};
    uint32_t unk4{0x70};
};

namespace command {
struct Base {
    uint32_t t : 8;
    uint32_t size : 24;
    uint64_t priority{};
};

static_assert(sizeof(Base) == 0x10);

struct Clear : public Base {
    uint32_t clear_type{};
    sdk::renderer::TargetState* target; // target state/uav
    union {
        sdk::renderer::RenderTargetView* rtv;
        sdk::renderer::DepthStencilView* dsv;
    } view;

    float clear_color[4]{};
};

static_assert(sizeof(Clear) == 0x38);

struct FenceBase : public Base {
    const char* name{};
    sdk::renderer::Fence fence{};
};

static_assert(sizeof(FenceBase) == 0x28);

struct Fence : public FenceBase {
    bool wait{};
    bool begin;
    bool end;
    bool complete;
};

static_assert(sizeof(command::Fence) == 0x30);

struct CopyBase : public Base {
    sdk::renderer::RenderResource* src{};
    sdk::renderer::RenderResource* dst{};
    ::sdk::renderer::Fence fence{};
};

struct CopyTexture : public CopyBase {
    int32_t src_subresource{-1};
    int32_t dst_subresource{-1};
};

static_assert(sizeof(CopyBase) == 0x30);
static_assert(sizeof(CopyTexture) == 0x38);
}

class RenderContext {
public:
    void set_pipeline_state(PipelineState* state);

    // tgx and y are usually width and height
    void dispatch_ray(uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z, Fence& fence);
    void dispatch_32bit_constant(uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z, uint32_t constant, bool disable_uav_barrier);
    void dispatch(uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z, bool disable_uav_barrier);

    // via::render::command::TypeId, can change between engine versions
    command::Base* alloc(uint32_t t, uint32_t size);
    void clear_rtv(sdk::renderer::RenderTargetView* rtv, float color[4], bool delay = false);
    void clear_rtv(sdk::renderer::RenderTargetView* rtv, bool delay = false) {
        float color[4]{0.0f, 0.0f, 0.0f, 0.0f};
        clear_rtv(rtv, color, delay);
    }

    void copy_texture(Texture* dest, Texture* src, Fence& fence);
    void copy_texture(Texture* dest, Texture* src) {
        Fence fence{};
        copy_texture(dest, src, fence);
    }

public:
    uint32_t get_protect_frame() const {
        return *(uint32_t*)((uintptr_t)this + s_protect_frame_offset);
    }

    sdk::renderer::TargetState* get_render_target() const {
        return *(sdk::renderer::TargetState**)((uintptr_t)this + s_current_render_state_offset);
    }

    bool is_delay_enabled() const {
        return *(bool*)((uintptr_t)this + s_is_delay_enabled_offset);
    }

    static constexpr inline auto s_protect_frame_offset = 0x68;
    static constexpr inline auto s_is_delay_enabled_offset = 0x7B;
    static constexpr inline auto s_current_render_state_offset = 0x98;
};

class Renderer {
public:
    void* get_device() const {
        return *(void**)((uintptr_t)this + sizeof(void*)); // simple!
    }

    std::optional<uint32_t> get_render_frame() const;
    
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
Texture* create_texture(Texture::Desc* desc);
RenderTargetView* create_render_target_view(sdk::renderer::RenderResource* resource, void* desc);
}
}
