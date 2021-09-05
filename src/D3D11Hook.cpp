#include <algorithm>
#include <spdlog/spdlog.h>

#include <openvr/headers/openvr.h>

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

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1, D3D11_SDK_VERSION,
            &swap_chain_desc, &swap_chain, &device, nullptr, &context))) {
        spdlog::error("Failed to create D3D11 device");
        return false;
    }

    auto present_fn = (*(uintptr_t**)swap_chain)[8];
    auto resize_buffers_fn = (*(uintptr_t**)swap_chain)[13];
    auto create_pixel_shader_fn = (*(uintptr_t**)device)[15];
    auto draw_fn = (*(uintptr_t**)context)[13];
    auto draw_indexed_fn = (*(uintptr_t**)context)[12];
    auto ps_set_shader_fn = (*(uintptr_t**)context)[9];
    auto om_set_render_targets_fn = (*(uintptr_t**)context)[33];
    auto rs_set_viewports_fn = (*(uintptr_t**)context)[44];

    m_present_hook = std::make_unique<FunctionHook>(present_fn, (uintptr_t)&D3D11Hook::present);
    m_resize_buffers_hook = std::make_unique<FunctionHook>(resize_buffers_fn, (uintptr_t)&D3D11Hook::resize_buffers);
    m_create_pixel_shader_hook = std::make_unique<FunctionHook>(create_pixel_shader_fn, (uintptr_t)&D3D11Hook::create_pixel_shader);
    m_draw_hook = std::make_unique<FunctionHook>(draw_fn, (uintptr_t)&D3D11Hook::draw);
    m_draw_indexed_hook = std::make_unique<FunctionHook>(draw_indexed_fn, (uintptr_t)&D3D11Hook::draw_indexed);
    m_ps_set_shader_hook = std::make_unique<FunctionHook>(ps_set_shader_fn, (uintptr_t)&D3D11Hook::ps_set_shader);
    m_om_set_render_targets_hook = std::make_unique<FunctionHook>(om_set_render_targets_fn, (uintptr_t)&D3D11Hook::om_set_render_targets);
    m_rs_set_viewports_hook = std::make_unique<FunctionHook>(rs_set_viewports_fn, (uintptr_t)rs_set_viewports);

    device->Release();
    context->Release();
    swap_chain->Release();

    m_hooked = m_present_hook->create() && m_resize_buffers_hook->create() && m_create_pixel_shader_hook->create() &&
               m_draw_hook->create() && m_draw_indexed_hook->create() && m_ps_set_shader_hook->create() &&
               m_om_set_render_targets_hook->create() && m_rs_set_viewports_hook->create();

    return m_hooked;
}

bool D3D11Hook::unhook() {
    if (m_present_hook->remove() && m_resize_buffers_hook->remove()) {
        m_hooked = false;
        return true;
    }

    return false;
}

HRESULT WINAPI D3D11Hook::present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    auto d3d11 = g_d3d11_hook;

    d3d11->m_swap_chain = swap_chain;
    swap_chain->GetDevice(__uuidof(d3d11->m_device), (void**)&d3d11->m_device);

    if (d3d11->m_ui_tex == nullptr) {
        d3d11->setup();
    }

    // This line must be called before calling our detour function because we might have to unhook the function inside our detour.
    auto present_fn = d3d11->m_present_hook->get_original<decltype(D3D11Hook::present)>();

    if (d3d11->m_on_present) {
        d3d11->m_on_present(*d3d11);
    }

    d3d11->m_is_drawing_ui = false;
    d3d11->m_stage = 0;

    auto result = present_fn(swap_chain, sync_interval, flags);

    ComPtr<ID3D11DeviceContext> context{};
    float clear_color[]{0.0f, 0.0f, 0.0f, 0.0f};

    d3d11->m_device->GetImmediateContext(&context);
    context->ClearRenderTargetView(d3d11->m_ui_rtv.Get(), clear_color);

    return result;
}

HRESULT WINAPI D3D11Hook::resize_buffers(
    IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    auto d3d11 = g_d3d11_hook;

    if (d3d11->m_on_resize_buffers) {
        d3d11->m_ui_tex.Reset();
        d3d11->m_on_resize_buffers(*d3d11);
    }

    auto resize_buffers_fn = d3d11->m_resize_buffers_hook->get_original<decltype(D3D11Hook::resize_buffers)>();

    return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}

