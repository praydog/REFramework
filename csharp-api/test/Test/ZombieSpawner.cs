// ZombieSpawner.cs v24 — Typed Proxy Conversion
// Uses REFramework typed proxies for compile-time safety on method signatures.

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Hexa.NET.ImGui;
using REFrameworkNET;
using REFrameworkNET.Attributes;
using REFrameworkNET.Callbacks;

public class ZombieSpawner {
    // -- Spawn tracking --
    struct TrackedZombie {
        public app.CharacterContext Context;
        public int Frame;
        public bool HitSetupDone;  // deferred: re-setup HitController after montage loads
    }

    static List<TrackedZombie> s_tracked = new List<TrackedZombie>();
    static app.PlayerContext s_playerCtx;
    static via.Folder s_playerFolder;

    // -- UI state --
    static List<string> s_availableKinds = new List<string>();
    static bool s_showWindow = true;
    static string s_lastStatus = "Ready";
    static string s_poolDiag = "";
    static int s_cloneCount = 0;

    // Timestamp to throttle scans
    static int s_scanFrame = 999;
    static string s_lastScanResult = "";

    // -- Clone + deferred spawn state machine --
    enum ClonePhase { Idle, Deactivated, WaitReady }
    static ClonePhase s_clonePhase = ClonePhase.Idle;
    static via.Folder s_cloneFolder;
    static int s_cloneFrame;
    static string s_cloneKind;
    static bool s_spawnAfterClone;

    // -- Cross-chapter loading state --
    enum XChapterPhase { Idle, Deactivated, WaitReady, Done }
    static XChapterPhase s_xchapterPhase = XChapterPhase.Idle;
    static via.Folder s_xchapterFolder;
    static int s_xchapterFrame;
    static string s_xchapterOrigPath;
    static string s_xchapterTargetPath;
    static string s_xchapterInput = "Chap3_02";
    static string s_xchapterScenePaths = "";
    static IntPtr s_allocatedStrBuf = IntPtr.Zero;
    static List<IntPtr> s_allocatedStrBufs = new List<IntPtr>();
    static List<via.Folder> s_createdFolders = new List<via.Folder>();

    // -- via.Folder native offsets --
    const int OFF_SCENE_PATH_PTR  = 0x38;
    const int OFF_SCENE_PATH_SIZE = 0x50;
    const int OFF_SCENE_PATH_CAP  = 0x54;
    const int OFF_SCENE_PTR       = 0x128;
    const int OFF_PARENT          = 0x70;
    const int OFF_CHILD           = 0x78;
    const int OFF_NEXT            = 0x80;

    // ═══════════ Helpers: typed proxy wrappers ═══════════

    /// Resolve a kind name string (e.g. "cp_B002") to the CharacterKindID static field.
    /// Returns null if not found.
    static app.CharacterKindID ResolveKindID(string kindName) {
        return ((app.CharacterKindID.REFType.Statics as IObject)?.GetField(kindName) as IObject).As<app.CharacterKindID>();
    }

    /// Check if a MontageID ManagedObject is valid (not -1/-1 and not 0/0 default)
    static bool IsValidMontageID(app.MontageID mid) {
        if (mid == null) return false;
        try {
            return mid.ModelID >= 0 && mid.PresetID >= 0;
        } catch { return false; }
    }

    /// Log MontageID values for diagnostics
    static string FormatMontageID(app.MontageID mid) {
        if (mid == null) return "{null}";
        try {
            return $"{{Model={mid.ModelID}, Preset={mid.PresetID}}}";
        } catch { return "{err}"; }
    }

    static T AsProxy<T>(ManagedObject obj) where T : class {
        return obj?.As<T>();
    }

    static bool SameObject(IObject left, IObject right) {
        return left != null && right != null && left.GetAddress() == right.GetAddress();
    }

    static ManagedObject ToManaged(IObject obj) {
        return obj as ManagedObject;
    }

    static string KindToString(object kind) {
        return kind?.ToString() ?? "";
    }

    // Extract kind prefix from GO name: "cp_B000" from "cp_B000" or "cp_B000_01"
    static string ExtractKindFromGOName(string goName) {
        if (string.IsNullOrEmpty(goName)) return "";
        // GO names are like "cp_B000" or "cp_B000_01" (cloned). Extract the cp_XXXX part.
        // Pattern: cp_ followed by a letter and digits
        if (!goName.StartsWith("cp_")) return "";
        // Find end of the kind prefix: cp_X### (letter + digits)
        int end = 3; // past "cp_"
        if (end < goName.Length && char.IsLetter(goName[end])) end++;
        while (end < goName.Length && char.IsDigit(goName[end])) end++;
        return goName.Substring(0, end);
    }

    static via.Quaternion GetPlayerRotation() {
        try {
            var playerTransform = s_playerCtx?.GameObject?.Transform;
            if (playerTransform != null)
                return playerTransform.Rotation;
        } catch { }

        return null;
    }

    [PluginEntryPoint]
    public static void Main() {
        API.LogInfo("[ZS] ========== Zombie Spawn v24 (Typed Proxies) ==========");
    }

    [PluginExitPoint]
    public static void OnUnload() {
        s_tracked.Clear();
        s_playerCtx = null;
        s_playerFolder = null;
        s_clonePhase = ClonePhase.Idle;
        s_cloneFolder = null;
        s_xchapterPhase = XChapterPhase.Idle;
        s_xchapterFolder = null;
        if (s_allocatedStrBuf != IntPtr.Zero) {
            Marshal.FreeHGlobal(s_allocatedStrBuf);
            s_allocatedStrBuf = IntPtr.Zero;
        }
        foreach (var buf in s_allocatedStrBufs)
            Marshal.FreeHGlobal(buf);
        s_allocatedStrBufs.Clear();
        s_createdFolders.Clear();
    }

    // ═══════════ ImGui UI ═══════════

    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void OnImGui() {
        if (!s_showWindow) return;
        if (!ImGui.Begin("Zombie Spawner v24", ref s_showWindow)) {
            ImGui.End();
            return;
        }

        try {
            s_scanFrame++;
            if (s_availableKinds.Count == 0 || s_scanFrame >= 120) {
                s_scanFrame = 0;
                ScanAvailableKinds();
            }
            ImGui.Text($"({s_availableKinds.Count} types)");

            ImGui.Separator();

            if (s_availableKinds.Count == 0) {
                ImGui.Text("Press 'Scan Level Enemies' first.");
            } else {
                foreach (var kind in s_availableKinds) {
                    if (ImGui.Button($"Spawn {kind}")) {
                        try { SpawnEnemy(kind); }
                        catch (Exception e) {
                            s_lastStatus = $"ERROR: {e.Message}";
                            API.LogError($"[ZS] Spawn {kind}: {e.Message}\n{e.StackTrace}");
                        }
                    }
                }
            }

            ImGui.Separator();

            bool cloneBusy = s_clonePhase != ClonePhase.Idle;
            if (cloneBusy) {
                ImGui.Text($"Cloning: {s_cloneKind} phase={s_clonePhase} frame={s_cloneFrame}");
            } else {
                if (ImGui.Button("Clone cp_B002 Pool")) {
                    try { StartClone("cp_B002", false); }
                    catch (Exception e) { s_lastStatus = $"Clone ERROR: {e.Message}"; }
                }
                ImGui.SameLine();
                if (ImGui.Button("Clone cp_B000 Pool")) {
                    try { StartClone("cp_B000", false); }
                    catch (Exception e) { s_lastStatus = $"Clone ERROR: {e.Message}"; }
                }
            }

            ImGui.SameLine();
            if (ImGui.Button("Pool Diag")) DumpPoolDiag();
            ImGui.SameLine();
            if (ImGui.Button("Dump Folders")) DumpCharacterPoolFolders();

            if (!string.IsNullOrEmpty(s_poolDiag)) {
                ImGui.TextWrapped(s_poolDiag);
            }

            ImGui.Separator();

            ImGui.Text("=== Cross-Chapter Scene Loading ===");
            if (ImGui.Button("Read Scene Paths")) ReadAllScenePaths();
            if (!string.IsNullOrEmpty(s_xchapterScenePaths)) {
                ImGui.TextWrapped(s_xchapterScenePaths);
            }

            ImGui.InputText("Target Chapter", ref s_xchapterInput, 64);

            bool xBusy = s_xchapterPhase != XChapterPhase.Idle;
            if (xBusy) {
                ImGui.Text($"XChapter: phase={s_xchapterPhase} frame={s_xchapterFrame}");
                ImGui.Text($"  target={s_xchapterTargetPath}");
            } else {
                if (ImGui.Button($"Hijack {s_xchapterInput} CharacterPool")) {
                    try { StartCrossChapterLoad(s_xchapterInput, "CharacterPool"); }
                    catch (Exception e) {
                        s_lastStatus = $"XChapter ERROR: {e.Message}";
                        API.LogError($"[ZS] XChapter: {e.Message}\n{e.StackTrace}");
                    }
                }
                ImGui.SameLine();
                if (ImGui.Button($"Fresh {s_xchapterInput} CharacterPool")) {
                    try { StartFreshFolderLoad(s_xchapterInput, "CharacterPool"); }
                    catch (Exception e) {
                        s_lastStatus = $"FreshFolder ERROR: {e.Message}";
                        API.LogError($"[ZS] FreshFolder: {e.Message}\n{e.StackTrace}");
                    }
                }
                if (ImGui.Button($"Load {s_xchapterInput} WeaponPool")) {
                    try { StartCrossChapterLoad(s_xchapterInput, "WeaponPool"); }
                    catch (Exception e) {
                        s_lastStatus = $"XChapter ERROR: {e.Message}";
                        API.LogError($"[ZS] XChapter: {e.Message}\n{e.StackTrace}");
                    }
                }
            }

            ImGui.Separator();
            ImGui.Text($"Tracked: {s_tracked.Count}  Clones: {s_cloneCount}");
            ImGui.Text($"Status: {s_lastStatus}");

            if (ImGui.Button("Teleport All Enemies To Me")) {
                try { TeleportAllEnemiesToPlayer(); }
                catch (Exception e) { s_lastStatus = $"Teleport ERROR: {e.Message}"; }
            }

            if (s_tracked.Count > 0 && ImGui.Button("Clear Tracking")) {
                s_tracked.Clear();
                s_lastStatus = "Tracking cleared";
            }
        } catch (Exception e) {
            ImGui.Text($"UI Error: {e.Message}");
        }

        ImGui.End();
    }

