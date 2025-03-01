/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "common.h"

class TCPClient_impl;

class TCPClient
{
public:
	TCPClient();
	~TCPClient();

	uint32 Connect(const string ip, 
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize = 8192, 
				 uint32 recvbuffersize = 8192);

	uint32 ConnectEx(const string ip, 
		uint16 port, 
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize = 8192, 
		uint32 recvbuffersize = 8192);

	bool SendMsg(int32 conn_idx, char* msg, uint32 len);
	void Disconnect(int32 conn_idx);
	char* GetIpAddress( int32 conn_idx );

private:
	TCPClient_impl *imp_;
};

#endif //_TCP_CLIENT_H_