#include "Mods.hpp"
#include "REFramework.hpp"
#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "PositionHooks.hpp"

PositionHooks* g_hook = nullptr;

PositionHooks::PositionHooks() {
    g_hook = this;
}

std::optional<std::string> PositionHooks::on_initialize() {
    auto game = g_framework->get_module().as<HMODULE>();

    const auto mod_size = utility::get_module_size(game);

    if (!mod_size) {
        return "Unable to get module size";
    }

    // The 48 8B 4D 40 bit might change.
    // Version 1.0 jmp stub: game+0x1dc7de0
    // Version 1
    //auto updateTransformCall = utility::scan(game, "E8 ? ? ? ? 48 8B 5B ? 48 85 DB 75 ? 48 8B 4D 40 48 31 E1");

    // Version 2 Dec 17th, 2019 (works on old version too) game.exe+0x1DD3FF0
    auto update_transform_call = utility::scan(game, "E8 ? ? ? ? 48 8B 5B ? 48 85 DB 75 ? 48 8B 4D 40 48 ? ?");

    if (!update_transform_call) {
        return "Unable to find UpdateTransform pattern.";
    }

    auto update_transform = utility::calculate_absolute(*update_transform_call + 1);
    spdlog::info("UpdateTransform: {:x}", update_transform);

    // Can be found by breakpointing RETransform's worldTransform
    m_update_transform_hook = std::make_unique<FunctionHook>(update_transform, &update_transform_hook);

    if (!m_update_transform_hook->create()) {
        return "Failed to hook UpdateTransform";
    }

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
    auto update_camera_controller = utility::scan(game, "40 55 ? 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? 00 00 48 8B 41 50");

    // Keep searching through the game module to find the correct function.
    while (update_camera_controller) {
        // Present in old versions, near the very top of the function
        // .srdata:0000000140D41B21 4C 89 B4 24 A0 02 00 00                       mov [rsp+2C0h+var_20], r14
        // .srdata:0000000140D41B29 E8 A2 51 C8 00                                call    sub_1419C6CD0
        // .srdata:0000000140D41B2E 0F B6 C8                                      movzx   ecx, al
        if (utility::scan(*update_camera_controller + 1, 0x100, "4C 89 ? ? ? ? ? ? E8 ? ? ? ? 0F B6 C8")) {
            // found the correct function
            break;
        }
        
        update_camera_controller = utility::scan(*update_camera_controller + 1, *mod_size - 100, "40 55 ? 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? 00 00 48 8B 41 50");
    }

    if (!update_camera_controller) {
        return "Failed to find UpdateCameraController pattern";
    }

    spdlog::info("UpdateCameraController: {:x}", *update_camera_controller);

    // Can be found by breakpointing camera controller's worldPosition
    m_update_camera_controller_hook = std::make_unique<FunctionHook>(*update_camera_controller, &update_camera_controller_hook);

    if (!m_update_camera_controller_hook->create()) {
        return "Failed to hook UpdateCameraController";
    }

    // Version 1.0 jmp stub: game+0xCF2510
    // Version 1.0 function: game+0xB436230
    
    // Version 1
    //auto updatecamera_controller2 = utility::scan(game, "40 53 57 48 81 ec ? ? ? ? 48 8b 41 ? 48 89 d7 48 8b 92 ? ? 00 00");
    // Version 2 Dec 17th, 2019 game.exe+0x6CD9C0 (works on old version too)
    auto update_camera_controller2 = utility::scan(game, "40 53 57 48 81 EC ? ? ? ? 48 ? ? ? 48 ? ? 48 ? ? ? ? 00 00");

#ifdef RE3
    while (update_camera_controller2) {
        if (utility::scan(*update_camera_controller2, 0x100, "0F B6 4F 51")) {
            break;
        }

        update_camera_controller2 = utility::scan(*update_camera_controller2 + 1,
            (uint32_t)(*utility::get_module_size(game) - ((*update_camera_controller2 + 1) - (uintptr_t)game)),
            "40 53 57 48 81 EC ? ? ? ? 48 ? ? ? 48 ? ? 48 ? ? ? ? 00 00");
    }
#endif

    if (!update_camera_controller2) {
        return "Unable to find UpdateCameraController2 pattern.";
    }

    spdlog::info("Updatecamera_controller2: {:x}", *update_camera_controller2);

    // Can be found by breakpointing camera controller's worldRotation
    m_update_camera_controller2_hook = std::make_unique<FunctionHook>(*update_camera_controller2, &update_camera_controller2_hook);

    if (!m_update_camera_controller2_hook->create()) {
        return "Failed to hook Updatecamera_controller2";
    }

    return Mod::on_initialize();
}

void* PositionHooks::update_transform_hook_internal(RETransform* t, uint8_t a2, uint32_t a3) {
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

void* PositionHooks::update_transform_hook(RETransform* t, uint8_t a2, uint32_t a3) {
    return g_hook->update_transform_hook_internal(t, a2, a3);
}

void* PositionHooks::update_camera_controller_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller) {
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

void* PositionHooks::update_camera_controller_hook(void* a1, RopewayPlayerCameraController* camera_controller) {
    return g_hook->update_camera_controller_hook_internal(a1, camera_controller);
}

void* PositionHooks::update_camera_controller2_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller) {
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

void* PositionHooks::update_camera_controller2_hook(void* a1, RopewayPlayerCameraController* camera_controller) {
    return g_hook->update_camera_controller2_hook_internal(a1, camera_controller);
}

