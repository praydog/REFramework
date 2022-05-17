if _re7lib ~= nil then
    return _re7lib
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

local function initialize_re7(re7)
    re7 = re7 or {}

    re7.player = nil
    re7.transform = nil
    re7.weapon = nil
    re7.weapon_gameobject = nil
    re7.inventory = nil
    re7.hand_touch = nil
    re7.order = nil
    re7.right_hand_ik = nil
    re7.left_hand_ik = nil
    re7.is_in_cutscene = false
    re7.is_arm_jacked = false
    re7.is_grapple_aim = false
    re7.event_action_controller = nil
    re7.wants_block = false
    re7.movement_speed_rate = 0.0
    re7.movement_speed_vector = Vector3f.new(0, 0, 0)
    re7.num_active_tasks = 0
    re7.active_tasks = {}
    re7.application = sdk.get_native_singleton("via.Application")
    re7.application_type = sdk.find_type_definition("via.Application")
    re7.delta_time = 0.0

    return re7
end

local re7 = initialize_re7()

local known_typeofs = {}

local function get_component(game_object, type_name)
    local t = known_typeofs[type_name] or sdk.typeof(type_name)

    if t == nil then 
        return nil
    end

    known_typeofs[type_name] = t
    return game_object:call("getComponent(System.Type)", t)
end

function re7.get_localplayer()
    local object_man = sdk.get_managed_singleton("app.ObjectManager")

    if not object_man then
        return nil
    end

    return object_man:get_field("PlayerObj")
end

function re7.get_weapon_object(player)
    return nil, nil
end

re.on_pre_application_entry("UpdateBehavior", function()
    re7.player = re7.get_localplayer()
    local player = re7.player

    if player == nil or not re7.application then
        initialize_re7(re7)
        return
    end

    re7.transform = player:call("get_Transform")
    re7.inventory = get_component(player, "app.Inventory")
    re7.hand_touch = get_component(player, "app.PlayerHandTouch")
    re7.order = get_component(player, "app.PlayerOrder")
    re7.delta_time = sdk.call_native_func(re7.application, re7.application_type, "get_DeltaTime")

    if re7.order ~= nil then
        re7.is_grapple_aim = re7.order:get_field("IsGrappleAimEnable")
    end

    if re7.hand_touch == nil then
        re7.right_hand_ik = nil
        re7.left_hand_ik = nil
    else
        local hand_ik = re7.hand_touch:get_field("HandIK"):get_elements()

        if #hand_ik < 2 then
            log.info("no hand ik")
            re7.right_hand_ik = nil
            re7.left_hand_ik = nil
        else
            --log.info("IK: " .. tostring(hand_ik))

            re7.right_hand_ik = hand_ik[1]
            re7.left_hand_ik = hand_ik[2]

            if re7.right_hand_ik and re7.left_hand_ik then
                re7.right_hand_ik_object = re7.right_hand_ik:get_field("TargetGameObject")
                re7.left_hand_ik_object = re7.left_hand_ik:get_field("TargetGameObject") 
                re7.right_hand_ik_transform = re7.right_hand_ik:get_field("Target")
                re7.left_hand_ik_transform = re7.left_hand_ik:get_field("Target")
            end

            --re7.is_in_cutscene = false
        end
    end

    re7.event_action_controller = get_component(player, "app.EventActionController")

    if re7.event_action_controller ~= nil then
        local current_task = re7.event_action_controller:get_field("CurrentTask")

        if current_task ~= nil then
            --re7.is_in_cutscene = true
        else
            --re7.is_in_cutscene = false
        end
    else
        re7.is_in_cutscene = false
    end

    if re7.inventory == nil then
        re7.weapon = nil
        re7.weapon_gameobject = nil
        return
    end

    local weapon_gameobject, weapon = re7.get_weapon_object(player)

    if weapon_gameobject == nil or weapon == nil then
        re7.weapon = nil
        re7.weapon_gameobject = nil
        return
    end

    re7.weapon = weapon
    re7.weapon_gameobject = weapon_gameobject
end)

local event_action_controller_type = sdk.find_type_definition("app.EventActionController")
local request_task_method = event_action_controller_type:get_method("requestTask")

