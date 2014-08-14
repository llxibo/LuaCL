#ifndef __LUACL_H
#define __LUACL_H

/* Include Lua libraries */
/* Lua.h is not protected with extern C */
extern "C" {
#pragma warning(push)
#pragma warning(disable:4334)
    #include <stdio.h>
    #include <string.h>
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
#pragma warning(pop)
} /* extern "C" */

/* Include OpenCL libraries */
/* OpenCL.h has built-in extern C protection */
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include "CL/OpenCL.h"
#endif

/* Switch for pausing the command line when debugging in MSVC */
#if defined(_MSC_VER)// && defined(_DEBUG)
#define _LUACL_PAUSE_SYSTEM
#endif

#if defined(_LUACL_PAUSE_SYSTEM)
#include <stdlib.h>
#endif

/* Structured Exception Handling (SEH) of Windows could handle potential system exceptions
   in OpenCL calls as C++ exception, and therefore recover from it. */
#ifdef _MSC_VER
#define LUACL_TRYCALL(call) __try { \
        call                        \
    }                               \
    __except (1) {                  \
        luaL_error(L, "Fatal error: error executing OpenCL API %s", #call); \
    }
#else
#define LUACL_TRYCALL(call) {call;}
#endif

//#define LUACL_TRYCALL(call) do { _LUACL_TRYCALL(call); } while (0)

#define LUACL_UNUSED(expr)      (void *)expr;
#define LUACL_LIKELY(expr)      __builtin_expect(!!(expr), true)
#define LUACL_UNLIKELY(expr)    __builtin_expect(!!(expr), false)

/* Include LuaCL modules */
#include "luacl_debug.hpp"
#include "luacl_object.hpp"
#include "luacl_platform.hpp"
#include "luacl_device.hpp"
#include "luacl_context.hpp"
#include "luacl_program.hpp"
#include "luacl_kernel.hpp"
#include "luacl_cmdqueue.hpp"
#include "luacl_buffer.hpp"
#include "luacl_event.hpp"
#include "luacl_endianness.hpp"
#include "luacl_math.hpp"

#endif /* __LUACL_H */
