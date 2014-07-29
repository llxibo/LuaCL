#ifndef __LUACL_BUFFER_HPP
#define __LUACL_BUFFER_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_MEM_REGISTRY[] = "LuaCL.Registry.Mem";
static const char LUACL_MEM_METATABLE[] = "LuaCL.Metatable.Mem";
static const char LUACL_MEM_TOSTRING[] = "LuaCL_Mem";

struct luacl_mem_info {
	cl_mem mem;
	void * data;
};

template <>
struct luacl_object_constants<cl_mem> {
	static const char * REGISTRY() {
		return LUACL_MEM_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_MEM_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_MEM_TOSTRING;
	}
	static cl_int Release(cl_mem mem) {
		return clReleaseMemObject(mem);
	}
};

struct luacl_mem {
	typedef luacl_object<cl_mem> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_MEM_METATABLE);
		lua_newtable(L);
		// lua_pushcfunction(L, GetDevices);
		// lua_setfield(L, -2, "GetDevices");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, traits::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, traits::Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
	}

	static int Create(lua_State *L) {
		cl_context context = luacl_object<cl_context>::CheckObject(L, 1);
		size_t size = static_cast<size_t>(luaL_checknumber(L, 2));
		void * data = malloc(size);
		CheckAllocError(L, data);
		memset(data, 0, size);
		cl_int err = 0;
		cl_mem mem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_PTR, size, data, &err);
		CheckCLError(L, err, "Failed creating buffer: %d.", data);
		traits::Wrap(L, mem);
		return 1;
	}

	template <typename T>
	static int Get(lua_State *L) {
		size_t addr = static_cast<size_t>(lua_tonumber(L, 1));
		
	}
};

#endif /* __LUACL_BUFFER_HPP */
