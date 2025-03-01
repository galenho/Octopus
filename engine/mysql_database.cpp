#include "mysql_database.h"
#include "mysql_database_impl.h"

MySqlDatabase::MySqlDatabase()
{
	imp_ = new MySqlDatabase_impl();
}

MySqlDatabase::~MySqlDatabase()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}


bool MySqlDatabase::Initialize( const string hostname, uint16 port,
						   const string username, const string password, 
						   const string database, const string charset_name,
						   int32 conn_count )
{
	return imp_->Initialize(hostname, port, username, password, database, charset_name, conn_count);
}

void MySqlDatabase::AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag /*= DB_TRANSACTION_NONE*/)
{
	imp_->AddQuery(stmt, handler, flag);
}

void MySqlDatabase::InsertFristQuery(Statement* stmt, HandleInfo handler)
{
	imp_->InsertFristQuery(stmt, handler);
}

void MySqlDatabase::Ping()
{
	imp_->Ping();
}

void MySqlDatabase::Shutdown()
{
	return imp_->Shutdown();
}

uint32 MySqlDatabase::get_next_connect()
{
	return imp_->get_next_connect();
}

uint32 MySqlDatabase::get_free_connect()
{
	return imp_->get_free_connect();
}

void MySqlDatabase::add_connect_ref( int32 conn_idx )
{
	imp_->add_connect_ref(conn_idx);
}

void MySqlDatabase::remove_connect_ref( int32 conn_idx )
{
	imp_->remove_connect_ref(conn_idx);
}

Statement* MySqlDatabase::CreateStatement(int32 conn_idx)
{
	return imp_->CreateStatement(conn_idx);
}