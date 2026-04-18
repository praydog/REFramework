"""
audit_direct_access_clang.py — libclang-based type-aware audit for direct struct field access.

Unlike the regex-based audit_direct_access.py, this script uses libclang to parse C++ source
files with full type information. It identifies MemberExpr nodes where the base expression's
type is a guarded struct, and the accessed member is a field that should go through an accessor.

This eliminates false positives from generic field names (name, size, type, etc.) by checking
the actual type of the expression.

Requirements:
    pip install libclang

Usage:
    python scripts/audit_direct_access_clang.py [file ...]
    
    If no files given, scans a default set of high-value targets.
"""
import argparse
import json
import os
import sys
import xml.etree.ElementTree as ET
from collections import defaultdict
from pathlib import Path

try:
    import clang.cindex as ci
except ImportError:
    print("ERROR: pip install libclang", file=sys.stderr)
    sys.exit(2)

# ============================================================================
# Configuration
# ============================================================================

REPO_ROOT = Path(__file__).resolve().parent.parent

GUARDED_TYPES = {
    "REGameObject": {
        "transform", "folder", "name", "shouldUpdate", "shouldDraw",
        "shouldUpdateSelf", "shouldDrawSelf", "shouldSelect", "timescale",
        "pad_0010", "pad_0017",
    },
    "REComponent": {
        "ownerGameObject", "childComponent", "prevComponent", "nextComponent",
    },
    "REManagedObject": {
        "info", "referenceCount",
    },
    "REFieldList": {
        "unknown", "next", "methods", "num", "maxItems", "variables", "deserializer",
    },
    "VariableDescriptor": {
        "name", "nameHash", "flags1", "function", "flags", "typeFqn", "typeName",
        "variableType", "staticVariableData", "size", "getter", "setter", "attributes",
    },
    "FunctionDescriptor": {
        "name", "nameHash", "functionPtr",
    },
    "REType": {
        "name", "size", "typeCRC", "super", "childType", "chainType",
        "fields", "classInfo", "type_flags", "methods", "type",
        "objectFlags", "objectType",
    },
    "TargetState": {"m_desc"},
    "RenderTargetView": {"m_desc"},
    "Buffer": {"m_size_in_bytes", "m_usage_type", "m_option_flags"},
}

# Flatten for quick lookup: field_name -> set of parent type names
FIELD_TO_TYPES = defaultdict(set)
for tname, fields in GUARDED_TYPES.items():
    for fname in fields:
        FIELD_TO_TYPES[fname].add(tname)

# Files where direct access is expected (accessor implementations)
WHITELIST_SUFFIXES = {
    "ReClass_Internal",  # all ReClass headers
    "REGameObject.cpp",
    "REManagedObject.cpp",
    "REType.cpp",
    "RETypeLayouts.hpp",
    "RETypeDefinition.cpp",
    "RETypeDefDispatch.hpp",
    "RETypeDB.cpp",
    "RETypeDB.hpp",
    "REComponent.hpp",
    "Renderer.hpp",
    "Renderer.cpp",
    "ViaDispatch.hpp",
    "CameraSystemDispatch.hpp",
}

DEFAULT_TARGETS = [
    "src/mods/Graphics.cpp",
    "src/mods/FirstPerson.cpp",
    "src/mods/FreeCam.cpp",
    "src/mods/Camera.cpp",
    "src/mods/VR.cpp",
    "src/mods/Hooks.cpp",
    "src/mods/PluginLoader.cpp",
    "src/mods/IntegrityCheckBypass.cpp",
    "src/mods/ManualFlashlight.cpp",
    "src/mods/tools/ObjectExplorer.cpp",
    "src/mods/tools/ChainViewer.cpp",
    "src/mods/bindings/Sdk.cpp",
    "src/mods/vr/games/RE8VR.cpp",
    "shared/sdk/REGlobals.cpp",
    "shared/sdk/REContext.cpp",
    "shared/sdk/RETransform.cpp",
    "shared/sdk/RETypes.cpp",
]


def is_whitelisted(filepath):
    basename = os.path.basename(filepath)
    for suffix in WHITELIST_SUFFIXES:
        if suffix in basename:
            return True
    return False


