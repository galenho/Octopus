server = tcpserver.new()
local g_byte = ""
for i = 1, 1000 do
	g_byte = g_byte .. "我是一个人"
end
local g_len = #g_byte
print(g_len)

local g_conn_list = {}
local g_msg_count = 0

fun_connect = function(conn_idx, is_success)
	print("fun_connect----" .. conn_idx)
	g_conn_list[conn_idx] = 1

	for i = 1, 1000 do
		server:send_msg(conn_idx, g_byte, g_len)
	end
	--print("send finish")
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
	g_conn_list[conn_idx] = nil
end

fun_recv = function(conn_idx, data, len)
	local now_time = octopus.get_ms_time()
	g_msg_count = g_msg_count + 1
	print("fun_recv conn_idx = " .. conn_idx .. ", now_time = " .. now_time .. ", msg_count = " .. g_msg_count)
	--server:send_msg(conn_idx, data, len)
end

server:start("127.0.0.1", 30061, fun_connect, fun_close, fun_recv, 1024 * 1024 * 4, 1024 * 1024 * 4)
