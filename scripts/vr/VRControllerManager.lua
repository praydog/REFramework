if _VRControllerManager ~= nil then
    return _VRControllerManager
end

local VRController = {
    velocity = Vector3f.new(0, 0, 0),
    angular_velocity = Vector3f.new(0, 0, 0),
    speed = 0.0,
    angular_speed = 0.0,
    position = Vector3f.new(0, 0, 0),
    rotation = Quaternion.new(0, 0, 0, 0),
    index = 0,
    new = function(self, index, o)
        o = o or {}
        setmetatable(o, self)
        self.__index = self
        o.index = index
        return o
    end,
    update = function(self)
        self.velocity = vrmod:get_velocity(self.index)
        self.angular_velocity = vrmod:get_angular_velocity(self.index)
        self.speed = self.velocity:length()
        self.angular_speed = self.angular_velocity:length()
    end,
    zero = function(self)
        self.velocity = Vector3f.new(0, 0, 0)
        self.angular_velocity = Vector3f.new(0, 0, 0)
        self.speed = 0.0
        self.angular_speed = 0.0
    end,
}

local VRControllerManager = {
    controllers = {},
    controllers_list = {},
    new = function(self, o)
        o = o or {}
        setmetatable(o, self)
        self.__index = self
        return o
    end,
    update = function(self)
        local vr_controllers = vrmod:get_controllers()
        for i, controller_index in ipairs(vr_controllers) do
            if not self.controllers[controller_index] then
                self.controllers[controller_index] = VRController:new(controller_index)
                table.insert(self.controllers_list, self.controllers[controller_index])
            end
        end

        if vrmod:is_using_controllers() then
            for i, controller in ipairs(self.controllers_list) do
                controller:update()
            end
        else
            for i, controller in ipairs(self.controllers_list) do
                controller:zero()
            end
        end
    end,
    has_controllers = function(self)
        return #self.controllers_list > 0
    end,
    draw_debug = function(self)
        if not vrmod:is_using_controllers() then
            imgui.text("Not using controllers")
            return
        end
    
        for i, controller in ipairs(self.controllers_list) do
            if imgui.tree_node("VRController " .. tostring(controller.index)) then
                local velocity = controller.velocity
                local angular_velocity = controller.angular_velocity
    
                imgui.text("Velocity: " .. tostring(velocity.x) .. " " .. tostring(velocity.y) .. " " .. tostring(velocity.z))
                imgui.text("Angular Velocity: " .. tostring(angular_velocity.x) .. " " .. tostring(angular_velocity.y) .. " " .. tostring(angular_velocity.z))

                imgui.text("Speed: " .. tostring(controller.speed))
                imgui.text("Angular Speed: " .. tostring(controller.angular_speed))
    
                imgui.tree_pop()
            end
        end
    end
}

local manager = VRControllerManager:new()
_VRControllerManager = manager

re.on_pre_application_entry("UpdateBehavior", function()
    manager:update()
end)

return manager