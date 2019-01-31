#pragma once

#include <functional>

#include <d3d11.h>
#include <dxgi.h>

#include "FunctionHook.hpp"

class D3D11Hook {
public:
    typedef std::function<void(D3D11Hook&)> OnPresentFn;
    typedef std::function<void(D3D11Hook&)> OnResizeBuffersFn;

    D3D11Hook() = default;
    virtual ~D3D11Hook();

    bool hook();
    bool unhook();

    void onPresent(OnPresentFn fn) { m_onPresent = fn; }
    void onResizeBuffers(OnResizeBuffersFn fn) { m_onResizeBuffers = fn; }

    ID3D11Device* getDevice() { return m_device; }
    IDXGISwapChain* getSwapChain() { return m_swapChain; }

protected:
    ID3D11Device* m_device{ nullptr };
    IDXGISwapChain* m_swapChain{ nullptr };
    bool m_hooked{ false };

    std::unique_ptr<FunctionHook> m_presentHook{};
    std::unique_ptr<FunctionHook> m_resizeBuffersHook{};
    OnPresentFn m_onPresent{ nullptr };
    OnResizeBuffersFn m_onResizeBuffers{ nullptr };

    static HRESULT WINAPI present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    static HRESULT WINAPI resizeBuffers(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
};
