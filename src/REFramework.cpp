#include <filesystem>

#include <spdlog/sinks/basic_file_sink.h>

#include <imgui/imgui.h>
#include "re2-imgui/font_robotomedium.hpp"
#include "re2-imgui/imgui_impl_dx11.h"
#include "re2-imgui/imgui_impl_dx12.h"
#include "re2-imgui/imgui_impl_win32.h"

#include "utility/Module.hpp"
#include "utility/Patch.hpp"
#include "utility/Scan.hpp"

#include "Mods.hpp"
#include "sdk/REGlobals.hpp"

#include "LicenseStrings.hpp"
#include "REFramework.hpp"

namespace fs = std::filesystem;

std::unique_ptr<REFramework> g_framework{};

REFramework::REFramework()
    : m_game_module{GetModuleHandle(0)}
    , m_logger{spdlog::basic_logger_mt("REFramework", "re2_framework_log.txt", true)} {
    spdlog::set_default_logger(m_logger);
    spdlog::flush_on(spdlog::level::info);
    spdlog::info("REFramework entry");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    // Create the typedef for RtlGetVersion
    typedef LONG (*RtlGetVersionFunc)(PRTL_OSVERSIONINFOW);

    const auto ntdll = GetModuleHandle("ntdll.dll");

    if (ntdll != nullptr) {
        // Manually get RtlGetVersion
        auto rtl_get_version = (RtlGetVersionFunc)GetProcAddress(ntdll, "RtlGetVersion");

        if (rtl_get_version != nullptr) {
            spdlog::info("Getting OS version information...");

            // Create an initial log that prints out the user's Windows OS version information
            // With the major and minor version numbers
            // Using RtlGetVersion()
            OSVERSIONINFOW os_version_info{};
            ZeroMemory(&os_version_info, sizeof(OSVERSIONINFOW));
            os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
            os_version_info.dwMajorVersion = 0;
            os_version_info.dwMinorVersion = 0;
            os_version_info.dwBuildNumber = 0;
            os_version_info.dwPlatformId = 0;

            if (rtl_get_version(&os_version_info) != 0) {
                spdlog::info("RtlGetVersion() failed");
            } else {
                // Log the Windows version information
                spdlog::info("OS Version Information");
                spdlog::info("\tMajor Version: {}", os_version_info.dwMajorVersion);
                spdlog::info("\tMinor Version: {}", os_version_info.dwMinorVersion);
                spdlog::info("\tBuild Number: {}", os_version_info.dwBuildNumber);
                spdlog::info("\tPlatform Id: {}", os_version_info.dwPlatformId);

                spdlog::info("Disclaimer: REFramework does not send this information to the developers or any other third party.");
                spdlog::info("This information is only used to help with the development of REFramework.");
            }
        } else {
            spdlog::info("RtlGetVersion() not found");
        }
    } else {
        spdlog::info("ntdll.dll not found");
    }

    // Hooking D3D12 initially because we need to retrieve the command queue before the first frame then switch to D3D11 if it failed later
    // on
    if (!hook_d3d12()) {
        spdlog::error("Failed to hook D3D12 for initial test.");
    }

    // Fixes a crash on some machines when starting the game
#ifdef RE8
    // auto startup_patch_addr = Address{m_game_module}.get(0x3E69E50);
    auto startup_patch_addr = utility::scan(m_game_module, "40 53 57 48 83 ec 28 48 83 b9 ? ? ? ? 00");

    if (startup_patch_addr) {
        static auto permanent_patch = Patch::create(*startup_patch_addr, {0xC3});
    } else {
        spdlog::info("Couldn't find RE8 crash fix patch location!");
    }
#endif
}

bool REFramework::hook_d3d11() {
    m_d3d11_hook = std::make_unique<D3D11Hook>();
    m_d3d11_hook->on_present([this](D3D11Hook& hook) { on_frame_d3d11(); });
    m_d3d11_hook->on_resize_buffers([this](D3D11Hook& hook) { on_reset(); });

    // Making sure D3D12 is not hooked
    if (!m_is_d3d12) {
        if (m_d3d11_hook->hook()) {
            spdlog::info("Hooked DirectX 11");
            m_valid = true;
            m_is_d3d11 = true;
            return true;
        }
        // We make sure to no unhook any unwanted hooks if D3D11 didn't get hooked properly
        if (m_d3d11_hook->unhook()) {
            spdlog::info("D3D11 unhooked!");
        } else {
            spdlog::info("Cannot unhook D3D11, this might crash.");
        }

        m_valid = false;
        m_is_d3d11 = false;
        return false;
    }

    return false;
}

