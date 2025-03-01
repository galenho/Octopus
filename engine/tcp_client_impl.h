/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _TCP_CLIENT_IMPL_H_
#define _TCP_CLIENT_IMPL_H_

class Socket;
class TCPClient_impl
{
public:
	TCPClient_impl();

	~TCPClient_impl();

	uint32 Connect( const string& ip, uint16 port,
				  const HandleInfo onconnected_handler,
				  const HandleInfo onclose_handler,
				  const HandleInfo onrecv_handler,
				  uint32 sendbuffersize, 
				  uint32 recvbuffersize);

	uint32 ConnectEx( const string& ip, uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize);

	void Disconnect( int32 conn_idx );
	bool SendMsg( int32 conn_idx, const void* msg, uint32 len );

	char* GetIpAddress( int32 conn_idx );
};

#endif //_TCP_CLIENT_IMPL_H_