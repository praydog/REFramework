#include <thread>
#include <future>
#include <unordered_set>
#include <stacktrace>
#include <cstring>
#include <wrl/client.h>

#include <spdlog/spdlog.h>
#include <utility/Thread.hpp>
#include <utility/Module.hpp>
#include <utility/String.hpp>
#include <utility/RTTI.hpp>
#include <utility/Scan.hpp>
#include <utility/ScopeGuard.hpp>

#include "REFramework.hpp"

#include "WindowFilter.hpp"

#include "D3D12Hook.hpp"

static D3D12Hook* g_d3d12_hook = nullptr;
thread_local bool g_inside_d3d12_hook = false;

D3D12Hook::~D3D12Hook() {
    unhook();
}

void* D3D12Hook::Streamline::link_swapchain_to_cmd_queue(void* rcx, void* rdx, void* r8, void* r9) {
    if (g_inside_d3d12_hook) {
        spdlog::info("[Streamline] linkSwapchainToCmdQueue: {:x} (inside D3D12 hook)", (uintptr_t)_ReturnAddress());

        auto& hook = D3D12Hook::s_streamline.link_swapchain_to_cmd_queue_hook;
        return hook->get_original<decltype(link_swapchain_to_cmd_queue)>()(rcx, rdx, r8, r9);
    }

    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("[Streamline] linkSwapchainToCmdQueue: {:x}", (uintptr_t)_ReturnAddress());

    bool hook_was_nullptr = g_d3d12_hook == nullptr;

    if (g_d3d12_hook != nullptr) {
        g_framework->on_reset(); // Needed to prevent a crash due to resources hanging around
        g_d3d12_hook->unhook(); // Removes all vtable hooks
    }

    auto& hook = D3D12Hook::s_streamline.link_swapchain_to_cmd_queue_hook;
    const auto result = hook->get_original<decltype(link_swapchain_to_cmd_queue)>()(rcx, rdx, r8, r9);

    // Re-hooks present after the above function creates the swapchain
    // This allows the hook to immediately still function
    // rather than waiting on the hook monitor to notice the hook isn't working
    if (!hook_was_nullptr) {
        g_framework->hook_d3d12();
    }

    return result;
}

HRESULT WINAPI D3D12Hook::create_swapchain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_fullscreen_desc, IDXGIOutput* p_restrict_to_output, IDXGISwapChain** swap_chain) {
    auto create_swap_chain_fn = s_create_swapchain_hook->get_original<decltype(D3D12Hook::create_swapchain)*>();

    if (g_inside_d3d12_hook) {
        spdlog::info("create_swapchain (inside D3D12 hook)");
        return create_swap_chain_fn(factory, device, hwnd, desc, p_fullscreen_desc, p_restrict_to_output, swap_chain);
    }

    spdlog::info("create_swapchain called");

    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    bool hook_was_nullptr = g_d3d12_hook == nullptr;

    if (g_d3d12_hook != nullptr && g_framework->get_d3d12_hook() != nullptr) {
        g_framework->on_reset(); // Needed to prevent a crash due to resources hanging around
        g_d3d12_hook->unhook(); // Removes all vtable hooks
    }

    const auto result = create_swap_chain_fn(factory, device, hwnd, desc, p_fullscreen_desc, p_restrict_to_output, swap_chain);

    // rather than waiting on the hook monitor to notice the hook isn't working
    if (!hook_was_nullptr) {
        g_framework->hook_d3d12();
    }

    return result;
}