    // ═══════════ Per-frame update ═══════════

    [Callback(typeof(UpdateBehavior), CallbackType.Pre)]
    public static void OnFrame() {
        if (s_clonePhase != ClonePhase.Idle) {
            s_cloneFrame++;
            UpdateCloneStateMachine();
        }
        if (s_xchapterPhase != XChapterPhase.Idle) {
            s_xchapterFrame++;
            UpdateXChapterStateMachine();
        }

        if (s_tracked.Count == 0) return;
        RefreshPlayerInfo();
        if (s_playerCtx == null) return;

        via.vec3 playerPos = s_playerCtx.Position;

        for (int i = s_tracked.Count - 1; i >= 0; i--) {
            var t = s_tracked[i];
            t.Frame++;
            try {
                var ctx = t.Context;
                if (ctx == null) { s_tracked.RemoveAt(i); continue; }

                bool isDead = ctx.IsDead;
                bool isSpawn = ctx.IsSpawn;
                if (isDead || !isSpawn) { s_tracked.RemoveAt(i); continue; }

                var go = ctx.GameObject;
                if (go == null) { s_tracked.RemoveAt(i); continue; }

                if (s_playerFolder != null) go.FolderSelf = s_playerFolder;
                ForceGOActive(go);

                if (t.Frame <= 180 && playerPos != null) {
                    t.Context.setTransformBody(playerPos, app.WarpOption.All);
                    go.Transform.Position = playerPos;
                }

                // Periodic montage diagnostics + retry
                if (t.Frame % 120 == 60) {
                    try {
                        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
                        if (charMgr != null) {
                            bool montageInst = charMgr.isMontageInstantiatedAny(go);
                            bool drawSelf = go.DrawSelf;
                            string goName = go.Name ?? "?";
                            API.LogInfo($"[ZS] TRACK f{t.Frame} {goName}: montageInst={montageInst} draw={drawSelf}");

                            // If montage not instantiated, retry
                            if (!montageInst && t.Frame < 600) {
                                var montageID = t.Context.MontageID;
                                if (montageID != null && IsValidMontageID(montageID)) {
                                    charMgr.addMontagePresetRef(t.Context.KindID, montageID, false);
                                    charMgr.requestMontageChange(t.Context, montageID, 0, false);
                                }
                            }

                            // Deferred: re-setup HitController once montage mesh is loaded
                            if (montageInst && !t.HitSetupDone) {
                                t.HitSetupDone = true;
                                API.LogInfo($"[ZS] Montage loaded for {goName}, running deferred hit setup");
                                try {
                                    var updMO = (t.Context as IObject)?.Call("get_CharacterUpdaterBase") as ManagedObject;
                                    if (updMO != null) {
                                        var updater = updMO.As<app.CharacterUpdaterBase>();
                                        if (updater != null) {
                                            var hitCtrl = updater.HitController;
                                            if (hitCtrl != null) {
                                                hitCtrl.Enabled = true;
                                                hitCtrl.IsInvincible = false;
                                                hitCtrl.IsEnableAttack = true; // CRITICAL: without this, setup() skips SequenceTrackUpdater creation

                                                hitCtrl.setup();

                                                // Verify: SequenceTrackUpdater should now be non-null
                                                try {
                                                    IntPtr hcAddr = (IntPtr)(hitCtrl as IObject).GetAddress();
                                                    var seqUpd = Marshal.ReadIntPtr(hcAddr + 0x70);
                                                    IntPtr seqUpdInner = seqUpd != IntPtr.Zero ? Marshal.ReadIntPtr(seqUpd + 0x10) : IntPtr.Zero;
                                                    var parentHC = Marshal.ReadIntPtr(hcAddr + 0x78); // _RDI[15] = collider config
                                                    bool inDict = false;
                                                    try {
                                                        var hitMgr = API.GetManagedSingletonT<app.HitManager>();
                                                        inDict = hitMgr?.HitControllerMapping?.ContainsKey(go) ?? false;
                                                    } catch { }
                                                    // Check updater TrackUpdater too
                                                    var updTrack = updater.TrackUpdater;
                                                    ulong updTrackAddr = updTrack != null ? (updTrack as IObject).GetAddress() : 0;
                                                    API.LogInfo($"[ZS]   HC post-setup: seqUpd=0x{seqUpd.ToInt64():X} inner=0x{seqUpdInner.ToInt64():X} parentHC=0x{parentHC.ToInt64():X} atkEn={hitCtrl.IsEnableAttack} inHitMgr={inDict} updTrack=0x{updTrackAddr:X}");
                                                    // Check AttackDamageDriver
                                                    var atkDmgDrv = updater.AttackDamageDriver;
                                                    API.LogInfo($"[ZS]   AttackDamageDriver: {(atkDmgDrv != null ? "0x" + (atkDmgDrv as IObject).GetAddress().ToString("X") : "null")}");
                                                } catch (Exception ex) { API.LogWarning($"[ZS]   Diag err: {ex.Message}"); }
                                            }
                                            // Check updater native data at +0x10 (GO ref for createAttackDamageDriver)
                                            try {
                                                IntPtr updAddr = (IntPtr)(updater as IObject).GetAddress();
                                                // Dump first 48 bytes of updater object
                                                var sb = new System.Text.StringBuilder();
                                                for (int di = 0; di < 6; di++) {
                                                    var val = Marshal.ReadIntPtr(updAddr + di * 8);
                                                    sb.Append($" +0x{di*8:X2}=0x{val.ToInt64():X}");
                                                }
                                                API.LogInfo($"[ZS]   Updater raw:{sb}");
                                                // Also dump first 48 bytes of native ptr
                                                var nativePtr = Marshal.ReadIntPtr(updAddr + 0x10);
                                                if (nativePtr != IntPtr.Zero) {
                                                    var sb2 = new System.Text.StringBuilder();
                                                    for (int dj = 0; dj < 6; dj++) {
                                                        var val2 = Marshal.ReadIntPtr(nativePtr + dj * 8);
                                                        sb2.Append($" +0x{dj*8:X2}=0x{val2.ToInt64():X}");
                                                    }
                                                    API.LogInfo($"[ZS]   Native raw:{sb2}");
                                                }
                                            } catch { }

                                            // Re-enable colliders
                                            var rsc = updater.RequestSetCollider;
                                            if (rsc != null) rsc.Enabled = true;

                                            // Re-create damage drivers now that mesh exists
                                            try {
                                                // Try calling via concrete type for proper virtual dispatch
                                                var b002Upd = updMO.As<app.Cp_B002Updater>();
                                                if (b002Upd != null) {
                                                    API.LogInfo($"[ZS]   Calling createAttackDamageDriver on Cp_B002Updater (typed)");
                                                    b002Upd.createAttackDamageDriver();
                                                } else {
                                                    // Fallback: call via ManagedObject.Call for virtual dispatch
                                                    API.LogInfo($"[ZS]   Calling createAttackDamageDriver via Call() for virtual dispatch");
                                                    updMO.Call("createAttackDamageDriver");
                                                }
                                                var atkPost = updater.AttackDamageDriver;
                                                API.LogInfo($"[ZS]   After createAttackDamageDriver: {(atkPost != null ? "0x" + (atkPost as IObject).GetAddress().ToString("X") : "null")}");
                                            } catch (Exception ex) { API.LogWarning($"[ZS]   createAttackDamageDriver error: {ex.Message}"); }

                                            // Compare: check a NORMAL enemy's AttackDamageDriver
                                            try {
                                                var charMgr2 = API.GetManagedSingletonT<app.CharacterManager>();
                                                var enemyList2 = charMgr2?.EnemyContextList;
                                                for (int ci = 0; ci < (enemyList2?.Count ?? 0); ci++) {
                                                    try {
                                                        var ec = enemyList2[ci];
                                                        if (ec == null || ec.IsDead || !ec.IsSpawn) continue;
                                                        var ego = ec.GameObject;
                                                        if (ego == null || (ego as IObject).GetAddress() == (go as IObject).GetAddress()) continue;
                                                        var eUpdMO = (ec as IObject)?.Call("get_CharacterUpdaterBase") as ManagedObject;
                                                        if (eUpdMO == null) continue;
                                                        var eUpd = eUpdMO.As<app.CharacterUpdaterBase>();
                                                        if (eUpd == null) continue;
                                                        var eAtkDrv = eUpd.AttackDamageDriver;
                                                        API.LogInfo($"[ZS]   NORMAL '{ego.Name}' AttackDamageDriver: {(eAtkDrv != null ? "exists" : "null")}");
                                                        break;
                                                    } catch { }
                                                }
                                            } catch { }

                                            updater.initializeWinceHP();
                                            updater.setupTargetSensorMarker();
                                            var enemyUpd = updMO.As<app.EnemyUpdaterBase>();
                                            if (enemyUpd != null) {
                                                enemyUpd.createBodyPartDriver();
                                                enemyUpd.createLockOnTargetDriver();
                                            }
                                            API.LogInfo($"[ZS]   Deferred damage drivers created");
                                        }
                                    }
                                } catch (Exception ex) { API.LogWarning($"[ZS] Deferred hit setup: {ex.Message}"); }
                            }
                        }
                    } catch { }
                }

                if (t.Frame > 900) { s_tracked.RemoveAt(i); continue; }
                s_tracked[i] = t;
            } catch { s_tracked.RemoveAt(i); }
        }
    }

