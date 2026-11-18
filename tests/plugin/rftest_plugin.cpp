// rftest plugin: tests for REFramework's C plugin API (include/reframework/API.h) and its C++ wrapper (API.hpp,
// which the C# API is built on). Game-agnostic: uses only System.* and via.* types.
//
// Two suites, written like the Lua ones to reframework/data/rftest/results/<suite>.json:
//   plugin_api         documented plugin API behavior; every test passes on a healthy build.
//   plugin_regression  bugs fixed in the 2026-11 bug hunt (Questlog reframework#14); fail without the fixes.
//
// The two callback re-entrancy tests run last because, without the fix, they deadlock the game. Before running them
// the plugin waits for the Lua suites' result files (or a frame budget), writes every result so far, and pre-records
// those two tests as FAIL, so a hang still leaves a complete results file behind.
//
// Build: tests/plugin/build.bat (MSVC, C++20). Install: copy rftest_plugin.dll to reframework/plugins.

#include <windows.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <reframework/API.hpp>

using API = reframework::API;

namespace {
const REFrameworkPluginInitializeParam* g_param{};

struct Result {
    std::string name;
    std::string status;
    std::string detail;
};

struct Suite {
    std::string name;
    std::vector<Result> results;

    void record(const std::string& test, bool ok, const std::string& detail = {}) {
        set(test, ok ? "PASS" : "FAIL", detail);
    }

    void set(const std::string& test, const char* status, const std::string& detail) {
        for (auto& r : results) {
            if (r.name == test) {
                r.status = status;
                r.detail = detail;
                log(r);
                return;
            }
        }

        results.push_back({test, status, detail});
        log(results.back());
    }

    void log(const Result& r) const {
        g_param->functions->log_info("[rftest] %s: %s %s%s%s", name.c_str(), r.status.c_str(), r.name.c_str(),
            r.detail.empty() ? "" : " -- ", r.detail.c_str());
    }

    void write() const;
};

Suite g_api{"plugin_api"};
Suite g_regression{"plugin_regression"};

std::string fmt(const char* format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return buf;
}

// Calls a C API list function (out buffer, size in bytes, out count) with a growing buffer and returns the full count.
template <typename T>
unsigned int full_count(REFrameworkResult (*fn)(T*, unsigned int, unsigned int*)) {
    for (size_t n = 64; n <= (1u << 20); n *= 2) {
        std::vector<T> buf(n);
        unsigned int count = 0;

        if (fn(buf.data(), (unsigned int)(n * sizeof(T)), &count) == REFRAMEWORK_ERROR_NONE) {
            return count;
        }
    }

    return 0;
}

std::filesystem::path game_dir() {
    wchar_t buf[MAX_PATH]{};
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return std::filesystem::path{buf}.parent_path();
}

std::filesystem::path results_dir() {
    return game_dir() / "reframework" / "data" / "rftest" / "results";
}

std::string json_escape(const std::string& s) {
    std::string out;

    for (const auto c : s) {
        switch (c) {
        case '"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if ((unsigned char)c < 0x20) {
                out += fmt("\\u%04x", (unsigned char)c);
            } else {
                out += c;
            }
        }
    }

    return out;
}

void Suite::write() const {
    int pass = 0, fail = 0, skip = 0;

    for (const auto& r : results) {
        if (r.status == "PASS") ++pass;
        else if (r.status == "SKIP") ++skip;
        else ++fail;
    }

    std::error_code ec{};
    std::filesystem::create_directories(results_dir(), ec);

    const auto path = results_dir() / (name + ".json");
    const auto tmp = std::filesystem::path{path}.concat(".tmp");

    {
        std::ofstream f{tmp, std::ios::trunc};
        const auto game = g_param->version->game_name != nullptr ? g_param->version->game_name : "";
        f << "{\"suite\":\"" << json_escape(name) << "\",\"game\":\"" << json_escape(game) << "\",\"pass\":" << pass
          << ",\"fail\":" << fail << ",\"skip\":" << skip << ",\"tests\":[";

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            f << (i ? "," : "") << "{\"name\":\"" << json_escape(r.name) << "\",\"status\":\"" << r.status
              << "\",\"detail\":\"" << json_escape(r.detail) << "\"}";
        }

        f << "]}";
    }

    std::filesystem::rename(tmp, path, ec);
    g_param->functions->log_info("[rftest] %s: DONE pass=%d fail=%d skip=%d (results %s)", name.c_str(), pass, fail, skip,
        ec ? "NOT written" : "written");
}

