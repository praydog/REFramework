# Testing REFramework

How to find real bugs in REFramework, **demonstrate them with a failing in-game test**, and fix them. This is the
companion to the suites in `tests/` and the tools in `tests/run/` (how to run them: [README.md](README.md)).

## Core principle

**Demonstrate before you fix.** Never claim a binding, hook or API is wrong without a test that *fails in a running
game against the current code*. An error line in the log, a suspicious-looking cast or "this looks like it reads past
the buffer" is a hypothesis, not evidence. The evidence is a red result from `Run-RFTests.ps1` against a build without
the fix.

**The same bar applies to a POSITIVE claim.** "This no longer allocates", "the hook sees the float", "the callback can't
race the reset", "the box is only skipped for value-type methods" are each a hypothesis until a test or a measurement
exercises that exact path and shows the outcome. Reading the code and concluding it *would* behave is not evidence:
the engine differs per game and per TDB version, and the case the author did not think of is exactly the one that
breaks. Prove it in a game, and in both a TDB < 74 and a TDB >= 74 game when the code branches on the version.

**When a test goes red, fix the CODE. Never weaken the test.** A failing assertion is the test doing its job. The
following are PROHIBITED as a way to get to green, because each one deletes the coverage that caught the bug:
- loosening an assertion (exact value to "either of two values", a specific error to "any error", `==` to a range);
- accepting several outcomes because you don't know which one is right (find out, then assert that one);
- narrowing the inputs, or switching to a method or signature that no longer triggers the failure;
- adding a SKIP path for the case that fails, or turning "the behavior didn't happen" into a SKIP;
- pre-recording a PASS, or dropping the suite from `Get-ExpectedSuites` / `EXPECTED`;
- special-casing the code so the asserted value matches broken output;
- deleting or commenting out the test.

The ONE exception is a test that is *itself* wrong: a wrong expected value, an assertion that encodes a
misunderstanding of the engine, a test that depends on game state it doesn't control. The bar is high. You must state
precisely *why* the test is wrong, independent of the change you want to land, and show that the corrected test still
fails the old code for the right reason. If you can't, the test is right and the code is wrong. "The test is buggy" is
the most dangerous sentence in this file; treat your urge to believe it as a red flag, not a finding.

This is not hypothetical here either. The first version of the struct-return hook fix decided "takes a return buffer"
with `invoke()`'s rule, which is also true for `System.Void`. A new API hook test (a void method with floats on the
stack) went red. The fix was in the hook code, not the test, and that test now pins the void case. And the reverse:
the rooted-path regression tests passed on `master` wherever the drive root wasn't writable, because "any error"
satisfied them. They were made *stricter*: each now requires the specific rejection (the sandbox's message, or for
`json.dump_file` a refused write to a rooted path that IS writable), never looser.

A later audit of this suite against these rules found more of the same kind, all fixed by making the tests stricter:
hook tests that SKIPped when their own call wasn't seen (now FAIL), a Guid test that checked only the string's
length (it passed on `master` while `master` printed garbage; it now checks the exact string), an assertion that
accepted a float return as "a float or a double" (it now asserts the double invoke() really returns), and results
that a second record with the same name could overwrite (only a pending entry can be replaced now).

Running the stricter suite then showed both sides of the exception bar:
- **A code bug.** A new test calling `ToString()` on a `ValueType` enum threw on every build, in TDB 70 and TDB 83
  games: methods a reference type declares (`System.Enum`, `System.Object`) got a fake object header as `this`. The
  code now boxes the value for real for those calls, in Lua and C#; the C# twin test is red on the build before.
