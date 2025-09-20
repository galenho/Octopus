/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef _EVENT_H
#define _EVENT_H																		

#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#endif

/**
 * 事件类
 */
class Event
{
public:
	Event();
	~Event();
	
	// 阻塞等待 (注: 返回值 0: ok, 1: timeout, -1: error)
	int Wait();

	// 带超时的阻塞等待 (注: 返回值 0: ok, 1: timeout, -1: error)
	int Wait(int ms);

	// 消息通知
	void Notify();

private:
#ifdef WIN32
	HANDLE	event_;
#else
	sem_t	sem_;
#endif
};

#endif //_EVENT_H
