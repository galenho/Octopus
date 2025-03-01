#include "kcp_socket.h"
#include "socket_defines.h"
#include "socket_ops.h"
#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

extern CLog* g_logger;

KCPSocket::KCPSocket(
	SOCKET fd, 
	int32 conn_idx, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	:UDPSocket(fd, conn_idx, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	sub_type_ = UDP_TYPE_KCP;

	p_kcp_ = NULL;

	p_kcp_ = ikcp_create(conn_idx, (void*)this);
	p_kcp_->output = &KCPSocket::udp_output;

	// 启动快速模式
	// 第二个参数 nodelay-启用以后若干常规加速将启动
	// 第三个参数 interval为内部处理时钟，默认设置为 10ms
	// 第四个参数 resend为快速重传指标，设置为2
	// 第五个参数 为是否禁用常规流控，这里禁止
	ikcp_nodelay(p_kcp_, 1, 10, 2, 1); // 设置成1次ACK跨越直接重传, 这样反应速度会更快. 内部时钟5毫秒.
}


KCPSocket::~KCPSocket()
{
	if (p_kcp_)
	{
		ikcp_release(p_kcp_);
	}
}

void KCPSocket::Update(int64 cur_time)
{
	if (is_udp_connected_ && p_kcp_)
	{
		ikcp_update(p_kcp_, (uint32)cur_time);
	}
}

void KCPSocket::OnRead()
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
		ikcp_input(p_kcp_, (const char*)(buffer_start), packet_len);

		while (true)
		{
			int kcp_len = ikcp_peeksize(p_kcp_);
			if (kcp_len > 0)
			{
				if (kcp_len > g_buff_len)
				{
					PRINTF_ERROR("ikcp_peeksize = %d, %d > %d", kcp_len, kcp_len, g_buff_len);

					// 重新分配内存
					delete[] g_buff;
					g_buff = NULL;

					g_buff_len = kcp_len;
					g_buff = new char[g_buff_len];
				}

				int len = ikcp_recv(p_kcp_, g_buff, g_buff_len);
				if (len > 0)
				{
					SocketReadTask::Process(onrecv_handler_, conn_idx_, (char*)g_buff, len);
					continue;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		GetReadBuffer().Remove(packet_len);
	}
}

bool KCPSocket::SendMsg(const void* buff, uint32 len)
{
	int send_ret = ikcp_send(p_kcp_, (const char*)buff, len);
	if (send_ret < 0)
	{
		return false;
	}
	else
	{
		ikcp_flush(p_kcp_);
		return true;
	}
}

int  KCPSocket::udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
{
	((KCPSocket*)user)->on_udp_package_send(buf, len);
	return 0;
}

void KCPSocket::on_udp_package_send(const char* buf, int len)
{
	Socket::Send(buf, len);
}
