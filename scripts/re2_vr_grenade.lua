if reframework:get_game_name() ~= "re2" and reframework:get_game_name() ~= "re3" then
    return
end


local vrc_manager = require("vr/VRControllerManager")
local statics = require("utility/Statics")
local ManagedObjectDict = require("utility/ManagedObjectDict")
local re2 = require("utility/RE2")
local GameObject = require("utility/GameObject")

local throw_grenade_generator_type = sdk.find_type_definition(sdk.game_namespace("weapon.generator.ThrowGrenadeGenerator"))
local is_grenade_out = false
local wants_spawn_nade = false

local last_thrown_grenade = nil
local last_camera_matrix = Matrix4x4f.new()

local function on_pre_rigid_instantiate(args)
    local go = sdk.to_managed_object(args[3])

    last_thrown_grenade = go
end

local function on_post_rigid_instantiate(retval)
    --[[if not is_grenade_out then
        return
    end]]

    local rigid_body = GameObject.get_component(last_thrown_grenade, "via.dynamics.RigidBodySet")

    if rigid_body ~= nil and vrmod:is_hmd_active() and vrmod:is_using_controllers() and vrc_manager:has_controllers() then
        local right_controller = vrc_manager.controllers_list[2]

        if right_controller ~= nil then
            --local camera = sdk.get_primary_camera()
            local original_camera_rotation = last_camera_matrix:to_quat()

            rigid_body:call("setLinearVelocity", 0, original_camera_rotation * right_controller.velocity)
        end
    end

    return retval
end

sdk.hook(throw_grenade_generator_type:get_method("onRigidInstantiate"), on_pre_rigid_instantiate, on_post_rigid_instantiate)

local inside_vr_throw = false

-- fixes a crash
local function on_pre_request_ringbuffer_element(args)
    log.debug("Requested ringbuffer element")

    if inside_vr_throw then
        log.debug("SKIPPING!!!!")
        --return sdk.PreHookResult.SKIP_ORIGINAL
    end
end

local function on_post_request_ringbuffer_element(retval)
    return retval
end

local shell_manager_type = sdk.find_type_definition(sdk.game_namespace("weapon.shell.ShellManager"))
sdk.hook(shell_manager_type:get_method("requestRingBufferElement"), on_pre_request_ringbuffer_element, on_post_request_ringbuffer_element)

local was_grip_down = false

re.on_application_entry("UpdateFSM", function()
    is_grenade_out = false

    --[[if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() or not vrc_manager:has_controllers() then
        return
    end]]

    local player = re2.player
    if not player then
        return
    end

    if not re2.weapon_gameobject or not re2.weapon then
        return
    end

    local shell_generator = re2.weapon:get_field("<ShellGenerator>k__BackingField")

    if not shell_generator then
        return
    end

    if not shell_generator:get_type_definition():is_a(throw_grenade_generator_type) then
        return
    end

    is_grenade_out = true

    local right_controller = vrc_manager.controllers_list[2]

    if right_controller ~= nil then
        local is_grip_down = right_controller:is_action_active(vrc_manager.Actions.GRIP)

        if was_grip_down and not is_grip_down then
            wants_spawn_nade = true
        end

        was_grip_down = is_grip_down
    end

    if wants_spawn_nade then
        shell_generator:call("onRelease")
        shell_generator:call("onSetup")
        inside_vr_throw = true
        re2.weapon:call("executeFire", 1)
        inside_vr_throw = false
        wants_spawn_nade = false
    end
end)

re.on_pre_application_entry("BeginRendering", function()
    local camera = sdk.get_primary_camera()

    if camera ~= nil then
        last_camera_matrix = camera:call("get_WorldMatrix")
    end
end)

re.on_draw_ui(function()
    if imgui.tree_node("RE2 VR Grenade Extension") then
        imgui.text("Last thrown grenade: " .. tostring(last_thrown_grenade))

        if imgui.button("Spawn grenade") then
            wants_spawn_nade = true
        end

        imgui.tree_pop()
    end
end)

re.on_frame(function()
    --[[if last_thrown_grenade and sdk.is_managed_object(last_thrown_grenade) then
        object_explorer:handle_address(last_thrown_grenade)
    end]]
end)