client = kcpclient.new()
msg_count = 0

fun_connect = function(conn_idx, is_success)
	print("onconnect" .. conn_idx)
    if is_success then    
		local byte = "我是一个人"
		local len = #byte
        client:send_msg(conn_idx, byte, len)
    end
end

fun_close = function(conn_idx)
	print("onclose" .. conn_idx)
end

fun_recv = function(conn_idx, data, len)
	--print("onrecv" .. conn_idx)
	msg_count = msg_count + 1
	if msg_count % 1000 == 0 then
		print(msg_count)
	end
    client:send_msg(conn_idx, data, len)
end

for i=1, 1, 1 do
    client:connect("127.0.0.1", 12345, fun_connect, fun_close, fun_recv, 8192, 8192) 
end