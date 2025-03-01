#ifndef _KCP_SERVER_IMPL_H_
#define _KCP_SERVER_IMPL_H_

#include "network.h"

class KCPListenSocket;
class KCPServer_impl
{
public:
	KCPServer_impl();
	~KCPServer_impl();

	bool Start(const string& ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);
	bool Close();
	bool SendMsg( int32 conn_idx, const void* msg, uint32 len );
	void Disconnect( int32 conn_idx );

	char* GetIpAddress( int32 conn_idx );

private:
	KCPListenSocket *listen_socket_;
};

#endif //_UDP_SERVER_IMPL_H_