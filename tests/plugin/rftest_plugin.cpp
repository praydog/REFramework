// rftest plugin: tests for REFramework's C plugin API (include/reframework/API.h) and its C++ wrapper (API.hpp,
// which the C# API is built on). Game-agnostic: uses only System.* and via.* types.
//
// Two suites, written like the Lua ones to reframework/data/rftest/results/<suite>.json:
//   plugin_api         documented plugin API behavior; every test passes on a healthy build.
//   plugin_regression  bugs fixed in the 2026-11 bug hunt; fail without the fixes.
//
// plugin_regression also runs Lua code in a Lua state the plugin creates (create_script_state), through the copy of
// dependencies/lua that build.bat compiles into the plugin.
//
// The two callback re-entrancy tests run late because, without the fix, they deadlock the game. Before running them
// the plugin waits for the Lua suites' result files (or a frame budget), writes every result so far, and pre-records
// those two tests as FAIL, so a hang still leaves a complete results file behind. The two-state test (a plugin's Lua
// state calling on its own thread while the main state calls) runs after them and is pre-recorded with them.
//
// It also registers functions in REFramework's Lua state for the Lua regression suite: rftest_peek(address, size), a
// guarded raw memory read (nil instead of a crash when the memory isn't readable), rftest_poke(address, value, size),
// its guarded write, and the rftest_two_states_* functions the suite's helper for the two-state test calls.
//
// Build: tests/plugin/build.bat (MSVC, C++20; links dependencies/lua). Install: copy rftest_plugin.dll to
// reframework/plugins.

#include <windows.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include <reframework/API.hpp>

