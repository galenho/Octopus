#include "tcp_socket.h"
#include "socket_ops.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

TCPSocket::TCPSocket(
	SOCKET fd, 
	int32 conn_idx, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:Socket(fd, conn_idx, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	socket_type_ = SOCKET_TYPE_TCP;
	sub_type_ = TCP_TYPE_NORMAL;

	if (fd_ == 0)
	{
		fd_ = SocketOps::CreateTCPFileDescriptor();
	}
}


bool TCPSocket::Connect(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
	{
		OnConnect(false);
		return false;
	}

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	SocketOps::Blocking(fd_);
	if (connect(fd_, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
	{
		OnConnect(false);
		return false;
	}

	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
#ifdef WIN32
	if (CreateIoCompletionPort((HANDLE)fd_, SocketMgr::get_instance()->GetCompletionPort(), (ULONG_PTR)this, 0) == 0)
	{
		OnConnect(false);
		return false;
	}
#endif

	return true;
}

#ifdef WIN32
bool TCPSocket::ConnectEx(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
	{
		OnConnect(false);
		return false;
	}

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
	if (CreateIoCompletionPort((HANDLE)fd_, SocketMgr::get_instance()->GetCompletionPort(), (ULONG_PTR)this, 0) == 0)
	{
		OnConnect(false);
		return false;
	}
	//-------------------------------------------------------------
	sockaddr_in a;
	memset(&a, 0, sizeof(a));
	a.sin_family = AF_INET;
	int ret = ::bind(fd_, (const sockaddr*)&a, sizeof(a));
	DWORD last_error = ::WSAGetLastError();
	//-------------------------------------------------------------
	if (ret != 0)
	{
		OnConnect(false);
		PRINTF_ERROR("ConnectEx bind fail, err = %d", last_error);
		return false;
	}
	else //绑定成功
	{
		connect_event_.SetEvent(SOCKET_IO_EVENT_CONNECT_COMPLETE);

		LPFN_CONNECTEX connect_ex = get_connect_ex(fd_);
		PVOID lpSendBuffer = NULL;
		DWORD dwSendDataLength = 0;
		DWORD dwBytesSent = 0;
		bool result = connect_ex(fd_, (const sockaddr*)&m_client, sizeof(m_client), lpSendBuffer, dwSendDataLength, &dwBytesSent, &connect_event_.overlap_);

		if (!result)
		{
			last_error = ::WSAGetLastError();
			if (last_error != ERROR_SUCCESS && last_error != ERROR_IO_PENDING)  // 调用失败
			{
				OnConnect(false);
				return false;
			}
		}
	}

	return true;
}
#else
bool TCPSocket::ConnectEx(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
	{
		OnConnect(false);
		return false;
	}

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	int ret = connect(fd_, (const sockaddr*)&m_client, sizeof(m_client));
	if (ret == -1) //直接连接成功是不可能的， 因为它是异步connect
	{
		return true;
	}

	return false;
}
#endif

void TCPSocket::OnRead()
{
	uint32 packet_len = GetReadBuffer().GetSize();
	char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());

	//---------------------------------------------------------------------------------------------
	// 解包 = uint32 + content_len，并移除
	uint32 cursor = 0;
	uint32 len = 0;
	while (packet_len > cursor + 4) //包体不能为0，所以要 >
	{
		// (1) 解析包头
		len = *((uint32*)(buffer_start + cursor));

		// 判断一下包头的内容长度是否符合限定范围
		if (len > 0 && len <= recvbuffersize_ - 4)
		{
			
		}
		else
		{
			PRINTF_ERROR("single packet length error! fd = %d, conn_idx = %d, len = %d, recv size = %d", fd_, conn_idx_, len, recvbuffersize_);
			Disconnect();

			break;
		}

		// (2) 解析包体
		if (packet_len >= cursor + 4 + len) //解包成功
		{
			cursor += 4;

			SocketReadTask::Process(onrecv_handler_, conn_idx_, buffer_start + cursor, len);

			cursor += len;
		}
		else //等待包体的后续部分
		{
			break;
		}
	}

	if (cursor > 0)
	{
		GetReadBuffer().Remove(cursor);
	}
}
