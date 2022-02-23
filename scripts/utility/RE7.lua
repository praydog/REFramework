if _RE7Lib ~= nil then
    return _RE7Lib
end

local RE7 = {
    player = nil,
    transform = nil,
    weapon = nil,
    weapon_gameobject = nil,
    inventory = nil,
    hand_touch = nil,
    right_hand_ik = nil,
    left_hand_ik = nil,
    is_in_cutscene = false,
    game_event_action_controller = nil,
    wants_block = false,
}

local known_typeofs = {}

local function get_component(game_object, type_name)
    local t = known_typeofs[type_name] or sdk.typeof(type_name)

    if t == nil then 
        return nil
    end

    known_typeofs[type_name] = t
    return game_object:call("getComponent(System.Type)", t)
end

function RE7.get_localplayer()
    local object_man = sdk.get_managed_singleton("app.ObjectManager")

    if not object_man then
        return nil
    end

    return object_man:get_field("PlayerObj")
end

function RE7.get_weapon_object(player)
    return nil, nil
end

re.on_pre_application_entry("UpdateBehavior", function()
    RE7.player = RE7.get_localplayer()
    local player = RE7.player

    if player == nil then
        RE7.weapon = nil
        RE7.weapon_gameobject = nil
        return
    end

    RE7.transform = player:call("get_Transform")
    RE7.inventory = get_component(player, "app.Inventory")
    RE7.hand_touch = get_component(player, "app.PlayerHandTouch")
    RE7.game_event_action_controller = get_component(player, "app.GameEventActionController")

    if RE7.hand_touch == nil then
        re7.right_hand_ik = nil
        re7.left_hand_ik = nil
    else
        local hand_ik = RE7.hand_touch:get_field("HandIK"):get_elements()

        if #hand_ik < 2 then
            log.info("no hand ik")
            RE7.right_hand_ik = nil
            RE7.left_hand_ik = nil
        else
            --log.info("IK: " .. tostring(hand_ik))

            RE7.right_hand_ik = hand_ik[1]
            RE7.left_hand_ik = hand_ik[2]

            if RE7.right_hand_ik and RE7.left_hand_ik then
                RE7.right_hand_ik_object = RE7.right_hand_ik:get_field("TargetGameObject")
                RE7.left_hand_ik_object = RE7.left_hand_ik:get_field("TargetGameObject") 
                RE7.right_hand_ik_transform = RE7.right_hand_ik:get_field("Target")
                RE7.left_hand_ik_transform = RE7.left_hand_ik:get_field("Target")
            end

            RE7.is_in_cutscene = false
        end
    end

    RE7.event_action_controller = get_component(player, "app.EventActionController")

    if RE7.event_action_controller ~= nil then
        local current_task = RE7.event_action_controller:get_field("CurrentTask")

        if current_task ~= nil then
            RE7.is_in_cutscene = true
        else
            RE7.is_in_cutscene = false
        end
    else
        RE7.is_in_cutscene = false
    end

    if RE7.inventory == nil then
        RE7.weapon = nil
        RE7.weapon_gameobject = nil
        return
    end

    local weapon_gameobject, weapon = RE7.get_weapon_object(player)

    if weapon_gameobject == nil or weapon == nil then
        RE7.weapon = nil
        RE7.weapon_gameobject = nil
        return
    end

    RE7.weapon = weapon
    RE7.weapon_gameobject = weapon_gameobject
end)

_RE7Lib = RE7

return RE7