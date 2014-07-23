#include "luacl_info.h"

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
	if (platformIds == NULL) {
		return NULL;
	}
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
	if (deviceIds == NULL) {
		return NULL;
	}
	cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds, &numDevices);
	cl_device_id deviceId = deviceIds[index];
	if (err != CL_SUCCESS) {
		deviceId = NULL;
	}
	free(deviceIds);
	return deviceId;
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
