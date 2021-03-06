#ifndef __LUACL_PLATFORM_HPP
#define __LUACL_PLATFORM_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"

static const char LUACL_PLATFORM_REGISTRY[] = "LuaCL_Platform_Registry";
static const char LUACL_PLATFORM_METATABLE[] = "LuaCL_Platform";
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
        traits::CreateMetatable(L);
        traits::RegisterFunction(L, GetInfo, "GetInfo");
        traits::RegisterFunction(L, luacl_device::Get, "GetDevices");
        traits::RegisterFunction(L, luacl_context::Create, "CreateContext");
        lua_setfield(L, -2, "__index");

        traits::CreateRegistry(L);

        traits::RegisterFunction(L, Get, "GetPlatform", LUA_GLOBALSINDEX);
    }

    static int PushInfo(lua_State *L, cl_platform_id platform, cl_platform_info param, std::string key) {
        /* Check size of value */
        size_t size = 0;
        cl_int err = clGetPlatformInfo(platform, param, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting size of info: %s.");

        /* Request platform parameter */
        std::vector<char> value(size);
        err = clGetPlatformInfo(platform, param, size, value.data(), NULL);
        CheckCLError(L, err, "Failed requesting platform info: %s.");

        /* Push key and value to table */
        lua_pushstring(L, key.c_str());
        lua_pushstring(L, std::string(value.data(), size).c_str());
        lua_settable(L, -3);
        return 0;
    }

    static int GetInfo(lua_State *L) {
        cl_platform_id platform = *traits::CheckObject(L, 1);
        lua_newtable(L);    /* Create a table */
        PushInfo(L, platform, CL_PLATFORM_PROFILE, "PROFILE");
        PushInfo(L, platform, CL_PLATFORM_VERSION, "VERSION");
        PushInfo(L, platform, CL_PLATFORM_NAME, "NAME");
        PushInfo(L, platform, CL_PLATFORM_VENDOR, "VENDOR");
        PushInfo(L, platform, CL_PLATFORM_EXTENSIONS, "EXTENSIONS");
        return 1;
    }

    static int Get(lua_State *L) {
        cl_uint numPlatforms = 0;
        cl_int err = clGetPlatformIDs(0, NULL, &numPlatforms);
        CheckCLError(L, err, "Failed requesting number of platforms: %s.");

        std::vector<cl_platform_id> platforms(numPlatforms);
        err = clGetPlatformIDs(numPlatforms, platforms.data(), NULL);
        CheckCLError(L, err, "Failed requesting platform list: %s.");

        for (cl_uint index = 0; index < numPlatforms; index++) {
            traits::Wrap(L, platforms[index]);
        }
        return static_cast<int>(numPlatforms);
    }
};

#endif /* __LUACL_PLATFORM_HPP */
