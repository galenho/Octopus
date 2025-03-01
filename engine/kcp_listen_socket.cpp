#include "kcp_listen_socket.h"
#include "socket_ops.h"
#include "scheduler.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

KCPListenSocket::KCPListenSocket(
	const char* listen_address, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:UDPListenSocket(listen_address, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	sub_type_ = UDP_TYPE_KCP;
}

KCPListenSocket::~KCPListenSocket()
{
	
}

bool KCPListenSocket::Start()
{
	return UDPListenSocket::Start();
}