#ifndef __LUACL_OBJECT_HPP
#define __LUACL_OBJECT_HPP

#include "LuaCL.h"
#include <vector>
#include <string>
#include <assert.h>

static const char LUACL_ERR_MALLOC[] = "Insufficient memory";

/* Macro instead of inline function to explicitly return from caller function.
 * The inline version will longjmp away from caller, leaving its following pointer usage safe.
 * However, compilers and analyzers may not recognize this behavior and therefore complain about it.
 * An exception could also jump out of caller function, but it comes with extra overhead, and could be hard to handle.
 * So we got this work-around, to shut compiler from complaining and keep away from exception implementation.
 * The macro is assumed to be used in a lua_CFunction, which returns int. */
#define CHECK_ALLOC_ERROR_MACRO 1
#if (CHECK_ALLOC_ERROR_MACRO)
#define CheckAllocError(L, p) {             \
    if (LUACL_UNLIKELY(p == NULL)) {        \
        luaL_error(L, LUACL_ERR_MALLOC);    \
        return 0;                           \
    }                                       \
};
#else
void CheckAllocError(lua_State *L, void *p, const char * msg = LUACL_ERR_MALLOC) {
    if (LUACL_UNLIKELY(p == NULL)) {
        luaL_error(L, msg);
    }
}
#endif

void CheckCLError(lua_State *L, cl_uint err, const char * msg) {
    if (LUACL_UNLIKELY(err != CL_SUCCESS)) {
        luaL_error(L, msg, err);
    }
}

/* template trait class holding constants */
template <class cl_object_const_type>
struct luacl_object_constants {};

/* Template class for all kinds of OpenCL objects */
template <typename cl_object>
struct luacl_object {
    typedef luacl_object_constants<cl_object> traits;

