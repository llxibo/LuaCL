print("LuaCL unit test")

-- debugInfo is an array that allows indexing values from the end with negative index
local debugInfo = setmetatable({}, {
    __index = function (t, key)
        if key < 0 then
            return rawget(t, #t + key)
        end
    end
})

local function debugCallback(msg)
    print("Debug:", msg)
    table.insert(debugInfo, msg)
end

RegisterDebugCallback(debugCallback)

-- require "UnitTest.platform"

-- -- function err(msg)
-- --   print("Error: " .. debug.traceback(msg, 2))
-- -- end
-- xpcall(UnitTest.platform.Test, err)
-- UnitTest.platform.Test()

require "UnitTest.helloworld"
UnitTest.HelloWorld()
print("All tests passed")
