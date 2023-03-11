if reframework:get_game_name() ~= "re4" then
    return
end

local statics = require("utility/Statics")
local re4 = require("utility/RE4")

re4.crosshair_pos = Vector3f.new(0, 0, 0)
re4.crosshair_normal = Vector3f.new(0, 0, 0)

local gameobject_get_transform = sdk.find_type_definition("via.GameObject"):get_method("get_Transform")
local cast_ray_async_method = sdk.find_type_definition("via.physics.System"):get_method("castRayAsync(via.physics.CastRayQuery, via.physics.CastRayResult)")

local joint_get_position = sdk.find_type_definition("via.Joint"):get_method("get_Position")
local joint_get_rotation = sdk.find_type_definition("via.Joint"):get_method("get_Rotation")

local CollisionLayer = statics.generate(sdk.game_namespace("CollisionUtil.Layer"))
local CollisionFilter = statics.generate(sdk.game_namespace("CollisionUtil.Filter"))

local crosshair_bullet_ray_result = nil
local crosshair_attack_ray_result = nil
local last_crosshair_time = os.clock()

local function cast_ray_async(ray_result, start_pos, end_pos, layer, filter_info)
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
    if filter_info == nil then
        filter_info = ray_query:call("get_FilterInfo")
        filter_info:call("set_Group", 0)
        filter_info:call("set_MaskBits", 0xFFFFFFFF & ~1) -- everything except the player.
        filter_info:call("set_Layer", layer)
    end
    
    ray_query:call("set_FilterInfo", filter_info)
    cast_ray_async_method:call(via_physics_system, ray_query, ray_result)

	return ray_result
end

local function update_crosshair_world_pos(start_pos, end_pos)
    -- asynchronous raycast
    if crosshair_attack_ray_result == nil or crosshair_bullet_ray_result == nil then
        crosshair_attack_ray_result = cast_ray_async(crosshair_attack_ray_result, start_pos, end_pos, 5)
        crosshair_bullet_ray_result = cast_ray_async(crosshair_bullet_ray_result, start_pos, end_pos, 10)
        crosshair_attack_ray_result:add_ref()
        crosshair_bullet_ray_result:add_ref()
    end

    local finished = crosshair_attack_ray_result:call("get_Finished") == true and crosshair_bullet_ray_result:call("get_Finished")
    local attack_hit = finished and crosshair_attack_ray_result:call("get_NumContactPoints") > 0
    local any_hit = finished and (attack_hit or crosshair_bullet_ray_result:call("get_NumContactPoints") > 0)
    local both_hit = finished and crosshair_attack_ray_result:call("get_NumContactPoints") > 0 and crosshair_bullet_ray_result:call("get_NumContactPoints") > 0

    if finished and any_hit then
        local best_result = nil --attack_hit and crosshair_attack_ray_result or crosshair_bullet_ray_result

        if both_hit then
            local attack_distance = crosshair_attack_ray_result:call("getContactPoint(System.UInt32)", 0):get_field("Distance")
            local bullet_distance = crosshair_bullet_ray_result:call("getContactPoint(System.UInt32)", 0):get_field("Distance")

            if attack_distance < bullet_distance then
                best_result = crosshair_attack_ray_result
            else
                best_result = crosshair_bullet_ray_result
            end
        else
            best_result = attack_hit and crosshair_attack_ray_result or crosshair_bullet_ray_result
        end

        local contact_point = best_result:call("getContactPoint(System.UInt32)", 0)

        if contact_point then
            re4.crosshair_dir = (end_pos - start_pos):normalized()
            re4.crosshair_normal = contact_point:get_field("Normal")
            re4.crosshair_distance = contact_point:get_field("Distance")

            --re2.crosshair_pos = contact_point:get_field("Position") -- We don't use the position because the cast was asynchronous
            -- instead we get the distance to the impact and add it to the current position
            re4.crosshair_pos = start_pos + (re4.crosshair_dir * re4.crosshair_distance * 0.95)
        end
    else
        re4.crosshair_dir = (end_pos - start_pos):normalized()

        if re4.crosshair_distance then
            re4.crosshair_pos = start_pos + (re4.crosshair_dir * re4.crosshair_distance)
        else
            re4.crosshair_pos = start_pos + (re4.crosshair_dir * 10.0)
            re4.crosshair_distance = 10.0
        end
    end
    
    if finished then
        -- restart it.
        cast_ray_async(crosshair_attack_ray_result, start_pos, end_pos, 5, CollisionFilter.DamageCheckOtherThanPlayer)
        cast_ray_async(crosshair_bullet_ray_result, start_pos, end_pos, 10)
    end
end

re.on_pre_application_entry("LockScene", function()
    if not vrmod:is_hmd_active() and not re4.fp_enabled then return true end

    if re4.player == nil then return end
    if re4.body == nil then return end

    local camera = sdk.get_primary_camera()
    if not camera then print("no camera") return end

    local camera_gameobject = camera:call("get_GameObject")
    if not camera_gameobject then print("no gameobject") return end

    local camera_transform = gameobject_get_transform(camera_gameobject)
    if not camera_transform then print("no cam transform") return end

    local body_transform = re4.body:call("get_Transform")
    if not body_transform then print("no body transform") return end

    local camera_joint = camera_transform:call("get_Joints")[0]
    local camrot = joint_get_rotation(camera_joint)
    local cam_end = joint_get_position(camera_joint) + (camrot:to_mat4()[2] * -8192.0)

    update_crosshair_world_pos(joint_get_position(camera_joint), cam_end)
end)


local reticle_names = {
    "Gui_ui2040"
}

for i, v in ipairs(reticle_names) do
    reticle_names[v] = true
end

local function write_vec4(obj, vec, offset)
    obj:write_float(offset, vec.x)
    obj:write_float(offset + 4, vec.y)
    obj:write_float(offset + 8, vec.z)
    obj:write_float(offset + 12, vec.w)
end

re.on_pre_gui_draw_element(function(element, context)
    if not vrmod:is_hmd_active() and not re4.fp_enabled then return true end

    local game_object = element:call("get_GameObject")
    if game_object == nil then return true end

    local name = game_object:call("get_Name")

    -- set the world position of the crosshair/reticle to the trace end position
    -- also fixes scopes.
    if reticle_names[name] then
        last_crosshair_time = os.clock()

        if re4.crosshair_pos then
            local transform = game_object:call("get_Transform")

            if transform then
                local gui_comp = re4.get_component(game_object, "via.gui.GUI")

                if not gui_comp then
                    print("No gui component")
                    return true
                end

                local view = gui_comp:call("get_View")

                if view ~= nil then
                    view:call("set_ViewType", 1) -- world space
                    view:call("set_Overlay", true)
                    view:call("set_Detonemap", true)
                    view:call("set_DepthTest", true)
                end

                vrmod:unhide_crosshair()

                local new_mat = re4.crosshair_dir:to_quat():to_mat4()
                local distance = re4.crosshair_distance * 0.1
                if distance > 10 then
                    distance = 10
                end

                if distance < 0.3 then
                    distance = 0.3
                end

                local crosshair_pos = Vector4f.new(re4.crosshair_pos.x, re4.crosshair_pos.y, re4.crosshair_pos.z, 1.0)

                write_vec4(transform, new_mat[0] * distance, 0x80)
                write_vec4(transform, new_mat[1] * distance, 0x90)
                write_vec4(transform, new_mat[2] * distance, 0xA0)
                write_vec4(transform, crosshair_pos, 0xB0)
            end
        end
    end

    return true
end)