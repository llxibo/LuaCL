#include "luacl_api.h"

static int luacl_GetNumPlatforms(lua_State *L) {
	lua_pushnumber(L, GetNumPlatforms());
	return 1;
}

static int luacl_GetPlatformInfo(lua_State *L) {
	cl_uint index = static_cast<cl_uint>(luaL_checknumber(L, 1));
	cl_platform_id platformId = GetPlatformId(index - 1);
	if (platformId == NULL) {
		return 0;
	}

	lua_newtable(L);	/* Create a table */
	PushPlatformInfo(L, platformId, CL_PLATFORM_PROFILE, "profile");
	PushPlatformInfo(L, platformId, CL_PLATFORM_VERSION, "version");
	PushPlatformInfo(L, platformId, CL_PLATFORM_NAME, "name");
	PushPlatformInfo(L, platformId, CL_PLATFORM_VENDOR, "vendor");
	PushPlatformInfo(L, platformId, CL_PLATFORM_EXTENSIONS, "extensions");
	return 1;
}

static int luacl_GetNumDevices(lua_State *L) {
	cl_uint index = static_cast<cl_uint>(luaL_checkinteger(L, 1));
	cl_platform_id platform = GetPlatformId(index - 1);
	lua_pushnumber(L, GetNumDevices(platform));
	return 1;
}

static int luacl_GetDeviceInfo(lua_State *L) {
	cl_uint platformIndex = static_cast<cl_uint>(luaL_checkinteger(L, 1));
	cl_platform_id platform = GetPlatformId(platformIndex - 1);
	if (platform == NULL) {
		return 0;
	}

	cl_uint deviceIndex = static_cast<cl_uint>(luaL_checkinteger(L, 2));
	cl_device_id device = GetDeviceId(platform, deviceIndex - 1);
	if (device == NULL) {
		return 0;
	}

	lua_newtable(L);
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_VENDOR_ID, "VENDOR_ID");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_MAX_COMPUTE_UNITS, "MAX_COMPUTE_UNITS");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "MAX_WORK_ITEM_DIMENSIONS");
	PushDeviceInfo<size_t>(L, device, CL_DEVICE_MAX_WORK_GROUP_SIZE, "MAX_WORK_GROUP_SIZE");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_MAX_CLOCK_FREQUENCY, "MAX_CLOCK_FREQUENCY");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_ADDRESS_BITS, "ADDRESS_BITS");
	PushDeviceInfo<cl_device_mem_cache_type>(L, device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, "GLOBAL_MEM_CACHE_TYPE");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "GLOBAL_MEM_CACHELINE_SIZE");
	/* cl_ulong seems to be problematic */
	// PushDeviceInfo<cl_ulong>(L, device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, "MAX_MEM_ALLOC_SIZE");
	// PushDeviceInfo<cl_ulong>(L, device, CL_DEVICE_LOCAL_MEM_SIZE, "LOCAL_MEM_SIZE");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_MAX_CONSTANT_ARGS, "MAX_CONSTANT_ARGS");
	PushDeviceInfo<cl_bool>(L, device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, "ERROR_CORRECTION_SUPPORT");
	PushDeviceInfo<cl_bool>(L, device, CL_DEVICE_HOST_UNIFIED_MEMORY, "HOST_UNIFIED_MEMORY");
	PushDeviceInfo<size_t>(L, device, CL_DEVICE_PROFILING_TIMER_RESOLUTION, "PROFILING_TIMER_RESOLUTION");
	PushDeviceInfo<cl_bool>(L, device, CL_DEVICE_ENDIAN_LITTLE, "ENDIAN_LITTLE");
	PushDeviceInfo<cl_bool>(L, device, CL_DEVICE_AVAILABLE, "AVAILABLE");
	PushDeviceInfo<cl_bool>(L, device, CL_DEVICE_COMPILER_AVAILABLE, "COMPILER_AVAILABLE");
	PushDeviceInfoStr(L, device, CL_DEVICE_NAME, "NAME");
	PushDeviceInfoStr(L, device, CL_DEVICE_VENDOR, "VENDOR");
	PushDeviceInfoStr(L, device, CL_DEVICE_VERSION, "VERSION");
	PushDeviceInfoStr(L, device, CL_DEVICE_PROFILE, "PROFILE");
	PushDeviceInfoStr(L, device, CL_DRIVER_VERSION, "DRIVER_VERSION");
	PushDeviceInfoStr(L, device, CL_DEVICE_EXTENSIONS, "EXTENSIONS");
	return 1;
}

static int luacl_CreateContext(lua_State *L) {
	cl_uint platformIndex = static_cast<cl_uint>(luaL_checkinteger(L, 1));
	cl_platform_id platform = GetPlatformId(platformIndex - 1);
	if (platform == NULL) {
		return luaL_error(L, "CreateContext: invalid platformIndex %d", platformIndex);
	}

	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0
	};
	
	luaL_checktype(L, 2, LUA_TTABLE);

	cl_uint size = static_cast<cl_uint>(lua_objlen(L, 2));
	//printf("Total: %d\n", size);

	cl_device_id * devices = static_cast<cl_device_id *>(malloc(size * sizeof(cl_device_id)));
	for (size_t index = 0; index < size; index++) {
		lua_rawgeti(L, 2, static_cast<lua_Number>(index + 1));
		cl_uint deviceIndex = static_cast<cl_uint>(lua_tonumber(L, -1));
		// printf("Device %d: %d\n", index, deviceIndex);
		devices[index] = GetDeviceId(platform, deviceIndex - 1);
		if (devices[index] == NULL) {
			free(devices);
			return luaL_error(L, "CreateContext: invalid deviceIndex %d", index);
		}
		lua_pop(L, 1);
	}

	cl_int err;
	cl_context context = clCreateContext(properties, size, devices, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		return luaL_error(L, "CreateContext: failed creating context - code %d", err);
	}

	cl_context * udata_context = static_cast<cl_context*>(lua_newuserdata(L, sizeof(cl_context *)));
	*udata_context = context;

	luaL_getmetatable(L, LUACL_UDATA_CONTEXT);
	lua_setmetatable(L, -2);

	return 1;
}

int luacl_ReleaseContext(lua_State *L) {
	cl_context * udata_context = static_cast<cl_context *>(luaL_checkudata(L, 1, LUACL_UDATA_CONTEXT));
	printf("__gc: Releasing context %p\n", udata_context);
	clReleaseContext(*udata_context);
	return 0;
}

int luacl_CreateProgram(lua_State *L) {
	return 0;
}

static const struct luaL_Reg LuaCL_API[] = {
	{ "CreateProgram", luacl_CreateProgram },
	{ "GetNumPlatforms", luacl_GetNumPlatforms },
	{ "GetPlatformInfo", luacl_GetPlatformInfo },
	{ "GetNumDevices", luacl_GetNumDevices },
	{ "GetDeviceInfo", luacl_GetDeviceInfo },
	{ "CreateContext", luacl_CreateContext },
	{ NULL, NULL }
};

LUALIB_API int luaopen_LuaCL(lua_State *L) {
	luaL_register(L, LUACL_GLOBAL_NAME, LuaCL_API);
    
	luaL_newmetatable(L, LUACL_UDATA_CONTEXT);
	lua_pushcfunction(L, luacl_ReleaseContext);
	lua_setfield(L, -2, "__gc");
	return 1;
}