    // ═══════════ Clone state machine ═══════════

    static void UpdateCloneStateMachine() {
        try {
            if (s_clonePhase == ClonePhase.Deactivated) {
                bool active = s_cloneFolder?.Active ?? true;
                if (!active || s_cloneFrame > 30) {
                    API.LogInfo($"[ZS] Clone: folder inactive (frame {s_cloneFrame}), activating...");
                    s_cloneFolder?.activate();
                    s_clonePhase = ClonePhase.WaitReady;
                    s_cloneFrame = 0;
                }
            }
            else if (s_clonePhase == ClonePhase.WaitReady) {
                bool active = s_cloneFolder?.Active ?? false;
                if ((active && s_cloneFrame > 10) || s_cloneFrame > 180) {
                    s_cloneCount++;
                    s_clonePhase = ClonePhase.Idle;
                    int poolCount = GetPoolCountForKind(s_cloneKind);
                    s_lastStatus = $"Clone done! {s_cloneKind} pool={poolCount}";
                    API.LogInfo($"[ZS] Clone complete: {s_lastStatus}");
                    DumpPoolDiag();

                    if (s_spawnAfterClone) {
                        s_spawnAfterClone = false;
                        try { SpawnEnemy(s_cloneKind); }
                        catch (Exception e) {
                            s_lastStatus = $"Auto-spawn ERROR: {e.Message}";
                            API.LogError($"[ZS] Auto-spawn: {e.Message}\n{e.StackTrace}");
                        }
                    }
                    s_cloneFolder = null;
                }
            }
        } catch (Exception e) {
            API.LogError($"[ZS] Clone error: {e.Message}");
            s_clonePhase = ClonePhase.Idle;
            s_cloneFolder = null;
            s_spawnAfterClone = false;
        }
    }

    // ═══════════ Cross-chapter state machine ═══════════

    static void UpdateXChapterStateMachine() {
        try {
            if (s_xchapterPhase == XChapterPhase.Deactivated) {
                bool active = s_xchapterFolder?.Active ?? true;
                if (!active || s_xchapterFrame > 60) {
                    API.LogInfo($"[ZS] XChapter: folder inactive (frame {s_xchapterFrame}), writing new path...");
                    WriteFolderScenePath(s_xchapterFolder, s_xchapterTargetPath);
                    string verify = ReadFolderScenePath(s_xchapterFolder);
                    API.LogInfo($"[ZS] XChapter: path after write = '{verify}'");
                    s_xchapterFolder?.activate();
                    s_xchapterPhase = XChapterPhase.WaitReady;
                    s_xchapterFrame = 0;
                }
            }
            else if (s_xchapterPhase == XChapterPhase.WaitReady) {
                bool active = s_xchapterFolder?.Active ?? false;
                if ((active && s_xchapterFrame > 15) || s_xchapterFrame > 300) {
                    s_xchapterPhase = XChapterPhase.Idle;
                    if (active) {
                        s_lastStatus = $"XChapter loaded! {s_xchapterTargetPath}";
                        API.LogInfo($"[ZS] XChapter: scene loaded successfully");
                    } else {
                        s_lastStatus = $"XChapter: activation timeout";
                        API.LogWarning($"[ZS] XChapter: folder never became active");
                        if (!string.IsNullOrEmpty(s_xchapterOrigPath)) {
                            WriteFolderScenePath(s_xchapterFolder, s_xchapterOrigPath);
                            API.LogInfo($"[ZS] XChapter: restored original path '{s_xchapterOrigPath}'");
                        }
                    }
                    DumpPoolDiag();
                    s_xchapterFolder = null;
                }
            }
        } catch (Exception e) {
            API.LogError($"[ZS] XChapter error: {e.Message}\n{e.StackTrace}");
            s_xchapterPhase = XChapterPhase.Idle;
            if (s_xchapterFolder != null && !string.IsNullOrEmpty(s_xchapterOrigPath)) {
                try { WriteFolderScenePath(s_xchapterFolder, s_xchapterOrigPath); } catch { }
            }
            s_xchapterFolder = null;
        }
    }

    // ═══════════ Cross-chapter scene loading ═══════════

