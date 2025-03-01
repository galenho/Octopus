#include "http_server_impl.h"
#include "http_socket.h"

HTTPServer_impl::HTTPServer_impl( )
{
	listen_socket_ = NULL;
}

HTTPServer_impl::~HTTPServer_impl()
{
	if (listen_socket_)
	{
		delete listen_socket_;
		listen_socket_ = NULL;
	}
}

bool HTTPServer_impl::Start(const string& ip,
	uint16 port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize,
	uint32 recvbuffersize)
{
	listen_socket_ = new HTTPListenSocket(ip.c_str(),
		port,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize);

	return listen_socket_->Start();
}

bool HTTPServer_impl::Close()
{
	listen_socket_->Close();
	return true;
}

void HTTPServer_impl::Disconnect( int32 conn_idx )
{
	return SocketMgr::get_instance()->Disconnect(conn_idx);
}

char* HTTPServer_impl::GetIpAddress(int32 conn_idx)
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}

void HTTPServer_impl::Clear(int32 conn_idx)
{
	hash_map<uint32, Socket*>& socket_map = SocketMgr::get_instance()->socket_map_;
	hash_map<uint32, Socket*>::iterator it = socket_map.find(conn_idx);
	if (it != socket_map.end())
	{
		HTTPSocket* s = (HTTPSocket*)it->second;
		s->response_.clear();
	}
}

void HTTPServer_impl::SetError(int32 conn_idx, int code, string& msg)
{
	hash_map<uint32, Socket*>& socket_map = SocketMgr::get_instance()->socket_map_;
	hash_map<uint32, Socket*>::iterator it = socket_map.find(conn_idx);
	if (it != socket_map.end())
	{
		HTTPSocket* s = (HTTPSocket*)it->second;
		s->response_.set_error(code, msg);
	}
}

void HTTPServer_impl::SetBody(int32 conn_idx, string& body)
{
	hash_map<uint32, Socket*>& socket_map = SocketMgr::get_instance()->socket_map_;
	hash_map<uint32, Socket*>::iterator it = socket_map.find(conn_idx);
	if (it != socket_map.end())
	{
		HTTPSocket* s = (HTTPSocket*)it->second;
		s->response_.set_body(body);
	}
}

void HTTPServer_impl::Response(int32 conn_idx)
{
	hash_map<uint32, Socket*>& socket_map = SocketMgr::get_instance()->socket_map_;
	hash_map<uint32, Socket*>::iterator it = socket_map.find(conn_idx);
	if (it != socket_map.end())
	{
		HTTPSocket* s = (HTTPSocket*)it->second;
		s->response_.gen_response();
		REF_ADD(s);
		string data = s->response_._res_bytes.str();
		int len = (int)data.length();
		s->Send(data.c_str(), len);
		s->response_.clear();
	}
}
