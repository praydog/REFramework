#include <algorithm>
#include <spdlog/spdlog.h>

#include "D3D11Hook.hpp"

using namespace std;

static D3D11Hook* g_d3d11_hook = nullptr;

D3D11Hook::~D3D11Hook() {
    unhook();
}

bool D3D11Hook::hook() {
    spdlog::info("Hooking D3D11");

    g_d3d11_hook = this;

    HWND h_wnd = GetDesktopWindow();
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = h_wnd;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, nullptr, &context))) {
        spdlog::error("Failed to create D3D11 device");
        return false;
    }

    auto present_fn = (*(uintptr_t**)swap_chain)[8];
    auto resize_buffers_fn = (*(uintptr_t**)swap_chain)[13];
    m_present_hook = std::make_unique<FunctionHook>(present_fn, (uintptr_t)&D3D11Hook::present);
    m_resize_buffers_hook = std::make_unique<FunctionHook>(resize_buffers_fn, (uintptr_t)&D3D11Hook::resize_buffers);

    device->Release();
    context->Release();
    swap_chain->Release();

    m_hooked = m_present_hook->create() && m_resize_buffers_hook->create();

    return m_hooked;
}

bool D3D11Hook::unhook() {
    return true;
}

HRESULT WINAPI D3D11Hook::present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    auto d3d11 = g_d3d11_hook;

    d3d11->m_swap_chain = swap_chain;
    swap_chain->GetDevice(__uuidof(d3d11->m_device), (void**)&d3d11->m_device);

    if (d3d11->m_on_present) {
        d3d11->m_on_present(*d3d11);
    }

    auto present_fn = d3d11->m_present_hook->get_original<decltype(D3D11Hook::present)>();

    return present_fn(swap_chain, sync_interval, flags);
}

HRESULT WINAPI D3D11Hook::resize_buffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    auto d3d11 = g_d3d11_hook;

    if (d3d11->m_on_resize_buffers) {
        d3d11->m_on_resize_buffers(*d3d11);
    }

    auto resize_buffers_fn = d3d11->m_resize_buffers_hook->get_original<decltype(D3D11Hook::resize_buffers)>();

    return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}