    static void StartCrossChapterLoad(string targetChapter, string sceneType) {
        if (s_xchapterPhase != XChapterPhase.Idle) { s_lastStatus = "Cross-chapter load already in progress"; return; }
        if (s_clonePhase != ClonePhase.Idle) { s_lastStatus = "Clone in progress, wait"; return; }

        var folder = FindAnySceneFolder();
        if (folder == null) { s_lastStatus = "No scene folder found to hijack"; return; }

        string currentPath = ReadFolderScenePath(folder);
        string folderName = folder.Name ?? "?";
        API.LogInfo($"[ZS] XChapter: hijacking folder '{folderName}' (path='{currentPath}')");

        string targetPath = $"GameAssets/Character/Scene/{targetChapter}/{targetChapter}_{sceneType}.scn";
        API.LogInfo($"[ZS] XChapter: target path = '{targetPath}'");

        IntPtr folderAddr = (IntPtr)(folder as IObject).GetAddress();
        int capacity = Marshal.ReadInt32(folderAddr + OFF_SCENE_PATH_CAP);
        if (targetPath.Length > capacity) {
            s_lastStatus = $"Path too long: {targetPath.Length} > capacity {capacity}";
            return;
        }

        s_xchapterOrigPath = currentPath;
        s_xchapterTargetPath = targetPath;
        s_xchapterFolder = folder;
        folder.deactivate();
        s_xchapterPhase = XChapterPhase.Deactivated;
        s_xchapterFrame = 0;
        s_lastStatus = $"Loading {targetPath}...";
    }

    // ═══════════ Fresh folder creation ═══════════

    static void StartFreshFolderLoad(string targetChapter, string sceneType) {
        if (s_xchapterPhase != XChapterPhase.Idle) { s_lastStatus = "Cross-chapter load already in progress"; return; }

        var sceneMgr = API.GetNativeSingletonT<via.SceneManager>();
        if (sceneMgr == null) { s_lastStatus = "No SceneManager"; return; }
        var mainScene = via.SceneManager.MainScene;
        if (mainScene == null) { s_lastStatus = "No main scene"; return; }
        IntPtr sceneAddr = (IntPtr)(mainScene as IObject).GetAddress();

        var folder = via.Folder.REFType.CreateInstance(0)?.As<via.Folder>();
        API.LogInfo($"[ZS] FreshFolder: created new folder instance, addr=0x{(folder as IObject).GetAddress():X}");
        if (folder == null) { s_lastStatus = "Failed to create Folder"; return; }
        var folderObj = (folder as IProxy).GetInstance() as ManagedObject;
        folderObj.Globalize();
        folderObj.AddRef();
        s_createdFolders.Add(folder);
        IntPtr folderAddr = (IntPtr)(folder as IObject).GetAddress();
        API.LogInfo($"[ZS] FreshFolder: created at 0x{folderAddr.ToInt64():X}");

        Marshal.WriteByte(folderAddr + 0x20, 1);
        Marshal.WriteIntPtr(folderAddr + OFF_SCENE_PTR, sceneAddr);
        API.LogInfo($"[ZS] FreshFolder: byte20=1, mScenePtr=0x{sceneAddr.ToInt64():X}");

        string path = $"GameAssets/Character/Scene/{targetChapter}/{targetChapter}_{sceneType}.scn";
        int bufSize = (path.Length + 1) * 2;
        if (s_allocatedStrBuf != IntPtr.Zero) s_allocatedStrBufs.Add(s_allocatedStrBuf);
        s_allocatedStrBuf = Marshal.AllocHGlobal(bufSize);

        for (int i = 0; i < path.Length; i++)
            Marshal.WriteInt16(s_allocatedStrBuf + i * 2, (short)path[i]);
        Marshal.WriteInt16(s_allocatedStrBuf + path.Length * 2, 0);

        Marshal.WriteIntPtr(folderAddr + OFF_SCENE_PATH_PTR, s_allocatedStrBuf);
        Marshal.WriteInt32(folderAddr + OFF_SCENE_PATH_SIZE, path.Length);
        Marshal.WriteInt32(folderAddr + OFF_SCENE_PATH_CAP, path.Length + 8);

        string verify = ReadFolderScenePath(folder);
        API.LogInfo($"[ZS] FreshFolder: path = '{verify}'");

        folder.activate();
        API.LogInfo($"[ZS] FreshFolder: activate() called");

        s_xchapterFolder = folder;
        s_xchapterPhase = XChapterPhase.WaitReady;
        s_xchapterFrame = 0;
        s_xchapterTargetPath = path;
        s_xchapterOrigPath = "";
        s_lastStatus = $"Fresh folder loading {path}...";
    }

    // ═══════════ Native memory helpers for via.Folder ═══════════

    static string ReadFolderScenePath(via.Folder folder) {
        IntPtr addr = (IntPtr)(folder as IObject).GetAddress();
        int size = Marshal.ReadInt32(addr + OFF_SCENE_PATH_SIZE);
        int capacity = Marshal.ReadInt32(addr + OFF_SCENE_PATH_CAP);
        if (size <= 0 || size > 1024) return "";
        IntPtr strPtr;
        if (capacity >= 12)
            strPtr = Marshal.ReadIntPtr(addr + OFF_SCENE_PATH_PTR);
        else
            strPtr = addr + OFF_SCENE_PATH_PTR;
        if (strPtr == IntPtr.Zero) return "";
        char[] chars = new char[size];
        for (int i = 0; i < size; i++)
            chars[i] = (char)Marshal.ReadInt16(strPtr + i * 2);
        return new string(chars);
    }

    static void WriteFolderScenePath(via.Folder folder, string newPath) {
        IntPtr addr = (IntPtr)(folder as IObject).GetAddress();
        int capacity = Marshal.ReadInt32(addr + OFF_SCENE_PATH_CAP);
        if (capacity < 12) throw new Exception($"Cannot write to SSO string (capacity={capacity})");
        if (newPath.Length > capacity) throw new Exception($"Path too long: {newPath.Length} > capacity {capacity}");
        IntPtr strPtr = Marshal.ReadIntPtr(addr + OFF_SCENE_PATH_PTR);
        if (strPtr == IntPtr.Zero) throw new Exception("Scene path heap pointer is null");
        for (int i = 0; i < newPath.Length; i++)
            Marshal.WriteInt16(strPtr + i * 2, (short)newPath[i]);
        Marshal.WriteInt16(strPtr + newPath.Length * 2, 0);
        Marshal.WriteInt32(addr + OFF_SCENE_PATH_SIZE, newPath.Length);
        API.LogInfo($"[ZS] WriteFolderScenePath: wrote {newPath.Length} chars (cap={capacity})");
    }

    static via.Folder FindAnySceneFolder() {
        var sceneMgr = API.GetNativeSingletonT<via.SceneManager>();
        if (sceneMgr == null) return null;
        var mainScene = via.SceneManager.MainScene;
        if (mainScene == null) return null;

        string[] prefixes = { "Chap1_", "Chap2_", "Chap3_", "Chap4_", "Chap5_", "Chap6_", "Chap7_", "Chap8_", "Chap9_" };
        foreach (var prefix in prefixes) {
            for (int n = 0; n <= 30; n++) {
                string folderName = $"{prefix}{n:D2}_CharacterPool";
                var cpFolder = mainScene.findFolder(folderName);
                if (cpFolder == null) continue;

                var child = cpFolder.Child;
                while (child != null) {
                    bool isSF = child.SceneFolder;
                    if (isSF) {
                        string name = child.Name ?? "";
                        if (name.Contains("StaticCharacter")) return child;
                    }
                    child = child.Next;
                }

                child = cpFolder.Child;
                while (child != null) {
                    bool isSF = child.SceneFolder;
                    if (isSF) return child;
                    child = child.Next;
                }
            }
        }
        return null;
    }

    // ═══════════ Read all scene paths diagnostic ═══════════

