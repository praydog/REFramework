#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "WindowsMessageHook.hpp"

using namespace std;

static WindowsMessageHook* g_windows_message_hook{ nullptr };
std::recursive_mutex g_proc_mutex{};

LRESULT WINAPI window_proc(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    std::lock_guard _{ g_proc_mutex };

    if (g_windows_message_hook == nullptr) {
        return 0;
    }

    // Call our onMessage callback.
    auto& on_message = g_windows_message_hook->on_message;

    if (on_message) {
        // If it returns false we don't call the original window procedure.
        if (!on_message(wnd, message, w_param, l_param)) {
            return DefWindowProc(wnd, message, w_param, l_param);
        }
    }

    // Call the original message procedure.
    return CallWindowProc(g_windows_message_hook->get_original(), wnd, message, w_param, l_param);
}

WindowsMessageHook::WindowsMessageHook(HWND wnd)
    : m_wnd{ wnd },
    m_original_proc{ nullptr }
{
    spdlog::info("Initializing WindowsMessageHook");

    g_windows_message_hook = this;

    // Save the original window procedure.
    m_original_proc = (WNDPROC)GetWindowLongPtr(m_wnd, GWLP_WNDPROC);

    // Set it to our "hook" procedure.
    SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)&window_proc);

    spdlog::info("Hooked Windows message handler");
}

WindowsMessageHook::~WindowsMessageHook() {
    std::lock_guard _{ g_proc_mutex };

    remove();
    g_windows_message_hook = nullptr;
}

bool WindowsMessageHook::remove() {
    // Don't attempt to restore invalid original window procedures.
    if (m_original_proc == nullptr || m_wnd == nullptr) {
        return true;
    }

    // Restore the original window procedure.
    SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)m_original_proc);

    // Invalidate this message hook.
    m_wnd = nullptr;
    m_original_proc = nullptr;

    return true;
}