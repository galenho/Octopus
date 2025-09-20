server = udpserver.new()
local g_conn_list = {}

local g_byte = ""
for i = 1, 50 do
	g_byte = g_byte .. "我是一个人"
end
local g_len = #g_byte
print(g_len)

function fun_connect(conn_idx, is_success)
	print("onconnect" .. conn_idx)
	g_conn_list[conn_idx] = 1
end

function fun_close(conn_idx)
	print("onclose----" ..conn_idx)
	g_conn_list[conn_idx] = nil
end

function fun_recv(conn_idx, data, len)
	print("onrecv" .. conn_idx)
end

function fun_console_callback(cmd)
	if cmd == "send" then
		for key, value in pairs(g_conn_list) do
			local conn_idx = key
			for i = 1, 10 do
				server:send_msg(conn_idx, g_byte, g_len)
			end
		end
	end
end

function GameLoop(timer_id)
	for key, value in pairs(g_conn_list) do
		local conn_idx = key
		server:send_msg(conn_idx, g_byte, g_len)
	end
end

octopus.set_console_handler(fun_console_callback)
octopus.add_timer(66, GameLoop)
server:start("127.0.0.1", 12345, fun_connect, fun_close, fun_recv, 1024 * 1024 * 4, 1024 * 1024 * 4)
