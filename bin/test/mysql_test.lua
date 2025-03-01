package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/hotfix/?.lua"

require("common")
local util = require("util")
db = mysql.new()
bind_conn_idx = 0

local begin_time = 0
local end_time = 0

fun_drop_callback = function(is_success, rs)
    if is_success then
		print("drop is success")
	end
end

function test_drop()
	sql = "DROP TABLE IF EXISTS `config_scene`;"
	db:add_query(sql, fun_drop_callback, {}, bind_conn_idx)
end

fun_create_callback = function(is_success, rs)
    if is_success then
		print("create is success")
	end
end

function test_create()
	sql = "CREATE TABLE `config_scene` (\
			`type_idx` int(10) NOT NULL,\
			`scene_name` char(36) NOT NULL,\
			`area_idx` int(10) NOT NULL,\
			`instance_rule_type` tinyint(3) unsigned NOT NULL,\
			`map_id` int(11) unsigned NOT NULL,\
			`map_instance_id` int(11) unsigned NOT NULL,\
			PRIMARY KEY (`type_idx`)\
			) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;"

	db:add_query(sql, fun_create_callback, {}, bind_conn_idx)
end

fun_query_callback = function(is_success, rs)
    if is_success then
		dump(rs)
	end
end

fun_query_callback_ex = function(is_success, rs, param)
    if is_success then
		dump(rs)
	end
end

function test_query()
	
	sql = "select scene_name, type_idx, map_id from config_scene where type_idx = ?"
	sql_param = {101}
	params = {a=1, b=2}
	tran_flag = 0

	db:add_query(sql, fun_query_callback, sql_param)
	db:add_query(sql, fun_query_callback_ex, sql_param, bind_conn_idx, params, tran_flag)
	
end

fun_insert_callback = function(is_success, rs, params)
    if is_success then
		--print("insert is success")
		if params.flag == 3 then
			end_time = octopus.get_ms_time()
			local use_time = end_time - begin_time
			print(use_time)
		end
	end
end

function test_insert()
	for i=1,100000,1 do
		local sql = "insert into config_scene values(?, ?, ?, ?, ?, ?)"
		local sql_param = {i, "kof", 1, 1, 1, 1}
		if i == 1 then
			begin_time = octopus.get_ms_time()
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 1}, 1)
		elseif i == 100000 then
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 3}, 3)
		else
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 2})
		end
	end
end

function table_merge(...)
	local result = {}
	for _, tbl in ipairs({...}) do
		for _, v in ipairs(tbl) do
			table.insert(result, v)
		end
	end
	return result
end

function test_batch_insert()
	for i=1,1000,1 do
		local sql = "insert into config_scene values"
		local sql_param = {}

		for j=1,100,1 do
			if j == 1 then
				sql = sql .. "(?, ?, ?, ?, ?, ?)"
			else
				sql = sql .. ",(?, ?, ?, ?, ?, ?)"	
			end

			local param = {(i-1)*100 + j, "kof", 1, 1, 1, 1}
			sql_param = table_merge(sql_param, param)
		end

		if i == 1 then
			begin_time = octopus.get_ms_time()
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 1}, 1)
		elseif i == 1000 then
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 3}, 3)
		else
			db:add_query(sql, fun_insert_callback, sql_param, bind_conn_idx, {flag = 2})
		end
	end
end

fun_query_callback = function(is_success, rs)
    if is_success then
		dump(rs)
	end
end

function test_query()
	
	sql = "select scene_name, type_idx, map_id from config_scene where type_idx = ?"
	sql_param = {101}
	bind_conn_idx = 0

	params = {a=1, b=2}
	tran_flag = 0

	db:add_query(sql, fun_query_callback, sql_param, bind_conn_idx)
	db:add_query(sql, fun_query_callback_ex, sql_param, bind_conn_idx, params, tran_flag)
	
end

function test_mysql()
	db:initialize("127.0.0.1", 3306, "root", "88104725galenho", "testdb", "utf8", 2)
	db:ping()	

	test_drop()
	test_create()
	--test_insert()
	test_batch_insert()

	test_query()
end

test_mysql()


