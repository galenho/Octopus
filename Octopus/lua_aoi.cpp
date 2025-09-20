#include "lua_aoi.h"
#include "lua_fix.h"


AOIManager::AOIManager()
{
	width_ = 0;
	height_ = 0;
	range_ = 0;

	width_count_ = 0;
	height_count_ = 0;
}

AOIManager::~AOIManager()
{
	for (int i = 0; i < (int)cell_list_.size(); i++)
	{
		AOICell* cell = cell_list_[i];
		delete cell;
		cell = NULL;
	}
	
	cell_list_.clear();

	for (std::map<UnitID, AOIObject*>::iterator it = unit_map_.begin(); it != unit_map_.end(); it++)
	{
		AOIObject* aoi_obj = it->second;
		delete aoi_obj;
		aoi_obj = NULL;
	}

	unit_map_.clear();


	if (enter_handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, enter_handler_.fun_id);
	}

	if (move_handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, move_handler_.fun_id);
	}

	if (exit_handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, exit_handler_.fun_id);
	}
}

bool AOIManager::Create(int32 width, int32 height, int32 range, HandleInfo enter_handler, HandleInfo move_handler, HandleInfo exit_handler, int64 scene_obj_idx)
{
	width_ = width;
	height_ = height;
	range_ = range;

	enter_handler_ = enter_handler;
	move_handler_ = move_handler;
	exit_handler_ = exit_handler;

	scene_obj_idx_ = scene_obj_idx;

	width_count_ = (width + range - 1) / range;
	height_count_ = (height + range - 1) / range;

	cell_list_.resize(width_count_ * height_count_);

	int32 id = 0;
	for (int32 x = 0; x < width_count_; x++)
	{
		for (int32 y = 0; y < height_count_; y++)
		{
			AOICell* cell = new AOICell();
			cell->id = id;
			cell->x = x;
			cell->y = y;
			cell_list_[y * width_count_ + x] = cell;

			id++;
		}
	}

	return true;
}

bool AOIManager::Enter(UnitID& unit_idx, int32 x, int32 y)
{
	x = x / range_;
	y = y / range_;

	std::map<UnitID, AOIObject*>::iterator it = unit_map_.find(unit_idx);
	if (it != unit_map_.end())
	{
		return false;
	}

	AOIObject* aoi_obj = new AOIObject();
	aoi_obj->unit_idx = unit_idx;
	aoi_obj->x = x;
	aoi_obj->y = y;

	AOICell* cell = GetCell(x, y);
	if (!cell)
	{
		return false;
	}
	
	cell->unit_map_[unit_idx] = aoi_obj;
	unit_map_[unit_idx] = aoi_obj;

	std::map<int32, AOICell*> cell_map;
	Get9Cell(cell->x, cell->y, cell_map);
	onEnterEvent(cell_map, unit_idx);
	
	return true;
}

