print("LuaCL unit test")

local function isLightUserdata(value)
	return type(value) == "number" and tostring(value) == ""
end

local function matchTableValue(tbl1, tbl2)
	local set1, set2 = {}, {}
	for key, value in pairs(tbl1) do
		set1[value] = key
	end
	for key, value in pairs(tbl2) do
		set2[value] = key
		assert(set1[value])
	end
	for key, value in pairs(tbl1) do
		assert(set2[value])
	end
end
-- assert(RegisterDebugCallback, "Unit test requires debug build.")

-- debugInfo is an array that allows indexing values from the end with negative index
local debugInfo = setmetatable({}, {
	__index = function (t, key)
		if key < 0 then
			return rawget(t, #t + key)
		end
	end
})

local function debugCallback(msg)
	print("Debug:", msg)
	table.insert(debugInfo, msg)
end

RegisterDebugCallback(debugCallback)

local reg = GetRegistry()

------ Platform ------
do
	assert(GetPlatform, "GetPlatform not found")

	local platformReg = reg.LuaCL_Platform_Registry
	assert(not next(platformReg), "Platform registry should be empty")
	assert(getmetatable(platformReg).__mode == "kv", "Platform registry should be pure weak")

	local platforms = {GetPlatform()}
	assert(#platforms, "No platform detected")
	-- dump_table(reg["LuaCL.Registry.Platform"], "LuaCL.Registry.Platform")

	for key, platform in ipairs(platformReg) do
		assert(isLightUserdata(key))
	end

	matchTableValue(platforms, platformReg)

	platform = nil
	collectgarbage()	-- Platform objects should be collected now
	assert(next(platformReg))
end

local platforms = {GetPlatform()}
for index, platform in ipairs(platforms) do
	assert(type(platform) == "userdata", "Wrong type for platform.")
	assert(getmetatable(platform) == reg.LuaCL_Platform)
	assert(tostring(platform):find("LuaCL_Platform: "), "Bad __tostring for platform")
	assert(type(platform.GetInfo) == "function")
	assert(type(platform.GetDevices) == "function")
	assert(type(platform.CreateContext) == "function")

	local ret, err = pcall(platform.GetInfo)
	assert(not ret)
	assert(err:find("LuaCL_Platform expected, got no value"))	-- "." could match any character, including "." itself
	local ret, err = pcall(platform.GetInfo, newproxy())
	assert(not ret)
	assert(err:find("LuaCL_Platform expected, got userdata"))

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

	------ Device ------
	do
		local ret, err = pcall(platform.GetDevices)
		assert(not ret)
		assert(err:find("LuaCL_Platform expected, got no value"))
		local ret, err = pcall(platform.GetDevices, newproxy())
		assert(not ret)
		assert(err:find("LuaCL_Platform expected, got userdata"))
		local devices = {platform:GetDevices()}
		for index, device in ipairs(devices) do
			print(getmetatable(device))
		end
	end
end

