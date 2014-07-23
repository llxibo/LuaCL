#ifndef __LUACL_CONTEXT_HPP
#define __LUACL_CONTEXT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_CONTEXT_REGISTRY[] = "LuaCL.Registry.Context";
static const char LUACL_CONTEXT_METATABLE[] = "LuaCL.Metatable.Context";
static const char LUACL_CONTEXT_TOSTRING[] = "LuaCL_Context";

template <>
struct luacl_object_template_constants<cl_context> {
	static const char * REGISTRY() {
		return LUACL_CONTEXT_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_CONTEXT_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_CONTEXT_TOSTRING;
	}
};

struct luacl_context {
    typedef luacl_object_template<cl_device_id> traits;
    
	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_CONTEXT_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, Create);
		// lua_setfield(L, -2, "Create");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		traits::CreateRegistry(L);
	}
    
    static int Create(lua_State *L) {
        cl_platform_id = luacl_object_template<cl_platform_id>::CheckObject(L, 1);
        cl_device_id device = luacl_object_template<cl_device_id>::CheckObject(L, 2);
        
    }
};

#endif /* __LUACL_CONTEXT_HPP */
