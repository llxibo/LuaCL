#ifndef __LUACL_EVENT_HPP
#define __LUACL_EVENT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include <vector>

static const char LUACL_EVENT_REGISTRY[] = "LuaCL.Registry.Event";
static const char LUACL_EVENT_METATABLE[] = "LuaCL.Metatable.Event";
static const char LUACL_EVENT_TOSTRING[] = "LuaCL_Event";

template <>
struct luacl_object_constants<cl_event> {
	static const char * REGISTRY() {
		return LUACL_EVENT_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_EVENT_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_EVENT_TOSTRING;
	}
};

struct luacl_event {
	typedef luacl_object<cl_event> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_EVENT_METATABLE);
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");

		traits::CreateRegistry(L);

		lua_pushcfunction(L, WaitForEvents);
		lua_setfield(L, LUA_GLOBALSINDEX, "WaitForEvents");
	}

	static int WaitForEvents(lua_State *L) {
		std::vector<cl_event> events = traits::CheckObjectTable(L, 1);
		if (events.size() == 0) {
			return luaL_error(L, "Bad arguments: expecting LuaCL_Event objects or table of LuaCL_Event objects.");
		}
		cl_int err = clWaitForEvents(events.size(), events.data());
		CheckCLError(L, err, "Failed requesting wait event: %d.");
		return 0;
	}
};

#endif /* __LUACL_EVENT_HPP */
