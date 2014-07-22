#ifndef __LUACI_API_H
#define __LUACI_API_H

#include "LuaCL.h"
#include <stdlib.h>
#include <string>
#include "luacl_info.h"

static const char LUACL_UDATA_CONTEXT[] = "LuaCL.context";
static const char LUACL_GLOBAL_NAME[] = "LuaCL";
static const char LUACL_KERNEL_DEFAULT_NAME[] = "<anonymous kernel>";
static const char LUACL_ERR_MALLOC[] = "Insufficient memory";

//LUALIB_API int luaopen_LuaCL(lua_State *L);

/* template trait class holding constants */
template <class cl_object_const_type>
struct luacl_object_template_constants {};

/* Template class for all kinds of OpenCL objects */
template <typename cl_object>
struct luacl_object_template {
	typedef luacl_object_template_constants<cl_object> traits;

	static int Wrap(lua_State *L, cl_object object) {
		lua_getfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());
		/* Now the top of stack is registry table */
		lua_pushlightuserdata(L, static_cast<void *>(object));								/* p, reg */
		lua_gettable(L, -2);	/* Query the registry table with value of pointer */
		void *p = lua_touserdata(L, -1);													/* udata/nil, reg */
		if (p == NULL) {
			//printf("Wrap: Creating cache entry\n");
			cl_object *p = static_cast<cl_object *>(lua_newuserdata(L, sizeof(cl_object)));	/* udata, nil, reg */
			*p = object;
			luaL_getmetatable(L, traits::METATABLE());										/* mt, udata, nil, reg */
			lua_setmetatable(L, -2);														/* udata(mt), nil, reg */
			lua_pushlightuserdata(L, static_cast<void *>(object));							/* p, udata(mt), nil, reg */
			lua_pushvalue(L, -2);															/* udata(mt), p, udata(mt), nil, reg */
			lua_settable(L, -5);															/* udata(mt), nil, reg */
			lua_remove(L, -3);
			lua_remove(L, -2);
		}
		else {
			luaL_checkudata(L, -1, traits::METATABLE());									/* udata, reg */
			lua_remove(L, -2);																/* udata */
		}
		return 1;
	}

	static cl_object CheckObject(lua_State *L) {
		cl_object *p = static_cast<cl_object *>(luaL_checkudata(L, 1, traits::METATABLE()));
		if (p == NULL) {
			luaL_error(L, "Failed resolving object from userdata.");	/* This function never returns */
		}
		return *p;
	}

	static int ToString(lua_State *L) {
		lua_pushfstring(L, "%s: %p", traits::TOSTRING(), CheckObject(L));
		return 1;
	}
};

void CheckAllocError(lua_State *L, void *p, const char * msg = LUACL_ERR_MALLOC) {
	if (p == NULL) {
		luaL_error(L, msg);
	}
}

void CheckCLError(lua_State *L, cl_uint err, const char * msg, void *p = NULL) {
	if (err != CL_SUCCESS) {
		free(p);
		luaL_error(L, msg);
	}
}

#endif
