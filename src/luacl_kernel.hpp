#ifndef __LUACL_KERNEL_HPP
#define __LUACL_KERNEL_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include <assert.h>

static const char LUACL_KERNEL_REGISTRY[] = "LuaCL.Registry.Kernel";
static const char LUACL_KERNEL_METATABLE[] = "LuaCL.Metatable.Kernel";
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
};

struct luacl_kernel {

	typedef luacl_object<cl_kernel> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_KERNEL_METATABLE);
		lua_newtable(L);
		//lua_pushcfunction(L, GetDevices);
		//lua_setfield(L, -1, "GetDevices");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
	}

	static int Create(lua_State *L) {
		cl_program program = luacl_object<cl_program>::CheckObject(L);
		const char * kernelName = luaL_checkstring(L, 2);
		cl_int err;
		cl_kernel krnl = clCreateKernel(program, kernelName, &err);
		CheckCLError(L, err, "Failed creating kernel: %d.");
		Wrap(L, krnl);
		return 1;
	}

	static int Release(lua_State *L) {
		cl_kernel krnl = CheckObject(L);
		printf("__gc Releasing kernel %p\n", krnl);
		LUACL_TRYCALL(clReleaseKernel(krnl));
		return 0;
	}

	static int Wrap(lua_State *L, cl_kernel krnl) {
		return traits::Wrap(L, krnl);
	}

	static cl_kernel CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_KERNEL_HPP */
