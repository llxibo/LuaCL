#include "luacl_info.h"
#include <assert.h>

void PushPlatformInfo(lua_State *L, cl_platform_id platform, cl_platform_info param, std::string key) {
	/* Check size of value */
	size_t size = 0;
	cl_int err = clGetPlatformInfo(platform, param, 0, NULL, &size);
	if (err != CL_SUCCESS) {
		return;
	}

	/* Request platform parameter */
	char * value = (char *)malloc(sizeof(char) * size);
	err = clGetPlatformInfo(platform, param, size, value, NULL);
	if (err != CL_SUCCESS) {
		free(value);
		return;
	}

	/* Push key and value to table */
	lua_pushstring(L, key.c_str());
	lua_pushlstring(L, value, size);
	lua_settable(L, -3);
}

cl_uint GetNumPlatforms() {
	cl_uint numPlatforms = 0;
	cl_int errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (errNum != CL_SUCCESS) {
		return 0;
	}
	return numPlatforms;
}

cl_platform_id GetPlatformId(cl_uint index) {
	cl_uint numPlatforms = GetNumPlatforms();
	if (index < 0 || index >= numPlatforms) {
		return NULL;
	}
	cl_platform_id *platformIds = static_cast<cl_platform_id *>(malloc(sizeof(cl_platform_id) * numPlatforms));
	cl_int errNum = clGetPlatformIDs(index + 1, platformIds, NULL);
	cl_platform_id platformId = platformIds[index];
	if (errNum != CL_SUCCESS) {
		platformId = NULL;
	}
	free(platformIds);
	return platformId;
}

cl_uint GetNumDevices(cl_platform_id platform) {
	if (platform == NULL) {
		return 0;
	}
	cl_uint numDevices = 0;
	cl_int errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	if (errNum != CL_SUCCESS) {
		return 0;
	}
	return numDevices;
}

cl_device_id GetDeviceId(cl_platform_id platform, cl_uint index) {
	cl_uint numDevices = GetNumDevices(platform);
	if (numDevices == 0) {
		return NULL;
	}
	cl_device_id *deviceIds = static_cast<cl_device_id *>(malloc(sizeof(cl_device_id) * numDevices));
	cl_int errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds, &numDevices);
	cl_device_id deviceId = deviceIds[index];
	if (errNum != CL_SUCCESS) {
		deviceId = NULL;
	}
	free(deviceIds);
	return deviceId;
}

template <typename T> T GetDeviceInfo(cl_device_id device, cl_device_info param) {
#if _DEBUG
	size_t size = 0;
	cl_int errNumDebug = clGetDeviceInfo(device, param, 0, NULL, &size);
	assert(size == sizeof(T));
#endif
	T value;
	cl_int errNum = clGetDeviceInfo(device, param, sizeof(T), &value, NULL);
	if (errNum == CL_SUCCESS) {
		value = 0;
	}
	return value;
}
