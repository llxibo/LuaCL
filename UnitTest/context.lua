require("UnitTest.object")

print("Unit.Test.context loaded")
local _M = UnitTest.NewTest("context", "LuaCL_Context", "LuaCL_Context_Registry", "LuaCL_Context")

function _M.Test(platform)
	print("Testing context")
	local devices = {platform:GetDevices()}

	UnitTest.AssertRegEmpty("context")
	do
		for index, device in ipairs(devices) do
			collectgarbage()

			local context - platform:CreateContext(device)
			UnitTest.AssertObject("context", context)
			UnitTest.AssertRegMatch("context", {context})

			local contextDevices = {context:GetDevices()}
			UnitTest.MatchTableValue(contextDevices, {device})
		end

		collectgarbage()
		local context = platform:CreateContext(devices)
		UnitTest.AssertObject("context", context)
		UnitTest.AssertRegMatch("context", {context})

		local contextDevices = {context:GetDevices()}
		UnitTest.MatchTableValue(contextDevices, devices)
	end
	collectgarbage()
	UnitTest.AssertRegEmpty("context")
end
