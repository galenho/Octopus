/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef LUA_FIX_H
#define LUA_FIX_H

#include "common.h"

#define TOLUA_REFID_FUNCTION_MAPPING "toluafix_refid_function_mapping"
#define TOLUA_REFID_PARAM_MAPPING "toluafix_refid_param_mapping"

void LuaCall(lua_State* L, int nargs);
void stack_dump(lua_State* L);

// 初始化函数
void toluafix_open(lua_State* L);

// 参数检查函数
void check_param(lua_State* L, int count, const char* s);

// 函数操作
int  generate_function_id();
int  toluafix_ref_function(lua_State* L, int lo);
void toluafix_get_function_by_refid(lua_State* L, int refid);
void toluafix_remove_function_by_refid(lua_State* L, int refid);

// 参数操作
int  generate_param_id();
int  toluafix_ref_param(lua_State* L, int lo);
void toluafix_get_param_by_refid(lua_State* L, int refid);
void toluafix_remove_param_by_refid(lua_State* L, int refid);

#endif //LUA_FIX_H