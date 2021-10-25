#include <cstdint>

#include <imgui.h>

#include "../sdk/REContext.hpp"
#include "../sdk/REManagedObject.hpp"
#include "../sdk/RETypeDB.hpp"
#include "../sdk/SceneManager.hpp"

#include "utility/String.hpp"

#include "ScriptRunner.hpp"

namespace api::re {
void msg(const char* text) {
    MessageBox(g_framework->get_window(), text, "ScriptRunner Message", MB_ICONINFORMATION | MB_OK);
}
}

namespace api::sdk {
void* get_thread_context() {
    return (void*)::sdk::get_thread_context();
}

auto find_type_definition(const char* name) {
    return ::sdk::RETypeDB::get()->find_type(name);
}

void* get_native_singleton(const char* name) {
    return (void*)::sdk::get_native_singleton<void>(name);
}

auto get_managed_singleton(const char* name) {
    return g_framework->get_globals()->get(name);
}

void* create_managed_string(const char* text) {
    return ::sdk::VM::create_managed_string(utility::widen(text));
}

sol::object call_native_func(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    static std::vector<void*> args{};
    auto l = va.lua_state();

    args.clear();

    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    for (auto&& arg : va) {
        auto i = arg.stack_index();

        // sol2 doesn't seem to differentiate between Lua integers and numbers. So
        // we must do it ourselves.
        if (lua_isinteger(l, i)) {
            auto n = (intptr_t)lua_tointeger(l, i);
            args.push_back((void*)n);
        } else if (lua_isnumber(l, i)) {
            auto f = lua_tonumber(l, i);
            auto n = *(intptr_t*)&f;
            args.push_back((void*)n);
        } else if (lua_isstring(l, i)) {
            auto s = lua_tostring(l, i);
            args.push_back(create_managed_string(s));
        } else {
            args.push_back(arg.as<void*>());
        }
    }

    auto ret_val = ::sdk::invoke_object_func(real_obj, ty, name, args);

    // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
    // let's return nil instead since that's a much more usable value.
    if (ret_val == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    // Convert return values to the correct Lua types.
    auto fn = ty->get_method(name);
    auto ret_ty = fn->get_return_type();

    if (ret_ty != nullptr && ret_ty->get_full_name() == "System.String") {
        auto managed_ret_val = (::REManagedObject*)ret_val;
        auto managed_str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(managed_ret_val) - sizeof(::REManagedObject));
        auto str = utility::narrow(managed_str->data);

        return sol::make_object(l, str);
    }

    // TODO: handle more return type conversions.

    return sol::make_object(l, ret_val);
}

auto call_object_func(sol::object obj, const char* name, sol::variadic_args va) {
    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    auto def = utility::re_managed_object::get_type_definition((::REManagedObject*)real_obj);

    return call_native_func(obj, def, name, va);
}

void* get_primary_camera() {
    return ::sdk::get_primary_camera();
}

void hook(sol::this_state s, ::sdk::REMethodDefinition* fn, sol::function cb) {
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    auto hook = std::make_unique<ScriptState::HookedFn>();

    hook->target_fn = (void*)fn->get_function();
    hook->script_fn = cb;
    hook->script_args = sol_state.create_table();

    auto& args = hook->args;
    auto& fn_hook = hook->fn_hook;
    auto& g = hook->facilitator_gen;

    // Make sure we have room to store the arguments.
    args.resize(2 + fn->get_num_params());

    // Generate the facilitator function that will store the arguments, call on_hook, 
    // restore the arguments, and call the original function.
    Xbyak::Label hook_label{}, args_label{}, this_label{}, on_hook_label{}, orig_label{};

    // Store args.
    // TODO: Handle all the arguments the function takes.
    g.sub(g.rsp, 40);
    g.mov(g.rax, g.ptr[g.rip + args_label]);
    g.mov(g.ptr[g.rax], g.rcx);
    g.mov(g.ptr[g.rax + 8], g.rdx);

    for (auto i = 0u; i < fn->get_num_params(); ++i) {
        auto args_offset = 16 + (i * 8);

        if (i == 0) { // stored in r8
            g.mov(g.ptr[g.rax + args_offset], g.r8);
        } else if (i == 1) { // stored in r9
            g.mov(g.ptr[g.rax + args_offset], g.r9);
        } else {
            // TODO: handle stack args.
        }
    }

    // Call on_hook.
    g.mov(g.rcx, g.ptr[g.rip + this_label]);
    g.mov(g.rdx, g.ptr[g.rip + hook_label]);
    g.call(g.ptr[g.rip + on_hook_label]);

    // Restore args.
    g.mov(g.rax, g.ptr[g.rip + args_label]);
    g.mov(g.rcx, g.ptr[g.rax]);
    g.mov(g.rdx, g.ptr[g.rax + 8]);

    for (auto i = 0u; i < fn->get_num_params(); ++i) {
        auto args_offset = 16 + (i * 8);

        if (i == 0) { // stored in r8
            g.mov(g.r8, g.ptr[g.rax + args_offset]);
        } else if (i == 1) { // stored in r9
            g.mov(g.r9, g.ptr[g.rax + args_offset]);
        } else {
            // TODO: handle stack args.
        }
    }

    // Call original function.
    g.add(g.rsp, 40);
    g.jmp(g.ptr[g.rip + orig_label]);
    
    g.L(hook_label);
    g.dq((uint64_t)hook.get());
    g.L(args_label);
    g.dq((uint64_t)args.data());
    g.L(this_label);
    g.dq((uint64_t)state);
    g.L(on_hook_label);
    g.dq((uint64_t)&ScriptState::on_hook_static);
    g.L(orig_label);
    // Can't do the following because the hook hasn't been created yet.
    //g.dq(fn_hook->get_original());
    g.dq(0);

    // Hook the function to our facilitator.
    fn_hook = std::make_unique<FunctionHook>(fn->get_function(), (void*)g.getCode());

    // Set the facilitators original function pointer.
    *(uintptr_t*)orig_label.getAddress() = fn_hook->get_original();

    fn_hook->create();
    state->hooked_fns().emplace_back(std::move(hook));
}
}

