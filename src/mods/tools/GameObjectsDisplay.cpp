#include <DirectXColors.h>
#include <../../directxtk12-src/Src/d3dx12.h>


#include <../../directxtk12-src/Inc/ResourceUploadBatch.h>

#include "../spritefonts/Roboto.spritefont.h"

#include "REFramework.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/REGameObject.hpp"

#include "../BackBufferRenderer.hpp"
#include "GameObjectsDisplay.hpp"

namespace detail {
struct Invocation {
    void* object_ptr;
    void* method_ptr;
    void* unk;
};

struct Predicate : public REManagedObject {
    uint32_t num_invocations{0};
    std::array<Invocation, 10> invocations;
};
}

std::optional<std::string> GameObjectsDisplay::on_initialize_d3d_thread() {
    return initialize_d3d_resources();
}

std::optional<std::string> GameObjectsDisplay::on_initialize() {
    // OK
    return Mod::on_initialize();
}

std::optional<std::string> GameObjectsDisplay::initialize_d3d_resources() {
    if (g_framework->is_dx12()) {
        m_d3d12 = {};

        DirectX::EffectPipelineStateDescription bbuffer_psd(
            &DirectX::DX12::GeometricPrimitive::VertexType::InputLayout,
            DirectX::DX12::CommonStates::AlphaBlend,
            DirectX::DX12::CommonStates::DepthNone,
            DirectX::DX12::CommonStates::CullCounterClockwise,
            BackBufferRenderer::get()->get_default_rt_state()
        );

        auto device = g_framework->get_d3d12_hook()->get_device();

        m_d3d12.states = std::make_unique<DirectX::DX12::CommonStates>(device);

        m_d3d12.effect = std::make_unique<DirectX::DX12::BasicEffect>(device, DirectX::EffectFlags::Texture, bbuffer_psd);
        m_d3d12.effect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect->SetView(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect->SetProjection(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect->SetAlpha(1.0f);
        //m_d3d12.effect->SetDiffuseColor(DirectX::Colors::Black);

        m_d3d12.effect_no_tex = std::make_unique<DirectX::DX12::BasicEffect>(device, DirectX::EffectFlags::None, bbuffer_psd);
        m_d3d12.effect_no_tex->SetWorld(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect_no_tex->SetView(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect_no_tex->SetProjection(DirectX::SimpleMath::Matrix::Identity);
        m_d3d12.effect_no_tex->SetDiffuseColor(DirectX::Colors::Blue);
        m_d3d12.effect_no_tex->SetAlpha(0.5f);


        m_d3d12.font_heap = std::make_unique<DirectX::DescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, FontHeapIndices::FontHeap_Count);

        // Upload sprite font
        DirectX::ResourceUploadBatch upload(device);
        upload.Begin();

        m_d3d12.font = std::make_unique<DirectX::SpriteFont>(
            device, upload, Roboto_spritefont, sizeof(Roboto_spritefont), 
            m_d3d12.font_heap->GetCpuHandle(FontHeapIndices::SpriteFont_Data),
            m_d3d12.font_heap->GetGpuHandle(FontHeapIndices::SpriteFont_Data)
        );
        
        DirectX::RenderTargetState output_state{DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN};
        DirectX::SpriteBatchPipelineStateDescription pd{output_state};

        m_d3d12.sprite_batch = std::make_unique<DirectX::DX12::SpriteBatch>(device, upload, pd);

        auto finished = upload.End(g_framework->get_d3d12_hook()->get_command_queue());
        finished.wait();

        m_d3d12.quad = DirectX::GeometricPrimitive::CreateBox({1.0f, 1.0f, 1.0f});

        // Create text texture
        d3d12::ComPtr<ID3D12Resource> text_texture{};
        const auto tex_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 512, 512, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &tex_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&text_texture)))) {
            return "Failed to create text texture";
        }

        m_d3d12.text_texture = std::make_unique<d3d12::TextureContext>();
        if (!m_d3d12.text_texture->setup(device, text_texture.Get(), std::nullopt, std::nullopt, L"GameObjectsDisplay Text Texture")) {
            return "Failed to setup text texture";
        }

        m_d3d12.effect->SetTexture(m_d3d12.text_texture->get_srv_gpu(), m_d3d12.states->LinearWrap());

        spdlog::info("GameObjectsDisplay D3D12 initialized");
    } else {
        // TODO
    }
    
    m_needs_d3d_init = false;

    return std::nullopt;
}

