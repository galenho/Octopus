/*
** Octopus
** Copyright (C) 2009-2023 RedLight Team
** author: galen
**     
*/

#ifndef _TCP_LISTEN_SOCKET_H_
#define _TCP_LISTEN_SOCKET_H_

#include "listen_socket.h"

class TCPListenSocket : public ListenSocket
{
public:
	TCPListenSocket(const char* listen_address,
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize);

	virtual ~TCPListenSocket();

	bool Start();

#ifdef WIN32
	// 投递一个accept请求
	bool PostAccept();
public:
	OverlappedStruct accept_event_;

	char buff_[128];
	DWORD buff_len_;

	LPFN_ACCEPTEX accept_ex_;
	LPFN_GETACCEPTEXSOCKADDRS accept_addrs_;
#endif
};

#endif //_TCP_LISTEN_SOCKET_H_