void D3D12Hook::hook_streamline(HMODULE dlssg_module) try {
    if (D3D12Hook::s_streamline.setup) {
        return;
    }

    std::scoped_lock _{D3D12Hook::s_streamline.hook_mutex};

    if (D3D12Hook::s_streamline.setup) {
        return;
    }

    spdlog::info("[Streamline] Hooking Streamline");

    if (dlssg_module == nullptr) {
        dlssg_module = GetModuleHandleW(L"sl.dlss_g.dll");
    }

    if (dlssg_module == nullptr) {
        spdlog::error("[Streamline] Failed to get sl.dlss_g.dll module handle");
        return;
    }

    const auto str = utility::scan_string(dlssg_module, "linkSwapchainToCmdQueue");

    if (!str) {
        spdlog::error("[Streamline] Failed to find linkSwapchainToCmdQueue");
        return;
    }

    const auto str_ref = utility::scan_displacement_reference(dlssg_module, *str);

    if (!str_ref) {
        spdlog::error("[Streamline] Failed to find linkSwapchainToCmdQueue reference");
        return;
    }

    const auto fn = utility::find_function_start_with_call(*str_ref);

    if (!fn) {
        spdlog::error("[Streamline] Failed to find linkSwapchainToCmdQueue function");
        return;
    }

    D3D12Hook::s_streamline.link_swapchain_to_cmd_queue_hook = std::make_unique<FunctionHook>(*fn, (uintptr_t)&Streamline::link_swapchain_to_cmd_queue);

    if (D3D12Hook::s_streamline.link_swapchain_to_cmd_queue_hook->create()) {
        spdlog::info("[Streamline] Hooked linkSwapchainToCmdQueue");
    } else {
        spdlog::error("[Streamline] Failed to hook linkSwapchainToCmdQueue");
    }

    D3D12Hook::s_streamline.setup = true;
} catch(...) {
    spdlog::error("[Streamline] Failed to hook Streamline");
}

// True only for Wine running on a macOS host (CrossOver / Game Porting Toolkit, where D3D12 is D3DMetal).
// Wine exports wine_get_version/wine_get_host_version from ntdll.dll; real Windows does not.
// Linux/Proton/Steam Deck report "Linux" here and keep the original code paths.
static bool is_wine_on_macos() {
    static const bool result = []() {
        const auto ntdll = GetModuleHandleA("ntdll.dll");

        if (ntdll == nullptr || GetProcAddress(ntdll, "wine_get_version") == nullptr) {
            return false;
        }

        using wine_get_host_version_t = void(__cdecl*)(const char** sysname, const char** release);
        const auto wine_get_host_version = (wine_get_host_version_t)GetProcAddress(ntdll, "wine_get_host_version");

        if (wine_get_host_version == nullptr) {
            return false;
        }

        const char* sysname = nullptr;
        const char* release = nullptr;
        wine_get_host_version(&sysname, &release);

        const bool is_macos = sysname != nullptr && strcmp(sysname, "Darwin") == 0;
        spdlog::info("Wine detected, host: {} {} -> macOS/D3DMetal paths {}", sysname != nullptr ? sysname : "?", release != nullptr ? release : "?", is_macos ? "enabled" : "disabled");

        return is_macos;
    }();

    return result;
}

// --- Wine/D3DMetal command queue lookup helpers (adapted from upstream PR #1589) ---
//
// D3DMetal wraps its COM objects, so the raw pointer stored inside the swapchain is often NOT
// the same address as the ID3D12CommandQueue* we created. QueryInterface can deadlock there,
// so instead we identify the queue by refcount: hold one extra reference on our queue, then
// AddRef/Release each candidate pointer and see if the count moves by exactly one.
static constexpr auto COMMAND_QUEUE_SCAN_BYTES = 512 * sizeof(void*);
static intptr_t s_wine_cq_delta = 0; // (our queue pointer) - (raw pointer found in the swapchain)

struct HeldRefcountProbe {
    IUnknown* object{};
    ULONG held_refcount{};
    bool valid{};
};

// These helpers use __try/__except, so they must not contain C++ objects that need unwinding.
static HeldRefcountProbe hold_refcount_probe(IUnknown* object) {
    HeldRefcountProbe probe{};

    if (object == nullptr || IsBadReadPtr(object, sizeof(void*))) {
        return probe;
    }

    __try {
        probe.object = object;
        probe.held_refcount = object->AddRef();
        probe.valid = true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Invalid COM pointer, ignore
    }

    return probe;
}

static void release_refcount_probe(HeldRefcountProbe& probe) {
    if (!probe.valid || probe.object == nullptr) {
        return;
    }

    __try {
        probe.object->Release();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Invalid COM pointer, ignore
    }

    probe.valid = false;
    probe.object = nullptr;
    probe.held_refcount = 0;
}

static bool matches_refcount_probe(IUnknown* candidate, const HeldRefcountProbe& probe) {
    if (!probe.valid || candidate == nullptr || IsBadReadPtr(candidate, sizeof(void*))) {
        return false;
    }

    bool match = false;

    __try {
        const auto addref_result = candidate->AddRef();
        const auto release_result = candidate->Release();
        match = addref_result == probe.held_refcount + 1 && release_result == probe.held_refcount;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Invalid COM pointer, ignore
    }

    return match;
}

