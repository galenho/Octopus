package.path = package.path..";../common/hotfix/?.lua"
package.cpath = package.cpath..";../?.dll"

test = require("hotfix_module")
hotfix = require("hotfix")


function fun_console_callback(cmd)
	octopus.write_log(cmd, 1)
	octopus.save_log()
	
	if cmd == "r" then
		hotfix.hotfix_module("hotfix_module")
		test.func()
		
	end
end


test.func()

octopus.set_console_handler(fun_console_callback)