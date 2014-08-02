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

int main(int argc, char **argv) {
	lua_State *L = luaL_newstate();
	if (NULL == L) {
		return 1;
	}

	luaL_openlibs(L);

	lua_atpanic(L, panic);

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

	int error = luaL_dofile(L, "test.lua");
	if (error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	lua_close(L);

#if defined(_LUACL_PAUSE_SYSTEM)
	system("pause");
#endif
	return 0;
}