static uint32_t scan_for_refcount_match(void* object, const HeldRefcountProbe& probe) {
    if (object == nullptr || !probe.valid) {
        return 0;
    }

    for (auto i = 0; i < COMMAND_QUEUE_SCAN_BYTES; i += sizeof(void*)) {
        const auto base = (uintptr_t)object + i;

        if (IsBadReadPtr((void*)base, sizeof(void*))) {
            break;
        }

        auto data = *(IUnknown**)base;

        if (matches_refcount_probe(data, probe)) {
            return i;
        }
    }

    return 0;
}

// Isolated on purpose: functions containing __try/__except cannot also contain
// C++ objects that need unwinding (e.g. std::vector, RAII guards), so this call
// is pulled out into its own small function with no such locals.
//
// This exists because calling through a temporarily-unpatched D3D12CreateDevice
// can crash outright under some non-Windows D3D12 translation layers (e.g.
// D3DMetal via CrossOver/Wine on macOS) instead of failing cleanly. Wrapping it
// turns that hard crash into a normal, loggable failure so the rest of the game
// can keep running even if this dummy-device step doesn't succeed.
static HRESULT __declspec(noinline) d3d12_hook_call_create_device_safe(decltype(D3D12CreateDevice)* fn, IUnknown* adapter, D3D_FEATURE_LEVEL feature_level, ID3D12Device** device_out) {
    HRESULT hr = E_FAIL;

    __try {
        hr = fn(adapter, feature_level, IID_PPV_ARGS(device_out));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        spdlog::error("Exception occurred while calling D3D12CreateDevice directly (code {:x})", (unsigned long)GetExceptionCode());
        hr = E_FAIL;
    }

    return hr;
}

