/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _SOCKET_DEFINES_H_
#define _SOCKET_DEFINES_H_

#include "common.h"
#include "referable.h"

using namespace std;

/* Implementation Selection */
#ifdef WIN32

#else

// unix defines
#define SOCKET int
#define SD_BOTH SHUT_RDWR

// epoll
#include <sys/epoll.h>

#endif

enum SocketIOEvent
{
	SOCKET_IO_EVENT_ACCEPT_TCP			= 1,
	SOCKET_IO_EVENT_ACCEPT_UDP			= 2,

	SOCKET_IO_EVENT_CONNECT_COMPLETE	= 3,
	SOCKET_IO_EVENT_READ_COMPLETE		= 4,
	SOCKET_IO_EVENT_WRITE_COMPLETE		= 5,
	
	SOCKET_IO_EVENT_CLOSE				= 6,
	
	SOCKET_IO_THREAD_SHUTDOWN			= 7,
	SOCKET_IO_THREAD_WAKEUP				= 8,
};

enum SocketType
{
	SOCKET_TYPE_TCP = 1,
	SOCKET_TYPE_UDP = 2
};

enum TCPSubType
{
	TCP_TYPE_NORMAL		= 1,
	TCP_TYPE_HTTP		= 2,
	TCP_TYPE_WEB_SOCKET	= 3,
};

enum UDPType
{
	UDP_TYPE_NORMAL		= 1,
	UDP_TYPE_KCP		= 2,
};

/* IOCP Defines */
#ifdef WIN32

LPFN_CONNECTEX get_connect_ex(SOCKET fd);
LPFN_ACCEPTEX get_accept_ex(SOCKET fd);
LPFN_GETACCEPTEXSOCKADDRS get_accept_addr(SOCKET fd);

class OverlappedStruct
{
public:
	OVERLAPPED overlap_;
	SocketIOEvent event_;
	SOCKET fd_;

	OverlappedStruct(SocketIOEvent ev)
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
		event_ = ev;
		fd_ = 0;
	};

	OverlappedStruct()
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
		fd_ = 0;
	}

	__forceinline void SetEvent(SocketIOEvent ev)
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
		event_ = ev;
	}
};

#endif

class SocketBase: public Referable
{
public:
	SocketBase() 
	{
		socket_type_ = SOCKET_TYPE_TCP; 
		sub_type_ = 0;
		is_listen_ = false;
	}

	virtual ~SocketBase() {}

public:
	SocketType socket_type_;
	int32 sub_type_;
	bool is_listen_;
};

#endif
