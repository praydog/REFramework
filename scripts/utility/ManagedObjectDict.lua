-- Allows using full userdata objects in dictionaries
-- particularly REManagedObjects
local ManagedObjectDict = {
    new = function(self, o)
        o = o or {}
        return setmetatable(o, self)
    end,
    __index = function(t, key)
        return rawget(t, key:get_address())
    end,
    __newindex = function(t, key, value)
        rawset(t, key:get_address(), value)
    end
}

return ManagedObjectDict