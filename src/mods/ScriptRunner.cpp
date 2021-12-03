#include <cstdint>
#include <filesystem>

#include <imgui.h>

#include "../sdk/REContext.hpp"
#include "../sdk/REManagedObject.hpp"
#include "../sdk/RETypeDB.hpp"
#include "../sdk/SceneManager.hpp"

#include "utility/String.hpp"

#include "Mods.hpp"

#include "bindings/Sdk.hpp"
#include "bindings/ImGui.hpp"

#include "ScriptRunner.hpp"

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
    spdlog::debug(str);
}
}

ScriptState::ScriptState() {
    std::scoped_lock _{ m_execution_mutex };

    m_lua.registry()["state"] = this;
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32, sol::lib::utf8);
    bindings::open_sdk(this);
    bindings::open_imgui(this);

    auto re = m_lua.create_table();
    re["msg"] = api::re::msg;
    re["on_pre_application_entry"] = [this](const char* name, sol::function fn) { m_pre_application_entry_fns.emplace(name, fn); };
    re["on_application_entry"] = [this](const char* name, sol::function fn) { m_application_entry_fns.emplace(name, fn); };
    re["on_update_transform"] = [this](RETransform* transform, sol::function fn) { m_update_transform_fns.emplace(transform, fn); };
    re["on_pre_update_transform"] = [this](RETransform* transform, sol::function fn) { m_pre_update_transform_fns.emplace(transform, fn); };
    re["on_pre_gui_draw_element"] = [this](sol::function fn) { m_pre_gui_draw_element_fns.emplace_back(fn); };
    re["on_gui_draw_element"] = [this](sol::function fn) { m_gui_draw_element_fns.emplace_back(fn); };
    re["on_draw_ui"] = [this](sol::function fn) { m_on_draw_ui_fns.emplace_back(fn); };
    re["on_frame"] = [this](sol::function fn) { m_on_frame_fns.emplace_back(fn); };
    re["on_script_reset"] = [this](sol::function fn) { m_on_script_reset_fns.emplace_back(fn); };
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
        "x", &Vector3f::x,
        "y", &Vector3f::y,
        "z", &Vector3f::z,
        "dot", [](Vector3f& v1, Vector3f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector3f& v1, Vector3f& v2) { return glm::cross(v1, v2); },
        "length", [](Vector3f& v) { return glm::length(v); },
        "normalize", [](Vector3f& v) { v = glm::normalize(v); },
        "normalized", [](Vector3f& v) { return glm::normalize(v); },
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
        "x", &Vector4f::x,
        "y", &Vector4f::y,
        "z", &Vector4f::z,
        "w", &Vector4f::w,
        "dot", [](Vector4f& v1, Vector4f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector4f& v1, Vector4f& v2) { return glm::cross(Vector3f{v1.x, v1.y, v1.z}, Vector3f{v2.x, v2.y, v2.z}); },
        "length", [](Vector4f& v) { return glm::length(v); },
        "normalize", [](Vector4f& v) { v = glm::normalize(v); },
        "normalized", [](Vector4f& v) { return glm::normalize(v); },
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
        "to_quat", [] (Matrix4x4f& m) { return glm::quat(m); },
        "inverse", [] (Matrix4x4f& m) { return glm::inverse(m); },
        "invert", [] (Matrix4x4f& m) { m = glm::inverse(m); },
        sol::meta_function::multiplication, sol::overload(
            [](Matrix4x4f& lhs, Matrix4x4f& rhs) {
                return lhs * rhs;
            },
            [](Matrix4x4f& lhs, Vector4f& rhs) {

                return lhs * rhs;
            }
        ),
        sol::meta_function::index, [](Matrix4x4f& lhs, int index) -> Vector4f& {
            return lhs[index];
        },
        sol::meta_function::new_index, [](Matrix4x4f& lhs, int index, Vector4f& rhs) {
            lhs[index] = rhs;
        }
    );

    // add glm::quat usertype
    m_lua.new_usertype<glm::quat>("Quaternion",
        sol::meta_function::construct, sol::constructors<glm::quat(), glm::quat(float, float, float, float), glm::quat(const Vector3f&)>(),
        "x", &glm::quat::x,
        "y", &glm::quat::y,
        "z", &glm::quat::z,
        "w", &glm::quat::w,
        "to_mat4", [](glm::quat& q) { return Matrix4x4f{q}; },
        "inverse", [](glm::quat& q) { return glm::inverse(q); },
        "invert", [](glm::quat& q) { q = glm::inverse(q); },
        "normalize", [](glm::quat& q) { q = glm::normalize(q); },
        "normalized", [](glm::quat& q) { return glm::normalize(q); },
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
        sol::meta_function::index, [](glm::quat& lhs, int index) -> float& {
            return lhs[index];
        },
        sol::meta_function::new_index, [](glm::quat& lhs, int index, float rhs) {
            lhs[index] = rhs;
        }
    );


    m_lua.new_usertype<REFramework>("REFramework",
        "is_drawing_ui", &REFramework::is_drawing_ui,
        "get_game_name", &REFramework::get_game_name
    );

    m_lua["reframework"] = g_framework.get();

    // clang-format on

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->add_lua_bindings(m_lua);
    }
}

ScriptState::~ScriptState() {
    std::scoped_lock _{m_execution_mutex};
    m_hooked_fns.clear();
}

