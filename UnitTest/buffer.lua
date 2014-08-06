require("UnitTest.object")

print("UnitTest.buffer loaded")
local _M = UnitTest.NewTest("buffer", "LuaCL_Buffer", "LuaCL_Buffer_Registry", "LuaCL_Buffer")

local bufferTypes = {
	Int = 4,
	-- Float = 4,
	-- Double = 8,
	-- Short = 2,
	-- Char = 1,
}

function _M.Test(context)
	local sizeFactorMax = 10
	local sizeFactorUnit = 2
	local sizeFactorBase = 4

	local randomBase = 255

	assert(context)
	assert(context.CreateBuffer)

	for sizeFactor = 1, sizeFactorMax do
		local size = (sizeFactorBase ^ sizeFactor) * sizeFactorUnit
		print("Testing buffer size", size)
		UnitTest.AssertRegEmpty("buffer")
		do
			local buffer = context:CreateBuffer(size)
			UnitTest.AssertObject("buffer", buffer)
			UnitTest.AssertRegMatch("buffer", {buffer})

			assert(buffer.GetBufferSize)
			assert(buffer:GetBufferSize() == size)
			for bufferType, bufferSize in pairs(bufferTypes) do
				print("Testing", bufferType, bufferSize)
				local get = buffer["Get" .. bufferType]
				local set = buffer["Set" .. bufferType]
				local varSize = buffer["GetSize" .. bufferType]()
				assert(varSize == bufferSize)

				assert(get)
				assert(set)

				-- Check lower boundary
				local ret, err = pcall(set, buffer, -1)
				assert(not ret)
				assert(err == "Buffer access out of bound.")

				-- Check upper boundary
				local ret, err = pcall(set, buffer, size / bufferSize)
				assert(not ret)
				assert(err == "Buffer access out of bound.")

				-- Check with RNG data sequence
				local seed = os.clock()
				math.randomseed(seed)
				for index = 0, size / bufferSize - 1 do
					local value = math.random() * randomBase
					-- print(set, index, value)
					set(buffer, index, value)
				end
				math.randomseed(seed)
				for index = 0, size / bufferSize - 1 do
					local value = math.random() * randomBase
					local readValue = get(buffer, index)
					-- print(get, index, value, buffer[get](buffer, index))
					assert(readValue == value or readValue == math.floor(value))
				end
			end
		end
		UnitTest.AssertRegEmpty("buffer")
	end
end
