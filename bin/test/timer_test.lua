package.path = package.path..";../common/?.lua"
local common = require "common"
local last_time = 0
local last_time2 = 0

function fun_timer_callback(timer_id, param)
	local now_time = octopus.get_ms_time()
	local diff_time = now_time - last_time
	last_time = now_time
	LOG_INFO("timer_id = %d, now_time = %d, diff_time = %d", timer_id, now_time, diff_time)
	--octopus.remove_timer(timer_id)

end

function fun_timer_callback2(timer_id, param)
	local now_time = octopus.get_ms_time()
	local diff_time = now_time - last_time2
	last_time2 = now_time
	LOG_INFO("timer_id = %d, now_time = %d, diff_time = %d", timer_id, now_time, diff_time)
	--octopus.remove_timer(timer_id)
end

function fun_console_callback(cmd)
	if cmd == "t" then
		last_time2 = octopus.get_ms_time()
		octopus.add_timer(1000, fun_timer_callback2, {a = 5, b = 6})
	end
end

last_time = octopus.get_ms_time()
octopus.set_console_handler(fun_console_callback)
octopus.add_timer(60000, fun_timer_callback, {a = 5, b = 6})