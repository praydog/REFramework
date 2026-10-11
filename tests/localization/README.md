# Localization regression tests

This standalone target uses the repository's real ImGui implementation and embedded
RobotoCJKSC-Medium font. It runs without a game, graphics device, or desktop window.
It requires CMake 3.24 or newer and a C++20 compiler; it does not build REFramework.

Run from the repository root with Visual Studio 2022 C++ build tools installed:

```powershell
git submodule update --init --recursive dependencies/imgui
python scripts/generate_localization.py --check
python tests/localization/test_generator.py
cmake -S tests/localization -B build/localization-tests -G "Visual Studio 17 2022" -A x64
cmake --build build/localization-tests --config Release
ctest --test-dir build/localization-tests -C Release --output-on-failure
```

Python 3 is needed for the generated-header and generator checks. If catalogs were edited,
run `python scripts/generate_localization.py` before that check. With another CMake
generator, adjust the generator and architecture options as appropriate.

The tests cover:

- English as the default, persisted language indices, and all five language names.
- Equal catalog key sets, UTF-8 validity, printf placeholders, and brace fields.
  Built-in module labels include `FaultyFileDetector`, `FirstPerson`, and
  `ManualFlashlight`; adding valid entries does not require changing a key count.
- Stable ImGui IDs across English, Simplified Chinese, Traditional Chinese,
  Japanese, and Korean, including existing `###` IDs and hidden `##` labels.
- Original option values, indices, and pointers, plus unknown, empty, and null text.
- Actual ObjectExplorer-style ImGui `TreeNode(ptr_id, format, ...)` calls across
  consecutive English/Chinese frames: unchanged pointer IDs, retained open and
  closed states, and rendered reflection/TDB counts and listener names/hit counts.
- Actual floating windows for Read Listeners, Pinned objects, and Hooked methods:
  translated title rendering, unchanged window instances/IDs, and retained moved
  positions when switching to Chinese and back to English.
- Actual progress bars and numeric input/drag controls: translated SDK progress
  text and labels, unchanged control IDs/values, and no visible `###` suffixes.
  Text is captured from ImGui's rendering log before `LogFinish` clears it; tree
  auto-expansion during logging is disabled so open-state checks remain meaningful.
- Coverage and actual CPU rasterization of every non-ASCII character in the four
  catalogs and language names using the embedded font. Missing glyphs are reported
  by Unicode code point with the entries that use them.

These checks do not launch a game or verify the final in-game layout.

The generator checks use temporary copies to reject missing or duplicate keys,
changed format placeholders, invalid language metadata, and stale generated headers.
