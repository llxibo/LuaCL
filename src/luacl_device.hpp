#ifndef __LUACL_DEVICE_HPP
#define __LUACL_DEVICE_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include <assert.h>
#include <string>

static const char LUACL_DEVICE_REGISTRY[] = "LuaCL.Registry.Device";
static const char LUACL_DEVICE_METATABLE[] = "LuaCL.Metatable.Device";
static const char LUACL_DEVICE_TOSTRING[] = "LuaCL_Device";

template <>
struct luacl_object_template_constants<cl_device_id> {
	static const char * REGISTRY() {
		return LUACL_DEVICE_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_DEVICE_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_DEVICE_TOSTRING;
	}
};

struct luacl_device {

	typedef luacl_object_template<cl_device_id> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_DEVICE_METATABLE);
		lua_newtable(L);
		lua_pushcfunction(L, GetInfo);
		lua_setfield(L, -2, "GetInfo");
		//lua_pushcfunction(L, GetDevices);
		//lua_setfield(L, -1, "GetDevices");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		traits::CreateRegistry(L);
	}

	static int Get(lua_State *L) {
		cl_platform_id platform = luacl_object_template<cl_platform_id>::CheckObject(L);

		cl_uint numDevices = 0;
		cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
		CheckCLError(L, err, "Failed requesting number of devices.", NULL);

		cl_device_id * devices = static_cast<cl_device_id *>(malloc(sizeof(cl_device_id) * numDevices));
		CheckAllocError(L, devices);
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
		CheckCLError(L, err, "Failed requesting platform list.", devices);

		for (cl_uint index = 0; index < numDevices; index++) {
			//printf("Wrapping device: %p\n", devices[index]);
			Wrap(L, devices[index]);
		}
		return static_cast<int>(numDevices);
	}

	static int GetInfo(lua_State *L) {
		cl_device_id device = traits::CheckObject(L);
		lua_newtable(L);
		PushDeviceInfo<cl_device_type>				(L, device, CL_DEVICE_TYPE,								"TYPE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_VENDOR_ID,						"VENDOR_ID");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_COMPUTE_UNITS,				"MAX_COMPUTE_UNITS");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,			"MAX_WORK_ITEM_DIMENSIONS");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_MAX_WORK_GROUP_SIZE,				"MAX_WORK_GROUP_SIZE");
		PushDeviceInfoArray<size_t>					(L, device, CL_DEVICE_MAX_WORK_ITEM_SIZES,				"MAX_WORK_ITEM_SIZES");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,		"PREFERRED_VECTOR_WIDTH_CHAR");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,		"PREFERRED_VECTOR_WIDTH_SHORT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,		"PREFERRED_VECTOR_WIDTH_INT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,		"PREFERRED_VECTOR_WIDTH_LONG");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,		"PREFERRED_VECTOR_WIDTH_FLOAT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,	"PREFERRED_VECTOR_WIDTH_DOUBLE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_CLOCK_FREQUENCY,				"MAX_CLOCK_FREQUENCY");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_ADDRESS_BITS,						"ADDRESS_BITS");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_READ_IMAGE_ARGS,				"MAX_READ_IMAGE_ARGS");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS,				"MAX_WRITE_IMAGE_ARGS");
		PushDeviceInfo<cl_ulong>					(L, device, CL_DEVICE_MAX_MEM_ALLOC_SIZE,				"MAX_MEM_ALLOC_SIZE");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_IMAGE2D_MAX_WIDTH,				"IMAGE2D_MAX_WIDTH");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_IMAGE2D_MAX_HEIGHT,				"IMAGE2D_MAX_HEIGHT");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_IMAGE3D_MAX_WIDTH,				"IMAGE3D_MAX_WIDTH");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_IMAGE3D_MAX_HEIGHT,				"IMAGE3D_MAX_HEIGHT");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_IMAGE3D_MAX_DEPTH,				"IMAGE3D_MAX_DEPTH");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_IMAGE_SUPPORT,					"IMAGE_SUPPORT");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_MAX_PARAMETER_SIZE,				"MAX_PARAMETER_SIZE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_SAMPLERS,						"MAX_SAMPLERS");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MEM_BASE_ADDR_ALIGN,				"MEM_BASE_ADDR_ALIGN");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,			"MIN_DATA_TYPE_ALIGN_SIZE");
		PushDeviceInfo<cl_device_fp_config>			(L, device, CL_DEVICE_SINGLE_FP_CONFIG,					"SINGLE_FP_CONFIG");
		PushDeviceInfo<cl_device_mem_cache_type>	(L, device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,			"GLOBAL_MEM_CACHE_TYPE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,		"GLOBAL_MEM_CACHELINE_SIZE");
		PushDeviceInfo<cl_ulong>					(L, device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,			"GLOBAL_MEM_CACHE_SIZE");
		PushDeviceInfo<cl_ulong>					(L, device, CL_DEVICE_GLOBAL_MEM_SIZE,					"GLOBAL_MEM_SIZE");
		PushDeviceInfo<cl_ulong>					(L, device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,			"MAX_CONSTANT_BUFFER_SIZE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_MAX_CONSTANT_ARGS,				"MAX_CONSTANT_ARGS");
		PushDeviceInfo<cl_device_local_mem_type>	(L, device, CL_DEVICE_LOCAL_MEM_TYPE,					"LOCAL_MEM_TYPE");
		PushDeviceInfo<cl_ulong>					(L, device, CL_DEVICE_LOCAL_MEM_SIZE,					"LOCAL_MEM_SIZE");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_ERROR_CORRECTION_SUPPORT,			"ERROR_CORRECTION_SUPPORT");
		PushDeviceInfo<size_t>						(L, device, CL_DEVICE_PROFILING_TIMER_RESOLUTION,		"PROFILING_TIMER_RESOLUTION");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_ENDIAN_LITTLE,					"ENDIAN_LITTLE");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_AVAILABLE,						"AVAILABLE");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_COMPILER_AVAILABLE,				"COMPILER_AVAILABLE");
		PushDeviceInfo<cl_device_exec_capabilities>	(L, device, CL_DEVICE_EXECUTION_CAPABILITIES,			"EXECUTION_CAPABILITIES");
		PushDeviceInfo<cl_command_queue_properties>	(L, device, CL_DEVICE_QUEUE_PROPERTIES,					"QUEUE_PROPERTIES");
		PushDeviceInfoStr							(L, device, CL_DEVICE_NAME,								"NAME");
		PushDeviceInfoStr							(L, device, CL_DEVICE_VENDOR,							"VENDOR");
		PushDeviceInfoStr							(L, device, CL_DRIVER_VERSION,							"DRIVER_VERSION");
		PushDeviceInfoStr							(L, device, CL_DEVICE_PROFILE,							"PROFILE");
		PushDeviceInfoStr							(L, device, CL_DEVICE_VERSION,							"VERSION");
		PushDeviceInfoStr							(L, device, CL_DEVICE_EXTENSIONS,						"EXTENSIONS");
		PushDeviceInfo<cl_device_fp_config>			(L, device, CL_DEVICE_DOUBLE_FP_CONFIG,					"DOUBLE_FP_CONFIG");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,		"PREFERRED_VECTOR_WIDTH_HALF");
		PushDeviceInfo<cl_bool>						(L, device, CL_DEVICE_HOST_UNIFIED_MEMORY,				"HOST_UNIFIED_MEMORY");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,			"NATIVE_VECTOR_WIDTH_CHAR");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,		"NATIVE_VECTOR_WIDTH_SHORT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,			"NATIVE_VECTOR_WIDTH_INT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,			"NATIVE_VECTOR_WIDTH_LONG");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,		"NATIVE_VECTOR_WIDTH_FLOAT");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,		"NATIVE_VECTOR_WIDTH_DOUBLE");
		PushDeviceInfo<cl_uint>						(L, device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,			"NATIVE_VECTOR_WIDTH_HALF");
		return 1;
	}

	static int Wrap(lua_State *L, cl_device_id device) {
		return traits::Wrap(L, device);
	}

	static cl_device_id CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}

	static int PushDeviceInfoStr(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
		if (device == NULL) {
			return 0;
		}
		size_t size = 0;
		cl_int err = clGetDeviceInfo(device, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of device info string.", NULL);

		char * value = static_cast<char *>(malloc(sizeof(char) * size));
		CheckAllocError(L, value);
		err = clGetDeviceInfo(device, param, size, value, NULL);
		CheckCLError(L, err, "Failed requesting device info as string.", value);

		std::string value_s(value, size);
		free(value);
		lua_pushstring(L, key.c_str());
		lua_pushstring(L, value_s.c_str());
		lua_settable(L, -3);
		return 0;
	}

	template <typename T> static void PushDeviceInfo(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
		size_t size = 0;
		cl_int err = clGetDeviceInfo(device, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of device info.", NULL);
		//printf("%d: %d - %d\n", param, size, sizeof(T));
		assert(size == sizeof(T));
		T value = 0;
		err = clGetDeviceInfo(device, param, sizeof(T), &value, NULL);
		CheckCLError(L, err, "Failed requesting device info.", NULL);
		
		lua_pushstring(L, key.c_str());
		lua_pushnumber(L, static_cast<lua_Number>(value));
		lua_settable(L, -3);
	}

	template <typename T> static int PushDeviceInfoArray(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
		size_t size = 0;
		cl_int err = clGetDeviceInfo(device, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of device info array.", NULL);
		//#if _DEBUG && _MSC_VER
		//	printf("Device Array: %Iu - %Iu\n", size, sizeof(T));
		//#endif
		assert(size % sizeof(T) == 0);
		T * value = static_cast<T *>(malloc(size));
		CheckAllocError(L, value);

		err = clGetDeviceInfo(device, param, size, value, NULL);
		CheckCLError(L, err, "Failed requesting device info as array.", NULL);
		lua_pushstring(L, key.c_str());
		lua_newtable(L);
		for (unsigned int index = 0; index < (size / sizeof(T)); index++) {
			lua_pushnumber(L, static_cast<lua_Number>(value[index]));
			lua_rawseti(L, -2, index + 1);
		}
		free(value);
		lua_settable(L, -3);
		return 0;
	}
};

#endif /* __LUACL_DEVICE_HPP */
