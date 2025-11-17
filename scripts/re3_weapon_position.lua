do return end -- remove this line to enable

if reframework:get_game_name() ~= 're3' and not vrmod:is_openxr_loaded() then
    return
end

local re2 = require('utility/RE2')

--[[
Common OpenVR
default: 0.2, -2.5, -1.7
handgun: 0.2, -2.5, -1.7

Common OpenXR
default: 0.28, -2.982, -1.495
handgun: 0.28, -2.982, -1.495

HP Reverb G2 OpenXR
default: 0.28, -2.62, -2.3
handgun: 0, -2.62, -1.8
]]--
local default_right_hand_rotation_vec = Vector3f.new(0.28, -2.982, -1.495)
local handgun_right_hand_rotation_vec = Vector3f.new(0.28, -2.982, -1.495)

-- https://github.com/SilverEzredes/Advanced-Weapon-Framework/blob/master/AWFCore/AWF/RE3R_WeaponData.lua
local handguns = {'wp0000', 'wp0100', 'wp0200', 'wp0300', 'wp0600', 'wp3000', 'wp3100'}

re.on_pre_application_entry('UpdateBehavior', function()
	if not re2.weapon then
		return
	end

	local weapon_gameobject = re2.weapon:call('get_GameObject')

	if not weapon_gameobject then
		return
	end

	local weapon_name = weapon_gameobject:call('get_Name')

	for i = 1, #handguns do
		if weapon_name == handguns[i] then
			firstpersonmod.right_hand_rotation_offset = handgun_right_hand_rotation_vec
			return
		end
	end

	firstpersonmod.right_hand_rotation_offset = default_right_hand_rotation_vec
end)