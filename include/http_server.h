/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "common.h"

class HTTPServer_impl;
class HTTPServer
{
public:
	HTTPServer();
	~HTTPServer();

	bool Start(const string ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	bool Close();
	void Disconnect(int32 conn_idx);

	char* GetIpAddress( int32 conn_idx );

	void Clear(int32 conn_idx);
	void SetError(int32 conn_idx, int code, string& msg);
	void SetBody(int32 conn_idx, string& body);
	void Response(int32 conn_idx);

private:
	HTTPServer_impl *imp_;
};

#endif //_HTTP_SERVER_H_