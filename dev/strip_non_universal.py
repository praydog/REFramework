"""
Strip non-universal preprocessor paths from REFramework source files.

For each #ifdef REFRAMEWORK_UNIVERSAL ... #else/#elif ... #endif:
  - Keep only the REFRAMEWORK_UNIVERSAL body
  - Remove the #ifdef line, all #else/#elif bodies, and the #endif line
  - EXCEPTION: if the dead body references TDB49/tdb49, keep the entire block

For standalone #if TDB_VER <cond> ... #endif (not inside UNIVERSAL blocks):
  - If condition is always true for TDB_VER=84: keep body, remove #if/#else/#endif
  - If condition is always false: remove body, keep #else body if present
  - EXCEPTION: if block references TDB49, keep as-is

For #ifndef REFRAMEWORK_UNIVERSAL ... #endif:
  - Remove entirely (dead code)
  - EXCEPTION: TDB49 references

Usage: python scripts/strip_non_universal.py [--dry-run] file1.cpp [file2.hpp ...]
"""
import re
import sys
import os


def evaluate_tdb_condition(line):
    """If line is '#if TDB_VER <op> <num>', return True/False/None for TDB_VER=84."""
    m = re.match(r'\s*#if\s+TDB_VER\s*(>=|>|<=|<|==|!=)\s*(\d+)\s*$', line)
    if not m:
        return None
    op, val = m.group(1), int(m.group(2))
    ops = {'>': 84 > val, '>=': 84 >= val, '<': 84 < val,
           '<=': 84 <= val, '==': 84 == val, '!=': 84 != val}
    return ops[op]


def has_tdb49_ref(lines):
    for line in lines:
        if 'TDB49' in line or 'tdb49' in line or 'TDB_VER <= 49' in line or 'TDB_VER == 49' in line or 'TDB_VER < 50' in line:
            return True
    return False


def find_block_end(lines, start):
    """From start (line after the opening #if), find matching #else/#elif (depth 0) and #endif.
    Returns (else_start_or_none, endif_line_index).
    else_start is the index of the first #else/#elif at depth 0, or None.
    """
    depth = 1
    first_else = None
    i = start
    while i < len(lines):
        s = lines[i].strip()
        if s.startswith('#if') or s.startswith('#ifdef') or s.startswith('#ifndef'):
            depth += 1
        elif s.startswith('#endif'):
            depth -= 1
            if depth == 0:
                return (first_else, i)
        elif (s.startswith('#else') or s.startswith('#elif')) and depth == 1:
            if first_else is None:
                first_else = i
        i += 1
    # Malformed — no matching endif
    return (first_else, len(lines) - 1)


def process_file(filepath, dry_run=False):
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    result = []
    i = 0
    changes = 0

    while i < len(lines):
        s = lines[i].strip()

        # --- #ifdef REFRAMEWORK_UNIVERSAL ---
        if s == '#ifdef REFRAMEWORK_UNIVERSAL':
            first_else, endif_idx = find_block_end(lines, i + 1)

            if first_else is None:
                # No else — just universal body
                universal_body = lines[i+1:endif_idx]
                dead_body = []
            else:
                universal_body = lines[i+1:first_else]
                dead_body = lines[first_else+1:endif_idx]

            # Check TDB49 exception
            all_dead = lines[first_else:endif_idx+1] if first_else else []
            if has_tdb49_ref(all_dead):
                # Keep entire block as-is
                result.extend(lines[i:endif_idx+1])
            else:
                # Keep only universal body
                result.extend(universal_body)
                changes += 1

            i = endif_idx + 1
            continue

        # --- #ifndef REFRAMEWORK_UNIVERSAL ---
        if s == '#ifndef REFRAMEWORK_UNIVERSAL':
            _, endif_idx = find_block_end(lines, i + 1)
            dead_body = lines[i+1:endif_idx]

            if has_tdb49_ref(dead_body):
                result.extend(lines[i:endif_idx+1])
            else:
                changes += 1

            i = endif_idx + 1
            continue

        # --- #if TDB_VER <cond> ---
        truth = evaluate_tdb_condition(s)
        if truth is not None:
            first_else, endif_idx = find_block_end(lines, i + 1)

            if first_else is None:
                if_body = lines[i+1:endif_idx]
                else_body = []
            else:
                if_body = lines[i+1:first_else]
                else_body = lines[first_else+1:endif_idx]

            all_in_block = lines[i:endif_idx+1]
            if has_tdb49_ref(all_in_block):
                result.extend(all_in_block)
            elif truth:
                result.extend(if_body)
                changes += 1
            else:
                result.extend(else_body)
                changes += 1

            i = endif_idx + 1
            continue

        # --- Orphaned #elif (left behind after stripping #ifdef REFRAMEWORK_UNIVERSAL) ---
        if s.startswith('#elif'):
            # Find matching #endif at this level
            depth = 0
            j = i + 1
            while j < len(lines):
                sj = lines[j].strip()
                if sj.startswith('#if') or sj.startswith('#ifdef') or sj.startswith('#ifndef'):
                    depth += 1
                elif sj.startswith('#endif'):
                    if depth == 0:
                        break
                    depth -= 1
                j += 1
            dead_block = lines[i:j+1]
            if has_tdb49_ref(dead_block):
                result.extend(dead_block)
            else:
                changes += 1
            i = j + 1
            continue

        # --- Default: pass through ---
        result.append(lines[i])
        i += 1

    if changes > 0:
        if not dry_run:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.writelines(result)

    return changes


def main():
    dry_run = '--dry-run' in sys.argv
    args = [a for a in sys.argv[1:] if a != '--dry-run']

    if not args:
        print("Usage: python strip_non_universal.py [--dry-run] file1.cpp ...")
        return 1

    total = 0
    for fp in args:
        if not os.path.isfile(fp):
            print(f"  SKIP {fp}")
            continue
        n = process_file(fp, dry_run)
        if n:
            print(f"  {n:>3} blocks  {fp}")
        total += n

    action = "would strip" if dry_run else "stripped"
    print(f"\nTotal: {total} blocks {action}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
