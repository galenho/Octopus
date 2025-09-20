/**
 * Octopus
 * Copyright (C) 2009-2025 RedLight Team
 * author: galen
 */

#ifndef __CONSOLE_POLLER_HEADER__
#define __CONSOLE_POLLER_HEADER__

#include "thread_base.h"

/**
 * øÿ÷∆Ã®GM¿‡
 */
class ConsolePoller : public ThreadBase, public Singleton<ConsolePoller>
{
public:
	ConsolePoller();
	virtual ~ConsolePoller();

	static void DestoryLocal();

	void set_callback_handler(HandleInfo on_input_handle);
	void Stop();

protected:	
	bool Run();
	
private:
	HandleInfo on_input_handle_;
};

#endif // __CONSOLE_POLLER_HEADER__