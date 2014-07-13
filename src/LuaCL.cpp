#include "LuaCL.h"

#include "luacl_api.h"
#include "luacl_info.h"

int main(int argc, char **argv) {
	lua_State *L = luaL_newstate();
	if (NULL == L) {
		return 1;
	}

	luaL_openlibs(L);
	
	lua_pushcfunction(L, luacl_GetPlatformInfo);
	lua_setglobal(L, "GetPlatformInfo");
	lua_pushcfunction(L, luacl_GetNumPlatforms);
	lua_setglobal(L, "GetNumPlatforms");
	lua_pushcfunction(L, luacl_GetNumDevices);
	lua_setglobal(L, "GetNumDevices");

	int error = luaL_dofile(L, "test.lua");
	if (error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	system("pause");
	return 0;
}
