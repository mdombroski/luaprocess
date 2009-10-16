require "procname"

name = "test"
procname.set( name )

print( "Set process name: \""..name.."\"" )
print( "Get process name: \""..procname.get().."\"" )

if procname.get() == name then
	print( "pass" )
else
	print( "fail" )
end

