#ifndef __LUACL_KERNEL_HPP
#define __LUACL_KERNEL_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include <assert.h>

static const char LUACL_KERNEL_REGISTRY[] = "LuaCL_Kernel_Registry";
static const char LUACL_KERNEL_METATABLE[] = "LuaCL_Kernel";
static const char LUACL_KERNEL_TOSTRING[] = "LuaCL_Kernel";
static const char LUACL_KERNEL_DEFAULT_NAME[] = "<anonymous kernel>";
template <>
struct luacl_object_constants<cl_kernel> {
	static const char * REGISTRY() {
		return LUACL_KERNEL_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_KERNEL_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_KERNEL_TOSTRING;
	}
	static cl_int Release(cl_kernel krnl) {
		return clReleaseKernel(krnl);
	}
};

struct luacl_kernel {
	typedef luacl_object<cl_kernel> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_KERNEL_METATABLE);
		lua_newtable(L);
		lua_pushcfunction(L, GetContext);
		lua_setfield(L, -2, "GetContext");
		lua_pushcfunction(L, GetProgram);
		lua_setfield(L, -2, "GetProgram");
		lua_pushcfunction(L, GetNumArgs);
		lua_setfield(L, -2, "GetNumArgs");
#ifdef CL_VERSION_1_2
		lua_pushcfunction(L, GetArgInfo);
		lua_setfield(L, -2, "GetArgInfo");
