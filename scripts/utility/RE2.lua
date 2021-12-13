if _RE2Lib ~= nil then
    return _RE2Lib
end

local RE2 = {}

local known_typeofs = {}

local function get_component(game_object, type_name)
    local t = known_typeofs[type_name] or sdk.typeof(type_name)

    if t == nil then 
        return nil
    end

    known_typeofs[type_name] = t
    return game_object:call("getComponent(System.Type)", t)
end

function RE2.get_localplayer()
    local playman = sdk.get_managed_singleton(sdk.game_namespace("PlayerManager"))

    if not playman then
        return nil
    end

    return playman:call("get_CurrentPlayer")
end

function RE2.get_inventory(player)
    if not player then
        return nil
    end

    return get_component(player, sdk.game_namespace("survivor.Inventory"))
end

function RE2.get_inventory_gui_gameobject()
    local gui_master = sdk.get_managed_singleton(sdk.game_namespace("gui.GUIMaster"))

    if not gui_master then
        return nil
    end

    return gui_master:get_field("RefInventoryUI")
end

function RE2.get_inventory_gui_behavior()
    local inventory_gui = RE2.get_inventory_gui_gameobject()

    if not inventory_gui then
        return nil
    end

    return get_component(inventory_gui, sdk.game_namespace("gui.NewInventoryBehavior"))
end

function RE2.get_weapon_object(player)
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

RE2.player = nil
RE2.inventory = nil
RE2.weapon = nil
RE2.weapon_gameobject = nil

re.on_pre_application_entry("UpdateBehavior", function()
    RE2.player = RE2.get_localplayer()

    if RE2.player == nil then
        RE2.weapon = nil
        RE2.weapon_gameobject = nil
        RE2.inventory = nil
        return
    end

    local weapon_gameobject, weapon = RE2.get_weapon_object(RE2.player)

    if weapon_gameobject == nil or weapon == nil then
        RE2.weapon = nil
        RE2.weapon_gameobject = nil
        RE2.inventory = nil
        return
    end

    RE2.weapon = weapon
    RE2.weapon_gameobject = weapon_gameobject
    RE2.inventory = RE2.get_inventory(RE2.player)
end)

_RE2Lib = RE2

return RE2