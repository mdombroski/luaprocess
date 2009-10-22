require "process"

name = arg[0]

print( "<<< process name: \""..process.getname().."\"" )
process.setname( name )
print( ">>> process name: \""..name.."\"" )
print( "<<< process name: \""..process.getname().."\"" )

pid = process.fork()
if pid == 0 then
	print( ">>> child: forked" )
	print( ">>> child: exit" )
	os.exit( 0 )
end

print( ">>> parent: fork child pid is "..pid )
print( ">>> parent: wait for child" )

process.wait( pid )

print( ">>> parent: child ended" )

process.exec( "echo", "exec(\"echo\") looks like it worked ok" )

