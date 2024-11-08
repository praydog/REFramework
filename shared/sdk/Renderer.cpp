#include <algorithm>

#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "Application.hpp"
#include "RETypeDB.hpp"
#include "SceneManager.hpp"

#include "Renderer.hpp"

namespace detail {
using AddSceneViewFn = void (*)(void*);
AddSceneViewFn get_add_scene_view() {
    static void (*add_scene_view_fn)(void*) = nullptr;

    if (add_scene_view_fn == nullptr) {
        spdlog::info("[Renderer] Finding add_scene_view_fn");

        /*
        .text:0000000142952B16 41 8B 86 B4 0A 00 00                          mov     eax, [r14+0AB4h]
        .text:0000000142952B1D 83 E0 01                                      and     eax, 1
        .text:0000000142952B20 48 83 C0 1C                                   add     rax, 1Ch
        .text:0000000142952B24 48 69 C0 88 00 00 00                          imul    rax, 88h
        .text:0000000142952B2B 49 03 C6                                      add     rax, r14
        .text:0000000142952B2E 49 89 86 F0 0F 00 00                          mov     [r14+0FF0h], rax
        .text:0000000142952B35 48 8B 3D 4C 0C 39 06                          mov     rdi, cs:g_scene_manager
        .text:0000000142952B3C 48 8B 5F 20                                   mov     rbx, [rdi+20h]
        .text:0000000142952B40 48 8B CB                                      mov     rcx, rbx        ; lpCriticalSection
        .text:0000000142952B43 FF 15 67 E7 F6 01                             call    cs:EnterCriticalSection
        .text:0000000142952B49 45 33 C9                                      xor     r9d, r9d
        .text:0000000142952B4C 4C 8D 05 8D 0F 00 00                          lea     r8, addSceneView(via::SceneView*)
        */
        // String refs in the function containing this pattern:
        // L"Renderer::DelayEndTask"
        // L"Renderer::DelayReleaseTask"
        const auto mod = utility::get_executable();
        auto ref = utility::scan(mod, "4C 8D 05 ? ? ? ? 48 8D ? ? 48 8D ? 08 E8 ? ? ? ? 48 ? ? FF 15");

        if (!ref) {
            spdlog::error("[Renderer] Failed to find add_scene_view_fn");
            return nullptr;
        }

        add_scene_view_fn = (decltype(add_scene_view_fn))utility::calculate_absolute(*ref + 3);

        spdlog::info("[Renderer] add_scene_view_fn: {:x}", (uintptr_t)add_scene_view_fn);
    }

    return add_scene_view_fn;
}
}

namespace sdk {
namespace renderer {
RenderLayer* RenderLayer::add_layer(::REType* layer_type, uint32_t priority, uint8_t offset) {
    // can be found inside addSceneView
    static RenderLayer* (*add_layer_fn)(RenderLayer*, ::REType*, uint32_t, uint8_t) = nullptr;
    
    if (add_layer_fn == nullptr) {
        spdlog::info("[Renderer] Finding RenderLayer::AddLayer");

        const auto mod = utility::get_executable();
        
        auto ref = utility::scan(mod, "41 B8 00 00 00 05 48 8B F8 E8 ? ? ? ?"); // mov r8d, 5000000h; call add_layer

        if (!ref) {
            // Fallback pattern
            ref = utility::scan(mod, "41 B8 00 00 00 05 48 89 C7 E8 ? ? ? ?"); // mov r8d, 5000000h; call add_layer

            if (!ref) {
                auto add_scene_view_fn = detail::get_add_scene_view();

                if (add_scene_view_fn != nullptr) {
                    // Use a disassembler to scan through the function
                    // to find the call to add_scene_view_fn
                    // the function will be called multiple times, and will be the most called function within AddSceneView
                    spdlog::info("[Renderer] Scanning for RenderLayer::AddLayer using disassembler");
                    const auto potential_jmp = utility::scan_opcode((uintptr_t)add_scene_view_fn, 4, 0xE9);

                    if (potential_jmp) {
                        add_scene_view_fn = (decltype(add_scene_view_fn))utility::calculate_absolute(*potential_jmp + 1);
                        spdlog::info("[Renderer] Jmp detected, add_scene_view_fn: {:x}", (uintptr_t)add_scene_view_fn);
                    }

                    uintptr_t ip = (uintptr_t)add_scene_view_fn;

                    std::unordered_map<uintptr_t, uint32_t> calls;
                    uintptr_t best_call = 0;

                    for (auto i = 0 ; i < 150; ++i) {
                        const auto decoded = utility::decode_one((uint8_t*)ip);

                        if (!decoded) {
                            spdlog::error("[Renderer] Failed to decode instruction @ 0x{:x} ({:x})", ip, ip - (uintptr_t)add_scene_view_fn);
                            break;
                        }

                        if (std::string_view{decoded->Mnemonic}.starts_with("RET") || std::string_view{decoded->Mnemonic}.starts_with("INT3")) {
                            spdlog::error("[Renderer] Encountering RET or INT3 @ 0x{:x} ({:x})", ip, ip - (uintptr_t)add_scene_view_fn);
                            break;
                        }

                        if (*(uint8_t*)ip == 0xE8) {
                            const auto addr = utility::calculate_absolute(ip + 1);
                            calls[addr]++;

                            if (best_call != 0) {
                                if (calls[best_call] < calls[addr]) {
                                    best_call = addr;
                                }
                            } else {
                                best_call = addr;
                            }

                            if (calls[addr] >= 3) {
                                spdlog::info("[Renderer] Found 3 calls to add_scene_view_fn, stopping scan");
                                break;
                            }
                        }

                        ip += decoded->Length;
                    }

                    if (best_call != 0) {
                        spdlog::info("[Renderer] RenderLayer::AddLayer found at {:x}", best_call);
                        add_layer_fn = (decltype(add_layer_fn))best_call;
                    } else {
                        spdlog::error("[Renderer] Failed to find RenderLayer::AddLayer using a disassembler");
                    }
                }

                if (!ref && add_layer_fn == nullptr) {
                    spdlog::error("[Renderer] Failed to find add_layer");
                    return nullptr;
                }
            }
        }

        if (add_layer_fn == nullptr) {
            add_layer_fn = (decltype(add_layer_fn))utility::calculate_absolute(*ref + 10);

            if (add_layer_fn == nullptr || IsBadReadPtr(add_layer_fn, sizeof(add_layer_fn))) {
                spdlog::error("[Renderer] Failed to calculate add_layer");
                return nullptr;
            }
        }

        spdlog::info("[Renderer] RenderLayer::AddLayer: {:x}", (uintptr_t)add_layer_fn);
    }

    return add_layer_fn(this, layer_type, priority, offset);
}

sdk::NativeArray<RenderLayer*>& RenderLayer::get_layers() {
    static uint32_t layers_offset = 0;

    if (layers_offset == 0) {
        spdlog::info("[Renderer] Finding RenderLayer::layers");

        const auto root_layer = sdk::renderer::get_root_layer();
        
        if (root_layer == nullptr) {
            spdlog::error("[Renderer] Failed to find root layer");
            throw std::runtime_error("[Renderer] Failed to find root layer");
        }
        
        // Scan through the root layer for a pointer to a RenderLayer object
        for (auto i = 0; i < 0x500; i += sizeof(void*)) {
            auto ptr = *(RenderLayer***)((uintptr_t)root_layer + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(ptr))) {
                continue;
            }

            const auto potential_layer = *ptr;

            if (potential_layer == nullptr || IsBadReadPtr(potential_layer, sizeof(potential_layer))) {
                continue;
            }

            if (!utility::re_managed_object::is_managed_object(potential_layer)) {
                continue;
            }

            if (utility::re_managed_object::is_a(potential_layer, "via.render.RenderLayer")) {
                layers_offset = i;
                break;
            }
        }

        spdlog::info("[Renderer] RenderLayer::layers: {:x}", layers_offset);
    }