bool REFramework::hook_d3d12() {
    // windows 7?
    if (LoadLibraryA("d3d12.dll") == nullptr) {
        spdlog::info("d3d12.dll not found, user is probably running Windows 7.");
        spdlog::info("Falling back to hooking D3D11.");

        m_is_d3d12 = false;
        return hook_d3d11();
    }

    m_d3d12_hook = std::make_unique<D3D12Hook>();
    m_d3d12_hook->on_present([this](D3D12Hook& hook) { on_frame_d3d12(); });
    m_d3d12_hook->on_resize_buffers([this](D3D12Hook& hook) { on_reset(); });
    m_d3d12_hook->on_resize_target([this](D3D12Hook& hook) { on_reset(); });
    m_d3d12_hook->on_create_swap_chain([this](D3D12Hook& hook) { m_pd3d_command_queue_d3d12 = m_d3d12_hook->get_command_queue(); });

    // Making sure D3D11 is not hooked
    if (!m_is_d3d11) {
        if (m_d3d12_hook->hook()) {
            spdlog::info("Hooked DirectX 12");
            m_valid = true;
            m_is_d3d12 = true;
            return true;
        }
        // We make sure to no unhook any unwanted hooks if D3D12 didn't get hooked properly
        if (m_d3d12_hook->unhook()) {
            spdlog::info("D3D12 Unhooked!");
        } else {
            spdlog::info("Cannot unhook D3D12, this might crash.");
        }

        m_valid = false;
        m_is_d3d12 = false;

        // Try to hook d3d11 instead
        return hook_d3d11();
    }

    return false;
}

REFramework::~REFramework() {
    if (m_is_d3d11) {
        ImGui_ImplDX11_Shutdown();
    }

    if (m_is_d3d12) {
        ImGui_ImplDX12_Shutdown();
    }

    ImGui_ImplWin32_Shutdown();

    if (m_initialized) {
        ImGui::DestroyContext();
    }
}

// D3D11 Draw funciton
void REFramework::on_frame_d3d11() {
    spdlog::debug("on_frame (D3D11)");

    if (!m_initialized) {
        if (!initialize()) {
            spdlog::error("Failed to initialize REFramework on DirectX 11");
            return;
        }

        spdlog::info("REFramework initialized");
        m_initialized = true;
        return;
    }

    consume_input();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_error.empty() && m_game_data_initialized) {
        // Write default config once if it doesn't exist.
        if (!std::exchange(m_created_default_cfg, true)) {
            if (!fs::exists({utility::widen("re2_fw_config.txt")})) {
                save_config();
            }
        }

        // Run mod frame callbacks.
        m_mods->on_frame();
    }

    draw_ui();
    m_last_draw_ui = m_draw_ui;

    ImGui::EndFrame();
    ImGui::Render();

    ID3D11DeviceContext* context = nullptr;
    m_d3d11_hook->get_device()->GetImmediateContext(&context);

    context->OMSetRenderTargets(1, &m_main_render_target_view_d3d11, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// D3D12 Draw funciton
void REFramework::on_frame_d3d12() {
    spdlog::debug("on_frame (D3D12)");
    
    if (!m_initialized) {
        if (!initialize()) {
            spdlog::error("Failed to initialize REFramework on DirectX 12");
            return;
        }

        spdlog::info("REFramework initialized");
        m_initialized = true;
        return;
    }

    if (m_pd3d_command_queue_d3d12 == nullptr) {
        spdlog::error("Null Command Queue");
        return;
    }

    consume_input();

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_error.empty() && m_game_data_initialized) {
        // Write default config once if it doesn't exist.
        if (!std::exchange(m_created_default_cfg, true)) {
            if (!fs::exists({utility::widen("re2_fw_config.txt")})) {
                save_config();
            }
        }

        // Run mod frame callbacks.
        m_mods->on_frame();
    }

    draw_ui();
    m_last_draw_ui = m_draw_ui;

    ImGui::EndFrame();
    ImGui::Render();

    // Rendering
    UINT back_buffer_idx = m_d3d12_hook->get_swap_chain()->GetCurrentBackBufferIndex();
    FrameContextD3D12* frame_context = &m_frame_context_d3d12[back_buffer_idx];
    frame_context->command_allocator->Reset();

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_main_render_target_resource_d3d12[back_buffer_idx];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    m_pd3d_command_list_d3d12->Reset(frame_context->command_allocator, NULL);
    m_pd3d_command_list_d3d12->ResourceBarrier(1, &barrier);

    // Render Dear ImGui graphics
    m_pd3d_command_list_d3d12->OMSetRenderTargets(1, &m_main_render_target_descriptor_d3d12[back_buffer_idx], FALSE, NULL);
    m_pd3d_command_list_d3d12->SetDescriptorHeaps(1, &m_pd3d_srv_desc_heap_d3d12);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pd3d_command_list_d3d12);
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_pd3d_command_list_d3d12->ResourceBarrier(1, &barrier);
    m_pd3d_command_list_d3d12->Close();

    m_pd3d_command_queue_d3d12->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_pd3d_command_list_d3d12);
}

