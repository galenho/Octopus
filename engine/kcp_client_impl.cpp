#include "common.h"
#include "kcp_client_impl.h"
#include "network.h"

KCPClient_impl::KCPClient_impl()
{
	
}

KCPClient_impl::~KCPClient_impl()
{
	
}

uint32 KCPClient_impl::Connect( const string& ip, uint16 port, uint16& local_port,
							  const HandleInfo onconnected_handler,
							  const HandleInfo onclose_handler,
							  const HandleInfo onrecv_handler,
							  uint32 sendbuffersize, 
							  uint32 recvbuffersize)
{
	int32 conn_idx = SocketMgr::get_instance()->ConnectUDP( ip, port, local_port,
								 onconnected_handler, 
								 onclose_handler, 
								 onrecv_handler,
								 sendbuffersize, 
								 recvbuffersize, 
								 UDP_TYPE_KCP,
								 false);

	return conn_idx;                                                                                                                     
}

void KCPClient_impl::Disconnect( int32 conn_idx )
{
	SocketMgr::get_instance()->Disconnect(conn_idx);
}

bool KCPClient_impl::SendMsg( int32 conn_idx, const void* msg, uint32 len )
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

char* KCPClient_impl::GetIpAddress( int32 conn_idx )
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}
