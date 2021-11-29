local vrc_manager = require("vr/VRControllerManager")
local statics = require("utility/Statics")

local via = {
    hid = {
        GamePadButton = statics.generate("via.hid.GamePadButton")
    }
}

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

local function get_localplayer()
    local playman = sdk.get_managed_singleton(sdk.game_namespace("PlayerManager"))

    if not playman then
        return nil
    end

    return playman:call("get_CurrentPlayer")
end

--[[local motion = get_component(player, "via.motion.Motion")
if not motion then
    log.info("no motion")
    return
end]]

local camera = sdk.get_primary_camera()
local camera_gameobject = camera:call("get_GameObject")

local motion_blur = get_component(camera_gameobject, "via.render.MotionBlur")
local tonemapping = get_component(camera_gameobject, "via.render.ToneMapping")

--[[if motion_blur then
    motion_blur:call("destroy", motion_blur)
end

if tonemapping then
    tonemapping:call("destroy", tonemapping)
else
    camera_gameobject:call("createComponent", sdk.typeof("via.render.ToneMapping"))
end]]

--[[local player = get_localplayer()
if not player then 
    log.info("no player")
    return 
end

local player_transform = player:call("get_Transform")
local transform_position = player_transform:call("get_Position")]]

local function get_weapon_object(player)
    local equipment = get_component(player, sdk.game_namespace("survivor.Equipment"))
    if not equipment then
        return nil, nil
    end

    local weapon = equipment:get_field("<EquipWeapon>k__BackingField")
    if not weapon then
        return nil, nil
    end

    local weapon_gameobject = weapon:call("get_GameObject")
    if not weapon_gameobject then
        return nil, nil
    end

    return weapon_gameobject, weapon
end

local melee_type = sdk.find_type_definition(sdk.game_namespace("implement.Melee"))

local function is_weapon_melee(weapon)
    if weapon == nil then return false end

    return weapon:get_type_definition():is_a(melee_type)
end

re.on_pre_update_transform(player_transform, function(transform)

end)

local last_weapon_pos = Vector3f.new(0, 0, 0)
local last_weapon_rotation = Quaternion.new(0, 0, 0, 0)
local last_weapon_gameobject = nil
local last_previous_weapon_gameobject = nil

local override_capsule_end = nil

local game_object_joints = {}

local capsule_origins = {}

local r_arm_wrist_hash = nil

re.on_application_entry("BeginRendering", function()
    local player = get_localplayer()
    if not player then 
        return 
    end

    override_capsule_end = nil

    local weapon_gameobject, weapon = get_weapon_object(player)
    if not weapon_gameobject or not weapon then
        local player_transform = player:call("get_Transform")

        local r_arm_wrist = nil

        if r_arm_wrist_hash == nil then
            r_arm_wrist = player_transform:call("getJointByName", "r_arm_wrist")

            if r_arm_wrist == nil then
                log.info("wrist nil 1")
                return
            end

            r_arm_wrist_hash = r_arm_wrist:call("get_NameHash")
        else
            r_arm_wrist = player_transform:call("getJointByHash", r_arm_wrist_hash)
        end

        if r_arm_wrist == nil then
            log.info("wrist nil 2")
            return
        end

        last_weapon_pos = r_arm_wrist:call("get_Position")
        last_weapon_rotation = r_arm_wrist:call("get_Rotation")

        return
    end

    if last_weapon_gameobject ~= nil and weapon_gameobject:get_address() ~= last_weapon_gameobject:get_address() then
        last_previous_weapon_gameobject = last_weapon_gameobject
    end

    last_weapon_gameobject = weapon_gameobject
    local weapon_transform = weapon_gameobject:call("get_Transform")

    game_object_joints[last_weapon_gameobject:get_address()] = {}

    local joints_tbl = game_object_joints[last_weapon_gameobject:get_address()]
    local joints = weapon_transform:call("get_Joints"):get_elements()

    for i, joint in ipairs(joints) do
        local joint_name = joint:call("get_Name")
        local joint_position = joint:call("get_Position")
        local joint_rotation = joint:call("get_Rotation")

        joints_tbl[joint_name] = {
            ["joint"] = joint,
            position = joint_position,
            rotation = joint_rotation
        }

        if joint_name == "vfx_muzzle1" then
            override_capsule_end = joint_position
        end
    end

    local weapon_transform = weapon_gameobject:call("get_Transform")
    
    last_weapon_pos = weapon_transform:call("get_Position")
    last_weapon_rotation = weapon_transform:call("get_Rotation")

    local collidables_list = weapon:get_field("<TargetCollidables>k__BackingField")
    if not collidables_list then
        return
    end

    --capsule_origins = {}

    --[[local request_set_collider = get_component(weapon_gameobject, "via.physics.RequestSetCollider")

    if request_set_collider == nil then
        return
    end

    local num_collidables = request_set_collider:call("getNumCollidablesFromIndex(System.UInt32)", 0)

    for i=0, num_collidables - 1 do
        local collidable = request_set_collider:call("getCollidableFromIndex(System.UInt32, System.UInt32)", 0, i)

        if collidable ~= nil then
            local transformed_shape = collidable:call("get_TransformedShape")

            if transformed_shape ~= nil and transformed_shape:get_type_definition():get_name() == "ContinuousCapsuleShape" then
                local transformed_capsule = transformed_shape:call("get_Capsule")

                if transformed_capsule ~= nil then
                    --table.insert(capsule_origins, { transformed_capsule:get_field("p0"), transformed_capsule:get_field("p1") })
                end
            end
        end
    end]]
end)

