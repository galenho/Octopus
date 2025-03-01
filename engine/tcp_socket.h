/*
* 3D MMORPG Server
* Copyright (C) 2009-2023 RedLight Team
* author: galen
*/

#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "socket.h"

class TCPSocket : public Socket
{
public:
	TCPSocket(SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	bool Connect(const char* address, uint16 port);
	bool ConnectEx(const char* address, uint16 port);

	virtual void OnRead();
};

#endif //_TCP_SOCKET_H_
