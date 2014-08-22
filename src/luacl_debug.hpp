#ifndef __LUACL_DEBUG_HPP
#define __LUACL_DEBUG_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_error.hpp"

struct luacl_debug {
    typedef luacl_object<void *> traits;

    static int Init(lua_State *L) {
        traits::RegisterFunction(L, RegisterDebugCallback, "RegisterDebugCallback", LUA_GLOBALSINDEX);
        traits::RegisterFunction(L, GetRegistry, "GetRegistry", LUA_GLOBALSINDEX);
        traits::RegisterFunction(L, GetOpenCLVersion, "GetOpenCLVersion", LUA_GLOBALSINDEX);
        return 0;
    }

    /* Lua function. Registers a function as debug callback function. [-0, +0, m] */
    static int RegisterDebugCallback(lua_State *L) {
        lua_pushvalue(L, 1);
        lua_setfield(L, LUA_REGISTRYINDEX, LUACL_DEBUG_CALLBACK_FUNC);
        return 0;
    }

    static int GetRegistry(lua_State *L) {
#if _DEBUG
        lua_pushvalue(L, LUA_REGISTRYINDEX);
        return 1;
#else
        return 0;
#endif
    }

    static int GetOpenCLVersion(lua_State *L) {
#if defined(CL_VERSION_2_0)
        lua_pushstring(L, "2.0");
#elif defined(CL_VERSION_1_2)
        lua_pushstring(L, "1.2");
#elif defined(CL_VERSION_1_1)
        lua_pushstring(L, "1.1");
#elif defined(CL_VERSION_1_0)
        lua_pushstring(L, "1.0");
#endif
        return 1;
    }
};

#endif /* __LUACL_DEBUG_HPP */