    static void ReadAllScenePaths() {
        try {
            var sb = new System.Text.StringBuilder();
            sb.AppendLine("=== Scene Folder Paths ===");

            var sceneMgr = API.GetNativeSingletonT<via.SceneManager>();
            if (sceneMgr == null) { s_xchapterScenePaths = "No SceneManager"; return; }
            var mainScene = via.SceneManager.MainScene;
            if (mainScene == null) { s_xchapterScenePaths = "No main scene"; return; }

            string[] prefixes = { "Chap1_", "Chap2_", "Chap3_", "Chap4_", "Chap5_", "Chap6_", "Chap7_", "Chap8_", "Chap9_" };
            foreach (var prefix in prefixes) {
                for (int n = 0; n <= 30; n++) {
                    string folderName = $"{prefix}{n:D2}_CharacterPool";
                    var cpFolder = mainScene.findFolder(folderName);
                    if (cpFolder == null) continue;

                    sb.AppendLine($"\n{folderName}:");
                    var child = cpFolder.Child;
                    while (child != null) {
                        string cName = child.Name ?? "?";
                        bool isSF = child.SceneFolder;
                        if (isSF) {
                            string scenePath = ReadFolderScenePath(child);
                            sb.AppendLine($"  {cName} => {scenePath}");
                        } else {
                            sb.AppendLine($"  {cName} (not scene folder)");
                        }
                        child = child.Next;
                    }
                }
            }

            s_xchapterScenePaths = sb.ToString();
            API.LogInfo($"[ZS] {s_xchapterScenePaths}");
            s_lastStatus = "Scene paths read";
        } catch (Exception e) {
            s_xchapterScenePaths = $"Error: {e.Message}";
            API.LogError($"[ZS] ReadAllScenePaths: {e.Message}\n{e.StackTrace}");
        }
    }

    // ═══════════ Clone pool entry via scene folder reload ═══════════

    static bool StartClone(string kindName, bool spawnAfter) {
        if (s_clonePhase != ClonePhase.Idle) { s_lastStatus = "Clone already in progress"; return false; }

        RefreshPlayerInfo();
        if (s_playerFolder == null) { s_lastStatus = "No player folder"; return false; }

        var sceneFolder = FindSceneFolderForKind(kindName);
        if (sceneFolder == null) { s_lastStatus = $"No scene folder for {kindName}"; return false; }

        var goInFolder = FindPoolGOInFolder(kindName, sceneFolder);
        if (goInFolder != null) {
            var goName = goInFolder.Name ?? "?";
            goInFolder.FolderSelf = s_playerFolder;
            API.LogInfo($"[ZS] Clone: moved '{goName}' to player folder");
        } else {
            API.LogInfo($"[ZS] Clone: no GO in scene folder to move");
        }

        int before = GetPoolCountForKind(kindName);
        API.LogInfo($"[ZS] Clone: {kindName} pool before={before}");

        var folderPath = sceneFolder.Path ?? "?";
        API.LogInfo($"[ZS] Clone: deactivating '{folderPath}'...");
        sceneFolder.deactivate();

        s_cloneFolder = sceneFolder;
        s_clonePhase = ClonePhase.Deactivated;
        s_cloneFrame = 0;
        s_cloneKind = kindName;
        s_spawnAfterClone = spawnAfter;
        s_lastStatus = $"Cloning {kindName}...";
        return true;
    }

    // ═══════════ Spawn enemy ═══════════

    static void SpawnEnemy(string kindName) {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) { s_lastStatus = "No CharacterManager"; return; }

        // Dynamic kind lookup — must use reflection since name comes from UI
        var kindID = ResolveKindID(kindName);
        if (kindID == null) { s_lastStatus = $"Unknown kind: {kindName}"; return; }

        // --- Get a free pool entry (typed proxy!) ---
        //var poolEntryMO = (charMgr as IObject).Call("findUsableCharacter", kindID) as ManagedObject;
        var poolInfo = charMgr.findUsableCharacter(kindID);
        if (poolInfo == null) {
            if (RecycleDeadEnemy(charMgr, kindID))
                poolInfo = charMgr.findUsableCharacter(kindID);
        }
        if (poolInfo == null) {
            if (StartClone(kindName, true))
                s_lastStatus = $"No free pool for {kindName} - auto-cloning...";
            return;
        }

        // Typed proxy access for pool info
        var poolUpdater = poolInfo.Updater;
        if (poolUpdater == null) {
            s_lastStatus = $"Pool entry for {kindName} has no updater";
            return;
        }

        // Force GO active before linking
        var poolGO = poolUpdater.GameObject;
        if (poolGO != null) {
            ForceGOActive(poolGO);
            API.LogInfo($"[ZS] Pool GO '{poolGO.Name}': forced all flags active");
        } else {
            API.LogWarning($"[ZS] Pool updater for {kindName} has no GO!");
        }

        // --- Find source MontageID ---
        var enemyList = charMgr.EnemyContextList;

        app.EnemySpawnData sourceSpawnData = null;
        app.MontageID sourceMontageID = null;

        // Pass 1: spawned context of same kind (only if montage is valid)
        foreach (var ctx in enemyList) {
            try {
                //var kidMO = ToManaged(ctx.KindID as IObject);
                //if (!SameObject(kidMO, kindMO)) continue;
                var kid = ctx.KindID;
                if (kid != kindID) continue;
                //var midMO = ToManaged(ctx.MontageID as IObject);
                var mid = ctx.MontageID;
                if (!IsValidMontageID(mid)) continue;  // skip -1/-1 and 0/0
                sourceMontageID = mid;
                sourceSpawnData = ctx.SpawnData;
                bool isDead = ctx.IsDead;
                API.LogInfo($"[ZS] Pass1: MontageID={FormatMontageID(mid)} from {(isDead?"dead":"alive")} {kindName}");
                if (!isDead) break;
            } catch { }
        }

        // Pass 2: get from pool updater's context
        if (!IsValidMontageID(sourceMontageID)) {
            sourceMontageID = null;
            try {
                var poolContext = poolUpdater.Context;
                if (poolContext != null) {
                    var mid2 = poolContext.MontageID;
                    if (IsValidMontageID(mid2)) {
                        sourceMontageID = mid2;
                        API.LogInfo($"[ZS] Pass2: MontageID={FormatMontageID(mid2)} from pool context");
                    }
                }
            } catch { }
        }

