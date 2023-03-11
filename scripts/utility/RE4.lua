if _RE4Lib ~= nil then
    return _RE4Lib
end

local RE4 = {}
local known_typeofs = {}

if reframework:get_game_name() ~= "re4" then
    return RE4
end

local function get_component(game_object, type_name)
    local t = known_typeofs[type_name] or sdk.typeof(type_name)

    if t == nil then 
        return nil
    end

    known_typeofs[type_name] = t
    return game_object:call("getComponent(System.Type)", t)
end

RE4.get_component = get_component

function RE4.get_localplayer_ctx()
    local character_manager = sdk.get_managed_singleton(sdk.game_namespace("CharacterManager"))
    if character_manager == nil then
        return nil
    end

    return character_manager:call("getPlayerContextRef")
end

function RE4.get_body(context)
    context = context or RE4.get_localplayer_ctx()
    if context == nil then
        return nil
    end

    return context:call("get_BodyGameObject")
end

function RE4.get_head(context)
    context = context or RE4.get_localplayer_ctx()
    if context == nil then
        return nil
    end

    return context:call("get_HeadGameObject")
end

function RE4.get_inventory(player)
    return nil
end

function RE4.get_inventory_gui_gameobject()
    return nil
end

function RE4.get_inventory_gui_behavior()
    local inventory_gui = RE4.get_inventory_gui_gameobject()

    if not inventory_gui then
        return nil
    end

    return get_component(inventory_gui, sdk.game_namespace("gui.NewInventoryBehavior"))
end

function RE4.get_weapon_object(player)
    return nil, nil
end

RE4.player = nil
RE4.body = nil
RE4.head = nil
RE4.inventory = nil
RE4.weapon = nil
RE4.weapon_gameobject = nil
RE4.last_inventory_shown_time = 0.0
RE4.fp_enabled = false

RE4.is_in_inventory_menu = function()
    return os.clock() - RE4.last_inventory_shown_time <= 0.1
end

re.on_pre_application_entry("UpdateBehavior", function()
    RE4.player = RE4.get_localplayer_ctx()
    RE4.body = RE4.get_body(RE4.player)
    RE4.head = RE4.get_head(RE4.player)

    if RE4.player == nil then
        RE4.body = nil
        RE4.head = nil
        RE4.weapon = nil
        RE4.weapon_gameobject = nil
        RE4.inventory = nil
        return
    end

    local weapon_gameobject, weapon = RE4.get_weapon_object(RE4.player)

    if weapon_gameobject == nil or weapon == nil then
        RE4.weapon = nil
        RE4.weapon_gameobject = nil
        RE4.inventory = nil
        return
    end

    RE4.weapon = weapon
    RE4.weapon_gameobject = weapon_gameobject
    RE4.inventory = RE4.get_inventory(RE4.player)
end)

local inventory_names = {
    "Gui_ui3030"
}

for i, v in ipairs(inventory_names) do
    inventory_names[v] = true
end

re.on_pre_gui_draw_element(function(element, context)
    local game_object = element:call("get_GameObject")
    if game_object == nil then return true end

    local name = game_object:call("get_Name")

    -- 0x10 = updating
    if inventory_names[name] ~= nil and game_object:read_byte(0x10) == 1 then
        RE4.last_inventory_shown_time = os.clock()
    end
end)

_RE4Lib = RE4

return RE4