/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"
#include "guard.h"
#include "scheduler.h"
#include "lua_fix.h"

#ifdef WIN32
#include "socket_mgr_win32.h"
#else
#include "socket_mgr_linux.h"
#endif

extern CLog* g_logger;

#ifdef WIN32
LPFN_CONNECTEX get_connect_ex(SOCKET fd)
{
	GUID guid = WSAID_CONNECTEX;

	LPFN_CONNECTEX ptr = NULL;
	DWORD bytes = 0;
	int ret = ::WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), &ptr, sizeof(ptr), &bytes, 0, 0);

	if (ret == SOCKET_ERROR)
	{
		ASSERT(false);
	}

	return ptr;
}

LPFN_ACCEPTEX get_accept_ex(SOCKET fd)
{
	GUID guid = WSAID_ACCEPTEX;

	LPFN_ACCEPTEX ptr = NULL;
	DWORD bytes = 0;
	if (::WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), &ptr, sizeof(ptr), &bytes, 0, 0) != 0)
	{
		ASSERT(false);
	}

	return ptr;
}

LPFN_GETACCEPTEXSOCKADDRS get_accept_addr(SOCKET fd)
{
	GUID guid = WSAID_GETACCEPTEXSOCKADDRS;

	LPFN_GETACCEPTEXSOCKADDRS ptr = NULL;
	DWORD bytes = 0;
	if (0 != WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), &ptr, sizeof(ptr), &bytes, NULL, NULL) != 0)
	{
		ASSERT(false);
	}

	return ptr;
}
#endif

#ifndef WIN32
Socket::Socket( SOCKET wakeup_fd )
{
	is_client_ = false;
	fd_ = wakeup_fd;
	memset(&m_client, 0, sizeof(m_client));

	isCloseSocket_ = false;
}
#endif

Socket::Socket(
			   SOCKET fd,
			   int32 conn_idx,
			   const HandleInfo onconnected_handler,
			   const HandleInfo onclose_handler,
			   const HandleInfo onrecv_handler,
			   uint32 sendbuffersize, 
			   uint32 recvbuffersize)
{
	is_listen_ = false;

	memset(&m_client, 0, sizeof(m_client));

	if (fd == 0) //˵����Client�˵�����
	{
		is_client_ = true;
	}
	else //˵����Server�˵�����
	{
		is_client_ = false;
	}

	fd_ = fd;
	conn_idx_ = conn_idx;

	onconnected_handler_ = onconnected_handler;
	onclose_handler_ = onclose_handler;
	onrecv_handler_ = onrecv_handler;

	// Allocate Buffers
	readBuffer.Allocate(recvbuffersize);
	recvbuffersize_ = recvbuffersize;

	writeBuffer.Allocate(sendbuffersize);

	status_ = socket_status_closed;
	is_sending_ = false;

	isCloseSocket_ = false;
	//PRINTF_INFO("new fd = %d, conn_idx = %d", fd_, conn_idx_);
}

Socket::~Socket()
{
	//PRINTF_INFO("delete fd = %d, conn_idx = %d", fd_, conn_idx_);
	if (is_client_)
	{
		SocketClientDeleteTask::Process(onconnected_handler_, onrecv_handler_, onclose_handler_);
	}
}

void Socket::Update(int64 cur_time)
{

}

string Socket::GetRemoteIP()
{
	char* ip = (char*)inet_ntoa(m_client.sin_addr);
	if (ip != NULL)
		return string(ip);
	else
		return string("noip");
}

#ifdef WIN32
bool Socket::PostReadEvent()
{
	//PRINTF_INFO("PostReadEvent fd = %d", GetFd());

	DWORD r_length = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.len = (ULONG)readBuffer.GetSpace();
	buf.buf = (char*)readBuffer.GetBuffer();

	read_event_.SetEvent(SOCKET_IO_EVENT_READ_COMPLETE);
	if (WSARecv(fd_, &buf, 1, &r_length, &flags, &read_event_.overlap_, 0) == SOCKET_ERROR)
	{
		int last_error = WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			//PRINTF_ERROR("PostReadEvent socket error %d", last_error);
			return false;
		}
	}

	REF_ADD(this); //����һ��

	return true;
}