        // Pass 3: random montage preset for this kind (typed proxy call!)
        if (!IsValidMontageID(sourceMontageID)) {
            sourceMontageID = null;
            try {
                var rnd = charMgr.getRandomMontagePresetID(kindID, 0);
                if (IsValidMontageID(rnd)) {
                    sourceMontageID = rnd;
                    API.LogInfo($"[ZS] Pass3: random MontageID={FormatMontageID(rnd)} for {kindName}");
                } else {
                    API.LogInfo($"[ZS] Pass3: getRandomMontagePresetID returned {FormatMontageID(rnd)} (invalid)");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] Pass3: {e.Message}"); }
        }

        // Pass 4: default zeroed MontageID
        if (!IsValidMontageID(sourceMontageID)) {
            API.LogWarning($"[ZS] No valid MontageID for {kindName}, creating default");
            var dm = app.MontageID.REFType.CreateInstance(0);
            if (dm != null) { dm.Globalize(); sourceMontageID = dm.As<app.MontageID>(); }
        }

        // Log final MontageID
        API.LogInfo($"[ZS] Using MontageID={FormatMontageID(sourceMontageID)} for {kindName}");

        // Force-load montage prefab resources BEFORE spawning
        if (IsValidMontageID(sourceMontageID)) {
            try {
                charMgr.addMontagePresetRef(kindID, sourceMontageID, false);
                API.LogInfo($"[ZS] addMontagePresetRef called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] addMontagePresetRef: {e.Message}"); }
        }

        RefreshPlayerInfo();
        if (s_playerCtx == null) { s_lastStatus = "No player"; return; }
        var playerPos = s_playerCtx.Position;
        var playerRotation = GetPlayerRotation();

        // --- Create context ---
        var newCtxID = app.ContextID.REFType.CreateInstance(0);
        newCtxID.Globalize();
        var guidBytes = Guid.NewGuid().ToByteArray();
        var objAddr = (long)newCtxID.GetAddress();
        for (int b = 0; b < 16; b++)
            Marshal.WriteByte((IntPtr)(objAddr + 0x10 + b), guidBytes[b]);

        // getCharacterContextFactory + readyContext use generic types — keep as reflection
        //var factory = (charMgr as IObject).Call("getCharacterContextFactory", kindID);
        var factory = charMgr.getCharacterContextFactory(kindID);
        if (factory == null) { s_lastStatus = $"No context factory for {kindName}"; return; }
        var newContext = charMgr.readyContext(newCtxID.As<app.ContextID>(), kindID, factory);
        if (newContext == null) { s_lastStatus = $"readyContext returned null for {kindName}"; return; }

        if (sourceMontageID != null)
            newContext.setMontage(sourceMontageID);

        if (sourceSpawnData != null) {
            var sd = sourceSpawnData.duplicate();
            //(sd as ManagedObject).Globalize();
            ((sd as IProxy).GetInstance() as ManagedObject).Globalize();
            sd.ContextID = newCtxID.As<app.ContextID>();
            sd.KindID = kindID;
            sd.IsFirstSpawn = true;
            sd.IsForceTransform = true;
            sd.Position = playerPos;
            newContext.applySpawnData(sd);
            charMgr.registerSpawnData(sd);

            API.LogInfo($"[ZS] Applied spawn data from source context: pos={sd.Position.ToString()} rot={sd.Rotation.ToString()}");
        }

        // --- Direct link to pool entry ---
        //newContextObj.Call("link", ToManaged(poolUpdater as IObject));
        newContext.link(poolUpdater);
        poolInfo.Used = true;
        newContext.IsSpawn = true;
        API.LogInfo($"[ZS] Direct link to pool updater for {kindName}");

        try { poolUpdater.spawnSetup(newContext); }
        catch (Exception e) { API.LogWarning($"[ZS] spawnSetup: {e.Message}"); }

        // Also try requestSpawn for engine-side montage loading
        try {
            charMgr.requestSpawn(newCtxID.As<app.ContextID>(), kindID, sourceMontageID, 0, false, 0);
        } catch (Exception e) { API.LogWarning($"[ZS] requestSpawn: {e.Message}"); }

        // --- Move GO to player zone ---
        var spawnGO = newContext.GameObject;
        if (spawnGO != null) {
            if (s_playerFolder != null) spawnGO.FolderSelf = s_playerFolder;
            ForceGOActive(spawnGO);
            var spawnTf = spawnGO.Transform;
            if (spawnTf != null) spawnTf.Position = playerPos;
            newContext.setTransformBody(playerPos, app.WarpOption.All);
            try { if (playerRotation != null) poolUpdater.warp(playerPos, playerRotation, 0); }
            catch { }

            // Warp CharacterController
            try {
                var charCtrl = poolUpdater.CharacterController;
                if (charCtrl != null) {
                    charCtrl.Enabled = false;
                    //(charCtrl as IObject)?.Call("warp", playerPos);
                    charCtrl.warp();
                    charCtrl.Enabled = true;
                    API.LogInfo($"[ZS] CharacterController warped for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] CharCtrl: {e.Message}"); }

            // === MONTAGE LOADING (3D model) ===
            // 1) CharacterManager.requestMontageChange — high-level wrapper
            try {
                /*(charMgr as IObject).Call(
                    "requestMontageChange(app.CharacterContext, app.MontageID, System.Int32, System.Boolean)",
                    newContext, sourceMontageID, 0, false
                );*/
                charMgr.requestMontageChange(newContext, sourceMontageID, 0, false);
                API.LogInfo($"[ZS] CharMgr.requestMontageChange called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] CharMgr.requestMontageChange: {e.Message}"); }

            // 2) Direct MontageManager.requestMontageChange
            try {
                var montageMgr = charMgr.MontageManager;
                if (montageMgr != null) {
                    charMgr.requestClearAllMontages(spawnGO, kindID);
                    montageMgr.requestMontageChange(spawnGO, kindID, sourceMontageID, null, true, 0, false);
                    API.LogInfo($"[ZS] MontageManager.requestMontageChange called for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] MontMgr.requestMontageChange: {e.Message}"); }

            // 3) requestInstantiateMontage
            try {
                charMgr.requestInstantiateMontage(spawnGO, kindID, sourceMontageID, null, true, 0, false);
                API.LogInfo($"[ZS] requestInstantiateMontage called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] requestInstantiateMontage: {e.Message}"); }

            // 4) Force-enable GO via GameObjectStateSetMediator
            try {
                var goMediator = API.GetManagedSingletonT<app.GameObjectStateSetMediator>();
                if (goMediator != null) {
                    var def = app.GameObjectStateSetArgs.Default;
                    goMediator.requestSetState(spawnGO, true, true, null, ref def);
                    API.LogInfo($"[ZS] requestSetState(update=true, draw=true) for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] requestSetState: {e.Message}"); }

            // 5) reqeustDrawMontage (engine typo)
            try {
                charMgr.reqeustDrawMontage(true, spawnGO, sourceMontageID);
                API.LogInfo($"[ZS] reqeustDrawMontage called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] drawMontage: {e.Message}"); }

            // === AI + COLLISION + DAMAGE ACTIVATION ===
            try {
                // Initialize drivers (creates ThinkDriver, ActionDriver, AttackDamageDriver, etc.)
                poolUpdater.initDrivers();
                API.LogInfo($"[ZS] initDrivers() called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] initDrivers: {e.Message}"); }

            // Check if AttackDamageDriver was created by initDrivers
            try {
                var atkDrv = poolUpdater.AttackDamageDriver;
                API.LogInfo($"[ZS] After initDrivers: AttackDamageDriver={atkDrv != null} for {kindName}");
            } catch { }

            try {
                // Set player context so enemy knows who to target
                var enemyUpd = (poolUpdater as IObject)?.As<app.EnemyUpdaterBase>();
                if (enemyUpd != null) {
                    enemyUpd.PlayerContext = s_playerCtx;
                    API.LogInfo($"[ZS] PlayerContext set via typed proxy for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] set_PlayerContext: {e.Message}"); }

            try {
                // Switch to think (AI) control mode
                poolUpdater.setThinkControl();
                API.LogInfo($"[ZS] setThinkControl() called for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] setThinkControl: {e.Message}"); }

            try {
                // Enable + setup HitController for damage/shooting
                var hitCtrl = poolUpdater.HitController;
                if (hitCtrl != null) {
                    hitCtrl.Enabled = true;
                    hitCtrl.IsInvincible = false;
                    hitCtrl.IsEnableAttack = true;
                    hitCtrl.setup();
                    API.LogInfo($"[ZS] HitController enabled + setup for {kindName}");
                } else {
                    API.LogWarning($"[ZS] No HitController on {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] HitController: {e.Message}"); }

            try {
                // Enable CharacterController for physics collision
                var charCtrl2 = poolUpdater.CharacterController;
                if (charCtrl2 != null) {
                    charCtrl2.Enabled = true;
                    API.LogInfo($"[ZS] CharacterController re-enabled for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] CharCtrl2: {e.Message}"); }

            try {
                // Initialize wince HP (stagger thresholds)
                poolUpdater.initializeWinceHP();
                API.LogInfo($"[ZS] initializeWinceHP() for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] initializeWinceHP: {e.Message}"); }

            try {
                // Enable RequestSetCollider for physics/damage colliders
                var rsc = poolUpdater.RequestSetCollider;
                if (rsc != null) {
                    rsc.Enabled = true;
                    API.LogInfo($"[ZS] RequestSetCollider enabled for {kindName}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] RequestSetCollider: {e.Message}"); }

            try {
                // Setup target sensor/marker so enemy is lockable/targetable
                poolUpdater.setupTargetSensorMarker();
                API.LogInfo($"[ZS] setupTargetSensorMarker() for {kindName}");
            } catch (Exception e) { API.LogWarning($"[ZS] setupTargetSensorMarker: {e.Message}"); }

            try {
                // Explicitly create damage-related drivers if initDrivers missed them
                poolUpdater.createAttackDamageDriver();
                var enemyUpd2 = (poolUpdater as IObject)?.As<app.EnemyUpdaterBase>();
                if (enemyUpd2 != null) {
                    var bodyPartDrv = enemyUpd2.createBodyPartDriver();
                    var lockOnDrv = enemyUpd2.createLockOnTargetDriver();
                    API.LogInfo($"[ZS] Damage drivers created for {kindName} bodyPart={bodyPartDrv != null} lockOn={lockOnDrv != null}");
                }
            } catch (Exception e) { API.LogWarning($"[ZS] createDrivers: {e.Message}"); }

            // === DIAGNOSTICS ===
            try {
                bool refbool = false;
                bool prefabLoaded = charMgr.isMontagePresetReady(kindID, sourceMontageID, ref refbool);
                bool montageInst = charMgr.isMontageInstantiatedAny(spawnGO);
                bool drawSelf = spawnGO.DrawSelf;
                bool updateSelf = spawnGO.UpdateSelf;
                API.LogInfo($"[ZS] DIAG {kindName}: prefabReady={prefabLoaded} montageInst={montageInst} drawSelf={drawSelf} updateSelf={updateSelf}");
            } catch (Exception e) { API.LogWarning($"[ZS] DIAG: {e.Message}"); }

            API.LogInfo($"[ZS] Spawned {kindName} - GO moved to player");
        } else {
            API.LogWarning($"[ZS] Spawned {kindName} - NO GO (will retry in frame callback)");
        }

        s_tracked.Add(new TrackedZombie { Context = newContext, Frame = 0 });
        s_lastStatus = $"Spawned {kindName}!";
    }

    // ═══════════ Pool diagnostics ═══════════

    static void DumpPoolDiag() {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) { s_poolDiag = "No CharacterManager"; return; }
        var pool = charMgr.CharacterPool;
        int count = pool?.Count ?? 0;

        var counts = new Dictionary<string, int[]>();
        //for (int i = 0; i < count; i++) {
        foreach (var entry in pool) {
            try {
                if (entry == null) continue;
                bool used = entry.Used;
                var entryMO = entry as IObject;
                var updater = entry.Updater;
                string kind = "unknown";
                if (updater != null) {
                    var updaterContext = updater.Context;
                    if (updaterContext != null) kind = KindToString(updaterContext.KindID);
                    if (kind == "" || kind == "unknown") kind = ExtractKindFromGOName(updater.GameObject?.Name);
                }
                if (!counts.ContainsKey(kind)) counts[kind] = new int[3];
                counts[kind][0]++;
                if (used) counts[kind][1]++; else counts[kind][2]++;
            } catch { }
        }

        var sb = new System.Text.StringBuilder();
        sb.AppendLine($"Pool: {count} entries");
        foreach (var kv in counts) {
            sb.AppendLine($"  {kv.Key}: {kv.Value[0]} total, {kv.Value[1]} used, {kv.Value[2]} free");
        }
        s_poolDiag = sb.ToString();
        API.LogInfo($"[ZS] {s_poolDiag}");
    }

    static int GetPoolCountForKind(string kindName) {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) return -1;
        var kindID = ResolveKindID(kindName);
        if (kindID == null) return -1;

        var pool = charMgr.CharacterPool;
        int result = 0;
        foreach (var entry in pool) {
            try {
                if (entry == null) continue;
                var updater = entry.Updater;
                if (updater == null) continue;
                var poolContext = updater.Context;
                string kind = KindToString(poolContext?.KindID);
                if (string.IsNullOrEmpty(kind)) kind = ExtractKindFromGOName(updater.GameObject?.Name);
                if (kind == kindName) result++;
            } catch { }
        }
        return result;
    }

    // ═══════════ Teleport all enemies ═══════════

    static void TeleportAllEnemiesToPlayer() {
        RefreshPlayerInfo();
        if (s_playerCtx == null) { s_lastStatus = "No player"; return; }
        var playerPos = s_playerCtx.Position;
        var playerRotation = GetPlayerRotation();
        if (playerPos == null) { s_lastStatus = "No player pos"; return; }

        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) { s_lastStatus = "No CharacterManager"; return; }

        var pool = charMgr.CharacterPool;
        int poolCount = pool?.Count ?? 0;
        int moved = 0;

        foreach (var entry in pool) {
            try {
            if (entry == null) continue;
            var updater = entry.Updater;
                if (updater == null) continue;
                var go = updater.GameObject;
                if (go == null) continue;

                if (s_playerFolder != null) go.FolderSelf = s_playerFolder;
                ForceGOActive(go);

                var tf = go.Transform;
                if (tf != null) tf.Position = playerPos;
                try { if (playerRotation != null) updater.warp(playerPos, playerRotation, 0); } catch { }
                var ctx = updater.Context;
                if (ctx != null) {
                    try { ctx.setTransformBody(playerPos, app.WarpOption.All); } catch { }
                }
                moved++;
            } catch { }
        }

        s_lastStatus = $"Teleported {moved}/{poolCount} pool entries to player";
        API.LogInfo($"[ZS] {s_lastStatus}");
    }

    // ═══════════ Helpers ═══════════

    static via.GameObject FindPoolGO(string kindName) {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) return null;
        var kindMO = ResolveKindID(kindName);
        if (kindMO == null) return null;

        var pool = charMgr.CharacterPool;
        int poolCount = pool?.Count ?? 0;

        for (int i = 0; i < poolCount; i++) {
            var entry = pool[i];
            if (entry == null) continue;
            var updater = entry.Updater;
            if (updater == null) continue;
            var ctx = updater.Context;
            if (ctx?.KindID != kindMO) continue;
            return updater.GameObject;
        }
        return null;
    }

    static via.Folder FindSceneFolderForKind(string kindName) {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) return null;
        var kindMO = ResolveKindID(kindName);
        if (kindMO == null) return null;

        var pool = charMgr.CharacterPool;
        int poolCount = pool?.Count ?? 0;

        via.GameObject anyGO = null;
        for (int i = 0; i < poolCount; i++) {
            var entry = pool[i];
            if (entry == null) continue;
            var updater = entry.Updater;
            if (updater == null) continue;
            var ctx = updater.Context;
            if (ctx?.KindID != kindMO) continue;
            var go = updater.GameObject;
            if (go == null) continue;
            anyGO = go;
            var folder = go.Folder;
            if (folder == null) continue;
            bool isSF = folder.SceneFolder;
            if (isSF) return folder;
        }

        if (anyGO == null) return null;
        var anyFolder = anyGO.Folder;
        if (anyFolder == null) return null;

        var current = anyFolder;
        for (int depth = 0; depth < 10; depth++) {
            var parent = current.Parent;
            if (parent == null) break;
            var parentPath = parent.Path ?? "";
            if (parentPath.Contains("CharacterPool")) {
                var child = parent.Child;
                while (child != null) {
                    var childPath = child.Path ?? "";
                    bool childIsSF = child.SceneFolder;
                    if (childIsSF && childPath.ToLower().Contains(kindName.ToLower())) {
                        return child;
                    }
                    child = child.Next;
                }
                break;
            }
            current = parent;
        }
        return null;
    }

    static via.GameObject FindPoolGOInFolder(string kindName, via.Folder targetFolder) {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) return null;
        var kindMO = ResolveKindID(kindName);
        if (kindMO == null) return null;

        var targetPath = targetFolder.Path ?? "";
        var pool = charMgr.CharacterPool;
        int poolCount = pool?.Count ?? 0;

        for (int i = 0; i < poolCount; i++) {
            var entry = pool[i];
            if (entry == null) continue;
            var updater = entry.Updater;
            if (updater == null) continue;
            var ctx = updater.Context;
            if (ctx?.KindID != kindMO) continue;
            var go = updater.GameObject;
            if (go == null) continue;
            var goFolder = go.Folder;
            if (goFolder == null) continue;
            var goFolderPath = goFolder.Path ?? "";
            if (goFolderPath == targetPath) return go;
        }
        return null;
    }

    /// Force all 5 native visibility/update flags on a via.GameObject and children
    static void ForceGOActive(via.GameObject go) {
        ForceGOFlags(go);
        try {
            var tf = go.Transform;
            if (tf != null) ForceChildrenActive(tf);
        } catch { }
    }

    static void ForceGOFlags(via.GameObject go) {
        try {
            IntPtr addr = (IntPtr)(go as IObject).GetAddress();
            Marshal.WriteByte(addr + 0x10, 1); // shouldUpdate
            Marshal.WriteByte(addr + 0x11, 1); // shouldDraw
            Marshal.WriteByte(addr + 0x12, 1); // shouldUpdateSelf
            Marshal.WriteByte(addr + 0x13, 1); // shouldDrawSelf
            Marshal.WriteByte(addr + 0x14, 1); // shouldSelect
        } catch { }
    }

    static void ForceChildrenActive(via.Transform parentTf) {
        try {
            var childTf = parentTf.Child;
            while (childTf != null) {
                var childGO = childTf.GameObject;
                if (childGO != null) ForceGOFlags(childGO);
                ForceChildrenActive(childTf);
                childTf = childTf.Next;
            }
        } catch { }
    }

    /// Recursively search a GO and its Transform children for a component of the given runtime type.
    /// Returns the first match as ManagedObject, or null.
    static ManagedObject FindComponentInChildrenByType(via.GameObject go, object runtimeType, int maxDepth = 8) {
        if (go == null || maxDepth <= 0) return null;
        try {
            var comp = (go as IObject)?.Call("getComponent(System.Type)", runtimeType) as ManagedObject;
            if (comp != null) return comp;
        } catch { }
        try {
            var tf = go.Transform;
            if (tf != null) {
                var childTf = tf.Child;
                while (childTf != null) {
                    var childGO = childTf.GameObject;
                    if (childGO != null) {
                        var found = FindComponentInChildrenByType(childGO, runtimeType, maxDepth - 1);
                        if (found != null) return found;
                    }
                    childTf = childTf.Next;
                }
            }
        } catch { }
        return null;
    }

    /// Diagnostic: dump child GO names and their component types (limited depth)
    static void DumpChildGONames(via.GameObject go, int depth) {
        if (go == null || depth > 3) return;
        string indent = new string(' ', depth * 2);
        try {
            string name = go.Name ?? "?";
            var components = go.Components;
            var sb = new System.Text.StringBuilder();
            sb.Append($"{indent}{name}: ");
            if (components != null) {
                foreach (var c in components) {
                    if (c == null) continue;
                    var td = (c as IObject)?.GetTypeDefinition();
                    sb.Append($"{td?.FullName ?? "?"} ");
                }
            }
            API.LogInfo($"[ZS] TREE {sb}");
        } catch { }
        try {
            var tf = go.Transform;
            if (tf == null) return;
            var childTf = tf.Child;
            while (childTf != null) {
                var childGO = childTf.GameObject;
                if (childGO != null) DumpChildGONames(childGO, depth + 1);
                childTf = childTf.Next;
            }
        } catch { }
    }

    static void ScanAvailableKinds() {
        s_availableKinds.Clear();
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) { s_lastStatus = "No CharacterManager"; return; }

        var seen = new HashSet<string>();

        // 1. Scan enemy context list
        var enemyList = charMgr.EnemyContextList;
        if (enemyList != null) {
            int count = enemyList.Count;
            for (int i = 0; i < count; i++) {
                try {
                    var ctx = enemyList[i];
                    if (ctx == null) continue;
                    string kindStr = KindToString(ctx.KindID);
                    if (!string.IsNullOrEmpty(kindStr)) seen.Add(kindStr);
                } catch { }
            }
        }

        // 2. Scan character pool
        var pool = charMgr.CharacterPool;
        int poolCount = pool?.Count ?? 0;
        for (int i = 0; i < poolCount; i++) {
            try {
                var entry = pool[i];
                if (entry == null) continue;
                var updater = entry.Updater;
                if (updater == null) continue;
                var ctx = updater.Context;
                string kindStr = KindToString(ctx?.KindID);
                if (string.IsNullOrEmpty(kindStr)) kindStr = ExtractKindFromGOName(updater.GameObject?.Name);
                if (!string.IsNullOrEmpty(kindStr)) seen.Add(kindStr);
            } catch { }
        }

        s_availableKinds.AddRange(seen);
        s_availableKinds.Sort();
        s_lastStatus = $"Found {s_availableKinds.Count} enemy types";
        string joined = string.Join(", ", s_availableKinds);
        if (joined != s_lastScanResult) {
            s_lastScanResult = joined;
            API.LogInfo($"[ZS] Scanned: {joined}");
        }
    }

