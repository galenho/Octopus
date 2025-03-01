#include "common.h"
#include "event.h"   

Event::Event()
{
#ifdef WIN32
	event_ = ::CreateEvent(0, false, false, NULL);
#else
	sem_init(&event_, 0, 0);
#endif
}

Event::~Event()
{
#ifdef WIN32
	
#else
	sem_destroy(&event_);
#endif
}

int Event::Wait()
{
#ifdef WIN32
	
	DWORD ret = ::WaitForSingleObject(event_, INFINITE);

	if (ret == WAIT_TIMEOUT)
	{
		return 1;
	}
	else if (ret == WAIT_OBJECT_0)
	{
		return 0;
	}
	else if (ret == WAIT_ABANDONED)
	{
		return -1;
	}
	return -1;
#else
	int ret = sem_wait(&event_);
	return ret;
#endif
}

int Event::Wait(int mill)
{
#ifdef WIN32
	DWORD ret = WaitForSingleObject(event_, mill);

	if (ret == WAIT_TIMEOUT)
	{
		return 1;
	}
	else if (ret == WAIT_OBJECT_0)
	{
		return 0;
	}
	else if (ret == WAIT_ABANDONED)
	{
		return -1;
	}
	return -1;
#else
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return -1;
	}

	int64 mills = ts.tv_nsec;
	mills += mill * (1000 * 1000);
	ts.tv_sec += int(mills / (1000 * 1000 * 1000));
	ts.tv_nsec = int(mills % (1000 * 1000 * 1000));
	int ret = sem_timedwait(&event_, &ts);

	if (-1 == ret)
	{
		if (errno == ETIMEDOUT)
		{
			return 1;
		}
		printf("sem_timedwait error %d\n", errno);
	}
	return ret;

#endif
}

void Event::Notify()
{
#ifdef WIN32
	::SetEvent(event_);
#else
	sem_post(&event_);
#endif
}