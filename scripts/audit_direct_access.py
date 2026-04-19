"""
audit_direct_access.py — Static analysis for direct struct field access bypassing dispatch.

Scans C++ source files for direct member access (->field or .field) on struct types
whose layout varies per RE Engine game. Flags violations that should use an accessor
method instead.

Usage:
    python scripts/audit_direct_access.py [--json] [--summary] [path ...]
    
    Default paths: src/ shared/sdk/ csharp-api/

Exit code: 0 if no violations, 1 if violations found.
"""
import argparse
import json
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

# ============================================================================
# GUARDED TYPES: struct name -> set of field names that must go through accessors.
#
# Every field listed here is known to have a different offset on at least one
# supported game, OR is a footgun because the struct's base class size varies
# (shifting all fields).
#
# Fields are categorized:
#   VARIES  = offset confirmed different across supported games
#   FOOTGUN = offset stable today but struct is layout-variant; accessor protects
#             against future games
# ============================================================================

GUARDED_FIELDS = {
    "REGameObject": {
        # VARIES: 3+ distinct offsets across games
        "transform":      "VARIES",  # 0x18 / 0x20 / 0x28
        "folder":         "VARIES",  # 0x20 / 0x28 / 0x30
        "name":           "VARIES",  # 0x28 / 0x30 / 0x38 + type varies (REString vs SystemString*)
        "shouldUpdate":   "VARIES",  # 0x10 / 0x12 / 0x22
        "shouldDraw":     "VARIES",  # 0x11 / 0x13 / 0x23
        "shouldUpdateSelf":"VARIES", # 0x12 / 0x14 / 0x24
        "shouldDrawSelf": "VARIES",  # 0x13 / 0x15 / 0x25
        "shouldSelect":   "VARIES",  # 0x14 / 0x16 / 0x26
        "timescale":      "VARIES",  # 0x34 / 0x3C / 0x4C / 0x5C
    },
    "REComponent": {
        # VARIES: RE7 shifts all by +0x10 due to enlarged REManagedObject base
        "ownerGameObject": "VARIES",  # 0x10 / 0x20
        "childComponent":  "VARIES",  # 0x18 / 0x28
        "prevComponent":   "VARIES",  # 0x20 / 0x30
        "nextComponent":   "VARIES",  # 0x28 / 0x38
    },
    "REManagedObject": {
        "info":            "VARIES",  # 0x00 / 0x18 (RE7)
        "referenceCount":  "FOOTGUN", # 0x08 everywhere — stable today
    },
    "REFieldList": {
        "unknown":       "VARIES",  # absent on RE7
        "next":          "VARIES",  # absent on RE7
        "methods":       "VARIES",  # 0x10 / 0x00 (RE7)
        "num":           "VARIES",  # 0x18 / 0x08 (RE7)
        "maxItems":      "VARIES",  # 0x1C / 0x0C (RE7)
        "variables":     "FOOTGUN", # 0x20 everywhere — coincidentally stable
        "deserializer":  "VARIES",  # 0x28 / 0x30 (RE7)
    },
    "VariableDescriptor": {
        # RE7 TDB49 has completely different layout; modern RE7 TDB70 matches RE8
        # but the struct is still a footgun if a new game rearranges it
        "name":              "FOOTGUN",
        "nameHash":          "FOOTGUN",
        "flags1":            "FOOTGUN",
        "function":          "FOOTGUN",  # 0x10 (RE8+) / 0x60 (RE7 TDB49)
        "flags":             "FOOTGUN",
        "typeFqn":           "FOOTGUN",
        "typeName":          "FOOTGUN",
        "variableType":      "FOOTGUN",
        "staticVariableData":"FOOTGUN",
        "size":              "FOOTGUN",
    },
    "FunctionDescriptor": {
        "name":         "FOOTGUN",
        "nameHash":     "FOOTGUN",
        "functionPtr":  "FOOTGUN",  # 0x18 everywhere today
    },
    # REType is handled via utility::re_type_accessor — flag direct access
    "REType": {
        "name":       "VARIES",  # 0x20 / 0x28 (RE7)
        "size":       "VARIES",  # 0x2C / 0x30 / 0x38 — AND swapped with typeCRC on TDB>=81
        "typeCRC":    "VARIES",  # 0x30 / 0x2C / 0x188
        "super":      "VARIES",  # 0x38 / 0x40 / 0x60
        "childType":  "VARIES",  # 0x40 / 0x48 / 0x68
        "chainType":  "VARIES",  # 0x48 / 0x50 / 0x70
        "fields":     "VARIES",  # 0x50 / 0x58 / 0x178
        "classInfo":  "VARIES",  # 0x58 / 0x60 / 0x180
        "type_flags": "VARIES",
        "methods":    "FOOTGUN",
        "type":       "FOOTGUN",
        "objectFlags":"FOOTGUN",
        "objectType": "FOOTGUN",
    },
    # RenderResource-derived: base class size varies, shifting ALL member offsets
    "TargetState": {
        "m_desc": "VARIES",  # this + sizeof(RenderResource) which is 0x10/0x18/0x20
    },
    "RenderTargetView": {
        "m_desc": "VARIES",
    },
    "Buffer": {
        "m_size_in_bytes": "VARIES",
        "m_usage_type":    "VARIES",
        "m_option_flags":  "VARIES",
    },
}

