#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <mutex>
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

    void force_reset() { m_force_reset = true; }

    const auto& get_backbuffer_size() const { return m_backbuffer_size; }

    auto is_initialized() const { return m_openvr.left_eye_tex[0].texture != nullptr; }

    auto& openxr() { return m_openxr; }

private:
    void setup();

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct ResourceCopier {
        virtual ~ResourceCopier() { this->reset(); }

        void setup();
        void reset();
        void wait(uint32_t ms);
        void copy(ID3D12Resource* src, ID3D12Resource* dst, D3D12_RESOURCE_STATES src_state = D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        void execute();

        ComPtr<ID3D12CommandAllocator> cmd_allocator{};
        ComPtr<ID3D12GraphicsCommandList> cmd_list{};
        ComPtr<ID3D12Fence> fence{};
        UINT64 fence_value{};
        HANDLE fence_event{};

        std::recursive_mutex mtx{};

        bool waiting_for_fence{false};
        bool has_commands{false};
    };

    ComPtr<ID3D12Resource> m_prev_backbuffer{};
    std::array<ResourceCopier, 3> m_generic_copiers{};

    // Mimicking what OpenXR does.
    struct OpenVR {
        struct TextureContext {
            ResourceCopier copier{};
            ComPtr<ID3D12Resource> texture{};
        };

        TextureContext& get_left() {
            auto& ctx = this->left_eye_tex[this->texture_counter % left_eye_tex.size()];

            return ctx;
        }

        TextureContext& get_right() {
            auto& ctx = this->right_eye_tex[this->texture_counter % right_eye_tex.size()];

            return ctx;
        }

        TextureContext& acquire_left() {
            auto& ctx = get_left();
            ctx.copier.wait(INFINITE);

            return ctx;
        }

        TextureContext& acquire_right() {
            auto& ctx = get_right();
            ctx.copier.wait(INFINITE);

            return ctx;
        }

        void copy_left(ID3D12Resource* src) {
            auto& ctx = this->acquire_left();
            ctx.copier.copy(src, ctx.texture.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            ctx.copier.execute();
        }

        void copy_right(ID3D12Resource* src) {
            auto& ctx = this->acquire_right();
            ctx.copier.copy(src, ctx.texture.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            ctx.copier.execute();
        }

        std::array<TextureContext, 3> left_eye_tex{};
        std::array<TextureContext, 3> right_eye_tex{};
        uint32_t texture_counter{0};
    } m_openvr;

    struct OpenXR {
        void initialize(XrSessionCreateInfo& session_info);
        std::optional<std::string> create_swapchains();
        void destroy_swapchains();
        void copy(uint32_t swapchain_idx, ID3D12Resource* src);
        void wait_for_all_copies() {
            std::scoped_lock _{this->mtx};

            for (auto& ctx : this->contexts) {
                for (auto& texture_ctx : ctx.texture_contexts) {
                    texture_ctx->copier.wait(INFINITE);
                }
            }
        }

        XrGraphicsBindingD3D12KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};

        struct SwapchainContext {
            struct TextureContext {
                ResourceCopier copier{};
            };

            std::vector<XrSwapchainImageD3D12KHR> textures{};
            std::vector<std::unique_ptr<TextureContext>> texture_contexts{};
            uint32_t num_textures_acquired{0};
        };

        std::vector<SwapchainContext> contexts{};
        std::recursive_mutex mtx{};
        std::array<uint32_t, 2> last_resolution{};
    } m_openxr;

    uint32_t m_backbuffer_size[2]{};
    bool m_force_reset{false};
};
} // namespace vrmod