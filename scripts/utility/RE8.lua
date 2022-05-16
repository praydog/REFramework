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

    re8vr.player = nil
    re8vr.transform = nil
    re8vr.weapon = nil
    re8vr.inventory = nil
    re8vr.hand_touch = nil
    re8vr.order = nil
    re8vr.right_hand_ik = nil
    re8vr.left_hand_ik = nil
    re8vr.right_hand_ik_transform = nil
    re8vr.left_hand_ik_transform = nil
    re8vr.is_in_cutscene = false
    re8vr.is_arm_jacked = false
    re8vr.is_grapple_aim = false
    re8vr.is_motion_play = false
    re8vr.is_reloading = false
    re8.has_postural_camera_control = true
    re8vr.can_use_hands = true
    re8vr.updater = nil
    re8vr.status = nil
    re8vr.event_action_controller = nil
    re8vr.game_event_action_controller = nil
    re8vr.hit_controller = nil
    re8vr.wants_block = false
    re8vr.wants_heal = false
    re8vr.movement_speed_rate = 0.0
    re8.movement_speed_vector = Vector3f.new(0, 0, 0)
    re8.num_active_tasks = 0
    re8.active_tasks = {}
    re8.application = sdk.get_native_singleton("via.Application")
    re8.application_type = sdk.find_type_definition("via.Application")
    re8vr.delta_time = 0.0

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

function re8.get_localplayer()
    return re8vr:get_localplayer()
end

function re8.get_weapon_object(player)
    return re8vr:get_weapon_object(player)
end

function re8.update_in_cutscene_state()
    re8vr.is_in_cutscene = re8.num_active_tasks > 0 or not re8.has_postural_camera_control or re8vr.is_arm_jacked or re8vr.is_motion_play
    re8vr.can_use_hands = not re8vr.is_arm_jacked and not re8vr.is_motion_play
end

re.on_pre_application_entry("UpdateBehavior", function()
    if not re8vr:update_pointers() or not re8.application then
        initialize_re8(re8)
        return
    end

    re8.update_in_cutscene_state()
end)

local event_action_controller_type = sdk.find_type_definition("app.EventActionController")
local request_task_method = event_action_controller_type:get_method("requestTask")

local function on_pre_event_request_task(args)
    if re8vr.event_action_controller == nil or sdk.to_ptr(args[2]) == nil or sdk.to_int64(args[2]) ~= re8vr.event_action_controller:get_address() then
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

        if game_object ~= re8vr.player then
            return retval
        end
    else
        local motion = controller:get_field("Motion")

        if motion == nil then
            return retval
        end

        local game_object = motion:call("get_GameObject")

        if game_object ~= re8vr.player then
            return retval
        end
    end

    re8vr.is_arm_jacked = controller:get_field("IsRArmJacked")
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
    local ch9_player_motion_controller_type = sdk.find_type_definition("app.CH9PlayerMotionController")

    if ch8_player_motion_controller_type then
        local ch8_update_postural_camera_motion_method = ch8_player_motion_controller_type:get_method("updatePosturalCameraMotion")
        -- chris
        sdk.hook(ch8_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
    end

    if ch9_player_motion_controller_type ~= nil then
        local ch9_update_postural_camera_motion_method = ch9_player_motion_controller_type:get_method("updatePosturalCameraMotion")

        -- ch9 (end of zoe?)
        sdk.hook(ch9_update_postural_camera_motion_method, on_pre_update_postural_camera_motion, on_post_update_postural_camera_motion)
    end
end

local player_movement_args = nil

local function on_pre_player_movement_late_update(args)
    player_movement_args = args
end

local function re7_on_post_player_movement_late_update(retval)
    local args = player_movement_args
    local movement = sdk.to_managed_object(args[2])

    re8vr.movement_speed_rate = movement:get_field("_SpeedRate")
    re8.movement_speed_vector = movement:get_field("_MoveSpeedVector")

    return retval
end

local function re8_on_post_player_movement_late_update(retval)
    local args = player_movement_args
    local movement = sdk.to_managed_object(args[2])

    re8vr.movement_speed_rate = movement:get_field("SpeedRate")
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

    if ch8_player_movement_type ~= nil then
        local ch8_player_movement_late_update_method = ch8_player_movement_type:get_method("doLateUpdate")
        sdk.hook(ch8_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
    end

    local ch9_player_movement_type = sdk.find_type_definition("app.CH9PlayerMovement")

    if ch8_player_movement_type ~= nil then
        local ch9_player_movement_late_update_method = ch9_player_movement_type:get_method("doLateUpdate")

        sdk.hook(ch9_player_movement_late_update_method, on_pre_player_movement_late_update, on_post_player_movement_late_update)
    end
end

function re8.notify_event_task_created(callback)
    callbacks["event_task_create"]:add(callback)
end

function re8.notify_event_task_terminated(callback)
    callbacks["event_task_terminate"]:add(callback)
end

_re8lib = re8

return re8