    return *(sdk::NativeArray<RenderLayer*>*)((uintptr_t)this + layers_offset);
}

RenderLayer** RenderLayer::find_layer(::REType* layer_type) {
    const auto& layers = get_layers();

    for (auto& layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            return &layer;
        }
    }

    return nullptr;
}

std::tuple<RenderLayer*, RenderLayer**> RenderLayer::find_layer_recursive(const ::REType* layer_type) {
    const auto& layers = get_layers();

    for (auto& layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            return std::make_tuple<RenderLayer*, RenderLayer**>(this, &layer);
        }

        if (auto f = layer->find_layer_recursive(layer_type); std::get<0>(f) != nullptr && std::get<1>(f) != nullptr) {
            return f;
        }
    }

    return std::make_tuple<RenderLayer*, RenderLayer**>(nullptr, nullptr);
}

std::tuple<RenderLayer*, RenderLayer**> RenderLayer::find_layer_recursive(std::string_view type_name) {
    const auto def = sdk::find_type_definition(type_name);

    if (def == nullptr) {
        return std::make_tuple<RenderLayer*, RenderLayer**>(nullptr, nullptr);
    }

    const auto t = def->get_type();

    if (t == nullptr) {
        return std::make_tuple<RenderLayer*, RenderLayer**>(nullptr, nullptr);
    }

    return find_layer_recursive(t);
}

std::vector<RenderLayer*> RenderLayer::find_layers(::REType* layer_type) {
    std::vector<RenderLayer*> out{};

    const auto& layers = get_layers();

    for (auto& layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            out.push_back(layer);
        }
    }

    return out;
}

std::vector<layer::Scene*> RenderLayer::find_all_scene_layers() {
    static auto scene_type = sdk::find_type_definition("via.render.layer.Scene")->get_type();

    if (scene_type == nullptr) {
        return {};
    }

    auto layers = find_layers(scene_type);

    if (layers.empty()) {
        return {};
    }

    return *(std::vector<layer::Scene*>*)&layers;
}

std::vector<layer::Scene*> RenderLayer::find_fully_rendered_scene_layers() {
    auto layers = find_all_scene_layers();

    if (layers.empty()) {
        return {};
    }

    std::erase_if(layers, [](auto& layer) {
        return !layer->is_fully_rendered();
    });

    std::sort(layers.begin(), layers.end(), [](auto& a, auto& b) {
        return a->get_view_id() < b->get_view_id();
    });

    return layers;
}

RenderLayer* RenderLayer::get_parent() {
    return sdk::call_object_func<RenderLayer*>(this, "get_Parent", sdk::get_thread_context(), this);
}

RenderLayer* RenderLayer::find_parent(::REType* layer_type) {
    for (auto parent = get_parent(); parent != nullptr; parent = parent->get_parent()) {
        if (parent->info == nullptr || parent->info->classInfo == nullptr) {
            break;
        }

        const auto t = utility::re_managed_object::get_type(parent);

        if (t == layer_type) {
            return parent;
        }
    }

    return nullptr;
}

RenderLayer* RenderLayer::clone(bool recursive) {
    auto new_layer = (RenderLayer*)utility::re_managed_object::get_type_definition(this)->create_instance_full();

    if (new_layer == nullptr) {
        spdlog::error("[Renderer] Failed to clone layer");
        return nullptr;
    }

    new_layer->clone(this, recursive);

    return new_layer;
}

void RenderLayer::clone(RenderLayer* other, bool recursive) {
    this->m_parent = other->m_parent;
    this->m_priority = other->m_priority;

#if TDB_VER > 49
    for (auto i = 0; i < sdk::renderer::RenderLayer::NUM_PRIORITY_OFFSETS; ++i) {
        this->m_priority_offsets[i] = other->m_priority_offsets[i];
    }
#endif

    this->clone_layers(other, recursive);
}

void RenderLayer::clone_layers(RenderLayer* other, bool recursive) {
    for (auto child_layer : other->get_layers()) {
        if (child_layer == this) {
            continue;
        }

        const auto def = utility::re_managed_object::get_type_definition(child_layer);

        if (def == nullptr) {
            continue;
        }

        const auto t = def->get_type();

        if (t == nullptr) {
            continue;
        }

        if (this->find_layer(t) != nullptr) {
            continue;
        }

        auto new_child_layer = add_layer(t, child_layer->m_priority);

        if (recursive && new_child_layer != nullptr) {
            new_child_layer->clone_layers(child_layer, recursive);
        }
    }
}

::sdk::renderer::TargetState* RenderLayer::get_target_state(std::string_view name) {
    return utility::re_managed_object::get_field<::sdk::renderer::TargetState*>(this, name);
}

