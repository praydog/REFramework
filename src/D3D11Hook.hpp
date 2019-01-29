#pragma once

#include <functional>

#include <d3d11.h>
#include <dxgi.h>

#include "FunctionHook.hpp"

class D3D11Hook {
public:
    typedef std::function<void(D3D11Hook&)> OnPresentFn;

    D3D11Hook() = default;
    virtual ~D3D11Hook();

    bool hook();
    bool unhook();

    void onPresent(OnPresentFn fn) { m_onPresent = fn; }

    ID3D11Device* getDevice() { return m_device; }
    IDXGISwapChain* getSwapChain() { return m_swapChain; }

protected:
    ID3D11Device* m_device{ nullptr };
    IDXGISwapChain* m_swapChain{ nullptr };
    bool m_hooked{ false };
    bool m_firstTime{ true };

    std::unique_ptr<FunctionHook> m_presentHook{};
    OnPresentFn m_onPresent{ nullptr };

    static HRESULT WINAPI present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
};