    static void RefreshPlayerInfo() {
        var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
        if (charMgr == null) return;
        s_playerCtx = charMgr.getPlayerContextRefFast();
        if (s_playerCtx == null) return;
        var playerGO = s_playerCtx.GameObject;
        if (playerGO != null)
            s_playerFolder = playerGO.Folder;
    }

    static bool RecycleDeadEnemy(object charMgrObj, app.CharacterKindID kindID) {
        var charMgr = charMgrObj as app.CharacterManager;
        if (charMgr == null) return false;
        var enemyList = charMgr.EnemyContextList;
        int count = enemyList?.Count ?? 0;
        for (int i = 0; i < count; i++) {
            var ctx = enemyList[i];
            if (ctx == null) continue;
            if (ctx.KindID != kindID) continue;
            bool isDead = ctx.IsDead;
            bool isSpawn = ctx.IsSpawn;
            if (!isDead || !isSpawn) continue;
            var upd = ToManaged(ctx.CharacterUpdaterBase as IObject);
            if (upd == null) continue;
            (ctx as IObject)?.Call("unlink");
            ctx.IsSpawn = false;
            var pool = charMgr.CharacterPool;
            int poolCount = pool?.Count ?? 0;
            for (int p = 0; p < poolCount; p++) {
                var entry = pool[p];
                if (entry == null) continue;
                bool used = entry.Used;
                if (!used) continue;
                var eu = entry.Updater as IObject;
                if (eu != null && eu.GetAddress() == upd.GetAddress()) {
                    entry.Used = false;
                    return true;
                }
            }
            return true;
        }
        return false;
    }

