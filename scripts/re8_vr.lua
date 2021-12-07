if reframework:get_game_name() ~= "re8" then
    return
end

-- Trim the "userdata: " off of a string
-- todo: FIX IT so we don't need to do this
local function get_raw_userdata(ud)
    return tonumber("0x"..string.sub(tostring(ud), 11))
end

local system_object_type = sdk.find_type_definition("System.Object")
local system_type = sdk.find_type_definition("System.RuntimeType")
local system_array_type = sdk.find_type_definition("System.Array")
local system_single = sdk.find_type_definition("System.Single")
local system_activator_type = sdk.find_type_definition("System.Activator")

local function get_gameobject(component)
    return component:call("get_GameObject")
end

local function get_component(game_object, type_name)
    local t = sdk.typeof(type_name)

    if t == nil then 
        return nil
    end

    return game_object:call("getComponent(System.Type)", t)
end

local function get_components(game_object)
    local transform = game_object:call("get_Transform")

    if not transform then
        return {}
    end
    
    return game_object:call("get_Components"):get_elements()
end

local propsman = sdk.get_managed_singleton(sdk.game_namespace("PropsManager"))

local function get_localplayer()
    if not propsman then
        propsman = sdk.get_managed_singleton(sdk.game_namespace("PropsManager"))
    end

    return propsman:call("get_Player")
end

local player_data = {}
local last_camera_matrix = Matrix4x4f.new()

local left_hand_rotation_offset = Quaternion.new(Vector3f.new(0.4, 2.4, 1.7)):normalized()
local right_hand_rotation_offset = Quaternion.new(Vector3f.new(0.2, -2.5, -1.7)):normalized()

local left_hand_position_offset = Vector4f.new(-0.05, 0.05, 0.15, 0.0)
local right_hand_position_offset = Vector4f.new( 0.05, 0.05, 0.15, 0.0)

local ray_typedef = sdk.find_type_definition("via.Ray")
local last_muzzle_pos = Vector3f.new(0.0, 0.0, 0.0)
local last_muzzle_rot = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_muzzle_forward = Vector3f.new(0.0, 0.0, 0.0)

local last_right_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_right_hand_position = Vector3f.new(0.0, 0.0, 0.0)
local last_left_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_left_hand_position = Vector3f.new(0.0, 0.0, 0.0)

local via_motion_ik = sdk.find_type_definition("via.motion.ik")
local via_hid_gamepad = sdk.find_type_definition("via.hid.GamePad")

local via = {
    hid = {
        GamePadButton = {}
    }
}

local app = {
    HIDInputMode = {}
}

local function generate_enum(typename)
    local t = sdk.find_type_definition(typename)
    if not t then return {} end

    local fields = t:get_fields()
    local enum = {}

    for i, field in ipairs(fields) do
        if field:is_static() then
            local name = field:get_name()
            local raw_value = field:get_data(nil)

            log.info(name .. " = " .. tostring(raw_value))

            enum[name] = raw_value
        end
    end

    return enum
end

via.hid.GamePadButton = generate_enum("via.hid.GamePadButton")
via.hid.MouseButton = generate_enum("via.hid.MouseButton")
app.HIDInputMode = generate_enum("app.HIDInputMode")

local via_hid_mouse_typedef = sdk.find_type_definition("via.hid.Mouse")
local via_hid_mouse = sdk.get_native_singleton("via.hid.Mouse")

local function get_mouse_device()
    return sdk.call_native_func(via_hid_mouse, via_hid_mouse_typedef, "get_Device")
end

local function set_inputmode(mode)
    local hid_manager = sdk.get_managed_singleton(sdk.game_namespace("HIDManager"))

    if hid_manager then
        hid_manager:call("set_inputMode", mode)
    end
end

local last_padman_args = nil

