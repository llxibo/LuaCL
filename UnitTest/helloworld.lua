require("UnitTest.object")

local CL_DEVICE_TYPE_GPU = 4
local bufferSize = 1024

print("UnitTest.helloworld loaded")

local source = [[
    __kernel void helloworld(   __global const float *a,
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
    local workSize
    for index = 1, 3 do
        local buffer
        if index < 3 then
            buffer = context:CreateBuffer(bufferSize)
        else
            buffer = context:CreateBuffer(bufferSize)
        end
        assert(buffer)
        buffer:Clear()
        workSize = bufferSize / buffer:GetSizeFloat()
        for index = 0, workSize - 1 do
            buffer:SetFloat(index, index)
        end
        kernel:SetArg(index, buffer)
        buffers[index] = buffer
    end
    local event1 = queue:EnqueueWriteBuffer(buffers[1])
    print("event1", event1)
    local event2 = queue:EnqueueWriteBuffer(buffers[2])
    print("event2", event2)
    local event = queue:EnqueueNDRangeKernel(kernel, nil, {workSize}, nil, {event1, event2})
    print(event)
    queue:EnqueueReadBuffer(buffers[3], {event})
    queue:Finish()
    for index = 0, workSize - 1 do
        local value = buffers[3]:GetFloat(index)
        assert(value == index * 2, ("Result mismatch: index %d got %d"):format(index, value))
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
