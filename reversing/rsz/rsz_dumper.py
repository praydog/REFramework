"""
RSZ Dumper GUI/CLI — unified launcher for emulation-dumper.py and non-native-dumper.py.

GUI:  python rsz_dumper.py
CLI:  python rsz_dumper.py --cli --exe re9_dump_v4.exe
      python rsz_dumper.py --cli --exe re9_dump_v4.exe --step both
      python rsz_dumper.py --cli --exe re9_dump_v4.exe --step 1
      python rsz_dumper.py --cli --help
"""

import argparse
import tempfile
import os
import subprocess
import sys
import threading
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent

# --- Defaults (matching the old .bat files) ---
DEFAULT_IL2CPP = "il2cpp_dump.json"
DEFAULT_POSTFIX = "re9"
DEFAULT_USE_TYPEDEFS = False
DEFAULT_USE_HASHKEYS = True
DEFAULT_INCLUDE_PARENTS = False
DEFAULT_UNPACK_STRUCT = True
DEFAULT_TEST_MODE = False


def discover_dump_exes(directory: Path) -> list[str]:
    """Find re9_dump*.exe files in the working directory, sorted newest first."""
    matches = sorted(
        directory.glob("re9_dump*.exe"),
        key=lambda p: p.stat().st_mtime,
        reverse=True,
    )
    return [p.name for p in matches]


def natives_path_for(exe_name: str) -> str:
    """Derive the native_layouts JSON filename that emulation-dumper.py produces."""
    return f"native_layouts_{exe_name}.json"


def build_part1_cmd(
    exe_path: str,
    il2cpp_path: str = DEFAULT_IL2CPP,
    test_mode: bool = DEFAULT_TEST_MODE,
) -> list[str]:
    return [
        sys.executable,
        str(SCRIPT_DIR / "emulation-dumper.py"),
        f"--p={exe_path}",
        f"--il2cpp_path={il2cpp_path}",
        f"--test_mode={test_mode}",
    ]


def build_part2_cmd(
    exe_name: str,
    il2cpp_path: str = DEFAULT_IL2CPP,
    out_postfix: str = DEFAULT_POSTFIX,
    use_typedefs: bool = DEFAULT_USE_TYPEDEFS,
    use_hashkeys: bool = DEFAULT_USE_HASHKEYS,
    include_parents: bool = DEFAULT_INCLUDE_PARENTS,
    unpack_struct: bool = DEFAULT_UNPACK_STRUCT,
) -> list[str]:
    return [
        sys.executable,
        str(SCRIPT_DIR / "non-native-dumper.py"),
        f"--out_postfix={out_postfix}",
        f"--natives_path={natives_path_for(exe_name)}",
        f"--il2cpp_path={il2cpp_path}",
        f"--use_typedefs={use_typedefs}",
        f"--use_hashkeys={use_hashkeys}",
        f"--include_parents={include_parents}",
        f"--unpack_struct={unpack_struct}",
    ]


# ---------------------------------------------------------------------------
# CLI mode
# ---------------------------------------------------------------------------

def run_cli(args: argparse.Namespace) -> int:
    cwd = str(SCRIPT_DIR)
    step = args.step
    exe = args.exe
    il2cpp = args.il2cpp
    postfix = args.postfix

    if step in ("1", "both"):
        cmd = build_part1_cmd(exe, il2cpp, args.test_mode)
        print(f"[Part 1] {' '.join(cmd)}")
        rc = subprocess.call(cmd, cwd=cwd)
        if rc != 0:
            print(f"[Part 1] FAILED (exit {rc})")
            return rc
        print("[Part 1] Done.")

    if step in ("2", "both"):
        exe_name = os.path.basename(exe)
        cmd = build_part2_cmd(
            exe_name, il2cpp, postfix,
            args.use_typedefs, args.use_hashkeys,
            args.include_parents, args.unpack_struct,
        )
        print(f"[Part 2] {' '.join(cmd)}")
        rc = subprocess.call(cmd, cwd=cwd)
        if rc != 0:
            print(f"[Part 2] FAILED (exit {rc})")
            return rc
        print("[Part 2] Done.")

    return 0


