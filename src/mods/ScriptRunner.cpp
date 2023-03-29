#define NOMINMAX

#include <cstdint>
#include <filesystem>

#include <imgui.h>

#include "sdk/REContext.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/REMath.hpp"

#include "utility/String.hpp"

#include "Mods.hpp"

#include "bindings/Sdk.hpp"
#include "bindings/ImGui.hpp"
#include "bindings/Json.hpp"
#include "bindings/FS.hpp"

#include "ScriptRunner.hpp"

#include <lstate.h> // weird include order because of sol
#include <lgc.h>

namespace api::re {
void msg(const char* text) {
    MessageBox(g_framework->get_window(), text, "ScriptRunner Message", MB_ICONINFORMATION | MB_OK);
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
    fprintf(stderr, "%s\n", str);
    spdlog::debug(str);
}
}

ScriptState::ScriptState(const ScriptState::GarbageCollectionData& gc_data) {
    std::scoped_lock _{ m_execution_mutex };

    m_lua.registry()["state"] = this;
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32,
        sol::lib::utf8, sol::lib::os, sol::lib::coroutine);

    // Disable garbage collection. We will manually do it at the end of each frame.
    gc_data_changed(gc_data);
    
    // Restrict os library
    auto os = m_lua["os"];
    os["remove"] = sol::nil;
    os["rename"] = sol::nil;
    os["execute"] = sol::nil;
    os["exit"] = sol::nil;
    os["setlocale"] = sol::nil;
    os["getenv"] = sol::nil;

    bindings::open_sdk(this);
    bindings::open_imgui(this);
    bindings::open_json(this);
    bindings::open_fs(this);

    auto re = m_lua.create_table();
    re["msg"] = api::re::msg;
    re["on_pre_application_entry"] = [this](const char* name, sol::function fn) { m_pre_application_entry_fns.emplace(utility::hash(name), fn); };
    re["on_application_entry"] = [this](const char* name, sol::function fn) { m_application_entry_fns.emplace(utility::hash(name), fn); };
    re["on_pre_gui_draw_element"] = [this](sol::function fn) { m_pre_gui_draw_element_fns.emplace_back(fn); };
    re["on_gui_draw_element"] = [this](sol::function fn) { m_gui_draw_element_fns.emplace_back(fn); };
    re["on_draw_ui"] = [this](sol::function fn) { m_on_draw_ui_fns.emplace_back(fn); };
    re["on_frame"] = [this](sol::function fn) { m_on_frame_fns.emplace_back(fn); };
    re["on_script_reset"] = [this](sol::function fn) { m_on_script_reset_fns.emplace_back(fn); };
    re["on_config_save"] = [this](sol::function fn) { m_on_config_save_fns.emplace_back(fn); };
    m_lua["re"] = re;


    auto log = m_lua.create_table();
    log["info"] = api::log::info;
    log["warn"] = api::log::warn;
    log["error"] = api::log::error;
    log["debug"] = api::log::debug;
    m_lua["log"] = log;

    
    // clang-format off
    // add vec2 usertype
    m_lua.new_usertype<Vector2f>("Vector2f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float)>(),
        "clone", [](Vector2f& v) -> Vector2f { return v; },
        "x", &Vector2f::x, 
        "y", &Vector2f::y, 
        "dot", [](Vector2f& v1, Vector2f& v2) { return glm::dot(v1, v2); },
        "length", [](Vector2f& v) { return glm::length(v); },
        "normalize", [](Vector2f& v) { v = glm::normalize(v); },
        "normalized", [](Vector2f& v) { return glm::normalize(v); },
        sol::meta_function::addition, [](Vector2f& lhs, Vector2f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector2f& lhs, Vector2f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector2f& lhs, float scalar) { return lhs * scalar; },
        "to_vec3", [](Vector2f& v) { return Vector3f{v.x, v.y, 0.0f}; },
        "to_vec4", [](Vector2f& v) { return Vector4f{v.x, v.y, 0.0f, 0.0f}; });

    // add vec3 usertype
    m_lua.new_usertype<Vector3f>("Vector3f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float, float)>(),
        "clone", [](Vector3f& v) -> Vector3f { return v; },
        "x", &Vector3f::x,
        "y", &Vector3f::y,
        "z", &Vector3f::z,
        "dot", [](Vector3f& v1, Vector3f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector3f& v1, Vector3f& v2) { return glm::cross(v1, v2); },
        "length", [](Vector3f& v) { return glm::length(v); },
        "normalize", [](Vector3f& v) { v = glm::normalize(v); },
        "normalized", [](Vector3f& v) { return glm::normalize(v); },
        "reflect", [](Vector3f& v, Vector3f& normal) { return glm::reflect(v, normal); },
        "refract", [](Vector3f& v, Vector3f& normal, float eta) { return glm::refract(v, normal, eta); },
        "lerp", [](Vector3f& v1, Vector3f& v2, float t) { return glm::lerp(v1, v2, t); },
        sol::meta_function::addition, [](Vector3f& lhs, Vector3f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector3f& lhs, Vector3f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector3f& lhs, float scalar) { return lhs * scalar; },
        "to_vec2", [](Vector3f& v) { return Vector2f{v.x, v.y}; },
        "to_vec4", [](Vector3f& v) { return Vector4f{v.x, v.y, v.z, 0.0f}; },
        "to_mat", [](Vector3f& v) { return glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, v, Vector3f{0.0f, 1.0f, 0.0f})); },
        "to_quat", [](Vector3f& v) { 
            auto mat = glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, v, Vector3f{0.0f, 1.0f, 0.0f}));

            return glm::quat{mat};
        });

    // add vec4 usertype
    m_lua.new_usertype<Vector4f>("Vector4f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float, float, float)>(),
        "clone", [](Vector4f& v) -> Vector4f { return v; },
        "x", &Vector4f::x,
        "y", &Vector4f::y,
        "z", &Vector4f::z,
        "w", &Vector4f::w,
        "dot", [](Vector4f& v1, Vector4f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector4f& v1, Vector4f& v2) { return glm::cross(Vector3f{v1.x, v1.y, v1.z}, Vector3f{v2.x, v2.y, v2.z}); },
        "length", [](Vector4f& v) { return glm::length(v); },
        "normalize", [](Vector4f& v) { v = glm::normalize(v); },
        "normalized", [](Vector4f& v) { return glm::normalize(v); },
        "reflect", [](Vector4f& v, Vector4f& normal) { return glm::reflect(v, normal); },
        "refract", [](Vector4f& v, Vector4f& normal, float eta) { return glm::refract(v, normal, eta); },
        "lerp", [](Vector4f& v1, Vector4f& v2, float t) { return glm::lerp(v1, v2, t); },
        sol::meta_function::addition, [](Vector4f& lhs, Vector4f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector4f& lhs, Vector4f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector4f& lhs, float scalar) { return lhs * scalar; },
        "to_vec2", [](Vector4f& v) { return Vector2f{v.x, v.y}; },
        "to_vec3", [](Vector4f& v) { return Vector3f{v.x, v.y, v.z}; },
        "to_mat", [](Vector4f& v) { return glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{v.x, v.y, v.z}, Vector3f{0.0f, 1.0f, 0.0f})); },
        "to_quat", [](Vector4f& v) { 
            auto mat = glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{v.x, v.y, v.z}, Vector3f{0.0f, 1.0f, 0.0f}));

            return glm::quat{mat};
        });

    // add Matrix4x4f (glm::mat4) usertype
    m_lua.new_usertype<Matrix4x4f>("Matrix4x4f",
        sol::meta_function::construct, 
         sol::constructors<
         Matrix4x4f(),
         Matrix4x4f(const Vector4f&, const Vector4f&, const Vector4f&, const Vector4f&),
         Matrix4x4f(float, float, float, float,
                    float, float, float, float,
                    float, float, float, float,
                    float, float, float, float)
        >(),
        "clone", [](Matrix4x4f& m) -> Matrix4x4f { return m; },
        "identity", []() { return glm::identity<Matrix4x4f>(); },
        "to_quat", [] (Matrix4x4f& m) { return glm::quat(m); },
        "inverse", [] (Matrix4x4f& m) { return glm::inverse(m); },
        "invert", [] (Matrix4x4f& m) { m = glm::inverse(m); },
        "interpolate", [](Matrix4x4f& m1, Matrix4x4f& m2, float t) { return glm::interpolate(m1, m2, t); },
        "matrix_rotation", [](Matrix4x4f& m) { return glm::extractMatrixRotation(m); },
        sol::meta_function::multiplication, sol::overload(
            [](Matrix4x4f& lhs, Matrix4x4f& rhs) {
                return lhs * rhs;
            },
            [](Matrix4x4f& lhs, Vector4f& rhs) {

                return lhs * rhs;
            }
        ),
        sol::meta_function::index, [](sol::this_state s, Matrix4x4f& lhs, sol::object index_obj) -> sol::object {
            if (!index_obj.is<int>()) {
                return sol::make_object(s, sol::lua_nil);
            }

            const auto index = index_obj.as<int>();

            if (index >= 4) {
                return sol::make_object(s, sol::lua_nil);
            }

            return sol::make_object(s, &lhs[index]);
        },
        sol::meta_function::new_index, [](Matrix4x4f& lhs, int index, Vector4f& rhs) {
            lhs[index] = rhs;
        }
    );

    // add glm::quat usertype
    m_lua.new_usertype<glm::quat>("Quaternion",
        sol::meta_function::construct, sol::constructors<glm::quat(), glm::quat(float, float, float, float), glm::quat(const Vector3f&)>(),
        "clone", [](glm::quat& q) -> glm::quat { return q; },
        "identity", []() { return glm::identity<glm::quat>(); },
        "x", &glm::quat::x,
        "y", &glm::quat::y,
        "z", &glm::quat::z,
        "w", &glm::quat::w,
        "to_mat4", [](glm::quat& q) { return Matrix4x4f{q}; },
        "to_euler", [](glm::quat& q) -> Vector3f { return utility::math::euler_angles(Matrix4x4f{q}); },
        "inverse", [](glm::quat& q) { return glm::inverse(q); },
        "invert", [](glm::quat& q) { q = glm::inverse(q); },
        "normalize", [](glm::quat& q) { q = glm::normalize(q); },
        "normalized", [](glm::quat& q) { return glm::normalize(q); },
        "slerp", [](glm::quat& q1, glm::quat& q2, float t) { return glm::slerp(q1, q2, t); },
        "dot", [](glm::quat& q1, glm::quat& q2) { return glm::dot(q1, q2); },
        "length", [](glm::quat& q) { return glm::length(q); },
        "conjugate", [](glm::quat& q) { return glm::conjugate(q); },
        sol::meta_function::multiplication, sol::overload( 
            [](glm::quat& lhs, glm::quat& rhs) -> glm::quat {
                return lhs * rhs;
            },
            [](glm::quat& lhs, Vector3f& rhs) -> Vector3f {
                return lhs * rhs;
            },
            [](glm::quat& lhs, Vector4f& rhs) -> Vector4f {
                return lhs * rhs;
            }
        ),
        sol::meta_function::index, [](sol::this_state s, glm::quat& lhs, sol::object index_obj) -> sol::object {
            if (!index_obj.is<int>()) {
                return sol::make_object(s, sol::lua_nil);
            }

            const auto index = index_obj.as<int>();

            if (index >= 4) {
                return sol::make_object(s, sol::lua_nil);
            }

            return sol::make_object(s, lhs[index]);
        },
        sol::meta_function::new_index, [](glm::quat& lhs, int index, float rhs) {
            lhs[index] = rhs;
        }
    );

    m_lua.new_usertype<REFramework>("REFramework",
        "save_config", &REFramework::save_config,
        "is_drawing_ui", &REFramework::is_drawing_ui,
        "get_game_name", &REFramework::get_game_name,
        "get_version_string", []() -> std::string { 
            return (std::stringstream{} 
            << REFRAMEWORK_PLUGIN_VERSION_MAJOR << "." 
            << REFRAMEWORK_PLUGIN_VERSION_MINOR << "." 
            << REFRAMEWORK_PLUGIN_VERSION_PATCH).str(); 
        },
        "get_version_major", []() -> int { return REFRAMEWORK_PLUGIN_VERSION_MAJOR; },
        "get_version_minor", []() -> int { return REFRAMEWORK_PLUGIN_VERSION_MINOR; },
        "get_version_patch", []() -> int { return REFRAMEWORK_PLUGIN_VERSION_PATCH; },
        "get_keyboard_state", &REFramework::get_keyboard_state,
        "is_key_down", [](REFramework& framework, int key) {
            if (key < 0 || key >= 256) {
                return false;
            }

            return framework.get_keyboard_state()[key] != 0; 
        },
        "get_first_key_down", [](sol::this_state s, REFramework& framework) -> sol::object {
            const auto& states = framework.get_keyboard_state();

            for (int i = 0; i < 256; i++) {
                if (states[i] != 0) {
                    return sol::make_object(s, i);
                }
            }

            return sol::make_object(s, sol::lua_nil);
        }
    );

    m_lua["reframework"] = g_framework.get();

    // clang-format on

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_lua_state_created(m_lua);
    }
}

