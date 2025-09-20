/*
** Octopus
** Copyright (C) 2009-2023 RedLight Team
** author: galen
**     
*/

#ifndef _WEBSOCKET_SERVER_H_
#define _WEBSOCKET_SERVER_H_

#include "common.h"

class WebSocketServer_impl;
class WebSocketServer
{
public:
	WebSocketServer();
	~WebSocketServer();

	bool Start(const string ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 send_buffer_size,
		uint32 read_buffer_size);

	bool Close();
	bool SendMsg(uint32 conn_idx, char* msg, uint32 len);
	void Disconnect(uint32 conn_idx);

	char* GetIpAddress( uint32 conn_idx );

private:
	WebSocketServer_impl *imp_;
};

#endif //_WEBSOCKET_SERVER_H_