# ---------------------------------------------------------------------------
# GUI mode
# ---------------------------------------------------------------------------

def run_gui():
    import tkinter as tk
    from tkinter import filedialog, scrolledtext, ttk

    root = tk.Tk()
    root.title("RSZ Dumper")
    root.resizable(True, True)
    root.minsize(680, 560)

    # --- State ---
    dump_exes = discover_dump_exes(SCRIPT_DIR)
    running = threading.Event()  # set while a subprocess is active

    # --- Style ---
    style = ttk.Style()
    style.configure("Run.TButton", padding=6)

    pad = dict(padx=6, pady=3)

    # ===== Paths frame =====
    paths_frame = ttk.LabelFrame(root, text="Paths", padding=8)
    paths_frame.pack(fill="x", **pad)

    # Dump exe
    ttk.Label(paths_frame, text="Dump EXE:").grid(row=0, column=0, sticky="w")
    exe_var = tk.StringVar(value=dump_exes[0] if dump_exes else "")
    exe_combo = ttk.Combobox(paths_frame, textvariable=exe_var, values=dump_exes, width=40)
    exe_combo.grid(row=0, column=1, sticky="ew", padx=4)

    def browse_exe():
        path = filedialog.askopenfilename(
            initialdir=str(SCRIPT_DIR),
            title="Select dump EXE",
            filetypes=[("Executables", "*.exe"), ("All files", "*.*")],
        )
        if path:
            exe_var.set(path)

    ttk.Button(paths_frame, text="Browse…", command=browse_exe).grid(row=0, column=2)

    # il2cpp dump
    ttk.Label(paths_frame, text="il2cpp JSON:").grid(row=1, column=0, sticky="w")
    il2cpp_var = tk.StringVar(value=DEFAULT_IL2CPP)
    ttk.Entry(paths_frame, textvariable=il2cpp_var, width=42).grid(row=1, column=1, sticky="ew", padx=4)

    def browse_il2cpp():
        path = filedialog.askopenfilename(
            initialdir=str(SCRIPT_DIR),
            title="Select il2cpp_dump.json",
            filetypes=[("JSON", "*.json"), ("All files", "*.*")],
        )
        if path:
            il2cpp_var.set(path)

    ttk.Button(paths_frame, text="Browse…", command=browse_il2cpp).grid(row=1, column=2)

    # Output postfix
    ttk.Label(paths_frame, text="Output postfix:").grid(row=2, column=0, sticky="w")
    postfix_var = tk.StringVar(value=DEFAULT_POSTFIX)
    ttk.Entry(paths_frame, textvariable=postfix_var, width=42).grid(row=2, column=1, sticky="ew", padx=4)

    paths_frame.columnconfigure(1, weight=1)

    # ===== Derived paths (read-only info) =====
    info_frame = ttk.LabelFrame(root, text="Derived Paths (auto-computed)", padding=8)
    info_frame.pack(fill="x", **pad)

    natives_label = ttk.Label(info_frame, text="", foreground="gray")
    natives_label.pack(anchor="w")
    output_label = ttk.Label(info_frame, text="", foreground="gray")
    output_label.pack(anchor="w")

    def update_derived(*_args):
        exe_name = os.path.basename(exe_var.get())
        natives_label.config(text=f"Part 1 output → native_layouts_{exe_name}.json")
        pf = postfix_var.get()
        output_label.config(text=f"Part 2 output → rsz{pf}.json / rsz{pf}.txt")

    exe_var.trace_add("write", update_derived)
    postfix_var.trace_add("write", update_derived)
    update_derived()

    # ===== Options frame =====
    opts_frame = ttk.LabelFrame(root, text="Options (Part 2)", padding=8)
    opts_frame.pack(fill="x", **pad)

    use_hashkeys_var = tk.BooleanVar(value=DEFAULT_USE_HASHKEYS)
    use_typedefs_var = tk.BooleanVar(value=DEFAULT_USE_TYPEDEFS)
    include_parents_var = tk.BooleanVar(value=DEFAULT_INCLUDE_PARENTS)
    unpack_struct_var = tk.BooleanVar(value=DEFAULT_UNPACK_STRUCT)
    test_mode_var = tk.BooleanVar(value=DEFAULT_TEST_MODE)

    ttk.Checkbutton(opts_frame, text="use_hashkeys", variable=use_hashkeys_var).grid(row=0, column=0, sticky="w")
    ttk.Checkbutton(opts_frame, text="use_typedefs", variable=use_typedefs_var).grid(row=0, column=1, sticky="w")
    ttk.Checkbutton(opts_frame, text="include_parents", variable=include_parents_var).grid(row=1, column=0, sticky="w")
    ttk.Checkbutton(opts_frame, text="unpack_struct", variable=unpack_struct_var).grid(row=1, column=1, sticky="w")
    ttk.Checkbutton(opts_frame, text="test_mode (Part 1)", variable=test_mode_var).grid(row=2, column=0, sticky="w")

    # ===== Run buttons =====
    btn_frame = ttk.Frame(root, padding=4)
    btn_frame.pack(fill="x", **pad)

    def make_runner(steps: str):
        """Return a callback that runs the given step(s) in a background thread."""
        def callback():
            if running.is_set():
                return
            running.set()
            for btn in (btn_part1, btn_part2, btn_both):
                btn.config(state="disabled")
            output_text.delete("1.0", tk.END)
            threading.Thread(target=_run_steps, args=(steps,), daemon=True).start()
        return callback

    # --- File-polling log viewer ---
    # Instead of piping stdout (which forces buffering overhead and
    # per-line cross-thread round-trips), the child writes to a temp
    # file and a tkinter after() timer tails it into the widget.
    _log_file = None      # open file handle for polling
    _log_path = None      # path for cleanup
    _poll_id = None       # after() id so we can cancel

    def _poll_log():
        nonlocal _poll_id
        if _log_file is not None:
            chunk = _log_file.read()
            if chunk:
                output_text.insert(tk.END, chunk)
                output_text.see(tk.END)
        if running.is_set():
            _poll_id = root.after(100, _poll_log)
        else:
            # Final drain after process finishes
            if _log_file is not None:
                chunk = _log_file.read()
                if chunk:
                    output_text.insert(tk.END, chunk)
                    output_text.see(tk.END)
            _cleanup_log()

    def _cleanup_log():
        nonlocal _log_file, _log_path
        if _log_file is not None:
            _log_file.close()
            _log_file = None
        if _log_path is not None:
            try:
                os.unlink(_log_path)
            except OSError:
                pass
            _log_path = None

    def _run_steps(steps: str):
        nonlocal _log_file, _log_path
        cwd = str(SCRIPT_DIR)
        exe = exe_var.get()
        il2cpp = il2cpp_var.get()
        exe_name = os.path.basename(exe)

        # Create a temp file for combined output of all steps.
        fd, _log_path = tempfile.mkstemp(prefix="rsz_", suffix=".log", dir=cwd)
        log_write = os.fdopen(fd, "w")
        _log_file = open(_log_path, "r", encoding="utf-8", errors="replace")

        # Start the GUI poll timer from the main thread.
        root.after(0, _poll_log)

        def run_one(label: str, cmd: list[str]) -> int:
            log_write.write(f"[{label}] {' '.join(cmd)}\n")
            log_write.flush()
            proc = subprocess.Popen(
                cmd, cwd=cwd,
                stdout=log_write, stderr=subprocess.STDOUT,
            )
            proc.wait()
            if proc.returncode != 0:
                log_write.write(f"\n[{label}] FAILED (exit {proc.returncode})\n")
            else:
                log_write.write(f"\n[{label}] Done.\n")
            log_write.flush()
            return proc.returncode

        ok = True

        if steps in ("1", "both"):
            cmd = build_part1_cmd(exe, il2cpp, test_mode_var.get())
            if run_one("Part 1", cmd) != 0:
                ok = False

        if ok and steps in ("2", "both"):
            cmd = build_part2_cmd(
                exe_name, il2cpp, postfix_var.get(),
                use_typedefs_var.get(), use_hashkeys_var.get(),
                include_parents_var.get(), unpack_struct_var.get(),
            )
            run_one("Part 2", cmd)

        log_write.close()
        running.clear()
        for btn in (btn_part1, btn_part2, btn_both):
            btn.after(0, lambda b=btn: b.config(state="normal"))

    btn_part1 = ttk.Button(btn_frame, text="Run Part 1 Only", style="Run.TButton", command=make_runner("1"))
    btn_part1.pack(side="left", padx=4)

    btn_part2 = ttk.Button(btn_frame, text="Run Part 2 Only", style="Run.TButton", command=make_runner("2"))
    btn_part2.pack(side="left", padx=4)

    btn_both = ttk.Button(btn_frame, text="Run Both (1 → 2)", style="Run.TButton", command=make_runner("both"))
    btn_both.pack(side="left", padx=4)

    # ===== Output log =====
    log_frame = ttk.LabelFrame(root, text="Output", padding=4)
    log_frame.pack(fill="both", expand=True, **pad)

    output_text = scrolledtext.ScrolledText(log_frame, height=12, wrap="word", font=("Consolas", 9))
    output_text.pack(fill="both", expand=True)

    root.mainloop()


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="RSZ Dumper — GUI/CLI launcher for emulation-dumper + non-native-dumper.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""\
examples:
  python rsz_dumper.py                              # launch GUI
  python rsz_dumper.py --cli --exe re9_dump_v5.exe  # run both steps via CLI
  python rsz_dumper.py --cli --exe re9_dump_v5.exe --step 1
  python rsz_dumper.py --cli --exe re9_dump_v5.exe --step 2 --postfix re9
