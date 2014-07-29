#ifndef __LUACL_CONTEXT_HPP
#define __LUACL_CONTEXT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_program.hpp"
#include "luacl_cmdqueue.hpp"

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

struct luacl_context {
    typedef luacl_object<cl_context> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_CONTEXT_METATABLE);
		lua_newtable(L);
		lua_pushcfunction(L, GetDevices);
		lua_setfield(L, -2, "GetDevices");
		lua_pushcfunction(L, GetPlatform);
		lua_setfield(L, -2, "GetPlatform");
		lua_pushcfunction(L, luacl_program::Create);
		lua_setfield(L, -2, "CreateProgram");
		lua_pushcfunction(L, luacl_cmdqueue::Create);
		lua_setfield(L, -2, "CreateCommandQueue");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
		lua_newtable(L);
		lua_setfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);
	}
    
    static int Create(lua_State *L) {
		/* Get arg1: platform */
		cl_platform_id platform = luacl_object<cl_platform_id>::CheckObject(L);
		
		/* Get arg2: device | {devices} */
		cl_uint numDevices = 0;
		std::vector<cl_device_id> devices;
		if (lua_istable(L, 2)) {
			//printf("CreateContext: Checking table as device list...\n");
			devices = luacl_object<cl_device_id>::CheckObjectTable(L, 2);
		}
		else if (lua_isuserdata(L, 2)) {
			//printf("CreateContext: Checking userdata as device...\n");
			cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
			devices.push_back(device);
		}
		else {
			return luaL_error(L, "CreateContext: Bad argument, expecting device(s) on arg #2.");
		}
		assert(!devices.empty());	/* The device list should not be NULL by now */

		/* Get arg3: callbackFunc | nil */
		lua_State * callbackThread = NULL;
		if (lua_isfunction(L, 3)) {
			lua_getfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);	/* reg */
			callbackThread = lua_newthread(L);										/* thread, reg */
		}

		/* Actually create context */
		cl_context_properties prop[] = {
			CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0	/* static_cast denied by MSVC? */
		};
		cl_int err;
		cl_context context = clCreateContext(prop, static_cast<cl_uint>(devices.size()), devices.data(), Callback, static_cast<void *>(callbackThread), &err);
		CheckCLError(L, err, "Failed creating context: %d");

		/* Use context pointer as a key, to register callback func */
		if (callbackThread != NULL) {	/* we have a callback func */
			lua_pushlightuserdata(L, static_cast<void *>(context));					/* p, thread, reg */
			lua_insert(L, -2);														/* thread, p, reg */
			lua_settable(L, -3);
		}

		/* Wrap it and return */
		traits::Wrap(L, context);
		if (callbackThread != NULL) {
			lua_pushvalue(L, -3);
			lua_pushvalue(L, -2);				/* Copy wrapped context object */
			lua_xmove(L, callbackThread, 2);	/* Move it to new thread */
		}

#if _DEBUG
		const char private_info[] = "private_info";
		/* Test callback */
		Callback("errinfo", private_info, sizeof(private_info), static_cast<void *>(callbackThread));
#endif
		return 1;
    }

	static void CL_CALLBACK Callback(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
		//printf("CALLBACK!\n");
		if (user_data != NULL) {
			lua_State * callbackThread = static_cast<lua_State *>(user_data);
			lua_pushstring(callbackThread, errinfo);
			lua_pushlstring(callbackThread, static_cast<const char *>(private_info), cb);
			assert(lua_type(callbackThread, -4) == LUA_TFUNCTION);
			lua_resume(callbackThread, 3);
		}
	}
	
	static int GetDevices(lua_State *L) {
		cl_context context = traits::CheckObject(L);
		cl_uint numDevices = 0;
		cl_int err = clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(numDevices), &numDevices, NULL);
		CheckCLError(L, err, "Failed requesting number of devices: %d.");

		size_t size = 0;
		err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of device list: %d.");
		assert(numDevices * sizeof(cl_device_id) == size);

		std::vector<cl_device_id> devices(numDevices);
		err = clGetContextInfo(context, CL_CONTEXT_DEVICES, size, devices.data(), NULL);
		CheckCLError(L, err, "Failed requesting device list: %d.");
		for (unsigned int index = 0; index < numDevices; index++) {
			luacl_object<cl_device_id>::Wrap(L, devices[index]);
		}
		return numDevices;
	}

	static int GetPlatform(lua_State *L) {
		cl_context context = traits::CheckObject(L);
		size_t size = 0;
		cl_int err = clGetContextInfo(context, CL_CONTEXT_PROPERTIES, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of property table: %d.");
		assert(size == 3 * sizeof(cl_context_properties));

		std::vector<cl_context_properties> prop(3);
		err = clGetContextInfo(context, CL_CONTEXT_PROPERTIES, size, prop.data(), NULL);
		CheckCLError(L, err, "Failed requesting property table: %d.");
		assert(prop[0] == CL_CONTEXT_PLATFORM);
		cl_platform_id platform = reinterpret_cast<cl_platform_id>(prop[1]);
		luacl_object<cl_platform_id>::Wrap(L, platform);
		return 1;
	}

	static int Release(lua_State *L) {
		cl_context context = CheckObject(L);
		//printf("__gc Releasing context %p\n", context);

		/* reg[p] = nil */
		lua_getfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);
		lua_pushlightuserdata(L, static_cast<void *>(context));
		lua_pushnil(L);
		lua_settable(L, -3);

		clReleaseContext(context);
		return 0;
	}

	static int Wrap(lua_State *L, cl_context context) {
		return traits::Wrap(L, context);
	}

	static cl_context CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}
};

#endif /* __LUACL_CONTEXT_HPP */
