require "process"

function test( msg, fn )
	local b,s = pcall( fn )
	if b then
		print( msg.." --- pass" )
	else
		print( msg.." --- fail" )
	end
end

name = arg[0]
pid = 0

test( "Get PID", function() pid = process.getpid() end )

test( "Set name", function() process.setname( name ) end )
test( "Get name", function() process.getname() end )
test( "Get name (pid)", function() process.getname(pid) end )

test( "fork", function() pid = process.fork(); if pid == 0 then os.exit(0) end end )
test( "wait", function() process.wait( pid ) end )

if not pcall( function()
	if process.fork() == 0 then
		local s,msg = pcall( function() process.exec( "/bin/echo", "-e", "exec --- pass" ) end )
		print( "exec --- fail ("..tostring(msg)..")" )
		os.exit(0)
	end
end ) then
	print( "exec test requires fork" )
end

test( "exec (spawn)", function() process.exec( false, "true" ) end )

