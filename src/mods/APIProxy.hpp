#pragma once

#include <memory>
#include <mutex>

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
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
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
    using REFOnImGuiDrawUICb = std::function<std::remove_pointer<::REFOnImGuiDrawUICb>::type>;
    using REFOnPreGuiDrawElementCb = std::function<std::remove_pointer<::REFOnPreGuiDrawElementCb>::type>;

    bool add_on_lua_state_created(REFLuaStateCreatedCb cb);
    bool add_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
    bool add_on_present(REFOnPresentCb cb);
    bool add_on_pre_application_entry(std::string_view name, REFOnPreApplicationEntryCb cb);
    bool add_on_post_application_entry(std::string_view name, REFOnPostApplicationEntryCb cb);
    bool add_on_device_reset(REFOnDeviceResetCb cb);
    bool add_on_message(REFOnMessageCb cb);
    bool add_on_imgui_frame(REFOnImGuiFrameCb cb);
    bool add_on_imgui_draw_ui(REFOnImGuiDrawUICb cb);
    bool add_on_pre_gui_draw_element(REFOnPreGuiDrawElementCb cb);

private:
    // Callback lists are immutable once published: registration copies the list, appends and swaps the pointer,
    // and dispatch iterates a snapshot without holding m_api_cb_mtx. A plugin callback may therefore register
    // more callbacks (e.g. on its first on_present) without deadlocking on the mutex its dispatcher holds.
    template <typename T>
    using CallbackList = std::shared_ptr<const std::vector<T>>;

    template <typename T>
    static void append_locked(CallbackList<T>& list, T cb) {
        auto next = list != nullptr ? std::make_shared<std::vector<T>>(*list) : std::make_shared<std::vector<T>>();
        next->push_back(std::move(cb));
        list = std::move(next);
    }

    template <typename T>
    CallbackList<T> snapshot(const CallbackList<T>& list) {
        std::scoped_lock _{m_api_cb_mtx};
        return list;
    }

    template <typename T>
    CallbackList<T> snapshot(const std::unordered_map<size_t, CallbackList<T>>& map, size_t hash) {
        std::scoped_lock _{m_api_cb_mtx};

        if (auto it = map.find(hash); it != map.end()) {
            return it->second;
        }

        return nullptr;
    }

    // API Callbacks
    std::mutex m_api_cb_mtx;
    CallbackList<APIProxy::REFLuaStateCreatedCb> m_on_lua_state_created_cbs{};
    CallbackList<APIProxy::REFLuaStateDestroyedCb> m_on_lua_state_destroyed_cbs{};
    CallbackList<APIProxy::REFOnPresentCb> m_on_present_cbs{};
    CallbackList<APIProxy::REFOnDeviceResetCb> m_on_device_reset_cbs{};
    CallbackList<APIProxy::REFOnMessageCb> m_on_message_cbs{};
    CallbackList<APIProxy::REFOnImGuiFrameCb> m_on_imgui_frame_cbs{};
    CallbackList<APIProxy::REFOnImGuiDrawUICb> m_on_imgui_draw_ui_cbs{};
    CallbackList<APIProxy::REFOnPreGuiDrawElementCb> m_on_pre_gui_draw_element_cbs{};

    // Application Entry Callbacks
    std::unordered_map<size_t, CallbackList<APIProxy::REFOnPreApplicationEntryCb>> m_on_pre_application_entry_cbs{};
    std::unordered_map<size_t, CallbackList<APIProxy::REFOnPostApplicationEntryCb>> m_on_post_application_entry_cbs{};
};