#include <lua.hpp>

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

    // Replaces an earlier entry only if it was pre-recorded as pending (the deadlock tests record their FAIL first);
    // any other repeat of a name is a FAIL of its own, so a PASS can never hide a FAIL.
    void set(const std::string& test, const char* status, const std::string& detail) {
        for (auto& r : results) {
            if (r.name == test) {
                if (r.detail.rfind("pending:", 0) == 0) {
                    r.status = status;
                    r.detail = detail;
                    log(r);
                } else {
                    results.push_back({test + " (duplicate name)", "FAIL", std::string{"recorded twice; second: "} + status + " " + detail});
                    log(results.back());
                }
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
    } else {
        S.record("invoke rejects a wrong argument count", false, "System.Math.Max(Int32, Int32) missing");
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

// A hook installed from reframework_plugin_initialize (REFramework's init thread, before the game thread runs any
// script) on a method with by-value float parameters and a float return. Its argument layout must not depend on
// what the managed runtime can answer at that point: the args must hold the floats.
// Control: it also passes on the build before the hook layout stopped asking the runtime (no red run shown); it
// guards the init-thread case, which the other hook tests don't reach.
std::atomic<bool> g_fhook_observe{false};
float g_fhook_a{0.0f}, g_fhook_b{0.0f};
unsigned int g_fhook_id{};
API::Method* g_fhook_method{};

int fhook_pre(int argc, void** argv, REFrameworkTypeDefinitionHandle*, unsigned long long) {
    if (g_fhook_observe && argc >= 3) {
        // argv[0] is the thread context; by-value floats are saved from their XMM registers.
        memcpy(&g_fhook_a, &argv[1], sizeof(float));
        memcpy(&g_fhook_b, &argv[2], sizeof(float));
    }
    return REFRAMEWORK_HOOK_CALL_ORIGINAL;
}

void fhook_post(void**, REFrameworkTypeDefinitionHandle, unsigned long long) {}

void api_init_hook_install() {
    const auto math = API::get()->tdb()->find_type("System.Math");
    g_fhook_method = math != nullptr ? math->find_method("Max(System.Single, System.Single)") : nullptr;
    if (g_fhook_method != nullptr) {
        g_fhook_id = g_fhook_method->add_hook(fhook_pre, fhook_post, false);
    }
}

void api_init_hook_test() {
    auto& S = g_api;
    constexpr auto name = "add_hook from plugin initialize: by-value Single args (Math.Max)";

    if (g_fhook_method == nullptr) {
        S.set(name, "SKIP", "System.Math.Max(Single, Single) missing");
        return;
    }

    // Single parameters are passed to invoke() as doubles.
    const double a = 1.5, b = -2.5;
    void* slots[2]{};
    memcpy(&slots[0], &a, sizeof(a));
    memcpy(&slots[1], &b, sizeof(b));

    g_fhook_a = g_fhook_b = 0.0f;
    g_fhook_observe = true;
    const auto r = g_fhook_method->invoke(nullptr, std::vector<void*>{slots[0], slots[1]});
    g_fhook_observe = false;
    g_fhook_method->remove_hook(g_fhook_id);

    // invoke() returns a Single widened to a double (seen: r.f = 0, r.d = 1.5).
    S.record(name, !r.exception_thrown && r.d == 1.5 && g_fhook_a == 1.5f && g_fhook_b == -2.5f,
        fmt("Max = %f, hook saw (%f, %f)", r.d, g_fhook_a, g_fhook_b));
}

// ----------------------------------------------------------------------------------------------------------------
// plugin_regression: out-parameters and terminators of the type database functions, and callback re-entrancy.
// ----------------------------------------------------------------------------------------------------------------
void regression_init_tests() {
    auto& S = g_regression;
    const auto sdk = g_param->sdk;
    const auto tdb = sdk->functions->get_tdb();

    S.record("REFrameworkModule instantiated-method functions are set",
        sdk->module->get_num_instantiated_methods != nullptr && sdk->module->get_instantiated_methods != nullptr);

    // A missing or failing case records a FAIL; a test must never silently disappear from the results.
    constexpr auto callable_name = "REFrameworkModule instantiated methods callable";
    if (sdk->module->get_num_instantiated_methods == nullptr || sdk->module->get_instantiated_methods == nullptr) {
        S.record(callable_name, false, "functions not set");
    } else {
        bool found = false;
        for (unsigned int i = 0; i < sdk->tdb->get_num_modules(tdb) && !found; ++i) {
            const auto m = sdk->tdb->get_module(tdb, i);
            const auto n = sdk->module->get_num_instantiated_methods(m);
            if (n == 0) continue;
            const auto p = sdk->module->get_instantiated_methods(m);
            S.record(callable_name, p != nullptr, fmt("module %u: %u methods", i, n));
            found = true;
        }
        // REModule::get_instantiated_methods() has no data from TDB 81 on (it logs "This TDB version does not support
        // ..."), so every module reports 0 there (DD2, MH Wilds): the game can't express the test. Before TDB 81 a
        // zero for every module would be a bug; the run's skip list shows which games skipped.
        if (!found) S.set(callable_name, "SKIP", "no module reports instantiated methods (no instantiation data in TDB 81+)");
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

    if (!fields_done) S.record("get_fields writes out_count 0 for a type without fields", false, "no type without fields");
    if (!methods_done) S.record("get_methods writes out_count 0 for a type without methods", false, "no type without methods");

    const auto get_type_m = sdk->type_definition->find_method(sdk->tdb->find_type(tdb, "System.Object"), "GetType");
    if (get_type_m != nullptr) {
        REFrameworkMethodParameter pb[1]{};
        unsigned int n = 0xDEADBEEF;
        r = sdk->method->get_params(get_type_m, pb, sizeof(pb), &n);
        S.record("get_params writes out_count 0 for a method without parameters", r == REFRAMEWORK_ERROR_NONE && n == 0,
            fmt("System.Object.GetType: r=%d out_count=0x%X", r, n));
    } else {
        S.record("get_params writes out_count 0 for a method without parameters", false, "System.Object.GetType missing");
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
// plugin_regression: a Lua state the plugin creates (create_script_state / delete_script_state)
// ----------------------------------------------------------------------------------------------------------------
// REFramework keeps its bookkeeping for engine objects (the wrapper cache, the engine references Lua holds, the
// tracker that releases them once their wrappers are gone) in each Lua state's registry. A state a plugin creates
// must get its own, keep what its scripts add_ref() like the main state does, and give its references back when the
// plugin deletes it.
//
// The state, L2, creates an array and keeps it with add_ref() the way the Lua regression test "an array kept with
// add_ref() stays intact across frames" does: stored into a table a full collection made old, then two forced steps.
// For MS_FRAMES frames L2 then collects every frame and checks the array's size and elements, and that pushing its
// address again gives the wrapper it holds. The plugin takes its own engine reference before the forced steps, so the
// array outlives L2 (a build whose collector finalizes the held wrapper fails on the count instead of freeing the
// array under L2) and its count can be read throughout: the count must not move while L2 holds the array, and must
// drop by exactly one (L2's reference) once ScriptRunner has processed the deletion, which it does in its next on_frame.
//
// Runs on the game thread (UpdateBehavior) while the main state's suites run. Every call into L2 is under lock_lua,
// which also keeps REFramework's per-frame collection of L2 out. A script reset destroys plugin-created states; the
// runner never resets scripts.
const char* const MULTI_STATE = "a plugin-created Lua state keeps add_ref'd objects and releases them when deleted";
constexpr int MS_FRAMES = 30;         // frames L2 holds the array, collecting every frame
constexpr int MS_DELETE_FRAMES = 300; // budget for the deletion to be processed
constexpr int MS_SETTLE_FRAMES = 10;  // frames after the count first moves before the final read
constexpr int MS_START_FRAMES = 600;  // budget for the main state to exist (the first script reset destroys states
                                      // created before it)

const char* const MS_SETUP = R"lua(
local M = { held = {} }
rftest_ms = M
-- In a separate function, so no stale stack slot keeps the array reachable during the collections.
local function make()
    local arr = sdk.create_managed_array("System.String", 4)
    if arr == nil then error("sdk.create_managed_array returned nil") end
    for i = 0, 3 do arr[i] = sdk.create_managed_string("rftest_ms_" .. i) end
    M.held[1] = arr:add_ref()
    M.elements = {}
    for i = 0, 3 do M.elements[i] = arr:get_element(i):get_address() end
    M.addr = arr:get_address()
end
collectgarbage("collect")
make()

-- Called once the plugin holds its own reference: two minor collections right after the store.
function M.settle()
    collectgarbage("step")
    collectgarbage("step")
end

-- Returns nil if the held array is intact, else what is wrong with it.
function M.check()
    collectgarbage("collect")
    for _ = 1, 4 do collectgarbage("step") end
    local arr = M.held[1]
    if arr == nil then return "held[1] is nil" end
    local ok, n = pcall(arr.get_size, arr)
    if not (ok and n == 4) then return "size is " .. tostring(n) end
    for i = 0, 3 do
        local okp, p = pcall(function() return arr:get_element(i):get_address() end)
        if not (okp and p == M.elements[i]) then
            return string.format("element %d is %s, was %x", i, okp and string.format("%x", p) or tostring(p), M.elements[i])
        end
    end
    if not rawequal(sdk.to_managed_object(M.addr), arr) then
        return "sdk.to_managed_object(address) gave a different wrapper than the one held"
    end
    return nil
end

return M.addr
)lua";

struct MultiState {
    int step{0};  // 0 waiting for the main state, 1 L2 holds the array, 2 L2 deleted, 3 count moved, 4 done
    int frame{0}; // frames in the current step
    lua_State* L{};
    API::ManagedObject* arr{};
    int rc_before{0}; // the array's engine reference count before the plugin's add_ref
    int rc_held{0};   // after it, while L2 holds the array
    int rc_moved{0};  // the first different count after delete_script_state
    int delete_frames{0};
};

MultiState g_ms{};
std::atomic<bool> g_ms_done{false};

std::string pop_lua_error(lua_State* L) {
    const auto msg = lua_tostring(L, -1);
    std::string out = msg != nullptr ? msg : "(error object is not a string)";
    lua_pop(L, 1);
    return out;
}

// Runs MS_SETUP in L. Returns an error message, or "" with the array's address in addr.
std::string ms_setup(lua_State* L, uintptr_t& addr) {
    if (luaL_loadstring(L, MS_SETUP) != LUA_OK || lua_pcall(L, 0, 1, 0) != LUA_OK) {
        return pop_lua_error(L);
    }

    int is_num = 0;
    addr = (uintptr_t)lua_tointegerx(L, -1, &is_num);
    lua_pop(L, 1);
    return is_num ? "" : "the setup chunk returned no address";
}

// Calls rftest_ms[name]() in L, which returns nil when all is well. Returns "", or what it returned or raised.
std::string ms_call(lua_State* L, const char* name) {
    if (lua_getglobal(L, "rftest_ms") != LUA_TTABLE) {
        lua_pop(L, 1);
        return "rftest_ms is gone";
    }

    lua_getfield(L, -1, name);
    lua_remove(L, -2);

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        return pop_lua_error(L);
    }

    const auto msg = lua_tostring(L, -1);
    std::string out = lua_isnil(L, -1) ? "" : msg != nullptr ? msg : fmt("%s() returned a non-string", name);
    lua_pop(L, 1);
    return out;
}

void ms_finish(bool ok, const std::string& detail) {
    auto& m = g_ms;

    if (m.L != nullptr) {
        g_param->functions->delete_script_state(m.L);
        m.L = nullptr;
    }

    if (m.arr != nullptr) {
        m.arr->release();
        m.arr = nullptr;
    }

    g_regression.record(MULTI_STATE, ok, detail);
    m.step = 4;
    g_ms_done = true;
}

// Called once per frame from on_pre_update until g_ms_done.
void multistate_step() {
    auto& m = g_ms;
    const auto fns = g_param->functions;
    ++m.frame;

    switch (m.step) {
    case 0: {
        if (g_lua_created == 0) {
            if (m.frame > MS_START_FRAMES) {
                ms_finish(false, fmt("no main Lua state after %d frames", MS_START_FRAMES));
            }
            return;
        }

        m.L = fns->create_script_state();
        if (m.L == nullptr) {
            ms_finish(false, "create_script_state returned null");
            return;
        }

        // The plugin's reference is taken under the same lock as the setup, before any collection of L2.
        uintptr_t addr = 0;
        int rc_settled = 0;
        fns->lock_lua();
        auto err = ms_setup(m.L, addr);
        if (err.empty() && !g_param->sdk->managed_object->is_managed_object((void*)addr)) {
            err = fmt("address 0x%llx from L2 is not a managed object", (unsigned long long)addr);
        }
        if (err.empty()) {
            m.arr = (API::ManagedObject*)addr;
            m.rc_before = m.arr->get_ref_count();
            m.arr->add_ref();
            m.rc_held = m.arr->get_ref_count();
            err = ms_call(m.L, "settle");
            rc_settled = m.arr->get_ref_count();
        }
        fns->unlock_lua();

        if (!err.empty()) {
            ms_finish(false, "setup in L2: " + err);
        } else if (m.rc_held != m.rc_before + 1) {
            ms_finish(false, fmt("the plugin's add_ref moved the engine reference count %d -> %d", m.rc_before, m.rc_held));
        } else if (rc_settled != m.rc_held) {
            ms_finish(false, fmt("engine refs %d after two collection steps in L2, expected %d (L2 released the array it holds)",
                rc_settled, m.rc_held));
        } else {
            m.step = 1;
            m.frame = 0;
        }
        break;
    }

    case 1: {
        fns->lock_lua();
        const auto err = ms_call(m.L, "check");
        fns->unlock_lua();
        const auto rc = m.arr->get_ref_count();

        if (!err.empty()) {
            ms_finish(false, fmt("frame %d, in L2: %s (engine refs %d, expected %d)", m.frame, err.c_str(), rc, m.rc_held));
        } else if (rc != m.rc_held) {
            ms_finish(false, fmt("frame %d: engine refs %d while L2 holds the array, expected %d (L2 released it)", m.frame, rc,
                m.rc_held));
        } else if (m.frame >= MS_FRAMES) {
            fns->delete_script_state(m.L);
            m.L = nullptr;
            m.step = 2;
            m.frame = 0;
        }
        break;
    }

    case 2: {
        const auto rc = m.arr->get_ref_count();

        if (rc != m.rc_held) {
            m.rc_moved = rc;
            m.delete_frames = m.frame;
            m.step = 3;
            m.frame = 0;
        } else if (m.frame > MS_DELETE_FRAMES) {
            ms_finish(false, fmt("engine refs still %d %d frames after delete_script_state: the deleted state kept its reference",
                rc, MS_DELETE_FRAMES));
        }
        break;
    }

    case 3:
        if (m.frame >= MS_SETTLE_FRAMES) {
            const auto rc = m.arr->get_ref_count();
            ms_finish(rc == m.rc_held - 1,
                fmt("engine refs %d before the plugin's add_ref, %d while L2 held the array (%d frames, collected every frame), "
                    "%d after delete_script_state (expected %d; first moved to %d after %d frames)",
                    m.rc_before, m.rc_held, MS_FRAMES, rc, m.rc_held - 1, m.rc_moved, m.delete_frames));
        }
        break;
    }
}

// ----------------------------------------------------------------------------------------------------------------
// plugin_regression: a plugin's Lua state calling on the plugin's own thread while the main state calls
// ----------------------------------------------------------------------------------------------------------------
// A hook callback of a Lua state the plugin creates runs on whatever thread calls the hooked method, under that
// state's own lock only, so its Lua can run on the plugin's thread while the main state's Lua runs on another. The
// buffers a Lua call keeps its arguments in (build_args' nesting depth and per-level argument lists) were one set per
// process, not per thread: a probe with both states calling at once got wrong sums or a crash in 11 of 11 runs (DMC5,
// PRAGMATA; 4 of the 9 crash stacks in build_args growing the shared lists). This test makes one interleaving of the
// two threads happen in order, each thread building its arguments only while the other waits, so an unfixed build
// fails on a value instead of racing:
//   1. the plugin's thread, in L3's hook on a rarely called method T, calls X with (100, 200, 300) / 4 and waits in
//      it (the plugin's own hook on X waits) until 2. has happened;
//   2. the main state, on its thread (the regression suite's helper for this test), calls X with (10, 20, 30) / 2 and
//      waits in it until 3. has happened;
//   3. the plugin's thread finishes its first call and calls X with (1000, 2000, 3000) / 4.
// X is via.vec3.op_Division(via.vec3, System.Single): it reads its vector through the address the call passed, after
// the hook on it returned. Neither suite hooks X or T in the main state (a main-state hook on a method the plugin's
// thread calls would wait for the main state's lock, which the waiting main-state thread holds). With one set of
// buffers the third call took the nesting level the main state's call was using and wrote its vector over that call's
// vector, so the main state got (1000, 2000, 3000) / 2. Each wait gives up after TS_WAIT_MS and the detail says so.
// Runs after the callback re-entrancy tests, pre-recorded as FAIL with them. L3 is deleted afterwards (which removes
// its hook on T) and the plugin's hook on X is removed.
const char* const TWO_STATES = "a plugin's Lua state calling on its own thread leaves the main state's call arguments alone";
constexpr double TS_MAGIC = 12345.6789;  // T's arguments in the plugin's call; L3's hook ignores every other call
constexpr DWORD TS_WAIT_MS = 5000;
constexpr int TS_HOOK_FRAMES = 10;      // frames for ScriptRunner to install L3's hook
constexpr int TS_HELPER_FRAMES = 600;   // budget for the main state's helper to report once the plugin's thread is done
constexpr ULONGLONG TS_WORKER_MS = 60000;

// T: rarely called static methods with Double parameters; the first one the game has is used (PRAGMATA has none
// before Atan2). Without any of them (RE9, MMSF), the first static System.Math method taking one or two Doubles and
// returning a Double. Not Sqrt: the api suite hooks it.
const char* const TS_T_CANDIDATES[][2] = {
    {"System.Math", "Log10(System.Double)"},
    {"System.Math", "IEEERemainder(System.Double, System.Double)"},
    {"System.Math", "Tanh(System.Double)"},
    {"System.Math", "Cosh(System.Double)"},
    {"System.Math", "Exp(System.Double)"},
    {"System.Math", "Log(System.Double)"},
    {"System.Math", "Atan2(System.Double, System.Double)"},
    {"System.Math", "Pow(System.Double, System.Double)"},
};

const char* const TS_SETUP = R"lua(
local t_type, t_sig, magic = ...
local T = sdk.find_type_definition(t_type):get_method(t_sig)
local X = sdk.find_type_definition("via.vec3"):get_method("op_Division(via.vec3, System.Single)")
local function fmt(v) return string.format("(%.9g, %.9g, %.9g)", v.x, v.y, v.z) end
sdk.hook(T, function(args)
    if sdk.to_double(args[2]) ~= magic then return end
    local ok, err = pcall(function()
        local first = X:call(nil, Vector3f.new(100, 200, 300), 4.0)
        local second = X:call(nil, Vector3f.new(1000, 2000, 3000), 4.0)
        rftest_two_states_worker(fmt(first), fmt(second))
    end)
    if not ok then rftest_two_states_worker(nil, nil, tostring(err)) end
end, function(retval) return retval end)
)lua";

struct TwoStates {
    int step{0};  // 0 set up, 1 waiting for L3's hook, 2 running, 3 done
    int frame{0}; // frames in the current step
    lua_State* L{};
    API::Method* x{};
    API::Method* t{};
    std::string t_name{};
    unsigned int x_hook{};
    bool x_hooked{false};
    HANDLE worker{};
    ULONGLONG worker_start{0};
    int worker_exit_frame{0};
};

TwoStates g_ts{};
std::atomic<bool> g_ts_done{false};

// Shared by the plugin's hook on X, the plugin's thread and the Lua functions below. Manual-reset events.
HANDLE g_ts_helper_ready{}, g_ts_worker_inside{}, g_ts_main_inside{}, g_ts_worker_finished{};
std::atomic<DWORD> g_ts_worker_tid{0}, g_ts_main_tid{0};
std::atomic<bool> g_ts_request{false}, g_ts_worker_armed{false}, g_ts_main_armed{false};
std::atomic<bool> g_ts_main_reported{false}, g_ts_worker_reported{false};
std::mutex g_ts_mux{};
std::string g_ts_main_result{}, g_ts_first{}, g_ts_third{}, g_ts_notes{};

void ts_note(const std::string& s) {
    std::scoped_lock _{g_ts_mux};
    g_ts_notes += (g_ts_notes.empty() ? "" : "; ") + s;
}

int ts_x_pre(int, void**, REFrameworkTypeDefinitionHandle*, unsigned long long) {
    const auto tid = GetCurrentThreadId();

    if (tid == g_ts_worker_tid && g_ts_worker_armed.exchange(false)) {
        SetEvent(g_ts_worker_inside);
        if (WaitForSingleObject(g_ts_main_inside, TS_WAIT_MS) != WAIT_OBJECT_0) {
            ts_note("the main state's call never reached X while the plugin thread's first call waited in it");
        }
    } else if (tid == g_ts_main_tid && g_ts_main_armed.exchange(false)) {
        SetEvent(g_ts_main_inside);
        if (WaitForSingleObject(g_ts_worker_finished, TS_WAIT_MS) != WAIT_OBJECT_0) {
            ts_note("the plugin thread's third call did not finish while the main state's call waited in X");
        }
    }

    return REFRAMEWORK_HOOK_CALL_ORIGINAL;
}

void ts_x_post(void**, REFrameworkTypeDefinitionHandle, unsigned long long) {}

// Main state: rftest_two_states_request() is true once, when the test wants the helper to run.
int l_ts_request(lua_State* L) {
    lua_pushboolean(L, g_ts_request.exchange(false));
    return 1;
}

// Main state: rftest_two_states_begin() lets the plugin's thread start and waits until its first call is in X.
int l_ts_begin(lua_State* L) {
    g_ts_main_tid = GetCurrentThreadId();
    SetEvent(g_ts_helper_ready);
    const bool ok = WaitForSingleObject(g_ts_worker_inside, TS_WAIT_MS) == WAIT_OBJECT_0;
    if (ok) {
        g_ts_main_armed = true;
    } else {
        ts_note("the plugin thread's first call never reached X");
    }
    lua_pushboolean(L, ok);
    return 1;
}

// Main state: rftest_two_states_main(result, error) reports the main state's call.
int l_ts_main(lua_State* L) {
    const std::string result = luaL_optstring(L, 1, "");
    if (!lua_isnoneornil(L, 2)) {
        ts_note(std::string{"main state's call raised: "} + luaL_checkstring(L, 2));
    }
    {
        std::scoped_lock _{g_ts_mux};
        g_ts_main_result = result;
    }
    g_ts_main_reported = true;
    return 0;
}

// L3: rftest_two_states_worker(first, third, error) reports the plugin thread's calls and lets the main state's go on.
int l_ts_worker(lua_State* L) {
    const std::string first = luaL_optstring(L, 1, "");
    const std::string third = luaL_optstring(L, 2, "");
    if (!lua_isnoneornil(L, 3)) {
        ts_note(std::string{"plugin thread's calls raised: "} + luaL_checkstring(L, 3));
    }
    {
        std::scoped_lock _{g_ts_mux};
        g_ts_first = first;
        g_ts_third = third;
    }
    g_ts_worker_reported = true;
    SetEvent(g_ts_worker_finished);
    return 0;
}

DWORD WINAPI ts_worker(LPVOID) {
    g_ts_worker_tid = GetCurrentThreadId();

    if (WaitForSingleObject(g_ts_helper_ready, 2 * TS_WAIT_MS) != WAIT_OBJECT_0) {
        ts_note("the main state's helper never started (it is in the regression suite)");
        return 0;
    }

    double magic = TS_MAGIC;
    void* slot{};
    memcpy(&slot, &magic, sizeof(magic));
    const std::vector<void*> args(g_ts.t->get_num_params(), slot);
    g_ts_worker_armed = true;
    g_ts.t->invoke(nullptr, args);
    g_ts_worker_armed = false;

    if (!g_ts_worker_reported) {
        ts_note("L3's hook on " + g_ts.t_name + " did not report: it did not run, or did not reach the end");
        SetEvent(g_ts_worker_finished); // don't leave the main state's call waiting
    }

    return 0;
}

void ts_finish(const char* status, const std::string& detail, bool clean_up = true) {
    auto& t = g_ts;
    g_ts_request = false;

    if (clean_up) {
        if (t.x_hooked) {
            t.x->remove_hook(t.x_hook);
            t.x_hooked = false;
        }
        if (t.L != nullptr) {
            g_param->functions->delete_script_state(t.L);
            t.L = nullptr;
        }
        if (t.worker != nullptr) {
            CloseHandle(t.worker);
            t.worker = nullptr;
        }
    }

    g_regression.set(TWO_STATES, status, detail);
    t.step = 3;
    g_ts_done = true;
}

// Called once per frame from on_pre_update (the game thread) until g_ts_done.
void two_states_step() {
    auto& t = g_ts;
    const auto fns = g_param->functions;
    ++t.frame;

    switch (t.step) {
    case 0: {
        const auto tdb = API::get()->tdb();
        t.x = tdb->find_method("via.vec3", "op_Division(via.vec3, System.Single)");
        if (t.x == nullptr) {
            ts_finish("SKIP", "via.vec3.op_Division(via.vec3, System.Single) is not in this game");
            return;
        }
        for (const auto& c : TS_T_CANDIDATES) {
            t.t = tdb->find_method(c[0], c[1]);
            if (t.t != nullptr) {
                t.t_name = std::string{c[0]} + "." + c[1];
                break;
            }
        }
        const auto math_t = t.t == nullptr ? tdb->find_type("System.Math") : nullptr;
        for (const auto m : math_t != nullptr ? math_t->get_methods() : std::vector<API::Method*>{}) {
            const auto ret = m->get_return_type();
            const auto params = m->get_params();
            if (!m->is_static() || std::string{m->get_name()} == "Sqrt" || params.empty() || params.size() > 2 ||
                ret == nullptr || ret->get_full_name() != "System.Double") {
                continue;
            }
            bool doubles = true;
            for (const auto& p : params) {
                doubles = doubles && p.t != nullptr && ((API::TypeDefinition*)p.t)->get_full_name() == "System.Double";
            }
            if (doubles) {
                t.t = m;
                t.t_name = std::string{"System.Math."} + m->get_name() + (params.size() == 1 ? "(System.Double)" : "(System.Double, System.Double)");
                break;
            }
        }
        if (t.t == nullptr) {
            ts_finish("SKIP", "System.Math has no static method taking one or two Doubles and returning a Double but Sqrt in this game");
            return;
        }

        g_ts_helper_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_ts_worker_inside = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_ts_main_inside = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_ts_worker_finished = CreateEventW(nullptr, TRUE, FALSE, nullptr);

        t.L = fns->create_script_state();
        if (t.L == nullptr) {
            ts_finish("FAIL", "create_script_state returned null");
            return;
        }

        std::string err{};
        fns->lock_lua();
        lua_register(t.L, "rftest_two_states_worker", l_ts_worker);
        if (luaL_loadstring(t.L, TS_SETUP) != LUA_OK) {
            err = pop_lua_error(t.L);
        } else {
            const auto dot = t.t_name.find('.', std::string{"System."}.size());
            lua_pushstring(t.L, t.t_name.substr(0, dot).c_str());
            lua_pushstring(t.L, t.t_name.substr(dot + 1).c_str());
            lua_pushnumber(t.L, TS_MAGIC);
            if (lua_pcall(t.L, 3, 0, 0) != LUA_OK) {
                err = pop_lua_error(t.L);
            }
        }
        fns->unlock_lua();

        if (!err.empty()) {
            ts_finish("FAIL", "setup in L3: " + err);
            return;
        }

        t.x_hook = t.x->add_hook(ts_x_pre, ts_x_post, false);
        t.x_hooked = true;
        t.step = 1;
        t.frame = 0;
        break;
    }

    case 1:
        if (t.frame < TS_HOOK_FRAMES) {
            return;
        }
        t.worker = CreateThread(nullptr, 0, ts_worker, nullptr, 0, nullptr);
        if (t.worker == nullptr) {
            ts_finish("FAIL", fmt("CreateThread failed (%lu)", GetLastError()));
            return;
        }
        t.worker_start = GetTickCount64();
        g_ts_request = true;
        t.step = 2;
        t.frame = 0;
        break;

    case 2: {
        if (WaitForSingleObject(t.worker, 0) != WAIT_OBJECT_0) {
            if (GetTickCount64() - t.worker_start > TS_WORKER_MS) {
                // Bounded waits make this unreachable; if it happens, leave L3 and the hook alone while it runs.
                ts_finish("FAIL", fmt("the plugin's thread still runs after %llu ms", (unsigned long long)TS_WORKER_MS), false);
            }
            return;
        }
        if (t.worker_exit_frame == 0) {
            t.worker_exit_frame = t.frame;
        }
        if (!g_ts_main_reported && t.frame - t.worker_exit_frame < TS_HELPER_FRAMES) {
            return;
        }

        std::string main_result, first, third, notes;
        {
            std::scoped_lock _{g_ts_mux};
            main_result = g_ts_main_result;
            first = g_ts_first;
            third = g_ts_third;
            notes = g_ts_notes;
        }
        if (!g_ts_main_reported) {
            notes += std::string{notes.empty() ? "" : "; "} + "the main state's helper did not report";
        }

        const bool ok = notes.empty() && main_result == "(5, 10, 15)" && first == "(25, 50, 75)" && third == "(250, 500, 750)";
        ts_finish(ok ? "PASS" : "FAIL",
            fmt("main state (thread %lu): (10, 20, 30) / 2 = %s, expected (5, 10, 15); plugin thread (%lu, L3's hook on %s): "
                "(100, 200, 300) / 4 = %s, then while the main state's call ran (1000, 2000, 3000) / 4 = %s%s%s",
                g_ts_main_tid.load(), main_result.empty() ? "(none)" : main_result.c_str(), g_ts_worker_tid.load(), t.t_name.c_str(),
                first.empty() ? "(none)" : first.c_str(), third.empty() ? "(none)" : third.c_str(), notes.empty() ? "" : "; ",
                notes.c_str()));
        break;
    }
    }
}

// ----------------------------------------------------------------------------------------------------------------
// Callbacks and the test schedule
// ----------------------------------------------------------------------------------------------------------------
enum class Stage { WaitGame, Runtime, WaitLua, LuaCreated, Present, Finish, TwoStates, Done };
std::atomic<Stage> g_stage{Stage::WaitGame};
std::atomic<int> g_stage_frame{0};
std::atomic<bool> g_runtime_requested{false};
std::atomic<bool> g_hooks_tested{false};

// ----------------------------------------------------------------------------------------------------------------
// rftest_peek(address [, size = 8]) for the Lua regression suite: the size (1, 2, 4 or 8) bytes at address as an
// unsigned integer, or nil when any of them isn't committed, readable memory. The Lua test reads engine headers at
// addresses it derives from an object's own header, so a game whose layout differs gets nil rather than a crash.
// ----------------------------------------------------------------------------------------------------------------
bool is_readable(uintptr_t addr, size_t size) {
    constexpr DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
        PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    const auto end = addr + size;
    if (addr == 0 || end < addr) {
        return false;
    }
    for (auto p = addr; p < end;) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery((LPCVOID)p, &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0 || (mbi.Protect & readable) == 0) {
            return false;
        }
        p = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    }
    return true;
}

// Memory can still be released between the check and the copy; the SEH guard covers that.
bool copy_guarded(uintptr_t addr, void* out, size_t size) {
    __try {
        memcpy(out, (const void*)addr, size);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

int rftest_peek(lua_State* L) {
    const auto addr = (uintptr_t)luaL_checkinteger(L, 1);
    const auto size = luaL_optinteger(L, 2, 8);
    luaL_argcheck(L, size == 1 || size == 2 || size == 4 || size == 8, 2, "size must be 1, 2, 4 or 8");
    uint64_t value{};
    if (is_readable(addr, (size_t)size) && copy_guarded(addr, &value, (size_t)size)) {
        lua_pushinteger(L, (lua_Integer)value);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

// rftest_poke(address, value [, size = 8]) for the Lua regression suite: writes the low size (1, 2, 4 or 8) bytes of
// value at address and returns true, or returns false and writes nothing when any of them isn't committed, writable
// memory. The Lua test writes a hooked method's result into the return buffer the hook was given.
bool is_writable(uintptr_t addr, size_t size) {
    constexpr DWORD writable = PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    const auto end = addr + size;
    if (addr == 0 || end < addr) {
        return false;
    }
    for (auto p = addr; p < end;) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery((LPCVOID)p, &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0 || (mbi.Protect & writable) == 0) {
            return false;
        }
        p = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    }
    return true;
}

bool write_guarded(uintptr_t addr, const void* in, size_t size) {
    __try {
        memcpy((void*)addr, in, size);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

int rftest_poke(lua_State* L) {
    const auto addr = (uintptr_t)luaL_checkinteger(L, 1);
    const auto value = (uint64_t)luaL_checkinteger(L, 2);
    const auto size = luaL_optinteger(L, 3, 8);
    luaL_argcheck(L, size == 1 || size == 2 || size == 4 || size == 8, 3, "size must be 1, 2, 4 or 8");
    lua_pushboolean(L, is_writable(addr, (size_t)size) && write_guarded(addr, &value, (size_t)size));
    return 1;
}

void on_lua_state_created(lua_State* L) {
    ++g_lua_created;
    // This runs the plugin's own copy of Lua (build.bat compiles dependencies/lua/src) on REFramework's state, and an
    // error in rftest_peek long-jumps from it into REFramework's. Both must be the same Lua release and build options:
    // build the plugin from a checkout whose dependencies/lua matches the REFramework build under test
    // (`git diff <build commit> -- dependencies/lua` empty; LUA_VERSION_RELEASE in dependencies/lua/src/lua.h). Nothing
    // checks this at run time.
    lua_register(L, "rftest_peek", rftest_peek);
    lua_register(L, "rftest_poke", rftest_poke);
    lua_register(L, "rftest_two_states_request", l_ts_request);
    lua_register(L, "rftest_two_states_begin", l_ts_begin);
    lua_register(L, "rftest_two_states_main", l_ts_main);
}
void on_imgui_frame(REFImGuiFrameCbData*) { ++g_imgui_frames; }

void on_pre_update() {
    ++g_pre_update;

    if (g_runtime_requested) {
        g_runtime_requested = false;
        api_runtime_tests();
        api_hook_install();
    }

    // g_ms_done is set only once the test has recorded its result: on_present writes the file as soon as it sees it.
    if (!g_ms_done && g_stage.load() == Stage::WaitLua) {
        multistate_step();
    }

    // g_ts_done is set only once the test has recorded its result: on_present writes the file as soon as it sees it.
    if (!g_ts_done && g_stage.load() == Stage::TwoStates) {
        two_states_step();
    }
}

void on_post_update() {
    ++g_post_update;

    // add_hook installs immediately; exercise it from the game thread a few frames after installing.
    // g_hooks_tested is set only once the test has recorded its results: on_present writes the file as soon as it sees it.
    static bool started = false;
    if (!started && g_stage.load() == Stage::WaitLua && g_stage_frame > 5) {
        started = true;
        api_hook_test();
        api_init_hook_test();
        g_hooks_tested = true;
    }
}

bool lua_suites_done() {
    std::error_code ec{};
    return std::filesystem::exists(results_dir() / "api.json", ec) && std::filesystem::exists(results_dir() / "regression.json", ec);
}

void on_present() {
    const auto frame = ++g_presents;
    ++g_stage_frame;

    // Progress trail for runs that stall: what the plugin is waiting on and whether the game thread is still updating.
    if (frame % 300 == 0 && g_stage.load() != Stage::Done) {
        g_param->functions->log_info("[rftest] plugin: frame %d stage %d (%d frames in) pre_update %d post_update %d lua_done %d hooks_tested %d multistate_done %d",
            frame, (int)g_stage.load(), g_stage_frame.load(), g_pre_update.load(), g_post_update.load(), (int)lua_suites_done(),
            (int)g_hooks_tested.load(), (int)g_ms_done.load());
    }

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
        if (g_hooks_tested && g_ms_done && (lua_suites_done() || g_stage_frame > 3000)) {
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
            g_regression.set(TWO_STATES, "FAIL", "pending: did not finish (crashed or hung the game?)");
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
        g_stage = ok ? Stage::Finish : Stage::TwoStates;
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
            g_stage = Stage::TwoStates;
            g_stage_frame = 0;
        }
        break;

    case Stage::TwoStates:
        if (g_ts_done) {
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
    api_init_hook_install();
    regression_init_tests();

    const auto f = param->functions;
    g_api.record("register callbacks", f->on_present(on_present) && f->on_lua_state_created(on_lua_state_created) &&
        f->on_pre_application_entry("UpdateBehavior", on_pre_update) && f->on_post_application_entry("UpdateBehavior", on_post_update) &&
        f->on_imgui_frame(on_imgui_frame));
    g_api.record("on_pre_application_entry rejects an empty name", !f->on_pre_application_entry("", on_pre_update));
    return true;
}
