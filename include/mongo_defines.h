/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _MONGO_DEFINES_H_
#define _MONGO_DEFINES_H_

#include "common.h"
#include "bson.h"
#include "mongoc.h"

struct MongoResultSet
{
	bool is_success;
	bool is_one_table;
	vector<bson_t*> doc_list;

	MongoResultSet()
	{
		is_success = true;
		is_one_table = false;
	}

	~MongoResultSet()
	{
		for (size_t i = 0; i < doc_list.size(); i++)
		{
			bson_t* t = doc_list[i];
			bson_destroy(t);
		}
	}
};

typedef function<void(bool, MongoResultSet*)> mongo_handler_type;

#endif //_MONGO_DEFINES_H_