#ifndef __LUACI_INFO_H
#define __LUACI_INFO_H

#include "LuaCL.h"
#include <string>
#include <assert.h>

cl_uint GetNumPlatforms();

cl_platform_id GetPlatformId(cl_uint index);

cl_uint GetNumDevices(cl_platform_id platform);

cl_device_id GetDeviceId(cl_platform_id platform, cl_uint index);

cl_uint GetContextReferenceCount(cl_context context);

void PushDeviceInfoStr(lua_State *L, cl_device_id device, cl_device_info param, std::string key);

#endif
