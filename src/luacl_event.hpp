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
        traits::RegisterFunction(L, GetCommandQueue, "GetCommandQueue");
        traits::RegisterFunction(L, GetContext, "GetContext");
        lua_setfield(L, -2, "__index");
        traits::RegisterRelease(L);
        traits::CreateRegistry(L);
        traits::RegisterFunction(L, WaitForEvents, "WaitForEvents", LUA_GLOBALSINDEX);
    }
    
    static int GetCommandQueue(lua_State *L) {
        cl_event event = traits::CheckObject(L);
        size_t size = 0;
        cl_command_queue cmdqueue = NULL;
        cl_int err = clGetEventInfo(event, CL_EVENT_COMMAND_QUEUE, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting size of command queue from event: %s.");
        if (LUACL_UNLIKELY(size != sizeof(cl_command_queue))) {
            return luaL_error(L, "Failed requesting command queue from event: value size mismatch.");
        }
        
        err = clGetEventInfo(event, CL_EVENT_COMMAND_QUEUE, size, &cmdqueue, NULL);
        CheckCLError(L, err, "Failed requesting command queue from event: %s.");
        luacl_object<cl_command_queue>::Wrap(L, cmdqueue);
        return 1;
    }
    
    static int GetContext(lua_State *L) {
        cl_event event = traits::CheckObject(L);
        size_t size = 0;
        cl_context context = NULL;
        cl_int err = clGetEventInfo(event, CL_EVENT_CONTEXT, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting size of context from event: %s.");
        if (LUACL_UNLIKELY(size != sizeof(cl_context))) {
            return luaL_error(L, "Failed requesting context from event: value size mismatch.");
        }
        
        err = clGetEventInfo(event, CL_EVENT_CONTEXT, size, &context, NULL);
        CheckCLError(L, err, "Failed requesting context from event: %s.");
        luacl_object<cl_context>::Wrap(L, context);
        return 1;
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

    static int RegisterCallback(lua_State *L) {
        cl_event event = traits::CheckObject(L);
        cl_int cmdType = luaL_checknumber(L, 2);
        lua_State *thread = traits::CreateCallbackThread(L, 3);
        if (thread == NULL) {
            return 0;
        }
        cl_int err = clSetEventCallback(event, cmdType, Callback, thread);
        CheckCLError(L, err, "Failed registering event callback: %s.");
        lua_pushvalue(L, 1);
        traits::RegisterCallback(L);
        return 0;
    }

    static void CL_CALLBACK Callback(cl_event event, cl_int event_command_exec_status, void *user_data) {
        lua_State *L = static_cast<lua_State *>(user_data);
        traits::Wrap(L, event);
        lua_pushnumber(L, event_command_exec_status);
        lua_call(L, 2, 0);
    }
};

#endif /* __LUACL_EVENT_HPP */
