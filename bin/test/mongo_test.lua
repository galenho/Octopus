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
	db:create_index("player", {pid=1}, fun_callback)

	begin_time =  octopus.get_ms_time()
	for i = 1, 100000 do
		if i == 100000 then
			db:insert_one("player", {pid=i, name="galen", hp=100}, fun_batch_callback_param, {flag = 3}, 0)
		else
			db:insert_one("player", {pid=i, name="galen", hp=100}, fun_batch_callback_param, {flag = 1}, 0)
		end
	end
end

function TestInsertBatch()
	local ret = db:initialize("mongodb://root:88104725galenho@127.0.0.1:27017/?authSource=admin", "testdb", 1)
	db:ping()
	db:drop("player", fun_callback)
	db:drop_index("player", "*", fun_callback)
	db:create_index("player", {pid=1}, fun_callback)

	begin_time =  octopus.get_ms_time()
	for i = 1, 1000 do
		local t = {}
		for j=1,100,1 do
			local t2 = {pid = (i - 1) * 100 + j, name="chen", hp=100}
			t[j] = t2
		end

		if i == 1000 then
			db:insert_batch("player", t, fun_batch_callback_param, {flag = 3}, 0)
		else
			db:insert_batch("player", t, fun_batch_callback_param, {flag = 1}, 0)
		end
	end
end

function Test()
	local ret = db:initialize("mongodb://root:88104725galenho@127.0.0.1:27017/?authSource=admin", "testdb", 1)
	db:ping()

	-- 删除表数据
	db:drop("player", fun_callback)
	db:drop("public_data", fun_callback)

	-- 删除索引
	db:drop_index("player", "*", fun_callback)
	
	-- 创建索引
	db:create_index("player", {pid=1}, fun_callback)
	--db:drop_index("player", "pid_1", fun_callback)

	-- 插入操作
	db:insert_one("player", {pid=1, name="galen", hp=100}, fun_callback)
	db:insert_batch("player", {{pid=2, name="chen", hp=100}, {pid=3, name="gui", hp=100}, {pid=4, name="hong", hp=100}}, fun_callback)

	-- 查询操作
	db:find("player", {}, {}, fun_callback)
	db:find("player", {hp=100}, {pid=true, name=true}, fun_callback)
	db:find_one("player", {pid=1}, {}, fun_callback)

	-- 更新操作
	db:update_one("player", {pid = 1}, {["$set"] = {hp = 388}}, fun_callback)
	db:update_many("player", {hp = 100}, {["$set"] = {hp = 200}}, fun_callback)
	db:update_batch("player", {{filter = {pid=1}, update = {["$set"] = {hp = 1}}}, {filter = {pid=2}, update = {["$set"] = {hp = 2}}}}, fun_callback)
	
	-- 更新操作2，如果集合不存在这个文档，则新建一个文档，一般用于没有预先insert一个文档,
	-- 如: public_data, friend表的key, data, 注：这种不能用于文档或子文档的部分数据更新
	db:update_one_upsert("public_data", {key=1}, {["$set"] = {data = 5}}, fun_callback)

	-- 查找并修改
	db:find_and_modify("public_data", {key=1}, {["$inc"] = {data = 1}}, fun_callback)

	-- 删除操作
	db:delete_one("player", {pid=1}, fun_callback)
	db:delete_many("player", {hp=100}, fun_callback)
	db:delete_batch("player", {{pid=2}, {pid=3}}, fun_callback)
end

local case = 1
if case == 1 then
	Test() -- 基本操作
elseif case == 2 then
	TestInsert() -- 批量操作1 (11222ms)
elseif case == 3 then
	TestInsertBatch() -- 批量操作(820ms)
else

end







