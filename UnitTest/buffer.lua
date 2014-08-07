require("UnitTest.object")

print("UnitTest.buffer loaded")
local _M = UnitTest.NewTest("buffer", "LuaCL_Buffer", "LuaCL_Buffer_Registry", "LuaCL_Buffer")

local random = math.random
local floor = math.floor
local abs = math.abs

local bufferTypes = {
	Int = {
		size = 4,
		assert = function (arg1, arg2) return floor(arg1) == floor(arg2) end,
		random = function () return random(-2147483648, 2147483647) end,
	},
	Float = {
		size = 4,
		assert = function (arg1, arg2) return abs(arg1 - arg2) < 1e-6 end,
		random = function () return (random() - 0.5) * 1e6 end,
	},
	Double = {
		size = 8,
		assert = function (arg1, arg2) return arg1 == arg2 end,
		random = function () return (random() - 0.5) * 1e6 end,
	},
	Short = {
		size = 2,
		assert = function (arg1, arg2) return floor(arg1) == floor(arg2) end,
		random = function () return random(-32768, 32767) end,
	},
	Char = {
		size = 1,
		assert = function (arg1, arg2) return floor(arg1) == floor(arg2) end,
		random = function () return random(-128, 127) end,
	},
}

function _M.Test(context)
	local sizeFactorMax = 8
	local sizeFactorUnit = 2
	local sizeFactorBase = 4

	local randomBase = 255

	assert(context)
	assert(context.CreateBuffer)

	for sizeFactor = 1, sizeFactorMax do
		local size = (sizeFactorBase ^ sizeFactor) * sizeFactorUnit + sizeFactor
		-- print("Testing buffer size", size)
		UnitTest.AssertRegEmpty("buffer")
		do
			local buffer = context:CreateBuffer(size)
			UnitTest.AssertObject("buffer", buffer)
			UnitTest.AssertRegMatch("buffer", {buffer})

			assert(buffer.GetBufferSize)
			assert(buffer:GetBufferSize() == size)
			for typeName, typeInfo in pairs(bufferTypes) do
				-- print("Testing", typeName, typeInfo.size)
				local get = buffer["Get" .. typeName]
				local set = buffer["Set" .. typeName]
				assert(buffer["GetSize" .. typeName]() == typeInfo.size)
				assert(get)
				assert(set)
				local assertValue = typeInfo.assert
				local randomValue = typeInfo.random

				-- Check lower boundary
				local ret, err = pcall(get, buffer, -1)
				assert(not ret)
				assert(err == "Buffer access out of bound.")

				-- Check upper boundary
				local uBound = floor(size / typeInfo.size)
				-- print("Asserting ubound", uBound)
				local ret, err = pcall(get, buffer, uBound)
				assert(not ret)
				assert(err == "Buffer access out of bound.")

				assert(buffer.Clear)
				buffer:Clear()

				-- Check with RNG data sequence
				local seed = os.clock()
				math.randomseed(seed)
				for index = 0, size / typeInfo.size - 1 do
					assert(get(buffer, index) == 0)
					local value = randomValue()
					set(buffer, index, value)
				end
				math.randomseed(seed)
				for index = 0, size / typeInfo.size - 1 do
					local value = randomValue()
					local readValue = get(buffer, index)
					assertValue(value, readValue)
				end

				assert(buffer["ReverseEndian" .. typeName])
				buffer["ReverseEndian" .. typeName](buffer)
			end
		end
		UnitTest.AssertRegEmpty("buffer")
	end
end
