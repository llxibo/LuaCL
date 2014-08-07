require("UnitTest.object")

print("UnitTest.context loaded")
local _M = UnitTest.NewTest("context", "LuaCL_Context", "LuaCL_Context_Registry", "LuaCL_Context")

function _M.Test(platform)
	print("Testing context")
	local devices = {platform:GetDevices()}

	UnitTest.AssertRegEmpty("context")
	do
		for index, device in ipairs(devices) do
			-- Test context creation with single device
			local context = platform:CreateContext(device)
			_M.TestContext(context, {device}, platform)
		end

		-- Test context creation with multiple devices
		local context = platform:CreateContext(devices)
		_M.TestContext(context, devices, platform)
	end
	UnitTest.AssertRegEmpty("context")
end

function _M.TestContext(context, devices, platform)
	assert(type(devices) == "table")
	UnitTest.AssertObject("platform", platform)
	UnitTest.AssertObject("context", context)
	UnitTest.AssertRegMatch("context", {context})

	assert(context:GetPlatform() == platform)
	local contextDevices = {context:GetDevices()}
	for index, device in ipairs(contextDevices) do
		UnitTest.AssertObject("device", device)
	end
	UnitTest.MatchTableValue(contextDevices, devices)

	require "UnitTest.program"
	UnitTest.program.Test(context)

	require "UnitTest.buffer"
	UnitTest.buffer.Test(context)

	require "UnitTest.cmdqueue"
	UnitTest.cmdqueue.Test(context)
end
