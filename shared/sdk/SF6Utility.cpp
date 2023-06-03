#include "RETypeDB.hpp"
#include "SystemArray.hpp"

#include "SF6Utility.hpp"

namespace sdk {
namespace sf6 {
REManagedObject* get_gui_manager() {
    return sdk::get_managed_singleton<REManagedObject>("app.GuiManager");
}

REManagedObject* get_gui_hud_manager() {
    const auto gui_manager = get_gui_manager();

    if (gui_manager == nullptr) {
        return nullptr;
    }

    const auto hud = sdk::get_object_field<::REManagedObject*>(gui_manager, "<Hud>k__BackingField");

    if (hud == nullptr) {
        return nullptr;
    }

    return *hud;
}

REManagedObject* get_hud_manager_base(HUD_GROUP_TYPE t) {
    const auto gui_hud_manager = get_gui_hud_manager();

    if (gui_hud_manager == nullptr) {
        return nullptr;
    }

    if (t == HUD_GROUP_TYPE::BATTLE) {
        auto hud_manager_list = sdk::get_object_field<sdk::SystemArray*>(gui_hud_manager, "mHudManagerList");

        if (hud_manager_list == nullptr || *hud_manager_list == nullptr) {
            return nullptr;
        }

        static const auto battle_hud_manager_t = sdk::find_type_definition("app.BattleHudManager");

        if (battle_hud_manager_t == nullptr) {
            return nullptr;
        }

        for (::REManagedObject* it : **hud_manager_list) {
            if (it != nullptr && utility::re_managed_object::get_type_definition(it) == battle_hud_manager_t) {
                return it;
            }
        }
    }

    return sdk::call_object_func_easy<::REManagedObject*>(gui_hud_manager, "GetHudManagerBase", (uint32_t)t);
}

::REManagedObject* get_battle_desc() {
    const auto hud_manager = get_hud_manager_base(HUD_GROUP_TYPE::BATTLE);

    if (hud_manager == nullptr) {
        return nullptr;
    }

    const auto desc = sdk::get_object_field<::REManagedObject*>(hud_manager, "<BattleDesc>k__BackingField");

    if (desc == nullptr) {
        return nullptr;
    }

    return *desc;
}


::REManagedObject* get_battle_rule(::REManagedObject* battle_desc) {
    if (battle_desc == nullptr) {
        return nullptr;
    }

    const auto rule = sdk::get_object_field<::REManagedObject*>(battle_desc, "Rule");

    if (rule == nullptr) {
        return nullptr;
    }

    return *rule;
}

REManagedObject* get_network_manager() {
    return sdk::get_managed_singleton<REManagedObject>("app.network.NetworkManager");
}

REManagedObject* get_network_session_manager() {
    const auto network_manager = get_network_manager();

    if (network_manager == nullptr) {
        return nullptr;
    }

    const auto session_manager = sdk::get_object_field<::REManagedObject*>(network_manager, "<Session>k__BackingField");

    if (session_manager == nullptr) {
        return nullptr;
    }

    return *session_manager;
}

REManagedObject* get_network_fg_battle() {
    const auto session_manager = get_network_session_manager();

    if (session_manager == nullptr) {
        return nullptr;
    }

    const auto fg_battle = sdk::get_object_field<::REManagedObject*>(session_manager, "<FGBattle>k__BackingField");

    if (fg_battle == nullptr) {
        return nullptr;
    }

    return *fg_battle;
}

REManagedObject* get_network_battle_rule() {
    const auto network_fg_battle = get_network_fg_battle();

    if (network_fg_battle == nullptr) {
        return nullptr;
    }

    const auto battle_rule = sdk::get_object_field<::REManagedObject*>(network_fg_battle, "<BattleRule>k__BackingField");

    if (battle_rule == nullptr) {
        return nullptr;
    }

    return *battle_rule;
}

std::optional<uint8_t*> get_network_game_mode() {
    const auto network_battle_rule = get_network_battle_rule();

    if (network_battle_rule == nullptr) {
        return std::nullopt;
    }

    const auto game_mode = sdk::get_object_field<uint8_t>(network_battle_rule, "GameMode");

    if (game_mode == nullptr) {
        return std::nullopt;
    }

    return game_mode;
}

std::optional<uint8_t*> get_game_mode() {
    const auto battle_desc = get_battle_desc();

    if (battle_desc == nullptr) {
        return std::nullopt;
    }

    const auto battle_rule = get_battle_rule(battle_desc);

    if (battle_rule == nullptr) {
        return std::nullopt;
    }

    const auto game_mode = sdk::get_object_field<uint8_t>(battle_rule, "GameMode");

    if (game_mode == nullptr) {
        return std::nullopt;
    }

    return game_mode;
}

void set_game_mode(EGameMode mode) {
    const auto game_mode = get_game_mode();

    if (game_mode.has_value()) {
        **game_mode = (uint8_t)mode;
    }
}

void set_network_game_mode(EGameMode mode) {
    const auto network_game_mode = get_network_game_mode();

    if (network_game_mode.has_value()) {
        **network_game_mode = (uint8_t)mode;
    }
}

bool is_online_match() {
    const auto network_game_mode = get_network_game_mode();

    if (network_game_mode.has_value()) {
        switch ((EGameMode)**network_game_mode) {
            case EGameMode::RANKED_MATCH:
            case EGameMode::PLAYER_MATCH:
            case EGameMode::CABINET_MATCH:
            case EGameMode::CUSTOM_ROOM_MATCH:
            case EGameMode::ONLINE_TRAINING:
                return true;

            default:
                break;
        }
    }

    const auto game_mode = get_game_mode();

    if (game_mode.has_value()) {
        switch ((EGameMode)**game_mode) {
            case EGameMode::RANKED_MATCH:
            case EGameMode::PLAYER_MATCH:
            case EGameMode::CABINET_MATCH:
            case EGameMode::CUSTOM_ROOM_MATCH:
            case EGameMode::ONLINE_TRAINING:
                return true;

            default:
                break;
        }
    }

    return false;
}
}
}