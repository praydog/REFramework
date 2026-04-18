# REFramework v2 Universal Build — Working Context

## Repository
- **Path**: `I:\Programming\projects\REFramework-v2`
- **Branch**: `v2`
- **Reference (master) project**: `I:\Programming\projects\re2-barebones`
- **PR**: praydog/REFramework#1609

## Build Commands
```bash
# Native DLL (dinput8.dll)
cmake --build build --config RelWithDebInfo -j 16 --target REFramework

# C# API (AssemblyGenerator + dependencies)
cmake --build csharp-api/build --config Release --target AssemblyGenerator

# Output: build/bin/REFramework/dinput8.dll
```

**PDB lock**: Must kill the game process before rebuilding — MSVC can't write the PDB while the DLL is loaded.

## Deployment

### Game Installations (19 games)

| Game | TDB | GameID | Exe stem | Install path |
|---|---|---|---|---|
| RE2 | 70 | RE2 | `re2` | `G:\SteamLibrary\steamapps\common\RESIDENT EVIL 2  BIOHAZARD RE2` |
| RE3 | 70 | RE3 | `re3` | `I:\SteamLibrary\steamapps\common\RE3` |
| RE4 | 71 | RE4 | `re4` | `H:\SteamLibrary\steamapps\common\RESIDENT EVIL 4  BIOHAZARD RE4` |
| RE7 | 70 | RE7 | `re7` | `I:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard` |
| RE8 | 69 | RE8 | `re8` | `G:\SteamLibrary\steamapps\common\Resident Evil Village BIOHAZARD VILLAGE` |
| RE9 | 83 | RE9 | `re9` | `I:\SteamLibrary\steamapps\common\RESIDENT EVIL requiem BIOHAZARD requiem` |
| DMC5 | 67 | DMC5 | `devilmaycry5` | `G:\SteamLibrary\steamapps\common\Devil May Cry 5` |
| MHRise | 71 | MHRISE | `monsterhunterrise` | `G:\SteamLibrary\steamapps\common\MonsterHunterRise` |
| MHWilds | 81 | MHWILDS | `monsterhunterwilds` | `I:\SteamLibrary\steamapps\common\MonsterHunterWilds` |
| MHStories3 | 82 | MHSTORIES3 | `monster_hunter_stories_3_twisted_reflection` | `J:\SteamLibrary\steamapps\common\MONSTER_HUNTER_STORIES_3_TWISTED_REFLECTION` |
| SF6 | 71 | SF6 | `streetfighter6` | `I:\SteamLibrary\steamapps\common\Street Fighter 6` |
| DD2 | 73 | DD2 | `dd2` | `J:\SteamLibrary\steamapps\common\Dragons Dogma 2` |
| Starforce | 78 | STARFORCE | `megaman_starforce_legacy_collection` | `J:\SteamLibrary\steamapps\common\MegaMan StarForce LegacyCollection` |
| Pragmata | 83 | PRAGMATA | `pragmata_sketchbook` | `W:\SteamLibrary\steamapps\common\PRAGMATA SKETCHBOOK` |
| Pragmata (full) | 83 | PRAGMATA | `pragmata` | `I:\SteamLibrary\steamapps\common\PRAGMATA` |
| Dead Rising DR | 73 | DRDR | `drdr` | `J:\SteamLibrary\steamapps\common\DEAD RISING DELUXE REMASTER` |
| Ghosts n Goblins | 69 | GGR | `makaimura_gg_re` | `G:\SteamLibrary\steamapps\common\Makaimura_GG_RE` |
| Apollo Justice | 73 | GS456 | `gs456` | `J:\SteamLibrary\steamapps\common\Apollo Justice Ace Attorney Trilogy` |
| Kunitsu-Gami | 73 | KUNITSU | `kunitsugami` | `I:\SteamLibrary\steamapps\common\Kunitsu-Gami Path of the Goddess` |
| Onimusha 2 | 74 | ONIMUSHA2 | `onimusha2` | `H:\SteamLibrary\steamapps\common\Onimusha2` |

**Not valid for testing**: `I:\SteamLibrary\steamapps\common\RESIDENT EVIL requiem BIOHAZARD requiem ROW` (ROW variant, not a real game), `I:\SteamLibrary\steamapps\common\MONSTER_HUNTER_STORIES_3_TWISTED_REFLECTION_TRIAL` (trial version, use full game instead).

