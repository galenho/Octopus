/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _MYSQL_DATABASE_H_
#define _MYSQL_DATABASE_H_

#include "mysql_defines.h"

class MySqlDatabase_impl;
class MySqlDatabase
{
public:
	MySqlDatabase();
	~MySqlDatabase();

	bool Initialize(const string hostname, 
					uint16 port, 
					const string username, 
					const string password,
					const string database,
					const string charset_name,
					int32 conn_count);

	void Shutdown();

	// �õ���һ������, һ����δָ��ִ�������̵߳�sql����õ�
	uint32 get_next_connect();

	// һ���Ǹ�memcache����, ��Ϊmemcache��new��delete
	uint32 get_free_connect();
	
	// ����ĳ�����̵߳����ü���
	void add_connect_ref(int32 conn_idx);
	
	// �Ƴ�ĳ�����̵߳����ü���
	void remove_connect_ref(int32 conn_idx);

	// ����һ��Statement
	Statement* CreateStatement(int32 conn_idx);

	// sqlִ�нӿ�(�������)
	void AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag = DB_TRANSACTION_NONE);

	// sqlִ�нӿ�(������ǰ)
	void InsertFristQuery(Statement* stmt, HandleInfo handler);

	void Ping();

private:
	MySqlDatabase_impl *imp_;
};

#endif //_MYSQL_DATABASE_H_