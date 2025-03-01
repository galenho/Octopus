#include "tcp_listen_socket.h"
#include "socket_ops.h"
#include "scheduler.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

TCPListenSocket::TCPListenSocket(
	const char* listen_address, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:ListenSocket(listen_address, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	socket_type_ = SOCKET_TYPE_TCP;
	sub_type_ = TCP_TYPE_NORMAL;

#ifdef WIN32
	socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
#endif

	SocketOps::ReuseAddr(socket_);
	SocketOps::Blocking(socket_);
}

TCPListenSocket::~TCPListenSocket()
{
	
}

bool TCPListenSocket::Start()
{
	int ret = ::bind(socket_, (const sockaddr*)&address_, sizeof(address_));
	if (ret != 0)
	{
		PRINTF_ERROR("Bind unsuccessful on port %u.", port_);
		return false;
	}

#ifdef WIN32
	buff_len_ = 0;
	accept_ex_ = NULL;
	accept_addrs_ = NULL;

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

	ret = listen(socket_, 5);
	if (ret != 0)
	{
		PRINTF_ERROR("Unable to listen on port %u.", port_);
		return false;
	}

#ifdef WIN32
	accept_ex_ = get_accept_ex(socket_);
	accept_addrs_ = get_accept_addr(socket_);

	// 先投递一个请求
	if (!PostAccept())
	{
		return false;
	}
#endif

	return true;
}

#ifdef WIN32
bool TCPListenSocket::PostAccept()
{
	if (!is_listen_)
	{
		return false;
	}

	// 投递一个accept请求
	SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	int addr_len = sizeof(sockaddr_in) + 16;
	accept_event_.SetEvent(SOCKET_IO_EVENT_ACCEPT_TCP);
	accept_event_.fd_ = client_socket;

	bool ret = accept_ex_(socket_, client_socket, buff_, 0, addr_len, addr_len, &buff_len_, &accept_event_.overlap_);
	if (!ret)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			PRINTF_ERROR("PostAccept error = %d", WSAGetLastError());
			return false;
		}
	}

	return true;
}
#endif