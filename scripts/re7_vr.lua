local statics = require("utility/Statics")
local re7 = require("utility/RE7")
local GameObject = require("utility/GameObject")

local renderer = sdk.get_native_singleton("via.render.Renderer")
local renderer_type = sdk.find_type_definition("via.render.Renderer")

local GamePadButton = statics.generate("via.hid.GamePadButton")

local last_camera_matrix = Matrix4x4f.new()
local last_right_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_right_hand_position = Vector3f.new(0.0, 0.0, 0.0)
local last_left_hand_rotation = Quaternion.new(0.0, 0.0, 0.0, 0.0)
local last_left_hand_position = Vector3f.new(0.0, 0.0, 0.0)
local left_hand_rotation_vec = Vector3f.new(-0.105 + 0.2, 2.43, 1.16) -- pitch yaw roll?
local right_hand_rotation_vec = Vector3f.new(-0.105, -2.43, -1.16) -- pitch yaw roll?
local left_hand_rotation_offset = Quaternion.new(left_hand_rotation_vec):normalized()
local right_hand_rotation_offset = Quaternion.new(right_hand_rotation_vec):normalized()
local left_hand_position_offset = Vector4f.new(-0.025, 0.075, 0.155, 0.0)
local right_hand_position_offset = Vector4f.new(0.025, 0.075, 0.155, 0.0)

local ray_typedef = sdk.find_type_definition("via.Ray")

local transform_get_position = sdk.find_type_definition("via.Transform"):get_method("get_Position")
local transform_get_rotation = sdk.find_type_definition("via.Transform"):get_method("get_Rotation")
local transform_set_position = sdk.find_type_definition("via.Transform"):get_method("set_Position")
local transform_set_rotation = sdk.find_type_definition("via.Transform"):get_method("set_Rotation")
local transform_get_joints = sdk.find_type_definition("via.Transform"):get_method("get_Joints")

local joint_get_position = sdk.find_type_definition("via.Joint"):get_method("get_Position")
local joint_get_rotation = sdk.find_type_definition("via.Joint"):get_method("get_Rotation")
local joint_set_position = sdk.find_type_definition("via.Joint"):get_method("set_Position")
local joint_set_rotation = sdk.find_type_definition("via.Joint"):get_method("set_Rotation")

local component_get_gameobject = sdk.find_type_definition("via.Component"):get_method("get_GameObject")
local gameobject_get_transform = sdk.find_type_definition("via.GameObject"):get_method("get_Transform")

local cfg_path = "re7_vr/main_config.json"

