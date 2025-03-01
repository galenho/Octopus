/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "common.h"

#ifndef WIN32
#include <sys/timeb.h>
#endif


// 这个函数只能用来计算时间差, 不能记录于DB
#ifdef WIN32

struct uv_timeval
{
	int64 tv_sec;
	int32 tv_usec;
};

int32 uv_gettimeofday(uv_timeval* tv);

inline int64 getMSTime()
{
	struct uv_timeval tv;
	uv_gettimeofday(&tv);
	return (int64)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}
#else
inline int64 getMSTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}
#endif

class IntervalTimer
{
public:
	IntervalTimer(int32 index, int64 cur_time, int32 interval, HandleInfo handler);
	~IntervalTimer();

	bool Update(int64 cur_time);

	uint32 get_index()
	{
		return index_;
	}

	bool IsDelete()
	{
		return is_delete_;
	}

	void Delete()
	{
		is_delete_ = true;
	}

public:
	int32 index_;
	int32 interval_;
	int64 last_time_;
	HandleInfo handler_;
	bool is_delete_;
};

#endif //_TIMER_H_