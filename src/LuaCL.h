#ifndef __LUACL_H
#define __LUACL_H

/* Lua.h is not protected with extern C */
extern "C" {
#pragma warning(push)
#pragma warning(disable: 4334)
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

#endif