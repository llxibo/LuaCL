#ifndef __LUACL_DEBUG_HPP
#define __LUACL_DEBUG_HPP

#include "LuaCL.h"

const char LUACL_DEBUG_CALLBACK_FUNC[] = "LuaCL_Debug_Callback_Func";

static int RegisterDebugCallback(lua_State *L) {
	lua_pushvalue(L, 1);
	lua_setfield(L, LUA_GLOBALSINDEX, LUACL_DEBUG_CALLBACK_FUNC);
	return 0;
}

void l_debug(lua_State *L, const char * message, ...) {
	lua_getfield(L, LUA_GLOBALSINDEX, LUACL_DEBUG_CALLBACK_FUNC);
	va_list argp;
	va_start(argp, message);
	lua_pushvfstring(L, message, argp);
	va_end(argp);
	lua_call(L, 1, 0);
}

#endif /* __LUACL_DEBUG_HPP */
