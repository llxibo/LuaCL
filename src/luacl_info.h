#ifndef __LUACI_INFO_H
#define __LUACI_INFO_H

#include "LuaCL.h"
#include <string>
#include <assert.h>

void PushPlatformInfo(lua_State *L, cl_platform_id platform, cl_platform_info param, std::string key);

cl_uint GetNumPlatforms();

cl_platform_id GetPlatformId(cl_uint index);

cl_uint GetNumDevices(cl_platform_id platform);

cl_device_id GetDeviceId(cl_platform_id platform, cl_uint index);

cl_uint GetContextReferenceCount(cl_context context);

template <typename T> T GetDeviceInfo(cl_device_id device, cl_device_info param) {
#if _DEBUG
	size_t size = 0;
	cl_int errDebug = clGetDeviceInfo(device, param, 0, NULL, &size);
	assert(size == sizeof(T));
#endif
	T value;
	cl_int err = clGetDeviceInfo(device, param, sizeof(T), &value, NULL);
	if (err != CL_SUCCESS) {
		value = 0;
	}
	return value;
}

template <typename T> void PushDeviceInfo(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
	T value = GetDeviceInfo<size_t>(device, param);
	lua_pushstring(L, key.c_str());
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void PushDeviceInfoStr(lua_State *L, cl_device_id device, cl_device_info param, std::string key);

#endif
