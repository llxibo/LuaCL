#ifndef __LUACL_PLATFORM_HPP
#define __LUACL_PLATFORM_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"

static const char LUACL_PLATFORM_REGISTRY[] = "LuaCL.Registry.Platform";
static const char LUACL_PLATFORM_METATABLE[] = "LuaCL.Metatable.Platform";
static const char LUACL_PLATFORM_TOSTRING[] = "LuaCL_Platform";

template <>
struct luacl_object_constants <cl_platform_id> {
	static const char * REGISTRY() {
		return LUACL_PLATFORM_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_PLATFORM_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_PLATFORM_TOSTRING;
	}
};

struct luacl_platform {

	typedef luacl_object<cl_platform_id> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_PLATFORM_METATABLE);	/* new metatable */
		lua_newtable(L);								/* mt = {} */
		lua_pushcfunction(L, GetInfo);
		lua_setfield(L, -2, "GetInfo");
		lua_pushcfunction(L, luacl_device::Get);
		lua_setfield(L, -2, "GetDevices");
		lua_pushcfunction(L, luacl_context::Create);
		lua_setfield(L, -2, "CreateContext");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");

		traits::CreateRegistry(L);

		lua_pushcfunction(L, Get);
		lua_setfield(L, LUA_GLOBALSINDEX, "GetPlatform");
	}

	static int PushInfo(lua_State *L, cl_platform_id platform, cl_platform_info param, std::string key) {
		/* Check size of value */
		size_t size = 0;
		cl_int err = clGetPlatformInfo(platform, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting size of info.", NULL);

		/* Request platform parameter */
		char * value = (char *)malloc(sizeof(char) * size);
		CheckAllocError(L, value);
		err = clGetPlatformInfo(platform, param, size, value, NULL);
		CheckCLError(L, err, "Failed requesting platform info.", value);

		/* Push key and value to table */
		std::string value_s(value, size);	/* Construct string with max length for safety constraint */
		free(value);
		lua_pushstring(L, key.c_str());
		lua_pushstring(L, value_s.c_str());
		lua_settable(L, -3);
		return 0;
	}

	static int GetInfo(lua_State *L) {
		cl_platform_id platform = traits::CheckObject(L);
		lua_newtable(L);	/* Create a table */
		PushInfo(L, platform, CL_PLATFORM_PROFILE, "profile");
		PushInfo(L, platform, CL_PLATFORM_VERSION, "version");
		PushInfo(L, platform, CL_PLATFORM_NAME, "name");
		PushInfo(L, platform, CL_PLATFORM_VENDOR, "vendor");
		PushInfo(L, platform, CL_PLATFORM_EXTENSIONS, "extensions");
		return 1;
	}

	static int Get(lua_State *L) {
		cl_uint numPlatforms = 0;
		cl_int err = clGetPlatformIDs(0, NULL, &numPlatforms);
		CheckCLError(L, err, "Failed requesting number of platforms.", NULL);

		cl_platform_id * platforms = static_cast<cl_platform_id *>(malloc(sizeof(cl_platform_id) * numPlatforms));
		CheckAllocError(L, platforms);
		err = clGetPlatformIDs(numPlatforms, platforms, NULL);
		CheckCLError(L, err, "Failed requesting platform list.", platforms);

		for (cl_uint index = 0; index < numPlatforms; index++) {
			traits::Wrap(L, platforms[index]);
		}
		return static_cast<int>(numPlatforms);
	}
};

#endif /* __LUACL_PLATFORM_HPP */
