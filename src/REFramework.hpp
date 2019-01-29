#pragma once

#include <spdlog/spdlog.h>

#include "D3D11Hook.hpp"
#include "WindowsMessageHook.hpp"

// Global facilitator
class REFramework {
public:
    REFramework();
    virtual ~REFramework() {};

    bool isValid() const {
        return m_valid;
    }

    void onFrame();
    bool onMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    void drawUI();

    bool initialize();
    void createRenderTarget();
    void cleanupRenderTarget();

    bool m_valid{ false };
    bool m_initialized{ false };

    IDXGISwapChain* m_lastSwapChain{ nullptr };

    HWND m_wnd{ 0 };

    std::unique_ptr<D3D11Hook> m_d3d11Hook{};
    std::unique_ptr<WindowsMessageHook> m_windowsMessageHook;
    std::shared_ptr<spdlog::logger> m_logger;

    ID3D11RenderTargetView* m_mainRenderTargetView{ nullptr };
};

extern std::unique_ptr<REFramework> g_framework;