#!/bin/bash
# Runs REFramework's in-game test suites against one Steam game on Linux (Proton), e.g. a Steam Deck.
# The Linux counterpart of Run-RFTests.ps1; needs bash, python3 and a running Steam client on the local display.
#
#   bash tests/run/run-rftests.sh -g <alias|appid> -b <dinput8.dll> [-s api,regression,plugin[,csharp]]
#                                 [-p <plugin dir>] [-c <csharp bin>] [-n <portable .NET dir>] [-o <out dir>] [-t <seconds>]
#
#   -g  game alias or Steam app id from tests/run/games.json
#   -b  REFramework build (dinput8.dll). Installed for the run; the game's own dinput8.dll (file or symlink) comes back.
#   -s  suites (default api,regression,plugin)
#   -p  folder with rftest_plugin.dll and rftest_loadlib.dll (default tests/plugin/out; build them on Windows with
#       tests/plugin/build.bat)
#   -c  csharp suite: folder with the C# API build output (REFramework.NET.dll, its runtimeconfig, Ijwhost.dll and the
#       managed dependencies)
#   -n  csharp suite: a portable .NET 10 runtime (the contents of C:\Program Files\dotnet from a Windows machine),
#       copied into the game's Proton prefix if it has no .NET, and removed afterwards
#   -o  results folder (default tests/out/<alias>-<time>)
#   -t  seconds to wait for results (default 900)
#
# The game must be installed and have run once under Proton (so its prefix exists). If the prefix has no
# "dinput8"="native,builtin" DLL override, one is added for the run and removed afterwards.
# Over SSH, run it detached from the session so a dropped connection doesn't kill it half way:
#   systemd-run --user --collect --same-dir --unit rftests bash tests/run/run-rftests.sh -g re2 -b ~/builds/dinput8.dll
# Exit code: 0 all suites passed, 1 failures, 2 usage or setup error.
set -u

TESTS=$(cd "$(dirname "$0")/.." && pwd)
GAMEARG="" BUILD="" SUITES_ARG="api,regression,plugin" PLUGINDIR="$TESTS/plugin/out" CSHARPBIN="" DOTNETDIR="" OUT="" TIMEOUT=900
while getopts "g:b:s:p:c:n:o:t:h" opt; do
  case $opt in
    g) GAMEARG=$OPTARG ;; b) BUILD=$OPTARG ;; s) SUITES_ARG=$OPTARG ;; p) PLUGINDIR=$OPTARG ;; c) CSHARPBIN=$OPTARG ;;
    n) DOTNETDIR=$OPTARG ;; o) OUT=$OPTARG ;; t) TIMEOUT=$OPTARG ;;
    *) sed -n '2,25p' "$0"; exit 2 ;;
  esac
done
die() { echo "error: $*" >&2; exit 2; }
[ -n "$GAMEARG" ] || die "-g <alias|appid> is required"
[ -n "$BUILD" ] && [ -f "$BUILD" ] || die "-b <dinput8.dll> is required and must exist"
BUILD=$(realpath "$BUILD")
command -v python3 >/dev/null || die "python3 not found"

# Steam root: native, legacy symlink, Flatpak.
STEAM=""
for d in "$HOME/.local/share/Steam" "$HOME/.steam/steam" "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam"; do
  [ -f "$d/steamapps/libraryfolders.vdf" ] && { STEAM=$(realpath "$d"); break; }
done
[ -n "$STEAM" ] || die "Steam not found"

