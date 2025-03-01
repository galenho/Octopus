#include "websocket_server_impl.h"
#include "websocket_server.h"


WebSocketServer::WebSocketServer( )
{
	imp_ = new WebSocketServer_impl();
}

WebSocketServer::~WebSocketServer()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool WebSocketServer::Start(
	const string ip, uint16 port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize, uint32 recvbuffersize)
{
	return imp_->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
}

bool WebSocketServer::Close()
{
	return imp_->Close();
}

bool WebSocketServer::SendMsg( int32 conn_idx, char* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

void WebSocketServer::Disconnect( int32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

char* WebSocketServer::GetIpAddress( int32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}