// ----------------------------------------------------------------------------------------------------------------
// Counters for callback tests
// ----------------------------------------------------------------------------------------------------------------
std::atomic<int> g_presents{0};
std::atomic<int> g_pre_update{0};
std::atomic<int> g_post_update{0};
std::atomic<int> g_imgui_frames{0};
std::atomic<int> g_lua_created{0};

// ----------------------------------------------------------------------------------------------------------------
// plugin_api: tests that run during initialize
// ----------------------------------------------------------------------------------------------------------------
void api_init_tests() {
    auto& S = g_api;
    const auto& api = API::get();
    const auto tdb = api->tdb();

    S.record("version", g_param->version->major == REFRAMEWORK_PLUGIN_VERSION_MAJOR && g_param->version->minor >= 0,
        fmt("%d.%d.%d", g_param->version->major, g_param->version->minor, g_param->version->patch));
    S.record("game name", g_param->version->game_name != nullptr && g_param->version->game_name[0] != '\0',
        g_param->version->game_name ? g_param->version->game_name : "(null)");
    S.record("renderer data", g_param->renderer_data != nullptr &&
        (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11 || g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12),
        g_param->renderer_data ? fmt("type %d", g_param->renderer_data->renderer_type) : "null");

    S.record("tdb type count", tdb->get_num_types() > 1000, fmt("%u types", tdb->get_num_types()));

    const auto str_t = tdb->find_type("System.String");
    const auto int_t = tdb->find_type("System.Int32");
    const auto math_t = tdb->find_type("System.Math");
    const auto vec3_t = tdb->find_type("via.vec3");

    if (str_t == nullptr || int_t == nullptr || math_t == nullptr || vec3_t == nullptr) {
        S.record("find_type (System.String, System.Int32, System.Math, via.vec3)", false);
        return;
    }

    S.record("TypeDefinition names", str_t->get_full_name() == "System.String" && std::string{str_t->get_name()} == "String" &&
        std::string{str_t->get_namespace()} == "System", str_t->get_full_name());
    S.record("TypeDefinition value types", !str_t->is_valuetype() && int_t->is_valuetype() && int_t->get_valuetype_size() == 4,
        fmt("Int32 valuetype size %u", int_t->get_valuetype_size()));
    S.record("TypeDefinition parent / derivation", str_t->get_parent_type() != nullptr &&
        str_t->get_parent_type()->get_full_name() == "System.Object" && str_t->is_derived_from("System.Object"));
    S.record("tdb find_type of a missing type", tdb->find_type("rftest.NoSuchType") == nullptr);

    const auto substring = str_t->find_method("Substring(System.Int32, System.Int32)");
    if (substring == nullptr) {
        S.record("find_method with signature", false);
    } else {
        const auto params = substring->get_params();
        const auto ret = substring->get_return_type();
        S.record("Method signature", substring->get_num_params() == 2 && params.size() == 2 &&
            params[0].t != nullptr && ((API::TypeDefinition*)params[0].t)->get_full_name() == "System.Int32" &&
            ret != nullptr && ret->get_full_name() == "System.String" && !substring->is_static() &&
            substring->get_declaring_type() == str_t && substring->get_function_raw() != nullptr,
            fmt("%u params", substring->get_num_params()));
    }

    const auto methods = str_t->get_methods();
    const auto fields = vec3_t->get_fields();
    S.record("TypeDefinition get_methods / get_num_methods", methods.size() == str_t->get_num_methods() && methods.size() > 10,
        fmt("%zu methods", methods.size()));
    S.record("TypeDefinition get_fields / get_num_fields", fields.size() == vec3_t->get_num_fields() && fields.size() >= 3,
        fmt("%zu fields", fields.size()));

    const auto y = vec3_t->find_field("y");
    S.record("Field lookup and offset (via.vec3.y)", y != nullptr && y->get_offset_from_fieldptr() == 4 &&
        y->get_type() != nullptr && y->get_type()->get_full_name() == "System.Single" && !y->is_static(),
        y ? fmt("offset %u", y->get_offset_from_fieldptr()) : "missing");
}

