#include "LuaCL.h"

LUA_API int panic(lua_State *L) {
	printf("Panic error:\n");
	const char * msg = lua_tostring(L, 1);
	printf("%s\n", msg);
#if defined(_LUACL_PAUSE_SYSTEM)
	system("pause");
#endif
	exit(0);
	return 1;
}

static int Run(lua_State *L) {
    lua_getfield(L, LUA_GLOBALSINDEX, "dofile");
    lua_pushstring(L, "UnitTest.lua");
    lua_call(L, 1, 0);
    return 0;
}

static int ErrorHandler(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "traceback");
    lua_pushvalue(L, -2);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_pop(L, 2);
    return 0;
}

int main(int argc, char **argv) {
	lua_State *L = luaL_newstate();
	if (NULL == L) {
		return 1;
	}

	luaL_openlibs(L);

	lua_atpanic(L, panic);

    /* Store traceback function to registry, to prevent potential alteration by user */
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    lua_getfield(L, -1, "traceback");
    lua_setfield(L, LUA_REGISTRYINDEX, "traceback");
    lua_pop(L, 1);
    
#if defined(_DEBUG)
    luacl_debug::Init(L);
#endif
	luacl_platform::Init(L);
	luacl_device::Init(L);
	luacl_context::Init(L);
	luacl_program::Init(L);
	luacl_kernel::Init(L);
	luacl_cmdqueue::Init(L);
	luacl_buffer::Init(L);
    luacl_event::Init(L);

    lua_getfield(L, LUA_GLOBALSINDEX, "xpcall");
    lua_pushcfunction(L, Run);
    lua_pushcfunction(L, ErrorHandler);
    lua_call(L, 2, 0);
	lua_close(L);

#if defined(_LUACL_PAUSE_SYSTEM)
	system("pause");
#endif
	return 0;
}
