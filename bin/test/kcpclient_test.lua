client = kcpclient.new()

local last_time = octopus.get_ms_time()
local msg_count = 0

function fun_connect(conn_idx, is_success)
	if is_success then
		print("onconnect " .. conn_idx .. ", success")
		last_time = octopus.get_ms_time()
	else
		print("onconnect " .. conn_idx .. ", fail")
	end
end

function fun_close(conn_idx)
	print("onclose" .. conn_idx)
end

function fun_recv(conn_idx, data, len)
	msg_count = msg_count + 1
	if msg_count == 1 then
		last_time = octopus.get_ms_time()
	end

	local now_time = octopus.get_ms_time()
	local diff_time = now_time - last_time
	last_time = now_time

	print("onrecv conn_idx = " .. conn_idx .. ", diff_time = " .. diff_time .. ", msg_count = " .. msg_count)
end

function UpdateSocket(timer_id)
	octopus.update_socket()
end

for i=1, 1 do
    client:connect("127.0.0.1", 12345, fun_connect, fun_close, fun_recv, 1024 * 1024 * 4, 1024 * 1024 * 4) 
end

octopus.add_timer(10, UpdateSocket)