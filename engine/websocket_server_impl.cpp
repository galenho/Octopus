#include "websocket_server_impl.h"

WebSocketServer_impl::WebSocketServer_impl( )
{
	listen_socket_ = NULL;
}

WebSocketServer_impl::~WebSocketServer_impl()
{
	if (listen_socket_)
	{
		delete listen_socket_;
		listen_socket_ = NULL;
	}
}

bool WebSocketServer_impl::Start(const string& ip,
	uint16 port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize,
	uint32 recvbuffersize)
{
	listen_socket_ = new WebListenSocket(ip.c_str(),
		port,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize);

	return listen_socket_->Start();
}

bool WebSocketServer_impl::Close()
{
	listen_socket_->Close();
	return true;
}

bool WebSocketServer_impl::SendMsg(int32 conn_idx, const void* msg, uint32 len)
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

void WebSocketServer_impl::Disconnect( int32 conn_idx )
{
	return SocketMgr::get_instance()->Disconnect(conn_idx);
}

char* WebSocketServer_impl::GetIpAddress(int32 conn_idx)
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}