    static int Wrap(lua_State *L, cl_object object) {
        l_debug(L, "Wrapping %s %p", typeid(cl_object).name(), object);
        lua_getfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());
        assert(lua_istable(L, -1));
        /* Now the top of stack is registry table */
        lua_pushlightuserdata(L, static_cast<void *>(object));                              /* p, reg */
        lua_gettable(L, -2);    /* Query the registry table with value of pointer */
        void *p = lua_touserdata(L, -1);                                                    /* udata/nil, reg */
        if (p == NULL) {
            // l_debug(L, "Wrap: Creating cache entry");
            cl_object *p = static_cast<cl_object *>(lua_newuserdata(L, sizeof(cl_object))); /* udata, nil, reg */
            *p = object;
            luaL_getmetatable(L, traits::METATABLE());                                      /* mt, udata, nil, reg */
            lua_setmetatable(L, -2);                                                        /* udata(mt), nil, reg */
            lua_pushlightuserdata(L, static_cast<void *>(object));                          /* p, udata(mt), nil, reg */
            lua_pushvalue(L, -2);                                                           /* udata(mt), p, udata(mt), nil, reg */
            lua_settable(L, -5);                                                            /* udata(mt), nil, reg */
            lua_remove(L, -3);
            lua_remove(L, -2);
        }
        else {
            luaL_checkudata(L, -1, traits::METATABLE());                                    /* udata, reg */
            lua_remove(L, -2);                                                              /* udata */
        }
        return 1;
    }

    static int Release(lua_State *L) {
        cl_object object = CheckObject(L);
        l_debug(L, "__gc Releasing %s: %p", traits::TOSTRING(), object);
        LUACL_TRYCALL(
            cl_int err = traits::Release(object);
            CheckCLError(L, err, "Failed releasing LuaCL object: %d.");
        );
        return 0;
    }
    
    static void CreateMetatable(lua_State *L) {
        luaL_newmetatable(L, traits::METATABLE());
        lua_pushcfunction(L, ToString);
        lua_setfield(L, -2, "__tostring");
        lua_newtable(L);
    }
    
    static void RegisterFunction(lua_State *L, lua_CFunction func, const char *name, int index = -2) {
        lua_pushcfunction(L, func);
        lua_setfield(L, index, name);
    }
    
    static void RegisterRelease(lua_State *L) {
        lua_pushcfunction(L, Release);
        lua_setfield(L, -2, "__gc");
    }
    
    static void CreateRegistry(lua_State *L) {
        /* Create device userdata registry */
        lua_newtable(L);                                            /* reg */
        lua_newtable(L);                                            /* mt, reg */
        lua_pushstring(L, "kv");                                    /* "kv", mt, reg */
        lua_setfield(L, -2, "__mode");                              /* mt(__mode="kv"), reg */
        lua_setmetatable(L, -2);                                    /* reg(mt) */
        lua_setfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());     /* (empty stack) */
    }

    /* Check and return OpenCL object wrapped in a userdata.
       This function always return a non-NULL value, or it will throw a Lua error. */
    static cl_object CheckObject(lua_State *L, int index = 1) {
        cl_object *p = static_cast<cl_object *>(luaL_checkudata(L, index, traits::METATABLE()));
        if (LUACL_UNLIKELY(p == NULL)) {
            // free(resource);
            luaL_error(L, "Failed resolving object from userdata.");    /* This function never returns */
            return NULL;
        }
        return *p;
    }

    static int ToString(lua_State *L) {
        lua_pushfstring(L, "%s: %p", traits::TOSTRING(), CheckObject(L));
        return 1;
    }

    static std::vector<cl_object> CheckObjectTable(lua_State *L, int index, bool allowNil = false) {
        if (allowNil && lua_isnoneornil(L, index)) {
            return std::vector<cl_object>();
        }
        if (LUACL_UNLIKELY(!lua_istable(L, index))) {
            luaL_error(L, "Bad argument #%d, table of %s expected, got %s.", index, traits::TOSTRING(), luaL_typename(L, index));
            return std::vector<cl_object>();
        }
        
        std::vector<cl_object> objects;
        size_t size = lua_objlen(L, index);
        for (unsigned int i = 0; i < size; i++) {
            lua_rawgeti(L, index, i + 1);
            cl_object object = CheckObject(L, -1);
            l_debug(L, "CheckObject %s %p", typeid(object).name(), object);
            objects.push_back(object);
            lua_pop(L, 1);
        }
        return objects;
    }

    static std::vector<cl_object> CheckNumberTable(lua_State *L, int index, bool allowNil = false) {
        if (allowNil && lua_isnoneornil(L, index)) {
            return std::vector<cl_object>();
        }
        if (LUACL_UNLIKELY(!lua_istable(L, index))) {
            luaL_error(L, "Bad argument #%d, table of number expected, got %s.", index, luaL_typename(L, index));
            return std::vector<cl_object>();
        }
        
        std::vector<cl_object> numbers;
        size_t size = lua_objlen(L, index);     /* Must be a table now */
        for (unsigned int i = 0; i < size; i++) {
            lua_rawgeti(L, index, i + 1);
            cl_object num = static_cast<cl_object>(luaL_checknumber(L, -1));
            numbers.push_back(num);
            lua_pop(L, 1);
        }
        return numbers;
    }

    static std::vector<std::string> CheckStringTable(lua_State *L, int index) {
        if (LUACL_UNLIKELY(!lua_istable(L, index))) {
            luaL_error(L, "Bad argument #%d, table of string expected, got %s.", index, luaL_typename(L, index));
            return std::vector<std::string>();
        }
        
        std::vector<std::string> strings;
        lua_pushnil(L);
        while (lua_next(L, index)) {
            size_t len = 0;
            const char * str = luaL_checklstring(L, -1, &len);
            l_debug(L, "CheckString key %d: %p", static_cast<int>(lua_tonumber(L, -2)), reinterpret_cast<void *>(len));
            strings.push_back(std::string(str, len));
            lua_pop(L, 1);
        }
        return strings;
    }
};

#endif /* __LUACL_OBJECT_HPP */
