

fun_timer_callback = function(timer_id, param)
	print("timer_id" .. "----" .. timer_id)
	--octopus.remove_timer(timer_id)
end

octopus.add_timer(1000, fun_timer_callback, {a = 5, b = 6})