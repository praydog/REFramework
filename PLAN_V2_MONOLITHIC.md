# REFramework v2 - Monolithic DLL Plan

## Goal
Convert the project from N per-game DLLs (each built with game-specific `#ifdef` guards) into one universal `dinput8.dll` that supports all RE Engine games with runtime detection and dispatch.

## Non-Goals
- Changing the csharp-api (kept separate).
- Changing the plugin ABI.
- Removing any game support.

## Architecture Decisions

### 1. Game Detection (`shared/sdk/GameIdentity.hpp/cpp`)
- `GameID` enum: RE2, RE3, RE4, RE7, RE8, RE9, DMC5, MHRISE, SF6, DD2, MHWILDS, MHSTORIES3, PRAGMATA
- Detected at `DLL_PROCESS_ATTACH` from the host executable name
- Singleton pattern: `sdk::GameIdentity::get()`
- Provides: `game()`, `tdb_ver()`, `type_index_bits()`, `field_bits()`, `is_reengine_packed()`, `is_reengine_at()`, `game_name()`, `target_name()`

### 2. TDB Version Handling
- Default TDB version derived from game identity (modern version for each game).
- Auto-detection from the TDB binary header at runtime to support legacy game versions (RE2_TDB66, RE3_TDB67, RE7_TDB49).
- `GameIdentity::tdb_ver()` is the authoritative source; may be updated after TDB header is read.
- All `#if TDB_VER >= X` guards become `if (GameIdentity::get().tdb_ver() >= X)`.

### 3. Struct Layout Strategy (ReClass, Regenny)
- All per-game struct definitions are kept and compiled simultaneously.
- Each `ReClass_Internal_*.hpp` is wrapped in a unique namespace (`reclass::re7_tdb49`, `reclass::re2_tdb70`, `reclass::re8`, `reclass::sf6`, `reclass::mhwilds`, etc.).
- A unified accessor layer dispatches to the correct namespace at runtime.
- The versioned `RETypeDefVersion*` structs in `RETypeDefinition.hpp` already have distinct names; they just need runtime selection instead of compile-time typedef.

### 4. Compile-Time Macros → Runtime
| Old Macro | New Runtime Equivalent |
|---|---|
| `RE2`, `RE3`, ... | `GameIdentity::get().game() == GameID::RE2` |
| `TDB_VER` | `GameIdentity::get().tdb_ver()` |
| `REENGINE_PACKED` | `GameIdentity::get().is_reengine_packed()` |
| `REENGINE_AT` | `GameIdentity::get().is_reengine_at()` |
| `TYPE_INDEX_BITS` | `GameIdentity::get().type_index_bits()` |
| `FIELD_BITS` | `GameIdentity::get().field_bits()` |
| `REFRAMEWORK_GAME_NAME` | `GameIdentity::get().target_name()` |

### 5. Build System
- Single SDK static library: `REFrameworkSDK` (all `shared/sdk/` sources)
- Single game DLL: `REFramework` (all `src/` sources, output as `dinput8.dll`)
- No game-specific compile definitions (no `RE2`, `RE3`, etc.)
- Keep `REFRAMEWORK_EXPORTS`, `DEVELOPER` defines

### 6. Mod Registration
- All mods are registered unconditionally in `Mods.cpp`.
- Each game-specific mod checks `GameIdentity::get()` in its `on_initialize()` and returns early/no-op if not applicable.

### 7. Performance Consideration
- `GameIdentity::get()` returns a `const&` to a singleton; it's a single pointer deref.
- For hot paths, callers can cache `const auto tdb_ver = GameIdentity::get().tdb_ver()` locally.
- Struct layout dispatch at accessor sites is a switch/if-chain that the compiler can optimize.

## Phases
1. Foundation: GameIdentity infrastructure
2. Build system: Single target cmake
3. ReClass namespace isolation
4. SDK ifdef conversion
5. src/ ifdef conversion
6. Verification and cleanup

## Risk Areas
- **ReClass structs**: Different sizes per game; any code that `sizeof()` or `offsetof()` these needs careful handling.
- **TDB struct accessor functions**: Heavy #if nesting; mechanical but tedious conversion.
- **IntegrityCheckBypass**: Per-game byte patterns that are completely different; these just become runtime switch cases.
- **VR module**: Heaviest ifdef user; partially already uses runtime detection.
- **Performance**: Runtime dispatch in hot loops (unlikely to matter vs cache misses from game engine itself).

## Legacy TDB Versions
Old games had different TDB versions. The monolithic DLL handles this by:
1. Starting with the modern TDB version for each game
2. Auto-detecting the actual TDB version from the binary header when the type database is first loaded
3. Updating `GameIdentity` TDB version accordingly
This means RE7 starts assuming TDB70 but if the actual binary has TDB49, it switches.
