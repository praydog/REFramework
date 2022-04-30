local game_name = reframework:get_game_name()
local is_re7 = game_name == "re7"
local is_re8 = game_name == "re8"

if not is_re7 and not is_re8 then
    return
end

local statics = require("utility/Statics")
local re8 = require("utility/RE8")
local GameObject = require("utility/GameObject")

local renderer = sdk.get_native_singleton("via.render.Renderer")
local renderer_type = sdk.find_type_definition("via.render.Renderer")

local GamePadButton = statics.generate("via.hid.GamePadButton")

local openxr = {
    left_hand_rotation_vec = Vector3f.new(0.186417 + 0.2, 2.820591, 1.221779), -- pitch yaw roll?
    right_hand_rotation_vec = Vector3f.new(0.186417, -2.820591, -1.221779), -- pitch yaw roll?
    left_hand_position_offset = Vector4f.new(-0.036934, 0.069525, 0.017501, 0.0),
    right_hand_position_offset = Vector4f.new(0.036934, 0.069525, 0.017501, 0.0)
}

local is_openxr = vrmod:is_openxr_loaded()

local last_original_right_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_camera_matrix = Matrix4x4f.new()
local last_right_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_right_hand_position = Vector3f.new(0.0, 0.0, 0.0)
local last_left_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_left_hand_position = Vector3f.new(0.0, 0.0, 0.0)

local left_hand_rotation_vec = Vector3f.new(-0.105 + 0.2, 2.37, 1.10) -- pitch yaw roll?
local right_hand_rotation_vec = Vector3f.new(-0.105, -2.37, -1.10) -- pitch yaw roll?

if is_openxr then
    left_hand_rotation_vec = openxr.left_hand_rotation_vec:clone()
    right_hand_rotation_vec = openxr.right_hand_rotation_vec:clone()
end

local left_hand_rotation_offset = Quaternion.new(left_hand_rotation_vec):normalized()
local right_hand_rotation_offset = Quaternion.new(right_hand_rotation_vec):normalized()

local left_hand_position_offset = Vector4f.new(-0.025, 0.045, 0.155, 0.0)
local right_hand_position_offset = Vector4f.new(0.025, 0.045, 0.155, 0.0)

if is_openxr then
    left_hand_position_offset = openxr.left_hand_position_offset:clone()
    right_hand_position_offset = openxr.right_hand_position_offset:clone()
end

local ray_typedef = sdk.find_type_definition("via.Ray")
local last_muzzle_pos = Vector4f.new(0.0, 0.0, 0.0, 1.0)
local last_muzzle_rot = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_muzzle_forward = Vector4f.new(0.0, 0.0, 0.0, 1.0)
local last_shoot_pos = Vector4f.new(0.0, 0.0, 0.0, 1.0)
local last_shoot_dir = Vector4f.new(0.0, 0.0, 0.0, 1.0)

local transform_get_position = sdk.find_type_definition("via.Transform"):get_method("get_Position")
local transform_get_rotation = sdk.find_type_definition("via.Transform"):get_method("get_Rotation")
local transform_set_position = sdk.find_type_definition("via.Transform"):get_method("set_Position")
local transform_set_rotation = sdk.find_type_definition("via.Transform"):get_method("set_Rotation")
local transform_get_joints = sdk.find_type_definition("via.Transform"):get_method("get_Joints")
local transform_get_joint_by_hash = sdk.find_type_definition("via.Transform"):get_method("getJointByHash")

local joint_get_position = sdk.find_type_definition("via.Joint"):get_method("get_Position")
local joint_get_rotation = sdk.find_type_definition("via.Joint"):get_method("get_Rotation")
local joint_set_position = sdk.find_type_definition("via.Joint"):get_method("set_Position")
local joint_set_rotation = sdk.find_type_definition("via.Joint"):get_method("set_Rotation")
local joint_get_parent = sdk.find_type_definition("via.Joint"):get_method("get_Parent")

local component_get_gameobject = sdk.find_type_definition("via.Component"):get_method("get_GameObject")
local gameobject_get_transform = sdk.find_type_definition("via.GameObject"):get_method("get_Transform")

local motion_get_joint_index_by_name_hash = sdk.find_type_definition("via.motion.Motion"):get_method("getJointIndexByNameHash")
local motion_get_world_position = sdk.find_type_definition("via.motion.Motion"):get_method("getWorldPosition")
local motion_get_world_rotation = sdk.find_type_definition("via.motion.Motion"):get_method("getWorldRotation")

local cast_ray_method = sdk.find_type_definition("via.physics.System"):get_method("castRay(via.physics.CastRayQuery, via.physics.CastRayResult)")
local cast_ray_async_method = sdk.find_type_definition("via.physics.System"):get_method("castRayAsync(via.physics.CastRayQuery, via.physics.CastRayResult)")

local cfg_path = "re7_vr/main_config.json"

local queue_recenter = false
local was_vert_limited = false

local last_gui_offset = Quaternion.identity()
local last_gui_quat = Quaternion.identity()
local last_gui_dot = 0.0
local last_gui_forced_slerp = os.clock()
local needs_cutscene_recenter = false
local last_inventory_open_time = 0.0
local last_shop_open_time = 0.0
local last_scope_time = 0.0
local head_hash = nil


local neg_forward_identity = Matrix4x4f.new(-1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, -1, 0,
                                            0, 0, 0, 1):to_quat()

local cfg = {
    movement_shake = false,
    all_camera_shake = true,
    disable_crosshair = false
}

local function load_cfg()
    local loaded_cfg = json.load_file(cfg_path)

    if loaded_cfg == nil then
        json.dump_file(cfg_path, cfg)
        return
    end

    for k, v in pairs(loaded_cfg) do
        cfg[k] = v
    end
end

load_cfg()

statics.generate_global("via.hid.GamePadButton")
statics.generate_global("via.hid.MouseButton")

local CollisionLayer = nil
local CollisionFilter = nil

if is_re8 then
    CollisionLayer = statics.generate("app.CollisionManager.Layer")
    CollisionFilter = statics.generate("app.CollisionManager.Filter")
elseif is_re7 then
    CollisionLayer = statics.generate("app.Collision.CollisionSystem.Layer")
    CollisionFilter = statics.generate("app.Collision.CollisionSystem.Filter")
end

if is_re7 then
    statics.generate_global("app.HIDManager.InputMode")
elseif is_re8 then
    statics.generate_global("app.HIDInputMode")
end

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

local is_inventory_open = false

local via_murmur_hash = sdk.find_type_definition("via.murmur_hash")
local via_murmur_hash_calc32 = via_murmur_hash:get_method("calc32")
local vfx_muzzle1_hash = via_murmur_hash_calc32:call(nil, "vfx_muzzle1")
local vfx_muzzle2_hash = via_murmur_hash_calc32:call(nil, "vfx_muzzle2")

local function update_muzzle_data()
    if re8.weapon then
        -- for some reason calling get_muzzleJoint causes lua to randomly freak out
        -- so we're just going to directly grab the field instead
        local muzzle_joint = re8.weapon:get_field("MuzzleJoint")

        if muzzle_joint == nil then
            local weapon_gameobject = nil
            
            if is_re7 then
                weapon_gameobject = re8.weapon:call("get_GameObject")
            elseif is_re8 then
                weapon_gameobject = re8.weapon:get_field("<owner>k__BackingField")
            end

            if weapon_gameobject ~= nil then
                local transform = gameobject_get_transform(weapon_gameobject)

                if transform ~= nil then
                    muzzle_joint = transform_get_joint_by_hash(transform, vfx_muzzle1_hash)

                    if not muzzle_joint then
                        muzzle_joint = transform_get_joint_by_hash(transform, vfx_muzzle2_hash)
                    end
                end
            end
        end

        if muzzle_joint then
            local muzzle_position = joint_get_position(muzzle_joint)
            local muzzle_rotation = joint_get_rotation(muzzle_joint)
    
            last_muzzle_pos = muzzle_position
            last_muzzle_rot = muzzle_rotation
            last_muzzle_forward = muzzle_joint:call("get_AxisZ")

            if vrmod:is_using_controllers() then
                last_shoot_dir = last_muzzle_forward
                last_shoot_pos = last_muzzle_pos
            end
        elseif vrmod:is_using_controllers() then
            last_muzzle_pos = last_right_hand_position
            last_muzzle_rot = last_camera_matrix:to_quat()
            last_muzzle_forward = (last_muzzle_rot * Vector3f.new(0, 0, -1)):normalized()

            last_shoot_dir = last_muzzle_forward
            last_shoot_pos = last_muzzle_pos
        else
            last_muzzle_pos = last_shoot_pos
            last_muzzle_forward = last_shoot_dir
        end
    end
end

