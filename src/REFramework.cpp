#include <spdlog/sinks/basic_file_sink.h>

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>
#include <imgui/examples/imgui_impl_dx11.h>

#include "utility/Module.hpp"

#include "sdk/REGlobals.hpp"
#include "Mods.hpp"

#include "LicenseStrings.hpp"
#include "REFramework.hpp"

// Commented out in original ImGui code
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unique_ptr<REFramework> g_framework{};

REFramework::REFramework()
    : m_logger{ spdlog::basic_logger_mt("REFramework", "re2_framework_log.txt", true) },
    m_gameModule{ GetModuleHandle(0) }
{
    spdlog::set_default_logger(m_logger);
    spdlog::flush_on(spdlog::level::info);
    spdlog::info("REFramework entry");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    m_d3d11Hook = std::make_unique<D3D11Hook>();
    m_d3d11Hook->onPresent([this](D3D11Hook& hook) { onFrame(); });
    m_d3d11Hook->onResizeBuffers([this](D3D11Hook& hook) { onReset(); });

    if (m_valid = m_d3d11Hook->hook()) {
        spdlog::info("Hooked D3D11");
    }
}

REFramework::~REFramework() {

}

void REFramework::onFrame() {
    spdlog::debug("OnFrame");

    if (!m_initialized) {
        if (!initialize()) {
            spdlog::error("Failed to initialize REFramework");
            return;
        }

        spdlog::info("REFramework initialized");
        m_initialized = true;
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_error.empty() && m_gameDataInitialized) {
        m_mods->onFrame();
    }

    drawUI();

    ImGui::EndFrame();
    ImGui::Render();

    ID3D11DeviceContext* context = nullptr;
    m_d3d11Hook->getDevice()->GetImmediateContext(&context);

    context->OMSetRenderTargets(1, &m_mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void REFramework::onReset() {
    spdlog::info("Reset!");

    // Crashes if we don't release it at this point.
    cleanupRenderTarget();
    m_initialized = false;
}

bool REFramework::onMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (!m_initialized) {
        return true;
    }

    if (m_drawUI && ImGui_ImplWin32_WndProcHandler(wnd, message, wParam, lParam) != 0) {
        // If the user is interacting with the UI we block the message from going to the game.
        auto& io = ImGui::GetIO();

        if (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput) {
            return false;
        }
    }

    return true;
}

// this is unfortunate.
void REFramework::onDirectInputKeys(const std::array<uint8_t, 256>& keys) {
    if (keys[m_menuKey] && m_lastKeys[m_menuKey] == 0) {
        std::lock_guard _{ m_inputMutex };
        m_drawUI = !m_drawUI;

        // Save the config if we close the UI
        if (!m_drawUI && m_gameDataInitialized) {
            saveConfig();
        }
    }

    m_lastKeys = keys;
}

void REFramework::saveConfig() {
    spdlog::info("Saving config re2_fw_config.txt");

    utility::Config cfg{};

    for (auto& mod : m_mods->getMods()) {
        mod->onConfigSave(cfg);
    }

    if (!cfg.save("re2_fw_config.txt")) {
        spdlog::info("Failed to save config");
        return;
    }

    spdlog::info("Saved config");
}

void REFramework::drawUI() {
    std::lock_guard _{ m_inputMutex };

    if (!m_drawUI) {
        m_dinputHook->acknowledgeInput();
        ImGui::GetIO().MouseDrawCursor = false;
        return;
    }

    auto& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard) {
        m_dinputHook->ignoreInput();
    }
    else {
        m_dinputHook->acknowledgeInput();
    }

    ImGui::GetIO().MouseDrawCursor = true;

    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_::ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_::ImGuiCond_Once);

    ImGui::Begin("REFramework", &m_drawUI);
    ImGui::Text("Menu Key: Insert");

    drawAbout();

    if (m_error.empty() && m_gameDataInitialized) {
        m_mods->onDrawUI();
    }
    else if (!m_gameDataInitialized) {
        ImGui::TextWrapped("REFramework is currently initializing...");
    }
    else if(!m_error.empty()) {
        ImGui::TextWrapped("REFramework error: %s", m_error.c_str());
    }

    ImGui::End();
}

void REFramework::drawAbout() {
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

        ImGui::TreePop();
    }

    ImGui::TreePop();
}

bool REFramework::initialize() {
    if (m_initialized) {
        return true;
    }

    spdlog::info("Attempting to initialize");

    auto device = m_d3d11Hook->getDevice();
    auto swapChain = m_d3d11Hook->getSwapChain();

    // Wait.
    if (device == nullptr || swapChain == nullptr) {
        spdlog::info("Device or SwapChain null. DirectX 12 may be in use. A crash may occur.");
        return false;
    }

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    swapChain->GetDesc(&swapDesc);

    m_wnd = swapDesc.OutputWindow;

    // Explicitly call destructor first
    m_windowsMessageHook.reset();
    m_windowsMessageHook = std::make_unique<WindowsMessageHook>(m_wnd);
    m_windowsMessageHook->onMessage = [this](auto wnd, auto msg, auto wParam, auto lParam) {
        return onMessage(wnd, msg, wParam, lParam);
    };

    // just do this instead of rehooking because there's no point.
    if (m_firstFrame) {
        m_dinputHook = std::make_unique<DInputHook>(m_wnd);
    }
    else {
        m_dinputHook->setWindow(m_wnd);
    }

    spdlog::info("Creating render target");

    createRenderTarget();

    spdlog::info("Window Handle: {0:x}", (uintptr_t)m_wnd);
    spdlog::info("Initializing ImGui");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

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

    ImGui::StyleColorsDark();

    if (m_firstFrame) {
        m_firstFrame = false;

        spdlog::info("Starting game data initialization thread");

        // Game specific initialization stuff
        std::thread initThread([this]() {
            m_globals = std::make_unique<REGlobals>();
            m_mods = std::make_unique<Mods>();

            if (!m_mods->onInitialize() && m_error.empty()) {
                m_error = "An unknown error has occurred.";
            }

            m_gameDataInitialized = true;
        });

        initThread.detach();
    }

    return true;
}

void REFramework::createRenderTarget() {
    cleanupRenderTarget();

    ID3D11Texture2D* backBuffer{ nullptr };
    if (m_d3d11Hook->getSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer) == S_OK) {
        m_d3d11Hook->getDevice()->CreateRenderTargetView(backBuffer, NULL, &m_mainRenderTargetView);
        backBuffer->Release();
    }
}

void REFramework::cleanupRenderTarget() {
    if (m_mainRenderTargetView != nullptr) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

