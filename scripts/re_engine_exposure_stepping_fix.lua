-- Stops the brightness stepping in daylight, where the image holds still and then
-- jumps to a different brightness as the lighting changes instead of moving
-- smoothly. Most people perceive it as a flickering.
--
-- ToneMapping comes with RoundExposure on, which rounds the auto exposure result
-- to set steps instead of letting it move freely. Auto exposure tracking the light
-- is fine, that's its job, the problem is the rounding sitting on top of it. So as
-- the lighting drifts (time of day, cloud coverage, fog, etc) the exposure snaps
-- from one step to the next instead of following the light.
--
-- Nothing else is touched. Adaptation, the EV range and the adaptation rates are
-- all left how the game comes with them, only the rounding changes.

local tonemapping_typedef = sdk.find_type_definition("via.render.ToneMapping")

if tonemapping_typedef == nil then
    return
end

local tonemapping_type = sdk.typeof("via.render.ToneMapping")

local get_round_exposure = tonemapping_typedef:get_method("get_RoundExposure")
local set_round_exposure = tonemapping_typedef:get_method("set_RoundExposure")
local get_valid = tonemapping_typedef:get_method("get_Valid")

local component_get_gameobject = sdk.find_type_definition("via.Component"):get_method("get_GameObject")
local gameobject_get_component = sdk.find_type_definition("via.GameObject"):get_method("getComponent(System.Type)")
local gameobject_get_name = sdk.find_type_definition("via.GameObject"):get_method("get_Name")

if get_round_exposure == nil or set_round_exposure == nil then
    return
end

local cfg = {
    enabled = true,
    hotkey = false,
    show_state = false
}

local KEY_TOGGLE = 117 -- F6

local tonemapping = nil
local found_on = "nothing yet"
local last_check = 0
local corrections = 0
local last_toggle = -1000

-- Pull the tonemapper off whatever camera is actually rendering so it keeps
-- working through cutscenes and area changes. findComponents comes back empty on
-- some builds, getComponent is fine.
local function find_tonemapping()
    local camera = sdk.get_primary_camera()

    if camera == nil then
        return nil
    end

    local gameobject = component_get_gameobject:call(camera)

    if gameobject == nil then
        return nil
    end

    local component = nil

    if gameobject_get_component ~= nil then
        component = gameobject_get_component:call(gameobject, tonemapping_type)
    else
        component = gameobject:call("getComponent(System.Type)", tonemapping_type)
    end

    if component ~= nil and gameobject_get_name ~= nil then
        found_on = gameobject_get_name:call(gameobject) or "unknown"
    end

    return component
end

local function apply()
    if not cfg.enabled then
        return
    end

    local time = os.clock()

    -- recheck once a second, and whenever the cached one goes invalid
    if tonemapping == nil or time - last_check > 1.0 then
        if tonemapping == nil or get_valid:call(tonemapping) == false then
            tonemapping = find_tonemapping()
        end

        last_check = time
    end

    if tonemapping == nil then
        return
    end

    if get_round_exposure:call(tonemapping) ~= false then
        set_round_exposure:call(tonemapping, false)
        corrections = corrections + 1
    end
end

-- This has to run from the engine update hooks, not just re.on_frame. Writing from
-- on_frame happens too late in the frame, the render state is already locked in by
-- then, so the value gets set but nothing actually changes on screen. It also has
-- to be re-applied every frame since the game writes its own values back over it
-- during the post effect update.
local entry_points = {
    "UpdateBehavior",
    "LateUpdateBehavior",
    "UpdateMotion",
    "PrepareRendering",
    "BeginRendering",
    "EndRendering"
}

for i = 1, #entry_points do
    local name = entry_points[i]

    pcall(function()
        re.on_pre_application_entry(name, apply)
        re.on_application_entry(name, apply)
    end)
end

-- no hotkey helper in the API so this reads the keyboard itself and does its own
-- edge detection, otherwise it fires every frame the key is held
local keyboard = nil
local keyboard_method = nil
local key_was_down = false

local function key_pressed(code)
    if keyboard == nil then
        local singleton = sdk.get_native_singleton("via.hid.Keyboard")
        local typedef = sdk.find_type_definition("via.hid.Keyboard")

        if singleton == nil or typedef == nil then
            return false
        end

        keyboard = sdk.call_native_func(singleton, typedef, "get_Device")

        if keyboard == nil then
            return false
        end

        local methods = { "getDown", "isDown", "getState" }

        for i = 1, #methods do
            local ok, result = pcall(function() return keyboard:call(methods[i], 65) end)

            if ok and result ~= nil then
                keyboard_method = methods[i]
                break
            end
        end
    end

    if keyboard_method == nil then
        return false
    end

    local ok, result = pcall(function() return keyboard:call(keyboard_method, code) end)
    local down = ok and (result == true or (type(result) == "number" and result ~= 0))
    local pressed = down and not key_was_down

    key_was_down = down

    return pressed
end

local function set_enabled(value)
    cfg.enabled = value
    last_toggle = os.clock()

    -- put it back straight away so the difference is visible immediately
    if not cfg.enabled and tonemapping ~= nil then
        set_round_exposure:call(tonemapping, true)
    end
end

re.on_frame(function()
    if cfg.hotkey and key_pressed(KEY_TOGGLE) then
        set_enabled(not cfg.enabled)
    end

    apply()

    local just_toggled = os.clock() - last_toggle < 2.0

    if not cfg.show_state and not just_toggled then
        return
    end

    local current = nil

    if tonemapping ~= nil then
        current = get_round_exposure:call(tonemapping)
    end

    local on = current == false

    draw.filled_rect(10, 10, 400, 24, 0xB0000000)
    draw.text(on and "EXPOSURE STEPPING FIX: ON" or "EXPOSURE STEPPING FIX: OFF",
        18, 16, on and 0xFF66BB6A or 0xFFFF5252)

    -- show the value it actually read back, so a toggle can be confirmed without
    -- leaving the overlay turned on
    if just_toggled then
        draw.text("RoundExposure = " .. tostring(current), 18, 38, 0xFFFFFFFF)
    end
end)

-- put the original value back on unload, otherwise it sticks around in the running
-- game after the script is gone
re.on_script_reset(function()
    if tonemapping ~= nil then
        set_round_exposure:call(tonemapping, true)
    end
end)

re.on_draw_ui(function()
    if imgui.tree_node("Exposure Stepping Fix") then
        local changed = false

        changed, cfg.enabled = imgui.checkbox("Enabled", cfg.enabled)

        if changed then
            set_enabled(cfg.enabled)
        end

        changed, cfg.hotkey = imgui.checkbox("F6 toggles in game", cfg.hotkey)
        changed, cfg.show_state = imgui.checkbox("Show state on screen", cfg.show_state)

        local current = nil

        if tonemapping ~= nil then
            current = get_round_exposure:call(tonemapping)
        end

        imgui.text("Tonemapper: " .. (tonemapping ~= nil and found_on or "not found"))
        imgui.text("RoundExposure: " .. tostring(current))
        imgui.text("Corrections: " .. corrections)

        imgui.tree_pop()
    end
end)
