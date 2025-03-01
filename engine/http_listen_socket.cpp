#include "http_listen_socket.h"
#include "socket_ops.h"
#include "scheduler.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

HTTPListenSocket::HTTPListenSocket(
	const char* listen_address, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	: TCPListenSocket(listen_address, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	sub_type_ = TCP_TYPE_HTTP;
}

HTTPListenSocket::~HTTPListenSocket()
{

}

bool HTTPListenSocket::Start()
{
	return TCPListenSocket::Start();
}
