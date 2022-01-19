// Install DebugView to view the OutputDebugString calls
#include <sstream>
#include <mutex>

#include <Windows.h>

#include <reframework/API.hpp>
#include <sol/sol.hpp>

// only really necessary if you want to render to the screen
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

#include "rendering/d3d11.hpp"
#include "rendering/d3d12.hpp"

#include "Plugin.hpp"

lua_State* g_lua{nullptr};
const REFrameworkPluginInitializeParam* g_param{nullptr};

std::unordered_map<std::string, sol::load_result> g_loaded_snippets{};
std::recursive_mutex g_lua_mtx{};
HWND g_wnd{};
bool g_initialized{false};

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

bool initialize_imgui() {
    if (g_initialized) {
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().IniFilename = "example_dll_ui.ini";

    DXGI_SWAP_CHAIN_DESC swap_desc{};
    auto swapchain = (IDXGISwapChain*)g_param->renderer_data->swapchain;
    swapchain->GetDesc(&swap_desc);

    g_wnd = swap_desc.OutputWindow;

    if (!ImGui_ImplWin32_Init(g_wnd)) {
        return false;
    }

    if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        if (!g_d3d11.initialize()) {
            return false;
        }
    } else if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        if (!g_d3d12.initialize()) {
            return false;
        }
    }

    g_initialized = true;
    return true;
}

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

void internal_frame() {
    LuaLocker _{};

    if (g_lua == nullptr) {
        return;
    }

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

    //OutputDebugString((std::stringstream{} << "Window Size: " << window_width << " " << window_height).str().c_str());

    if (ImGui::Begin("Super Cool Plugin")) {
        ImGui::Text("Hello from the super cool plugin!");
        ImGui::Text("Game Window Size: %f %f", window_width, window_height);

        ImGui::End();
    }
}

void on_frame() {
    if (!g_initialized) {
        if (!initialize_imgui()) {
            OutputDebugString("Failed to initialize imgui");
            return;
        } else {
            OutputDebugString("Initialized imgui");
        }
    }

    if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        internal_frame();

        ImGui::EndFrame();
        ImGui::Render();

        g_d3d11.render_imgui();
    } else if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        auto command_queue = (ID3D12CommandQueue*)g_param->renderer_data->command_queue;

        if (command_queue == nullptr ){
            return;
        }

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        internal_frame();

        ImGui::EndFrame();
        ImGui::Render();

        g_d3d12.render_imgui();
    }
}

void on_pre_begin_rendering() {
    //OutputDebugString("Example Pre Begin Rendering");
}

void on_post_end_rendering() {
    //OutputDebugString("Example Post End Rendering");
}

void on_device_reset() {
    OutputDebugString("Example Device Reset");

    if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        ImGui_ImplDX11_Shutdown();
        g_d3d11 = {};
    }

    if (g_param->renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        ImGui_ImplDX12_Shutdown();
        g_d3d12 = {};
    }

    g_initialized = false;
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
    functions->on_device_reset(on_device_reset);

    return true;
}