ScriptState::~ScriptState() {
    std::scoped_lock _{m_execution_mutex};
    for (auto&& [fn, hook_ids] : m_hooks) {
        for (auto&& id : hook_ids) {
            g_hookman.remove(fn, id);
        }
    }
}

void ScriptState::run_script(const std::string& p) {
    std::scoped_lock _{ m_execution_mutex };

    spdlog::info("[ScriptState] Running script {}...", p);

    std::string old_path = m_lua["package"]["path"];

    try {
        auto path = std::filesystem::path(p);
        auto dir = path.parent_path();

        std::string package_path = m_lua["package"]["path"];

        package_path = old_path + ";" + dir.string() + "/?.lua";
        package_path = package_path + ";" + dir.string() + "/?/init.lua";
        package_path = package_path + ";" + dir.string() + "/?.dll";

        m_lua["package"]["path"] = package_path;
        m_lua.safe_script_file(p);
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
        api::re::msg(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error((std::stringstream{} << "Unknown error when running script " << p).str());
        api::re::msg((std::stringstream{} << "Unknown error when running script " << p).str().c_str());
    }

    m_lua["package"]["path"] = old_path;
}

// i have to wonder why this isn't in sol when they have safe_script stuff
sol::protected_function_result ScriptState::handle_protected_result(sol::protected_function_result result) {
    if (result.valid()) {
        return result;
    }

    sol::script_default_on_error(m_lua.lua_state(), std::move(result));
    return result;
}

void ScriptState::on_frame() {
    try {
        std::scoped_lock _{ m_execution_mutex };

        for (auto& fn : m_on_frame_fns) {
            handle_protected_result(fn());
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown error in on_frame");
    }

    api::imnodes::cleanup();
}

void ScriptState::on_draw_ui() {
    if (!ImGui::CollapsingHeader("Script Generated UI")) {
        return;
    }

    try {
        std::scoped_lock _{ m_execution_mutex };

        for (auto& fn : m_on_draw_ui_fns) {
            handle_protected_result(fn());
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown error in on_draw_ui");
    }

    api::imnodes::cleanup();
}

void ScriptState::on_pre_application_entry(size_t hash) {
    try {
        if (m_pre_application_entry_fns.empty()) {
            return;
        }

        auto range = m_pre_application_entry_fns.equal_range(hash);

        if (range.first != range.second) {
            std::scoped_lock _{ m_execution_mutex };

            for (auto it = range.first; it != range.second; ++it) {
                handle_protected_result(it->second());
            }
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown exception in on_pre_application_entry");
    }
}

void ScriptState::on_application_entry(size_t hash) {
    try {
        if (!m_application_entry_fns.empty()) {
            auto range = m_application_entry_fns.equal_range(hash);

            if (range.first != range.second) {
                std::scoped_lock _{ m_execution_mutex };

                for (auto it = range.first; it != range.second; ++it) {
                    handle_protected_result(it->second());
                }
            }
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown exception in on_application_entry");
    }

    if (hash == "EndRendering"_fnv && m_gc_data.gc_handler == ScriptState::GarbageCollectionHandler::REFRAMEWORK_MANAGED) {
        switch (m_gc_data.gc_type) {
            case ScriptState::GarbageCollectionType::FULL:
                lua_gc(m_lua, LUA_GCCOLLECT);
                break;
            case ScriptState::GarbageCollectionType::STEP: 
                {
                    const auto now = std::chrono::high_resolution_clock::now();

                    if (m_gc_data.gc_mode == ScriptState::GarbageCollectionMode::GENERATIONAL) {
                        lua_gc(m_lua, LUA_GCSTEP, 1);
                    } else {
                        while (lua_gc(m_lua, LUA_GCSTEP, 1) == 0) {
                            if (std::chrono::high_resolution_clock::now() - now >= m_gc_data.gc_budget) {
                                break;
                            }
                        }
                    }
                }
                break;
            default:
                lua_gc(m_lua, LUA_GCCOLLECT);
                break;
        };
    }
}

bool ScriptState::on_pre_gui_draw_element(REComponent* gui_element, void* context) {
    bool any_false = false;

    try {
        std::scoped_lock _{ m_execution_mutex };

        for (auto& fn : m_pre_gui_draw_element_fns) {
            if (sol::object result = handle_protected_result(fn(gui_element, context)); !result.is<sol::nil_t>() && result.is<bool>() && result.as<bool>() == false) {
                any_false = true;
            }
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown exception in on_pre_gui_draw_element");
    }

    return !any_false;
}

void ScriptState::on_gui_draw_element(REComponent* gui_element, void* context) {
    try {
        std::scoped_lock _{ m_execution_mutex };

        for (auto& fn : m_gui_draw_element_fns) {
            handle_protected_result(fn(gui_element, context));
        }
    } catch (const std::exception& e) {
        ScriptRunner::get()->spew_error(e.what());
    } catch (...) {
        ScriptRunner::get()->spew_error("Unknown exception in on_gui_draw_element");
    }
}

void ScriptState::on_script_reset() try {
    std::scoped_lock _{ m_execution_mutex };

    // We first call on_config_save functions so scripts can save prior to reset.
    for (auto& fn : m_on_config_save_fns) {
        handle_protected_result(fn());
    }

    for (auto& fn : m_on_script_reset_fns) {
        handle_protected_result(fn());
    }
} catch (const std::exception& e) {
    ScriptRunner::get()->spew_error(e.what());
} catch (...) {
    ScriptRunner::get()->spew_error("Unknown exception in on_script_reset");
}

void ScriptState::on_config_save() try {
    std::scoped_lock _{ m_execution_mutex };

    for (auto& fn : m_on_config_save_fns) {
        handle_protected_result(fn());
    }
}
catch (const std::exception& e) {
    ScriptRunner::get()->spew_error(e.what());
} catch (...) {
    ScriptRunner::get()->spew_error("Unknown exception in on_config_save");
}

void ScriptState::add_hook(
    sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb, sol::object ignore_jmp_obj) {
    m_hooks_to_add.emplace_back((::REManagedObject*)nullptr, fn, pre_cb, post_cb, ignore_jmp_obj);
}

void ScriptState::add_vtable(::REManagedObject* obj, sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb) {
    m_hooks_to_add.emplace_back(obj, fn, pre_cb, post_cb);
}

void ScriptState::install_hooks() {
    for (; !m_hooks_to_add.empty(); m_hooks_to_add.pop_front()) {
        auto hookdef = m_hooks_to_add.front();
        auto fn = hookdef.fn;
        auto pre_cb = hookdef.pre_cb;
        auto post_cb = hookdef.post_cb;
        auto ignore_jmp_object = hookdef.ignore_jmp_obj;
        const auto hookman_data = HookManager::EitherOr{hookdef.obj, hookdef.fn, ignore_jmp_object.is<bool>() ? ignore_jmp_object.as<bool>() : false};
        auto id = g_hookman.add_either_or(
            hookman_data,
            [pre_cb, state = this](auto& args, auto& arg_tys) -> HookManager::PreHookResult {
                using PreHookResult = HookManager::PreHookResult;

                auto _ = state->scoped_lock();
                auto result = PreHookResult::CALL_ORIGINAL;

                try {
                    if (pre_cb.is<sol::nil_t>()) {
                        return result;
                    }

                    auto script_args = state->lua().create_table();

                    // Call the script function.
                    // Convert the args to a table that we pass to the script function.
                    for (auto i = 0u; i < args.size(); ++i) {
                        script_args[i + 1] = (void*)args[i];
                    }

                    auto script_result = pre_cb(script_args);

                    if (!script_result.valid()) {
                        sol::script_default_on_error(state->lua(), std::move(script_result));
                    }

                    auto script_result_obj = script_result.get<sol::object>();

                    if (script_result_obj.is<PreHookResult>()) {
                        result = script_result_obj.as<PreHookResult>();
                    }

                    // Apply the changes to arguments that the script function may have made.
                    for (auto i = 0u; i < args.size(); ++i) {
                        auto arg = script_args[i + 1];
                        args[i] = (uintptr_t)arg.get<void*>();
                    }
                } catch (const std::exception& e) {
                    ScriptRunner::get()->spew_error(e.what());
                } catch (...) {
                    ScriptRunner::get()->spew_error("Unknown exception in pre_hook");
                }

                return result;
            },
            [post_cb, state = this](auto& ret_val, auto* ret_ty) {
                auto _ = state->scoped_lock();

                try {
                    if (post_cb.is<sol::nil_t>()) {
                        return;
                    }

                    auto script_result = post_cb((void*)ret_val);

                    if (!script_result.valid()) {
                        sol::script_default_on_error(state->lua(), std::move(script_result));
                    }

                    ret_val = (uintptr_t)script_result.get<void*>();
                } catch (const std::exception& e) {
                    ScriptRunner::get()->spew_error(e.what());
                } catch (...) {
                    ScriptRunner::get()->spew_error("Unknown exception in post_hook");
                }
            }
        );
        m_hooks[fn].emplace_back(id);
    }
}

void ScriptState::gc_data_changed(GarbageCollectionData data) {
    // Handler
    switch (data.gc_handler) {
    case ScriptState::GarbageCollectionHandler::REFRAMEWORK_MANAGED:
        lua_gc(m_lua, LUA_GCSTOP);
        break;
    case ScriptState::GarbageCollectionHandler::LUA_MANAGED:
        lua_gc(m_lua, LUA_GCRESTART);
        break;
    default:
        lua_gc(m_lua, LUA_GCRESTART);
        data.gc_handler = ScriptState::GarbageCollectionHandler::LUA_MANAGED;
        break;
    }

    // Type 
    if (data.gc_type >= ScriptState::GarbageCollectionType::LAST) {
       data.gc_type = ScriptState::GarbageCollectionType::STEP;
    }

    // Mode
    if (data.gc_mode >= ScriptState::GarbageCollectionMode::LAST) {
        data.gc_mode = ScriptState::GarbageCollectionMode::GENERATIONAL;
    }

    switch (data.gc_mode) {
    case ScriptState::GarbageCollectionMode::GENERATIONAL:
        lua_gc(m_lua, LUA_GCGEN, data.gc_minor_multiplier, data.gc_major_multiplier);
        break;
    case ScriptState::GarbageCollectionMode::INCREMENTAL:
        lua_gc(m_lua, LUA_GCINC);
        break;
    default:
        lua_gc(m_lua, LUA_GCGEN, data.gc_minor_multiplier, data.gc_major_multiplier);
        data.gc_mode = ScriptState::GarbageCollectionMode::GENERATIONAL;
        break;
    }

    m_gc_data = data;
}

std::shared_ptr<ScriptRunner>& ScriptRunner::get() {
    static auto instance = std::make_shared<ScriptRunner>();
    return instance;
}

std::optional<std::string> ScriptRunner::on_initialize() {
    return Mod::on_initialize();
}

void ScriptRunner::on_config_load(const utility::Config& cfg) {
    std::scoped_lock _{m_access_mutex};

    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    if (m_state != nullptr) {
        m_state->gc_data_changed(make_gc_data());
    }
}

void ScriptRunner::on_config_save(utility::Config& cfg) {
    std::scoped_lock _{m_access_mutex};

    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }

    if (m_state != nullptr) {
        m_state->on_config_save();
    }
}

void ScriptRunner::on_frame() {
    std::scoped_lock _{m_access_mutex};
    
    if (m_needs_first_reset) {
        spdlog::info("[ScriptRunner] Initializing Lua state for the first time...");

        // Calling reset_scripts even though the scripts have never been set yet still works.
        reset_scripts();
        m_needs_first_reset = false;

        spdlog::info("[ScriptRunner] Lua state initialized.");
    }

    if (m_state == nullptr) {
        return;
    }

    m_state->on_frame();

    // install_hooks gets called here because it ensures hooks get installed the next frame after they've been 
    // enqueued. This prevents a race that can occur if hooks were installed immediately during script loading.
    m_state->install_hooks();
}

void ScriptRunner::on_draw_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (ImGui::CollapsingHeader(get_name().data())) {
        if (ImGui::Button("Run script")) {
            OPENFILENAME ofn{};
            char file[260]{};

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = g_framework->get_window();
            ofn.lpstrFile = file;
            ofn.nMaxFile = sizeof(file);
            ofn.lpstrFilter = "Lua script files (*.lua)\0*.lua\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileName(&ofn) != FALSE) {
                std::scoped_lock _{ m_access_mutex };
                m_state->run_script(file);
                m_loaded_scripts.emplace_back(std::filesystem::path{file}.filename().string());
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset scripts")) {
            reset_scripts();
        }

        ImGui::SameLine();

        if (ImGui::Button("Spawn Debug Console")) {
            if (!m_console_spawned) {
                AllocConsole();
                freopen("CONIN$", "r", stdin);
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);

                m_console_spawned = true;
            }
        }

        if (ImGui::TreeNode("Garbage Collection Stats")) {
            std::scoped_lock _{ m_access_mutex };

            auto g = G(m_state->lua().lua_state());
            const auto bytes_in_use = g->totalbytes + g->GCdebt;

            ImGui::Text("Megabytes in use: %.2f", (float)bytes_in_use / 1024.0f / 1024.0f);

            ImGui::TreePop();
        }

        if (m_gc_handler->draw("Garbage Collection Handler")) {
            std::scoped_lock _{ m_access_mutex };
            m_state->gc_data_changed(make_gc_data());
        }

        if (m_gc_mode->draw("Garbage Collection Mode")) {
            std::scoped_lock _{ m_access_mutex };
            m_state->gc_data_changed(make_gc_data());
        }

        if ((uint32_t)m_gc_mode->value() == (uint32_t)ScriptState::GarbageCollectionMode::GENERATIONAL) {
            if (m_gc_minor_multiplier->draw("Minor GC Multiplier")) {
                std::scoped_lock _{ m_access_mutex };
                m_state->gc_data_changed(make_gc_data());
            }

            if (m_gc_major_multiplier->draw("Major GC Multiplier")) {
                std::scoped_lock _{ m_access_mutex };
                m_state->gc_data_changed(make_gc_data());
            }
        }

        if (m_gc_handler->value() == (int32_t)ScriptState::GarbageCollectionHandler::REFRAMEWORK_MANAGED) {
            if (m_gc_type->draw("Garbage Collection Type")) {
                std::scoped_lock _{ m_access_mutex };
                m_state->gc_data_changed(make_gc_data());
            }

            if ((uint32_t)m_gc_mode->value() != (uint32_t)ScriptState::GarbageCollectionMode::GENERATIONAL) {
                if (m_gc_budget->draw("Garbage Collection Budget")) {
                    std::scoped_lock _{ m_access_mutex };
                    m_state->gc_data_changed(make_gc_data());
                }
            }
        }

        m_log_to_disk->draw("Log Lua Errors to Disk");

        if (!m_last_script_error.empty()) {
            std::shared_lock _{m_script_error_mutex};

            const auto now = std::chrono::system_clock::now();
            const auto diff = now - m_last_script_error_time;
            const auto sec = std::chrono::duration<float>(diff).count();

            ImGui::TextWrapped("Last Error Time: %.2f seconds ago", sec);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped("Last Script Error: %s", m_last_script_error.data());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextWrapped("No Script Errors... yet!");
        }

        if (!m_known_scripts.empty()) {
            ImGui::Text("Known scripts:");

            for (auto&& name : m_known_scripts) {
                if (ImGui::Checkbox(name.data(), &m_loaded_scripts_map[name])) {
                    reset_scripts();
                    break;
                }
            }
        } else {
            ImGui::Text("No scripts loaded.");
        }
    }

    { 
        std::scoped_lock _{ m_access_mutex };

        if (m_state == nullptr) {
            return;
        }

        m_state->on_draw_ui();
    }
}

void ScriptRunner::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{ m_access_mutex };

    if (m_state == nullptr) {
        return;
    }

    m_state->on_pre_application_entry(hash);
}

void ScriptRunner::on_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{ m_access_mutex };

    if (m_state == nullptr) {
        return;
    }

    m_state->on_application_entry(hash);
}

bool ScriptRunner::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    std::scoped_lock _{ m_access_mutex };

    if (m_state == nullptr) {
        return true;
    }

    return m_state->on_pre_gui_draw_element(gui_element, primitive_context);
}