void REFramework::on_reset() {
    spdlog::info("Reset!");

    // Crashes if we don't release it at this point.
    if (m_is_d3d11) {
        cleanup_render_target_d3d11();
    }

    if (m_is_d3d12) {
        // For some reason if we don't destroy the context and recreate it later, the text fields will not work.
        if (m_initialized) {
            ImGui::DestroyContext();
        }
        cleanup_render_target_d3d12();
    }

    m_initialized = false;
}

bool REFramework::on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    if (!m_initialized) {
        return true;
    }

    bool is_mouse_moving{false};
    switch (message) {
    case WM_INPUT: {
        // RIM_INPUT means the window has focus
        if (GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUT) {
            uint32_t size = sizeof(RAWINPUT);
            RAWINPUT raw{};
            
            // obtain size
            GetRawInputData((HRAWINPUT)l_param, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

            auto result = GetRawInputData((HRAWINPUT)l_param, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

            if (raw.header.dwType == RIM_TYPEMOUSE) {
                m_accumulated_mouse_delta[0] += (float)raw.data.mouse.lLastX;
                m_accumulated_mouse_delta[1] += (float)raw.data.mouse.lLastY;

                // Allowing camera movement when the UI is hovered while not focused
                if (raw.data.mouse.lLastX || raw.data.mouse.lLastY) {
                    is_mouse_moving = true;
                }
            }
        }
    } break;

    case RE_TOGGLE_CURSOR: {
        if (!m_is_internal_message) {
            m_cursor_state = w_param;

            if (m_draw_ui && !w_param)
                return ImGui_ImplWin32_WndProcHandler(wnd, RE_TOGGLE_CURSOR, true, l_param);
        }
    } break;
    default:
        break;
    }
    m_is_internal_message = false;

    ImGui_ImplWin32_WndProcHandler(wnd, message, w_param, l_param);

    {
        // If the user is interacting with the UI we block the message from going to the game.
        const auto& io = ImGui::GetIO();
        if (m_draw_ui && !m_ui_passthrough) {
            // Fix of a bug that makes the input key down register but the key up will never register
            // when clicking on the ui while the game is not focused
            if (message == WM_INPUT && GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUTSINK)
                return false;

            if (m_is_ui_focused) {
                if (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput)
                    return false;
            } else {
                if (!is_mouse_moving && (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput))
                    return false;
            }
        }
    }

    return true;
}

// this is unfortunate.
void REFramework::on_direct_input_keys(const std::array<uint8_t, 256>& keys) {
    if (keys[m_menu_key] && m_last_keys[m_menu_key] == 0) {
        std::lock_guard _{m_input_mutex};
        m_draw_ui = !m_draw_ui;

        // Save the config if we close the UI
        if (!m_draw_ui && m_game_data_initialized) {
            save_config();
        }
    }

    m_last_keys = keys;
}

void REFramework::save_config() {
    spdlog::info("Saving config re2_fw_config.txt");

    utility::Config cfg{};

    for (auto& mod : m_mods->get_mods()) {
        mod->on_config_save(cfg);
    }

    if (!cfg.save("re2_fw_config.txt")) {
        spdlog::info("Failed to save config");
        return;
    }

    spdlog::info("Saved config");
}

void REFramework::consume_input() {
    m_mouse_delta[0] = m_accumulated_mouse_delta[0];
    m_mouse_delta[1] = m_accumulated_mouse_delta[1];

    m_accumulated_mouse_delta[0] = 0.0f;
    m_accumulated_mouse_delta[1] = 0.0f;
}

void REFramework::draw_ui() {
    std::lock_guard _{m_input_mutex};

    if (!m_draw_ui) {
        m_is_ui_focused = false;
        if (m_last_draw_ui) {
            m_is_internal_message = true;
            m_windows_message_hook->window_toggle_cursor(m_cursor_state);
        }
        m_dinput_hook->acknowledge_input();
        // ImGui::GetIO().MouseDrawCursor = false;
        return;
    }
    
    // UI Specific code:
    m_is_ui_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

    if (m_ui_option_transparent) {
        auto& style = ImGui::GetStyle();
        if (m_is_ui_focused) {
            style.Alpha = 1.0f;
        } else {
            if (ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow)) {
                style.Alpha = 0.9f;
            } else {
                style.Alpha = 0.8f;
            }
        }
    } else {
        auto& style = ImGui::GetStyle();
        style.Alpha = 1.0f;
    }

    auto& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard) {
        m_dinput_hook->ignore_input();
    } else {
        m_dinput_hook->acknowledge_input();
    }

    // ImGui::GetIO().MouseDrawCursor = true;
    if (!m_last_draw_ui) {
        m_is_internal_message = true;
        m_windows_message_hook->window_toggle_cursor(true);
    }

    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_::ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_::ImGuiCond_Once);

    ImGui::Begin("REFramework", &m_draw_ui);
    ImGui::Text("Menu Key: Insert");
    ImGui::Checkbox("Transparency", &m_ui_option_transparent);
    ImGui::SameLine();
    ImGui::Text("(?)");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Makes the UI transparent when not focused.");
    ImGui::Checkbox("Input Passthrough", &m_ui_passthrough);
    ImGui::SameLine();
    ImGui::Text("(?)");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Allows mouse and keyboard inputs to register to the game while the UI is focused.");

    // Mods:
    draw_about();

    if (m_error.empty() && m_game_data_initialized) {
        m_mods->on_draw_ui();
    } else if (!m_game_data_initialized) {
        ImGui::TextWrapped("REFramework is currently initializing...");
    } else if (!m_error.empty()) {
        ImGui::TextWrapped("REFramework error: %s", m_error.c_str());
    }

    ImGui::End();
}