bool D3D12Hook::hook() {
    spdlog::info("Hooking D3D12");

    g_d3d12_hook = this;
    g_inside_d3d12_hook = true;

    utility::ScopeGuard guard{[]() {
        g_inside_d3d12_hook = false;
    }};

    if (s_command_queue_offset != 0 && s_swapchain_vtable != nullptr && s_factory_vtable != nullptr) {
        spdlog::info("Reinitializing D3D12Hook via known pointers");

        try {
            hook_impl();
        } catch (const std::exception& e) {
            spdlog::error("Failed to initialize hooks: {}", e.what());
            m_hooked = false;
        }

        return m_hooked;
    }

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

    // Manually get CreateDXGIFactory export because the user may be running Windows 7
    // (loaded before the dummy device now, because the Wine path below needs it to find an adapter)
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

    spdlog::info("Creating dummy device");

    if (is_wine_on_macos()) {
        // Wine/CrossOver/D3DMetal: skip the "is D3D12CreateDevice hooked?" check used in the else branch.
        // It compares the loaded d3d12.dll against the file on disk, which is not meaningful here
        // (this isn't Microsoft's d3d12.dll), and patching live D3DMetal code is risky.
        // Also, D3DMetal reportedly crashes when given a null adapter (see upstream PR #1589),
        // so hand it a real adapter from DXGI instead.
        spdlog::info("Wine on macOS detected, creating dummy device with an enumerated adapter (no unhook dance)");

        IDXGIFactory4* adapter_factory{ nullptr };
        if (FAILED(create_dxgi_factory(IID_PPV_ARGS(&adapter_factory)))) {
            spdlog::error("Wine: failed to create DXGI factory for adapter enumeration");
            return false;
        }

        IDXGIAdapter* adapter{ nullptr };
        const auto enum_hr = adapter_factory->EnumAdapters(0, &adapter);
        adapter_factory->Release();

        if (FAILED(enum_hr) || adapter == nullptr) {
            spdlog::error("Wine: no DXGI adapter found ({:x})", (uint32_t)enum_hr);
            return false;
        }

        spdlog::info("Wine: calling D3D12CreateDevice with adapter {:x}", (uintptr_t)adapter);

        const auto hr = d3d12_hook_call_create_device_safe(d3d12_create_device, adapter, feature_level, &device);
        adapter->Release();

        if (FAILED(hr)) {
            spdlog::error("Wine: failed to create D3D12 dummy device ({:x})", (uint32_t)hr);
            return false;
        }
    } else {
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

            if (FAILED(d3d12_hook_call_create_device_safe(d3d12_create_device, nullptr, feature_level, &device))) {
                spdlog::error("Failed to create D3D12 Dummy device");
                memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
                return false;
            }

            spdlog::info("Restoring hooked bytes for D3D12CreateDevice");
            memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
        } else { // D3D12CreateDevice is not hooked
            if (FAILED(d3d12_hook_call_create_device_safe(d3d12_create_device, nullptr, feature_level, &device))) {
                spdlog::error("Failed to create D3D12 Dummy device");
                return false;
            }
        }
    }

    spdlog::info("Dummy device: {:x}", (uintptr_t)device);

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

    try {
        const auto ti = utility::rtti::get_type_info(swap_chain1);
        const auto swapchain_classname = ti != nullptr && ti->name() != nullptr ? std::string_view{ti->name()} : "unknown";
        const auto raw_name = ti != nullptr && ti->raw_name() != nullptr ? std::string_view{ti->raw_name()} : "unknown";

        spdlog::info("Swapchain type info: {}", swapchain_classname);
        spdlog::info("Swapchain raw type info: {}", raw_name);
        
        if (swapchain_classname.contains("interposer::DXGISwapChain")) { // DLSS3
            spdlog::info("Found Streamline (DLSSFG) swapchain during dummy initialization: {:x}", (uintptr_t)swap_chain1);
            m_using_frame_generation_swapchain = true;
        } else if (swapchain_classname.contains("FrameInterpolationSwapChain")) { // FSR3
            spdlog::info("Found FSR3 swapchain during dummy initialization: {:x}", (uintptr_t)swap_chain1);
            m_using_frame_generation_swapchain = true;
        }
    } catch (const std::exception& e) {
        spdlog::error("Failed to get type info: {}", e.what());
    } catch (...) {
        spdlog::error("Failed to get type info: unknown exception");
    }


    spdlog::info("Finding command queue offset");

    s_command_queue_offset = 0;
    s_wine_cq_delta = 0;

    const auto wine = is_wine_on_macos();
    auto command_queue_refcount_probe = wine ? hold_refcount_probe(command_queue) : HeldRefcountProbe{};
    utility::ScopeGuard refcount_guard{[&]() {
        release_refcount_probe(command_queue_refcount_probe);
    }};

    // Find the command queue offset in the swapchain
    for (auto i = 0; i < 512 * sizeof(void*); i += sizeof(void*)) {
        const auto base = (uintptr_t)swap_chain1 + i;

        // reached the end
        if (IsBadReadPtr((void*)base, sizeof(void*))) {
            break;
        }

        auto data = *(ID3D12CommandQueue**)base;

        if (data == command_queue) {
            s_command_queue_offset = i;
            spdlog::info("Found command queue offset: {:x}", i);
            break;
        }
    }

    // Wine/D3DMetal: the queue may be stored under a different (wrapped) pointer, identify it by refcount
    if (s_command_queue_offset == 0 && wine) {
        s_command_queue_offset = scan_for_refcount_match(swap_chain1, command_queue_refcount_probe);

        if (s_command_queue_offset != 0) {
            spdlog::info("Found command queue offset via Wine refcount scan: {:x}", s_command_queue_offset);
        }
    }

    // Wine/D3DMetal last resort: hardcoded offset reported in upstream PR #1589
    if (s_command_queue_offset == 0 && wine) {
        constexpr uint32_t WINE_D3DMETAL_CQ_OFFSET = 0x4C8;
        const auto base = (uintptr_t)swap_chain1 + WINE_D3DMETAL_CQ_OFFSET;

        if (!IsBadReadPtr((void*)base, sizeof(void*))) {
            auto candidate = *(ID3D12CommandQueue**)base;

            if (candidate != nullptr && !IsBadReadPtr((void*)candidate, sizeof(void*))) {
                s_command_queue_offset = WINE_D3DMETAL_CQ_OFFSET;
                spdlog::warn("Wine: using hardcoded command queue offset 0x{:x} (D3DMetal fallback)", WINE_D3DMETAL_CQ_OFFSET);
            }
        }
    }

    auto target_swapchain = swap_chain;

    // Scan throughout the swapchain for a valid pointer to scan through
    // this is usually only necessary for Proton
    if (s_command_queue_offset == 0) {
        bool should_break = false;

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

                const auto matched_by_refcount = wine && matches_refcount_probe(data, command_queue_refcount_probe);

                if (data == command_queue || matched_by_refcount) {
                    // If we hook Streamline's Swapchain, the menu fails to render correctly/flickers
                    // So we switch out the swapchain with the internal one owned by Streamline
                    // Side note: Even though we are scanning for Proton here,
                    // this doubles as an offset scanner for the real swapchain inside Streamline (or FSR3)
                    if (m_using_frame_generation_swapchain) {
                        target_swapchain = (IDXGISwapChain3*)scan_base;
                    }

                    if (!m_using_frame_generation_swapchain) {
                        m_using_proton_swapchain = true;
                    }

                    s_command_queue_offset = i;
                    s_proton_swapchain_offset = base;
                    should_break = true;

                    spdlog::info("Proton potentially detected");
                    spdlog::info("Found command queue offset: {:x}", i);
                    break;
                }
            }

            if (m_using_proton_swapchain || should_break) {
                break;
            }
        }
    }

    if (s_command_queue_offset == 0) {
        spdlog::error("Failed to find command queue offset");
        return false;
    }

    // Wine/D3DMetal: remember how far the raw pointer stored in the swapchain is from our real queue pointer.
    // present() adds this difference back with plain arithmetic, so no COM calls are needed there.
    if (wine && s_command_queue_offset != 0) {
        const auto scan_object = m_using_proton_swapchain ? *(uintptr_t*)((uintptr_t)swap_chain1 + s_proton_swapchain_offset) : (uintptr_t)swap_chain1;
        const auto raw = *(uintptr_t*)(scan_object + s_command_queue_offset);
        s_wine_cq_delta = (intptr_t)((uintptr_t)command_queue - raw);

        if (s_wine_cq_delta != 0) {
            spdlog::info("Wine command queue delta: 0x{:X}", (uintptr_t)s_wine_cq_delta);
        }
    }

    //utility::ThreadSuspender suspender{};

    try {
        s_swapchain_vtable = *(void***)target_swapchain;
        s_factory_vtable = *(void***)factory;

        hook_impl();
    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize hooks: {}", e.what());
        m_hooked = false;
    }

    //suspender.resume();

    command_queue->Release();
    swap_chain1->Release();
    swap_chain->Release();
    device->Release();
    factory->Release();

    if (hwnd) {
        ::DestroyWindow(hwnd);
    }

    if (wc.lpszClassName != nullptr) {
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    return m_hooked;
}