### Deploy DLL to all games
```bash
for dir in \
    "G:\SteamLibrary\steamapps\common\RESIDENT EVIL 2  BIOHAZARD RE2" \
    "I:\SteamLibrary\steamapps\common\RE3" \
    "H:\SteamLibrary\steamapps\common\RESIDENT EVIL 4  BIOHAZARD RE4" \
    "I:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard" \
    "G:\SteamLibrary\steamapps\common\Resident Evil Village BIOHAZARD VILLAGE" \
    "I:\SteamLibrary\steamapps\common\RESIDENT EVIL requiem BIOHAZARD requiem" \
    "G:\SteamLibrary\steamapps\common\MonsterHunterRise" \
    "I:\SteamLibrary\steamapps\common\MonsterHunterWilds" \
    "I:\SteamLibrary\steamapps\common\Street Fighter 6" \
    "G:\SteamLibrary\steamapps\common\Devil May Cry 5" \
    "J:\SteamLibrary\steamapps\common\Dragons Dogma 2" \
    "J:\SteamLibrary\steamapps\common\MONSTER_HUNTER_STORIES_3_TWISTED_REFLECTION" \
    "W:\SteamLibrary\steamapps\common\PRAGMATA SKETCHBOOK" \
    "I:\SteamLibrary\steamapps\common\PRAGMATA" \
    "J:\SteamLibrary\steamapps\common\MegaMan StarForce LegacyCollection" \
    "J:\SteamLibrary\steamapps\common\DEAD RISING DELUXE REMASTER" \
    "G:\SteamLibrary\steamapps\common\Makaimura_GG_RE" \
    "J:\SteamLibrary\steamapps\common\Apollo Justice Ace Attorney Trilogy" \
    "I:\SteamLibrary\steamapps\common\Kunitsu-Gami Path of the Goddess" \
    "H:\SteamLibrary\steamapps\common\Onimusha2"; do
    [ -d "$dir" ] && cp build/bin/REFramework/dinput8.dll "$dir/dinput8.dll"
done
```

### Deploy C# API to a game
```bash
cd csharp-api
python make_symlinks.py --gamedir="<GAME_DIR>" --just_copy=True --skip_test_scripts=True
```

### Clear generated assembly cache (forces regeneration)
```bash
rm -rf "<GAME_DIR>/reframework/plugins/managed/generated"
```

## Architecture — What We Built

### The core problem
One monolithic `dinput8.dll` covering 19 RE Engine games. Each game has a different TDB version (67-83) with different struct layouts for the Type Database types. The DLL compiles with `REFRAMEWORK_UNIVERSAL` and `TDB_VER=84`, meaning every struct is compiled at the tdb84 layout. Runtime dispatch is required when the game's real layout differs.

### Enforcement layers (bottom to top)

1. **Private fields / opaque structs** — The compiler rejects `obj->field` on layout-variant types.
2. **Dispatch macros** — `TDEF_FIELD`, `TMETH_FIELD`, `TFIELD_FIELD`, `RMOD_FIELD`, `RPROP_FIELD`, `TDB_DISPATCH`, etc. Cast `this` to the correct per-version struct at runtime.
3. **Stride-aware accessors** — `get_type(i)`, `get_method(i)`, `get_field_impl_at(i)`, etc. Use byte arithmetic with runtime stride instead of `sizeof(empty_struct)`.
4. **`runtime_size()`** — `REManagedObject::runtime_size()`, `REType::runtime_size()`. Replace `sizeof()` for pointer arithmetic.
5. **`const void*` raw array pointers** — `get_types_ptr()` etc. return `const void*` under universal, preventing accidental `[index]`.
6. **Audit scripts** — `scripts/audit_direct_access.py` (regex, CI-ready), `scripts/audit_direct_access_clang.py` (libclang, type-aware, catches sizeof/subscript too).

### Type enforcement status

