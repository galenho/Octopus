#include "tcp_client.h"
#include "tcp_client_impl.h"


TCPClient::TCPClient()
{
	imp_ = new TCPClient_impl();
}

TCPClient::~TCPClient()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

uint32 TCPClient::Connect( const string ip, 
						 uint16 port,
						 const HandleInfo onconnected_handler,
						 const HandleInfo onclose_handler,
						 const HandleInfo onrecv_handler,
						 uint32 sendbuffersize, 
						 uint32 recvbuffersize)
{
	return imp_->Connect(ip, port, 
						 onconnected_handler, onclose_handler, onrecv_handler, 
						 sendbuffersize, recvbuffersize);
	

	return false;
}


uint32 TCPClient::ConnectEx( const string ip, 
						  uint16 port,
						  const HandleInfo onconnected_handler,
						  const HandleInfo onclose_handler,
						  const HandleInfo onrecv_handler,
						  uint32 sendbuffersize /*= 8192*/, 
						  uint32 recvbuffersize /*= 8192*/)
{
	return imp_->ConnectEx(ip, port, 
			onconnected_handler, onclose_handler, onrecv_handler, 
			sendbuffersize, recvbuffersize);
}

void TCPClient::Disconnect( int32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

bool TCPClient::SendMsg( int32 conn_idx, char* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

char* TCPClient::GetIpAddress( int32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}
