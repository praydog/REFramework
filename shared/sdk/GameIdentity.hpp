#pragma once

#include <cstdint>
#include <string_view>

namespace sdk {

// Every RE Engine game supported by REFramework.
// Values are stable; do not reorder. Append-only.
enum class GameID : uint8_t {
    Unknown = 0,
    RE2,
    RE3,
    RE4,
    RE7,
    RE8,        // Resident Evil Village
    RE9,
    DMC5,
    MHRISE,
    SF6,
    DD2,
    MHWILDS,
    MHSTORIES3,
    PRAGMATA,
    Count
};

// Singleton that identifies the running game and its engine parameters.
// Initialised once at DLL load time before any SDK work happens.
//
// IMPORTANT: this replaces the old compile-time macros:
//   RE2, RE3, RE4, RE7, RE8, RE9, DMC5, MHRISE, SF6, DD2, MHWILDS,
//   MHSTORIES3, PRAGMATA, REENGINE_PACKED, REENGINE_AT,
//   TDB_VER, TYPE_INDEX_BITS, FIELD_BITS
//
// Usage:
//   const auto& id = sdk::GameIdentity::get();
//   if (id.tdb_ver() >= 69) { ... }
//   if (id.game() == sdk::GameID::RE8) { ... }
//   if (id.is_reengine_at()) { ... }

class GameIdentity {
public:
    // Must be called once during DLL_PROCESS_ATTACH, before anything else.
    static void initialize();

    // Singleton accessor. Only valid after initialize().
    static const GameIdentity& get();

    GameID  game()              const { return m_game; }
    int     tdb_ver()           const { return m_tdb_ver; }
    int     type_index_bits()   const { return m_type_index_bits; }
    int     field_bits()        const { return m_field_bits; }
    bool    is_reengine_packed()const { return m_reengine_packed; }
    bool    is_reengine_at()    const { return m_reengine_at; }

    // Short lowercase name used for config dirs, lua scripts, etc.
    // Examples: "re2", "re3", "dmc5", "mhwilds"
    std::string_view game_name() const { return m_game_name; }

    // The REFRAMEWORK_GAME_NAME equivalent (uppercase target name).
    // Examples: "RE2", "DMC5", "MHWILDS"
    std::string_view target_name() const { return m_target_name; }

    // Convenience helpers that mirror old #ifdef patterns.
    bool is_re2()       const { return m_game == GameID::RE2; }
    bool is_re3()       const { return m_game == GameID::RE3; }
    bool is_re4()       const { return m_game == GameID::RE4; }
    bool is_re7()       const { return m_game == GameID::RE7; }
    bool is_re8()       const { return m_game == GameID::RE8; }
    bool is_re9()       const { return m_game == GameID::RE9; }
    bool is_dmc5()      const { return m_game == GameID::DMC5; }
    bool is_mhrise()    const { return m_game == GameID::MHRISE; }
    bool is_sf6()       const { return m_game == GameID::SF6; }
    bool is_dd2()       const { return m_game == GameID::DD2; }
    bool is_mhwilds()   const { return m_game == GameID::MHWILDS; }
    bool is_mhstories3()const { return m_game == GameID::MHSTORIES3; }
    bool is_pragmata()  const { return m_game == GameID::PRAGMATA; }

private:
    GameIdentity() = default;

    // Detect the running game from the host process executable name.
    void detect_game();

    // Derive engine parameters (TDB version, bit widths, etc.) from the game ID.
    void derive_engine_params();

    GameID          m_game{GameID::Unknown};
    int             m_tdb_ver{0};
    int             m_type_index_bits{0};
    int             m_field_bits{0};
    bool            m_reengine_packed{false};
    bool            m_reengine_at{false};
    std::string_view m_game_name{"unknown"};
    std::string_view m_target_name{"UNKNOWN"};

    static inline GameIdentity s_instance{};
    static inline bool s_initialized{false};
};

} // namespace sdk
