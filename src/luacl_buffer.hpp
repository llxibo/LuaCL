#ifndef __LUACL_BUFFER_HPP
#define __LUACL_BUFFER_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_MEM_REGISTRY[] = "LuaCL.Registry.Mem";
static const char LUACL_MEM_METATABLE[] = "LuaCL.Metatable.Mem";
static const char LUACL_MEM_TOSTRING[] = "LuaCL_Mem";

template <>
struct luacl_object_constants<cl_mem> {
	static const char * REGISTRY() {
		return LUACL_MEM_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_MEM_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_MEM_TOSTRING;
	}
};

struct luacl_mem {
	typedef luacl_object<cl_mem> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_MEM_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, GetDevices);
		// lua_setfield(L, -2, "GetDevices");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
	}

	static int Create(lua_State *L) {

		return 1;
	}

	static int Release(lua_State *L) {
		cl_mem mem = traits::CheckObject(L);
		//printf("__gc Releasing mem %p\n", mem);

		LUACL_TRYCALL(clReleaseMemObject(mem));
		return 0;
	}
};

#endif /* __LUACL_BUFFER_HPP */
