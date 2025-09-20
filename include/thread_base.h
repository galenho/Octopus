/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_

#ifdef WIN32
#include <windows.h>
#include <Process.h>
#else
#include <pthread.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#endif

#include <string>
using namespace std;

/**
 * 线程基类
 */
class ThreadBase
{
public:
	ThreadBase();
	virtual ~ThreadBase() {}

	// 设置线程名
	void set_name(string name);

	// 激活线程
	void Activate();

	// 子类继承基于，并重写虚方法Run()
	virtual bool Run();

protected:
	bool is_running_;

#ifdef WIN32
	HANDLE handle;
#else
	pthread_t id;
	pthread_t new_thread;
#endif

	string thread_name_;
};

#endif //_THREAD_BASE_H_

