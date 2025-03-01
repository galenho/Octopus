/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _SHEDULER_IMPL_H_
#define _SHEDULER_IMPL_H_

#include "thread_base.h"
#include "mutex.h"
#include "timer.h"
#include "work_thread.h"

class Scheduler_impl
{
public:
	Scheduler_impl();
	virtual ~Scheduler_impl();

	bool startup(init_handler_type init_handler);
	bool shutdown();

	void CleanupNetwork();

	uint32 add_timer(uint32 interval, HandleInfo handler);
	void remove_timer(uint32 index);
	void stop_all_timer();

	void PushTask(Task* task);

	void add_thread_ref(string thread_name);
	void remove_thread_ref(string thread_name);
	int32 get_thread_count();
	
	void PrintThreadInfos();

private:
	WorkThread*	work_thread_;
	
	Mutex thread_count_lock_;
	hash_map<string, string> thread_name_map_;
};

#endif //_SHEDULER_IMPL_H_
