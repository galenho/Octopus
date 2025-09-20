/*
** Octopus
** Copyright (C) 2009-2023 RedLight Team
** author: galen
**     
*/

#ifndef _WEBSOCKET_CLIENT_H_
#define _WEBSOCKET_CLIENT_H_

#include "common.h"

class WebSocketClient_impl;
class WebSocketClient
{
public:
	WebSocketClient();
	~WebSocketClient();

	int32 Connect(const string ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 send_buffer_size = 8192,
		uint32 read_buffer_size = 8192);

	bool SendMsg(uint32 conn_idx, char* msg, uint32 len);
	void Disconnect(uint32 conn_idx);

	char* GetIpAddress( uint32 conn_idx );

private:
	WebSocketClient_impl *imp_;
};

#endif //_WEBSOCKET_CLIENT_H_