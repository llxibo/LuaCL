require("UnitTest.object")

local CL_DEVICE_TYPE_GPU = 4
local bufferSize = 1024

print("UnitTest.helloworld loaded")

local source = [[
	__kernel void helloworld(	__global const float *a,
								__global const float *b,
								__global float *result
	) {
		int gid = get_global_id(0);
		result[gid] = a[gid] + b[gid];
	};
]]

function UnitTest.HelloWorld()
	local device = UnitTest.ChooseDevice(CL_DEVICE_TYPE_GPU)
		 or UnitTest.ChooseDevice(CL_DEVICE_TYPE_CPU)
		 or UnitTest.ChooseDevice()
	local platform = device:GetPlatform()
	assert(device)
	print("Using device ", device, device:GetInfo().VENDOR, device:GetInfo().MAX_CLOCK_FREQUENCY)

	local context = platform:CreateContext(device)
	local program = context:CreateProgram(source)
	program:Build()
	local kernel = program:CreateKernel("helloworld")
	assert(kernel)
	local queue = context:CreateCommandQueue(device)
	local buffers = {}
	for index = 1, 3 do
		buffers[index] = context:CreateBuffer(bufferSize)
		assert(buffers[index])
		kernel:SetArg(index, buffers[index])
	end
	buffers[1]:Clear()
	for index = 0, bufferSize / buffers[1]:GetSizeFloat() - 1 do
		buffers[1]:SetFloat(index, index)
		buffers[2]:SetFloat(index, index)
	end
	local event2 = queue:EnqueueWriteBuffer(buffers[2])
	local event = queue:EnqueueNDRangeKernel(kernel, {1}, {256}, nil, {event1, event2})
	print(event)
	queue:EnqueueReadBuffer(buffers[3], {event})
	queue:Finish()
	for index = 0, bufferSize / buffers[1]:GetSizeFloat() - 1 do
		print(buffers[3]:Get(index))
	end
end

function UnitTest.ChooseDevice(deviceType)
	local platforms = {GetPlatform()}
	for index, platform in ipairs(platforms) do
		local devices = {platform:GetDevices()}
		for index, device in ipairs(devices) do
			local info = device:GetInfo()
			if not deviceType or info.TYPE == deviceType then
				return device, platform
			end
		end
	end
end
