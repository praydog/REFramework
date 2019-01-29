#include <spdlog/sinks/basic_file_sink.h>

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>
#include <imgui/examples/imgui_impl_dx11.h>
#include <vectormath/vectormath.hpp>

#include "ReClass.hpp"

#include "REFramework.hpp"

// Commented out in original ImGui code
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unique_ptr<REFramework> g_framework{};

REFramework::REFramework() 
    : m_logger{ spdlog::basic_logger_mt("REFramework", "re2_framework_log.txt", true) }
{
    spdlog::flush_on(spdlog::level::info);
    spdlog::set_default_logger(m_logger);
    spdlog::info("REFramework entry");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    m_d3d11Hook = std::make_unique<D3D11Hook>();
    m_d3d11Hook->onPresent([this](D3D11Hook& hook) { onFrame(); });

    if (m_valid = m_d3d11Hook->hook()) {
        spdlog::info("Hooked D3D11");
    }
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

    auto swapChain = m_d3d11Hook->getSwapChain();

    if (m_lastSwapChain != nullptr && m_lastSwapChain != swapChain) {
        spdlog::info("SwapChain modified, reinitializing render target.");
        cleanupRenderTarget();
        createRenderTarget();
    }

    m_lastSwapChain = swapChain;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    drawUI();

    ImGui::EndFrame();
    ImGui::Render();

    ID3D11DeviceContext* context = nullptr;
    m_d3d11Hook->getDevice()->GetImmediateContext(&context);

    context->OMSetRenderTargets(1, &m_mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool REFramework::onMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(wnd, message, wParam, lParam) != 0) {
        // If the user is interacting with the UI we block the message from going to the game.
        auto& io = ImGui::GetIO();

        if (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput) {
            return false;
        }
    }

    return true;
}

void REFramework::drawUI() {
    auto& io = ImGui::GetIO();

    ImGui::GetIO().MouseDrawCursor = true;

    ImGui::ShowDemoWindow();
}

bool REFramework::initialize() {
    if (m_initialized) {
        return true;
    }

    spdlog::info("Attempting to initialize");
    m_logger->flush();

    auto device = m_d3d11Hook->getDevice();
    auto swapChain = m_d3d11Hook->getSwapChain();

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);
    
    DXGI_SWAP_CHAIN_DESC swapDesc{};
    swapChain->GetDesc(&swapDesc);

    m_wnd = swapDesc.OutputWindow;
    m_windowsMessageHook = std::make_unique<WindowsMessageHook>(m_wnd);
    m_windowsMessageHook->onMessage = [this](auto wnd, auto msg, auto wParam, auto lParam) {
        return onMessage(wnd, msg, wParam, lParam);
    };

    spdlog::info("Creating render target");
    m_logger->flush();

    createRenderTarget();

    spdlog::info("Window Handle: {0:x}", (uintptr_t)m_wnd);
    m_logger->flush();
    spdlog::info("Initializing ImGui");
    m_logger->flush();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    spdlog::info("Initializing ImGui Win32");
    m_logger->flush();

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

    return true;
}

void REFramework::createRenderTarget() {
    ID3D11Texture2D* backBuffer{ nullptr };
    m_d3d11Hook->getSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    m_d3d11Hook->getDevice()->CreateRenderTargetView(backBuffer, NULL, &m_mainRenderTargetView);
    backBuffer->Release();
}

void REFramework::cleanupRenderTarget() {
    if (m_mainRenderTargetView != nullptr) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

