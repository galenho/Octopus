#ifndef _KCP_SERVER_H_
#define _KCP_SERVER_H_

#include "common.h"

class KCPServer_impl;
class KCPServer
{
public:
	KCPServer();
	~KCPServer();

	bool Start(const string& ip, 
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize = 8192, 
		uint32 recvbuffersize = 8192);

	bool Close();
	bool SendMsg(int32 conn_idx, const void* msg, uint32 len);
	void Disconnect(int32 conn_idx);

	char* GetIpAddress( int32 conn_idx );

private:
	KCPServer_impl *imp_;
};

#endif //_KCP_SERVER_H_