# Field names that are too generic to match without type context.
# These are only flagged with --pedantic.
AMBIGUOUS_FIELDS = {
    "name", "size", "type", "info", "flags", "methods", "num",
    "function", "next", "unknown", "variables",
    "type_flags", "objectFlags", "objectType",
    # Generic names that collide with user-defined structs (DescType::super, etc.)
    "super", "fields", "classInfo", "referenceCount", "typeName",
}

# Files/directories to scan
DEFAULT_SCAN_PATHS = ["src/", "shared/sdk/", "csharp-api/"]

# Files where direct access is EXPECTED (accessor implementations)
ACCESSOR_IMPL_FILES = {
    "shared/sdk/REGameObject.cpp",          # get_transform() impl
    "shared/sdk/REManagedObject.cpp",       # classinfo_accessor, is_managed_object
    "shared/sdk/REType.cpp",                # re_type_accessor impls
    "shared/sdk/RETypeLayouts.hpp",         # re_type_accessor definitions
    "shared/sdk/RETypeDefinition.cpp",      # TDEF_FIELD implementations
    "shared/sdk/RETypeDefDispatch.hpp",     # TDEF_FIELD macro definitions
    "shared/sdk/RETypeDB.cpp",              # TDB accessor implementations
    "shared/sdk/RETypeDB.hpp",              # TDB_DISPATCH definitions
    "shared/sdk/REComponent.hpp",           # re_component accessors
    "shared/sdk/Renderer.hpp",              # TargetState/RTV accessor defs
    "shared/sdk/Renderer.cpp",              # accessor implementations
    "shared/sdk/ViaDispatch.hpp",           # ViaDispatch implementations
    "shared/sdk/CameraSystemDispatch.hpp",
    "shared/sdk/REVariableDescriptor.hpp",  # accessor impl for VariableDescriptor::flags
}

# Also whitelist: ReClass_Internal headers themselves (struct definitions)
RECLASS_PATTERN = re.compile(r"ReClass_Internal.*\.hpp$")


def normalize_path(p):
    return p.replace("\\", "/")


def is_whitelisted(filepath):
    fp = normalize_path(filepath)
    for af in ACCESSOR_IMPL_FILES:
        if fp.endswith(af):
            return True
    if RECLASS_PATTERN.search(os.path.basename(filepath)):
        return True
    return False


