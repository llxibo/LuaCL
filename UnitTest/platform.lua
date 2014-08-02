require("UnitTest.object")

print("Unit.Test.platform loaded")
UnitTest.NewTest("platform", "LuaCL_Platform", "LuaCL_Platform_Registry", "LuaCL_Platform")

assert(type(GetPlatform) == "function")

local platformInfoKeys = {
	NAME = "string",
	VERSION = "string",
	VENDOR = "string",
	PROFILE = "string",
	EXTENSIONS = "string",
}

function Test()
	print("Testing platform")

	UnitTest.AssertRegEmpty("platform")
	do
		local platforms = {GetPlatform()}
		assert(#platforms > 0)

		UnitTest.AssertRegMatch("platform", platforms)
		for index, platform in pairs(platforms) do
			UnitTest.AssertObject("platform", platform)

			-- UnitTest.TestFuncArgs{platform.GetInfo, platform)
			local info = platform:GetInfo()
			UnitTest.AssertInfoTable(info, platformInfoKeys)
		end
	end
	collectgarbage()
	UnitTest.AssertRegEmpty("platform")
end