bool Socket::PostSendEvent()
{
	//PRINTF_INFO("WriteData fd = %d", GetFd());

	if (writeBuffer.GetSize() > 0)
	{
		int send_len = writeBuffer.GetSize();
		if (send_len > 1000000) //1000K����
		{
			send_len = 1000000;
		}

		DWORD w_length = 0;
		DWORD flags = 0;

		// attempt to push all the data out in a non-blocking fashion.
		WSABUF buf;
		buf.len = send_len;
		buf.buf = (char*)writeBuffer.GetBufferStart();

		write_event_.SetEvent(SOCKET_IO_EVENT_WRITE_COMPLETE);
		int r = WSASend(fd_, &buf, 1, &w_length, flags, &write_event_.overlap_, 0);
		if (r == SOCKET_ERROR)
		{
			int last_error = WSAGetLastError();
			if (last_error != WSA_IO_PENDING)
			{
				//PRINTF_ERROR("WriteData socket error %d", last_error);
				return false;
			}
		}
	}

	is_sending_ = true;
	REF_ADD(this); //����һ��
	return true;
}

#else
bool Socket::PostEvent(uint32 events)
{
	int epoll_fd = SocketMgr::get_instance()->GetEpollFd();

	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.events = events | EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
	ev.data.ptr = this;

	// post actual event
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd_, &ev))
	{
		PRINTF_ERROR("Could not post event on fd %u", fd_);
		return false;
	}
	else
	{
		return true;
	}
}

bool Socket::WriteData()
{
	//-------------------------------------------------------------------------------------------------------------
	// galen:����Ҫһ���Է���д��������ߵ�����, ���ʧ�ܾ͵�EPOLLIN���¼���, ���л�����
	//-------------------------------------------------------------------------------------------------------------
	while (writeBuffer.GetSize() > 0)
	{
		int send_len = writeBuffer.GetSize();

		if (send_len > 1000000) //1000K����
		{
			send_len = 1000000;
		}

		int len = send(fd_, writeBuffer.GetBufferStart(), send_len, 0);
		if (len < 0)
		{
			if (errno == EINTR) //���͹����б��ж�, ������
			{
				continue;
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK) // ��socket�Ƿ�����ʱ,�緵�ش˴���,��ʾд�����������, �ȴ���һ��EPOLLOUT�¼��Ĵ���
			{
				//PRINTF_INFO("conn_idx = %d, send fail, system socket buff is full, errno = %d", GetConnectIdx(), errno)
				break;
			}
			else
			{
				//PRINTF_ERROR("send fail errno 1 = %d", errno);
				return false;
			}
		}
		else
		{
			writeBuffer.Remove(len); //Remove������߻��Զ�FillVector()

			if (len < send_len)
			{
				break;
			}
			else
			{
				// �����߼�, ��������
			}
		}
	}

	return true;
}
#endif

void Socket::Accept(sockaddr_in* address)
{
	memcpy(&m_client, address, sizeof(*address));

	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

#ifdef WIN32
	CreateIoCompletionPort((HANDLE)fd_, SocketMgr::get_instance()->GetCompletionPort(), (ULONG_PTR)this, 0);
#endif
}

void Socket::OnConnect(bool is_success)
{
	SocketConnectTask::Process(onconnected_handler_, conn_idx_, is_success);
}

void Socket::OnRead()
{
	
}

void Socket::OnDisconnect()
{
	SocketCloseTask::Process(onclose_handler_, conn_idx_);
}

void Socket::CloseSocket()
{
	if (!isCloseSocket_)
	{
		isCloseSocket_ = true;
		SocketOps::CloseSocket(fd_);
	}
}

void Socket::Disconnect()
{
#ifdef WIN32
	close_event_.SetEvent(SOCKET_IO_EVENT_CLOSE);
	PostQueuedCompletionStatus(SocketMgr::get_instance()->GetCompletionPort(), 0, (ULONG_PTR)this, &close_event_.overlap_);
#else
	SocketEvent event;
	event.s = this;
	event.customized_events = SOCKET_IO_EVENT_CLOSE;
	SocketMgr::get_instance()->event_queue_.push_back(event);
	SocketMgr::get_instance()->WakeUp();
#endif

	AddRef(); //����һ��
}

bool Socket::Send(const void* buff, uint32 len)
{
	if (status_ != socket_status_connected)
	{
		return true;
	}

	writeBuffer.Write(buff, len);
	if (!is_sending_)
	{
#ifdef WIN32
		bool ret = PostSendEvent();
		if (!ret)
		{
			Disconnect();
		}
#else
		is_sending_ = true;
		WriteData();
		PostEvent(EPOLLOUT);
#endif
	}

	return true;
}

bool Socket::SendMsg(const void* buff, uint32 len)
{
	if (status_ != socket_status_connected)
	{
		return true;
	}

	writeBuffer.WriteMsg(&len, 4, buff, len); // ƴ��
	if (!is_sending_)
	{
#ifdef WIN32
		bool ret = PostSendEvent();
		if (!ret)
		{
			Disconnect();
		}
#else
		is_sending_ = true;
		WriteData();
		PostEvent(EPOLLOUT);
#endif
	}

	return true;
}