local curious = {}
local wanted_scale = 1.0


local function write_vec34(managed_object, offset, vector, doVec3)
    if sdk.is_managed_object(managed_object) then 
        managed_object:write_float(offset, vector.x)
        managed_object:write_float(offset + 4, vector.y)
        managed_object:write_float(offset + 8, vector.z)
        if not doVec3 then  managed_object:write_float(offset + 12, vector.w) end
    end
end

local function write_mat4(managed_object, offset, mat4)
    write_vec34(managed_object, offset, mat4[0])
    write_vec34(managed_object, offset + 16, mat4[1])
    write_vec34(managed_object, offset + 32, mat4[2])
    write_vec34(managed_object, offset + 48, mat4[3])
end

local function read_vec34(managed_object, offset, doVec3)
    if sdk.is_managed_object(managed_object) then
        if not doVec3 then
            return Vector4f.new(managed_object:read_float(offset), managed_object:read_float(offset + 4), managed_object:read_float(offset + 8), managed_object:read_float(offset + 12))
        end

        return Vector3f.new(managed_object:read_float(offset), managed_object:read_float(offset + 4), managed_object:read_float(offset + 8))
    end

    return Vector3f.new(0, 0, 0)
end

local function read_mat4(managed_object, offset)
    local mat4 = Matrix4x4f.new()
    mat4[0] = read_vec34(managed_object, offset)
    mat4[1] = read_vec34(managed_object, offset + 0x10)
    mat4[2] = read_vec34(managed_object, offset + 0x10 + 0x10)
    mat4[3] = read_vec34(managed_object, offset + 0x10 + 0x10 + 0x10)
    return mat4
end

re.on_pre_application_entry("UpdateBehavior", function()
    local player = get_localplayer()
    if not player then 
        return 
    end

    local weapon_gameobject, weapon = get_weapon_object(player)
    if not weapon_gameobject then
        return
    end

    local weapon_transform = weapon_gameobject:call("get_Transform")

    local collidables_list = weapon:get_field("<TargetCollidables>k__BackingField")
    if not collidables_list then
        return
    end

    local collidables = collidables_list:get_field("mItems"):get_elements()

    local request_set_collider = get_component(weapon_gameobject, "via.physics.RequestSetCollider")

    if request_set_collider == nil then
        return
    end

    --[[for i, collidable in ipairs(collidables) do
        curious[collidable:get_address()] = true
    end]]

    --[[local num_collidables = request_set_collider:call("getNumCollidablesFromIndex(System.UInt32)", 0)

    for i=0, num_collidables - 1 do
        local collidable = request_set_collider:call("getCollidableFromIndex(System.UInt32, System.UInt32)", 0, i)

        if collidable ~= nil then
            curious[collidable:get_address()] = true

            local transformed_shape = collidable:call("get_TransformedShape")

            if transformed_shape ~= nil and transformed_shape:get_type_definition():get_name() == "ContinuousCapsuleShape" then
                local transformed_capsule = transformed_shape:call("get_Capsule")

                if transformed_capsule ~= nil then
                    local p0 = transformed_capsule:get_field("p0")
                    local p1 = transformed_capsule:get_field("p1")

                    local delta = p1 - p0
                    local dir = delta:normalized()

                    transformed_capsule:set_field("p0", last_weapon_pos)
                    transformed_capsule:set_field("p1", p0 + delta)
                    transformed_shape:call("set_Capsule", transformed_capsule)
                end
            end
        end
    end]]
end)

