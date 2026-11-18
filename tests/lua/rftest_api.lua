-- rftest API suite: checks that the documented Lua API (reframework-book/src/api) behaves as documented.
-- Game-agnostic: uses only System.*, via.* types present in every RE Engine game, so it can be run over every
-- supported game. Expected result on a healthy build: every test PASSES (SKIP only where a game lacks something).
-- See tests/README.md.

local H = require("rftest.harness")
local S = H.suite("api")

local function tdef(name) return H.truthy(sdk.find_type_definition(name), name) end

-------------------------------------------------------------------------------------------------------------------
-- reframework / re / log / thread
-------------------------------------------------------------------------------------------------------------------
S:test("reframework info", function()
    H.eq(type(reframework:get_game_name()), "string", "get_game_name")
    H.ne(reframework:get_game_name(), "", "get_game_name")
    H.eq(type(reframework:get_version_string()), "string", "get_version_string")
    H.eq(type(reframework:get_commit_hash()), "string", "get_commit_hash")
    H.eq(type(reframework:is_drawing_ui()), "boolean", "is_drawing_ui")
    H.eq(reframework:is_key_down(-1), false, "is_key_down(-1)")
    H.eq(reframework:is_key_down(256), false, "is_key_down(256)")
end)

S:test("log functions accept strings", function()
    log.info("[rftest] api: log.info"); log.warn("[rftest] api: log.warn"); log.debug("[rftest] api: log.debug")
end)

S:test("thread ids", function()
    H.eq(math.type(thread.get_id()), "integer", "thread.get_id")
    H.eq(type(thread.get_hash()), "number", "thread.get_hash") -- size_t: large values arrive as floats
end)

