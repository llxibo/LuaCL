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
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");

		traits::CreateRegistry(L);

		lua_pushcfunction(L, WaitForEvents);
		lua_setfield(L, LUA_GLOBALSINDEX, "WaitForEvents");
	}

	static int WaitForEvents(lua_State *L) {
		std::vector<cl_event> events;
		if (lua_istable(L, 1)) {
			lua_pushnil(L);
			while (lua_next(L, 1)) {
				cl_event e = CheckObject(L, -1);
				lua_pop(L, 1);
				events.push_back(e);
			}
		}
		else {
			cl_uint index = 1;
			while (lua_isuserdata(L, index)) {
				cl_event e = CheckObject(L, index++);
				events.push_back(e);
			}
		}
		if (events.size() == 0) {
			return luaL_error(L, "Bad arguments: expecting LuaCL_Event objects or table of LuaCL_Event objects.");
		}
		cl_int err = clWaitForEvents(events.size(), events.data());
		CheckCLError(L, err, "Failed requesting wait event: %d.");
		return 0;
	}

	static int Wrap(lua_State *L, cl_event event) {
		return traits::Wrap(L, event);
	}

	static cl_event CheckObject(lua_State *L, int index) {
		return traits::CheckObject(L, index);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_EVENT_HPP */
