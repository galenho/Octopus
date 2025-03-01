/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#include "scheduler.h"
#include "clog.h"
#include "tinyxml/tinyxml.h"

//-------------------------------------------------------------------------------------
//					公共接口
//-------------------------------------------------------------------------------------
int lua_get_octopus_version(lua_State* L);
int lua_set_console_handler(lua_State* L);
int lua_add_timer(lua_State* L);
int lua_remove_timer(lua_State* L);
int lua_stop_all_timer(lua_State* L);
int lua_set_log_level(lua_State* L);
int lua_get_log_level(lua_State* L);
int lua_save_log(lua_State* L);
int lua_write_log(lua_State* L);
int lua_stop(lua_State* L);
int lua_exit(lua_State* L);
int lua_loadxml(lua_State* L);
int lua_get_config_file(lua_State* L);
int lua_get_ms_time(lua_State* L);

static const struct luaL_Reg lua_common_functions[] = {
	{ "get_octopus_version", lua_get_octopus_version },
	{ "set_console_handler", lua_set_console_handler },
	{ "add_timer", lua_add_timer },
	{ "remove_timer", lua_remove_timer },
	{ "stop_all_timer", lua_stop_all_timer },
	{ "set_log_level", lua_set_log_level },
	{ "get_log_level", lua_get_log_level },
	{ "save_log", lua_save_log },
	{ "write_log", lua_write_log },
	{ "stop", lua_stop },
	{ "exit", lua_exit },
	{ "loadxml", lua_loadxml },
	{ "get_config_file", lua_get_config_file },
	{ "get_ms_time", lua_get_ms_time },
	{ NULL, NULL }
};

//-------------------------------------------------------------------------------------
//							luaopen接口
//-------------------------------------------------------------------------------------
int luaopen_lua_octopus(lua_State* L);