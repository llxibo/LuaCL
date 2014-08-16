#ifndef __LUACL_EVENT_HPP
#define __LUACL_EVENT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include <vector>

static const char LUACL_EVENT_REGISTRY[] = "LuaCL_Event_Registry";
static const char LUACL_EVENT_METATABLE[] = "LuaCL_Event";
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
    static cl_uint Release(cl_event event) {
        return clReleaseEvent(event);
    }
};

struct luacl_event {
    typedef luacl_object<cl_event> traits;

    static void Init(lua_State *L) {
        traits::CreateMetatable(L);
        lua_pop(L, 1);      /* CreateMetatable will push __index table into stack, and we don't need it now */
        traits::RegisterRelease(L);
        traits::CreateRegistry(L);
        traits::RegisterFunction(L, WaitForEvents, "WaitForEvents", LUA_GLOBALSINDEX);
    }

    static int WaitForEvents(lua_State *L) {
        std::vector<cl_event> events = traits::CheckObjectTable(L, 1);
        if (LUACL_UNLIKELY(events.size() == 0)) {
            return luaL_error(L, "Bad arguments: expecting LuaCL_Event objects or table of LuaCL_Event objects.");
        }
        cl_int err = clWaitForEvents(static_cast<cl_uint>(events.size()), events.data());
        CheckCLError(L, err, "Failed requesting wait event: %s.");
        return 0;
    }
};

#endif /* __LUACL_EVENT_HPP */
