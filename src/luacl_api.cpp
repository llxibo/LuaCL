#include "luacl_api.h"

int luacl_GetNumPlatforms(lua_State *L) {
	lua_pushnumber(L, GetNumPlatforms());
	return 1;
}

int luacl_GetPlatformInfo(lua_State *L) {
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

int luacl_GetNumDevices(lua_State *L) {
	cl_platform_id platform = NULL;
	lua_pushnumber(L, GetNumDevices(platform));
	return 1;
}
