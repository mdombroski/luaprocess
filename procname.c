/*
* procname.c
* procname library for Lua 5.1
* Matthew Dombroski <matt@mattski.net.nz>
*
* Based on original for python
* http://code.google.com/p/procname/
*/

#include <string.h>

# if defined __linux__
#include <sys/prctl.h>
# endif

#define PNAME		"procname"
#define PVERSION	PNAME " library for " LUA_VERSION " / October 2009"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


int Pgetname( lua_State* L )
{
	int err = 0;

# if defined __linux__
	char name[17] = {0};
	err = prctl( PR_GET_NAME, name );
	lua_pushstring( L, name );
# else
	return luaL_error( L, "procname is not supported on this platform" );
# endif

	return 1;
}


int Psetname( lua_State* L )
{
	int err = 0;
	const char* name = luaL_checkstring( L, 1 );

# if defined __linux__
	err = prctl( PR_SET_NAME, name );
# else
	return luaL_error( L, "procname is not supported on this platform" );
# endif

	return 0;
}


static const luaL_reg R[] =
{
	{ "get",	Pgetname },
	{ "set",	Psetname },
	{ NULL,		NULL }
};

LUALIB_API int luaopen_procname (lua_State *L)
{
	luaL_register( L, PNAME, R );

	lua_pushliteral( L, "version" );
	lua_pushliteral( L, PVERSION );
	lua_settable( L, -3 );

	return 1;
}