void GameObjectsDisplay::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void GameObjectsDisplay::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}


void GameObjectsDisplay::on_draw_dev_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // todo
    }

    m_max_distance->draw("Max Distance for GameObjects");

    if (ImGui::SliderFloat("Object Effect Alpha", &m_effect_alpha, 0.0f, 1.0f)) {
        m_effect_dirty = true;
    }
}

void GameObjectsDisplay::on_present() {
    if (m_needs_d3d_init) {
        if (auto result = initialize_d3d_resources(); result.has_value()) {
            spdlog::error(result.value());
        }
    }

    if (m_effect_dirty) {
        m_d3d12.effect_no_tex->SetAlpha(m_effect_alpha);
        m_effect_dirty = false;
    }
}

void GameObjectsDisplay::on_frame() {
    if (!m_enabled->value() || m_needs_d3d_init) {
        return;
    }

    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::find_type_definition("via.Scene");
    auto first_transform = sdk::call_native_func_easy<RETransform*>(scene, scene_def, "get_FirstTransform");

    if (first_transform == nullptr) {
        return;
    }

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_position_method = transform_def->get_method("get_Position");
    static auto get_axisz_method = transform_def->get_method("get_AxisZ");
    static auto get_world_matrix_method = transform_def->get_method("get_WorldMatrix");

    auto math = sdk::get_native_singleton("via.math");
    auto math_t = sdk::find_type_definition("via.math");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return;
    }

    auto camera_gameobject = get_gameobject_method->call<REGameObject*>(context, camera);
    auto camera_transform = camera_gameobject->transform;

    Vector4f camera_origin{};
    get_position_method->call<void*>(&camera_origin, context, camera_transform);

    camera_origin.w = 1.0f;

    /*Vector4f camera_forward{};
    get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    camera_forward.w = 1.0f;*/

    // Translate 2d position to 3d position (screen to world)
    __declspec(align(16)) Matrix4x4f proj{}, view{};
    float screen_size[2]{};
    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    sdk::call_object_func<void*>(main_view, "get_WindowSize", &screen_size, context, main_view);

    // Extract the camera forward vector from the view matrix
    auto camera_forward = *(Vector4f*)&view[2];
    //auto camera_origin = *(Vector4f*)&view[3];

    static auto world_to_screen = math_t->get_method("worldPos2ScreenPos(via.vec3, via.mat4, via.mat4, via.Size)");

    __declspec(align(16)) Vector4f pos{};
    __declspec(align(16)) Vector4f screen_pos{};

    auto draw_list = ImGui::GetBackgroundDrawList();
    const auto has_max_distance = m_max_distance->value() > 0.0f;

    const auto is_d3d12 = g_framework->is_dx12();

    auto proj_directx = DirectX::SimpleMath::Matrix{&proj[0][0]};
    auto view_directx = DirectX::SimpleMath::Matrix{&view[0][0]};

    std::vector<BackBufferRenderer::D3D12RenderWorkFn> d3d12_work{};

    // Set the view and projection matrices for the effect once per frame
    if (is_d3d12) {
        d3d12_work.emplace_back([this, proj_directx, view_directx](const BackBufferRenderer::RenderWorkData& data) {
            // Viewport for our text texture
            D3D12_VIEWPORT rt_viewport{};
            rt_viewport.Width = 512.0f;
            rt_viewport.Height = 512.0f;
            rt_viewport.MinDepth = D3D12_MIN_DEPTH;
            rt_viewport.MaxDepth = D3D12_MAX_DEPTH;
            m_d3d12.sprite_batch->SetViewport(rt_viewport);

            ID3D12DescriptorHeap* heaps[] = { 
                m_d3d12.font_heap->Heap(), 
                m_d3d12.states->Heap()
            };
            data.command_list->SetDescriptorHeaps(2, heaps);

            m_d3d12.effect->SetProjection(proj_directx);
            m_d3d12.effect->SetView(view_directx);
            m_d3d12.effect->SetTexture(m_d3d12.text_texture->get_srv_gpu(), m_d3d12.states->LinearWrap());

            m_d3d12.effect_no_tex->SetProjection(proj_directx);
            m_d3d12.effect_no_tex->SetView(view_directx);
        });
    }

    __declspec(align(16)) Matrix4x4f world_matrix{};

    for (auto transform = first_transform; 
        transform != nullptr; 
        transform = next_transform_method->call<RETransform*>(context, transform)) 
    {
        auto owner = get_gameobject_method->call<REGameObject*>(context, transform);

        if (owner == nullptr) {
            continue;
        }

        auto owner_name = utility::re_game_object::get_name(owner);

        if (owner_name.empty()) {
            continue;
        }

        get_position_method->call<void*>(&pos, context, transform);
        pos.w = 1.0f;

        const auto delta = pos - camera_origin;

        // behind camera
        /*if (glm::dot(Vector3f{delta}, Vector3f{camera_forward}) <= 0.0f) {
            continue;
        }*/

        const auto distance = glm::length(Vector3f{delta});

        if (has_max_distance) {
            if (distance > m_max_distance->value()) {
                continue;
            }
        }

        if (is_d3d12) {
            get_world_matrix_method->call<void*>(&world_matrix, context, transform);

            DirectX::SimpleMath::Matrix world = 
                DirectX::SimpleMath::Matrix{&world_matrix[0][0]};

            d3d12_work.emplace_back([this, owner_name, world](const BackBufferRenderer::RenderWorkData& data) {
                // Create barrier to transition to render target
                D3D12_RESOURCE_BARRIER barrier{};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Transition.pResource = m_d3d12.text_texture->texture.Get();
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                data.command_list->ResourceBarrier(1, &barrier);

                D3D12_CPU_DESCRIPTOR_HANDLE rtv_heaps[] = { m_d3d12.text_texture->get_rtv() };
                data.command_list->OMSetRenderTargets(1, rtv_heaps, FALSE, nullptr);

                // Set viewport to text texture
                D3D12_VIEWPORT viewport{};
                viewport.Width = 512.0f;
                viewport.Height = 512.0f;
                viewport.MinDepth = D3D12_MIN_DEPTH;
                viewport.MaxDepth = D3D12_MAX_DEPTH;
                data.command_list->RSSetViewports(1, &viewport);

                // Set scissors to text texture
                D3D12_RECT scissor_rect{};
                scissor_rect.left = 0;
                scissor_rect.top = 0;
                scissor_rect.right = 512;
                scissor_rect.bottom = 512;
                data.command_list->RSSetScissorRects(1, &scissor_rect);

                // Clear text texture
                data.command_list->ClearRenderTargetView(m_d3d12.text_texture->get_rtv(), DirectX::Colors::Transparent, 0, nullptr);
                
                m_d3d12.sprite_batch->Begin(data.command_list);
                
                const auto measurements = m_d3d12.font->MeasureString(owner_name.c_str());
                m_d3d12.font->DrawString(m_d3d12.sprite_batch.get(), owner_name.c_str(), DirectX::SimpleMath::Vector2(256.0f - glm::min(DirectX::XMVectorGetX(measurements) / 2.0f, 256.0f), 256.0f), DirectX::Colors::White);

                m_d3d12.sprite_batch->End();

                // Transition back to pixel shader resource
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                data.command_list->ResourceBarrier(1, &barrier);

                // Reset viewport to backbuffer
                data.command_list->RSSetViewports(1, &data.viewport);

                // Reset scissors to backbuffer
                scissor_rect.right = (LONG)data.viewport.Width;
                scissor_rect.bottom = (LONG)data.viewport.Height;
                data.command_list->RSSetScissorRects(1, &scissor_rect);

                // Reset render target to backbuffer
                D3D12_CPU_DESCRIPTOR_HANDLE bbrtv_heaps[] = { data.backbuffer_ctx->get_rtv() };
                data.command_list->OMSetRenderTargets(1, bbrtv_heaps, FALSE, nullptr);

                if (m_effect_alpha > 0.0f) {
                    m_d3d12.effect_no_tex->SetWorld(world);
                    m_d3d12.effect_no_tex->Apply(data.command_list);
                    m_d3d12.quad->Draw(data.command_list);
                }

                m_d3d12.effect->SetWorld(world);
                m_d3d12.effect->Apply(data.command_list);
                m_d3d12.quad->Draw(data.command_list);
            });
        } else {
            world_to_screen->call<void*>(&screen_pos, context, &pos, &view, &proj, &screen_size);
            draw_list->AddText(ImVec2(screen_pos.x, screen_pos.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), owner_name.c_str());
        }
    }

    if (is_d3d12) {
        BackBufferRenderer::get()->submit_work_d3d12(std::move(d3d12_work));
    }
}

