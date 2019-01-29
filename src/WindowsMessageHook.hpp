#pragma once

#include <functional>

#include <Windows.h>

// This type of hook replaces a windows message procedure so that it can intercept
// messages sent to the window.
class WindowsMessageHook {
public:
    std::function<bool(HWND, UINT, WPARAM, LPARAM)> onMessage;

    WindowsMessageHook() = delete;
    WindowsMessageHook(const WindowsMessageHook& other) = delete;
    WindowsMessageHook(WindowsMessageHook&& other) = delete;
    WindowsMessageHook(HWND wnd);
    virtual ~WindowsMessageHook();

    // This gets called automatically by the destructor but you can call it
    // explicitly if you need to remove the message hook for some reason.
    bool remove();

    auto isValid() const {
        return m_originalProc != nullptr;
    }

    auto getOriginal() const {
        return m_originalProc;
    }

    WindowsMessageHook& operator=(const WindowsMessageHook& other) = delete;
    WindowsMessageHook& operator=(const WindowsMessageHook&& other) = delete;

private:
    HWND m_wnd;
    WNDPROC m_originalProc;
};