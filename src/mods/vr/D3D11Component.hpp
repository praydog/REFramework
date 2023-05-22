#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <openvr.h>
#include <wrl.h>

#include <SpriteBatch.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D12
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class VR;

namespace vrmod {
class D3D11Component {
public:
    vr::EVRCompositorError on_frame(VR* vr);
    void on_reset(VR* vr);

    auto& openxr() { return m_openxr; }

private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct TextureContext {
        ComPtr<ID3D11Resource> tex{};
        ComPtr<ID3D11RenderTargetView> rtv{};
        ComPtr<ID3D11ShaderResourceView> srv{};

        TextureContext(ID3D11Resource* in_tex, std::optional<DXGI_FORMAT> rtv_format = std::nullopt, std::optional<DXGI_FORMAT> srv_format = std::nullopt);
        TextureContext() = default;

        virtual ~TextureContext() {
            reset();
        }

        bool set(ID3D11Resource* in_tex, std::optional<DXGI_FORMAT> rtv_format = std::nullopt, std::optional<DXGI_FORMAT> srv_format = std::nullopt);
        bool clear_rtv(float* color);

        void reset() {
            tex.Reset();
            rtv.Reset();
            srv.Reset();
        }

        bool has_texture() const {
            return tex != nullptr;
        }

        bool has_rtv() const {
            return rtv != nullptr;
        }

        bool has_srv() const {
            return srv != nullptr;
        }

        operator bool() const {
            return tex != nullptr;
        }

        operator ID3D11Resource*() const {
            return tex.Get();
        }

        operator ID3D11Texture2D*() const {
            return (ID3D11Texture2D*)tex.Get();
        }

        operator ID3D11RenderTargetView*() const {
            return rtv.Get();
        }

        operator ID3D11ShaderResourceView*() const {
            return srv.Get();
        }
    };

    std::array<float, 2> m_backbuffer_size{};

    ComPtr<ID3D11Texture2D> m_left_eye_tex{};
    ComPtr<ID3D11Texture2D> m_right_eye_tex{};
    ComPtr<ID3D11Texture2D> m_left_eye_depthstencil{};
    ComPtr<ID3D11Texture2D> m_right_eye_depthstencil{};
    vr::HmdMatrix44_t m_left_eye_proj{};
    vr::HmdMatrix44_t m_right_eye_proj{};

    ComPtr<ID3D11Texture2D> m_backbuffer_copy_tex{};
    TextureContext m_backbuffer_copy_rt{};

    ComPtr<ID3D11RenderTargetView> m_backbuffer_rtv{};

    TextureContext m_left_eye_rt{};
    TextureContext m_right_eye_rt{};
    std::unique_ptr<DirectX::DX11::SpriteBatch> m_sprite_batch{};

    bool m_backbuffer_is_8bit{false};

    struct OpenXR {
        void initialize(XrSessionCreateInfo& session_info);
        std::optional<std::string> create_swapchains();
        void destroy_swapchains();
        void copy(uint32_t swapchain_idx, ID3D11Texture2D* resource);

        XrGraphicsBindingD3D11KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};

        struct SwapchainContext {
            std::vector<XrSwapchainImageD3D11KHR> textures{};
            uint32_t num_textures_acquired{0};
        };

        std::vector<SwapchainContext> contexts{};
        std::recursive_mutex mtx{};
        std::array<uint32_t, 2> last_resolution{};
    } m_openxr;

    bool setup();
};
} // namespace vrmod
