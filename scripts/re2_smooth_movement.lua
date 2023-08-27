local gn = reframework:get_game_name()
if gn ~= "re2" and gn ~= "re3" then 
    return
end

local re2 = require("utility/RE2")

local cfg = {
    movement_stabilization = true
}

local transform_get_position = sdk.find_type_definition("via.Transform"):get_method("get_Position")
local transform_get_rotation = sdk.find_type_definition("via.Transform"):get_method("get_Rotation")
local transform_set_position = sdk.find_type_definition("via.Transform"):get_method("set_Position")
local transform_set_rotation = sdk.find_type_definition("via.Transform"):get_method("set_Rotation")
local transform_get_joints = sdk.find_type_definition("via.Transform"):get_method("get_Joints")
local transform_get_joint_by_hash = sdk.find_type_definition("via.Transform"):get_method("getJointByHash")
local transform_get_joint_by_name = sdk.find_type_definition("via.Transform"):get_method("getJointByName")


local joint_get_position = sdk.find_type_definition("via.Joint"):get_method("get_Position")
local joint_get_rotation = sdk.find_type_definition("via.Joint"):get_method("get_Rotation")
local joint_set_position = sdk.find_type_definition("via.Joint"):get_method("set_Position")
local joint_set_rotation = sdk.find_type_definition("via.Joint"):get_method("set_Rotation")
local joint_get_parent = sdk.find_type_definition("via.Joint"):get_method("get_Parent")

local component_get_gameobject = sdk.find_type_definition("via.Component"):get_method("get_GameObject")
local gameobject_get_transform = sdk.find_type_definition("via.GameObject"):get_method("get_Transform")


local last_player_position = Vector3f.new(0, 0, 0)
local has_valid_player_position = false

local gamepad_singleton_t = sdk.find_type_definition("via.hid.GamePad")

local last_ema_speed = 0
local alpha = 0.05

local function EMA(current_value, last_EMA, alpha)
    return alpha * current_value + (1 - alpha) * last_EMA
end


local jackdominator_rtt = nil
local jackdominator_td = sdk.find_type_definition(sdk.game_namespace("JackDominator"))
local jacked_method = jackdominator_td:get_method("get_Jacked")

local function is_jacked(go)
    jackdominator_rtt = jackdominator_rtt or sdk.typeof(sdk.game_namespace("JackDominator"))
    local jd = go:call("getComponent(System.Type)", jackdominator_rtt)
    if not jd then return false end

    return jacked_method:call(jd)
end

local function get_left_input_axis()
    if vrmod:is_using_controllers() then
        local axis = vrmod:get_left_stick_axis()

        if axis:length() > 0.0 then
            return axis
        end
    end

    local gamepad_singleton = sdk.get_native_singleton("via.hid.GamePad")
    if not gamepad_singleton then return Vector2f.new(0, 0) end

    local pad = sdk.call_native_func(gamepad_singleton, gamepad_singleton_t, "get_LastInputDevice")
    if not pad then return Vector2f.new(0, 0) end

    return pad:get_AxisL()
end

local last_time = os.clock()

re.on_application_entry("UpdateMotion", function()
    local delta_t = os.clock() - last_time
    last_time = os.clock()

    if not re2.player then
        has_valid_player_position = false
        return 
    end

    if not firstpersonmod:will_be_used() or is_jacked(re2.player) then
        has_valid_player_position = false
        return
    end

    local camera = sdk.get_primary_camera()
    if not camera then print("no cam") return end

    local camera_gameobject = component_get_gameobject(camera)
    if not camera_gameobject then print("no go") return end

    local camera_transform = gameobject_get_transform(camera_gameobject)
    if not camera_transform then print("no cam tran") return end

    local body_transform =  gameobject_get_transform(re2.player)
    if not body_transform then print("no body transform") return end

    local camera_joint = camera_transform:call("get_Joints")[0]
    
    if cfg.movement_stabilization then
        local current_player_position = transform_get_position(body_transform)

        if has_valid_player_position then
            local delta = current_player_position - last_player_position
            delta.y = 0.0
            local speed = delta:length()

            -- cap the speed to something that won't balloon out of control.
            speed = math.min(speed, 1.0)

            local speed_normalized = speed / delta_t

            local ema_speed = EMA(speed_normalized, last_ema_speed, alpha)
            last_ema_speed = ema_speed

            -- un-normalize the speed
            speed = ema_speed * delta_t

            local camera_dir = joint_get_rotation(camera_joint) * Vector3f.new(0, 0, 1)
            local axis_l = get_left_input_axis()

            -- Try to adjust the player position so it moves towards pad direction
            -- instead of using root motion
            if axis_l:length() > 0.0 then
                local flat_camera_dir = Vector3f.new(camera_dir.x, 0.0, camera_dir.z):normalized()
                local flat_camera_rot = flat_camera_dir:to_quat()
                local axis_l_dir = (flat_camera_rot * Vector3f.new(axis_l.x, 0.0, -axis_l.y)):normalized()

                if axis_l_dir:length() > 0.0 then
                    local new_pos = last_player_position + (axis_l_dir * speed)
                    new_pos.y = current_player_position.y
                    body_transform:set_position(new_pos)
                end
            end
        end

        last_player_position = transform_get_position(body_transform)
        has_valid_player_position = true
    else
        has_valid_player_position = false
    end
end)

re.on_pre_application_entry("LockScene", function()
    if not re2.player then return end

    local camera = sdk.get_primary_camera()
    if not camera then print("no cam") return end

    local camera_gameobject = component_get_gameobject(camera)
    if not camera_gameobject then print("no go") return end

    local camera_transform = gameobject_get_transform(camera_gameobject)
    if not camera_transform then print("no cam tran") return end

    local camera_joint = camera_transform:call("get_Joints")[0]
    if camera_joint == nil then return end

    -- Render position
    if re2.player and cfg.movement_stabilization and firstpersonmod:will_be_used() then
        last_player_position = transform_get_position(gameobject_get_transform(re2.player))
        has_valid_player_position = true
    else
        last_player_position = Vector3f.new(0, 0, 0)
        has_valid_player_position = false
    end
end)