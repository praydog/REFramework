#pragma once

#include <array>
#include <unordered_set>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <utility/Patch.hpp>

#include "mods/vr/d3d12/CommandContext.hpp"

class Mods;
class REGlobals;
class RETypes;

#include "D3D11Hook.hpp"
#include "D3D12Hook.hpp"
#include "DInputHook.hpp"
#include "WindowsMessageHook.hpp"

// Global facilitator
class REFramework {
private:
    void hook_monitor();

public:
    REFramework(HMODULE reframework_module);
    virtual ~REFramework();

    auto get_reframework_module() const { return m_reframework_module; }

    bool is_valid() const { return m_valid; }

    bool is_dx11() const { return m_is_d3d11; }

    bool is_dx12() const { return m_is_d3d12; }

    const auto& get_mods() const { return m_mods; }

    const auto& get_mouse_delta() const { return m_mouse_delta; }
    const auto& get_keyboard_state() const { return m_last_keys; }

    Address get_module() const { return m_game_module; }

    bool is_ready() const { return m_initialized && m_game_data_initialized; }

    void run_imgui_frame(bool from_present);

    void on_frame_d3d11();
    void on_post_present_d3d11();
    void on_frame_d3d12();
    void on_post_present_d3d12();
    void on_reset();

    void patch_set_cursor_pos();
    void remove_set_cursor_pos_patch();

    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);
    void on_direct_input_keys(const std::array<uint8_t, 256>& keys);

    static inline bool s_fallback_appdata{false};
    static inline bool s_checked_file_permissions{false};
    static std::filesystem::path get_persistent_dir();
    static std::filesystem::path get_persistent_dir(const std::string& dir) {
        return get_persistent_dir() / dir;
    }

    void save_config();

    enum class RendererType : uint8_t {
        D3D11,
        D3D12
    };
    
    auto get_renderer_type() const { return m_renderer_type; }
    auto& get_d3d11_hook() const { return m_d3d11_hook; }
    auto& get_d3d12_hook() const { return m_d3d12_hook; }

    auto get_window() const { return m_wnd; }
    auto get_last_window_pos() const { return m_last_window_pos; } // REFramework imgui window
    auto get_last_window_size() const { return m_last_window_size; } // REFramework imgui window

    static const char* get_game_name() {
    #if defined(RE2)
        return "re2";
    #elif defined(RE3)
        return "re3";
    #elif defined(RE4)
        return "re4";
    #elif defined(RE7)
        return "re7";
    #elif defined(RE8)
        return "re8";
    #elif defined(DMC5)
        return "dmc5";
    #elif defined(MHRISE)
        return "mhrise";
    #elif defined(SF6)
        return "sf6";
    #elif defined(DD2)
        return "dd2";
    #else
        return "unknown";
    #endif
    }

    bool is_drawing_ui() const {
        return m_draw_ui;
    }

    void set_draw_ui(bool state, bool should_save = true);

    auto& get_hook_monitor_mutex() {
        return m_hook_monitor_mutex;
    }

    void set_font_size(int size) { 
        if (m_font_size != size) {
            m_font_size = size;
            m_fonts_need_updating = true;
        }
    }

    auto get_font_size() const { return m_font_size; }

    int add_font(const std::filesystem::path& filepath, int size, const std::vector<ImWchar>& ranges = {});

    ImFont* get_font(int index) const {
        if (index >= 0 && index < m_additional_fonts.size()) {
            return m_additional_fonts[index].font;
        } else {
            return nullptr;
        }
    }