void D3D12Hook::hook_impl() {
    spdlog::info("Initializing hooks");

    hook_streamline();

    m_present_hook.reset();
    m_swapchain_hook.reset();

    m_is_phase_1 = true;

    auto& present_fn = s_swapchain_vtable[8]; // Present
    m_present_hook = std::make_unique<PointerHook>(&present_fn, &D3D12Hook::present);

    if (s_create_swapchain_hook == nullptr) {
        auto& create_swapchain_fn = s_factory_vtable[15]; // CreateSwapChainForHwnd
        s_create_swapchain_hook = std::make_unique<PointerHook>(&create_swapchain_fn, &D3D12Hook::create_swapchain);
    }

    m_hooked = true;
}

bool D3D12Hook::unhook() {
    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    if (!m_hooked) {
        return true;
    }

    spdlog::info("Unhooking D3D12");

    m_present_hook.reset();
    m_swapchain_hook.reset();

    m_hooked = false;
    m_is_phase_1 = true;

    return true;
}

thread_local int32_t g_present_depth = 0;

HRESULT WINAPI D3D12Hook::present(IDXGISwapChain3* swap_chain, uint64_t sync_interval, uint64_t flags, void* r9) {
    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto d3d12 = g_d3d12_hook;

    decltype(D3D12Hook::present)* present_fn{nullptr};

    if (d3d12->m_is_phase_1) {
        present_fn = d3d12->m_present_hook->get_original<decltype(D3D12Hook::present)*>();
    } else {
        present_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::present)*>(8);
    }

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    if (d3d12->m_is_phase_1 && WindowFilter::get().is_filtered(swapchain_wnd)) {
        //present_fn = d3d12->m_present_hook->get_original<decltype(D3D12Hook::present)*>();
        return present_fn(swap_chain, sync_interval, flags, r9);
    }

    if (!d3d12->m_is_phase_1 && swap_chain != d3d12->m_swapchain_hook->get_instance()) {
        return present_fn(swap_chain, sync_interval, flags, r9);
    }

    if (d3d12->m_is_phase_1) {
        // Remove the present hook, we will just rely on the vtable hook below
        // because we don't want to cause any conflicts with other hooks
        // vtable hooks are the least intrusive
        // And doing a global pointer replacement seems to have
        // conflicts with Streamline's hooks, causing unexplainable crashes
        d3d12->m_present_hook.reset();

        // vtable hook the swapchain instead of global hooking
        // this seems safer for whatever reason
        // if we globally hook the vtable pointers, it causes all sorts of weird conflicts with other hooks
        // dont hook present though via this hook so other hooks dont get confused
        d3d12->m_swapchain_hook = std::make_unique<VtableHook>(swap_chain);
        //d3d12->m_swapchain_hook->hook_method(2, (uintptr_t)&D3D12Hook::release);
        d3d12->m_swapchain_hook->hook_method(8, (uintptr_t)&D3D12Hook::present);
        d3d12->m_swapchain_hook->hook_method(13, (uintptr_t)&D3D12Hook::resize_buffers);
        d3d12->m_swapchain_hook->hook_method(14, (uintptr_t)&D3D12Hook::resize_target);
        d3d12->m_is_phase_1 = false;

        present_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::present)*>(8);
    }

    d3d12->m_inside_present = true;
    d3d12->m_swap_chain = swap_chain;

    {
        Microsoft::WRL::ComPtr<ID3D12Device4> temp_device{};
        swap_chain->GetDevice(IID_PPV_ARGS(&temp_device));
        d3d12->m_device = temp_device.Get();
    }

    if (d3d12->m_using_proton_swapchain) {
        const auto real_swapchain = *(uintptr_t*)((uintptr_t)swap_chain + d3d12->s_proton_swapchain_offset);
        d3d12->m_command_queue = *(ID3D12CommandQueue**)(real_swapchain + d3d12->s_command_queue_offset);
    } else {
        d3d12->m_command_queue = *(ID3D12CommandQueue**)((uintptr_t)swap_chain + d3d12->s_command_queue_offset);
    }

    // Wine/D3DMetal: adjust the raw swapchain pointer by the delta computed in hook() (no COM calls)
    if (s_wine_cq_delta != 0) {
        d3d12->m_command_queue = (ID3D12CommandQueue*)((uintptr_t)d3d12->m_command_queue + s_wine_cq_delta);
    }

    if (d3d12->m_swapchain_0 == nullptr) {
        d3d12->m_swapchain_0 = swap_chain;
    } else if (d3d12->m_swapchain_1 == nullptr && swap_chain != d3d12->m_swapchain_0) {
        d3d12->m_swapchain_1 = swap_chain;
    }
    
    // Restore the original bytes
    // if an infinite loop occurs, this will prevent the game from crashing
    // while keeping our hook intact
    if (g_present_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{present_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{present_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(present_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Present fixed");
        }

        if ((uintptr_t)present_fn != (uintptr_t)D3D12Hook::present && g_present_depth == 1) {
            spdlog::info("Attempting to call real present function");

            ++g_present_depth;
            const auto result = present_fn(swap_chain, sync_interval, flags, r9);
            --g_present_depth;

            if (result != S_OK) {
                spdlog::error("Present failed: {:x}", result);
            }

            return result;
        }

        spdlog::info("Just returning S_OK");
        return S_OK;
    }

    if (d3d12->m_on_present) {
        d3d12->m_on_present(*d3d12);
    }

    ++g_present_depth;

    auto result = S_OK;
    
    if (!d3d12->m_ignore_next_present) {
        result = present_fn(swap_chain, sync_interval, flags, r9);

        if (result != S_OK) {
            spdlog::error("Present failed: {:x}", result);
        }
    } else {
        d3d12->m_ignore_next_present = false;
    }

    --g_present_depth;

    if (d3d12->m_on_post_present) {
        d3d12->m_on_post_present(*d3d12);
    }

    d3d12->m_inside_present = false;
    
    return result;
}

thread_local int32_t g_resize_buffers_depth = 0;

HRESULT WINAPI D3D12Hook::resize_buffers(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize buffers called");
    spdlog::info(" Parameters: buffer_count {} width {} height {} new_format {} swap_chain_flags {}", buffer_count, width, height, new_format, swap_chain_flags);

    // Walk the callstack and print out module names
    try {
        std::string callstack_str{};
        for (const auto& entry : std::stacktrace::current()) {
            //spdlog::info(" {}", entry.description());
            callstack_str += entry.description() + "\n";
        }

        spdlog::info("callstack: \n{}", callstack_str); // because this can be running on a different thread and get garbled in the middle of the log
    } catch (const std::exception& e) {
        spdlog::error("Failed to print callstack: {}", e.what());
    } catch(...) {
        spdlog::error("Failed to print callstack: unknown exception");
    }

    auto d3d12 = g_d3d12_hook;
    //auto& hook = d3d12->m_resize_buffers_hook;
    //auto resize_buffers_fn = hook->get_original<decltype(D3D12Hook::resize_buffers)*>();

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    /*if (WindowFilter::get().is_filtered(swapchain_wnd)) {
        return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    }*/

    auto resize_buffers_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::resize_buffers)*>(13);

    d3d12->m_display_width = width;
    d3d12->m_display_height = height;

    if (g_resize_buffers_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{resize_buffers_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_buffers_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_buffers_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize buffers fixed");
        }

        if ((uintptr_t)resize_buffers_fn != (uintptr_t)&D3D12Hook::resize_buffers && g_resize_buffers_depth == 1) {
            spdlog::info("Attempting to call the real resize buffers function");

            ++g_resize_buffers_depth;
            const auto result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
            --g_resize_buffers_depth;

            if (result != S_OK) {
                spdlog::error("Resize buffers failed: {:x}", result);
            }

            return result;
        } else {
            spdlog::info("Just returning S_OK");
            return S_OK;
        }
    }

    if (d3d12->m_on_resize_buffers) {
        d3d12->m_on_resize_buffers(*d3d12);
    }

    ++g_resize_buffers_depth;

    const auto result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    
    if (result != S_OK) {
        spdlog::error("Resize buffers failed: {:x}", result);
    }

    --g_resize_buffers_depth;

    return result;
}

