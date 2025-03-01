/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#ifndef _KCP_LISTEN_SOCKET_H_
#define _KCP_LISTEN_SOCKET_H_

#include "udp_listen_socket.h"

class KCPListenSocket : public UDPListenSocket
{
public:
	KCPListenSocket(const char* listen_address,
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize);

	virtual ~KCPListenSocket();

	bool Start();
};

#endif //_KCP_LISTEN_SOCKET_H_
