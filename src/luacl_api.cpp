#include "luacl_api.h"

int luaopen_LuaCL(lua_State *L) {
	luaL_register(L, LUACL_GLOBAL_NAME, LuaCL_API);

	luaL_newmetatable(L, LUACL_UDATA_CONTEXT);
	lua_pushcfunction(L, luacl_ReleaseContext);
	lua_setfield(L, -2, "__gc");
	return 1;
}

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
	PushDeviceInfo<size_t>(L, device, CL_DEVICE_MAX_WORK_GROUP_SIZE, "MAX_WORK_GROUP_SIZE");
	PushDeviceInfo<cl_uint>(L, device, CL_DEVICE_MAX_CLOCK_FREQUENCY, "MAX_CLOCK_FREQUENCY");
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

	size_t size = lua_objlen(L, 2);
	//printf("Total: %d\n", size);

	cl_device_id * devices = static_cast<cl_device_id *>(malloc(size * sizeof(cl_device_id)));
	for (size_t index = 0; index < size; index++) {
		lua_rawgeti(L, 2, index + 1);
		cl_uint deviceIndex = static_cast<cl_uint>(lua_tonumber(L, -1));
		// printf("Device %d: %d\n", index, deviceIndex);
		devices[index] = GetDeviceId(platform, index);
		if (devices[index] == NULL) {
			free(devices);
			return luaL_error(L, "CreateContext: invalid deviceIndex %d", index);
		}
		lua_pop(L, 1);
	}

	cl_int err;
	cl_context context = clCreateContext(properties, size, devices, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		return luaL_error(L, "CreateContext: failed creating context");
	}

	cl_context * udata_context = static_cast<cl_context*>(lua_newuserdata(L, sizeof(cl_context *)));
	*udata_context = context;

	luaL_getmetatable(L, LUACL_UDATA_CONTEXT);
	lua_setmetatable(L, -2);

	return 1;
}

int luacl_ReleaseContext(lua_State *L) {
	cl_context * udata_context = static_cast<cl_context *>(luaL_checkudata(L, 1, LUACL_UDATA_CONTEXT));
	printf("__gc: Releasing context %X\n", reinterpret_cast<size_t>(udata_context));
	clReleaseContext(*udata_context);
	return 0;
}

int luacl_CreateProgram(lua_State *L) {
	return 0;
}
