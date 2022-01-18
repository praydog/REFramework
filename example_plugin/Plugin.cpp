// Install DebugView to view the OutputDebugString calls
#include <sstream>
#include <mutex>

#include <Windows.h>

#include <reframework/API.hpp>
#include <sol/sol.hpp>

lua_State* g_lua{nullptr};
const REFrameworkPluginInitializeParam* g_param{nullptr};

std::unordered_map<std::string, sol::load_result> g_loaded_snippets{};
std::recursive_mutex g_lua_mtx{};

struct LuaLocker {
    LuaLocker() {
        g_lua_mtx.lock();
        g_param->functions->lock_lua();
    }

    virtual ~LuaLocker() {
        g_param->functions->unlock_lua();
        g_lua_mtx.unlock();
    }
};

void on_lua_state_created(lua_State* l) {
    LuaLocker _{};

    g_lua = l;
    g_loaded_snippets.clear();

    sol::state_view lua{g_lua};

    // adds a new function to call from lua!
    lua["foobar"] = []() {
        MessageBox(NULL, "foobar", "foobar", MB_OK);
    };

    lua["my_cool_storage"] = sol::new_table{};
}

void on_lua_state_destroyed(lua_State* l) {
    LuaLocker _{};

    g_lua = nullptr;
    g_loaded_snippets.clear();
}

void on_frame() {
    OutputDebugString("Example Frame");

    if (g_lua == nullptr) {
        return;
    }

    LuaLocker _{};
    sol::state_view lua{g_lua};

    if (!g_loaded_snippets.contains("window_metadata")) {
        g_loaded_snippets["window_metadata"] = lua.load(R"(
            if not my_cool_storage.scene_manager or not my_cool_storage.scene_manager then
                my_cool_storage.scene_manager = sdk.get_native_singleton("via.SceneManager")
                my_cool_storage.scene_manager_type = sdk.find_type_definition("via.SceneManager")
            end

            if not my_cool_storage.scene_manager or not my_cool_storage.scene_manager then
                return false
            end

            local main_view = sdk.call_native_func(my_cool_storage.scene_manager, my_cool_storage.scene_manager_type, "get_MainView")
            if not main_view then
                return false
            end

            local size = main_view:call("get_Size")
            my_cool_storage.window_width = size:get_field("w")
            my_cool_storage.window_height = size:get_field("h")

            return true;
        )");
    }

    // Fill in the window size and stuff
    bool window_metadata_result = g_loaded_snippets["window_metadata"]();

    if (!window_metadata_result) {
        return;
    }

    // Interop with Lua from C++
    sol::table storage = lua["my_cool_storage"];

    float window_width = storage["window_width"];
    float window_height = storage["window_height"];

    OutputDebugString((std::stringstream{} << "Window Size: " << window_width << " " << window_height).str().c_str());
}

void on_pre_begin_rendering() {
    OutputDebugString("Example Pre Begin Rendering");
}

void on_post_end_rendering() {
    OutputDebugString("Example Post End Rendering");
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    g_param = param;

    const auto functions = param->functions;
    functions->on_lua_state_created(on_lua_state_created);
    functions->on_lua_state_destroyed(on_lua_state_destroyed);
    functions->on_frame(on_frame);
    functions->on_pre_application_entry("BeginRendering", on_pre_begin_rendering); // Look at via.ModuleEntry or the wiki for valid names here
    functions->on_post_application_entry("EndRendering", on_post_end_rendering);

    return true;
}
