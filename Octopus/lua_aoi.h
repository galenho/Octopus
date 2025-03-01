/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef LUA_AOI_H
#define LUA_AOI_H

#include "common.h"

struct UnitID
{
	UnitID()
	{
		type = 0;
		obj_idx = 0;
	}

	bool operator<(const UnitID& unit_idx) const
	{
		if (type < unit_idx.type)
		{
			return true;
		}
		else if (type == unit_idx.type)
		{
			if (obj_idx < unit_idx.obj_idx)
			{
				return true;
			}
		}

		return false;
	}

	bool operator==(const UnitID& unit_idx) const
	{
		if (unit_idx.type == type && unit_idx.obj_idx == obj_idx)
		{
			return true;
		}

		return false;
	}

	bool operator!=(const UnitID& unit_idx) const
	{
		if (unit_idx.type != type || unit_idx.obj_idx != obj_idx)
		{
			return true;
		}

		return false;
	}

	int32 type;
	int64 obj_idx;
};

// AOI����
struct AOIObject
{
	AOIObject()
	{
		x = 0;
		y = 0;
	}

	UnitID unit_idx;
	int32 x; //����X����
	int32 y; //����X����
};

// AOI����
struct AOICell
{
	AOICell()
	{
		id = 0;
		x = 0;
		y = 0;
	}

	~AOICell()
	{
		
	}

	int32 id;
	int32 x; //����X����
	int32 y; //����X����

	std::map<UnitID, AOIObject*> unit_map_;
};

class AOIManager
{
public:
	AOIManager();
	~AOIManager();

	// ����һ��AOI�ռ�
	bool Create(int32 width, int32 height, int32 range, HandleInfo enter_handler, HandleInfo move_handler, HandleInfo exit_handler, int64 scene_obj_idx);

	// ����AOI(x, yΪ��ҵ�2D����)
	bool Enter(UnitID& unit_idx, int32 x, int32 y);

	// �ƶ�(x, yΪ��ҵ�2D����)
	bool Move(UnitID& unit_idx, int32 x, int32 y);
	
	// �뿪AOI
	bool Exit(UnitID& unit_idx);

	// �õ�AOIObject
	AOIObject* GetAOIObject(UnitID& unit_idx);

	// �õ�����(x, yΪ���ӵ�����)
	AOICell* GetCell(int32 x, int32 y);

	// �õ��Ź���(x, yΪ���ӵ�����)
	void Get9Cell(int32 x, int32 y, std::map<int32, AOICell*>& cell_map);

	// �¼�
	void onEnterEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx);
	void onMoveEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx);
	void onExitEvent(std::map<int32, AOICell*>& cell_map, UnitID& unit_idx);
	
public:
	int32 width_;
	int32 height_;
	int32 range_;

	HandleInfo enter_handler_;
	HandleInfo move_handler_;
	HandleInfo exit_handler_;

	int64 scene_obj_idx_;

	std::map<UnitID, AOIObject*> unit_map_;

	int32 width_count_;
	int32 height_count_;

	std::vector<AOICell*> cell_list_; // (y * width_count_ + x)
};

//-------------------------------------------------------------------------------------
//					AIO�ӿ�
//-------------------------------------------------------------------------------------
int32 lua_aoi_new(lua_State* L);
int32 lua_aoi_create(lua_State* L);
int32 lua_aoi_enter(lua_State* L);
int32 lua_aoi_move(lua_State* L);
int32 lua_aoi_exit(lua_State* L);
int32 lua_aoi_get_9_cell(lua_State* L);
int32 lua_aoi_destroy(lua_State* L);

static const struct luaL_Reg lua_aoi_methods[] = {
	{ "create", lua_aoi_create },
	{ "enter", lua_aoi_enter },
	{ "move", lua_aoi_move },
	{ "exit", lua_aoi_exit },
	{ "get_9_cell", lua_aoi_get_9_cell },
	{ "__gc",  lua_aoi_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_aoi_functions[] = {
	{ "new", lua_aoi_new },
	{ NULL, NULL }
};

typedef struct
{
	AOIManager* aoi;
} aoi_t;

//-------------------------------------------------------------------------------------
//							luaopen�ӿ�
//-------------------------------------------------------------------------------------
int32 luaopen_lua_aoi(lua_State* L);

#endif //LUA_AOI_H