void ScriptRunner::on_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    std::scoped_lock _{ m_access_mutex };

    if (m_state == nullptr) {
        return;
    }

    m_state->on_gui_draw_element(gui_element, primitive_context);
}

void ScriptRunner::spew_error(const std::string& p) {
    OutputDebugString(p.c_str());

    if (m_console_spawned) {
        fprintf(stderr, "%s\n", p.c_str());
    }

    if (m_log_to_disk->value()) {
        spdlog::error(p);
    }

    std::unique_lock _{ m_script_error_mutex };
    m_last_script_error = p;
    m_last_script_error_time = std::chrono::system_clock::now();
}

void ScriptRunner::reset_scripts() {
    std::scoped_lock _{ m_access_mutex };

    {
        std::unique_lock _{ m_script_error_mutex };
        m_last_script_error.clear();
    }

    if (m_state != nullptr) {
        auto& mods = g_framework->get_mods()->get_mods();

        for (auto& mod : mods) {
            mod->on_lua_state_destroyed(m_state->lua());
        }

        m_state->on_script_reset();
    }

    // We need to explicitly destroy the state before we can create a new one.
    // otherwise the destructor will be called after the new state is created.
    // this is useful in FirstPerson, where we use sdk.hook.
    // if we didn't destroy the state before creating a new one
    // the FirstPerson mod would attempt to hook an already hooked function
    m_state.reset();
    m_state = std::make_unique<ScriptState>(make_gc_data());
    m_loaded_scripts.clear();
    m_known_scripts.clear();

    std::string module_path{};

    module_path.resize(1024, 0);
    module_path.resize(GetModuleFileName(nullptr, module_path.data(), module_path.size()));
    spdlog::info("[ScriptRunner] Module path {}", module_path);

    // Load from the reframework/autorun directory.
    const auto autorun_path = REFramework::get_persistent_dir() / "reframework" / "autorun";

    spdlog::info("[ScriptRunner] Creating directories {}", autorun_path.string());
    std::filesystem::create_directories(autorun_path);
    spdlog::info("[ScriptRunner] Loading scripts...");

    for (auto&& entry : std::filesystem::directory_iterator{autorun_path}) {
        auto&& path = entry.path();

        if (path.has_extension() && path.extension() == ".lua") {
            if (!m_loaded_scripts_map.contains(path.filename().string())) {
                m_loaded_scripts_map.emplace(path.filename().string(), true);
            }

            if (m_loaded_scripts_map[path.filename().string()] == true) {
                m_state->run_script(path.string());
                m_loaded_scripts.emplace_back(path.filename().string());
            }

            m_known_scripts.emplace_back(path.filename().string());
        }
    }

    std::sort(m_known_scripts.begin(), m_known_scripts.end());
    std::sort(m_loaded_scripts.begin(), m_loaded_scripts.end());
}
