#pragma once

#include <functional>

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

#include "utility/FunctionHook.hpp"

class D3D11Hook {
public:
    typedef std::function<void(D3D11Hook&)> OnPresentFn;
    typedef std::function<void(D3D11Hook&)> OnResizeBuffersFn;

    D3D11Hook() = default;
    virtual ~D3D11Hook();

	bool is_hooked() {
		return m_hooked;
	}

    bool hook();
    bool unhook();

    void on_present(OnPresentFn fn) { m_on_present = fn; }
    void on_resize_buffers(OnResizeBuffersFn fn) { m_on_resize_buffers = fn; }

    ID3D11Device* get_device() { return m_device; }
    IDXGISwapChain* get_swap_chain() { return m_swap_chain; }
    auto get_ui_srv() { return m_ui_srv.Get(); }

protected:
    ID3D11Device* m_device{ nullptr };
    IDXGISwapChain* m_swap_chain{ nullptr };
    bool m_hooked{ false };

    std::unique_ptr<FunctionHook> m_present_hook{};
    std::unique_ptr<FunctionHook> m_resize_buffers_hook{};
    std::unique_ptr<FunctionHook> m_create_pixel_shader_hook{};
    std::unique_ptr<FunctionHook> m_draw_hook{};
    std::unique_ptr<FunctionHook> m_draw_indexed_hook{};
    std::unique_ptr<FunctionHook> m_ps_set_shader_hook{};
    std::unique_ptr<FunctionHook> m_om_set_render_targets_hook{};
    std::unique_ptr<FunctionHook> m_rs_set_viewports_hook{};
    OnPresentFn m_on_present{ nullptr };
    OnResizeBuffersFn m_on_resize_buffers{ nullptr };
    ID3D11PixelShader* m_ui_shader{};
    ID3D11PixelShader* m_done_ui_shader{};
    bool m_is_drawing_ui{};
    int m_stage{};

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture2D> m_ui_tex{};
    ComPtr<ID3D11ShaderResourceView> m_ui_srv{};
    ComPtr<ID3D11RenderTargetView> m_ui_rtv{};
    D3D11_VIEWPORT m_backup_vp{};

    static HRESULT WINAPI present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    static HRESULT WINAPI resize_buffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags);
    static HRESULT WINAPI create_pixel_shader(ID3D11Device* device, const void* bytecode, SIZE_T bytecode_len, ID3D11ClassLinkage* linkage, ID3D11PixelShader** shader);
    static void WINAPI draw(ID3D11DeviceContext* context, UINT vert_count, UINT start_vert_loc);
    static void WINAPI draw_indexed(ID3D11DeviceContext* context, UINT index_count, UINT start_index_loc, INT base_vert_loc);
    static void WINAPI ps_set_shader(ID3D11DeviceContext* context, ID3D11PixelShader* shader, ID3D11ClassInstance* const* class_instances, UINT num_class_instances);
    static void WINAPI om_set_render_targets(ID3D11DeviceContext* context, UINT num_views, ID3D11RenderTargetView* const* views, ID3D11DepthStencilView* depth);
    static void WINAPI rs_set_viewports(ID3D11DeviceContext* context, UINT num_viewports, const D3D11_VIEWPORT* viewports);

    void setup();
};
