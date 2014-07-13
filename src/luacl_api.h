#ifndef __LUACI_API_H
#define __LUACI_API_H

#include "LuaCL.h"
#include <stdlib.h>
#include <string>

#include "luacl_info.h"

int luacl_GetNumPlatforms(lua_State *L);

int luacl_GetPlatformInfo(lua_State *L);

int luacl_GetNumDevices(lua_State *L);

#endif