HRESULT __stdcall D3D11Hook::create_pixel_shader(
    ID3D11Device* device, const void* bytecode, SIZE_T bytecode_len, ID3D11ClassLinkage* linkage, ID3D11PixelShader** shader) {
    // This is basically DXBCContainer::GetHash(...) method from RenderDoc.
    auto get_hash = [](uint32_t hash[4], const void* ByteCode, size_t BytecodeLength) {
        using byte = uint8_t;

        // this isn't a proper chunk, it's the file header before all the chunks.
        struct FileHeader {
            uint32_t fourcc;       // "DXBC"
            uint32_t hashValue[4]; // unknown hash function and data
            uint32_t containerVersion;
            uint32_t fileLength;
            uint32_t numChunks;
            // uint32 chunkOffsets[numChunks]; follows
        };

        enum class HASHFlags : uint32_t {
            INCLUDES_SOURCE = 0x1,
        };

        struct HASHHeader {
            HASHFlags Flags;
            uint32_t hashValue[4];
        };

#define MAKE_FOURCC(a, b, c, d) (((uint32_t)(d) << 24) | ((uint32_t)(c) << 16) | ((uint32_t)(b) << 8) | (uint32_t)(a))

        constexpr uint32_t FOURCC_DXBC = MAKE_FOURCC('D', 'X', 'B', 'C');
        constexpr uint32_t FOURCC_HASH = MAKE_FOURCC('H', 'A', 'S', 'H');

        if (BytecodeLength < sizeof(FileHeader)) {
            memset(hash, 0, sizeof(uint32_t) * 4);
            return;
        }

        const byte* data = (byte*)ByteCode; // just for convenience

        FileHeader* header = (FileHeader*)ByteCode;

        memset(hash, 0, sizeof(uint32_t) * 4);

        if (header->fourcc != FOURCC_DXBC)
            return;

        if (header->fileLength != (uint32_t)BytecodeLength)
            return;

        memcpy(hash, header->hashValue, sizeof(header->hashValue));

        uint32_t* chunkOffsets = (uint32_t*)(header + 1); // right after the header

        for (uint32_t chunkIdx = 0; chunkIdx < header->numChunks; chunkIdx++) {
            uint32_t* fourcc = (uint32_t*)(data + chunkOffsets[chunkIdx]);
            uint32_t* chunkSize = (uint32_t*)(fourcc + 1);

            char* chunkContents = (char*)(chunkSize + 1);

            if (*fourcc == FOURCC_HASH) {
                HASHHeader* hashHeader = (HASHHeader*)chunkContents;

                memcpy(hash, hashHeader->hashValue, sizeof(hashHeader->hashValue));
            }
        }
    };

    uint32_t hash[4]{};

    get_hash(hash, bytecode, bytecode_len);

    // spdlog::info("[D3D11] Pixel shader created with hash {:08X}-{:08X}-{:08X}-{:08X}", hash[0], hash[1], hash[2], hash[3]);

    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_create_pixel_shader_hook->get_original<decltype(D3D11Hook::create_pixel_shader)>();
    auto result = orig(device, bytecode, bytecode_len, linkage, shader);
    constexpr std::array<uint32_t, 4> re2_ui_hash{0x19141a2e, 0x7602d546, 0x5aef776c, 0x0125cc22};
    constexpr std::array<uint32_t, 4> re2_done_ui_hash{0xf5f2b438,0xa45ac091,0xdc675381,0xfc8fa796};


    if (std::equal(std::begin(re2_ui_hash), std::end(re2_ui_hash), std::begin(hash))) {
        d3d11->m_ui_shader = *shader;
        spdlog::info("[D3D11] UI pixel shader found @ {}", fmt::ptr(*shader));
    }

    if (std::equal(std::begin(re2_done_ui_hash), std::end(re2_done_ui_hash), std::begin(hash))) {
        d3d11->m_done_ui_shader = *shader;
        spdlog::info("[D3D11] UI done pixel shader found @ {}", fmt::ptr(*shader));
    }

    return result;
}

void __stdcall D3D11Hook::draw(ID3D11DeviceContext* context, UINT vert_count, UINT start_vert_loc) {
    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_draw_hook->get_original<decltype(D3D11Hook::draw)>();

    if (d3d11->m_stage == 1) {
        d3d11->m_stage = 2;
    }

    orig(context, vert_count, start_vert_loc);
}

void __stdcall D3D11Hook::draw_indexed(ID3D11DeviceContext* context, UINT index_count, UINT start_index_loc, INT base_vert_loc) {
    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_draw_indexed_hook->get_original<decltype(D3D11Hook::draw_indexed)>();

    //if (d3d11->m_stage != 2) {
        orig(context, index_count, start_index_loc, base_vert_loc);
    //}
}

