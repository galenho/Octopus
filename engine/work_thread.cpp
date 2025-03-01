#include "common.h"
#include "work_thread.h"
#include "scheduler.h"
#include "network.h"

WorkThread::WorkThread()
{
	auto_timer_idx_ = 1;

	init_handler_ = NULL;
}

WorkThread::~WorkThread()
{
	del_timer_list_.clear();
	//-----------------------------------------------------------------------------------------
	hash_map<int32, IntervalTimer*>::iterator it = timer_map_.begin();
	for (; it != timer_map_.end(); ++it)
	{
		IntervalTimer* timer = it->second;
		delete timer;
		timer = NULL;
	}
	timer_map_.clear();
}

void WorkThread::set_init_handler(init_handler_type init_handler)
{
	init_handler_ = init_handler;
}

bool WorkThread::Run()
{
	// 线程开始运行
	set_name("work thread");
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	// 初始化网络
	SocketMgr::get_instance()->Init();

	// 初始化函数
	if (init_handler_)
	{
		init_handler_();
	}

	int32 wait_time = g_loop_interval;						//每帧多少ms (默认100ms)
	int64 next_time = getMSTime() + (int64)wait_time;		//下一帧的时间点

	while (is_running_)
	{
		//------------------------------------------------------------------------
		// (1) 处理消息队列
		//------------------------------------------------------------------------
		Task* task = NULL;
		while (task_list_.pop(task))
		{
			task->Process();

			delete task;
			task = NULL;
		}

		//------------------------------------------------------------------------
		// (2) 处理定时器
		//------------------------------------------------------------------------
		int64 cur_time = getMSTime();
		for (int n = 0; n < (int)del_timer_list_.size(); n++)
		{
			IntervalTimer* timer = del_timer_list_[n];
			hash_map<int32, IntervalTimer*>::iterator it = timer_map_.find(timer->get_index());
			if (it != timer_map_.end())
			{
				timer_map_.erase(it);
			}
			delete timer;
			timer = NULL;
		}
		del_timer_list_.clear();

		hash_map<int32, IntervalTimer*>::iterator it = timer_map_.begin();
		for (; it != timer_map_.end(); ++it)
		{
			IntervalTimer* timer = it->second;
			if (timer->IsDelete())
			{
				continue;
			}

			bool ret = timer->Update(cur_time);
			if (ret)
			{
				TimerTask::Process(timer->handler_, timer->index_);
			}
		}

		//------------------------------------------------------------------------
		// (3) 处理Socket
		//------------------------------------------------------------------------
		cur_time = getMSTime();
		if (next_time > cur_time)
		{
			// 还有时间则处理网络消息
			SocketMgr::get_instance()->EventLoop(next_time);
		}

		//------------------------------------------------------------------------
		// 进入下一帧时间
		//------------------------------------------------------------------------
		next_time += wait_time;
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return false;
}

void WorkThread::Shutdown()
{
	is_running_ = false;
	WakeUp();
}

void WorkThread::PushTask(Task* task)
{
	int size = task_list_.push_back(task);
	if (size == 1)
	{
		WakeUp();
	}
}

void WorkThread::WakeUp()
{
	SocketMgr::get_instance()->WakeUp();
}

int32 WorkThread::MakeGeneralTimerID()
{
	int32 timer_idx = INVALID_INDEX;
	while (true)
	{
		hash_map<int32, IntervalTimer*>::iterator it = timer_map_.find(auto_timer_idx_);
		if (it != timer_map_.end())
		{
			++timer_idx;
			if (timer_idx == INVALID_INDEX)
			{
				timer_idx = 1; //从头开始
			}
		}
		else
		{
			break;
		}
	}

	//-----------------------------------------------------------------
	timer_idx = auto_timer_idx_;
	++auto_timer_idx_;
	if (auto_timer_idx_ == INVALID_INDEX)
	{
		auto_timer_idx_ = 1;
	}

	return timer_idx;
}

int32 WorkThread::add_timer(int32 interval, HandleInfo handler)
{
	int32 timer_idx = MakeGeneralTimerID();
	IntervalTimer* timer = new IntervalTimer(timer_idx, getMSTime(), interval, handler);
	timer_map_.insert(make_pair(timer_idx, timer));

	WakeUp();

	return timer_idx;
}

void WorkThread::remove_timer(int32 index)
{
	hash_map<int32, IntervalTimer*>::iterator it = timer_map_.find(index);
	if (it != timer_map_.end())
	{
		IntervalTimer* timer = it->second;
		if (!timer->IsDelete())
		{
			timer->Delete();
			del_timer_list_.push_back(timer);
		}
	}
}

void WorkThread::stop_all_timer()
{
	hash_map<int32, IntervalTimer*>::iterator it = timer_map_.begin();
	for (; it != timer_map_.end(); ++it)
	{
		IntervalTimer* timer = it->second;
		if (!timer->IsDelete())
		{
			timer->Delete();
			del_timer_list_.push_back(timer);
		}
	}
}