bool AOIManager::Move(UnitID& unit_idx, int32 x, int32 y)
{
	x = x / range_;
	y = y / range_;

	std::map<UnitID, AOIObject*>::iterator it = unit_map_.find(unit_idx);
	if (it == unit_map_.end())
	{
		return false;
	}

	AOIObject* aoi_obj = it->second;
	int32 old_x = aoi_obj->x;
	int32 old_y = aoi_obj->y;

	AOICell* old_cell = GetCell(old_x, old_y);
	AOICell* new_cell = GetCell(x, y);
	if (!old_cell || !new_cell)
	{
		return false;
	}

	aoi_obj->x = x;
	aoi_obj->y = y;

	//------------------------------------------------------
	// 算出enter_cells, exit_cells, move_cells三个集合
	//------------------------------------------------------
	std::map<int32, AOICell*> enter_cells;
	std::map<int32, AOICell*> exit_cells;
	std::map<int32, AOICell*> move_cells;

	std::map<int32, AOICell*> old_cell_map;
	Get9Cell(old_cell->x, old_cell->y, old_cell_map);

	std::map<int32, AOICell*> new_cell_map;
	Get9Cell(new_cell->x, new_cell->y, new_cell_map);

	int new_min_x = x - 1;
	int new_max_x = x + 1;
	int new_min_y = y - 1;
	int new_max_y = y + 1;

	for (std::map<int32, AOICell*>::iterator it = old_cell_map.begin(); it != old_cell_map.end(); it++)
	{
		AOICell* old_cell = it->second;

		/*
		if (new_cell_map.find(old_cell->id) != new_cell_map.end())
		{
			move_cells[old_cell->id] = old_cell;
		}
		else
		{
			exit_cells[old_cell->id] = old_cell;
		}
		*/

		if ((old_cell->x >= new_min_x && old_cell->x <= new_max_x) && (old_cell->y >= new_min_y && old_cell->y <= new_max_y))
		{
			move_cells[old_cell->id] = old_cell;
		}
		else
		{
			exit_cells[old_cell->id] = old_cell;
		}
	}

	int old_min_x = old_x - 1;
	int old_max_x = old_x + 1;
	int old_min_y = old_y - 1;
	int old_max_y = old_y + 1;

	for (std::map<int32, AOICell*>::iterator it = new_cell_map.begin(); it != new_cell_map.end(); it++)
	{
		AOICell* new_cell = it->second;

		/*
		if (old_cell_map.find(new_cell->id) != old_cell_map.end())
		{
			move_cells[new_cell->id] = new_cell;
		}
		else
		{
			enter_cells[new_cell->id] = new_cell;
		}
		*/
		
		if ((new_cell->x >= old_min_x && new_cell->x <= old_max_x) && (new_cell->y >= old_min_y && new_cell->y <= old_max_y)) //交集
		{
			move_cells[new_cell->id] = new_cell;
		}
		else
		{
			enter_cells[new_cell->id] = new_cell;
		}
	}

	onEnterEvent(enter_cells, unit_idx);
	onMoveEvent(move_cells, unit_idx);
	onExitEvent(exit_cells, unit_idx);

	if (old_cell->id != new_cell->id)
	{
		old_cell->unit_map_.erase(unit_idx);
		new_cell->unit_map_[unit_idx] = aoi_obj;
	}
	
	return true;
}

bool AOIManager::Exit(UnitID& unit_idx)
{
	std::map<UnitID, AOIObject*>::iterator it = unit_map_.find(unit_idx);
	if (it == unit_map_.end())
	{
		return false;
	}

	AOIObject* aoi_obj = it->second;
	AOICell* cell = GetCell(aoi_obj->x, aoi_obj->y);
	if (!cell)
	{
		return false;
	}

	std::map<int32, AOICell*> cell_map;
	Get9Cell(cell->x, cell->y, cell_map);
	onExitEvent(cell_map, unit_idx);

	cell->unit_map_.erase(unit_idx);
	unit_map_.erase(unit_idx);
	
	delete aoi_obj;
	aoi_obj = NULL;

	return true;
}

AOIObject* AOIManager::GetAOIObject(UnitID& unit_idx)
{
	std::map<UnitID, AOIObject*>::iterator it = unit_map_.find(unit_idx);
	if (it != unit_map_.end())
	{
		return it->second;
	}

	return NULL;
}

AOICell* AOIManager::GetCell(int32 x, int32 y)
{
	int32 cursor = y * width_count_ + x;
	if (cursor >= (int32)cell_list_.size())
	{
		return NULL;
	}

	return cell_list_[cursor];
}

