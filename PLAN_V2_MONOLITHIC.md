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
- `GameIdentity::tdb_ver()` returns the modern TDB version for each game (hardcoded from the game roster).
- Legacy binaries that ship a different TDB version than the modern release (`RE2_TDB66`, `RE3_TDB67`, `RE7_TDB49`) are **out of scope** for the monolithic build — they need a dedicated `GameID` entry with a different hardcoded `tdb_ver`, or a runtime downgrade path that is not currently implemented.
- Runtime struct-layout dispatch is driven by the `TDB_DISPATCH` / `TDB_DISPATCH_PTR` macros in `RETypeDB.hpp`, which `switch` on `tdb_ver()` to select the correct per-version struct layout.
- All `#if TDB_VER >= X` guards in `src/` were converted to `if (GameIdentity::get().tdb_ver() >= X)`. Most guards in `shared/sdk/` were collapsed into stride-aware accessors and the `TDB_DISPATCH` chain; a small residual set of `#if TDB_VER` blocks remains for legacy per-game builds compiled without `REFRAMEWORK_UNIVERSAL`.

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
Old non-RT releases of RE2/RE3/RE7 shipped with earlier TDB versions (66/67/49). The monolithic build does **not** auto-detect the TDB version from the binary header or downgrade `GameIdentity::tdb_ver()` at runtime — that path was considered and rejected in favour of keeping legacy binaries out of scope. Users on those binaries should continue to use the pre-monolithic per-game DLLs.


## Current Status (v2 branch)

### Completed
- `GameIdentity` singleton: runtime game detection from exe name; `DllMain`-scoped initialisation (safe under loader lock).
- `cmake.toml`: single `REFrameworkSDK` + `REFramework` (dinput8.dll) targets.
- `TDBVer.hpp`: compile-time `TDB_VER=84` under `REFRAMEWORK_UNIVERSAL`.
- `ReClass.hpp`: RE8 layout as the canonical base; per-game divergences handled via `utility::re_type_accessor`, `classinfo_accessor`, and the REType predicate split (`retype_has_shifted_pointers` vs `retype_has_field_reorder`).
- `RETypeDB.hpp`: `TDB_DISPATCH` / `TDB_DISPATCH_PTR` macro chain dispatches struct layout to the correct tier at runtime. All production accessors (`get_type`, `get_method`, `get_field`, `get_property`, `get_module`, `get_num_types`, ...) are stride-aware.
- Plugin ABI (`src/mods/PluginLoader.cpp`): direct-field accesses that bypassed `TDB_DISPATCH` were eliminated in commit `99a5f49a` (eight lambdas in `g_tdb_data`).
- .NET assembly generation verified across all 14 supported games.

### Game Compatibility
| TDB | Games | Status |
|---|---|---|
| 67 | DMC5 | Works — `classinfo_accessor` handles REClassInfo layout divergence; `GenericListData` tier 67 bitfield in place |
| 69 | RE8 | Works |
| 70 | RE2, RE3, RE7 (modern only) | Works |
| 71 | RE4, MHRISE, SF6 | Works |
| 73 | DD2 | Works — D3D12 freeze resolved by `99a5f49a` (plugin ABI `numTypes`) and `dc980944` (`copy_texture`) |
| 78 | STARFORCE | Works — Mega Man Star Force Legacy Collection |
| 81 | MHWILDS | Works |
| 82 | MHSTORIES3 | Works — window uses DD2 layout (borderless_size @ 0xa0) |
| 83 | PRAGMATA, RE9 | Works |

### Out of scope
- Legacy non-RT binaries (`RE2_TDB66`, `RE3_TDB67`, `RE7_TDB49`).
- Full VR (motion controls) on RE2/RE3/RE7/RE8 — not tested in the monolithic build.
- Old (non-RT) DMC5 builds.