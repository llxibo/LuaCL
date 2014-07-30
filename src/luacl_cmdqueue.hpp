#ifndef __LUACL_CMDQUEUE_HPP
#define __LUACL_CMDQUEUE_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_buffer.hpp"
#include "luacl_event.hpp"

static const char LUACL_CMDQUEUE_REGISTRY[] = "LuaCL.Registry.CmdQueue";
static const char LUACL_CMDQUEUE_METATABLE[] = "LuaCL.Metatable.CmdQueue";
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
		luaL_newmetatable(L, LUACL_CMDQUEUE_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, GetDevices);
		// lua_setfield(L, -2, "GetDevices");
		// lua_pushcfunction(L, GetPlatform);
		// lua_setfield(L, -2, "GetPlatform");
		// lua_pushcfunction(L, luacl_program::Create);
		// lua_setfield(L, -2, "CreateProgram");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, traits::Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
		// lua_newtable(L);
		// lua_setfield(L, LUA_REGISTRYINDEX, LUACL_CMDQUEUE_REGISTRY_CALLBACK);
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
        return 0;
	}

    static int EnqueueWriteBuffer(lua_State *L) {
        cl_command_queue cmdqueue = traits::CheckObject(L);
        luacl_buffer_info buffer = luacl_object<luacl_buffer_info>::CheckObject(L);
        size_t size = static_cast<size_t>(luaL_checknumber(L, 2));
        size_t offset = static_cast<size_t>(lua_tonumber(L, 3));
		size = size == 0 ? buffer->size : size;
		if (size + offset > buffer->size) {
			return luaL_error(L, "Invalid size or offset.");
		}
		std::vector<cl_event> eventList = luacl_object<cl_event>::CheckObjectTable(L, 4);
        cl_bool blocking = lua_toboolean(L, 5);
        cl_event event = 0;
        cl_int err = clEnqueueWriteBuffer(cmdqueue, buffer->mem, blocking, offset, size, buffer->data, static_cast<cl_uint>(eventList.size()), eventList.data(), &event);
        CheckCLError(L, err, "Failed requesting enqueue write buffer: %d.");
        luacl_object<cl_event>::Wrap(L, event);
        return 1;
    }

	static int Finish(lua_State *L) {
		cl_command_queue cmdqueue = traits::CheckObject(L);
		cl_int err = clFinish(cmdqueue);
		CheckCLError(L, err, "Failed finishing command queue: %d.");
		return 0;
	}
};

#endif /* __LUACL_CMDQUEUE_HPP */
