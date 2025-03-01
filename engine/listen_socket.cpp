#include "listen_socket.h"
#include "socket_ops.h"

ListenSocket::ListenSocket(
	const char* listen_address, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
{
	port_ = port;

	address_.sin_family = AF_INET;
	address_.sin_port = ntohs((u_short)port_);
	address_.sin_addr.s_addr = htonl(INADDR_ANY);

	struct hostent* hostname = gethostbyname(listen_address);
	if (hostname != 0)
		memcpy(&address_.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);

	onconnected_handler_ = onconnected_handler;
	onclose_handler_ = onclose_handler;
	onrecv_handler_ = onrecv_handler;

	sendbuffersize_ = sendbuffersize;
	recvbuffersize_ = recvbuffersize;

	is_listen_ = true;
}

ListenSocket::~ListenSocket()
{
	Close();
}

void ListenSocket::Close()
{
	is_listen_ = false;
	SocketOps::CloseSocket(socket_);
}

int ListenSocket::GetFd()
{
	return (int)socket_;
}