void REFramework::draw_about() {
    if (!ImGui::CollapsingHeader("About")) {
        return;
    }

    ImGui::TreePush("About");

    ImGui::Text("Author: praydog");
    ImGui::Text("Inspired by the Kanan project.");
    ImGui::Text("https://github.com/praydog/RE2-Mod-Framework");

    if (ImGui::CollapsingHeader("Licenses")) {
        ImGui::TreePush("Licenses");

        if (ImGui::CollapsingHeader("glm")) {
            ImGui::TextWrapped(license::glm);
        }

        if (ImGui::CollapsingHeader("imgui")) {
            ImGui::TextWrapped(license::imgui);
        }

        if (ImGui::CollapsingHeader("minhook")) {
            ImGui::TextWrapped(license::minhook);
        }

        if (ImGui::CollapsingHeader("spdlog")) {
            ImGui::TextWrapped(license::spdlog);
        }

        if (ImGui::CollapsingHeader("robotomedium")) {
            ImGui::TextWrapped(license::roboto);
        }

        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void REFramework::set_imgui_style() noexcept {
    ImGui::StyleColorsDark();

    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.WindowBorderSize = 2.0f;
    style.WindowPadding = ImVec2(2.0f, 0.0f);

    auto& colors = ImGui::GetStyle().Colors;
    // Window BG
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Navigatation highlight
    colors[ImGuiCol_NavHighlight] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Checkbox
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.5505f, 0.551f, 1.0f);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.211f, 0.210f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.8f, 0.805f, 0.81f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.2f, 0.205f, 0.21f, 0.0f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};

    // Font
    auto& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium_compressed_data, RobotoMedium_compressed_size, 16.0f);
}