# Game: alias/app id -> app id, exe, install folder (from games.json and the Steam library manifests).
read -r APPID EXE ALIAS GAME < <(python3 - "$TESTS/run/games.json" "$STEAM" "$GAMEARG" <<'EOF'
import json, os, re, sys
catalog, steam, arg = sys.argv[1:]
game = next((g for g in json.load(open(catalog))["games"] if g["alias"] == arg or str(g["appid"]) == arg), None)
if not game:
    sys.exit(f"unknown game {arg}")
libs = [steam] + re.findall(r'"path"\s+"([^"]+)"', open(os.path.join(steam, "steamapps", "libraryfolders.vdf")).read())
for lib in libs:
    manifest = os.path.join(lib, "steamapps", f"appmanifest_{game['appid']}.acf")
    if os.path.exists(manifest):
        installdir = re.search(r'"installdir"\s+"([^"]+)"', open(manifest).read()).group(1)
        print(game["appid"], game["exe"], game["alias"], os.path.join(lib, "steamapps", "common", installdir))
        sys.exit(0)
sys.exit(f"{game['name']} is not installed")
EOF
) || die "could not resolve $GAMEARG"
LIB=${GAME%/steamapps/common/*}
PFX="$LIB/steamapps/compatdata/$APPID/pfx"
[ -d "$PFX" ] || die "no Proton prefix at $PFX; start the game once through Steam first"
RF="$GAME/reframework"
LOG="$GAME/re2_framework_log.txt"
[ -n "$OUT" ] || OUT="$TESTS/out/$ALIAS-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT"
WORK="$OUT/.restore"
mkdir -p "$WORK"

IFS=',' read -r -a SUITE_LIST <<< "$SUITES_ARG"
has() { local s; for s in "${SUITE_LIST[@]}"; do [ "$s" = "$1" ] && return 0; done; return 1; }
EXPECTED=""
has api && EXPECTED="$EXPECTED api"
has regression && EXPECTED="$EXPECTED regression"
has plugin && EXPECTED="$EXPECTED plugin_api plugin_regression"
has csharp && EXPECTED="$EXPECTED csharp"
if has plugin; then [ -f "$PLUGINDIR/rftest_plugin.dll" ] || die "$PLUGINDIR/rftest_plugin.dll not found (-p)"; fi
if has csharp; then [ -f "$CSHARPBIN/REFramework.NET.dll" ] || die "the csharp suite needs -c <csharp bin>"; fi

game_pid() { pgrep -a -u "$(id -u)" -f "\\\\${EXE}\$" | grep -vi 'steam\.exe' | awk '{print $1; exit}'; }
proton_wine() {
  local p
  p=$(pgrep -a -u "$(id -u)" -f 'proton waitforexitandrun' | awk '$2 ~ /python/' | head -1 | sed 's/^[0-9]* [^ ]* //; s/ waitforexitandrun.*//')
  [ -n "$p" ] && echo "$(dirname "$p")/files/bin/wine"
}
# WM_CLOSE (or /f) through the game's own Proton wine.
wine_taskkill() {
  local w; w=$(proton_wine)
  [ -n "$w" ] && [ -x "$w" ] && WINEPREFIX=$PFX WINEDEBUG=-all timeout 60 "$w" taskkill "$@" /im "$EXE" >/dev/null 2>&1
}
wait_gone() { local i; for i in $(seq 1 "$1"); do [ -z "$(game_pid)" ] && return 0; sleep 1; done; return 1; }
# Steam starts one game at a time; wait until no Proton session is left. Fails after 120 s.
wait_idle() { local i; for i in $(seq 1 120); do pgrep -u "$(id -u)" -f 'proton waitforexitandrun|steam-launch-wrapper' >/dev/null || return 0; sleep 1; done; return 1; }
launch() {
  export DISPLAY=${DISPLAY:-:0}
  if command -v steam >/dev/null; then timeout 20 steam "steam://rungameid/$APPID" >/dev/null 2>&1 &
  else timeout 20 flatpak run com.valvesoftware.Steam "steam://rungameid/$APPID" >/dev/null 2>&1 & fi
}
suites_done() {
  python3 - "$RF/data/rftest/results" $EXPECTED <<'EOF'
import json, os, sys
d = sys.argv[1]
for s in sys.argv[2:]:
    try:
        r = json.load(open(os.path.join(d, s + ".json")))
    except Exception:
        sys.exit(1)
    # The plugin pre-records its deadlock tests as "pending" before running them.
    if any(str(t.get("detail", "")).startswith("pending:") for t in r["tests"]):
        sys.exit(1)
EOF
}
# Adds/removes "dinput8"="native,builtin" in the prefix's DLL overrides. Prints "added" when it added one.
dll_override() {
  python3 - "$PFX/user.reg" "$1" <<'EOF'
import re, sys, time
path, action = sys.argv[1:]
text = open(path, encoding="utf-8", errors="surrogateescape").read()
section = "[Software\\\\Wine\\\\DllOverrides]"
line = '"dinput8"="native,builtin"'
if action == "add":
    if re.search(r'^"\*?dinput8"="native', text, re.M):
        sys.exit(0)
    if section in text:
        i = text.index("\n", text.index(section)) + 1
        if text[i:].startswith("#time="):
            i = text.index("\n", i) + 1
        text = text[:i] + line + "\n" + text[i:]
    else:
        text += f"\n{section} {int(time.time())}\n{line}\n"
    print("added")
else:
    text = text.replace(line + "\n", "", 1)
open(path, "w", encoding="utf-8", errors="surrogateescape").write(text)
EOF
}

