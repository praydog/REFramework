#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class VR;

namespace vrmod {
class D3D11Component {
public:
    void on_frame(VR* vr);
    void on_reset(VR* vr);

    void on_reinitialize_openvr() {
        m_overlay_data = {};
    }

private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture2D> m_left_eye_tex{};
    ComPtr<ID3D11Texture2D> m_right_eye_tex{};

    // Cached data for imgui VR overlay so we know when we need to update it
    // instead of doing it constantly every frame
    struct {
        uint32_t last_render_target_width{};
        uint32_t last_render_target_height{};
        float last_width{};
        float last_height{};
        float last_x{};
        float last_y{};
    } m_overlay_data;

    void setup();
    void update_overlay();
};
} 

