#ifndef __LUACL_ERROR_HPP
#define __LUACL_ERROR_HPP

#include <string>

#define LUACL_ERROR_DEFINE(name) case CL_ ## name: return #name; break;

inline const char *GetCLErrorInfo(cl_int err) {
    switch (err) {
        LUACL_ERROR_DEFINE(DEVICE_NOT_FOUND)
        LUACL_ERROR_DEFINE(DEVICE_NOT_AVAILABLE)
        LUACL_ERROR_DEFINE(COMPILER_NOT_AVAILABLE)
        LUACL_ERROR_DEFINE(MEM_OBJECT_ALLOCATION_FAILURE)
        LUACL_ERROR_DEFINE(OUT_OF_RESOURCES)
        LUACL_ERROR_DEFINE(OUT_OF_HOST_MEMORY)
        LUACL_ERROR_DEFINE(PROFILING_INFO_NOT_AVAILABLE)
        LUACL_ERROR_DEFINE(MEM_COPY_OVERLAP)
        LUACL_ERROR_DEFINE(IMAGE_FORMAT_MISMATCH)
        LUACL_ERROR_DEFINE(IMAGE_FORMAT_NOT_SUPPORTED)
        LUACL_ERROR_DEFINE(BUILD_PROGRAM_FAILURE)
        LUACL_ERROR_DEFINE(MAP_FAILURE)
        LUACL_ERROR_DEFINE(MISALIGNED_SUB_BUFFER_OFFSET)
        LUACL_ERROR_DEFINE(EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
        LUACL_ERROR_DEFINE(COMPILE_PROGRAM_FAILURE)
        LUACL_ERROR_DEFINE(LINKER_NOT_AVAILABLE)
        LUACL_ERROR_DEFINE(LINK_PROGRAM_FAILURE)
        LUACL_ERROR_DEFINE(DEVICE_PARTITION_FAILED)
        LUACL_ERROR_DEFINE(KERNEL_ARG_INFO_NOT_AVAILABLE)
        LUACL_ERROR_DEFINE(INVALID_VALUE)
        LUACL_ERROR_DEFINE(INVALID_DEVICE_TYPE)
        LUACL_ERROR_DEFINE(INVALID_PLATFORM)
        LUACL_ERROR_DEFINE(INVALID_DEVICE)
        LUACL_ERROR_DEFINE(INVALID_CONTEXT)
        LUACL_ERROR_DEFINE(INVALID_QUEUE_PROPERTIES)
        LUACL_ERROR_DEFINE(INVALID_COMMAND_QUEUE)
        LUACL_ERROR_DEFINE(INVALID_HOST_PTR)
        LUACL_ERROR_DEFINE(INVALID_MEM_OBJECT)
        LUACL_ERROR_DEFINE(INVALID_IMAGE_FORMAT_DESCRIPTOR)
        LUACL_ERROR_DEFINE(INVALID_IMAGE_SIZE)
        LUACL_ERROR_DEFINE(INVALID_SAMPLER)
        LUACL_ERROR_DEFINE(INVALID_BINARY)
        LUACL_ERROR_DEFINE(INVALID_BUILD_OPTIONS)
        LUACL_ERROR_DEFINE(INVALID_PROGRAM)
        LUACL_ERROR_DEFINE(INVALID_PROGRAM_EXECUTABLE)
        LUACL_ERROR_DEFINE(INVALID_KERNEL_NAME)
        LUACL_ERROR_DEFINE(INVALID_KERNEL_DEFINITION)
        LUACL_ERROR_DEFINE(INVALID_KERNEL)
        LUACL_ERROR_DEFINE(INVALID_ARG_INDEX)
        LUACL_ERROR_DEFINE(INVALID_ARG_VALUE)
        LUACL_ERROR_DEFINE(INVALID_ARG_SIZE)
        LUACL_ERROR_DEFINE(INVALID_KERNEL_ARGS)
        LUACL_ERROR_DEFINE(INVALID_WORK_DIMENSION)
        LUACL_ERROR_DEFINE(INVALID_WORK_GROUP_SIZE)
        LUACL_ERROR_DEFINE(INVALID_WORK_ITEM_SIZE)
        LUACL_ERROR_DEFINE(INVALID_GLOBAL_OFFSET)
        LUACL_ERROR_DEFINE(INVALID_EVENT_WAIT_LIST)
        LUACL_ERROR_DEFINE(INVALID_EVENT)
        LUACL_ERROR_DEFINE(INVALID_OPERATION)
        LUACL_ERROR_DEFINE(INVALID_GL_OBJECT)
        LUACL_ERROR_DEFINE(INVALID_BUFFER_SIZE)
        LUACL_ERROR_DEFINE(INVALID_MIP_LEVEL)
        LUACL_ERROR_DEFINE(INVALID_GLOBAL_WORK_SIZE)
        LUACL_ERROR_DEFINE(INVALID_PROPERTY)
        LUACL_ERROR_DEFINE(INVALID_IMAGE_DESCRIPTOR)
        LUACL_ERROR_DEFINE(INVALID_COMPILER_OPTIONS)
        LUACL_ERROR_DEFINE(INVALID_LINKER_OPTIONS)
        LUACL_ERROR_DEFINE(INVALID_DEVICE_PARTITION_COUNT)
        default:
            return std::to_string(err).c_str();
    }
}


static const char LUACL_ERR_MALLOC[] = "Insufficient memory";
static const char LUACL_DEBUG_CALLBACK_FUNC[] = "LuaCL_Debug_Callback_Func";

#if defined(_DEBUG)
/* Debug function. Calls registered callback function with debug string. [-0, +0, e] */
inline void l_debug(lua_State *L, const char * message, ...) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUACL_DEBUG_CALLBACK_FUNC);  /* func/nil */
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);      /* Pop nil to keep stack balanced */
        return;
    }
    va_list argp;
    va_start(argp, message);
    lua_pushvfstring(L, message, argp);                             /* str, func */
    va_end(argp);
    lua_call(L, 1, 0);                                              /* (empty stack) */
}
#else
void l_debug(lua_State *L, const char * message, ...) {
    LUACL_UNUSED(L);
    LUACL_UNUSED(message);
}
#endif /* _DEBUG */

/* Macro instead of inline function to explicitly return from caller function.
 * The inline version will longjmp away from caller, leaving its following pointer usage safe.
 * However, compilers and analyzers may not recognize this behavior and therefore complain about it.
 * An exception could also jump out of caller function, but it comes with extra overhead, and could be hard to handle.
 * So we got this work-around, to shut compiler from complaining and keep away from exception implementation.
 * The macro is assumed to be used in a lua_CFunction, which returns int. */
#define CHECK_ALLOC_ERROR_MACRO 1
#if (CHECK_ALLOC_ERROR_MACRO)
#define CheckAllocError(L, p) {             \
    if (LUACL_UNLIKELY(p == NULL)) {        \
        luaL_error(L, LUACL_ERR_MALLOC);    \
        return 0;                           \
    }                                       \
};
#else
void CheckAllocError(lua_State *L, void *p, const char * msg = LUACL_ERR_MALLOC) {
    if (LUACL_UNLIKELY(p == NULL)) {
        luaL_error(L, msg);
    }
}
#endif

void CheckCLError(lua_State *L, cl_int err, const char * msg) {
    if (LUACL_UNLIKELY(err != CL_SUCCESS)) {
        luaL_error(L, msg, GetCLErrorInfo(err));
    }
}

#endif  /* __LUACL_ERROR_HPP */