# Files REFramework writes next to the game, besides reframework/.
ROOT_FILES="re2_fw_config.txt re2_framework_log.txt re2_framework_log.prev.txt ref_ui.ini reframework_accessed_files.txt
  reframework_loose_files.txt reframework_faulty_files.txt reframework_crash.dmp"

restore() {
  # Everything under reframework/ that wasn't there before: the suites, REFramework.NET, generated assemblies, fonts...
  if [ -d "$RF" ]; then
    find "$RF" -depth -print | while IFS= read -r p; do grep -Fxq -- "$p" "$WORK/rf-before" || rm -rf -- "$p"; done
  fi
  for f in $ROOT_FILES; do
    if [ -f "$WORK/$f" ]; then cp -p "$WORK/$f" "$GAME/$f"; else rm -f "$GAME/$f"; fi
  done
  [ -n "$dotnet_installed" ] && rm -rf "$DOTNET"
  [ -n "$override_added" ] && dll_override remove
  rm -f "$GAME/dinput8.dll"
  if [ -n "$old_link" ]; then ln -s "$old_link" "$GAME/dinput8.dll"
  elif [ -f "$WORK/dinput8.dll" ]; then mv "$WORK/dinput8.dll" "$GAME/dinput8.dll"; fi
  rm -rf "$WORK"
}

[ -z "$(game_pid)" ] || die "the game is already running; close it first"
# With another game still running, Steam would never start this one.
wait_idle || die "another game is still running; close it first"

# Install (everything the game had is saved in $WORK and put back by restore).
old_link="" dotnet_installed="" override_added=""
DOTNET="$PFX/drive_c/Program Files/dotnet"
find "$RF" -print 2>/dev/null > "$WORK/rf-before"
for f in $ROOT_FILES; do [ -f "$GAME/$f" ] && cp -p "$GAME/$f" "$WORK/$f"; done
old_link=$(readlink "$GAME/dinput8.dll" || true)
[ -z "$old_link" ] && [ -f "$GAME/dinput8.dll" ] && mv "$GAME/dinput8.dll" "$WORK/dinput8.dll"
trap restore EXIT
rm -f "$GAME/dinput8.dll"
cp "$BUILD" "$GAME/dinput8.dll"
override_added=$(dll_override add)
mkdir -p "$RF/autorun" "$RF/data" "$RF/plugins"
rm -rf "$RF/data/rftest"
cp -r "$TESTS/lua/rftest" "$RF/autorun/"
has api && cp "$TESTS/lua/rftest_api.lua" "$RF/autorun/"
if has regression; then
  cp "$TESTS/lua/rftest_regression.lua" "$RF/autorun/"
  if [ -f "$PLUGINDIR/rftest_loadlib.dll" ]; then cp "$PLUGINDIR/rftest_loadlib.dll" "$RF/data/rftest_loadlib.dll"
  else echo "warning: $PLUGINDIR/rftest_loadlib.dll not found; the package.loadlib test will SKIP"; fi
fi
has plugin && cp "$PLUGINDIR/rftest_plugin.dll" "$RF/plugins/rftest_plugin.dll"
if has csharp; then
  mkdir -p "$RF/plugins/source"
  if [ ! -f "$RF/plugins/REFramework.NET.dll" ]; then
    mkdir -p "$RF/plugins/managed/dependencies"
    cp "$CSHARPBIN/REFramework.NET.dll" "$CSHARPBIN/REFramework.NET.runtimeconfig.json" "$CSHARPBIN/Ijwhost.dll" "$RF/plugins/"
    for f in AssemblyGenerator.dll REFCoreDeps.dll Microsoft.CodeAnalysis.CSharp.dll Microsoft.CodeAnalysis.dll Hexa.NET.ImGui.dll HexaGen.Runtime.dll; do
      cp "$CSHARPBIN/$f" "$RF/plugins/managed/dependencies/"
    done
  fi
  cp "$TESTS/csharp/rftest_csharp.cs" "$RF/plugins/source/"
  if [ ! -d "$DOTNET/shared/Microsoft.NETCore.App" ]; then
    [ -n "$DOTNETDIR" ] && [ -d "$DOTNETDIR/shared/Microsoft.NETCore.App" ] || die "the prefix has no .NET; pass -n <portable .NET 10 dir>"
    mkdir -p "$DOTNET"; cp -r "$DOTNETDIR/." "$DOTNET/"; dotnet_installed=1
  fi
