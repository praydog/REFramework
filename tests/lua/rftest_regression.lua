-- rftest regression suite: tests for the Lua bugs fixed in the 2026-11 bug hunt (some bugs have several).
-- Game-agnostic: uses only System.*, via.* types and REFramework APIs. The tests fail on a build without the
-- corresponding fix and pass with it, except five controls ("relative paths still work", "integer address to an out
-- System.Single / System.Double parameter", "a wrapper that calls add_ref() again after release() releases that
-- reference when collected", "REField:get_data(nil) on a non-literal static System.String field reads the String",
-- "REManagedObject read_dword(0xE) on a String reads across the header's end") that pass on both. See
-- tests/README.md.
--
-- Needs from the runner (tests/run/Run-RFTests.ps1):
--   reframework/data/rftest_loadlib.dll   any loadable DLL (the runner copies one); without it the package.loadlib
--                                         tests fail.
--   reframework/plugins/rftest_plugin.dll the plugin suite's DLL: it registers rftest_peek (a guarded memory read);
--                                         without it the local page test fails.
-- Side effect on unfixed builds: the root-relative write tests create \rftest_escape\ at the root of the game's
-- drive; the Windows runner removes it. Tests that would corrupt memory on an unfixed build are not run there, except
-- "require inside a coroutine that is collected afterwards": its failure is the corruption, so it runs after every
-- other test.

local H = require("rftest.harness")
local S = H.suite("regression")

-------------------------------------------------------------------------------------------------------------------
-- Lua file APIs must reject rooted paths ("\foo" has a root directory but no drive, "C:foo" a drive
-- but no root directory; neither is absolute, but both replace the data directory when appended to it).
-------------------------------------------------------------------------------------------------------------------
-- The write tests check the error message: on an unfixed build a write can also fail because the drive root isn't
-- writable (e.g. Z: = / under Proton), which must not count as a rejection.
local function raises_rooted(fn)
    local err = tostring(H.raises(fn))
    H.truthy(err:find("absolute paths", 1, true), "rejected as a rooted path, got: " .. err)
end

S:test("fs.write rejects a root-relative path", function()
    raises_rooted(function() fs.write("\\rftest_escape\\fs.txt", "x") end)
end)

S:test("fs.read rejects a drive-relative path", function()
    raises_rooted(function() fs.read("C:rftest_no_such_file.txt") end)
end)

S:test("fs.read rejects a root-relative path", function()
    raises_rooted(function() fs.read("\\Windows\\win.ini") end)
end)

S:test("io.open rejects a root-relative path", function()
    raises_rooted(function() io.open("\\rftest_escape\\io.txt", "w") end)
end)

-- A root-relative path to a place that IS writable: this script's own data directory, with the drive dropped. An
-- unfixed build appends it, which replaces the data directory, and writes there (true); the fix refuses it (false).
-- A path at the drive root would not do: dump_file also returns false when the write fails.
local function rooted_writable_path(name)
    local source = debug.getinfo(1, "S").source:gsub("^@", "")
    local autorun = source:match("^(.*)[\\/][^\\/]+$")
    local data = autorun:gsub("[\\/]autorun$", "\\data")
    return (data:gsub("^%a:", "")) .. "\\rftest\\scratch\\" .. name
end

S:test("json.dump_file refuses a root-relative path", function()
    local path = rooted_writable_path("rooted.json")
    H.truthy(path:sub(1, 1) == "\\", "test setup: root-relative path " .. path)
    H.eq(json.dump_file(path, { x = 1 }), false, "dump_file('" .. path .. "')")
end)

S:test("json.load_file rejects a root-relative path", function()
    raises_rooted(function() json.load_file("\\rftest_escape\\json.json") end)
end)

S:test("imgui.load_font rejects a root-relative path", function()
    local err = tostring(H.raises(function() imgui.load_font("\\Windows\\Fonts\\arial.ttf", 16) end))
    H.truthy(err:find("must not be absolute", 1, true), "rejected as a rooted path, got: " .. err)
end)

S:test("relative paths still work (fs, io, json)", function()
    fs.write("rftest\\scratch\\fs.txt", "hello")
    H.eq(fs.read("rftest\\scratch\\fs.txt"), "hello", "fs round trip")
    local f = H.truthy(io.open("rftest\\scratch\\io.txt", "w"), "io.open")
    f:write("io"); f:close()
    H.eq(json.dump_file("rftest\\scratch\\j.json", { a = 1 }), true, "dump_file")
    H.eq(json.load_file("rftest\\scratch\\j.json").a, 1, "load_file")
end)

-- rftest_loadlib.dll is copied into reframework/data by the runner (tests/run/Run-RFTests.ps1).
S:test("package.loadlib loads a DLL from reframework/data", function()
    H.eq(package.loadlib("rftest_loadlib.dll", "*"), true, "loadlib('rftest_loadlib.dll', '*')")
end)

S:test("package.loadlib returns nil plus a message for a missing DLL", function()
    local r, msg = package.loadlib("rftest_no_such.dll", "*")
    H.eq(r, nil, "result")
    H.eq(type(msg), "string", "error message type")
end)

-- On master this fails because loadlib never finds a DLL in reframework/data at all. What it guards is the first
-- version of the data-dir fix, which returned loadlib's own arguments when called from a coroutine (results read
-- from the wrong stack); that version is the red run for this test.
S:test("package.loadlib inside a coroutine returns its own results", function()
    local r = coroutine.wrap(function() return package.loadlib("rftest_loadlib.dll", "*") end)()
    H.eq(r, true, "loadlib('rftest_loadlib.dll', '*') in coroutine.wrap")
    local missing, msg = coroutine.wrap(function() return package.loadlib("rftest_no_such.dll", "*") end)()
    H.eq(missing, nil, "missing DLL result in a coroutine")
    H.eq(type(msg), "string", "missing DLL message in a coroutine")
end)

S:test("DLL write check covers names Windows normalizes (trailing dot, trailing space, NUL)", function()
    H.raises(function() io.open("rftest\\scratch\\evil.dll.", "wb") end, "io.open('evil.dll.')")
    H.raises(function() io.open("rftest\\scratch\\evil.dll ", "wb") end, "io.open('evil.dll ')")
    H.raises(function() io.open("rftest\\scratch\\evil.exe. . ", "wb") end, "io.open('evil.exe. . ')")
    H.raises(function() fs.write("rftest\\scratch\\evil.dll\0.txt", "x") end, "fs.write('evil.dll\\0.txt')")
    H.no_raise(function() fs.write("rftest\\scratch\\dll.notes.txt", "x") end, "fs.write('dll.notes.txt')")
end)

-- "snbypass.dllx" passes the write check. Where the volume generates 8.3 names (the default on a system drive),
-- Windows also gives it the short name SNBYPA~1.DLL, which loadlib and require's C searcher would load as a DLL.
S:test("loadlib and require refuse a file reached by an 8.3 short name ending in .DLL", function()
    local f = H.truthy(io.open("rftest\\scratch\\snbypass.dllx", "wb"), "io.open('snbypass.dllx')")
    f:write("{}")
    f:close()

    -- json.load_file reads any name, so it finds the file under its short name only if the volume made one. It
    -- returns nil for a missing file; an error here is a failure, not an absent short name.
    local t = json.load_file("rftest\\scratch\\SNBYPA~1.DLL")
    if t == nil then H.skip("this volume has no 8.3 short names") end
    H.eq(type(t), "table", "the short name read back the JSON written under the long name")

    local err = tostring(H.raises(function() package.loadlib("rftest\\scratch\\SNBYPA~1.DLL", "*") end, "loadlib"))
    H.truthy(err:find("only DLLs", 1, true), "loadlib refused it as not a DLL, got: " .. err)

    local loaded, rerr = pcall(require, "reframework.data.rftest.scratch.SNBYPA~1")
    H.eq(loaded, false, "require succeeded")
    H.truthy(tostring(rerr):find("is not a DLL", 1, true), "require refused it as not a DLL, got: " .. tostring(rerr))
end)

