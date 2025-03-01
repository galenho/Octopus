/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "common.h"

class TCPServer_impl;
class TCPServer
{
public:
	TCPServer();
	~TCPServer();

	bool Start(const string ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	bool Close();
	bool SendMsg(int32 conn_idx, char* msg, uint32 len);
	void Disconnect(int32 conn_idx);

	char* GetIpAddress( int32 conn_idx );

private:
	TCPServer_impl *imp_;
};

#endif //_TCP_SERVER_H_