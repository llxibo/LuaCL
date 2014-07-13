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
	cl_platform_id *platformIds = (cl_platform_id *)malloc(sizeof(cl_platform_id) * numPlatforms);
	cl_int errNum = clGetPlatformIDs(index + 1, platformIds, NULL);
	cl_platform_id platformId = platformIds[index];
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

//GetDeviceInfo(cl_platform_id platform, cl_device_info param) {

//}