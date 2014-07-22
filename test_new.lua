dofile("dump_table.lua")

print("LuaCL environment test")

local platforms = {GetPlatform()}

dump_table(platforms, "platforms")

for index, platform in ipairs(platforms) do
	local info = platform:GetInfo()
	dump_table(info, ("(%s):GetInfo() "):format(tostring(platform)))

	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices() "):format(tostring(platform)))
	devices = nil

	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices() "):format(tostring(platform)))

	print("Collecting garbage")
	devices = nil
	collectgarbage()

	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices() "):format(tostring(platform)))
end
