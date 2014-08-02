dofile("dump_table.lua")

print("LuaCL environment test")

RegisterDebugCallback(print)

function load()
	dofile("UnitTest.lua")
end
xpcall(load, function (err)
	print(debug.traceback(err))
end)
do return end

local platforms = {GetPlatform()}

dump_table(platforms, "platforms")

for index, platform in ipairs(platforms) do
	local info = platform:GetInfo()
	dump_table(info, ("(%s):GetInfo()"):format(tostring(platform)))

	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices()"):format(tostring(platform)))
	devices = nil

	local devices = {platform:GetDevices()}
	dump_table(devices, ("(%s):GetDevices()"):format(tostring(platform)))

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
	dump_table(devices, ("(%s):GetDevices()"):format(tostring(platform)))

	for index, device in ipairs(devices) do
		local info = device:GetInfo()
		dump_table(info, ("(%s):GetInfo()"):format(tostring(device)))
	end

	-- print(platform.CreateContext(platform, devices))
	-- devices = {devices[1]}
	local context = platform:CreateContext(devices)
	print("Created", context)
	print("Got devices:", context:GetDevices())
	devices = {context:GetDevices()}
	print("Got platform:", context:GetPlatform())

	local source = [[
		__constant uint WhereIsIt[] = { 0x33333333, 0x55555555, 0xCCCCCCCC, 0xCDCDCDCD };
		__kernel void myfunc(   __global const float *a,
								__global const float *b,
								__global float *result) {
			int gid = get_global_id(0);
			// int i = 1;
			result[gid] = a[gid] + b[gid] + WhereIsIt[gid];
		};
	]]
	local program = context:CreateProgram(source)
	print("Created", program)
	print("Context of program:", program:GetContext())
	print("Devices of program:", program:GetDevices())

	devices = {program:GetDevices()}
	program:Build()
	print("\nCollecting garbage...")
	program = nil
	collectgarbage()

	program = context:CreateProgram(source)
	program:Build("-cl-opt-disable")
	for index, device in ipairs(devices) do
		print(program:GetBuildStatus(device))
		print(program:GetBuildLog(device))
	end

	if program:GetBuildStatus(devices[1]) == 0 then
		print("=======Binaries======")
		local binaries = {program:GetBinary()}
		for index, binary in ipairs(binaries) do
			print(("Lua: Length of binary #%d: %08X"):format(index, binary:len()))
			local file = io.open(index .. ".bin", "w")
			file:write(binary)
			file:close()
		end
		print("===End of Binaries===")

		print("\nCreating program from binaries...")
		local newProgram, status = context:CreateProgramFromBinary(devices, binaries)
		print("Created binary ", tostring(newProgram))
		dump_table(status, "build_status")
		newProgram:Build()
		print("Program built from binary:", tostring(newProgram))
	else
		print("Cannot dump binaries for unsuccessful build")
	end
	print("\nCreating kernel...")
	local kernel = program:CreateKernel("myfunc")
	print(kernel)

	print("GetContext", kernel:GetContext())
	print("GetProgram", kernel:GetProgram())
	print("GetNumArgs", kernel:GetNumArgs())
	print("GetFunctionName", kernel:GetFunctionName())

	for index, device in ipairs(devices) do
		local info = kernel:GetWorkGroupInfo(device)
		-- print("WorkGroupInfo: ", tostring(device))
		dump_table(info, ("(%s):GetWorkGroupInfo(%s)"):format(tostring(kernel), tostring(device)))

		local cmdqueue = context:CreateCommandQueue(device)
		print("\nCreated command queue:", tostring(cmdqueue))
	end
	-- for index = 1, kernel:GetNumArgs() do
	--  kernel:SetArgFloat(index, index)
	-- end
	local numTests = 1
	for index = 1, numTests do
		local bufferSize = 1048576 * 16
		local buffer = context:CreateBuffer(bufferSize)
		print("Created buffer ", buffer)
		local startTime = os.clock()
		for index = 0, bufferSize / buffer:GetSizeInt() - 1 do
			buffer:SetInt(index, index)
		end
		print("Write time ", os.clock() - startTime)
		local startTime = os.clock()
		for index = 0, bufferSize / buffer:GetSizeInt() - 1 do
			-- buffer:SetInt(index, index)
			buffer:GetInt(index)
			-- assert(buffer:GetInt(index) == index, index)
		end
		print("Read time ", os.clock() - startTime)

		local startTime = os.clock()
		buffer:ReverseEndianInt()
		print("ReverseTime ", os.clock() - startTime)

		buffer = nil
		collectgarbage()
	end
end
print("\nCollecting garbage...")
collectgarbage()