bool REFramework::initialize() {
    if (m_initialized) {
        return true;
    }

    if (m_is_d3d11) {
        spdlog::info("Attempting to initialize DirectX 11");

        if (!m_d3d11_hook->is_hooked()) {
            return false;
        }

        auto device = m_d3d11_hook->get_device();
        auto swap_chain = m_d3d11_hook->get_swap_chain();

        // Wait.
        if (device == nullptr || swap_chain == nullptr) {
            spdlog::info("Device or SwapChain null. DirectX 12 may be in use. Unhooking D3D11...");

            // We unhook D3D11
            if (m_d3d11_hook->unhook()) {
                spdlog::info("D3D11 unhooked!");
            } else {
                spdlog::error("Cannot unhook D3D11, this might crash.");
            }

            m_is_d3d11 = false;
            m_valid = false;

            // We hook D3D12
            if (!hook_d3d12()) {
                spdlog::error("Failed to hook D3D12 after unhooking D3D11.");
            }
            return false;
        }

        ID3D11DeviceContext* context = nullptr;
        device->GetImmediateContext(&context);

        DXGI_SWAP_CHAIN_DESC swap_desc{};
        swap_chain->GetDesc(&swap_desc);

        m_wnd = swap_desc.OutputWindow;

        // Explicitly call destructor first
        m_windows_message_hook.reset();
        m_windows_message_hook = std::make_unique<WindowsMessageHook>(m_wnd);
        m_windows_message_hook->on_message = [this](auto wnd, auto msg, auto w_param, auto l_param) {
            return on_message(wnd, msg, w_param, l_param);
        };

        // just do this instead of rehooking because there's no point.
        if (m_first_frame) {
            m_dinput_hook = std::make_unique<DInputHook>(m_wnd);
        } else {
            m_dinput_hook->set_window(m_wnd);
        }

        spdlog::info("Creating render target");

        create_render_target_d3d11();

        spdlog::info("Window Handle: {0:x}", (uintptr_t)m_wnd);
        spdlog::info("Initializing ImGui");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        set_imgui_style();

        ImGui::GetIO().IniFilename = "ref_ui.ini";

        spdlog::info("Initializing ImGui Win32");

        if (!ImGui_ImplWin32_Init(m_wnd)) {
            spdlog::error("Failed to initialize ImGui.");
            return false;
        }

        spdlog::info("Initializing ImGui D3D11");

        if (!ImGui_ImplDX11_Init(device, context)) {
            spdlog::error("Failed to initialize ImGui.");
            return false;
        }
    } else if (m_is_d3d12) {
        spdlog::info("Attempting to initialize DirectX 12");

        if (!m_d3d12_hook->is_hooked()) {
            return false;
        }

        auto device = m_d3d12_hook->get_device();
        auto swap_chain = m_d3d12_hook->get_swap_chain();

        if (device == nullptr || swap_chain == nullptr || m_pd3d_command_queue_d3d12 == nullptr) {
            spdlog::info("Device or SwapChain null. DirectX 11 may be in use. Unhooking D3D12...");

            // We unhook D3D12
            if (m_d3d12_hook->unhook())
                spdlog::info("D3D12 unhooked!");
            else
                spdlog::error("Cannot unhook D3D12, this might crash.");

            m_valid = false;
            m_is_d3d12 = false;

            // We hook D3D11
            if (!hook_d3d11()) {
                spdlog::error("Failed to hook D3D11 after unhooking D3D12.");
            }
            return false;
        }

        DXGI_SWAP_CHAIN_DESC swap_desc{};
        swap_chain->GetDesc(&swap_desc);

        m_wnd = swap_desc.OutputWindow;

        m_windows_message_hook.reset();
        m_windows_message_hook = std::make_unique<WindowsMessageHook>(m_wnd);
        m_windows_message_hook->on_message = [this](auto wnd, auto msg, auto w_param, auto l_param) {
            return on_message(wnd, msg, w_param, l_param);
        };

        if (m_first_frame) {
            m_dinput_hook = std::make_unique<DInputHook>(m_wnd);
        } else {
            m_dinput_hook->set_window(m_wnd);
        }

        if (!create_rtv_descriptor_heap_d3d12()) {
            spdlog::error("Failed to create RTV Descriptor.");
            return false;
        }

        if (!create_srv_descriptor_heap_d3d12()) {
            spdlog::error("Failed to create SRV Descriptor.");
            return false;
        }

        if (!create_command_allocator_d3d12()) {
            spdlog::error("Failed to create Command Allocator.");
            return false;
        }

        if (!create_command_list_d3d12()) {
            spdlog::error("Failed to create Command List.");
            return false;
        }

        create_render_target_d3d12();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        set_imgui_style();

        ImGui::GetIO().IniFilename = "ref_ui.ini";
        
        if (!ImGui_ImplWin32_Init(m_wnd)) {
            spdlog::error("Failed to initialize ImGui ImplWin32.");
            return false;
        }

        if (!ImGui_ImplDX12_Init(device, s_NUM_FRAMES_IN_FLIGHT_D3D12, DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3d_srv_desc_heap_d3d12,
                m_pd3d_srv_desc_heap_d3d12->GetCPUDescriptorHandleForHeapStart(),
                m_pd3d_srv_desc_heap_d3d12->GetGPUDescriptorHandleForHeapStart())) {
            spdlog::error("Failed to initialize ImGui ImplDX12.");
            return false;
        }

        ImGui_ImplDX12_InvalidateDeviceObjects();
        if (!ImGui_ImplDX12_CreateDeviceObjects()) {
            spdlog::error("Failed to initialize ImGui CreateDeviceObjects.");
            return false;
        }

        /*m_target_width = m_d3d12_hook->get_display_width();
        m_target_height = m_d3d12_hook->get_display_height();

        m_render_width = m_d3d12_hook->get_render_width();
        m_render_height = m_d3d12_hook->get_render_height();*/
    } else {
        return false;
    }

    if (m_first_frame) {
        m_first_frame = false;

        spdlog::info("Starting game data initialization thread");

        // Game specific initialization stuff
        std::thread init_thread([this]() {
            m_types = std::make_unique<RETypes>();
            m_globals = std::make_unique<REGlobals>();
            m_mods = std::make_unique<Mods>();

            auto e = m_mods->on_initialize();

            if (e) {
                if (e->empty()) {
                    m_error = "An unknown error has occurred.";
                } else {
                    m_error = *e;
                }
            }

            m_game_data_initialized = true;
        });

        init_thread.detach();
    }

    return true;
}

