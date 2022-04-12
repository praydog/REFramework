#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D12
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class VR;

namespace vrmod {
class D3D12Component {
public:
    vr::EVRCompositorError on_frame(VR* vr);
    void on_reset(VR* vr);

	void force_reset() {
		m_force_reset = true;
	}

	const auto& get_backbuffer_size() const {
		return m_backbuffer_size;
	}

	auto is_initialized() const {
		return m_left_eye_tex != nullptr;
	}

	auto& openxr() {
        return m_openxr;
    }

private:
	void wait_for_texture_copy(uint32_t ms);

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12CommandAllocator> m_cmd_allocator{};
	ComPtr<ID3D12GraphicsCommandList> m_cmd_list{};
	ComPtr<ID3D12Fence> m_fence{};
	UINT64 m_fence_value{};
	HANDLE m_fence_event{};

	uint32_t m_backbuffer_size[2]{};
	bool m_force_reset{false};
	bool m_waiting_for_fence{false};

	ComPtr<ID3D12Resource> m_left_eye_tex{};
	ComPtr<ID3D12Resource> m_right_eye_tex{}; 

	struct OpenXR {
		void initialize();
		std::optional<std::string> create_swapchains();

		XrGraphicsBindingD3D12KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};

		struct SwapchainContext {
			std::vector<XrSwapchainImageD3D12KHR> textures{};
		};

		XrFrameState frame_state{};
		std::vector<SwapchainContext> contexts{};
	} m_openxr;

    void setup();
    void copy_texture(ID3D12Resource* src, ID3D12Resource* dst);
};
}