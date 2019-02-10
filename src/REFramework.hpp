#pragma once

#include <spdlog/spdlog.h>

class Mods;
class REGlobals;

#include "D3D11Hook.hpp"
#include "WindowsMessageHook.hpp"
#include "DInputHook.hpp"

// Global facilitator
class REFramework {
public:
    REFramework();
    virtual ~REFramework();

    bool isValid() const {
        return m_valid;
    }

    const auto& getMods() const {
        return m_mods;
    }

    const auto& getKeyboardState() const {
        return m_lastKeys;
    }

    const auto& getGlobals() const {
        return m_globals;
    }

    Address getModule() const {
        return m_gameModule;
    }

    void signalError(std::string_view err) {
        m_error = err;
    }

    bool isReady() const {
        return m_gameDataInitialized;
    }

    void onFrame();
    void onReset();
    bool onMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
    void onDirectInputKeys(const std::array<uint8_t, 256>& keys);

    void saveConfig();

private:
    void drawUI();
    void drawAbout();

    bool initialize();
    void createRenderTarget();
    void cleanupRenderTarget();

    bool m_firstFrame{ true };
    bool m_valid{ false };
    bool m_initialized{ false };
    bool m_drawUI{ true };
    std::atomic<bool> m_gameDataInitialized{ false };

    std::mutex m_inputMutex{};
    
    HWND m_wnd{ 0 };
    HMODULE m_gameModule{ 0 };
    uint8_t m_menuKey{ DIK_INSERT };

    std::array<uint8_t, 256> m_lastKeys{ 0 };
    std::unique_ptr<D3D11Hook> m_d3d11Hook{};
    std::unique_ptr<WindowsMessageHook> m_windowsMessageHook;
    std::unique_ptr<DInputHook> m_dinputHook;
    std::shared_ptr<spdlog::logger> m_logger;

    std::string m_error{ "" };

    // Game-specific stuff
    std::unique_ptr<class Mods> m_mods;
    std::unique_ptr<class REGlobals> m_globals;

    ID3D11RenderTargetView* m_mainRenderTargetView{ nullptr };
};

extern std::unique_ptr<REFramework> g_framework;