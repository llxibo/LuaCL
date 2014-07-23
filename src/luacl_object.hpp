#ifndef __LUACL_OBJECT_HPP
#define __LUACL_OBJECT_HPP

#include "LuaCL.h"

static const char LUACL_ERR_MALLOC[] = "Insufficient memory";

/* template trait class holding constants */
template <class cl_object_const_type>
struct luacl_object_constants {};

/* Template class for all kinds of OpenCL objects */
template <typename cl_object>
struct luacl_object {
	typedef luacl_object_constants<cl_object> traits;

	static int Wrap(lua_State *L, cl_object object) {
        printf("Wrapping object %p\n", object);
		lua_getfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());
		/* Now the top of stack is registry table */
		lua_pushlightuserdata(L, static_cast<void *>(object));								/* p, reg */
		lua_gettable(L, -2);	/* Query the registry table with value of pointer */
		void *p = lua_touserdata(L, -1);													/* udata/nil, reg */
		if (p == NULL) {
			printf("Wrap: Creating cache entry\n");
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

	static void CreateRegistry(lua_State *L) {
		/* Create device userdata registry */
		lua_newtable(L);											/* reg */
		lua_newtable(L);											/* mt, reg */
		lua_pushstring(L, "kv");									/* "kv", mt, reg */
		lua_setfield(L, -2, "__mode");								/* mt(__mode="kv), reg */
		lua_setmetatable(L, -2);									/* reg(mt) */
		lua_setfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());		/* (empty stack) */
	}

	/* Check and return OpenCL object wrapped in a userdata.
	   This function always return a non-NULL value, or it will throw a Lua error. */
	static cl_object CheckObject(lua_State *L, cl_object *resource = NULL) {
		cl_object *p = static_cast<cl_object *>(luaL_checkudata(L, 1, traits::METATABLE()));
		if (p == NULL) {
			free(resource);
			luaL_error(L, "Failed resolving object from userdata.");	/* This function never returns */
			return NULL;
		}
		return *p;
	}

	static int ToString(lua_State *L) {
		lua_pushfstring(L, "%s: %p", traits::TOSTRING(), CheckObject(L));
		return 1;
	}
};

#define CHECK_ALLOC_ERROR_MACRO 1
#if (CHECK_ALLOC_ERROR_MACRO)
    #define CheckAllocError(L, p) {if (p == NULL) {luaL_error(L, LUACL_ERR_MALLOC); return 0;}};
#else
void CheckAllocError(lua_State *L, void *p, const char * msg = LUACL_ERR_MALLOC) {
	if (p == NULL) {
		luaL_error(L, msg);
	}
}
#endif

void CheckCLError(lua_State *L, cl_uint err, const char * msg, void *p = NULL) {
	if (err != CL_SUCCESS) {
		free(p);
		luaL_error(L, msg, err);
	}
}

#endif /* __LUACL_OBJECT_HPP */