void RenderContext::set_pipeline_state(sdk::renderer::PipelineState* pipeline_state) {
    using Fn = void (*)(RenderContext*, sdk::renderer::PipelineState*);
    static Fn set_pipeline_state_fn = []() -> Fn {
        spdlog::info("[RenderContext::set_pipeline_state] Searching for RenderContext::set_pipeline_state");

        const auto game = utility::get_executable();
        const auto string_data = utility::scan_string(game, "UpdateDepthBlockerState");

        if (!string_data) {
            spdlog::error("[RenderContext::set_pipeline_state] Failed to find UpdateDepthBlockerState string");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string_data);

        if (!string_ref) {
            spdlog::error("[RenderContext::set_pipeline_state] Failed to find UpdateDepthBlockerState reference");
            return nullptr;
        }

        std::optional<uintptr_t> current_function_call{};
        uintptr_t current_ip{*string_ref + 4};

        // First one is murmur hash calc function
        // second: MasterMaterialResource::find
        // third: RenderResource::add_ref
        // fourth: RenderContext::set_pipeline_state
        for (size_t i = 0; i < 4; ++i) {
            current_function_call = utility::scan_mnemonic(current_ip, 100, "CALL");

            if (!current_function_call) {
                spdlog::error("[RenderContext::set_pipeline_state] Failed to find next CALL instruction");
                return nullptr;
            }

            current_ip = *current_function_call + 5;
        }

        const auto result = utility::resolve_displacement(*current_function_call);

        if (!result) {
            spdlog::error("[RenderContext::set_pipeline_state] Failed to resolve displacement");
            return nullptr;
        }

        spdlog::info("[RenderContext::set_pipeline_state] Found RenderContext::set_pipeline_state at {:x}", *result);

        return (Fn)*result;
    }();

    if (set_pipeline_state_fn == nullptr) {
        return;
    }

    set_pipeline_state_fn(this, pipeline_state);
}

void RenderContext::dispatch_ray(uint32_t tgx, uint32_t tgy, uint32_t tgz, Fence& fence) {
    using Fn = void (*)(RenderContext*, uint32_t, uint32_t, uint32_t, Fence*);
    static auto func = []() -> Fn {
        spdlog::info("[RenderContext::dispatch_ray] Searching for RenderContext::dispatch_ray");

        const auto game = utility::get_executable();
        const auto string_data = utility::scan_string(game, "PathSpaceRayTracing");

        if (!string_data) {
            spdlog::error("[RenderContext::dispatch_ray] Failed to find PathSpaceRayTracing string");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string_data);

        if (!string_ref) {
            spdlog::error("[RenderContext::dispatch_ray] Failed to find PathSpaceRayTracing reference");
            return nullptr;
        }

        std::optional<uintptr_t> current_function_call{};
        uintptr_t current_ip{*string_ref + 4};

        // First one is murmur hash calc function
        // second: MasterMaterialResource::find
        // third: RenderResource::add_ref
        // fourth: RenderContext::set_pipeline_state
        // fifth: RenderResource::release
        // sixth: RenderContext::dispatch_ray
        for (size_t i = 0; i < 6; ++i) {
            current_function_call = utility::scan_mnemonic(current_ip, 100, "CALL");

            if (!current_function_call) {
                spdlog::error("[RenderContext::dispatch_ray] Failed to find next CALL instruction");
                return nullptr;
            }

            current_ip = *current_function_call + 5;
        }

        const auto result = utility::resolve_displacement(*current_function_call);

        if (!result) {
            spdlog::error("[RenderContext::dispatch_ray] Failed to resolve displacement");
            return nullptr;
        }

        spdlog::info("[RenderContext::dispatch_ray] Found RenderContext::dispatch_ray at {:x}", *result);

        return (Fn)*result;
    }();

    if (func == nullptr) {
        return;
    }

    func(this, tgx, tgy, tgz, &fence);
}

void RenderContext::dispatch_32bit_constant(uint32_t tgx, uint32_t tgy, uint32_t tgz, uint32_t constant, bool disable_uav_barrier) {
    using Fn = void (*)(RenderContext*, uint32_t, uint32_t, uint32_t, uint32_t, bool);
    static auto func = []() -> Fn {
        spdlog::info("[RenderContext::dispatch_32bit_constant] Searching for RenderContext::dispatch_32bit_constant");

        const auto game = utility::get_executable();
        const auto string_data = utility::scan_string(game, "ClearDepthBlockerState");

        if (!string_data) {
            spdlog::error("[RenderContext::dispatch_32bit_constant] Failed to find ClearDepthBlockerState string");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string_data);

        if (!string_ref) {
            spdlog::error("[RenderContext::dispatch_32bit_constant] Failed to find ClearDepthBlockerState reference");
            return nullptr;
        }

        std::optional<uintptr_t> current_function_call{};
        uintptr_t current_ip{*string_ref + 4};

        // First one is murmur hash calc function
        // second: MasterMaterialResource::find
        // third: RenderResource::add_ref
        // fourth: RenderContext::set_pipeline_state
        // fifth: RenderResource::release
        // sixth: RenderContext::dispatch_32bit_constant
        for (size_t i = 0; i < 6; ++i) {
            current_function_call = utility::scan_mnemonic(current_ip, 100, "CALL");

            if (!current_function_call) {
                spdlog::error("[RenderContext::dispatch_32bit_constant] Failed to find next CALL instruction");
                return nullptr;
            }

            current_ip = *current_function_call + 5;
        }

        const auto result = utility::resolve_displacement(*current_function_call);

        if (!result) {
            spdlog::error("[RenderContext::dispatch_32bit_constant] Failed to resolve displacement");
            return nullptr;
        }

        spdlog::info("[RenderContext::dispatch_32bit_constant] Found RenderContext::dispatch_32bit_constant at {:x}", *result);

        return (Fn)*result;
    }();

    if (func == nullptr) {
        return;
    }

    func(this, tgx, tgy, tgz, constant, disable_uav_barrier);
}

