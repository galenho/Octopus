package.path = package.path..";../common/?.lua"
local common = require "common"

aoi = aoi.new()

function fun_enter_callback(src_list, dst_list, scene_obj_idx)
	print("fun_enter_callback")
	dump(src_list)
	dump(dst_list)
	dump(scene_obj_idx)
end

function fun_move_callback(src_list, dst_list, scene_obj_idx)
	print("fun_move_callback")
	dump(src_list)
	dump(dst_list)
	dump(scene_obj_idx)
end

function fun_exit_callback(src_list, dst_list, scene_obj_idx)
	print("fun_exit_callback")
	dump(src_list)
	dump(dst_list)
	dump(scene_obj_idx)
end

aoi:create(100, 100, 10, fun_enter_callback, fun_move_callback, fun_exit_callback, 101)

aoi:enter(1, 1, 1, 1)
print("-----------------------------------------")
aoi:enter(1, 2, 20, 20)
print("-----------------------------------------")
aoi:enter(1, 3, 40, 40)
print("-----------------------------------------")
aoi:move(1, 2, 10, 10)
print("-----------------------------------------")
local result = aoi:get_9_cell(2, 2)
dump(result)

aoi:exit(1, 1)
aoi:exit(1, 2)
aoi:exit(1, 3)