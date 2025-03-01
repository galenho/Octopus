package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/hotfix/?.lua"

require("common")
local util = require("util")

db = mongo.new()
local begin_time = 0
local end_time = 0

fun_callback = function(is_success, rs)
    if is_success then
		dump(rs)
	end
end

fun_callback_param = function(is_success, rs, param)
    if is_success then
		dump(rs)
	end
end

fun_batch_callback_param = function(is_success, rs, param)
    if is_success then
		if param.flag == 3 then
			end_time = octopus.get_ms_time()
			local use_time = end_time - begin_time
			print(use_time)
		end
	end
end

function TestInsert()
	local ret = db:initialize("mongodb://root:88104725galenho@127.0.0.1:27017/?authSource=admin", "testdb", 1)
	db:ping()
	db:drop("player", fun_callback)
	db:drop_index("player", "*", fun_callback)
	db:create_index("player", {obj_idx=1}, fun_callback)

	begin_time =  octopus.get_ms_time()
	for i = 1, 100000 do
		if i == 100000 then
			db:insert_one("player", {obj_idx=i, name="galen", hp=100}, fun_batch_callback_param, {flag = 3}, 0)
		else
			db:insert_one("player", {obj_idx=i, name="galen", hp=100}, fun_batch_callback_param, {flag = 1}, 0)
		end
	end
end
function TestBatchInsert()
	local ret = db:initialize("mongodb://root:88104725galenho@127.0.0.1:27017/?authSource=admin", "testdb", 1)
	db:ping()
	db:drop("player", fun_callback)
	db:drop_index("player", "*", fun_callback)
	db:create_index("player", {obj_idx=1}, fun_callback)

	begin_time =  octopus.get_ms_time()
	for i = 1, 1000 do
		local t = {}
		for j=1,100,1 do
			local t2 = {obj_idx = (i - 1) * 100 + j, name="chen", hp=100}
			t[j] = t2
		end

		if i == 1000 then
			db:insert_many("player", t, fun_batch_callback_param, {flag = 3}, 0)
		else
			db:insert_many("player", t, fun_batch_callback_param, {flag = 1}, 0)
		end
	end
end

function Test()
	local ret = db:initialize("mongodb://root:88104725galenho@127.0.0.1:27017/?authSource=admin", "testdb", 1)
	db:ping()
	db:drop("player", fun_callback)
	db:drop_index("player", "*", fun_callback)
	
	db:create_index("player", {obj_idx=1}, fun_callback)
	db:drop_index("player", "obj_idx_1", fun_callback)
	
	
	db:insert_one("player", {obj_idx=1, name="galen", hp=100}, fun_callback)
	db:insert_many("player", {{obj_idx=2, name="chen", hp=100}, {obj_idx=3, name="flashboy", hp=100}}, fun_callback)
	db:insert_many("player", {{obj_idx=3, name="gui", hp=100}, {obj_idx=5, name="flashboy", hp=100}}, fun_callback)
	db:insert_many("player", {{obj_idx=4, name="hong", hp=100}, {obj_idx=5, name="flashboy", hp=100}}, fun_callback)
	
	db:delete_one("player", {obj_idx=1}, fun_callback)
	db:delete_many("player", {{obj_idx=4}, {obj_idx=5}}, fun_callback)
		
	db:find("player", {}, {}, fun_callback)
	db:find("player", {obj_idx=1}, {obj_idx=true, name=true}, fun_callback)
	db:find("player", {obj_idx=2}, {}, fun_callback_param, {a=1, b=2}, 0)
	
	db:find_one("player", {pid=10002}, {}, fun_callback)
	
	db:update_one("player", {pid=10002}, {["$set"] = {hp22 = 388}}, fun_callback)
	
	db:find_one("player", {pid=10002}, {hp22 = true}, fun_callback)
	db:insert_one("player", {pid=10000000000033332}, fun_callback)
	db:find("player", {}, {pid = true}, fun_callback)
	
	
	db:update_many("player", {hp = 100}, {["$set"] = {name = "kof"}}, fun_callback)
	db:find("player", {obj_idx=1}, {}, fun_callback)
end

Test()
--TestInsert()
--TestBatchInsert()