void RenderContext::dispatch(uint32_t tgx, uint32_t tgy, uint32_t tgz, bool disable_uav_barrier) {
    using Fn = void (*)(RenderContext*, uint32_t, uint32_t, uint32_t, bool);
    static auto func = []() -> Fn {
        spdlog::info("[RenderContext::dispatch] Searching for RenderContext::dispatch");

        const auto game = utility::get_executable();
        std::optional<uintptr_t> string_data{};
        const auto all_strings = utility::scan_strings(game, "Reconstruct", true); // part of path space filter routine

        if (all_strings.empty()) {
            spdlog::error("[RenderContext::dispatch] Failed to find Reconstruct strings");
            return nullptr;
        }

        for (const auto& str : all_strings) {
            if (*(uint8_t*)(str - 1) == 0) { // Makes sure this string is standalone and not in the middle of another string
                string_data = str;
                break;
            }
        }

        if (!string_data) {
            spdlog::error("[RenderContext::dispatch] Failed to find correct Reconstruct string");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string_data);

        if (!string_ref) {
            spdlog::error("[RenderContext::dispatch] Failed to find Reconstruct reference");
            return nullptr;
        }

        std::optional<uintptr_t> current_function_call{};
        uintptr_t current_ip{*string_ref + 4};

        // First one is murmur hash calc function
        // second: MasterMaterialResource::find
        // third: RenderResource::add_ref
        // fourth: RenderContext::set_pipeline_state
        // fifth: RenderResource::release
        // sixth: RenderContext::dispatch
        for (size_t i = 0; i < 6; ++i) {
            current_function_call = utility::scan_mnemonic(current_ip, 100, "CALL");

            if (!current_function_call) {
                spdlog::error("[RenderContext::dispatch] Failed to find next CALL instruction");
                return nullptr;
            }

            current_ip = *current_function_call + 5;
        }

        const auto result = utility::resolve_displacement(*current_function_call);

        if (!result) {
            spdlog::error("[RenderContext::dispatch] Failed to resolve displacement");
            return nullptr;
        }

        spdlog::info("[RenderContext::dispatch] Found RenderContext::dispatch at {:x}", *result);

        return (Fn)*result;
    }();

    if (func == nullptr) {
        return;
    }

    func(this, tgx, tgy, tgz, disable_uav_barrier);
}

/*
- 0x9B CopyImage
+ 0x93 ReadonlyDepth
- 0x8A TemporalDenoiserGBufferCombine
+ 0x85 PrevAODepth
- 0x75 InputVelocity
- 0x6D ModifiedGBufferSRV
+ 0x66 g_BilateralUpscaleDownscaledDepth
- 0x62 RE_POSTPROCESS_Color
- 0x5B CopyImage
+ 0x48 PostEffect Copy
- 0x2D InputVelocity
- 0x24 InterleaveNormalDepth
- 0x1D InterleaveNormalDepthHalf
- 0x1D RE_POSTPROCESS_Color
- 0x14 InterleaveNormalDepthWithoutGBuffer
- 0x11 CopyImage
- 0xD InterleaveNormalDepthHalfWithoutGBuffer
*/
void RenderContext::copy_texture(Texture* dest, Texture* src, Fence& fence) {
    static auto func = []() -> void (*)(RenderContext*, Texture*, Texture*, Fence&) {
        spdlog::info("Searching for RenderContext::copy_texture");

        std::vector<std::string> string_choices {
            "InterleaveNormalDepthHalfWithoutGBuffer",
            "CopyImage",
        };

        for (const auto& str_choice : string_choices) {
            spdlog::info("Scanning for string: {}", str_choice);

            const auto game = utility::get_executable();
            const auto string = utility::scan_string(game, str_choice, true);

            if (!string) {
                spdlog::error("Failed to find copy_texture (no string)");
                continue;
            }

            const auto string_ref = utility::scan_displacement_reference(game, *string);

            if (!string_ref) {
                spdlog::error("Failed to find copy_texture (no string ref)");
                continue;
            }

            uintptr_t ip = *string_ref;

            for (auto i = 0; i < 20; ++i) {
                const auto resolved = utility::resolve_instruction(ip);

                if (!resolved) {
                    spdlog::error("Failed to find copy_texture (could not resolve instruction)");
                    continue;
                }

                ip = resolved->addr;

                if (*(uint8_t*)ip == 0xE8) {
                    const auto result = (void (*)(RenderContext*, Texture*, Texture*, Fence&))utility::calculate_absolute(ip + 1);

                    spdlog::info("Found copy_texture: {:x}", (uintptr_t)result);
                    return result;
                }

                ip -= 1;
            }
        }

        spdlog::error("Could not find copy_texture");
        return nullptr;
    }();

    func(this, dest, src, fence);
}

std::optional<uint32_t> Renderer::get_render_frame() const {
    static auto tdef = sdk::find_type_definition("via.render.Renderer");
    static auto m = tdef != nullptr ? tdef->get_method("get_RenderFrame") : nullptr;

    if (m == nullptr) {
        return std::nullopt;
    }

    return m->call<uint32_t>(sdk::get_thread_context(), this);
}

ConstantBuffer* Renderer::get_constant_buffer(std::string_view name) const {
    static auto tdef = sdk::find_type_definition("via.render.Renderer");
    static auto t = tdef->get_type();
    const auto field_desc = utility::re_type::get_field_desc(t, name);
    return utility::re_managed_object::get_field<ConstantBuffer*>((::REManagedObject*)this, field_desc);
}

Renderer* get_renderer() {
    return (Renderer*)sdk::get_native_singleton("via.render.Renderer");
}

void wait_rendering() {
    static auto wait_rendering_entry = sdk::Application::get()->get_function("WaitRendering");

    return wait_rendering_entry->func(wait_rendering_entry->entry);
}

void begin_rendering() {
    static auto begin_rendering_entry = sdk::Application::get()->get_function("BeginRendering");

    return begin_rendering_entry->func(begin_rendering_entry->entry);
}

void end_rendering() {
    static auto end_rendering_entry = sdk::Application::get()->get_function("EndRendering");

    return end_rendering_entry->func(end_rendering_entry->entry);
}

void begin_update_primitive() {
    static auto begin_update_primitive_entry = sdk::Application::get()->get_function("BeginUpdatePrimitive");

    return begin_update_primitive_entry->func(begin_update_primitive_entry->entry);
}

void update_primitive() {
    static auto update_primitive_entry = sdk::Application::get()->get_function("UpdatePrimitive");

    return update_primitive_entry->func(update_primitive_entry->entry);
}

