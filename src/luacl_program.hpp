#ifndef __LUACL_PROGRAM_HPP
#define __LUACL_PROGRAM_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"

static const char LUACL_PROGRAM_REGISTRY[] = "LuaCL.Registry.Program";
static const char LUACL_PROGRAM_METATABLE[] = "LuaCL.Metatable.Program";
static const char LUACL_PROGRAM_TOSTRING[] = "LuaCL_Program";

template <>
struct luacl_object_constants <cl_program> {
	static const char * REGISTRY() {
		return LUACL_PROGRAM_REGISTRY;
	}
	static const char * METATABLE() {
		return LUACL_PROGRAM_METATABLE;
	}
	static const char * TOSTRING() {
		return LUACL_PROGRAM_TOSTRING;
	}
};

struct luacl_program {

	typedef luacl_object<cl_program> traits;

	static void Init(lua_State *L) {
		luaL_newmetatable(L, LUACL_PROGRAM_METATABLE);	/* new metatable */
		lua_newtable(L);								/* mt = {} */
		lua_pushcfunction(L, Build);
		lua_setfield(L, -2, "Build");
		lua_pushcfunction(L, GetBuildStatus);
		lua_setfield(L, -2, "GetBuildStatus");
		lua_pushcfunction(L, GetBuildLog);
		lua_setfield(L, -2, "GetBuildLog");
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, Release);
		lua_setfield(L, -2, "__gc");

		traits::CreateRegistry(L);
	}

	static int GetInfo(lua_State *L) {
		cl_program platform = CheckObject(L);
		
		return 0;
	}

	static int Create(lua_State *L) {
		cl_context context = luacl_object<cl_context>::CheckObject(L);
		size_t size = 0;
		const char * source = luaL_checklstring(L, 2, &size);
		cl_int err;
		cl_program program = clCreateProgramWithSource(context, 1, &source, &size, &err);
		CheckCLError(L, err, "Failed building program from source: %d.");
		Wrap(L, program);
		return 1;
	}

	static int Build(lua_State *L) {
		cl_program program = CheckObject(L);
		const char * options = lua_tostring(L, 2);
		cl_int err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
		lua_pushnumber(L, err);
		return 1;
	}

	static int GetBuildStatus(lua_State *L) {
		cl_program program = CheckObject(L);
		cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
		cl_build_status status = CL_BUILD_NONE;
		cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &status, NULL);
		CheckCLError(L, err, "Failed requesting build status: %d.");
		lua_pushnumber(L, status);
		return 1;
	}

	static int GetBuildLog(lua_State *L) {
		cl_program program = CheckObject(L);
		cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
		size_t size = 0;
		cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		CheckCLError(L, err, "Failed requesting length of build log: %d.");

		char * log_string = static_cast<char *>(malloc(size));
		CheckAllocError(L, log_string);
		err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, log_string, NULL);
		CheckCLError(L, err, "Failed requesting build log: %d.", log_string);
		lua_pushlstring(L, log_string, size);
		return 1;
	}

	static int Release(lua_State *L) {
		cl_program program = CheckObject(L);
		clReleaseProgram(program);
		printf("__gc Releasing program %p\n", program);
		return 0;
	}

	static int Wrap(lua_State *L, cl_program program) {
		return traits::Wrap(L, program);
	}

	static cl_program CheckObject(lua_State *L) {
		return traits::CheckObject(L);
	}

	static int ToString(lua_State *L) {
		return traits::ToString(L);
	}
};

#endif /* __LUACL_PROGRAM_HPP */
