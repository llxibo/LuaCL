#include "LuaCL.h"

#include "luacl_api.h"
#include "luacl_info.h"

int main(int argc, char **argv) {
	lua_State *L = luaL_newstate();
	if (NULL == L) {
		return 1;
	}

	luaL_openlibs(L);
	
	luaopen_LuaCL(L);

	int error = luaL_dofile(L, "test.lua");
	if (error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	system("pause");
	return 0;
}
