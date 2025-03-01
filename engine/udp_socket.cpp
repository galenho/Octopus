#include "udp_socket.h"
#include "socket_defines.h"
#include "socket_ops.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

extern CLog* g_logger;

UDPSocket::UDPSocket(
	SOCKET fd, 
	int32 conn_idx, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:Socket(fd, conn_idx, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	socket_type_ = SOCKET_TYPE_UDP;
	sub_type_ = UDP_TYPE_NORMAL;

	if (fd_ == 0)
	{
		fd_ = SocketOps::CreateUDPFileDescriptor();
	}

	is_udp_connected_ = false;
}


UDPSocket::~UDPSocket()
{
	
}

bool UDPSocket::Connect(const char* address, uint16 port, uint16& local_port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	sockaddr_in a;
	a.sin_family = AF_INET;
	a.sin_port = htons((u_short)local_port);
	a.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = ::bind(fd_, (const sockaddr*)&a, sizeof(a));
	if (ret != 0)
	{
		PRINTF_ERROR("Bind unsuccessful on port:%d", local_port);
		return false;
	}

#ifdef WIN32
	int len = sizeof(a);
#else
	socklen_t len = sizeof(a);
#endif

	getsockname(fd_, (sockaddr*)&a, &len);
	local_port = ntohs(a.sin_port); // 获取端口号

	if (connect(fd_, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
	{
		return false;
	}

	SocketOps::Nonblocking(fd_);
	// set common parameters on the file descriptor
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
#ifdef WIN32
	if (CreateIoCompletionPort((HANDLE)fd_, SocketMgr::get_instance()->GetCompletionPort(), (ULONG_PTR)this, 0) == 0)
	{
		return false;
	}
#endif

	return true;
}

void UDPSocket::OnRead()
{
	uint32 packet_len = GetReadBuffer().GetSize();
	char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());

	if (!is_udp_connected_)
	{
		if (packet_len == sizeof(RepServerPort))
		{
			RepServerPort* msg = static_cast<RepServerPort*>((void*)(buffer_start));
			char src_port_str[8] = "port";
			if (strcmp(msg->flags, src_port_str) == 0)
			{
				m_client.sin_port = ntohs((u_short)msg->port);
				if (connect(fd_, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
				{
					OnConnect(false);

					Disconnect(); // 断开连接
				}
				else
				{
					is_udp_connected_ = true;

					// 连接成功
					OnConnect(true);
				}
			}
		}
		else
		{
			Disconnect(); // 断开连接
		}

		GetReadBuffer().Remove(packet_len);
	}
	else
	{
		// 需要解包 = uint32 + content_len，并移除
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
				PRINTF_ERROR("read packet is error! fd = %d, conn_idx = %d", fd_, conn_idx_);
				Disconnect(); // 断开连接

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
}

void UDPSocket::Disconnect()
{
	Socket::Disconnect();
}
