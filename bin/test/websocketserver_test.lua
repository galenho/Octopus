package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/lua-protobuf/?.lua"
package.path = package.path..";../common/lua-xml/?.lua"
package.path = package.path..";../config/?.lua"
package.path = package.path..";../proto/?.lua"

local common = require "common"
local protoc = require "protoc"
local serpent = require "serpent"


function GetCmd(bytes)
	return string.byte(bytes, 1) + string.byte(bytes, 2) * 256 + string.byte(bytes, 3) * 65535 + string.byte(bytes, 4) * 16777216
end

server = websocketserver.new()

local proto = protoc.new()
proto:loadfile("test.proto")

fun_connect = function(conn_idx, is_success)
	print("fun_connect----" .. conn_idx )
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	local cmd = GetCmd(data)
	dump(cmd)
	local msg = pb.decode_cmd("C2SReqClientLogin", data)
	dump(msg)

	-- 回复消息
	local rep_msg = {login_result = 1, account_idx = 1, account_name = "test1"}
	local rep_cmd_name = "S2CRepClientLogin"
	local rep_cmd = 2001
	local bytes = pb.encode_cmd(rep_cmd_name, rep_msg, rep_cmd)
	server:send_msg(conn_idx, bytes, #bytes)
end

server:start("127.0.0.1", 8810, fun_connect, fun_close, fun_recv, 8192, 8192)