// Runs on the game thread once the game is up (from on_pre_application_entry).
void api_runtime_tests() {
    auto& S = g_api;
    const auto& api = API::get();
    const auto tdb = api->tdb();
    const auto str_t = tdb->find_type("System.String");
    const auto math_t = tdb->find_type("System.Math");

    S.record("get_vm_context", api->get_vm_context() != nullptr);

    // Managed strings and instance invoke.
    auto s = api->create_managed_string_normal("RfTest");
    if (s == nullptr) {
        S.record("create_managed_string_normal", false);
    } else {
        s->add_ref();
        const auto td = s->get_type_definition();
        S.record("ManagedObject type / is_managed_object", td == str_t && s->is_managed_object() &&
            s->get_vm_obj_type() == REFRAMEWORK_VM_OBJ_TYPE_STRING, td ? td->get_full_name() : "null");
        const auto len = s->invoke("get_Length", {});
        S.record("Method::invoke instance (String.get_Length)", !len.exception_thrown && len.dword == 6, fmt("%u", len.dword));
        const auto upper = s->invoke("ToUpper", {});
        const auto upper_obj = (API::ManagedObject*)upper.ptr;
        S.record("Method::invoke returning an object (String.ToUpper)", !upper.exception_thrown && upper_obj != nullptr &&
            upper_obj->get_type_definition() == str_t);
        const auto rc = s->get_ref_count();
        s->add_ref();
        const auto rc2 = s->get_ref_count();
        s->release();
        S.record("ManagedObject add_ref / release", rc2 == rc + 1 && s->get_ref_count() == rc, fmt("%d -> %d", rc, rc2));
        s->release();
    }

    const auto wide = api->create_managed_string(L"wide");
    S.record("create_managed_string (wchar_t)", wide != nullptr && wide->get_type_definition() == str_t);

    // Static invoke: arguments are 8-byte slots.
    const auto max_m = math_t->find_method("Max(System.Int32, System.Int32)");
    const auto sqrt_m = math_t->find_method("Sqrt(System.Double)");
    if (max_m != nullptr) {
        std::vector<void*> args{(void*)(uintptr_t)3, (void*)(uintptr_t)7};
        const auto r = max_m->invoke(nullptr, args);
        S.record("Method::invoke static Int32 (Math.Max)", !r.exception_thrown && r.dword == 7, fmt("%u", r.dword));
    } else {
        S.record("Method::invoke static Int32 (Math.Max)", false, "method missing");
    }
    if (sqrt_m != nullptr) {
        double four = 4.0;
        void* slot{};
        memcpy(&slot, &four, sizeof(four));
        const auto r = sqrt_m->invoke(nullptr, std::vector<void*>{slot});
        S.record("Method::invoke static Double (Math.Sqrt)", !r.exception_thrown && r.d == 2.0, fmt("%f", r.d));
    } else {
        S.record("Method::invoke static Double (Math.Sqrt)", false, "method missing");
    }

    // Invoke argument-size validation (documented: in_args_size must match).
    if (max_m != nullptr) {
        void* one[1]{(void*)(uintptr_t)3};
        reframework::InvokeRet out{};
        const auto r = g_param->sdk->method->invoke(*max_m, nullptr, one, sizeof(one), &out, sizeof(out));
        S.record("invoke rejects a wrong argument count", r == REFRAMEWORK_ERROR_IN_ARGS_SIZE_MISMATCH, fmt("result %d", r));
    }

    // Types and singletons.
    const auto rt = api->typeof("System.String");
    S.record("typeof", rt != nullptr && rt->get_type_definition() != nullptr &&
        rt->get_type_definition()->get_full_name().find("RuntimeType") != std::string::npos,
        rt && rt->get_type_definition() ? rt->get_type_definition()->get_full_name() : "null");
    S.record("get_native_singleton(via.SceneManager)", api->get_native_singleton("via.SceneManager") != nullptr);
    const auto natives = api->get_native_singletons();
    const auto native_count = full_count<REFrameworkNativeSingleton>(g_param->sdk->functions->get_native_singletons);
    S.record("get_native_singletons returns every native singleton", native_count > 0 && natives.size() == native_count,
        fmt("API.hpp %zu, C API %u", natives.size(), native_count));

    const auto arr = api->create_managed_array(tdb->find_type("System.Int32"), 5);
    S.record("create_managed_array", arr != nullptr && arr->get_vm_obj_type() == REFRAMEWORK_VM_OBJ_TYPE_ARRAY &&
        arr->get_type_definition() != nullptr && arr->get_type_definition()->get_full_name() == "System.Int32[]",
        arr && arr->get_type_definition() ? arr->get_type_definition()->get_full_name() : "null");
}

