#include "LuaCL.h"

#include "luacl_platform.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"

LUA_API int panic(lua_State *L) {
	printf("Panic error:\n");
	const char * msg = lua_tostring(L, 1);
	printf("%s\n", msg);
	system("pause");
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

	luacl_platform::Init(L);
	luacl_device::Init(L);
	luacl_context::Init(L);

	int error = luaL_dofile(L, "test.lua");
	if (error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	system("pause");
	return 0;
}
