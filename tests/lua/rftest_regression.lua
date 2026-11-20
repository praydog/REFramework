-- rftest regression suite: tests for the Lua bugs fixed in the 2026-11 bug hunt (some bugs have several).
-- Game-agnostic: uses only System.*, via.* types and REFramework APIs. The tests fail on a build without the
-- corresponding fix and pass with it, except two controls ("relative paths still work", "integer address to an out
-- System.Single / System.Double parameter") that pass on both. See tests/README.md.
--
-- Needs from the runner (tests/run/Run-RFTests.ps1):
--   reframework/data/rftest_loadlib.dll   any loadable DLL (the runner copies one); without it the package.loadlib
--                                         tests fail.
-- Side effect on unfixed builds: the root-relative write tests create \rftest_escape\ at the root of the game's
-- drive; the Windows runner removes it. Tests that would corrupt memory on an unfixed build are not run there.

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
    H.raises(function() fs.read("C:rftest_no_such_file.txt") end)
end)

S:test("fs.read rejects a root-relative path", function()
    H.raises(function() fs.read("\\Windows\\win.ini") end)
end)

S:test("io.open rejects a root-relative path", function()
    raises_rooted(function() io.open("\\rftest_escape\\io.txt", "w") end)
end)

-- dump_file returns false for a rejected path, and also when the write fails, so this one can pass on an unfixed
-- build whose drive root isn't writable; the load_file test below covers json on such systems.
S:test("json.dump_file refuses a root-relative path", function()
    H.eq(json.dump_file("\\rftest_escape\\json.json", { x = 1 }), false, "dump_file return")
end)

S:test("json.load_file rejects a root-relative path", function()
    H.raises(function() json.load_file("\\rftest_escape\\json.json") end)
end)

S:test("imgui.load_font rejects a root-relative path", function()
    H.raises(function() imgui.load_font("\\Windows\\Fonts\\arial.ttf", 16) end)
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
    local armed, target, observed = false, nil, nil

    if outer and inner then
        sdk.hook(outer.method, function(args)
            if not armed then return end
            armed = false
            local before = thread.get_hook_storage()
            target:call(inner.name)
            local after = thread.get_hook_storage()
            observed = { before = before, after = after }
        end, function(retval) return retval end)
        sdk.hook(inner.method, function(args) end, function(retval) return retval end)
    end

    S:frame_test("thread.get_hook_storage() after a nested hooked call", function(frame)
        if not (outer and inner) then H.skip("need two distinct hookable System.String methods") end
        if frame < 5 then return false end
        target = sdk.create_managed_string("RfTest")
        armed = true
        target:call(outer.name)
        armed = false
        if observed == nil then H.skip("hook on " .. outer.name .. " did not fire") end
        H.truthy(observed.before ~= nil, "storage before")
        H.truthy(observed.after ~= nil, "storage after the nested call")
        H.truthy(rawequal(observed.before, observed.after), "same storage table")
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
                if i == 30 and frame == 2 then act(horizontal and imgui.get_cursor_pos().x or imgui.get_cursor_pos().y) end
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

S:finish()
