#ifndef __LUACL_PROGRAM_HPP
#define __LUACL_PROGRAM_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"
#include "luacl_kernel.hpp"
#include <assert.h>

static const char LUACL_PROGRAM_REGISTRY[] = "LuaCL_Program_Registry";
static const char LUACL_PROGRAM_METATABLE[] = "LuaCL_Program";
static const char LUACL_PROGRAM_TOSTRING[] = "LuaCL_Program";
static const char LUACL_PROGRAM_REGISTRY_CALLBACK[] = "LuaCL_Program_Callback";

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
    static cl_int Release(cl_program program) {
        return clReleaseProgram(program);
    }
    static const char * CALLBACK() {
        return LUACL_PROGRAM_REGISTRY_CALLBACK;
    }
};

struct luacl_program {
    typedef luacl_object<cl_program> traits;

    static void Init(lua_State *L) {
        traits::CreateMetatable(L);
        traits::RegisterFunction(L, Build, "Build");
        traits::RegisterFunction(L, GetBuildStatus, "GetBuildStatus");
        traits::RegisterFunction(L, GetBuildLog, "GetBuildLog");
        traits::RegisterFunction(L, GetBinary, "GetBinary");
        traits::RegisterFunction(L, GetContext, "GetContext");
        traits::RegisterFunction(L, GetDevices, "GetDevices");
        traits::RegisterFunction(L, luacl_kernel::Create, "CreateKernel");
        lua_setfield(L, -2, "__index");
        traits::RegisterRelease(L);

        traits::CreateRegistry(L);
        traits::CreateCallbackRegistry(L);
    }

    static int Create(lua_State *L) {
        cl_context context = luacl_object<cl_context>::CheckObject(L);
        size_t size = 0;
        const char * source = luaL_checklstring(L, 2, &size);
        cl_int err;
        cl_program program = clCreateProgramWithSource(context, 1, &source, &size, &err);
        CheckCLError(L, err, "Failed building program from source: %s.");
        traits::Wrap(L, program);
        return 1;
    }

    static int CreateFromBinary(lua_State *L) {
        cl_context context = luacl_object<cl_context>::CheckObject(L);
        std::vector<cl_device_id> devices = luacl_object<cl_device_id>::CheckObjectTable(L, 2);
        std::vector<std::string> binaries = traits::CheckStringTable(L, 3);

        size_t size = devices.size();
        // l_debug(L, "Got %zu binary objects", size);
        if (LUACL_UNLIKELY(size != binaries.size())) {
            return luaL_error(L, "Bad argument #2 and #3: table length mismatch.");
        }
        std::vector<size_t> lengths;
        std::vector<const unsigned char *> binaryPointers;  /* No need to free pointers in this container */
        for (size_t index = 0; index < size; index++) {
            lengths.push_back(binaries[index].size());
            // l_debug(L, "Binary size #%zu: %lx", index, binaries[index].size());
            binaryPointers.push_back(
                reinterpret_cast<const unsigned char *>(binaries[index].c_str())
            );
        }
        cl_int err = 0;
        std::vector<cl_int> binaryStatus(size);
        cl_program program = clCreateProgramWithBinary(
            context,
            static_cast<cl_uint>(size),
            devices.data(),
            lengths.data(),
            binaryPointers.data(),
            binaryStatus.data(),
            &err
        );
        CheckCLError(L, err, "Failed creating program from binaries: %s.");

        traits::Wrap(L, program);
        lua_newtable(L);
        for (size_t index = 0; index < size; index++) {
            lua_pushnumber(L, binaryStatus[index]);
            lua_rawseti(L, -2, static_cast<int>(index + 1));
        }
        return 2;
    }

    static int Build(lua_State *L) {
        cl_program program = traits::CheckObject(L);
        const char *options = lua_tostring(L, 2);
        lua_State *thread = traits::CreateCallbackThread(L, 3);
        cl_int err = clBuildProgram(program, 0, NULL, options, thread ? Callback : NULL, thread);
        CheckCLError(L, err, "Failed building program: %s.");
        lua_pushvalue(L, 1);
        traits::RegisterCallback(L);
        // Callback(program, thread); /* Test callback function */
        return 0;
    }

