# In-game test suites

Tests that run inside a game with REFramework loaded. Not wired into CI: they need a Windows machine with the game
installed and a GPU session (the game has to render). Run them by hand, on a test VM, or in a loop over every
installed game.

| Suite | File | What it checks | Expected result on a healthy build |
|---|---|---|---|
| `api` | `lua/rftest_api.lua` | The documented Lua API (reframework-book `src/api`): `re`, `log`, `thread`, `json`, `fs`, `io`, `sdk` type database, managed objects, strings, arrays, `ValueType`, scene access, `sdk.hook`, math types, `imgui`, `draw` | all PASS |
| `regression` | `lua/rftest_regression.lua` | Tests for the Lua bugs fixed in the 2026-11 bug hunt; some bugs have several, and a few ImGui fixes have none (`set_tooltip`/`tree_node_*` format strings, `end_rect`, `add_ellipse`, `path_elliptical_arc_to`, the `world_text` default color) | all PASS. On builds without the fixes all FAIL except two controls |
| `plugin_api` | `plugin/rftest_plugin.cpp` | The C plugin API (`include/reframework/API.h`) and its C++ wrapper `API.hpp` (which the C# API uses): type database, invoke, managed objects, singletons, arrays, hooks, callbacks | all PASS |
| `plugin_regression` | `plugin/rftest_plugin.cpp` | Plugin API bugs fixed in the same bug hunt, including two callback deadlocks; a Lua state the plugin creates with `create_script_state` keeps objects its script `add_ref()`s and releases them when deleted | all PASS (the game hangs on builds without the deadlock fix; see below) |
| `csharp` | `csharp/rftest_csharp.cs` | The C# API (REFramework.NET), untyped: type database, method and field lookup, static/instance invoke, managed strings, struct arguments and returns, `ValueType` and boxed value-type instance calls, `MethodHook` (args, return replacement, `Skip`, a struct return with a Double), the `UpdateBehavior` callback | all PASS. Not in the default `-Suites`; needs REFramework.NET (`-CSharpBin`). The first launch in a game generates its reference assemblies, which takes a few minutes |
| `config` | `run/Run-RFTests.ps1 -Scenario MalformedConfig` | A non-numeric value for a numeric config key must not stop REFramework from starting or discard later settings (kananlib `Config::get`) | PASS |

Everything is game-agnostic: only `System.*` and `via.*` types that RE Engine games share are used, so the same suites
run over every supported game. Where a game lacks a method a test needs, that test SKIPs and names it (PRAGMATA has
no `String.PadLeft(Int32, Char)`, Mega Man Star Force no `via.vec3.set_Item`); games from TDB 81 on skip the plugin's
instantiated-methods test, as the SDK has no instantiation data there. Game-specific suites are a later step.

## Running

1. Build the plugin and the `package.loadlib` test DLL (needs Visual Studio with the C++ workload):

   ```bat
   tests\plugin\build.bat
   ```

2. Optionally copy `tests\run\local.example.json` to `tests\run\local.json` (gitignored) and fill in the paths that
   apply to this machine: the REFramework build to test, the C# API build output, procdump, a results folder. Every
   key is optional; command-line parameters override them.

3. One game:

   ```powershell
   powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -Game re2 -Build C:\path\to\dinput8.dll
   ```

   - `-Game` takes an alias or Steam app id from [`run/games.json`](run/games.json). The install folder comes from
     Steam's library manifests. `-GameDir <folder>` (plus `-AppId` to launch through Steam) works for anything else.
   - `-Build` installs that `dinput8.dll` for the run and puts the game's own back afterwards. Without it (and without
     `build` in `local.json`) the REFramework already installed in the game is tested.
   - The runner installs the suites into `<game>\reframework`, starts the game, waits for every suite's results,
     closes the game, prints a summary and copies the results to `tests\out\<game>-<time>\` (`summary.txt`,
     `summary.json`, the suites' JSON files, a log excerpt). Then it puts the game back as it was: everything new
     under `reframework\` is removed, and the config, logs and `ref_ui.ini` are restored. Exit code 0 means no
     failures.
   - `-Suites api,regression,plugin` picks suites. Add `csharp` for the C# suite; `-CSharpBin <csharp-api\build\bin>`
     installs REFramework.NET for the run if the game doesn't have it.
   - `-Scenario MalformedConfig` adds the config check.
   - `-Action Install|Wait|Collect|Uninstall` splits the run for setups where the game is started some other way.

4. Every installed game, one after another:

   ```powershell
   powershell -ExecutionPolicy Bypass -File tests\run\Run-AllGames.ps1 -Build C:\path\to\dinput8.dll -Suites api,regression,plugin,csharp
   ```

   `-Games re2,dmc5` limits the run. The combined results go to `summary.md` (one row per game, pass/fail/skip per
   suite) and `summary.json`; exit code 0 means every suite passed in every game.

The suites only need the game to reach its title screen; no input is required.

### Over SSH

All Windows tools work from an SSH session, e.g. on a test VM:

```sh
ssh testbox powershell -ExecutionPolicy Bypass -File C:\src\REFramework\tests\run\Run-AllGames.ps1
```

SSH sessions can't show windows, so the tools start and close games on the desktop of the logged-in user through a
one-off scheduled task. Requirements:

- the same user is logged in on the machine's desktop (auto-logon works);
- Steam is running in that session.

### Linux (Proton, Steam Deck)

[`run/run-rftests.sh`](run/run-rftests.sh) is the same runner for Proton. It needs bash, python3, a running Steam
client, and the plugin DLLs built on Windows (copy `tests\plugin\out` along with `tests`):

```sh
bash tests/run/run-rftests.sh -g dd2 -b ~/builds/dinput8.dll
```

- It installs the build and suites, adds the `dinput8` DLL override to the game's Proton prefix if missing, launches
  through Steam, and puts everything back afterwards.
- `-s api,regression,plugin,csharp` with `-c <csharp bin>` runs the C# suite. A Proton prefix has no .NET, so it also
  needs `-n <dir>`: a portable .NET 10 runtime (the contents of `C:\Program Files\dotnet` from a Windows machine with
  the .NET 10 runtime), which is copied into the prefix for the run.
- Over SSH, start it with `systemd-run --user --collect --same-dir` so a dropped connection doesn't stop it half way
  (`--same-dir` keeps the working directory; the unit starts in `~` otherwise).

## Results

Each suite writes `reframework\data\rftest\results\<suite>.json`:

```json
{ "suite": "api", "game": "re2", "pass": 61, "fail": 0, "skip": 0,
  "tests": [ { "name": "find_type_definition", "status": "PASS", "detail": "" } ] }