local function update_pad_device(device)
    if not vrmod:is_hmd_active() then
        re8.is_holding_left_grip = false
        return
    end

    if is_re7 then
        local menu_manager = sdk.get_managed_singleton("app.MenuManager")

        if menu_manager ~= nil then
            is_inventory_open = menu_manager:call("isOpenInventoryMenu")
        end
    elseif is_re8 then
        is_inventory_open = (os.clock() - last_inventory_open_time) < 0.25
    end

    local raw_left_stick_axis = vrmod:get_left_stick_axis()
    
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
    if is_re8 then
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
    end

    local action_trigger = vrmod:get_action_trigger()
    local action_grip = vrmod:get_action_grip()
    local action_a_button = vrmod:get_action_a_button()
    local action_b_button = vrmod:get_action_b_button()
    local action_joystick_click = vrmod:get_action_joystick_click()
    local action_weapon_dial = vrmod:get_action_weapon_dial()
    local action_minimap = vrmod:get_action_minimap()
    local action_block = vrmod:get_action_block()
    local action_dpad_up = vrmod:get_action_dpad_up()
    local action_dpad_down = vrmod:get_action_dpad_down()
    local action_dpad_left = vrmod:get_action_dpad_left()
    local action_dpad_right = vrmod:get_action_dpad_right()
    local action_heal = vrmod:get_action_heal()

    local right_joystick = vrmod:get_right_joystick()
    local left_joystick = vrmod:get_left_joystick()

    if vrmod:is_action_active(action_trigger, right_joystick) then
        device:call("set_AnalogR", 1.0)
        cur_button = cur_button | via.hid.GamePadButton.RTrigBottom

        if is_inventory_open then
            cur_button = cur_button | via.hid.GamePadButton.RTrigTop
        end
    end

    -- gripping right joystick causes "left trigger" to be pressed (aiming)
    if vrmod:is_action_active(action_grip, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LTrigBottom
        device:call("set_AnalogL", 1.0)
    end

    if vrmod:is_action_active(action_weapon_dial, left_joystick) or vrmod:is_action_active(action_weapon_dial, right_joystick) then
        -- DPad mimickry
        if vr_left_stick_axis.y >= 0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LUp
        elseif vr_left_stick_axis.y <= -0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LDown
        end

        if vr_left_stick_axis.x >= 0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LRight
        elseif vr_left_stick_axis.x <= -0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LLeft
        end
    else
        if vrmod:is_action_active(action_dpad_up, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.LUp
        end

        if vrmod:is_action_active(action_dpad_down, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.LDown
        end

        if vrmod:is_action_active(action_dpad_left, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.LLeft
        end

        if vrmod:is_action_active(action_dpad_right, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.LRight
        end
    end

    if vrmod:is_action_active(action_trigger, left_joystick) then
        if is_inventory_open then
            cur_button = cur_button | via.hid.GamePadButton.LTrigTop
        end

        -- set right bumper (heal) if holding both trigger and grip
        --[[if vrmod:is_action_active(action_grip, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.RTrigTop
        end]]
    end

    if vrmod:is_action_active(action_heal, left_joystick) or vrmod:is_action_active(action_heal, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RTrigTop
    end
    
    re8.is_holding_left_grip = vrmod:is_action_active(action_grip, left_joystick)

    if re8.wants_block or vrmod:is_action_active(action_block, left_joystick) or vrmod:is_action_active(action_block, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LTrigTop
        re8.wants_block = true
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

    if vrmod:is_action_active(action_minimap, right_joystick) or vrmod:is_action_active(action_minimap, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.CLeft
    end

    device:call("set_Button", cur_button)
    device:call("set_ButtonDown", cur_button)
end

local function update_padman(padman)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        re8.is_holding_left_grip = false
        return
    end

    local merged_pad = padman:call("get_mergedPad")

    if not merged_pad then
        return
    end

    --padman:call("set_activePad", merged_pad)

    local device = merged_pad:get_field("Device")

    if not device then
        return
    end

    update_pad_device(device)
    --merged_pad:call("updateStick")
    --merged_pad:call("updateButton")

    if is_re7 then
        set_inputmode(app.HIDManager.InputMode.Pad)
    elseif is_re8 then
        set_inputmode(app.HIDInputMode.Pad)
    end
end

-- RE8 only.
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

local function on_pre_app_pad_update(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    local pad = sdk.to_managed_object(args[2])

    local padman = sdk.get_managed_singleton(sdk.game_namespace("PadManager"))

    if not padman then
        return
    end

    local merged_pad = padman:call("get_mergedPad")

    if not merged_pad or merged_pad ~= pad then
        return
    end

    local device = merged_pad:get_field("Device")

    if not device then
        return
    end

    update_pad_device(device)
    --merged_pad:call("updateStick")
    --merged_pad:call("updateButton")

    if is_re7 then
        set_inputmode(app.HIDManager.InputMode.Pad)
    elseif is_re8 then
        set_inputmode(app.HIDInputMode.Pad)
    end
end

local function on_post_app_pad_update(retval)
    return retval
end

if is_re7 then
    sdk.hook(
        sdk.find_type_definition("app.Pad"):get_method("update"),
        on_pre_app_pad_update,
        on_post_app_pad_update
    )
elseif is_re8 then
    sdk.hook(sdk.find_type_definition(sdk.game_namespace("HIDPadManager")):get_method("doUpdate"), on_pre_hid_padman_update, on_post_hid_padman_update)
end

local function on_pre_try_guard_start(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    -- this will only allow blocking by physically holding your hands up.
    if not re8.wants_block then
        return sdk.PreHookResult.SKIP_ORIGINAL
    end
end

local function on_post_try_guard_start(retval)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() or re8.wants_block then
        return retval
    end

    return sdk.to_ptr(0)
end

sdk.hook(
    sdk.find_type_definition("app.PlayerBase"):get_method("tryGuardStart"),
    on_pre_try_guard_start,
    on_post_try_guard_start
)

local function calculate_tpose_world(joint, depth)
    if not depth then
        depth = 1
    end

    local original_positions = {}
    local original_rotations = {}
    local current_positions = {}

    local player_transform = re8.transform
    local player_pos = transform_get_position:call(player_transform)
    local player_rot = transform_get_rotation:call(player_transform)

    local joints = {}

    local cur_joint = joint

    for i=1, depth do
        cur_joint = joint_get_parent:call(cur_joint)
        table.insert(joints, cur_joint)
    end

    local parent_pos = joint_get_position:call(cur_joint)
    local parent_rot = joint_get_rotation:call(cur_joint)
    local original_parent_pos = player_pos + (player_rot * player_transform:calculate_base_transform(cur_joint)[3])

    for i=1, depth do
        local joint = joints[depth-i]

        local original_pos = player_pos + (player_rot * player_transform:calculate_base_transform(joint)[3])
        local diff = original_pos - original_parent_pos
        local updated_pos = parent_pos + diff
        
        original_parent_pos = original_pos
        parent_pos = updated_pos
    end

    local original_pos = player_pos + (player_rot * player_transform:calculate_base_transform(joint)[3])
    local diff = original_pos - original_parent_pos
    return parent_pos + diff
end

local function set_hand_joints_to_tpose(hand_ik)
    -- fixes an instance in not a hero where
    -- chris is supposed to jump down something without tripping a laser
    if re8.is_in_cutscene then return end

    local hashes = {
    }

    if is_re7 then
        hashes = {
            hand_ik:get_field("HashJoint0"),
            hand_ik:get_field("HashJoint1"),
            hand_ik:get_field("HashJoint2"),
            hand_ik:get_field("HashJoint3")
        }
    elseif is_re8 then
        hashes = {
            hand_ik:get_field("<HashJoint0>k__BackingField"),
            hand_ik:get_field("<HashJoint1>k__BackingField"),
            hand_ik:get_field("<HashJoint2>k__BackingField")
        }
    end

    local original_positions = {}
    local original_rotations = {}
    local current_positions = {}

    local player_transform = re8.transform
    local player_pos = player_transform:get_position()
    local player_rot = player_transform:get_rotation()

    local joints = {}

    for i, hash in ipairs(hashes) do
        if hash and hash ~= 0 then
            local joint = transform_get_joint_by_hash:call(player_transform, hash)

            if joint then
                table.insert(joints, joint_get_parent:call(joint))
            end
        end
    end

    if #joints > 0 and joints[1] ~= nil then
        table.insert(joints, 1, joint_get_parent:call(joints[1]))

        if not re8.is_grapple_aim then
            table.insert(joints, 1, joint_get_parent:call(joints[1]))
            table.insert(joints, 1, joint_get_parent:call(joints[1]))
        end
    end

    for i, joint in ipairs(joints) do
        local base_transform = player_transform:calculate_base_transform(joint)
        original_positions[i] = player_pos + (player_rot * base_transform[3])
        original_rotations[i] = player_rot * base_transform:to_quat()
        current_positions[i] = joint_get_position:call(joint)
    end

    -- second pass
    for i, joint in ipairs(joints) do
        if joint then
            local next_joint = joints[i + 1]

            if next_joint ~= nil then
                local diff = original_positions[i + 1] - original_positions[i]
                local updated_pos = current_positions[i] + diff

                joint_set_position:call(next_joint, updated_pos)
                joint_set_rotation:call(next_joint, original_rotations[i+1])

                current_positions[i + 1] = updated_pos
            end
        end
    end
end

local function update_hand_ik()
    if not re8.player then return end
    if not vrmod:is_hmd_active() then return end

    local controllers = vrmod:get_controllers()

    if #controllers == 0 then
        re8.was_gripping_weapon = false
        return
    end

    if not re8.left_hand_ik or not re8.right_hand_ik then
        re8.was_gripping_weapon = false
        return
    end

    --if re8.is_in_cutscene then return end
    if not re8.can_use_hands then
        re8.was_gripping_weapon = false
        return 
    end

    if not vrmod:is_using_controllers() then
        re8.was_gripping_weapon = false
        re8.is_holding_left_grip = false
        --[[set_hand_joints_to_tpose(re8.left_hand_ik)
        set_hand_joints_to_tpose(re8.right_hand_ik)
        re8.left_hand_ik:call("calc")
        re8.right_hand_ik:call("calc")]]
        return
    end

    local player = re8.player
    local original_head_rotation = nil
    local head_joint = nil
    local motion = player:call("getComponent(System.Type)", sdk.typeof("via.motion.Motion"))

    local original_right_rot = Quaternion.identity()
    local original_left_rot_relative = Quaternion.identity()
    local original_left_pos_relative = Vector3f.new(0, 0, 0)
    local original_right_rot_relative = Quaternion.identity()
    local original_right_pos_relative = Vector3f.new(0, 0, 0)

    -- the Point of this is to fix the head rotation during cutscenes
    -- because the camera seems to be parented to the head during these events
    -- so modifying the joint when we set the tpose won't cause some extremely jarring movement
    if motion then
        local transform = re8.transform

        if not head_hash then
            head_hash = get_joint_hash(transform, motion, "Head")
        end
    
        head_joint = transform_get_joint_by_hash:call(transform, head_hash)

        if head_joint then
            original_head_rotation = joint_get_rotation:call(head_joint)
        end

        local left_hash = nil
        local right_hash = nil
    
        if is_re7 then
            left_hash = re8.left_hand_ik:get_field("HashJoint2")
            right_hash = re8.right_hand_ik:get_field("HashJoint2")
        elseif is_re8 then
            left_hash = re8.left_hand_ik:get_field("<HashJoint2>k__BackingField")
            right_hash = re8.right_hand_ik:get_field("<HashJoint2>k__BackingField")
        end
        
        
        local left_index = motion_get_joint_index_by_name_hash(motion, left_hash)
        local right_index = motion_get_joint_index_by_name_hash(motion, right_hash)

        local original_left_pos = motion_get_world_position(motion, left_index)
        local original_right_pos = motion_get_world_position(motion, right_index)
        local original_left_rot = motion_get_world_rotation(motion, left_index)
        original_right_rot = motion_get_world_rotation(motion, right_index)

        local right_rot_inverse = original_right_rot:inverse()
        original_left_pos_relative = right_rot_inverse * (original_left_pos - original_right_pos)
        original_left_rot_relative = right_rot_inverse * original_left_rot

        local left_rot_inverse = original_left_rot:inverse()
        original_right_pos_relative = left_rot_inverse * (original_right_pos - original_left_pos)
        original_right_rot_relative = left_rot_inverse * original_right_rot
    end

    local left_controller_transform = vrmod:get_transform(controllers[1])
    local right_controller_transform = vrmod:get_transform(controllers[2])
    local left_controller_rotation = left_controller_transform:to_quat()
    local right_controller_rotation = right_controller_transform:to_quat()

    local hmd_transform = vrmod:get_transform(0)
    --local hmd_rotation = (vrmod:get_rotation_offset() * hmd_transform:to_quat()):normalized()

    local left_controller_offset = left_controller_transform[3] - hmd_transform[3]
    local right_controller_offset = right_controller_transform[3] - hmd_transform[3]

    local camera = sdk.get_primary_camera()
    local camera_rotation = last_camera_matrix:to_quat()

    local original_camera_matrix = camera:call("get_WorldMatrix")
    local original_camera_rotation = original_camera_matrix:to_quat()
    local updated_camera_pos = original_camera_matrix[3]

    vrmod:apply_hmd_transform(original_camera_rotation, updated_camera_pos)

    -- Handles decoupled camera pitch
    original_camera_rotation = (original_camera_rotation * hmd_transform:to_quat():inverse()):normalized()

    local rh_rotation = original_camera_rotation * right_controller_rotation * right_hand_rotation_offset
    local rh_pos = updated_camera_pos
                + ((original_camera_rotation * right_controller_offset) 
                + ((original_camera_rotation * right_controller_rotation):normalized() * right_hand_position_offset))

    rh_pos.w = 1.0

    local lh_grip_position = rh_pos + (rh_rotation:normalized() * original_left_pos_relative)
    lh_grip_position.w = 1.0

    local lh_rotation = original_camera_rotation * left_controller_rotation * left_hand_rotation_offset
    local lh_pos = updated_camera_pos
                + ((original_camera_rotation * left_controller_offset) 
                + ((original_camera_rotation * left_controller_rotation):normalized() * left_hand_position_offset))
    
    lh_pos.w = 1.0

    local lh_delta_to_rh = (lh_pos - rh_pos)
    local lh_grip_delta_to_rh = (lh_grip_position - rh_pos)
    local lh_grip_delta = (lh_grip_position - lh_pos)
    local lh_grip_distance = lh_grip_delta:length()

    re8.was_gripping_weapon = lh_grip_distance <= 0.1 or (re8.was_gripping_weapon and re8.is_holding_left_grip)

    -- Lets the player hold their left hand near the original (grip) position of the weapon
    if re8.was_gripping_weapon and not re8.is_reloading then
        if original_left_pos_relative:length() >= 0.1 then
            local original_grip_rot = lh_grip_delta_to_rh:normalized():to_quat()
            local current_grip_rot = lh_delta_to_rh:normalized():to_quat()

            local grip_rot_delta = (current_grip_rot * original_grip_rot:inverse()):normalized()

            -- Adjust the right hand rotation
            rh_rotation = (grip_rot_delta * rh_rotation):normalized()

            -- Adjust the grip position
            lh_grip_position = rh_pos + (rh_rotation * original_left_pos_relative)
            lh_grip_position.w = 1.0
        end

        -- Set the left hand position and rotation to the grip position
        lh_pos = lh_grip_position
        lh_rotation = rh_rotation * original_left_rot_relative
    else
        if re8.is_reloading then
            lh_pos = lh_grip_position
            lh_rotation = rh_rotation * original_left_rot_relative
        else
            lh_pos = lh_pos
            lh_rotation = lh_rotation
        end
    end

    last_left_hand_position = lh_pos:clone()
    last_left_hand_rotation = lh_rotation:clone()

    set_hand_joints_to_tpose(re8.left_hand_ik)

    re8.left_hand_ik_transform:set_position(lh_pos)
    re8.left_hand_ik_transform:set_rotation(lh_rotation)
    re8.left_hand_ik:set_field("Transition", 1.0)
    re8.left_hand_ik:call("calc")

    set_hand_joints_to_tpose(re8.right_hand_ik)

    last_right_hand_position = rh_pos:clone()
    last_right_hand_rotation = rh_rotation:clone()
    last_right_hand_position.w = 1.0

    last_original_right_hand_rotation = (last_right_hand_rotation * right_hand_rotation_offset:inverse()):normalized()

    re8.right_hand_ik_transform:set_position(rh_pos)
    re8.right_hand_ik_transform:set_rotation(rh_rotation)
    re8.right_hand_ik:set_field("Transition", 1.0)
    re8.right_hand_ik:call("calc")

    if head_joint ~= nil and original_head_rotation ~= nil then
        joint_set_rotation:call(head_joint, original_head_rotation)
    end
end

local last_real_camera_rotation = Quaternion.new(1, 0, 0, 0)
local last_real_camera_joint_rotation = Quaternion.new(1, 0, 0, 0)
local last_real_camera_joint_pos = Vector3f.new(0, 0, 0)

local neg_identity = Matrix4x4f.new(-1, 0, 0, 0,
                                    0, -1, 0, 0,
                                    0, 0, -1, 0,
                                    0, 0, 0, -1):to_quat()

local center_hash = nil
local chest_hash = nil

local known_hashes = {}

local function get_joint_hash(transform, motion, name)
    if not known_hashes[transform] then
        known_hashes[transform] = {}
    end

    local hash = known_hashes[transform][name]

    if hash then
        return hash
    end

    local joint = transform:call("getJointByName", name)

    if not joint then
        log.debug("Failed to get " .. name .. " joint")
        return nil
    end

    hash = joint:call("get_NameHash")
    log.info(name .. " hash: " .. string.format("%x", hash))

    known_hashes[transform][name] = hash

    return hash
end

local zero_vec = Vector3f.new(0, 0, 0)

local function update_body_ik(camera_rotation, camera_pos)
    if not re8.player then return end

    local player = re8.player
    local ik_leg = player:call("getComponent(System.Type)", sdk.typeof("via.motion.IkLeg"))

    if not ik_leg then
        if not vrmod:is_using_controllers() or re8.is_in_cutscene then
            return
        end

        ik_leg = player:call("createComponent(System.Type)", sdk.typeof("via.motion.IkLeg"))

        if not ik_leg then
            log.error("Failed to create IK leg component")
            return
        end
    end

    if re8.is_in_cutscene or not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        --ik_leg:call("set_Enabled", false)
        ik_leg:call("set_CenterOffset", Vector3f.new(0, 0, 0))
        ik_leg:call("setCenterAdjust", 0)
        ik_leg:call("set_CenterPositionCtrl", 2) -- world offset
        ik_leg:call("set_GroundContactUpDistance", 0.0) -- Fixes the whole player being jarringly moved upwards.

        if not vrmod:is_using_controllers() then
            ik_leg:call("destroy", ik_leg)
        end
        
        return
    else
        ik_leg:call("set_Enabled", true)
    end

    local motion = player:call("getComponent(System.Type)", sdk.typeof("via.motion.Motion"))

    if not motion then
        log.error("Failed to get motion component")
        return
    end

    local transform = player:call("get_Transform")

    if not head_hash then
        head_hash = get_joint_hash(transform, motion, "Head")
    end

    --[[if not chest_hash then
        chest_hash = get_joint_hash(transform, motion, "Chest")
    end]]

    if not center_hash then
        center_hash = get_joint_hash(transform, motion, "Hip")
    end

    local transform_rot = transform:call("get_Rotation")
    local transform_pos = transform:call("get_Position")

    local head_joint = transform_get_joint_by_hash:call(transform, head_hash)
    --local chest_joint = transform:call("getJointByHash", chest_hash)
    --local hip_joint = transform:call("getJointByHash", center_hash)

    --local head_index = motion:call("getJointIndexByNameHash", head_hash)
    --chest_index = motion:call("getJointIndexByNameHash", chest_hash)
    --local original_head_pos = motion:call("getWorldPosition", head_index)
    --local original_chest_pos = motion:call("getWorldPosition", chest_index)

    local normal_dir = camera_rotation * Vector3f.new(0, 0, 1)
    local flattened_dir = camera_rotation * Vector3f.new(0, 0, 1)
    flattened_dir.y = 0.0
    flattened_dir:normalize()

    local original_head_pos = calculate_tpose_world(head_joint, 4) + (flattened_dir * (math.abs(normal_dir.y) * -0.1)) + (flattened_dir * 0.025)

    --[[if not vrmod:is_using_controllers() then
        original_head_pos = joint_get_position:call(head_joint) + (flattened_dir * (math.abs(normal_dir.y) * -0.1)) + (flattened_dir * 0.025)
    end]]

    --original_head_pos = transform_rot * original_head_pos
    --original_head_pos = transform:call("getJointByName", "root"):call("get_Rotation") * original_head_pos
    --original_head_pos = transform_rot * transform:calculate_base_transform(head_joint)[3]
    --original_chest_pos = transform_rot * original_chest_pos

    --original_head_pos.x = original_chest_pos.x
    --original_head_pos.z = original_chest_pos.z

    --[[local center_index = motion:call("getJointIndexByNameHash", center_hash)
    local original_center_pos = motion:call("getWorldPosition", center_index)

    original_center_pos = transform_rot * original_center_pos

    local current_center_pos = transform:call("getJointByHash", center_hash):call("get_Position")
    local center_diff = (original_center_pos * -1.0)
    center_diff.y = 0.0]]

    --local current_head_pos = transform:call("getJointByName", "Head"):call("get_Position")

    --[[local center_joint = transform:call("getJointByName", "Hip")

    local center_pos = center_joint:call("get_Position")
    local transform_pos = transform:call("get_Position")]]

    local diff_to_camera = ((camera_pos) - original_head_pos)

    --ik_leg:call("set_CenterJointName", "Hip")
    ik_leg:call("set_CenterOffset", diff_to_camera)
    ik_leg:call("setCenterAdjust", 0)
    ik_leg:call("set_CenterPositionCtrl", 2) -- world offset
    ik_leg:call("set_GroundContactUpDistance", 0.0) -- Fixes the whole player being jarringly moved upwards.
    --ik_leg:call("set_UpdateTiming", 2) -- ConstraintsBegin
end

local function on_pre_shoot(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    local weapon = sdk.to_managed_object(args[2])

    -- this happens in RE7 with the turrets.
    if weapon ~= re8.weapon then
        return
    end

    update_hand_ik()

    local ray = args[3]

   --[[if is_re7 then
        local muzzle_joint = weapon:call("get_muzzleJoint")

        if muzzle_joint then
            local muzzle_pos = joint_get_position:call(muzzle_joint)
            local muzzle_forward = joint_get_rotation:call(muzzle_joint) * Vector3f.new(0, 0, 1)

            local pos = muzzle_pos + (muzzle_forward * 0.01)
            local from = Vector4f.new(pos.x, pos.y, pos.z, 1.0)
            local dir = Vector4f.new(forward.x, forward.y, forward.z, 0.0)

            -- nudge the start position slightly forward because
            -- apparently the bullets can collide with the weapon.... wtf
            sdk.set_native_field(ray, ray_typedef, "from", from)
            sdk.set_native_field(ray, ray_typedef, "dir", dir)
        else
            log.info("No muzzle joint found")
        end
    elseif is_re8 then]]
        local pos = last_muzzle_pos + (last_muzzle_forward * 0.02)
        local from = Vector4f.new(pos.x, pos.y, pos.z, 1.0)
        local dir = Vector4f.new(last_muzzle_forward.x, last_muzzle_forward.y, last_muzzle_forward.z, 1.0)

        sdk.set_native_field(ray, ray_typedef, "from", from)
        sdk.set_native_field(ray, ray_typedef, "dir", dir)
    --end

    --sdk.call_native_func(ray, ray_typedef, ".ctor(via.vec3, via.vec3)", last_muzzle_pos, last_muzzle_forward)
end

local function on_post_shoot(retval)
    return retval
end

if is_re7 then
    sdk.hook(sdk.find_type_definition("app.WeaponGun"):get_method("shoot"), on_pre_shoot, on_post_shoot)
elseif is_re8 then
    sdk.hook(sdk.find_type_definition("app.WeaponGunCore"):get_method("shoot"), on_pre_shoot, on_post_shoot)
end

local throwable_was_right_grip_down = false
local throw_ray = ValueType.new(sdk.find_type_definition("via.Ray"))
local inside_throw = false
local threw_bomb = false
local last_throwable_update = os.clock()

local function on_pre_throwable_late_update(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then return end

    local weapon = sdk.to_managed_object(args[2])
    if weapon ~= re8.weapon then return end

    if os.clock() - last_throwable_update > 1.0 then
        throwable_was_right_grip_down = false
    end

    last_throwable_update = os.clock()

    local action_grip = vrmod:get_action_grip()
    local right_joystick = vrmod:get_right_joystick()
    --local left_joystick = vrmod:get_left_joystick()
    local is_grip_down = vrmod:is_action_active(action_grip, right_joystick)

    if not is_grip_down and throwable_was_right_grip_down then
        local vel_norm = Vector3f.new(0.0, 1.0, 0.0):normalized()
        local from = Vector3f.new(last_right_hand_position.x, last_right_hand_position.y, last_right_hand_position.z)

        from = Vector4f.new(from.x, from.y, from.z, 1.0)
        vel_norm = Vector4f.new(vel_norm.x, vel_norm.y, vel_norm.z, 1.0)

        -- some BS to just throw it
        -- we will fix it inside on_post_bomb_activate_throwable
        -- by modifying the rigid body's velocity
        throw_ray:set_field("from", from)
        throw_ray:set_field("dir", vel_norm)

        inside_throw = true
        pcall(weapon.call, weapon, "throwWeapon", throw_ray)
        inside_throw = false

        local inventory = re8.updater:get_field("References"):call("get_inventory")

        -- Decrement the grenade count
        if threw_bomb and inventory ~= nil then
            local work = re8.weapon:call("get_work")

            if work ~= nil then
                inventory:call("reduceItem(app.ItemCore.InstanceWork, System.Int32, System.Boolean)", work, 1, false)
            end
        end

        threw_bomb = false
    end

    throwable_was_right_grip_down = is_grip_down
end

local function on_post_throwable_late_update(retval)
    return retval
end

local bomb_args = nil

local function on_pre_bomb_activate_throwable(args)
    if not inside_throw then return end

    bomb_args = args
end

local function on_post_bomb_activate_throwable(retval)
    if not inside_throw or bomb_args == nil then
        return retval
    end

    local bomb = sdk.to_managed_object(bomb_args[2])
    local physics_rigid_body = bomb:get_field("<rigidBodySet>k__BackingField")

    if physics_rigid_body == nil then
        return retval
    end

    local rigid_body = physics_rigid_body:get_field("RigidBodySet")

    if rigid_body == nil then
        return retval
    end

    -- Physical throwing logic
    local controllers = vrmod:get_controllers()
    local right_velocity = vrmod:get_velocity(controllers[2])
    local right_angular_velocity = vrmod:get_angular_velocity(controllers[2])

    local original_camera_rotation = last_camera_matrix:to_quat()

    local hmd_transform = vrmod:get_transform(0)
    local hmd_rotation = hmd_transform:to_quat()

    local rotation = (original_camera_rotation * hmd_rotation:inverse()):normalized()
    right_velocity = rotation * right_velocity

    rigid_body:call("setLinearVelocity", 0, right_velocity)
    rigid_body:call("setAngularVelocity", 0, rotation * right_angular_velocity)

    bomb_args = nil
    threw_bomb = true

    return retval
end

if is_re7 then
    --sdk.hook(sdk.find_type_definition("app.WeaponThrowable"):get_method("lateUpdate"), on_pre_throwable_late_update, on_post_throwable_late_update)
else
    sdk.hook(sdk.find_type_definition("app.WeaponThrowableCore"):get_method("lateUpdate"), on_pre_throwable_late_update, on_post_throwable_late_update)
    sdk.hook(sdk.find_type_definition("app.BombDefault"):get_method("activateThrowable"), on_pre_bomb_activate_throwable, on_post_bomb_activate_throwable)
end

local old_camera_rot = nil
local old_camera_pos = nil

-- let player look at interaction elements with the camera
local function on_pre_interact_manager_lateupdate(args)
    if not vrmod:is_hmd_active() then
        return
    end

    local camera = sdk.get_primary_camera()
    local camera_gameobject = component_get_gameobject:call(camera)
    local camera_transform = gameobject_get_transform:call(camera_gameobject)

    local joint = transform_get_joints:call(camera_transform)[0]

    old_camera_rot = joint_get_rotation:call(joint)
    old_camera_pos = joint_get_position:call(joint)

    joint_set_rotation:call(joint, last_camera_matrix:to_quat())
    joint_set_position:call(joint, last_camera_matrix[3])
end

local function on_post_interact_manager_lateupdate(retval)
    if not vrmod:is_hmd_active() then
        return
    end

    local camera = sdk.get_primary_camera()
    local camera_gameobject = component_get_gameobject:call(camera)
    local camera_transform = gameobject_get_transform:call(camera_gameobject)

    local joint = transform_get_joints:call(camera_transform)[0]

    joint_set_rotation:call(joint, old_camera_rot)
    joint_set_position:call(joint, old_camera_pos)

    return retval
end

sdk.hook(
    sdk.find_type_definition("app.InteractManager"):get_method("doLateUpdate"), 
    on_pre_interact_manager_lateupdate, 
    on_post_interact_manager_lateupdate
)

-- force the gui to recenter when opening the inventory
if is_re7 then
    sdk.hook(
        sdk.find_type_definition("app.MenuManager"):get_method("openInventoryMenu"),
        function(args)
            last_gui_forced_slerp = os.clock()
        end,
        function(retval)
            return retval
        end
    )
elseif is_re8 then
    sdk.hook(
        sdk.find_type_definition("app.GUIInventory"):get_method("openInventory"),
        function(args)
            last_gui_forced_slerp = os.clock()
        end,
        function(retval)
            return retval
        end
    )
end

local crosshair_bullet_ray_result = nil
local crosshair_attack_ray_result = nil

local function cast_ray(start_pos, end_pos, layer)
    if layer == nil then
        layer = CollisionLayer.Bullet
    end

    local via_physics_system = sdk.get_native_singleton("via.physics.System")
	local ray_query = sdk.create_instance("via.physics.CastRayQuery")
    local ray_result = sdk.create_instance("via.physics.CastRayResult")

    ray_query:call("setRay(via.vec3, via.vec3)", start_pos, end_pos)
	ray_query:call("clearOptions")
	ray_query:call("enableAllHits")
	ray_query:call("enableNearSort")
	--ray_query:call("enableFrontFacingTriangleHits")
	--ray_query:call("disableBackFacingTriangleHits")
    local filter_info = ray_query:call("get_FilterInfo")
	filter_info:call("set_Group", 0)
	filter_info:call("set_MaskBits", 0xFFFFFFFF & ~1) -- everything except the player.

    filter_info:call("set_Layer", layer)
    ray_query:call("set_FilterInfo", filter_info)
    cast_ray_method:call(via_physics_system, ray_query, ray_result)

	return ray_result
end

local function cast_ray_async(ray_result, start_pos, end_pos, layer)
    if layer == nil then
        layer = CollisionLayer.Bullet
    end

    local via_physics_system = sdk.get_native_singleton("via.physics.System")
	local ray_query = sdk.create_instance("via.physics.CastRayQuery")
    local ray_result = ray_result or sdk.create_instance("via.physics.CastRayResult")

    ray_query:call("setRay(via.vec3, via.vec3)", start_pos, end_pos)
	ray_query:call("clearOptions")
	ray_query:call("enableAllHits")
	ray_query:call("enableNearSort")
	--ray_query:call("enableFrontFacingTriangleHits")
	--ray_query:call("disableBackFacingTriangleHits")
    local filter_info = ray_query:call("get_FilterInfo")
	filter_info:call("set_Group", 0)
	filter_info:call("set_MaskBits", 0xFFFFFFFF & ~1) -- everything except the player.

    filter_info:call("set_Layer", layer)
    ray_query:call("set_FilterInfo", filter_info)
    cast_ray_async_method:call(via_physics_system, ray_query, ray_result)

	return ray_result
end

local function update_crosshair_world_pos(start_pos, end_pos)
    if not vrmod:is_hmd_active() then return end

    if os.clock() - last_scope_time > 0.2 then
        if vrmod:is_using_controllers() and cfg.disable_crosshair then
            return
        end
    end
    
    -- asynchronous raycast
    if crosshair_attack_ray_result == nil or crosshair_bullet_ray_result == nil then
        crosshair_attack_ray_result = cast_ray_async(crosshair_ray_result, start_pos, end_pos, CollisionLayer.Attack)
        crosshair_bullet_ray_result = cast_ray_async(crosshair_ray_result, start_pos, end_pos, CollisionLayer.Bullet)
        crosshair_attack_ray_result:add_ref()
        crosshair_bullet_ray_result:add_ref()
    end

    local finished = crosshair_attack_ray_result:call("get_Finished") == true and crosshair_bullet_ray_result:call("get_Finished")
    local attack_hit = finished and crosshair_attack_ray_result:call("get_NumContactPoints") > 0
    local any_hit = finished and (attack_hit or crosshair_bullet_ray_result:call("get_NumContactPoints") > 0)

    if finished and any_hit then
        local best_result = attack_hit and crosshair_attack_ray_result or crosshair_bullet_ray_result
        local contact_point = best_result:call("getContactPoint(System.UInt32)", 0)

        if contact_point then
            re8.crosshair_dir = (end_pos - start_pos):normalized()
            re8.crosshair_normal = contact_point:get_field("Normal")
            re8.crosshair_distance = contact_point:get_field("Distance")

            --re8.crosshair_pos = contact_point:get_field("Position") -- We don't use the position because the cast was asynchronous
            -- instead we get the distance to the impact and add it to the current position
            re8.crosshair_pos = start_pos + (re8.crosshair_dir * re8.crosshair_distance)
        end
    else
        re8.crosshair_dir = (end_pos - start_pos):normalized()

        if re8.crosshair_distance then
            re8.crosshair_pos = start_pos + (re8.crosshair_dir * re8.crosshair_distance)
        else
            re8.crosshair_pos = start_pos + (re8.crosshair_dir * 10.0)
            re8.crosshair_distance = 10.0
        end
    end
    
    if finished then
        -- restart it.
        cast_ray_async(crosshair_attack_ray_result, start_pos, end_pos, CollisionLayer.Attack)
        cast_ray_async(crosshair_bullet_ray_result, start_pos, end_pos, CollisionLayer.Bullet)
    end
end

local last_camera_update_args = nil
local last_cutscene_state = false
local last_time_not_maximum_controllable = 0.0
local GUI_MAX_SLERP_TIME = 1.5

local function slerp_gui(new_gui_quat)
    if re8.movement_speed_rate > 0.0 then
        last_gui_forced_slerp = os.clock() - ((1.0 - re8.movement_speed_rate))
    end

    last_gui_dot = last_gui_quat:dot(new_gui_quat)
    local dot_dist = 1.0 - math.abs(last_gui_dot)
    local dot_ang = math.acos(math.abs(last_gui_dot)) * (180.0 / math.pi)
    last_gui_dot = dot_ang

    local now = os.clock()

    -- trigger gui slerp
    if dot_ang >= 20 or re8.is_in_cutscene then
        last_gui_forced_slerp = now
    end

    local slerp_time_diff = now - last_gui_forced_slerp

    if slerp_time_diff <= GUI_MAX_SLERP_TIME then
        if dot_ang >= 10 then
            last_gui_forced_slerp = now
        end

        last_gui_quat = last_gui_quat:slerp(new_gui_quat, dot_dist * math.max((GUI_MAX_SLERP_TIME - slerp_time_diff) * re8.delta_time, 0.0))
    end

    if re8.is_in_cutscene then
        vrmod:recenter_gui(last_gui_quat)
    else
        vrmod:recenter_gui(last_gui_quat * new_gui_quat:inverse())
    end
end

local last_hmd_active_state = false
local wants_posture_param_restore = false
local modified_posture_param = nil
local original_posture_camera_offset = Vector3f.new(0.0, 0.0, 0.0)

local function pre_fix_player_camera(player_camera)
end

local function fix_player_camera(player_camera)
    if not vrmod:is_hmd_active() then
        -- so the camera doesnt go wacky
        if last_hmd_active_state then
            -- disables the body IK component
            update_body_ik(nil, nil)

            vrmod:set_gui_rotation_offset(Quaternion.identity())
            vrmod:recenter_view()
    
            last_hmd_active_state = false
        end

        -- Restore the vertical camera movement after taking headset off/not using controllers
        if was_vert_limited then
            --local player_camera = sdk.to_managed_object(args[2])
            local base_transform_solver = player_camera:get_field("BaseTransSolver")

            if base_transform_solver then
                local camera_controller = base_transform_solver:get_field("CurrentController")

                if not camera_controller then
                    camera_controller = base_transform_solver:get_field("<CurrentController>k__BackingField")
                end

                -- Stop the player from rotating the camera vertically
                if camera_controller then
                    camera_controller:set_field("IsVerticalRotateLimited", false)
                end
            end

            was_vert_limited = false
        end

        last_real_camera_rotation = nil
        last_real_camera_joint_rotation = nil

        return retval
    end

    re8.upper_body_transform_rate = player_camera:get_field("<upperBodyTransformRate>k__BackingField")

    last_hmd_active_state = true

    local base_transform_solver = player_camera:get_field("BaseTransSolver")
    local is_maximum_controllable = true

    if base_transform_solver then
        local current_type = base_transform_solver:get_field("<currentType>k__BackingField")

        if is_re8 then
            current_type = current_type:get_field("Value")
        end

        if current_type ~= 0 then -- MaximumOperatable
            re8.is_in_cutscene = true
            is_maximum_controllable = false
            last_time_not_maximum_controllable = os.clock()
        else
            if os.clock() - last_time_not_maximum_controllable <= 1.0 then
                re8.is_in_cutscene = true
            end
        end
    end

    local wants_recenter = false

    if re8.is_in_cutscene and not last_cutscene_state then
        --vrmod:recenter_view()

        -- force the gui to be recentered when we exit the cutscene
        last_gui_forced_slerp = os.clock()
        last_gui_quat = Quaternion.identity()
        wants_recenter = true
        --queue_recenter = true

        vrmod:recenter_gui(last_gui_quat)
    elseif not re8.is_in_cutscene and last_cutscene_state then
        last_gui_forced_slerp = os.clock()
        last_gui_quat = vrmod:get_rotation(0):to_quat():inverse()
        wants_recenter = true

        vrmod:recenter_gui(vrmod:get_rotation(0):to_quat())
    end

    local camera = sdk.get_primary_camera()

    -- apply the camera rot to the real camera
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    local camera_rot = transform_get_rotation:call(camera_transform)
    local camera_pos = transform_get_position:call(camera_transform)

    -- fix camera position.
    if is_maximum_controllable and vrmod:is_using_controllers() then
        local param_container = player_camera:get_field("_CurrentParamContainer")

        if param_container == nil then
            param_container = player_camera:get_field("CurrentParamContainer")
        end

        if param_container ~= nil then
            local posture_param = param_container:get_field("PostureParam")

            if posture_param ~= nil then
                local current_camera_offset = posture_param:get_field("CameraOffset")

                current_camera_offset.y = 0.0
                camera_pos = camera_pos + (camera_rot * current_camera_offset)
            end
        end
    end

    local camera_rot_pre_hmd = camera_rot:clone()
    local camera_pos_pre_hmd = camera_pos:clone()

    -- So the camera doesn't spin uncontrollably when attacking or the camera moves outside of player control.
    local camera_rot_no_shake = player_camera:get_field("<CameraRotation>k__BackingField")

    vrmod:apply_hmd_transform(camera_rot_no_shake, Vector3f.new(0, 0, 0))
    vrmod:apply_hmd_transform(camera_rot, camera_pos)
    
    local camera_joint = camera_transform:call("get_Joints")[0]

    -- Transform is used for things like Ethan's light
    -- and determining where the player is looking
    transform_set_position:call(camera_transform, camera_pos)
    transform_set_rotation:call(camera_transform, camera_rot)
    
    last_real_camera_joint_rotation = camera_rot_pre_hmd
    last_real_camera_joint_pos = camera_pos_pre_hmd

    -- Joint is used for the actual final rendering of the game world
    --if not wants_recenter then
    if re8.is_in_cutscene then
        joint_set_position:call(camera_joint, camera_pos_pre_hmd)
        joint_set_rotation:call(camera_joint, camera_rot_pre_hmd)
    else
        local rot_delta = camera_rot_pre_hmd:inverse() * camera_rot

        local forward = rot_delta * Vector3f.new(0, 0, 1)
        forward = Vector3f.new(forward.x, 0.0, forward.z):normalized()

        joint_set_position:call(camera_joint, camera_pos_pre_hmd)
        joint_set_rotation:call(camera_joint, camera_rot_pre_hmd * forward:to_quat())
    end

    --last_gui_offset = last_gui_offset * (camera_rot:inverse() * camera_rot_pre_hmd)

    -- just update the body IK right after we update the camera.
    update_body_ik(camera_rot, camera_pos)

    -- Slerp the gui around
    slerp_gui(re8.is_in_cutscene and (camera_rot_pre_hmd * camera_rot:inverse()) or vrmod:get_rotation(0):to_quat():inverse())

    local fixed_dir = ((neg_forward_identity * camera_rot_no_shake) * Vector3f.new(0, 0, -1)):normalized()
    local fixed_rot = fixed_dir:to_quat()
    --local fixed_rot = neg_forward_identity * camera_rot

    player_camera:set_field("<CameraRotation>k__BackingField", fixed_rot)
    player_camera:set_field("<CameraPosition>k__BackingField", camera_pos)

    if is_re8 then
        player_camera:set_field("FixedAimRotation", fixed_rot) -- RE8
    end

    player_camera:set_field("CameraRotationWithMovementShake", fixed_rot)
    --player_camera:set_field("CameraPositionWithMovementShake", camera_pos)
    player_camera:set_field("CameraRotationWithCameraShake", fixed_rot)
    --player_camera:set_field("CameraPositionWithCameraShake", camera_pos)
    player_camera:set_field("PrevCameraRotation", fixed_rot)
    --player_camera:set_field("OldCameraRotation", fixed_rot)
    --player_camera:set_field("InterpRotationStart", fixed_rot)
    --player_camera:set_field("<DesiredCameraRot>k__BackingField", fixed_rot)
    --player_camera:set_field("<OperatedCameraRot>k__BackingField", fixed_rot)

    local camera_controller_param = player_camera:get_field("CameraCtrlParam")

    if camera_controller_param then
        camera_controller_param:set_field("CameraRotation", fixed_rot)
    end

    local base_transform_solver = player_camera:get_field("BaseTransSolver")

    if base_transform_solver then
        local camera_controller = base_transform_solver:get_field("CurrentController")

        if not camera_controller then
            camera_controller = base_transform_solver:get_field("<CurrentController>k__BackingField")
        end

        -- Stop the player from rotating the camera vertically
        if camera_controller then
            local camera_controller_rot = Quaternion.identity()

            if re8.is_in_cutscene then
                if is_re7 then
                    camera_controller_rot = camera_controller:get_field("<rotation>k__BackingField")
                elseif is_re8 then
                    camera_controller_rot = camera_controller:get_field("<Rotation>k__BackingField")
                end
            else
                camera_controller_rot = Quaternion.new(fixed_rot.w, fixed_rot.x, fixed_rot.y, fixed_rot.z)
            end

            local controller_forward = camera_controller_rot * Vector3f.new(0.0, 0.0, 1.0)
            controller_forward.y = 0.0  
            camera_controller_rot = controller_forward:normalized():to_quat()
            
            --if wants_recenter or not re8.is_in_cutscene then
            if not re8.is_in_cutscene or is_maximum_controllable then
                if not re8.is_in_cutscene then
                    vrmod:recenter_view()
                end
                
                if is_re7 then
                    camera_controller:set_field("<rotation>k__BackingField", camera_controller_rot)
                elseif is_re8 then
                    camera_controller:set_field("<Rotation>k__BackingField", camera_controller_rot)
                end
            end

            base_transform_solver:set_field("<rotation>k__BackingField", camera_controller_rot)

            --[[for i, controller in ipairs(base_transform_solver:get_field("CameraControllers"):get_elements()) do
                controller:set_field("<rotation>k__BackingField", camera_controller_rot)
                controller:set_field("RelativeCamRotAtEndOfMotion", camera_controller_rot)
            end]]
            
            --[[local maximum_operatable_controller = base_transform_solver:get_field("CameraControllers")[0]

            if maximum_operatable_controller ~= camera_controller then
                maximum_operatable_controller:set_field("<rotation>k__BackingField", camera_controller_rot)
            end]]
        
            camera_controller:set_field("IsVerticalRotateLimited", is_maximum_controllable)
            was_vert_limited = true
        end
    end

    -- stops the camera from pivoting around the player
    -- so we can use VR to look around without the body sticking out
    --if vrmod:is_using_controllers() then
    --[[if not re8.is_in_cutscene then
        local param_container = player_camera:get_field("_CurrentParamContainer")

        if param_container == nil then
            param_container = player_camera:get_field("CurrentParamContainer")
        end

        if param_container ~= nil then
            local posture_param = param_container:get_field("PostureParam")

            if posture_param ~= nil then
                local current_camera_offset = posture_param:get_field("CameraOffset")
                current_camera_offset.x = 0.0
                current_camera_offset.z = 0.0

                posture_param:set_field("CameraOffset", current_camera_offset)
            end
        end
    end]]

    local look_ray_offset = player_camera:get_type_definition():get_field("LookRay"):get_offset_from_base()
    local shoot_ray_offset = player_camera:get_type_definition():get_field("ShootRay"):get_offset_from_base()
    local look_ray = player_camera:get_address() + look_ray_offset
    local shoot_ray = player_camera:get_address() + shoot_ray_offset
    
    sdk.set_native_field(sdk.to_ptr(look_ray), ray_typedef, "from", camera_pos)
    sdk.set_native_field(sdk.to_ptr(look_ray), ray_typedef, "dir", fixed_dir)

    if vrmod:is_using_controllers() then
        local pos = last_muzzle_pos + (last_muzzle_forward * 0.02)
        --local scooted_pos = last_muzzle_pos - (last_muzzle_forward * 2)
        --local scooted_from = Vector4f.new(scooted_pos.x, scooted_pos.y, scooted_pos.z, 1.0)
        local from = Vector4f.new(pos.x, pos.y, pos.z, 1.0)
        local dir = Vector4f.new(last_muzzle_forward.x, last_muzzle_forward.y, last_muzzle_forward.z, 1.0)

        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "from", from)
        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "dir", dir)

        -- called in LockScene
        --update_crosshair_world_pos(pos, pos + (last_muzzle_forward * 1000.0))
    else
        last_shoot_pos = camera_pos
        last_shoot_dir = fixed_dir

        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "from", camera_pos)
        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "dir", fixed_dir)

        -- called in LockScene
        --update_crosshair_world_pos(camera_pos, camera_pos + (fixed_dir * 1000.0))
    end

    last_cutscene_state = re8.is_in_cutscene
end

local function on_pre_player_camera_update(args)
    last_camera_update_args = args

    if not vrmod:is_hmd_active() then
        return
    end

    pre_fix_player_camera(sdk.to_managed_object(args[2]))

    --local player_camera = sdk.to_managed_object(args[2])
    --fix_player_camera(player_camera)

    --[[local player_camera = sdk.to_managed_object(args[2])

    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")]]
    --last_real_camera_rotation = camera_transform:call("get_Rotation")
    --last_real_camera_joint_rotation = camera_transform:call("get_Joints")[0]:call("get_Rotation")

    --return sdk.PreHookResult.SKIP_ORIGINAL
end

local function on_post_player_camera_update(retval)
    local args = last_camera_update_args

    local player_camera = sdk.to_managed_object(args[2])
    fix_player_camera(player_camera)

    return retval
end

local function on_pre_player_interp_rotation(args)
    local player_camera = sdk.to_managed_object(args[2])
    
    local camera = sdk.get_primary_camera()
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    re8.is_in_cutscene = true
    fix_player_camera(player_camera)
end

local function on_post_player_interp_rotation(retval)
    return retval
end

-- Normal Ethan camera
--[[sdk.hook(
    sdk.find_type_definition("app.PlayerCamera"):get_method("interpRotation"), 
    on_pre_player_interp_rotation, 
    on_post_player_interp_rotation
)]]

-- Normal Ethan camera
sdk.hook(
    sdk.find_type_definition("app.PlayerCamera"):get_method("lateUpdate"), 
    on_pre_player_camera_update, 
    on_post_player_camera_update
)

if is_re7 then
    -- Not a hero camera
    sdk.hook(
        sdk.find_type_definition("app.CH8PlayerCamera"):get_method("lateUpdate"), 
        on_pre_player_camera_update, 
        on_post_player_camera_update
    )

    -- idk the other DLC?
    sdk.hook(
        sdk.find_type_definition("app.CH9PlayerCamera"):get_method("lateUpdate"), 
        on_pre_player_camera_update, 
        on_post_player_camera_update
    )
end

-- Zero out the camera shake
sdk.hook(
    sdk.find_type_definition("app.PlayerCamera"):get_method("updateCameraShakeValue"), 
    function(args)
        if not cfg.all_camera_shake then
            return sdk.PreHookResult.SKIP_ORIGINAL
        end
    end, 
    function(retval)
        if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
            return retval
        end

        local args = last_camera_update_args
        if args == nil then return retval end

        local player_camera = sdk.to_managed_object(args[2])

        local zero_quat = Quaternion.new(1, 0, 0, 0)
        local zero_vec = Vector3f.new(0, 0, 0)
    
        if not cfg.movement_shake then
            player_camera:set_field("MovementShakePosition", zero_vec)
            player_camera:set_field("MovementShakeRotation", zero_quat)
        end

        return retval
    end
)

local function on_pre_upper_vertical_update(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    --[[local upper_vertical = sdk.to_managed_object(args[2])

    if not last_camera_update_args then return end
    local player_camera = sdk.to_managed_object(last_camera_update_args[2])

    local camera_rot = player_camera:get_field("<CameraRotation>k__BackingField")
    local camera_pos = player_camera:get_field("<CameraPosition>k__BackingField")

    vrmod:apply_hmd_transform(camera_rot, camera_pos)

    player_camera:set_field("<CameraRotation>k__BackingField", camera_rot)
    player_camera:set_field("<CameraPosition>k__BackingField", camera_pos)

    player_camera:set_field("CameraRotationWithMovementShake", camera_rot)
    player_camera:set_field("CameraPositionWithMovementShake", camera_pos)
    player_camera:set_field("CameraRotationWithCameraShake", camera_rot)
    player_camera:set_field("CameraPositionWithCameraShake", camera_pos)]]
    --player_camera:set_field("OtherShakeRotation", camera_rot)
    --player_camera:set_field("OtherShakePosition", camera_pos)
end

local function on_post_upper_vertical_update(retval)
    return retval
end

if is_re7 then
    sdk.hook(
        sdk.find_type_definition("app.PlayerUpperVerticalRotate"):get_method("doLateUpdate"), 
        on_pre_upper_vertical_update, 
        on_post_upper_vertical_update
    )
elseif is_re8 then
    sdk.hook(
        sdk.find_type_definition("app.PlayerUpperVerticalRotate"):get_method("lateUpdate"), 
        on_pre_upper_vertical_update, 
        on_post_upper_vertical_update
    )
end

local function check_player_hands_up()
    local player = re8.player

    if not player then 
        re8.wants_block = false
        return 
    end

    local right_hand_up = false
    local left_hand_up = false


    local controllers = vrmod:get_controllers()
    if #controllers < 2 then
        re8.wants_block = false
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
        re8.wants_block = false
        return
    end

    -- now we need to check if the hands are facing up
    local left_hand_up_dot = math.abs(hmd_forward:dot(left_hand[0]))
    local right_hand_up_dot = math.abs(hmd_forward:dot(right_hand[0]))

    left_hand_up = left_hand_up_dot >= 0.5
    right_hand_up = right_hand_up_dot >= 0.5

    re8.wants_block = left_hand_up and right_hand_up

    --log.info("left hand dot: " .. tostring(left_hand_dot))
    --log.info("right hand dot: " .. tostring(right_hand_dot))
end

local should_reset_view_no_player = false

re.on_pre_application_entry("UpdateBehavior", function()
    check_player_hands_up()

    if not re8.player then
        if should_reset_view_no_player then
            vrmod:recenter_view()
            vrmod:set_gui_rotation_offset(Quaternion.identity())
            should_reset_view_no_player = false
        end
    else
        should_reset_view_no_player = true
    end
end)

re.on_pre_application_entry("PrepareRendering", function()
    update_hand_ik()
end)

re.on_application_entry("UpdateMotion", function()
    --[[local camera = sdk.get_primary_camera()

    -- apply the camera rot to the real camera
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")

    local camera_rot = camera_transform:call("get_Rotation")
    local camera_pos = camera_transform:call("get_Position")

    local camera_rot_pre_hmd = Quaternion.new(camera_rot.w, camera_rot.x, camera_rot.y, camera_rot.z)

    vrmod:apply_hmd_transform(neg_forward_identity * camera_rot, camera_pos)

    update_body_ik(camera_rot_pre_hmd, camera_pos)]]

    update_hand_ik()
end)

--[[re.on_pre_application_entry("LateUpdateBehavior", function()
    update_hand_ik()
end)]]

re.on_application_entry("LateUpdateBehavior", function()
    update_hand_ik()
end)

re.on_application_entry("UpdateHID", function()
    --[[local padman = sdk.get_managed_singleton(sdk.game_namespace("PadManager"))

    if padman then
        update_padman(padman)
    end]]
end)

re.on_application_entry("LockScene", function()
    if not vrmod:is_hmd_active() then return end

    update_muzzle_data()

    if last_shoot_pos then
        local pos = last_shoot_pos + (last_shoot_dir * 0.02)
        update_crosshair_world_pos(pos, pos + (last_shoot_dir * 1000.0))
    end
    
    --[[if not vrmod:is_hmd_active() then return end
    local camera = sdk.get_primary_camera()

    if camera ~= nil and last_real_camera_joint_rotation ~= nil then
        local camera_gameobject = camera:call("get_GameObject")
        local camera_transform = camera_gameobject:call("get_Transform")
        local camera_joint = camera_transform:call("get_Joints")[0]

        joint_set_position:call(camera_joint, last_real_camera_joint_pos)
        joint_set_rotation:call(camera_joint, last_real_camera_joint_rotation)
    end]]
end)

local last_roomscale_failure = os.clock()

re.on_pre_application_entry("UnlockScene", function()
    if queue_recenter then
        vrmod:recenter_view()
        queue_recenter = false
    end

    if not vrmod:is_hmd_active() then last_roomscale_failure = os.clock() return end

    last_camera_matrix = vrmod:get_last_render_matrix()

    if not re8.player or not re8.transform then last_roomscale_failure = os.clock() return end
    if not re8.status then last_roomscale_failure = os.clock() return end -- in the main menu or something.
    if not last_camera_matrix then last_roomscale_failure = os.clock() return end
    if re8.is_in_cutscene then last_roomscale_failure = os.clock() return end

    if os.clock() - last_roomscale_failure < 1.0 then return end

    local standing_origin = vrmod:get_standing_origin()
    local hmd_pos = vrmod:get_position(0)

    hmd_pos.y = 0.0
    standing_origin.y = 0.0

    if (hmd_pos - standing_origin):length() >= 0.01 then
        standing_origin = vrmod:get_standing_origin()
        hmd_pos.y = standing_origin.y

        local old_standing_origin = Vector4f.new(standing_origin.x, standing_origin.y, standing_origin.z, standing_origin.w)

        standing_origin = standing_origin:lerp(hmd_pos, (hmd_pos - standing_origin):length() * 0.1)

        local standing_diff = standing_origin - old_standing_origin

        vrmod:set_standing_origin(standing_origin)

        local player_pos = transform_get_position:call(re8.transform)
        local lerp_to = Vector3f.new(last_camera_matrix[3].x, player_pos.y, last_camera_matrix[3].z)

        player_pos = player_pos + ((lerp_to - player_pos):normalized() * standing_diff:length())
        --player_pos:lerp(lerp_to, 0.1)
        --player_pos.x = last_camera_matrix[3].x
        --player_pos.z = last_camera_matrix[3].z
        
        --transform_set_position:call(re8.transform, player_pos)
        re8.transform:set_position(player_pos, true) -- NO DIRTY
    end
end)

--[[re.on_application_entry("BeginRendering", function()

end)]]

re.on_config_save(function()
    json.dump_file(cfg_path, cfg)
end)

local type_to_table = function(obj)
end

local function obj_to_table(obj, seen_objects)
    seen_objects = seen_objects or {}

    if obj == nil or seen_objects[obj] ~= nil and seen_objects[obj] > 0 then
        return { __null = true }
    end

    if tostring(type(obj)) ~= "userdata" then
        --log.debug(tostring(obj))
        return obj
    end

    local out = {}

    local readable_type_name = tostring(getmetatable(obj).__name)

    --log.debug(readable_type_name)

    if readable_type_name:find("glm::mat<4,4") then
        out = {
            { x = obj[0][0], y = obj[0][1], z = obj[0][2], w = obj[0][3]},
            { x = obj[1][0], y = obj[1][1], z = obj[1][2], w = obj[1][3]},
            { x = obj[2][0], y = obj[2][1], z = obj[2][2], w = obj[2][3]},
            { x = obj[3][0], y = obj[3][1], z = obj[3][2], w = obj[3][3]},
        }

        return out
    elseif readable_type_name:find("glm::vec<4") or readable_type_name:find("glm::qua<") then
        out = {
            x = obj.x,
            y = obj.y,
            z = obj.z,
            w = obj.w
        }

        return out
    elseif readable_type_name:find("glm::vec<3") then
        out = {
            x = obj.x,
            y = obj.y,
            z = obj.z
        }

        return out
    elseif readable_type_name:find("glm::vec<2") then
        out = {
            x = obj.x,
            y = obj.y
        }

        return out
    end

    if seen_objects[obj] ~= nil then
        seen_objects[obj] = seen_objects[obj] + 1
    else
        seen_objects[obj] = 1
    end

    if getmetatable(obj).get_type_definition == nil then
        out = { __REFRAMEWORK_UNIMPLEMENTED_TYPE = getmetatable(obj).__name }

        return out
    end

    out["__type"] = obj:get_type_definition():get_full_name()

    if obj:get_type_definition():is_a("via.GameObject") then
        local components = obj:call("get_Components")
        components = components and components:get_elements() or {}

        out["__components"] = { __num = #components }

        for i, component in ipairs(components) do
            out["__components"][tostring(i)] = obj_to_table(component, seen_objects)
        end
    end

    --log.debug(tostring(type(obj)))
    --log.debug(tostring(obj) .. ": " .. tostring(getmetatable(obj).__name))
    local fields = obj:get_type_definition():get_fields()

    for i, field in ipairs(fields) do
        if not field:is_static() then
            local field_type = field:get_type()
            --log.debug("field: " .. field:get_name() .. " " .. field_type:get_full_name())
            local ok, value = pcall(obj.get_field, obj, field:get_name())

            --log.debug(" " .. tostring(ok))

            if not ok then
                log.debug("error on field: " .. obj:get_type_definition():get_full_name() .. "." .. field:get_name())
            end

            if not ok then goto continue end

            --log.debug(field:get_name())
            local type_definition = nil

            if value ~= nil and tostring(type(value)) == "userdata" and getmetatable(value).get_type_definition ~= nil then
                type_definition = value:get_type_definition()
            end

            if type_definition and type_definition:is_array() and tostring(type(value)) == "userdata" then
                local array = {}
                local array_elems = value ~= nil and value:get_elements() or {}

                for i, elem in ipairs(array_elems) do
                    --array[i] = obj_to_table(elem)
                end

                out[field:get_name()] = array
            elseif field_type:is_primitive() or field_type:is_enum() then
                --out[field:get_name()] = value
            elseif not field_type:is_value_type() then
                if tostring(type(value)) == "userdata" then
                    --log.debug(tostring(value) .. ": " .. tostring(getmetatable(value).__name))
                    out[field:get_name()] = obj_to_table(value, seen_objects)
                else
                    --out[field:get_name()] = value
                end
            else -- value type
                --out[field:get_name()] = obj_to_table(value, seen_objects)
            end

            ::continue::
        end
    end

    seen_objects[obj] = seen_objects[obj] - 1

    return out
end

--[[local tbl = obj_to_table(__object_explorer_object)
json.dump_file("object_explorer/" .. __object_explorer_object_path .. ".json", tbl)

collectgarbage("collect")]] -- force a GC to free up the memory

local function re8_on_pre_order_vibration(args)
    if not vrmod:is_using_controllers() then
        return
    end

    local task = sdk.to_managed_object(args[3])

    local left_power = task:get_field("MotorPower_0")
    local right_power = task:get_field("MotorPower_1")
    local duration = task:get_field("TimeSecond")

    if left_power > 0 then
        local left_joystick = vrmod:get_left_joystick()
        vrmod:trigger_haptic_vibration(0.0, duration, 1, left_power, left_joystick)
    elseif re8.was_gripping_weapon then
        local left_joystick = vrmod:get_left_joystick()
        vrmod:trigger_haptic_vibration(0.0, duration, 1, right_power, left_joystick)
    end

    if right_power > 0 then
        local right_joystick = vrmod:get_right_joystick()
        vrmod:trigger_haptic_vibration(0.0, duration, 1, right_power, right_joystick)
    end
end

local function re8_on_post_order_vibration(retval)
    return retval
end

if is_re8 then
    local vibrationmanager_t = sdk.find_type_definition("app.HIDVibrationManager")
    sdk.hook(vibrationmanager_t:get_method("OrderVibration(app.VibrationTask)"), re8_on_pre_order_vibration, re8_on_post_order_vibration)
end

local function re8_on_pre_order_vibration2(args)
    if not vrmod:is_using_controllers() then
        return
    end

    local left_power = sdk.to_float(args[4])
    local duration = sdk.to_float(args[3])

    if left_power > 0 then
        local left_joystick = vrmod:get_left_joystick()
        vrmod:trigger_haptic_vibration(0.0, duration, 1, left_power, left_joystick)
    end

    --if right_power > 0 then
        local right_joystick = vrmod:get_right_joystick()
        vrmod:trigger_haptic_vibration(0.0, duration, 1, left_power, right_joystick)
    --end
end

local function re8_on_post_order_vibration2(retval)
    return retval
end

if is_re8 then
    local vibrationmanager_t = sdk.find_type_definition("app.HIDVibrationManager")
    local vibmethod2 = vibrationmanager_t:get_method("OrderVibration(System.Single, System.Single, System.Single, System.Boolean, System.Int32, System.Boolean, System.Single, System.Single, via.GameObject, System.Single, System.Single)")
    sdk.hook(vibmethod2, re8_on_pre_order_vibration2, re8_on_post_order_vibration2)
end

local function re7_on_pre_request_add_vibration(args)
    if not vrmod:is_using_controllers() then
        return
    end

    local param = sdk.to_managed_object(args[3])
    local duration = sdk.to_float(args[4])

    local power = param:get_field("HighMotorPower")

    local left_joystick = vrmod:get_left_joystick()
    vrmod:trigger_haptic_vibration(0.0, duration, 1, power, left_joystick)

    local right_joystick = vrmod:get_right_joystick()
    vrmod:trigger_haptic_vibration(0.0, duration, 1, power, right_joystick)
end

local function re7_on_post_request_add_vibration(retval)
    return retval
end

if is_re7 then
    local vibrationmanager_t = sdk.find_type_definition("app.VibrationManager")
    sdk.hook(vibrationmanager_t:get_method("requestAdd(app.VibrationParam, System.Single)"), re7_on_pre_request_add_vibration, re7_on_post_request_add_vibration)
end

local cached_contact_pos = nil

re.on_frame(function()
    if cached_contact_pos then
        local screen = draw.world_to_screen(cached_contact_pos)

        if screen then
            draw.filled_rect(screen.x - 10, screen.y - 10, 5, 5, 0xFFFFFFFF)
        end
    end
end)

local debug_adjust_hand_offset = false

re.on_draw_ui(function()
    local changed = false
    
    changed, cfg.movement_shake = imgui.checkbox("Movement Shake", cfg.movement_shake)
    changed, cfg.all_camera_shake = imgui.checkbox("All Other Camera Shakes", cfg.all_camera_shake)
    changed, cfg.disable_crosshair = imgui.checkbox("Disable Crosshair", cfg.disable_crosshair)

    changed, left_hand_rotation_vec = imgui.drag_float3("Left Hand Rotation Offset", left_hand_rotation_vec, 0.005, -5.0, 5.0)

    if changed then
        left_hand_rotation_offset = Quaternion.new(left_hand_rotation_vec):normalized()
    end

    changed, right_hand_rotation_vec = imgui.drag_float3("Right Hand Rotation Offset", right_hand_rotation_vec, 0.005, -5.0, 5.0)

    if changed then
        right_hand_rotation_offset = Quaternion.new(right_hand_rotation_vec):normalized()
    end

    changed, left_hand_position_offset = imgui.drag_float4("Left Hand Position Offset", left_hand_position_offset, 0.005, -5.0, 5.0)
    changed, right_hand_position_offset = imgui.drag_float4("Right Hand Position Offset", right_hand_position_offset, 0.005, -5.0, 5.0)

    if imgui.tree_node("Debug") then
        changed, debug_adjust_hand_offset = imgui.checkbox("Adjust Hand Offset", debug_adjust_hand_offset)

        if debug_adjust_hand_offset then
            local left_axis = vrmod:get_left_stick_axis()
            local right_axis = vrmod:get_right_stick_axis()
            local right_joystick = vrmod:get_right_joystick()
            local left_joystick = vrmod:get_left_joystick()
            local action_grip = vrmod:get_action_grip()
            local action_trigger = vrmod:get_action_trigger()

            local is_right_grip_active = vrmod:is_action_active(action_grip, right_joystick)
            local is_left_grip_active = vrmod:is_action_active(action_grip, left_joystick)
            local is_right_trigger_active = vrmod:is_action_active(action_trigger, right_joystick)
            local is_left_trigger_active = vrmod:is_action_active(action_trigger, left_joystick)

            -- adjust the rotation offset based on how the user is moving the controller
            if not is_right_trigger_active then
                if not is_right_grip_active then
                    right_hand_rotation_vec.x = right_hand_rotation_vec.x + (right_axis.y * 0.001)
                    right_hand_rotation_vec.y = right_hand_rotation_vec.y + (right_axis.x * 0.001)
                else
                    right_hand_rotation_vec.z = right_hand_rotation_vec.z + ((right_axis.y + right_axis.x) * 0.001)
                end
            else
                if not is_right_grip_active then
                    right_hand_position_offset.x = right_hand_position_offset.x + (right_axis.y * 0.001)
                    right_hand_position_offset.y = right_hand_position_offset.y + (right_axis.x * 0.001)
                else
                    right_hand_position_offset.z = right_hand_position_offset.z + ((right_axis.y + right_axis.x) * 0.001)
                end
            end

            right_hand_rotation_offset = Quaternion.new(right_hand_rotation_vec):normalized()

            if not is_left_trigger_active then
                if not is_left_grip_active then
                    left_hand_rotation_vec.x = left_hand_rotation_vec.x + (left_axis.y * 0.001)
                    left_hand_rotation_vec.y = left_hand_rotation_vec.y + (left_axis.x * 0.001)
                else
                    left_hand_rotation_vec.z = left_hand_rotation_vec.z + ((left_axis.y + left_axis.x) * 0.001)
                end
            else
                if not is_left_grip_active then
                    left_hand_position_offset.x = left_hand_position_offset.x + (left_axis.y * 0.001)
                    left_hand_position_offset.y = left_hand_position_offset.y + (left_axis.x * 0.001)
                else
                    left_hand_position_offset.z = left_hand_position_offset.z + ((left_axis.y + left_axis.x) * 0.001)
                end
            end

            left_hand_rotation_offset = Quaternion.new(left_hand_rotation_vec):normalized()
        end

        imgui.text("Last GUI Dot: " .. tostring(last_gui_dot))

        if imgui.button("Cast ray") then
            local camera = sdk.get_primary_camera()
            local start_pos = camera:call("get_WorldMatrix")[3]
            local end_pos = start_pos - (camera:call("get_WorldMatrix")[2] * 1000.0)

            log.debug("Casting from " .. tostring(start_pos.x) .. tostring(start_pos.y) .. tostring(start_pos.z))

            local ray_result = cast_ray(start_pos, end_pos)
            local contact_point = ray_result:call("getContactPoint(System.UInt32)", 0)
            local contact_pos = contact_point:get_field("Position")
            
            cached_contact_pos = contact_pos

            log.debug("hit: " .. tostring(contact_pos.x) .. ", " .. tostring(contact_pos.y) .. ", " .. tostring(contact_pos.z))
        end

        if imgui.tree_node("Player") then
            object_explorer:handle_address(re8.player)

            imgui.tree_pop()
        end

        if imgui.tree_node("Inventory") then
            object_explorer:handle_address(re8.inventory)

            imgui.tree_pop()
        end

        if imgui.tree_node("Right Hand IK") then
            local right_hand_ik = re8.right_hand_ik
    
            object_explorer:handle_address(right_hand_ik)
    
            imgui.tree_pop()
        end
    
        if imgui.tree_node("Left Hand IK") then
            local left_hand_ik = re8.left_hand_ik
    
            object_explorer:handle_address(left_hand_ik)
    
            imgui.tree_pop()
        end

        if imgui.tree_node("Weapon") then
            local weapon = re8.weapon
    
            object_explorer:handle_address(weapon)
    
            imgui.tree_pop()
        end
    
        if imgui.button("test dump") then
            local d = function(name)
                local obj = sdk.get_managed_singleton(name)
    
                if obj then
                    local tbl2 = obj_to_table(obj, {})
                    json.dump_file("object_explorer/" .. obj:get_type_definition():get_full_name() .. ".json", tbl2)
                end
            end
    
            --d("app.VrGuiManager")
            --d("app.vr.VrManager")
            --d("app.GameManager")
            d("app.ObjectManager")
        end

        imgui.text("Num tasks: " .. tostring(re8.num_active_tasks))
        imgui.text("Has postural camera control: " .. tostring(re8.has_postural_camera_control))
        imgui.text("Is arm jacked: " .. tostring(re8.is_arm_jacked))
        imgui.text("Is motion play: " .. tostring(re8.is_motion_play))
        imgui.text("Is in cutscene: " .. tostring(re8.is_in_cutscene))
        imgui.text("Can use hands: " .. tostring(re8.can_use_hands))

        imgui.tree_pop()
    end
end)

-- GUI elements that require usage of LB RB
local re8_inventory_names = {
    "GUIInventory",
    "GUIInventoryMenu",
    "GUIInventoryTreasure",
    "GUIInventoryCraft",
    "GUIInventoryKeyItem",
    "GUIMap",
    "GUIShopBg"
}

for i, v in ipairs(re8_inventory_names) do
    re8_inventory_names[v] = true
end

local reticle_names = {
    "ReticleGUI",
    "GUIReticle"
}

for i, v in ipairs(reticle_names) do
    reticle_names[v] = true
end

local scope_names = {
    "GUIScope"
}

for i, v in ipairs(scope_names) do
    scope_names[v] = true
end

local shop_names = {
    "GUIShopBg",
}

for i, v in ipairs(shop_names) do
    shop_names[v] = true
end

local function read_vec4(obj, offset)
    return Vector4f.new(obj:read_float(offset), obj:read_float(offset + 4), obj:read_float(offset + 8), obj:read_float(offset + 12))
end

local function write_vec4(obj, vec, offset)
    obj:write_float(offset, vec.x)
    obj:write_float(offset + 4, vec.y)
    obj:write_float(offset + 8, vec.z)
    obj:write_float(offset + 12, vec.w)
end

re.on_pre_gui_draw_element(function(element, context)
    if not vrmod:is_hmd_active() then return true end

    local game_object = element:call("get_GameObject")
    if game_object == nil then return true end

    local name = game_object:call("get_Name")

    --log.info("drawing element: " .. name)

    if reticle_names[name] then
        if cfg.disable_crosshair and vrmod:is_using_controllers() then
            return false
        end
    end

    if scope_names[name] then
        last_scope_time = os.clock()
    end

    -- set the world position of the crosshair/reticle to the trace end position
    -- also fixes scopes.
    if reticle_names[name] or scope_names[name] then
        if re8.crosshair_pos then
            local transform = game_object:call("get_Transform")

            if transform then
                --[[if transform.set_position ~= nil then
                    transform:set_position(re8.crosshair_pos, true)
                else
                    transform_set_position(transform, re8.crosshair_pos)
                end]]

                local new_mat = re8.crosshair_dir:to_quat():to_mat4()
                local distance = re8.crosshair_distance * 0.1
                if distance > 10 then
                    distance = 10
                end

                if distance < 0.3 then
                    distance = 0.3
                end

                local crosshair_pos = Vector4f.new(re8.crosshair_pos.x, re8.crosshair_pos.y, re8.crosshair_pos.z, 1.0)

                if is_re8 then
                    write_vec4(transform, new_mat[0] * distance, 0x80)
                    write_vec4(transform, new_mat[1] * distance, 0x90)
                    write_vec4(transform, new_mat[2] * distance, 0xA0)
                    write_vec4(transform, crosshair_pos, 0xB0)
                elseif is_re7 then
                    write_vec4(transform, new_mat[0] * distance, 0x90)
                    write_vec4(transform, new_mat[1] * distance, 0xA0)
                    write_vec4(transform, new_mat[2] * distance, 0xB0)
                    write_vec4(transform, crosshair_pos, 0xC0)
                end
            end
        end
    end

    if re8_inventory_names[name] then
        last_inventory_open_time = os.clock()
    end

    if shop_names[name] then
        vrmod:set_gui_rotation_offset(Quaternion.identity())
        last_shop_open_time = os.clock()

        local transform = game_object:call("get_Transform")
        local old_pos = transform:get_position()
        
        local camera = sdk.get_primary_camera()
        local camera_pos = camera:call("get_WorldMatrix")[3]
        local camera_forward = camera:call("get_WorldMatrix")[2]

        local old_distance = (old_pos - camera_pos):length()

        camera_forward.y = 0.0
        camera_forward = camera_forward:normalized()

        local new_pos = (camera_pos - (camera_forward * old_distance))
        local new_rot = camera_forward:to_quat()

        transform:set_position(new_pos, true)
        transform:set_rotation(new_rot, true)
    end

    return true
end)