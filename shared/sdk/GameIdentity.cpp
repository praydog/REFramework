#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

#include <utility/Module.hpp>

#include "GameIdentity.hpp"

namespace sdk {

GameIdentity GameIdentity::s_instance{};
bool GameIdentity::s_initialized{false};
void GameIdentity::initialize() {
    if (s_initialized) {
        return;
    }

    s_instance.detect_game();
    s_instance.derive_engine_params();
    s_initialized = true;

    spdlog::info("[GameIdentity] Detected game: {} (TDB ver {}, type_index_bits={}, field_bits={})",
        s_instance.m_target_name, s_instance.m_tdb_ver,
        s_instance.m_type_index_bits, s_instance.m_field_bits);
}

const GameIdentity& GameIdentity::get() {
    return s_instance;
}

// Detection table: maps executable stem (lowercased) to GameID.
// Some games have multiple known exe names (e.g. demo versions).
struct ExeMapping {
    std::string_view stem;  // lowercase
    GameID           id;
};

static constexpr ExeMapping s_exe_table[] = {
    // Resident Evil 2 (2019)
    {"re2",                     GameID::RE2},
    {"bhd2",                    GameID::RE2},       // Biohazard RE:2 (JP)

    // Resident Evil 3 (2020)
    {"re3",                     GameID::RE3},
    {"bhd3",                    GameID::RE3},       // Biohazard RE:3 (JP)

    // Resident Evil 4 (2023)
    {"re4",                     GameID::RE4},
    {"bhd4",                    GameID::RE4},

    // Resident Evil 7 (2017)
    {"re7",                     GameID::RE7},
    {"bhd7",                    GameID::RE7},

    // Resident Evil Village / RE8
    {"re8",                     GameID::RE8},
    {"village",                 GameID::RE8},

    // Resident Evil 9
    {"re9",                     GameID::RE9},

    // Devil May Cry 5
    {"devilmaycry5",            GameID::DMC5},
    {"dmc5",                    GameID::DMC5},

    // Monster Hunter Rise
    {"monsterhunterrise",       GameID::MHRISE},
    {"mhrise",                  GameID::MHRISE},
    {"mhrisesunbreakdemo",      GameID::MHRISE},

    // Street Fighter 6
    {"streetfighter6",          GameID::SF6},
    {"sf6",                     GameID::SF6},

    // Dragon's Dogma 2
    {"dd2",                     GameID::DD2},

    // Monster Hunter Wilds
    {"monsterhunterwilds",      GameID::MHWILDS},
    {"mhwilds",                 GameID::MHWILDS},

    // Monster Hunter Stories 3 (provisional)
    {"monster_hunter_stories_3_twisted_reflection_trial",   GameID::MHSTORIES3},
    {"monster_hunter_stories_3_twisted_reflection",              GameID::MHSTORIES3},

    // Pragmata (provisional)
    {"pragmata",                GameID::PRAGMATA},
};

void GameIdentity::detect_game() {
    const auto exe = utility::get_executable();
    const auto module_path = utility::get_module_path(exe);

    if (!module_path) {
        spdlog::error("[GameIdentity] Failed to get executable module path");
        m_game = GameID::Unknown;
        return;
    }

    // Get the stem (filename without extension), lowercased.
    auto stem = std::filesystem::path(*module_path).stem().string();
    std::transform(stem.begin(), stem.end(), stem.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    spdlog::info("[GameIdentity] Executable stem: '{}'", stem);

    // Exact match first.
    for (const auto& mapping : s_exe_table) {
        if (stem == mapping.stem) {
            m_game = mapping.id;
            return;
        }
    }

    // Substring match as fallback (for things like "re8demo", "streetfighter6_trial").
    for (const auto& mapping : s_exe_table) {
        if (stem.find(mapping.stem) != std::string::npos) {
            m_game = mapping.id;
            return;
        }
    }

    spdlog::warn("[GameIdentity] Could not identify game from executable: {}", stem);
    m_game = GameID::Unknown;
}

void GameIdentity::derive_engine_params() {
    // This mapping mirrors the old TDBVer.hpp #ifdef chain.
    // Note: the old build had variant builds (RE2_TDB66, RE3_TDB67, RE7_TDB49)
    // for older TDB versions of the same game. In the monolithic DLL, we detect
    // the TDB version from the actual running binary at SDK init time, not here.
    // The values below are the MODERN/LATEST TDB version for each game.
    // If a user runs an older game version, the TDB auto-detection in RETypeDB
    // will override this.

    switch (m_game) {
    case GameID::RE7:
        m_tdb_ver          = 70;    // Modern RE7; old RE7 was TDB49
        m_type_index_bits  = 18;
        m_field_bits       = 18;
        m_reengine_packed  = false;
        m_reengine_at      = false;
        m_game_name        = "re7";
        m_target_name      = "RE7";
        break;

    case GameID::RE2:
        m_tdb_ver          = 70;    // Modern RE2; old RE2 was TDB66
        m_type_index_bits  = 18;
        m_field_bits       = 18;
        m_reengine_packed  = false;
        m_reengine_at      = false;
        m_game_name        = "re2";
        m_target_name      = "RE2";
        break;

    case GameID::RE3:
        m_tdb_ver          = 70;    // Modern RE3; old RE3 was TDB67
        m_type_index_bits  = 18;
        m_field_bits       = 18;
        m_reengine_packed  = false;
        m_reengine_at      = false;
        m_game_name        = "re3";
        m_target_name      = "RE3";
        break;

    case GameID::DMC5:
        m_tdb_ver          = 67;
        m_type_index_bits  = 17;
        m_field_bits       = 17;
        m_reengine_packed  = false;
        m_reengine_at      = false;
        m_game_name        = "dmc5";
        m_target_name      = "DMC5";
        break;

    case GameID::RE8:
        m_tdb_ver          = 69;
        m_type_index_bits  = 18;
        m_field_bits       = 18;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "re8";
        m_target_name      = "RE8";
        break;

    case GameID::RE4:
        m_tdb_ver          = 71;
        m_type_index_bits  = 19;
        m_field_bits       = 19;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "re4";
        m_target_name      = "RE4";
        break;

    case GameID::MHRISE:
        m_tdb_ver          = 71;
        m_type_index_bits  = 19;
        m_field_bits       = 19;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "mhrise";
        m_target_name      = "MHRISE";
        break;

    case GameID::SF6:
        m_tdb_ver          = 71;
        m_type_index_bits  = 19;
        m_field_bits       = 19;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "sf6";
        m_target_name      = "SF6";
        break;

    case GameID::DD2:
        m_tdb_ver          = 73;
        m_type_index_bits  = 19;
        m_field_bits       = 19;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "dd2";
        m_target_name      = "DD2";
        break;

    case GameID::MHWILDS:
        m_tdb_ver          = 81;
        m_type_index_bits  = 19;
        m_field_bits       = 20;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "mhwilds";
        m_target_name      = "MHWILDS";
        break;

    case GameID::MHSTORIES3:
        m_tdb_ver          = 82;
        m_type_index_bits  = 19;
        m_field_bits       = 20;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "mhstories3";
        m_target_name      = "MHSTORIES3";
        break;

    case GameID::RE9:
        m_tdb_ver          = 83;
        m_type_index_bits  = 19;
        m_field_bits       = 20;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "re9";
        m_target_name      = "RE9";
        break;

    case GameID::PRAGMATA:
        m_tdb_ver          = 83; // PRAGMATA_SKETCHBOOK uses TDB 83, not 84
        m_type_index_bits  = 19;
        m_field_bits       = 20;
        m_reengine_packed  = true;
        m_reengine_at      = true;
        m_game_name        = "pragmata";
        m_target_name      = "PRAGMATA";
        break;

    case GameID::Unknown:
    default:
        m_tdb_ver          = 0;
        m_type_index_bits  = 0;
        m_field_bits       = 0;
        m_reengine_packed  = false;
        m_reengine_at      = false;
        m_game_name        = "unknown";
        m_target_name      = "UNKNOWN";
        break;
    }
}

} // namespace sdk
