local game_name = reframework:get_game_name()
local is_re7 = game_name == "re7"
local is_re8 = game_name == "re8"

if not is_re7 and not is_re8 then
    return
end

local tdb_version = sdk.get_tdb_version();

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

re8vr.left_hand_rotation_offset = left_hand_rotation_offset
re8vr.right_hand_rotation_offset = right_hand_rotation_offset
re8vr.left_hand_position_offset = left_hand_position_offset
re8vr.right_hand_position_offset = right_hand_position_offset

local ray_typedef = sdk.find_type_definition("via.Ray")
local last_muzzle_rot = Quaternion.new(0.0, 0.0, 0.0, 0.0)

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
local last_book_open_time = 0.0
local last_shop_open_time = 0.0
local last_map_open_time = 0.0
local last_scope_time = 0.0
local head_hash = nil

local neg_forward_identity = Matrix4x4f.new(-1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, -1, 0,
                                            0, 0, 0, 1):to_quat()

local cfg = {
    movement_shake = false,
    all_camera_shake = false,
    disable_crosshair = false,
    no_melee_cooldown = false,
    roomscale_movement = true,
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
        local current_mode = hid_manager:call("get_InputMode")

        if is_re8 and current_mode ~= mode then
            hid_manager:set_field("<changedCursorShow>k__BackingField", false)
        end

        hid_manager:call("set_inputMode", mode)
    end
end

local is_inventory_open = false

local via_murmur_hash = sdk.find_type_definition("via.murmur_hash")
local via_murmur_hash_calc32 = via_murmur_hash:get_method("calc32")
local vfx_muzzle1_hash = via_murmur_hash_calc32:call(nil, "vfx_muzzle1")
local vfx_muzzle2_hash = via_murmur_hash_calc32:call(nil, "vfx_muzzle2")

local function update_muzzle_data()
    if re8vr.weapon then
        -- for some reason calling get_muzzleJoint causes lua to randomly freak out
        -- so we're just going to directly grab the field instead
        local muzzle_joint = re8vr.weapon:get_field("MuzzleJoint")

        if muzzle_joint == nil then
            local weapon_gameobject = nil
            
            if is_re7 then
                weapon_gameobject = re8vr.weapon:call("get_GameObject")
            elseif is_re8 then
                weapon_gameobject = re8vr.weapon:get_field("<owner>k__BackingField")
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
    
            re8vr.last_muzzle_pos = muzzle_position
            last_muzzle_rot = muzzle_rotation
            re8vr.last_muzzle_forward = muzzle_joint:call("get_AxisZ")

            if vrmod:is_using_controllers() then
                re8vr.last_shoot_dir = re8vr.last_muzzle_forward
                re8vr.last_shoot_pos = re8vr.last_muzzle_pos
            end
        elseif vrmod:is_using_controllers() then
            re8vr.last_muzzle_pos = re8vr.last_right_hand_position
            last_muzzle_rot = last_camera_matrix:to_quat()
            re8vr.last_muzzle_forward = (last_muzzle_rot * Vector3f.new(0, 0, -1)):normalized()

            re8vr.last_shoot_dir = re8vr.last_muzzle_forward
            re8vr.last_shoot_pos = re8vr.last_muzzle_pos
        else
            re8vr.last_muzzle_pos = re8vr.last_shoot_pos
            re8vr.last_muzzle_forward = re8vr.last_shoot_dir
        end
    end
end

local function update_pad_device(device)
    if not vrmod:is_hmd_active() then
        re8vr.is_holding_left_grip = false
        return
    end

    if is_re7 then
        local menu_manager = sdk.get_managed_singleton("app.MenuManager")

        if menu_manager ~= nil then
            is_inventory_open = menu_manager:call("isOpenInventoryMenu") or (os.clock() - last_inventory_open_time) < 0.25
        end
    elseif is_re8 then
        is_inventory_open = (os.clock() - last_inventory_open_time) < 0.25
    end

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
    local is_map_open = os.clock() - last_map_open_time < 0.25

    -- set cur_button | according to the right stick axis
    if sdk.get_tdb_version() >= 69 then
        local is_book_open = os.clock() - last_book_open_time < 0.25

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
            if is_book_open then
                cur_button = cur_button | via.hid.GamePadButton.LRight
            end

            cur_button = cur_button | via.hid.GamePadButton.EmuLright
        elseif vr_left_stick_axis.x < -0.1 then
            if is_book_open then
                cur_button = cur_button | via.hid.GamePadButton.LLeft
            end

            cur_button = cur_button | via.hid.GamePadButton.EmuLleft
        end
    
        if vr_left_stick_axis.y > 0.1 then
            if is_book_open then
                cur_button = cur_button | via.hid.GamePadButton.LUp
            end

            cur_button = cur_button | via.hid.GamePadButton.EmuLup
        elseif vr_left_stick_axis.y < -0.1 then
            if is_book_open then
                cur_button = cur_button | via.hid.GamePadButton.LDown
            end

            cur_button = cur_button | via.hid.GamePadButton.EmuLdown
        end
    end

    if is_map_open then
        if vr_right_stick_axis.y >= 0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LUp
        elseif vr_right_stick_axis.y <= -0.9 then
            cur_button = cur_button | via.hid.GamePadButton.LDown
        end
    end

    --cur_button = cur_button | via.hid.GamePadButton.CRight

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

    local is_minimap_active = vrmod:is_action_active(action_minimap, right_joystick) or vrmod:is_action_active(action_minimap, left_joystick)
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

    if re8vr.wants_heal or vrmod:is_action_active(action_heal, left_joystick) or vrmod:is_action_active(action_heal, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RTrigTop
        re8vr.wants_heal = false
    end
    
    re8vr.is_holding_left_grip = vrmod:is_action_active(action_grip, left_joystick)

    if re8vr.wants_block or vrmod:is_action_active(action_block, left_joystick) or vrmod:is_action_active(action_block, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LTrigTop
        re8vr.wants_block = true
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

    if not is_minimap_active and vrmod:is_action_active(action_b_button, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RUp
    end

    if vrmod:is_action_active(action_joystick_click, right_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.RStickPush
    end

    if vrmod:is_action_active(action_joystick_click, left_joystick) then
        cur_button = cur_button | via.hid.GamePadButton.LStickPush
    end

    if is_minimap_active then
        cur_button = cur_button | via.hid.GamePadButton.CLeft
    end

    if is_re7 and vrmod:should_handle_pause() then
        cur_button = cur_button | via.hid.GamePadButton.CRight
        vrmod:set_handle_pause(false)
    end

    device:call("set_Button", cur_button)
    device:call("set_ButtonDown", cur_button)
end

local function update_padman(padman)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        re8vr.is_holding_left_grip = false
        return
    end

    local merged_pad = padman:call("get_mergedPad")

    if not merged_pad then
        return
    end

    local active_pad = padman:call("get_activePad")

    if active_pad ~= nil then
        merged_pad = active_pad
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
    local active_pad = padman:call("get_activePad")

    if active_pad ~= nil then
        merged_pad = active_pad
    end

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
    if not re8vr.wants_block then
        return sdk.PreHookResult.SKIP_ORIGINAL
    end
end

local function on_post_try_guard_start(retval)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() or re8vr.wants_block then
        return retval
    end

    return sdk.to_ptr(0)
end

sdk.hook(
    sdk.find_type_definition("app.PlayerBase"):get_method("tryGuardStart"),
    on_pre_try_guard_start,
    on_post_try_guard_start
)

local function update_hand_ik()
    if re8vr.in_re8_end_game_event then return end

    re8vr:update_hand_ik()
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
    if re8vr.in_re8_end_game_event then return end

    re8vr:update_body_ik(camera_rotation, camera_pos)
end

local function on_pre_shoot(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then
        return
    end

    local weapon = sdk.to_managed_object(args[2])

    -- this happens in RE7 with the turrets.
    if weapon ~= re8vr.weapon then
        return
    end

    update_hand_ik()

    local ray = args[3]

    local pos = re8vr.last_muzzle_pos + (re8vr.last_muzzle_forward * 0.02)
    local from = Vector4f.new(pos.x, pos.y, pos.z, 1.0)
    local dir = Vector4f.new(re8vr.last_muzzle_forward.x, re8vr.last_muzzle_forward.y, re8vr.last_muzzle_forward.z, 1.0)

    sdk.set_native_field(ray, ray_typedef, "from", from)
    sdk.set_native_field(ray, ray_typedef, "dir", dir)
    --sdk.call_native_func(ray, ray_typedef, ".ctor(via.vec3, via.vec3)", re8vr.last_muzzle_pos, re8vr.last_muzzle_forward)
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
    if weapon ~= re8vr.weapon then return end

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
        local from = re8vr.last_right_hand_position

        vel_norm = Vector4f.new(vel_norm.x, vel_norm.y, vel_norm.z, 1.0)

        -- some BS to just throw it
        -- we will fix it inside on_post_bomb_activate_throwable
        -- by modifying the rigid body's velocity
        throw_ray:set_field("from", from)
        throw_ray:set_field("dir", vel_norm)

        inside_throw = true
        pcall(weapon.call, weapon, "throwWeapon", throw_ray)
        inside_throw = false

        local inventory = re8vr.updater:get_field("References"):call("get_inventory")

        -- Decrement the grenade count
        if threw_bomb and inventory ~= nil then
            local work = re8vr.weapon:call("get_work")

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
    if re8vr.movement_speed_rate > 0.0 then
        last_gui_forced_slerp = os.clock() - ((1.0 - re8vr.movement_speed_rate))
    end

    last_gui_dot = last_gui_quat:dot(new_gui_quat)
    local dot_dist = 1.0 - math.abs(last_gui_dot)
    local dot_ang = math.acos(math.abs(last_gui_dot)) * (180.0 / math.pi)
    last_gui_dot = dot_ang

    local now = os.clock()

    -- trigger gui slerp
    if dot_ang >= 20 or re8vr.is_in_cutscene then
        last_gui_forced_slerp = now
    end

    local slerp_time_diff = now - last_gui_forced_slerp

    if slerp_time_diff <= GUI_MAX_SLERP_TIME then
        if dot_ang >= 10 then
            last_gui_forced_slerp = now
        end

        last_gui_quat = last_gui_quat:slerp(new_gui_quat, dot_dist * math.max((GUI_MAX_SLERP_TIME - slerp_time_diff) * re8vr.delta_time, 0.0))
    end

    if re8vr.is_in_cutscene then
        vrmod:recenter_gui(last_gui_quat)
    else
        vrmod:recenter_gui(last_gui_quat * new_gui_quat:inverse())
    end
end

local last_hmd_active_state = false
local wants_posture_param_restore = false
local modified_posture_param = nil
local original_posture_camera_offset = Vector3f.new(0.0, 0.0, 0.0)
local re8_end_game_events = {
    "c32e390_01",
    "c32e390_02",
    "c32e390_03",
    "c32e390_04",
    "c32e390_05",
    "c32e390_06",
    "c32e390_07",
    "c32e390_08",
    "c32e390_09",
}

for i, v in ipairs(re8_end_game_events) do
    re8_end_game_events[v] = true
end

local function pre_fix_player_camera(player_camera)
end

local function fix_player_camera(player_camera)
    if true then
        re8vr:fix_player_camera(player_camera)
        return
    end

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

        return retval
    end

    re8vr.in_re8_end_game_event = false

    -- Check whether we're in the event at the end of RE8
    -- and return early if we are.
    if is_re8 and re8vr.is_in_cutscene and re8vr.game_event_action_controller ~= nil then
        local event_action = re8vr.game_event_action_controller:get_field("_GameEventAction")

        if event_action ~= nil then
            local event_name = event_action:get_field("_EventName")

            if re8_end_game_events[event_name] ~= nil then
                re8vr.in_re8_end_game_event = true
                return
            end
        end
    end

    last_hmd_active_state = true

    local base_transform_solver = player_camera:get_field("BaseTransSolver")
    local is_maximum_controllable = true

    if base_transform_solver then
        local current_type = base_transform_solver:get_field("<currentType>k__BackingField")

        if is_re8 then
            current_type = current_type:get_field("Value")
            re8vr.has_vehicle = player_camera:get_field("RideVehicleObject") ~= nil

            if re8vr.has_vehicle and re8vr.is_arm_jacked then return end
        end

        if current_type ~= 0 and not re8vr.has_vehicle then -- MaximumOperatable
            re8vr.is_in_cutscene = true
            is_maximum_controllable = false
            last_time_not_maximum_controllable = os.clock()
        else
            if os.clock() - last_time_not_maximum_controllable <= 1.0 then
                re8vr.is_in_cutscene = true
            end

            if re8vr.has_vehicle then
                re8vr.is_in_cutscene = false
            end
        end
    end

    local wants_recenter = false

    if re8vr.is_in_cutscene and not last_cutscene_state then
        --vrmod:recenter_view()

        -- force the gui to be recentered when we exit the cutscene
        last_gui_forced_slerp = os.clock()
        last_gui_quat = Quaternion.identity()
        wants_recenter = true
        --queue_recenter = true

        vrmod:recenter_gui(last_gui_quat)
    elseif not re8vr.is_in_cutscene and last_cutscene_state then
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

    -- Joint is used for the actual final rendering of the game world
    --if not wants_recenter then
    if re8vr.is_in_cutscene then
        joint_set_position:call(camera_joint, camera_pos_pre_hmd)
        joint_set_rotation:call(camera_joint, camera_rot_pre_hmd)
    else
        local rot_delta = camera_rot_pre_hmd:inverse() * camera_rot

        local forward = rot_delta * Vector3f.new(0, 0, 1)
        forward = Vector3f.new(forward.x, 0.0, forward.z):normalized()

        joint_set_position:call(camera_joint, camera_pos_pre_hmd)
        joint_set_rotation:call(camera_joint, camera_rot_pre_hmd * forward:to_quat())

        --camera_rot = camera_rot_pre_hmd * forward:to_quat()
    end

    --last_gui_offset = last_gui_offset * (camera_rot:inverse() * camera_rot_pre_hmd)

    -- just update the body IK right after we update the camera.
    update_body_ik(camera_rot, camera_pos)

    -- Slerp the gui around
    slerp_gui(re8vr.is_in_cutscene and (camera_rot_pre_hmd * camera_rot:inverse()) or vrmod:get_rotation(0):to_quat():inverse())

    local fixed_dir = ((neg_forward_identity * camera_rot_no_shake) * Vector3f.new(0, 0, -1)):normalized()
    local fixed_rot = fixed_dir:to_quat()
    --local fixed_rot = neg_forward_identity * camera_rot

    player_camera:set_field("<CameraRotation>k__BackingField", fixed_rot)
    player_camera:set_field("<CameraPosition>k__BackingField", camera_pos)

    if is_re8 then
        player_camera:set_field("FixedAimRotation", fixed_rot) -- RE8
    end

    player_camera:set_field("CameraRotationWithMovementShake", fixed_rot)
    player_camera:set_field("CameraRotationWithCameraShake", fixed_rot)
    player_camera:set_field("PrevCameraRotation", fixed_rot)
    --player_camera:set_field("CameraPositionWithMovementShake", camera_pos)
    --player_camera:set_field("CameraPositionWithCameraShake", camera_pos)
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

            if re8vr.is_in_cutscene then
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
            
            --if wants_recenter or not re8vr.is_in_cutscene then
            if not re8vr.is_in_cutscene or is_maximum_controllable then
                if not re8vr.is_in_cutscene then
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
    --[[if not re8vr.is_in_cutscene then
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

    if not re8vr.has_vehicle and vrmod:is_using_controllers() and re8vr.weapon ~= nil then
        local pos = re8vr.last_muzzle_pos + (re8vr.last_muzzle_forward * 0.02)
        --local scooted_pos = re8vr.last_muzzle_pos - (re8vr.last_muzzle_forward * 2)
        --local scooted_from = Vector4f.new(scooted_pos.x, scooted_pos.y, scooted_pos.z, 1.0)
        local from = Vector4f.new(pos.x, pos.y, pos.z, 1.0)
        local dir = Vector4f.new(re8vr.last_muzzle_forward.x, re8vr.last_muzzle_forward.y, re8vr.last_muzzle_forward.z, 1.0)

        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "from", from)
        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "dir", dir)

        -- called in LockScene
        --update_crosshair_world_pos(pos, pos + (re8vr.last_muzzle_forward * 1000.0))
    else
        re8vr.last_shoot_pos = camera_pos
        re8vr.last_shoot_dir = fixed_dir

        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "from", camera_pos)
        sdk.set_native_field(sdk.to_ptr(shoot_ray), ray_typedef, "dir", fixed_dir)

        -- called in LockScene
        --update_crosshair_world_pos(camera_pos, camera_pos + (fixed_dir * 1000.0))
    end

    last_cutscene_state = re8vr.is_in_cutscene
end

if is_re8 then
    local on_pre_cannon_calcShootPosRot = function(args)
        if not vrmod:is_hmd_active() then
            return
        end

        if re8vr.is_arm_jacked then
            return
        end

        -- Causes the bullet to come out of the player's face
        -- Not ideal, but it fixes being unable to aim downwards during the tank fight.
        return sdk.PreHookResult.SKIP_ORIGINAL
    end

    local on_post_cannon_calcShootPosRot = function(retval)
        return retval
    end

    sdk.hook(sdk.find_type_definition("app.CannonRoaderCore"):get_method("calcShootPosRot"), on_pre_cannon_calcShootPosRot, on_post_cannon_calcShootPosRot)
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

    re8vr.is_in_cutscene = true
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
    local ch8pcam = sdk.find_type_definition("app.CH8PlayerCamera")

    -- Not a hero camera
    if ch8pcam ~= nil then
        sdk.hook(
            ch8pcam:get_method("lateUpdate"), 
            on_pre_player_camera_update, 
            on_post_player_camera_update
        )
    end

    local ch9pcam = sdk.find_type_definition("app.CH9PlayerCamera")

    -- idk the other DLC?
    if ch9pcam ~= nil then
        sdk.hook(
            ch9pcam:get_method("lateUpdate"), 
            on_pre_player_camera_update, 
            on_post_player_camera_update
        )
    end
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

    -- updating the hand IK here fixes shadows.
    --[[re8vr:update_hand_ik()

    if not re8vr.is_in_cutscene and re8vr.can_use_hands and not re8vr.is_grapple_aim then
        return sdk.PreHookResult.SKIP_ORIGINAL
    end]]

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

local function update_player_gestures()
    re8vr:update_player_gestures()
end

local should_reset_view_no_player = false

re.on_pre_application_entry("UpdateBehavior", function()
    update_player_gestures()

    if not re8vr.player then
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

local last_melee = {os.clock(), os.clock()}
local last_melee_request_state = {false, false}
local swing_history = { 0, 0, 0, 0 }
local swing_index = 0

local function melee_attack(hit_controller)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then return end
    if re8vr.is_in_cutscene or not re8vr.can_use_hands then return end

    if not re8vr.weapon then
        return
    end

    if is_re7 then
        local go = re8vr.weapon:call("get_GameObject")

        if go ~= nil then
            if go:call("get_Name"):find("Timebomb") ~= nil then
                return
            end
        end
    end

    local is_end_of_zoe_melee = is_re7 and re8vr.weapon:get_type_definition():is_a("app.CH9WeaponMelee")

    if not is_end_of_zoe_melee then
        local real_hit_controller = re8vr.weapon:call("get_hitController")

        if not real_hit_controller then
            return
        end
        
        -- RE7
        if hit_controller ~= nil and hit_controller ~= real_hit_controller then
            return
        end
    else
        if re8vr.hit_controller ~= hit_controller then
            return
        end
    end

    local right_controller = vrmod:get_controllers()[2]
    local right_controller_velocity = vrmod:get_velocity(right_controller)
    local right_controller_angular_velocity = vrmod:get_angular_velocity(right_controller)
    local right_controller_speed = right_controller_velocity:length()
    local right_controller_angular_speed = right_controller_angular_velocity:length()

    local left_controller = vrmod:get_controllers()[1]
    local left_controller_velocity = vrmod:get_velocity(left_controller)
    local left_controller_angular_velocity = vrmod:get_angular_velocity(left_controller)
    local left_controller_speed = left_controller_velocity:length()
    local left_controller_angular_speed = left_controller_angular_velocity:length()

    local is_right_swing = right_controller_speed >= 2.2 or right_controller_angular_speed >= 20
    local is_left_swing = is_end_of_zoe_melee and left_controller_speed >= 2.2 or left_controller_angular_speed >= 20

    if not is_left_swing then
        last_melee_request_state[1] = false
    end

    if not is_right_swing then
        last_melee_request_state[2] = false
    end

    if not is_right_swing and not is_left_swing then
        return 
    end

    -- only end of zoe can left swing
    if not is_right_swing and is_left_swing then
        if not is_end_of_zoe_melee then
            return
        end
    end

    local action_grip = vrmod:get_action_grip()
    local right_joystick = vrmod:get_right_joystick()
    local is_stab = vrmod:is_action_active(action_grip, right_joystick)

    local light_cooldown_time = is_end_of_zoe_melee and 0.5 or 0.5
    local cooldown_time = (is_stab and not is_end_of_zoe_melee) and 1.0 or light_cooldown_time
    local now = os.clock()

    if not cfg.no_melee_cooldown then
        if is_left_swing then
            if not last_melee_request_state[1] and now - last_melee[1] < cooldown_time then
                if not is_end_of_zoe_melee then
                    return
                else
                    is_left_swing = false
                end
            end
        end

        if is_right_swing then
            if not last_melee_request_state[2] and now - last_melee[2] < cooldown_time then
                if not is_end_of_zoe_melee then
                    return
                else
                    is_right_swing = false
                end
            end
        end
    end

    local rcol = hit_controller:get_field("RequestSetCollider")

    if not rcol then
        return
    end

    local collider_handler = is_re8 and hit_controller:get_field("RequestedColliderHandler")
    local registered_collider_handler = is_re8 and hit_controller:get_field("RegisteredColliderHandler")

    if is_re8 then
        if not collider_handler or not registered_collider_handler then
            return
        end
    end

    local resource_id = is_re8 and hit_controller:get_field("RequestSetResourceIndex")
    local num_request_sets = is_re8 and rcol:call("getNumRequestSets(System.UInt32)", resource_id) or rcol:call("get_NumRequestSets")

    rcol:call("updatePose")

    if is_left_swing then
        if not last_melee_request_state[1] then
            last_melee[1] = now
        end

        last_melee_request_state[1] = true
    end

    if is_right_swing then
        if not last_melee_request_state[2] then
            last_melee[2] = now
        end

        last_melee_request_state[2] = true
    end
    
    if not is_end_of_zoe_melee then
        for i=0, num_request_sets - 1 do
            if is_re7 then
                local num_collidables = rcol:call("getNumCollidablesFromIndex", i)
                local userdata = rcol:call("getRequestSetUserDataFromIndex", i)

                if userdata ~= nil and userdata:get_type_definition():is_a("app.Collision.ContactBaseUserData") then
                    for j=0, num_collidables - 1 do
                        local collidable = rcol:call("getCollidableFromIndex", i, j)

                        collidable:call("set_Enabled", true)
                        collidable:call("enable")
                        --collidable:call("set_UpdateShape", true)
                    end

                    log.info(tostring(i) .. ": " ..userdata:get_type_definition():get_full_name())
                end

                if not is_stab then
                    hit_controller:call("addManualRequest", 0)
                elseif i > 0 then
                    hit_controller:call("addManualRequest", i)
                end
            else
                local num_collidables = rcol:call("getNumCollidables(System.UInt32, System.UInt32)", resource_id, i)
                local userdata = rcol:call("getRequestSetUserData(System.UInt32, System.UInt32)", resource_id, i)
        
                if userdata:get_type_definition():is_a("app.ContactUserData") then
                    for j=0, num_collidables - 1 do
                        local collidable = rcol:call("getCollidable(System.UInt32, System.UInt32, System.UInt32)", resource_id, i, j)
                        --known_colliders[collidable] = true
        
                        collidable:call("set_Enabled", true)
                        collidable:call("enable")
                        collidable:call("set_UpdateShape", true)
            
                        if not is_stab then
                            hit_controller:call("setManualRequest", 0)
                        elseif i > 0 then
                            hit_controller:call("setManualRequest", i)
                        end
                    end
                end
            end
        end
    else
        --hit_controller:call("addManualRequest", 25) -- fucking SUPER punch!!!

        if is_right_swing then
            hit_controller:call("addManualRequest", 16)
        end

        if is_left_swing then
            hit_controller:call("addManualRequest", 11)
        end
    end

    if is_right_swing then
        if is_stab or is_end_of_zoe_melee then
            vrmod:trigger_haptic_vibration(0.0, 0.1, 1, 5, vrmod:get_right_joystick())
        else
            vrmod:trigger_haptic_vibration(0.0, 0.1, 1, 0.5, vrmod:get_right_joystick())
        end
    end
    
    if is_left_swing then
        vrmod:trigger_haptic_vibration(0.0, 0.1, 1, 5, vrmod:get_left_joystick())
    end
end

local hit_controller_args = nil

local function on_pre_hit_controller_update(args)
    hit_controller_args = args
end

local function on_post_hit_controller_update(args)
    if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() then return end
    if re8vr.is_in_cutscene or not re8vr.can_use_hands then return end

    local hit_controller = sdk.to_managed_object(hit_controller_args[2])

    melee_attack(hit_controller)
end

if is_re7 then
    sdk.hook(sdk.find_type_definition("app.Collision.HitController"):get_method("update"), on_pre_hit_controller_update, on_post_hit_controller_update)
else
    sdk.hook(sdk.find_type_definition("app.HitController"):get_method("update"), on_pre_hit_controller_update, on_post_hit_controller_update)
end

re.on_application_entry("LateUpdateBehavior", function()
    update_hand_ik()
end)

re.on_application_entry("UpdateBehavior", function()
end)

local do_once = true

re.on_application_entry("UpdateHID", function()
    --[[local padman = sdk.get_managed_singleton(sdk.game_namespace("PadManager"))

    if padman then
        update_padman(padman)
    end]]
end)

re.on_application_entry("LockScene", function()
    if not vrmod:is_hmd_active() then return end

    update_muzzle_data()

    if re8vr.last_shoot_pos then
        local pos = re8vr.last_shoot_pos + (re8vr.last_shoot_dir * 0.02)
        update_crosshair_world_pos(pos, pos + (re8vr.last_shoot_dir * 1000.0))
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
local last_seen_player = nil

re.on_pre_application_entry("UnlockScene", function()
    if queue_recenter then
        vrmod:recenter_view()
        queue_recenter = false
    end

    if not vrmod:is_hmd_active() then last_roomscale_failure = os.clock() return end

    last_camera_matrix = vrmod:get_last_render_matrix()

    if not re8vr.player or not re8vr.transform then last_roomscale_failure = os.clock() return end
    if not re8vr.status then last_roomscale_failure = os.clock() return end -- in the main menu or something.
    if not last_camera_matrix then last_roomscale_failure = os.clock() return end
    if re8vr.is_in_cutscene then last_roomscale_failure = os.clock() return end
    if re8vr.is_grapple_aim then last_roomscale_failure = os.clock() return end

    if re8vr.player ~= last_seen_player then
        last_roomscale_failure = os.clock()
        last_seen_player = re8vr.player
    end

    if os.clock() - last_roomscale_failure < 1.0 then return end

    local standing_origin = vrmod:get_standing_origin()
    local hmd_pos = vrmod:get_position(0)

    hmd_pos.y = 0.0
    standing_origin.y = 0.0

    if (hmd_pos - standing_origin):length() >= 0.01 and cfg.roomscale_movement then
        standing_origin = vrmod:get_standing_origin()
        hmd_pos.y = standing_origin.y

        local old_standing_origin = Vector4f.new(standing_origin.x, standing_origin.y, standing_origin.z, standing_origin.w)

        standing_origin = standing_origin:lerp(hmd_pos, (hmd_pos - standing_origin):length() * 0.1)

        local standing_diff = standing_origin - old_standing_origin

        vrmod:set_standing_origin(standing_origin)

        local player_pos = transform_get_position:call(re8vr.transform)
        local lerp_to = Vector3f.new(last_camera_matrix[3].x, player_pos.y, last_camera_matrix[3].z)

        player_pos = player_pos + ((lerp_to - player_pos):normalized() * standing_diff:length())
        --player_pos:lerp(lerp_to, 0.1)
        --player_pos.x = last_camera_matrix[3].x
        --player_pos.z = last_camera_matrix[3].z
        
        --transform_set_position:call(re8vr.transform, player_pos)
        re8vr.transform:set_position(player_pos, true) -- NO DIRTY
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
    elseif re8vr.was_gripping_weapon then
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

    if duration == 0 then
        duration = 0.1
    end

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
    local method = vibrationmanager_t:get_method("requestAdd(app.VibrationParam, System.Single)")

    if method == nil then
        method = vibrationmanager_t:get_method("requestAdd(app.VibrationParam, System.Single, System.Boolean)")
    end

    sdk.hook(method, re7_on_pre_request_add_vibration, re7_on_post_request_add_vibration)
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
local debug_hit_controller = false
local debug_hands = false

re.on_frame(function()
    if debug_hands and vrmod:is_using_controllers() then
        local controllers = vrmod:get_controllers()
        local left_index = controllers[1]
        local right_index = controllers[2]

        local left_transform = re8vr.last_left_hand_rotation:to_mat4()
        local right_transform = re8vr.last_right_hand_rotation:to_mat4()

        left_transform[3] = re8vr.last_left_hand_position
        right_transform[3] = re8vr.last_right_hand_position

        draw.matrix44(left_transform)
        draw.matrix44(right_transform)
    end
end)

re.on_draw_ui(function()
    local changed = false
    
    changed, cfg.movement_shake = imgui.checkbox("Movement Shake", cfg.movement_shake)
    changed, cfg.all_camera_shake = imgui.checkbox("All Other Camera Shakes", cfg.all_camera_shake)
    changed, cfg.disable_crosshair = imgui.checkbox("Disable Crosshair", cfg.disable_crosshair)
    changed, cfg.roomscale_movement = imgui.checkbox("Roomscale Movement", cfg.roomscale_movement)

    if imgui.tree_node("Cheats") then
        changed, cfg.no_melee_cooldown = imgui.checkbox("No Melee Cooldown", cfg.no_melee_cooldown)

        imgui.tree_pop()
    end

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
        changed, debug_hands = imgui.checkbox("Debug Hands", debug_hands)

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
                    re8vr.right_hand_rotation_vec.x = right_hand_rotation_vec.x + (right_axis.y * 0.001)
                    re8vr.right_hand_rotation_vec.y = right_hand_rotation_vec.y + (right_axis.x * 0.001)
                else
                    re8vr.right_hand_rotation_vec.z = right_hand_rotation_vec.z + ((right_axis.y + right_axis.x) * 0.001)
                end
            else
                if not is_right_grip_active then
                    re8vr.right_hand_position_offset.x = right_hand_position_offset.x + (right_axis.y * 0.001)
                    re8vr.right_hand_position_offset.y = right_hand_position_offset.y + (right_axis.x * 0.001)
                else
                    re8vr.right_hand_position_offset.z = right_hand_position_offset.z + ((right_axis.y + right_axis.x) * 0.001)
                end
            end

            re8vr.right_hand_rotation_offset = Quaternion.new(right_hand_rotation_vec):normalized()

            if not is_left_trigger_active then
                if not is_left_grip_active then
                    re8vr.left_hand_rotation_vec.x = left_hand_rotation_vec.x + (left_axis.y * 0.001)
                    re8vr.left_hand_rotation_vec.y = left_hand_rotation_vec.y + (left_axis.x * 0.001)
                else
                    re8vr.left_hand_rotation_vec.z = left_hand_rotation_vec.z + ((left_axis.y + left_axis.x) * 0.001)
                end
            else
                if not is_left_grip_active then
                    re8vr.left_hand_position_offset.x = left_hand_position_offset.x + (left_axis.y * 0.001)
                    re8vr.left_hand_position_offset.y = left_hand_position_offset.y + (left_axis.x * 0.001)
                else
                    re8vr.left_hand_position_offset.z = left_hand_position_offset.z + ((left_axis.y + left_axis.x) * 0.001)
                end
            end

            re8vr.left_hand_rotation_offset = Quaternion.new(left_hand_rotation_vec):normalized()
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
            object_explorer:handle_address(re8vr.player)

            imgui.tree_pop()
        end

        if imgui.tree_node("Status") then
            object_explorer:handle_address(re8vr.status)

            imgui.tree_pop()
        end

        if imgui.tree_node("Updater") then
            object_explorer:handle_address(re8vr.updater)

            imgui.tree_pop()
        end

        if imgui.tree_node("Inventory") then
            object_explorer:handle_address(re8vr.inventory)

            imgui.tree_pop()
        end

        if imgui.tree_node("Right Hand IK") then
            local right_hand_ik = re8vr.right_hand_ik
    
            object_explorer:handle_address(right_hand_ik)
    
            imgui.tree_pop()
        end
    
        if imgui.tree_node("Left Hand IK") then
            local left_hand_ik = re8vr.left_hand_ik
    
            object_explorer:handle_address(left_hand_ik)
    
            imgui.tree_pop()
        end

        if imgui.tree_node("Weapon") then
            local weapon = re8vr.weapon
    
            object_explorer:handle_address(weapon)
    
            imgui.tree_pop()
        end

        if imgui.tree_node("Shadow") then
            --if not vrmod:is_using_controllers() then return end
        
            if is_re7 then
                local mesh_controller = re8vr.player and GameObject.get_component(re8vr.player, "app.PlayerMeshController") or nil

                if mesh_controller then
                    local upper_shadow_mesh = mesh_controller:get_field("UpperBodyShadowMesh")

                    if upper_shadow_mesh then
                
                        local shadow_body_gameobject = upper_shadow_mesh:call("get_GameObject")
                        if shadow_body_gameobject then
                            object_explorer:handle_address(shadow_body_gameobject)
                        end
                    end
                end
            end
        end

        if imgui.tree_node("Hit Controller") then
            local hit_controller = re8vr.hit_controller

            changed, debug_hit_controller = imgui.checkbox("Debug hit controller", debug_hit_controller)

            if debug_hit_controller then
                local rcol = hit_controller:get_field("RequestSetCollider")

                if not rcol then
                    return
                end

                local num_request_sets = is_re8 and rcol:call("getNumRequestSets(System.UInt32)", resource_id) or rcol:call("get_NumRequestSets")
            
                
                for i=0, num_request_sets - 1 do
                    if is_re7 then
                        
                        local num_collidables = rcol:call("getNumCollidablesFromIndex", i)
                        local userdata = rcol:call("getRequestSetUserDataFromIndex", i)
            
                        if userdata ~= nil and userdata:get_type_definition():is_a("app.Collision.ContactBaseUserData") then
                            for j=0, num_collidables - 1 do
                                local collidable = rcol:call("getCollidableFromIndex", i, j)

                                if collidable then
                                    local shape = collidable:call("get_TransformedShape")

                                    if imgui.button(tostring(i) .. ", " .. tostring(j) .. ": " .. shape:get_type_definition():get_full_name()) then
                                        hit_controller:call("addManualRequest", i)
                                    end

                                    if shape:get_type_definition():is_a("via.physics.CapsuleShape") then
                                        local pos = shape:call("get_PosA")
                                        local pos_screen = draw.world_to_screen(pos)

                                        if pos_screen then
                                            draw.text(tostring(i), pos_screen.x, pos_screen.y, 0xffffffff)
                                        end
                                    end
                                end
                            end
                        end
                    end
                end
            end
    
            object_explorer:handle_address(hit_controller)
    
            imgui.tree_pop()
        end

        if imgui.tree_node("Weapon Colliders") then
            local i = 0
            for collider, _ in pairs(known_colliders) do
                if imgui.tree_node(tostring(i)) then
                    object_explorer:handle_address(collider)
                    imgui.tree_pop()
                end
                i = i + 1
            end
    
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
        imgui.text("Is arm jacked: " .. tostring(re8vr.is_arm_jacked))
        imgui.text("Is motion play: " .. tostring(re8vr.is_motion_play))
        imgui.text("Is in cutscene: " .. tostring(re8vr.is_in_cutscene))
        imgui.text("Can use hands: " .. tostring(re8vr.can_use_hands))
        imgui.text("Is grapple aim: " .. tostring(re8vr.is_grapple_aim))
        imgui.text("In RE8 end game event: " .. tostring(re8vr.in_re8_end_game_event))
        imgui.text("Has vehicle: " .. tostring(re8vr.has_vehicle))


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
    "GUIShopBg",
    "GUIFigureList",
    "GUIBonusMenu",
    "GUIMainMenu",
    "GUIPhotoMode",
    "GUIPause",
    "GUISaveLoad",
    "GUIBinder"
}

local re7_inventory_names = {
    "CH9PauseMenu",
    "CH9MapMaskGUI",
    "CH9MultiSubMenu",
    "PauseMenu",
    "MapMaskGUI",
    "MultiSubMenu",
    "FileMenu",
}

local re8_book_names = {
    "GUIBook"
}

-- UI elements that require use of the dpad up/down
local re7_map_names = {
    "MapMaskGUI",
    "Ch9MapMaskGUI"
}

local re8_map_names = {
    "GUIMap",
    "GUIBinder"
}

for i, v in ipairs(re8_inventory_names) do
    re8_inventory_names[v] = true
end

for i, v in ipairs(re7_inventory_names) do
    re7_inventory_names[v] = true
end

for i, v in ipairs(re8_book_names) do
    re8_book_names[v] = true
end

for i, v in ipairs(re7_map_names) do
    re7_map_names[v] = true
end

for i, v in ipairs(re8_map_names) do
    re8_map_names[v] = true
end

local reticle_names = {
    "ReticleGUI",
    "CH8ReticleGUI",
    "CH9ReticleGUI",
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

                if tdb_version >= 69 then
                    write_vec4(transform, new_mat[0] * distance, 0x80)
                    write_vec4(transform, new_mat[1] * distance, 0x90)
                    write_vec4(transform, new_mat[2] * distance, 0xA0)
                    write_vec4(transform, crosshair_pos, 0xB0)
                else
                    write_vec4(transform, new_mat[0] * distance, 0x90)
                    write_vec4(transform, new_mat[1] * distance, 0xA0)
                    write_vec4(transform, new_mat[2] * distance, 0xB0)
                    write_vec4(transform, crosshair_pos, 0xC0)
                end
            end
        end
    end

    if re8vr.player == nil or re8_inventory_names[name] or re7_inventory_names[name] then
        last_inventory_open_time = os.clock()
    end

    if is_re8 and re8_book_names[name] then
        last_book_open_time = os.clock()
    end

    if (is_re7 and re7_map_names[name]) or (is_re8 and re8_map_names[name]) then
        last_map_open_time = os.clock()
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