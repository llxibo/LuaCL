#ifndef __LUACL_DEBUG_HPP
#define __LUACL_DEBUG_HPP

#include "LuaCL.h"

const char LUACL_DEBUG_CALLBACK_FUNC[] = "LuaCL_Debug_Callback_Func";

#if defined(_DEBUG)
    inline void l_debug(lua_State *L, const char * message, ...) {
        lua_getfield(L, LUA_REGISTRYINDEX, LUACL_DEBUG_CALLBACK_FUNC);
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return;
        }
        va_list argp;
        va_start(argp, message);
        lua_pushvfstring(L, message, argp);
        va_end(argp);
        lua_call(L, 1, 0);
    }
#else
    void l_debug(lua_State *L, const char * message, ...) {
        LUACL_UNUSED(L);
        LUACL_UNUSED(message);
    }
#endif /* _DEBUG */

struct luacl_debug {
    static int Init(lua_State *L) {
        lua_pushcfunction(L, RegisterDebugCallback);
        lua_setfield(L, LUA_GLOBALSINDEX, "RegisterDebugCallback");
        lua_pushcfunction(L, GetRegistry);
        lua_setfield(L, LUA_GLOBALSINDEX, "GetRegistry");
        lua_pushcfunction(L, GetOpenCLVersion);
        lua_setfield(L, LUA_GLOBALSINDEX, "GetOpenCLVersion");
        return 0;
    }

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
