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

using namespace reframework;

typedef void* (*REFGenericFunction)(...);


lua_State* g_lua{nullptr};
lua_State* sandbox_stateview_1{nullptr};
lua_State* sandbox_stateview_2{nullptr};
bool sandbox_mod_enabled_1{nullptr};
bool sandbox_mod_enabled_2{nullptr};
std::unordered_map<std::string, sol::load_result> g_loaded_snippets{};
float royalguardguage = 5000.0;
HWND g_wnd{};
bool g_initialized{false};

bool initialize_imgui() {
    if (g_initialized) {
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().IniFilename = "example_dll_ui.ini";

    const auto renderer_data = API::get()->param()->renderer_data;

    DXGI_SWAP_CHAIN_DESC swap_desc{};
    auto swapchain = (IDXGISwapChain*)renderer_data->swapchain;
    swapchain->GetDesc(&swap_desc);

    g_wnd = swap_desc.OutputWindow;

    if (!ImGui_ImplWin32_Init(g_wnd)) {
        return false;
    }

    if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        if (!g_d3d11.initialize()) {
            return false;
        }
    } else if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        if (!g_d3d12.initialize()) {
            return false;
        }
    }

    g_initialized = true;
    return true;
}


void sandbox_lua_create() {
    if (sandbox_stateview_1 == nullptr) {
        sandbox_stateview_1 = API::get()->param()->functions->create_script_state();
        // do load here
        sol::state_view sandbox_lua{sandbox_stateview_1};
        sandbox_lua.safe_script_file("sandboxscript1.lua");
    }
}

void sandbox_lua_destroy() {
    if (sandbox_stateview_1 != nullptr) {
        API::get()->param()->functions->delete_script_state(sandbox_stateview_1);
        sandbox_stateview_1 = nullptr;
    }
}


void sandbox_lua_create_2() {
    if (sandbox_stateview_2 == nullptr) {
        sandbox_stateview_2 = API::get()->param()->functions->create_script_state();
        // do load here
        sol::state_view sandbox_lua{sandbox_stateview_2};
        sandbox_lua.safe_script_file("sandboxscript2.lua");
    } else {
        sol::state_view sandbox_lua{sandbox_stateview_2};
        sandbox_lua["royalguardgauge"] = royalguardguage;
    }
}

void sandbox_lua_destroy_2() {
    if (sandbox_stateview_2 != nullptr) {
        API::get()->param()->functions->delete_script_state(sandbox_stateview_2);
        sandbox_stateview_2 = nullptr;
    }
}

void on_lua_state_created(lua_State* l) {
    API::LuaLock _{};


    g_lua = l;
    g_loaded_snippets.clear();

    sol::state_view lua{g_lua};

    // adds a new function to call from lua!
    //lua["foobar"] = []() {
    //    MessageBox(NULL, "foobar", "foobar", MB_OK);
    //};

    lua["my_cool_storage"] = sol::new_table{};
}

void on_lua_state_destroyed(lua_State* l) {
    API::LuaLock _{};
   
    g_lua = nullptr;
    g_loaded_snippets.clear();

    //sandbox mod disable
    sandbox_lua_destroy();
    sandbox_lua_destroy_2();
}




