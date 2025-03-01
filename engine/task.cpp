#include "common.h"
#include "task.h"
#include "lua_fix.h"
#include "lua_bson.h"
#include "bson.h"
#include "scheduler.h"

//------------------------------------------------------------------------------------------------------
// 下面是直接由WorkThread线程直接调用(不跨线程)
//------------------------------------------------------------------------------------------------------
void TimerTask::Process(HandleInfo handler, uint32 index)
{
	if (handler.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler.fun_id);
		lua_pushinteger(g_lua_state, index);

		if (handler.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handler.param_id);
			LuaCall(g_lua_state, 2);
		}
		else
		{
			LuaCall(g_lua_state, 1);
		}
	}
}

void TimerTask::DeleteTimer(HandleInfo handler, uint32 index)
{
	if (handler.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handler.fun_id);
	}

	if (handler.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handler.param_id);
	}
}

void SocketConnectTask::Process(HandleInfo connect_handler, int32 conn_idx, bool is_success)
{
	if (connect_handler.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, connect_handler.fun_id);
		lua_pushinteger(g_lua_state, conn_idx);
		lua_pushboolean(g_lua_state, is_success);

		if (connect_handler.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, connect_handler.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

void SocketReadTask::Process(HandleInfo recv_handler, int32 conn_idx, char* data, uint32 data_len)
{
	if (recv_handler.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, recv_handler.fun_id);
		lua_pushinteger(g_lua_state, conn_idx);
		lua_pushlstring(g_lua_state, data, data_len);
		lua_pushinteger(g_lua_state, data_len);

		if (recv_handler.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, recv_handler.param_id);
			LuaCall(g_lua_state, 4);
		}
		else
		{
			LuaCall(g_lua_state, 3);
		}
	}
}

void SocketCloseTask::Process(HandleInfo close_handler, int32 conn_idx)
{
	if (close_handler.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, close_handler.fun_id);
		lua_pushinteger(g_lua_state, conn_idx);

		if (close_handler.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, close_handler.param_id);
			LuaCall(g_lua_state, 2);
		}
		else
		{
			LuaCall(g_lua_state, 1);
		}
	}
}

void SocketClientDeleteTask::Process(HandleInfo connect_handler, HandleInfo recv_handler, HandleInfo close_handler)
{
	if (connect_handler.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, connect_handler.fun_id);
	}

	if (connect_handler.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, connect_handler.param_id);
	}

	if (recv_handler.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, recv_handler.fun_id);
	}

	if (recv_handler.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, recv_handler.param_id);
	}

	if (close_handler.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, close_handler.fun_id);
	}

	if (close_handler.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, close_handler.param_id);
	}
}

void HttpReadTask::Process(HandleInfo handler, int32 conn_idx, Request* request)
{
	if (handler.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler.fun_id);
		lua_pushinteger(g_lua_state, conn_idx);
		request->BuildLuaResult(g_lua_state);

		if (handler.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handler.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

//------------------------------------------------------------------------------------------------------
// 下面是跨线程调用
//------------------------------------------------------------------------------------------------------
Task::Task()
{

}

Task::~Task()
{

}

HttpTask::HttpTask()
{
	is_success_ = false;
	use_time_ = 0;
}

HttpTask::~HttpTask()
{
	if (handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handler_.fun_id);
	}

	if (handler_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handler_.param_id);
	}
}

void HttpTask::Init(HandleInfo handler, bool is_success, string& recv_buff, int32 use_time)
{
	handler_ = handler;
	is_success_ = is_success;
	recv_buff_ = recv_buff;
	use_time_ = use_time;
}

void HttpTask::Process()
{
	if (handler_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler_.fun_id);
		lua_pushboolean(g_lua_state, is_success_);
		lua_pushstring(g_lua_state, recv_buff_.c_str());
		lua_pushinteger(g_lua_state, use_time_);

		LuaCall(g_lua_state, 3);
	}
}

InputTask::InputTask()
{

}

InputTask::~InputTask()
{

}

void InputTask::Init(HandleInfo handler, string& cmd)
{
	handler_ = handler;
	cmd_ = cmd;
}

void InputTask::Process()
{
	if (handler_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler_.fun_id);
		lua_pushstring(g_lua_state, cmd_.c_str());
		LuaCall(g_lua_state, 1);
	}
}

MySQLTask::MySQLTask()
{
	rs_ = NULL;
}

MySQLTask::~MySQLTask()
{
	if (handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handler_.fun_id);
	}

	if (handler_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handler_.param_id);

	}

	if (rs_)
	{
		delete rs_;
		rs_ = NULL;
	}
}

void MySQLTask::Init(HandleInfo handler, ResultSet* rs)
{
	handler_ = handler;
	rs_ = rs;
}

void MySQLTask::Process()
{
	if (handler_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler_.fun_id);

		lua_pushboolean(g_lua_state, rs_->is_success);
		bool is_success = rs_->BuildLuaResult(g_lua_state);
		ASSERT(is_success);

		if (handler_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handler_.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

MongoDBTask::MongoDBTask()
{
	rs_ = NULL;
	is_one_table_ = false;
}

MongoDBTask::~MongoDBTask()
{
	if (handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handler_.fun_id);
	}

	if (handler_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handler_.param_id);
	}

	if (rs_)
	{
		delete rs_;
		rs_ = NULL;
	}
}

void MongoDBTask::Init(HandleInfo handler, MongoResultSet* rs, bool is_one_table /*= false*/)
{
	handler_ = handler;
	rs_ = rs;
	is_one_table_ = is_one_table;
}

void MongoDBTask::Process()
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第3步
	//---------------------------------------------
	if (handler_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handler_.fun_id);

		lua_pushboolean(g_lua_state, rs_->is_success);
		
		if (is_one_table_) //单个table
		{
			if (rs_->doc_list.size() == 1)
			{
				const bson_t* doc = rs_->doc_list[0];
				struct bson_reader br = { bson_get_data(doc), (int)doc->len };
				unpack_dict(g_lua_state, &br, false);
			}
			else if (rs_->doc_list.size() == 0)
			{
				lua_newtable(g_lua_state); //空的表格
			}
			else
			{
				luaL_error(g_lua_state, "one_table error! ------ the result count is > 1");
			}
		}
		else
		{
			lua_newtable(g_lua_state);//创建一个表格，放在栈顶

			for (uint32 n = 0; n < rs_->doc_list.size(); n++)
			{
				//stack_dump(g_lua_state);

				lua_pushnumber(g_lua_state, n + 1); // key为自增int

				const bson_t* doc = rs_->doc_list[n];
				struct bson_reader br = { bson_get_data(doc), (int)doc->len };
				unpack_dict(g_lua_state, &br, false);

				lua_settable(g_lua_state, -3);
			}
		}

		if (handler_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handler_.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}