    static void DumpCharacterPoolFolders() {
        try {
            var sceneMgrNative = API.GetNativeSingletonT<via.SceneManager>();
            if (sceneMgrNative == null) { s_lastStatus = "No SceneManager"; return; }
            var mainScene = via.SceneManager.MainScene;
            if (mainScene == null) { s_lastStatus = "No main scene"; return; }

            var sb = new System.Text.StringBuilder();
            sb.AppendLine("=== CharacterPool Scene Folders ===");

            string[] prefixes = { "Chap1_", "Chap2_", "Chap3_", "Chap4_", "Chap5_", "Chap6_", "Chap7_", "Chap8_", "Chap9_", "Chap10_" };
            foreach (var prefix in prefixes) {
                for (int n = 0; n <= 30; n++) {
                    string folderName = $"{prefix}{n:D2}_CharacterPool";
                    var folder = mainScene.findFolder(folderName);
                    if (folder == null) continue;

                    bool active = folder.Active;
                    bool standby = folder.Standby;
                    sb.AppendLine($"\n{folderName} (active={active}, standby={standby}):");

                    var child = folder.Child;
                    while (child != null) {
                        string cName = child.Name ?? "?";
                        bool cSF = child.SceneFolder;
                        bool cActive = child.Active;
                        bool cStandby = child.Standby;
                        sb.AppendLine($"  {cName} scene={cSF} active={cActive} standby={cStandby}");
                        child = child.Next;
                    }
                }
            }

            sb.AppendLine("\n=== Pool Entry Kinds ===");
            var charMgr = API.GetManagedSingletonT<app.CharacterManager>();
            if (charMgr != null) {
                var pool = charMgr.CharacterPool;
                int poolCount = pool?.Count ?? 0;
                var kindCounts = new Dictionary<string, int[]>();
                for (int i = 0; i < poolCount; i++) {
                    try {
                        var entry = pool[i];
                        if (entry == null) continue;
                        bool used = entry.Used;
                        var updater = entry.Updater;
                        string kind = "null_updater";
                        if (updater != null) {
                            var ctx = updater.Context;
                            kind = KindToString(ctx?.KindID);
                        }
                        if (!kindCounts.ContainsKey(kind)) kindCounts[kind] = new int[3];
                        kindCounts[kind][0]++;
                        if (used) kindCounts[kind][1]++; else kindCounts[kind][2]++;
                    } catch { }
                }
                foreach (var kv in kindCounts)
                    sb.AppendLine($"  {kv.Key}: {kv.Value[0]} total, {kv.Value[1]} used, {kv.Value[2]} free");
            }

            s_poolDiag = sb.ToString();
            API.LogInfo($"[ZS] {s_poolDiag}");
            s_lastStatus = "Folder dump complete";
        } catch (Exception e) {
            s_lastStatus = $"Dump error: {e.Message}";
            API.LogError($"[ZS] Dump: {e.Message}\n{e.StackTrace}");
        }
    }
}
