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


## Current Status (v2 branch)

### Completed
- GameIdentity singleton: runtime game detection from exe name
- cmake.toml: single `REFrameworkSDK` + `REFramework` (dinput8.dll) targets
- TDBVer.hpp: compile-time TDB_VER=84 under REFRAMEWORK_UNIVERSAL
- ReClass.hpp: includes RE8 layout as canonical base
- ~200 game-specific `#ifdef` guards in src/ converted to runtime GameIdentity checks
- Build compiles and links successfully (MSVC 19.44 / VS 2022)

### Game Compatibility
| TDB Version | Games | Status |
|---|---|---|
| 84 | PRAGMATA | Full (compiled struct matches) |
| 83 | RE9 | Full |
| 82 | MHSTORIES3 | Full |
| 81 | MHWILDS | Full |
| 73 | DD2 | Likely works (tdb84 struct is superset) |
| 71 | RE4, MHRISE, SF6 | Likely works |
| 70 | RE2, RE3, RE7 (modern) | Needs runtime struct dispatch |
| 69 | RE8 | Needs runtime struct dispatch |
| 67 | DMC5 | Needs runtime struct dispatch |
| 66 | RE2 (legacy) | Needs runtime struct dispatch |
| 49 | RE7 (legacy) | Needs runtime struct dispatch |

### Remaining: SDK TDB Struct Dispatch (BLOCKER for old games)
209 compile-time `#if TDB_VER` guards remain in `shared/sdk/`.

The core problem: `RETypeDB.hpp` lines 1712-1847 select ONE struct layout at compile time.
With TDB_VER=84, the code compiles against `tdb84::TDB`, `tdb84::REMethodDefinition`, etc.
These POD structs have different field counts and offsets per TDB version:
- tdb84::TDB: `modules` at ~offset 0x88 (after 20+ uint32 fields)
- tdb49::TDB: `modules` at offset 0x38 (after 10 uint32 fields)

When the monolithic DLL runs inside DMC5 (TDB67), the actual TDB in memory has the
tdb67 layout, but compiled code interprets it as tdb84. Member offsets don't match.

**Options for runtime struct dispatch:**
1. Virtual accessor wrapper: `sdk::RETypeDB` becomes a polymorphic class with
   version-specific subclasses that override accessors
2. Offset table: A per-TDB-version offset table drives field access at runtime
3. Reinterpret + switch: Cast `this` to the correct versioned struct in each accessor
4. Template-based dispatch: Game identity → template param → correct struct type at each callsite

This requires an architectural decision from the maintainer.
The `shared/sdk/*.cpp` accessor functions (RETypeDB.cpp: 30 guards, RETypeDefinition.cpp: 25 guards)
can be converted to runtime `if` chains AFTER the struct selection is solved.