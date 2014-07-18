#ifndef __LUACI_INFO_H
#define __LUACI_INFO_H

#include "LuaCL.h"
#include <string>
#include <assert.h>

void CheckAllocError(lua_State *L, void *p, const char * msg);

void CheckCLError(lua_State *L, cl_uint err, const char * msg, void *p = NULL);

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
	//printf("%d: %d - %d\n", param, size, sizeof(T));
	assert(size == sizeof(T));
#endif
	T value = 0;
	cl_int err = clGetDeviceInfo(device, param, sizeof(T), &value, NULL);
	if (err != CL_SUCCESS) {
		value = 0;
	}
	return value;
}

template <typename T> void PushDeviceInfo(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
	T value = GetDeviceInfo<T>(device, param);
	lua_pushstring(L, key.c_str());
	lua_pushnumber(L, static_cast<lua_Number>(value));
	lua_settable(L, -3);
}

template <typename T> void PushDeviceInfoArray(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
	size_t size = 0;
	cl_int err = clGetDeviceInfo(device, param, 0, NULL, &size);
	if (err != CL_SUCCESS) {
		return;
	}
//#if _DEBUG && _MSC_VER
//	printf("Device Array: %Iu - %Iu\n", size, sizeof(T));
//#endif
	assert(size % sizeof(T) == 0);
	T * value = static_cast<T *>(malloc(size));
	if (value == NULL) {
		return;
	}
	err = clGetDeviceInfo(device, param, size, value, NULL);
	if (err != CL_SUCCESS) {
        free(value);
		return;
	}
	lua_pushstring(L, key.c_str());
	lua_newtable(L);
	for (unsigned int index = 0; index < (size / sizeof(T)); index++) {
		lua_pushnumber(L, static_cast<lua_Number>(value[index]));
		lua_rawseti(L, -2, index + 1);
	}
    free(value);
	lua_settable(L, -3);
}

void PushDeviceInfoStr(lua_State *L, cl_device_id device, cl_device_info param, std::string key);

#endif
