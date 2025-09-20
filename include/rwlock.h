/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef _RWLOCK_H
#define _RWLOCK_H

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

 /**
  * ¶ÁÐ´Ëø
  */
class RwLock
{
public:
	RwLock();
	~RwLock();

	// ¶ÁËø
	void ReadLock();
	void ReadUnLock();

	// Ð´Ëø
	void WriteLock();
	void WriteUnLock();

private:
#ifdef WIN32
	SRWLOCK lock_;
#else
	pthread_rwlock_t lock_;
#endif
};

#endif //_RWLOCK_H