void AOIManager::Get9Cell(int32 x, int32 y, std::map<int32, AOICell*>& cell_map)
{
	// 中
	if (x >= 0 && y >= 0)
	{
		AOICell* cell = GetCell(x, y);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 左上
	if (x - 1 >= 0 && y - 1 >= 0)
	{
		AOICell* cell = GetCell(x - 1, y - 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 中上
	if (x >= 0 && y - 1 >= 0)
	{
		AOICell* cell = GetCell(x, y - 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 右上
	if (x + 1 < width_count_ && y - 1 >= 0)
	{
		AOICell* cell = GetCell(x + 1, y - 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 右
	if (x + 1 < width_count_ && y >= 0)
	{
		AOICell* cell = GetCell(x + 1, y);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 右下
	if (x + 1 < width_count_ && y + 1 < height_count_)
	{
		AOICell* cell = GetCell(x + 1, y + 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 下
	if (x >= 0 && y + 1 < height_count_)
	{
		AOICell* cell = GetCell(x, y + 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 左下
	if (x - 1 >= 0 && y + 1 < height_count_)
	{
		AOICell* cell = GetCell(x - 1, y + 1);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}

	// 左
	if (x - 1 >= 0 && y >= 0)
	{
		AOICell* cell = GetCell(x - 1, y);
		if (cell)
		{
			cell_map[cell->id] = cell;
		}
	}
}

void AOIManager::onEnterEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx)
{
	std::vector<AOIObject*> other_obj_list;
	for (std::map<int32, AOICell*>::iterator it = cell_map.begin(); it != cell_map.end(); it++)
	{
		AOICell* cell = it->second;

		for (std::map<UnitID, AOIObject*>::iterator it_sub = cell->unit_map_.begin(); it_sub != cell->unit_map_.end(); it_sub++)
		{
			AOIObject* obj = it_sub->second;
			if (obj->unit_idx != unit_idx)
			{
				other_obj_list.push_back(obj);
			}
		}
	}

	if (other_obj_list.size() == 0)
	{
		return;
	}

	lua_State* L = g_lua_state;
	
	//-----------------------------------------------------------------
	// (1)第一次回调
	//-----------------------------------------------------------------
	toluafix_get_function_by_refid(L, enter_handler_.fun_id);
	//-------------------------
	// 参数1:src
	//-------------------------
	lua_newtable(L);
	
	lua_pushnumber(L, 1);

	lua_newtable(L); // table ---- unit_idx begin

	lua_pushstring(L, "type");
	lua_pushinteger(L, unit_idx.type);
	lua_settable(L, -3);

	lua_pushstring(L, "obj_idx");
	lua_pushnumber(L, unit_idx.obj_idx);
	lua_settable(L, -3);

	lua_settable(L, -3); // table ---- unit_idx end

	//-------------------------
	// 参数2:dst
	//-------------------------
	lua_newtable(L);

	for (int32 i = 0; i < (int32)other_obj_list.size(); i++)
	{
		lua_pushnumber(L, i+1);

		int32 type = other_obj_list[i]->unit_idx.type;
		int64 obj_idx = other_obj_list[i]->unit_idx.obj_idx;

		lua_newtable(L); // table ---- unit_idx begin

		lua_pushstring(L, "type");
		lua_pushinteger(L, type);
		lua_settable(L, -3);

		lua_pushstring(L, "obj_idx");
		lua_pushnumber(L, obj_idx);
		lua_settable(L, -3);

		lua_settable(L, -3); // table ---- unit_idx end
	}

	//-------------------------
	// 参数3:scene_obj_idx
	//-------------------------
	lua_pushnumber(L, scene_obj_idx_);

	// 执行回调
	LuaCall(L, 3);

	//-----------------------------------------------------------------
	// (2)第二次回调
	//-----------------------------------------------------------------
	toluafix_get_function_by_refid(L, enter_handler_.fun_id);
	//stack_dump(L);
	//-------------------------
	// 参数1:src
	//-------------------------
	lua_newtable(L);

	for (int32 i = 0; i < (int32)other_obj_list.size(); i++)
	{
		lua_pushnumber(L, i + 1);

		int32 type = other_obj_list[i]->unit_idx.type;
		int64 obj_idx = other_obj_list[i]->unit_idx.obj_idx;

		lua_newtable(L); // table ---- unit_idx begin

		lua_pushstring(L, "type");
		lua_pushinteger(L, type);
		lua_settable(L, -3);

		lua_pushstring(L, "obj_idx");
		lua_pushnumber(L, obj_idx);
		lua_settable(L, -3);

		lua_settable(L, -3); // table ---- unit_idx end
	}

	//-------------------------
	// 参数2:dst
	//-------------------------
	lua_newtable(L);

	lua_pushnumber(L, 1);

	lua_newtable(L); // table ---- unit_idx begin

	lua_pushstring(L, "type");
	lua_pushinteger(L, unit_idx.type);
	lua_settable(L, -3);

	lua_pushstring(L, "obj_idx");
	lua_pushnumber(L, unit_idx.obj_idx);
	lua_settable(L, -3);

	lua_settable(L, -3); // table ---- unit_idx end

	//-------------------------
	// 参数3:scene_obj_idx
	//-------------------------
	lua_pushnumber(L, scene_obj_idx_);

	// 执行回调
	LuaCall(L, 3);
}

void AOIManager::onMoveEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx)
{
	std::vector<AOIObject*> other_obj_list;
	for (std::map<int32, AOICell*>::iterator it = cell_map.begin(); it != cell_map.end(); it++)
	{
		AOICell* cell = it->second;

		for (std::map<UnitID, AOIObject*>::iterator it_sub = cell->unit_map_.begin(); it_sub != cell->unit_map_.end(); it_sub++)
		{
			AOIObject* obj = it_sub->second;
			if (obj->unit_idx != unit_idx)
			{
				other_obj_list.push_back(obj);
			}
		}
	}

	if (other_obj_list.size() == 0)
	{
		return;
	}


	lua_State* L = g_lua_state;

	toluafix_get_function_by_refid(L, move_handler_.fun_id);

	//---------------------------------------------------------------------------
	// 参数1:src
	//---------------------------------------------------------------------------
	lua_newtable(L);

	lua_pushnumber(L, 1);

	lua_newtable(L); // table ---- unit_idx begin

	lua_pushstring(L, "type");
	lua_pushinteger(L, unit_idx.type);
	lua_settable(L, -3);

	lua_pushstring(L, "obj_idx");
	lua_pushnumber(L, unit_idx.obj_idx);
	lua_settable(L, -3);

	lua_settable(L, -3); // table ---- unit_idx end

	//---------------------------------------------------------------------------
	// 参数2:dst
	//---------------------------------------------------------------------------
	lua_newtable(L);

	for (int32 i = 0; i < (int32)other_obj_list.size(); i++)
	{
		lua_pushnumber(L, i + 1);

		int32 type = other_obj_list[i]->unit_idx.type;
		int64 obj_idx = other_obj_list[i]->unit_idx.obj_idx;

		lua_newtable(L); // table ---- unit_idx begin

		lua_pushstring(L, "type");
		lua_pushinteger(L, type);
		lua_settable(L, -3);

		lua_pushstring(L, "obj_idx");
		lua_pushnumber(L, obj_idx);
		lua_settable(L, -3);

		lua_settable(L, -3); // table ---- unit_idx end
	}

	//-------------------------
	// 参数3:scene_obj_idx
	//-------------------------
	lua_pushnumber(L, scene_obj_idx_);

	// 执行回调
	LuaCall(L, 3);
}

void AOIManager::onExitEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx)
{
	std::vector<AOIObject*> other_obj_list;
	for (std::map<int32, AOICell*>::iterator it = cell_map.begin(); it != cell_map.end(); it++)
	{
		AOICell* cell = it->second;

		for (std::map<UnitID, AOIObject*>::iterator it_sub = cell->unit_map_.begin(); it_sub != cell->unit_map_.end(); it_sub++)
		{
			AOIObject* obj = it_sub->second;
			if (obj->unit_idx != unit_idx)
			{
				other_obj_list.push_back(obj);
			}
		}
	}

	if (other_obj_list.size() == 0)
	{
		return;
	}

	lua_State* L = g_lua_state;

	//-----------------------------------------------------------------
	// (1)第一次回调
	//-----------------------------------------------------------------
	toluafix_get_function_by_refid(L, exit_handler_.fun_id);
	//-------------------------
	// 参数1:src
	//-------------------------
	lua_newtable(L);

	lua_pushnumber(L, 1);

	lua_newtable(L); // table ---- unit_idx begin

	lua_pushstring(L, "type");
	lua_pushinteger(L, unit_idx.type);
	lua_settable(L, -3);

	lua_pushstring(L, "obj_idx");
	lua_pushnumber(L, unit_idx.obj_idx);
	lua_settable(L, -3);

	lua_settable(L, -3); // table ---- unit_idx end

	//-------------------------
	// 参数2:dst
	//-------------------------
	lua_newtable(L);

	for (int32 i = 0; i < (int32)other_obj_list.size(); i++)
	{
		lua_pushnumber(L, i + 1);

		int32 type = other_obj_list[i]->unit_idx.type;
		int64 obj_idx = other_obj_list[i]->unit_idx.obj_idx;

		lua_newtable(L); // table ---- unit_idx begin

		lua_pushstring(L, "type");
		lua_pushinteger(L, type);
		lua_settable(L, -3);

		lua_pushstring(L, "obj_idx");
		lua_pushnumber(L, obj_idx);
		lua_settable(L, -3);

		lua_settable(L, -3); // table ---- unit_idx end
	}

	//-------------------------
	// 参数3:scene_obj_idx
	//-------------------------
	lua_pushnumber(L, scene_obj_idx_);

	// 执行回调
	LuaCall(L, 3);

	//-----------------------------------------------------------------
	// (2)第二次回调
	//-----------------------------------------------------------------
	toluafix_get_function_by_refid(L, exit_handler_.fun_id);
	//stack_dump(L);
	//-------------------------
	// 参数1:src
	//-------------------------
	lua_newtable(L);

	for (int32 i = 0; i < (int32)other_obj_list.size(); i++)
	{
		lua_pushnumber(L, i + 1);

		int32 type = other_obj_list[i]->unit_idx.type;
		int64 obj_idx = other_obj_list[i]->unit_idx.obj_idx;

		lua_newtable(L); // table ---- unit_idx begin

		lua_pushstring(L, "type");
		lua_pushinteger(L, type);
		lua_settable(L, -3);

		lua_pushstring(L, "obj_idx");
		lua_pushnumber(L, obj_idx);
		lua_settable(L, -3);

		lua_settable(L, -3); // table ---- unit_idx end
	}

	//-------------------------
	// 参数2:dst
	//-------------------------
	lua_newtable(L);

	lua_pushnumber(L, 1);

	lua_newtable(L); // table ---- unit_idx begin

	lua_pushstring(L, "type");
	lua_pushinteger(L, unit_idx.type);
	lua_settable(L, -3);

	lua_pushstring(L, "obj_idx");
	lua_pushnumber(L, unit_idx.obj_idx);
	lua_settable(L, -3);

	lua_settable(L, -3); // table ---- unit_idx end

	//-------------------------
	// 参数3:scene_obj_idx
	//-------------------------
	lua_pushnumber(L, scene_obj_idx_);

	// 执行回调
	LuaCall(L, 3);
}

int32 lua_aoi_new(lua_State* L)
{
	aoi_t* t = (aoi_t*)lua_newuserdata(L, sizeof(*t));
	t->aoi = new AOIManager();

	luaL_getmetatable(L, "aoi");
	lua_setmetatable(L, -2);

	return 1;
}

int32 lua_aoi_create(lua_State* L)
{
	check_param(L, 8, "unnnfffn");
	
	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");

	int32 width = (int32)lua_tointeger(L, 2);
	int32 height = (int32)lua_tointeger(L, 3);
	int32 range = (int32)lua_tointeger(L, 4);

	HandleInfo enter_handler;
	enter_handler.fun_id = (int32)toluafix_ref_function(L, 5);

	HandleInfo move_handler;
	move_handler.fun_id = (int32)toluafix_ref_function(L, 6);

	HandleInfo exit_handler;
	exit_handler.fun_id = (int32)toluafix_ref_function(L, 7);

	int64 scene_obj_idx = (int64)lua_tointeger(L, 8);

	bool ret = t->aoi->Create(width, height, range, enter_handler, move_handler, exit_handler, scene_obj_idx);
	lua_pushboolean(L, ret);
	
	return 1;
}

int32 lua_aoi_enter(lua_State* L)
{
	check_param(L, 5, "unnnn");

	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");

	int32 type = (int32)lua_tointeger(L, 2);
	int64 obj_idx = (int64)lua_tointeger(L, 3);
	int32 x = (int32)lua_tointeger(L, 4);
	int32 y = (int32)lua_tointeger(L, 5);

	UnitID unit_idx;
	unit_idx.type = type;
	unit_idx.obj_idx = obj_idx;

	t->aoi->Enter(unit_idx, x, y);

	return 0;
}

int32 lua_aoi_move(lua_State* L)
{
	check_param(L, 5, "unnnn");

	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");

	int32 type = (int32)lua_tointeger(L, 2);
	int64 obj_idx = (int64)lua_tointeger(L, 3);
	int32 x = (int32)lua_tointeger(L, 4);
	int32 y = (int32)lua_tointeger(L, 5);

	UnitID unit_idx;
	unit_idx.type = type;
	unit_idx.obj_idx = obj_idx;

	t->aoi->Move(unit_idx, x, y);

	return 0;
}

int32 lua_aoi_exit(lua_State* L)
{
	check_param(L, 3, "unn");

	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");

	int32 type = (int32)lua_tointeger(L, 2);
	int64 obj_idx = (int64)lua_tointeger(L, 3);

	UnitID unit_idx;
	unit_idx.type = type;
	unit_idx.obj_idx = obj_idx;

	t->aoi->Exit(unit_idx);

	return 0;
}

int32 lua_aoi_get_9_cell(lua_State* L)
{
	check_param(L, 3, "unn");

	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");

	int32 x = (int32)lua_tointeger(L, 2);
	int32 y = (int32)lua_tointeger(L, 3);

	x = x / t->aoi->range_;
	y = y / t->aoi->range_;

	std::map<int32, AOICell*> cell_map;
	t->aoi->Get9Cell(x, y, cell_map);

	lua_newtable(L);

	int count = 0;
	for (std::map<int32, AOICell*>::iterator it = cell_map.begin(); it != cell_map.end(); it++)
	{
		AOICell* cell = it->second;
		
		for (std::map<UnitID, AOIObject*>::iterator it_sub = cell->unit_map_.begin(); it_sub != cell->unit_map_.end(); it_sub++)
		{
			AOIObject* obj = it_sub->second;

			count++;
			lua_pushnumber(L, count);

			int32 type = obj->unit_idx.type;
			int64 obj_idx = obj->unit_idx.obj_idx;

			lua_newtable(L); // table ---- unit_idx begin

			lua_pushstring(L, "type");
			lua_pushinteger(L, type);
			lua_settable(L, -3);

			lua_pushstring(L, "obj_idx");
			lua_pushnumber(L, obj_idx);
			lua_settable(L, -3);

			lua_settable(L, -3); // table ---- unit_idx end
		}
	}

	return 1;
}

int32 lua_aoi_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	aoi_t* t = (aoi_t*)luaL_checkudata(L, 1, "aoi");
	if (t->aoi)
	{
		delete t->aoi;
		t->aoi = NULL;
	}

	return 0;
}

int32 luaopen_lua_aoi(lua_State* L)
{
	luaL_newmetatable(L, "aoi");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_aoi_methods, 0);
	luaL_newlib(L, lua_aoi_functions);

	return 1;
}