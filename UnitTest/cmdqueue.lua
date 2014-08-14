require("UnitTest.object")

print("UnitTest.cmdqueue loaded")
local _M = UnitTest.NewTest("cmdqueue", "LuaCL_CmdQueue", "LuaCL_CmdQueue_Registry", "LuaCL_CmdQueue")

function _M.Test(context)
    assert(context)
    assert(context.CreateCommandQueue)

    UnitTest.AssertRegEmpty("cmdqueue")
    do
        local devices = {context:GetDevices()}
        for index, device in ipairs(devices) do
            local queue = context:CreateCommandQueue(device)
            UnitTest.AssertObject("cmdqueue", queue)
            UnitTest.AssertRegMatch("cmdqueue", {queue})

            assert(queue.EnqueueWriteBuffer)
            assert(queue.EnqueueReadBuffer)
            assert(queue.EnqueueNDRangeKernel)
        end
    end
    UnitTest.AssertRegEmpty("cmdqueue")
end
