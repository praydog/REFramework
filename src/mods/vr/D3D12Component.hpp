#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

class VR;

namespace vrmod {
class D3D12Component {
public:
    void on_frame(VR* vr);
    void on_reset(VR* vr);

private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12CommandAllocator> m_cmd_allocator{};
	ComPtr<ID3D12GraphicsCommandList> m_cmd_list{};
	ComPtr<ID3D12Fence> m_fence{};
	UINT64 m_fence_value{};
	HANDLE m_fence_event{};

	ComPtr<ID3D12Resource> m_left_eye_tex{};
	ComPtr<ID3D12Resource> m_right_eye_tex{}; 

    void setup();
    void copy_texture(ID3D12Resource* src, ID3D12Resource* dst);
};
}