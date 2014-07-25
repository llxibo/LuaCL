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

	local devices = {platform:GetDevices()}
	print("\nCreating context with single device...")
	context = platform:CreateContext(devices[1], print)
	print("Created", context)

	print("\nCollecting garbage...")
	context = nil
	collectgarbage()

	print("\nCreating context with list of devices...")
	dump_table(devices, ("(%s):GetDevices() "):format(tostring(platform)))

	-- for index, device in ipairs(devices) do
	-- 	local info = device:GetInfo()
	-- 	dump_table(info, ("(%s):GetInfo() "):format(tostring(device)))
	-- end

	-- print(platform.CreateContext(platform, devices))
	local context = platform:CreateContext(devices)
	print("Created", context)
	print("Got devices:", context:GetDevices())
	print("Got platform:", context:GetPlatform())

	local source = [[
		__constant uint WhereIsIt[] = { 0x33333333, 0x55555555, 0xCCCCCCCC, 0xCDCDCDCD };
		__kernel void myfunc(	__global const float *a,
								__global const float *b,
								__global float *result) {
			int gid = get_global_id(0);
			// int i = 1;
			result[gid] = a[gid] + b[gid] + WhereIsIt[gid];
		};
	]]
	local program = context:CreateProgram(source)
	print("Created", program)
	program:Build()
	print("\nCollecting garbage...")
	program = nil
	collectgarbage()

	program = context:CreateProgram(source)
	program:Build("-cl-opt-disable")
	print(program:GetBuildStatus(devices[1]))
	print(program:GetBuildLog(devices[1]))
	print(program:GetBuildStatus(devices[2]))
	print(program:GetBuildLog(devices[2]))

	print("=======Binaries======")
	local binaries = {program:GetBinary()}
	for index, binary in ipairs(binaries) do
		print(("Lua: Length of binary #%d: %08X"):format(index, binary:len()))
		local file = io.open(index .. ".bin", "w")
		file:write(binary)
		file:close()
	end
	print("===End of Binaries===")

	print("\nCollecting garbage...")
	program = nil
	collectgarbage()
	-- context = platform:CreateContext(device)
end
