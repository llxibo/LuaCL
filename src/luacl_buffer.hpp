#ifndef __LUACL_BUFFER_HPP
#define __LUACL_BUFFER_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"

static const char LUACL_MEM_REGISTRY[] = "LuaCL.Registry.Mem";
static const char LUACL_MEM_METATABLE[] = "LuaCL.Metatable.Mem";
static const char LUACL_MEM_TOSTRING[] = "LuaCL_Mem";

struct luacl_buffer_object {
	cl_mem mem = NULL;
	void * data = NULL;
	size_t size = 0;

	~luacl_buffer_object() {
		if (mem != NULL) {
			clReleaseMemObject(mem);
		}
		free(data);
	}
};

typedef luacl_buffer_object* luacl_buffer_info;

template <>
struct luacl_object_constants<luacl_buffer_info> {
	static const char * REGISTRY() {
		return LUACL_MEM_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_MEM_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_MEM_TOSTRING;
	}
	static cl_int Release(luacl_buffer_info mem) {
		delete mem;
		return CL_SUCCESS;
	}
};

struct luacl_buffer {
	typedef luacl_object<luacl_buffer_info> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_MEM_METATABLE);
		lua_newtable(L);
		lua_pushcfunction(L, Get<int>);
		lua_setfield(L, -2, "GetInt");
		lua_pushcfunction(L, Set<int>);
		lua_setfield(L, -2, "SetInt");
		lua_pushcfunction(L, Get<float>);
		lua_setfield(L, -2, "GetFloat");
		lua_pushcfunction(L, Set<float>);
		lua_setfield(L, -2, "SetFloat");
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
		cl_mem_flags flags = static_cast<cl_mem_flags>(lua_tonumber(L, 3));
		flags = (flags == 0) ? (CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR) : flags;

		void * data = malloc(size);
		CheckAllocError(L, data);
		memset(data, 0, size);
		luacl_buffer_info bufferObject = new luacl_buffer_object;
		bufferObject->data = data;		/* The allocated memory is now guarded by bufferObject */
		bufferObject->size = size;

		cl_int err = 0;
		cl_mem mem = clCreateBuffer(context, flags, size, data, &err);
		CheckCLError(L, err, "Failed creating buffer: %d.");	/* Potential function exit, *data will be released by bufferObject destructor */
		bufferObject->mem = mem;
		traits::Wrap(L, bufferObject);
		return 1;
	}

	template <typename T>
	static int Get(lua_State *L) {
		luacl_buffer_info buffer = traits::CheckObject(L, 1);
		size_t addr = static_cast<size_t>(lua_tonumber(L, 2));
		// assert(addr >= 0 && addr * sizeof(T) < buffer->size);
		T * data = reinterpret_cast<T *>(buffer->data);
		lua_pushnumber(L, static_cast<lua_Number>(data[addr]));
		return 1;
	}

	template <typename T>
	static int Set(lua_State *L) {
		luacl_buffer_info buffer = traits::CheckObject(L, 1);
		size_t addr = static_cast<size_t>(lua_tonumber(L, 2));
		T value = static_cast<T>(lua_tonumber(L, 3));
		// assert(addr >= 0 && sizeof(addr) < buffer->size);
		T * data = reinterpret_cast<T *>(buffer->data);
		data[addr] = value;
		//* (reinterpret_cast<T *>(buffer->data) + addr) = value;
		return 0;
	}
};

#endif /* __LUACL_BUFFER_HPP */
