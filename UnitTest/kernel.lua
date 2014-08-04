require("UnitTest.object")

local CL_BUILD_SUCCESS = 0
local CL_BUILD_NONE = -1
local CL_BUILD_ERROR = -2
local CL_BUILD_IN_PROGRESS = -3

print("UnitTest.kernel loaded")
local _M = UnitTest.NewTest("kernel", "LuaCL_Kernel", "LuaCL_Kernel_Registry", "LuaCL_Kernel")

local kernelInfoKeys = {
	WORK_GROUP_SIZE = "number",
	COMPILE_WORK_GROUP_SIZE = "table",
	LOCAL_MEM_SIZE = "number",
	PREFERRED_WORK_GROUP_SIZE_MULTIPLE = "number",
	PRIVATE_MEM_SIZE = "number",
}

function _M.Test(program, funcName, numArgs)
	assert(program)
	assert(program.CreateKernel)

	local context = program:GetContext()
	local devices = {program:GetDevices()}

	UnitTest.AssertRegEmpty("kernel")
	do
		local kernel = program:CreateKernel(funcName)
		UnitTest.AssertObject("kernel", kernel)
		UnitTest.AssertRegMatch("kernel", {kernel})

		assert(kernel.GetContext)
		assert(kernel.GetProgram)
		local kernelContext = kernel:GetContext()
		local kernelProgram = kernel:GetProgram()
		UnitTest.AssertObject("context", kernelContext)
		UnitTest.AssertObject("program", kernelProgram)
		assert(kernelContext == context)
		assert(kernelProgram == program)

		assert(kernel.GetNumArgs)
		assert(kernel.GetFunctionName)
		local kernelNumArgs = kernel:GetNumArgs()
		local kernelFuncName = kernel:GetFunctionName()
		assert(type(kernelNumArgs) == "number")
		assert(type(kernelFuncName) == "string")
		assert(kernelNumArgs == numArgs)
		assert(kernelFuncName == funcName)

		assert(kernel.GetWorkGroupInfo)
		for index, device in ipairs(devices) do
			local info = kernel:GetWorkGroupInfo(device)
			assert(type(info) == "table")
			UnitTest.AssertInfoTable(info, kernelInfoKeys)
		end
	end
	UnitTest.AssertRegEmpty("kernel")
end
