#include "luacl_info.h"

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
	std::string value_s(value, size);	/* Construct string with max length for safety constraint */
	lua_pushstring(L, key.c_str());
	lua_pushstring(L, value_s.c_str());
	lua_settable(L, -3);
}

cl_uint GetNumPlatforms() {
	cl_uint numPlatforms = 0;
	cl_int err = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (err != CL_SUCCESS) {
		return 0;
	}
	return numPlatforms;
}

cl_platform_id GetPlatformId(cl_uint index) {
	cl_uint numPlatforms = GetNumPlatforms();
	if (index >= numPlatforms) {
		return NULL;
	}
	cl_platform_id *platformIds = static_cast<cl_platform_id *>(malloc(sizeof(cl_platform_id) * numPlatforms));
	cl_int err = clGetPlatformIDs(index + 1, platformIds, NULL);
	cl_platform_id platformId = platformIds[index];
	if (err != CL_SUCCESS) {
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
	cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	if (err != CL_SUCCESS) {
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
	cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds, &numDevices);
	cl_device_id deviceId = deviceIds[index];
	if (err != CL_SUCCESS) {
		deviceId = NULL;
	}
	free(deviceIds);
	return deviceId;
}

void PushDeviceInfoStr(lua_State *L, cl_device_id device, cl_device_info param, std::string key) {
	if (device == NULL) {
		return;
	}
	size_t size = 0;
	cl_int err = clGetDeviceInfo(device, param, 0, NULL, &size);
	if (err != CL_SUCCESS || size == 0) {
		return;
	}
	char * value = static_cast<char *>(malloc(sizeof(char) * size));
	err = clGetDeviceInfo(device, param, size, value, NULL);
	if (err != CL_SUCCESS) {
		return;
	}

	std::string value_s(value, size);
	lua_pushstring(L, key.c_str());
	lua_pushstring(L, value_s.c_str());
	lua_settable(L, -3);
}

cl_uint GetContextReferenceCount(cl_context context) {
	cl_uint count = 0;
	cl_int err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &count, NULL);
	if (err != CL_SUCCESS) {
		printf("GetContextReferenceCount failed\n");
		return 0;
	}
	printf("context ref: %d\n", count);
	return count;
}
