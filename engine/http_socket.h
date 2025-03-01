/*
* 3D MMORPG Server
* Copyright (C) 2009-2023 RedLight Team
* author: galen
*/

#ifndef _HTTP_SOCKET_H_
#define _HTTP_SOCKET_H_

#include "tcp_socket.h"
#include "http_parser.h"
#include "sim_parser.h"

class HTTPSocket : public TCPSocket
{
public:
	HTTPSocket(
		SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	virtual void OnRead();
	
public:
	http_parser_settings settings_;
	http_parser parser_;
	Request request_;
	Response response_;
};

#endif //_HTTP_SOCKET_H_