namespace api::log {
void info(const char* str) {
    spdlog::info(str);
}

void warn(const char* str) {
    spdlog::warn(str);
}

void error(const char* str) {
    spdlog::error(str);
}

void debug(const char* str) {
    OutputDebugString(str);
    spdlog::debug(str);
}
}

ScriptState::ScriptState() {
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32, sol::lib::utf8);

    m_lua.registry()["state"] = this;

    auto re = m_lua.create_table();
    re["msg"] = api::re::msg;
    re["on_pre_application_entry"] = [this](const char* name, sol::function fn) { m_pre_application_entry_fns.emplace(name, fn); };
    re["on_application_entry"] = [this](const char* name, sol::function fn) { m_application_entry_fns.emplace(name, fn); };
    m_lua["re"] = re;

    auto sdk = m_lua.create_table();
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_primary_camera"] = api::sdk::get_primary_camera;
    sdk["hook"] = api::sdk::hook;
    sdk["to_managed_object"] = [](void* ptr) { return (REManagedObject*)ptr; };
    m_lua["sdk"] = sdk;

    auto log = m_lua.create_table();
    log["info"] = api::log::info;
    log["warn"] = api::log::warn;
    log["error"] = api::log::error;
    log["debug"] = api::log::debug;
    m_lua["log"] = log;

    m_lua.new_usertype<::sdk::RETypeDefinition>("RETypeDefinition", 
        "get_method", &::sdk::RETypeDefinition::get_method);
    m_lua.new_usertype<REManagedObject>("REManagedObject", 
        "get_type_definition", [](REManagedObject* obj) { return utility::re_managed_object::get_type_definition(obj); });
}

void ScriptState::run_script(const std::string& p) {
    /*m_lua.safe_script_file(p, [](lua_State*, sol::protected_function_result pfr) {
        if (!pfr.valid()) {
            sol::error err = pfr;
            api::re::msg(err.what());
        }
        return pfr;
    });*/
    try {
        m_lua.safe_script_file(p);
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
        api::re::msg(e.what());
    }
}

void ScriptState::on_pre_application_entry(const char* name) {
    try {
        auto range = m_pre_application_entry_fns.equal_range(name);

        for (auto it = range.first; it != range.second; ++it) {
            it->second();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_application_entry(const char* name) {
    try {
        auto range = m_application_entry_fns.equal_range(name);

        for (auto it = range.first; it != range.second; ++it) {
            it->second();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_hook(HookedFn* fn) {
    try {
        // Call the script function.
        // TODO: Pass arguments to the script function.
        // TODO: Take return value from the script function into account.
        // TODO: Take changes to the arguments into account.
        for (auto i = 0u; i < fn->args.size(); ++i) {
            fn->script_args[i + 1] = (void*)fn->args[i];
        }

        fn->script_fn(fn->script_args);
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptRunner::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (ImGui::Button("Run script")) {
        OPENFILENAME ofn{};
        char file[260]{};

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_framework->get_window();
        ofn.lpstrFile = file;
        ofn.nMaxFile = sizeof(file);
        ofn.lpstrFilter = "Lua script files (*.lua)\0*.lua\0";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) != FALSE) {
            m_state->run_script(file);
        }
    }

    if (ImGui::Button("Reset scripts")) {
        m_state = std::make_unique<ScriptState>();
    }
}

void ScriptRunner::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    m_state->on_pre_application_entry(name);
}

void ScriptRunner::on_application_entry(void* entry, const char* name, size_t hash) {
    m_state->on_application_entry(name);
}
