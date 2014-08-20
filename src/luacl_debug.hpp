#ifndef __LUACL_DEBUG_HPP
#define __LUACL_DEBUG_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

const char LUACL_DEBUG_CALLBACK_FUNC[] = "LuaCL_Debug_Callback_Func";

#if defined(_DEBUG)
    /* Debug function. Calls registered callback function with debug string. [-0, +0, e] */
    inline void l_debug(lua_State *L, const char * message, ...) {
        lua_getfield(L, LUA_REGISTRYINDEX, LUACL_DEBUG_CALLBACK_FUNC);  /* func/nil */
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);      /* Pop nil to keep stack balanced */
            return;
        }
        va_list argp;
        va_start(argp, message);
        lua_pushvfstring(L, message, argp);                             /* str, func */
        va_end(argp);
        lua_call(L, 1, 0);                                              /* (empty stack) */
    }
#else
    void l_debug(lua_State *L, const char * message, ...) {
        LUACL_UNUSED(L);
        LUACL_UNUSED(message);
    }
#endif /* _DEBUG */

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
