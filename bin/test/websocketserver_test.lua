package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/hotfix/?.lua"

local util = require "util"

server = websocketserver.new()

fun_connect = function(conn_idx, is_success)
	print("fun_connect----" .. conn_idx )
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	--print(data)
	server:send_msg(conn_idx, data, len)
end

server:start("127.0.0.1", 8810, fun_connect, fun_close, fun_recv, 8192, 8192)