thread_local int32_t g_resize_target_depth = 0;

HRESULT WINAPI D3D12Hook::resize_target(IDXGISwapChain3* swap_chain, const DXGI_MODE_DESC* new_target_parameters) {
    while (g_framework == nullptr) {
        std::this_thread::yield();
    }

    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize target called");
    spdlog::info(" Parameters: new_target_parameters {:x}", (uintptr_t)new_target_parameters);

    // Walk the callstack and print out module names
    try {
        std::string callstack_str{};
        for (const auto& entry : std::stacktrace::current()) {
            //spdlog::info(" {}", entry.description());
            callstack_str += entry.description() + "\n";
        }

        spdlog::info("callstack: \n{}", callstack_str); // because this can be running on a different thread and get garbled in the middle of the log
    } catch (const std::exception& e) {
        spdlog::error("Failed to print callstack: {}", e.what());
    } catch(...) {
        spdlog::error("Failed to print callstack: unknown exception");
    }

    auto d3d12 = g_d3d12_hook;
    //auto resize_target_fn = d3d12->m_resize_target_hook->get_original<decltype(D3D12Hook::resize_target)*>();

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    /*if (WindowFilter::get().is_filtered(swapchain_wnd)) {
        return resize_target_fn(swap_chain, new_target_parameters);
    }*/

    auto resize_target_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::resize_target)*>(14);

    d3d12->m_render_width = new_target_parameters->Width;
    d3d12->m_render_height = new_target_parameters->Height;

    // Restore the original code to the resize_buffers function.
    if (g_resize_target_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{resize_target_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_target_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_target_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize target fixed");
        }

        if ((uintptr_t)resize_target_fn != (uintptr_t)&D3D12Hook::resize_target && g_resize_target_depth == 1) {
            spdlog::info("Attempting to call the real resize target function");

            ++g_resize_target_depth;
            const auto result = resize_target_fn(swap_chain, new_target_parameters);
            --g_resize_target_depth;

            if (result != S_OK) {
                spdlog::error("Resize target failed: {:x}", result);
            }

            return result;
        } else {
            spdlog::info("Just returning S_OK");
            return S_OK;
        }
    }

    if (d3d12->m_on_resize_target) {
        d3d12->m_on_resize_target(*d3d12);
    }

    ++g_resize_target_depth;

    const auto result = resize_target_fn(swap_chain, new_target_parameters);
    
    if (result != S_OK) {
        spdlog::error("Resize target failed: {:x}", result);
    }

    --g_resize_target_depth;

    return result;
}