- **Two wrong tests.** An audit assertion claimed a skipped struct-returning original leaves `(0, 0, 0)`; the run showed
  different leftover bytes on every build, so the buffer is undefined and the test asserts the defined outcome (the
  original's result never lands there). The positive control added to the scroll test showed the x case of
  `set_scroll_from_pos` never scrolled: the test passed the window's left edge as the position, so both variants gave 0
  and the comparison was vacuous. It now passes the position after the item, and the control holds.

## What a test is here

| Suite | File | Assertions go through |
|---|---|---|
| `api`, `regression` | `lua/rftest_api.lua`, `lua/rftest_regression.lua` | `lua/rftest/harness.lua`: `S:test`, `S:frame_test`, `S:last_frame_test`, `H.eq`/`H.ne`/`H.near`/`H.truthy`/`H.raises`/`H.no_raise`, `H.skip` |
| `plugin_api`, `plugin_regression` | `plugin/rftest_plugin.cpp` | `Suite::record(name, ok, detail)` / `Suite::set(name, status, detail)` |
| `csharp` | `csharp/rftest_csharp.cs` | `Test(name, fn)` with its assertion helpers |

Each suite writes `reframework/data/rftest/results/<suite>.json` (`pass`, `fail`, `skip`, one entry per test with a
detail) and one `[rftest]` log line per test. The runner reads the entries and lists every FAIL and every SKIP with
its detail in `summary.txt` / `summary.md`; nothing else counts. A test that can't run its check records a FAIL that
says why; it never just leaves its entry out.

**Every assertion goes through the suite's helper.** Do not `pcall` an assertion away, keep a private pass/fail flag
that never reaches `record`, or log a failure instead of failing. A test whose failure only shows up in the log is a
test that passes.

## Skips are for absence, never for failure

A SKIP means "this game cannot express the test", and it must name what is missing:
- a method or type the game doesn't have (`method(...)` in `rftest_api.lua` skips with "X is not in this game");
- an environment that can't produce the case (a volume without 8.3 short names);
- engine data the SDK doesn't read for that TDB (module method instantiations from TDB 81 on).

A SKIP never means "the behavior under test didn't happen". If the test calls a hooked method itself and the hook
doesn't fire, that is a FAIL. If an operation the test needs returns nothing, that is a FAIL. Every SKIP in a full run
is reported with its reason; a SKIP count that grows is a regression until shown otherwise.

## The rules that make an in-game test VALID

Break one and the test passes against buggy code, which is worse than no test.

1. **Drive the code path yourself; don't depend on the game.** Call the hooked method from the test, create the value
   you read, and assert on what your own call produced. A test that waits for the game to call something, or to have
   something in the scene, passes or fails with the game's state, not with REFramework's.
2. **It must fail on `master` for the stated reason.** Run it against a `master` build and read the failure *detail*,
   not just the status. A FAIL for an unrelated reason (missing type, a crash in setup) proves nothing about the fix.
3. **Assert the specific outcome.** Not "it raised", but that it raised the sandbox's rejection; not "a number came
   back", but the value. An assertion that any failure satisfies is satisfied by the unfixed code too.
4. **Unfixed builds must not corrupt the run.** A test whose failure mode is memory corruption detects the unfixed
   build and fails without performing the write. A test whose failure mode is a hang runs last
   (`S:last_frame_test`, the plugin's deadlock stage) with its FAIL recorded *before* it starts, so a hang leaves
   complete results. A test with side effects on unfixed builds (the rooted-path writes create `\rftest_escape\`) says
   so, and the runner cleans up.
5. **Game-agnostic.** Use only `System.*` and `via.*` types that exist across RE Engine games. When the code under test
   depends on the TDB version (value-type `this` before and after TDB 74), the full run must include games on both
   sides, and the test must not assume one.
6. **Controls are labeled.** A test that passes on `master` on purpose (it guards a fix against an earlier version of
   itself) says so in its comment, and the docs count it as a control.

## Step by step: demonstrate, then fix

1. **Write the test** in the right suite (`regression` for a bug fixed here, `api` for documented behavior). Name the
   behavior, not the fix.
2. **Run it against a build without the fix and confirm it FAILS for the right reason:**
   `Run-RFTests.ps1 -Game <g> -Build <master dinput8.dll> -Suites <suite>`. Read the detail in `<suite>.json`. If it
   passes, you are testing the wrong thing.
3. **Fix the code, rebuild, confirm it PASSES** with `-Build <fixed dinput8.dll>`.
4. **Run everything everywhere:** `Run-AllGames.ps1 -Build <fixed> -Suites api,regression,plugin,csharp`, and
   `run-rftests.sh` on Proton when the change touches platform behavior (paths, DLL loading, threads). "My test passes"
   is necessary, not sufficient; the full run in every game is the contract.
5. **Cite the evidence in the commit:** the failing detail on `master`, the passing run, the games covered.

## Bugs a suite can't show

Some failures don't fit an assertion. They still need demonstration; the form changes.

- **Hangs are red tests too.** A deadlock shows up as a game that stops producing results; the runner's timeout and
  the pre-recorded FAIL are the red. Keep such tests last, and keep the FAIL recorded first.
- **Races.** A race between threads (a callback registering while a script reset destroys the Lua state) usually can't
  be forced deterministically from a test. State the exact interleaving and the lock that now prevents it, say plainly
  in the commit that there is no red test *because the symptom is timing, not a value*, and keep the existing tests
  for the surrounding behavior green. Do not present a passing functional test as proof of a race fix; it passed
  before too.
- **Performance.** "No longer allocates on every call" is a positive claim: measure it (same script, same game,
  build A vs build B) and report the numbers.
- **The environment.** Steam Cloud options, a console window on the desktop, a volume with 8.3 names disabled, a
  first Proton start: these change results without any REFramework change. When a result moves, rule the environment
  out before blaming the code (see README "Games stuck resizing").

## Fix the shape, not just the symptom

Passing tests say "is it correct?", not "is it the right fix?". When a failure has a root cause, removing the failure
mode is the default; guarding the symptom is a fallback you must justify in the commit, with the proper fix named.
Example from this codebase: hook installation asked the managed runtime whether a return type is primitive, under the
hook map's lock, and cached a wrong answer for good if the runtime wasn't ready. Retrying or caching harder would hide
it; deciding from type metadata alone removes it.

## Common pitfalls

- **"It returned the right value, so the fix works."** Did it return the right value on `master` too? Rule 2.
- **"Either result is fine."** If you can't say which value is correct, you don't understand the code path yet.
- **A test that passes because the game didn't do something.** Rule 1.
- **Counting on log lines.** The results JSON is the contract; log lines are for reading.
- **Testing only one TDB generation.** Value-type behavior changed at TDB 74; one game proves one side.
