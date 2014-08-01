print("LuaCL unit test")

-- assert(RegisterDebugCallback, "Unit test requires debug build.")
local function debugCallback(...)
	print(...)
end
RegisterDebugCallback(debugCallback)

local reg = GetRegistry()

assert(GetPlatform, "GetPlatform not found")

assert(not next(reg["LuaCL.Registry.Platform"]))

local platforms = {GetPlatform()}
assert(#platforms, "No platform detected")
dump_table(reg["LuaCL.Registry.Platform"], "LuaCL.Registry.Platform")

-- for index, platform in ipairs()

for index, platform in ipairs(platforms) do
	assert(type(platform) == "userdata", "Wrong type for platform.")
	assert(tostring(platform):find("LuaCL_Platform: "), "Bad __tostring for platform")
	assert(type(platform.GetInfo) == "function")
	assert(type(platform.GetDevices) == "function")
	assert(type(platform.CreateContext) == "function")

	local ret, err = pcall(platform.GetInfo)
	assert(not ret)
	assert(err:find("LuaCL.Metatable.Platform expected, got no value"))	-- "." could match any character, including "." itself
	local ret, err = pcall(platform.GetInfo, newproxy())
	assert(not ret)
	assert(err:find("LuaCL.Metatable.Platform expected, got userdata"))

	local info = platform:GetInfo()
	assert(type(info) == "table")
	local platformInfoKeys = {
		"NAME",
		"VERSION",
		"VENDOR",
		"PROFILE",
		"EXTENSIONS",
	}
	for _, key in ipairs(platformInfoKeys) do
		assert(info[key])
	end


end

