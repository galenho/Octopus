/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef _TASK_H_
#define _TASK_H_

#include "mutex.h"
#include "mysql_defines.h"
#include "mongo_task.h"
#include "sim_parser.h"

//--------------------------------------------------------------
// 下面的任务在工作线程，直接调用就可以，不需要进消息队列
//--------------------------------------------------------------
class TimerTask
{
public:
	static void Process(HandleInfo handler, int32 index);
	static void DeleteTimer(HandleInfo handler, int32 index);
};

class SocketConnectTask
{
public:
	static void Process(HandleInfo connect_handler, uint32 conn_idx, bool is_success);
};

class SocketReadTask
{
public:
	static void Process(HandleInfo recv_handler, uint32 conn_idx, char* data, uint32 data_len);
};

class SocketCloseTask
{
public:
	static void Process(HandleInfo close_handler, uint32 conn_idx);
};

class SocketClientDeleteTask
{
public:
	static void Process(HandleInfo connect_handler, HandleInfo recv_handler, HandleInfo close_handler);
};

class SocketServerDeleteTask
{
public:
	static void Process(HandleInfo connect_handler, HandleInfo recv_handler, HandleInfo close_handler);
};

class HttpReadTask
{
public:
	static void Process(HandleInfo handler, uint32 conn_idx, Request* request);
};

//--------------------------------------------------------------
// 下面的任务在其他线程，需要进消息队列，顺序执行
//--------------------------------------------------------------

/**
* 任务基类
*/
class Task
{
public:
	Task();
	virtual ~Task();

public:
	virtual void Process() = 0;
};

class HttpTask : public Task
{
public:
	HttpTask();
	virtual ~HttpTask();

	void Init(HandleInfo handler, bool is_success, string& recv_buff, int32 use_time);
	virtual void Process();

public:
	HandleInfo handler_;
	bool is_success_;
	string recv_buff_;
	int32 use_time_;
};

class InputTask : public Task
{
public:
	InputTask();
	virtual ~InputTask();

	void Init(HandleInfo handler, string& cmd);

	virtual void Process();

public:
	HandleInfo handler_;
	string cmd_;
};

class ConsoleDeleteTask : public Task
{
public:
	ConsoleDeleteTask();
	virtual ~ConsoleDeleteTask();

	void Init(HandleInfo handler);
	virtual void Process();

public:
	HandleInfo handler_;
};

class MySQLTask : public Task
{
public:
	MySQLTask();
	virtual ~MySQLTask();

	void Init(HandleInfo handler, ResultSet* rs);

	virtual void Process();

public:
	HandleInfo handler_;
	ResultSet* rs_;
};

class MongoDBTask : public Task
{
public:
	MongoDBTask();
	virtual ~MongoDBTask();

	void Init(HandleInfo handler, MongoResultSet* rs, bool is_one_table = false);

	virtual void Process();

public:
	HandleInfo handler_;
	MongoResultSet* rs_;
	bool is_one_table_;
};


#endif //_TASK_H_