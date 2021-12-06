#include "ScriptRunner.hpp"

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
    
}

void APIProxy::on_application_entry(void* entry, const char* name, size_t hash) {
    
}
