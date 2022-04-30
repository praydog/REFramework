if _re8lib ~= nil then
    return _re8lib
end

local game_name = reframework:get_game_name()
local is_re7 = game_name == "re7"
local is_re8 = game_name == "re8"

if not is_re7 and not is_re8 then
    error("Unsupported game: " .. game_name)
end

local CallbackList = {
    callbacks = {},

    new = function(self, o)
        o = o or {}

        self.__index = self
        return setmetatable(o, self)
    end,

    add = function(self, callback)
        table.insert(self.callbacks, callback)
    end,

    dispatch = function(self, args)
        for i, callback in ipairs(self.callbacks) do
            callback(args)
        end
    end
}

local CallbackManager = {
    callback_lists = {},

    new = function(self, o)
        o = o or {}

        return setmetatable(o, self)
    end,

    __index = function(self, key)
        local rawval = rawget(self, key) or rawget(getmetatable(self), key)

        if rawval ~= nil then
            return rawval
        end

        if not self.callback_lists[key] then
            self.callback_lists[key] = CallbackList:new()
        end

        return self.callback_lists[key]
    end,
}

local callbacks = CallbackManager:new()

local function initialize_re8(re8)
    re8 = re8 or {}

    re8.player = nil
    re8.transform = nil
    re8.weapon = nil
    re8.weapon_gameobject = nil
    re8.inventory = nil
    re8.hand_touch = nil
    re8.order = nil
    re8.right_hand_ik = nil
    re8.left_hand_ik = nil
    re8.right_hand_ik_transform = nil
    re8.left_hand_ik_transform = nil
    re8.is_in_cutscene = false
    re8.is_arm_jacked = false
    re8.is_grapple_aim = false
    re8.is_motion_play = false
    re8.is_reloading = false
    re8.has_postural_camera_control = true
    re8.can_use_hands = true
    re8.updater = nil
    re8.status = nil
    re8.event_action_controller = nil
    re8.game_event_action_controller = nil
    re8.wants_block = false
    re8.movement_speed_rate = 0.0
    re8.movement_speed_vector = Vector3f.new(0, 0, 0)
    re8.num_active_tasks = 0
    re8.active_tasks = {}
    re8.application = sdk.get_native_singleton("via.Application")
    re8.application_type = sdk.find_type_definition("via.Application")
    re8.delta_time = 0.0

    return re8
end

local re8 = initialize_re8()

local known_typeofs = {}
local known_invalids = {}

local function get_component(game_object, type_name)
    if known_invalids[type_name] then
        return nil
    end

    local t = known_typeofs[type_name] or sdk.typeof(type_name)

    if t == nil then
        known_invalids[type_name] = true
        return nil
    end

    known_typeofs[type_name] = t
    return game_object:call("getComponent(System.Type)", t)
end

if is_re7 then
    re8.get_localplayer = function()
        local object_man = sdk.get_managed_singleton("app.ObjectManager")

        if not object_man then
            return nil
        end

        return object_man:get_field("PlayerObj")
    end
elseif is_re8 then
    re8.get_localplayer = function()
        local propsman = sdk.get_managed_singleton("app.PropsManager")

        if not propsman then
            return nil
        end

        return propsman:get_field("<Player>k__BackingField")
    end
end

function re8.get_weapon_object(player)
    if is_re7 then
        local player_gun = get_component(re8.player, "app.PlayerGun")

        if not player_gun then
            return nil
        end

        local equipped_weapon = player_gun:get_field("WeaponGun")
    
        if not equipped_weapon then
            return nil
        end

        return equipped_weapon
    elseif is_re8 then
        if not re8.updater then
            return nil
        end

        local player_gun = re8.updater:call("get_playerGun")

        if not player_gun then
            return nil
        end

        local equipped_weapon = player_gun:call("get_equipWeaponObject")
    
        if not equipped_weapon then
            return nil
        end

        return equipped_weapon
    end

    return nil
end

function re8.update_in_cutscene_state()
    re8.is_in_cutscene = re8.num_active_tasks > 0 or not re8.has_postural_camera_control or re8.is_arm_jacked or re8.is_motion_play
    re8.can_use_hands = not re8.is_arm_jacked and not re8.is_motion_play
