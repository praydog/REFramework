#include "Mods.hpp"
#include "REFramework.hpp"
#include "utility/Scan.hpp"
#include "utility/Module.hpp"
#include "utility/String.hpp"

#include "sdk/Application.hpp"

#include "Hooks.hpp"

Hooks* g_hook = nullptr;

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
        auto result = hook();

        // Error occurred when hooking
        if (result) {
            return result;
        }
    }

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
    auto absolute_offset = 1;
    auto update_transform_call = utility::scan(game, "E8 ? ? ? ? 48 8B 5B ? 48 85 DB 75 ? 48 8B 4D 40 48 ? ?");

    if (!update_transform_call) {
        // RE7
        update_transform_call = utility::scan(game, "0F B6 D1 48 8B CB E8 ? ? ? ? 48 8B 9B ? ? ? ?");

        if (!update_transform_call) {
            return "Unable to find UpdateTransform pattern.";
        }

        absolute_offset = 7;
    }

    auto update_transform = utility::calculate_absolute(*update_transform_call + absolute_offset);
    spdlog::info("UpdateTransform: {:x}", update_transform);

    // Can be found by breakpointing RETransform's worldTransform
    m_update_transform_hook = std::make_unique<FunctionHook>(update_transform, &update_transform_hook);

    if (!m_update_transform_hook->create()) {
        return "Failed to hook UpdateTransform";
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
    auto gui_draw_call = utility::scan(game, "49 8B 0C CE 48 83 79 10 00 74 ? E8 ? ? ? ?");

    if (!gui_draw_call) {
        // RE7 (+0x20 grabs the owner ptr, 0x10 in others)
        gui_draw_call = utility::scan(game, "49 8B 0C CE 48 83 79 20 00 74 ? E8 ? ? ? ?");

        if (!gui_draw_call) {
            return "Unable to find gui_draw_call pattern.";
        }
    }

    auto gui_draw = utility::calculate_absolute(*gui_draw_call + 12);
    spdlog::info("gui_draw_call: {:x}", gui_draw);

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
    auto application = sdk::Application::get();

    if (application == nullptr) {
        return "Failed to get via.Application";
    }

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

    if (m_profiling_enabled) {
        Hooks::ApplicationEntryData profiler_entry{};
        
        auto now = std::chrono::high_resolution_clock::now();
        auto& mods = g_framework->get_mods()->get_mods();

        if (hash == "BeginRendering"_fnv) {
            g_framework->run_imgui_frame();
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
            g_framework->run_imgui_frame();
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
