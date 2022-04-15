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
    void setup();

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct ResourceCopier {
		void setup();
		void reset();
		void wait(uint32_t ms);
		void copy(ID3D12Resource* src, ID3D12Resource* dst, D3D12_RESOURCE_STATES src_state = D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		void execute();

		ComPtr<ID3D12CommandAllocator> cmd_allocator{};
		ComPtr<ID3D12GraphicsCommandList> cmd_list{};
		ComPtr<ID3D12Fence> fence{};
		UINT64 fence_value{};
		HANDLE fence_event{};

		bool waiting_for_fence{false};
		bool has_commands{false};
	} m_resource_copier;
	};

	// Mimicking what OpenXR does.
	std::array<ResourceCopier, 6> m_copiers{};
	std::array<ComPtr<ID3D12Resource>, 3> m_left_eye_tex{};
	std::array<ComPtr<ID3D12Resource>, 3> m_right_eye_tex{};
	uint32_t m_counter{0};
	uint32_t m_texture_counter{0};

	uint32_t m_backbuffer_size[2]{};
	bool m_force_reset{false};

	struct OpenXR {
		void initialize(XrSessionCreateInfo& session_info);
		std::optional<std::string> create_swapchains();
		void copy(uint32_t swapchain_idx, ID3D12Resource* src);

		XrGraphicsBindingD3D12KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};

		struct SwapchainContext {
			struct TextureContext {
				ResourceCopier copier;
			};

			std::vector<XrSwapchainImageD3D12KHR> textures{};
			std::vector<TextureContext> texture_contexts{};
			uint32_t num_textures_acquired{0};
		};

		std::vector<SwapchainContext> contexts{};
	} m_openxr;

	std::array<ComPtr<ID3D12Resource>, 2> m_prev_backbuffers{};
};
}