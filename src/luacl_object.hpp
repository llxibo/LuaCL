#ifndef __LUACL_OBJECT_HPP
#define __LUACL_OBJECT_HPP

#include "LuaCL.h"
#include "luacl_debug.hpp"
#include "luacl_error.hpp"
#include <vector>
#include <string>
#include <assert.h>

/* template trait class holding constants */
template <class cl_object_const_type>
struct luacl_object_constants {};

/* Template class for all kinds of OpenCL objects */
template <typename cl_object>
struct luacl_object {
    typedef luacl_object_constants<cl_object> traits;

    /* Wrap an object as lua userdata, and push it onto stack.
       It relies on a registry table, which is located in LUA_REGISTRY. [-0, +1, v] */
    static int Wrap(lua_State *L, cl_object object) {
        if (object == NULL) {
            return luaL_error(L, "Attempt to wrap a null object.");
        }
        l_debug(L, "Wrapping %s %p", typeid(cl_object).name(), object);
        lua_getfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());                             /* reg */
        assert(lua_istable(L, -1));

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

    /* Lua function, release an object by calling object-specific release function. [-1, 0, v] */
    static int Release(lua_State *L) {
        cl_object object = CheckObject(L);
        l_debug(L, "__gc Releasing %s: %p", traits::TOSTRING(), object);
        LUACL_TRYCALL(
            cl_int err = traits::Release(object);
            CheckCLError(L, err, "Failed releasing LuaCL object: %s.");
        );
        return 0;
    }
    
    /* Create object metatable, and register __tostring to it. [-0, +1, m] */
    static void CreateMetatable(lua_State *L) {
        luaL_newmetatable(L, traits::METATABLE());                  /* mt */
        lua_pushcfunction(L, ToString);                             /* tostring, mt */
        lua_setfield(L, -2, "__tostring");                          /* mt */
        lua_newtable(L);
    }
    
    /* Create userdata registry, setting its __mode as weak. [-0, +0, m] */
    static void CreateRegistry(lua_State *L) {
        lua_newtable(L);                                            /* reg */
        lua_newtable(L);                                            /* mt, reg */
        lua_pushstring(L, "kv");                                    /* "kv", mt, reg */
        lua_setfield(L, -2, "__mode");                              /* mt(__mode="kv"), reg */
        lua_setmetatable(L, -2);                                    /* reg(mt) */
        lua_setfield(L, LUA_REGISTRYINDEX, traits::REGISTRY());     /* (empty stack) */
    }

    /* Create registry for callback functions. [-0, +0, m] */
    static void CreateCallbackRegistry(lua_State *L) {
        lua_newtable(L);                                            /* reg */
        lua_newtable(L);                                            /* mt, reg */
        lua_pushstring(L, "k");                                     /* "k*, mt, reg */
        lua_setfield(L, -2, "__mode");                              /* mt(__mode="k"), reg */
        lua_setmetatable(L, -2);                                    /* reg(mt) */
        lua_setfield(L, LUA_REGISTRYINDEX, traits::CALLBACK());     /* (empty stack) */
    }

    /* Create a new lua thread, and wrap the callback function into its stack. [-0, +1, m] */
    static lua_State *CreateCallbackThread(lua_State *L, int index) {
        if (!lua_isfunction(L, index)) {
            lua_pushnil(L);                                         /* nil */
            return NULL;
        }
        /* Make a copy of callback func in the stack before touching the stack, or the index of func may be altered */
        lua_pushvalue(L, index);                                    /* func */
        lua_State *thread = lua_newthread(L);                       /* thread, func */
        lua_insert(L, -2);                                          /* func, thread */
        printf("CallbackFuncType: %s\n", luaL_typename(L, -1));
        /* Move top 1 item(func) into thread stack */
        lua_xmove(L, thread, 1);                                    /* thread */
        return thread;
    }

    /* Registers the key on stack -1 and value on -2 into callback registry. Pops the key and value.
       Notice that the key-value sequence in stack is NOT the same with lua_settable. [-2, +0, v] */
    static void RegisterCallback(lua_State *L) {
        lua_getfield(L, LUA_REGISTRYINDEX, traits::CALLBACK());     /* reg, key, value */
        if (lua_isnil(L, -1)) {
            luaL_error(L, "Failed registering callback: registry not found.");
        }
        lua_insert(L, -3);                                          /* key, value, reg */
        lua_insert(L, -2);                                          /* value, key, reg */
        lua_settable(L, -3);                                        /* reg */
        lua_pop(L, 1);                                              /* (empty) */
    }

    /* Calls a callback function, leaving the function in the stack for next call. [-nargs, +0, e] */
    static void DoCallback(lua_State *L, int nargs) {
        lua_pushvalue(L, -nargs - 1);                               /* func, (args), func */
        lua_insert(L, -nargs - 1);                                  /* (args), func, func */
        lua_call(L, nargs, 0);                                      /* func */
    }

    /* Register a CFunction to a table. [-0, +0, -] */
    static void RegisterFunction(lua_State *L, lua_CFunction func, const char *name, int index = -2) {
        lua_pushcfunction(L, func);
        lua_setfield(L, index, name);
    }
    
    /* Register object release function to be called upon __gc. [-0, +0, -] */
    static void RegisterRelease(lua_State *L) {
        lua_pushcfunction(L, Release);
        lua_setfield(L, -2, "__gc");
    }

    /* Check and return OpenCL object wrapped in a userdata.
       This function always return a non-NULL value, or it will throw a Lua error. [-0, +0, v] */
    static cl_object CheckObject(lua_State *L, int index = 1) {
        cl_object *p = static_cast<cl_object *>(luaL_checkudata(L, index, traits::METATABLE()));
        if (LUACL_UNLIKELY(p == NULL)) {
            luaL_error(L, "Failed checking object from userdata."); /* This function never returns */
            return NULL;                                            /* return to make compiler happy */
        }
        return *p;
    }

    /* Lua function, returns lua string for an object. [0, +1, m] */
    static int ToString(lua_State *L) {
        lua_pushfstring(L, "%s: %p", traits::TOSTRING(), CheckObject(L));
        return 1;
    }

    /* Check a stack index for a table of objects, returns vector of objects. [-0, +0, v] */
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
