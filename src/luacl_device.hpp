#ifndef __LUACL_DEVICE_HPP
#define __LUACL_DEVICE_HPP

#include "LuaCL.h"
#include "luacl_api.h"
#include "luacl_object.h"

static const char LUACL_DEVICE_REGISTRY[] = "LuaCL.Registry.Device";
static const char LUACL_DEVICE_METATABLE[] = "LuaCL.Metatable.Device";
static const char LUACL_DEVICE_TOSTRING[] = "LuaCL_Device";

template <>
struct luacl_object_template_constants<cl_device_id> {
	static const char * REGISTRY() {
		return LUACL_DEVICE_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_DEVICE_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_DEVICE_TOSTRING;
	}
};

struct luacl_device {

	typedef luacl_object_template<cl_device_id> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_DEVICE_METATABLE);
		lua_newtable(L);
		//lua_pushcfunction(L, GetInfo);
		//lua_setfield(L, -1, "GetInfo");
		//lua_pushcfunction(L, GetDevices);
		//lua_setfield(L, -1, "GetDevices");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		traits::CreateRegistry(L);
	}

	static int Get(lua_State *L) {
		cl_platform_id platform = luacl_object_template<cl_platform_id>::CheckObject(L);

		cl_uint numDevices = 0;
		cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
		CheckCLError(L, err, "Failed requesting number of devices.", NULL);

		cl_device_id * devices = static_cast<cl_device_id *>(malloc(sizeof(cl_device_id) * numDevices));
		CheckAllocError(L, devices, "Insufficient memory.");
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
		CheckCLError(L, err, "Failed requesting platform list.", devices);

		for (cl_uint index = 0; index < numDevices; index++) {
			//printf("Wrapping device: %p\n", devices[index]);
			Wrap(L, devices[index]);
		}
		return static_cast<int>(numDevices);
	}

	static int Wrap(lua_State *L, cl_device_id device) {
		return traits::Wrap(L, device);
	}

	static cl_device_id CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_DEVICE_HPP */
