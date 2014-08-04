require("UnitTest.object")

local CL_BUILD_SUCCESS = 0
local CL_BUILD_NONE = -1
local CL_BUILD_ERROR = -2
local CL_BUILD_IN_PROGRESS = -3

print("UnitTest.program loaded")
local _M = UnitTest.NewTest("program", "LuaCL_Program", "LuaCL_Program_Registry", "LuaCL_Program")

local source = [[
	__constant uint WhereIsIt[] = { 0x33333333, 0x55555555, 0xCCCCCCCC, 0xCDCDCDCD };
	__kernel void myfunc(   __global const float *a,
							__global const float *b,
							__global float *result) {
		int gid = get_global_id(0);
		result[gid] = a[gid] + b[gid];
	};
]]
local funcName = "myfunc"
local numArgs = 3

function _M.Test(context)
	print("Testing program")
	assert(context.CreateProgram)
	assert(context.CreateProgramFromBinary)

	local devices = {context:GetDevices()}
	local binaries

	require "UnitTest.kernel"

	UnitTest.AssertRegEmpty("program")
	do
		local program = context:CreateProgram(source)
		_M.TestProgram(program, context, devices)

		binaries = {program:GetBinary()}
		for index, binary in ipairs(binaries) do
			assert(type(binary) == "string")
			assert(binary:len() > 0)
		end

		UnitTest.kernel.Test(program, funcName, numArgs)
	end
	UnitTest.AssertRegEmpty("program")
	do
		local programFromBinary, binaryStatus = context:CreateProgramFromBinary(devices, binaries)
		for index, status in ipairs(binaryStatus) do
			assert(status == CL_BUILD_SUCCESS)
		end
		_M.TestProgram(programFromBinary, context, devices)
		UnitTest.kernel.Test(programFromBinary, funcName, numArgs)
	end
	UnitTest.AssertRegEmpty("program")
end

function _M.TestProgram(program, context, devices)
	UnitTest.AssertObject("program", program)
	UnitTest.AssertRegMatch("program", {program})

	assert(program.GetContext)
	assert(program.GetDevices)
	assert(program.GetBuildStatus)
	assert(program.GetBuildLog)
	assert(program.Build)

	local programContext = program:GetContext()
	assert(context == programContext)

	local programDevices = {program:GetDevices()}
	UnitTest.MatchTableValue(programDevices, devices)	-- Rough match
	-- for index, device in ipairs(programDevices) do		-- Exact match, not always apply
	-- 	assert(devices[index] == device)
	-- end

	for index, device in ipairs(programDevices) do
		local buildStatus = program:GetBuildStatus(device)
		assert(buildStatus == CL_BUILD_NONE)

		local buildInfo = program:GetBuildLog(device)
		assert(buildInfo:len() == 0)		-- Build not done, expecting empty log
	end

	program:Build()

	for index, device in ipairs(programDevices) do
		local buildStatus = program:GetBuildStatus(device)
		assert(buildStatus == CL_BUILD_SUCCESS)

		local buildInfo = program:GetBuildLog(device)
		assert(type(buildInfo) == "string")
		assert(buildInfo:len() == 0)
	end
end
