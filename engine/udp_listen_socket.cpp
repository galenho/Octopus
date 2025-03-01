#include "udp_listen_socket.h"
#include "socket_ops.h"
#include "scheduler.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

UDPListenSocket::UDPListenSocket(
	const char* listen_address, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:ListenSocket(listen_address, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	socket_type_ = SOCKET_TYPE_UDP;
	sub_type_ = UDP_TYPE_NORMAL;

#ifdef WIN32
	socket_ = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	socket_ = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	
	SocketOps::ReuseAddr(socket_);
	SocketOps::Blocking(socket_);
}

UDPListenSocket::~UDPListenSocket()
{
	
}

bool UDPListenSocket::Start()
{
	int ret = ::bind(socket_, (const sockaddr*)&address_, sizeof(address_));
	if (ret != 0)
	{
		PRINTF_ERROR("Bind unsuccessful on port %u.", port_);
		return false;
	}

#ifdef WIN32
	//监听端口与完成端口绑定
	CreateIoCompletionPort((HANDLE)socket_, SocketMgr::get_instance()->GetCompletionPort(), (ULONG_PTR)this, 0);
#else
	//监听端口与完成端口绑定
	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.events = EPOLLIN; //使用LT模式
	ev.data.ptr = this;

	int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_, &ev))
	{
		PRINTF_ERROR("epoll", "Could not add event to epoll set on fd %u ", socket_);
		return false;
	}
#endif

#ifdef WIN32
	// 先投递一个请求
	if (!PostAccept())
	{
		return false;
	}
#endif

	return true;
}

#ifdef WIN32
bool UDPListenSocket::PostAccept()
{
	DWORD flags = 0;
	DWORD r_length = 0;
	WSABUF buf;
	buf.len = sizeof(buff_);
	buf.buf = buff_;

	int addr_len = sizeof(client_addr_);
	read_event_.SetEvent(SOCKET_IO_EVENT_ACCEPT_UDP);
	read_event_.fd_ = socket_;

	if (WSARecvFrom(socket_, &buf, 1, &r_length, &flags, (sockaddr*)&(client_addr_), &addr_len, &read_event_.overlap_, 0) == SOCKET_ERROR)
	{
		int last_error = WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			return false;
		}
	}

	return true;
}
#endif