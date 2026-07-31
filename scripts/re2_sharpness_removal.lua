if reframework:get_game_name() ~= "re2" and reframework:get_game_name() ~= "re3" then
    return
end

local statics = require("utility/Statics")

local TAAStrength = statics.generate("via.render.ToneMapping.TemporalAA", true)

local wanted_sharpness = 0.0
local wanted_sharpness_enable = false
local wanted_jitter = false

local prev_filter_self = nil

local function on_pre_apply_layer_param(args)
    prev_filter_self = args[2]
    return sdk.PreHookResult.CALL_ORIGINAL
end

local function on_post_apply_layer_param(retval)
    local tonemapping_controller = sdk.to_managed_object(prev_filter_self)
    local current_param = tonemapping_controller:get_field("CurrentParam")

    if current_param ~= nil then
        current_param:set_field("EchoEnabled", not wanted_jitter)
        current_param:set_field("TemporalAA", wanted_sharpness_enable and TAAStrength.Strong or TAAStrength.Disable)
        current_param:set_field("Sharpness", wanted_sharpness)
    end

    return retval
end

local tonemap_controller_type = sdk.find_type_definition(sdk.game_namespace("posteffect.ToneMapController"))
sdk.hook(tonemap_controller_type:get_method("applyLayerParam"), on_pre_apply_layer_param, on_post_apply_layer_param)

re.on_draw_ui(function()
    if imgui.tree_node("Sharpness Settings") then
        local changed_enable, changed_slider, changed_jitter = false, false, false

        changed_enable, wanted_sharpness_enable = imgui.checkbox("Sharpness Enabled", wanted_sharpness_enable)

        if changed_enable then
            wanted_sharpness = wanted_sharpness_enable and 1.0 or 0.0
        end

        changed_jitter, wanted_jitter = imgui.checkbox("TAA Jitter Enabled", wanted_jitter)

        imgui.tree_pop()
    end
end)