-- require inside a coroutine. The require wrapper built the C searcher wrappers it puts into package.searchers (3 and
-- 4) from the calling thread's lua_State, and they outlive the call: the next require replaces package.searchers, and
-- when the old table is collected each wrapper releases its reference through that lua_State. Once the coroutine had
-- been collected that was freed memory: luaL_unref on a freed lua_State, an access violation in lua_rawgeti as soon as
-- the block had been reused (RE2 and DMC5, in the first frame of a probe doing the steps below 25 times per frame;
-- under cdb after 31 and 33 coroutines). Nothing a script can read shows the freed state (require returns the module
-- inside the coroutine on every build), so the failure is what the writes through it do. Depending on what reused the
-- block that is the access violation (RE2, DMC5, SF6, MH Rise with the regression and plugin suites installed) or,
-- with the api suite installed too, the C-call count of the thread running the test growing by 5 per iteration until
-- require raises "C stack overflow" (iteration 50 in the same four games); with every coroutine kept alive neither
-- happens. Each iteration creates the timing: require of an already loaded module (no file is searched) in a fresh
-- coroutine, a full collection that frees the coroutine, strings of the sizes of the freed blocks that reuse them,
-- require on the main thread (the old searchers table becomes garbage) and a full collection that finalizes its
-- wrappers. Runs last, after every other last test, recorded as FAIL before it starts. The module is put into
-- package.loaded for the test and taken out again.
local REQUIRE_CO_TEST = "require inside a coroutine that is collected afterwards"
do
    local MODULE, ITERATIONS, PER_FRAME = "rftest_require_in_coroutine", 300, 25
    local st = nil
    -- Long strings (never interned) of the sizes a freed thread leaves: its lua_State (~0xd0 bytes) and its first
    -- stack (45 TValues, 720 bytes). 0xFF bytes, so a lua_State read from a reused block holds garbage pointers.
    local function churn(i)
        local keep = {}
        for n = 0, 63 do keep[#keep + 1] = string.rep("\255", 150 + n) .. i end
        for n = 0, 15 do keep[#keep + 1] = string.rep("\255", 680 + n * 8) .. i end
        return keep
    end
    -- In a separate function, so no stack slot keeps the coroutine reachable.
    local function require_in_coroutine(weak)
        local co = coroutine.create(function() return require(MODULE) end)
        weak[co] = true
        return coroutine.resume(co)
    end
    local function iteration()
        st.i = st.i + 1
        local weak = setmetatable({}, { __mode = "k" })
        local ok, r = require_in_coroutine(weak)
        if not (ok and rawequal(r, st.module)) then st.wrong[#st.wrong + 1] = string.format("%d: %s %s", st.i, tostring(ok), tostring(r)) end
        collectgarbage("collect")
        if next(weak) == nil then st.collected = st.collected + 1 end
        local keep = churn(st.i)
        if not rawequal(require(MODULE), st.module) then st.wrong[#st.wrong + 1] = string.format("%d: main thread", st.i) end
        keep = nil
        collectgarbage("collect")
    end
    S:last_frame_test(REQUIRE_CO_TEST, function(frame)
        -- An unfixed build can crash here: wait until no other last test is still pending.
        for _, r in ipairs(S.results) do
            if r.name ~= REQUIRE_CO_TEST and tostring(r.detail):sub(1, 8) == "pending:" then return false end
        end
        if st == nil then
            st = { i = 0, collected = 0, wrong = {}, module = { name = MODULE }, before = package.loaded[MODULE], first = frame }
            package.loaded[MODULE] = st.module
        end
        local ok, err = pcall(function()
            for _ = 1, PER_FRAME do
                if st.i >= ITERATIONS then break end
                iteration()
            end
        end)
        if not ok or st.i >= ITERATIONS then package.loaded[MODULE] = st.before end
        if not ok then error(string.format("iteration %d of %d: %s", st.i, ITERATIONS, tostring(err)), 0) end
        if st.i < ITERATIONS then return false end
        H.eq(#st.wrong, 0, "require returned another value than the loaded module (" .. table.concat(st.wrong, ", ") .. ")")
        H.eq(st.collected, ITERATIONS, "coroutines collected by the full collection after their require")
        return string.format("%d coroutines, each collected right after its require, in %d frames", ITERATIONS, frame - st.first + 1)
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- ValueType field writes and bounds.
-------------------------------------------------------------------------------------------------------------------
S:test("set_field with a ValueType writes only the field (via.vec3.x)", function()
    local vec3 = H.truthy(sdk.find_type_definition("via.vec3"), "via.vec3")
    local single = sdk.find_type_definition("System.Single")
    local fx = H.truthy(vec3:get_field("x"), "via.vec3.x")
    local off = fx:get_offset_from_fieldptr()
    local size = vec3:get_size()
    local container = ValueType.new(vec3)
    for i = 0, size - 1 do container:write_byte(i, 0xAA) end
    local src = ValueType.new(single)
    for i = 0, single:get_valuetype_size() - 1 do src:write_byte(i, 0x11) end
    container:set_field("x", src)
    local clobbered = 0
    for i = 0, size - 1 do
        local b = container:read_byte(i)
        if i >= off and i < off + 4 then H.eq(b, 0x11, "field byte " .. i)
        elseif b ~= 0xAA then clobbered = clobbered + 1 end
    end
    H.eq(clobbered, 0, "bytes changed outside the field")
end)

S:test("set_field on a System.Char field stores the value", function()
    local td = sdk.find_type_definition("System.Char")
    local field
    for _, f in ipairs(td:get_fields()) do if not f:is_static() then field = f break end end
    if field == nil then H.skip("System.Char has no instance field in this game") end
    local vt = ValueType.new(td)
    local off = field:get_offset_from_fieldptr()
    vt:write_short(off, 0x41)
    vt:set_field(field:get_name(), 0x42)
    H.eq(vt:read_short(off), 0x42, "char after set_field")
end)

S:test("ValueType read_* rejects reads that run past the buffer", function()
    local td = sdk.find_type_definition("System.Int32")
    local vt = ValueType.new(td)
    local size = td:get_size()
    vt:write_dword(size - 4, 0x11223344)
    H.eq(vt:read_dword(size - 4), 0x11223344, "in-bounds dword")
    H.eq(vt:read_qword(size - 4), 0, "qword crossing the end")
    H.eq(vt:read_byte(size), 0, "byte at size")
end)

-- REManagedObject read_*/write_* must reject a value that starts inside the object and ends past it. A new System.Byte
-- array is the object: is_valid_offset bounds an array by its data offset + length (one dimension), so its last element
-- is its last valid byte. Unfixed, a read starting there returns that byte plus memory past the object; fixed, 0. The
-- reads run first (unfixed, read_short runs 1 byte past and fails the test), the out-of-range writes only once they
-- are rejected.
S:test("REManagedObject read_*/write_* reject values that run past the object", function()
    local N = 13
    local arr = H.truthy(sdk.create_managed_array("System.Byte", N), "create_managed_array")
    arr[0] = sdk.create_byte(0x5A)
    arr[N - 1] = sdk.create_byte(0xA5)
    -- Find the data offset with single-byte reads (never past the object, fixed or not): 0x5A, N - 2 zeros, 0xA5.
    local data
    for o = 0, 0x100 - N do
        if arr:read_byte(o) == 0x5A and arr:read_byte(o + N - 1) == 0xA5 then
            local zeros = true
            for i = 1, N - 2 do if arr:read_byte(o + i) ~= 0 then zeros = false break end end
            if zeros then data = o break end
        end
    end
    H.truthy(data, "array data found by read_byte")
    H.eq(arr:read_dword(data - 4), N, "element count right before the data")
    H.truthy(arr:read_dword(data - 8) <= 1, "one-dimensional array (the bound is data offset + length)")
    local last = data + N - 1
    -- In range on either build.
    H.eq(arr:read_byte(data), 0x5A, "byte at the first element")
    H.eq(arr:read_dword(data), 0x5A, "dword at the first element")
    H.eq(arr:read_byte(last), 0xA5, "byte at the last element")
    H.eq(arr:read_dword(last - 3), 0xA5000000, "last full dword")
    H.eq(arr:read_qword(last - 7), 0xA5 << 56, "last full qword")
    -- Crossing the end.
    H.eq(arr:read_short(last), 0, "short starting at the last byte")
    H.eq(arr:read_dword(last), 0, "dword starting at the last byte")
    H.eq(arr:read_qword(last), 0, "qword starting at the last byte")
    H.eq(arr:read_qword(last - 3), 0, "qword starting 4 bytes before the end")
    arr:write_dword(last, 0x11111111)
    arr:write_qword(last - 3, 0x2222222222222222)
    H.eq(arr:read_dword(last - 3), 0xA5000000, "last dword after the rejected writes")
    arr:write_dword(last - 3, 0x01020304)
    H.eq(arr:read_dword(last - 3), 0x01020304, "in-range dword write")
    arr:write_qword(last - 7, 0x0102030405060708)
    H.eq(arr:read_qword(last - 7), 0x0102030405060708, "in-range qword write")
end)

-- arr[i] = <ValueType> must store the value. DMC5's (TDB 67) array types declare only Set/Get, so the "set_Item" the
-- binding looks up first is the parent's System.Array.set_Item(Int32, Object). That one takes an object: the engine
-- read the value's bytes as one and faulted ("Invoke threw an exception"), and the element stayed 0. The other games'
-- array types declare their own set_Item(Int32, T), so this passes there on any build.
S:test("array[i] = ValueType stores the value (System.Guid)", function()
    local arr = H.truthy(sdk.create_managed_array("System.Guid", 3), "create_managed_array")
    local v = ValueType.new(sdk.find_type_definition("System.Guid"))
    v:write_qword(0, 0x1122334455667788)
    v:write_qword(8, 0x0102030405060708)
    arr[1] = v
    -- Reading an element gives a boxed copy: its data follows the 0x10-byte object header.
    local e = H.truthy(arr[1], "arr[1]")
    H.eq(e:read_qword(0x10), 0x1122334455667788, "element 1, bytes 0-7")
    H.eq(e:read_qword(0x18), 0x0102030405060708, "element 1, bytes 8-15")
end)

-- arr[i] = value must check the index and the kind of value. The checks sit on the array's Set. Arrays with a set_Item
-- of their own (every game but DMC5, DD2 and PRAGMATA) never got there: a string stored its managed string's pointer
-- bits in a value element, and a fractional index wrote the element below it. The number into a reference element
-- comes last: unchecked, it would leave a bogus pointer in the array, and an unfixed build fails before it.
S:test("array[i] = value checks the index and the kind of value", function()
    local arr = H.truthy(sdk.create_managed_array("System.Int32", 3), "create_managed_array")
    arr[1] = 0x11223344
    -- Reading an element gives a boxed copy: its data follows the 0x10-byte object header.
    H.eq(arr[1]:read_dword(0x10), 0x11223344, "element 1 after a number")

    local err = tostring(H.raises(function() arr[1] = "text" end, "a string into a System.Int32 element"))
    H.truthy(err:find("Cannot assign a string", 1, true), "rejected as a string, got: " .. err)
    H.eq(arr[1]:read_dword(0x10), 0x11223344, "element 1 after the rejected string")

    err = tostring(H.raises(function() arr[1.5] = 7 end, "index 1.5"))
    H.truthy(err:find("Array index must be an integer", 1, true), "rejected as a fractional index, got: " .. err)
    H.eq(arr[1]:read_dword(0x10), 0x11223344, "element 1 after the rejected index")

    local strings = H.truthy(sdk.create_managed_array("System.String", 2), "create_managed_array(System.String)")
    err = tostring(H.raises(function() strings[0] = 5 end, "a number into a System.String element"))
    H.truthy(err:find("Cannot assign a number", 1, true), "rejected as a number, got: " .. err)
end)

-- arr[0] = "text" on a two-dimensional System.String array. Its Set is Set(Int32, Int32, String), which arr[i] = v
-- can't call; up to TDB 71 the array inherits System.Array.set_Item(Int32, Object), and master stored the string
-- through it (in the element [0,0]). The Set checks above left such an array no setter at all: "Attempted to new_index
-- invalid REManagedObject field". The type database can't create a multi-dimensional array (its System.Array has only
-- CreateInstance(Type, Int32)), so the test clones one the game holds in a static field and writes only into the
-- clone. The fields below are the System.String[,] instances a search of every type's fields found (SF6, MH Rise; the
-- other games have none, or no such type). Where System.Array has no set_Item (TDB 73 and later), no build stores a
-- value through arr[i] = v in such an array, and the test skips.
local STRING_2D_FIELDS = {
    { "app.UIPartsContactPanelBase", "mArrPlayStateName" },        -- SF6
    { "snow.eventcut.UniqueEventManager", "OtomoClipNodeMatrix" }, -- MH Rise
}
S:frame_test("a string written into a two-dimensional string array is stored", function(frame)
    local array_t = H.truthy(sdk.find_type_definition("System.Array"), "System.Array")
    if array_t:get_method("set_Item") == nil then
        H.skip("System.Array.set_Item(Int32, Object) is not in this game: no build stores arr[i] = v in a multi-dimensional array")
    end
    local looked, field, src = {}, nil, nil
    for _, c in ipairs(STRING_2D_FIELDS) do
        looked[#looked + 1] = c[1] .. "." .. c[2]
        local t = sdk.find_type_definition(c[1])
        local f = t and t:get_field(c[2])
        if f ~= nil and f:is_static() then field = looked[#looked]; src = f:get_data(nil); break end
    end
    if field == nil then H.skip("no System.String[,] instance known in this game (looked for the static fields " .. table.concat(looked, ", ") .. ")") end
    -- The game fills the field itself; wait for it (the suite's frame budget bounds the wait).
    if src == nil then return false end
    H.eq(H.type_name(src), "System.String[,]", field)
    local clone_m = H.truthy(src:get_type_definition():get_method("Clone"), "System.String[,] Clone()")
    local clone = H.truthy(clone_m:call(src), field .. ":Clone()")
    H.ne(clone:get_address(), src:get_address(), "Clone() returned a new array")
    local original = src:call("Get", 0, 0)
    H.eq(clone:call("Get", 0, 0), original, "the clone's [0,0] before the write")
    H.ne(original, "rftest-2d", "the game's [0,0]")
    H.no_raise(function() clone[0] = "rftest-2d" end, "clone[0] = 'rftest-2d'")
    H.eq(clone:call("Get", 0, 0), "rftest-2d", "the clone's [0,0] after clone[0] = 'rftest-2d'")
    H.eq(src:call("Get", 0, 0), original, "the game's " .. field .. " [0,0] after the write into its clone")
    return string.format("%s (cloned, frame %d): [0,0] %s -> rftest-2d", field, frame, tostring(original))
end)

-- A String's real size is 0x14 + 2 * (length + 1): the 0x10-byte header, the length dword at 0x10, then the UTF-16
-- characters from 0x14 and a terminator. is_valid_offset bounded every object but an array by its type's size, and
-- the String type's size S is one fixed number per game: 0x10 (the header alone) in some games, 0x18 in others.
-- With S = 0x10 every read at 0x10 or later returned 0, the length included. With S = 0x18 reads past 0x18 returned
-- 0, while reads starting below 0x18 on a 1- or 2-character String (0x18 / 0x1A bytes) ran past its end. The first
-- version of the whole-value rule also rejected in-object reads that cross S. Strings are now bounded by their own
-- size. The layout is checked with get_object_size(), which reads the length at 0x10 itself, before anything relies
-- on it. Reads inside the object run first, the writes past its end only once the reads past it returned 0.
do
    local function new_string(text)
        local str = H.truthy(sdk.create_managed_string(text), "create_managed_string(\"" .. text .. "\")")
        H.eq(str:get_object_size(), 0x14 + 2 * (#text + 1), "get_object_size() of \"" .. text .. "\"")
        return str
    end

    -- Control: passes without the fix too. Guards the fix against its first version, which rejected this read where
    -- S = 0x10 (it lies inside S = 0x18 everywhere else, so only those games can show that).
    S:test("REManagedObject read_dword(0xE) on a String reads across the header's end (control)", function()
        local str = new_string("0123456789")
        H.eq(str:read_dword(0xE) >> 16, 10, "upper half of the dword at 0xE (the length's low half)")
    end)

    S:test("REManagedObject read_*/write_* on a String are bounded by the String's own size", function()
        local text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_"
        local n = #text
        local function ch(i) return text:byte(i + 1) end -- character i, 0-based
        local last = 0x14 + 2 * (n - 1) -- offset of the last character
        local str = new_string(text)
        -- Inside the object.
        H.eq(str:read_dword(0x10), n, "length at 0x10")
        H.eq(str:read_short(0x14), ch(0), "first character")
        H.eq(str:read_qword(0x10), n | (ch(0) << 32) | (ch(1) << 48), "qword at 0x10 (length, first two characters)")
        H.eq(str:read_dword(0x16), ch(1) | (ch(2) << 16), "dword at 0x16 (characters 1 and 2)")
        H.eq(str:read_short(last), ch(n - 1), "last character")
        H.eq(str:read_short(last + 2), 0, "terminator")
        H.eq(str:read_dword(last), ch(n - 1), "dword ending at the object's end")
        H.eq(str:read_qword(last - 4), ch(n - 3) | (ch(n - 2) << 16) | (ch(n - 1) << 32), "qword ending at the object's end")
        -- Past the end.
        H.eq(str:read_qword(last), 0, "qword starting at the last character")
        H.eq(str:read_dword(last + 2), 0, "dword starting at the terminator")
        str:write_qword(last, 0x0041004100410041)
        str:write_dword(last + 2, 0x00410041)
        H.eq(str:read_dword(last), ch(n - 1), "last character and terminator after the rejected writes")
        -- In range.
        str:write_short(0x14 + 2 * 10, string.byte("!"))
        H.eq(str:read_short(0x14 + 2 * 10), string.byte("!"), "in-range write to character 10")
        H.eq(str:call("ToString"), text:sub(1, 10) .. "!" .. text:sub(12), "the String's text after the write")
    end)

    -- 1- and 2-character Strings end at 0x18 / 0x1A, at or just past S = 0x18: there the reads past their end are
    -- what the type-size bound let through. With S = 0x10 the in-object reads are the ones that fail without the fix.
    S:test("REManagedObject read_*/write_* on a short String stop at the String's end", function()
        for _, text in ipairs({ "A", "BC" }) do
            local str = new_string(text)
            local size = 0x14 + 2 * (#text + 1)
            local last_char = text:byte(-1)
            local label = "\"" .. text .. "\": "
            -- Inside the object.
            H.eq(str:read_short(0x14), text:byte(1), label .. "first character")
            H.eq(str:read_dword(size - 4), last_char, label .. "dword ending at the object's end")
            -- Past the end.
            H.eq(str:read_qword(0x14), 0, label .. "qword at 0x14")
            H.eq(str:read_qword(size - 4), 0, label .. "qword starting at the last character")
            H.eq(str:read_dword(size - 2), 0, label .. "dword starting at the terminator")
            str:write_qword(size - 4, 0x0041004100410041)
            str:write_dword(size - 2, 0x00410041)
            H.eq(str:read_dword(size - 4), last_char, label .. "last character and terminator after the rejected writes")
        end
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- Static String fields read through REField:get_data(nil). A literal (const) field has no storage: its data is the
-- constant's text in the type database's string table, which the String read used as a pointer to a managed String,
-- so every such read raised "caught (...) exception". No System.* literal String field is in every game; of the two
-- below, each game has one or both (System.Exception.InnerExceptionPrefix is in 11 of the 20 games, the LinkedList
-- one in 13). Every one the game has is checked; the pass detail names them.
-------------------------------------------------------------------------------------------------------------------
S:frame_test("REField:get_data(nil) on a literal System.String field returns the constant's text", function()
    local candidates = {
        { "System.Exception", "InnerExceptionPrefix", " ---> " },
        { "System.Collections.Generic.LinkedList`1<!0>", "VersionName", "Version" },
    }
    local found = {}
    for _, c in ipairs(candidates) do
        local t = sdk.find_type_definition(c[1])
        local f = t and t:get_field(c[2])
        if f ~= nil then
            local name = c[1] .. "." .. c[2]
            H.eq(f:get_type():get_full_name(), "System.String", name .. " type")
            H.eq(f:is_literal(), true, name .. " is_literal()")
            H.eq(f:get_data(nil), c[3], name .. " get_data(nil)")
            found[#found + 1] = name
        end
    end
    H.truthy(#found > 0, "System.Exception.InnerExceptionPrefix or System.Collections.Generic.LinkedList`1<!0>.VersionName "
        .. "in this game")
    return "checked " .. table.concat(found, ", ")
end)

-- Control: passes without the fix too. A static String field that isn't a literal holds a managed String.
S:test("REField:get_data(nil) on a non-literal static System.String field reads the String (control)", function()
    local f = H.truthy(sdk.find_type_definition("System.String"):get_field("Empty"), "System.String.Empty")
    H.eq(f:is_static(), true, "is_static()")
    H.eq(f:is_literal(), false, "is_literal()")
    H.eq(f:get_data(nil), "", "get_data(nil)")
end)

-------------------------------------------------------------------------------------------------------------------
-- release() right after add_ref() on a new object. A new small object (an array here) lives in one of the engine's
-- per-thread pages: 16-byte slots, bump-allocated behind a 0x40-byte page header, with the slot number k in byte 0xE
-- of the object's header, so the page starts at object - 16k. A page the thread started since the engine's last
-- local cleanup is itself local (negative count). add_ref() on an object in it makes the page global with one
-- reference, the object's. Releasing the object's last reference destroys it and releases that page reference, so
-- the page was freed while the thread still allocated from it and listed it, with other local objects in it. Game
-- code never gets there: a last release inside the engine's update scopes waits until after the cleanup. A script's
-- release() runs outside them, so the engine freed the page at once: a probe doing add_ref() then release() on new
-- arrays crashed RE2 in 10 of 36 runs, frames later in unrelated engine code (0 of 10 with add_ref() alone).
-- The test add_ref()s a second array in the same page and keeps it until the end, so on an unfixed build the page
-- survives and the failure is recorded instead of crashing: right after release() the page must still hold both
-- references. From frame 3 on (REFramework hands its delayed release to the engine at the second frame boundary) the
-- test polls the released array's engine count until it reads 0: the release is delayed, not lost. It passes with
-- the frame that happened in the result's detail, so each game's latency shows in the results, and fails if it hasn't
-- happened within RELEASE_BOUND frames of release(). get_reference_count() must read 0 right after release(): with
-- the fix the reference REFramework gave up isn't counted, so a "release until the count is 0" loop ends. Headers are
-- read through the rftest plugin's guarded rftest_peek (the engine's own count, delays included), and the layout is
-- checked on the page itself before anything relies on it.
-- The engine delays a last release itself while any thread is inside one of its update scopes, keeping the object at
-- count 1 until it flushes its own list. RE9 does that at on_frame: there an unfixed build fails at the
-- get_reference_count() check (it reads the engine's 1, as it always did on RE9), not at the page check, because the
-- page is never freed early; with the fix the engine delays REFramework's release once more, so the released array
-- reaches 0 later than elsewhere. In such a game a page-check pass on an unfixed build proves nothing; the page check
-- is red where the release is immediate (RE2, SF6, MH Rise, Kunitsu-Gami, PRAGMATA).
-------------------------------------------------------------------------------------------------------------------
do
    -- Frames after release() within which the released array must be destroyed. Provisional, to be confirmed against
    -- the RE9 probe (the engine's own delay there is the longest seen).
    local RELEASE_BOUND = 600
    local state
    local kept = {} -- arrays kept for good when the layout isn't recognized: releasing them could be the bug itself
    local function s32(v) if v ~= nil and v >= 0x80000000 then return v - 0x100000000 end return v end
    local function count_of(addr) return s32(rftest_peek(addr + 0x8, 4)) end -- reference count: int32 at +0x8
    local function slot_of(addr) return rftest_peek(addr + 0xE, 1) end -- k: byte at +0xE, 0 when not in a page
    local function page_of(addr)
        local k = slot_of(addr)
        if k == nil or k == 0 then return nil, k end
        return addr - 16 * k, k
    end
    -- New System.Object[4] arrays until one is the first object (k = 4, right after the header) of a page that is
    -- still local (count < 0, and k = 0 for the page itself) and the next one lands in the same page. The page the
    -- thread uses after a cleanup is global (count >= 1), so a first slot alone doesn't qualify.
    local function new_page_pair(limit)
        local first
        for _ = 1, limit do
            local arr = sdk.create_managed_array("System.Object", 4)
            local addr = arr:get_address()
            local page, k = page_of(addr)
            if first ~= nil and page == first.page then return first, { arr = arr, addr = addr } end
            first = nil
            local page_count = page and count_of(page)
            if k == 4 and page_count ~= nil and page_count < 0 and slot_of(page) == 0 then
                first = { arr = arr, addr = addr, page = page }
            end
        end
    end
    S:frame_test("release() right after add_ref() on a new array keeps the engine's local page alive", function(frame)
        if frame == 1 then
            H.truthy(rftest_peek, "rftest_peek (registered by tests/plugin's rftest_plugin.dll)")
            local a, keep = new_page_pair(400)
            if a == nil then H.skip("no new System.Object[4] array started a local engine page in 400 allocations") end
            local page = a.page
            keep.arr:add_ref()
            -- The page is now global with one reference, keep's. Check the layout before relying on it: that count,
            -- keep's count 1, keep's bit in the page bitmap at +0x20, and the page's high-water mark at +0x12 (in
            -- 16-byte units after the header) covering keep's first slot.
            local k = slot_of(keep.addr)
            local bits, high = rftest_peek(page + 0x20 + (k >> 3), 1), rftest_peek(page + 0x12, 1)
            if count_of(page) ~= 1 or count_of(keep.addr) ~= 1 or bits == nil or (bits >> (k & 7)) & 1 ~= 1
                or high == nil or high < k - 3 then
                kept[#kept + 1] = keep.arr
                H.skip(string.format("engine page layout not recognized: page count %s, array count %s, "
                    .. "bitmap byte %s, high-water %s, slot %d", tostring(count_of(page)), tostring(count_of(keep.addr)),
                    tostring(bits), tostring(high), k))
            end
            a.arr:add_ref()
            local before = count_of(page)
            a.arr:release()
            state = { a = a, keep = keep, before = before, after = count_of(page),
                visible = a.arr:get_reference_count() }
            return false
        end
        if frame < 3 then return false end
        -- The page is still alive while keep holds it, so the released array's header is live memory.
        local a_count = count_of(state.a.addr)
        if a_count ~= 0 and frame - 1 < RELEASE_BOUND then return false end
        -- The engine's local cleanup ran in between (every frame on this thread), so releasing keep is safe on any
        -- build.
        state.keep.arr:release()
        H.eq(state.before, 2, "page count with both arrays add_ref()'d")
        H.eq(state.visible, 0, "get_reference_count() right after release()")
        H.eq(state.after, 2, "page count right after release() (1: only the kept array holds the page; without it "
            .. "the engine frees the page here, while the thread still allocates from it)")
        H.eq(a_count, 0, string.format("released array's engine count %d frames after release() (the release must "
            .. "still happen, within %d frames)", frame - 1, RELEASE_BOUND))
        return string.format("released array destroyed %d frames after release()", frame - 1)
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- Objects a script creates and keeps. The book (REManagedObject.md): "The only time you will need to manually call
-- self:add_ref() and self:release() is when a newly created object is returned by the engine, e.g. an array". Kept
-- the usual way, add_ref()'s result stored straight into a long-lived table, Lua finalized the wrapper while the table
-- still held it and REFramework's __gc released the array: sizes read 0, 1, 2 or garbage a few frames later (RE2, RE9,
-- Kunitsu-Gami; master too). The cached-wrapper push had cleared the wrapper's GC color bits, which skipped Lua's
-- write barrier. The checks go through GetLength/GetValue inside pcall and compare element addresses recorded at
-- creation. Runs last, recorded as FAIL before it starts: once Lua has released a held array, the game can crash.
-------------------------------------------------------------------------------------------------------------------
do
    -- The arrays go into `held` directly, the usual way (cache[key] = obj:add_ref()); per-array data lives in `info`.
    -- REFramework runs Lua's collector in generational mode by default. A full collection first makes everything that
    -- exists old (`held` included) and resets the growth baseline, so the two steps after storing are minor
    -- collections even after the suite's own allocations; a minor collection is what swept the wrapper. A wrapper on
    -- the array metatable's __gc counts finalizations of held arrays; it is removed before any assertion can fail. On a
    -- build whose wrappers have no __gc it is the only __gc there, and removing it leaves none.
    local held, info = {}, {}
    local finalized, mt, orig_gc, installed = 0, nil, nil, false
    local function restore_gc() if installed then mt.__gc = orig_gc installed = false end end
    -- In a separate function, so no stale stack slot keeps an array reachable during the collections.
    local function make_and_hold(frame)
        local arr = sdk.create_managed_array("System.String", 4)
        if mt == nil then
            mt = getmetatable(arr)
            orig_gc = mt.__gc
            mt.__gc = function(o)
                for _, h in ipairs(held) do if rawequal(o, h) then finalized = finalized + 1 end end
                if orig_gc ~= nil then return orig_gc(o) end
            end
            installed = true
        end
        for i = 0, 3 do arr[i] = sdk.create_managed_string("s" .. i) end
        held[#held + 1] = arr:add_ref()
        local elements = {}
        for i = 0, 3 do elements[i] = arr:get_element(i):get_address() end
        info[#info + 1] = { frame = frame, elements = elements }
    end
    S:last_frame_test("an array kept with add_ref() stays intact across frames", function(frame)
        if frame == 1 then
            collectgarbage("collect")
            for _ = 1, 8 do make_and_hold(frame) end
            collectgarbage("step")
            collectgarbage("step")
        end
        if finalized > 0 then restore_gc() end
        H.eq(finalized, 0, string.format("frame %d: arrays still held that Lua finalized", frame))
        for k, arr in ipairs(held) do
            local e = info[k]
            local ok, n = pcall(arr.get_size, arr)
            if not (ok and n == 4) then restore_gc() end
            H.truthy(ok and n == 4, string.format("frame %d: array %d has size %s", frame, k, tostring(n)))
            for i = 0, 3 do
                local okp, p = pcall(function() return arr:get_element(i):get_address() end)
                if not (okp and p == e.elements[i]) then restore_gc() end
                H.truthy(okp and p == e.elements[i], string.format("frame %d: array %d, element %d is %s, was %x",
                    frame, k, i, okp and string.format("%x", p) or tostring(p), e.elements[i]))
            end
        end
        if frame < 30 then return false end
        restore_gc()
        for _, arr in ipairs(held) do arr:release() end
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- add_ref() then release() on a new (local) object: release() has to drop REFramework's cached wrapper, like it does
-- for any other object. add_ref() left the local-object count at 0 instead of removing it, and release() keeps the
-- cached wrapper while that count exists. The next push of the address then returned the old wrapper without taking
-- a reference: for an object the engine still holds, and for a new object the engine allocates at that address once
-- the old one is freed (which may be a different type).
-- An extra add_ref_permanent() keeps the array alive across release(), so the same address can be pushed again (with
-- sdk.to_managed_object) without depending on address reuse. That push has to take an engine reference of its own.
-- The array is never freed: the old wrapper is still alive, and on a build whose wrappers release by address in
-- __gc, once the array were gone that __gc could release a reference owned by a later object at the same address. A
-- full collection first removes wrappers of freed objects that Lua no longer reaches, so the new array's push can't
-- hit one.
-------------------------------------------------------------------------------------------------------------------
S:test("an object pushed again after add_ref() and release() holds its own reference", function()
    collectgarbage("collect")
    local a = sdk.create_managed_array("System.Object", 1)
    a:add_ref()
    a:add_ref_permanent()
    a:release()
    local addr = a:get_address()
    local rc_before = a:get_reference_count()
    H.truthy(rc_before > 0, "test setup: reference count after add_ref_permanent() and release() is " .. rc_before)
    local b = H.truthy(sdk.to_managed_object(addr), "to_managed_object")
    H.eq(b:get_reference_count(), rc_before + 1, "reference count after pushing the object again")
    H.truthy(not rawequal(a, b), "pushing the object again returned the released wrapper")
end)

-------------------------------------------------------------------------------------------------------------------
-- A wrapper's __gc released one reference by address, also when the script had already given that wrapper's
-- reference up with release(). Once the object had been fetched again, the reference it released belonged to the
-- new wrapper, and the new wrapper's cache entry went with it: the script held a wrapper the engine could free under
-- it, and the next fetch of the address made yet another wrapper. Wrappers without __gc release nothing when they
-- are collected: REFramework releases an address's reference a few frames after the address has no cached wrapper
-- left, so a wrapper fetched again keeps it.
-- The wrappers are made in helpers, so no stack slot keeps them reachable. A build that keeps the wrappers pushed in
-- the last two frames strongly (f35d040a) lets them be collected only from frame 4 on, so the test collects then.
-- The `seen` tables (weak values) show when they have been collected; where wrappers have a __gc, a full collection
-- runs the finalizers of what it collects before it returns. add_ref_permanent() keeps each array alive whatever is
-- released; the arrays are never freed (see the test above).
-------------------------------------------------------------------------------------------------------------------
do
    local function collect_until_gone(seen)
        for _ = 1, 5 do
            if next(seen) == nil then return end
            collectgarbage("collect")
        end
        H.eq(next(seen), nil, "test setup: a wrapper the test dropped is still alive after 5 full collections")
    end

    local seen, st = setmetatable({}, { __mode = "v" }), {}
    local function create_and_release()
        local w1 = sdk.create_managed_array("System.Object", 1)
        w1:add_ref()
        w1:add_ref_permanent()
        w1:release()
        seen[1] = w1
        return w1:get_address(), w1:get_reference_count()
    end
    S:frame_test("a wrapper fetched after release() keeps its reference when the released wrapper is collected", function(frame)
        if frame == 1 then
            local addr, rc_released = create_and_release()
            st.addr = addr
            st.w2 = H.truthy(sdk.to_managed_object(addr), "to_managed_object")
            st.rc = st.w2:get_reference_count()
            H.eq(st.rc, rc_released + 1, "test setup: reference count after fetching the released object again")
            return false
        end
        if frame < 4 then return false end
        collect_until_gone(seen)
        H.eq(st.w2:get_reference_count(), st.rc, "reference count after the released wrapper was collected")
        H.truthy(rawequal(sdk.to_managed_object(st.addr), st.w2), "fetching the object again returns the live wrapper")
        return true
    end)

    -- Control for the fix: add_ref() after release() gives the wrapper a reference of its own again, and that
    -- reference has to be given back once the wrapper is gone (the fix must not skip it). add_ref() makes the wrapper
    -- it's called on the cached one again, so the object it returns is that wrapper (on a build whose add_ref()
    -- doesn't, a second wrapper with a reference of its own, released by its __gc). Once they are collected, the
    -- reference is released a few frames later, or taken over by a push of the address before that: either way a new
    -- fetch leaves the array with the permanent reference plus exactly one.
    local seen2, st2 = setmetatable({}, { __mode = "v" }), {}
    local function release_then_add_ref()
        local w = sdk.create_managed_array("System.Object", 1)
        w:add_ref()
        w:add_ref_permanent()
        w:release()
        local rc_released = w:get_reference_count()
        seen2[1] = w
        seen2[2] = w:add_ref()
        return w:get_address(), rc_released
    end
    S:frame_test("a wrapper that calls add_ref() again after release() releases that reference when collected", function(frame)
        if frame == 1 then
            st2.addr, st2.rc_released = release_then_add_ref()
            return false
        end
        if frame < 4 then return false end
        collect_until_gone(seen2)
        local w3 = H.truthy(sdk.to_managed_object(st2.addr), "to_managed_object")
        H.eq(w3:get_reference_count(), st2.rc_released + 1, "reference count of a new fetch after both wrappers were collected")
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- Numbers are encoded by the callee's parameter type.
-------------------------------------------------------------------------------------------------------------------
S:test("integer argument to a System.Double parameter", function()
    local sqrt = H.truthy(sdk.find_type_definition("System.Math"):get_method("Sqrt(System.Double)"), "Math.Sqrt")
    H.eq(sqrt:call(nil, 4), 2.0, "Sqrt(4)")
    H.eq(sqrt:call(nil, 4.0), 2.0, "Sqrt(4.0)")
end)

S:test("float argument to a System.Int32 parameter", function()
    local s = sdk.create_managed_string("ABCDEF")
    H.eq(s:call("Substring(System.Int32)", 4 / 2), "CDEF", "Substring(4/2)")
    H.eq(s:call("Substring(System.Int32)", 2), "CDEF", "Substring(2)")
end)

S:test("integer address to an out System.Single / System.Double parameter", function()
    local tested = 0
    for _, tn in ipairs({ "System.Single", "System.Double" }) do
        local t = sdk.find_type_definition(tn)
        local try_parse
        for _, m in ipairs(t:get_methods()) do
            if m:get_name() == "TryParse" and m:get_num_params() == 2 then try_parse = m break end
        end
        if try_parse ~= nil then
            local out = ValueType.new(t)
            local read = tn == "System.Single" and out.read_float or out.read_double
            local write = tn == "System.Single" and out.write_float or out.write_double
            write(out, 0, -1.0)
            H.eq(try_parse:call(nil, sdk.create_managed_string("1.5"), out:get_address()), true, tn .. ".TryParse result")
            H.eq(read(out, 0), 1.5, tn .. ": value written through the address")
            tested = tested + 1
        end
    end
    if tested == 0 then H.skip("neither System.Single nor System.Double has TryParse(String, out T) in this game") end
end)

-------------------------------------------------------------------------------------------------------------------
-- Math type operators and indexing.
-------------------------------------------------------------------------------------------------------------------
S:test("scalar * vector", function()
    local v = 2.0 * Vector3f.new(1, 2, 3)
    H.eq(v.x, 2); H.eq(v.y, 4); H.eq(v.z, 6)
    local w = 0.5 * Vector4f.new(2, 4, 6, 8)
    H.eq(w.w, 4)
end)

S:test("vector * vector raises instead of returning zeros", function()
    H.raises(function() return Vector3f.new(1, 2, 3) * Vector3f.new(2, 2, 2) end)
end)

S:test("Matrix4x4f / Quaternion negative index reads return nil", function()
    local m = Matrix4x4f.identity()
    local q = Quaternion.identity()
    H.eq(m[-1], nil, "m[-1]"); H.eq(m[4], nil, "m[4]")
    H.eq(q[-1], nil, "q[-1]"); H.eq(q[4], nil, "q[4]")
end)

S:test("Matrix4x4f / Quaternion out-of-range writes raise", function()
    local m = Matrix4x4f.identity()
    local q = Quaternion.identity()
    -- Without the fix these writes land outside the userdata and corrupt the Lua heap; detect that build by its
    -- read behavior (same commit) and fail without writing.
    if m[-1] ~= nil or q[-1] ~= nil then error("unfixed build (negative index reads succeed); writes not attempted") end
    H.raises(function() m[4] = Vector4f.new(1, 1, 1, 1) end, "m[4] =")
    H.raises(function() m[-1] = Vector4f.new(1, 1, 1, 1) end, "m[-1] =")
    H.raises(function() q[4] = 1.0 end, "q[4] =")
    m[1] = Vector4f.new(5, 6, 7, 8)
    H.eq(m[1].w, 8, "in-range write")
end)

-------------------------------------------------------------------------------------------------------------------
-- thread.get_hook_storage() survives a nested hooked call. Hooks install at the end of the frame
-- in which they are added, so the check runs from a frame test.
-------------------------------------------------------------------------------------------------------------------
do
    local str_t = sdk.find_type_definition("System.String")
    local picked, seen = {}, {}
    for _, name in ipairs({ "ToUpper()", "ToLower()", "Trim()", "ToUpperInvariant()", "ToLowerInvariant()", "TrimStart()", "TrimEnd()" }) do
        local m = str_t:get_method(name)
        if m ~= nil and not m:is_static() and m:get_num_params() == 0 then
            local addr = sdk.to_int64(m:get_function())
            if addr ~= 0 and not seen[addr] then seen[addr] = true; picked[#picked + 1] = { name = name, method = m } end
        end
    end
    local outer, inner = picked[1], picked[2]
    local armed, nested, inner_fired, target, observed = false, false, false, nil, nil

    if outer and inner then
        sdk.hook(outer.method, function(args)
            if not armed then return end
            armed = false
            local before = thread.get_hook_storage()
            nested = true
            target:call(inner.name)
            nested = false
            local after = thread.get_hook_storage()
            observed = { before = before, after = after }
        end, function(retval) return retval end)
        -- The nested call must really go through a hook: that is what pops a storage table in the middle.
        sdk.hook(inner.method, function(args) if nested then inner_fired = true end end, function(retval) return retval end)
    end

    S:frame_test("thread.get_hook_storage() after a nested hooked call", function(frame)
        if not (outer and inner) then H.skip("need two distinct hookable System.String methods") end
        if frame < 5 then return false end
        target = sdk.create_managed_string("RfTest")
        armed = true
        target:call(outer.name)
        armed = false
        H.truthy(observed ~= nil, "hook on " .. outer.name .. " fired for the test's own call")
        H.truthy(inner_fired, "hook on " .. inner.name .. " fired for the nested call")
        H.truthy(observed.before ~= nil, "storage before")
        H.truthy(observed.after ~= nil, "storage after the nested call")
        H.truthy(rawequal(observed.before, observed.after), "same storage table")
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- A call made from a hook callback must not change the arguments of the call it interrupts. Vector2f / Vector3f
-- arguments are passed as addresses into a buffer REFramework keeps for the call. The nested call reused that buffer,
-- so the outer method computed with the nested call's vectors: (1, 2, 3) + (4, 5, 6) gave (110, 220, 330).
-------------------------------------------------------------------------------------------------------------------
do
    local vec3_t = sdk.find_type_definition("via.vec3")
    local add = vec3_t and vec3_t:get_method("op_Addition(via.vec3, via.vec3)")
    local sub = vec3_t and vec3_t:get_method("op_Subtraction(via.vec3, via.vec3)")
    local armed, armed_thread, nested = false, nil, nil
    local function fmt(v) return string.format("(%g, %g, %g)", v.x, v.y, v.z) end

    if add and sub then
        sdk.hook(add, function(args)
            -- The game adds vectors too, on any thread: only the test's own call counts.
            if not armed or thread.get_id() ~= armed_thread then return end
            armed = false
            nested = sub:call(nil, Vector3f.new(100, 200, 300), Vector3f.new(10, 20, 30))
        end, function(retval) return retval end)
    end

    S:frame_test("a vector call made from a hook leaves the outer call's vector arguments alone", function(frame)
        if not (add and sub) then H.skip("need via.vec3.op_Addition and op_Subtraction") end
        if frame < 5 then return false end
        armed, armed_thread = true, thread.get_id()
        local sum = add:call(nil, Vector3f.new(1, 2, 3), Vector3f.new(4, 5, 6))
        armed = false
        H.truthy(nested ~= nil, "hook on op_Addition fired for the test's own call")
        H.eq(fmt(nested), "(90, 180, 270)", "nested (100, 200, 300) - (10, 20, 30)")
        H.eq(fmt(sum), "(5, 7, 9)", "outer (1, 2, 3) + (4, 5, 6)")
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- imgui bindings. Each check runs inside a window in a frame test.
-------------------------------------------------------------------------------------------------------------------
local function width_of(draw)
    -- Width of the item drawn by draw(): cursor x after same_line() minus the start x and the item spacing.
    local x0 = imgui.get_cursor_pos().x
    draw()
    imgui.same_line(); local x1 = imgui.get_cursor_pos().x
    imgui.new_line()
    return x1 - x0
end

S:frame_test("imgui.text_colored shows '%' sequences literally", function(frame)
    if frame < 3 then return false end
    return H.in_window("rftest regression: text_colored", function()
        local label = "rftest 50%% off 100%d"
        local plain = width_of(function() imgui.text(label) end)
        local colored = width_of(function() imgui.text_colored(label, 0xFFFFFFFF) end)
        H.truthy(plain > 0, "plain text has a width (" .. tostring(plain) .. ")")
        H.near(colored, plain, 0.5, "text_colored width vs text width")
        return true
    end)
end)

S:frame_test("imgui.get_id(int) hashes with the ID stack", function(frame)
    return H.in_window("rftest regression: get_id", function()
        imgui.push_id("A"); local a = imgui.get_id(5); imgui.pop_id()
        imgui.push_id("B"); local b = imgui.get_id(5); imgui.pop_id()
        H.ne(a, b, "get_id(5) under A vs B")
        H.ne(a, 5, "get_id(5) returned the raw int")
        return true
    end)
end)

S:frame_test("imgui.indent takes fractional widths", function(frame)
    return H.in_window("rftest regression: indent", function()
        local x0 = imgui.get_cursor_screen_pos().x
        imgui.indent(0.4)
        local dx = imgui.get_cursor_screen_pos().x - x0
        imgui.unindent(0.4)
        H.near(dx, 0.4, 0.05, "indent(0.4)")
        return true
    end)
end)

S:frame_test("imgui.table_get_column_name() means the current column", function(frame)
    return H.in_window("rftest regression: table", function()
        H.truthy(imgui.begin_table("rftest_t", 3), "begin_table")
        local ok, name = pcall(function()
            imgui.table_setup_column("A"); imgui.table_setup_column("B"); imgui.table_setup_column("C")
            imgui.table_next_row(); imgui.table_set_column_index(2)
            return imgui.table_get_column_name()
        end)
        imgui.end_table()
        H.truthy(ok, name)
        H.eq(name, "C", "column name")
        return true
    end)
end)

do
    local scroll = {}
    local function child(id, horizontal, act, frame)
        local pos
        if imgui.begin_child_window(id, Vector2f.new(300, horizontal and 60 or 120), true, horizontal and 2048 or 0) then
            for i = 1, 60 do
                if horizontal and i > 1 then imgui.same_line() end
                imgui.text(string.format("[%02d]", i))
                if i == 30 and frame == 2 then
                    -- After text() the cursor is already at the start of the next line; for x, stay on this line so
                    -- the position is after item 30, not the window's left edge (that made both x cases scroll 0).
                    if horizontal then imgui.same_line() end
                    act(horizontal and imgui.get_cursor_pos().x or imgui.get_cursor_pos().y)
                end
            end
            pos = horizontal and imgui.get_scroll_x() or imgui.get_scroll_y()
            imgui.end_child_window()
        end
        return pos
    end
    local cases = {
        { "set_scroll_here_y", false, function() imgui.set_scroll_here_y() end, function() imgui.set_scroll_here_y(0.5) end },
        { "set_scroll_from_pos_y", false, function(p) imgui.set_scroll_from_pos_y(p) end, function(p) imgui.set_scroll_from_pos_y(p, 0.5) end },
        { "set_scroll_here_x", true, function() imgui.set_scroll_here_x() end, function() imgui.set_scroll_here_x(0.5) end },
        { "set_scroll_from_pos_x", true, function(p) imgui.set_scroll_from_pos_x(p) end, function(p) imgui.set_scroll_from_pos_x(p, 0.5) end },
    }
    S:frame_test("imgui scroll setters default to centering (0.5)", function(frame)
        return H.in_window("rftest regression: scroll", function()
            for _, c in ipairs(cases) do
                local a = child(c[1] .. " default", c[2], c[3], frame)
                imgui.same_line()
                scroll[c[1]] = { a, child(c[1] .. " 0.5", c[2], c[4], frame) }
            end
            if frame < 6 then return false end
            for _, c in ipairs(cases) do
                local r = scroll[c[1]]
                H.truthy(r[1] ~= nil and r[2] ~= nil, c[1] .. ": both child windows visible")
                H.truthy(r[2] > 0, c[1] .. "(..., 0.5) scrolled (" .. tostring(r[2]) .. ")")
                H.near(r[1], r[2], 1.0, c[1] .. "() vs " .. c[1] .. "(..., 0.5)")
            end
            return true
        end, 680, 500)
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- Hooking a method that returns a struct and takes a by-value float. Its native arguments are
-- (return buffer, thread context, &vec3, float in xmm3); the hook saved and restored registers one slot early, so
-- r8 (&vec3) and xmm3 were clobbered and the game hung as soon as the hook was installed. args keep the native
-- order: args[1] return buffer, args[2] thread context, args[3] &vec3, args[4] the float.
-------------------------------------------------------------------------------------------------------------------
do
    local m = sdk.find_type_definition("via.vec3"):get_method("op_Multiply(via.vec3, System.Single)")
    local installed_frame, game_calls, ours, armed_tid = nil, 0, nil, nil

    S:last_frame_test("hooking a struct-returning method with a float parameter", function(frame)
        if m == nil then H.skip("via.vec3.op_Multiply(via.vec3, System.Single) is not in this game") end

        if installed_frame == nil then
            installed_frame = frame
            sdk.hook(m, function(args)
                if thread.get_id() == armed_tid then
                    ours = { v = sdk.to_valuetype(args[3], "via.vec3"):read_float(4), f = sdk.to_float(args[4]) }
                else
                    game_calls = game_calls + 1
                end
            end, function(retval) return retval end)
            return false
        end

        -- Let the game run through the hook for a while; on a broken build it never gets here.
        if frame - installed_frame < 120 then return false end

        armed_tid = thread.get_id()
        local product = m:call(nil, Vector3f.new(1, 2, 3), 2.0)
        armed_tid = nil
        H.truthy(ours ~= nil, "the hook fired for our call")
        H.eq(ours.v, 2.0, "args[3] points at the vector (y)")
        H.eq(ours.f, 2.0, "args[4] is the float")
        H.eq(product.x, 2.0); H.eq(product.y, 4.0); H.eq(product.z, 6.0)
        log.info(string.format("[rftest] regression: op_Multiply hook saw %d game calls", game_calls))
        return true
    end)
end

-------------------------------------------------------------------------------------------------------------------
-- draw.sphere / draw.capsule when the camera's up vector can't be found. get_camera_up() looks up the camera's
-- GameObject, its transform, the transform's joints and the first joint, and has to give up (the shape isn't drawn)
-- when one of them is null. It wrote `std::nullopt;` without `return`, so it went on with the null.
-- REFramework calls via.Transform.get_GameObject and get_Joints through their function pointers, so the test's hooks
-- replace REFramework's own calls while the test draws: get_Joints returns null or an empty via.Joint[] (whose
-- element 0 is null), get_GameObject returns null or a GameObject whose transform field is null. The step an
-- unfixed build takes next with the null is a hooked method too (get_Joints on the transform, System.Array.GetLength
-- on the array, via.Joint.get_Rotation on the joint): its hook counts the call and skips the original, so the
-- unfixed build fails on that count instead of running engine code on a null object.
-- A null GameObject is different: REFramework reads the transform field itself, so no hook can stop it, and an
-- unfixed build takes an access violation there (REFramework is built with /EHa, so sol is expected to turn it into
-- a Lua error). That case runs after every other last test has finished.
-- Both are last tests: game code calls the hooked methods too, and every call runs the Lua hooks for the rest of the
-- session. Their pass details count the calls the hooks passed through since they were installed.
-------------------------------------------------------------------------------------------------------------------
do
    local transform_t = sdk.find_type_definition("via.Transform")
    local get_go = transform_t:get_method("get_GameObject")
    local get_joints = transform_t:get_method("get_Joints")
    local get_length = sdk.find_type_definition("System.Array"):get_method("GetLength")
    local get_rotation = sdk.find_type_definition("via.Joint"):get_method("get_Rotation")
    local NULL_GO, NO_TRANSFORM = "null GameObject", "GameObject without a transform"
    local NULL_JOINTS, NO_JOINT = "null joints array", "empty joints array"
    local armed_tid, mode, run, installed_frame = nil, nil, nil, nil
    local passed = { get_GameObject = 0, get_Joints = 0, GetLength = 0, get_Rotation = 0 }

    local function ours() return armed_tid ~= nil and thread.get_id() == armed_tid end
    local function is_null(p) return sdk.to_int64(p) == 0 end

    local function install_hooks()
        -- Only the first get_GameObject call is replaced: an unfixed build that went on draws with the real camera.
        sdk.hook(get_go, function(args)
            if ours() and (mode == NULL_GO or mode == NO_TRANSFORM) and not run.go_replaced then
                run.go_replaced, run.go_pending = true, true
                return sdk.PreHookResult.SKIP_ORIGINAL
            end
            passed.get_GameObject = passed.get_GameObject + 1
        end, function(retval)
            if not (ours() and run.go_pending) then return retval end
            run.go_pending = false
            return sdk.to_ptr(mode == NULL_GO and 0 or run.fake_go)
        end)
        sdk.hook(get_joints, function(args)
            if ours() and is_null(args[2]) then
                run.joints_on_null, run.joints_pending = run.joints_on_null + 1, true
                return sdk.PreHookResult.SKIP_ORIGINAL
            end
            if ours() and (mode == NULL_JOINTS or mode == NO_JOINT) then
                run.joints_replaced, run.joints_pending = run.joints_replaced + 1, true
                return sdk.PreHookResult.SKIP_ORIGINAL
            end
            passed.get_Joints = passed.get_Joints + 1
        end, function(retval)
            if not (ours() and run.joints_pending) then return retval end
            run.joints_pending = false
            return sdk.to_ptr(mode == NO_JOINT and run.empty:get_address() or 0)
        end)
        sdk.hook(get_length, function(args)
            if ours() and is_null(args[2]) then
                run.length_on_null, run.length_pending = run.length_on_null + 1, true
                return sdk.PreHookResult.SKIP_ORIGINAL
            end
            passed.GetLength = passed.GetLength + 1
        end, function(retval)
            if not (ours() and run.length_pending) then return retval end
            run.length_pending = false
            return sdk.to_ptr(0)
        end)
        -- A struct return: args[1] is the return buffer, args[3] the joint.
        sdk.hook(get_rotation, function(args)
            if ours() and is_null(args[3]) then
                run.rotation_on_null, run.rotation_buffer = run.rotation_on_null + 1, sdk.to_int64(args[1])
                return sdk.PreHookResult.SKIP_ORIGINAL
            end
            passed.get_Rotation = passed.get_Rotation + 1
        end, function(retval)
            if not (ours() and run.rotation_buffer ~= nil) then return retval end
            local buffer = run.rotation_buffer
            run.rotation_buffer = nil
            return sdk.to_ptr(buffer)
        end)
    end

    -- Color 0 (transparent) and no outline: an unfixed build that goes on draws nothing visible.
    local draws = {
        { "draw.sphere", function() draw.sphere(Vector3f.new(0, 0, 0), 1.0, 0, false) end },
        { "draw.capsule", function() draw.capsule(Vector3f.new(0, 0, 0), Vector3f.new(0, 1, 0), 1.0, 0, false) end },
    }

    local function run_case(case_mode, setup)
        for _, d in ipairs(draws) do
            local label = d[1] .. ", " .. case_mode
            run = { joints_replaced = 0, joints_on_null = 0, length_on_null = 0, rotation_on_null = 0 }
            setup(run)
            mode, armed_tid = case_mode, thread.get_id()
            local ok, err = pcall(d[2])
            mode, armed_tid = nil, nil
            H.truthy(ok, label .. ": raised " .. tostring(err))
            if case_mode == NULL_GO or case_mode == NO_TRANSFORM then
                H.truthy(run.go_replaced, label .. ": the get_GameObject hook replaced REFramework's call")
            else
                H.truthy(run.joints_replaced > 0, label .. ": the get_Joints hook replaced REFramework's call")
            end
            H.eq(run.joints_on_null, 0, label .. ": via.Transform.get_Joints calls on the null transform")
            H.eq(run.length_on_null, 0, label .. ": System.Array.GetLength calls on the null joints array")
            H.eq(run.rotation_on_null, 0, label .. ": via.Joint.get_Rotation calls on the null joint")
        end
    end

    local function need_camera()
        H.truthy(sdk.get_primary_camera(), "sdk.get_primary_camera() (without a camera get_camera_up returns before "
            .. "the calls this test replaces)")
    end

    local function passed_through(frame)
        return string.format("calls the hooks passed through in the %d frames since they were installed: "
            .. "get_GameObject %d, get_Joints %d, System.Array.GetLength %d, via.Joint.get_Rotation %d",
            frame, passed.get_GameObject, passed.get_Joints, passed.GetLength, passed.get_Rotation)
    end

    S:last_frame_test("draw.sphere / draw.capsule draw nothing when the camera's transform or joints are missing", function(frame)
        if frame == 1 then
            H.truthy(get_go, "via.Transform.get_GameObject (get_camera_up calls it)")
            H.truthy(get_joints, "via.Transform.get_Joints (get_camera_up calls it)")
            H.truthy(get_length, "System.Array.GetLength (get_camera_up calls it on the joints array)")
            H.truthy(get_rotation, "via.Joint.get_Rotation (get_camera_up calls it on the first joint)")
            install_hooks()
            installed_frame = S.frame
            return false
        end
        if frame < 3 then return false end
        need_camera()

        run_case(NULL_JOINTS, function() end)
        run_case(NO_JOINT, function(r)
            r.empty = H.truthy(sdk.create_managed_array("via.Joint", 0), "create_managed_array(via.Joint, 0)")
        end)
        -- The fake GameObject starts 0x40 bytes into a new System.Byte[0x100], inside its zeroed data (the array
        -- header is 0x20 or 0x30 bytes), so the transform field REFramework reads (at 0x18 or 0x20) is 0.
        run_case(NO_TRANSFORM, function(r)
            r.fake_array = H.truthy(sdk.create_managed_array("System.Byte", 0x100), "create_managed_array(System.Byte, 0x100)")
            for off = 0x40, 0x78, 8 do H.eq(r.fake_array:read_qword(off), 0, string.format("fake GameObject bytes at 0x%X", off)) end
            r.fake_go = r.fake_array:get_address() + 0x40
        end)

        return passed_through(S.frame - installed_frame)
    end)

    local NULL_GO_TEST = "draw.sphere / draw.capsule draw nothing when the camera's GameObject is null"
    S:last_frame_test(NULL_GO_TEST, function(frame)
        -- An unfixed build takes an access violation here: wait until no other last test is still pending (but the
        -- require-in-a-coroutine test, which waits for this one).
        for _, r in ipairs(S.results) do
            if r.name ~= NULL_GO_TEST and r.name ~= REQUIRE_CO_TEST and tostring(r.detail):sub(1, 8) == "pending:" then return false end
        end
        H.truthy(installed_frame, "hooks installed by the transform/joints test")
        need_camera()
        run_case(NULL_GO, function() end)
        return passed_through(S.frame - installed_frame)
    end)
end

S:finish()
