client = httpclient.new()

fun_get_callback = function(is_success, data, use_time)
	print(data)
end

client:init()
client:set_timeout(5000)
client:get("http://127.0.0.1:30061/do?mapId=101&subline=1", fun_get_callback, "")