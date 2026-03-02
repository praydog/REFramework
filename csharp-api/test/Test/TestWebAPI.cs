using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Threading;
using REFrameworkNET;
using REFrameworkNET.Attributes;

class REFrameworkWebAPI {
    static HttpListener s_listener;
    static Thread s_thread;
    static CancellationTokenSource s_cts = new();
    static int s_port = 8899;
    static string s_webRoot;

    static readonly Dictionary<string, string> s_mimeTypes = new() {
        { ".html", "text/html; charset=utf-8" },
        { ".css",  "text/css; charset=utf-8" },
        { ".js",   "application/javascript; charset=utf-8" },
    };

    [PluginEntryPoint]
    public static void Main() {
        try {
            var pluginDir = API.GetPluginDirectory(typeof(REFrameworkWebAPI).Assembly);
            s_webRoot = Path.Combine(pluginDir, "WebAPI");

            if (!Directory.Exists(s_webRoot)) {
                API.LogError($"[WebAPI] WebAPI folder not found at {s_webRoot}");
                return;
            }

            s_listener = new HttpListener();
            s_listener.Prefixes.Add($"http://localhost:{s_port}/");
            s_listener.Start();

            s_thread = new Thread(ListenLoop) { IsBackground = true };
            s_thread.Start();

            API.LogInfo($"[WebAPI] Listening on http://localhost:{s_port}/ (serving from {s_webRoot})");
        } catch (Exception e) {
            API.LogError("[WebAPI] Failed to start: " + e.Message);
        }
    }

    [PluginExitPoint]
    public static void OnUnload() {
        s_cts.Cancel();
        s_listener?.Stop();
        s_thread?.Join(2000);
        s_listener?.Close();
        API.LogInfo("[WebAPI] Stopped");
    }

    static void ListenLoop() {
        while (!s_cts.IsCancellationRequested) {
            try {
                var ctx = s_listener.GetContext();
                ThreadPool.QueueUserWorkItem(_ => HandleRequest(ctx));
            } catch (HttpListenerException) {
                break;
            } catch (ObjectDisposedException) {
                break;
            }
        }
    }

    static void HandleRequest(HttpListenerContext ctx) {
        if (s_cts.IsCancellationRequested) return;
        bool calledGameApi = false;
        try {
            var path = ctx.Request.Url.AbsolutePath.TrimEnd('/').ToLower();

            // API endpoints
            if (path.StartsWith("/api")) {
                calledGameApi = true;
                var method = ctx.Request.HttpMethod;

                // POST endpoints
                if (method == "POST") {
                    object postResult = path switch {
#if MHWILDS
                        "/api/player/health" => SetPlayerHealth(ctx.Request),
                        "/api/player/position" => SetPlayerPosition(ctx.Request),
                        "/api/meshes" => SetMeshVisibility(ctx.Request),
                        "/api/materials" => SetMaterialVisibility(ctx.Request),
                        "/api/chat" => SendChat(ctx.Request),
#elif RE9
                        "/api/player/health" => SetPlayerHealthRE9(ctx.Request),
#endif
                        "/api/explorer/field" => PostExplorerField(ctx.Request),
                        "/api/explorer/method" => PostExplorerMethod(ctx.Request),
                        "/api/explorer/batch" => PostExplorerBatch(ctx.Request),
                        "/api/explorer/chain" => PostExplorerChain(ctx.Request),
                        _ => null
                    };

                    if (postResult == null) {
                        ctx.Response.StatusCode = 404;
                        WriteJson(ctx.Response, new { error = "Not found" });
                        return;
                    }

                    WriteJson(ctx.Response, postResult);
                    return;
                }

                // Plain-text endpoints (not JSON)
                if (path == "/api/help") {
                    var agentMd = Path.Combine(s_webRoot, "AGENT.md");
                    if (File.Exists(agentMd)) {
                        var text = File.ReadAllText(agentMd);
                        ctx.Response.ContentType = "text/plain; charset=utf-8";
                        ctx.Response.Headers.Add("Access-Control-Allow-Origin", "*");
                        var bytes = Encoding.UTF8.GetBytes(text);
                        ctx.Response.OutputStream.Write(bytes);
                        ctx.Response.Close();
                        return;
                    }
                }

                // GET endpoints
                object result = path switch {
                    "/api" => GetIndex(),
                    "/api/camera" => GetCameraInfo(),
                    "/api/tdb" => GetTDBStats(),
                    "/api/singletons" => GetSingletonList(),
                    "/api/explorer/singletons" => GetExplorerSingletons(),
                    "/api/explorer/object" => GetExplorerObject(ctx.Request),
                    "/api/explorer/summary" => GetExplorerSummary(ctx.Request),
                    "/api/explorer/field" => GetExplorerField(ctx.Request),
                    "/api/explorer/method" => GetExplorerMethod(ctx.Request),
                    "/api/explorer/array" => GetExplorerArray(ctx.Request),
                    "/api/explorer/search" => GetExplorerSearch(ctx.Request),
                    "/api/explorer/type" => GetExplorerType(ctx.Request),
                    "/api/explorer/singleton" => GetExplorerSingleton(ctx.Request),
                    "/api/localize" => ResolveGuid(ctx.Request),
#if MHWILDS
                    "/api/player" => GetPlayerInfo(),
                    "/api/lobby" => GetLobbyMembers(),
                    "/api/weather" => GetWeather(),
                    "/api/equipment" => GetEquipment(),
                    "/api/inventory" => GetInventory(),
                    "/api/meshes" => GetMeshList(),
                    "/api/materials" => GetMaterials(),
                    "/api/map" => GetMapInfo(),
                    "/api/chat" => GetChatHistory(),
                    "/api/huntlog" => GetHuntLog(),
                    "/api/palico" => GetPalicoStats(),
                    "/api/debug/byref" => DebugByRef(),
#elif RE9
                    "/api/player" => GetPlayerInfoRE9(),
                    "/api/enemies" => GetEnemiesRE9(),
                    "/api/gameinfo" => GetGameInfoRE9(),
#endif
                    _ => null
                };

                if (result == null) {
                    ctx.Response.StatusCode = 404;
                    WriteJson(ctx.Response, new { error = "Not found" });
                    return;
                }

                WriteJson(ctx.Response, result);
                return;
            }

            // Static file serving
            ServeFile(ctx, path);
        } catch (ObjectDisposedException) {
            // Listener closed during hot-reload
        } catch (Exception e) {
            try {
                ctx.Response.StatusCode = 500;
                WriteJson(ctx.Response, new { error = e.Message });
            } catch (ObjectDisposedException) { }
        } finally {
            // Clean up thread-local managed objects created by game API calls.
            // Only call when we actually invoked game APIs — calling on a thread
            // that only served static files can crash if there's no frame to GC.
            if (calledGameApi) {
                try { API.LocalFrameGC(); } catch { }
            }
        }
    }

    static void ServeFile(HttpListenerContext ctx, string path) {
        if (path == "" || path == "/") path = "/index.html";

        // Sanitize: only allow filenames directly in WebAPI folder
        var fileName = Path.GetFileName(path);
        var filePath = Path.Combine(s_webRoot, fileName);

        if (!File.Exists(filePath)) {
            ctx.Response.StatusCode = 404;
            WriteJson(ctx.Response, new { error = "Not found" });
            return;
        }

        try {
            var ext = Path.GetExtension(fileName).ToLower();
            ctx.Response.ContentType = s_mimeTypes.GetValueOrDefault(ext, "application/octet-stream");
            var bytes = File.ReadAllBytes(filePath);
            ctx.Response.OutputStream.Write(bytes);
            ctx.Response.Close();
        } catch (ObjectDisposedException) { }
    }

    static void WriteJson(HttpListenerResponse response, object data) {
        try {
            response.ContentType = "application/json";
            response.Headers.Add("Access-Control-Allow-Origin", "*");
            var json = JsonSerializer.SerializeToUtf8Bytes(data, new JsonSerializerOptions { WriteIndented = true });
            response.OutputStream.Write(json);
            response.Close();
        } catch (ObjectDisposedException) {
            // Listener was closed during hot-reload while request was in-flight
        }
    }

    static object GetIndex() {
        var endpoints = new List<string> {
            "/api/camera", "/api/tdb", "/api/singletons", "/api/localize", "/api/help",
            "/api/explorer/singletons", "/api/explorer/singleton",
            "/api/explorer/object", "/api/explorer/summary", "/api/explorer/field", "/api/explorer/method",
            "/api/explorer/array", "/api/explorer/search", "/api/explorer/type",
            "/api/explorer/batch", "/api/explorer/chain"
        };
#if MHWILDS
        endpoints.AddRange(new[] { "/api/player", "/api/lobby", "/api/weather", "/api/equipment",
            "/api/inventory", "/api/meshes", "/api/materials", "/api/map", "/api/chat",
            "/api/huntlog", "/api/palico" });
#elif RE9
        endpoints.AddRange(new[] { "/api/player", "/api/enemies", "/api/gameinfo" });
#endif
        return new {
            name = "REFramework.NET Web API",
            game = System.Diagnostics.Process.GetCurrentProcess().ProcessName,
            endpoints
        };
    }

