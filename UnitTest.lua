print("LuaCL unit test")

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

require "UnitTest.object"

require "UnitTest.platform"

-- -- function err(msg)
-- -- 	print("Error: " .. debug.traceback(msg, 2))
-- -- end
-- xpcall(UnitTest.platform.Test, err)
UnitTest.platform.Test()
print("All tests passed")
do return end

local reg = GetRegistry()

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


------ Platform ------
do
	assert(GetPlatform, "GetPlatform not found")

	local platformReg = reg.LuaCL_Platform_Registry
	assert(not next(platformReg), "Platform registry should be empty")
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

	do
		------ platform.GetDevices ------
		local ret, err = pcall(platform.GetDevices)
		assert(not ret)
		assert(err:find("LuaCL_Platform expected, got no value"))
		local ret, err = pcall(platform.GetDevices, newproxy())
		assert(not ret)
		assert(err:find("LuaCL_Platform expected, got userdata"))

		assert(not next(reg.LuaCL_Device_Registry))
		local devices = {platform:GetDevices()}
		assert(#devices > 0, "No device found")
		for index, device in ipairs(devices) do
			assert(getmetatable(device) == reg.LuaCL_Device)
			assert(tostring(device):find("LuaCL_Device: "))

			assert(device.GetInfo)
			local ret, err = pcall(device.GetInfo)
			assert(not ret)
			assert(err:find("LuaCL_Device expected, got no value"))
			local ret, err = pcall(device.GetInfo, platform)
			assert(not ret)
			assert(err:find("LuaCL_Device expected, got userdata"))

			local info = device:GetInfo()
			assert(type(info) == "table")
			local deviceInfoKeys = {
				TYPE = "number",
				VENDOR_ID = "number",
				MAX_COMPUTE_UNITS = "number",
				MAX_WORK_ITEM_DIMENSIONS = "number",
				MAX_WORK_GROUP_SIZE = "number",
				MAX_WORK_ITEM_SIZES = "table",
				PREFERRED_VECTOR_WIDTH_CHAR = "number",
				PREFERRED_VECTOR_WIDTH_SHORT = "number",
				PREFERRED_VECTOR_WIDTH_INT = "number",
				PREFERRED_VECTOR_WIDTH_LONG = "number",
				PREFERRED_VECTOR_WIDTH_FLOAT = "number",
				PREFERRED_VECTOR_WIDTH_DOUBLE = "number",
				MAX_CLOCK_FREQUENCY = "number",
				ADDRESS_BITS = "number",
				MAX_READ_IMAGE_ARGS = "number",
				MAX_WRITE_IMAGE_ARGS = "number",
				MAX_MEM_ALLOC_SIZE = "number",
				IMAGE2D_MAX_WIDTH = "number",
				IMAGE2D_MAX_HEIGHT = "number",
				IMAGE3D_MAX_WIDTH = "number",
				IMAGE3D_MAX_HEIGHT = "number",
				IMAGE3D_MAX_DEPTH = "number",
				IMAGE_SUPPORT = "number",
				MAX_PARAMETER_SIZE = "number",
				MAX_SAMPLERS = "number",
				MEM_BASE_ADDR_ALIGN = "number",
				MIN_DATA_TYPE_ALIGN_SIZE = "number",
				SINGLE_FP_CONFIG = "number",
				GLOBAL_MEM_CACHE_TYPE = "number",
				GLOBAL_MEM_CACHELINE_SIZE = "number",
				GLOBAL_MEM_CACHE_SIZE = "number",
				GLOBAL_MEM_SIZE = "number",
				MAX_CONSTANT_BUFFER_SIZE = "number",
				MAX_CONSTANT_ARGS = "number",
				LOCAL_MEM_TYPE = "number",
				LOCAL_MEM_SIZE = "number",
				ERROR_CORRECTION_SUPPORT = "number",
				PROFILING_TIMER_RESOLUTION = "number",
				ENDIAN_LITTLE = "number",
				AVAILABLE = "number",
				COMPILER_AVAILABLE = "number",
				EXECUTION_CAPABILITIES = "number",
				QUEUE_PROPERTIES = "number",
				NAME = "string",
				VENDOR = "string",
				DRIVER_VERSION = "string",
				PROFILE = "string",
				VERSION = "string",
				EXTENSIONS = "string",
				DOUBLE_FP_CONFIG = "number",
				PREFERRED_VECTOR_WIDTH_HALF = "number",
				HOST_UNIFIED_MEMORY = "number",
				NATIVE_VECTOR_WIDTH_CHAR = "number",
				NATIVE_VECTOR_WIDTH_SHORT = "number",
				NATIVE_VECTOR_WIDTH_INT = "number",
				NATIVE_VECTOR_WIDTH_LONG = "number",
				NATIVE_VECTOR_WIDTH_FLOAT = "number",
				NATIVE_VECTOR_WIDTH_DOUBLE = "number",
				NATIVE_VECTOR_WIDTH_HALF = "number",
			}
			for key, value in pairs(deviceInfoKeys) do
				assert(type(info[key]) == value)
			end
		end

		matchTableValue(devices, reg.LuaCL_Device_Registry)

		devices = nil
		collectgarbage()
		assert(not next(reg.LuaCL_Device_Registry), "Failed collecting devices")

		------ platform.CreateContext ------
		local devices = {platform:GetDevices()}
		local ret, err = pcall(platform.CreateContext)
		assert(not ret)
		assert(err:find("LuaCL_Platform expected, got no value"))
		local ret, err = pcall(platform.CreateContext, platform)
		assert(not ret)
		assert(err:find("Bad argument, expecting one or more valid devices on arg #2."))
		local ret, err = pcall(platform.CreateContext, platform, newproxy())
		assert(not ret)
		assert(err:find("LuaCL_Device expected, got userdata"))
		local ret, err = pcall(platform.CreateContext, platform, {})	-- Create context with empty device list
		assert(not ret)
		assert(err:find("Bad argument, expecting one or more valid devices on arg #2."))

		-- Context creation with single device
		for index, device in ipairs(devices) do
			collectgarbage()	-- Collect context objects from previous cycle
			assert(not next(reg.LuaCL_Context_Registry))	-- The registry should be empty
			local context = platform:CreateContext(device)
			assert(getmetatable(context) == reg.LuaCL_Context)

			assert(type(context.GetDevices) == "function")
			local ret, err = pcall(context.GetDevices)
			assert(not ret)
			assert(err:find("LuaCL_Context expected, got no value"))
			local ret, err = pcall(context.GetDevices, newproxy())
			assert(not ret)
			assert(err:find("LuaCL_Context expected, got userdata"))

			local contextDevices = {context:GetDevices()}
			assert(#contextDevices == 1)
			assert(contextDevices[1] == device)
		end

		-- Context creation with multiple devices
		local context = platform:CreateContext(devices)
		local contextDevices = {context:GetDevices()}
		assert(#devices == #contextDevices)
		for index, device in ipairs(devices) do
			-- Notice that the returned device list may not match the sequence of the device list it was created from
			-- assert(device == contextDevices[index])
		end
		matchTableValue(devices, contextDevices)	-- But the listed devices should match
	end
end

print("All tests passed")
