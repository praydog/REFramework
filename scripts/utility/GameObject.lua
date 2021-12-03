local GameObject = {}

local known_typeofs = {}

function GameObject.get_component(game_object, type_obj)
    if type(type_obj) == "string" then
        local t = known_typeofs[type_name] or sdk.typeof(type_obj)

        if t == nil then 
            return nil
        end
    
        known_typeofs[type_obj] = t
        return game_object:call("getComponent(System.Type)", t)
    else
        return game_object:call("getComponent(System.Type)", type_obj)
    end
end

function GameObject.get_transform(game_object)
    return game_object:call("get_Transform")
end

function GameObject.get_components(game_object)
    local transform = GameObject.get_transform(game_object)

    if not transform then
        return {}
    end

    return game_object:call("get_Components"):get_elements()
end

return GameObject