def get_compile_flags():
    """Extract include paths and defines from the MSVC vcxproj."""
    vcxproj = REPO_ROOT / "build" / "REFrameworkSDK.vcxproj"
    if not vcxproj.is_file():
        # Also check REFramework.vcxproj
        vcxproj = REPO_ROOT / "build" / "REFramework.vcxproj"
    if not vcxproj.is_file():
        print(f"WARNING: No vcxproj found at {vcxproj}, using minimal flags", file=sys.stderr)
        return get_minimal_flags()

    tree = ET.parse(vcxproj)
    root = tree.getroot()
    ns = {"ms": "http://schemas.microsoft.com/developer/msbuild/2003"}

    includes = []
    defines = []

    for idg in root.findall(".//ms:ItemDefinitionGroup", ns):
        condition = idg.get("Condition", "")
        if "RelWithDebInfo" in condition or "Release" in condition:
            cc = idg.find("ms:ClCompile", ns)
            if cc is not None:
                inc_el = cc.find("ms:AdditionalIncludeDirectories", ns)
                def_el = cc.find("ms:PreprocessorDefinitions", ns)
                if inc_el is not None:
                    for p in inc_el.text.split(";"):
                        p = p.strip()
                        if p and not p.startswith("%"):
                            includes.append(f"-I{p}")
                if def_el is not None:
                    for d in def_el.text.split(";"):
                        d = d.strip()
                        if d and not d.startswith("%") and "CMAKE_INTDIR" not in d:
                            defines.append(f"-D{d}")
            break

    # Add MSVC compatibility flags for clang
    flags = [
        "-x", "c++",
        "-std=c++20",
        "-fms-extensions",
        "-fms-compatibility",
        "-fdelayed-template-parsing",
        "-Wno-everything",  # suppress all warnings, we only want AST
    ]
    flags.extend(includes)
    flags.extend(defines)

    # Add Windows SDK include if available
    win_sdk = os.environ.get("WindowsSdkDir", r"C:\Program Files (x86)\Windows Kits\10")
    win_sdk_ver = os.environ.get("WindowsSDKVersion", "10.0.22621.0").rstrip("\\")
    msvc_dir = os.environ.get("VCToolsInstallDir", r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207")

    for d in [
        f"{msvc_dir}\\include",
        f"{win_sdk}\\Include\\{win_sdk_ver}\\ucrt",
        f"{win_sdk}\\Include\\{win_sdk_ver}\\shared",
        f"{win_sdk}\\Include\\{win_sdk_ver}\\um",
    ]:
        if os.path.isdir(d):
            flags.append(f"-I{d}")

    return flags


def get_minimal_flags():
    """Fallback flags when no vcxproj is available."""
    return [
        "-x", "c++", "-std=c++20",
        "-fms-extensions", "-fms-compatibility",
        "-fdelayed-template-parsing",
        "-Wno-everything",
        "-DREFRAMEWORK_UNIVERSAL",
        "-DWIN32", "-D_WINDOWS", "-DNDEBUG",
        f"-I{REPO_ROOT / 'shared'}",
        f"-I{REPO_ROOT / 'include'}",
        f"-I{REPO_ROOT / 'src'}",
        f"-I{REPO_ROOT / 'dependencies'}",
    ]


def get_type_name(cursor):
    """Extract the unqualified struct/class name from a cursor's type."""
    t = cursor.type
    if t.kind == ci.TypeKind.POINTER:
        t = t.get_pointee()
    # Strip qualifiers and get the declaration
    decl = t.get_declaration()
    if decl.kind != ci.CursorKind.NO_DECL_FOUND:
        return decl.spelling
    # Fallback: parse from the type spelling
    spelling = t.spelling.replace("const ", "").replace("class ", "").replace("struct ", "").strip()
    if "::" in spelling:
        spelling = spelling.split("::")[-1]
    if spelling.endswith("*"):
        spelling = spelling[:-1].strip()
    return spelling


def scan_file(filepath, flags):
    """Parse a file with libclang and find guarded field accesses."""
    index = ci.Index.create()
    
    abs_path = str(REPO_ROOT / filepath) if not os.path.isabs(filepath) else filepath
    
    tu = index.parse(abs_path, args=flags,
                     options=ci.TranslationUnit.PARSE_INCOMPLETE)
    
    if tu is None:
        print(f"  FAILED TO PARSE: {filepath}", file=sys.stderr)
        return [], True

    # Count errors for reporting, but don't bail — partial ASTs are still useful.
    error_count = sum(1 for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error)

    violations = []

    def visit(cursor):
        # Only look at MemberRefExpr (->field or .field)
        if cursor.kind == ci.CursorKind.MEMBER_REF_EXPR:
            field_name = cursor.spelling
            if field_name in FIELD_TO_TYPES:
                # Check the type of the base expression
                # The base is the first child of a MemberRefExpr
                children = list(cursor.get_children())
                if children:
                    base_type_name = get_type_name(children[0])
                    if base_type_name in GUARDED_TYPES and field_name in GUARDED_TYPES[base_type_name]:
                        loc = cursor.location
                        if loc.file and not is_whitelisted(loc.file.name):
                            violations.append({
                                "file": os.path.relpath(loc.file.name, REPO_ROOT).replace("\\", "/"),
                                "line": loc.line,
                                "col": loc.column,
                                "type": base_type_name,
                                "field": field_name,
                            })

        for child in cursor.get_children():
            visit(child)

    visit(tu.cursor)
    return violations, error_count


def main():
    parser = argparse.ArgumentParser(description="libclang-based direct field access audit")
    parser.add_argument("files", nargs="*", help="Files to scan (default: high-value targets)")
    parser.add_argument("--json", action="store_true")
    parser.add_argument("--all", action="store_true", help="Scan all .cpp files in src/ and shared/sdk/")
    args = parser.parse_args()

    flags = get_compile_flags()

    if args.all:
        targets = []
        for d in ["src", "shared/sdk"]:
            for root, _, files in os.walk(REPO_ROOT / d):
                for f in files:
                    if f.endswith(".cpp"):
                        fp = os.path.relpath(os.path.join(root, f), REPO_ROOT)
                        if not is_whitelisted(fp):
                            targets.append(fp)
    elif args.files:
        targets = args.files
    else:
        targets = DEFAULT_TARGETS

    all_violations = []

    for filepath in sorted(targets):
        print(f"  Scanning {filepath}...", file=sys.stderr, end="", flush=True)
        violations, error_count = scan_file(filepath, flags)
        if error_count > 0:
            print(f" {len(violations)} violations ({error_count} parse errors)", file=sys.stderr)
        else:
            print(f" {len(violations)} violations", file=sys.stderr)
        all_violations.extend(violations)

    if args.json:
        print(json.dumps({
            "total": len(all_violations),
            "violations": all_violations,
        }, indent=2))
    else:
        by_file = defaultdict(list)
        for v in all_violations:
            by_file[v["file"]].append(v)

        print(f"\nTotal violations: {len(all_violations)}")
        print()
        for fp in sorted(by_file):
            vv = by_file[fp]
            print(f"--- {fp} ({len(vv)}) ---")
            for v in vv:
                print(f"  L{v['line']:>5}:{v['col']:<3}  {v['type']}::{v['field']}")
            print()

    return 1 if all_violations else 0


if __name__ == "__main__":
    sys.exit(main())
