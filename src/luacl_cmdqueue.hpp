#ifndef __LUACL_CMDQUEUE_HPP
#define __LUACL_CMDQUEUE_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_CMDQUEUE_REGISTRY[] = "LuaCL.Registry.CmdQueue";
static const char LUACL_CMDQUEUE_METATABLE[] = "LuaCL.Metatable.CmdQueue";
static const char LUACL_CMDQUEUE_TOSTRING[] = "LuaCL_CmdQueue";

template <>
struct luacl_object_constants<cl_command_queue> {
	static const char * REGISTRY() {
		return LUACL_CMDQUEUE_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_CMDQUEUE_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_CMDQUEUE_TOSTRING;
	}
};

struct luacl_command_queue {
    typedef luacl_object<cl_command_queue> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_CMDQUEUE_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, GetDevices);
		// lua_setfield(L, -2, "GetDevices");
		// lua_pushcfunction(L, GetPlatform);
		// lua_setfield(L, -2, "GetPlatform");
		// lua_pushcfunction(L, luacl_program::Create);
		// lua_setfield(L, -2, "CreateProgram");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
		// lua_newtable(L);
		// lua_setfield(L, LUA_REGISTRYINDEX, LUACL_CMDQUEUE_REGISTRY_CALLBACK);
	}

    static int Create(lua_State *L) {
    	return 1;
    }

	static int Release(lua_State *L) {
		cl_command_queue cmdqueue = CheckObject(L);
		//printf("__gc Releasing cmdqueue %p\n", cmdqueue);

		LUACL_TRYCALL(clReleaseContext(cmdqueue));
		return 0;
	}

	static int Wrap(lua_State *L, cl_command_queue cmdqueue) {
		return traits::Wrap(L, cmdqueue);
	}

	static cl_command_queue CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_CMDQUEUE_HPP */
