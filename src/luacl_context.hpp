#ifndef __LUACL_CONTEXT_HPP
#define __LUACL_CONTEXT_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_program.hpp"
#include "luacl_cmdqueue.hpp"
#include "luacl_buffer.hpp"

static const char LUACL_CONTEXT_REGISTRY[] = "LuaCL_Context_Registry";
static const char LUACL_CONTEXT_METATABLE[] = "LuaCL_Context";
static const char LUACL_CONTEXT_TOSTRING[] = "LuaCL_Context";
static const char LUACL_CONTEXT_REGISTRY_CALLBACK[] = "LuaCL_ContextCallback_Registry";

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
    static cl_int Release(cl_context context) {
        return clReleaseContext(context);
    }
    static const char * CALLBACK() {
        return LUACL_CONTEXT_REGISTRY_CALLBACK;
    }
};

struct luacl_context {
    typedef luacl_object<cl_context> traits;
    typedef luacl_object<cl_platform_id> traits_platform;
    typedef luacl_object<cl_device_id> traits_device;

    static void Init(lua_State *L) {
        traits::CreateMetatable(L);
        traits::RegisterFunction(L, GetDevices, "GetDevices");
        traits::RegisterFunction(L, GetPlatform, "GetPlatform");
        traits::RegisterFunction(L, luacl_program::Create, "CreateProgram");
        traits::RegisterFunction(L, luacl_program::CreateFromBinary, "CreateProgramFromBinary");
        traits::RegisterFunction(L, luacl_cmdqueue::Create, "CreateCommandQueue");
        traits::RegisterFunction(L, luacl_buffer::Create, "CreateBuffer");
        lua_setfield(L, -2, "__index");
        traits::RegisterRelease(L);
        
        traits::CreateRegistry(L);
        lua_newtable(L);
        lua_setfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);
    }

    static int Create(lua_State *L) {
        /* Get arg1: platform */
        cl_platform_id platform = *traits_platform::CheckObject(L, 1);

        /* Get arg2: device | {devices} */
        std::vector<cl_device_id> devices;
        if (lua_istable(L, 2)) {
            //l_debug(L, "CreateContext: Checking table as device list...");
            devices = traits_device::CheckObjectTable(L, 2);
        }
        else if (lua_isuserdata(L, 2)) {
            //l_debug(L, "CreateContext: Checking userdata as device...");
            cl_device_id device = *traits_device::CheckObject(L, 2);
            devices.push_back(device);
        }
        luaL_argcheck(L, !devices.empty(), 2, "Expecting one or more valid devices");

        /* Get arg3: callbackFunc | nil */
        /*  The easiest way of registering callback function is to insert func into a registry table.
            However, it requires a unique key, for calling back as well as removing callback entry upon __gc.
            Ideally, cl_context is the best key, but it is not available before clCreateContext.
            Another issue is that lua_State will not be available unless it is kept by somewhere "global".

            To work around this, push callback function to a new thread prior to creation of context.
            Then insert the thread to registry, and set context as key of it.
            The pointer of new lua_State is a nice reference, contains all information we need to call a lua function. */
        lua_State *thread = traits::CreateCallbackThread(L, 3);                     /* thread */

        cl_context_properties prop[] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0   /* static_cast denied by MSVC? */
        };
        cl_int err = 0;
        cl_context context = clCreateContext(prop, static_cast<cl_uint>(devices.size()), devices.data(), thread ? Callback : NULL, thread, &err);
        CheckCLError(L, err, "Failed creating context: %d");

        traits::Wrap(L, context);                                                   /* context, thread */
        traits::RegisterCallback(L);                                                /* (empty stack) */

        traits::Wrap(L, context);                                                   /* context */
#if _DEBUG              /* Test callback */
        const char private_info[] = "private_info"; 
        Callback("errinfo", private_info, sizeof(private_info), static_cast<void *>(thread));
#endif
        return 1;
    }

    static void CL_CALLBACK Callback(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
        //printf("CALLBACK!\n");
        if (user_data != NULL) {
            lua_State *L = static_cast<lua_State *>(user_data);                     /* context, func */
            lua_pushvalue(L, -1);                                                   /* context, context, func */
            lua_insert(L, -3);                                                      /* context, func, context */
            lua_pushstring(L, errinfo);                                             /* errinfo, context, func, context */
            lua_pushlstring(L, static_cast<const char *>(private_info), cb);        /* private_info, errinfo, context, func, context */
            traits::DoCallback(L, 3);                                               /* func, context */
            lua_insert(L, -2);                                                      /* context, func */
        }
    }

    static int GetDevices(lua_State *L) {
        cl_context context = *traits::CheckObject(L, 1);
        cl_uint numDevices = 0;
        cl_int err = clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(numDevices), &numDevices, NULL);
        CheckCLError(L, err, "Failed requesting number of devices: %s.");

        size_t size = 0;
        err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting length of device list: %s.");
        assert(numDevices * sizeof(cl_device_id) == size);

        std::vector<cl_device_id> devices(numDevices);
        err = clGetContextInfo(context, CL_CONTEXT_DEVICES, size, devices.data(), NULL);
        CheckCLError(L, err, "Failed requesting device list: %s.");
        for (unsigned int index = 0; index < numDevices; index++) {
            luacl_object<cl_device_id>::Wrap(L, devices[index]);
        }
        return numDevices;
    }

    static int GetPlatform(lua_State *L) {
        cl_context context = *traits::CheckObject(L, 1);
        size_t size = 0;
        cl_int err = clGetContextInfo(context, CL_CONTEXT_PROPERTIES, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting length of property table: %s.");
        assert(size == 3 * sizeof(cl_context_properties));

        std::vector<cl_context_properties> prop(3);
        err = clGetContextInfo(context, CL_CONTEXT_PROPERTIES, size, prop.data(), NULL);
        CheckCLError(L, err, "Failed requesting property table: %s.");
        assert(prop[0] == CL_CONTEXT_PLATFORM);
        cl_platform_id platform = reinterpret_cast<cl_platform_id>(prop[1]);
        luacl_object<cl_platform_id>::Wrap(L, platform);
        return 1;
    }

    static int Release(lua_State *L) {
        cl_context context = *traits::CheckObject(L, 1);

        /* reg[p] = nil */
        lua_getfield(L, LUA_REGISTRYINDEX, LUACL_CONTEXT_REGISTRY_CALLBACK);
        lua_pushlightuserdata(L, static_cast<void *>(context));
        lua_pushnil(L);
        lua_settable(L, -3);
        lua_pop(L, 1);

        return traits::Release(L);
    }
};

#endif /* __LUACL_CONTEXT_HPP */