void __stdcall D3D11Hook::ps_set_shader(
    ID3D11DeviceContext* context, ID3D11PixelShader* shader, ID3D11ClassInstance* const* class_instances, UINT num_class_instances) {

    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_ps_set_shader_hook->get_original<decltype(D3D11Hook::ps_set_shader)>();

    orig(context, shader, class_instances, num_class_instances);

    if (shader == d3d11->m_ui_shader) {
        d3d11->m_is_drawing_ui = true;
        d3d11->m_stage = 1;
    }

    if (shader == d3d11->m_done_ui_shader) {
        d3d11->m_is_drawing_ui = false;
        d3d11->m_stage = 3;
        context->RSSetViewports(1, &d3d11->m_backup_vp);
    }
}

void __stdcall D3D11Hook::om_set_render_targets(
    ID3D11DeviceContext* context, UINT num_views, ID3D11RenderTargetView* const* views, ID3D11DepthStencilView* depth) {
    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_om_set_render_targets_hook->get_original<decltype(D3D11Hook::om_set_render_targets)>();

    /* if (d3d11->m_stage == 2 && num_views == 1) {
        // Get backbuffer.
        ComPtr<ID3D11Texture2D> backbuffer{};
        ComPtr<ID3D11Resource> render_target{};

        d3d11->m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
        views[0]->GetResource(&render_target);

        if (render_target == backbuffer) {
            OutputDebugString("[D3D11] Overwriting render target!");
            orig(context, 1, &d3d11->m_ui_rtv, nullptr);
            return;
        }
    }*/

    orig(context, num_views, views, depth);
}

#include "mods/VR.hpp"

void __stdcall D3D11Hook::rs_set_viewports(ID3D11DeviceContext* context, UINT num_viewports, const D3D11_VIEWPORT* viewports) {
    auto d3d11 = g_d3d11_hook;
    auto orig = d3d11->m_rs_set_viewports_hook->get_original<decltype(D3D11Hook::rs_set_viewports)>();

    if (d3d11->m_stage == 2) {
        D3D11_VIEWPORT vp = viewports[0];
        auto& vr = VR::get();

        auto delta_x = vp.Width - (vp.Width * vr->get_ui_scale());
        auto delta_y = vp.Height - (vp.Height * vr->get_ui_scale());

        if (vr->get_frame_count() % 2 == 0) {
            vp.TopLeftX += vr->get_ui_offset() + delta_x;
        } else {
            vp.TopLeftX -= vr->get_ui_offset() - delta_x;
        }

        vp.TopLeftY += delta_y;
        vp.Width -= delta_x * 2;
        vp.Height -= delta_y * 2;
        //vp.TopLeftY += 300;
        //vp.Width -= 600;
        //vp.Height -= 600;

        orig(context, 1, &vp);
        //orig(context, num_viewports, viewports);
        d3d11->m_backup_vp = viewports[0];
    } else {
        orig(context, num_viewports, viewports);
    }
}

void D3D11Hook::setup() {
    // Get backbuffer.
    ComPtr<ID3D11Texture2D> backbuffer{};

    if (FAILED(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[D3D11] Failed to get backbuffer during setup");
        return;
    }

    // Get backbuffer description.
    D3D11_TEXTURE2D_DESC backbuffer_desc{};

    backbuffer->GetDesc(&backbuffer_desc);

    // Create UI texture.
    D3D11_TEXTURE2D_DESC texture_desc{};
    texture_desc.Width = backbuffer_desc.Width;
    texture_desc.Height = backbuffer_desc.Height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = backbuffer_desc.Format;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    if (FAILED(m_device->CreateTexture2D(&texture_desc, nullptr, &m_ui_tex))) {
        spdlog::error("[D3D11] Failed to create UI texture");
        return;
    }

    // Create UI shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};

    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Texture2D.MostDetailedMip = 0;

    if (FAILED(m_device->CreateShaderResourceView(m_ui_tex.Get(), &srv_desc, &m_ui_srv))) {
        spdlog::error("[D3D11] Failed to create UI shader resource view");
        return;
    }

    // Create UI render target view.
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};

    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateRenderTargetView(m_ui_tex.Get(), &rtv_desc, &m_ui_rtv))) {
        spdlog::error("[D3D11] Failed to create UI render target view");
        return;
    }
}
