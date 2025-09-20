/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef _GUARD_H_
#define _GUARD_H_

#include "mutex.h"

 /**
 *  ÿŒ¿¿‡
 */
class Guard
{
public:
	Guard(Mutex& mutex) : target(mutex)
	{
		target.Lock();
	}

	~Guard()
	{
		target.UnLock();
	}

protected:
	Mutex& target;
};

#endif //_GUARD_H_

