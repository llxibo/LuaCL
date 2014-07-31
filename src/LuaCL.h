#ifndef __LUACL_H
#define __LUACL_H

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

#if _MSC_VER && _DEBUG
#define _LUACL_PAUSE_SYSTEM
#endif

#if _LUACL_PAUSE_SYSTEM
#include <stdlib.h>
#include <stdint.h>
#endif

/* OpenCL.h has built-in extern C protection */
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include "CL/OpenCL.h"
#endif

#include "luacl_endianness.hpp"

#ifdef _MSC_VER
#define _LUACL_TRYCALL(call) __try {	\
		call						\
	}								\
	__except (1) {					\
		luaL_error(L, "Fatal error: error executing OpenCL API %s", #call);	\
	}
#else
#define _LUACL_TRYCALL(call) call
#endif

#define LUACL_TRYCALL(call) do { _LUACL_TRYCALL(call) } while (0)

#endif /* __LUACL_H */
