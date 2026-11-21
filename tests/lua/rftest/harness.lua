-- rftest: minimal test harness for REFramework's Lua API, run inside a game.
--
-- A suite is a top-level autorun script:
--
--   local H = require("rftest.harness")
--   local S = H.suite("api")
--   S:test("strings round-trip", function() H.eq(1 + 1, 2) end)
--   S:frame_test("imgui window", function(frame) ... return true when done end)
--   S:finish()
--
-- Plain tests run while the script loads. Frame tests run inside re.on_frame (where imgui and drawing are valid)
-- until they return true. When every frame test is done (or after max_frames), the suite writes
--   reframework/data/rftest/results/<suite>.json   -- machine-readable, read by tests/run/Run-RFTests.ps1
-- and logs one "[rftest] <suite> ..." line per test plus a summary to re2_framework_log.txt.

local H = {}

local RESULTS_DIR = "rftest\\results\\"

-- Assertions raise; a test passes when its function returns without raising.
local SKIP = {}

function H.skip(reason)
    error(setmetatable({ reason = reason }, SKIP), 0)
end

local function fail(msg, level)
    error(msg, (level or 1) + 2)
end

function H.truthy(v, msg)
    if not v then fail(string.format("%s: expected a truthy value, got %s", msg or "truthy", tostring(v))) end
    return v
end

function H.eq(actual, expected, msg)
    if actual ~= expected then
        fail(string.format("%s: expected %s (%s), got %s (%s)", msg or "eq", tostring(expected), type(expected), tostring(actual), type(actual)))
    end
end

function H.ne(actual, unexpected, msg)
    if actual == unexpected then fail(string.format("%s: did not expect %s", msg or "ne", tostring(actual))) end
end

function H.near(actual, expected, eps, msg)
    eps = eps or 1e-4
    if type(actual) ~= "number" or math.abs(actual - expected) > eps then
        fail(string.format("%s: expected %s +- %s, got %s", msg or "near", tostring(expected), tostring(eps), tostring(actual)))
    end
end

function H.raises(fn, msg)
    local ok, err = pcall(fn)
    if ok then fail(string.format("%s: expected an error, call succeeded", msg or "raises")) end
    return err
end

function H.no_raise(fn, msg)
    local ok, err = pcall(fn)
    if not ok then fail(string.format("%s: unexpected error: %s", msg or "no_raise", tostring(err))) end
    return err
end

function H.type_name(obj)
    -- Full type name of a managed object or ValueType, nil otherwise.
    if type(obj) ~= "userdata" then return nil end
    local ok, td = pcall(function() return obj:get_type_definition() end)
    if not ok or td == nil then return nil end
    return td:get_full_name()
end

-- Runs fn inside an imgui window (only valid from a frame test) and always closes the window, so a failing
-- assertion does not leave the ImGui window stack unbalanced. Returns fn's result.
function H.in_window(title, fn, width, height)
    imgui.set_next_window_pos(Vector2f.new(40, 40), 1 << 3, Vector2f.new(0, 0)) -- ImGuiCond_Appearing
    imgui.set_next_window_size(Vector2f.new(width or 600, height or 400), 1 << 3)
    local opened = imgui.begin_window(title, true, 0)
    local ok, res = pcall(fn, opened)
    imgui.end_window()
    if not ok then error(res, 0) end
    return res
end

local Suite = {}
Suite.__index = Suite

function H.suite(name, opts)
    opts = opts or {}
    local s = setmetatable({
        name = name,
        results = {},
        pending = {},
        last = {},
        stage = "main",
        frame = 0,
        max_frames = opts.max_frames or 900,
        finished = false,
    }, Suite)
    log.info(string.format("[rftest] %s: starting (game %s, REFramework %s %s)", name, tostring(reframework:get_game_name()),
        tostring(reframework:get_commit_hash()), tostring(reframework:get_build_date())))
    return s
end

