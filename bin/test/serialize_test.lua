package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/lua-protobuf/?.lua"
package.path = package.path..";../common/lua-xml/?.lua"
package.path = package.path..";../config/?.lua"
package.path = package.path..";../proto/?.lua"

local common = require "common"
local luaxml = require "luaxml"
local protoc = require "protoc"
local serpent = require "serpent"
local cjson = require "cjson"

-- (1)服务器各个节点交互		Lua Table ---->BinStream ----> Lua Table
function test1()
	local data = {a = 1, b = 2}
	local byte, len = seri.pack(data)
	local msg = seri.unpack(byte, len)
	dump(msg)
end

-- (2)客户端与服务器(protobuf)	protobuf ----> Lua Table ----> protobuf
function test2()
	local proto = protoc.new()
	proto:loadfile("test.proto")
	local data = {platform_idx = 1, account_name = "galenho", password = "123456"}
	local cmd_name = "C2SReqClientLogin"
	local cmd = 1001
	local bytes = pb.encode_cmd(cmd_name, data, cmd)
	local data2 = pb.decode_cmd("C2SReqClientLogin", bytes)
	dump(data2)
end

-- (3)客户端与服务器(json)		json ----> Lua Table ----> json
function test3()
	local json_object = "{\"name\":\"Jiang\",\"addr\":\"BeiJing\",\"age\":24,\"tel\":\"1569989xxxx\",\"email\":\"1569989xxxx@126.com\"}"
	print(json_object)
	local lua_object = cjson.decode(json_object)
	dump(lua_object)
	local json_object2 = cjson.encode(json_object)
	print(json_object2)
end

-- (4)读取xml配置文件			xml ----> Lua Table
function test4()
	local xml_file = "test.xml"
	local xml_config = luaxml.load(xml_file)
	dump(xml_config)

	local success, ip_emt = luaxml.find_node(xml_config, "ip")
	local ip = ip_emt[1]

	local success, port_emt = luaxml.find_node(xml_config, "port")
	local port = tonumber(port_emt[1])
end

-- (5)Mongo读写数据				Bson ----> Lua Table ----> Bson
	-- 示例见 bson_test.lua


test1()
print("-----------------------------------------------------------")
test2()
print("-----------------------------------------------------------")
test3()
print("-----------------------------------------------------------")
test4()