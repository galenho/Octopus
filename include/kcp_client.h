#ifndef _KCP_CLIENT_H_
#define _KCP_CLIENT_H_

#include "common.h"

class KCPClient_impl;

class KCPClient
{
public:
	KCPClient();
	~KCPClient();

	int32 Connect(const string& ip, 
				 uint16 port, 
				 uint16& local_port,
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 send_buffer_size = 8192, 
				 uint32 read_buffer_size = 8192);

	void Disconnect(uint32 conn_idx);
	bool SendMsg(uint32 conn_idx, const void* msg, uint32 len);

	char* GetIpAddress( uint32 conn_idx );

private:
	KCPClient_impl *imp_;
};

#endif //_KCP_CLIENT_H_