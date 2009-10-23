/*
* process.c
* process library for Lua 5.1
* Matthew Dombroski <matt@mattski.net.nz>
*
* Based on original for python
* http://code.google.com/p/process/
*/

#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>

# if defined(__linux__)
#   include <sys/prctl.h>
#   include <sys/wait.h>
#   include <fcntl.h>
# endif

# if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   include <Windows.h>
#   include <tchar.h>
#   include <psapi.h>
#   include <process.h>
#   define LUA_BUILD_AS_DLL
#   define LUA_LIB
# endif

#define PNAME		"process"
#define PVERSION	PNAME " library for " LUA_VERSION " / October 2009"


#if defined(__cplusplus)
extern "C"
{
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#if defined(__cplusplus)
}
#endif


int Pgetpid( lua_State* L )
{
# if defined(__linux__)
	lua_pushnumber( L, getpid() );
# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	lua_pushnumber( L, GetCurrentProcessId() );
# endif

	return 1;
}


int Pgetname( lua_State* L )
{
# if defined(__linux__)
	int fd = 0;
	char buf[256] = {0};
	void *ptr = 0;
	pid_t pid = luaL_optinteger( L, 1, getpid() );

	snprintf( buf, sizeof(buf), "/proc/%d/status", pid );

	fd = open( buf, O_RDONLY );
	if( fd < 0 )
	{
		luaL_argerror( L, 1, "Invalid process ID" );
	}

	lseek( fd, strlen( "Name:\t" ), SEEK_SET );
	read( fd, buf, 256 );
	close( fd );

	ptr = memchr( buf, '\n', sizeof(buf) );
	if( ptr )
	{
		memset( ptr, 0x00, sizeof(buf)- ( (size_t)ptr - (size_t)buf ) );
	}

	lua_pushstring( L, buf );

# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	TCHAR szProcessName[MAX_PATH] = {0};
	DWORD pid = luaL_optinteger( L, 1, GetCurrentProcessId() );
	
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
	if( hProcess == NULL )
	{
		luaL_error( L, "Invalid process ID" );
	}
	else
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
		}
	
		CloseHandle( hProcess );
	}

	lua_pushstring( L, szProcessName );

# else
	return luaL_error( L, PNAME ".getname is not supported on this platform" );
# endif
	
	return 1;
}


int Psetname( lua_State* L )
{
# if defined(__linux__)
	const char* name = luaL_checkstring( L, 1 );
	if( prctl( PR_SET_NAME, name ) < 0 )
	{
		luaL_error( L, PNAME ".setname: %s", strerror( errno ) );
	}
# else
	return luaL_error( L, PNAME ".setname is not supported on this platform" );
# endif
	
	return 0;
}


char** helper_argv( lua_State* L, int istart, int argc )
{
	int i = 0;
	char** argv;

	for( i = 0; i < argc; ++i )
	{
		luaL_checkstring( L, i + istart );
	}

	argv = calloc( argc + 1, sizeof(char*) );

	for( i = 0; i < argc; ++i )
	{
		argv[i] = (char*) lua_tostring( L, i + istart );
	}
	
	argv[argc] = 0x00;
	return argv;
}


int Pexec( lua_State* L )
{
	int pid = 0;
	int err = 0;
	char** argv = NULL;
	int arg_start = 1;
	int arg_count = lua_gettop( L );
	
# if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	int mode = _P_OVERLAY;
# endif
	
	/* replace mode - first arg is false/nil */
	if( ! lua_toboolean( L, 1 ) )
	{
		arg_start = 2;
		arg_count--;
# if defined(__linux__)
		pid = fork();
# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		mode = _P_NOWAIT;
# endif
	}

	/* Only exec if pid == 0 (process is child or the mode was replace) */
	if( pid == 0 )
	{
		argv = helper_argv( L, arg_start, arg_count );
# if defined(__linux__)
		err = execvp( argv[0], argv );
# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		_spawnvp( mode, argv[0], argv );
# endif
		free( argv );
	}

	/* return a pid object */
	lua_pushnumber( L, pid );

	if( err < 0 )
	{
		luaL_error( L, PNAME ".exec: %s", strerror( errno ) );
	}

	return 1;
}


int Pfork( lua_State* L )
{
	pid_t pid = fork();
	
	if( pid < 0 )
	{
		return luaL_error( L, PNAME ".fork: %s", strerror( errno ) );
	}

	lua_pushnumber( L, pid );

	return 1;
}


int Pwait( lua_State* L )
{
	/* by default wait on *any* child process */
	pid_t pid = luaL_optinteger( L, 1, -1 );
	int status = 0;

	if( waitpid( pid, &status, 0 ) < 0 )
	{
		return luaL_error( L, PNAME ".wait: %s", strerror( errno ) );
	}

	lua_pushnumber( L, status );

	return 1;
}


static const luaL_reg R[] =
{
	{ "getpid",		Pgetpid },
	{ "getname",	Pgetname },
	{ "exec",		Pexec },
	{ "wait",		Pwait },
# if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
	{ "setname",	Psetname },
	{ "fork",		Pfork },
# endif
	{ NULL,			NULL }
};

LUALIB_API int luaopen_process (lua_State *L)
{
	luaL_register( L, PNAME, R );

	lua_pushliteral( L, "version" );
	lua_pushliteral( L, PVERSION );
	lua_settable( L, -3 );

	return 1;
}

