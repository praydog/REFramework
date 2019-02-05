#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "WindowsMessageHook.hpp"

using namespace std;

static WindowsMessageHook* g_windowsMessageHook{ nullptr };
std::recursive_mutex g_procMutex{};

LRESULT WINAPI windowProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::lock_guard _{ g_procMutex };

    if (g_windowsMessageHook == nullptr) {
        return 0;
    }

    // Call our onMessage callback.
    auto& onMessage = g_windowsMessageHook->onMessage;

    if (onMessage) {
        // If it returns false we don't call the original window procedure.
        if (!onMessage(wnd, message, wParam, lParam)) {
            return DefWindowProc(wnd, message, wParam, lParam);
        }
    }

    // Call the original message procedure.
    return CallWindowProc(g_windowsMessageHook->getOriginal(), wnd, message, wParam, lParam);
}

WindowsMessageHook::WindowsMessageHook(HWND wnd)
    : m_wnd{ wnd },
    m_originalProc{ nullptr }
{
    spdlog::info("Initializing WindowsMessageHook");

    g_windowsMessageHook = this;

    // Save the original window procedure.
    m_originalProc = (WNDPROC)GetWindowLongPtr(m_wnd, GWLP_WNDPROC);

    // Set it to our "hook" procedure.
    SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)&windowProc);

    spdlog::info("Hooked Windows message handler");
}

WindowsMessageHook::~WindowsMessageHook() {
    std::lock_guard _{ g_procMutex };

    remove();
    g_windowsMessageHook = nullptr;
}

bool WindowsMessageHook::remove() {
    // Don't attempt to restore invalid original window procedures.
    if (m_originalProc == nullptr || m_wnd == nullptr) {
        return true;
    }

    // Restore the original window procedure.
    SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)m_originalProc);

    // Invalidate this message hook.
    m_wnd = nullptr;
    m_originalProc = nullptr;

    return true;
}