void end_update_primitive() {
    static auto end_update_primitive_entry = sdk::Application::get()->get_function("EndUpdatePrimitive");

    return end_update_primitive_entry->func(end_update_primitive_entry->entry);
}

void add_scene_view(void* scene_view) {
    detail::get_add_scene_view()(scene_view);
}

void remove_scene_view(void* scene_view) {
    static void (*remove_scene_view_fn)(void*) = nullptr;

    if (remove_scene_view_fn == nullptr) {
        spdlog::info("[Renderer] Finding remove_scene_view_fn");

        // Almost the same as add_scene_view pattern, is set up right after add_scene_view
        const auto mod = utility::get_executable();
        auto ref = utility::scan(mod, "4C 8D 05 ? ? ? ? 48 8D ? ? ? 48 8D ? 28 E8 ? ? ? ? 48 ? ? FF 15");

        if (!ref) {
            spdlog::error("[Renderer] Failed to find remove_scene_view_fn");
            return;
        }

        remove_scene_view_fn = (decltype(remove_scene_view_fn))utility::calculate_absolute(*ref + 3);

        spdlog::info("[Renderer] remove_scene_view_fn: {:x}", (uintptr_t)remove_scene_view_fn);
    }

    remove_scene_view_fn(scene_view);
}

RenderLayer* get_root_layer() {
    auto renderer = sdk::get_native_singleton("via.render.Renderer");

    if (renderer == nullptr) {
        spdlog::error("[Renderer] Failed to find renderer");
        return nullptr;
    }

    static uint32_t root_layer_offset = 0;

    if (root_layer_offset == 0) {
        spdlog::info("[Renderer] Finding root_layer_offset");

        auto get_output_layer_fn = sdk::find_native_method("via.render.Renderer", "getOutputLayer");

        if (get_output_layer_fn == nullptr) {
            spdlog::error("[Renderer] Failed to find getOutputLayer");

            // Hacky fix for >= TDB74
            for (uint32_t i = 0; i < 0x10000; i += sizeof(void*)) {
                const auto ptr = *(REManagedObject**)((uintptr_t)renderer + i);

                if (ptr == nullptr) {
                    continue;
                }

                if (!utility::re_managed_object::is_managed_object(ptr)) {
                    continue;
                }

                if (utility::re_managed_object::is_a(ptr, "via.render.RenderLayer")) {
                    root_layer_offset = i;
                    return *(RenderLayer**)((uintptr_t)renderer + root_layer_offset);
                }
            }

            spdlog::error("[Renderer] Failed to find root_layer_offset with fallback");

            return nullptr;
        }

        // Resolve the jmp to the real function
        if (((uint8_t*)get_output_layer_fn)[0] == 0xE9) {
            get_output_layer_fn = (decltype(get_output_layer_fn))utility::calculate_absolute((uintptr_t)get_output_layer_fn + 1);
        } else {
            // Scan for jump with disassembler
            spdlog::info("[Renderer] Scanning for getOutputLayer jmp");

            const auto potential_jmp = utility::scan_opcode((uintptr_t)get_output_layer_fn, 10, 0xE9);

            if (potential_jmp) {
                get_output_layer_fn = (decltype(get_output_layer_fn))utility::calculate_absolute(*potential_jmp + 1);
                spdlog::info("[Renderer] Found getOutputLayer jmp, new function {:x}", (uintptr_t)get_output_layer_fn);
            } else {
                spdlog::info("[Renderer] No jmp found");
            }
        }

        spdlog::info("[Renderer] Real getOutputLayer: {:x}", (uintptr_t)get_output_layer_fn);

        // Find the offset to the root layer (RE3, RE8)
        auto ref = utility::scan((uintptr_t)get_output_layer_fn, 0x100, "48 8B 81 ? ? ? ?");

        if (!ref) {
            // Fallback pattern to scan for (RE2)
            ref = utility::scan((uintptr_t)get_output_layer_fn, 0x100, "4C 8B 80 ? ? ? ?");

            // fallback pattern to scan for (RE7)
            if (!ref) {
                ref = utility::scan((uintptr_t)get_output_layer_fn, 0x100, "4C 8B 89 ? ? ? ?"); // mov r9, [rcx+?]
            }

            if (!ref) {
                spdlog::error("[Renderer] Failed to find root_layer_offset");
                return nullptr;
            }
        }

        root_layer_offset = *(uint32_t*)(*ref + 3);

        spdlog::info("[Renderer] root_layer_offset: {:x}", root_layer_offset);
    }

    return *(RenderLayer**)((uintptr_t)renderer + root_layer_offset);
}

RenderLayer* find_layer(::REType* layer_type) {
    auto renderer = sdk::get_native_singleton("via.render.Renderer");

    if (renderer == nullptr) {
        spdlog::error("[Renderer] Failed to find renderer");
        return nullptr;
    }

    static uint32_t layers_offset = 0;

    // Scan through the renderer object to find a RenderLayer pointer
    if (layers_offset == 0) {
        spdlog::info("[Renderer] Finding layers_offset");

        for (uint32_t i = 0; i < 0x10000; i += sizeof(void*)) {
            const auto ptr = *(REManagedObject**)((uintptr_t)renderer + i);

            if (ptr == nullptr) {
                continue;
            }

            if (!utility::re_managed_object::is_managed_object(ptr)) {
                continue;
            }

            if (utility::re_managed_object::is_a(ptr, "via.render.RenderLayer")) {
                layers_offset = i;
                break;
            }
        }

        if (layers_offset == 0) {
            spdlog::error("[Renderer] Failed to find layers_offset");
            return nullptr;
        }

        spdlog::info("[Renderer] layers_offset: {:x}", layers_offset);
    }

    const auto& layers = *(std::array<RenderLayer*, 256>*)((uintptr_t)renderer + layers_offset);

    for (auto& layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            return layer;
        }
    }

    return nullptr;
}

sdk::renderer::layer::Output* get_output_layer() {
    auto renderer_t = sdk::find_type_definition("via.render.Renderer");

    if (renderer_t == nullptr) {
        spdlog::error("[Renderer] Failed to find via.render.Renderer type");
        return nullptr;
    }

    return sdk::call_native_func<sdk::renderer::layer::Output*>(nullptr, renderer_t, "getOutputLayer", sdk::get_thread_context(), nullptr);
}

