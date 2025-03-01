#include "kcp_client.h"
#include "kcp_client_impl.h"

KCPClient::KCPClient()
{
	imp_ = new KCPClient_impl();
}

KCPClient::~KCPClient()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

uint32 KCPClient::Connect( const string& ip, 
						 uint16 port,
						 uint16& local_port,
						 const HandleInfo onconnected_handler,
						 const HandleInfo onclose_handler,
						 const HandleInfo onrecv_handler,
						 uint32 sendbuffersize, 
						 uint32 recvbuffersize)
{
	return imp_->Connect(ip, port, local_port,
						 onconnected_handler, onclose_handler, onrecv_handler, 
						 sendbuffersize, recvbuffersize);
	

	return false;
}

void KCPClient::Disconnect( int32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

bool KCPClient::SendMsg( int32 conn_idx, const void* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

char* KCPClient::GetIpAddress( int32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}


