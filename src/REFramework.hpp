#pragma once

#include <spdlog/spdlog.h>

#include "D3D11Hook.hpp"
#include "WindowsMessageHook.hpp"
#include "DInputHook.hpp"

#include "Mods.hpp"

// Global facilitator
class REFramework {
public:
    REFramework();
    virtual ~REFramework() {};

    bool isValid() const {
        return m_valid;
    }

    const auto& getMods() const {
        return m_mods;
    }

    const auto& getKeyboardState() const {
        return m_lastKeys;
    }

    void onFrame();
    void onReset();
    bool onMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
    void onDirectInputKeys(const std::array<uint8_t, 256>& keys);

private:
    void drawUI();
    void drawAbout();

    bool initialize();
    void createRenderTarget();
    void cleanupRenderTarget();

    bool m_valid{ false };
    bool m_initialized{ false };
    bool m_drawUI{ true };

    HWND m_wnd{ 0 };
    uint8_t m_menuKey{ DIK_INSERT };

    std::array<uint8_t, 256> m_lastKeys{ 0 };
    std::unique_ptr<D3D11Hook> m_d3d11Hook{};
    std::unique_ptr<WindowsMessageHook> m_windowsMessageHook;
    std::unique_ptr<DInputHook> m_dinputHook;
    std::shared_ptr<spdlog::logger> m_logger;

    Mods m_mods;

    ID3D11RenderTargetView* m_mainRenderTargetView{ nullptr };
};

extern std::unique_ptr<REFramework> g_framework;