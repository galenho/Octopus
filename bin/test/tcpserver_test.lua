server = tcpserver.new()

fun_connect = function(conn_idx, is_success)
	print("fun_connect----" .. conn_idx)
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	--print("fun_recv----" ..conn_idx)
	server:send_msg(conn_idx, data, len)
end


server:start("127.0.0.1", 30061, fun_connect, fun_close, fun_recv, 4096 * 10, 4096 * 10)