| Type | Mechanism | Direct `->field` compiles? |
|---|---|---|
| RETypeDefinition | Opaque (no inheritance) | No — `error C2039` |
| RETypeDB | Opaque (no inheritance) | No |
| REMethodDefinition | Opaque (no inheritance) | No |
| REField | Opaque (no inheritance) | No |
| REProperty | Opaque (no inheritance) | No |
| REModule | Opaque (no inheritance) | No |
| REType | Private fields + dispatching members | No — `error C2248` |
| REGameObject | Private fields + dispatching members | No |
| REComponent | Private fields + forwarding members | No |
| REManagedObject | Private fields + get/set/ptr | No |
| REFieldList | Private fields + forwarding members | No |
| FunctionDescriptor | Private fields + forwarding members | No |
| VariableDescriptor | Private fields + forwarding members | No |
| RETransform | `get_joints()` dispatches (joints offset varies) | Partially — only `joints` is dispatched |

### Dispatch macros reference

| Macro | Struct | When |
|---|---|---|
| `TDB_DISPATCH(field)` | RETypeDB | All TDB versions, scalar fields |
| `TDB_DISPATCH_PTR(field)` | RETypeDB | All TDB versions, pointer fields |
| `TDB_DISPATCH_69(field, fallback)` | RETypeDB | TDB >= 69 fields |
| `TDB_DISPATCH_PTR_69(field)` | RETypeDB | TDB >= 69 pointer fields |
| `TDEF_FIELD(ptr, field)` | RETypeDefinition | All versions |
| `TDEF_FIELD_69(ptr, field)` | RETypeDefinition | TDB >= 69 (impl_index, etc.) |
| `TDEF_FIELD_PRE_IMPL(ptr, field)` | RETypeDefinition | TDB < 69 (namespace_offset, etc.) |
| `TDEF_FIELD_SET(ptr, field, val)` | RETypeDefinition | Write dispatch |
| `TMETH_FIELD(ptr, field)` | REMethodDefinition | tdb69 vs tdb71+ |
| `TMETH_FIELD_71(ptr, field)` | REMethodDefinition | TDB >= 71 only (encoded_offset, params) |
| `TFIELD_FIELD(ptr, field)` | REField | tdb69 vs tdb71+ |
| `TFIELD_FIELD_71(ptr, field)` | REField | TDB >= 71 only (field_typeid, init_data_hi) |
| `TPARAM_FIELD(ptr, field)` | REParameterDef | tdb69 vs tdb71+ |
| `RMOD_FIELD(ptr, field)` | REModule | All versions |
| `RPROP_FIELD(ptr, field)` | REProperty | tdb67 vs tdb69+ |
| `RPROP_FIELD_69(ptr, field)` | REProperty | TDB >= 69 only (impl_id) |
| `TIMPL_FIELD(ref, field)` | RETypeImpl | tdb82 vs tdb83+ |
| `TIMPL_DISPATCH(T, ref, field)` | RETypeImpl | Full per-version dispatch |

### File organization

Type headers extracted from ReClass to `shared/sdk/types/`:
```
types/REObject.hpp          — REObject base class
types/REType.hpp            — REType with dispatching members
types/REManagedObject.hpp   — REManagedObject with ref counting
types/REComponent.hpp       — REComponent + RECamera
types/REGameObject.hpp      — REGameObject with runtime dispatch
types/REReflection.hpp      — REFieldList, FunctionHolder, FunctionDescriptor, VariableDescriptor
```

`ReClass_Internal_RE8.hpp` includes these at the top and contains only non-guarded types.

### Accessor implementations

| File | What it implements |
|---|---|
| `REGameObject.cpp` | `get_transform()`, `get_folder()`, `get_shouldDraw/Update()`, `offset_of_*()` |
| `RETransform.cpp` | `get_joints()` |
| `REType.cpp` | `runtime_size()` |
| `REManagedObject.cpp` | `runtime_size()`, `classinfo_accessor` |
| `RETypeLayouts.hpp` | `REType::get_size()`, `get_super()`, `get_fields()`, `get_classInfo()`, etc. |
| `RETypeDefDispatch.hpp` | All `TDEF_FIELD`, `TMETH_FIELD`, `TFIELD_FIELD`, `RMOD_FIELD`, `RPROP_FIELD` macros |
| `ViaDispatch.hpp` | `window_width/height()`, `window_borderless_w/h()`, `sv_window()`, `sv_size_w/h()`, `sv_display_type()` |

## Known Issues / Deferred Work