local melee_collidables = {}
local original_capsules = {}

local function hijack_capsules(collidable)
    if collidable == nil then return end
    
    local transformed_shape = collidable:call("get_TransformedShape")

    if transformed_shape ~= nil and transformed_shape:get_type_definition():get_name() == "ContinuousCapsuleShape" then
        local untransformed_shape = collidable:call("get_Shape")
        local untransformed_capsule = untransformed_shape:call("get_Capsule")

        local transformed_capsule = transformed_shape:call("get_Capsule")

        if untransformed_capsule ~= nil and transformed_capsule ~= nil then
            local utp0 = untransformed_capsule:get_field("p0")
            local utp1 = untransformed_capsule:get_field("p1")

            local p0 = transformed_capsule:get_field("p0")
            local p1 = transformed_capsule:get_field("p1")
            local r = transformed_capsule:get_field("r")

            local delta = utp1 - utp0
            local dir = delta:normalized()

            original_capsules[collidable:get_address()] = { p0, p1, r }

            local new_p0 = last_weapon_pos
            local new_p1 = override_capsule_end

            if new_p1 == nil then
                new_p1 = last_weapon_pos + (last_weapon_rotation * Vector3f.new(0, 0, delta:length()))
            end

            transformed_capsule:set_field("p0", new_p0)
            transformed_capsule:set_field("p1", new_p1)
            transformed_capsule:set_field("r", 0.01)

            table.insert(capsule_origins, { new_p0, new_p1 })

            --untransformed_capsule:set_field("p1", (last_weapon_rotation * Vector3f.new(0, 0, 1.0)))

            transformed_shape:call("set_Capsule", transformed_capsule)
            --untransformed_shape:call("set_Capsule", untransformed_capsule)

            collidable:call("set_UpdateShape", false)
        end
    end
end

local function restore_capsules(collidable)
    if collidable == nil then return end

    local original_capsule = original_capsules[collidable:get_address()]
    if original_capsule == nil then return end
    
    local transformed_shape = collidable:call("get_TransformedShape")

    if transformed_shape ~= nil and transformed_shape:get_type_definition():get_name() == "ContinuousCapsuleShape" then
        local transformed_capsule = transformed_shape:call("get_Capsule")

        if transformed_capsule ~= nil then
            collidable:call("set_UpdateShape", true)
            
            transformed_capsule:set_field("p0", original_capsule[1])
            transformed_capsule:set_field("p1", original_capsule[2])
            transformed_capsule:set_field("r", original_capsule[3])
            transformed_shape:call("set_Capsule", transformed_capsule)
        end
    end
end

local scene_manager = sdk.get_native_singleton("via.SceneManager")
local scene_manager_typedef = sdk.find_type_definition("via.SceneManager")

local main_view = sdk.call_native_func(scene_manager, scene_manager_typedef, "get_MainView")

--[[if main_view ~= nil then
    re.msg(tostring(main_view))

    local size = main_view:call("get_Size")

    re.msg(tostring(size:get_field("w")) .. " " .. tostring(size:get_field("h")))
end]]

local application = sdk.get_native_singleton("via.Application")
local application_type = sdk.find_type_definition("via.Application")
local get_uptime_second_function = application_type:get_method("get_UpTimeSecond")

local last_physical_swing_time = 0.0

local function should_physically_swing()
    local now = get_uptime_second_function:call(application)

    if now - last_physical_swing_time <= 0.1 then
        -- keeps the swing alive for a bit
        return true
    end

    if vrc_manager:has_controllers() then
        local right_controller = vrc_manager.controllers_list[2]

        if right_controller ~= nil and right_controller.speed > 2.5 then
            last_physical_swing_time = now
            return true
        end
    end

    return false
end

local function get_frame_count()
    return sdk.call_native_func(application, application_type, "get_FrameCount")
end