def scan_file(filepath):
    """Scan a single C++ file for direct field access violations."""
    violations = []
    try:
        with open(filepath, "r", encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
    except OSError:
        return violations

    # Build a combined regex: match ->fieldName or .fieldName where fieldName
    # is in any guarded type's field set.
    # We can't know the LHS type statically without a real parser, so we match
    # ALL occurrences of ->fieldName and then filter by context.
    all_fields = set()
    field_to_types = defaultdict(set)
    for stype, fields in GUARDED_FIELDS.items():
        for fname in fields:
            all_fields.add(fname)
            field_to_types[fname].add(stype)

    # Pattern: -> or . followed by a guarded field name, followed by a non-identifier char
    # We use word boundary after the field name to avoid partial matches
    field_pattern = re.compile(
        r"(?:->|\.)\s*(" + "|".join(re.escape(f) for f in sorted(all_fields, key=len, reverse=True)) + r")\b"
    )

    for lineno, line in enumerate(lines, 1):
        stripped = line.strip()
        # Skip comments
        if stripped.startswith("//"):
            continue
        # Skip lines inside block comments (crude heuristic)
        if stripped.startswith("*") or stripped.startswith("/*"):
            continue
        # Skip string literals (crude: skip lines with the field name inside quotes)
        # Skip static_assert lines
        if "static_assert" in stripped:
            continue
        # Skip #include / #define / #if lines
        if stripped.startswith("#"):
            continue

        for m in field_pattern.finditer(line):
            field_name = m.group(1)
            # Check context: is this likely a direct access on a guarded type?
            # Heuristic: look at what's before the -> or .
            prefix = line[:m.start()].rstrip()

            # Skip if this is inside a comment (// before the match)
            comment_pos = line.find("//")
            if comment_pos >= 0 and comment_pos < m.start():
                continue

            # Skip if inside a string literal
            # Count quotes before the match position
            pre_match = line[:m.start()]
            if pre_match.count('"') % 2 != 0:
                continue

            # Skip accessor method definitions (get_fieldname, set_fieldname)
            if re.search(r"get_" + re.escape(field_name) + r"\s*\(", line):
                continue
            if re.search(r"set_" + re.escape(field_name) + r"\s*\(", line):
                continue

            # Record the violation
            category = "VARIES"
            for stype in field_to_types[field_name]:
                cat = GUARDED_FIELDS[stype][field_name]
                if cat == "VARIES":
                    category = "VARIES"
                    break

            violations.append({
                "file": normalize_path(filepath),
                "line": lineno,
                "field": field_name,
                "category": category,
                "types": sorted(field_to_types[field_name]),
                "text": stripped[:120],
            })

    return violations


def scan_tree(paths):
    """Walk directories and scan all .cpp/.hpp/.h files."""
    all_violations = []
    files_scanned = 0

    for root_path in paths:
        if os.path.isfile(root_path):
            files_to_scan = [root_path]
        else:
            files_to_scan = []
            for dirpath, _, filenames in os.walk(root_path):
                for fn in filenames:
                    if fn.endswith((".cpp", ".hpp", ".h")):
                        files_to_scan.append(os.path.join(dirpath, fn))

        for filepath in files_to_scan:
            if is_whitelisted(filepath):
                continue
            # Skip dependencies
            if "dependencies" in normalize_path(filepath):
                continue
            files_scanned += 1
            violations = scan_file(filepath)
            all_violations.extend(violations)

    return all_violations, files_scanned


def main():
    parser = argparse.ArgumentParser(description="Audit direct struct field access in REFramework")
    parser.add_argument("paths", nargs="*", default=DEFAULT_SCAN_PATHS, help="Paths to scan")
    parser.add_argument("--json", action="store_true", help="Output as JSON")
    parser.add_argument("--summary", action="store_true", help="Summary only (no per-line detail)")
    parser.add_argument("--varies-only", action="store_true", help="Only show VARIES (skip FOOTGUN)")
    parser.add_argument("--pedantic", action="store_true", help="Include ambiguous field names (name, size, type, etc.)")
    args = parser.parse_args()

    violations, files_scanned = scan_tree(args.paths)

    if args.varies_only:
        violations = [v for v in violations if v["category"] == "VARIES"]


    if not args.pedantic:
        violations = [v for v in violations if v['field'] not in AMBIGUOUS_FIELDS]
    if args.json:
        print(json.dumps({
            "files_scanned": files_scanned,
            "total_violations": len(violations),
            "violations": violations,
        }, indent=2))
    elif args.summary:
        # Group by file
        by_file = defaultdict(list)
        for v in violations:
            by_file[v["file"]].append(v)

        varies_count = sum(1 for v in violations if v["category"] == "VARIES")
        footgun_count = sum(1 for v in violations if v["category"] == "FOOTGUN")

        print(f"Files scanned: {files_scanned}")
        print(f"Total violations: {len(violations)} ({varies_count} VARIES, {footgun_count} FOOTGUN)")
        print()
        for filepath in sorted(by_file.keys()):
            vv = by_file[filepath]
            v_count = sum(1 for v in vv if v["category"] == "VARIES")
            f_count = sum(1 for v in vv if v["category"] == "FOOTGUN")
            print(f"  {filepath}: {len(vv)} ({v_count}V/{f_count}F)")
    else:
        # Detailed output
        by_file = defaultdict(list)
        for v in violations:
            by_file[v["file"]].append(v)

        varies_count = sum(1 for v in violations if v["category"] == "VARIES")
        footgun_count = sum(1 for v in violations if v["category"] == "FOOTGUN")

        print(f"Files scanned: {files_scanned}")
        print(f"Total violations: {len(violations)} ({varies_count} VARIES, {footgun_count} FOOTGUN)")
        print()

        for filepath in sorted(by_file.keys()):
            vv = by_file[filepath]
            print(f"--- {filepath} ({len(vv)} violations) ---")
            for v in vv:
                marker = "!!" if v["category"] == "VARIES" else "  "
                types_str = "/".join(v["types"])
                print(f"  {marker} L{v['line']:>5}: {types_str}::{v['field']}  {v['text'][:80]}")
            print()

    return 1 if violations else 0


if __name__ == "__main__":
    sys.exit(main())
