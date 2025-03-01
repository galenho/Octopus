client = tcpclient.new()
local g_msg_count = 0

local g_byte = ""
for i = 1, 4000 do
	g_byte = g_byte .. "我是一个人"
end
local g_len = #g_byte
print(g_len)

fun_connect = function(conn_idx, is_success)
	if is_success then
		print("fun_connect---- success, " .. conn_idx)
	else
		print("fun_connect---- fail, " .. conn_idx)
	end

    if is_success then
		client:send_msg(conn_idx, g_byte, g_len)
   end

end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	--print("fun_recv----" ..conn_idx)

	g_msg_count = g_msg_count + 1
	if g_msg_count % 1000 == 0 then
		print(g_msg_count)
	end
    client:send_msg(conn_idx, data, len)

end

client:connect("127.0.0.1", 30061, fun_connect, fun_close, fun_recv, 4096 * 10, 4096 * 10) 	
