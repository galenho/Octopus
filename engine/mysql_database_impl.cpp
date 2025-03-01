#include "mysql_database_impl.h"
#include "guard.h"

MySqlDatabase_impl::MySqlDatabase_impl()
{
	conn_count_ = 0;
	next_conn_idx_ = 0;
}

MySqlDatabase_impl::~MySqlDatabase_impl()
{
	for (uint32 n=0; n < connect_lists.size(); n++)
	{
		MySqlConnection *conn = connect_lists[n];
		delete conn;
		conn = NULL;
	}

	connect_lists.clear();
}

bool MySqlDatabase_impl::Initialize(const string& hostname,
							   uint16 port, 
							   const string& username, 
							   const string& password, 
							   const string& database, 
							   const string& charset_name,
							   int32 conn_count)
{
	conn_count_ = conn_count;

	for (int32 i = 0; i < conn_count_; ++i)
	{
		MySqlConnection* conn = new MySqlConnection();
		string name = "Connection " + to_string(i);
		conn->set_name(name);
		conn->set_conn_idx(i);
		if (!conn->Init(hostname, port, username, password, database, charset_name))
		{
			return false;
		}

		conn->Activate();
		connect_lists.push_back(conn);
	}

	return true;
}

uint32 MySqlDatabase_impl::get_next_connect()
{
	int32 cur_conn_idx = next_conn_idx_;
	next_conn_idx_ = (cur_conn_idx + 1) % connect_lists.size();
	return cur_conn_idx;
}

uint32 MySqlDatabase_impl::get_free_connect()
{
	ASSERT(connect_lists.size() > 0);

	Guard guard(memcache_ref_mutex_);
	int32 min_count = connect_lists[0]->memcache_ref_count_;
	int32 conn_idx = 0;
	for (uint32 i = 1; i < connect_lists.size(); ++i)
	{
		if (connect_lists[i]->memcache_ref_count_ < min_count)
		{
			min_count = connect_lists[i]->memcache_ref_count_;
			conn_idx = i;
		}
	}

	return conn_idx;
}

void MySqlDatabase_impl::add_connect_ref( int32 conn_idx )
{
	ASSERT(conn_idx < (int)connect_lists.size());

	Guard guard(memcache_ref_mutex_);
	connect_lists[conn_idx]->memcache_ref_count_++;
}

void MySqlDatabase_impl::remove_connect_ref( int32 conn_idx )
{
	ASSERT(conn_idx < (int)connect_lists.size());

	Guard guard(memcache_ref_mutex_);
	ASSERT(connect_lists[conn_idx]->memcache_ref_count_ > 0);
	connect_lists[conn_idx]->memcache_ref_count_--;
}

Statement* MySqlDatabase_impl::CreateStatement(int32 conn_idx)
{
	if (conn_idx != INVALID_INDEX)
	{
		ASSERT(conn_idx < (int)connect_lists.size());
	}
	
	MySqlConnection* conn = NULL;
	if (conn_idx == INVALID_INDEX) //任意一条线程
	{
		conn = connect_lists[get_next_connect()];
	}
	else
	{
		conn = connect_lists[conn_idx];
	}

	return conn->CreateStatement();
}

void MySqlDatabase_impl::AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag)
{
	int32 conn_idx = stmt->GetConnIdx();
	ASSERT(conn_idx < (int)connect_lists.size());

	MySqlConnection* conn = connect_lists[conn_idx];
	conn->AddQuery(stmt, handler, flag);
}

void MySqlDatabase_impl::InsertFristQuery(Statement* stmt, HandleInfo handler)
{
	int32 conn_idx = stmt->GetConnIdx();
	ASSERT(conn_idx != INVALID_INDEX);

	MySqlConnection* conn = connect_lists[conn_idx];
	conn->InsertFristQuery(stmt, handler);
}

void MySqlDatabase_impl::Shutdown()
{
	for (uint32 n=0; n < connect_lists.size(); n++)
	{
		MySqlConnection* conn = connect_lists[n];
		conn->Shutdown();
	}
}

void MySqlDatabase_impl::Ping()
{
	for (uint32 n = 0; n < connect_lists.size(); n++)
	{
		MySqlConnection* conn = connect_lists[n];
		conn->Ping();
	}
}