local function update_pad_device(device)
    if not vrmod:is_hmd_active() then
        return
    end

    local raw_left_stick_axis = vrmod:get_left_stick_axis()

    local forward_dir = Vector4f.new(raw_left_stick_axis.x, 0.0, raw_left_stick_axis.y, 0.0)

    --local vr_left_stick_axis = last_camera_matrix:to_quat() * Vector4f.new(raw_left_stick_axis.x, raw_left_stick_axis.y, 0.0, 0.0)
    local vr_left_stick_axis = vrmod:get_left_stick_axis()
    local vr_right_stick_axis = vrmod:get_right_stick_axis()

    local cur_button = device:call("get_Button")
    
    device:call("set_AxisL", vr_left_stick_axis)
    device:call("set_RawAxisR", vr_right_stick_axis)
    device:call("set_AxisR", vr_right_stick_axis)

    -- we have these via.hid.GamePadButton values for the right stick
    -- EmuRup
    -- EmuRright
    -- EmuRdown
    -- EmuRleft

    -- set cur_button | according to the right stick axis
    if vr_right_stick_axis.x > 0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuRright
    elseif vr_right_stick_axis.x < -0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuRleft
    end

    if vr_right_stick_axis.y > 0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuRup
    elseif vr_right_stick_axis.y < -0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuRdown
    end

    if vr_left_stick_axis.x > 0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuLright
    elseif vr_left_stick_axis.x < -0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuLleft
    end

    if vr_left_stick_axis.y > 0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuLup
    elseif vr_left_stick_axis.y < -0.1 then
        cur_button = cur_button | via.hid.GamePadButton.EmuLdown
    end

    local action_trigger = vrmod:get_action_trigger()
    local action_grip = vrmod:get_action_grip()
    local action_a_button = vrmod:get_action_a_button()
    local action_b_button = vrmod:get_action_b_button()
    local action_joystick_click = vrmod:get_action_joystick_click()

    local right_joystick = vrmod:get_right_joystick()
    local left_joystick = vrmod:get_left_joystick()

    if vrmod:is_action_active(action_trigger, right_joystick) then
        device:call("set_AnalogR", 1.0)
        cur_button = cur_button | via.hid.GamePadButton.RTrigBottom
    end

    -- gripping right joystick causes "left trigger" to be pressed (aiming)
    if vrmod:is_action_active(action_grip, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LTrigBottom
        device:call("set_AnalogL", 1.0)
    end

    if vrmod:is_action_active(action_trigger, left_joystick) then
        -- set right bumper instead of left trigger
        cur_button = cur_button | via.hid.GamePadButton.LTrigTop

        -- set right bumper (heal) if holding both trigger and grip
        if vrmod:is_action_active(action_grip, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.RTrigTop
        end
    end

    if player_data.wants_block then
        cur_button = cur_button | via.hid.GamePadButton.LTrigTop
    end

    if vrmod:is_action_active(action_a_button, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.Decide | via.hid.GamePadButton.RDown
    end

    if vrmod:is_action_active(action_b_button, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RLeft
    end

    if vrmod:is_action_active(action_a_button, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.Cancel | via.hid.GamePadButton.RRight
    end

    if vrmod:is_action_active(action_b_button, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RUp
    end

    if vrmod:is_action_active(action_joystick_click, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RStickPush
    end

    if vrmod:is_action_active(action_joystick_click, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LStickPush
    end

    device:call("set_Button", cur_button)
    device:call("set_ButtonDown", cur_button)
end

local function update_padman(padman)
    if not vrmod:is_hmd_active() then
        return
    end

    local merged_pad = padman:call("get_mergedPad")

    if not merged_pad then
        return
    end

    local device = merged_pad:get_field("Device")

    if not device then
        return
    end

    update_pad_device(device)
    --merged_pad:call("updateStick")
    --merged_pad:call("updateButton")

    set_inputmode(app.HIDInputMode.Pad)
end

local function on_pre_hid_padman_update(args)
    last_padman_args = args

    local padman = sdk.to_managed_object(args[2])

    update_padman(padman)

    --[[padman:call("changeActiveUserPad", device)
    padman:set_field("ActivePad", merged_pad)

    local pad_list = padman:get_field("<PadList>k__BackingField")

    if pad_list then
        pad_list:call("set_Item(System.Int32, app.HIDPad)", 0, merged_pad)
    end]]
end

local function on_post_hid_padman_update(retval)
    local args = last_padman_args

    return retval
end

sdk.hook(sdk.find_type_definition(sdk.game_namespace("HIDPadManager")):get_method("doUpdate"), on_pre_hid_padman_update, on_post_hid_padman_update)

local function on_pre_is_satisfy(args)
    update_padman(sdk.to_managed_object(last_padman_args[2]))
    local padman = sdk.to_managed_object(last_padman_args[2])

    local merged_pad = padman:call("get_mergedPad")

    if not merged_pad then
        return
    end

    local stick_right_vertical = merged_pad:get_field("_StickRightVertical")
    --merged_pad:set_field("_StickRightVertical", 1.0)

    log.info(tostring(stick_right_vertical))
end

local function on_post_is_satisfy(retval)
    return sdk.to_ptr(1)
end

--sdk.hook(sdk.find_type_definition("app.Input.StickChecker"):get_method("isSatisfy(app.HIDPad)"), on_pre_is_satisfy, on_post_is_satisfy)

local inside_transform_count = 0

local function on_pre_update_player_transform(transform)
    if inside_transform_count > 0 or player_data.is_in_cutscene then
        return
    end

    inside_transform_count = inside_transform_count + 1

    local controllers = vrmod:get_controllers()

    if #controllers == 0 then
        inside_transform_count = inside_transform_count - 1
        return
    end

    --log.info("Muzzle rotation: " .. tostring(muzzle_rotation.x) .. " " .. tostring(muzzle_rotation.y) .. " " .. tostring(muzzle_rotation.z) .. " " .. tostring(muzzle_rotation.w))

    local joints = transform:call("get_Joints"):get_elements()
    local rotation = transform:call("get_Rotation")
    local position = transform:call("get_Position")

    local left_controller_transform = vrmod:get_transform(controllers[1])
    local right_controller_transform = vrmod:get_transform(controllers[2])
    local left_controller_rotation = left_controller_transform:to_quat()
    local right_controller_rotation = right_controller_transform:to_quat()

    local hmd_transform = vrmod:get_transform(0)
    local hmd_rotation = hmd_transform:to_quat()

    local left_controller_offset = left_controller_transform[3] - hmd_transform[3]
    local right_controller_offset = right_controller_transform[3] - hmd_transform[3]

    local camera = sdk.get_primary_camera()
    local camera_rotation = last_camera_matrix:to_quat()
    local original_camera_rotation = camera:call("get_WorldMatrix"):to_quat()

    --log.info("num joints: " .. tostring(#joints))

    for i, joint in ipairs(joints) do
        local joint_name = joint:call("get_Name")
        --log.info(joint_name)

        --[[local constraint_joint = joint:call("get_ConstraintJoint")

        if constraint_joint then
            --log.info(" Constraint: " .. constraint_joint:call("get_Name"))
        end

        local parent = joint:call("get_Parent")

        if parent then
            --log.info(" Parent: " .. parent:call("get_Name"))
        end]]

        if joint_name == "L_Hand" or joint_name == "R_Hand" then
            local joint_position = joint:call("get_Position")
            local new_pos = nil
            local new_rotation = nil

            if joint_name == "L_Hand" then
                new_rotation = original_camera_rotation * left_controller_rotation * left_hand_rotation_offset
                new_pos = last_camera_matrix[3] 
                            + ((original_camera_rotation * left_controller_offset) 
                            + ((original_camera_rotation * left_controller_rotation):normalized() * left_hand_position_offset))

                last_left_hand_position = new_pos
                last_left_hand_rotation = new_rotation

                player_data.left_hand_ik_transform:call("set_Position", last_left_hand_position)
                player_data.left_hand_ik_transform:call("set_Rotation", last_left_hand_rotation)
                player_data.left_hand_ik:set_field("Transition", 1.0)
                player_data.left_hand_ik:call("calc")
            else
                new_rotation = original_camera_rotation * right_controller_rotation * right_hand_rotation_offset
                new_pos = last_camera_matrix[3] 
                            + ((original_camera_rotation * right_controller_offset) 
                            + ((original_camera_rotation * right_controller_rotation):normalized() * right_hand_position_offset))

                last_right_hand_position = new_pos
                last_right_hand_rotation = new_rotation

                player_data.right_hand_ik_transform:call("set_Position", last_right_hand_position)
                player_data.right_hand_ik_transform:call("set_Rotation", last_right_hand_rotation)
                player_data.right_hand_ik:set_field("Transition", 1.0)
                player_data.right_hand_ik:call("calc")

                --[[local forearm = joint:call("get_Parent")
                local upper_arm = forearm:call("get_Parent")

                local w2 = transform:calculate_base_transform(joint)
                local w1 = transform:calculate_base_transform(forearm)
                local w0 = transform:calculate_base_transform(upper_arm)

                sdk.call_native_func(nil, via_motion_ik, "calculate2BoneCCDIK(via.mat4, via.mat4, via.mat4, via.vec3, via.motion.AxisDirection, via.motion.AxisDirection)",
                    w0, w1, w2, last_right_hand_position - position, 262147, 196610)

                local world_matrix = transform:call("get_WorldMatrix")

                w0 = world_matrix * w0
                w1 = world_matrix * w1
                w2 = world_matrix * w2

                joint:call("set_Position", w0[3])
                joint:call("set_Rotation", w0:to_quat())

                forearm:call("set_Position", w1[3])
                forearm:call("set_Rotation", w1:to_quat())

                upper_arm:call("set_Position", w2[3])
                upper_arm:call("set_Rotation", w2:to_quat())]]
            end

            --joint:call("set_Position", new_pos)
            --joint:call("set_Rotation", new_rotation)
        end
    end

    inside_transform_count = inside_transform_count - 1
end

--[[local function left_hand_ik_update(transform)
    --player_data.left_hand_ik_transform:call("set_Position", last_left_hand_position)
    --player_data.left_hand_ik:get_field("Joint2"):call("set_Position", last_left_hand_position)
end

local function right_hand_ik_update(transform)
    --player_data.right_hand_ik_transform:call("set_Position", last_right_hand_position)
end]]

local function update_player_data()
    if not player or player ~= get_localplayer() then
        player = get_localplayer()
    else 
        return
    end

    log.info("updating player data")
    player_data = {}

    if not player then 
        log.info("no player")
        return 
    end

    player_data.transform = player:call("get_Transform")
    player_data.updater = get_component(player, "app.PlayerUpdater")
    player_data.hand_touch = get_component(player, "app.PlayerHandTouch")
    player_data.game_event_action_controller = get_component(player, "app.GameEventActionController")

    if not player_data.updater or not player_data.hand_touch or not player_data.game_event_action_controller then
        log.info("no player updater or hand touch")
        player = nil
        player_data = {}
        return
    end

    player_data.reference = player_data.updater:get_field("playerContainer")

    if not player_data.reference then
        log.info("no player reference")
        player = nil
        player_data = {}
        return
    end


    local hand_ik = player_data.hand_touch:get_field("HandIK"):get_elements()
    --local hand_ik = { player_data.reference:get_field("<ikRHand>k__BackingField"), player_data.reference:get_field("<ikLHand>k__BackingField") }

    if #hand_ik < 2 then
        log.info("no hand ik")
        player = nil
        player_data = {}
        return
    end

    player_data.right_hand_ik = hand_ik[1]
    player_data.left_hand_ik = hand_ik[2]
    player_data.right_hand_ik_object = player_data.right_hand_ik:get_field("TargetGameObject")
    player_data.left_hand_ik_object = player_data.left_hand_ik:get_field("TargetGameObject")
    player_data.right_hand_ik_transform = player_data.right_hand_ik:get_field("Target")
    player_data.left_hand_ik_transform = player_data.left_hand_ik:get_field("Target")
    player_data.is_in_cutscene = false

    log.info("SETTING TRANFORM!!!")

    re.on_pre_update_transform(player_data.transform, on_pre_update_player_transform)
    --re.on_pre_update_transform(player_data.right_hand_ik_transform, right_hand_ik_update)
    --re.on_pre_update_transform(player_data.left_hand_ik_transform, left_hand_ik_update)
end

local function on_pre_shoot(args)
    if not vrmod:is_hmd_active() then
        return
    end

    local weapon = sdk.to_managed_object(args[2])
    local ray = args[3]

    sdk.set_native_field(ray, ray_typedef, "from", last_muzzle_pos)
    sdk.set_native_field(ray, ray_typedef, "dir", last_muzzle_forward)

    --sdk.call_native_func(ray, ray_typedef, ".ctor(via.vec3, via.vec3)", last_muzzle_pos, last_muzzle_forward)
end

local function on_post_shoot(retval)
    return retval
end

sdk.hook(sdk.find_type_definition("app.WeaponGunCore"):get_method("shoot"), on_pre_shoot, on_post_shoot)

local old_camera_rot = nil
local old_camera_pos = nil

--[[local function on_pre_playercamera_lateupdate(args)
    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    local hmd_transform = vrmod:get_transform(0)
    local hmd_rotation = hmd_transform:to_quat()

    old_camera_rot = camera_transform:call("get_Rotation")
    camera_transform:call("set_Rotation", last_camera_matrix:to_quat())
end

local function on_post_playercamera_lateupdate(retval)

    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    --camera_transform:call("set_Rotation", old_camera_rot)

    return retval
end

sdk.hook(sdk.find_type_definition("app.PlayerCamera"):get_method("lateUpdate"), on_pre_playercamera_lateupdate, on_post_playercamera_lateupdate)]]

local function on_pre_interact_manager_lateupdate(args)
    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    local hmd_transform = vrmod:get_transform(0)
    local hmd_rotation = hmd_transform:to_quat()

    old_camera_rot = camera_transform:call("get_Rotation")
    old_camera_pos = camera_transform:call("get_Position")
    camera_transform:call("set_Rotation", last_camera_matrix:to_quat())
    camera_transform:call("set_Position", last_camera_matrix[3])
end

local function on_post_interact_manager_lateupdate(retval)
    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    camera_transform:call("set_Rotation", old_camera_rot)
    camera_transform:call("set_Position", old_camera_pos)

    return retval
end

sdk.hook(sdk.find_type_definition("app.InteractManager"):get_method("doLateUpdate"), on_pre_interact_manager_lateupdate, on_post_interact_manager_lateupdate)

--re.on_pre_application_entry("UpdateBehavior", on_pre_interact_manager_lateupdate)
--re.on_application_entry("LateUpdateBehavior", on_post_interact_manager_lateupdate)

-- function to check if the player's hands are facing generally up and in front of the camera
-- so we can press the "LB" button in-game to block
local function check_player_hands_up()
    update_player_data()
    if not player then 
        player_data.wants_block = false
        return 
    end

    local right_hand_up = false
    local left_hand_up = false


    local controllers = vrmod:get_controllers()
    if #controllers < 2 then
        player_data.wants_block = false
        return
    end

    local hmd = vrmod:get_transform(0)
    local left_hand = vrmod:get_transform(controllers[1])
    local right_hand = vrmod:get_transform(controllers[2])

    local delta_to_left = left_hand[3] - hmd[3]
    local delta_to_right = right_hand[3] - hmd[3]
    local dir_to_left = delta_to_left:normalized()
    local dir_to_right = delta_to_right:normalized()

    local hmd_forward = hmd[2]

    local left_hand_dot = math.abs(hmd_forward:dot(dir_to_left))
    local right_hand_dot = math.abs(hmd_forward:dot(dir_to_right))

    local left_hand_in_front = left_hand_dot >= 0.8
    local right_hand_in_front = right_hand_dot >= 0.8

    local first_test = left_hand_in_front and right_hand_in_front

    if not first_test then
        player_data.wants_block = false
        return
    end

    -- now we need to check if the hands are facing up
    local left_hand_up_dot = math.abs(hmd_forward:dot(left_hand[0]))
    local right_hand_up_dot = math.abs(hmd_forward:dot(right_hand[0]))

    left_hand_up = left_hand_up_dot >= 0.5
    right_hand_up = right_hand_up_dot >= 0.5

    player_data.wants_block = left_hand_up and right_hand_up

    --log.info("left hand dot: " .. tostring(left_hand_dot))
    --log.info("right hand dot: " .. tostring(right_hand_dot))
end

re.on_application_entry("UpdateHID", function()
end)

re.on_application_entry("BeginRendering", function()
    update_player_data()
    if not player then return end

    --on_pre_update_player_transform(player_data.transform)

    check_player_hands_up()

    local camera = sdk.get_primary_camera()

    if camera ~= nil then
        last_camera_matrix = camera:call("get_WorldMatrix")
    end

    local game_event_action = player_data.game_event_action_controller:get_field("_GameEventAction")

    if game_event_action ~= nil then
        local is_motion_play = player_data.game_event_action_controller:get_field("_isMotionPlay")

        if is_motion_play then
            player_data.is_in_cutscene = true
        else
            player_data.is_in_cutscene = false
        end
    else
        player_data.is_in_cutscene = false
    end

    local player_gun = player_data.updater:call("get_playerGun")

    if not player_gun then
        return
    end

    local equipped_weapon = player_gun:call("get_equipWeaponObject")

    if not equipped_weapon then
        return
    end

    -- for some reason calling get_muzzleJoint causes lua to randomly freak out
    -- so we're just going to directly grab the field instead
    local muzzle_joint = equipped_weapon:get_field("MuzzleJoint")

    if not muzzle_joint then
        return
    end

    local muzzle_position = muzzle_joint:call("get_Position")
    local muzzle_rotation = muzzle_joint:call("get_Rotation")

    last_muzzle_pos = muzzle_position
    last_muzzle_rot = muzzle_rotation
    last_muzzle_forward = muzzle_joint:call("get_AxisZ")
end)

re.on_pre_gui_draw_element(function(element, context)
    if not vrmod:is_hmd_active() then return true end

    local game_object = element:call("get_GameObject")
    if game_object == nil then return true end

    local name = game_object:call("get_Name")

    log.info("drawing element: " .. name)

    if name == "GUIReticle" then
        if vrmod:is_using_controllers() then
            return false
        end
    end

    return true
end)