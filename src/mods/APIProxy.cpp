#include "utility/FunctionHookMinHook.hpp"
#include "utility/PersistentTreeState.hpp"
#include "utility/String.hpp"

#include "ScriptRunner.hpp"

#include "PluginLoader.hpp"
#include "APIProxy.hpp"

std::shared_ptr<APIProxy>& APIProxy::get() {
    static auto instance = std::make_shared<APIProxy>();
    return instance;
}

bool APIProxy::add_on_lua_state_created(APIProxy::REFLuaStateCreatedCb cb) {
    {
        std::scoped_lock _{m_api_cb_mtx};
        append_locked(m_on_lua_state_created_cbs, cb);
    }

    // Called without the lock: the callback may register more callbacks.
    auto& state = ScriptRunner::get()->get_state();

    if (state != nullptr && state->lua().lua_state() != nullptr) {
        cb(state->lua());
    }

    return true;
}

bool APIProxy::add_on_lua_state_destroyed(APIProxy::REFLuaStateDestroyedCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_lua_state_destroyed_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_present(APIProxy::REFOnPresentCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_present_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_pre_application_entry(std::string_view name, REFOnPreApplicationEntryCb cb) {
    if (name.empty()) {
        return false;
    }

    const auto name_hash = utility::hash(name);

    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_pre_application_entry_cbs[name_hash], std::move(cb));
    return true;
}

bool APIProxy::add_on_post_application_entry(std::string_view name, REFOnPostApplicationEntryCb cb) {
    if (name.empty()) {
        return false;
    }

    const auto name_hash = utility::hash(name);

    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_post_application_entry_cbs[name_hash], std::move(cb));
    return true;
}

bool APIProxy::add_on_device_reset(REFOnDeviceResetCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_device_reset_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_message(REFOnMessageCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_message_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_imgui_frame(REFOnImGuiFrameCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_imgui_frame_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_imgui_draw_ui(REFOnImGuiDrawUICb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_imgui_draw_ui_cbs, std::move(cb));
    return true;
}

bool APIProxy::add_on_pre_gui_draw_element(REFOnPreGuiDrawElementCb cb) {
    std::scoped_lock _{m_api_cb_mtx};

    append_locked(m_on_pre_gui_draw_element_cbs, std::move(cb));
    return true;
}

void APIProxy::on_lua_state_created(sol::state& state) {
    const auto cbs = snapshot(m_on_lua_state_created_cbs);

    if (cbs == nullptr) {
        return;
    }

    for (auto& cb : *cbs) {
        try {
            cb(state.lua_state());
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_lua_state_created callback; one of the plugins has an error.");
        }
    }
}

void APIProxy::on_lua_state_destroyed(sol::state& state) {
    const auto cbs = snapshot(m_on_lua_state_destroyed_cbs);

    if (cbs == nullptr) {
        return;
    }

    for (auto& cb : *cbs) {
        try {
            cb(state.lua_state());
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_lua_state_destroyed callback; one of the plugins has an error.");
        }
    }
}

void APIProxy::on_present() {
    reframework::g_renderer_data.renderer_type = (int)g_framework->get_renderer_type();
    
    if (reframework::g_renderer_data.renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        auto& d3d11 = g_framework->get_d3d11_hook();

        reframework::g_renderer_data.device = d3d11->get_device();
        reframework::g_renderer_data.swapchain = d3d11->get_swap_chain();
    } else if (reframework::g_renderer_data.renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        auto& d3d12 = g_framework->get_d3d12_hook();

        reframework::g_renderer_data.device = d3d12->get_device();
        reframework::g_renderer_data.swapchain = d3d12->get_swap_chain();
        reframework::g_renderer_data.command_queue = d3d12->get_command_queue();
    }

    const auto cbs = snapshot(m_on_present_cbs);

    if (cbs == nullptr) {
        return;
    }

    for (auto&& cb : *cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_present callback; one of the plugins has an error.");
        }
    }
}

// For cimgui redirection when on_imgui_frame is called.
namespace cimgui {
std::unique_ptr<FunctionHookMinHook> g_load_library_ex_w_hook{nullptr};

HMODULE load_library_ex_w_hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (lpLibFileName != nullptr) {
        if (std::wstring_view{lpLibFileName}.contains(L"cimgui.dll")) {
            spdlog::info("[LoadLibraryExW] Redirecting cimgui.dll to ourselves");
            return REFramework::get_reframework_module();
        }
    }

    auto og = g_load_library_ex_w_hook->get_original<decltype(LoadLibraryExW)>();
    return og(lpLibFileName, hFile, dwFlags);
}

void setup_hook() {
    if (cimgui::g_load_library_ex_w_hook == nullptr) {
        auto llxw = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryExW");

        if (llxw != nullptr) {
            spdlog::info("[REFramework] Hooking LoadLibraryExW for cimgui.dll redirection");

            cimgui::g_load_library_ex_w_hook = std::make_unique<FunctionHookMinHook>(LoadLibraryExW, cimgui::load_library_ex_w_hook);
            
            if (!cimgui::g_load_library_ex_w_hook->create()) {
                spdlog::error("[REFramework] Failed to hook LoadLibraryExW for cimgui.dll redirection");
                return;
            }
        }
    }
}
} // namespace cimgui