### Impl type inheritance (contributor footgun)
REMethodImpl, REFieldImpl, RETypeImpl, REPropertyImpl, REParameterDef still inherit from versioned structs. Current code already dispatches explicitly with per-version casts. Risk is a future contributor writing `impl.field` without the cast. Should be decoupled same as the main types but layouts are stable across tdb71-84.

### ViaDispatch missing verified layouts
STARFORCE, PRAGMATA, GGR, GS456, KUNITSU, ONIMUSHA2 don't have verified regenny headers. They fall through to nearest-neighbor defaults (RE9 or DD2). If their `via::Window`/`via::SceneView` layouts differ, ultrawide/borderless dispatch reads wrong offsets.

### GGR .NET generation errors
Ghosts 'n Goblins Resurrection (TDB 69) has .NET assembly generation errors. Core framework (menu, hooks, REGlobals) works. Needs investigation.

### CI enforcement
Audit scripts exist but aren't wired into GitHub workflow. `build-pr.yml` should run `python scripts/audit_direct_access.py --varies-only` as a check step.

### utility::re_managed_object refactor
Free functions in `utility::re_managed_object` namespace should be migrated to member methods on `REManagedObject` struct. Historical limitation from ReClass workflow — now that type headers are separate, the refactor is possible. TODO comment added to `REManagedObject.hpp`.

### Non-universal dead code
Hundreds of lines in `#else`/`#elif TDB_VER` blocks. Dead in universal build but adds visual noise and can mislead contributors.

### Legacy TDB versions
Old (non-RT) binaries of RE2 (TDB66), RE3 (TDB67), RE7 (TDB49) are out of scope for the monolithic build. Users on those binaries should use pre-monolithic per-game DLLs.

## Commit History (this session)

```
67cc4112 Add Ghosts n Goblins, Apollo Justice, Kunitsu-Gami, Onimusha 2
bda88757 Add Dead Rising Deluxe Remaster (DRDR) game identity
2e672563 Replace sizeof(REManagedObject) and sizeof(REType) with runtime_size()
10fea153 Add sizeof and array-subscript checks to libclang audit
0b768380 Proactive hardening — RETransform joints, C++/CLI refcount, private TDB array ptrs
7aec7583 Add stride-aware Impl/Param accessors, eliminate all direct TDB array indexing
246e86df Fix array stride breakage from REField/REProperty/REMethodDefinition decoupling
62c4aaec Decouple RETypeDB, REMethodDefinition, REField, REModule from inheritance
1d1817b1 Decouple RETypeDefinition from compiled struct inheritance
c69c4662 Extract guarded structs from ReClass header into proper type headers
cb40d8a6 Privatize REType fields — all 6 guarded structs now compiler-enforced
1b25065c Make guarded struct fields private — compiler-enforced accessor use
4ba444ab Move hardcoded struct offsets to reusable static methods
f3dbbe68 Complete accessor migration — zero violations across entire tree
9880c811 Add accessors for all remaining guarded structs, zero audit violations
5ea90cb9 Fix remaining direct field access violations + annotate stable sites
df7400cf Add libclang-based type-aware direct access audit
c64cfecd Add accessors for REComponent/REGameObject, eliminate 57 direct field accesses
405db5da Add direct struct field access audit script
bdb21aa6 Fix REGameObject::transform offset on SF6/RE4 (0x20 vs 0x18)
6054deb9 Fix R3 adversarial review findings (6 of 14)
288d3b51 Fix IL2CPP dump crashes on DMC5 and other TDB<69 games
0f9a5095 Address adversarial review R2 findings (R2-1, R2-2, R2-4, R2-6)
73da6828 Collapse workflows to single monolithic REFramework target
99a5f49a Fix plugin ABI TDB count fields bypassing dispatch
```

## Testing Checklist (empirical)

When making changes, test at minimum:
- **DMC5** (TDB 67) — lowest TDB, most divergent layout, ObjectExplorer SDK dump
- **RE2** (TDB 70) — mid-range TDB, good general regression test
- **SF6** (TDB 71) — REGameObject::transform at 0x20 (unique), ultrawide UI
- **RE4** (TDB 71) — REGameObject::transform at 0x20, scope ADS
- **MHWilds** (TDB 81) — REType size/typeCRC swap, D3D12 resource path
- **RE9 or Pragmata** (TDB 83) — highest TDB with shifted REType pointers
