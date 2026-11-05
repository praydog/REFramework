Contributing to REFramework
REFramework is a shared mod framework, scripting platform and modding tool that runs inside RE Engine games. A single build (REFramework.zip / dinput8.dll) is loaded by many titles at once, so a change that looks safe in isolation can regress every game it ships in. The rules below exist to keep that risk under control and to keep review fast.

They describe how the repository is actually maintained today, not an aspiration.

1. Pull requests vs. issues
Small, self-contained fixes can go straight to a pull request. This repository is maintained by one person; a focused PR with a working description is seen faster than an issue that waits in the queue.
Open an issue first for anything large or debatable: new features, new APIs, refactors of shared code, or a change of behaviour that other mods could depend on. Getting agreement on the approach before writing code saves rework.
One logical change per pull request. Do not bundle a bug fix with an unrelated refactor, a formatting pass, or a dependency bump.
Do not add notes, reports, analysis documents or new .md files as part of a code change. Put the explanation in the PR description. Documentation changes belong in their own PR.
Keep the diff minimal. Do not reformat or restructure surrounding code that your change does not touch. Reviewers read diffs, not final files.
Leave "Allow edits by maintainers" enabled. Maintainers may push commits onto a contributor's branch during review rather than requesting another round trip.
Bug reports and feature requests should use the templates in .github/ISSUE_TEMPLATE/ (bug_report.md, feature_request.md). Bug reports are expected to include the framework log file for the affected game (for example re2_framework_log.txt, re8_framework_log.txt) and, for crashes, the dump — the whole file, not the last line.

2. Build and verify locally
Full instructions live in COMPILING.md. The short version:

git submodule update --init --recursive

cmake -S . -B build -G "Visual Studio 18 2026" -A x64 ^
      -DCMAKE_BUILD_TYPE=Release -DDEVELOPER_MODE=ON ^
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

