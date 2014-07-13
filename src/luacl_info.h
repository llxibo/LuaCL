#ifndef __LUACI_INFO_H
#define __LUACI_INFO_H

#include "LuaCL.h"
#include <string>

void PushPlatformInfo(lua_State *L, cl_platform_id platform, cl_platform_info param, std::string key);

cl_uint GetNumPlatforms();

cl_platform_id GetPlatformId(cl_uint index);

cl_uint GetNumDevices(cl_platform_id platform);

#endif
