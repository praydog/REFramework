#include "mods/APIProxy.hpp"
#include "mods/ScriptRunner.hpp"

#include "REFramework.hpp"

#include "../include/API.hpp"

bool reframework_on_initialized(REFInitializedCb cb) {
    return APIProxy::add_on_initialized(cb);
}

bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return APIProxy::add_on_initialized([cb]() {
        APIProxy::get()->add_on_lua_state_created(cb);
    });
}

bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return APIProxy::add_on_initialized([cb]() {
        APIProxy::get()->add_on_lua_state_destroyed(cb);
    });
}

bool reframework_on_frame(REFOnFrameCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return APIProxy::add_on_initialized([cb]() {
        APIProxy::get()->add_on_frame(cb);
    });
}

bool reframework_on_pre_application_entry(const char* name, REFOnPreApplicationEntryCb cb) {
    if (cb == nullptr) {
        return false;
    }

    auto cppname = std::string{name};

    if (cppname.empty()) {
        return false;
    }

    return APIProxy::add_on_initialized([cppname, cb]() {
        APIProxy::get()->add_on_pre_application_entry(cppname, cb);
    });   
}

bool reframework_on_post_application_entry(const char* name, REFOnPostApplicationEntryCb cb) {
    if (cb == nullptr || name == nullptr) {
        return false;
    }

    auto cppname = std::string{name};

    if (cppname.empty()) {
        return false;
    }

    return APIProxy::add_on_initialized([cppname, cb]() {
        APIProxy::get()->add_on_post_application_entry(cppname, cb);
    });   
}
