#include "utility/String.hpp"

#include "ScriptRunner.hpp"

#include "PluginLoader.hpp"
#include "APIProxy.hpp"

std::recursive_mutex APIProxy::s_api_cb_mtx{};
std::vector<APIProxy::REFInitializedCb> APIProxy::s_on_initialized_cbs{};

std::shared_ptr<APIProxy>& APIProxy::get() {
    static auto instance = std::make_shared<APIProxy>();
    return instance;
}

bool APIProxy::add_on_initialized(APIProxy::REFInitializedCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    s_on_initialized_cbs.push_back(cb);
    return true;
}

bool APIProxy::add_on_lua_state_created(APIProxy::REFLuaStateCreatedCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    m_on_lua_state_created_cbs.push_back(cb);

    auto& state = ScriptRunner::get()->get_state();

    if (state != nullptr && state->lua().lua_state() != nullptr) {
        cb(state->lua());
    }

    return true;
}

bool APIProxy::add_on_lua_state_destroyed(APIProxy::REFLuaStateDestroyedCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    m_on_lua_state_destroyed_cbs.push_back(cb);
    return true;
}

bool APIProxy::add_on_frame(APIProxy::REFOnFrameCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    m_on_frame_cbs.push_back(cb);
    return true;
}

bool APIProxy::add_on_pre_application_entry(std::string_view name, REFOnPreApplicationEntryCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    if (name.empty()) {
        return false;
    }

    const auto name_hash = utility::hash(name);

    m_on_pre_application_entry_cbs[name_hash].push_back(cb);
    return true;
}

bool APIProxy::add_on_post_application_entry(std::string_view name, REFOnPostApplicationEntryCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    if (name.empty()) {
        return false;
    }

    const auto name_hash = utility::hash(name);

    m_on_post_application_entry_cbs[name_hash].push_back(cb);
    return true;
}

bool APIProxy::add_on_device_reset(REFOnDeviceResetCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    m_on_device_reset_cbs.push_back(cb);
    return true;
}

bool APIProxy::add_on_message(REFOnMessageCb cb) {
    std::scoped_lock _{s_api_cb_mtx};

    m_on_message_cbs.push_back(cb);
    return true;
}

void APIProxy::on_lua_state_created(sol::state& state) {
    std::scoped_lock _{s_api_cb_mtx};

    for (auto& cb : m_on_lua_state_created_cbs) {
        cb(state.lua_state());
    }
}

void APIProxy::on_lua_state_destroyed(sol::state& state) {
    std::scoped_lock _{s_api_cb_mtx};

    for (auto& cb : m_on_lua_state_destroyed_cbs) {
        cb(state.lua_state());
    }
}

void APIProxy::on_frame() {
    std::scoped_lock _{s_api_cb_mtx};

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

    if (!s_on_initialized_cbs.empty()) {
        for (auto&& cb : s_on_initialized_cbs) {
            cb();
        }

        s_on_initialized_cbs.clear();
    }

    for (auto&& cb : m_on_frame_cbs) {
        cb();
    }
}

void APIProxy::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{s_api_cb_mtx};

    if (auto it = m_on_pre_application_entry_cbs.find(hash); it != m_on_pre_application_entry_cbs.end()) {
        for (auto&& cb : it->second) {
            cb();
        }
    }
}

void APIProxy::on_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{s_api_cb_mtx};

    if (auto it = m_on_post_application_entry_cbs.find(hash); it != m_on_post_application_entry_cbs.end()) {
        for (auto&& cb : it->second) {
            cb();
        }
    }
}

void APIProxy::on_device_reset() {
    std::scoped_lock _{s_api_cb_mtx};

    for (auto&& cb : m_on_device_reset_cbs) {
        cb();
    }
}

bool APIProxy::on_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    std::scoped_lock _{s_api_cb_mtx};

    for (auto&& cb : m_on_message_cbs) {
        if (!cb(hwnd, msg, wparam, lparam)) {
            return false;
        }
    }

    return true;
}