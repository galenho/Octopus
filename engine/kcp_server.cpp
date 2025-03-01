#include "kcp_server_impl.h"
#include "kcp_server.h"

KCPServer::KCPServer()
{
	imp_ = new KCPServer_impl();
}

KCPServer::~KCPServer()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool KCPServer::Start(const string& ip, uint16 port, const HandleInfo onconnected_handler, const HandleInfo onclose_handler, const HandleInfo onrecv_handler, uint32 sendbuffersize /*= 8192*/, uint32 recvbuffersize /*= 8192*/)
{
	return imp_->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
}

bool KCPServer::Close()
{
	return imp_->Close();
}

bool KCPServer::SendMsg( int32 conn_idx, const void* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

void KCPServer::Disconnect( int32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

char* KCPServer::GetIpAddress( int32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}
