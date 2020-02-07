#pragma once

#include <memory>
#include <vector>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "utility/FunctionHook.hpp"

class DInputHook {
public:
    DInputHook() = delete;
    DInputHook(const DInputHook& other) = delete;
    DInputHook(DInputHook&& other) = delete;
    DInputHook(HWND wnd);
    virtual ~DInputHook();

    void ignore_input() {
        m_is_ignoring_input = true;
    }

    void acknowledge_input() {
        m_is_ignoring_input = false;
    }

    auto is_ignoring_input() const {
        return m_is_ignoring_input;
    }

    auto is_valid() const {
        return m_get_device_state_hook->is_valid();
    }

    void set_window(HWND wnd) {
        m_wnd = wnd;
    }

    DInputHook& operator=(const DInputHook& other) = delete;
    DInputHook& operator=(DInputHook&& other) = delete;

private:
    HWND m_wnd;

    std::unique_ptr<FunctionHook> m_get_device_state_hook;

    bool m_is_ignoring_input;
    bool m_do_once;

    bool hook();

    HRESULT get_device_state_internal(IDirectInputDevice* device, DWORD size, LPVOID data);
    static HRESULT WINAPI get_device_state(IDirectInputDevice* device, DWORD size, LPVOID data);
};
