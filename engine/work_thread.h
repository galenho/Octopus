/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include "thread_base.h"
#include "mutex.h"
#include "task.h"
#include "safe_list.h"
#include "timer.h"
#include "event.h"

class WorkThread : public ThreadBase
{
public:
	WorkThread();
	virtual ~WorkThread();

	virtual void Shutdown();

public:
	void set_init_handler(init_handler_type init_handler);

	int32 add_timer(int32 interval, HandleInfo handler);
	void remove_timer(int32 index);
	void stop_all_timer();

	void PushTask(Task* task);
	void WakeUp();

protected:
	virtual bool Run();

	int32 MakeGeneralTimerID();
	
private:
	init_handler_type init_handler_;
	SafeList<Task*> task_list_;

	hash_map<int32, IntervalTimer*> timer_map_;
	vector<IntervalTimer*> del_timer_list_;

	int32 auto_timer_idx_;
};

#endif //_WORK_THREAD_H_