#ifndef __LUACI_API_H
#define __LUACI_API_H

#include "LuaCL.h"
#include <stdlib.h>
#include <string>
#include "luacl_info.h"

static const char LUACL_UDATA_CONTEXT[] = "LuaCL.context";
static const char LUACL_GLOBAL_NAME[] = "LuaCL";

int luaopen_LuaCL(lua_State *L);

int luacl_GetNumPlatforms(lua_State *L);

int luacl_GetPlatformInfo(lua_State *L);

int luacl_GetNumDevices(lua_State *L);

int luacl_GetDeviceInfo(lua_State *L);

int luacl_CreateContext(lua_State *L);

int luacl_ReleaseContext(lua_State *L);

int luacl_CreateProgram(lua_State *L);

static const struct luaL_Reg LuaCL_API[] = {
	{ "CreateProgram", luacl_CreateProgram },
	{ "GetNumPlatforms", luacl_GetNumPlatforms },
	{ "GetPlatformInfo", luacl_GetPlatformInfo },
	{ "GetNumDevices", luacl_GetNumDevices },
	{ "GetDeviceInfo", luacl_GetDeviceInfo },
	{ "CreateContext", luacl_CreateContext },
	{ "ReleaseContext", luacl_ReleaseContext },
	{ NULL, NULL }
};

#endif
