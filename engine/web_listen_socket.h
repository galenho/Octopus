/*
** Octopus
** Copyright (C) 2009-2023 RedLight Team
** author: galen
**     
*/

#ifndef _WEB_LISTEN_SOCKET_H_
#define _WEB_LISTEN_SOCKET_H_

#include "tcp_listen_socket.h"

class WebListenSocket : public TCPListenSocket
{
public:
	WebListenSocket(const char* listen_address,
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize);

	virtual ~WebListenSocket();

	bool Start();
};

#endif //_WEB_LISTEN_SOCKET_H_