local function on_pre_melee_late_update(args)
    melee_collidables = {}
    original_capsules = {}
    capsule_origins = {}

    local melee = sdk.to_managed_object(args[2])
    local gameobject = melee:call("get_GameObject")

    local request_set_collider = get_component(gameobject, "via.physics.RequestSetCollider")

    if request_set_collider == nil then
        return
    end

    local num_collidables = request_set_collider:call("getNumCollidablesFromIndex(System.UInt32)", 0)

    for i=0, num_collidables - 1 do
        local collidable = request_set_collider:call("getCollidableFromIndex(System.UInt32, System.UInt32)", 0, i)

        if collidable ~= nil then
            table.insert(melee_collidables, collidable)
            hijack_capsules(collidable)
        end
    end

    local collidables_list = melee:get_field("<TargetCollidables>k__BackingField")
    if not collidables_list then
        return
    end
    
    local collidables = collidables_list:get_field("mItems"):get_elements()

    -- physically swing to issue a melee attack
    if should_physically_swing() then

        local player = get_localplayer()
        if not player then
            return
        end
    
        local player_transform = player:call("get_Transform")
    
        local equipment = get_component(player, sdk.game_namespace("survivor.Equipment"))
    
        if equipment ~= nil then
            local equip_type = equipment:get_field("<EquipType>k__BackingField")
    
            --log.info("equip type: " .. tostring(equip_type))

            -- lets us use our bare hands as a melee weapon
            if equip_type == 0 then
                equipment:set_field("<EquipType>k__BackingField", 1)
            end
        end

        local request_set_id = melee:get_field("<RequestSetID>k__BackingField")

        if request_set_id ~= nil then
            --request_set_id:set_field("<Current>k__BackingField", 1)
            request_set_id:set_field("<Old>k__BackingField", -1)
            --request_set_id:set_field("<FrameCount>k__BackingField", get_frame_count() + 1)
        end
    
        melee:call("enableAttack")
        --melee:call("setupMeleeAttackUserData")
        melee:set_field("<GeneratedDecal>k__BackingField", false)
    end

    for i, collidable in ipairs(collidables) do
        --collidable:call("set_Enabled", true)
        --collidable:call("enable")

        curious[collidable:get_address()] = true

        table.insert(melee_collidables, collidable)
        hijack_capsules(collidable)
    end
end

local function on_post_melee_late_update(retval)
    for i, collidable in ipairs(melee_collidables) do
        restore_capsules(collidable)
    end

    return retval
end

sdk.hook(sdk.find_type_definition(sdk.game_namespace("implement.Melee")):get_method("lateUpdate"), on_pre_melee_late_update, on_post_melee_late_update)

local function on_pre_melee_check_attack(args)
    if should_physically_swing() then
        return sdk.PreHookResult.SKIP_ORIGINAL
    end
end

local function on_post_melee_check_attack(retval)
    if should_physically_swing() then
        return sdk.to_ptr(0)
    end

    return retval
end

sdk.hook(sdk.find_type_definition(sdk.game_namespace("survivor.Equipment")):get_method("checkActiveMeleeAttack"), on_pre_melee_check_attack, on_post_melee_check_attack)

local disable_enemy_ai = false

local function on_pre_enemy_controller_update(args)
    local enemy = sdk.to_managed_object(args[2])

    if disable_enemy_ai then
        return sdk.PreHookResult.SKIP_ORIGINAL
    else
        
    end
end

local function on_post_enemy_controller_update(retval)
    return retval
end

sdk.hook(sdk.find_type_definition(sdk.game_namespace("EnemyController")):get_method("update"), on_pre_enemy_controller_update, on_post_enemy_controller_update)
sdk.hook(sdk.find_type_definition(sdk.game_namespace("EnemyController")):get_method("lateUpdate"), on_pre_enemy_controller_update, on_post_enemy_controller_update)

local should_open = true
local draw_without_reframework = false

re.on_draw_ui(function()
    local changed = false

    changed, should_open = imgui.checkbox("Funny Window", should_open)
    changed, draw_without_reframework = imgui.checkbox("Draw without reframework", draw_without_reframework)
    changed, disable_enemy_ai = imgui.checkbox("Disable enemy AI", disable_enemy_ai)
end)

local wanted_typename = "System.UInt32"

local function draw_vr_info()
    imgui.text("VR")

    local controllers = vrmod:get_controllers()

    if #controllers == 0 then
        imgui.text("No controllers")
        return
    end

    if not vrmod:is_hmd_active() then
        imgui.text("HMD not active")
    end

    vrc_manager:draw_debug()
end