-- Records a result. It replaces an earlier entry only if that one was pre-recorded as pending (tests that could hang
-- record their FAIL first); any other repeat of a name is a FAIL of its own, so a PASS can never hide a FAIL.
function Suite:record(test_name, status, detail)
    local entry = { name = test_name, status = status, detail = detail or "" }
    local replaced = false
    for i, r in ipairs(self.results) do
        if r.name == test_name then
            if tostring(r.detail):sub(1, 8) == "pending:" then
                self.results[i] = entry
            else
                entry = { name = test_name .. " (duplicate name)", status = "FAIL", detail = "recorded twice; second: " .. status .. " " .. (detail or "") }
                self.results[#self.results + 1] = entry
            end
            replaced = true
            break
        end
    end
    if not replaced then self.results[#self.results + 1] = entry end
    log.info(string.format("[rftest] %s: %s %s%s", self.name, entry.status, entry.name, (entry.detail ~= "") and (" -- " .. entry.detail) or ""))
    -- Last-stage tests are the ones that can crash or hang the game on a broken build: write each outcome as soon as
    -- it is known, so a crash right after an assertion keeps the assertion's detail instead of the pending FAIL.
    if self.stage == "last" then self:write_results() end
end

local function classify(err)
    if getmetatable(err) == SKIP then return "SKIP", err.reason end
    return "FAIL", tostring(err)
end

function Suite:test(test_name, fn)
    local ok, err = pcall(fn)
    if ok then self:record(test_name, "PASS") else self:record(test_name, classify(err)) end
end

-- fn(frame) runs once per frame, starting next frame, until it returns true (PASS), raises (FAIL/SKIP), or the
-- suite's frame budget runs out (FAIL).
function Suite:frame_test(test_name, fn)
    self.pending[#self.pending + 1] = { name = test_name, fn = fn, frames = 0 }
end

-- A frame test that hangs or crashes the game on a build without the fix it covers. It runs after every other
-- test, after this suite's results were written with it pre-recorded as FAIL "pending: ...", and after the plugin
-- suite finished (or max_frames frames), so a hang or crash costs nothing but these tests.
function Suite:last_frame_test(test_name, fn)
    self.last[#self.last + 1] = { name = test_name, fn = fn, frames = 0 }
end

local function plugin_done()
    -- fs.read returns "" for a missing file; json.load_file would log a parse error every frame until it exists.
    local ok, text = pcall(fs.read, RESULTS_DIR .. "plugin_regression.json")
    if not ok or text == nil or text == "" then return false end
    local r = json.load_string(text)
    if type(r) ~= "table" or type(r.tests) ~= "table" then return false end
    for _, t in ipairs(r.tests) do
        if type(t.detail) == "string" and t.detail:find("^pending:") then return false end
    end
    return true
end

function Suite:write_results()
    local counts = { PASS = 0, FAIL = 0, SKIP = 0 }
    for _, r in ipairs(self.results) do counts[r.status] = (counts[r.status] or 0) + 1 end
    local out = {
        suite = self.name,
        game = reframework:get_game_name(),
        commit = reframework:get_commit_hash(),
        build_date = reframework:get_build_date(),
        frames = self.frame,
        pass = counts.PASS, fail = counts.FAIL, skip = counts.SKIP,
        tests = self.results,
    }
    local ok = json.dump_file(RESULTS_DIR .. self.name .. ".json", out)
    log.info(string.format("[rftest] %s: DONE pass=%d fail=%d skip=%d (results %s)", self.name, counts.PASS, counts.FAIL,
        counts.SKIP, ok and "written" or "NOT written"))
end

function Suite:run_pending()
    local still = {}
    for _, t in ipairs(self.pending) do
        t.frames = t.frames + 1
        local ok, done = pcall(t.fn, t.frames)
        if not ok then
            self:record(t.name, classify(done))
        elseif done then
            self:record(t.name, "PASS")
        elseif t.frames >= self.max_frames then
            self:record(t.name, "FAIL", "did not finish within " .. self.max_frames .. " frames")
        else
            still[#still + 1] = t
        end
    end
    self.pending = still
end

function Suite:finish()
    re.on_frame(function()
        if self.finished then return end
        self.frame = self.frame + 1

        if self.stage == "wait" then
            -- Wait for the plugin suite (its deadlock tests run last too), then start the last tests.
            if not plugin_done() and self.frame - self.wait_start < self.max_frames then return end
            self.pending = self.last
            self.stage = "last"
        end

        self:run_pending()
        if #self.pending > 0 then return end

        if self.stage == "main" and #self.last > 0 then
            for _, t in ipairs(self.last) do self:record(t.name, "FAIL", "pending: did not finish (hung or crashed the game?)") end
            self:write_results()
            self.stage = "wait"
            self.wait_start = self.frame
            return
        end

        self.finished = true
        self:write_results()
    end)
end

return H