do
    local counts = { frame = 0, pre_update = 0, update = 0 }
    re.on_frame(function() counts.frame = counts.frame + 1 end)
    re.on_pre_application_entry("UpdateBehavior", function() counts.pre_update = counts.pre_update + 1 end)
    re.on_application_entry("UpdateBehavior", function() counts.update = counts.update + 1 end)
    S:frame_test("re.on_frame / on_pre_application_entry / on_application_entry fire", function(frame)
        if frame < 30 then return false end
        H.truthy(counts.frame >= 29, "on_frame count " .. counts.frame)
        H.truthy(counts.pre_update > 0, "on_pre_application_entry(UpdateBehavior) count " .. counts.pre_update)
        H.truthy(counts.update > 0, "on_application_entry(UpdateBehavior) count " .. counts.update)
        return true
    end)

    local saved = false
    re.on_config_save(function() saved = true end)
    S:frame_test("re.on_config_save fires after reframework:save_config()", function(frame)
        if frame == 1 then reframework:save_config() end
        return saved
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- json / fs / io
-------------------------------------------------------------------------------------------------------------------
S:test("json string round trip", function()
    local src = { int = 42, float = 1.5, neg = -7, str = "h\195\169llo", yes = true, no = false, list = { 1, 2, 3 }, nested = { a = { b = "c" } } }
    local back = H.truthy(json.load_string(json.dump_string(src)), "load_string")
    H.eq(back.int, 42); H.eq(back.float, 1.5); H.eq(back.neg, -7); H.eq(back.str, src.str)
    H.eq(back.yes, true); H.eq(back.no, false); H.eq(#back.list, 3); H.eq(back.list[3], 3); H.eq(back.nested.a.b, "c")
end)

S:test("json.load_string of invalid JSON returns nil", function()
    H.eq(json.load_string("{not json"), nil)
end)

S:test("json file round trip and indent", function()
    H.eq(json.dump_file("rftest\\scratch\\api.json", { k = "v", n = 3 }, 2), true, "dump_file")
    local back = H.truthy(json.load_file("rftest\\scratch\\api.json"), "load_file")
    H.eq(back.k, "v"); H.eq(back.n, 3)
    H.eq(json.load_file("rftest\\scratch\\no_such.json"), nil, "missing file")
end)

S:test("json rejects parent-directory paths", function()
    H.eq(json.dump_file("..\\rftest_parent.json", {}), false, "dump_file('..')")
    H.raises(function() json.load_file("..\\re2_framework_log.txt") end, "load_file('..')")
end)

S:test("fs.write / fs.read / fs.glob", function()
    fs.write("rftest\\scratch\\glob_a.txt", "A")
    fs.write("rftest\\scratch\\glob_b.txt", "BB")
    H.eq(fs.read("rftest\\scratch\\glob_b.txt"), "BB", "fs.read")
    local found = {}
    for _, p in ipairs(fs.glob([[rftest\\scratch\\glob_.*\.txt]])) do found[p:match("[^\\/]+$")] = true end
    H.truthy(found["glob_a.txt"] and found["glob_b.txt"], "glob found both files")
end)

S:test("fs rejects parent-directory and absolute paths", function()
    H.raises(function() fs.read("..\\re2_framework_log.txt") end, "fs.read('..')")
    H.raises(function() fs.write("C:\\rftest_abs.txt", "x") end, "fs.write('C:\\\\...')")
end)

S:test("io.open / write / lines / read", function()
    local f = H.truthy(io.open("rftest\\scratch\\io.txt", "w"), "io.open w")
    f:write("one\n", "two\n"); f:close()
    local lines = {}
    for l in io.lines("rftest\\scratch\\io.txt") do lines[#lines + 1] = l end
    H.eq(#lines, 2, "line count"); H.eq(lines[2], "two")
    local r = H.truthy(io.open("rftest\\scratch\\io.txt", "r"), "io.open r")
    H.eq(r:read("a"), "one\ntwo\n"); r:close()
    H.raises(function() io.open("..\\rftest_parent.txt", "w") end, "io.open('..')")
end)

S:test("unsafe os/io functions are removed", function()
    H.eq(io.popen, nil, "io.popen")
    H.eq(os.execute, nil, "os.execute")
    H.eq(os.remove, nil, "os.remove")
    H.eq(os.rename, nil, "os.rename")
end)

-------------------------------------------------------------------------------------------------------------------
-- Type database
-------------------------------------------------------------------------------------------------------------------
S:test("sdk.get_tdb_version / game_namespace", function()
    H.truthy(sdk.get_tdb_version() > 0, "tdb version")
    H.eq(type(sdk.game_namespace("x")), "string", "game_namespace")
end)

S:test("find_type_definition", function()
    local s = tdef("System.String")
    H.eq(s:get_full_name(), "System.String"); H.eq(s:get_name(), "String"); H.eq(s:get_namespace(), "System")
    H.eq(s:is_value_type(), false, "String is_value_type")
    H.eq(s:get_parent_type():get_full_name(), "System.Object", "String parent")
    H.truthy(s:is_a("System.Object"), "String is_a Object")
    H.eq(s:is_a("System.Int32"), false, "String is_a Int32")
    local i = tdef("System.Int32")
    H.eq(i:is_value_type(), true, "Int32 is_value_type")
    H.eq(i:get_valuetype_size(), 4, "Int32 valuetype size")
    H.eq(sdk.find_type_definition("rftest.NoSuchType"), nil, "missing type")
end)

S:test("methods: lookup, signature, enumeration", function()
    local s = tdef("System.String")
    local m = H.truthy(s:get_method("Substring(System.Int32, System.Int32)"), "Substring(Int32, Int32)")
    H.eq(m:get_name(), "Substring"); H.eq(m:get_num_params(), 2); H.eq(m:is_static(), false)
    H.eq(m:get_return_type():get_full_name(), "System.String", "return type")
    local pts = m:get_param_types()
    H.eq(#pts, 2); H.eq(pts[1]:get_full_name(), "System.Int32")
    H.eq(#m:get_param_names(), 2, "param names")
    H.eq(m:get_declaring_type():get_full_name(), "System.String", "declaring type")
    H.ne(sdk.to_int64(m:get_function()), 0, "function pointer")
    H.truthy(#s:get_methods() > 10, "String has methods")
    H.truthy(tdef("System.Math"):get_method("Max(System.Int32, System.Int32)"):is_static(), "Math.Max is static")
end)

S:test("fields: lookup and offsets (via.vec3)", function()
    local v = tdef("via.vec3")
    H.eq(v:is_value_type(), true)
    local names = {}
    for _, f in ipairs(v:get_fields()) do if not f:is_static() then names[f:get_name()] = f end end
    H.truthy(names.x and names.y and names.z, "x, y, z fields")
    H.eq(v:get_field("y"):get_offset_from_fieldptr(), 4, "y offset")
    H.eq(v:get_field("z"):get_offset_from_fieldptr(), 8, "z offset")
    H.eq(v:get_field("x"):get_type():get_full_name(), "System.Single", "x type")
end)

S:test("sdk.typeof returns a runtime type", function()
    local t = H.truthy(sdk.typeof("System.String"), "typeof")
    H.truthy(H.type_name(t):find("RuntimeType", 1, true), "typeof type name " .. tostring(H.type_name(t)))
end)

-------------------------------------------------------------------------------------------------------------------
-- Managed objects, strings, arrays, boxing
-------------------------------------------------------------------------------------------------------------------
S:test("managed strings: create, call, convert", function()
    local s = H.truthy(sdk.create_managed_string("RfTest"), "create_managed_string")
    H.eq(H.type_name(s), "System.String")
    H.eq(s:call("ToUpper"), "RFTEST", "ToUpper")
    H.eq(s:call("get_Length"), 6, "get_Length")
    H.eq(s:call("Substring(System.Int32, System.Int32)", 1, 3), "fTe", "Substring(1, 3)")
    H.eq(s:call("Contains(System.String)", "Te"), true, "Contains")
    H.eq(s:call("IndexOf(System.String)", "Te"), 2, "IndexOf")
end)

S:test("method call via REMethodDefinition (instance and static)", function()
    local s = sdk.create_managed_string("abc")
    H.eq(tdef("System.String"):get_method("ToUpper"):call(s), "ABC", "instance call")
    H.eq(tdef("System.Math"):get_method("Max(System.Int32, System.Int32)"):call(nil, 3, 7), 7, "Math.Max")
    H.near(tdef("System.Math"):get_method("Sqrt(System.Double)"):call(nil, 6.25), 2.5, 1e-9, "Math.Sqrt(Double)")
end)

S:test("object addresses round trip", function()
    local s = sdk.create_managed_string("addr")
    local addr = s:get_address()
    H.eq(math.type(addr), "integer")
    H.eq(sdk.is_managed_object(addr), true, "is_managed_object(addr)")
    local back = H.truthy(sdk.to_managed_object(addr), "to_managed_object")
    H.eq(back:get_address(), addr, "same object")
    H.eq(sdk.is_managed_object(0), false, "is_managed_object(0)")
    H.eq(sdk.to_managed_object(0), nil, "to_managed_object(0)")
end)

S:test("managed arrays", function()
    local a = H.truthy(sdk.create_managed_array("System.Int32", 5), "create_managed_array")
    H.eq(a:get_size(), 5, "size")
    H.eq(H.type_name(a), "System.Int32[]", "array type")
    local strs = sdk.create_managed_array("System.String", 2)
    strs[0] = sdk.create_managed_string("first")
    H.eq(strs[0]:call("ToString"), "first", "string element")
    H.eq(#strs:get_elements(), 2, "get_elements")
    H.raises(function() sdk.create_managed_array("System.Int32", -1) end, "negative length")
end)

S:test("boxed primitives", function()
    H.eq(H.type_name(sdk.create_int32(5)), "System.Int32")
    H.eq(H.type_name(sdk.create_single(1.5)), "System.Single")
    H.eq(H.type_name(sdk.create_double(1.5)), "System.Double")
    H.eq(H.type_name(sdk.create_uint64(1)), "System.UInt64")
    H.eq(H.type_name(sdk.create_byte(1)), "System.Byte")
end)

S:test("pointer conversions", function()
    H.eq(sdk.to_int64(sdk.to_ptr(123456789)), 123456789)
    H.eq(sdk.to_int64(sdk.to_ptr(-1)), -1, "negative integer")
    -- Floats convert to the nearest integer (hooks return computed integers this way, e.g. to_ptr(x / 2)).
    H.eq(sdk.to_int64(sdk.to_ptr(6 / 2)), 3, "to_ptr(6 / 2)")
    H.eq(sdk.to_float(sdk.float_to_ptr(1.5)), 1.5)
    H.eq(sdk.to_int64(sdk.to_ptr(true)), 1)
end)

-------------------------------------------------------------------------------------------------------------------
-- ValueType / MemoryView
-------------------------------------------------------------------------------------------------------------------
S:test("ValueType fields and memory", function()
    local v = ValueType.new(tdef("via.vec3"))
    v:set_field("x", 1.5); v.y = 2.5; v:set_field("z", -3)
    H.eq(v:get_field("x"), 1.5, "x"); H.eq(v.y, 2.5, "y"); H.eq(v.z, -3, "z")
    H.eq(v:read_float(4), 2.5, "read_float(4)")
    v:write_float(0, 9.0)
    H.eq(v.x, 9.0, "x after write_float")
    H.eq(H.type_name(v), "via.vec3")
    H.ne(v:get_address(), 0, "address")
end)

S:test("ValueType method call", function()
    local g = ValueType.new(tdef("System.Guid"))
    for i = 0, 15 do g:write_byte(i, i) end
    local str = g:call("ToString()")
    H.eq(type(str), "string", "Guid.ToString type")
    H.eq(#str, 36, "Guid.ToString length " .. tostring(str))
end)

-------------------------------------------------------------------------------------------------------------------
-- Call variants: parameter counts, types and orders. Every signature below exists in re7, re2, ggr, mhrise, dd2, re9
-- and gs456 (scan of 2026-11-17); a game without one SKIPs that test.
-------------------------------------------------------------------------------------------------------------------
local function method(tn, sig)
    local t = sdk.find_type_definition(tn)
    local m = t and t:get_method(sig)
    if m == nil then H.skip(tn .. "." .. sig .. " is not in this game") end
    return m
end

local function scall(tn, sig, ...) return method(tn, sig):call(nil, ...) end

local function string_array(...)
    local values = { ... }
    local arr = sdk.create_managed_array("System.String", #values)
    for i, v in ipairs(values) do arr[i - 1] = sdk.create_managed_string(v) end
    return arr
end

S:test("3 integer parameters (Int32, Int64, UInt32)", function()
    H.eq(scall("via.math", "clamp(System.Int32, System.Int32, System.Int32)", 15, 0, 10), 10, "clamp(15, 0, 10)")
    H.eq(scall("via.math", "clamp(System.Int32, System.Int32, System.Int32)", -3, 0, 10), 0, "clamp(-3, 0, 10)")
    H.eq(scall("via.math", "clamp(System.Int64, System.Int64, System.Int64)", 1 << 40, 0, 1 << 35), 1 << 35, "Int64 clamp")
    H.eq(scall("via.math", "clamp(System.Int64, System.Int64, System.Int64)", -(1 << 40), -(1 << 36), 0), -(1 << 36), "Int64 negative clamp")
    H.eq(scall("via.math", "clamp(System.UInt32, System.UInt32, System.UInt32)", 5, 1, 3), 3, "UInt32 clamp")
end)

S:test("3 floating-point parameters (Single, Double)", function()
    H.eq(scall("via.math", "clamp(System.Single, System.Single, System.Single)", 1.5, 0.0, 1.0), 1.0, "Single clamp high")
    H.eq(scall("via.math", "clamp(System.Single, System.Single, System.Single)", 0.25, 0.0, 1.0), 0.25, "Single clamp inside")
    H.near(scall("via.math", "lerp(System.Single, System.Single, System.Single)", 10.0, 20.0, 0.25), 12.5, 1e-5, "Single lerp")
    H.near(scall("via.math", "inverseLerp(System.Single, System.Single, System.Single)", 10.0, 20.0, 15.0), 0.5, 1e-5, "inverseLerp")
    H.eq(scall("via.math", "clamp(System.Double, System.Double, System.Double)", 2.5, -1.0, 2.0), 2.0, "Double clamp")
    H.near(scall("via.math", "lerp(System.Double, System.Double, System.Double)", -1.0, 1.0, 0.75), 0.5, 1e-12, "Double lerp")
end)

S:test("Single parameters by value and Single returns", function()
    H.eq(scall("System.Math", "Max(System.Single, System.Single)", 1.5, -2.5), 1.5, "Max")
    H.eq(scall("System.Math", "Min(System.Single, System.Single)", 1.5, -2.5), -2.5, "Min")
    H.eq(sdk.create_single(1.5):call("CompareTo(System.Single)", 2.0), -1, "boxed Single.CompareTo(Single)")
end)

S:test("64-bit integer parameters and returns", function()
    H.eq(scall("System.Convert", "ToInt64(System.Double)", 1099511627776.0), 1099511627776, "ToInt64(2^40)")
    H.eq(scall("System.Convert", "ToDouble(System.Int64)", -(1 << 53)), -9007199254740992.0, "ToDouble(Int64 -2^53)")
    H.eq(scall("System.Convert", "ToDouble(System.UInt64)", -1), 18446744073709551615.0, "ToDouble(UInt64 max)")
end)

S:test("Boolean parameter after two strings", function()
    H.eq(scall("System.String", "Compare(System.String, System.String, System.Boolean)", "abc", "ABC", true), 0, "ignoreCase = true")
    H.ne(scall("System.String", "Compare(System.String, System.String, System.Boolean)", "abc", "ABC", false), 0, "ignoreCase = false")
end)

S:test("Char parameters and Int32 + Char", function()
    local pad = method("System.String", "PadLeft(System.Int32, System.Char)")
    local replace = method("System.String", "Replace(System.Char, System.Char)")
    H.eq(pad:call(sdk.create_managed_string("ab"), 5, 42), "***ab", "PadLeft(5, '*')")
    H.eq(replace:call(sdk.create_managed_string("a.b.c"), 46, 45), "a-b-c", "Replace('.', '-')")
end)

S:test("enum parameter (StringComparison)", function()
    local equals = method("System.String", "Equals(System.String, System.StringComparison)")
    local s = sdk.create_managed_string("abc")
    H.eq(equals:call(s, "ABC", 5), true, "OrdinalIgnoreCase")
    H.eq(equals:call(s, "ABC", 4), false, "Ordinal")
end)

S:test("4 string parameters", function()
    H.eq(scall("System.String", "Concat(System.String, System.String, System.String, System.String)", "a", "b", "c", "d"), "abcd")
end)

S:test("string + 3 boxed object parameters", function()
    H.eq(scall("System.String", "Format(System.String, System.Object, System.Object, System.Object)", "{0}-{1}-{2}",
        sdk.create_int32(1), sdk.create_managed_string("x"), sdk.create_int32(3)), "1-x-3")
end)

S:test("4 parameters: array, object, Int32, Int32", function()
    local arr = string_array("a", "b", "c", "d")
    H.eq(scall("System.Array", "IndexOf(System.Array, System.Object, System.Int32, System.Int32)", arr, arr[2], 1, 3), 2,
        "IndexOf(arr, arr[2], 1, 3)")
    H.eq(scall("System.Array", "IndexOf(System.Array, System.Object, System.Int32, System.Int32)", arr, arr[0], 1, 3), -1,
        "IndexOf outside the range")
end)

S:test("5 parameters: array, Int32, array, Int32, Int32", function()
    local src = string_array("a", "b", "c", "d")
    local dst = string_array("w", "x", "y", "z")
    scall("System.Array", "Copy(System.Array, System.Int32, System.Array, System.Int32, System.Int32)", src, 1, dst, 2, 2)
    H.eq(dst[0]:call("ToString"), "w"); H.eq(dst[1]:call("ToString"), "x")
    H.eq(dst[2]:call("ToString"), "b"); H.eq(dst[3]:call("ToString"), "c")
end)

S:test("struct parameters and struct returns (via.vec2/vec3, Quaternion, mat4)", function()
    local v = scall("via.vec3", "op_Addition(via.vec3, via.vec3)", Vector3f.new(1, 2, 3), Vector3f.new(4, 5, 6))
    H.eq(v.x, 5); H.eq(v.y, 7); H.eq(v.z, 9)
    local m = scall("via.vec3", "op_Multiply(via.vec3, System.Single)", Vector3f.new(1, 2, 3), 2.0)
    H.eq(m.x, 2); H.eq(m.z, 6)
    local m2 = scall("via.vec2", "op_Multiply(via.vec2, System.Single)", Vector2f.new(3, 4), 0.5)
    H.eq(m2.x, 1.5); H.eq(m2.y, 2)
    H.eq(scall("via.vec3", "op_Equality(via.vec3, via.vec3)", Vector3f.new(1, 2, 3), Vector3f.new(1, 2, 3)), true, "op_Equality")
    H.eq(scall("via.vec3", "op_Equality(via.vec3, via.vec3)", Vector3f.new(1, 2, 3), Vector3f.new(1, 2, 4)), false, "op_Equality")
    local half = math.sqrt(0.5)
    local q = scall("via.Quaternion", "op_Multiply(via.Quaternion, via.Quaternion)", Quaternion.identity(), Quaternion.new(half, 0, 0, half))
    H.near(q.w, half, 1e-6, "Quaternion op_Multiply w"); H.near(q.z, half, 1e-6, "Quaternion op_Multiply z")
    local t = Matrix4x4f.new(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 5, 6, 7, 1)
    local p = scall("via.mat4", "op_Multiply(via.mat4, via.mat4)", Matrix4x4f.identity(), t)
    H.eq(p[3].x, 5, "mat4 op_Multiply translation x"); H.eq(p[3].z, 7, "mat4 op_Multiply translation z")
end)

S:test("ValueType instance call with Int32 + Single", function()
    method("via.vec3", "set_Item(System.Int32, System.Single)") -- skips where via.vec3 has no set_Item (starforce)
    local v = ValueType.new(tdef("via.vec3"))
    v:call("set_Item(System.Int32, System.Single)", 1, 2.5)
    v:call("set_Item(System.Int32, System.Single)", 2, -4.0)
    H.eq(v.x, 0, "x untouched"); H.eq(v.y, 2.5, "y"); H.eq(v.z, -4.0, "z")
end)

-------------------------------------------------------------------------------------------------------------------
-- Scene access (via.* singletons exist in every game)
-------------------------------------------------------------------------------------------------------------------
-- A frame test: some games (re7) have no transforms in the scene yet when autorun scripts load.
S:frame_test("native singleton + call_native_func + scene components", function(frame)
    local sm_t = tdef("via.SceneManager")
    local sm = H.truthy(sdk.get_native_singleton("via.SceneManager"), "via.SceneManager singleton")
    local scene = H.truthy(sdk.call_native_func(sm, sm_t, "get_CurrentScene()"), "get_CurrentScene")
    H.truthy(H.type_name(scene):find("via.Scene", 1, true), "scene type " .. tostring(H.type_name(scene)))
    local comps = H.truthy(scene:call("findComponents(System.Type)", sdk.typeof("via.Transform")), "findComponents")
    if comps:get_size() == 0 then
        if frame < 600 then return false end
        error("no via.Transform in the current scene after 600 frames")
    end
    local t = comps[0]
    H.eq(H.type_name(t), "via.Transform")
    local go = H.truthy(t:call("get_GameObject"), "get_GameObject")
    H.eq(type(go:call("get_Name")), "string", "GameObject name")
    local pos = t:call("get_Position")
    H.eq(type(pos.x), "number", "position.x")
    return true
end)

S:test("sdk.get_thread_context", function()
    H.truthy(sdk.get_thread_context() ~= nil, "thread context")
end)

-------------------------------------------------------------------------------------------------------------------
-- Hooks (installed at the end of the frame in which sdk.hook is called; checked from a frame test)
-------------------------------------------------------------------------------------------------------------------
do
    local max = tdef("System.Math"):get_method("Max(System.Int32, System.Int32)")
    local mode, seen = nil, {}
    sdk.hook(max, function(args)
        if mode == nil then return end
        seen.a, seen.b = sdk.to_int64(args[2]) & 0xFFFFFFFF, sdk.to_int64(args[3]) & 0xFFFFFFFF
        seen.storage = thread.get_hook_storage()
        if seen.storage ~= nil then seen.storage.marker = mode end
        if mode == "args" then args[3] = sdk.to_ptr(100) end
        if mode == "skip" then return sdk.PreHookResult.SKIP_ORIGINAL end
    end, function(retval)
        if mode == nil then return retval end
        seen.post_storage = thread.get_hook_storage()
        if mode == "retval" or mode == "skip" then return sdk.to_ptr(42) end
        return retval
    end)

    S:frame_test("sdk.hook: args, return value, SKIP_ORIGINAL, hook storage", function(frame)
        if frame < 3 then return false end
        local function run(m, a, b)
            mode, seen = m, {}
            local ok, r = pcall(max.call, max, nil, a, b)
            mode = nil
            if not ok then error(r) end
            return r
        end
        H.eq(run("observe", 3, 7), 7, "unmodified call")
        H.eq(seen.a, 3, "args[2]"); H.eq(seen.b, 7, "args[3]")
        H.truthy(seen.storage ~= nil, "hook storage in pre")
        H.truthy(rawequal(seen.storage, seen.post_storage), "same hook storage in post")
        H.eq(seen.storage.marker, "observe", "storage contents")
        H.eq(run("args", 3, 7), 100, "modified args[3]")
        H.eq(run("retval", 3, 7), 42, "replaced return value")
        H.eq(run("skip", 3, 7), 42, "SKIP_ORIGINAL with post return value")
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- sdk.hook across signatures: what the pre-hook sees in args (args[1] is the thread context; static methods have
-- their parameters from args[2], instance methods have this in args[2] and parameters from args[3]), changes to
-- args and return values, register vs stack arguments, int/float/double/bool/struct returns. Each hook only acts
-- on our own call (same case, same thread), since the game calls most of these methods too.
-------------------------------------------------------------------------------------------------------------------
do
    local armed_case, armed_tid = nil, nil

    local function i64(p) return sdk.to_int64(p) end
    local function i32(p) return sdk.to_int64(p) & 0xFFFFFFFF end
    local function dbl(x) return sdk.to_ptr((string.unpack("<i8", string.pack("<d", x)))) end

    -- def: args (list), this (instance object or nil), invoke(this, ...) (calls the method; default m:call), pre(args,
    -- seen) (reads into seen, may change args, may return a PreHookResult), post(retval, seen) (may return a
    -- replacement), check(result, seen) (assertions).
    local function hook_case(name, tn, sig, def)
        local t = sdk.find_type_definition(tn)
        local m = t and t:get_method(sig)
        local c = {}

        if m ~= nil then
            sdk.hook(m, function(args)
                if armed_case ~= c or thread.get_id() ~= armed_tid then return end
                c.seen.pre = true
                if def.pre then return def.pre(args, c.seen) end
            end, function(retval)
                if armed_case ~= c or thread.get_id() ~= armed_tid then return retval end
                c.seen.post = true
                c.seen.retval = retval
                if def.post then return def.post(retval, c.seen) end
                return retval
            end)
        end

        S:frame_test("sdk.hook " .. name, function(frame)
            if m == nil then H.skip(tn .. "." .. sig .. " is not in this game") end
            if frame < 3 then return false end
            c.seen = {}
            local this = def.this and def.this() or nil
            armed_case, armed_tid = c, thread.get_id()
            local call = def.invoke or function(obj, ...) return m:call(obj, ...) end
            local ok, result = pcall(call, this, table.unpack(def.args()))
            armed_case = nil
            if not ok then error(result, 0) end
            if not c.seen.pre then H.skip("the hook did not fire for a call through invoke") end
            H.truthy(c.seen.post, "post hook ran")
            c.seen.this = this
            def.check(result, c.seen)
            return true
        end)
    end

    hook_case("static, 3 Int32: read args, change the last one", "via.math", "clamp(System.Int32, System.Int32, System.Int32)", {
        args = function() return { 15, 0, 10 } end,
        pre = function(args, seen) seen.a = { i32(args[2]), i32(args[3]), i32(args[4]) }; args[4] = sdk.to_ptr(5) end,
        check = function(result, seen)
            H.eq(seen.a[1], 15, "args[2]"); H.eq(seen.a[2], 0, "args[3]"); H.eq(seen.a[3], 10, "args[4]")
            H.eq(result, 5, "clamp(15, 0, 5) after changing args[4]")
        end,
    })

    hook_case("static, 3 Int32: replace the Int32 return", "via.math", "clamp(System.Int32, System.Int32, System.Int32)", {
        args = function() return { 15, 0, 10 } end,
        post = function() return sdk.to_ptr(99) end,
        check = function(result) H.eq(result, 99, "replaced return value") end,
    })

    hook_case("static, 3 Single (xmm1-3): read args, change one", "via.math", "clamp(System.Single, System.Single, System.Single)", {
        args = function() return { 1.5, 0.0, 1.0 } end,
        pre = function(args, seen)
            seen.f = { sdk.to_float(args[2]), sdk.to_float(args[3]), sdk.to_float(args[4]) }
            args[4] = sdk.float_to_ptr(0.5)
        end,
        check = function(result, seen)
            H.eq(seen.f[1], 1.5, "args[2]"); H.eq(seen.f[2], 0.0, "args[3]"); H.eq(seen.f[3], 1.0, "args[4]")
            H.eq(result, 0.5, "clamp(1.5, 0, 0.5) after changing args[4]")
        end,
    })

    hook_case("static, 3 Single: replace the Single return (xmm0)", "via.math", "clamp(System.Single, System.Single, System.Single)", {
        args = function() return { 0.25, 0.0, 1.0 } end,
        post = function() return sdk.float_to_ptr(0.75) end,
        check = function(result) H.eq(result, 0.75, "replaced return value") end,
    })

    hook_case("static, 3 Double: read args", "via.math", "lerp(System.Double, System.Double, System.Double)", {
        args = function() return { -1.0, 1.0, 0.75 } end,
        pre = function(args, seen) seen.d = { sdk.to_double(args[2]), sdk.to_double(args[3]), sdk.to_double(args[4]) } end,
        check = function(result, seen)
            H.eq(seen.d[1], -1.0, "args[2]"); H.eq(seen.d[2], 1.0, "args[3]"); H.eq(seen.d[3], 0.75, "args[4]")
            H.near(result, 0.5, 1e-12, "result")
        end,
    })

    do
        local src, dst
        hook_case("static, 5 args (2 on the stack): read and change a stack arg", "System.Array",
            "Copy(System.Array, System.Int32, System.Array, System.Int32, System.Int32)", {
            args = function()
                src = string_array("a", "b", "c", "d")
                dst = string_array("w", "x", "y", "z")
                return { src, 1, dst, 2, 2 }
            end,
            pre = function(args, seen)
                seen.a = { H.type_name(sdk.to_managed_object(args[2])), i32(args[3]), H.type_name(sdk.to_managed_object(args[4])), i32(args[5]), i32(args[6]) }
                args[6] = sdk.to_ptr(1) -- length: copy one element instead of two
            end,
            check = function(_, seen)
                H.eq(seen.a[1], "System.String[]", "args[2] is an array"); H.eq(seen.a[2], 1, "args[3]")
                H.eq(seen.a[3], "System.String[]", "args[4] is an array"); H.eq(seen.a[4], 2, "args[5] (stack)")
                H.eq(seen.a[5], 2, "args[6] (stack)")
                H.eq(dst[2]:call("ToString"), "b", "copied element"); H.eq(dst[3]:call("ToString"), "z", "length changed to 1")
            end,
        })
    end

    hook_case("instance, Int32 + Char: this in args[2], change the Char", "System.String", "PadLeft(System.Int32, System.Char)", {
        this = function() return sdk.create_managed_string("ab") end,
        args = function() return { 5, 42 } end,
        pre = function(args, seen) seen.this_addr = i64(args[2]); seen.a = { i32(args[3]), i32(args[4]) & 0xFFFF }; args[4] = sdk.to_ptr(45) end,
        check = function(result, seen)
            H.eq(seen.this_addr, seen.this:get_address(), "args[2] is this")
            H.eq(seen.a[1], 5, "args[3]"); H.eq(seen.a[2], 42, "args[4]")
            H.eq(result, "---ab", "PadLeft(5, '-') after changing args[4]")
        end,
    })

    hook_case("instance, enum arg: change it, Boolean return follows", "System.String", "Equals(System.String, System.StringComparison)", {
        this = function() return sdk.create_managed_string("abc") end,
        args = function() return { "ABC", 5 } end,
        pre = function(args, seen) seen.cmp = i32(args[4]); args[4] = sdk.to_ptr(4) end,
        check = function(result, seen)
            H.eq(seen.cmp, 5, "args[4] (OrdinalIgnoreCase)")
            H.eq(result, false, "Equals with Ordinal after changing args[4]")
        end,
    })

    hook_case("instance: replace the Boolean return", "System.String", "Equals(System.String, System.StringComparison)", {
        this = function() return sdk.create_managed_string("abc") end,
        args = function() return { "ABC", 4 } end,
        post = function() return sdk.to_ptr(1) end,
        check = function(result) H.eq(result, true, "replaced return value") end,
    })

    -- A method returning a struct gets the return buffer as its first native argument, so for these args[1] is
    -- that buffer, args[2] the thread context, and the parameters (here pointers to the vectors) follow.
    hook_case("static, struct return (vec3) with struct args: args[1] is the return buffer", "via.vec3", "op_Addition(via.vec3, via.vec3)", {
        args = function() return { Vector3f.new(1, 2, 3), Vector3f.new(4, 5, 6) } end,
        pre = function(args, seen)
            seen.ret_buffer = i64(args[1])
            seen.ax = sdk.to_valuetype(args[3], "via.vec3"):read_float(0)
            seen.bz = sdk.to_valuetype(args[4], "via.vec3"):read_float(8)
        end,
        check = function(result, seen)
            H.eq(seen.ax, 1, "args[3] points at the first vector"); H.eq(seen.bz, 6, "args[4] points at the second vector")
            H.eq(i64(seen.retval), seen.ret_buffer, "post-hook retval is the return buffer")
            H.eq(result.x, 5); H.eq(result.y, 7); H.eq(result.z, 9)
        end,
    })

    -- Instance method of a value type returning a struct: return buffer, context, this, then the Double in xmm3.
    hook_case("value-type instance, struct return (DateTime) + Double: args[4] is the Double", "System.DateTime", "AddHours(System.Double)", {
        this = function() return ValueType.new(sdk.find_type_definition("System.DateTime")) end,
        invoke = function(this, ...) return this:call("AddHours(System.Double)", ...) end,
        args = function() return { 2.5 } end,
        pre = function(args, seen)
            seen.ret_buffer = i64(args[1])
            seen.hours = sdk.to_double(args[4])
            args[4] = dbl(1.0)
        end,
        check = function(result, seen)
            H.eq(seen.hours, 2.5, "args[4]")
            H.eq(i64(seen.retval), seen.ret_buffer, "post-hook retval is the return buffer")
            -- The top two bits of DateTime's data are its Kind (some games set Local); the rest are ticks.
            H.eq(result:read_qword(0) & 0x3FFFFFFFFFFFFFFF, 36000000000, "ticks of AddHours(1.0) after changing args[4]")
        end,
    })

    -- Four Single parameters after context and this: two in xmm2/xmm3, two on the stack.
    hook_case("value-type instance, 4 Single (2 on the stack): read them, change a register and a stack one", "via.vec4",
        ".ctor(System.Single, System.Single, System.Single, System.Single)", {
        this = function() return ValueType.new(sdk.find_type_definition("via.vec4")) end,
        invoke = function(this, ...) this:call(".ctor(System.Single, System.Single, System.Single, System.Single)", ...); return this end,
        args = function() return { 1.0, 2.0, 3.0, 4.0 } end,
        pre = function(args, seen)
            seen.f = { sdk.to_float(args[3]), sdk.to_float(args[4]), sdk.to_float(args[5]), sdk.to_float(args[6]) }
            args[3] = sdk.float_to_ptr(0.5)
            args[6] = sdk.float_to_ptr(8.0)
        end,
        check = function(v, seen)
            H.eq(seen.f[1], 1.0, "args[3] (xmm2)"); H.eq(seen.f[2], 2.0, "args[4] (xmm3)")
            H.eq(seen.f[3], 3.0, "args[5] (stack)"); H.eq(seen.f[4], 4.0, "args[6] (stack)")
            H.eq(v:read_float(0), 0.5, "x after changing args[3]"); H.eq(v:read_float(4), 2.0, "y")
            H.eq(v:read_float(8), 3.0, "z"); H.eq(v:read_float(12), 8.0, "w after changing args[6]")
        end,
    })

    hook_case("static: replace the Double return (xmm0)", "System.Math", "Sqrt(System.Double)", {
        args = function() return { 16.0 } end,
        pre = function(args, seen) seen.d = sdk.to_double(args[2]) end,
        post = function() return dbl(2.5) end,
        check = function(result, seen)
            H.eq(seen.d, 16.0, "args[2]")
            H.eq(result, 2.5, "replaced return value")
        end,
    })

    -- Skipping a struct-returning method leaves the caller's return buffer unwritten (invoke's buffer holds whatever
    -- was there), so only check that the original did not run.
    hook_case("static, struct return: SKIP_ORIGINAL skips the original", "via.vec3", "op_Subtraction(via.vec3, via.vec3)", {
        args = function() return { Vector3f.new(9, 9, 9), Vector3f.new(1, 2, 3) } end,
        pre = function(args, seen) seen.ret_buffer = i64(args[1]); return sdk.PreHookResult.SKIP_ORIGINAL end,
        post = function(retval, seen) return sdk.to_ptr(seen.ret_buffer) end,
        check = function(result)
            H.truthy(not (result.x == 8 and result.y == 7 and result.z == 6), "result is not the original's (8, 7, 6)")
        end,
    })
end

-------------------------------------------------------------------------------------------------------------------
-- Math types
-------------------------------------------------------------------------------------------------------------------
S:test("Vector2f / Vector3f / Vector4f", function()
    local a, b = Vector3f.new(1, 2, 3), Vector3f.new(4, 5, 6)
    local s = a + b
    H.eq(s.x, 5); H.eq(s.z, 9)
    local d = b - a
    H.eq(d.y, 3)
    H.eq(a:dot(b), 32, "dot")
    local c = Vector3f.new(1, 0, 0):cross(Vector3f.new(0, 1, 0))
    H.eq(c.z, 1, "cross")
    H.near(Vector3f.new(3, 4, 0):length(), 5, 1e-5, "length")
    H.near(Vector3f.new(0, 0, 9):normalized().z, 1, 1e-6, "normalized")
    local m = a * 2.0
    H.eq(m.y, 4, "vec * scalar")
    local l = Vector3f.new(0, 0, 0):lerp(Vector3f.new(10, 10, 10), 0.25)
    H.near(l.x, 2.5, 1e-5, "lerp")
    local v2 = Vector2f.new(3, 4)
    H.near(v2:length(), 5, 1e-5, "vec2 length")
    H.eq(v2:to_vec3().z, 0, "vec2 to_vec3")
    local v4 = Vector4f.new(1, 2, 3, 4)
    H.eq(v4:to_vec3().z, 3, "vec4 to_vec3")
    local cl = a:clone(); cl.x = 100
    H.eq(a.x, 1, "clone is a copy")
end)

S:test("Matrix4x4f", function()
    local id = Matrix4x4f.identity()
    H.eq(id[0].x, 1); H.eq(id[1].y, 1); H.eq(id[3].w, 1); H.eq(id[0].y, 0)
    local v = id * Vector4f.new(1, 2, 3, 1)
    H.eq(v.x, 1); H.eq(v.z, 3)
    local t = Matrix4x4f.new(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 5, 6, 7, 1)
    local moved = t * Vector4f.new(1, 1, 1, 1)
    H.eq(moved.x, 6, "translation x"); H.eq(moved.z, 8, "translation z")
    local inv = t:inverse() * t
    H.near(inv[3].x, 0, 1e-5, "inverse"); H.near(inv[0].x, 1, 1e-5, "inverse diagonal")
    local q = id:to_quat()
    H.near(q.w, 1, 1e-6, "to_quat")
end)

S:test("Quaternion", function()
    local id = Quaternion.identity()
    H.eq(id.w, 1); H.eq(id.x, 0)
    local v = id * Vector3f.new(1, 2, 3)
    H.near(v.y, 2, 1e-6, "identity rotate")
    local half = math.sqrt(0.5)
    local rz = Quaternion.new(half, 0, 0, half) -- 90 degrees about +Z (w, x, y, z)
    local r = rz * Vector3f.new(1, 0, 0)
    H.near(r.x, 0, 1e-5, "rot x"); H.near(r.y, 1, 1e-5, "rot y")
    H.near(rz:length(), 1, 1e-6, "length")
    H.near(rz:dot(rz), 1, 1e-6, "dot")
    local s = id:slerp(rz, 0.5)
    H.near(s:length(), 1, 1e-5, "slerp unit")
    local back = rz * rz:inverse()
    H.near(back.w, 1, 1e-5, "q * inverse")
    H.near(rz:to_mat4()[1].x, -1, 1e-5, "to_mat4")
end)

-------------------------------------------------------------------------------------------------------------------
-- imgui (frame tests run inside a window)
-------------------------------------------------------------------------------------------------------------------
S:frame_test("imgui widgets return documented values", function(frame)
    return H.in_window("rftest api: widgets", function(opened)
        H.eq(type(opened), "boolean", "begin_window")
        H.eq(imgui.button("rftest button"), false, "button not clicked")
        local changed, value = imgui.checkbox("rftest checkbox", true)
        H.eq(changed, false); H.eq(value, true)
        changed, value = imgui.slider_float("rftest slider", 0.25, 0.0, 1.0)
        H.eq(changed, false); H.near(value, 0.25, 1e-6, "slider value")
        local c2, v2 = imgui.slider_int("rftest slider_int", 3, 0, 10)
        H.eq(c2, false); H.eq(v2, 3)
        local c3, v3 = imgui.input_text("rftest input", "text")
        H.eq(c3, false); H.eq(v3, "text")
        local c4, v4 = imgui.combo("rftest combo", 2, { "a", "b", "c" })
        H.eq(c4, false); H.eq(v4, 2)
        local c5, v5 = imgui.drag_float3("rftest drag3", Vector3f.new(1, 2, 3), 0.1, 0.0, 10.0)
        H.eq(c5, false); H.eq(v5.z, 3)
        local sz = imgui.calc_text_size("rftest")
        H.truthy(sz.x > 0 and sz.y > 0, "calc_text_size")
        H.truthy(imgui.get_window_size().x > 0, "get_window_size")
        H.truthy(imgui.get_display_size().x > 0, "get_display_size")
        return true
    end)
end)

S:frame_test("imgui id stack, trees, tables, styles", function(frame)
    return H.in_window("rftest api: layout", function()
        imgui.push_id("rftest"); local a = imgui.get_id("x"); imgui.pop_id()
        H.ne(a, imgui.get_id("x"), "push_id changes ids")
        imgui.set_next_item_open(false)
        if imgui.tree_node("rftest tree") then imgui.tree_pop(); error("collapsed tree node reported open") end
        if imgui.begin_table("rftest api table", 2) then
            imgui.table_setup_column("one"); imgui.table_setup_column("two")
            imgui.table_headers_row()
            imgui.table_next_row(); imgui.table_next_column(); imgui.text("1"); imgui.table_next_column(); imgui.text("2")
            local cols = imgui.table_get_column_count()
            imgui.end_table()
            H.eq(cols, 2, "table_get_column_count")
        else
            error("begin_table returned false")
        end
        imgui.push_style_color(0, 0xFF00FF00); imgui.text("rftest colored"); imgui.pop_style_color(1)
        imgui.push_item_width(100); H.near(imgui.calc_item_width(), 100, 1, "push_item_width"); imgui.pop_item_width()
        local x0 = imgui.get_cursor_pos().x
        imgui.indent(12); H.near(imgui.get_cursor_pos().x - x0, 12, 0.5, "indent"); imgui.unindent(12)
        return true
    end)
end)

S:frame_test("imgui draw lists accept documented arguments", function(frame)
    return H.in_window("rftest api: draw list", function()
        local dl = H.truthy(imgui.get_window_draw_list(), "get_window_draw_list")
        local p = imgui.get_cursor_screen_pos()
        dl:add_line(p, Vector2f.new(p.x + 50, p.y), 0xFFFFFFFF, 1.0)
        dl:add_rect(p, Vector2f.new(p.x + 50, p.y + 20), 0xFFFFFFFF, 0, 0, 1.0)
        dl:add_circle(Vector2f.new(p.x + 80, p.y + 10), 10, 0xFFFFFFFF, 16, 1.0)
        dl:add_text(Vector2f.new(p.x + 100, p.y), 0xFFFFFFFF, "rftest")
        dl:path_clear(); dl:path_line_to(p); dl:path_line_to(Vector2f.new(p.x + 10, p.y + 10)); dl:path_stroke(0xFFFFFFFF, 0, 1.0)
        H.truthy(imgui.get_background_draw_list() ~= nil, "background draw list")
        return true
    end)
end)

-------------------------------------------------------------------------------------------------------------------
-- draw (screen/world helpers; valid during on_frame)
-------------------------------------------------------------------------------------------------------------------
S:frame_test("draw: 2D primitives", function(frame)
    draw.text("rftest", 10, 10, 0xFFFFFFFF)
    draw.filled_rect(10, 30, 20, 10, 0x8000FF00)
    draw.outline_rect(10, 30, 20, 10, 0xFFFFFFFF)
    draw.line(10, 50, 40, 50, 0xFFFFFFFF)
    draw.filled_circle(60, 40, 5, 0xFFFFFFFF, 12)
    return true
end)

S:frame_test("draw.world_to_screen", function(frame)
    local p = draw.world_to_screen(Vector3f.new(0, 0, 0))
    H.truthy(p == nil or type(p.x) == "number", "world_to_screen returns nil or a Vector2f")
    return true
end)

S:finish()
