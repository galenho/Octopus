#include "timer.h"
#include "task.h"

#if WIN32
int32 uv_gettimeofday(uv_timeval* tv)
{
	/* Based on https://doxygen.postgresql.org/gettimeofday_8c_source.html */
	const uint64_t epoch = (uint64_t)116444736000000000ULL;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTimeAsFileTime(&file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;
	tv->tv_sec = (int64_t)((ularge.QuadPart - epoch) / 10000000L);
	tv->tv_usec = (int32_t)(((ularge.QuadPart - epoch) % 10000000L) / 10);
	return 0;
}
#endif

IntervalTimer::IntervalTimer(int32 index, int64 cur_time, int32 interval, HandleInfo handler)
{
	index_ = index;
	interval_ = interval;
	last_time_ = cur_time;
	handler_ = handler;
	is_delete_ = false;
}

IntervalTimer::~IntervalTimer()
{
	TimerTask::DeleteTimer(handler_, index_);
}

bool IntervalTimer::Update(int64 cur_time)
{
	int64 diff_time = cur_time - last_time_;
	if ((int32)diff_time >= interval_)
	{
		last_time_ = cur_time;

		return true;
	}
	else
	{
		return false;
	}
}