local function on_pre_event_request_task(args)
    if re7.event_action_controller == nil or sdk.to_ptr(args[2]) == nil or sdk.to_int64(args[2]) ~= re7.event_action_controller:get_address() then
        return sdk.PreHookResult.CALL_ORIGINAL
    end

    local controller = sdk.to_managed_object(args[2])
    local task = sdk.to_managed_object(args[3])

    if task == nil then
        log.debug("No task!")
        return sdk.PreHookResult.CALL_ORIGINAL
    end

    if not re7.active_tasks[task] then
        re7.num_active_tasks = re7.num_active_tasks + 1
    end

    re7.is_in_cutscene = true
    re7.active_tasks[task] = true

    callbacks["event_task_create"]:dispatch(args)
end

local function on_post_event_request_task(retval)
    return retval
end

sdk.hook(request_task_method, on_pre_event_request_task, on_post_event_request_task)

local event_action_task_type = sdk.find_type_definition("app.EventActionTask")
local terminate_method = event_action_task_type:get_method("terminate")

local function on_pre_task_terminate(args)
    local task = sdk.to_managed_object(args[2])

    if task == nil or not re7.active_tasks[task] then
        return
    end

    if re7.active_tasks[task] then
        re7.num_active_tasks = re7.num_active_tasks - 1
        re7.active_tasks[task] = nil
    end

    if re7.num_active_tasks < 0 then
        re7.num_active_tasks = 0
    end

    re7.is_in_cutscene = re7.num_active_tasks > 0 or not re7.has_postural_camera_control or re7.is_arm_jacked

    callbacks["event_task_terminate"]:dispatch(args)
end

local function on_post_task_terminate(retval)
    return retval
end

sdk.hook(terminate_method, on_pre_task_terminate, on_post_task_terminate)

local player_motion_controller_type = sdk.find_type_definition("app.PlayerMotionController")
local update_postural_camera_motion_method = player_motion_controller_type:get_method("updatePosturalCameraMotion")
local ch8_player_motion_controller_type = sdk.find_type_definition("app.CH8PlayerMotionController")
local ch8_update_postural_camera_motion_method = ch8_player_motion_controller_type:get_method("updatePosturalCameraMotion")
local ch9_player_motion_controller_type = sdk.find_type_definition("app.CH9PlayerMotionController")
local ch9_update_postural_camera_motion_method = ch9_player_motion_controller_type:get_method("updatePosturalCameraMotion")

local postural_camera_motion_args = nil

local function on_pre_update_postural_camera_motion(args)
    postural_camera_motion_args = args
end

local function on_post_update_postural_camera_motion(retval)
    local args = postural_camera_motion_args
    local controller = sdk.to_managed_object(args[2])
    local game_object = controller:call("get_GameObject")

    if game_object ~= re7.player then
        return retval
    end

    re7.is_arm_jacked = controller:get_field("IsRArmJacked")
    re7.has_postural_camera_control = controller:get_field("IsPosturalCameraControl")
    re7.is_in_cutscene = re7.num_active_tasks > 0 or not re7.has_postural_camera_control or re7.is_arm_jacked

    return retval
end

-- ethan
sdk.hook(update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
-- chris
sdk.hook(ch8_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
-- ch9 (end of zoe?)
sdk.hook(ch9_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)

local player_movement_type = sdk.find_type_definition("app.PlayerMovement")
local player_movement_late_update_method = player_movement_type:get_method("doLateUpdate")
local ch8_player_movement_type = sdk.find_type_definition("app.CH8PlayerMovement")
local ch8_player_movement_late_update_method = ch8_player_movement_type:get_method("doLateUpdate")
local ch9_player_movement_type = sdk.find_type_definition("app.CH9PlayerMovement")
local ch9_player_movement_late_update_method = ch9_player_movement_type:get_method("doLateUpdate")

local player_movement_args = nil

local function on_pre_player_movement_late_update(args)
    player_movement_args = args
end

local function on_post_player_movement_late_update(retval)
    local args = player_movement_args
    local movement = sdk.to_managed_object(args[2])

    re7.movement_speed_rate = movement:get_field("_SpeedRate")
    re7.movement_speed_vector = movement:get_field("_MoveSpeedVector")

    return retval
end

sdk.hook(player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
sdk.hook(ch8_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
sdk.hook(ch9_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)

function re7.notify_event_task_created(callback)
    callbacks["event_task_create"]:add(callback)
end

function re7.notify_event_task_terminated(callback)
    callbacks["event_task_terminate"]:add(callback)
end

_re7lib = re7

return re7