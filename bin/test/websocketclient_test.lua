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

client = websocketclient.new()

local proto = protoc.new()
proto:loadfile("test.proto")

fun_connect = function(conn_idx, is_success)
	print("fun_connect----" .. conn_idx )
	
	local data = {platform_idx = 1, account_name = "galenho", password = "123456"}
	local cmd_name = "C2SReqClientLogin"
	local cmd = 1001
	local bytes = pb.encode_cmd(cmd_name, data, cmd)
	client:send_msg(conn_idx, bytes, #bytes)
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	local cmd = GetCmd(data)
	dump(cmd)
	local msg = pb.decode_cmd("S2CRepClientLogin", data)
	dump(msg)
end

client:connect("127.0.0.1", 8810, fun_connect, fun_close, fun_recv, 8192, 8192)
