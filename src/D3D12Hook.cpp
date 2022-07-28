#include <thread>
#include <future>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <utility/Thread.hpp>
#include <utility/Module.hpp>

#include "REFramework.hpp"

#include "D3D12Hook.hpp"

static D3D12Hook* g_d3d12_hook = nullptr;

D3D12Hook::~D3D12Hook() {
    unhook();
}

bool D3D12Hook::hook() {
    spdlog::info("Hooking D3D12");

    g_d3d12_hook = this;

    IDXGISwapChain1* swap_chain1{ nullptr };
    IDXGISwapChain3* swap_chain{ nullptr };
    ID3D12Device* device{ nullptr };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1;

    ZeroMemory(&swap_chain_desc1, sizeof(swap_chain_desc1));

    swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc1.BufferCount = 2;
    swap_chain_desc1.SampleDesc.Count = 1;
    swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    swap_chain_desc1.Width = 1;
    swap_chain_desc1.Height = 1;

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

    spdlog::info("Creating dummy device");

    // Get the original on-disk bytes of the D3D12CreateDevice export
    const auto original_bytes = utility::get_original_bytes(d3d12_create_device);

    // Temporarily unhook D3D12CreateDevice
    // it allows compatibility with ReShade and other overlays that hook it
    // this is just a dummy device anyways, we don't want the other overlays to be able to use it
    if (original_bytes) {
        spdlog::info("D3D12CreateDevice appears to be hooked, temporarily unhooking");

        std::vector<uint8_t> hooked_bytes(original_bytes->size());
        memcpy(hooked_bytes.data(), d3d12_create_device, original_bytes->size());

        ProtectionOverride protection_override{ d3d12_create_device, original_bytes->size(), PAGE_EXECUTE_READWRITE };
        memcpy(d3d12_create_device, original_bytes->data(), original_bytes->size());
        
        if (FAILED(d3d12_create_device(nullptr, feature_level, IID_PPV_ARGS(&device)))) {
            spdlog::error("Failed to create D3D12 Dummy device");
            memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
            return false;
        }

        spdlog::info("Restoring hooked bytes for D3D12CreateDevice");
        memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
    } else { // D3D12CreateDevice is not hooked
        if (FAILED(d3d12_create_device(nullptr, feature_level, IID_PPV_ARGS(&device)))) {
            spdlog::error("Failed to create D3D12 Dummy device");
            return false;
        }
    }

    spdlog::info("Dummy device: {:x}", (uintptr_t)device);

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

    spdlog::info("Creating dummy DXGI factory");

    IDXGIFactory4* factory{ nullptr };
    if (FAILED(create_dxgi_factory(IID_PPV_ARGS(&factory)))) {
        spdlog::error("Failed to create D3D12 Dummy DXGI Factory");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = 0;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;

    spdlog::info("Creating dummy command queue");

    ID3D12CommandQueue* command_queue{ nullptr };
    if (FAILED(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)))) {
        spdlog::error("Failed to create D3D12 Dummy Command Queue");
        return false;
    }

    spdlog::info("Creating dummy swapchain");

    // used in CreateSwapChainForHwnd fallback
    HWND hwnd = 0;
    WNDCLASSEX wc{};

    auto init_dummy_window = [&]() {
        // fallback to CreateSwapChainForHwnd
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = DefWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = TEXT("REFRAMEWORK_DX12_DUMMY");
        wc.hIconSm = NULL;

        ::RegisterClassEx(&wc);

        hwnd = ::CreateWindow(wc.lpszClassName, TEXT("REF DX Dummy Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);

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
    };

    std::vector<std::function<bool ()>> swapchain_attempts{
        // we call CreateSwapChainForComposition instead of CreateSwapChainForHwnd
        // because some overlays will have hooks on CreateSwapChainForHwnd
        // and all we're doing is creating a dummy swapchain
        // we don't want to screw up the overlay
        [&]() {
            return !FAILED(factory->CreateSwapChainForComposition(command_queue, &swap_chain_desc1, nullptr, &swap_chain1));
        },
        [&]() {
            init_dummy_window();

            return !FAILED(factory->CreateSwapChainForHwnd(command_queue, hwnd, &swap_chain_desc1, nullptr, nullptr, &swap_chain1));
        },
        [&]() {
            return !FAILED(factory->CreateSwapChainForHwnd(command_queue, GetDesktopWindow(), &swap_chain_desc1, nullptr, nullptr, &swap_chain1));
        },
    };

    bool any_succeed = false;

    for (auto i = 0; i < swapchain_attempts.size(); i++) {
        auto& attempt = swapchain_attempts[i];
        
        try {
            spdlog::info("Trying swapchain attempt {}", i);

            if (attempt()) {
                spdlog::info("Created dummy swapchain on attempt {}", i);
                any_succeed = true;
                break;
            }
        } catch (std::exception& e) {
            spdlog::error("Failed to create dummy swapchain on attempt {}: {}", i, e.what());
        } catch(...) {
            spdlog::error("Failed to create dummy swapchain on attempt {}: unknown exception", i);
        }

        spdlog::error("Attempt {} failed", i);
    }

    if (!any_succeed) {
        spdlog::error("Failed to create D3D12 Dummy Swap Chain");

        if (hwnd) {
            ::DestroyWindow(hwnd);
        }

        if (wc.lpszClassName != nullptr) {
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        }

        return false;
    }

    spdlog::info("Querying dummy swapchain");

    if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain)))) {
        spdlog::error("Failed to retrieve D3D12 DXGI SwapChain");
        return false;
    }

    spdlog::info("Finding command queue offset");

    // Find the command queue offset in the swapchain
    for (auto i = 0; i < 512 * sizeof(void*); i += sizeof(void*)) {
        const auto base = (uintptr_t)swap_chain1 + i;

        // reached the end
        if (IsBadReadPtr((void*)base, sizeof(void*))) {
            break;
        }

        auto data = *(ID3D12CommandQueue**)base;

        if (data == command_queue) {
            m_command_queue_offset = i;
            spdlog::info("Found command queue offset: {:x}", i);
            break;
        }
    }

    // Scan throughout the swapchain for a valid pointer to scan through
    // this is usually only necessary for Proton
    if (m_command_queue_offset == 0) {
        for (auto base = 0; base < 512 * sizeof(void*); base += sizeof(void*)) {
            const auto pre_scan_base = (uintptr_t)swap_chain1 + base;

            // reached the end
            if (IsBadReadPtr((void*)pre_scan_base, sizeof(void*))) {
                break;
            }

            const auto scan_base = *(uintptr_t*)pre_scan_base;

            if (scan_base == 0 || IsBadReadPtr((void*)scan_base, sizeof(void*))) {
                continue;
            }

            for (auto i = 0; i < 512 * sizeof(void*); i += sizeof(void*)) {
                const auto pre_data = scan_base + i;

                if (IsBadReadPtr((void*)pre_data, sizeof(void*))) {
                    break;
                }

                auto data = *(ID3D12CommandQueue**)pre_data;

                if (data == command_queue) {
                    m_using_proton_swapchain = true;
                    m_command_queue_offset = i;
                    m_proton_swapchain_offset = base;

                    spdlog::info("Proton potentially detected");
                    spdlog::info("Found command queue offset: {:x}", i);
                    break;
                }
            }

            if (m_using_proton_swapchain) {
                break;
            }
        }
    }

    if (m_command_queue_offset == 0) {
        spdlog::error("Failed to find command queue offset");
        return false;
    }

    utility::ThreadSuspender suspender{};

    try {
        spdlog::info("Initializing hooks");

        m_present_hook.reset();
        m_resize_buffers_hook.reset();
        m_resize_target_hook.reset();

        auto& present_fn = (*(void***)swap_chain)[8]; // Present
        auto& resize_buffers_fn = (*(void***)swap_chain)[13]; // ResizeBuffers
        auto& resize_target_fn = (*(void***)swap_chain)[14]; // ResizeTarget
        auto& create_swap_chain_fn = (*(void***)factory)[15]; // CreateSwapChainForHwnd

        m_present_hook = std::make_unique<PointerHook>(&present_fn, (void*)&D3D12Hook::present);
        m_resize_buffers_hook = std::make_unique<PointerHook>(&resize_buffers_fn, (void*)&D3D12Hook::resize_buffers);
        m_resize_target_hook = std::make_unique<PointerHook>(&resize_target_fn, (void*)&D3D12Hook::resize_target);
        //m_create_swap_chain_hook = std::make_unique<FunctionHook>(create_swap_chain_fn, (uintptr_t)&D3D12Hook::create_swap_chain);

        m_hooked = true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize hooks: {}", e.what());
        m_hooked = false;
    }

    suspender.resume();

    device->Release();
    command_queue->Release();
    factory->Release();
    swap_chain1->Release();
    swap_chain->Release();

    if (hwnd) {
        ::DestroyWindow(hwnd);
    }

    if (wc.lpszClassName != nullptr) {
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    return m_hooked;
}