std::optional<Vector2f> world_to_screen(const Vector3f& world_pos) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return std::nullopt;
    }

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return std::nullopt;
    }

    auto context = sdk::get_thread_context();

    static auto transform_def = sdk::find_type_definition("via.Transform");
    static auto math_t = sdk::find_type_definition("via.math");

    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_axisz_method = transform_def->get_method("get_AxisZ");
    static auto world_to_screen = math_t->get_method("worldPos2ScreenPos(via.vec3, via.mat4, via.mat4, via.Size)");

    auto camera_gameobject = get_gameobject_method->call<REGameObject*>(context, camera);
    auto camera_transform = camera_gameobject->transform;

    Matrix4x4f proj{}, view{};
    float screen_size[2]{};

    auto camera_origin = sdk::get_transform_position(camera_transform);
    camera_origin.w = 1.0f;

    Vector4f camera_forward{};
    get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    camera_forward.w = 1.0f;

    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    sdk::call_object_func<void*>(main_view, "get_WindowSize", &screen_size, context, main_view);

    const Vector4f pos = Vector4f{world_pos, 1.0f};
    Vector4f screen_pos{};

    const auto delta = pos - camera_origin;

    // behind camera
    if (glm::dot(Vector3f{delta}, Vector3f{-camera_forward}) <= 0.0f) {
        return std::nullopt;
    }

    world_to_screen->call<void*>(&screen_pos, context, &pos, &view, &proj, &screen_size);

    return Vector2f{screen_pos.x, screen_pos.y};
}

/*
- 0x4B VortexelTurbulenceGPU::VelocitiesX
- 0x4A systems/shader/rayTracingDenoiserOld/rayTracingSimulation.sdf
- 0x46 systems/rendering/NullWhite.tex
- 0x46 systems/effect/Noise3D_MSK4.tex
- 0x19 UpdateDepthBlocker
- 0x19 Deinterlace
- 0x19 cbGeneratePolyline
- 0x19 cbTransformBasePoints
- 0x19 CBBakeType
- 0x19 cbGenerateBasePoints
*/
ConstantBuffer* create_constant_buffer(void* desc) {
    static auto fn = []() -> ConstantBuffer* (*)(void*, void*) {
        spdlog::info("Searching for create_constant_buffer");

        const auto game = utility::get_executable();
        const auto string = utility::scan_string(game, "cbTransformBasePoints");

        if (!string) {
            spdlog::error("Failed to find create_constant_buffer (no string)");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string);

        if (!string_ref) {
            spdlog::error("Failed to find create_constant_buffer (no string ref)");
            return nullptr;
        }

        uintptr_t ip = *string_ref;

        for (auto i = 0; i < 20; ++i) {
            const auto resolved = utility::resolve_instruction(ip);

            if (!resolved) {
                spdlog::error("Failed to find create_constant_buffer (could not resolve instruction)");
                return nullptr;
            }

            ip = resolved->addr;

            if (*(uint8_t*)ip == 0xE8) {
                const auto result = (ConstantBuffer* (*)(void*, void*))utility::calculate_absolute(ip + 1);

                spdlog::info("Found create_constant_buffer: {:x}", (uintptr_t)result);
                return result;
            }

            ip -= 1;
        }

        return nullptr;
    }();

    return fn(nullptr, desc);
}

/*
+ 0x8B CircularDOF_WorkComponent0Im
+ 0x83 CircularDOF_WorkTexture
- 0x82 omposite
- 0x79 systems/effect/Stochastic_Sample8_MSK4.tex
+ 0x75 HDRImage
+ 0x73 HDRImage
- 0x6A HDRImage
- 0x67 systems/effect/Stochastic_Sample4_MSK4.tex
- 0x67 DensityMapTexture
- 0x5C systems/shader/advancedSystem.sdf
- 0x5A BaseColorTextrure
- 0x52 tSrc
- 0x4E HDRImage
- 0x4C CircularDOF_NearCOCFilteredHQ
- 0x3F CircularDOF_SceneMipTexture
*/
TargetState* create_target_state(TargetState::Desc* desc) {
    static auto fn = []() -> TargetState* (*)(void*, TargetState::Desc*) {
        spdlog::info("Searching for create_target_state");

        const auto game = utility::get_executable();
        const auto string = utility::scan_string(game, "CircularDOF_SceneMipTexture");

        if (!string) {
            spdlog::error("Failed to find create_target_state (no string)");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string);

            spdlog::error("Failed to find create_target_state (no string ref)");
        if (!string_ref) {
            return nullptr;
        }

        uintptr_t ip = *string_ref;
        uint32_t found_count = 0;

        for (auto i = 0; i < 50; ++i) {
            const auto resolved = utility::resolve_instruction(ip);

            if (!resolved) {
                spdlog::error("Failed to find create_target_state (could not resolve instruction)");
                return nullptr;
            }

            ip = resolved->addr;

            if (*(uint8_t*)ip == 0xE8) {
                ++found_count;
            }

            // third call back from this string reference is the one we want
            if (*(uint8_t*)ip == 0xE8 && found_count == 3) {
                const auto result = (TargetState* (*)(void*, TargetState::Desc*))utility::calculate_absolute(ip + 1);

                spdlog::info("Found create_target_state: {:x}", (uintptr_t)result);
                return result;
            }

            ip -= 1;
        }

        return nullptr;
    }();

    return fn(nullptr, desc);
}