cmake --build build --config Release --target REFramework
Toolchain: MSVC with C++23 support (Visual Studio 2022 is the tested configuration; CI uses the 2026 generator). Other compilers are not supported.
DEVELOPER_MODE=ON is what CI builds. Keep it on locally so you see the developer tools path compile too.
The output is build/bin/REFramework/dinput8.dll — the same monolithic DLL used by every supported game.
build_vs2022.bat builds all targets in Release.
Submodules (dependencies/imgui, minhook, spdlog, glm, and the CMake FetchContent deps) must be initialised; a stale submodule tree is the most common cause of "it does not build".
CMakeLists.txt is generated — do not edit it
CMakeLists.txt is generated from cmake.toml by cmkr and carries a DO NOT EDIT banner. If your change needs build-system work (new target, new source glob, new dependency, new compile flag, new define), edit cmake.toml. Source files under src/ are picked up by glob (src/**.cpp, src/**.hpp), so new files in existing directories need no build-system change at all.

3. CI
.github/workflows/build-pr.yml runs on every pull request and does three things:

dev/audit_direct_access_clang.py — a libclang-based, type-aware audit. It flags direct member access on guarded RE Engine structures (REGameObject, REComponent, REManagedObject, REType, REFieldList, VariableDescriptor, FunctionDescriptor, TargetState, RenderTargetView, Buffer, …). Those fields must be reached through the accessors, because their layout and virtual layout differ between games and builds. Any violation fails the job.
Configure with DEVELOPER_MODE=ON and build the REFramework target in Release.
Upload build/bin/REFramework/dinput8.dll as an artifact.
Run the audit locally before pushing — it is much faster than waiting for CI:

pip install libclang
python dev/audit_direct_access_clang.py            # default high-value targets
python dev/audit_direct_access_clang.py src/Foo.cpp # specific files
It exits non-zero when it finds violations. If your MSVC STL is 14.44 or newer the bundled LLVM 18 is too old for it; install LLVM 19+ or point CLANG_LIBRARY_FILE at your system libclang.dll.

CI runs on pull requests from forks only after a maintainer approves the run, so a red "waiting for approval" state is not a failure of your branch.

4. Testing is a hard requirement
Code alone is not reviewed. A change to this codebase is evaluated on what happened when it ran inside a game.

Build your branch and run the affected game(s) before opening the pull request.
State exactly what you tested in the description: which game, which build configuration, what you hooked or toggled, what you did, and what you observed.
State what you did not test. Untested paths are acceptable when they are called out; silently implying coverage is not.
Do not write claims you have not verified. Statements about race conditions, crash causes or "partial fixes" are treated as testable claims and will be checked. If something is a suspicion, label it as a suspicion and keep it out of the commit message.
If the current code already handles the scenario your change claims to fix, say so and drop the change instead of shipping a no-op.
A concrete note is worth more than a paragraph of theory:

Hooked PlayerBase::update (instance method, first parameter is float) in RE4, ran in-game for ~10 minutes, arguments decoded correctly, no crash on repeated add/remove.

Verification matrix for hooking changes
Anything that touches HookManager, the JIT facilitator, or argument/return marshalling affects every hook in every supported game. Cover at least:

Case	What it exercises
Static method hook	Argument decoding without an instance context
Instance method hook	Argument decoding with this present
First parameter is float / double / System.Single	Floating-point argument marshalling
Floating-point return value	Floating-point return marshalling
out / ref parameters	Indirect parameter passing
Virtual (vtable) hook	A separate code path from inline hooks, with its own locking
Hook added and removed at runtime from a script callback	Locking and lifetime under re-entrancy
Lua script reset with hooks still registered	Hook teardown during script state destruction
5. Changes to shared components
HookManager, the JIT facilitator, the D3D11/D3D12 hooks, the Lua manager, TDB and the plugin loader are used by every game and every mod at once, so changes there are held to a higher bar than changes to a single mod:

Understand the invariants before changing them. Argument and return marshalling, register usage, lock ordering and object lifetimes in these components follow rules that are not obvious from the surrounding code. If you cannot explain why something is written the way it is, treat it as intentional and ask in the PR instead of "fixing" it.
One behaviour change at a time. A PR that alters several invariants at once cannot be reviewed or bisected. Split it.
Do not rewrite a shared path to fix one game. Per-game behaviour belongs behind an explicit game check, not in a global code path.
Locking. Do not hold a framework lock while calling into game code, JIT-generated code, Lua or script callbacks. Callbacks may legitimately re-enter the hook API, and concurrent add/remove from different threads must not deadlock.
Failure paths must be honest. A handle that was never created must not be reported as success, cleanup must tolerate partial failure, and removing an unknown ID must not corrupt state.
Comments must match reality. If you change behaviour, update the comment that describes it. A stale or wrong comment is treated as a defect, not as a cosmetic issue.
Justify, do not assume. "This might be a race" is not a reason to restructure code. Show the interleaving, or say explicitly that the change is defensive and keep the diff minimal.
6. Code style
.clang-format is checked in: LLVM base style, 4-space indent, 140-column limit. Format the files you touched.
.clang-tidy enables readability-identifier-naming (and readability-else-after-return):

namespaces, functions, methods, parameters, variables, members: lower_case

classes, structs, enums, unions, typedefs: CamelCase

no private member suffix


Language level is C++23. Prefer existing utilities and helpers in shared/utility/ over new third-party dependencies; a new dependency needs a justification in the PR.
Logging goes through spdlog with a bracketed component prefix, e.g. spdlog::info("[HookManager] Using actual function @ {:p}", ...). Match the surrounding file.
Comments should explain why, and must be updated when the behaviour they describe changes.
Match the style of the file you are editing rather than converting it.
7. Commit messages
The history follows one loose convention, keep to it:

Lua: Make json.dump_file write atomically (fixes #1795)
DD2: Fix ultrawide fix breaking windowing behavior
LooseFileLoader: Don't spam log when path_to_hash hook fails (fixes #1876)
Area: Imperative summary — area first when the change is scoped to one component (HookManager, Lua, Graphics, LooseFileLoader, .NET, SDK/API, a game abbreviation such as DD2).
Reference the issue with (fixes #1234) when the change closes one.
Explain the reason in the body when the subject alone cannot carry it. No filler, no restating the diff.
PRs are squash-merged, so the PR title becomes the commit subject. Make the title conform to the same format.
8. Pull request description
## What
One or two sentences: the problem and the fix.

## Why
The mechanism. Cite the file/function/line and explain the failure mode,
including how you confirmed it (in-game observation, log line, disassembly).

## Changes
- file: what changed and why
- file: what changed and why

## Testing
- Game(s) and version:
- Build: Release / DEVELOPER_MODE, branch commit
- Scenario: what you hooked/toggled and what you did in-game
- Observed: what actually happened (log excerpt, value, behaviour)
- Not tested: paths you could not exercise

## Related
refs #NNNN
Keep it factual. If a claim did not survive your own testing, delete it before submitting.

9. API and compatibility changes
Do not silently break existing Lua bindings, the plugin ABI, or the C# API (csharp-api/). If a binding must change, explain the migration path in the PR description.
Test any API change with a real script or plugin that uses the affected surface, not just a compile.
New or changed scripting documentation belongs in the docs repositories, not in this one.
The framework is built as one universal binary (REFRAMEWORK_UNIVERSAL); do not add per-game conditional compilation without a concrete compatibility reason, and do not strip the universal build paths (dev/strip_non_universal.py exists for analysis, not for shipping changes).
10. Legal
No game assets, extracted game data, decompiled game source, or other copyrighted material in pull requests.
Reverse-engineered offsets, signatures and structure layouts should be described in terms of your analysis and the tools you used (Ghidra, IDA, runtime inspection), not by pasting proprietary content.
Feature codes / signatures that were not verified against a real binary do not belong in a PR.
11. Questions
If you are unsure whether a change belongs here, or whether an approach is acceptable, ask in the PR or open an issue describing the plan. Scripting documentation: https://refdocs.praydog.com.
