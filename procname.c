/*
* procname.c
* procname library for Lua 5.1
* Matthew Dombroski <matt@mattski.net.nz>
*
* Based on original for python
* http://code.google.com/p/procname/
*/

#include <string.h>

# if defined(__linux__)
#include <sys/prctl.h>
# endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <Windows.h>
#include <tchar.h>
#include <psapi.h>
#endif

#define PNAME		"procname"
#define PVERSION	PNAME " library for " LUA_VERSION " / October 2009"


#define LUA_BUILD_AS_DLL
#define LUA_LIB

#if defined(__cplusplus)
extern "C"
{
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#if defined(__cplusplus)
}
#endif


int Pgetname( lua_State* L )
{
# if defined(__linux__)
	char name[17] = {0};
	int err = prctl( PR_GET_NAME, name );
	lua_pushstring( L, name );
	return 1;
# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	DWORD pid = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );

	if( hProcess != NULL )
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
		}
	}

	CloseHandle( hProcess );

	lua_pushstring( L, szProcessName );
	return 1;
# else
	return luaL_error( L, "procname is not supported on this platform" );
# endif
}


int Psetname( lua_State* L )
{
# if defined(__linux__)
	const char* name = luaL_checkstring( L, 1 );
	int err = prctl( PR_SET_NAME, name );
	return 0;
# else
	return luaL_error( L, "procname.set is not supported on this platform" );
# endif
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

