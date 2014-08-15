#ifndef __LUACL_CMDQUEUE_HPP
#define __LUACL_CMDQUEUE_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_buffer.hpp"
#include "luacl_event.hpp"

static const char LUACL_CMDQUEUE_REGISTRY[] = "LuaCL_CmdQueue_Registry";
static const char LUACL_CMDQUEUE_METATABLE[] = "LuaCL_CmdQueue";
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
    static cl_int Release(cl_command_queue cmdqueue) {
        return clReleaseCommandQueue(cmdqueue);
    }
};

struct luacl_cmdqueue {
    typedef luacl_object<cl_command_queue> traits;

    static void Init(lua_State *L) {
        traits::CreateMetatable(L);
        traits::RegisterFunction(L, Finish, "Finish");
        traits::RegisterFunction(L, Flush, "Flush");
        traits::RegisterFunction(L, EnqueueReadBuffer, "EnqueueReadBuffer");
        traits::RegisterFunction(L, EnqueueWriteBuffer, "EnqueueWriteBuffer");
        traits::RegisterFunction(L, EnqueueNDRangeKernel, "EnqueueNDRangeKernel");
        lua_setfield(L, -2, "__index");
        traits::RegisterRelease(L);
        traits::CreateRegistry(L);
    }

    static int Create(lua_State *L) {
        cl_context context = luacl_object<cl_context>::CheckObject(L);
        cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
        lua_checkstack(L, 4);
        int outOfOrder = lua_toboolean(L, 3);
        int profiling = lua_toboolean(L, 4);
        
        cl_command_queue_properties prop = outOfOrder ? CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE : 0x0;
        prop |= profiling ? CL_QUEUE_PROFILING_ENABLE : 0x0;

        cl_int err = 0;
        cl_command_queue cmdqueue = clCreateCommandQueue(context, device, prop, &err);
        CheckCLError(L, err, "Failed creating command queue: %d.");
        
        traits::Wrap(L, cmdqueue);
        return 1;
    }

    static int EnqueueNDRangeKernel(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        cl_kernel krnl = luacl_object<cl_kernel>::CheckObject(L, 2);
        std::vector<size_t> localWorkSize = luacl_object<size_t>::CheckNumberTable(L, 3, true); /* Optional arg */
        std::vector<size_t> globalWorkSize = luacl_object<size_t>::CheckNumberTable(L, 4);
        std::vector<size_t> globalWorkOffset = luacl_object<size_t>::CheckNumberTable(L, 5, true);  /* Optional arg */

        /* Work dimension extraction and check */
        cl_uint workDim = static_cast<cl_uint>(globalWorkSize.size());
        if (LUACL_UNLIKELY(workDim == 0)) {
            return luaL_error(L, "Invalid working dimension.");
        }
        /* Dimension of local work size must match. Global work offset could match the dimension or be (optionally) empty */
        if (LUACL_UNLIKELY(
            (!localWorkSize.empty() && workDim != localWorkSize.size()) ||
                (!globalWorkOffset.empty() && globalWorkOffset.size() != workDim))) {
            return luaL_error(L, "Working dimension mismatch.");
        }

        std::vector<cl_event> events = luacl_object<cl_event>::CheckObjectTable(L, 6, true);    /* Event list could be empty */

        const cl_event * eventsData = events.empty() ? NULL : events.data();
        const cl_uint eventSize = static_cast<cl_uint>(events.size());

        cl_event event = NULL;
        cl_int err = clEnqueueNDRangeKernel(
            cmdqueue,
            krnl,
            workDim,
            globalWorkOffset.empty() ? NULL : globalWorkOffset.data(),
            globalWorkSize.data(),
            localWorkSize.empty() ? NULL : localWorkSize.data(),
            eventSize,
            eventsData,
            &event
        );
        CheckCLError(L, err, "Failed requesting enqueue NDRange: %d.");
        return 0;
    }

    static int EnqueueWriteBuffer(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        luacl_buffer_info buffer = luacl_object<luacl_buffer_info>::CheckObject(L, 2);
        std::vector<cl_event> eventList = luacl_object<cl_event>::CheckObjectTable(L, 3, true);
        size_t size = static_cast<size_t>(lua_tonumber(L, 4));
        size_t offset = static_cast<size_t>(lua_tonumber(L, 5));
        size = (size == 0) ? buffer->size : size;
        if (LUACL_UNLIKELY(size + offset > buffer->size)) {
            return luaL_error(L, "Invalid size or offset.");
        }
        cl_bool blocking = lua_toboolean(L, 6);

        cl_event event = NULL;
        cl_int err = clEnqueueWriteBuffer(
            cmdqueue,
            buffer->mem,
            blocking,
            offset,
            size,
            buffer->data,
            static_cast<cl_uint>(eventList.size()),
            eventList.empty() ? NULL : eventList.data(),
            &event
        );
        CheckCLError(L, err, "Failed requesting enqueue write buffer: %d.");
        
        luacl_object<cl_event>::Wrap(L, event);
        return 1;
    }

    static int EnqueueReadBuffer(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        luacl_buffer_info buffer = luacl_object<luacl_buffer_info>::CheckObject(L, 2);
        std::vector<cl_event> eventList = luacl_object<cl_event>::CheckObjectTable(L, 3);
        size_t size = static_cast<size_t>(lua_tonumber(L, 4));
        size_t offset = static_cast<size_t>(lua_tonumber(L, 5));
        size = size == 0 ? buffer->size : size;
        if (LUACL_UNLIKELY(size + offset > buffer->size)) {
            return luaL_error(L, "Invalid size or offset.");
        }
        cl_bool blocking = lua_toboolean(L, 6);

        cl_event event = NULL;
        cl_int err = clEnqueueReadBuffer(
            cmdqueue,
            buffer->mem,
            blocking,
            offset,
            size,
            buffer->data,
            static_cast<cl_uint>(eventList.size()),
            eventList.empty() ? NULL : eventList.data(),
            &event
        );
        CheckCLError(L, err, "Failed requesting enqueue read buffer: %d.");
        
        luacl_object<cl_event>::Wrap(L, event);
        return 1;
    }

    static int Finish(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        cl_int err = clFinish(cmdqueue);
        CheckCLError(L, err, "Failed finishing command queue: %d.");
        return 0;
    }
    
    static int Flush(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        cl_int err = clFlush(cmdqueue);
        CheckCLError(L, err, "Failed flushing command queue: %d.");
        return 0;
    }
};

#endif /* __LUACL_CMDQUEUE_HPP */
