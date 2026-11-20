"""Validate catalog generation and rejection of malformed localization input.

Run with Python 3 from any working directory. The checkout is read only: all
mutations and generated files live in a temporary directory removed after use.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def write_json(path: Path, value) -> None:
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def change_json(fixture: Path, filename: str, mutation) -> None:
    target = fixture / "src/localization" / filename
    value = json.loads(target.read_text(encoding="utf-8"))
    mutation(value)
    write_json(target, value)


def set_translation(key: str, value):
    return lambda fixture: change_json(
        fixture, "ko-KR.json", lambda catalog: catalog.__setitem__(key, value)
    )


def duplicate_key(fixture: Path) -> None:
    target = fixture / "src/localization/ko-KR.json"
    text = target.read_text(encoding="utf-8").rstrip()
    target.write_text(text[:-1] + ',\n  "Configuration": "Duplicate"\n}\n', encoding="utf-8")


def stale_header(fixture: Path, filename: str) -> None:
    target = fixture / "src/localization" / filename
    target.write_text(target.read_text(encoding="utf-8") + "// Stale test content\n", encoding="utf-8")


def registry_change(mutation):
    return lambda fixture: change_json(fixture, "languages.json", mutation)


def swap_languages(registry, first, second):
    languages = registry["languages"]
    languages[first], languages[second] = languages[second], languages[first]


def new_language():
    # Reuse valid catalog data to test registry growth independently of translation.
    return {"id": "German", "tag": "de-DE", "name": "Deutsch", "catalog": "ko-KR.json"}


def run_generator(fixture: Path, check=False) -> subprocess.CompletedProcess:
    command = [sys.executable, str(fixture / "scripts/generate_localization.py")]
    if check:
        command.append("--check")
    return subprocess.run(command, cwd=fixture, capture_output=True, encoding="utf-8", timeout=30)


def exercise(source: Path, run_root: Path) -> list:
    baseline = run_root / "baseline"
    catalog_root = baseline / "src/localization"
    (baseline / "scripts").mkdir(parents=True)
    catalog_root.mkdir(parents=True)
    shutil.copy2(source / "scripts/generate_localization.py", baseline / "scripts")
    registry = json.loads((source / "src/localization/languages.json").read_text(encoding="utf-8"))
    catalogs = [item["catalog"] for item in registry["languages"] if item["catalog"]]
    for filename in ["languages.json", *catalogs]:
        shutil.copy2(source / "src/localization" / filename, catalog_root / filename)
    entry_count = len(json.loads((catalog_root / catalogs[0]).read_text(encoding="utf-8")))
    description = (
        f"{len(catalogs)} catalogs, {entry_count} entries each, "
        f"{len(registry['languages'])} selectable languages"
    )
    results = []

    def record(name, result, expected_success, diagnostic):
        output = (result.stdout + result.stderr).strip()
        passed = (result.returncode == 0) == expected_success and diagnostic in output
        results.append({
            "name": name,
            "passed": passed,
            "exit_code": result.returncode,
            "required_diagnostic": diagnostic,
            "output": output,
        })

    record("normal_generation", run_generator(baseline), True, "Generated " + description)
    record("normal_check", run_generator(baseline, check=True), True, "Verified " + description)
    if not all(item["passed"] for item in results):
        return results

    cases = [
        (
            "printf_type_changed",
            set_translation("Last Script Error: %s", "Script error: %d"),
            "printf placeholders changed", False,
        ),
        (
            "printf_precision_changed",
            set_translation("Megabytes in use: %.2f", "Memory: %.3f"),
            "printf placeholders changed", False,
        ),
        (
            "printf_order_changed",
            set_translation("Game Time: %s: %.2fms", "Time: %.2f: %sms"),
            "printf placeholders changed", False,
        ),
        (
            "missing_key",
            lambda p: change_json(p, "ko-KR.json", lambda c: c.pop("Configuration")),
            "missing keys ['Configuration']", False,
        ),
        (
            "extra_key",
            set_translation("Unexpected Test Key", "Test"),
            "extra keys ['Unexpected Test Key']", False,
        ),
        (
            "duplicate_json_key", duplicate_key,
            "Duplicate JSON key: Configuration", False,
        ),
        (
            "brace_placeholder_missing",
            set_translation("Custom Shader {}", "Custom shader"),
            "format fields changed", False,
        ),
        (
            "imgui_id_injected",
            set_translation("Configuration", "Settings##injected"),
            "translation introduces an ImGui ID or null byte", False,
        ),
        (
            "null_byte_injected",
            set_translation("Configuration", "Settings\0tail"),
            "translation introduces an ImGui ID or null byte", False,
        ),
        (
            "empty_translation", set_translation("Configuration", ""),
            "empty or invalid translation", False,
        ),
        (
            "non_string_translation", set_translation("Configuration", 123),
            "empty or invalid translation", False,
        ),
        (
            "newline_count_changed", set_translation("Configuration", "Settings\n"),
            "newline count changed", False,
        ),
        (
            "stale_catalog_header", lambda p: stale_header(p, "ko_KR.hpp"),
            "ko_KR.hpp is out of date", True,
        ),
        (
            "stale_registry_header", lambda p: stale_header(p, "catalogs.hpp"),
            "catalogs.hpp is out of date", True,
        ),
        (
            "missing_generated_header", lambda p: (p / "src/localization/ko_KR.hpp").unlink(),
            "ko_KR.hpp is out of date", True,
        ),
        (
            "duplicate_language_id",
            registry_change(lambda r: r["languages"][-1].__setitem__("id", r["languages"][-2]["id"])),
            "Duplicate language ID or tag", False,
        ),
        (
            "duplicate_language_tag",
            registry_change(lambda r: r["languages"][-1].__setitem__("tag", r["languages"][-2]["tag"])),
            "Duplicate language ID or tag", False,
        ),
        (
            "invalid_default_language",
            registry_change(lambda r: r.__setitem__("default", "Unknown")),
            "The default language must remain English", False,
        ),
        (
            "persisted_language_order_changed",
            registry_change(lambda r: r["languages"].__setitem__(
                slice(0, 2), list(reversed(r["languages"][:2]))
            )),
            "Preserve persisted language IDs", False,
        ),
    ]
    # All shipped IDs are persisted, including the three added after Simplified Chinese.
    for first, second in [(2, 3), (2, 4), (3, 4)]:
        cases.append((
            f"persisted_languages_swapped_{first}_{second}",
            registry_change(lambda r, a=first, b=second: swap_languages(r, a, b)),
            "Preserve persisted language IDs", False,
        ))
    for index in range(2, 5):
        cases.append((
            f"persisted_language_removed_{index}",
            registry_change(lambda r, i=index: r["languages"].pop(i)),
            "Preserve persisted language IDs", False,
        ))
    for index in range(1, 5):
        cases.append((
            f"language_inserted_before_persisted_id_{index}",
            registry_change(lambda r, i=index: r["languages"].insert(i, new_language())),
            "Preserve persisted language IDs", False,
        ))
    for language in registry["languages"][1:5]:
        identifier = language["id"]
        cases.append((
            f"non_english_default_{identifier}",
            registry_change(lambda r, name=identifier: r.__setitem__("default", name)),
            "The default language must remain English", False,
        ))
    for name, mutation, diagnostic, check in cases:
        fixture = run_root / name
        shutil.copytree(baseline, fixture)
        mutation(fixture)
        record(name, run_generator(fixture, check=check), False, diagnostic)

    # Appending a language must remain supported without editing the generator.
    fixture = run_root / "appended_language"
    shutil.copytree(baseline, fixture)
    registry_change(lambda r: r["languages"].append(new_language()))(fixture)
    extended_description = (
        f"{len(catalogs) + 1} catalogs, {entry_count} entries each, "
        f"{len(registry['languages']) + 1} selectable languages"
    )
    record("appended_language_generation", run_generator(fixture), True, "Generated " + extended_description)
    record(
        "appended_language_check", run_generator(fixture, check=True),
        True, "Verified " + extended_description,
    )

    # Valid edits must invalidate the generated output until it is regenerated.
    fixture = run_root / "valid_edit_regeneration"
    shutil.copytree(baseline, fixture)
    set_translation("Configuration", "Updated configuration")(fixture)
    record(
        "valid_edit_check_rejects_stale_output", run_generator(fixture, check=True),
        False, "ko_KR.hpp is out of date",
    )
    record("valid_edit_regeneration", run_generator(fixture), True, "Generated " + description)
    record(
        "valid_edit_regenerated_check", run_generator(fixture, check=True),
        True, "Verified " + description,
    )
    return results


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    temporary_root = Path(tempfile.gettempdir()).resolve()
    with tempfile.TemporaryDirectory(prefix="reframework-localization-", dir=temporary_root) as temporary:
        run_root = Path(temporary).resolve()
        if run_root.parent != temporary_root:
            raise RuntimeError("Fixture directory must remain inside the system temporary directory")
        results = exercise(args.source.resolve(), run_root)

    passed = sum(item["passed"] for item in results)
    print(f"{passed}/{len(results)} localization generator regression checks passed.")
    for item in results:
        if not item["passed"]:
            print(f"FAIL {item['name']} (exit {item['exit_code']})", file=sys.stderr)
            print(f"Expected diagnostic: {item['required_diagnostic']}", file=sys.stderr)
            print(item["output"] or "<no output>", file=sys.stderr)
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
