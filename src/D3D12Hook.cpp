#include <spdlog/spdlog.h>

#include "D3D12Hook.hpp"

static D3D12Hook* g_d3d12_hook = nullptr;

D3D12Hook::~D3D12Hook() {
    unhook();
}

bool D3D12Hook::hook() {
    spdlog::info("Hooking D3D12");

    g_d3d12_hook = this;

    WNDCLASSEX windowClass;
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = TEXT("DX12");
    windowClass.hIconSm = NULL;

    ::RegisterClassEx(&windowClass);

    HWND h_wnd = ::CreateWindow(windowClass.lpszClassName, TEXT("DX Dummy Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

    IDXGISwapChain1* swap_chain1{ nullptr };
    IDXGISwapChain3* swap_chain{ nullptr };
    ID3D12Device* device{ nullptr };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1;

    ZeroMemory(&swap_chain_desc1, sizeof(swap_chain_desc1));

    swap_chain_desc1.BufferCount = 3;
    swap_chain_desc1.Width = 0;
    swap_chain_desc1.Height = 0;
    swap_chain_desc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc1.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc1.SampleDesc.Count = 1;
    swap_chain_desc1.SampleDesc.Quality = 0;
    swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc1.Stereo = FALSE;

    // Manually get D3D12CreateDevice export because the user may be running Windows 7
    const auto d3d12_module = LoadLibraryA("d3d12.dll");
    if (d3d12_module == nullptr) {
        spdlog::error("Failed to load d3d12.dll");
        return false;
    }

    auto d3d12_create_device = (decltype(D3D12CreateDevice)*)GetProcAddress(d3d12_module, "D3D12CreateDevice");
    if (d3d12_create_device == nullptr) {
        spdlog::error("Failed to get D3D12CreateDevice export");
        return false;
    }

    if (FAILED(d3d12_create_device(nullptr, feature_level, IID_PPV_ARGS(&device)))) {
        spdlog::error("Failed to create D3D12 Dummy device");
        return false;
    }

    // Manually get CreateDXGIFactory export because the user may be running Windows 7
    const auto dxgi_module = LoadLibraryA("dxgi.dll");
    if (dxgi_module == nullptr) {
        spdlog::error("Failed to load dxgi.dll");
        return false;
    }

    auto create_dxgi_factory = (decltype(CreateDXGIFactory)*)GetProcAddress(dxgi_module, "CreateDXGIFactory");

    if (create_dxgi_factory == nullptr) {
        spdlog::error("Failed to get CreateDXGIFactory export");
        return false;
    }

    IDXGIFactory4* factory{ nullptr };
    if (FAILED(create_dxgi_factory(IID_PPV_ARGS(&factory)))) {
        spdlog::error("Failed to create D3D12 Dummy DXGI Factory");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc;
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = 0;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;

    ID3D12CommandQueue* command_queue{ nullptr };
    if (FAILED(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)))) {
        spdlog::error("Failed to create D3D12 Dummy Command Queue");
        return false;
    }

    if (FAILED(factory->CreateSwapChainForHwnd(command_queue, h_wnd, &swap_chain_desc1, NULL, NULL, &swap_chain1))) {
        spdlog::error("Failed to create D3D12 Dummy DXGI SwapChain");
        return false;
    }

    if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain)))) {
        spdlog::error("Failed to retrieve D3D12 DXGI SwapChain");
        return false;
    }

    auto present_fn = (*(uintptr_t**)swap_chain)[8]; // Present
    auto resize_buffers_fn = (*(uintptr_t**)swap_chain)[13]; // ResizeBuffers
    auto resize_target_fn = (*(uintptr_t**)swap_chain)[14]; // ResizeTarget
    auto create_swap_chain_fn = (*(uintptr_t**)factory)[15]; // CreateSwapChainForHwnd

    m_present_hook = std::make_unique<FunctionHook>(present_fn, (uintptr_t)&D3D12Hook::present);
    m_resize_buffers_hook = std::make_unique<FunctionHook>(resize_buffers_fn, (uintptr_t)&D3D12Hook::resize_buffers);
    m_resize_target_hook = std::make_unique<FunctionHook>(resize_target_fn, (uintptr_t)&D3D12Hook::resize_target);
    m_create_swap_chain_hook = std::make_unique<FunctionHook>(create_swap_chain_fn, (uintptr_t)&D3D12Hook::create_swap_chain);

    device->Release();
    command_queue->Release();
    factory->Release();
    swap_chain1->Release();
    swap_chain->Release();

    ::DestroyWindow(h_wnd);
    ::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

    m_hooked = m_present_hook->create() && m_resize_buffers_hook->create() && m_resize_target_hook->create() && m_create_swap_chain_hook->create();

    return m_hooked;
}

bool D3D12Hook::unhook() {
    if (m_present_hook->remove() && m_resize_buffers_hook->remove() && m_resize_target_hook->remove() && m_create_swap_chain_hook->remove()) {
        m_hooked = false;
        return true;
    }

    return false;
}

HRESULT WINAPI D3D12Hook::present(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags) {
    auto d3d12 = g_d3d12_hook;

    d3d12->m_swap_chain = swap_chain;
    swap_chain->GetDevice(IID_PPV_ARGS(&d3d12->m_device));

	// This line must be called before calling our detour function because we might have to unhook the function inside our detour.
    auto present_fn = d3d12->m_present_hook->get_original<decltype(D3D12Hook::present)>();

    if (d3d12->m_on_present) {
        d3d12->m_on_present(*d3d12);
    }

    auto result = present_fn(swap_chain, sync_interval, flags);

    if (d3d12->m_on_post_present) {
        d3d12->m_on_post_present(*d3d12);
    }
    
    return result;
}

HRESULT WINAPI D3D12Hook::resize_buffers(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    auto d3d12 = g_d3d12_hook;

    d3d12->m_display_width = width;
    d3d12->m_display_height = height;

    if (d3d12->m_on_resize_buffers) {
        d3d12->m_on_resize_buffers(*d3d12);
    }

    auto resize_buffers_fn = d3d12->m_resize_buffers_hook->get_original<decltype(D3D12Hook::resize_buffers)>();

    return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}

HRESULT WINAPI D3D12Hook::resize_target(IDXGISwapChain3* swap_chain, const DXGI_MODE_DESC* new_target_parameters)
{
    auto d3d12 = g_d3d12_hook;

    d3d12->m_render_width = new_target_parameters->Width;
    d3d12->m_render_height = new_target_parameters->Height;

    if (d3d12->m_on_resize_target) {
        d3d12->m_on_resize_target(*d3d12);
    }

    auto resize_buffers_fn = d3d12->m_resize_target_hook->get_original<decltype(D3D12Hook::resize_target)>();

    return resize_buffers_fn(swap_chain, new_target_parameters);
}

HRESULT WINAPI D3D12Hook::create_swap_chain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_fullscreen_desc, IDXGIOutput* p_restrict_to_output, IDXGISwapChain** swap_chain)
{
    auto d3d12 = g_d3d12_hook;

    d3d12->m_command_queue = (ID3D12CommandQueue*)device;
    
    if (d3d12->m_on_create_swap_chain) {
        d3d12->m_on_create_swap_chain(*d3d12);
    }

    auto create_swap_chain_fn = d3d12->m_create_swap_chain_hook->get_original<decltype(D3D12Hook::create_swap_chain)>();

    return create_swap_chain_fn(factory, device, hwnd, desc, p_fullscreen_desc, p_restrict_to_output, swap_chain);
}

