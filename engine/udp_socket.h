/*
* 3D MMORPG Server
* Copyright (C) 2009-2023 RedLight Team
* author: galen
*/

#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

#include "socket.h"
#include "ikcp.h"

class UDPSocket : public Socket
{
public:
	UDPSocket(SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	~UDPSocket();

	bool Connect(const char* address, uint16 port, uint16& local_port);

	virtual void OnRead();
	virtual void Disconnect();

public:
	bool is_udp_connected_;
};

#endif //_UDP_SOCKET_H_