fi
echo "installed build and suites ($EXPECTED ) into $GAME"

# Run
t0=$(date +%s)
launch
# Proton can take minutes to start a game the first time (prefix updates, shader processing).
pid=""; for _ in $(seq 1 600); do pid=$(game_pid); [ -n "$pid" ] && break; sleep 1; done
result=TIMEOUT
if [ -z "$pid" ]; then
  result=NO_PROCESS
  # Steam may still start it: don't leave it running against restored files.
  for _ in $(seq 1 60); do
    if [ -n "$(game_pid)" ]; then echo "game started after the runner gave up; killing it"; kill -9 "$(game_pid)" 2>/dev/null; wait_gone 20; break; fi
    sleep 1
  done
else
  echo "game started (pid $pid)"
  for _ in $(seq 1 "$TIMEOUT"); do
    if suites_done; then result=DONE; break; fi
    [ -z "$(game_pid)" ] && { result=EXITED; break; }
    sleep 1
  done
  sleep 5
  if [ -z "$(game_pid)" ]; then echo "game already exited"
  elif wine_taskkill && wait_gone 60; then echo "game closed (WM_CLOSE)"
  elif wine_taskkill /f && wait_gone 20; then echo "game did not close; terminated it"
  else echo "game did not close; killed it"; kill -9 "$(game_pid)" 2>/dev/null; wait_gone 20; fi
fi
secs=$(( $(date +%s) - t0 ))

# Collect
cp "$RF"/data/rftest/results/*.json "$OUT/" 2>/dev/null
grep -E '\[rftest\]|\[error\]' "$LOG" > "$OUT/log-excerpt.txt" 2>/dev/null
resets=$(grep -c '\] Reset!$' "$LOG" 2>/dev/null); resets=${resets:-0}
echo "device resets: $resets" >> "$OUT/log-excerpt.txt"
python3 - "$OUT" "$ALIAS" "$result" "$secs" "$resets" $EXPECTED <<'EOF'
import json, os, sys
out, alias, result, secs, resets = sys.argv[1:6]
rows, failed, text = [], [], [f"{alias}: {result} after {secs} s"]
for s in sys.argv[6:]:
    try:
        r = json.load(open(os.path.join(out, s + ".json")))
    except Exception:
        rows.append({"Suite": s, "Pass": 0, "Fail": 1, "Skip": 0, "Note": "no results"})
        failed.append(f"{s}: no results file"); text.append(f"{s:18} no results"); continue
    st = [t["status"] for t in r["tests"]]
    rows.append({"Suite": s, "Pass": st.count("PASS"), "Fail": st.count("FAIL"), "Skip": st.count("SKIP"), "Note": r.get("game", "")})
    text.append(f"{s:18} {st.count('PASS'):3} pass {st.count('FAIL'):3} fail {st.count('SKIP'):3} skip  {r.get('game', '')}")
    for t in r["tests"]:
        if t["status"] == "FAIL":
            failed.append(f"{s}: {t['name']} -- {t['detail']}"); text.append(f"  FAIL {t['name']} -- {t['detail'][:200]}")
if result != "DONE":
    # The game never started, exited early, or the suites didn't finish in time (e.g. a deadlock).
    failed.append(f"run: {result}")
open(os.path.join(out, "summary.txt"), "w").write("\n".join(text) + "\n")
json.dump({"game": alias, "result": result, "seconds": int(secs), "suites": rows, "failed": failed, "deviceResets": int(resets)},
          open(os.path.join(out, "summary.json"), "w"), indent=2)
print("\n".join(text))
EOF
echo "results: $OUT"
python3 -c 'import json,sys; sys.exit(1 if json.load(open(sys.argv[1]))["failed"] else 0)' "$OUT/summary.json"
