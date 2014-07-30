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

#include <stdlib.h>

/* OpenCL.h has built-in extern C protection */
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include "CL/OpenCL.h"
#endif

#ifdef _MSC_VER
#define LUACL_TRYCALL2(call) __try {	\
		call						\
	}								\
	__except (1) {					\
		luaL_error(L, "Fatal error: error executing OpenCL API %s", #call);	\
	}
#else
#define LUACL_TRYCALL2(call) call
#endif

#define LUACL_TRYCALL(call) do{ LUACL_TRYCALL2(call) }while(0)

#endif