bool D3D12Hook::unhook() {
    if (!m_hooked) {
        return true;
    }

    spdlog::info("Unhooking D3D12");

    if (m_present_hook->remove() && m_resize_buffers_hook->remove() && m_resize_target_hook->remove() /*&& m_create_swap_chain_hook->remove()*/) {
        m_hooked = false;
        return true;
    }

    return false;
}

thread_local bool g_inside_present = false;
HRESULT last_present_result = S_OK;

HRESULT WINAPI D3D12Hook::present(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto d3d12 = g_d3d12_hook;

    d3d12->m_inside_present = true;
    d3d12->m_swap_chain = swap_chain;

    swap_chain->GetDevice(IID_PPV_ARGS(&d3d12->m_device));

    if (d3d12->m_device != nullptr) {
        if (d3d12->m_using_proton_swapchain) {
            const auto real_swapchain = *(uintptr_t*)((uintptr_t)swap_chain + d3d12->m_proton_swapchain_offset);
            d3d12->m_command_queue = *(ID3D12CommandQueue**)(real_swapchain + d3d12->m_command_queue_offset);
        } else {
            d3d12->m_command_queue = *(ID3D12CommandQueue**)((uintptr_t)swap_chain + d3d12->m_command_queue_offset);
        }
    }

    if (d3d12->m_swapchain_0 == nullptr) {
        d3d12->m_swapchain_0 = swap_chain;
    } else if (d3d12->m_swapchain_1 == nullptr && swap_chain != d3d12->m_swapchain_0) {
        d3d12->m_swapchain_1 = swap_chain;
    }

	// This line must be called before calling our detour function because we might have to unhook the function inside our detour.
    auto present_fn = d3d12->m_present_hook->get_original<decltype(D3D12Hook::present)*>();

    // Restore the original bytes
    // if an infinite loop occurs, this will prevent the game from crashing
    // while keeping our hook intact
    if (g_inside_present) {
        auto original_bytes = utility::get_original_bytes(Address{present_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{present_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(present_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Present fixed");
        }

        return last_present_result;
    }

    if (d3d12->m_on_present) {
        d3d12->m_on_present(*d3d12);
    }

    g_inside_present = true;
    
    if (!d3d12->m_ignore_next_present) {
        last_present_result = present_fn(swap_chain, sync_interval, flags);
    } else {
        last_present_result = S_OK;
        d3d12->m_ignore_next_present = false;
    }

    g_inside_present = false;

    if (d3d12->m_on_post_present) {
        d3d12->m_on_post_present(*d3d12);
    }

    d3d12->m_inside_present = false;
    
    return last_present_result;
}

thread_local bool g_inside_resize_buffers = false;
HRESULT last_resize_buffers_result = S_OK;

HRESULT WINAPI D3D12Hook::resize_buffers(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize buffers called");

    auto d3d12 = g_d3d12_hook;

    d3d12->m_display_width = width;
    d3d12->m_display_height = height;

    auto& hook = d3d12->m_resize_buffers_hook;
    auto resize_buffers_fn = hook->get_original<decltype(D3D12Hook::resize_buffers)*>();

    if (g_inside_resize_buffers) {
        auto original_bytes = utility::get_original_bytes(Address{resize_buffers_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_buffers_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_buffers_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize buffers fixed");
        }

        return last_resize_buffers_result;
    }

    if (d3d12->m_on_resize_buffers) {
        d3d12->m_on_resize_buffers(*d3d12);
    }

    g_inside_resize_buffers = true;

    last_resize_buffers_result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

    g_inside_resize_buffers = false;

    return last_resize_buffers_result;
}

thread_local bool g_inside_resize_target = false;
HRESULT last_resize_target_result = S_OK;

HRESULT WINAPI D3D12Hook::resize_target(IDXGISwapChain3* swap_chain, const DXGI_MODE_DESC* new_target_parameters)
{
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize target called");

    auto d3d12 = g_d3d12_hook;

    d3d12->m_render_width = new_target_parameters->Width;
    d3d12->m_render_height = new_target_parameters->Height;

    auto resize_buffers_fn = d3d12->m_resize_target_hook->get_original<decltype(D3D12Hook::resize_target)*>();

    // Restore the original code to the resize_buffers function.
    if (g_inside_resize_target) {
        auto original_bytes = utility::get_original_bytes(Address{resize_buffers_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_buffers_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_buffers_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize target fixed");
        }

        return last_resize_target_result;
    }

    if (d3d12->m_on_resize_target) {
        d3d12->m_on_resize_target(*d3d12);
    }

    g_inside_resize_target = true;

    last_resize_target_result = resize_buffers_fn(swap_chain, new_target_parameters);

    g_inside_resize_target = false;

    return last_resize_target_result;
}

/*HRESULT WINAPI D3D12Hook::create_swap_chain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_fullscreen_desc, IDXGIOutput* p_restrict_to_output, IDXGISwapChain** swap_chain)
{
    spdlog::info("D3D12 create swapchain called");

    auto d3d12 = g_d3d12_hook;

    d3d12->m_command_queue = (ID3D12CommandQueue*)device;
    
    if (d3d12->m_on_create_swap_chain) {
        d3d12->m_on_create_swap_chain(*d3d12);
    }

    auto create_swap_chain_fn = d3d12->m_create_swap_chain_hook->get_original<decltype(D3D12Hook::create_swap_chain)>();

    return create_swap_chain_fn(factory, device, hwnd, desc, p_fullscreen_desc, p_restrict_to_output, swap_chain);
}*/