#endif /* CL_VERSION_1_2 */
		lua_pushcfunction(L, GetFunctionName);
		lua_setfield(L, -2, "GetFunctionName");
		lua_pushcfunction(L, GetWorkGroupInfo);
		lua_setfield(L, -2, "GetWorkGroupInfo");
		lua_pushcfunction(L, SetArg<float>);
		lua_setfield(L, -2, "SetArgFloat");
		lua_pushcfunction(L, SetArg<cl_int>);
		lua_setfield(L, -2, "SetArgInt");
		lua_pushcfunction(L, SetArg<cl_uint>);
		lua_setfield(L, -2, "SetArgUInt");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, traits::Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
	}

	static int Create(lua_State *L) {
		cl_program program = luacl_object<cl_program>::CheckObject(L);
		const char * kernelName = luaL_checkstring(L, 2);
		cl_int err;
		cl_kernel krnl = clCreateKernel(program, kernelName, &err);
		CheckCLError(L, err, "Failed creating kernel: %d.");
		traits::Wrap(L, krnl);
		return 1;
	}

	static int GetContext(lua_State *L) {
		cl_kernel krnl = traits::CheckObject(L);
		cl_context context = NULL;
		cl_int err = clGetKernelInfo(krnl, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);
		CheckCLError(L, err, "Failed requesting context info from kernel: %d.");
		luacl_object<cl_context>::Wrap(L, context);
		return 1;
	}

	static int GetProgram(lua_State *L) {
		cl_kernel krnl = traits::CheckObject(L);
		cl_program program = NULL;
		cl_int err = clGetKernelInfo(krnl, CL_KERNEL_PROGRAM, sizeof(cl_program), &program, NULL);
		CheckCLError(L, err, "Failed requesting program info from kernel: %d.");
		luacl_object<cl_program>::Wrap(L, program);
		return 1;
	}

	static int GetNumArgs(lua_State *L) {
		cl_kernel krnl = traits::CheckObject(L);
		cl_uint numArgs = 0;
		cl_int err = clGetKernelInfo(krnl, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &numArgs, NULL);
		CheckCLError(L, err, "Failed requesting number of args info from kernel: %d.");
		lua_pushnumber(L, numArgs);
		return 1;
	}

	static int GetFunctionName(lua_State *L) {
		cl_kernel krnl = traits::CheckObject(L);
		size_t size = 0;
		cl_int err = clGetKernelInfo(krnl, CL_KERNEL_FUNCTION_NAME, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting size of function name from kernel: %d.");

		std::vector<char> funcName(size);
		err = clGetKernelInfo(krnl, CL_KERNEL_FUNCTION_NAME, size, funcName.data(), NULL);
		CheckCLError(L, err, "Failed requesting number of args info from kernel: %d.");
		lua_pushstring(L, std::string(funcName.data(), size).c_str());
		return 1;
	}
    
    static int GetWorkGroupInfo(lua_State *L) {
        cl_kernel krnl = traits::CheckObject(L);
        cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
        lua_newtable(L);
        PushWorkGroupInfo<size_t>(L, krnl, device, CL_KERNEL_WORK_GROUP_SIZE, "WORK_GROUP_SIZE");
        PushWorkGroupInfo<size_t>(L, krnl, device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, "COMPILE_WORK_GROUP_SIZE", 3);
        PushWorkGroupInfo<cl_ulong>(L, krnl, device, CL_KERNEL_LOCAL_MEM_SIZE, "LOCAL_MEM_SIZE");
        PushWorkGroupInfo<size_t>(L, krnl, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, "PREFERRED_WORK_GROUP_SIZE_MULTIPLE");
        PushWorkGroupInfo<cl_ulong>(L, krnl, device, CL_KERNEL_PRIVATE_MEM_SIZE, "PRIVATE_MEM_SIZE");
        return 1;
    }
    
    template <typename T>
    static int SetArg(lua_State *L) {
        cl_kernel krnl = traits::CheckObject(L);
        cl_uint index = static_cast<cl_uint>(luaL_checknumber(L, 2));
        T value = static_cast<T>(luaL_checknumber(L, 3));
        cl_int err = clSetKernelArg(krnl, index - 1, sizeof(value), &value);
        CheckCLError(L, err, "Failed setting kernel arg: %d.");
        return 0;
    }
    
    template <typename T>
    static int PushWorkGroupInfo(lua_State *L, cl_kernel krnl, cl_device_id device, cl_kernel_work_group_info param, const char * paramName, int length = 1) {
        size_t size = 0;
        cl_int err = clGetKernelWorkGroupInfo(krnl, device, param, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting size of workgroup info: %d.");
        assert(size == sizeof(T) * length);
        
        std::vector<T> value(length);
        err = clGetKernelWorkGroupInfo(krnl, device, param, size, value.data(), NULL);
        CheckCLError(L, err, "Failed requesting workgroup info: %d.");
        if (length > 1) {
            lua_newtable(L);
            for (int index = 0; index < length; index++) {
                lua_pushnumber(L, static_cast<lua_Number>(value[index]));
                lua_rawseti(L, -2, index + 1);
            }
        }
        else {
            lua_pushnumber(L, static_cast<lua_Number>(value[0]));
        }
        lua_setfield(L, -2, paramName);
        return 0;
    }

#ifdef CL_VERSION_1_2
	static int GetArgInfo(lua_State *L) {
		cl_kernel krnl = traits::CheckObject(L);
		cl_uint index = static_cast<cl_uint>(luaL_checknumber(L, 2));
		lua_newtable(L);
		PushArgInfo<cl_kernel_arg_address_qualifier>(L, krnl, index, CL_KERNEL_ARG_ADDRESS_QUALIFIER, "ADDRESS_QUALIFIER");
		PushArgInfo<cl_kernel_arg_access_qualifier>(L, krnl, index, CL_KERNEL_ARG_ACCESS_QUALIFIER, "ACCESS_QUALIFIER");
		PushArgInfoStr(L, krnl, index, CL_KERNEL_ARG_TYPE_NAME, "TYPE_NAME");
		PushArgInfo<cl_kernel_arg_type_qualifier>(L, krnl, index, CL_KERNEL_ARG_TYPE_QUALIFIER, "TYPE_QUALIFIER");
		PushArgInfoStr(L, krnl, index, CL_KERNEL_ARG_NAME, "NAME");
		return 1;
	}

	template <typename T>
	static int PushArgInfo(lua_State *L, cl_kernel krnl, cl_uint index, cl_kernel_arg_info param, const char * paramName) {
		size_t size = 0;
		cl_int err = clGetKernelArgInfo(krnl, index, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting size of kernel arg info: %d.");
		assert(size == sizeof(T));

		T value = 0;
		err = clGetKernelArgInfo(krnl, index, param, size, &value, NULL);
		CheckCLError(L, err, "Failed requesting kernel arg info: %d.");
		lua_pushnumber(L, static_cast<lua_Number>(value));
		lua_setfield(L, -2, paramName);
		return 0;
	}

	static int PushArgInfoStr(lua_State *L, cl_kernel krnl, cl_uint index, cl_kernel_arg_info param, const char * paramName) {
		size_t size = 0;
		cl_int err = clGetKernelArgInfo(krnl, index, param, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting size of kernel arg info string: %d.");
		
        l_debug(L, "Pushing arg info str %s with length %d", paramName, size);
		std::vector<char> value(size);
		err = clGetKernelArgInfo(krnl, index, param, size, value.data(), NULL);
		CheckCLError(L, err, "Failed requesting kernel arg info string: %d.");
		lua_pushstring(L, std::string(value.data(), value.size()).c_str());
		lua_setfield(L, -2, paramName);
		return 0;
	}
#endif /* CL_VERSION_1_2 */
};

#endif /* __LUACL_KERNEL_HPP */
