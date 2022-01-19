#include "../Plugin.hpp"
#include "../imgui/imgui_impl_dx11.h"

#include "d3d11.hpp"

D3D11 g_d3d11{};

bool D3D11::initialize() {
    auto swapchain = (IDXGISwapChain*)g_param->renderer_data->swapchain;
    auto device = (ID3D11Device*)g_param->renderer_data->device;

    // Get back buffer.
    ComPtr<ID3D11Texture2D> backbuffer{};

    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        return false;
    }

    // Create a render target view of the back buffer.
    if (FAILED(device->CreateRenderTargetView(backbuffer.Get(), nullptr, &this->bb_rtv))) {
        return false;
    }

    // Get backbuffer description.
    D3D11_TEXTURE2D_DESC backbuffer_desc{};

    backbuffer->GetDesc(&backbuffer_desc);

    backbuffer_desc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Create our blank render target.
    if (FAILED(device->CreateTexture2D(&backbuffer_desc, nullptr, &this->blank_rt))) {
        return false;
    }

    // Create our render target.
    if (FAILED(device->CreateTexture2D(&backbuffer_desc, nullptr, &this->rt))) {
        return false;
    }

    // Create our blank render target view.
    if (FAILED(device->CreateRenderTargetView(this->blank_rt.Get(), nullptr, &this->blank_rt_rtv))) {
        return false;
    }


    // Create our render target view.
    if (FAILED(device->CreateRenderTargetView(this->rt.Get(), nullptr, &this->rt_rtv))) {
        return false;
    }

    // Create our render target shader resource view.
    if (FAILED(device->CreateShaderResourceView(this->rt.Get(), nullptr, &this->rt_srv))) {
        return false;
    }

    this->rt_width = backbuffer_desc.Width;
    this->rt_height = backbuffer_desc.Height;

    ComPtr<ID3D11DeviceContext> context{};

    device->GetImmediateContext(&context);

    if (!ImGui_ImplDX11_Init(device, context.Get())) {
        return false;
    }

    return true;
}

void D3D11::render_imgui() {
    ComPtr<ID3D11DeviceContext> context{};
    float clear_color[]{0.0f, 0.0f, 0.0f, 0.0f};

    auto device = (ID3D11Device*)g_param->renderer_data->device;
    device->GetImmediateContext(&context);
    context->ClearRenderTargetView(this->blank_rt_rtv.Get(), clear_color);
    context->ClearRenderTargetView(this->rt_rtv.Get(), clear_color);
    context->OMSetRenderTargets(1, this->rt_rtv.GetAddressOf(), NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Set the back buffer to be the render target.
    context->OMSetRenderTargets(1, this->bb_rtv.GetAddressOf(), nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

