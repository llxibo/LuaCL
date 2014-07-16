#ifndef __LUACI_API_H
#define __LUACI_API_H

#include "LuaCL.h"
#include <stdlib.h>
#include <string>
#include "luacl_info.h"

static const char LUACL_UDATA_CONTEXT[] = "LuaCL.context";
static const char LUACL_GLOBAL_NAME[] = "LuaCL";

LUALIB_API int luaopen_LuaCL(lua_State *L);

#endif
