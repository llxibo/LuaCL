require("UnitTest.object")

print("Unit.Test.device loaded")
local _M = UnitTest.NewTest("device", "LuaCL_Device", "LuaCL_Device_Registry", "LuaCL_Device")

local deviceInfoKeys = {
	TYPE = "number",
	VENDOR_ID = "number",
	MAX_COMPUTE_UNITS = "number",
	MAX_WORK_ITEM_DIMENSIONS = "number",
	MAX_WORK_GROUP_SIZE = "number",
	MAX_WORK_ITEM_SIZES = "table",
	PREFERRED_VECTOR_WIDTH_CHAR = "number",
	PREFERRED_VECTOR_WIDTH_SHORT = "number",
	PREFERRED_VECTOR_WIDTH_INT = "number",
	PREFERRED_VECTOR_WIDTH_LONG = "number",
	PREFERRED_VECTOR_WIDTH_FLOAT = "number",
	PREFERRED_VECTOR_WIDTH_DOUBLE = "number",
	MAX_CLOCK_FREQUENCY = "number",
	ADDRESS_BITS = "number",
	MAX_READ_IMAGE_ARGS = "number",
	MAX_WRITE_IMAGE_ARGS = "number",
	MAX_MEM_ALLOC_SIZE = "number",
	IMAGE2D_MAX_WIDTH = "number",
	IMAGE2D_MAX_HEIGHT = "number",
	IMAGE3D_MAX_WIDTH = "number",
	IMAGE3D_MAX_HEIGHT = "number",
	IMAGE3D_MAX_DEPTH = "number",
	IMAGE_SUPPORT = "number",
	MAX_PARAMETER_SIZE = "number",
	MAX_SAMPLERS = "number",
	MEM_BASE_ADDR_ALIGN = "number",
	MIN_DATA_TYPE_ALIGN_SIZE = "number",
	SINGLE_FP_CONFIG = "number",
	GLOBAL_MEM_CACHE_TYPE = "number",
	GLOBAL_MEM_CACHELINE_SIZE = "number",
	GLOBAL_MEM_CACHE_SIZE = "number",
	GLOBAL_MEM_SIZE = "number",
	MAX_CONSTANT_BUFFER_SIZE = "number",
	MAX_CONSTANT_ARGS = "number",
	LOCAL_MEM_TYPE = "number",
	LOCAL_MEM_SIZE = "number",
	ERROR_CORRECTION_SUPPORT = "number",
	PROFILING_TIMER_RESOLUTION = "number",
	ENDIAN_LITTLE = "number",
	AVAILABLE = "number",
	COMPILER_AVAILABLE = "number",
	EXECUTION_CAPABILITIES = "number",
	QUEUE_PROPERTIES = "number",
	NAME = "string",
	VENDOR = "string",
	DRIVER_VERSION = "string",
	PROFILE = "string",
	VERSION = "string",
	EXTENSIONS = "string",
	DOUBLE_FP_CONFIG = "number",
	PREFERRED_VECTOR_WIDTH_HALF = "number",
	HOST_UNIFIED_MEMORY = "number",
	NATIVE_VECTOR_WIDTH_CHAR = "number",
	NATIVE_VECTOR_WIDTH_SHORT = "number",
	NATIVE_VECTOR_WIDTH_INT = "number",
	NATIVE_VECTOR_WIDTH_LONG = "number",
	NATIVE_VECTOR_WIDTH_FLOAT = "number",
	NATIVE_VECTOR_WIDTH_DOUBLE = "number",
	NATIVE_VECTOR_WIDTH_HALF = "number",
}

function _M.Test(platform)
	assert(platform.GetDevices)
	UnitTest.AssertRegEmpty("device")
	do
		local devices = {platform:GetDevices()}
		UnitTest.AssertRegMatch("device", devices)
		for index, device in ipairs(devices) do
			UnitTest.AssertObject("device", device)

			assert(device.GetInfo)
			local info = device:GetInfo()
			UnitTest.AssertInfoTable(info, deviceInfoKeys)
		end
	end
	collectgarbage()
	UnitTest.AssertRegEmpty("device")
end
