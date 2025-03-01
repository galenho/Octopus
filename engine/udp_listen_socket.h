/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#ifndef _UDP_LISTEN_SOCKET_H_
#define _UDP_LISTEN_SOCKET_H_

#include "listen_socket.h"

class UDPListenSocket : public ListenSocket
{
public:
	UDPListenSocket(const char* listen_address, 
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize);

	virtual ~UDPListenSocket();

	bool Start();

#ifdef WIN32
	// 投递一个连接操作(仿TCP)
	bool PostAccept();
#endif
	
public:
	char buff_[128];

#ifdef WIN32
	sockaddr_in	client_addr_;
	OverlappedStruct read_event_;
#endif
};

#endif //_UDP_LISTEN_SOCKET_H_