// imgui frame.
void APIProxy::on_frame() {
    const auto cbs = snapshot(m_on_imgui_frame_cbs);

    if (cbs != nullptr && !cbs->empty()) {
        cimgui::setup_hook();

        ::REFImGuiFrameCbData data{};
        data.context = ImGui::GetCurrentContext();

        ImGui::GetAllocatorFunctions((ImGuiMemAllocFunc*)&data.malloc_fn, (ImGuiMemFreeFunc*)&data.free_fn, &data.user_data);

        for (auto&& cb : *cbs) {
            try {
                cb(&data);
            } catch(...) {
                spdlog::error("[APIProxy] Exception occurred in on_imgui_frame callback; one of the plugins has an error.");
            }
        }
    }
}

// imgui draw ui.
void APIProxy::on_draw_ui() {
    const auto cbs = snapshot(m_on_imgui_draw_ui_cbs);

    if (cbs != nullptr && !cbs->empty()) {
        cimgui::setup_hook();

        ::REFImGuiFrameCbData data{};
        data.context = ImGui::GetCurrentContext();

        ImGui::GetAllocatorFunctions((ImGuiMemAllocFunc*)&data.malloc_fn, (ImGuiMemFreeFunc*)&data.free_fn, &data.user_data);

        for (auto&& cb : *cbs) {
            try {
                cb(&data);
            } catch(...) {
                spdlog::error("[APIProxy] Exception occurred in on_imgui_draw_ui callback; one of the plugins has an error.");
            }
        }
    }
}

void APIProxy::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    const auto cbs = snapshot(m_on_pre_application_entry_cbs, hash);

    if (cbs == nullptr) {
        return;
    }

    for (auto&& cb : *cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_application_entry callback ({}); one of the plugins has an error.", name);
        }
    }
}

void APIProxy::on_application_entry(void* entry, const char* name, size_t hash) {
    const auto cbs = snapshot(m_on_post_application_entry_cbs, hash);

    if (cbs == nullptr) {
        return;
    }

    for (auto&& cb : *cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_application_entry callback ({}); one of the plugins has an error.", name);
        }
    }
}

void APIProxy::on_device_reset() {
    const auto cbs = snapshot(m_on_device_reset_cbs);

    if (cbs == nullptr) {
        return;
    }

    for (auto&& cb : *cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_device_reset callback; one of the plugins has an error.");
        }
    }
}

bool APIProxy::on_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    const auto cbs = snapshot(m_on_message_cbs);

    if (cbs == nullptr) {
        return true;
    }

    for (auto&& cb : *cbs) {
        try {
            if (!cb(hwnd, msg, wparam, lparam)) {
                return false;
            }
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_message callback; one of the plugins has an error.");
            continue;
        }
    }

    return true;
}

bool APIProxy::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    const auto cbs = snapshot(m_on_pre_gui_draw_element_cbs);

    if (cbs == nullptr) {
        return true;
    }

    bool any_false = false;

    for (auto&& cb : *cbs) {
        try {
            if (!cb(gui_element, primitive_context)) {
                any_false = true;
            }
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_gui_draw_element callback; one of the plugins has an error.");
            continue;
        }
    }

    return !any_false;
}
