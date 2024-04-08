#pragma once

#include <memory>

#include "Mod.hpp"

#include "reframework/API.hpp"

// API manager/proxy to call callbacks added via the C interface
class APIProxy : public Mod {
public:
    static std::shared_ptr<APIProxy>& get();

public:
    std::string_view get_name() const override { return "APIProxy"; }

    void on_present() override;
    void on_frame() override;
    void on_draw_ui() override;
    void on_lua_state_created(sol::state& state) override;
    void on_lua_state_destroyed(sol::state& state) override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_device_reset() override;
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) override;

public:
    using REFLuaStateCreatedCb = std::function<std::remove_pointer<::REFLuaStateCreatedCb>::type>;
    using REFLuaStateDestroyedCb = std::function<std::remove_pointer<::REFLuaStateDestroyedCb>::type>;
    using REFOnPresentCb = std::function<std::remove_pointer<::REFOnPresentCb>::type>;
    using REFOnPreApplicationEntryCb = std::function<std::remove_pointer<::REFOnPreApplicationEntryCb>::type>;
    using REFOnPostApplicationEntryCb = std::function<std::remove_pointer<::REFOnPostApplicationEntryCb>::type>;
    using REFOnDeviceResetCb = std::function<std::remove_pointer<::REFOnDeviceResetCb>::type>;
    using REFOnMessageCb = std::function<std::remove_pointer<::REFOnMessageCb>::type>;
    using REFOnImGuiFrameCb = std::function<std::remove_pointer<::REFOnImGuiFrameCb>::type>;

    bool add_on_lua_state_created(REFLuaStateCreatedCb cb);
    bool add_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
    bool add_on_present(REFOnPresentCb cb);
    bool add_on_pre_application_entry(std::string_view name, REFOnPreApplicationEntryCb cb);
    bool add_on_post_application_entry(std::string_view name, REFOnPostApplicationEntryCb cb);
    bool add_on_device_reset(REFOnDeviceResetCb cb);
    bool add_on_message(REFOnMessageCb cb);
    bool add_on_imgui_frame(REFOnImGuiFrameCb cb);
    bool add_on_imgui_draw_ui(REFOnImGuiDrawUICb cb);

private:
    // API Callbacks
    std::shared_mutex m_api_cb_mtx;
    std::vector<APIProxy::REFLuaStateCreatedCb> m_on_lua_state_created_cbs;
    std::vector<APIProxy::REFLuaStateDestroyedCb> m_on_lua_state_destroyed_cbs;
    std::vector<APIProxy::REFOnPresentCb> m_on_present_cbs{};
    std::vector<APIProxy::REFOnDeviceResetCb> m_on_device_reset_cbs{};
    std::vector<APIProxy::REFOnMessageCb> m_on_message_cbs{};
    std::vector<APIProxy::REFOnImGuiFrameCb> m_on_imgui_frame_cbs{};
    std::vector<APIProxy::REFOnImGuiDrawUICb> m_on_imgui_draw_ui_cbs{};

    // Application Entry Callbacks
    std::unordered_map<size_t, std::vector<APIProxy::REFOnPreApplicationEntryCb>> m_on_pre_application_entry_cbs{};
    std::unordered_map<size_t, std::vector<APIProxy::REFOnPostApplicationEntryCb>> m_on_post_application_entry_cbs{};
};