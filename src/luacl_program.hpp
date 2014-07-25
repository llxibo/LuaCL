#ifndef __LUACL_PROGRAM_HPP
#define __LUACL_PROGRAM_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"
#include <assert.h>

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
		lua_pushcfunction(L, GetBinary);
		lua_setfield(L, -2, "GetBinary");
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

	static int CreateFromBinary(lua_State *L) {
		cl_context context = luacl_object<cl_context>::CheckObject(L);
		size_t size = 0;
		const char * source = luaL_checklstring(L, 2, &size);
		//cl_int err;
		//cl_program program = clCreateProgramWithBinary();

		return 0;
	}

	static int Build(lua_State *L) {
		cl_program program = CheckObject(L);
		const char * options = lua_tostring(L, 2);
		cl_int err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
		lua_pushnumber(L, err);
		return 1;
	}

	static int GetContext(lua_State *L) {
		cl_program program = CheckObject(L);
		cl_context context = NULL;
		cl_int err = clGetProgramInfo(program, CL_PROGRAM_CONTEXT, sizeof(cl_context), &context, NULL);
		CheckCLError(L, err, "Failed requesting context from program: %d.");
		luacl_object<cl_context>::Wrap(L, context);
		return 1;
	}

	static int GetBinary(lua_State *L) {
		cl_program program = CheckObject(L);
        
		/* Get size of binary size list */
		size_t sizeOfSizes = 0;
		cl_int err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, 0, NULL, &sizeOfSizes);
		CheckCLError(L, err, "Failed requesting length of binaries sizes from program: %d.");

		/* Get size of string array */
		size_t sizeOfStrings = 0;
		err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, 0, NULL, &sizeOfStrings);
		CheckCLError(L, err, "Failed requesting length of binaries from program: %d.");
        
		/* Assertion check */
		int numBinaries = static_cast<int>(sizeOfStrings / sizeof(intptr_t));
        if (sizeOfSizes / sizeof(size_t) != numBinaries) {
            return luaL_error(L, "Length of binaries mismatch.");
        }
        
		/* Allocate and request binary size list */
		size_t * sizes = static_cast<size_t *>(malloc(sizeOfSizes));
		CheckAllocError(L, sizes);
		err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeOfSizes, sizes, NULL);
        CheckCLError(L, err, "Failed requesting sizes of binaries from program: %d.", sizes);
		
		/* Allocate binary arrays */
        char ** binaries = static_cast<char **>(malloc(sizeOfStrings * sizeof(char *)));
		if (binaries == NULL) {
			CleanupGetBinary(sizes, binaries, 0);
			return luaL_error(L, LUACL_ERR_MALLOC);
		}
		for (int index = 0; index < numBinaries; index++) {
			binaries[index] = static_cast<char *>(malloc(sizes[index]));
			if (binaries[index] == NULL) {	/* Failed allocation could crash clGetProgramInfo */
				CleanupGetBinary(sizes, binaries, numBinaries);
				return luaL_error(L, LUACL_ERR_MALLOC);
			}
		}

		/* Request copy of binaries */
		err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeOfStrings, binaries, NULL);
		if (err != CL_SUCCESS) {
			CleanupGetBinary(sizes, binaries, numBinaries);
			return luaL_error(L, "Failed requesting binaries from program: %d.", err);
		}
		
		/* Push binaries to Lua */
        for (int index = 0; index < numBinaries; index++) {
            lua_pushlstring(L, binaries[index], sizes[index]);
        }
		CleanupGetBinary(sizes, binaries, numBinaries);
		return numBinaries;
	}

	static void CleanupGetBinary(size_t * sizes, char ** binaries, int numBinaries) { 
		free(sizes);
		for (int index = 0; index < numBinaries; index++) {
			free(binaries[index]);
		}
		free(binaries);
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
		printf("__gc Releasing program %p\n", program);
		LUACL_TRYCALL(clReleaseProgram(program));
		LUACL_TRYCALL(clReleaseProgram(program));
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
