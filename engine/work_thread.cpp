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
	// �߳̿�ʼ����
	set_name("work thread");
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	// ��ʼ������
	SocketMgr::get_instance()->Init();

	// ��ʼ������
	if (init_handler_)
	{
		init_handler_();
	}

	int32 wait_time = g_loop_interval;						//ÿ֡����ms (Ĭ��100ms)
	int64 next_time = getMSTime() + (int64)wait_time;		//��һ֡��ʱ���

	while (is_running_)
	{
		//------------------------------------------------------------------------
		// (1) ������Ϣ����
		//------------------------------------------------------------------------
		Task* task = NULL;
		while (task_list_.pop(task))
		{
			task->Process();

			delete task;
			task = NULL;
		}

		//------------------------------------------------------------------------
		// (2) ����ʱ��
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
		// (3) ����Socket
		//------------------------------------------------------------------------
		cur_time = getMSTime();
		if (next_time > cur_time)
		{
			// ����ʱ������������Ϣ
			SocketMgr::get_instance()->EventLoop(next_time);
		}

		//------------------------------------------------------------------------
		// ������һ֡ʱ��
		//------------------------------------------------------------------------
		next_time += wait_time;
	}

	// �߳̽�������
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
				timer_idx = 1; //��ͷ��ʼ
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