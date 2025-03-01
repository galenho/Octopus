package.path = package.path..";../common/?.lua"
local common = require "common"

function fun_stop_callback(timer_id)
	-- 这里写相关的逻辑，直到判定可以退出
	octopus.remove_timer(timer_id)
	octopus.exit()
end

function fun_console_callback(cmd)
	if cmd == "exit" then
		octopus.stop(fun_stop_callback)
	elseif cmd == "print" then
		LOG_DEBUG("LOG_DEBUG") 
    	LOG_INFO("LOG_INFO") 
	elseif cmd == "set log_level" then
		octopus.set_log_level(0)
		g_log_level = octopus.get_log_level()
	end
end

g_log_level = octopus.get_log_level()
octopus.set_console_handler(fun_console_callback)