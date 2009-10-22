require "process"

name = arg[0]
pid = process.getpid()

print( "get pid: "..pid )
print( "get pname: \""..process.getname().."\"" )
process.setname( name )
print( "set pname: \""..name.."\"" )
print( "get pname: \""..process.getname().."\"" )

print( "get pname (pid): \""..process.getname(pid).."\"" )

pid = process.fork()
if pid == 0 then
	print( "child: forked" )
	print( "child: exit" )
	os.exit( 0 )
end

print( "parent: fork child pid is "..pid..", name is: \""..process.getname(pid).."\"" )
print( "parent: wait for child" )

process.wait( pid )

print( "parent: child ended" )

process.exec( "echo", "exec(\"echo\") -- works ok" )

