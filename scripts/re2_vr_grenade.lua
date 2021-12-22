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

local r_arm_wrist_hash = nil
local last_wrist_position = Vector3f.new(0, 0, 0)
local last_wrist_rotation = Quaternion.new(0, 0, 0, 0)

local function on_pre_rigid_instantiate(args)
    wants_spawn_nade = false

    if not firstpersonmod:will_be_used() then return end

    last_thrown_grenade = sdk.to_managed_object(args[3])
end

local function on_post_rigid_instantiate(retval)
    if not firstpersonmod:will_be_used() then
        return retval
    end
    
    --[[if not is_grenade_out then
        return
    end]]

    local rigid_body = GameObject.get_component(last_thrown_grenade, "via.dynamics.RigidBodySet")

    if rigid_body ~= nil and vrmod:is_hmd_active() and vrmod:is_using_controllers() and vrc_manager:has_controllers() then
        local right_controller = vrc_manager.controllers_list[2]

        if right_controller ~= nil then
            --local camera = sdk.get_primary_camera()
            local original_camera_rotation = last_camera_matrix:to_quat()

            local hmd_transform = vrmod:get_transform(0)
            local hmd_rotation = hmd_transform:to_quat()

            rigid_body:call("setLinearVelocity", 0, (original_camera_rotation * hmd_rotation:inverse()):normalized() * right_controller.velocity)
            rigid_body:call("setAngularVelocity", 0, (original_camera_rotation * hmd_rotation:inverse()):normalized() * right_controller.angular_velocity)

            --[[prigid_body:call("setPosition", last_wrist_position)
            rigid_body:call("setRotation", last_wrist_rotation)

            last_thrown_grenade:call("get_Transform"):call("set_Position", last_wrist_position)
            last_thrown_grenade:call("get_Transform"):call("set_Rotation", last_wrist_rotation)]]
        end
    end

    return retval
end

sdk.hook(throw_grenade_generator_type:get_method("onRigidInstantiate"), on_pre_rigid_instantiate, on_post_rigid_instantiate)

local inside_vr_throw = false

local was_grip_down = false
local hooked = false

local last_grenade_position = Vector3f.new(0, 0, 0)
local last_grenade_rotation = Quaternion.new(0, 0, 0, 0)

re.on_application_entry("BeginRendering", function()
    if not firstpersonmod:will_be_used() then return end

    local camera = sdk.get_primary_camera()

    if camera ~= nil then
        last_camera_matrix = camera:call("get_WorldMatrix")
    end

    if re2.player ~= nil then
        local transform = re2.player:call("get_Transform")
        local r_arm_wrist = nil

        if r_arm_wrist_hash == nil then
            r_arm_wrist = transform:call("getJointByName", "r_arm_wrist")

            if r_arm_wrist == nil then
                log.info("wrist nil 1")
                return
            end

            r_arm_wrist_hash = r_arm_wrist:call("get_NameHash")
        else
            r_arm_wrist = transform:call("getJointByHash", r_arm_wrist_hash)
        end
        
        if r_arm_wrist ~= nil then
            last_wrist_position = r_arm_wrist:call("get_Position")
            last_wrist_rotation = r_arm_wrist:call("get_Rotation")
        end
    end

    if re2.weapon_gameobject ~= nil then
        last_grenade_position = re2.weapon_gameobject:call("get_Transform"):call("get_Position")
        last_grenade_rotation = re2.weapon_gameobject:call("get_Transform"):call("get_Rotation")
    end
end)

local function on_pre_shell_cartridge_lateupdate(args)
    is_grenade_out = false
    if not firstpersonmod:will_be_used() then return end

    --[[if not vrmod:is_hmd_active() or not vrmod:is_using_controllers() or not vrc_manager:has_controllers() then
        return
    end]]

    local player = re2.player
    if not player then
        wants_spawn_nade = false
        was_grip_down = false
        return
    end

    if not re2.weapon_gameobject or not re2.weapon then
        wants_spawn_nade = false
        was_grip_down = false
        return
    end

    local shell_generator = re2.weapon:get_field("<ShellGenerator>k__BackingField")

    if not shell_generator then
        wants_spawn_nade = false
        was_grip_down = false
        return
    end

    if not shell_generator:get_type_definition():is_a(throw_grenade_generator_type) then
        wants_spawn_nade = false
        was_grip_down = false
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
        --shell_generator:call("onRelease")
        --shell_generator:call("onSetup")

        --[[local shell_cartridge_controller = re2.weapon:get_field("<ShellCartridgeController>k__BackingField")

        if shell_cartridge_controller then
            shell_cartridge_controller:call("request")
        end]]

        local r_arm_wrist = nil
        
        if re2.player ~= nil then
            local transform = re2.player:call("get_Transform")
    
            if r_arm_wrist_hash == nil then
                r_arm_wrist = transform:call("getJointByName", "r_arm_wrist")
    
                if r_arm_wrist == nil then
                    log.info("wrist nil 1")
                    return
                end
    
                r_arm_wrist_hash = r_arm_wrist:call("get_NameHash")
            else
                r_arm_wrist = transform:call("getJointByHash", r_arm_wrist_hash)
            end
            
            if r_arm_wrist ~= nil then
                r_arm_wrist:call("set_Position", last_wrist_position)
                r_arm_wrist:call("set_Rotation", last_wrist_rotation)
            end
        end

        local transform = re2.weapon_gameobject:call("get_Transform")

        local old_position = transform:call("get_Position")
        local old_rotation = transform:call("get_Rotation")

        transform:call("set_Position", last_grenade_position)
        transform:call("set_Rotation", last_grenade_rotation)

        inside_vr_throw = true
        re2.weapon:call("executeFire", 1) -- 1 == ammo decrease
        inside_vr_throw = false
        wants_spawn_nade = false

        transform:call("set_Position", old_position)
        transform:call("set_Rotation", old_rotation)
    end
end

local function on_post_shell_cartridge_lateupdate(retval)
    return retval
end

sdk.hook(sdk.find_type_definition(sdk.game_namespace("weapon.shell.ShellCartridgeController")):get_method("lateUpdate"), on_pre_shell_cartridge_lateupdate, on_post_shell_cartridge_lateupdate)

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