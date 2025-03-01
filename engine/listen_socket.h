/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _LISTEN_SOCKET_H_
#define _LISTEN_SOCKET_H_

#include "socket_defines.h"

class ListenSocket : public SocketBase
{
public:
	ListenSocket(
		const char* listen_address,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	virtual ~ListenSocket();

	virtual bool Start() = 0;
	virtual void Close();

	int GetFd();

public:
	SOCKET socket_;
	struct sockaddr_in address_;
	uint16 port_;

	uint32 sendbuffersize_;
	uint32 recvbuffersize_;

	HandleInfo onconnected_handler_;
	HandleInfo onclose_handler_;
	HandleInfo onrecv_handler_;
};

#endif