/*
+ 0x217 Wrinkle_VertAreaSkin
- 0x20A EchoParam
+ 0x203 CapturePlane
+ 0x1F9 systems/shader/systemDevelop.sdf
+ 0x1F3 Wrinkle_ProbagateDupVertex
+ 0x1CF Wrinkle_ProbagateDupVertex_MaxMode
- 0x1CA PrevLDRImage
+ 0x1AB Wrinkle_DrawAreaToTexture2
- 0x1A5 MeshToUVTextureMap_2ndUVto1stUV
- 0x18A LDRImage
+ 0x187 Wrinkle_DrawAreaToTexture2_MaxMode
+ 0x163 Wrinkle_CheapBlur
- 0x145 MeshToUVTextureSkin2nd_Pos
+ 0xD9 systems/shader/speedTree/speedTree.sdf
- 0x18 width=%u,height=%u,depth=%u,mip=%u,array=%u,format=%u,usage=%u,bind=%u
*/
Texture* create_texture(void* desc) {
    static auto fn = []() -> Texture* (*)(void*, void*) {
        spdlog::info("Searching for create_texture");

        const auto game = utility::get_executable();
        const auto string = utility::scan_string(game, L"width=%u,height=%u,depth=%u,mip=%u,array=%u,format=%u,usage=%u,bind=%u");

        if (!string) {
            spdlog::error("Failed to find create_texture (no string)");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string);

        if (!string_ref) {
            spdlog::error("Failed to find create_texture (no string ref)");
            return nullptr;
        }

        uintptr_t ip = *string_ref;

        for (auto i = 0; i < 20; ++i) {
            const auto resolved = utility::resolve_instruction(ip);

            if (!resolved) {
                spdlog::error("Failed to find create_texture (could not resolve instruction)");
                return nullptr;
            }

            ip = resolved->addr;

            if (*(uint8_t*)ip == 0xE8) {
                const auto result = (Texture* (*)(void*, void*))utility::calculate_absolute(ip + 1);

                spdlog::info("Found create_texture: {:x}", (uintptr_t)result);
                return result;
            }

            ip -= 1;
        }

        return nullptr;
    }();

    return fn(nullptr, desc);
}

/*
+ 0x20A Wrinkle_DrawAreaToTexture2
+ 0x1E6 Wrinkle_DrawAreaToTexture2_MaxMode
+ 0x1C2 Wrinkle_CheapBlur
- 0x1B8 EchoParam
+ 0x185 width=%u,height=%u,depth=%u,mip=%u,array=%u,format=%u,usage=%u,bind=%u
- 0x178 PrevLDRImage
- 0x168 systems/shader/advancedSystem.sdf
- 0x138 LDRImage
- 0xE0 BaseColorTextrure
- 0xD4 DensityMapTexture
- 0x9F BaseColorTextrure
*/
/*
48 C7 44 24 24 05 00 00 00                    mov     [rsp+78h+var_54], 5
C7 44 24 30 01 00 00 00                       mov     [rsp+78h+var_48], 1
44 89 7C 24 2C                                mov     [rsp+78h+var_4C], r15d
C7 44 24 20 1C 00 00 00                       mov     [rsp+78h+var_58], 1Ch
E8 89 EB 78 00                                call    create_render_target_view
*/

// In RE4+:
/*
- 0x269 CircularDOF_NearCOCFilteredHQ
- 0x266 CircularDOF_NearCOCMaskForTile
+ 0x261 CircularDOF_WorkComponent0Re
- 0x235 tSrc
+ 0x212 Wrinkle_CheapBlur
+ 0x212 Echo
- 0x1F1 CircularDOF_NearCOCMaskForTileHQ
+ 0x1F0 CircularDOF_WorkComponent0Im
- 0x1D0 CircularDOF_NearCOCFiltered
- 0x19A EchoParam
+ 0x15B width=%u,height=%u,depth=%u,mip=%u,array=%u,format=%u,usage=%u,bind=%u
- 0x15A PrevLDRImage
- 0x149 CircularDOF_NearCOCFilteredHQ
- 0x11A LDRImage
+ 0xC3 width=%u,height=%u,depth=%u,mip=%u,array=%u,format=%u,usage=%u,bind=%u
*/
/*
4C 8D 45 B8                                   lea     r8, [rbp+40h+var_88]
49 8B CE                                      mov     rcx, r14
E8 ? ? ? ?                                    call    create_render_target_view
48 8B 8F F0 04 00 00                          mov     rcx, [rdi+4F0h]
48 8B D8                                      mov     rbx, rax
4C 89 BF F0 04 00 00                          mov     [rdi+4F0h], r15
*/
RenderTargetView* create_render_target_view(sdk::renderer::RenderResource* resource, void* desc) {
    static auto fn = []() -> RenderTargetView* (*)(void*, sdk::renderer::RenderResource* resource, void*) {
        spdlog::info("Searching for create_render_target_view");

        const auto game = utility::get_executable();
        const auto ref = utility::scan(game, "44 89 7C 24 2C C7 44 24 20 1C 00 00 00 E8 ? ? ? ?");

        if (!ref) {
            spdlog::info("Could not find first ref, performing fallback scan");
            const auto ref2 = utility::scan(game, "4C 8D 45 B8 49 8B CE E8 ? ? ? ?");

            if (ref2) {
                const auto result = (RenderTargetView* (*)(void*, sdk::renderer::RenderResource*, void*))utility::calculate_absolute(*ref2 + 8);
                spdlog::info("Found create_render_target_view: {:x}", (uintptr_t)result);

                return result;
            }

            spdlog::error("Failed to find create_render_target_view (no ref)");
            return nullptr;
        }

        const auto result = (RenderTargetView* (*)(void*, sdk::renderer::RenderResource*, void*))utility::calculate_absolute(*ref + 14);
        spdlog::info("Found create_render_target_view: {:x}", (uintptr_t)result);

        return result;
    }();

    return fn(nullptr, resource, desc);
}

ID3D12Resource* TargetState::get_native_resource_d3d12() const {
    const auto rtv = get_rtv(0);

    if (rtv == nullptr) {
        return nullptr;
    }

    // sizeof(via.render.RenderTargetView) + 8;
    const auto tex = rtv->get_texture_d3d12();

    if (tex == nullptr) {
        /*auto target_state = rtv->get_target_state_d3d12();

        if (target_state != nullptr && target_state != this) {
            return target_state->get_native_resource_d3d12();
        }*/

        return nullptr;
    }

    const auto internal_resource = tex->get_d3d12_resource_container();

    if (internal_resource == nullptr) {
        return nullptr;
    }

    return internal_resource->get_native_resource();
}

Texture* Texture::clone() {
    return sdk::renderer::create_texture(get_desc());
}

RenderTargetView* RenderTargetView::clone() {
    auto tex = this->get_texture_d3d12();

    if (tex == nullptr) {
        return nullptr;
    }

    return sdk::renderer::create_render_target_view(tex->clone(), &get_desc());
}