```

and logs one `[rftest] <suite>: PASS|FAIL|SKIP <test> -- <detail>` line per test to `re2_framework_log.txt`. The
runner's `summary.txt` lists every failure and every skip with its detail.

## Builds without the fixes

The regression suites are meant to fail on such builds, and they do so safely where possible:

- Tests whose failure mode is memory corruption (out-of-range `Matrix4x4f`/`Quaternion` writes) detect the unfixed
  build and fail without performing the write.
- The rooted-path tests write `\rftest_escape\` at the root of the game's drive when the fix is missing; the Windows
  runner deletes it (the Linux runner doesn't yet).
- The plugin's two callback re-entrancy tests deadlock the game without the fix. They run last, after the Lua suites
  have written their results, and are recorded as FAIL before they start, so a hang still leaves complete results;
  the runner notices the hang and kills the game.
- The Lua regression test for hooking a struct-returning method with a float parameter hangs the
  game without the fix. It is a `last_frame_test`: it runs after the plugin suite finished, with its result
  pre-recorded as FAIL.

## Other tools

| Tool | What it's for |
|---|---|
| `run/Compare-ScriptErrors.ps1 -Game <g> -Scripts <dir> -BuildA <dll> -BuildB <dll>` | Lua API compatibility. Runs a folder of real scripts (mods) with two builds and compares the script errors (log lines and the "ScriptRunner Message" boxes, which it dismisses). Exit code 0 means the same distinct messages with both builds. |
| `run/Find-StartupHang.ps1 -Game <g> -Runs <n> -DumpDir <dir>` | Intermittent startup hangs. Starts the game until the REFramework log stops growing before Lua initializes, then takes a full dump with procdump. |
| `run/tools/Watch-GameWindow.ps1 -Game <g> -Log <file>` | Logs the game window's size, monitor, style, DPI and focus over time, for games that fight over their window. |
| `run/tools/Watch-MessageBoxes.ps1` | Dismisses and logs "ScriptRunner Message" boxes (used by Compare-ScriptErrors.ps1). |
| `run/Set-SteamOffline.ps1 -Mode Offline\|Online` | Keeps a dedicated test machine from syncing Steam Cloud saves (see below). |

## Games stuck resizing

A game that never finishes starting, with thousands of `Reset!` lines in `re2_framework_log.txt` (`deviceResets` in
`summary.json`), is stuck in a swap chain resize loop. Two causes seen so far, neither in REFramework:

| Cause | Seen with | What helps |
|---|---|---|
| Another window in front when the game starts (e.g. a console window) | Ghosts 'n Goblins Resurrection | The tools start their helpers without windows. Keep the test machine's desktop clear. |
| Cloud-synced options from another PC with a resolution the display can't do (e.g. 3840x2160 on 1080p) | Resident Evil 7, Ghosts 'n Goblins Resurrection | `Set-SteamOffline.ps1` (below) |

`tools/Watch-GameWindow.ps1` shows which one it is: a game that stays windowed and never gets the foreground is the
first.

### Steam Cloud on a test machine

On a dedicated test machine, `Set-SteamOffline.ps1 -Mode Offline -ResetSaves re7,gngr` keeps cloud saves out of the
way:

- it backs up the games' Steam userdata folders, removes the local cloud saves of the listed games, and puts Steam
  in offline mode, so nothing syncs either way;
- `-Mode Online` restores the backup before going online (for game updates), so nothing a test run wrote is uploaded.

Steam's own cloud switches can't do this: they are stored in the account's roaming settings and apply to every PC.

## Writing tests

Lua (`lua/rftest/harness.lua`):

```lua
local H = require("rftest.harness")
local S = H.suite("my_suite")                       -- results file name
S:test("name", function() H.eq(1 + 1, 2, "math") end)
S:frame_test("imgui thing", function(frame)          -- runs every frame until it returns true
    return H.in_window("title", function() ... return true end)
end)
S:finish()                                           -- writes results when all frame tests are done
```

Assertions: `H.eq`, `H.ne`, `H.near`, `H.truthy`, `H.raises`, `H.no_raise`; `H.skip(reason)` marks a test skipped
(use it only for something the game lacks, never for behavior that didn't happen; see [TESTING.md](TESTING.md)). A
test that hangs or crashes the game on a build without its
fix goes in `S:last_frame_test(name, fn)` (same contract as `frame_test`). New Lua suites need their file name added
to `Install-Suites` and `Get-ExpectedSuites` in the runner.

Plugin: add a `record(...)` call to the matching stage in `rftest_plugin.cpp`; tests that need the game running go in
`api_runtime_tests` (game thread) or the `on_present` schedule; tests over several frames on the game thread step from
`on_pre_update` (like `multistate_step`). The plugin links REFramework's Lua (`dependencies/lua`, compiled in by
`build.bat`) to run code in the Lua states it creates; call into them only between `lock_lua` and `unlock_lua`.