private:
    void consume_input();
    void update_fonts();
    void invalidate_device_objects();

    void draw_ui();
    void draw_about();

    bool hook_d3d11();
    bool hook_d3d12();

    bool initialize();
    bool initialize_game_data();
    bool initialize_windows_message_hook();

    bool first_frame_initialize();

    void call_on_frame();

    HMODULE m_reframework_module{};

    bool m_first_frame{true};
    bool m_first_frame_d3d_initialize{true};
    bool m_is_d3d12{false};
    bool m_is_d3d11{false};
    bool m_valid{false};
    bool m_initialized{false};
    bool m_created_default_cfg{false};
    bool m_started_game_data_thread{false};
    std::atomic<bool> m_terminating{false}; // Destructor is called
    std::atomic<bool> m_game_data_initialized{false};
    std::atomic<bool> m_mods_fully_initialized{false};
    
    // UI
    bool m_has_frame{false};
    bool m_wants_device_object_cleanup{false};
    bool m_draw_ui{true};
    bool m_last_draw_ui{m_draw_ui};
    bool m_is_ui_focused{false};
    bool m_cursor_state{false};
    bool m_cursor_state_changed{true};
    bool m_ui_option_transparent{true};
    bool m_ui_passthrough{false};
    
    ImVec2 m_last_window_pos{};
    ImVec2 m_last_window_size{};

    struct AdditionalFont {
        std::filesystem::path filepath{};
        int size{16};
        std::vector<ImWchar> ranges{};
        ImFont* font{};
    };

    bool m_fonts_need_updating{true};
    int m_font_size{16};
    std::vector<AdditionalFont> m_additional_fonts{};

    std::mutex m_input_mutex{};
    std::recursive_mutex m_config_mtx{};
    std::recursive_mutex m_imgui_mtx{};
    std::recursive_mutex m_patch_mtx{};

    HWND m_wnd{0};
    HMODULE m_game_module{0};

    float m_accumulated_mouse_delta[2]{};
    float m_mouse_delta[2]{};
    std::array<uint8_t, 256> m_last_keys{0};
    std::unique_ptr<D3D11Hook> m_d3d11_hook{};
    std::unique_ptr<D3D12Hook> m_d3d12_hook{};
    std::unique_ptr<WindowsMessageHook> m_windows_message_hook;
    std::unique_ptr<DInputHook> m_dinput_hook;
    std::shared_ptr<spdlog::logger> m_logger;
    Patch::Ptr m_set_cursor_pos_patch{};

    std::string m_error{""};

    // Game-specific stuff
    std::unique_ptr<Mods> m_mods;

    std::recursive_mutex m_hook_monitor_mutex{};
    std::recursive_mutex m_startup_mutex{};
    std::unique_ptr<std::jthread> m_d3d_monitor_thread{};
    std::chrono::steady_clock::time_point m_last_present_time{};
    std::chrono::steady_clock::time_point m_last_message_time{};
    std::chrono::steady_clock::time_point m_last_sendmessage_time{};
    std::chrono::steady_clock::time_point m_last_chance_time{};
    uint32_t m_frames_since_init{0};
    bool m_has_last_chance{true};
    bool m_first_initialize{true};

    bool m_sent_message{false};
    bool m_message_hook_requested{false};

    RendererType m_renderer_type{RendererType::D3D11};

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: // D3D misc
    void set_imgui_style() noexcept;

private: // D3D11 Init
    bool init_d3d11();
    void deinit_d3d11();

private: // D3D12 Init
    bool init_d3d12();
    void deinit_d3d12();

private: // D3D11 members
    struct D3D11 {
        ComPtr<ID3D11Texture2D> blank_rt{};
		ComPtr<ID3D11Texture2D> rt{};
        ComPtr<ID3D11RenderTargetView> blank_rt_rtv{};
		ComPtr<ID3D11RenderTargetView> rt_rtv{};
		ComPtr<ID3D11ShaderResourceView> rt_srv{};
        uint32_t rt_width{};
        uint32_t rt_height{};
		ComPtr<ID3D11RenderTargetView> bb_rtv{};
    } m_d3d11{};

public:
    auto& get_blank_rendertarget_d3d11() { return m_d3d11.blank_rt; }
    auto& get_rendertarget_d3d11() { return m_d3d11.rt; }
    auto get_rendertarget_width_d3d11() const { return m_d3d11.rt_width; }
    auto get_rendertarget_height_d3d11() const { return m_d3d11.rt_height; }

private: // D3D12 members
    struct D3D12 {
        std::vector<std::unique_ptr<d3d12::CommandContext>> cmd_ctxs{};
        uint32_t cmd_ctx_index{0};

        enum class RTV : int{
            BACKBUFFER_0,
            BACKBUFFER_1,
            BACKBUFFER_2,
            IMGUI,
            BLANK,
            COUNT,
        };

        enum class SRV : int {
            IMGUI_FONT_BACKBUFFER,
            IMGUI_FONT_VR,
            IMGUI_VR,
            BLANK,
            COUNT
        };

        ComPtr<ID3D12DescriptorHeap> rtv_desc_heap{};
        ComPtr<ID3D12DescriptorHeap> srv_desc_heap{};
        ComPtr<ID3D12Resource> rts[(int)RTV::COUNT]{};

        auto& get_rt(RTV rtv) { return rts[(int)rtv]; }

        D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_rtv(ID3D12Device* device, RTV rtv) {
            return {rtv_desc_heap->GetCPUDescriptorHandleForHeapStart().ptr +
                    (SIZE_T)rtv * (SIZE_T)device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)};
        }

        D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_srv(ID3D12Device* device, SRV srv) {
            return {srv_desc_heap->GetCPUDescriptorHandleForHeapStart().ptr +
                    (SIZE_T)srv * (SIZE_T)device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)};
        }

        D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_srv(ID3D12Device* device, SRV srv) {
            return {srv_desc_heap->GetGPUDescriptorHandleForHeapStart().ptr +
                    (SIZE_T)srv * (SIZE_T)device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)};
        }

        uint32_t rt_width{};
        uint32_t rt_height{};

        std::array<void*, 2> imgui_backend_datas{};
    } m_d3d12{};

public:
    auto& get_blank_rendertarget_d3d12() { return m_d3d12.get_rt(D3D12::RTV::BLANK); }
    auto& get_rendertarget_d3d12() { return m_d3d12.get_rt(D3D12::RTV::IMGUI); }
    auto get_rendertarget_width_d3d12() { return m_d3d12.rt_width; }
    auto get_rendertarget_height_d3d12() { return m_d3d12.rt_height; }

private:
};

extern std::unique_ptr<REFramework> g_framework;
