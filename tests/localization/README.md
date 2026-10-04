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
- Stable ImGui IDs across English, Simplified Chinese, Traditional Chinese,
  Japanese, and Korean, including existing `###` IDs and hidden `##` labels.
- Original option values, indices, and pointers, plus unknown, empty, and null text.
- Coverage and actual CPU rasterization of every non-ASCII character in the four
  catalogs and language names using the embedded font. Missing glyphs are reported
  by Unicode code point with the entries that use them.

These checks do not launch a game or verify the final in-game layout.

The generator checks use temporary copies to reject missing or duplicate keys,
changed format placeholders, invalid language metadata, and stale generated headers.
