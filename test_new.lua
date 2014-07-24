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

	print("\nCollecting garbage...")
	devices = nil
	collectgarbage()

	print("\nCreating context with list of devices...")
	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices() "):format(tostring(platform)))

	-- for index, device in ipairs(devices) do
	-- 	local info = device:GetInfo()
	-- 	dump_table(info, ("(%s):GetInfo() "):format(tostring(device)))
	-- end

	-- print(platform.CreateContext(platform, devices))
	local context = platform:CreateContext(devices)
	print("Created", context)

	print("\nCollecting garbage...")
	context = nil
	collectgarbage()

	print("\nCreating context with single device...")
	context = platform:CreateContext(devices[1], print)
	print("Created", context)

	-- context = platform:CreateContext(device)
end
