#include "kcp_server_impl.h"

KCPServer_impl::KCPServer_impl()
{
	listen_socket_ = NULL;
}

KCPServer_impl::~KCPServer_impl()
{
	
}

bool KCPServer_impl::Start(
	const string& ip, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
{
	listen_socket_ = new KCPListenSocket(ip.c_str(), 
		port,
		onconnected_handler, 
		onclose_handler, 
		onrecv_handler, 
		sendbuffersize, 
		recvbuffersize);

	listen_socket_->Start();

	return true;
}

bool KCPServer_impl::Close()
{
	listen_socket_->Close();
	return true;
}

bool KCPServer_impl::SendMsg( int32 conn_idx, const void* msg, uint32 len )
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

void KCPServer_impl::Disconnect( int32 conn_idx )
{
	return SocketMgr::get_instance()->Disconnect(conn_idx);
}

char* KCPServer_impl::GetIpAddress( int32 conn_idx )
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}