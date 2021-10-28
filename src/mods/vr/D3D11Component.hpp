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

private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture2D> m_left_eye_tex{};
    ComPtr<ID3D11Texture2D> m_right_eye_tex{};

    void setup();
};
} 