// Hooks: installed from the game thread, exercised by invoking the hooked method.
std::atomic<int> g_hook_mode{0}; // 0 off, 1 observe, 2 replace return value, 3 skip original
int g_hook_argc{0};
uint32_t g_hook_a{0}, g_hook_b{0};
bool g_hook_post{false};

int hook_pre(int argc, void** argv, REFrameworkTypeDefinitionHandle*, unsigned long long) {
    const auto mode = g_hook_mode.load();
    if (mode == 0) return REFRAMEWORK_HOOK_CALL_ORIGINAL;
    g_hook_argc = argc;
    // argv[0] is the thread context; a static method's parameters follow.
    g_hook_a = (uint32_t)(uintptr_t)argv[1];
    g_hook_b = (uint32_t)(uintptr_t)argv[2];
    return mode == 3 ? REFRAMEWORK_HOOK_SKIP_ORIGINAL : REFRAMEWORK_HOOK_CALL_ORIGINAL;
}

void hook_post(void** ret_val, REFrameworkTypeDefinitionHandle, unsigned long long) {
    const auto mode = g_hook_mode.load();
    if (mode == 0) return;
    g_hook_post = true;
    if (mode == 2 || mode == 3) *ret_val = (void*)(uintptr_t)42;
}

unsigned int g_hook_id{};
API::Method* g_hook_method{};

void api_hook_install() {
    g_hook_method = API::get()->tdb()->find_type("System.Math")->find_method("Max(System.Int32, System.Int32)");
    if (g_hook_method != nullptr) {
        g_hook_id = g_hook_method->add_hook(hook_pre, hook_post, false);
    }
}

void api_hook_test() {
    auto& S = g_api;

    if (g_hook_method == nullptr) {
        S.set("add_hook: args, return value, SKIP_ORIGINAL", "SKIP", "System.Math.Max(Int32, Int32) missing");
        return;
    }

    auto run = [](int mode) {
        g_hook_mode = mode;
        g_hook_post = false;
        g_hook_a = g_hook_b = 0;
        const auto r = g_hook_method->invoke(nullptr, std::vector<void*>{(void*)(uintptr_t)3, (void*)(uintptr_t)7});
        g_hook_mode = 0;
        return r.dword;
    };

    const auto observed = run(1);
    const bool args_ok = g_hook_a == 3 && g_hook_b == 7 && g_hook_argc >= 3 && g_hook_post;
    const auto replaced = run(2);
    const auto skipped = run(3);
    S.record("add_hook: args, return value, SKIP_ORIGINAL", observed == 7 && args_ok && replaced == 42 && skipped == 42,
        fmt("observe=%u args=(%u,%u) argc=%d post=%d replace=%u skip=%u", observed, g_hook_a, g_hook_b, g_hook_argc, (int)g_hook_post,
            replaced, skipped));

    g_hook_method->remove_hook(g_hook_id);
    const auto after = run(2);
    S.record("remove_hook", after == 7, fmt("Max(3, 7) after remove = %u", after));
}