local cfg = {
    movement_shake = false,
    all_camera_shake = true
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
statics.generate_global("app.HIDManager.InputMode")

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

local function update_pad_device(device)
    if not vrmod:is_hmd_active() then
        return
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
    if vr_right_stick_axis.x > 0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.RRight
    elseif vr_right_stick_axis.x < -0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.RLeft
    end

    if vr_right_stick_axis.y > 0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.RUp
    elseif vr_right_stick_axis.y < -0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.RDown
    end

    if vr_left_stick_axis.x > 0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.LRight
    elseif vr_left_stick_axis.x < -0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.LLeft
    end

    if vr_left_stick_axis.y > 0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.LUp
    elseif vr_left_stick_axis.y < -0.1 then
        --cur_button = cur_button | via.hid.GamePadButton.LDown
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

        -- set right bumper instead of left trigger
        cur_button = cur_button | via.hid.GamePadButton.LTrigTop

        -- set right bumper (heal) if holding both trigger and grip
        if vrmod:is_action_active(action_grip, left_joystick) then
            cur_button = cur_button | via.hid.GamePadButton.RTrigTop
        end
    end

    if re7.wants_block then
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
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
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

    set_inputmode(app.HIDManager.InputMode.Pad)
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

    set_inputmode(app.HIDManager.InputMode.Pad)
end

local function on_post_app_pad_update(retval)
    return retval
end

sdk.hook(
    sdk.find_type_definition("app.Pad"):get_method("update"),
    on_pre_app_pad_update,
    on_post_app_pad_update
)

local function on_pre_try_guard_start(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    -- this will only allow blocking by physically holding your hands up.
    if not re7.wants_block then
        return sdk.PreHookResult.SKIP_ORIGINAL
    end
end

local function on_post_try_guard_start(retval)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() or re7.wants_block then
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

    local player_transform = re7.transform
    local player_pos = transform_get_position:call(player_transform)
    local player_rot = transform_get_rotation:call(player_transform)

    local joints = {}

    local cur_joint = joint

    for i=0, depth do
        local parent = cur_joint:call("get_Parent")
        table.insert(joints, parent)
        cur_joint = parent
    end
end

local function set_hand_joints_to_tpose(hand_ik)
    local hashes = {
        hand_ik:get_field("HashJoint0"),
        hand_ik:get_field("HashJoint1"),
        hand_ik:get_field("HashJoint2"),
        hand_ik:get_field("HashJoint3")
    }

    local original_positions = {}
    local original_rotations = {}
    local current_positions = {}

    local player_transform = re7.transform
    local player_pos = transform_get_position:call(player_transform)
    local player_rot = transform_get_rotation:call(player_transform)

    local joints = {}

    for i, hash in ipairs(hashes) do
        if hash and hash ~= 0 then
            local joint = player_transform:call("getJointByHash", hash)

            if joint then
                table.insert(joints, joint:call("get_Parent"))
            end
        end
    end

    if #joints > 0 and joints[1] ~= nil then
        table.insert(joints, 1, joints[1]:call("get_Parent"))
        table.insert(joints, 1, joints[1]:call("get_Parent"))
        table.insert(joints, 1, joints[1]:call("get_Parent"))
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
    if not re7.player then return end
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then return end

    local controllers = vrmod:get_controllers()

    if #controllers == 0 then
        return
    end

    if not re7.left_hand_ik or not re7.right_hand_ik then
        return
    end

    if re7.is_in_cutscene then return end

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
    local original_camera_rotation_pre = original_camera_matrix:to_quat()
    local original_camera_rotation = (original_camera_rotation_pre * vrmod:get_rotation_offset()):normalized()

    local fake_quat = Quaternion.new(original_camera_rotation_pre.w, original_camera_rotation_pre.x, original_camera_rotation_pre.y, original_camera_rotation_pre.z)
    local updated_camera_pos = original_camera_matrix[3]

    vrmod:apply_hmd_transform(fake_quat, updated_camera_pos)

    local new_rotation = original_camera_rotation * left_controller_rotation * left_hand_rotation_offset
    local new_pos = updated_camera_pos
                + ((original_camera_rotation * left_controller_offset) 
                + ((original_camera_rotation * left_controller_rotation):normalized() * left_hand_position_offset))

    last_left_hand_position = new_pos
    last_left_hand_rotation = new_rotation

    set_hand_joints_to_tpose(re7.left_hand_ik)

    transform_set_position:call(re7.left_hand_ik_transform, new_pos)
    transform_set_rotation:call(re7.left_hand_ik_transform, new_rotation)
    re7.left_hand_ik:set_field("Transition", 1.0)
    re7.left_hand_ik:call("calc")

    --re7.transform:call("getJointByHash", re7.left_hand_ik:get_field("HashJoint2")):call("set_Position", new_pos)

    new_rotation = original_camera_rotation * right_controller_rotation * right_hand_rotation_offset
    new_pos = updated_camera_pos
                + ((original_camera_rotation * right_controller_offset) 
                + ((original_camera_rotation * right_controller_rotation):normalized() * right_hand_position_offset))
            
    last_right_hand_position = new_pos
    last_right_hand_rotation = new_rotation

    set_hand_joints_to_tpose(re7.right_hand_ik)
    
    transform_set_position:call(re7.right_hand_ik_transform, new_pos)
    transform_set_rotation:call(re7.right_hand_ik_transform, new_rotation)
    re7.right_hand_ik:set_field("Transition", 1.0)
    re7.right_hand_ik:call("calc")

    --re7.transform:call("getJointByHash", re7.right_hand_ik:get_field("HashJoint2")):call("set_Position", new_pos)
end

local last_real_camera_rotation = Quaternion.new(1, 0, 0, 0)
local last_real_camera_joint_rotation = Quaternion.new(1, 0, 0, 0)
local last_real_camera_joint_pos = Vector3f.new(0, 0, 0)

local neg_forward_identity = Matrix4x4f.new(-1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, -1, 0,
                                            0, 0, 0, 1):to_quat()

local head_hash = nil
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
    if not re7.player then return end

    local player = re7.player
    local ik_leg = player:call("getComponent(System.Type)", sdk.typeof("via.motion.IkLeg"))

    if not ik_leg then
        ik_leg = player:call("createComponent(System.Type)", sdk.typeof("via.motion.IkLeg"))

        if not ik_leg then
            log.error("Failed to create IK leg component")
            return
        end
    end

    if re7.is_in_cutscene or not vrmod:is_hmd_active() then
        --ik_leg:call("set_Enabled", false)
        ik_leg:call("set_CenterOffset", Vector3f.new(0, 0, 0))
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

    if not chest_hash then
        chest_hash = get_joint_hash(transform, motion, "Chest")
    end

    if not center_hash then
        center_hash = get_joint_hash(transform, motion, "Hip")
    end

    local transform_rot = transform:call("get_Rotation")
    local transform_pos = transform:call("get_Position")

    --local head_joint = transform:call("getJointByHash", head_hash)
    --local chest_joint = transform:call("getJointByHash", chest_hash)

    local head_index = motion:call("getJointIndexByNameHash", head_hash)
    --chest_index = motion:call("getJointIndexByNameHash", chest_hash)
    local original_head_pos = motion:call("getWorldPosition", head_index)
    --local original_chest_pos = motion:call("getWorldPosition", chest_index)

    original_head_pos = transform_rot * original_head_pos
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

    local diff_to_camera = ((camera_pos - transform_pos) - original_head_pos)

    --ik_leg:call("set_CenterJointName", "Hip")
    ik_leg:call("set_CenterOffset", diff_to_camera)
    ik_leg:call("setCenterAdjust", 0)
    --ik_leg:call("set_UpdateTiming", 2) -- ConstraintsBegin
end

local function on_pre_shoot(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    update_hand_ik()

    local weapon = sdk.to_managed_object(args[2])
    local ray = args[3]

    local muzzle_joint = weapon:call("get_muzzleJoint")

    if muzzle_joint then
        local muzzle_pos = joint_get_position:call(muzzle_joint)
        local muzzle_forward = joint_get_rotation:call(muzzle_joint) * Vector3f.new(0, 0, 1)

        -- nudge the start position slightly forward because
        -- apparently the bullets can collide with the weapon.... wtf
        sdk.set_native_field(ray, ray_typedef, "from", muzzle_pos + (muzzle_forward * 0.01))
        sdk.set_native_field(ray, ray_typedef, "dir", muzzle_forward)
    else
        log.info("No muzzle joint found")
    end

    --sdk.call_native_func(ray, ray_typedef, ".ctor(via.vec3, via.vec3)", last_muzzle_pos, last_muzzle_forward)
end

local function on_post_shoot(retval)
    return retval
end

sdk.hook(sdk.find_type_definition("app.WeaponGun"):get_method("shoot"), on_pre_shoot, on_post_shoot)

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

local was_vert_limited = false

local last_gui_quat = Quaternion.identity()
local last_gui_dot = 0.0
local last_gui_forced_slerp = os.clock()
local needs_cutscene_recenter = false

-- force the gui to recenter when opening the inventory
sdk.hook(
    sdk.find_type_definition("app.MenuManager"):get_method("openInventoryMenu"),
    function(args)
        last_gui_forced_slerp = os.clock()
    end,
    function(retval)
        return retval
    end
)

local last_camera_update_args = nil

local function fix_player_camera(player_camera)
    if not vrmod:is_hmd_active() then
        -- so the camera doesnt go wacky
        if needs_cutscene_recenter then
            -- disables the body IK component
            update_body_ik(nil, nil)

            vrmod:set_gui_rotation_offset(Quaternion.identity())
            vrmod:recenter_view()
    
            needs_cutscene_recenter = false
        end

        -- Restore the vertical camera movement after taking headset off/not using controllers
        if was_vert_limited then
            --local player_camera = sdk.to_managed_object(args[2])
            local base_transform_solver = player_camera:get_field("BaseTransSolver")

            if base_transform_solver then
                local camera_controller = base_transform_solver:get_field("CurrentController")

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

    if re7.is_in_cutscene and needs_cutscene_recenter then
        vrmod:set_gui_rotation_offset(Quaternion.identity())
        vrmod:recenter_view()

        -- force the gui to be recentered when we exit the cutscene
        last_gui_forced_slerp = os.clock()
        needs_cutscene_recenter = false
    end

    local camera = sdk.get_primary_camera()

    -- apply the camera rot to the real camera
    local camera_gameobject = camera:call("get_GameObject")
    local camera_transform = camera_gameobject:call("get_Transform")
    --last_real_camera_rotation = camera_transform:call("get_Rotation")
    --last_real_camera_joint_rotation = camera_transform:call("get_Joints")[0]:call("get_Rotation")

    local camera_rot = transform_get_rotation:call(camera_transform)
    local camera_pos = transform_get_position:call(camera_transform)

    local camera_rot_pre_hmd = Quaternion.new(camera_rot.w, camera_rot.x, camera_rot.y, camera_rot.z)
    local camera_pos_pre_hmd = Vector3f.new(camera_pos.x, camera_pos.y, camera_pos.z)

    --camera_rot = (neg_forward_identity * camera_rot):normalized()
    --camera_rot = neg_forward_identity * camera_rot

    --[[local forward = camera_rot * Vector3f.new(0, 0, 1)
    forward.y = 0.0
    forward:normalize()]]

    --local camera_rot = (forward * -1.0):to_quat()

    vrmod:apply_hmd_transform(camera_rot, camera_pos)

    --local neg_camera_rot = (neg_forward_identity * camera_rot):normalized()

    local camera_joint = camera_transform:call("get_Joints")[0]

    -- Transform is used for things like Ethan's light
    -- and determining where the player is looking
    transform_set_position:call(camera_transform, camera_pos)
    transform_set_rotation:call(camera_transform, camera_rot)
    
    last_real_camera_joint_rotation = camera_rot_pre_hmd
    last_real_camera_joint_pos = camera_pos_pre_hmd

    --joint_set_position:call(camera_joint, camera_pos)
    --joint_set_rotation:call(camera_joint, neg_camera_rot)

    -- Joint is used for the actual final rendering of the game world
    joint_set_position:call(camera_joint, camera_pos_pre_hmd)
    joint_set_rotation:call(camera_joint, camera_rot_pre_hmd)

    -- just update the body IK right after we update the camera.
    update_body_ik(camera_rot_pre_hmd, camera_pos)

    -- Slerp the gui around
    if not re7.is_in_cutscene then
        local new_gui_quat = camera_rot_pre_hmd * camera_rot:inverse()
        last_gui_dot = last_gui_quat:dot(new_gui_quat)
        local dot_dist = 1.0 - math.abs(last_gui_dot)
        local dot_ang = math.acos(math.abs(last_gui_dot)) * (180.0 / math.pi)
        last_gui_dot = dot_ang

        local now = os.clock()

        -- trigger gui slerp
        if dot_ang >= 20 then
            last_gui_forced_slerp = os.clock()
        end

        local slerp_time_diff = now - last_gui_forced_slerp

        if slerp_time_diff <= 1.5 then
            if dot_ang >= 10 then
                last_gui_forced_slerp = now
            end

            last_gui_quat = last_gui_quat:slerp(new_gui_quat, dot_dist * math.max((1.5 - slerp_time_diff), 0.0))
            vrmod:recenter_gui(last_gui_quat)
        end

        needs_cutscene_recenter = true
    end
    
    --[[local fixed_dir = camera_rot * Vector3f.new(0, 0, 1)
    fixed_dir.y = fixed_dir.y * -1.0
    fixed_dir:normalize()
    local fixed_rot = fixed_dir:to_quat()]]

    local fixed_rot = neg_forward_identity * camera_rot
    local fixed_dir = fixed_rot * Vector3f.new(0, 0, 1)

    player_camera:set_field("<CameraRotation>k__BackingField", fixed_rot)
    player_camera:set_field("<CameraPosition>k__BackingField", camera_pos)

    player_camera:set_field("CameraRotationWithMovementShake", fixed_rot)
    player_camera:set_field("CameraPositionWithMovementShake", camera_pos)
    player_camera:set_field("CameraRotationWithCameraShake", fixed_rot)
    player_camera:set_field("CameraPositionWithCameraShake", camera_pos)
    player_camera:set_field("PrevCameraRotation", fixed_rot)

    local camera_controller_param = player_camera:get_field("CameraCtrlParam")

    if camera_controller_param then
        camera_controller_param:set_field("CameraRotation", fixed_rot)
    end

    local base_transform_solver = player_camera:get_field("BaseTransSolver")

    if base_transform_solver then
        local camera_controller = base_transform_solver:get_field("CurrentController")

        -- Stop the player from rotating the camera vertically
        if camera_controller then
            local camera_controller_rot = camera_controller:get_field("<rotation>k__BackingField")
            local controller_forward = camera_controller_rot * Vector3f.new(0.0, 0.0, 1.0)
            controller_forward.y = 0.0
            controller_forward:normalize()
            camera_controller_rot = controller_forward:to_quat()
            base_transform_solver:set_field("<rotation>k__BackingField", camera_controller_rot)
            camera_controller:set_field("<rotation>k__BackingField", camera_controller_rot)
            camera_controller:set_field("IsVerticalRotateLimited", true)
            was_vert_limited = true
        end
    end

    -- stops the camera from pivoting around the player
    -- so we can use VR to look around without the body sticking out
    if vrmod:is_using_controllers() then
        local param_container = player_camera:get_field("_CurrentParamContainer")

        if param_container ~= nil then
            local posture_param = param_container:get_field("PostureParam")

            if posture_param ~= nil then
                local current_camera_offset = posture_param:get_field("CameraOffset")
                current_camera_offset.x = 0.0
                current_camera_offset.z = 0.0

                posture_param:set_field("CameraOffset", current_camera_offset)
            end
        end
    end

    local look_ray_offset = player_camera:get_type_definition():get_field("LookRay"):get_offset_from_base()
    local shoot_ray_offset = player_camera:get_type_definition():get_field("ShootRay"):get_offset_from_base()
    local look_ray = player_camera:get_address() + look_ray_offset
    local shoot_ray = player_camera:get_address() + shoot_ray_offset
    
    --local new_dir = Vector4f.new(0.0, 0.0, 0.0, 1.0)
    local new_pos = Vector4f.new(camera_pos.x, camera_pos.y, camera_pos.z, 1.0)

    sdk.set_native_field(sdk.to_ptr(look_ray), ray_typedef, "from", camera_pos)
    sdk.set_native_field(sdk.to_ptr(look_ray), ray_typedef, "dir", fixed_dir)
    sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "from", camera_pos)
    sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "dir", fixed_dir)
    --look_ray:set_field("from", camera_pos)
    --look_ray:set_field("dir", new_dir)

    --player_camera:set_field("LookRay", look_ray)

    --look_ray:set_field("from", new_pos)
    --look_ray:set_field("dir", new_dir)
    --player_camera:set_field("ShootRay", look_ray)
end

local function on_pre_player_camera_update(args)
    last_camera_update_args = args

    if not vrmod:is_hmd_active() then
        return
    end

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

-- Normal Ethan camera
sdk.hook(
    sdk.find_type_definition("app.PlayerCamera"):get_method("lateUpdate"), 
    on_pre_player_camera_update, 
    on_post_player_camera_update
)

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

sdk.hook(
    sdk.find_type_definition("app.PlayerUpperVerticalRotate"):get_method("doLateUpdate"), 
    on_pre_upper_vertical_update, 
    on_post_upper_vertical_update
)

local function check_player_hands_up()
    local player = re7.player

    if not player then 
        re7.wants_block = false
        return 
    end

    local right_hand_up = false
    local left_hand_up = false


    local controllers = vrmod:get_controllers()
    if #controllers < 2 then
        re7.wants_block = false
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
        re7.wants_block = false
        return
    end

    -- now we need to check if the hands are facing up
    local left_hand_up_dot = math.abs(hmd_forward:dot(left_hand[0]))
    local right_hand_up_dot = math.abs(hmd_forward:dot(right_hand[0]))

    left_hand_up = left_hand_up_dot >= 0.5
    right_hand_up = right_hand_up_dot >= 0.5

    re7.wants_block = left_hand_up and right_hand_up

    --log.info("left hand dot: " .. tostring(left_hand_dot))
    --log.info("right hand dot: " .. tostring(right_hand_dot))
end

local should_reset_view_no_player = false

re.on_pre_application_entry("UpdateBehavior", function()
    check_player_hands_up()

    if not re7.player then
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

re.on_pre_application_entry("UnlockScene", function()
    if not vrmod:is_hmd_active() then return end
    if not re7.transform then return end
    if not last_camera_matrix then return end
    if re7.is_in_cutscene then return end

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

        local player_pos = transform_get_position:call(re7.transform)
        local lerp_to = Vector3f.new(last_camera_matrix[3].x, player_pos.y, last_camera_matrix[3].z)

        player_pos = player_pos + ((lerp_to - player_pos):normalized() * standing_diff:length())
        --player_pos:lerp(lerp_to, 0.1)
        --player_pos.x = last_camera_matrix[3].x
        --player_pos.z = last_camera_matrix[3].z
        
        --transform_set_position:call(re7.transform, player_pos)
        re7.transform:set_position(player_pos, true) -- NO DIRTY
    end
end)

re.on_application_entry("BeginRendering", function()
    if not vrmod:is_hmd_active() then return end
    local camera = sdk.get_primary_camera()

    if camera ~= nil then
        --local camera_gameobject = camera:call("get_GameObject")
        --[[local camera_transform = camera_gameobject:call("get_Transform")
        local camera_joint = camera_transform:call("get_Joints")[0]

        last_camera_matrix = joint_get_rotation:call(camera_joint):to_mat4()
        last_camera_matrix[3] = joint_get_position:call(camera_joint)]]

        last_camera_matrix = camera:call("get_WorldMatrix")
    end
end)

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

re.on_draw_ui(function()
    local changed = false
    
    changed, cfg.movement_shake = imgui.checkbox("Movement Shake", cfg.movement_shake)
    changed, cfg.all_camera_shake = imgui.checkbox("All Other Camera Shakes", cfg.all_camera_shake)

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
        imgui.text("Last GUI Dot: " .. tostring(last_gui_dot))
    end

    if imgui.tree_node("Right Hand IK") then
        local right_hand_ik = re7.right_hand_ik

        object_explorer:handle_address(right_hand_ik)

        imgui.tree_pop()
    end

    if imgui.tree_node("Left Hand IK") then
        local left_hand_ik = re7.left_hand_ik

        object_explorer:handle_address(left_hand_ik)

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
end)

re.on_pre_gui_draw_element(function(element, context)
    if not vrmod:is_hmd_active() then return true end

    local game_object = element:call("get_GameObject")
    if game_object == nil then return true end

    local name = game_object:call("get_Name")

    --log.info("drawing element: " .. name)

    if name == "ReticleGUI" then
        if vrmod:is_using_controllers() then
            return false
        end
    end

    return true
end)