end

re.on_pre_application_entry("UpdateBehavior", function()
    re8.player = re8.get_localplayer()
    local player = re8.player

    if player == nil or not re8.application then
        initialize_re8(re8)
        return
    end

    re8.transform = player:call("get_Transform")
    re8.hand_touch = get_component(player, "app.PlayerHandTouch")
    re8.order = get_component(player, "app.PlayerOrder")
    re8.updater = get_component(player, "app.PlayerUpdater")
    
    if is_re7 then
        re8.inventory = get_component(player, "app.Inventory")
    elseif re8.updater ~= nil then
        local container = re8.updater:get_field("playerContainer")

        if container then
            re8.inventory = container:call("get_inventory")
        else
            re8.inventory = nil
        end
    else
        re8.inventory = nil
    end

    re8.delta_time = sdk.call_native_func(re8.application, re8.application_type, "get_DeltaTime")

    if is_re7 then
        re8.status = get_component(player, "app.PlayerStatus")
    elseif re8.updater ~= nil then
        re8.status = re8.updater:call("get_playerstatus")
    end

    if re8.status ~= nil then
        re8.is_reloading = re8.status:call("get_isReload")
    end

    if re8.order ~= nil then
        re8.is_grapple_aim = re8.order:get_field("IsGrappleAimEnable")
    end

    if re8.hand_touch == nil then
        re8.right_hand_ik = nil
        re8.left_hand_ik = nil
    else
        local hand_ik = re8.hand_touch:get_field("HandIK"):get_elements()

        if #hand_ik < 2 then
            log.info("no hand ik")
            re8.right_hand_ik = nil
            re8.left_hand_ik = nil
        else
            re8.right_hand_ik = hand_ik[1]
            re8.left_hand_ik = hand_ik[2]

            if re8.right_hand_ik and re8.left_hand_ik then
                re8.right_hand_ik_object = re8.right_hand_ik:get_field("TargetGameObject")
                re8.left_hand_ik_object = re8.left_hand_ik:get_field("TargetGameObject") 
                re8.right_hand_ik_transform = re8.right_hand_ik:get_field("Target")
                re8.left_hand_ik_transform = re8.left_hand_ik:get_field("Target")
            end
        end
    end

    re8.event_action_controller = get_component(player, "app.EventActionController")
    re8.game_event_action_controller = get_component(player, "app.GameEventActionController")

    if is_re8 and re8.game_event_action_controller ~= nil then
        re8.is_motion_play = re8.game_event_action_controller:get_field("_isMotionPlay")
    else
        re8.is_motion_play = false
    end

    local weapon = re8.get_weapon_object(player)

    if weapon == nil then
        re8.weapon = nil
        return
    end

    re8.weapon = weapon

    re8.update_in_cutscene_state()
end)

local event_action_controller_type = sdk.find_type_definition("app.EventActionController")
local request_task_method = event_action_controller_type:get_method("requestTask")

local function on_pre_event_request_task(args)
    if re8.event_action_controller == nil or sdk.to_ptr(args[2]) == nil or sdk.to_int64(args[2]) ~= re8.event_action_controller:get_address() then
        return sdk.PreHookResult.CALL_ORIGINAL
    end

    local controller = sdk.to_managed_object(args[2])
    local task = sdk.to_managed_object(args[3])

    if task == nil then
        log.debug("No task!")
        return sdk.PreHookResult.CALL_ORIGINAL
    end

    if not re8.active_tasks[task] then
        re8.num_active_tasks = re8.num_active_tasks + 1
    end

    re8.active_tasks[task] = true
    re8.update_in_cutscene_state()

    callbacks["event_task_create"]:dispatch(args)
end

local function on_post_event_request_task(retval)
    return retval
end

if is_re7 then
    sdk.hook(request_task_method, on_pre_event_request_task, on_post_event_request_task)
end

local event_action_task_type = sdk.find_type_definition("app.EventActionTask")
local terminate_method = event_action_task_type:get_method("terminate")