// ----------------------------------------------------------------------------------------------------------------
// plugin_regression (reframework#18, reframework#19)
// ----------------------------------------------------------------------------------------------------------------
void regression_init_tests() {
    auto& S = g_regression;
    const auto sdk = g_param->sdk;
    const auto tdb = sdk->functions->get_tdb();

    S.record("REFrameworkModule instantiated-method functions are set",
        sdk->module->get_num_instantiated_methods != nullptr && sdk->module->get_instantiated_methods != nullptr);

    if (sdk->module->get_num_instantiated_methods != nullptr && sdk->tdb->get_num_modules(tdb) > 0) {
        const auto m = sdk->tdb->get_module(tdb, 0);
        const auto n = sdk->module->get_num_instantiated_methods(m);
        const auto p = sdk->module->get_instantiated_methods(m);
        S.record("REFrameworkModule instantiated methods callable", n == 0 || p != nullptr, fmt("module 0: %u", n));
    }

    const auto str_t = sdk->tdb->find_type(tdb, "System.String");
    char buf[64];
    memset(buf, 'X', sizeof(buf));
    unsigned int len = 0xFFFFFFFF;
    auto r = sdk->type_definition->get_full_name(str_t, buf, sizeof(buf), &len);
    S.record("get_full_name writes the terminator", r == REFRAMEWORK_ERROR_NONE && len == 13 && buf[13] == '\0',
        fmt("r=%d len=%u buf[13]=%d", r, len, (int)buf[13]));

    memset(buf, 'X', sizeof(buf));
    r = sdk->type_definition->get_full_name(str_t, buf, 13, &len);
    S.record("get_full_name fails when there is no room for the terminator", r == REFRAMEWORK_ERROR_OUT_TOO_SMALL,
        fmt("r=%d (size 13 for a 13-character name)", r));

    bool fields_done = false, methods_done = false;
    const auto n_types = sdk->tdb->get_num_types(tdb);

    for (unsigned int i = 0; i < n_types && !(fields_done && methods_done); ++i) {
        const auto t = sdk->tdb->get_type(tdb, i);
        if (t == nullptr) continue;

        if (!fields_done && sdk->type_definition->get_num_fields(t) == 0) {
            REFrameworkFieldHandle fb[1]{};
            unsigned int n = 0xDEADBEEF;
            r = sdk->type_definition->get_fields(t, fb, sizeof(fb), &n);
            S.record("get_fields writes out_count 0 for a type without fields", r == REFRAMEWORK_ERROR_NONE && n == 0,
                fmt("%s: r=%d out_count=0x%X", sdk->type_definition->get_name(t), r, n));
            fields_done = true;
        }

        if (!methods_done && sdk->type_definition->get_num_methods(t) == 0) {
            REFrameworkMethodHandle mb[1]{};
            unsigned int n = 0xDEADBEEF;
            r = sdk->type_definition->get_methods(t, mb, sizeof(mb), &n);
            S.record("get_methods writes out_count 0 for a type without methods", r == REFRAMEWORK_ERROR_NONE && n == 0,
                fmt("%s: r=%d out_count=0x%X", sdk->type_definition->get_name(t), r, n));
            methods_done = true;
        }
    }

    const auto get_type_m = sdk->type_definition->find_method(sdk->tdb->find_type(tdb, "System.Object"), "GetType");
    if (get_type_m != nullptr) {
        REFrameworkMethodParameter pb[1]{};
        unsigned int n = 0xDEADBEEF;
        r = sdk->method->get_params(get_type_m, pb, sizeof(pb), &n);
        S.record("get_params writes out_count 0 for a method without parameters", r == REFRAMEWORK_ERROR_NONE && n == 0,
            fmt("System.Object.GetType: r=%d out_count=0x%X", r, n));
    }
}

const char* const REENTRANT_PRESENT = "registering a callback from inside on_present does not deadlock";
const char* const REENTRANT_LUA_CREATED = "on_lua_state_created callback may register callbacks";

std::atomic<bool> g_reentrant_imgui_fired{false};
std::atomic<bool> g_lua_created_cb_done{false};

void on_imgui_frame_late(REFImGuiFrameCbData*) { g_reentrant_imgui_fired = true; }
void on_device_reset_noop() {}

void on_lua_created_registers(lua_State*) {
    // Registering from inside the callback: without the fix the caller still holds the callback lock here.
    g_param->functions->on_device_reset(on_device_reset_noop);
    g_lua_created_cb_done = true;
}

// ----------------------------------------------------------------------------------------------------------------
// Callbacks and the test schedule
// ----------------------------------------------------------------------------------------------------------------
enum class Stage { WaitGame, Runtime, WaitLua, LuaCreated, Present, Finish, Done };
std::atomic<Stage> g_stage{Stage::WaitGame};
std::atomic<int> g_stage_frame{0};
std::atomic<bool> g_runtime_requested{false};
std::atomic<bool> g_hooks_tested{false};

void on_lua_state_created(lua_State*) { ++g_lua_created; }
void on_imgui_frame(REFImGuiFrameCbData*) { ++g_imgui_frames; }

void on_pre_update() {
    ++g_pre_update;

    if (g_runtime_requested) {
        g_runtime_requested = false;
        api_runtime_tests();
        api_hook_install();
    }
}

void on_post_update() {
    ++g_post_update;

    // add_hook installs immediately; exercise it from the game thread a few frames after installing.
    if (!g_hooks_tested && g_stage.load() == Stage::WaitLua && g_stage_frame > 5) {
        g_hooks_tested = true;
        api_hook_test();
    }
}

bool lua_suites_done() {
    std::error_code ec{};
    return std::filesystem::exists(results_dir() / "api.json", ec) && std::filesystem::exists(results_dir() / "regression.json", ec);
}

