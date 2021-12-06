#include "REFramework.hpp"
#include "mods/ScriptRunner.hpp"

#include "../include/API.hpp"

bool reframework_is_ready() {
    if (g_framework == nullptr) {
        return false;
    }

    return g_framework->is_ready();
}

lua_State* reframework_get_lua_state() {
    if (!reframework_is_ready()) {
        return nullptr;
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto& script_runner = ScriptRunner::get();
    auto& state = script_runner->get_state();

    if (state == nullptr) {
        return nullptr;
    }

    return state->lua();
}

bool reframework_on_initialized(REFInitializedCb cb) {
    return REFramework::add_on_initialized(cb);
}

bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return REFramework::add_on_initialized([cb]() {
        auto& script_runner = ScriptRunner::get();

        script_runner->add_on_lua_state_created(cb);
    });
}

bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return REFramework::add_on_initialized([cb]() {
        auto& script_runner = ScriptRunner::get();

        script_runner->add_on_lua_state_destroyed(cb);
    });
}