void internal_frame() {
    API::LuaLock _{};

    if (g_lua == nullptr) {
        return;
    }
    sol::state_view lua{g_lua};
    //check if the sandbox doesn't exist and create it
    if (sandbox_mod_enabled_1) {
        sandbox_lua_create();
    } else {
        sandbox_lua_destroy();
    }
    if (sandbox_mod_enabled_2) {
        sandbox_lua_create_2();
    } else {
        sandbox_lua_destroy_2();
    }
    // Get the window size in Lua, and then in C++
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

    // Do the same thing now, but in C++
    auto& api = API::get();
    const auto tdb = api->tdb();

    auto vm_context = api->get_vm_context();

    const auto scene_manager = api->get_native_singleton("via.SceneManager");
    const auto scene_manager_type = tdb->find_type("via.SceneManager");

    auto scene_manager_full_name = scene_manager_type->get_full_name();

    static bool print_name_once = true;

    if (print_name_once) {
        OutputDebugString((std::stringstream{} << scene_manager_full_name << " Size: " << scene_manager_full_name.size()).str().c_str());
        print_name_once = false;
    }

    const auto get_main_view = scene_manager_type->find_method("get_MainView");
    const auto main_view_type = get_main_view->get_return_type();

    const auto main_view = get_main_view->call<API::ManagedObject*>(vm_context, scene_manager);

    if (main_view == nullptr) {
        return;
    }

    float size[3]{};
    main_view->call("get_Size", &size, vm_context, main_view);

    auto get_size_invoke_result = main_view->invoke("get_Size", {});
    float* size_invoke = (float*)&get_size_invoke_result;

    if (ImGui::Begin("Super Cool Plugin")) {
        ImGui::Text("Hello from the super cool plugin!");
#if defined(__clang__)
        ImGui::Text("Plugin Compiler: Clang");
#elif defined(_MSC_VER)
        ImGui::Text("Plugin Compiler: Visual Studio");
#elif defined(__GNUC__)
        ImGui::Text("Plugin Compiler: GCC");
#else
        ImGui::Text("Plugin Compiler: Unknown");
#endif
        ImGui::Text("Game Window Size from Lua: %f %f", window_width, window_height);
        ImGui::Text("Game Window Size from C++ Call: %f %f", size[0], size[1]);
        ImGui::Text("Game Window Size from C++ Invoke: %f %f", size_invoke[0], size_invoke[1]);

        // Tests for the TDB
        const auto num_types = tdb->get_num_types();
        const auto num_methods = tdb->get_num_methods();
        const auto num_fields = tdb->get_num_fields();
        const auto num_properties = tdb->get_num_properties();
        const auto strings_size = tdb->get_strings_size();
        const auto raw_data_size = tdb->get_raw_data_size();
        const auto string_database = tdb->get_string_database();
        const auto raw_database = tdb->get_raw_database();

        if (ImGui::TreeNode("TDB")) {
            ImGui::Text("Num types: %d", num_types);
            ImGui::Text("Num methods: %d", num_methods);
            ImGui::Text("Num fields: %d", num_fields);
            ImGui::Text("Num properties: %d", num_properties);
            ImGui::Text("Strings size: %d", strings_size);
            ImGui::Text("Raw data size: %d", raw_data_size);
            ImGui::Text("String database: %p", string_database);
            ImGui::Text("Raw database: %p", raw_database);
            ImGui::TreePop();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Dumbed down Object Explorer start
        ////////////////////////////////////////////////////////////////////////////////
        auto native_singletons = api->get_native_singletons();
        auto managed_singletons = api->get_managed_singletons();

        static auto generate_method_prototype = [](API::Method* method) -> std::string {
            if (method == nullptr) {
                return "";
            }

            auto params = method->get_params();

            std::stringstream ss{};

            auto return_type = method->get_return_type();

            if (return_type != nullptr) {
                ss << return_type->get_full_name() << " ";
            }

            ss << method->get_name() << "(";

            for (auto i = 0; i < params.size(); i++) {
                auto& param = params[i];

                const auto param_type = (API::TypeDefinition*)param.t;

                auto param_type_name = param_type->get_full_name();
                auto param_name = param.name;

                ss << param_type_name << " " << param_name;

                if (i < params.size() - 1) {
                    ss << ", ";
                }
            }

            ss << ")";

            return ss.str();
        };

        static void (*display_type)(void* obj, API::TypeDefinition* t) = nullptr;
        
        static auto display_methods = [](API::TypeDefinition* t) {
            if (t == nullptr) {
                return;
            }

            auto methods = t->get_methods();

            for (auto method : methods) {
                auto method_name = generate_method_prototype(method);

                if (ImGui::TreeNode(method_name.c_str(), "%s", method_name.c_str())) {
                    ImGui::Text("Address: 0x%p", method);
                    ImGui::TreePop();
                }
            }
        };

        static auto display_fields = [](void* obj, API::TypeDefinition* t) {
            if (t == nullptr) {
                return;
            }

            auto fields = t->get_fields();

            for (auto field : fields) {
                auto field_name = field->get_name();

                const auto type = field->get_type();
                const std::string type_name = type != nullptr ? type->get_full_name() : "";
                const auto full_field_name = type_name + " " + field_name;

                if (ImGui::TreeNode(full_field_name.c_str(), "%s", full_field_name.c_str())) {
                    ImGui::Text("Address: 0x%p", field);

                    if (type != nullptr) {
                        if (ImGui::TreeNode(type_name.c_str(), "%s", type_name.c_str())) {
                            display_type(obj, type);
                            ImGui::TreePop();
                        }
                    }

                    const auto offset_from_base = field->get_offset_from_base();
                    const auto offset_from_fieldptr = field->get_offset_from_fieldptr();
                    const auto flags = field->get_flags();
                    const auto is_static = field->is_static();
                    const auto is_literal = field->is_literal();
                    const auto init_data = field->get_init_data();

                    ImGui::Text("Offset from base: %d", offset_from_base);
                    ImGui::Text("Offset from fieldptr: %d", offset_from_fieldptr);
                    ImGui::Text("Flags: %d", flags);
                    ImGui::Text("Is static: %d", is_static);
                    ImGui::Text("Is literal: %d", is_literal);
                    ImGui::Text("Init data: %p", init_data);

                    ImGui::TreePop();
                }
            }
        };

        static auto display_managed_object = [](API::ManagedObject* obj) {
            if (obj == nullptr) {
                return;
            }

            auto t = obj->get_type_definition();

            if (t == nullptr) {
                return;
            }

            display_type(obj, t);
        };

        static void (*display_type_info)(void* obj, API::TypeInfo* ti) = nullptr;
        display_type_info = [](void* obj, API::TypeInfo* ti) {
            if (ti == nullptr) {
                return;
            }

            const auto typeinfo_name = ti->get_name();
            const auto is_singleton = ti->is_singleton();
            const auto is_clr_type = ti->is_clr_type();
            const auto deserializer = ti->get_deserializer_fn();
            const auto crc = ti->get_crc();

            if (const auto parent = ti->get_parent(); parent != nullptr) {
                if (ImGui::TreeNode(parent->get_name(), "%s", parent->get_name())) {
                    display_type_info(obj, parent);
                    ImGui::TreePop();
                }
            }
            
            ImGui::Text("Name: %s", typeinfo_name);
            ImGui::Text("Is singleton: %d", is_singleton);
            ImGui::Text("Is CLR type: %d", is_clr_type);
            ImGui::Text("Deserializer: %p", typeinfo_name);
            ImGui::Text("CRC: %x", crc);

            if (is_singleton) {
                const auto instance = ti->get_singleton_instance();

                ImGui::Text("Instance: %p", instance);
            }

            if (auto tdef = ti->get_type_definition(); tdef != nullptr) {
                if (ImGui::TreeNode("Methods")) {
                    display_methods(tdef);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Fields")) {
                    display_fields(obj, tdef);
                    ImGui::TreePop();
                }
            }
        };
        
        display_type = [](void* obj, API::TypeDefinition* t) -> void {
            if (ImGui::TreeNode("Methods")) {
                display_methods(t);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Fields")) {
                display_fields(obj, t);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("TypeInfo")) {
                display_type_info(obj, t->get_type_info());
                ImGui::TreePop();
            }
        };

        if (ImGui::TreeNode("Managed Singletons")) {
            for (const auto& managed_singleton : managed_singletons) {
                const auto t = (API::TypeDefinition*)managed_singleton.t;
                const auto full_name = t->get_full_name();

                if (ImGui::TreeNode(full_name.c_str(), "%s", full_name.c_str())) {
                    ImGui::Text("Address: 0x%p", managed_singleton.instance);

                    display_managed_object((API::ManagedObject*)managed_singleton.instance);

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Native Singletons")) {
            for (const auto& native_singleton : native_singletons) {
                const auto t = (API::TypeDefinition*)native_singleton.t;

                if (ImGui::TreeNode(native_singleton.name, "%s", native_singleton.name)) {
                    ImGui::Text("Address: 0x%p", native_singleton.instance);

                    display_type(native_singleton.instance, t);

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        ImGui::Checkbox("Sandbox mod 1", &sandbox_mod_enabled_1);
        ImGui::Checkbox("Sandbox mod 2", &sandbox_mod_enabled_2);
        ImGui::InputFloat("Royalguardguage", &royalguardguage, 0.00f, 30000.0f, "%.1f");
        
        ImGui::End();
    }
}

void on_present() {
    if (!g_initialized) {
        if (!initialize_imgui()) {
            OutputDebugString("Failed to initialize imgui");
            return;
        } else {
            OutputDebugString("Initialized imgui");
        }
    }

    const auto renderer_data = API::get()->param()->renderer_data;

    if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        internal_frame();

        ImGui::EndFrame();
        ImGui::Render();

        g_d3d11.render_imgui();
    } else if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        auto command_queue = (ID3D12CommandQueue*)renderer_data->command_queue;

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

    const auto renderer_data = API::get()->param()->renderer_data;

    if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        ImGui_ImplDX11_Shutdown();
        g_d3d11 = {};
    }

    if (renderer_data->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        ImGui_ImplDX12_Shutdown();
        g_d3d12 = {};
    }

    g_initialized = false;
}

bool on_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

    return !ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard;
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;

    // Optionally, specify a specific game name that this plugin is compatible with.
    //version->game_name = "MHRISE";
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    API::initialize(param);
    ImGui::CreateContext();

    const auto functions = param->functions;
    functions->on_lua_state_created(on_lua_state_created);
    functions->on_lua_state_destroyed(on_lua_state_destroyed);
    functions->on_present(on_present);
    functions->on_pre_application_entry("BeginRendering", on_pre_begin_rendering); // Look at via.ModuleEntry or the wiki for valid names here
    functions->on_post_application_entry("EndRendering", on_post_end_rendering);
    functions->on_device_reset(on_device_reset);
    functions->on_message((REFOnMessageCb)on_message);
    functions->log_error("%s %s", "Hello", "error");
    functions->log_warn("%s %s", "Hello", "warning");
    functions->log_info("%s %s", "Hello", "info");
    API::get()->log_error("%s %s", "Hello", "error");
    API::get()->log_warn("%s %s", "Hello", "warning");
    API::get()->log_info("%s %s", "Hello", "info");

    return true;
}

// you DO NOT need to have a DllMain, this is only necessary
// if your DLL needs to load immediately, like in a raw plugin.
// or if you want to do some pre-initialization in the DllMain
// or if you want to do some cleanup in the DllMain
BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        OutputDebugString("Plugin early load test.");
    }

    return TRUE;
}
