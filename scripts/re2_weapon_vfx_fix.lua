if reframework:get_game_name() ~= "re2" and reframework:get_game_name() ~= "re3" then
    return
end

local re2 = require("utility/RE2")
local GameObject = require("utility/GameObject")

local last_weapon_pos = Vector3f.new(0, 0, 0)
local last_weapon_rot = Quaternion.new(0, 0, 0, 0)

local last_weapon_transform = nil
local original_weapon_pos = Vector3f.new(0, 0, 0)
local original_weapon_rot = Quaternion.new(0, 0, 0, 0)

-- optimizations to prevent constant method lookups
local via_transform = sdk.find_type_definition("via.Transform")
local get_position_method = via_transform:get_method("get_Position")
local get_rotation_method = via_transform:get_method("get_Rotation")
local set_position_method = via_transform:get_method("set_Position")
local set_rotation_method = via_transform:get_method("set_Rotation")

local via_gameobject = sdk.find_type_definition("via.GameObject")
local get_transform_method = via_gameobject:get_method("get_Transform")

-- Sets the weapon position and rotation to the last known position and rotation
-- at the time of rendering, and then restores it after the callback
-- reason being, this callback displays effects on the weapon before
-- any of our VR modifications have been applied
re.on_pre_application_entry("EndUpdateEffect", function()
    last_weapon_transform = nil

    local player = re2.player
    if not player then return end

    local player_transform = get_transform_method:call(player)
    if not player_transform then return end

    local weapon = re2.weapon
    if not weapon then return end

    local weapon_gameobject = re2.weapon_gameobject
    if not weapon_gameobject then return end

    local weapon_transform = get_transform_method:call(weapon_gameobject)
    if not weapon_transform then return end

    last_weapon_transform = weapon_transform

    original_weapon_pos = get_position_method:call(weapon_transform)
    original_weapon_rot = get_rotation_method:call(weapon_transform)

    set_position_method:call(weapon_transform, last_weapon_pos)
    set_rotation_method:call(weapon_transform, last_weapon_rot)
end)

re.on_application_entry("EndUpdateEffect", function()
    if last_weapon_transform ~= nil then
        set_position_method:call(last_weapon_transform, original_weapon_pos)
        set_rotation_method:call(last_weapon_transform, original_weapon_rot)
        last_weapon_transform = nil
    end
end)

re.on_application_entry("BeginRendering", function()
    local player = re2.player
    if not player then return end

    local player_transform = get_transform_method:call(player)
    if not player_transform then return end

    local weapon = re2.weapon
    if not weapon then return end

    local weapon_gameobject = re2.weapon_gameobject
    if not weapon_gameobject then return end

    local weapon_transform = get_transform_method:call(weapon_gameobject)
    if not weapon_transform then return end

    last_weapon_pos = get_position_method:call(weapon_transform)
    last_weapon_rot = get_rotation_method:call(weapon_transform)
end)