void ScriptState::run_script(const std::string& p) {
    std::scoped_lock _{ m_execution_mutex };

    spdlog::info("[ScriptState] Running script {}...", p);

    std::string old_path = m_lua["package"]["path"];

    try {
        auto path = std::filesystem::path(p);
        auto dir = path.parent_path();

        m_lua["package"]["path"] = old_path + ";" + dir.string() + "/?.lua";
        m_lua.safe_script_file(p);
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
        api::re::msg(e.what());
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
        OutputDebugString(e.what());
    }
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
        OutputDebugString(e.what());
    }
}

void ScriptState::on_pre_application_entry(const char* name) {
    try {
        auto range = m_pre_application_entry_fns.equal_range(name);

        if (range.first != range.second) {
            std::scoped_lock _{ m_execution_mutex };

            for (auto it = range.first; it != range.second; ++it) {
                handle_protected_result(it->second());
            }
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_application_entry(const char* name) {
    try {
        auto range = m_application_entry_fns.equal_range(name);

        if (range.first != range.second) {
            std::scoped_lock _{ m_execution_mutex };

            for (auto it = range.first; it != range.second; ++it) {
                handle_protected_result(it->second());
            }
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_pre_update_transform(RETransform* transform) {
    try {
        auto range = m_pre_update_transform_fns.equal_range(transform);

        if (range.first != range.second) {
            std::scoped_lock _{ m_execution_mutex };

            for (auto it = range.first; it != range.second; ++it) {
                handle_protected_result(it->second(transform));
            }
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_update_transform(RETransform* transform) {
    try {
        auto range = m_update_transform_fns.equal_range(transform);

        if (range.first != range.second) {
            std::scoped_lock _{ m_execution_mutex };

            for (auto it = range.first; it != range.second; ++it) {
                handle_protected_result(it->second(transform));
            }
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
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
        OutputDebugString(e.what());
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
        OutputDebugString(e.what());
    }
}

void ScriptState::on_script_reset() {
    try {
        std::scoped_lock _{ m_execution_mutex };

        for (auto& fn : m_on_script_reset_fns) {
            handle_protected_result(fn());
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

ScriptState::PreHookResult ScriptState::on_pre_hook(HookedFn* fn) {
    std::scoped_lock _{ m_execution_mutex };
    auto result = PreHookResult::CALL_ORIGINAL;

    try {
        // Call the script function.
        // TODO: Take return value from the script function into account.

        // Convert the args to a table that we pass to the script function.
        for (auto i = 0u; i < fn->args.size(); ++i) {
            fn->script_args[i + 1] = (void*)fn->args[i];
        }

        for (auto&& pre_fn : fn->script_pre_fns) {
            auto script_result = handle_protected_result(pre_fn(fn->script_args)).get<sol::object>();

            if (script_result.is<PreHookResult>()) {
                result = script_result.as<PreHookResult>();
            }
        }

        // Apply the changes to arguments that the script function may have made.
        for (auto i = 0u; i < fn->args.size(); ++i) {
            auto arg = fn->script_args[i + 1];
            fn->args[i] = (uintptr_t)arg.get<void*>();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }

    return result;
}

void ScriptState::on_post_hook(HookedFn* fn) {
    std::scoped_lock _{ m_execution_mutex };

    try {
        for (auto&& post_fn : fn->script_post_fns) {
            fn->ret_val = (uintptr_t)handle_protected_result(post_fn((void*)fn->ret_val)).get<void*>();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

std::optional<std::string> ScriptRunner::on_initialize() {
    // Calling reset_scripts even though the scripts have never been set yet still works.
    reset_scripts();

    return Mod::on_initialize();
}

void ScriptRunner::on_frame() {
    std::scoped_lock _{m_access_mutex};
    m_state->on_frame();
}

void ScriptRunner::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

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
            }
        }

        if (ImGui::Button("Reset scripts")) {
            reset_scripts();
        }
    }

    { 
        std::scoped_lock _{ m_access_mutex };
        m_state->on_draw_ui();
    }
}

void ScriptRunner::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{ m_access_mutex };

    m_state->on_pre_application_entry(name);
}

void ScriptRunner::on_application_entry(void* entry, const char* name, size_t hash) {
    std::scoped_lock _{ m_access_mutex };

    m_state->on_application_entry(name);
}

void ScriptRunner::on_pre_update_transform(RETransform* transform) {
    std::scoped_lock _{ m_access_mutex };

    m_state->on_pre_update_transform(transform);
}

void ScriptRunner::on_update_transform(RETransform* transform) {
    std::scoped_lock _{ m_access_mutex };

    m_state->on_update_transform(transform);
}
bool ScriptRunner::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    std::scoped_lock _{ m_access_mutex };

    return m_state->on_pre_gui_draw_element(gui_element, primitive_context);
}

void ScriptRunner::on_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    std::scoped_lock _{ m_access_mutex };

    m_state->on_gui_draw_element(gui_element, primitive_context);
}

void ScriptRunner::reset_scripts() {
    std::scoped_lock _{ m_access_mutex };

    if (m_state != nullptr) {
        m_state->on_script_reset();
    }

    m_state = std::make_unique<ScriptState>();

    std::string module_path{};

    module_path.resize(1024, 0);
    module_path.resize(GetModuleFileName(nullptr, module_path.data(), module_path.size()));
    spdlog::info("[ScriptRunner] Module path {}", module_path);

    auto autorun_path = std::filesystem::path{module_path}.parent_path() / "autorun";

    spdlog::info("[ScriptRunner] Creating directories {}", autorun_path.string());
    std::filesystem::create_directories(autorun_path);
    spdlog::info("[ScriptRunner] Loading scripts...");

    for (auto&& entry : std::filesystem::directory_iterator{autorun_path}) {
        auto&& path = entry.path();

        if (path.has_extension() && path.extension() == ".lua") {
            m_state->run_script(path.string());
        }
    }
}
