// Even though this is called RE8VR, it also affects RE7.
#pragma once

#if defined(RE7) || defined(RE8)
#include <chrono>
#include <sdk/REMath.hpp>
#include <reframework/API.h>

#include "HookManager.hpp"
#include "../../../Mod.hpp"

class RE8VR : public Mod {
public:
    static std::shared_ptr<RE8VR>& get();

    std::string_view get_name() const override {
        return "RE8VR";
    }

    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_lua_state_created(sol::state& lua) override;
    void on_lua_state_destroyed(sol::state& lua) override;

    void on_draw_ui() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    // non-virtual callbacks
    void on_pre_lock_scene(void* entry);

    void reset_data();

    void set_hand_joints_to_tpose(::REManagedObject* hand_ik);
    void update_hand_ik();
    void update_body_ik(glm::quat* camera_rotation, Vector4f* camera_pos);
    void update_player_gestures();
    void fix_player_camera(::REManagedObject* player_camera);
    void fix_player_shadow();

    ::REGameObject* get_localplayer() const;
    ::REManagedObject* get_weapon_object(::REGameObject* player) const;

    bool update_pointers();
    bool update_ik_pointers();

private:
    void update_block_gesture();
    void update_heal_gesture();

    static HookManager::PreHookResult pre_shadow_late_update(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys);
    static void post_shadow_late_update(uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty);

private:
    const ModToggle::Ptr m_hide_upper_body{ ModToggle::create(generate_name("HideUpperBody"), false) };
    const ModToggle::Ptr m_hide_lower_body{ ModToggle::create(generate_name("HideLowerBody"), false) };
    const ModToggle::Ptr m_hide_arms{ ModToggle::create(generate_name("HideArms"), false) };
    const ModToggle::Ptr m_hide_upper_body_cutscenes{ ModToggle::create(generate_name("AutoHideUpperBodyCutscenes"), true) };
    const ModToggle::Ptr m_hide_lower_body_cutscenes{ ModToggle::create(generate_name("AutoHideLowerBodyCutscenes"), true) };

    ValueList m_options {
        *m_hide_upper_body,
        *m_hide_lower_body,
        *m_hide_arms,
        *m_hide_upper_body_cutscenes,
        *m_hide_lower_body_cutscenes
    };

    union {
        ::REGameObject* m_player{nullptr};
        ::REManagedObject* m_player_downcast;
    };

    ::RETransform* m_transform{nullptr};

    ::REManagedObject* m_inventory{nullptr};
    ::REManagedObject* m_updater{nullptr};
    ::REManagedObject* m_weapon{nullptr};
    ::REManagedObject* m_hand_touch{nullptr};
    ::REManagedObject* m_order{nullptr};
    ::REManagedObject* m_status{nullptr};
    ::REManagedObject* m_event_action_controller{nullptr};
    ::REManagedObject* m_game_event_action_controller{nullptr};
    ::REManagedObject* m_hit_controller{nullptr};

    ::REManagedObject* m_left_hand_ik{nullptr};
    ::REManagedObject* m_right_hand_ik{nullptr};
    ::RETransform* m_left_hand_ik_transform{nullptr};
    ::RETransform* m_right_hand_ik_transform{nullptr};
    ::REManagedObject* m_left_hand_ik_object{nullptr};
    ::REManagedObject* m_right_hand_ik_object{nullptr};

    Vector4f m_left_hand_position_offset{};
    Vector4f m_right_hand_position_offset{};
    glm::quat m_left_hand_rotation_offset{};
    glm::quat m_right_hand_rotation_offset{};

    Vector4f m_last_right_hand_position{};
    glm::quat m_last_right_hand_rotation{};
    Vector4f m_last_left_hand_position{};
    glm::quat m_last_left_hand_rotation{};

    bool m_was_gripping_weapon{false};
    bool m_is_holding_left_grip{false};
    bool m_is_in_cutscene{false};
    bool m_is_grapple_aim{false};
    bool m_is_reloading{false};
    bool m_can_use_hands{true};
    bool m_is_motion_play{false};
    bool m_has_vehicle{false};

    bool m_wants_block{false};
    bool m_wants_heal{false};

    Vector3f m_hmd_delta_to_left{};
    Vector3f m_hmd_delta_to_right{};

    Vector3f m_hmd_dir_to_left{};
    Vector3f m_hmd_dir_to_right{};

    float m_delta_time{1.0f/60.0f};

    struct HealGesture {
        bool was_grip_down{false};
        bool was_trigger_down{false};
        ::REManagedObject* last_grip_weapon{nullptr};
        std::chrono::steady_clock::time_point last_grab_time{};

        glm::quat re8_medicine_rotation{0.728f, 0.409f, 0.222f, 0.504f};
    };

    HealGesture m_heal_gesture;
};
#endif