// DirectX 11 Initialization methods

void REFramework::create_render_target_d3d11() {
    cleanup_render_target_d3d11();

    ID3D11Texture2D* back_buffer{nullptr};
    if (m_d3d11_hook->get_swap_chain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer) == S_OK) {
        m_d3d11_hook->get_device()->CreateRenderTargetView(back_buffer, NULL, &m_main_render_target_view_d3d11);
        back_buffer->Release();
    }
}

void REFramework::cleanup_render_target_d3d11() {
    if (m_main_render_target_view_d3d11 != nullptr) {
        m_main_render_target_view_d3d11->Release();
        m_main_render_target_view_d3d11 = nullptr;
    }
}

// DirectX 12 Initialization methods

bool REFramework::create_rtv_descriptor_heap_d3d12() {
    auto device = m_d3d12_hook->get_device();

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.NumDescriptors = s_NUM_BACK_BUFFERS_D3D12;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 1;
    if (device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3d_rtv_desc_heap_d3d12)) != S_OK) {
        return false;
    }

    SIZE_T rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_pd3d_rtv_desc_heap_d3d12->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < s_NUM_BACK_BUFFERS_D3D12; i++) {
        m_main_render_target_descriptor_d3d12[i] = rtv_handle;
        rtv_handle.ptr += rtv_descriptor_size;
    }

    return true;
}

bool REFramework::create_srv_descriptor_heap_d3d12() {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = 1;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (m_d3d12_hook->get_device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3d_srv_desc_heap_d3d12)) != S_OK) {
        return false;
    }

    return true;
}

bool REFramework::create_command_allocator_d3d12() {
    for (UINT i = 0; i < s_NUM_FRAMES_IN_FLIGHT_D3D12; i++) {
        auto res = m_d3d12_hook->get_device()->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frame_context_d3d12[i].command_allocator));

        if (res != S_OK) {
            return false;
        }
    }

    return true;
}

bool REFramework::create_command_list_d3d12() {
    auto res = m_d3d12_hook->get_device()->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frame_context_d3d12[0].command_allocator, NULL, IID_PPV_ARGS(&m_pd3d_command_list_d3d12));

    if (res != S_OK || m_pd3d_command_list_d3d12->Close() != S_OK) {
        return false;
    }

    return true;
}

void REFramework::cleanup_render_target_d3d12() {
    for (UINT i = 0; i < s_NUM_BACK_BUFFERS_D3D12; i++) {
        if (m_main_render_target_resource_d3d12[i]) {
            m_main_render_target_resource_d3d12[i]->Release();
            m_main_render_target_resource_d3d12[i] = NULL;
        }
    }
}

void REFramework::create_render_target_d3d12() {
    cleanup_render_target_d3d12();

    for (UINT i = 0; i < s_NUM_BACK_BUFFERS_D3D12; i++) {
        ID3D12Resource* back_buffer{nullptr};
        if (m_d3d12_hook->get_swap_chain()->GetBuffer(i, IID_PPV_ARGS(&back_buffer)) == S_OK) {
            m_d3d12_hook->get_device()->CreateRenderTargetView(back_buffer, NULL, m_main_render_target_descriptor_d3d12[i]);
            m_main_render_target_resource_d3d12[i] = back_buffer;
        }
    }
}