    static string ResolveGuid(_System.Guid guid) {
        try { return via.gui.message.get(guid)?.ToString(); } catch { return null; }
    }

#if MHWILDS
    static object GetEquipment() {
        try {
            var pm = API.GetManagedSingletonT<app.PlayerManager>();
            if (pm == null) return new { error = "PlayerManager not available" };

            var player = pm.getMasterPlayer();
            if (player == null) return new { error = "Player is null" };

            var createInfo = player.ContextHolder.Hunter.CreateInfo;
            if (createInfo == null) return new { error = "CreateInfo not available" };

            var wpType = (app.WeaponDef.TYPE)createInfo._WpType;
            int wpId = (int)createInfo._WpID;

            string weaponName = null, weaponDesc = null;
            try { weaponName = ResolveGuid(app.WeaponDef.Name(wpType, wpId)); } catch { }
            try { weaponDesc = ResolveGuid(app.WeaponDef.Explain(wpType, wpId)); } catch { }
            if (weaponDesc == null) try { weaponDesc = ResolveGuid(app.WeaponDef.Data(wpType, wpId).Explain); } catch { }

            string wpTypeName = null;
            try { wpTypeName = wpType.ToString(); } catch { }

            int attack = 0, critical = 0, defense = 0, attributeValue = 0, subAttributeValue = 0;
            string attribute = null, subAttribute = null, rarity = null;
            int[] slotLevels = new int[3];
            try { attack = (int)app.WeaponDef.Attack(wpType, wpId); } catch { }
            try { critical = (int)app.WeaponDef.Critical(wpType, wpId); } catch { }
            try { defense = (int)app.WeaponDef.Defense(wpType, wpId); } catch { }
            try { attribute = app.WeaponDef.Attribute(wpType, wpId).ToString(); } catch { }
            try { attributeValue = (int)app.WeaponDef.AttributeValue(wpType, wpId); } catch { }
            try { subAttribute = app.WeaponDef.SubAttribute(wpType, wpId).ToString(); } catch { }
            try { subAttributeValue = (int)app.WeaponDef.SubAttributeValue(wpType, wpId); } catch { }
            try { rarity = app.WeaponDef.Rare(wpType, wpId).ToString(); } catch { }
            try {
                for (uint s = 0; s < 3; s++)
                    slotLevels[s] = (int)app.WeaponDef.SlotLevel(wpType, wpId, s);
            } catch { }

            var armorIds = new int[5];
            var armorLevels = new uint[5];
            try {
                var seriesArr = createInfo._ArmorSeriesID;
                var levelArr = createInfo._ArmorUpgradeLevel;
                for (int i = 0; i < 5; i++) {
                    armorIds[i] = seriesArr[i];
                    armorLevels[i] = levelArr[i];
                }
            } catch { }

            string[] armorSlotNames = { "Helm", "Body", "Arms", "Waist", "Legs" };
            var armorPieces = new List<object>();
            for (int i = 0; i < 5; i++) {
                string name = null, desc = null;
                if (armorIds[i] > 0) {
                    var parts = (app.ArmorDef.ARMOR_PARTS)i;
                    var series = (app.ArmorDef.SERIES)armorIds[i];
                    try { name = ResolveGuid(app.ArmorDef.Name(parts, series)); } catch { }
                    try { desc = ResolveGuid(app.ArmorDef.Explain(parts, series)); } catch { }
                }
                armorPieces.Add(new {
                    slot = armorSlotNames[i],
                    name = name ?? (armorIds[i] > 0 ? $"Series {armorIds[i]}" : "Empty"),
                    description = desc,
                    seriesId = armorIds[i],
                    upgradeLevel = armorLevels[i]
                });
            }

            // Palico equipment
            object palico = null;
            try {
                var om = API.GetManagedSingletonT<app.OtomoManager>();
                var otomoInfo = om?.getMasterOtomoInfo();
                if (otomoInfo != null && otomoInfo.Valid) {
                    var otomoCtx = otomoInfo.ContextHolder?.Otomo?.CreateInfo;
                    if (otomoCtx != null && otomoCtx._IsValid) {
                        var wpDataId = (app.OtEquipDef.EQUIP_DATA_ID)otomoCtx._WeaponDataId;
                        var helmDataId = (app.OtEquipDef.EQUIP_DATA_ID)otomoCtx._HeadDataId;
                        var bodyDataId = (app.OtEquipDef.EQUIP_DATA_ID)otomoCtx._ArmorDataId;

                        string wpName = null, helmName = null, bodyName = null;
                        string wpDesc = null, helmDesc = null, bodyDesc = null;
                        string wpRare = null, helmRare = null, bodyRare = null;
                        try {
                            var vdm = API.GetManagedSingletonT<app.VariousDataManager>();
                            var otWpData = vdm?.Setting?.EquipDatas?.OtomoWeaponData;
                            if (otWpData != null) {
                                var idx = vdm.Setting.EquipDatas.OtomoWeaponDataIndex[(int)wpDataId];
                                if (idx >= 0) {
                                    var cData = otWpData.getDataByIndex(idx);
                                    if (cData != null) {
                                        wpName = ResolveGuid(cData.Name);
                                        wpDesc = ResolveGuid(cData.Explain);
                                    }
                                }
                            }
                        } catch { }
                        if (wpName == null) try { wpName = ResolveGuid(app.OtEquipDef.Name(wpDataId)); } catch { }
                        try {
                            var d = app.OtEquipDef.Data(app.OtEquipDef.EQUIP_TYPE.HELM, helmDataId);
                            helmName = ResolveGuid(d.Name);
                            try { helmDesc = ResolveGuid(d.Explain); } catch { }
                        } catch { }
                        try {
                            var d = app.OtEquipDef.Data(app.OtEquipDef.EQUIP_TYPE.BODY, bodyDataId);
                            bodyName = ResolveGuid(d.Name);
                            try { bodyDesc = ResolveGuid(d.Explain); } catch { }
                        } catch { }
                        try { wpRare = app.OtEquipDef.Rare(wpDataId).ToString(); } catch { }
                        try { helmRare = app.OtEquipDef.Rare(helmDataId).ToString(); } catch { }
                        try { bodyRare = app.OtEquipDef.Rare(bodyDataId).ToString(); } catch { }

                        palico = new {
                            weapon = new { name = wpName ?? $"Weapon {otomoCtx._WeaponDataId}", description = wpDesc, rarity = wpRare },
                            helm = new { name = helmName ?? $"Helm {otomoCtx._HeadDataId}", description = helmDesc, rarity = helmRare },
                            body = new { name = bodyName ?? $"Body {otomoCtx._ArmorDataId}", description = bodyDesc, rarity = bodyRare }
                        };
                    }
                }
            } catch { }

            return new {
                weapon = new {
                    name = weaponName ?? $"Weapon {wpId}",
                    description = weaponDesc,
                    type = wpTypeName ?? wpType.ToString(),
                    typeId = (int)wpType,
                    id = wpId,
                    attack,
                    critical,
                    defense,
                    element = attribute,
                    elementValue = attributeValue,
                    subElement = subAttribute,
                    subElementValue = subAttributeValue,
                    rarity,
                    slots = slotLevels
                },
                armor = armorPieces,
                palico
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static readonly Dictionary<string, string> s_stageNames = new() {
        { "ST101", "Windward Plains" },
        { "ST102", "Scarlet Forest" },
        { "ST103", "Oilwell Basin" },
        { "ST104", "Iceshard Cliffs" },
        { "ST105", "Wounded Hollow" },
        { "ST201", "Training Area" },
        { "ST202", "Arena" },
        { "ST203", "Forlorn Arena" },
        { "ST204", "Special Arena" },
        { "ST401", "Kunafa" },
        { "ST402", "Research Base" },
        { "ST403", "Aslana" },
        { "ST404", "Capcom HQ" },
        { "ST405", "Elder's Lair" },
        { "ST502", "Hub" },
        { "ST503", "Gathering Hub" },
    };

    static object GetMapInfo() {
        try {
            var fm = API.GetManagedSingletonT<app.MasterFieldManager>();
            if (fm == null) return new { error = "MasterFieldManager not available" };

            var currentStage = fm.CurrentStage;
            var prevStage = fm.PrevStage;
            var stageCode = currentStage.ToString().Split(' ')[0]; // "ST502 (14)" -> "ST502"
            var prevCode = prevStage.ToString().Split(' ')[0];

            s_stageNames.TryGetValue(stageCode, out var stageName);
            s_stageNames.TryGetValue(prevCode, out var prevName);

            // Quest info from MissionManager
            var mm = API.GetManagedSingletonT<app.MissionManager>();
            bool isQuest = false;
            bool isPlaying = false;
            string questId = null;
            float? questRemainTime = null;
            float? questElapsedTime = null;
            string questBeforeStage = null;

            if (mm != null) {
                try { isQuest = mm.IsActiveQuest; } catch { }
                try { isPlaying = mm.IsPlayingQuest; } catch { }
                try { questId = mm.AcceptedQuestID.ToString(); } catch { }

                if (isQuest) {
                    try {
                        var qd = mm.QuestDirector;
                        if (qd != null) {
                            try { questRemainTime = qd.QuestRemainTime; } catch { }
                            try { questElapsedTime = qd.QuestElapsedTime; } catch { }
                            try {
                                var bs = qd.QuestBeforeStage.ToString().Split(' ')[0];
                                s_stageNames.TryGetValue(bs, out questBeforeStage);
                                questBeforeStage = questBeforeStage ?? bs;
                            } catch { }
                        }
                    } catch { }
                }
            }

            return new {
                stage = stageCode,
                stageName = stageName ?? stageCode,
                areaNo = fm._CurrentAreaNo,
                prevStage = prevCode,
                prevStageName = prevName ?? prevCode,
                quest = new {
                    active = isQuest,
                    playing = isPlaying,
                    id = questId,
                    remainTime = questRemainTime,
                    elapsedTime = questElapsedTime,
                    beforeStage = questBeforeStage
                }
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetInventory() {
        try {
            var sdm = API.GetManagedSingletonT<app.SaveDataManager>();
            if (sdm == null) return new { error = "SaveDataManager not available" };

            var saves = sdm.UserSaveData;
            if (saves == null) return new { error = "No save data" };

            app.savedata.cUserSaveParam activeSave = null;
            for (int i = 0; i < saves.Length; i++) {
                if (saves[i] != null && saves[i].Active == 1) { activeSave = saves[i]; break; }
            }
            if (activeSave == null) return new { error = "No active save" };

            var itemParam = activeSave._Item;
            if (itemParam == null) return new { error = "Item data not available" };

            var pouch = itemParam._PouchItem;
            if (pouch == null) return new { error = "Pouch not available" };

            var items = new List<object>();
            for (int i = 0; i < pouch.Length; i++) {
                var slot = pouch[i];
                if (slot == null) continue;

                int num = (int)slot.Num;
                if (num <= 0) continue;

                app.ItemDef.ID itemId;
                try { itemId = slot.ItemId; } catch { itemId = (app.ItemDef.ID)slot.ItemIdFixed; }

                string name = null;
                try { name = app.ItemDef.NameString(itemId); } catch { }

                items.Add(new {
                    slotIndex = i,
                    id = (int)itemId,
                    name = name ?? $"Item {(int)itemId}",
                    quantity = num
                });
            }

            return new { count = items.Count, capacity = pouch.Length, items };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetPlayerInfo() {
        var pm = API.GetManagedSingletonT<app.PlayerManager>();
        if (pm == null) return new { error = "PlayerManager not available" };

        var player = pm.getMasterPlayer();
        if (player == null) return new { error = "Player is null" };

        var ctx = player.ContextHolder;
        var pl = ctx.Pl;

        float? posX = null, posY = null, posZ = null;
        try {
            var go = player.Object;
            if (go != null) {
                var tf = go.Transform;
                if (tf != null) {
                    var pos = tf.Position;
                    posX = pos.x; posY = pos.y; posZ = pos.z;
                }
            }
        } catch { }

        float? health = null, maxHealth = null;
        try {
            var hm = ctx.Chara.HealthManager;
            if (hm != null) {
                health = hm._Health.read();
                maxHealth = hm._MaxHealth.read();
            }
        } catch { }

        string otomoName = null, seikretName = null;
        int? zenny = null, points = null;
        uint? playTime = null;
        try {
            var sdm = API.GetManagedSingletonT<app.SaveDataManager>();
            if (sdm != null) {
                var saves = sdm.UserSaveData;
                if (saves != null) {
                    for (int i = 0; i < saves.Length; i++) {
                        if (saves[i] != null && saves[i].Active == 1) {
                            var basic = saves[i]._BasicData;
                            if (basic != null) {
                                try { otomoName = basic.OtomoName?.ToString(); } catch { }
                                try { seikretName = basic.SeikretName?.ToString(); } catch { }
                                try { zenny = basic.getMoney(); } catch { }
                                try { points = basic.getPoint(); } catch { }
                            }
                            try { playTime = saves[i].PlayTime; } catch { }
                            break;
                        }
                    }
                }
            }
        } catch { }

        return new {
            name = pl._PlayerName?.ToString(),
            level = (int)pl._CurrentStage,
            health,
            maxHealth,
            zenny,
            points,
            playTimeSeconds = playTime,
            position = new { x = posX, y = posY, z = posZ },
            generalPos = new { x = pl._GeneralPos.x, y = pl._GeneralPos.y, z = pl._GeneralPos.z },
            distToCamera = pl._DistToCamera,
            isMasterRow = pl._NetMemberInfo.IsMasterRow,
            otomoName,
            seikretName
        };
    }

    static object SetPlayerHealth(HttpListenerRequest request) {
        using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
        var body = reader.ReadToEnd();
        var doc = JsonDocument.Parse(body);
        var value = doc.RootElement.GetProperty("value").GetSingle();

        var pm = API.GetManagedSingletonT<app.PlayerManager>();
        if (pm == null) return new { error = "PlayerManager not available" };

        var player = pm.getMasterPlayer();
        if (player == null) return new { error = "Player is null" };

        var hm = player.ContextHolder.Chara.HealthManager;
        if (hm == null) return new { error = "HealthManager not available" };

        hm._Health.write(value);
        return new { ok = true, health = value };
    }

    static object SetPlayerPosition(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var pm = API.GetManagedSingletonT<app.PlayerManager>();
            if (pm == null) return new { error = "PlayerManager not available" };

            var player = pm.getMasterPlayer();
            if (player == null) return new { error = "Player is null" };

            var go = player.Object;
            if (go == null) return new { error = "GameObject is null" };

            var tf = go.Transform;
            if (tf == null) return new { error = "Transform is null" };

            var pos = tf.Position;

            if (root.TryGetProperty("x", out var xProp)) pos.x = xProp.GetSingle();
            if (root.TryGetProperty("y", out var yProp)) pos.y = yProp.GetSingle();
            if (root.TryGetProperty("z", out var zProp)) pos.z = zProp.GetSingle();

            tf.Position = pos;

            return new { ok = true, position = new { x = pos.x, y = pos.y, z = pos.z } };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static readonly Dictionary<string, string> s_meshLabels = new() {
        { "Player_Face", "Face" },
        { "SlingerRope", "Slinger Rope" },
        { "HeadToHip", "Head-to-Hip" },
    };

    static string GuessMeshLabel(string name) {
        if (s_meshLabels.TryGetValue(name, out var label)) return label;
        if (name.StartsWith("Acc")) return "Accessory";
        if (name.StartsWith("ch02_")) return "Body";
        if (name.StartsWith("Wp_")) return "Weapon";
        if (name.StartsWith("WpSub_")) return "Sub Weapon";
        return name;
    }

    static List<(via.GameObject go, string name)> GetPlayerChildObjects() {
        var pm = API.GetManagedSingletonT<app.PlayerManager>();
        if (pm == null) return null;
        var player = pm.getMasterPlayer();
        if (player == null) return null;
        var go = player.Object;
        if (go == null) return null;
        var tf = go.Transform;
        if (tf == null) return null;

        var children = new List<(via.GameObject, string)>();
        var child = tf.Child;
        while (child != null) {
            try {
                var childGo = child.GameObject;
                if (childGo != null) {
                    children.Add((childGo, childGo.Name));
                }
            } catch { }
            try { child = child.Next; } catch { break; }
        }
        return children;
    }

    static object GetMeshList() {
        try {
            var children = GetPlayerChildObjects();
            if (children == null) return new { error = "Player not available" };

            var meshes = new List<object>();
            foreach (var (go, name) in children) {
                try {
                    meshes.Add(new {
                        name,
                        label = GuessMeshLabel(name),
                        visible = go.DrawSelf
                    });
                } catch { }
            }
            return new { count = meshes.Count, meshes };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object SetMeshVisibility(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var targetName = root.GetProperty("name").GetString();
            var visible = root.GetProperty("visible").GetBoolean();

            var children = GetPlayerChildObjects();
            if (children == null) return new { error = "Player not available" };

            foreach (var (go, name) in children) {
                if (name == targetName) {
                    go.DrawSelf = visible;
                    return new { ok = true, name, visible };
                }
            }
            return new { error = $"Mesh '{targetName}' not found" };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static REFrameworkNET.IObject GetPlayerMeshSettingController() {
        try {
            var pm = API.GetManagedSingletonT<app.PlayerManager>();
            if (pm == null) return null;
            var player = pm.getMasterPlayer();
            if (player == null) return null;
            var go = player.Object;
            if (go == null) return null;

            var components = go.Components;
            if (components == null) return null;

            for (int i = 0; i < components.Length; i++) {
                var comp = components[i];
                if (comp == null) continue;
                var iobj = comp as REFrameworkNET.IObject;
                if (iobj == null) continue;
                var tname = iobj.GetTypeDefinition()?.GetFullName();
                if (tname == "app.MeshSettingController")
                    return iobj;
            }
            return null;
        } catch {
            return null;
        }
    }

    static List<(REFrameworkNET.IObject meshSetting, string goName)> GetPlayerMeshSettings() {
        var ctrl = GetPlayerMeshSettingController();
        if (ctrl == null) return null;

        // get_MeshSettingsAll() returns an IEnumerable (C# iterator state machine).
        // Must call the explicit interface GetEnumerator to get a properly initialized enumerator.
        var enumerable = ctrl.Call("get_MeshSettingsAll") as REFrameworkNET.IObject;
        if (enumerable == null) return null;

        var enumerator = enumerable.Call("System.Collections.IEnumerable.GetEnumerator") as REFrameworkNET.IObject;
        if (enumerator == null) return null;

        var results = new List<(REFrameworkNET.IObject, string)>();
        int safety = 0;
        while (safety++ < 100) {
            object moveResult = null;
            try { moveResult = enumerator.Call("MoveNext"); } catch { break; }
            if (moveResult == null || !(bool)moveResult) break;

            var msObj = enumerator.Call("System.Collections.IEnumerator.get_Current") as REFrameworkNET.IObject;
            if (msObj == null) continue;

            string goName = "";
            try {
                var goObj = msObj.Call("get_GameObject") as REFrameworkNET.IObject;
                if (goObj != null) goName = goObj.Call("get_Name") as string ?? "";
            } catch { }

            results.Add((msObj, goName));
        }
        return results;
    }

    static object GetMaterials() {
        try {
            var meshSettings = GetPlayerMeshSettings();
            if (meshSettings == null) return new { error = "MeshSettingController not available" };

            var meshSettingsList = new List<object>();
            foreach (var (msObj, goName) in meshSettings) {
                try {
                    var meshObj = msObj.Call("get_Mesh") as REFrameworkNET.IObject;
                    if (meshObj == null) continue;

                    uint matNum = 0;
                    try { matNum = (uint)meshObj.Call("get_MaterialNum"); } catch { }

                    var materials = new List<object>();
                    for (uint j = 0; j < matNum; j++) {
                        string matName = null;
                        try { matName = meshObj.Call("getMaterialName", (object)j) as string; } catch { }

                        bool matEnabled = true;
                        try { matEnabled = (bool)meshObj.Call("getMaterialsEnable", (object)(ulong)j); } catch { }

                        materials.Add(new {
                            index = j,
                            name = matName ?? $"Material {j}",
                            enabled = matEnabled
                        });
                    }

                    bool visible = true;
                    try { visible = (bool)msObj.Call("get_Visible"); } catch { }

                    meshSettingsList.Add(new {
                        gameObject = goName,
                        label = GuessMeshLabel(goName),
                        materialCount = matNum,
                        visible,
                        materials
                    });
                } catch { }
            }
            return new { count = meshSettingsList.Count, meshSettings = meshSettingsList };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object SetMaterialVisibility(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var targetGo = root.GetProperty("gameObject").GetString();
            var materialIndex = root.GetProperty("materialIndex").GetUInt32();
            var enabled = root.GetProperty("enabled").GetBoolean();

            var meshSettings = GetPlayerMeshSettings();
            if (meshSettings == null) return new { error = "MeshSettingController not available" };

            foreach (var (msObj, goName) in meshSettings) {
                if (goName == targetGo) {
                    var meshObj = msObj.Call("get_Mesh") as REFrameworkNET.IObject;
                    if (meshObj == null) return new { error = "No mesh on this MeshSetting" };
                    meshObj.Call("setMaterialsEnable", (object)(ulong)materialIndex, (object)enabled);
                    return new { ok = true, gameObject = targetGo, materialIndex, enabled };
                }
            }
            return new { error = $"MeshSetting for '{targetGo}' not found" };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object SendChat(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var message = root.GetProperty("message").GetString();
            if (string.IsNullOrEmpty(message)) return new { error = "message is required" };

            var chatMgr = API.GetManagedSingletonT<app.ChatManager>();
            if (chatMgr == null) return new { error = "ChatManager not available" };

            var iobj = chatMgr as REFrameworkNET.IObject;
            if (iobj == null) return new { error = "Cannot get IObject for ChatManager" };

            iobj.Call("sendText", (object)message);

            return new { ok = true, message };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }
#endif

    static object ResolveGuid(HttpListenerRequest req) {
        try {
            var guidStr = req.QueryString["guid"];
            if (string.IsNullOrEmpty(guidStr)) return new { error = "Missing 'guid' parameter" };

            // Support comma-separated GUIDs for batch resolution
            var guids = guidStr.Split(',');
            if (guids.Length == 1) {
                var text = via.gui.message.get(_System.Guid.Parse(guidStr.Trim()))?.ToString();
                return new { guid = guidStr, text = text ?? "" };
            }

            var results = new List<object>();
            foreach (var g in guids) {
                var trimmed = g.Trim();
                try {
                    var text = via.gui.message.get(_System.Guid.Parse(trimmed))?.ToString();
                    results.Add(new { guid = trimmed, text = text ?? "" });
                } catch {
                    results.Add(new { guid = trimmed, text = "", error = "Failed to parse" });
                }
            }
            return new { count = results.Count, results };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

#if MHWILDS
    static object DebugByRef() {
        try {
            var tdef = REFrameworkNET.TDB.Get().FindType("ace.cFixedRingBuffer`1<app.ChatDef.MessageElement>");
            if (tdef == null) return new { error = "Type not found" };

            var methods = tdef.GetMethods();
            var results = new List<object>();
            foreach (var m in methods) {
                var name = m.GetName();
                if (name == "get_Item" || name == "front" || name == "back" || name == "get_Size") {
                    var retType = m.GetReturnType();
                    results.Add(new {
                        method = name,
                        returnType = retType?.GetFullName(),
                        isByRef = retType?.IsByRef(),
                        isValueType = retType?.IsValueType(),
                        isPointer = retType?.IsPointer(),
                    });
                }
            }
            return new { results };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetChatHistory() {
        try {
            var chatMgr = API.GetManagedSingletonT<app.ChatManager>();
            if (chatMgr is null) return new { error = "ChatManager not available" };

            var logObj = (chatMgr as REFrameworkNET.IObject)?.GetField("_AllLog") as REFrameworkNET.IObject;
            if (logObj is null) return new { error = "_AllLog is null" };

            int size = 0;
            try { var s = logObj.Call("get_Size"); if (s != null) size = (int)s; } catch { }

            // Cache via.gui.message.get(Guid) for resolving system message GUIDs
            REFrameworkNET.Method messageGetMethod = null;
            try {
                var msgTdef = REFrameworkNET.TDB.Get().FindType("via.gui.message");
                messageGetMethod = msgTdef?.GetMethod("get");
            } catch { }

            var messages = new List<object>();

            for (int i = 0; i < size; i++) {
                try {
                    var elem = logObj.Call("get_Item", (object)i) as REFrameworkNET.IObject;
                    if (elem is null) continue;

                    string typeName = null;
                    try { typeName = elem.GetTypeDefinition()?.GetFullName(); } catch { }

                    string msgType = null;
                    try { msgType = elem.GetField("<MsgType>k__BackingField")?.ToString(); } catch { }

                    string text = null, sender = null, target = null;
                    bool isChatBase = typeName != null && (
                        typeName.Contains("ChatBase") || typeName.Contains("ChatMessage") ||
                        typeName.Contains("ChatSystemLog") || typeName.Contains("ChatSystemSendLog"));

                    if (isChatBase) {
                        try { sender = elem.GetField("_SenderName") as string; } catch { }
                        try { target = elem.GetField("<SendTarget>k__BackingField")?.ToString(); } catch { }
                    }

                    // ChatMessage has <Text>k__BackingField for user-typed text
                    if (typeName != null && typeName.Contains("ChatMessage")) {
                        try { text = elem.GetField("<Text>k__BackingField") as string; } catch { }
                    }

                    // If no direct text, try resolving the MessageInfo GUID to localized text
                    if (string.IsNullOrEmpty(text)) {
                        try {
                            var msgInfo = elem.GetField("<MessageInfo>k__BackingField") as REFrameworkNET.IObject;
                            if (msgInfo != null && messageGetMethod != null) {
                                var msgId = msgInfo.GetField("<MsgID>k__BackingField");
                                if (msgId != null) {
                                    text = messageGetMethod.InvokeBoxed(typeof(string), null, new object[] { msgId }) as string;
                                }

                                // Substitute {0}, {1}, etc. with paramToString() results
                                if (!string.IsNullOrEmpty(text) && text.Contains("{0}")) {
                                    try {
                                        var paramArray = msgInfo.Call("paramToString") as REFrameworkNET.IObject;
                                        if (paramArray != null) {
                                            var arrTdef = paramArray.GetTypeDefinition();
                                            int len = 0;
                                            try { len = (int)paramArray.Call("get_Length"); } catch { }
                                            for (int p = 0; p < len; p++) {
                                                var paramVal = paramArray.Call("Get", (object)p);
                                                var paramStr = (paramVal as REFrameworkNET.IObject)?.Call("ToString") as string
                                                    ?? paramVal?.ToString() ?? "";
                                                text = text.Replace($"{{{p}}}", paramStr);
                                            }
                                        }
                                    } catch { }
                                }

                                // Strip markup tags: <BOLD>x</BOLD> -> x, <PLURAL n "singular" "plural"> -> pick by n
                                if (!string.IsNullOrEmpty(text)) {
                                    text = System.Text.RegularExpressions.Regex.Replace(text, @"<BOLD>(.*?)</BOLD>", "$1");
                                    text = System.Text.RegularExpressions.Regex.Replace(text, @"<PLURAL\s+(\d+)\s+""([^""]*)""\s+""([^""]*)"">",
                                        m => m.Groups[1].Value == "1" ? m.Groups[2].Value : m.Groups[3].Value);
                                    text = text.Replace("\r\n", " ");
                                }
                            }
                        } catch { }
                    }

                    messages.Add(new {
                        type = msgType ?? "unknown",
                        sender = sender ?? "System",
                        text = text ?? "",
                        target,
                        elementType = typeName
                    });
                } catch {
                    // Skip this element
                }
            }

            return new { count = messages.Count, messages };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── Hunt Log ──────────────────────────────────────────────────────

    // Cache FixedId → monster name, built once from EnemyDef.ID enum
    static Dictionary<int, string> s_monsterNames;

    static Dictionary<int, string> GetMonsterNameMap() {
        if (s_monsterNames != null) return s_monsterNames;
        var map = new Dictionary<int, string>();
        try {
            for (int id = 0; id < 120; id++) {
                try {
                    var eid = (app.EnemyDef.ID)id;
                    if (!app.EnemyDef.isBossID(eid)) continue;
                    var fixedId = (int)app.EnemyDef.enemyId(eid);
                    var name = app.EnemyDef.NameString(eid, 0, 0);
                    if (!string.IsNullOrEmpty(name) && fixedId != 0)
                        map[fixedId] = name;
                } catch { }
            }
        } catch { }
        if (map.Count > 0) s_monsterNames = map;
        return map;
    }

    static object GetHuntLog() {
        try {
            var sdm = API.GetManagedSingletonT<app.SaveDataManager>();
            if (sdm == null) return new { error = "SaveDataManager not available" };

            var saves = sdm.UserSaveData;
            if (saves == null) return new { error = "No save data" };

            app.savedata.cUserSaveParam activeSave = null;
            for (int i = 0; i < saves.Length; i++) {
                if (saves[i] != null && saves[i].Active == 1) { activeSave = saves[i]; break; }
            }
            if (activeSave == null) return new { error = "No active save" };

            var report = activeSave._EnemyReport;
            if (report == null) return new { error = "EnemyReport not available" };

            var bossArr = report._Boss;
            if (bossArr == null) return new { error = "Boss report array not available" };

            var nameMap = GetMonsterNameMap();
            var monsters = new List<object>();

            for (int i = 0; i < bossArr.Length; i++) {
                var boss = bossArr[i];
                if (boss == null) continue;

                int hunt = 0, slay = 0, capture = 0;
                try { hunt = boss.getHuntingNum(); } catch { }
                try { slay = boss.getSlayingNum(); } catch { }
                try { capture = boss.getCaptureNum(); } catch { }

                if (hunt == 0 && slay == 0 && capture == 0) continue;

                int fixedId = boss.FixedId;
                string name = null;
                nameMap.TryGetValue(fixedId, out name);

                monsters.Add(new {
                    fixedId,
                    name = name ?? $"Monster {fixedId}",
                    huntCount = hunt,
                    slayCount = slay,
                    captureCount = capture,
                    totalCount = hunt + capture
                });
            }

            return new { count = monsters.Count, monsters };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── Palico Stats ────────────────────────────────────────────────────

    static object GetPalicoStats() {
        try {
            var om = API.GetManagedSingletonT<app.OtomoManager>();
            if (om == null) return new { error = "OtomoManager not available" };

            var otomoInfo = om.getMasterOtomoInfo();
            if (otomoInfo == null || !otomoInfo.Valid) return new { error = "No palico info" };

            var ctxHolder = otomoInfo.ContextHolder;
            if (ctxHolder == null) return new { error = "No context holder" };

            var otomoCtx = ctxHolder.Otomo;
            if (otomoCtx == null) return new { error = "No otomo context" };

            int? level = null;
            float? hp = null, maxHp = null;

            var statusMgr = otomoCtx.StatusManager;
            if (statusMgr != null) {
                try { level = statusMgr._Level; } catch { }

                try {
                    var healthMgr = statusMgr._HealthManager;
                    if (healthMgr != null) {
                        try { hp = healthMgr.Health; } catch { }
                        try { maxHp = healthMgr.MaxHealth; } catch { }
                    }
                } catch { }
            }

            uint? attackMelee = null, attackRange = null, attributeValue = null;
            int? defense = null, critical = null;
            string attribute = null;

            var paramMgr = statusMgr?.OtomoStatusParamManager;
            if (paramMgr != null) {
                try { attackMelee = paramMgr._Attack_Melee; } catch { }
                try { attackRange = paramMgr._Attack_Range; } catch { }
                try { defense = paramMgr._Defence; } catch { }
                try { critical = paramMgr._Critical; } catch { }
                try { attribute = paramMgr._Attribute.ToString(); } catch { }
                try { attributeValue = paramMgr._AttributeValue; } catch { }
            }

            return new {
                level,
                health = hp,
                maxHealth = maxHp,
                attack = attackMelee,
                rangedAttack = attackRange,
                defense,
                critical,
                element = attribute,
                elementValue = attributeValue
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }
#endif

    static object GetCameraInfo() {
        try {
            var camera = via.SceneManager.MainView.PrimaryCamera;
            if (camera == null) return new { error = "No primary camera" };

            var tf = camera.GameObject.Transform;
            var pos = tf.Position;

            return new {
                position = new { x = pos.x, y = pos.y, z = pos.z },
                fov = camera.FOV,
                nearClip = camera.NearClipPlane,
                farClip = camera.FarClipPlane
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetTDBStats() {
        var tdb = TDB.Get();
        return new {
            types = tdb.GetNumTypes(),
            methods = tdb.GetNumMethods(),
            fields = tdb.GetNumFields(),
            properties = tdb.GetNumProperties(),
            stringsKB = tdb.GetStringsSize() / 1024,
            rawDataKB = tdb.GetRawDataSize() / 1024
        };
    }

    static object GetSingletonList() {
        var singletons = API.GetManagedSingletons();
        singletons.RemoveAll(s => s.Instance == null);

        var list = new List<object>();
        foreach (var desc in singletons) {
            var instance = desc.Instance;
            var tdef = instance.GetTypeDefinition();
            list.Add(new {
                type = tdef.GetFullName(),
                address = "0x" + instance.GetAddress().ToString("X"),
                methods = (int)tdef.GetNumMethods(),
                fields = (int)tdef.GetNumFields()
            });
        }

        return new { count = list.Count, singletons = list };
    }

    // ── Explorer helpers ──────────────────────────────────────────────

    static readonly TypeDefinition s_systemArrayT = TDB.Get().GetType("System.Array");

    static IObject ResolveObject(HttpListenerRequest request) {
        var qs = request.QueryString;
        var addressStr = qs["address"];
        var kind = qs["kind"];
        var typeName = qs["typeName"];

        if (string.IsNullOrEmpty(addressStr) || string.IsNullOrEmpty(kind) || string.IsNullOrEmpty(typeName))
            return null;

        ulong address = 0;
        if (addressStr.StartsWith("0x") || addressStr.StartsWith("0X"))
            address = Convert.ToUInt64(addressStr.Substring(2), 16);
        else
            address = Convert.ToUInt64(addressStr, 16);

        if (address == 0) return null;

        if (kind == "managed") {
            return ManagedObject.ToManagedObject(address);
        } else if (kind == "native") {
            var tdef = TDB.Get().GetType(typeName);
            if (tdef == null) return null;
            return new NativeObject(address, tdef);
        }

        return null;
    }

    static string ReadFieldValueAsString(IObject obj, Field field, TypeDefinition ft, bool isValueType = false) {
        var finalName = ft.IsEnum() ? ft.GetUnderlyingType().GetFullName() : ft.GetFullName();

        object fieldData = null;
        switch (finalName) {
            case "System.Byte":
                fieldData = field.GetDataT<byte>(obj.GetAddress(), isValueType);
                break;
            case "System.SByte":
                fieldData = field.GetDataT<sbyte>(obj.GetAddress(), isValueType);
                break;
            case "System.Int16":
                fieldData = field.GetDataT<short>(obj.GetAddress(), isValueType);
                break;
            case "System.UInt16":
                fieldData = field.GetDataT<ushort>(obj.GetAddress(), isValueType);
                break;
            case "System.Int32":
                fieldData = field.GetDataT<int>(obj.GetAddress(), isValueType);
                break;
            case "System.UInt32":
                fieldData = field.GetDataT<uint>(obj.GetAddress(), isValueType);
                break;
            case "System.Int64":
                fieldData = field.GetDataT<long>(obj.GetAddress(), isValueType);
                break;
            case "System.UInt64":
                fieldData = field.GetDataT<ulong>(obj.GetAddress(), isValueType);
                break;
            case "System.Single":
                fieldData = field.GetDataT<float>(obj.GetAddress(), isValueType);
                break;
            case "System.Boolean":
                fieldData = field.GetDataT<bool>(obj.GetAddress(), isValueType);
                break;
            case "System.String": {
                try {
                    var strObj = field.GetDataBoxed(obj.GetAddress(), isValueType);
                    return strObj?.ToString();
                } catch {
                    return null;
                }
            }
            case "System.Guid": {
                // Read GUID components from field memory
                var addr = obj.GetAddress() + (isValueType ? field.GetOffsetFromFieldPtr() : field.GetOffsetFromBase());
                var guidObj = new NativeObject(addr, ft);
                try {
                    object guidStr = null;
                    guidObj.HandleInvokeMember_Internal(
                        ft.GetMethod("ToString()"), null, ref guidStr);
                    return guidStr?.ToString();
                } catch {
                    return null;
                }
            }
            default:
                return null;
        }

        if (fieldData == null) return null;

        if (ft.IsEnum()) {
            long longValue = Convert.ToInt64(fieldData);
            try {
                var boxedEnum = _System.Enum.InternalBoxEnum(ft.GetRuntimeType().As<_System.RuntimeType>(), longValue);
                return (boxedEnum as IObject).Call("ToString()") + " (" + fieldData.ToString() + ")";
            } catch {
                return fieldData.ToString();
            }
        }

        return fieldData.ToString();
    }

    // Read a ValueType's contents inline since its address is ephemeral (GC-managed).
    // Returns a serializable object with the value type's fields, or a string for known types.
    static object ReadValueTypeInline(IObject vtObj) {
        var tdef = vtObj.GetTypeDefinition();
        var fullName = tdef.GetFullName();

        // For known types, just call ToString() to get a clean representation
        switch (fullName) {
            case "System.Guid": {
                try {
                    object str = null;
                    vtObj.HandleInvokeMember_Internal(tdef.GetMethod("ToString()"), null, ref str);
                    return new { isValueType = true, typeName = fullName, value = str?.ToString() };
                } catch {
                    return new { isValueType = true, typeName = fullName, value = (string)null };
                }
            }
        }

        // Generic value type: read all instance fields using fieldptr offsets (no 0x10 header)
        var fields = new Dictionary<string, object>();
        for (var t = tdef; t != null; t = t.ParentType) {
            foreach (var field in t.GetFields()) {
                if (field.IsStatic()) continue;
                var fname = field.GetName();
                if (fields.ContainsKey(fname)) continue;
                var ft = field.GetType();
                if (ft == null) continue;
                try {
                    if (ft.IsValueType()) {
                        fields[fname] = ReadFieldValueAsString(vtObj, field, ft, true);
                    } else {
                        var child = field.GetDataBoxed(vtObj.GetAddress(), true);
                        if (child is IObject childObj) {
                            var childTdef = childObj.GetTypeDefinition();
                            fields[fname] = new {
                                address = "0x" + childObj.GetAddress().ToString("X"),
                                kind = childObj is ManagedObject ? "managed" : "native",
                                typeName = childTdef.GetFullName()
                            };
                        } else {
                            fields[fname] = child?.ToString();
                        }
                    }
                } catch { fields[fname] = null; }
            }
        }
        return new { isValueType = true, typeName = fullName, fields };
    }

    static IObject ResolveObjectFromParams(string addressStr, string kind, string typeName) {
        if (string.IsNullOrEmpty(addressStr) || string.IsNullOrEmpty(kind) || string.IsNullOrEmpty(typeName))
            return null;

        ulong address = 0;
        if (addressStr.StartsWith("0x") || addressStr.StartsWith("0X"))
            address = Convert.ToUInt64(addressStr.Substring(2), 16);
        else
            address = Convert.ToUInt64(addressStr, 16);

        if (address == 0) return null;

        if (kind == "managed") {
            return ManagedObject.ToManagedObject(address);
        } else if (kind == "native") {
            var tdef = TDB.Get().GetType(typeName);
            if (tdef == null) return null;
            return new NativeObject(address, tdef);
        }

        return null;
    }

    static object ParseValueFromJson(JsonElement value, string typeName) {
        switch (typeName) {
            case "System.Byte":
            case "byte":
                return value.GetByte();
            case "System.SByte":
            case "sbyte":
                return value.GetSByte();
            case "System.Int16":
            case "short":
                return value.GetInt16();
            case "System.UInt16":
            case "ushort":
                return value.GetUInt16();
            case "System.Int32":
            case "int":
                return value.GetInt32();
            case "System.UInt32":
            case "uint":
                return value.GetUInt32();
            case "System.Int64":
            case "long":
                return value.GetInt64();
            case "System.UInt64":
            case "ulong":
                return value.GetUInt64();
            case "System.Single":
            case "float":
                return value.GetSingle();
            case "System.Double":
            case "double":
                return value.GetDouble();
            case "System.Boolean":
            case "bool":
                return value.GetBoolean();
            case "System.String":
            case "string":
                return value.GetString();
            case "System.Guid":
                // Accept GUID as string "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
                if (value.ValueKind == JsonValueKind.String && Guid.TryParse(value.GetString(), out var parsedGuid)) {
                    var guidTdef = TDB.Get().GetType("System.Guid");
                    if (guidTdef == null) return null;
                    var guidVt = guidTdef.CreateValueType();
                    // RE Engine GUID fields: mData1 (uint), mData2 (ushort), mData3 (ushort), mData4_0..mData4_7 (bytes)
                    var bytes = parsedGuid.ToByteArray();
                    var guidFields = new Dictionary<string, Field>();
                    for (var p = guidTdef; p != null; p = p.ParentType)
                        foreach (var f in p.GetFields())
                            if (!f.IsStatic()) guidFields.TryAdd(f.GetName(), f);
                    if (guidFields.TryGetValue("mData1", out var fd1)) fd1.SetDataBoxed(guidVt.GetAddress(), BitConverter.ToUInt32(bytes, 0), true);
                    if (guidFields.TryGetValue("mData2", out var fd2)) fd2.SetDataBoxed(guidVt.GetAddress(), BitConverter.ToUInt16(bytes, 4), true);
                    if (guidFields.TryGetValue("mData3", out var fd3)) fd3.SetDataBoxed(guidVt.GetAddress(), BitConverter.ToUInt16(bytes, 6), true);
                    for (int gi = 0; gi < 8; gi++) {
                        if (guidFields.TryGetValue($"mData4_{gi}", out var fdi))
                            fdi.SetDataBoxed(guidVt.GetAddress(), bytes[8 + gi], true);
                    }
                    return guidVt;
                }
                return null;
            default:
                return ParseComplexValueFromJson(value, typeName);
        }
    }

    static object ParseComplexValueFromJson(JsonElement value, string typeName) {
        var tdef = TDB.Get().GetType(typeName);
        if (tdef == null) return null;

        // Enum: accept integer or string representation
        if (tdef.IsEnum()) {
            long longValue;
            if (value.ValueKind == JsonValueKind.Number) {
                longValue = value.GetInt64();
            } else if (value.ValueKind == JsonValueKind.String) {
                if (!long.TryParse(value.GetString(), out longValue)) return null;
            } else {
                return null;
            }
            return _System.Enum.InternalBoxEnum(
                tdef.GetRuntimeType().As<_System.RuntimeType>(), longValue);
        }

        // Value type (struct): create instance and populate fields from JSON object
        if (tdef.IsValueType() && value.ValueKind == JsonValueKind.Object) {
            var vt = tdef.CreateValueType();

            // Collect non-static fields from type hierarchy
            var fieldMap = new Dictionary<string, Field>();
            for (var parent = tdef; parent != null; parent = parent.ParentType) {
                foreach (var f in parent.GetFields()) {
                    if (!f.IsStatic()) fieldMap.TryAdd(f.GetName(), f);
                }
            }

            // Set each field from the JSON properties
            foreach (var prop in value.EnumerateObject()) {
                if (!fieldMap.TryGetValue(prop.Name, out var field)) continue;
                var ft = field.GetType();
                if (ft == null) continue;
                var fieldValue = ParseValueFromJson(prop.Value, ft.GetFullName());
                if (fieldValue != null) {
                    field.SetDataBoxed(vt.GetAddress(), fieldValue, true);
                }
            }

            return vt;
        }

        return null;
    }

    static Method FindMethod(TypeDefinition tdef, string methodName, string methodSignature) {
        for (var parent = tdef; parent != null; parent = parent.ParentType) {
            foreach (var m in parent.GetMethods()) {
                if (m.GetName() == methodName) {
                    if (!string.IsNullOrEmpty(methodSignature) && m.GetMethodSignature() != methodSignature)
                        continue;
                    return m;
                }
            }
        }
        return null;
    }

    static object FormatMethodResult(object result, Method method) {
        if (result == null) return new { isObject = false, value = "null" };

        if (result is IObject objResult) {
            var childTdef = objResult.GetTypeDefinition();

            // ValueType results are ephemeral (GC-managed) — read inline before they go out of scope
            if (objResult is REFrameworkNET.ValueType) {
                return ReadValueTypeInline(objResult);
            }

            bool childManaged = objResult is ManagedObject;
            return new {
                isObject = true,
                childAddress = "0x" + objResult.GetAddress().ToString("X"),
                childKind = childManaged ? "managed" : "native",
                childTypeName = childTdef.GetFullName()
            };
        }

        var returnType = method.GetReturnType();
        if (returnType != null && returnType.IsEnum()) {
            long longValue = Convert.ToInt64(result);
            try {
                var boxedEnum = _System.Enum.InternalBoxEnum(returnType.GetRuntimeType().As<_System.RuntimeType>(), longValue);
                return new { isObject = false, value = (boxedEnum as IObject).Call("ToString()") + " (" + result.ToString() + ")" };
            } catch { }
        }

        return new { isObject = false, value = result.ToString() };
    }

#if MHWILDS
    // ── Lobby endpoint ─────────────────────────────────────────────────

    static object GetLobbyMembers() {
        try {
            var nm = API.GetManagedSingletonT<app.NetworkManager>();
            if (nm == null) return new { error = "NetworkManager not available" };

            var userInfoMgr = nm._UserInfoManager;
            if (userInfoMgr == null) return new { error = "UserInfoManager not available" };

            var lobbyInfo = (IObject)userInfoMgr._mlInfo;
            if (lobbyInfo == null) return new { error = "Lobby info not available" };
            var listInfoObj = lobbyInfo.GetField("_ListInfo") as IObject;
            if (listInfoObj == null) return new { error = "ListInfo array is null" };

            var members = new List<object>();
            var arr = listInfoObj.As<_System.Array>();
            int len = arr.Length;

            for (int i = 0; i < len; i++) {
                try {
                    var element = arr.GetValue(i);
                    if (element == null) continue;

                    var userInfo = (element as IObject)?.As<app.Net_UserInfo>();
                    if (userInfo == null || !userInfo.IsValid) continue;

                    string name = null;
                    try { name = userInfo.PlName; } catch { }
                    if (string.IsNullOrEmpty(name)) continue;

                    string otomoName = null;
                    try { otomoName = userInfo.OtomoName; } catch { }

                    int hunterRank = 0;
                    try { hunterRank = userInfo.HunterRank; } catch { }

                    int weaponType = -1;
                    try { weaponType = userInfo.WeaponType; } catch { }

                    int weaponId = 0;
                    try { weaponId = userInfo.WeaponId; } catch { }

                    bool isSelf = false;
                    try { isSelf = userInfo.IsSelf; } catch { }

                    bool isQuest = false;
                    try { isQuest = userInfo.IsQuest; } catch { }

                    members.Add(new {
                        name,
                        otomoName,
                        hunterRank,
                        weaponType,
                        weaponId,
                        isSelf,
                        isQuest,
                        memberIndex = userInfo.MemberIndex
                    });
                } catch { }
            }

            return new { count = members.Count, members };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── Weather endpoint ──────────────────────────────────────────────────

    static object GetWeather() {
        try {
            var wm = API.GetManagedSingletonT<ace.WeatherManager>();
            if (wm == null) return new { error = "WeatherManager not available" };

            var currentName = wm.CurrentWeatherName;

            string nextName = null;
            try { nextName = wm.NextWeatherName; } catch { }

            float blendRate = 0;
            try { blendRate = wm._CurrentBlendRate; } catch { }

            float arrivalTime = 0;
            try { arrivalTime = wm._ArrivalTime; } catch { }

            float time = 0;
            try { time = wm._Time; } catch { }

            // Get per-weather blend rates from _values array
            var blends = new List<object>();
            try {
                var valuesObj = ((IObject)wm).GetField("_values") as IObject;
                if (valuesObj != null) {
                    var arr = valuesObj.As<_System.Array>();
                    int len = arr.Length;
                    for (int i = 0; i < len; i++) {
                        try {
                            var el = arr.GetValue(i) as IObject;
                            if (el == null) continue;
                            var tdef = el.GetTypeDefinition();

                            string name = null;
                            float rate = 0;

                            var nameField = tdef.FindField("_WeatherName");
                            if (nameField != null) {
                                var nameObj = el.GetField("_WeatherName");
                                name = nameObj?.ToString();
                            }

                            var rateField = tdef.FindField("_BlendRate");
                            if (rateField != null) {
                                try { rate = (float)rateField.GetDataBoxed(el.GetAddress(), false); } catch { }
                            }

                            blends.Add(new { name, blendRate = rate });
                        } catch { }
                    }
                }
            } catch { }

            // In-game time of day
            string timeZone = null;
            int hour = 0;
            int minute = 0;
            try {
                var em = API.GetManagedSingletonT<app.EnvironmentManager>();
                if (em != null) {
                    var mainTime = em._MainTime;
                    if (mainTime != null) {
                        float gameCount = mainTime.Count;
                        float timeVal = em.convertGameCountToTime(gameCount);
                        float oneDay = em.getOneDayTimeSecond();
                        if (oneDay > 0) {
                            float fraction = timeVal / oneDay;
                            float hours = fraction * 24f;
                            hour = (int)hours;
                            minute = (int)((hours - hour) * 60);
                        }
                    }
                    try { timeZone = em.getTimeZone(0).ToString(); } catch { }
                }
            } catch { }

            return new {
                current = currentName,
                next = nextName,
                blendRate,
                arrivalTime,
                time,
                blends,
                clock = $"{hour:D2}:{minute:D2}",
                timeZone
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }
#endif

#if RE9
    // ── RE9 Name Resolution ─────────────────────────────────────────────

    static readonly Dictionary<string, string> s_characterNames = new() {
        { "cp_A000", "Leon" },
        { "cp_A100", "Grace" },
    };

    static readonly Dictionary<string, string> s_weaponNameCache = new();

    static string ResolveCharacterName(string kindId) {
        if (kindId == null) return null;
        return s_characterNames.TryGetValue(kindId, out var name) ? name : kindId;
    }

    static string ResolveWeaponName(string weaponIdStr) {
        if (weaponIdStr == null) return null;
        if (s_weaponNameCache.TryGetValue(weaponIdStr, out var cached)) return cached;

        try {
            // Navigate: EquipmentManager._ItemWeaponIDDic._Dict._entries[]
            // Each entry has .key (ItemID) and .value (ManagedItem with _Value = WeaponID)
            // Find the entry where value matches our weaponIdStr, get the ItemID
            // Then look up ItemDetailData from ItemManager._ItemCatalog, read _NameMessageId, localize

            var equipMgr = API.GetManagedSingletonT<app.EquipmentManager>() as IObject;
            var itemMgr = API.GetManagedSingletonT<app.ItemManager>() as IObject;
            if (equipMgr == null || itemMgr == null) return weaponIdStr;

            // Get ItemID for this WeaponID by iterating _ItemWeaponIDDic
            var iwDicField = equipMgr.GetTypeDefinition().FindField("_ItemWeaponIDDic");
            var iwDic = iwDicField?.GetDataBoxed(equipMgr.GetAddress(), false) as IObject;
            if (iwDic == null) return weaponIdStr;

            var dictField = iwDic.GetTypeDefinition().FindField("_Dict");
            var dict = dictField?.GetDataBoxed(iwDic.GetAddress(), false) as IObject;
            if (dict == null) return weaponIdStr;

            var entriesField = dict.GetTypeDefinition().FindField("_entries");
            var entries = entriesField?.GetDataBoxed(dict.GetAddress(), false) as IObject;
            if (entries == null) return weaponIdStr;

            var entryTd = entries.GetTypeDefinition();
            int entryCount = entries.GetAddress() != 0 ? (int)entries.Call("get_Length") : 0;

            IObject matchedItemId = null;
            for (int i = 0; i < entryCount && matchedItemId == null; i++) {
                try {
                    var entry = entries.Call("Get", i) as IObject;
                    if (entry == null) continue;

                    var valueMgd = entry.GetTypeDefinition().FindField("value")?.GetDataBoxed(entry.GetAddress(), false) as IObject;
                    if (valueMgd == null) continue;

                    var weaponVal = valueMgd.GetTypeDefinition().FindField("_Value")?.GetDataBoxed(valueMgd.GetAddress(), false) as IObject;
                    if (weaponVal == null) continue;

                    if (weaponVal.Call("ToString").ToString() == weaponIdStr) {
                        matchedItemId = entry.GetTypeDefinition().FindField("key")?.GetDataBoxed(entry.GetAddress(), false) as IObject;
                    }
                } catch { }
            }

            if (matchedItemId == null) return weaponIdStr;

            // Now look up ItemDetailData from ItemManager._ItemCatalog
            var catalogField = itemMgr.GetTypeDefinition().FindField("_ItemCatalog");
            var catalog = catalogField?.GetDataBoxed(itemMgr.GetAddress(), false) as IObject;
            if (catalog == null) return weaponIdStr;

            var catDictField = catalog.GetTypeDefinition().FindField("_Dict");
            var catDict = catDictField?.GetDataBoxed(catalog.GetAddress(), false) as IObject;
            if (catDict == null) return weaponIdStr;

            var catEntries = catDict.GetTypeDefinition().FindField("_entries")?.GetDataBoxed(catDict.GetAddress(), false) as IObject;
            if (catEntries == null) return weaponIdStr;

            int catCount = (int)catEntries.Call("get_Length");
            string matchedItemStr = matchedItemId.Call("ToString").ToString();

            for (int i = 0; i < catCount; i++) {
                try {
                    var entry = catEntries.Call("Get", i) as IObject;
                    if (entry == null) continue;

                    var key = entry.GetTypeDefinition().FindField("key")?.GetDataBoxed(entry.GetAddress(), false) as IObject;
                    if (key == null || key.Call("ToString").ToString() != matchedItemStr) continue;

                    var valueMgd = entry.GetTypeDefinition().FindField("value")?.GetDataBoxed(entry.GetAddress(), false) as IObject;
                    if (valueMgd == null) continue;

                    var detail = valueMgd.GetTypeDefinition().FindField("_Value")?.GetDataBoxed(valueMgd.GetAddress(), false) as IObject;
                    if (detail == null) continue;

                    // Read _NameMessageId (System.Guid, value type at offset 0x80)
                    var nameGuidField = detail.GetTypeDefinition().FindField("_NameMessageId");
                    if (nameGuidField == null) continue;

                    var guidObj = nameGuidField.GetDataBoxed(detail.GetAddress(), false) as IObject;
                    if (guidObj == null) continue;

                    // Call via.gui.message.get(guid) to localize
                    var msgTd = TDB.Get().FindType("via.gui.message");
                    var getMethod = msgTd?.FindMethod("get(System.Guid)");
                    if (getMethod == null) continue;

                    var localizedStr = getMethod.Invoke(null, new object[] { guidObj });
                    var result = localizedStr.ToString();
                    if (!string.IsNullOrEmpty(result)) {
                        s_weaponNameCache[weaponIdStr] = result;
                        return result;
                    }
                } catch { }
            }
        } catch { }

        return weaponIdStr;
    }

    // ── RE9 Player Info ──────────────────────────────────────────────────

    static object GetPlayerInfoRE9() {
        try {
            var cm = API.GetManagedSingletonT<app.CharacterManager>();
            if (cm == null) return new { error = "CharacterManager not available" };

            var pc = cm.getPlayerContextRefFast();
            if (pc == null) return new { error = "Player not available" };

            bool isInSafeRoom = false, isGameOver = false;
            try { isInSafeRoom = cm.IsPlayerInSafeRoom; } catch { }
            try { isGameOver = cm.IsGameOvered; } catch { }

            string characterId = null;
            try { characterId = pc.KindID?.ToString(); } catch { }

            float? posX = null, posY = null, posZ = null;
            try {
                var pos = pc.PositionFast;
                posX = pos.x; posY = pos.y; posZ = pos.z;
            } catch { }

            int? health = null, maxHealth = null;
            bool? isDead = null, invincible = null;
            try {
                var hp = pc.HitPoint;
                if (hp != null) {
                    health = hp.CurrentHitPoint;
                    maxHealth = hp.CurrentMaximumHitPoint;
                    isDead = hp.IsDead;
                    invincible = hp.Invincible;
                }
            } catch { }

            bool isSpawn = false;
            try { isSpawn = pc.IsSpawn; } catch { }

            return new {
                characterId,
                characterName = ResolveCharacterName(characterId),
                health,
                maxHealth,
                isDead,
                invincible,
                isInSafeRoom,
                isGameOver,
                isSpawn,
                position = new { x = posX, y = posY, z = posZ }
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object SetPlayerHealthRE9(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var value = doc.RootElement.GetProperty("value").GetInt32();

            var cm = API.GetManagedSingletonT<app.CharacterManager>();
            if (cm == null) return new { error = "CharacterManager not available" };

            var pc = cm.getPlayerContextRefFast();
            if (pc == null) return new { error = "Player not available" };

            var hp = pc.HitPoint;
            if (hp == null) return new { error = "HitPoint not available" };

            hp.CurrentHitPoint = value;
            return new { ok = true, health = value };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── RE9 Enemy List ───────────────────────────────────────────────────

    static object GetEnemiesRE9() {
        try {
            var cm = API.GetManagedSingletonT<app.CharacterManager>();
            if (cm == null) return new { error = "CharacterManager not available" };

            var enemies = new List<object>();
            var enemyList = cm.EnemyContextList;
            if (enemyList != null) {
                int count = enemyList.Count;
                for (int i = 0; i < count; i++) {
                    try {
                        var ec = enemyList[i];
                        if (ec == null) continue;

                        bool isSpawn = false, isSuspended = false;
                        try { isSpawn = ec.IsSpawn; } catch { }
                        try { isSuspended = ec.IsSuspended; } catch { }
                        if (!isSpawn || isSuspended) continue;

                        string kindId = null;
                        try { kindId = ec.KindID?.ToString(); } catch { }

                        int? hp = null, maxHp = null;
                        bool? isDead = null;
                        try {
                            var hitPoint = ec.HitPoint;
                            if (hitPoint != null) {
                                hp = hitPoint.CurrentHitPoint;
                                maxHp = hitPoint.CurrentMaximumHitPoint;
                                isDead = hitPoint.IsDead;
                            }
                        } catch { }

                        bool isElite = false;
                        try { isElite = ec.IsElite; } catch { }

                        float? posX = null, posY = null, posZ = null;
                        try {
                            var tf = ec.Transform;
                            if (tf != null) {
                                var pos = tf.Position;
                                posX = pos.x; posY = pos.y; posZ = pos.z;
                            }
                        } catch { }

                        enemies.Add(new {
                            kindId,
                            health = hp,
                            maxHealth = maxHp,
                            isDead,
                            isElite,
                            position = new { x = posX, y = posY, z = posZ }
                        });
                    } catch { }
                }
            }

            return new {
                count = enemies.Count,
                isPlayerInSafeRoom = cm.IsPlayerInSafeRoom,
                enemies
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── RE9 Game Info ────────────────────────────────────────────────────

    static object GetGameInfoRE9() {
        try {
            var result = new Dictionary<string, object>();

            // Chapter / Story progress
            try {
                var lfm = API.GetManagedSingletonT<app.LevelFlowManager>();
                if (lfm != null) {
                    result["chapter"] = lfm.getDyingProgressName();
                    result["progressNo"] = lfm.getDyingProgressNo();
                }
            } catch { }

            // Scenario time
            try {
                var stm = API.GetManagedSingletonT<app.ScenarioTimeManager>();
                if (stm != null) {
                    result["scenarioTime"] = stm.CurrentKind?.ToString();
                }
            } catch { }

            // Difficulty
            try {
                var gdm = API.GetManagedSingletonT<app.GameDifficultyManager>();
                if (gdm != null) {
                    result["difficulty"] = gdm.DifficultyID?.ToString();
                }
            } catch { }

            // Adaptive rank
            try {
                var rm = API.GetManagedSingletonT<app.RankManager>();
                if (rm != null) {
                    result["rank"] = rm.getCurrentRank();
                    result["rankMax"] = 10;
                    result["enemyDamageFactor"] = rm.getEnemyDamageFactor();
                    result["playerDamageFactor"] = rm.getPlayerDamageFactor();
                    result["enemyMoveFactor"] = rm.getEnemyMoveFactor();
                    result["enemyWinceFactor"] = rm.getEnemyWinceFactor();
                }
            } catch { }

            // Play time from GameClock (OperationTimerType enum not in proxy, use reflection)
            try {
                var gc = API.GetManagedSingletonT<app.GameClock>();
                if (gc != null) {
                    var gcObj = gc as IObject;
                    var timerTd = TDB.Get().FindType("app.OperationTimerType");
                    if (timerTd != null) {
                        var enumVal = _System.Enum.InternalBoxEnum(timerTd.GetRuntimeType().As<_System.RuntimeType>(), 2); // GameElapsedTime = 2
                        var gameTime = gcObj.Call("getElapsedTime", enumVal);
                        if (gameTime != null) {
                            var us = Convert.ToInt64(gameTime.ToString());
                            result["playTimeMicroseconds"] = us;
                            result["playTimeSeconds"] = (double)us / 1_000_000.0;
                        }
                    }
                }
            } catch { }

            // Game data (clear count, NG count)
            try {
                var gdm2 = API.GetManagedSingletonT<app.GameDataManager>();
                if (gdm2 != null) {
                    result["newGameStarts"] = gdm2.NewGameCount;
                    result["totalClears"] = gdm2.TotalClearCount;
                }
            } catch { }

            // Collectibles from GimmickManager (use reflection for max methods not in proxy)
            try {
                var gm = API.GetManagedSingletonT<app.GimmickManager>();
                if (gm != null) {
                    var gmObj = gm as IObject;
                    int? safeCount = null, safeMax = null;
                    int? containerCount = null, containerMax = null;
                    int? fragileCount = null, fragileMax = null;
                    try { safeCount = (int)gm.getAchievementSafeObjectCount(); } catch { }
                    try { safeMax = Convert.ToInt32(gmObj.Call("getAchievementSafeObjectMaxCount").ToString()); } catch { }
                    try { containerCount = (int)gm.getAchievementContainerObjectCount(); } catch { }
                    try { containerMax = Convert.ToInt32(gmObj.Call("getAchievementContainerObjectMaxCount").ToString()); } catch { }
                    try { fragileCount = (int)gm.getAchievementFragileSymbolCount(); } catch { }
                    try { fragileMax = Convert.ToInt32(gmObj.Call("getAchievementFragileSymbolMaxCount").ToString()); } catch { }
                    result["collectibles"] = new {
                        safes = new { found = safeCount, max = safeMax },
                        containers = new { found = containerCount, max = containerMax },
                        fragileSymbols = new { found = fragileCount, max = fragileMax }
                    };
                }
            } catch { }

            // Combat state from PlayerContextUnit_Common
            try {
                var cm = API.GetManagedSingletonT<app.CharacterManager>();
                var pc = cm?.getPlayerContextRefFast();
                if (pc != null) {
                    var common = pc.Common;
                    if (common != null) {
                        var weaponIdStr = common.EquipWeaponID?.ToString();
                        result["weapon"] = weaponIdStr;
                        result["weaponName"] = ResolveWeaponName(weaponIdStr);
                        try {
                            var commonObj = common as IObject;
                            var fearField = commonObj?.GetTypeDefinition()?.FindField("_FearLevelRate");
                            if (fearField != null) result["fearLevel"] = fearField.GetDataBoxed(commonObj.GetAddress(), false);
                        } catch { }
                        result["combat"] = new {
                            isHolding = common.IsHolding,
                            isShooting = common.IsShooting,
                            isReloading = common.IsReloading,
                            isMeleeAttack = common.IsMeleeAttack,
                            isCrouch = common.IsCrouch,
                            isRun = common.IsRun,
                            isIdle = common.IsIdle
                        };
                    }
                }
            } catch { }

            // Scene info from MainGameFlowManager
            try {
                var mgfm = API.GetManagedSingletonT<app.MainGameFlowManager>();
                if (mgfm != null) {
                    result["isMainGame"] = mgfm.IsMainGame();
                }
            } catch { }

            return result;
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }
#endif

    // ── Chain endpoint ───────────────────────────────────────────────────

    static object PostExplorerChain(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            // Resolve start object
            var startProp = root.GetProperty("start");
            IObject startObj = null;

            if (startProp.TryGetProperty("singleton", out var singletonProp)) {
                var typeName = singletonProp.GetString();
                // Search managed singletons
                try {
                    foreach (var desc in API.GetManagedSingletons()) {
                        if (desc.Instance?.GetTypeDefinition().GetFullName() == typeName) {
                            startObj = desc.Instance; break;
                        }
                    }
                } catch { }
                // Search native singletons
                if (startObj == null) {
                    try {
                        foreach (var desc in API.GetNativeSingletons()) {
                            if (desc.Instance?.GetTypeDefinition().GetFullName() == typeName) {
                                startObj = desc.Instance; break;
                            }
                        }
                    } catch { }
                }
                if (startObj == null) return new { error = $"Singleton '{typeName}' not found" };
            } else {
                startObj = ResolveObjectFromParams(
                    startProp.GetProperty("address").GetString(),
                    startProp.GetProperty("kind").GetString(),
                    startProp.GetProperty("typeName").GetString());
            }

            if (startObj == null) return new { error = "Could not resolve start object" };

            // Process steps
            var steps = root.GetProperty("steps");
            var current = new List<IObject> { startObj };

            foreach (var step in steps.EnumerateArray()) {
                var stepType = step.GetProperty("type").GetString();

                switch (stepType) {
                    case "method": {
                        var methodName = step.GetProperty("name").GetString();
                        var sig = step.TryGetProperty("signature", out var sigP) ? sigP.GetString() : null;
                        var next = new List<IObject>();
                        foreach (var obj in current) {
                            try {
                                var tdef = obj.GetTypeDefinition();
                                var method = FindMethod(tdef, methodName, sig);
                                if (method == null) continue;
                                object result = null;
                                obj.HandleInvokeMember_Internal(method, null, ref result);
                                if (result is IObject ioResult) next.Add(ioResult);
                            } catch { }
                        }
                        current = next;
                        break;
                    }
                    case "field": {
                        var fieldName = step.GetProperty("name").GetString();
                        var next = new List<IObject>();
                        foreach (var obj in current) {
                            try {
                                var child = obj.GetField(fieldName) as IObject;
                                if (child != null) next.Add(child);
                            } catch { }
                        }
                        current = next;
                        break;
                    }
                    case "array": {
                        int offset = step.TryGetProperty("offset", out var offP) ? offP.GetInt32() : 0;
                        int count = step.TryGetProperty("count", out var cntP) ? cntP.GetInt32() : 10000;
                        var next = new List<IObject>();
                        foreach (var obj in current) {
                            try {
                                var easyArray = obj.TryAs<_System.Array>();
                                if (easyArray == null) continue;
                                int len = easyArray.Length;
                                int end = Math.Min(offset + count, len);
                                for (int i = offset; i < end; i++) {
                                    var el = easyArray.GetValue(i);
                                    if (el is IObject ioEl) next.Add(ioEl);
                                }
                            } catch { }
                        }
                        current = next;
                        break;
                    }
                    case "filter": {
                        var filterMethod = step.GetProperty("method").GetString();
                        var filterValue = step.TryGetProperty("value", out var fvP) ? fvP.GetString() : "True";
                        var next = new List<IObject>();
                        foreach (var obj in current) {
                            try {
                                var tdef = obj.GetTypeDefinition();
                                var method = FindMethod(tdef, filterMethod, null);
                                if (method == null) continue;
                                object result = null;
                                obj.HandleInvokeMember_Internal(method, null, ref result);
                                if (result?.ToString() == filterValue) next.Add(obj);
                            } catch { }
                        }
                        current = next;
                        break;
                    }
                    case "collect": {
                        // Terminal step: read multiple methods on each object, return results
                        var methods = step.GetProperty("methods");
                        var collected = new List<object>();
                        foreach (var obj in current) {
                            var entry = new Dictionary<string, object>();
                            var tdef = obj.GetTypeDefinition();
                            foreach (var mProp in methods.EnumerateArray()) {
                                var mName = mProp.GetString();
                                try {
                                    var method = FindMethod(tdef, mName, null);
                                    if (method == null) { entry[mName] = null; continue; }
                                    object result = null;
                                    obj.HandleInvokeMember_Internal(method, null, ref result);
                                    if (result is REFrameworkNET.ValueType vtRes) {
                                        entry[mName] = ReadValueTypeInline(vtRes);
                                    } else if (result is IObject ioRes) {
                                        entry[mName] = new {
                                            isObject = true,
                                            address = "0x" + ioRes.GetAddress().ToString("X"),
                                            kind = ioRes is ManagedObject ? "managed" : "native",
                                            typeName = ioRes.GetTypeDefinition().GetFullName()
                                        };
                                    } else {
                                        entry[mName] = result?.ToString();
                                    }
                                } catch { entry[mName] = null; }
                            }
                            collected.Add(entry);
                        }
                        return new { count = collected.Count, results = collected };
                    }
                }

                if (current.Count == 0)
                    return new { error = $"Chain broken at step '{stepType}': no results" };
            }

            // Default terminal: return addresses/types of current objects
            var finalResults = new List<object>();
            foreach (var obj in current) {
                try {
                    if (obj is REFrameworkNET.ValueType) {
                        finalResults.Add(ReadValueTypeInline(obj));
                    } else {
                        var tdef = obj.GetTypeDefinition();
                        bool managed = obj is ManagedObject;
                        finalResults.Add(new {
                            address = "0x" + obj.GetAddress().ToString("X"),
                            kind = managed ? "managed" : "native",
                            typeName = tdef.GetFullName()
                        });
                    }
                } catch { }
            }

            return new { count = finalResults.Count, results = finalResults };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── Explorer endpoints ────────────────────────────────────────────

    static object GetExplorerSingletons() {
        var managedList = new List<object>();
        var nativeList = new List<object>();

        try {
            var managed = API.GetManagedSingletons();
            managed.RemoveAll(s => s.Instance == null);
            managed.Sort((a, b) => a.Instance.GetTypeDefinition().GetFullName()
                .CompareTo(b.Instance.GetTypeDefinition().GetFullName()));

            foreach (var desc in managed) {
                var instance = desc.Instance;
                var tdef = instance.GetTypeDefinition();
                managedList.Add(new {
                    type = tdef.GetFullName(),
                    address = "0x" + instance.GetAddress().ToString("X"),
                    kind = "managed"
                });
            }
        } catch { }

        try {
            var native = API.GetNativeSingletons();
            native.Sort((a, b) => a.Instance.GetTypeDefinition().GetFullName()
                .CompareTo(b.Instance.GetTypeDefinition().GetFullName()));

            foreach (var desc in native) {
                var instance = desc.Instance;
                if (instance == null) continue;
                var tdef = instance.GetTypeDefinition();
                nativeList.Add(new {
                    type = tdef.GetFullName(),
                    address = "0x" + instance.GetAddress().ToString("X"),
                    kind = "native"
                });
            }
        } catch { }

        return new { managed = managedList, native = nativeList };
    }

    static object GetExplorerObject(HttpListenerRequest request) {
        try {
            var obj = ResolveObject(request);
            if (obj == null) return new { error = "Could not resolve object" };

            var qs = request.QueryString;
            bool noFields = string.Equals(qs["noFields"], "true", StringComparison.OrdinalIgnoreCase);
            bool noMethods = string.Equals(qs["noMethods"], "true", StringComparison.OrdinalIgnoreCase);
            var filterFields = qs["fields"];   // comma-separated field names
            var filterMethods = qs["methods"]; // comma-separated method names

            var tdef = obj.GetTypeDefinition();
            var typeName = tdef.GetFullName();

            int? refCount = null;
            if (obj is ManagedObject managed) {
                refCount = managed.GetReferenceCount();
            }

            // Collect fields from type hierarchy
            List<object> fieldList = null;
            if (!noFields) {
                var fields = new List<Field>();
                for (var parent = tdef; parent != null; parent = parent.ParentType) {
                    fields.AddRange(parent.GetFields());
                }
                fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

                HashSet<string> wantFields = null;
                if (filterFields != null) {
                    wantFields = new HashSet<string>(filterFields.Split(','), StringComparer.OrdinalIgnoreCase);
                }

                fieldList = new List<object>();
                foreach (var field in fields) {
                    if (wantFields != null && !wantFields.Contains(field.GetName())) continue;

                    var ft = field.GetType();
                    var ftName = ft != null ? ft.GetFullName() : "null";
                    bool isValueType = ft != null && ft.IsValueType();
                    bool isStatic = field.IsStatic();

                    string value = null;
                    if (ft != null && (isValueType || ftName == "System.String")) {
                        try { value = ReadFieldValueAsString(obj, field, ft); } catch { }
                    }

                    ulong fieldAddr = 0;
                    if (isStatic) {
                        try { fieldAddr = field.GetDataRaw(obj.GetAddress(), false); } catch { }
                    } else {
                        fieldAddr = (obj as UnifiedObject)?.GetAddress() ?? 0;
                        fieldAddr += field.GetOffsetFromBase();
                    }

                    fieldList.Add(new {
                        name = field.GetName(),
                        typeName = ftName,
                        isValueType,
                        isStatic,
                        offset = isStatic ? (string)null : "0x" + field.GetOffsetFromBase().ToString("X"),
                        value
                    });
                }
            }

            // Collect methods from type hierarchy
            List<object> methodList = null;
            if (!noMethods) {
                var methods = new List<Method>();
                for (var parent = tdef; parent != null; parent = parent.ParentType) {
                    methods.AddRange(parent.GetMethods());
                }
                methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
                methods.RemoveAll(m => m.GetParameters().Exists(p => p.Type.Name.Contains("!")));

                HashSet<string> wantMethods = null;
                if (filterMethods != null) {
                    wantMethods = new HashSet<string>(filterMethods.Split(','), StringComparer.OrdinalIgnoreCase);
                }

                methodList = new List<object>();
                foreach (var method in methods) {
                    if (wantMethods != null && !wantMethods.Contains(method.GetName())) continue;

                    var returnT = method.GetReturnType();
                    var returnTName = returnT != null ? returnT.GetFullName() : "void";

                    var ps = method.GetParameters();
                    var paramList = new List<object>();
                    foreach (var p in ps) {
                        paramList.Add(new {
                            type = p.Type.GetFullName(),
                            name = p.Name
                        });
                    }

                    bool isGetter = (method.Name.StartsWith("get_") || method.Name.StartsWith("Get") || method.Name == "ToString") && ps.Count == 0;

                    methodList.Add(new {
                        name = method.GetName(),
                        returnType = returnTName,
                        parameters = paramList,
                        isGetter,
                        signature = method.GetMethodSignature()
                    });
                }
            }

            // Check if array
            bool isArray = tdef.IsDerivedFrom(s_systemArrayT);
            int? arrayLength = null;
            if (isArray) {
                try { arrayLength = (int)obj.Call("get_Length"); } catch { }
            }

            return new {
                typeName,
                address = "0x" + (obj as UnifiedObject).GetAddress().ToString("X"),
                refCount,
                fields = fieldList,
                methods = methodList,
                isArray,
                arrayLength
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerSummary(HttpListenerRequest request) {
        try {
            var obj = ResolveObject(request);
            if (obj == null) return new { error = "Could not resolve object" };

            var tdef = obj.GetTypeDefinition();
            var typeName = tdef.GetFullName();

            // Short type name helper: strip namespaces, clean backtick generics
            // "System.Collections.Generic.List`1<app.Foo.Bar>" → "List<Bar>"
            static string ShortType(string fullName) {
                if (fullName == null) return "?";
                // Strip namespaces inside generic args first
                var result = System.Text.RegularExpressions.Regex.Replace(
                    fullName, @"[\w]+\.", m => {
                        // Only strip if it looks like a namespace segment (lowercase or known prefixes)
                        var seg = m.Value.TrimEnd('.');
                        if (seg == "System" || seg == "Collections" || seg == "Generic" ||
                            seg == "app" || seg == "ace" || seg == "via" || seg == "soundlib" ||
                            seg.Contains("_") || char.IsLower(seg[0]))
                            return "";
                        return m.Value;
                    });
                // Clean up generic backtick: "List`1<Foo>" → "List<Foo>"
                result = System.Text.RegularExpressions.Regex.Replace(result, @"`\d+", "");
                return result;
            }

            // Fields: "name: ShortType" or "name: ShortType = value" for primitives
            var fields = new List<Field>();
            for (var parent = tdef; parent != null; parent = parent.ParentType)
                fields.AddRange(parent.GetFields());
            fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            var fieldLines = new List<string>();
            foreach (var field in fields) {
                var ft = field.GetType();
                var ftName = ft != null ? ft.GetFullName() : "?";
                bool isValueType = ft != null && ft.IsValueType();
                string line = field.GetName() + ": " + ShortType(ftName);
                if (field.IsStatic()) line += " [static]";
                if (ft != null && (isValueType || ftName == "System.String")) {
                    try {
                        var val = ReadFieldValueAsString(obj, field, ft);
                        if (val != null) line += " = " + val;
                    } catch { }
                }
                fieldLines.Add(line);
            }

            // Methods: "name(ParamType, ...) → ReturnType" but skip .ctor, .cctor, dupes
            var methods = new List<Method>();
            for (var parent = tdef; parent != null; parent = parent.ParentType)
                methods.AddRange(parent.GetMethods());
            methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
            methods.RemoveAll(m => m.GetParameters().Exists(p => p.Type.Name.Contains("!")));

            var seen = new HashSet<string>();
            var methodLines = new List<string>();
            foreach (var method in methods) {
                var name = method.GetName();
                if (name == ".ctor" || name == ".cctor" || name == "Finalize" || name == "MemberwiseClone") continue;
                if (name == "Equals" || name == "GetHashCode" || name == "GetType") continue;
                // Skip compiler-generated lambda methods (noise)
                if (name.Contains(">g__") || name.Contains("<>")) continue;

                var ps = method.GetParameters();
                var paramStr = string.Join(", ", ps.Select(p => ShortType(p.Type.GetFullName())));
                var retType = method.GetReturnType();
                var retStr = retType != null ? ShortType(retType.GetFullName()) : "Void";
                var line = $"{name}({paramStr}) → {retStr}";
                if (seen.Add(line)) methodLines.Add(line);
            }

            return new { typeName, fields = fieldLines, methods = methodLines };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerField(HttpListenerRequest request) {
        try {
            var obj = ResolveObject(request);
            if (obj == null) return new { error = "Could not resolve object" };

            var fieldName = request.QueryString["fieldName"];
            if (string.IsNullOrEmpty(fieldName)) return new { error = "fieldName required" };

            var child = obj.GetField(fieldName) as IObject;
            if (child == null) return new { isNull = true };

            var childTdef = child.GetTypeDefinition();
            bool childManaged = child is ManagedObject;

            return new {
                isNull = false,
                childAddress = "0x" + child.GetAddress().ToString("X"),
                childKind = childManaged ? "managed" : "native",
                childTypeName = childTdef.GetFullName()
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerMethod(HttpListenerRequest request) {
        try {
            var obj = ResolveObject(request);
            if (obj == null) return new { error = "Could not resolve object" };

            var methodName = request.QueryString["methodName"];
            var methodSignature = request.QueryString["methodSignature"];
            if (string.IsNullOrEmpty(methodName)) return new { error = "methodName required" };

            // Find the method by name and optionally signature
            var tdef = obj.GetTypeDefinition();
            Method targetMethod = null;
            for (var parent = tdef; parent != null; parent = parent.ParentType) {
                foreach (var m in parent.GetMethods()) {
                    if (m.GetName() == methodName) {
                        if (!string.IsNullOrEmpty(methodSignature) && m.GetMethodSignature() != methodSignature)
                            continue;
                        targetMethod = m;
                        break;
                    }
                }
                if (targetMethod != null) break;
            }

            if (targetMethod == null) return new { error = "Method not found" };

            // Only invoke 0-parameter methods (getters, ToString, or other read-only calls)
            var ps = targetMethod.GetParameters();
            if (ps.Count != 0) return new { error = "Method has parameters, use invoke_method instead" };

            object result = null;
            obj.HandleInvokeMember_Internal(targetMethod, null, ref result);

            if (result == null) return new { isObject = false, value = "null" };

            if (result is IObject objResult) {
                // ValueType results are ephemeral — read inline
                if (objResult is REFrameworkNET.ValueType) {
                    return ReadValueTypeInline(objResult);
                }

                var childTdef = objResult.GetTypeDefinition();
                bool childManaged = objResult is ManagedObject;
                return new {
                    isObject = true,
                    childAddress = "0x" + objResult.GetAddress().ToString("X"),
                    childKind = childManaged ? "managed" : "native",
                    childTypeName = childTdef.GetFullName()
                };
            }

            // Primitive result - check for enum
            var returnType = targetMethod.GetReturnType();
            if (returnType != null && returnType.IsEnum()) {
                long longValue = Convert.ToInt64(result);
                try {
                    var boxedEnum = _System.Enum.InternalBoxEnum(returnType.GetRuntimeType().As<_System.RuntimeType>(), longValue);
                    return new { isObject = false, value = (boxedEnum as IObject).Call("ToString()") + " (" + result.ToString() + ")" };
                } catch { }
            }

            return new { isObject = false, value = result.ToString() };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerArray(HttpListenerRequest request) {
        try {
            var obj = ResolveObject(request);
            if (obj == null) return new { error = "Could not resolve object" };

            var tdef = obj.GetTypeDefinition();
            if (!tdef.IsDerivedFrom(s_systemArrayT))
                return new { error = "Object is not an array" };

            int offset = 0;
            int count = 50;
            if (!string.IsNullOrEmpty(request.QueryString["offset"]))
                int.TryParse(request.QueryString["offset"], out offset);
            if (!string.IsNullOrEmpty(request.QueryString["count"]))
                int.TryParse(request.QueryString["count"], out count);

            var easyArray = obj.As<_System.Array>();
            int totalLength = easyArray.Length;

            int end = Math.Min(offset + count, totalLength);
            var elements = new List<object>();

            for (int i = offset; i < end; i++) {
                try {
                    var element = easyArray.GetValue(i);
                    if (element == null) {
                        elements.Add(new { index = i, isNull = true, isObject = false, value = "null" });
                        continue;
                    }

                    if (element is IObject objElement) {
                        string display = null;
                        try { display = objElement.Call("ToString()") as string; } catch { }
                        var elTdef = objElement.GetTypeDefinition();
                        bool elManaged = objElement is ManagedObject;
                        elements.Add(new {
                            index = i,
                            isNull = false,
                            isObject = true,
                            address = "0x" + objElement.GetAddress().ToString("X"),
                            kind = elManaged ? "managed" : "native",
                            typeName = elTdef.GetFullName(),
                            display
                        });
                    } else {
                        elements.Add(new {
                            index = i,
                            isNull = false,
                            isObject = false,
                            value = element.ToString()
                        });
                    }
                } catch {
                    elements.Add(new { index = i, isNull = false, isObject = false, value = "error" });
                }
            }

            return new {
                totalLength,
                offset,
                count = elements.Count,
                hasMore = end < totalLength,
                elements
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerSearch(HttpListenerRequest request) {
        try {
            var query = request.QueryString["query"];
            if (string.IsNullOrEmpty(query)) return new { error = "query parameter required" };

            int limit = 50;
            if (!string.IsNullOrEmpty(request.QueryString["limit"]))
                int.TryParse(request.QueryString["limit"], out limit);

            var tdb = TDB.Get();
            uint numTypes = tdb.GetNumTypes();
            var queryLower = query.ToLower();
            var results = new List<object>();

            for (uint i = 0; i < numTypes && results.Count < limit; i++) {
                try {
                    var t = tdb.GetType(i);
                    if (t == null) continue;
                    var fullName = t.GetFullName();
                    if (string.IsNullOrEmpty(fullName)) continue;
                    if (!fullName.ToLower().Contains(queryLower)) continue;

                    var parentT = t.ParentType;
                    results.Add(new {
                        fullName,
                        isValueType = t.IsValueType(),
                        isEnum = t.IsEnum(),
                        numFields = (int)t.GetNumFields(),
                        numMethods = (int)t.GetNumMethods(),
                        parentType = parentT?.GetFullName()
                    });
                } catch { }
            }

            return new { query, count = results.Count, results };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerType(HttpListenerRequest request) {
        try {
            var typeName = request.QueryString["typeName"];
            if (string.IsNullOrEmpty(typeName)) return new { error = "typeName parameter required" };

            var tdef = TDB.Get().GetType(typeName);
            if (tdef == null) return new { error = $"Type '{typeName}' not found" };

            var parentT = tdef.ParentType;
            var declaringT = tdef.DeclaringType;
            var qs = request.QueryString;
            bool includeInherited = string.Equals(qs["includeInherited"], "true", StringComparison.OrdinalIgnoreCase);
            bool noFields = string.Equals(qs["noFields"], "true", StringComparison.OrdinalIgnoreCase);
            bool noMethods = string.Equals(qs["noMethods"], "true", StringComparison.OrdinalIgnoreCase);

            // Collect fields — own type only by default, full hierarchy if requested
            var fields = new List<Field>();
            if (!noFields) {
                if (includeInherited) {
                    for (var parent = tdef; parent != null; parent = parent.ParentType)
                        fields.AddRange(parent.GetFields());
                } else {
                    fields.AddRange(tdef.GetFields());
                }
                fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
            }

            var fieldList = new List<object>();
            foreach (var field in fields) {
                var ft = field.GetType();
                var ftName = ft != null ? ft.GetFullName() : "null";
                bool isValueType = ft != null && ft.IsValueType();
                bool isStatic = field.IsStatic();

                fieldList.Add(new {
                    name = field.GetName(),
                    typeName = ftName,
                    isValueType,
                    isStatic,
                    offset = isStatic ? (string)null : "0x" + field.GetOffsetFromBase().ToString("X")
                });
            }

            // Collect methods — own type only by default, full hierarchy if requested
            var methods = new List<Method>();
            if (!noMethods) {
                if (includeInherited) {
                    for (var parent = tdef; parent != null; parent = parent.ParentType)
                        methods.AddRange(parent.GetMethods());
                } else {
                    methods.AddRange(tdef.GetMethods());
                }
                methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
                methods.RemoveAll(m => m.GetParameters().Exists(p => p.Type.Name.Contains("!")));
                // Filter noise: constructors, finalizers, common inherited methods, compiler-generated
                methods.RemoveAll(m => {
                    var name = m.GetName();
                    return name == ".ctor" || name == ".cctor" || name == "Finalize" || name == "MemberwiseClone"
                        || name == "Equals" || name == "GetHashCode" || name == "GetType"
                        || name.StartsWith("<")
                        || name.Contains(">g__") || name.Contains("<>");
                });
            }

            var seen = new HashSet<string>();
            var dedupedMethods = new List<(string returnType, string signature)>();
            foreach (var method in methods) {
                var returnT = method.GetReturnType();
                var returnTName = returnT != null ? returnT.GetFullName() : "void";
                var sig = method.GetMethodSignature();
                if (!seen.Add(sig)) continue;
                dedupedMethods.Add((returnTName, sig));
            }

            // Collapse repetitive auto-generated methods (names differing only in numbers).
            // Normalize full signature (digit runs → #), if group has >2 members show one + count.
            var methodList = new List<object>();
            var groups = dedupedMethods.GroupBy(m =>
                System.Text.RegularExpressions.Regex.Replace(m.signature, @"\d+", "#")
            ).ToList();

            foreach (var group in groups) {
                var first = group.First();
                if (group.Count() > 2) {
                    methodList.Add(new {
                        returnType = first.returnType,
                        signature = first.signature,
                        similarCount = group.Count()
                    });
                } else {
                    foreach (var m in group)
                        methodList.Add(new { returnType = m.returnType, signature = m.signature });
                }
            }

            // Count totals (before filtering) for informational purposes
            int totalFields = noFields ? tdef.GetFields().Count : fieldList.Count;
            int totalMethods = noMethods ? tdef.GetMethods().Count : methodList.Count;

            return new {
                fullName = tdef.GetFullName(),
                @namespace = tdef.GetNamespace(),
                isValueType = tdef.IsValueType(),
                isEnum = tdef.IsEnum(),
                size = tdef.GetSize(),
                parentType = parentT?.GetFullName(),
                declaringType = declaringT?.GetFullName(),
                fieldCount = totalFields,
                methodCount = totalMethods,
                fields = fieldList,
                methods = methodList
            };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object GetExplorerSingleton(HttpListenerRequest request) {
        try {
            var typeName = request.QueryString["typeName"];
            if (string.IsNullOrEmpty(typeName)) return new { error = "typeName parameter required" };

            // Search managed singletons
            try {
                var managed = API.GetManagedSingletons();
                foreach (var desc in managed) {
                    var instance = desc.Instance;
                    if (instance == null) continue;
                    if (instance.GetTypeDefinition().GetFullName() == typeName) {
                        return new {
                            address = "0x" + instance.GetAddress().ToString("X"),
                            kind = "managed",
                            typeName
                        };
                    }
                }
            } catch { }

            // Search native singletons
            try {
                var native = API.GetNativeSingletons();
                foreach (var desc in native) {
                    var instance = desc.Instance;
                    if (instance == null) continue;
                    if (instance.GetTypeDefinition().GetFullName() == typeName) {
                        return new {
                            address = "0x" + instance.GetAddress().ToString("X"),
                            kind = "native",
                            typeName
                        };
                    }
                }
            } catch { }

            return new { error = $"Singleton '{typeName}' not found" };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object PostExplorerField(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var addressStr = root.GetProperty("address").GetString();
            var kind = root.GetProperty("kind").GetString();
            var typeName = root.GetProperty("typeName").GetString();
            var fieldName = root.GetProperty("fieldName").GetString();

            var obj = ResolveObjectFromParams(addressStr, kind, typeName);
            if (obj == null) return new { error = "Could not resolve object" };

            // Find field by walking parent chain
            var tdef = obj.GetTypeDefinition();
            Field targetField = null;
            for (var parent = tdef; parent != null; parent = parent.ParentType) {
                foreach (var f in parent.GetFields()) {
                    if (f.GetName() == fieldName) {
                        targetField = f;
                        break;
                    }
                }
                if (targetField != null) break;
            }

            if (targetField == null) return new { error = $"Field '{fieldName}' not found" };

            var ft = targetField.GetType();
            if (ft == null) return new { error = "Field type is null" };
            if (!ft.IsValueType()) return new { error = "Can only write value-type fields" };

            // Determine type name for parsing
            var valueTypeName = root.TryGetProperty("valueType", out var vtProp) ? vtProp.GetString() : null;
            if (string.IsNullOrEmpty(valueTypeName)) {
                valueTypeName = ft.IsEnum() ? ft.GetUnderlyingType().GetFullName() : ft.GetFullName();
            }

            var valueElement = root.GetProperty("value");
            var boxedValue = ParseValueFromJson(valueElement, valueTypeName);
            if (boxedValue == null) return new { error = $"Could not parse value as '{valueTypeName}'" };

            targetField.SetDataBoxed(obj.GetAddress(), boxedValue, false);

            return new { ok = true, field = fieldName, value = boxedValue.ToString() };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object PostExplorerMethod(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            var addressStr = root.TryGetProperty("address", out var addrProp) ? addrProp.GetString() : null;
            var kind = root.TryGetProperty("kind", out var kindProp) ? kindProp.GetString() : null;
            var typeName = root.GetProperty("typeName").GetString();
            var methodName = root.GetProperty("methodName").GetString();
            var methodSignature = root.TryGetProperty("methodSignature", out var sigProp) ? sigProp.GetString() : null;

            IObject obj;
            TypeDefinition tdef;

            // Static call: no address provided, create a temporary instance
            if (string.IsNullOrEmpty(addressStr)) {
                tdef = TDB.Get().GetType(typeName);
                if (tdef == null) return new { error = $"Type '{typeName}' not found" };
                // Try managed instance first, fall back to native object at address 0
                obj = tdef.CreateInstance(0) ?? (IObject)new NativeObject(0, tdef);
            } else {
                obj = ResolveObjectFromParams(addressStr, kind, typeName);
                if (obj == null) return new { error = "Could not resolve object" };
                tdef = obj.GetTypeDefinition();
            }

            var targetMethod = FindMethod(tdef, methodName, methodSignature);
            if (targetMethod == null) return new { error = "Method not found" };

            // Parse arguments
            object[] args = null;
            if (root.TryGetProperty("args", out var argsProp) && argsProp.ValueKind == JsonValueKind.Array) {
                var ps = targetMethod.GetParameters();
                args = new object[argsProp.GetArrayLength()];
                int i = 0;
                foreach (var argEl in argsProp.EnumerateArray()) {
                    var argValue = argEl.GetProperty("value");
                    // Use explicit type if provided, otherwise infer from method parameter
                    var argType = argEl.TryGetProperty("type", out var atProp) ? atProp.GetString() : null;
                    if (string.IsNullOrEmpty(argType) && i < ps.Count) {
                        argType = ps[i].Type.GetFullName();
                    }
                    args[i] = ParseValueFromJson(argValue, argType ?? "System.Int32");
                    i++;
                }
            }

            object result = null;
            obj.HandleInvokeMember_Internal(targetMethod, args, ref result);

            return FormatMethodResult(result, targetMethod);
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    static object PostExplorerBatch(HttpListenerRequest request) {
        try {
            using var reader = new StreamReader(request.InputStream, request.ContentEncoding);
            var body = reader.ReadToEnd();
            var doc = JsonDocument.Parse(body);
            var root = doc.RootElement;

            if (!root.TryGetProperty("operations", out var opsProp) || opsProp.ValueKind != JsonValueKind.Array)
                return new { error = "operations array required" };

            var results = new List<object>();

            foreach (var op in opsProp.EnumerateArray()) {
                try {
                    var opType = op.GetProperty("type").GetString();
                    var parms = op.GetProperty("params");

                    object opResult = opType switch {
                        "singleton" => BatchGetSingleton(parms),
                        "object" => BatchGetObject(parms),
                        "field" => BatchGetField(parms),
                        "method" => BatchInvokeMethod(parms),
                        "search" => BatchSearch(parms),
                        "type" => BatchGetType(parms),
                        "setField" => BatchSetField(parms),
                        _ => new { error = $"Unknown operation type: {opType}" }
                    };
                    results.Add(opResult);
                } catch (Exception e) {
                    results.Add(new { error = e.Message });
                }
            }

            return new { results };
        } catch (Exception e) {
            return new { error = e.Message };
        }
    }

    // ── Batch operation dispatchers ───────────────────────────────────

    static object BatchGetSingleton(JsonElement p) {
        var typeName = p.GetProperty("typeName").GetString();

        try {
            var managed = API.GetManagedSingletons();
            foreach (var desc in managed) {
                var instance = desc.Instance;
                if (instance == null) continue;
                if (instance.GetTypeDefinition().GetFullName() == typeName)
                    return new { address = "0x" + instance.GetAddress().ToString("X"), kind = "managed", typeName };
            }
        } catch { }

        try {
            var native = API.GetNativeSingletons();
            foreach (var desc in native) {
                var instance = desc.Instance;
                if (instance == null) continue;
                if (instance.GetTypeDefinition().GetFullName() == typeName)
                    return new { address = "0x" + instance.GetAddress().ToString("X"), kind = "native", typeName };
            }
        } catch { }

        return new { error = $"Singleton '{typeName}' not found" };
    }

    static object BatchGetObject(JsonElement p) {
        var obj = ResolveObjectFromParams(
            p.GetProperty("address").GetString(),
            p.GetProperty("kind").GetString(),
            p.GetProperty("typeName").GetString());
        if (obj == null) return new { error = "Could not resolve object" };

        bool noFields = p.TryGetProperty("noFields", out var nf) && nf.GetBoolean();
        bool noMethods = p.TryGetProperty("noMethods", out var nm) && nm.GetBoolean();
        string filterFields = p.TryGetProperty("fields", out var ff) ? ff.GetString() : null;
        string filterMethods = p.TryGetProperty("methods", out var fm) ? fm.GetString() : null;

        var tdef = obj.GetTypeDefinition();
        int? refCount = obj is ManagedObject m ? m.GetReferenceCount() : (int?)null;

        List<object> fieldList = null;
        if (!noFields) {
            var fields = new List<Field>();
            for (var parent = tdef; parent != null; parent = parent.ParentType)
                fields.AddRange(parent.GetFields());
            fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            HashSet<string> wantFields = null;
            if (filterFields != null)
                wantFields = new HashSet<string>(filterFields.Split(','), StringComparer.OrdinalIgnoreCase);

            fieldList = new List<object>();
            foreach (var field in fields) {
                if (wantFields != null && !wantFields.Contains(field.GetName())) continue;

                var ft = field.GetType();
                var ftName = ft != null ? ft.GetFullName() : "null";
                bool isValueType = ft != null && ft.IsValueType();
                string value = null;
                if (ft != null && (isValueType || ftName == "System.String")) {
                    try { value = ReadFieldValueAsString(obj, field, ft); } catch { }
                }
                fieldList.Add(new {
                    name = field.GetName(), typeName = ftName, isValueType,
                    isStatic = field.IsStatic(),
                    offset = field.IsStatic() ? (string)null : "0x" + field.GetOffsetFromBase().ToString("X"),
                    value
                });
            }
        }

        List<object> methodList = null;
        if (!noMethods) {
            var methods = new List<Method>();
            for (var parent = tdef; parent != null; parent = parent.ParentType)
                methods.AddRange(parent.GetMethods());
            methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
            methods.RemoveAll(mt => mt.GetParameters().Exists(pr => pr.Type.Name.Contains("!")));

            HashSet<string> wantMethods = null;
            if (filterMethods != null)
                wantMethods = new HashSet<string>(filterMethods.Split(','), StringComparer.OrdinalIgnoreCase);

            methodList = new List<object>();
            foreach (var method in methods) {
                if (wantMethods != null && !wantMethods.Contains(method.GetName())) continue;

                var returnT = method.GetReturnType();
                var ps = method.GetParameters();
                var paramList = new List<object>();
                foreach (var pr in ps) paramList.Add(new { type = pr.Type.GetFullName(), name = pr.Name });
                bool isGetter = (method.Name.StartsWith("get_") || method.Name.StartsWith("Get") || method.Name == "ToString") && ps.Count == 0;
                methodList.Add(new {
                    name = method.GetName(), returnType = returnT != null ? returnT.GetFullName() : "void",
                    parameters = paramList, isGetter, signature = method.GetMethodSignature()
                });
            }
        }

        bool isArray = tdef.IsDerivedFrom(s_systemArrayT);
        int? arrayLength = null;
        if (isArray) { try { arrayLength = (int)obj.Call("get_Length"); } catch { } }

        return new {
            typeName = tdef.GetFullName(),
            address = "0x" + (obj as UnifiedObject).GetAddress().ToString("X"),
            refCount, fields = fieldList, methods = methodList, isArray, arrayLength
        };
    }

    static object BatchGetField(JsonElement p) {
        var obj = ResolveObjectFromParams(
            p.GetProperty("address").GetString(),
            p.GetProperty("kind").GetString(),
            p.GetProperty("typeName").GetString());
        if (obj == null) return new { error = "Could not resolve object" };

        var fieldName = p.GetProperty("fieldName").GetString();
        var child = obj.GetField(fieldName) as IObject;
        if (child == null) return new { isNull = true };

        var childTdef = child.GetTypeDefinition();
        bool childManaged = child is ManagedObject;
        return new {
            isNull = false,
            childAddress = "0x" + child.GetAddress().ToString("X"),
            childKind = childManaged ? "managed" : "native",
            childTypeName = childTdef.GetFullName()
        };
    }

    static object BatchInvokeMethod(JsonElement p) {
        var addressStr = p.TryGetProperty("address", out var addrProp) ? addrProp.GetString() : null;
        var kind = p.TryGetProperty("kind", out var kindProp) ? kindProp.GetString() : null;
        var typeName = p.GetProperty("typeName").GetString();

        IObject obj;
        TypeDefinition tdef;

        if (string.IsNullOrEmpty(addressStr)) {
            // Static call
            tdef = TDB.Get().GetType(typeName);
            if (tdef == null) return new { error = $"Type '{typeName}' not found" };
            obj = tdef.CreateInstance(0) ?? (IObject)new NativeObject(0, tdef);
        } else {
            obj = ResolveObjectFromParams(addressStr, kind, typeName);
            if (obj == null) return new { error = "Could not resolve object" };
            tdef = obj.GetTypeDefinition();
        }

        var methodName = p.GetProperty("methodName").GetString();
        var methodSignature = p.TryGetProperty("methodSignature", out var sigProp) ? sigProp.GetString() : null;
        var targetMethod = FindMethod(tdef, methodName, methodSignature);
        if (targetMethod == null) return new { error = "Method not found" };

        object[] args = null;
        if (p.TryGetProperty("args", out var argsProp) && argsProp.ValueKind == JsonValueKind.Array) {
            var ps = targetMethod.GetParameters();
            args = new object[argsProp.GetArrayLength()];
            int i = 0;
            foreach (var argEl in argsProp.EnumerateArray()) {
                var argValue = argEl.GetProperty("value");
                var argType = argEl.TryGetProperty("type", out var atProp) ? atProp.GetString() : null;
                if (string.IsNullOrEmpty(argType) && i < ps.Count) argType = ps[i].Type.GetFullName();
                args[i] = ParseValueFromJson(argValue, argType ?? "System.Int32");
                i++;
            }
        }

        object result = null;
        obj.HandleInvokeMember_Internal(targetMethod, args, ref result);
        return FormatMethodResult(result, targetMethod);
    }

    static object BatchSearch(JsonElement p) {
        var query = p.GetProperty("query").GetString();
        int limit = p.TryGetProperty("limit", out var limProp) ? limProp.GetInt32() : 50;

        var tdb = TDB.Get();
        uint numTypes = tdb.GetNumTypes();
        var queryLower = query.ToLower();
        var results = new List<object>();

        for (uint i = 0; i < numTypes && results.Count < limit; i++) {
            try {
                var t = tdb.GetType(i);
                if (t == null) continue;
                var fullName = t.GetFullName();
                if (string.IsNullOrEmpty(fullName) || !fullName.ToLower().Contains(queryLower)) continue;
                var parentT = t.ParentType;
                results.Add(new {
                    fullName, isValueType = t.IsValueType(), isEnum = t.IsEnum(),
                    numFields = (int)t.GetNumFields(), numMethods = (int)t.GetNumMethods(),
                    parentType = parentT?.GetFullName()
                });
            } catch { }
        }

        return new { query, count = results.Count, results };
    }

    static object BatchGetType(JsonElement p) {
        var typeName = p.GetProperty("typeName").GetString();
        var tdef = TDB.Get().GetType(typeName);
        if (tdef == null) return new { error = $"Type '{typeName}' not found" };

        var parentT = tdef.ParentType;
        var declaringT = tdef.DeclaringType;

        var fields = new List<Field>();
        for (var parent = tdef; parent != null; parent = parent.ParentType) fields.AddRange(parent.GetFields());
        fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

        var fieldList = new List<object>();
        foreach (var field in fields) {
            var ft = field.GetType();
            fieldList.Add(new {
                name = field.GetName(), typeName = ft != null ? ft.GetFullName() : "null",
                isValueType = ft != null && ft.IsValueType(), isStatic = field.IsStatic(),
                offset = field.IsStatic() ? (string)null : "0x" + field.GetOffsetFromBase().ToString("X")
            });
        }

        var methods = new List<Method>();
        for (var parent = tdef; parent != null; parent = parent.ParentType) methods.AddRange(parent.GetMethods());
        methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));
        methods.RemoveAll(mt => mt.GetParameters().Exists(pr => pr.Type.Name.Contains("!")));

        var methodList = new List<object>();
        foreach (var method in methods) {
            var returnT = method.GetReturnType();
            var ps = method.GetParameters();
            var paramList = new List<object>();
            foreach (var pr in ps) paramList.Add(new { type = pr.Type.GetFullName(), name = pr.Name });
            bool isGetter = (method.Name.StartsWith("get_") || method.Name.StartsWith("Get") || method.Name == "ToString") && ps.Count == 0;
            methodList.Add(new {
                name = method.GetName(), returnType = returnT != null ? returnT.GetFullName() : "void",
                parameters = paramList, isGetter, signature = method.GetMethodSignature()
            });
        }

        return new {
            fullName = tdef.GetFullName(), @namespace = tdef.GetNamespace(),
            isValueType = tdef.IsValueType(), isEnum = tdef.IsEnum(), size = tdef.GetSize(),
            parentType = parentT?.GetFullName(), declaringType = declaringT?.GetFullName(),
            fields = fieldList, methods = methodList
        };
    }

    static object BatchSetField(JsonElement p) {
        var obj = ResolveObjectFromParams(
            p.GetProperty("address").GetString(),
            p.GetProperty("kind").GetString(),
            p.GetProperty("typeName").GetString());
        if (obj == null) return new { error = "Could not resolve object" };

        var fieldName = p.GetProperty("fieldName").GetString();
        var tdef = obj.GetTypeDefinition();
        Field targetField = null;
        for (var parent = tdef; parent != null; parent = parent.ParentType) {
            foreach (var f in parent.GetFields()) {
                if (f.GetName() == fieldName) { targetField = f; break; }
            }
            if (targetField != null) break;
        }
        if (targetField == null) return new { error = $"Field '{fieldName}' not found" };

        var ft = targetField.GetType();
        if (ft == null) return new { error = "Field type is null" };
        if (!ft.IsValueType()) return new { error = "Can only write value-type fields" };

        var valueTypeName = p.TryGetProperty("valueType", out var vtProp) ? vtProp.GetString() : null;
        if (string.IsNullOrEmpty(valueTypeName))
            valueTypeName = ft.IsEnum() ? ft.GetUnderlyingType().GetFullName() : ft.GetFullName();

        var boxedValue = ParseValueFromJson(p.GetProperty("value"), valueTypeName);
        if (boxedValue == null) return new { error = $"Could not parse value as '{valueTypeName}'" };

        targetField.SetDataBoxed(obj.GetAddress(), boxedValue, false);
        return new { ok = true, field = fieldName, value = boxedValue.ToString() };
    }
}