re.on_frame(function()
    draw.world_text("Hello", last_weapon_pos, 0xffffffff)

    for i, capsule in ipairs(capsule_origins) do
        local capsule_pos1 = draw.world_to_screen(capsule[1])
        local capsule_pos2 = draw.world_to_screen(capsule[2])

        if capsule_pos1 and capsule_pos2 then
            draw.line(capsule_pos1.x, capsule_pos1.y, capsule_pos2.x, capsule_pos2.y, 0xffffffff)
        end

        --draw.world_text("ASDF: " .. tostring(i), capsule_origin, 0xffffffff)
    end

    -- only display the funny window when REFramework is actually drawing its own UI
    if draw_without_reframework or reframework:is_drawing_ui() then
        if imgui.begin_window("Test", should_open) then
            draw_vr_info()

            local changed = false
            --[[changed, wanted_typename = imgui.input_text("Type name", wanted_typename)

            local t = sdk.find_type_definition(wanted_typename)

            if t ~= nil then
                imgui.text("is byref: " .. tostring(t:is_by_ref()))
                imgui.text("is pointer: " .. tostring(t:is_pointer()))
                imgui.text("is primitive: " .. tostring(t:is_primitive()))
            end]]

            imgui.text(reframework:get_game_name())

            if main_view ~= nil then
                local size = main_view:call("get_Size")

                --re.msg(tostring(size:get_field("w")) .. " " .. tostring(size:get_field("h")))

                imgui.text(tostring(size:get_field("w")) .. " " .. tostring(size:get_field("h")))
            end

            local handle_game_object = function(game_object)
                if sdk.is_managed_object(game_object) then
                    --[[local changed = false
                                        
                    changed, wanted_scale = imgui.drag_float("Scale", wanted_scale, 0.001, 0.001, 10.0)

                    if changed then
                        local transform = game_object:call("get_Transform")

                        if transform ~= nil and sdk.is_managed_object(transform) then
                            local mat = read_mat4(transform, 0x80)

                            -- create scale matrix
                            local scale_mat = Matrix4x4f.new()
                            scale_mat[0] = Vector4f.new(wanted_scale, 0, 0, 0)
                            scale_mat[1] = Vector4f.new(0, wanted_scale, 0, 0)
                            scale_mat[2] = Vector4f.new(0, 0, wanted_scale, 0)
                            scale_mat[3] = Vector4f.new(0, 0, 0, 1)

                            mat = mat * scale_mat
                            write_mat4(transform, 0x80, mat)
                        end
                    end]]

                    local name = game_object:call("get_Name")

                    if name ~= nil then
                        if imgui.tree_node(name) then
                            if imgui.tree_node_ptr_id(sdk.to_ptr(game_object:get_address()), "Joints") then
                                local joints = game_object_joints[game_object:get_address()]

                                for joint_name, joint in pairs(joints) do
                                    if imgui.tree_node(joint_name) then
                                        local joint_pos = joint.position
                                        local joint_rot = joint.rotation

                                        imgui.text("Position: " .. tostring(joint_pos.x) .. " " .. tostring(joint_pos.y) .. " " .. tostring(joint_pos.z))
                                        imgui.text("Rotation: " .. tostring(joint_rot.x) .. " " .. tostring(joint_rot.y) .. " " .. tostring(joint_rot.z) .. " " .. tostring(joint_rot.w))

                                        local start_pos_screen = draw.world_to_screen(joint_pos)
                                        local end_pos_screen = draw.world_to_screen(joint_pos + (joint_rot:to_mat4()[2] * 10))

                                        if start_pos_screen and end_pos_screen then
                                            draw.line(start_pos_screen.x, start_pos_screen.y, end_pos_screen.x, end_pos_screen.y, 0xffffffff)
                                        end

                                        imgui.tree_pop()
                                    end
                                end

                                imgui.tree_pop()
                            end

                            object_explorer:handle_address(game_object)

                            imgui.tree_pop()
                        end
                    end
                else
                    imgui.text("Not a managed object")
                end
            end

            handle_game_object(last_weapon_gameobject)
            handle_game_object(last_previous_weapon_gameobject)

            if imgui.tree_node("Curious") then
                local count = 0

                for addr, v in pairs(curious) do
                    if imgui.tree_node(tostring(count)) then
                        object_explorer:handle_address(addr)
                        imgui.tree_pop()
                    end

                    count = count + 1
                end
            end

            imgui.end_window()
        else
            should_open = false
        end
    end
end)

--[[re.on_pre_gui_draw_element(function(element, context)
    local game_object = element:call("get_GameObject")
    if not game_object then
        return
    end

    local transform = game_object:call("get_Transform")
    --log.info(game_object:call("get_Name") .. ": " .. tostring(game_object:get_address()))

    if game_object:call("get_Name") == "sm42_020_keystrokeDevice01A_gimmick" then

    end
end)]]