    static void CL_CALLBACK Callback(cl_program program, void *user_data) {
        if (user_data != NULL) {
            lua_State *L = static_cast<lua_State *>(user_data);
            luacl_object<cl_program>::Wrap(L, program);
            traits::DoCallback(L, 1);
        }
    }

    static int GetContext(lua_State *L) {
        cl_program program = traits::CheckObject(L);
        cl_context context = NULL;
        cl_int err = clGetProgramInfo(program, CL_PROGRAM_CONTEXT, sizeof(cl_context), &context, NULL);
        CheckCLError(L, err, "Failed requesting context from program: %s.");
        luacl_object<cl_context>::Wrap(L, context);
        return 1;
    }

    static int GetDevices(lua_State *L) {
        cl_program program = luacl_object<cl_program>::CheckObject(L);
        cl_uint numDevices = 0;
        cl_int err = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(numDevices), &numDevices, NULL);
        CheckCLError(L, err, "Failed requesting number of devices in program: %s.");

        std::vector<cl_device_id> devices(numDevices);
        err = clGetProgramInfo(program, CL_PROGRAM_DEVICES, numDevices * sizeof(cl_device_id), devices.data(), NULL);
        CheckCLError(L, err, "Failed requesting device list in progtam: %s.");

        for (cl_uint index = 0; index < numDevices; index++) {
            luacl_object<cl_device_id>::Wrap(L, devices[index]);
        }
        return static_cast<int>(numDevices);
    }

    static int GetBinary(lua_State *L) {
        cl_program program = traits::CheckObject(L);

        /* Get size of binary size list */
        size_t sizeOfSizes = 0;
        cl_int err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, 0, NULL, &sizeOfSizes);
        CheckCLError(L, err, "Failed requesting length of binaries sizes from program: %s.");

        /* Get size of string array */
        size_t sizeOfStrings = 0;
        err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, 0, NULL, &sizeOfStrings);
        CheckCLError(L, err, "Failed requesting length of binaries from program: %s.");

        /* Assertion check */
        int numBinaries = static_cast<int>(sizeOfStrings / sizeof(intptr_t));
        if (LUACL_UNLIKELY(sizeOfSizes / sizeof(size_t) != numBinaries)) {
            return luaL_error(L, "Length of binaries mismatch.");
        }

        std::vector<size_t> sizes(numBinaries);

        /* Allocate and request binary size list */
        err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeOfSizes, sizes.data(), NULL);
        CheckCLError(L, err, "Failed requesting sizes of binaries from program: %s.");

        /* Allocate binary buffers */
        std::vector< std::vector<char> > binaries(numBinaries);
        std::vector<char *> binaryPointers;
        for (int index = 0; index < numBinaries; index++) {
            //l_debug(L, "Allocating %lx for binary array #%d", sizes[index], index);
            binaries[index].resize(sizes[index]);
            binaryPointers.push_back(binaries[index].data());
        }

        /* Request copy of binaries */
        assert(binaryPointers.size() == numBinaries);
        err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeOfStrings, binaryPointers.data(), NULL);
        CheckCLError(L, err, "Failed requesting binaries from program: %s.");

        /* Push binaries to Lua */
        for (int index = 0; index < numBinaries; index++) {
            lua_pushlstring(L, binaries[index].data(), sizes[index]);
        }
        return numBinaries;
    }

    static int GetBuildStatus(lua_State *L) {
        cl_program program = traits::CheckObject(L);
        cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
        cl_build_status status = CL_BUILD_NONE;
        cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &status, NULL);
        CheckCLError(L, err, "Failed requesting build status: %s.");
        lua_pushnumber(L, status);
        return 1;
    }

    static int GetBuildLog(lua_State *L) {
        cl_program program = traits::CheckObject(L);
        cl_device_id device = luacl_object<cl_device_id>::CheckObject(L, 2);
        size_t size = 0;
        cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
        CheckCLError(L, err, "Failed requesting length of build log: %s.");

        std::vector<char> buffer(size / sizeof(char));
        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, buffer.data(), NULL);
        CheckCLError(L, err, "Failed requesting build log: %s.");
        lua_pushstring(L, std::string(buffer.data(), size).c_str());
        return 1;
    }
};

#endif /* __LUACL_PROGRAM_HPP */