local function on_pre_task_terminate(args)
    local task = sdk.to_managed_object(args[2])

    if task == nil or not re8.active_tasks[task] then
        return
    end

    if re8.active_tasks[task] then
        re8.num_active_tasks = re8.num_active_tasks - 1
        re8.active_tasks[task] = nil
    end

    if re8.num_active_tasks < 0 then
        re8.num_active_tasks = 0
    end

    re8.update_in_cutscene_state()

    callbacks["event_task_terminate"]:dispatch(args)
end

local function on_post_task_terminate(retval)
    return retval
end

if is_re7 then
    sdk.hook(terminate_method, on_pre_task_terminate, on_post_task_terminate)
end

local postural_camera_motion_args = nil

local function on_pre_update_postural_camera_motion(args)
    postural_camera_motion_args = args
end

local function on_post_update_postural_camera_motion(retval)
    local args = postural_camera_motion_args
    local controller = sdk.to_managed_object(args[2])

    if is_re7 then
        local game_object = controller:call("get_GameObject")

        if game_object ~= re8.player then
            return retval
        end
    else
        local motion = controller:get_field("Motion")

        if motion == nil then
            return retval
        end

        local game_object = motion:call("get_GameObject")

        if game_object ~= re8.player then
            return retval
        end
    end

    re8.is_arm_jacked = controller:get_field("IsRArmJacked")
    re8.has_postural_camera_control = controller:get_field("IsPosturalCameraControl")
    re8.update_in_cutscene_state()

    return retval
end

local player_motion_controller_type = sdk.find_type_definition("app.PlayerMotionController")
local update_postural_camera_motion_method = player_motion_controller_type:get_method("updatePosturalCameraMotion")

-- ethan
sdk.hook(update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)

if is_re7 then
    local ch8_player_motion_controller_type = sdk.find_type_definition("app.CH8PlayerMotionController")
    local ch8_update_postural_camera_motion_method = ch8_player_motion_controller_type:get_method("updatePosturalCameraMotion")
    local ch9_player_motion_controller_type = sdk.find_type_definition("app.CH9PlayerMotionController")
    local ch9_update_postural_camera_motion_method = ch9_player_motion_controller_type:get_method("updatePosturalCameraMotion")

    -- chris
    sdk.hook(ch8_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
    -- ch9 (end of zoe?)
    sdk.hook(ch9_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
end

local player_movement_args = nil

local function on_pre_player_movement_late_update(args)
    player_movement_args = args
end

local function re7_on_post_player_movement_late_update(retval)
    local args = player_movement_args
    local movement = sdk.to_managed_object(args[2])

    re8.movement_speed_rate = movement:get_field("_SpeedRate")
    re8.movement_speed_vector = movement:get_field("_MoveSpeedVector")

    return retval
end

local function re8_on_post_player_movement_late_update(retval)
    local args = player_movement_args
    local movement = sdk.to_managed_object(args[2])

    re8.movement_speed_rate = movement:get_field("SpeedRate")
    re8.movement_speed_vector = movement:get_field("MoveSpeedVector")

    return retval
end

local player_movement_type = sdk.find_type_definition("app.PlayerMovement")
local player_movement_late_update_method = player_movement_type:get_method("doLateUpdate")

if player_movement_late_update_method == nil then
    player_movement_late_update_method = player_movement_type:get_method("lateUpdate")
end

if is_re7 then
    sdk.hook(player_movement_late_update_method, on_pre_player_movement_late_update, re7_on_post_player_movement_late_update)
else
    sdk.hook(player_movement_late_update_method, on_pre_player_movement_late_update, re8_on_post_player_movement_late_update)
end

if is_re7 then
    local ch8_player_movement_type = sdk.find_type_definition("app.CH8PlayerMovement")
    local ch8_player_movement_late_update_method = ch8_player_movement_type:get_method("doLateUpdate")
    local ch9_player_movement_type = sdk.find_type_definition("app.CH9PlayerMovement")
    local ch9_player_movement_late_update_method = ch9_player_movement_type:get_method("doLateUpdate")

    sdk.hook(ch8_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
    sdk.hook(ch9_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
end

function re8.notify_event_task_created(callback)
    callbacks["event_task_create"]:add(callback)
end

function re8.notify_event_task_terminated(callback)
    callbacks["event_task_terminate"]:add(callback)
end

_re8lib = re8

return re8