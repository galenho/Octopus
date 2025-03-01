#include "http_server_impl.h"
#include "http_server.h"


HTTPServer::HTTPServer( )
{
	imp_ = new HTTPServer_impl();
}
HTTPServer::~HTTPServer()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool HTTPServer::Start(
	const string ip, uint16 port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize, uint32 recvbuffersize)
{
	return imp_->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
}

bool HTTPServer::Close()
{
	return imp_->Close();
}

void HTTPServer::Disconnect( int32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

char* HTTPServer::GetIpAddress( int32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}

void HTTPServer::Clear(int32 conn_idx)
{
	imp_->Clear(conn_idx);
}

void HTTPServer::SetError(int32 conn_idx, int code, string& msg)
{
	imp_->SetError(conn_idx, code, msg);
}

void HTTPServer::SetBody(int32 conn_idx, string& body)
{
	imp_->SetBody(conn_idx, body);
}

void HTTPServer::Response(int32 conn_idx)
{
	imp_->Response(conn_idx);
}
