/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: 
*/

#ifndef _SAFE_LIST_H_
#define _SAFE_LIST_H_

#include <list>
#include "mutex.h"

template <class T>
class SafeList
{
public:
	~SafeList()
	{
	}

	inline int push_back(const T& element)
	{
		int size = 0;
		mutex.Lock();
		list.push_back(element);
		size = (int)list.size();
		mutex.UnLock();
		return size;
	}

	inline int push_front(const T& element)
	{
		int size = 0;
		mutex.Lock();
		list.push_front(element);
		size = (int)list.size();
		mutex.UnLock();
		return size;
	}

	inline bool pop(T& element)
	{
		mutex.Lock();
		if (list.size() > 0)
		{
			element = list.front();
			list.pop_front();
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline bool front(T& element)
	{
		mutex.Lock();
		if (list.size() > 0)
		{
			element = list.at(0);
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline bool pop_front(T& element)
	{
		mutex.Lock();
		if (list.size() > 0)
		{
			list.pop_front();
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline int size()
	{
		mutex.Lock();
		int c = (int)list.size();
		mutex.UnLock();
		return c;
	}

	inline bool empty()
	{
		bool is_empty = false;
		mutex.Lock();
		is_empty = list.empty();
		mutex.UnLock();
		return is_empty;
	}

protected:
	std::list<T> list;
	Mutex mutex;
};

#endif //_SAFE_LIST_H_
