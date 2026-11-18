# In-game test suites

Tests that run inside a game with REFramework loaded. Not wired into CI: they need a Windows machine with the game
installed and a GPU session (the game has to render). Run them by hand, on a test VM, or in a loop over every
installed game.

| Suite | File | What it checks | Expected result on a healthy build |
|---|---|---|---|
| `api` | `lua/rftest_api.lua` | The documented Lua API (reframework-book `src/api`): `re`, `log`, `thread`, `json`, `fs`, `io`, `sdk` type database, managed objects, strings, arrays, `ValueType`, scene access, `sdk.hook`, math types, `imgui`, `draw` | all PASS |
| `regression` | `lua/rftest_regression.lua` | One test per Lua bug fixed in the 2026-11 bug hunt | all PASS (FAIL on builds without the fixes) |
| `plugin_api` | `plugin/rftest_plugin.cpp` | The C plugin API (`include/reframework/API.h`) and its C++ wrapper `API.hpp` (which the C# API uses): type database, invoke, managed objects, singletons, arrays, hooks, callbacks | all PASS |
| `plugin_regression` | `plugin/rftest_plugin.cpp` | Plugin API bugs fixed in the same bug hunt, including two callback deadlocks | all PASS (the game hangs on builds without the deadlock fix; see below) |
| `csharp` | `csharp/rftest_csharp.cs` | The C# API (REFramework.NET), untyped: type database, method and field lookup, static/instance invoke, managed strings, struct arguments and returns, `ValueType` and boxed value-type instance calls, `MethodHook` (args, return replacement, `Skip`, a struct return with a Double), the `UpdateBehavior` callback | all PASS. Not in the default `-Suites`; needs REFramework.NET (`-CSharpBin`). The first launch in a game generates its reference assemblies, which takes a few minutes |
| `config` | `run/Run-RFTests.ps1 -Scenario MalformedConfig` | A non-numeric value for a numeric config key must not stop REFramework from starting or discard later settings (kananlib `Config::get`) | PASS |

Everything is game-agnostic: only `System.*` and `via.*` types that exist in every RE Engine game are used, so the
same suites run over every supported game. Game-specific suites are a later step.

## Running

1. Build the plugin and the `package.loadlib` test DLL (needs Visual Studio with the C++ workload):

   ```bat
   tests\plugin\build.bat
   ```

2. Install REFramework into the game as usual, then:

   ```powershell
   powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -GameDir 'D:\SteamLibrary\steamapps\common\RESIDENT EVIL 2  BIOHAZARD RE2' -AppId 883710
   ```

   The runner installs the suites into `<game>\reframework`, starts the game (through Steam with `-AppId`, otherwise
   the game's exe), waits for every suite's results, closes the game, prints a summary, copies the results to
   `tests\out\<game>-<time>\` and removes everything it installed (the config file is restored). Exit code 0 means
   no failures.

   `-Suites api,regression,plugin` picks suites (add `csharp` for the C# suite, with `-CSharpBin <csharp-api\build\bin>`
   if the game doesn't have REFramework.NET installed: the runner then installs it and removes it afterwards);
   `-Scenario MalformedConfig` adds the config check; `-Action Install|Wait|Collect|Uninstall` splits the run for
   setups where the game is started some other way (e.g. over SSH into a VM's console session: `Install`, start the
   game, `Wait`, close the game, `Collect`, `Uninstall`).

3. Over several games:

   ```powershell
   $games = @{ 883710 = 'D:\Games\RE2'; 601150 = 'D:\Games\Devil May Cry 5'; 2246340 = 'D:\Games\MonsterHunterWilds' }
   foreach ($id in $games.Keys) { powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -GameDir $games[$id] -AppId $id }
   ```

The suites only need the game to reach its title screen; no input is required.

## Results

Each suite writes `reframework\data\rftest\results\<suite>.json`:

```json
{ "suite": "api", "game": "re2", "pass": 61, "fail": 0, "skip": 0,
  "tests": [ { "name": "find_type_definition", "status": "PASS", "detail": "" } ] }
```

and logs one `[rftest] <suite>: PASS|FAIL|SKIP <test> -- <detail>` line per test to `re2_framework_log.txt`. The
runner's `summary.txt` lists every failure.

## Builds without the fixes

The regression suites are meant to fail on such builds, and they do so safely where possible:

- Tests whose failure mode is memory corruption (out-of-range `Matrix4x4f`/`Quaternion` writes) detect the unfixed
  build and fail without performing the write.
- The rooted-path tests write `\rftest_escape\` at the root of the game's drive when the fix is missing; the runner
  deletes it.
- The plugin's two callback re-entrancy tests deadlock the game without the fix. They run last, after the Lua suites
  have written their results, and are recorded as FAIL before they start, so a hang still leaves complete results;
  the runner notices the hang and kills the game.
- The Lua regression test for hooking a struct-returning method with a float parameter (reframework#20) hangs the
  game without the fix. It is a `last_frame_test`: it runs after the plugin suite finished, with its result
  pre-recorded as FAIL.

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
(use it only for things a game may legitimately lack). A test that hangs or crashes the game on a build without its
fix goes in `S:last_frame_test(name, fn)` (same contract as `frame_test`). New Lua suites need their file name added
to `Install-Suites` and `Get-ExpectedSuites` in the runner.

Plugin: add a `record(...)` call to the matching stage in `rftest_plugin.cpp`; tests that need the game running go in
`api_runtime_tests` (game thread) or the `on_present` schedule.