void on_present() {
    const auto frame = ++g_presents;
    ++g_stage_frame;

    switch (g_stage.load()) {
    case Stage::WaitGame:
        if (frame >= 30) {
            g_runtime_requested = true;
            g_stage = Stage::Runtime;
            g_stage_frame = 0;
        }
        break;

    case Stage::Runtime:
        if (!g_runtime_requested) {
            g_stage = Stage::WaitLua;
            g_stage_frame = 0;
        }
        break;

    case Stage::WaitLua:
        // Wait for the Lua suites (so a hang below cannot cost their results), at most ~3000 frames.
        if (g_hooks_tested && (lua_suites_done() || g_stage_frame > 3000)) {
            auto& S = g_api;
            // Late, not in api_runtime_tests: by now the game has created most of its managed singletons, which is
            // when a fixed-size buffer in the wrapper runs out.
            const auto managed = API::get()->get_managed_singletons();
            const auto managed_count = full_count<REFrameworkManagedSingleton>(g_param->sdk->functions->get_managed_singletons);
            S.record("get_managed_singletons returns every managed singleton", managed_count > 0 && managed.size() == managed_count,
                fmt("API.hpp %zu, C API %u", managed.size(), managed_count));
            S.record("on_present fires", g_presents > 0, fmt("%d", g_presents.load()));
            S.record("on_pre_application_entry(UpdateBehavior) fires", g_pre_update > 0, fmt("%d", g_pre_update.load()));
            S.record("on_post_application_entry(UpdateBehavior) fires", g_post_update > 0, fmt("%d", g_post_update.load()));
            S.record("on_lua_state_created fires", g_lua_created > 0, fmt("%d", g_lua_created.load()));
            S.record("on_imgui_frame fires", g_imgui_frames > 0, fmt("%d", g_imgui_frames.load()));
            g_param->functions->lock_lua();
            g_param->functions->unlock_lua();
            S.record("lock_lua / unlock_lua", true);
            S.write();

            g_regression.set(REENTRANT_LUA_CREATED, "FAIL", "pending: did not finish (deadlock?)");
            g_regression.set(REENTRANT_PRESENT, "FAIL", "pending: did not finish (deadlock?)");
            g_regression.write();

            // Registering on_lua_state_created after the state exists calls the callback immediately; it then
            // registers another callback. Run on a separate thread: without the fix it deadlocks holding the lock.
            CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
                g_param->functions->on_lua_state_created(on_lua_created_registers);
                return 0;
            }, nullptr, 0, nullptr);
            g_stage = Stage::LuaCreated;
            g_stage_frame = 0;
        }
        break;

    case Stage::LuaCreated:
        if (g_lua_created_cb_done || g_stage_frame > 120) {
            g_regression.record(REENTRANT_LUA_CREATED, g_lua_created_cb_done, g_lua_created_cb_done ? "" : "callback did not run");
            g_regression.write();
            g_stage = Stage::Present;
            g_stage_frame = 0;
        }
        break;

    case Stage::Present: {
        // Inside the on_present dispatch: without the fix this blocks the render thread forever.
        const auto ok = g_param->functions->on_imgui_frame(on_imgui_frame_late);
        g_stage = ok ? Stage::Finish : Stage::Done;
        g_stage_frame = 0;
        if (!ok) {
            g_regression.record(REENTRANT_PRESENT, false, "on_imgui_frame returned false");
            g_regression.write();
        }
        break;
    }

    case Stage::Finish:
        if (g_reentrant_imgui_fired || g_stage_frame > 300) {
            g_regression.record(REENTRANT_PRESENT, g_reentrant_imgui_fired,
                g_reentrant_imgui_fired ? "" : "registered, but the callback never ran");
            g_regression.write();
            g_stage = Stage::Done;
        }
        break;

    case Stage::Done:
        break;
    }
}
} // namespace

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    g_param = param;
    API::initialize(param);

    std::error_code ec{};
    std::filesystem::remove(results_dir() / "plugin_api.json", ec);
    std::filesystem::remove(results_dir() / "plugin_regression.json", ec);

    g_param->functions->log_info("[rftest] plugin: starting");
    api_init_tests();
    regression_init_tests();

    const auto f = param->functions;
    g_api.record("register callbacks", f->on_present(on_present) && f->on_lua_state_created(on_lua_state_created) &&
        f->on_pre_application_entry("UpdateBehavior", on_pre_update) && f->on_post_application_entry("UpdateBehavior", on_post_update) &&
        f->on_imgui_frame(on_imgui_frame));
    g_api.record("on_pre_application_entry rejects an empty name", !f->on_pre_application_entry("", on_pre_update));
    return true;
}