""",
    )
    parser.add_argument("--cli", action="store_true", help="Run in CLI mode (no GUI)")
    parser.add_argument("--exe", type=str, help="Path to the dump EXE (required in CLI mode)")
    parser.add_argument("--il2cpp", type=str, default=DEFAULT_IL2CPP, help=f"Path to il2cpp dump JSON (default: {DEFAULT_IL2CPP})")
    parser.add_argument("--postfix", type=str, default=DEFAULT_POSTFIX, help=f"Output postfix for rsz files (default: {DEFAULT_POSTFIX})")
    parser.add_argument("--step", choices=["1", "2", "both"], default="both", help="Which step(s) to run (default: both)")
    parser.add_argument("--test-mode", action="store_true", default=DEFAULT_TEST_MODE, help="Enable test_mode for Part 1")
    parser.add_argument("--use-typedefs", action="store_true", default=DEFAULT_USE_TYPEDEFS, help="Part 2: use_typedefs")
    parser.add_argument("--use-hashkeys", action=argparse.BooleanOptionalAction, default=DEFAULT_USE_HASHKEYS, help="Part 2: use_hashkeys (default: True)")
    parser.add_argument("--include-parents", action="store_true", default=DEFAULT_INCLUDE_PARENTS, help="Part 2: include_parents")
    parser.add_argument("--unpack-struct", action=argparse.BooleanOptionalAction, default=DEFAULT_UNPACK_STRUCT, help="Part 2: unpack_struct (default: True)")

    args = parser.parse_args()

    if args.cli:
        if not args.exe:
            parser.error("--exe is required in CLI mode")
        sys.exit(run_cli(args))
    else:
        run_gui()


if __name__ == "__main__":
    main()