TargetState* TargetState::clone() {
    auto cloned_desc = get_desc();

    if (cloned_desc.num_rtv > 0) {
        cloned_desc.rtvs = (sdk::renderer::RenderTargetView**)sdk::memory::allocate(cloned_desc.num_rtv * sizeof(void*));

        for (auto i = 0; i < cloned_desc.num_rtv; ++i) {
            auto rtv = get_rtv(i);

            if (rtv == nullptr) {
                continue;
            }

            cloned_desc.rtvs[i] = rtv->clone();
        }
    } else {
        cloned_desc.rtvs = nullptr;
    }

    return sdk::renderer::create_target_state(&cloned_desc);
}

void*& layer::Output::get_present_state() {
    static uint32_t output_target_offset = 0;

    if (output_target_offset == 0) {
        spdlog::info("[Renderer] Finding output_target_offset");

        auto get_scene_view_fn = sdk::find_native_method("via.render.layer.Output", "get_SceneView");

        if (get_scene_view_fn == nullptr) {
            spdlog::error("[Renderer] Failed to find get_SceneView");
            return *(void**)((uintptr_t)this + sdk::find_type_definition("via.render.RenderLayer")->get_size());
        }

        // Resolve the jmp to the real function
        if (((uint8_t*)get_scene_view_fn)[0] == 0xE9) {
            get_scene_view_fn = (decltype(get_scene_view_fn))utility::calculate_absolute((uintptr_t)get_scene_view_fn + 1);
        }

        // Find the offset to the output target
        // First instruction is a mov, so we don't need to pattern scan for it
        output_target_offset = *(uint8_t*)((uintptr_t)get_scene_view_fn + 3);

        spdlog::info("[Renderer] output_target_offset: {:x}", output_target_offset);
    }

    return *(void**)((uintptr_t)this + output_target_offset);
}

REManagedObject*& layer::Output::get_scene_view() {
    static uint32_t scene_view_offset = 0;

    if (scene_view_offset == 0) {
        spdlog::info("[Renderer] Finding scene_view_offset");

        // because if this is a manually created output layer,
        // we might not have the scene view and output state set up yet
        auto top_output_layer = sdk::renderer::get_output_layer();

        if (top_output_layer == nullptr) {
            spdlog::error("[Renderer] Failed to find top_output_layer");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        // Call get_SceneView so we can get a scene view
        // to scan the object for
        const auto scene_view = sdk::call_object_func<void*>(top_output_layer, "get_SceneView", sdk::get_thread_context(), top_output_layer);
        const auto output_target = top_output_layer->get_present_state();

        if (scene_view == nullptr) {
            spdlog::error("[Renderer] Failed to find scene_view");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        if (output_target == nullptr) {
            spdlog::error("[Renderer] Failed to find output_target");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        // Find the offset to the scene view
        for (auto i = 0; i < 0x1000; i += sizeof(void*)) {
            if (*(void**)((uintptr_t)output_target + i) == scene_view) {
                scene_view_offset = i;
                break;
            }
        }

        spdlog::info("[Renderer] scene_view_offset: {:x}", scene_view_offset);
    }

    return *(REManagedObject**)((uintptr_t)get_present_state() + scene_view_offset);
}

uint32_t layer::Scene::get_view_id() const {
    static auto get_view_id_method = sdk::find_method_definition("via.render.layer.Scene", "get_ViewID");

    if (get_view_id_method == nullptr) {
        return 0;
    }

    return get_view_id_method->call<uint32_t>(sdk::get_thread_context(), this);
}

RECamera* layer::Scene::get_camera() const {
    static auto get_camera_method = sdk::find_method_definition("via.render.layer.Scene", "get_Camera");

    if (get_camera_method == nullptr) {
        return nullptr;
    }

    return get_camera_method->call<RECamera*>(sdk::get_thread_context(), this);
}

RECamera* layer::Scene::get_main_camera_if_possible() const {
    const auto camera = get_camera();

    if (camera == nullptr) {
        return nullptr;
    }

    const auto camera_gameobject = utility::re_component::get_game_object(camera);

    if (camera_gameobject == nullptr) {
        return nullptr;
    }

    if (utility::re_string::get_view(camera_gameobject->name) == L"MainCamera" ||
        utility::re_string::get_view(camera_gameobject->name) == L"Main Camera") {
        return camera;
    }

    return nullptr;
}

REManagedObject* layer::Scene::get_mirror() const {
    static auto get_mirror_method = sdk::find_method_definition("via.render.layer.Scene", "get_Mirror");

    if (get_mirror_method == nullptr) {
        return nullptr;
    }

    return get_mirror_method->call<REManagedObject*>(sdk::get_thread_context(), this);
}

bool layer::Scene::is_enabled() const {
    static auto is_enabled_method = sdk::find_method_definition("via.render.layer.Scene", "get_Enable");

    if (is_enabled_method == nullptr) {
        return false;
    }

    return is_enabled_method->call<bool>(sdk::get_thread_context(), this);
}

sdk::renderer::SceneInfo* layer::Scene::get_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "SceneInfo");
}

sdk::renderer::SceneInfo* layer::Scene::get_depth_distortion_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "DepthDistortionSceneInfo");
}

sdk::renderer::SceneInfo* layer::Scene::get_filter_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "FilterSceneInfo");
}

sdk::renderer::SceneInfo* layer::Scene::get_jitter_disable_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "JitterDisableSceneInfo");
}

sdk::renderer::SceneInfo* layer::Scene::get_jitter_disable_post_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "JitterDisablePostSceneInfo");
}

sdk::renderer::SceneInfo* layer::Scene::get_z_prepass_scene_info() {
    return utility::re_managed_object::get_field<SceneInfo*>(this, "ZPrepassSceneInfo");
}

Texture* layer::Scene::get_depth_stencil() {
    return utility::re_managed_object::get_field<::sdk::renderer::Texture*>(this, "DepthStencilTex");;
}

TargetState* layer::Scene::get_motion_vectors_state() {
    return utility::re_managed_object::get_field<::sdk::renderer::TargetState*>(this, "VelocityTarget");
}

ID3D12Resource* layer::Scene::get_depth_stencil_d3d12() {
    const auto tex = utility::re_managed_object::get_field<::sdk::renderer::Texture*>(this, "DepthStencilTex");

    if (tex == nullptr) {
        return nullptr;
    }

    const auto internal_resource = tex->get_d3d12_resource_container();

    if (internal_resource == nullptr) {
        return nullptr;
    }

    return internal_resource->get_native_resource();
}
}
}