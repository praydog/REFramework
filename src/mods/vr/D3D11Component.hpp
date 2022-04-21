#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>
#include <openvr.h>

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

    auto& openxr() {
        return m_openxr;
    }

private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture2D> m_left_eye_tex{};
    ComPtr<ID3D11Texture2D> m_right_eye_tex{};
    ComPtr<ID3D11Texture2D> m_left_eye_depthstencil{};
    ComPtr<ID3D11Texture2D> m_right_eye_depthstencil{};
    vr::HmdMatrix44_t m_left_eye_proj{};
    vr::HmdMatrix44_t m_right_eye_proj{};

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
    } m_openxr;

    void setup();
};
} 

