#pragma once

#include <optional>

class REManagedObject;

namespace sdk {
namespace sf6 {
enum class EGameMode : uint8_t  {
    NONE = 0,
    ARCADE = 1,
    TRAINING = 2,
    VERSUS_2P = 3,
    VERSUS_CPU = 4,
    TUTORIAL = 5,
    CHARACTER_GUIDE = 6,
    MISSION = 7,
    DEATHMATCH = 8,
    STORY = 9,
    STORY_TRAINING = 10,
    STORY_MATCH = 11,
    STORY_TUTORIAL = 12,
    STORY_SPECTATE = 13,
    RANKED_MATCH = 14,
    PLAYER_MATCH = 15,
    CABINET_MATCH = 16,
    CUSTOM_ROOM_MATCH = 17,
    ONLINE_TRAINING = 18,
    TEAMBATTLE = 19,
    EXAM_CPU_MATCH = 20,
    CABINET_CPU_MATCH = 21,
    LEARNING_AI_MATCH = 22,
    LEARNING_AI_SPECTATE = 23,
    REPLAY = 24,
    SPECTATE = 25,
};

enum class HUD_GROUP_TYPE : uint8_t {
    BATTLE = 0,
    CITY = 1,
    TUTORIAL = 2,
    BATTLE_HUB = 3,
    BATTLE_OPTION = 4
};

REManagedObject* get_gui_manager();
REManagedObject* get_gui_hud_manager();
REManagedObject* get_hud_manager_base(HUD_GROUP_TYPE t);
REManagedObject* get_battle_desc();
REManagedObject* get_battle_rule(REManagedObject* battle_desc);

REManagedObject* get_network_manager();
REManagedObject* get_network_session_manager();
REManagedObject* get_network_fg_battle();
REManagedObject* get_network_battle_rule();

std::optional<uint8_t*> get_game_mode();
std::optional<uint8_t*> get_network_game_mode();
bool is_online_match();
void set_game_mode(EGameMode mode);
void set_network_game_mode(EGameMode mode);
}
}