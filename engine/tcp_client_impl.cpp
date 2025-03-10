#include "common.h"
#include "tcp_client_impl.h"
#include "network.h"

TCPClient_impl::TCPClient_impl()
{
	
}

TCPClient_impl::~TCPClient_impl()
{
	
}

uint32 TCPClient_impl::Connect( const string& ip, uint16 port,
							  const HandleInfo onconnected_handler, 
							  const HandleInfo onclose_handler,
							  const HandleInfo onrecv_handler,
							  uint32 sendbuffersize, 
							  uint32 recvbuffersize )
{
	return SocketMgr::get_instance()->Connect(  ip, port,
								 onconnected_handler, 
								 onclose_handler, 
								 onrecv_handler,
								 sendbuffersize, 
								 recvbuffersize);
}

uint32 TCPClient_impl::ConnectEx( const string& ip, uint16 port,
							   const HandleInfo onconnected_handler,
							   const HandleInfo onclose_handler,
							   const HandleInfo onrecv_handler,
							   uint32 sendbuffersize, 
							   uint32 recvbuffersize)
{
	return SocketMgr::get_instance()->ConnectEx( ip, port,
		onconnected_handler, 
		onclose_handler, 
		onrecv_handler,
		sendbuffersize, 
		recvbuffersize);
}

void TCPClient_impl::Disconnect( int32 conn_idx )
{
	SocketMgr::get_instance()->Disconnect(conn_idx);
}

bool TCPClient_impl::SendMsg( int32 conn_idx, const void* msg, uint32 len )
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

char* TCPClient_impl::GetIpAddress( int32 conn_idx )
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}