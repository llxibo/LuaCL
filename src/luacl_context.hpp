#ifndef __LUACL_CONTEXT_HPP
#define __LUACL_CONTEXT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_CONTEXT_REGISTRY[] = "LuaCL.Registry.Context";
static const char LUACL_CONTEXT_METATABLE[] = "LuaCL.Metatable.Context";
static const char LUACL_CONTEXT_TOSTRING[] = "LuaCL_Context";
static const char LUACL_CONTEXT_REGISTRY_CALLBACK[] = "LuaCL.Registry.ContextCallback";

template <>
struct luacl_object_constants<cl_context> {
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

template <>
struct luacl_object_constants<lua_CFunction> {
	static const char * REGISTRY() {
		return LUACL_CONTEXT_REGISTRY_CALLBACK;
	}
};

struct luacl_context {
    typedef luacl_object<cl_context> traits;
	//typedef luacl_object<lua_CFunction> traits_callback;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_CONTEXT_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, Create);
		// lua_setfield(L, -2, "Create");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		traits::CreateRegistry(L);
		//traits_callback::CreateRegistry(L);
	}
    
    static int Create(lua_State *L) {
		/* Get arg1: platform */
		cl_platform_id platform = luacl_object<cl_platform_id>::CheckObject(L);
		lua_remove(L, 1);

		/* Get arg2: device | {devices} */
		size_t numDevices = 0;
		cl_device_id * devices = NULL;
		if (lua_istable(L, 1)) {
			numDevices = lua_objlen(L, 1);
			devices = static_cast<cl_device_id *>(malloc(sizeof(cl_device_id) * numDevices));
			CheckAllocError(L, devices);
			for (size_t index = 0; index < numDevices; index++) {
				devices[index] = luacl_object<cl_device_id>::CheckObject(L, devices);
			}
		}
		else if (lua_isuserdata(L, 1)) {
			numDevices = 1;
			devices = static_cast<cl_device_id *>(malloc(sizeof(cl_device_id)));
			CheckAllocError(L, devices);
			*devices = luacl_object<cl_device_id>::CheckObject(L, devices);
		}
		else {
			return luaL_error(L, "CreateContext: Bad argument, expecting device(s) on arg #2.");
		}
		assert(devices != NULL);	/* The device list should not be NULL by now */

		/* Get arg3: callbackFunc | nil */
		lua_State * callbackThread = NULL;
		if (lua_isfunction(L, 2)) {	/* We poped out arg1, so it should be 2 */
			lua_getfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);	/* reg */
			callbackThread = lua_newthread(L);										/* thread, reg */
		}

		/* Actually create context */
		cl_context_properties prop[] = {
			CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0	/* static_cast denied by MSVC? */
		};
		cl_int err;
		cl_context context = clCreateContext(prop, numDevices, devices, Callback, static_cast<void *>(callbackThread), &err);
		CheckCLError(L, err, "Failed creating context: %d", devices);

		/* Use context pointer as key, to register callback func */
		if (callbackThread != NULL) {	/* as long as we have a callback func */
			lua_pushlightuserdata(L, static_cast<void *>(context));					/* p, thread, reg */
			lua_insert(L, -2);														/* thread, p, reg */
			lua_settable(L, -3);
		}

		/* Wrap it and return */
		Wrap(L, context);

		if (callbackThread != NULL) {
			lua_pushvalue(L, 2);				/* Copy the callback function */
			lua_xmove(L, callbackThread, -1);	/* Move it to new thread */
			lua_pushvalue(L, -1);				/* Copy wrapped context object */
			lua_xmove(L, callbackThread, -1);	/* Move it to new thread */
		}
		return 1;
    }

	static void CL_CALLBACK Callback(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
		lua_State * callbackThread = static_cast<lua_State *>(user_data);
		lua_pushstring(callbackThread, errinfo);
		lua_pushlstring(callbackThread, static_cast<const char *>(private_info), cb);
		lua_resume(callbackThread, 3);
	}

	static int Release(lua_State *L) {
		cl_context context = CheckObject(L);
		printf("__gc Releasing context %p\n", context);

		/* reg[p] = nil */
		lua_getfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);
		lua_pushlightuserdata(L, static_cast<void *>(context));
		lua_pushnil(L);
		lua_settable(L, -3);

		clReleaseContext(context);
	}

	static int Wrap(lua_State *L, cl_context context) {
		return traits::Wrap(L, context);
	}

	static cl_context CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_CONTEXT_HPP */
