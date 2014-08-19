require("UnitTest.object")

local CL_DEVICE_TYPE_GPU = 4
local bufferSize = 1024 * 128

print("UnitTest.helloworld loaded")

local source = [[
    __kernel void helloworld(   __global const float *a,
                                __global const float *b,
                                __global float *result
    ) {
        int gid = get_global_id(0);
        int c = 0;
        for (int i = 0; i < 1024; i++) {
            c += (int)a[gid] | i;
        }
        result[gid] = a[gid] + b[gid] + c;
    };
]]

-- local source = [[
--     uchar4 color(float i)
--     {
--         if (i >= 256.0f)
--             return (uchar4)(0, 0, 0, 255);
--         float f = sin(i * 0.0061359f);
--         return convert_uchar4_rtz((float4)(f * 256.0f, f * f * 256.0f, f * f * 256.0f, 255.5f));
--     }
--     __kernel void mandelbrot(__global uchar4* image)
--     {
--         uint x_coord = get_global_id(0)*2;
--         uint y_coord = get_global_id(1)*2;
--         const uint width = get_global_size(0)*2;
--         const uint height = get_global_size(1)*2;
--         float x = 0.0f;
--         float y = 0.0f;
--         uint i = 0;
--         float dif, out = .0f;
--         int os = 4;

--         while (os-- > 0){
--             x = .0;
--             y = .0;
--             i = 0;
--             float x_origin = ((float)x_coord / width) * 2.5f * width / height - 2.0f * width / height;
--             float y_origin = ((float)y_coord / height) * 2.5f - 1.25f;
--             do{
--                 float tmp = x*x - y*y + x_origin;
--                 y = 2 * x*y + y_origin;
--                 x = tmp;
--                 i++;
--             } while (x*x + y*y < 4.f && i < 256);

--             if (i < 256)
--                 dif = i + 1.0f - log(log(x*x + y*y) * 0.5f / log(2.0f));
--             else
--                 dif = 1024.0f;
--             out += dif / 4.0f;

--             switch (os){
--             case 3: x_coord++; break;
--             case 2: x_coord--; y_coord++; break;
--             case 1: x_coord++; break;
--             default:break;
--             }
--         }
--         ulong p = (get_global_id(0) + get_global_id(1) * get_global_size(0));

--         image[p] = color(out);
--     };
-- ]]

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
    local start1 = os.clock()
    local event1 = queue:EnqueueWriteBuffer(buffers[1])
    print("event1", event1)
    local event2 = queue:EnqueueWriteBuffer(buffers[2])
    local start2 = os.clock()
    local event = queue:EnqueueNDRangeKernel(kernel, nil, {workSize}, nil, {event1, event2})
    print(event)
    queue:EnqueueReadBuffer(buffers[3], {event})
    print("event2", event2)
    queue:Finish()
    local start3 = os.clock()
    print(start1, start2, start3)
    print("Enqueue cost time ", start2 - start1)
    print("Calc cost time", start3 - start2)
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
