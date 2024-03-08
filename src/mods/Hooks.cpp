#include "Mods.hpp"
#include "REFramework.hpp"
#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <utility/String.hpp>
#include <utility/Memory.hpp>

#include "sdk/Application.hpp"

#include "Hooks.hpp"

Hooks* g_hook = nullptr;

std::shared_ptr<Hooks>& Hooks::get() {
    static std::shared_ptr<Hooks> instance = std::make_shared<Hooks>();
    return instance;
}

Hooks::Hooks() {
    g_hook = this;
}

std::optional<std::string> Hooks::on_initialize() {
    auto game = g_framework->get_module().as<HMODULE>();

    const auto mod_size = utility::get_module_size(game);

    if (!mod_size) {
        return "Unable to get module size";
    }

    for (auto hook : m_hook_list) {
        spdlog::info("[Hooks] Entering hook...");

        auto result = hook();

        // Error occurred when hooking
        if (result) {
            return result;
        }
    }

    spdlog::info("[Hooks] Finished hooking");

    return Mod::on_initialize();
}

void Hooks::on_draw_ui() {
    if (!ImGui::CollapsingHeader("Performance")) {
        return;
    }

    ImGui::Checkbox("Enable Profiling", &m_profiling_enabled);

    if (!m_profiling_enabled) {
        return;
    }

    ImGui::Text("Application Entry Times");

    std::vector<const char*> sorted_times{};
    std::scoped_lock _{m_profiler_mutex};

    std::chrono::high_resolution_clock::duration total_reframework_time{};
    std::chrono::high_resolution_clock::duration total_game_time{};

    for (auto& entry : m_application_entry_times) {
        sorted_times.emplace_back(entry.first);
        total_reframework_time += entry.second.reframework_pre_time + entry.second.reframework_post_time;
        total_game_time += entry.second.callback_time;
    }

    std::sort(sorted_times.begin(), sorted_times.end(), [&](const char* a, const char* b) {
        const auto& a_entry = m_application_entry_times[a];
        const auto& b_entry = m_application_entry_times[b];

        return a_entry.callback_time + a_entry.reframework_pre_time + a_entry.reframework_post_time 
                > b_entry.callback_time + b_entry.reframework_pre_time + b_entry.reframework_post_time;
    });

    ImGui::Text("Total REFramework Time: %.3fms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(total_reframework_time).count());
    ImGui::Text("Total Game Time: %.3fms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(total_game_time).count());

    for (auto name : sorted_times) {
        auto& entry = m_application_entry_times[name];
        
        ImGui::SetNextItemOpen(true);

        if (ImGui::TreeNode(name)) {
            ImGui::Text("Game Time: %s: %.2fms", name, entry.callback_time.count() / 1000000.0f);
            ImGui::Text("REFramework Pre Time: %.2fms", entry.reframework_pre_time.count() / 1000000.0f);
            ImGui::Text("REFramework Post Time: %.2fms", entry.reframework_post_time.count() / 1000000.0f);
            ImGui::Text("Total Time: %.2fms", (entry.callback_time + entry.reframework_pre_time + entry.reframework_post_time).count() / 1000000.0f);
            
            ImGui::TreePop();
        }
    }
}

#define LAYER_HOOK_BODY(x, x2, x3) \
if (!g_framework->is_ready()) {\
    auto original_func = g_hook->m_layer_hooks.##x##.##x3##_hook->get_original<decltype(RenderLayerHook<sdk::renderer::layer::##x2##>::##x3##)>();\
    original_func(layer, render_ctx); \
    return; \
} \
bool any_false = false; \
const auto& mods = g_framework->get_mods()->get_mods(); \
for (auto& mod : mods) { \
    const auto result = mod->on_pre_##x##_layer_##x3##(layer, render_ctx); \
    if (!result) { \
        any_false = true; \
    } \
} \
if (!any_false) { \
    auto original_func = g_hook->m_layer_hooks.##x##.##x3##_hook->get_original<decltype(RenderLayerHook<sdk::renderer::layer::##x2##>::##x3##)>();\
    original_func(layer, render_ctx); \
} \
for (auto& mod : mods) { \
    mod->on_##x##_layer_##x3##(layer, render_ctx); \
}

void Hooks::RenderLayerHook<sdk::renderer::layer::Scene>::update(sdk::renderer::layer::Scene* layer, void* render_ctx) {
    LAYER_HOOK_BODY(scene, Scene, update);
}

void Hooks::RenderLayerHook<sdk::renderer::layer::Scene>::draw(sdk::renderer::layer::Scene* layer, void* render_ctx) {
    LAYER_HOOK_BODY(scene, Scene, draw);
}

void Hooks::RenderLayerHook<sdk::renderer::layer::PostEffect>::update(sdk::renderer::layer::PostEffect* layer, void* render_ctx) {
    LAYER_HOOK_BODY(post_effect, PostEffect, update);
}

void Hooks::RenderLayerHook<sdk::renderer::layer::PostEffect>::draw(sdk::renderer::layer::PostEffect* layer, void* render_ctx) {
    LAYER_HOOK_BODY(post_effect, PostEffect, draw);
}

void Hooks::RenderLayerHook<sdk::renderer::layer::Overlay>::update(sdk::renderer::layer::Overlay* layer, void* render_ctx) {
    LAYER_HOOK_BODY(overlay, Overlay, update);
}

void Hooks::RenderLayerHook<sdk::renderer::layer::Overlay>::draw(sdk::renderer::layer::Overlay* layer, void* render_ctx) {
    LAYER_HOOK_BODY(overlay, Overlay, draw);
}

std::optional<std::string> Hooks::hook_update_transform() {
    auto game = g_framework->get_module().as<HMODULE>();

    // The 48 8B 4D 40 bit might change.
    // Version 1.0 jmp stub: game+0x1dc7de0
    // Version 1
    //auto updateTransformCall = utility::scan(game, "E8 ? ? ? ? 48 8B 5B ? 48 85 DB 75 ? 48 8B 4D 40 48 31 E1");

    // Version 2 Dec 17th, 2019 (works on old version too) game.exe+0x1DD3FF0
    // If this ever changes, get the singleton for via.SceneManager, find its
    // constructor function, and look for the job function added near the end of the constructor
    // UpdateTransform gets called near the end of the job, looks like this:
    /*
      if ( *(_BYTE *)(v2 + 0x114) )
        UpdateTransform(v14, 0, v10);
      else
        sub_141DD4140(v14, 0i64, v10);
    */

    struct TransformPattern {
        std::string pat;
        uint32_t offset;
    };

    std::vector<TransformPattern> pats {
        { "E8 ? ? ? ? 48 8B 5B ? 48 85 DB 75 ? 48 8B 4D 40 48 ? ?", 1 }, // RE2 - MHRise v1.0
        { "33 D2 E8 ? ? ? ? B8 01 00 00 00 F0 0F", 3 }, // RE7/RE2/RE3 update to TDB v70/newer games?
        { "0F B6 D1 48 8B CB E8 ? ? ? ? 48 8B 9B ? ? ? ?", 7 }, // RE7
        { "0F B6 D0 48 8B CB E8 ? ? ? ? 48 8B 9B ? ? ? ?", 7 } // RE7 Demo
    };

    uintptr_t update_transform = 0;

    for (auto& pat : pats) {
        auto result = utility::scan(game, pat.pat.c_str());

        if (result) {
            update_transform = utility::calculate_absolute(*result + pat.offset);
            break;
        }
    }

    if (update_transform == 0) {
        spdlog::error("Unable to find UpdateTransform pattern.");
        return std::nullopt; // Allow it to continue anyways, it's not strictly necessary except for freecam
    }

    spdlog::info("UpdateTransform: {:x}", update_transform);

    // Can be found by breakpointing RETransform's worldTransform
    m_update_transform_hook = std::make_unique<FunctionHook>(update_transform, &update_transform_hook);

    if (!m_update_transform_hook->create()) {
        //return "Failed to hook UpdateTransform";
        spdlog::error("Failed to hook UpdateTransform");
        return std::nullopt; // who cares
    }

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_update_camera_controller() {
#if defined(RE2) || defined(RE3)
    // Version 1.0 jmp stub: game+0xB4685A0
    // Version 1
    /*auto updatecamera_controllerCall = utility::scan(game, "75 ? 48 89 FA 48 89 D9 E8 ? ? ? ? 48 8B 43 50 48 83 78 18 00 75 ? 45 89");

    if (!updatecamera_controllerCall) {
        return "Unable to find Updatecamera_controller pattern.";
    }

    auto updatecamera_controller = utility::calculate_absolute(*updatecamera_controllerCall + 9);*/

    // Version 2 Dec 17th, 2019 game.exe+0x7CF690 (works on old version too)
    //auto update_camera_controller = utility::scan(game, "40 55 56 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? 00 00 48 8B 41 50");

    // Version 3 June 2nd, 2020 game.exe+0xD41AD0 (works on old version too)
    auto update_camera_controller = sdk::find_native_method(game_namespace("camera.PlayerCameraController"), "updateCameraPosition");

    if (update_camera_controller == nullptr) {
        return std::string{"Failed to find "} + game_namespace("camera.PlayerCameraController") + "::updateCameraPosition";
    }

    spdlog::info("camera.PlayerCameraController.updateCameraPosition: {:x}", (uintptr_t)update_camera_controller);

    // Can be found by breakpointing camera controller's worldPosition
    m_update_camera_controller_hook = std::make_unique<FunctionHook>(update_camera_controller, &update_camera_controller_hook);

    if (!m_update_camera_controller_hook->create()) {
        return "Failed to hook UpdateCameraController";
    }
#endif

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_update_camera_controller2() {
#if defined(RE2) || defined(RE3)
    // Version 1.0 jmp stub: game+0xCF2510
    // Version 1.0 function: game+0xB436230
    
    // Version 1
    //auto updatecamera_controller2 = utility::scan(game, "40 53 57 48 81 ec ? ? ? ? 48 8b 41 ? 48 89 d7 48 8b 92 ? ? 00 00");
    // Version 2 Dec 17th, 2019 game.exe+0x6CD9C0 (works on old version too)
    auto update_camera_controller2 = sdk::find_native_method(game_namespace("camera.TwirlerCameraControllerRoot"), "update");

    if (update_camera_controller2 == nullptr) {
        return std::string{"Failed to find "} + game_namespace("camera.TwirlerCameraControllerRoot") + "::update";
    }

    spdlog::info("camera.TwirlerCameraControllerRoot.update: {:x}", (uintptr_t)update_camera_controller2);

    // Can be found by breakpointing camera controller's worldRotation
    m_update_camera_controller2_hook = std::make_unique<FunctionHook>(update_camera_controller2, &update_camera_controller2_hook);

    if (!m_update_camera_controller2_hook->create()) {
        return "Failed to hook Updatecamera_controller2";
    }
#endif

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_gui_draw() {
    spdlog::info("[Hooks] Attempting to hook GUI functions...");

    auto game = g_framework->get_module().as<HMODULE>();
    auto application = sdk::Application::get();

    // This pattern appears to work all the way from RE2 to RE8.
    // If this ever breaks, its parent function is found within via.gui.GUIManager.
    // It is used as a draw callback. The assignment can be found within the constructor near the end.
    // "onEnd(via.gui.TextAnimationEndArg)" can be used as a reference to find the constructor.
    // "copyProperties(via.gui.PlayObject)" also works in RE7 and onwards
    // In RE2:
    /*  
    *(_QWORD *)(v23 + 8 * v22) = &vtable_thing;
    *(_QWORD *)(v23 + 8 * v22 + 8) = gui_manager;
    *(_OWORD *)(v23 + 8 * v22 + 16) = v34;
    *(_QWORD *)(v23 + 8 * v22 + 32) = gui_manager;
    ++*(_DWORD *)(gui_manager + 232);
    *(_QWORD *)&v35 = draw_task_function; <-- "gui_draw_call" is found within this function.
    */
    spdlog::info("[Hooks] Scanning for first GUI draw call...");
    auto gui_draw_call = utility::scan(game, "49 8B 0C CE 48 83 79 10 00 74 ? E8 ? ? ? ?");

    if (!gui_draw_call) {
        spdlog::info("[Hooks] Scanning for fallback GUI draw call...");
        // RE7 (+0x20 grabs the owner ptr, 0x10 in others)
        gui_draw_call = utility::scan(game, "49 8B 0C CE 48 83 79 20 00 74 ? E8 ? ? ? ?");

        if (!gui_draw_call) {
            return "Unable to find gui_draw_call pattern.";
        }
    }

    spdlog::info("[Hooks] Found gui_draw_call at {:x}", *gui_draw_call);

    auto gui_draw = utility::calculate_absolute(*gui_draw_call + 12);
    spdlog::info("[Hooks] gui_draw: {:x}", gui_draw);

    m_gui_draw_hook = std::make_unique<FunctionHook>(gui_draw, &gui_draw_hook);

    if (!m_gui_draw_hook->create()) {
        return "Failed to hook GUI::draw";
    }

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_application_entry(std::string name, std::unique_ptr<FunctionHook>& hook, void (*hook_fn)(void*)) {
    auto application = sdk::Application::get();

    if (application == nullptr) {
        return "Failed to get via.Application";
    }

    auto entry = application->get_function(name);

    if (entry == nullptr) {
        return "Unable to find via::Application::" + name;
    }

    auto func = entry->func;

    if (func == nullptr) {
        return "via::Application::" + name + " is null";
    }

    spdlog::info("{} entry: {:x}", name, (uintptr_t)entry);
    spdlog::info("{}: {:x}", name, (uintptr_t)func - g_framework->get_module());

    hook = std::make_unique<FunctionHook>(func, hook_fn);

    if (!hook->create()) {
        return "Failed to hook via::Application::" + name;
    }
    
    spdlog::info("Hooked via::Application::{}", name);

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_all_application_entries() {
    spdlog::info("[Hooks] Attempting to application entries...");

    auto application = sdk::Application::get();

    if (application == nullptr) {
        return "Failed to get via.Application";
    }

    spdlog::info("[Hooks] Found via.Application at {:x}", (uintptr_t)application);

    auto generate_mov_rdx = [](uintptr_t target) {
        std::vector<uint8_t> mov_rdx{ 0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        *(uintptr_t*)&mov_rdx[2] = target;

        return mov_rdx;
    };

    auto generate_mov_r8 = [](uintptr_t target) {
        std::vector<uint8_t> mov_r8{ 0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        *(uintptr_t*)&mov_r8[2] = target;

        return mov_r8;
    };

    auto generate_mov_r9 = [](uintptr_t target) {
        std::vector<uint8_t> mov_r9{ 0x49, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        *(uintptr_t*)&mov_r9[2] = target;

        return mov_r9;
    };

    auto generate_jmp_r9 = []() {
        std::vector<uint8_t> jmp_r8{ 0x41, 0xFF, 0xE1 };
        return jmp_r8;
    };

    // movabs rdx, entry_name_addr
    // movabs r8, entry_name_hash
    // movabs r9, hook_addr
    // jmp r9 (Hooks::global_application_entry_hook)
    // The purpose of this is so we can pass some state to the hook callback
    // So we can know which hook is being called, as a global hook handler
    // gets called for every hook (Hooks::global_application_entry_hook)
    auto generate_hook_func = [&](const char* name, uintptr_t target) {
        auto mov_rdx = generate_mov_rdx((uintptr_t)name);
        auto mov_r8 = generate_mov_r8(utility::hash(name));
        auto mov_r9 = generate_mov_r9(target);
        auto jmp_r9 = generate_jmp_r9();

        // Concats the above vectors into a single vector.
        std::vector<uint8_t> hook{};
        hook.insert(hook.end(), mov_rdx.begin(), mov_rdx.end());
        hook.insert(hook.end(), mov_r8.begin(), mov_r8.end());
        hook.insert(hook.end(), mov_r9.begin(), mov_r9.end());
        hook.insert(hook.end(), jmp_r9.begin(), jmp_r9.end());

        // Allocate some permanent memory for the hook
        // and copy the hook into it. Set the permissions to RWX.
        auto hook_addr = VirtualAlloc(nullptr, hook.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        memcpy(hook_addr, hook.data(), hook.size());

        return hook_addr;
    };

    // Seemingly only necessary when using MinHook rather than pointer hooking.
    /*std::unordered_set<void*> bad_funcs{};
    std::unordered_map<void*, sdk::Application::Function*> funcs_to_entries{};

    for (auto i = 0; i < 1024; ++i) {
        auto entry = application->get_function(i);

        if (entry == nullptr || entry->description == nullptr) {
            continue;
        }

        auto func = entry->func;

        if (func == nullptr) {
            continue;
        }

        if (funcs_to_entries.find(func) == funcs_to_entries.end()) {
            funcs_to_entries[func] = entry;
        } else {
            bad_funcs.insert(func);
        }
    }*/

    for (auto i = 0; i < 1024; ++i) {
        auto entry = application->get_function(i);

        if (entry == nullptr || entry->description == nullptr) {
            continue;
        }

        auto func = entry->func;

        if (func == nullptr) {
            continue;
        }

        /*if (bad_funcs.find(func) != bad_funcs.end()) {
            spdlog::warn("Duplicate function: {}", entry->description);
            continue;
        }*/

        spdlog::info("{} {} entry: {:x}", i, entry->description, (uintptr_t)entry);

        auto generated_hook = generate_hook_func((const char*)entry->description, (uintptr_t)&global_application_entry_hook);

        //m_application_entry_hooks[entry->description] = std::make_unique<FunctionHook>(func, generated_hook);
        
        // We are just going to replace the pointer to the function for now
        // Doing a full hook with FunctionHook eats up a lot of initialization time because of
        // the constant thread suspension. 
        m_application_entry_hooks[entry->description] = func;
        entry->func = (void (*)(void*))generated_hook;

        spdlog::info("Hooked {} {:x}->{:x}", entry->description, (uintptr_t)func, (uintptr_t)generated_hook);
    }

    /*for (auto& entry : m_application_entry_hooks) {
        if (!entry.second->create()) {
            return "Failed to hook via::Application::" + std::string{entry.first};
        }
    }*/

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_update_before_lock_scene() {
    // Hook updateBeforeLockScene
    auto update_before_lock_scene = sdk::find_native_method("via.render.EntityRenderer", "updateBeforeLockScene");

    if (update_before_lock_scene == nullptr) {
        return "Unable to find via::render::EntityRenderer::updateBeforeLockScene";
    }

    spdlog::info("updateBeforeLockScene: {:x}", (uintptr_t)update_before_lock_scene);

    m_update_before_lock_scene_hook = std::make_unique<FunctionHook>(update_before_lock_scene, &update_before_lock_scene_hook);

    if (!m_update_before_lock_scene_hook->create()) {
        return "Failed to hook via::render::EntityRenderer::updateBeforeLockScene";
    }

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_lightshaft_draw() {
#if 0
    // Create a fake via.render.LightShaft instance
    // so we can get the draw method and hook it.
    auto lightshaft_t = sdk::find_type_definition("via.render.LightShaft");

    if (lightshaft_t == nullptr) {
        return "Unable to find via::render::LightShaft";
    }

    auto lightshaft = lightshaft_t->create_instance();

    if (lightshaft == nullptr) {
        return "Unable to create via::render::LightShaft instance";
    }

    auto lightshaft_vtable = *(void***)lightshaft;

    if (lightshaft_vtable == nullptr) {
        return "Unable to get via::render::LightShaft vtable";
    }

#if defined(RE8) || defined(MHRISE)
    auto draw = lightshaft_vtable[13];
#else
    auto draw = lightshaft_vtable[10];
#endif

    if (draw == nullptr) {
        return "Unable to get via::render::LightShaft::draw";
    }

    spdlog::info("LightShaft::draw: {:x}", (uintptr_t)draw);

    m_lightshaft_draw_hook = std::make_unique<FunctionHook>((uintptr_t)draw, (uintptr_t)&lightshaft_draw_hook);

    if (!m_lightshaft_draw_hook->create()) {
        return "Failed to hook via::render::LightShaft::draw";
    }
#endif

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_view_get_size() {
    // We're going to hook via.SceneView.get_Size so we can
    // spoof the render target size to the HMD's resolution.
    auto get_size_func = sdk::find_native_method("via.SceneView", "get_Size");

    if (get_size_func == nullptr) {
        return "Hook init failed: via.SceneView.get_Size function not found.";
    }

    spdlog::info("via.SceneView.get_Size: {:x}", (uintptr_t)get_size_func);

    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)get_size_func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "Hook init failed: via.SceneView.get_Size native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    m_view_get_size_hook = std::make_unique<FunctionHook>(native_func, view_get_size_hook);

    if (!m_view_get_size_hook->create()) {
        return "Hook init failed: via.SceneView.get_Size native function hook failed.";
    }

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_camera_get_projection_matrix() {
    // We're going to hook via.Camera.get_ProjectionMatrix so we can
    // override the camera's Projection matrix with the HMD's Projection matrix (per-eye)
    auto func = sdk::find_native_method("via.Camera", "get_ProjectionMatrix");

    if (func == nullptr) {
        return "Hook init failed: via.Camera.get_ProjectionMatrix function not found.";
    }

    spdlog::info("via.Camera.get_ProjectionMatrix: {:x}", (uintptr_t)func);
    
    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "Hook init failed: via.Camera.get_ProjectionMatrix native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    m_camera_get_projection_matrix_hook = std::make_unique<FunctionHook>(native_func, camera_get_projection_matrix_hook);

    if (!m_camera_get_projection_matrix_hook->create()) {
        return "Hook init failed: via.Camera.get_ProjectionMatrix native function hook failed.";
    }

    spdlog::info("Hooked via.Camera.get_ProjectionMatrix");

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_camera_get_view_matrix() {
    auto func = sdk::find_native_method("via.Camera", "get_ViewMatrix");

    if (func == nullptr) {
        return "Hook init failed: via.Camera.get_ViewMatrix function not found.";
    }

    spdlog::info("via.Camera.get_ViewMatrix: {:x}", (uintptr_t)func);

    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "Hook init failed: via.Camera.get_ViewMatrix native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    m_camera_get_view_matrix_hook = std::make_unique<FunctionHook>(native_func, camera_get_view_matrix_hook);

    if (!m_camera_get_view_matrix_hook->create()) {
        return "Hook init failed: via.Camera.get_ViewMatrix native function hook failed.";
    }

    return std::nullopt;
}

std::optional<std::string> Hooks::hook_render_layer(Hooks::RenderLayerHook<sdk::renderer::RenderLayer>& hook) {
    auto t = sdk::find_type_definition(hook.name);

    if (t == nullptr) {
        return std::string{"Hooks init failed: "} + hook.name + " type not found.";
    }

    void* fake_obj = t->create_instance();

    if (fake_obj == nullptr) { 
        return std::string{"Hooks init failed: "} + "Failed to create fake " + hook.name + " instance.";
    }

    auto obj_vtable = *(uintptr_t**)fake_obj;

    if (obj_vtable == nullptr) {
        return std::string{"Hooks init failed: "} + hook.name + " vtable not found.";
    }

    spdlog::info("{:s} vtable: {:x}", hook.name, (uintptr_t)obj_vtable - g_framework->get_module());

    auto draw_native = obj_vtable[sdk::renderer::RenderLayer::DRAW_VTABLE_INDEX];

    if (draw_native == 0) {
        return std::string{"Hooks init failed: "} + hook.name + " draw native not found.";
    }

    spdlog::info("{:s}.Draw: {:x}", hook.name, (uintptr_t)draw_native);

    // Set the first byte to the ret instruction
    //m_overlay_draw_patch = Patch::create(draw_native, { 0xC3 });

    if (!utility::is_stub_code((uint8_t*)draw_native)) {
        if (!hook.hook_draw(draw_native)) {
            return std::string{"Hooks init failed: "} + hook.name + " draw native function hook failed.";
        }
    } else {
        spdlog::info("Skipping draw hook for {:s}, stub code detected", hook.name);
    }

    auto update_native = obj_vtable[sdk::renderer::RenderLayer::UPDATE_VTABLE_INDEX];

    if (update_native == 0) {
        return std::string{"Hooks init failed: "} + hook.name + " update native not found.";
    }

    spdlog::info("{:s}.Update: {:x}", hook.name, (uintptr_t)update_native);

    if (!utility::is_stub_code((uint8_t*)update_native)) {
        if (!hook.hook_update(update_native)) {
            return std::string{"Hooks init failed: "} + hook.name + " update native function hook failed.";
        }
    } else {
        spdlog::info("Skipping update hook for {:s}, stub code detected", hook.name);
    }

    return std::nullopt;
}

void* Hooks::update_transform_hook_internal(RETransform* t, uint8_t a2, uint32_t a3) {
    if (!g_framework->is_ready()) {
        return m_update_transform_hook->get_original<decltype(update_transform_hook)>()(t, a2, a3);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_update_transform(t);
    }

    auto ret = m_update_transform_hook->get_original<decltype(update_transform_hook)>()(t, a2, a3);

    for (auto& mod : mods) {
        mod->on_update_transform(t);
    }

    return ret;
}

void* Hooks::update_transform_hook(RETransform* t, uint8_t a2, uint32_t a3) {
    return g_hook->update_transform_hook_internal(t, a2, a3);
}

void* Hooks::update_camera_controller_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller) {
    if (!g_framework->is_ready()) {
        return m_update_camera_controller_hook->get_original<decltype(update_camera_controller_hook)>()(a1, camera_controller);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_update_camera_controller(camera_controller);
    }

    auto ret = m_update_camera_controller_hook->get_original<decltype(update_camera_controller_hook)>()(a1, camera_controller);

    for (auto& mod : mods) {
        mod->on_update_camera_controller(camera_controller);
    }

    return ret;
}

void* Hooks::update_camera_controller_hook(void* a1, RopewayPlayerCameraController* camera_controller) {
    return g_hook->update_camera_controller_hook_internal(a1, camera_controller);
}

void* Hooks::update_camera_controller2_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller) {
    if (!g_framework->is_ready()) {
        return m_update_camera_controller2_hook->get_original<decltype(update_camera_controller2_hook)>()(a1, camera_controller);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_update_camera_controller2(camera_controller);
    }

    auto ret = m_update_camera_controller2_hook->get_original<decltype(update_camera_controller2_hook)>()(a1, camera_controller);

    for (auto& mod : mods) {
        mod->on_update_camera_controller2(camera_controller);
    }

    return ret;
}

void* Hooks::update_camera_controller2_hook(void* a1, RopewayPlayerCameraController* camera_controller) {
    return g_hook->update_camera_controller2_hook_internal(a1, camera_controller);
}

void* Hooks::gui_draw_hook_internal(REComponent* gui_element, void* primitive_context) {
    auto original_func = m_gui_draw_hook->get_original<decltype(gui_draw_hook)>();

    if (!g_framework->is_ready()) {
        return original_func(gui_element, primitive_context);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    bool any_false = false;

    for (auto& mod : mods) {
        if (!mod->on_pre_gui_draw_element(gui_element, primitive_context)) {
            any_false = true;
        }
    }

    void* ret = nullptr;

    if (!any_false) {
        ret = original_func(gui_element, primitive_context);
    }

    for (auto& mod : mods) {
        mod->on_gui_draw_element(gui_element, primitive_context);
    }

    return ret;
}

void* Hooks::gui_draw_hook(REComponent* gui_element, void* primitive_context) {
    return g_hook->gui_draw_hook_internal(gui_element, primitive_context);
}

void Hooks::update_before_lock_scene_hook_internal(void* ctx) {
    auto original = m_update_before_lock_scene_hook->get_original<decltype(update_before_lock_scene_hook)>();

    if (!g_framework->is_ready()) {
        return original(ctx);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_update_before_lock_scene(ctx);
    }

    original(ctx);

    for (auto& mod : mods) {
        mod->on_update_before_lock_scene(ctx);
    }
}

void Hooks::update_before_lock_scene_hook(void* ctx) {
    g_hook->update_before_lock_scene_hook_internal(ctx);
}

void Hooks::lightshaft_draw_hook_internal(void* shaft, void* render_context) {
    auto original = m_lightshaft_draw_hook->get_original<decltype(lightshaft_draw_hook)>();

    if (!g_framework->is_ready()) {
        return original(shaft, render_context);
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_lightshaft_draw(shaft, render_context);
    }

    original(shaft, render_context);

    for (auto& mod : mods) {
        mod->on_lightshaft_draw(shaft, render_context);
    }
}

void Hooks::lightshaft_draw_hook(void* shaft, void* render_context) {
    g_hook->lightshaft_draw_hook_internal(shaft, render_context);
}

void Hooks::global_application_entry_hook_internal(void* entry, const char* name, size_t hash) {
    //spdlog::info("{}", name);

    auto original = m_application_entry_hooks[name];

    if (!g_framework->is_ready()) {
        return original(entry);
    }

    const auto should_allow_ignore = sdk::VM::s_tdb_version >= 73 ?
                                     (hash != 0x76b8100bec7c12c3 && hash != 0x9f63c0fc4eea6626) :
                                     true;

    if (should_allow_ignore) {
        std::shared_lock _{m_application_entry_data_mutex};

        if (m_ignored_application_entries.contains(hash)) {
            return;
        }
    }

    if (m_profiling_enabled) {
        Hooks::ApplicationEntryData profiler_entry{};
        
        auto now = std::chrono::high_resolution_clock::now();
        auto& mods = g_framework->get_mods()->get_mods();

        if (hash == "BeginRendering"_fnv) {
            g_framework->run_imgui_frame(false);
        }

        for (auto& mod : mods) {
            mod->on_pre_application_entry(entry, name, hash);
        }

        profiler_entry.reframework_pre_time = std::chrono::high_resolution_clock::now() - now;

        now = std::chrono::high_resolution_clock::now();
        
        original(entry);

        profiler_entry.callback_time = std::chrono::high_resolution_clock::now() - now;

        now = std::chrono::high_resolution_clock::now();

        for (auto& mod : mods) {
            mod->on_application_entry(entry, name, hash);
        }

        profiler_entry.reframework_post_time = std::chrono::high_resolution_clock::now() - now;
        
        std::scoped_lock _{m_profiler_mutex};
        m_application_entry_times[name] = profiler_entry;
    } else {
        if (hash == "BeginRendering"_fnv) {
            g_framework->run_imgui_frame(false);
        }

        auto& mods = g_framework->get_mods()->get_mods();

        for (auto& mod : mods) {
            mod->on_pre_application_entry(entry, name, hash);
        }
        
        original(entry);

        for (auto& mod : mods) {
            mod->on_application_entry(entry, name, hash);
        }
    }
}

void Hooks::global_application_entry_hook(void* entry, const char* name, size_t hash) {
    g_hook->global_application_entry_hook_internal(entry, name, hash);
}

float* Hooks::view_get_size_hook_internal(REManagedObject* scene_view, float* result) {
    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_view_get_size(scene_view, result);
    }

    auto original = m_view_get_size_hook->get_original<decltype(view_get_size_hook)>();

    auto ret = original(scene_view, result);

    for (auto& mod : mods) {
        mod->on_view_get_size(scene_view, result);
    }

    return ret;
}

float* Hooks::view_get_size_hook(REManagedObject* scene_view, float* result) {
    return g_hook->view_get_size_hook_internal(scene_view, result);
}

Matrix4x4f* Hooks::camera_get_projection_matrix_hook_internal(REManagedObject* camera, Matrix4x4f* result) {
    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_camera_get_projection_matrix(camera, result);
    }

    auto original = m_camera_get_projection_matrix_hook->get_original<decltype(camera_get_projection_matrix_hook)>();

    auto ret = original(camera, result);

    for (auto& mod : mods) {
        mod->on_camera_get_projection_matrix(camera, result);
    }

    return ret;
}

Matrix4x4f* Hooks::camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    return g_hook->camera_get_projection_matrix_hook_internal(camera, result);
}

Matrix4x4f* Hooks::camera_get_view_matrix_hook_internal(REManagedObject* camera, Matrix4x4f* result) {
    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_pre_camera_get_view_matrix(camera, result);
    }

    auto original = m_camera_get_view_matrix_hook->get_original<decltype(camera_get_view_matrix_hook)>();

    auto ret = original(camera, result);

    for (auto& mod : mods) {
        mod->on_camera_get_view_matrix(camera, result);
    }

    return ret;
}

Matrix4x4f* Hooks::camera_get_view_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    return g_hook->camera_get_view_matrix_hook_internal(camera, result);
}