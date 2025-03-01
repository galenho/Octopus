/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"

#ifndef WIN32

#include "netinet/tcp.h"
#include "guard.h"
#include "tcp_listen_socket.h"
#include "udp_listen_socket.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "kcp_socket.h"
#include "http_socket.h"
#include "web_socket.h"

void HandleAcceptTCPComplete(TCPListenSocket* s)
{
	int len = 0;
	struct sockaddr_in tempAddress;

	SOCKET aSocket = accept(s->socket_, (sockaddr*)&tempAddress, (socklen_t*)&len);
	if (aSocket == -1)
	{
		printf("accept error\n");
		return;
	}

	SocketMgr::get_instance()->Accept(aSocket,
		tempAddress,
		s->onconnected_handler_,
		s->onclose_handler_,
		s->onrecv_handler_,
		s->sendbuffersize_,
		s->recvbuffersize_,
		s->sub_type_);
}

void HandleAcceptUDPComplete(UDPListenSocket* s)
{
	sockaddr_in	client_addr_;
	socklen_t addr_len = sizeof(client_addr_);
	int bytes = recvfrom(s->socket_, s->buff_, sizeof(s->buff_), 0, (sockaddr*)&(client_addr_), &addr_len);
	if (bytes == 8)
	{
		uint8* buffer_start = (uint8*)(s->buff_);
		char src_conn_str[8] = "connect";
		char dst_conn_str[8];
		memcpy(dst_conn_str, (uint8*)buffer_start, 8);
		if (strcmp(src_conn_str, dst_conn_str) == 0)
		{
			// ģ��tcp��accept, ����һ���µ�һ��udp socket
			uint16 out_port = 0;
			bool ret = SocketMgr::get_instance()->AcceptUDP(client_addr_,
				s->onconnected_handler_,
				s->onclose_handler_,
				s->onrecv_handler_,
				s->sendbuffersize_,
				s->recvbuffersize_,
				s->sub_type_,
				out_port);

			// �ط�һ�����Ӱ���UDP�ͻ���, ���߿ͻ����µĶ˿ں�
			if (ret)
			{
				RepServerPort rep_msg;
				rep_msg.port = out_port;
				sendto(s->socket_, (char*)&rep_msg, sizeof(rep_msg), 0, (sockaddr*)&client_addr_, sizeof(client_addr_));
			}
		}
	}
}

void HandleConnect(Socket* s, bool is_success)
{
	if (is_success)
	{
		s->status_ = socket_status_connected;

		bool ret = s->PostEvent(EPOLLIN);
		if (ret)
		{
			if (s->GetSocketType() == SOCKET_TYPE_TCP && s->GetSubType() == TCP_TYPE_WEB_SOCKET)
			{
				// �ȴ���֤��ͷ��Ϣ����OnConnect
			}
			else
			{
				s->OnConnect(true);
			}
		}
		else
		{
			s->CloseSocket();
			s->status_ = socket_status_closed;
			SocketMgr::get_instance()->RemoveSocket(s->GetConnectIdx());

			s->OnConnect(false);
		}
	}
	else
	{
		s->CloseSocket();
		s->status_ = socket_status_closed;
		SocketMgr::get_instance()->RemoveSocket(s->GetConnectIdx());

		s->OnConnect(false);
	}
}

void HandleReadComplete(Socket* s)
{
	if (s->status_ != socket_status_connected && s->status_ != socket_status_closing)
	{
		return;
	}

	//---------------------------------------------------------------------------------------------------------------
	// ������ѭ����ȡ
	while(true)
	{
		int space = (int)(s->GetReadBuffer().GetSpace());
		if (space == 0)
		{
			// �������ǲ�����������
			//PRINTF_ERROR("s->GetFd() = %d, space == 0", s->GetFd());
			return;
		}

		int len = recv(s->GetFd(), s->GetReadBuffer().GetBuffer(), space, 0); //�ȶ���ϵͳ������
		if (len < 0)
		{
			// �����Ƿ�������ģʽ,���Ե�errnoΪEAGAINʱ,��ʾ��ǰ�������������ݿɶ�
			// ������͵����Ǹô��¼��Ѷ���
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//galen: ���ù�, ����ճ��������, recv��һ�ΰѵڶ��εİ�������
				break;
			}
			else
			{
				//PRINTF_ERROR("conn_idx = %d, len = %d, errno = %d", s->GetConnectIdx(), len, errno);
				SocketMgr::get_instance()->CloseSocket(s);
				return;
			}
		}
		else if(len == 0) //Socket�ر��¼�
		{
			//PRINTF_ERROR("conn_idx = %d, len = %d, errno = %d", s->GetConnectIdx(), len, errno);
			
			if (s->GetSocketType() == SOCKET_TYPE_TCP)
			{
				//PRINTF_INFO("CloseSocket fd = %d", s->GetFd());
				SocketMgr::get_instance()->CloseSocket(s);
			}
			else //UDP
			{
				if (((UDPSocket*)s)->is_udp_connected_)
				{
					SocketMgr::get_instance()->CloseSocket(s);
				}
				else
				{
					s->CloseSocket();
					s->status_ = socket_status_closed;
					s->OnConnect(false);
				}
			}

			return;
		}
		else
		{
			// ��������
			//PRINTF_INFO("conn_idx = %d, recv len = %d", s->GetConnectIdx(), len);

			s->GetReadBuffer().IncrementWritten(len);
			s->OnRead();
		}
	}

	//---------------------------------------------------------------------
	if (s->GetWriteBuffer().GetSize() > 0)
	{
		s->PostEvent(EPOLLOUT); //���д��������������ݵĻ����ѿ���Ȩ���д
	}
}

void HandleWrite(Socket* s)
{
	if (s->status_ == socket_status_connected)
	{
		bool ret = s->WriteData();
		if (!ret)
		{
			SocketMgr::get_instance()->CloseSocket(s);
		}
		else
		{
			if (s->GetWriteBuffer().GetSize() == 0)
			{
				s->is_sending_ = false;
				s->PostEvent(EPOLLIN);
			}
			else
			{
				// ���ҲҪ�е�EPOLLIN״̬, ��ҲҪ����ԭ����EPOLLOUT״̬, 
				// ��Ϊ������Ͷ˶�����ϵͳ��д����������, �����շ���û�ս������ݣ����ҽ��շ���ϵͳд������������
				// Ҳ�ڵȴ��Է�recv�������ֽڲŻ᷵��EPOLLOUT�¼�, �Ǿ�������
				s->PostEvent(EPOLLIN | EPOLLOUT);
			}
		}
	}
	else if (s->status_ == socket_status_closing) 
	{
		SocketMgr::get_instance()->CloseSocket(s);
	}
}

void HandleClose(Socket* s)
{
	if (s->status_ == socket_status_connected)
	{
		s->status_ = socket_status_closing;
		if (s->GetSocketType() == SOCKET_TYPE_TCP)
		{
			::shutdown(s->GetFd(), SHUT_WR);
		}
		else
		{
			SocketMgr::get_instance()->CloseSocket(s);
		}		
	}
}

//----------------------------------------------------------------------------------------------------
initialiseSingleton(SocketMgr);
SocketMgr::SocketMgr()
{
	auto_conn_idx_ = 1;
	epoll_fd_ = 0;
	wakeup_s_ = NULL;
	is_wakeup_doing_ = false;
}

SocketMgr::~SocketMgr()
{	
	if (wakeup_s_)
	{
		delete wakeup_s_;
		wakeup_s_ = NULL;
	}
}

bool SocketMgr::Init()
{
	epoll_fd_ = epoll_create(SOCKET_HOLDER_SIZE);

	if (epoll_fd_ == -1)
	{
		PRINTF_ERROR("Could not create epoll fd (/dev/epoll).");
		return false;
	}

	int wakeup_fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (wakeup_fd < 0)
	{
		return false;
	}

	wakeup_s_ = new Socket(wakeup_fd);

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.ptr = wakeup_s_;
	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, wakeup_s_->GetFd(), &event);
	if (ret < 0)
	{
		return false;
	}

	return true;
}

bool SocketMgr::Close()
{
	if (wakeup_s_)
	{
		epoll_event ev = { 0, { 0 } };
		epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, wakeup_s_->GetFd(), &ev);

		close(wakeup_s_->GetFd());
	}

	if (epoll_fd_)
	{
		close(epoll_fd_);
	}

	return true;
}

void SocketMgr::WakeUp()
{
	if (wakeup_s_ && !is_wakeup_doing_)
	{
		uint64 one = 1;
		::write(wakeup_s_->GetFd(), &one, sizeof(one));

		is_wakeup_doing_ = true;
	}
}

uint32 SocketMgr::MakeGeneralConnID()
{
	int32 conn_idx = INVALID_INDEX;

	while (true)
	{
		hash_map<uint32, Socket*>::iterator it = socket_map_.find(auto_conn_idx_);
		if (it != socket_map_.end())
		{
			++auto_conn_idx_;
			if (auto_conn_idx_ == 1000000)
			{
				auto_conn_idx_ = 1; //��ͷ��ʼ
			}
		}
		else
		{
			break;
		}
	}

	conn_idx = auto_conn_idx_;
	++auto_conn_idx_;
	if (auto_conn_idx_ == INVALID_INDEX)
	{
		auto_conn_idx_ = 1;
	}

	return conn_idx;
}

int SocketMgr::GetEpollFd()
{
	return epoll_fd_;
}

void SocketMgr::HandleDelayEvent()
{
	SocketEvent event;
	while (event_queue_.pop(event))
	{
		Socket* s = event.s;

		if (event.customized_events == SOCKET_IO_EVENT_CLOSE)
		{
			HandleClose(s);
		}

		REF_RELEASE(s);
	}
}

int SocketMgr::EventLoop(int64 timeout_time)
{
	Update(getMSTime());

	while (true)
	{
		int64 cur_time = getMSTime();
		int64 timeout = timeout_time - cur_time;
		if (timeout <= 0)
		{
			break;
		}

		int32 fd_count = epoll_wait(epoll_fd_, events, THREAD_EVENT_SIZE, (int32)timeout);

		//PRINTF_INFO("epoll_fd = %d , epoll_wait fd_count = %d", epoll_fd, fd_count);
		//PRINTF_INFO("---------------------------------------------------");

		if (fd_count == 0) // ��ʱ��
		{
			break;
		}
		else if (fd_count > 0)
		{
			for (int i = 0; i < fd_count; i++)
			{
				uint32 event = events[i].events;
				SocketBase* socket = (Socket*)(events[i].data.ptr);

				//PRINTF_INFO("epoll_wait events = %d", events[i].events);

				if (socket == wakeup_s_) //�����¼�
				{
					//PRINTF_INFO("wakeup");
					if (event & EPOLLIN)
					{
						uint64 one = 1;
						uint32 size = read(wakeup_s_->GetFd(), &one, sizeof(one));
						if (size == sizeof(one))
						{
							HandleDelayEvent();
							is_wakeup_doing_ = false;
						}
					}

					continue;
				}
				else if (socket->is_listen_) //����
				{
					if (socket->socket_type_ == SOCKET_TYPE_TCP)
					{
						TCPListenSocket* s = (TCPListenSocket*)socket;
						HandleAcceptTCPComplete(s);
					}
					else if(socket->socket_type_ == SOCKET_TYPE_UDP)
					{
						UDPListenSocket* s = (UDPListenSocket*)socket;
						HandleAcceptUDPComplete(s);
					}
					
					continue;
				}
				else
				{
					Socket* s = (Socket*)socket;
					REF_ADD(s);

					if (event & EPOLLHUP || event & EPOLLERR)
					{
						if (s->status_ == socket_status_connecting)
						{
							HandleConnect(s, false);
						}
						else
						{
							SocketMgr::get_instance()->CloseSocket(s);
						}
					}
					else
					{
						if (event & EPOLLIN)
						{
							HandleReadComplete(s);
						}

						if (event & EPOLLOUT)
						{
							if (s->status_ == socket_status_connecting)
							{
								HandleConnect(s, true);
							}
							else
							{
								HandleWrite(s);
							}
						}
					}
			
					REF_RELEASE(s);
				}
			}
		}
		else
		{
			// �д���
			break;
		}
	}
	
	return 0;
}

void SocketMgr::Update( int64 cur_time )
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.begin();
	for (; it != socket_map_.end(); it++)
	{
		Socket* s = it->second;
	
		REF_ADD(s);
		s->Update(cur_time);
		REF_RELEASE(s);
	}
}

void SocketMgr::Accept( SOCKET aSocket, 
						sockaddr_in& address, 
						const HandleInfo onconnected_handler,
						const HandleInfo onclose_handler,
						const HandleInfo onrecv_handler,
						uint32 sendbuffersize, 
						uint32 recvbuffersize,
						int sub_type)
{
	Socket* s = NULL;
	if (sub_type == TCP_TYPE_NORMAL)
	{
		s = new TCPSocket(
			aSocket,
			MakeGeneralConnID(),
			onconnected_handler,
			onclose_handler,
			onrecv_handler,
			sendbuffersize,
			recvbuffersize);
	}
	else if (sub_type == TCP_TYPE_HTTP)
	{
		s = new HTTPSocket(
			aSocket,
			MakeGeneralConnID(),
			onconnected_handler,
			onclose_handler,
			onrecv_handler,
			sendbuffersize,
			recvbuffersize);
	}
	else
	{
		s = new WebSocket(
			aSocket,
			MakeGeneralConnID(),
			onconnected_handler,
			onclose_handler,
			onrecv_handler,
			sendbuffersize,
			recvbuffersize);
	}
	
	AddSocket(s); //����socket�б�

	s->status_ = socket_status_connected;

	// Add epoll event based on socket activity.
	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.events = (s->GetWriteBuffer().GetSize()) ? EPOLLOUT : EPOLLIN;
	ev.events = ev.events | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
	ev.data.ptr = s;

	int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, aSocket, &ev))
	{
		PRINTF_ERROR("epoll", "Could not add event to epoll set on fd %u ", s->GetFd());
		
		s->CloseSocket();
		RemoveSocket(s->GetConnectIdx());
	}
	else
	{
		s->Accept(&address);

		if (s->GetSocketType() == SOCKET_TYPE_TCP && s->GetSubType() == TCP_TYPE_WEB_SOCKET)
		{
			// �ȴ���֤��ͷ��Ϣ����OnConnect
		}
		else
		{
			s->OnConnect(true);
		}
	}
}

uint32 SocketMgr::Connect(const string& ip, uint16 port,
						  const HandleInfo onconnected_handler,
						  const HandleInfo onclose_handler,
						  const HandleInfo onrecv_handler,
						  uint32 sendbuffersize, 
						  uint32 recvbuffersize)
{
	int32 conn_idx = MakeGeneralConnID();
	TCPSocket* s = new TCPSocket(
		0,
		conn_idx,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize);

	s->status_ = socket_status_connecting;

	bool ret = s->Connect(ip.c_str(), port);
	if (!ret)
	{
		delete s;
		s = NULL;
		return INVALID_INDEX;
	}
	else
	{
		AddSocket(s);

		s->status_ = socket_status_connected;

		//----------------------------------------------------------------------------------------
		struct epoll_event ev;
		memset(&ev, 0, sizeof(epoll_event));
		ev.events = (s->GetWriteBuffer().GetSize()) ? EPOLLOUT : EPOLLIN;
		ev.events = ev.events | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET; 
		ev.data.ptr = s;

		int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->GetFd(), &ev))
		{
			PRINTF_ERROR("Could not add event to epoll set on fd %u", s->GetFd());
		}
		//----------------------------------------------------------------------------------------
		s->OnConnect(true);

		return s->GetConnectIdx();
	}
}

uint32 SocketMgr::ConnectEx( const string& ip, uint16 port,
						  const HandleInfo onconnected_handler,
						  const HandleInfo onclose_handler,
						  const HandleInfo onrecv_handler,
						  uint32 sendbuffersize, 
						  uint32 recvbuffersize)
{
	int32 conn_idx = MakeGeneralConnID();
	TCPSocket* s = new TCPSocket(
		0,
		conn_idx,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize);

	s->status_ = socket_status_connecting;

	bool ret = s->ConnectEx(ip.c_str(), port);
	if (ret)
	{
		AddSocket(s);
		//----------------------------------------------------------------------------------------
		struct epoll_event ev;
		memset(&ev, 0, sizeof(epoll_event));
		ev.events = EPOLLOUT | EPOLLIN;
		ev.events = ev.events | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET; 
		ev.data.ptr = s;

		int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->GetFd(), &ev))
		{
			PRINTF_ERROR("Could not add event to epoll set on fd %u", s->GetFd());
		}
	}
	else
	{
		delete s;
		s = NULL;
	}

	return conn_idx;
}
//------------------------------------------------------------------------------
uint32 SocketMgr::ConnectUDP(const string& ip, uint16 port, uint16& local_port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize,
	uint32 recvbuffersize,
	int sub_type,
	bool is_server_build /*= false*/)
{
	int32 conn_idx = MakeGeneralConnID();

	UDPSocket* s = NULL;
	if (sub_type == UDP_TYPE_NORMAL)
	{
		s = new UDPSocket(0,
			conn_idx,
			onconnected_handler,
			onclose_handler,
			onrecv_handler,
			sendbuffersize,
			recvbuffersize);
	}
	else if (sub_type == UDP_TYPE_KCP)
	{
		s = new KCPSocket(0,
			conn_idx,
			onconnected_handler,
			onclose_handler,
			onrecv_handler,
			sendbuffersize,
			recvbuffersize);
	}
	else
	{

	}

	s->status_ = socket_status_connecting;

	bool ret = s->Connect(ip.c_str(), port, local_port);
	if (!ret)
	{
		delete s;
		s = NULL;
		return INVALID_INDEX;
	}
	else
	{
		AddSocket(s); //����socket�б�

		s->status_ = socket_status_connected;
		//----------------------------------------------------------------------------------------
		// Add epoll event based on socket activity.
		struct epoll_event ev;
		memset(&ev, 0, sizeof(epoll_event));
		ev.events = EPOLLOUT | EPOLLIN;
		ev.events = ev.events | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
		ev.data.ptr = s;

		int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->GetFd(), &ev))
		{
			PRINTF_ERROR("Could not add event to epoll set on fd %u", s->GetFd());

			s->CloseSocket();
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			RemoveSocket(s->GetConnectIdx());

			return INVALID_INDEX;
		}
		else
		{
			if (is_server_build) //����Ƿ���������������
			{
				s->is_udp_connected_ = true;
				s->OnConnect(true);
			}
			else
			{
				// ����һ�����Ӱ���UDP��������
				char conn_str[8] = "connect";
				s->Send(conn_str, sizeof(conn_str));
			}

			return conn_idx;
		}

	}
}

bool SocketMgr::AcceptUDP(sockaddr_in& remote_address,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize,
	uint32 recvbuffersize,
	int32 sub_type,
	uint16& local_port)
{
	// ����һ����ʱ��socket
	string ip = inet_ntoa(remote_address.sin_addr);
	uint16 port = htons(remote_address.sin_port);

	int32 conn_idx = ConnectUDP(ip, port, local_port,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize,
		sub_type,
		true);

	if (conn_idx != INVALID_INDEX)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
void SocketMgr::AddSocket(Socket* s)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(s->GetConnectIdx());
	if (it != socket_map_.end())
	{
		ASSERT(false);
	}
	else
	{
		if (socket_map_.insert(make_pair(s->GetConnectIdx(), s)).second)
		{
			REF_ADD(s);
			//PRINTF_DEBUG("add socket count = %d", socket_map_.size());
		}
		else
		{
			ASSERT(false);
		}
	}
}

void SocketMgr::RemoveSocket(int32 conn_idx)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket *s = it->second;
		//--------------------------------------------------------------------------------------------
		epoll_event ev = { 0, { 0 } };
		int epoll_fd = SocketMgr::get_instance()->GetEpollFd();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, s->GetFd(), &ev))
		{
			PRINTF_ERROR("RemoveSocket Could not remove fd %u from epoll set, errno %u", conn_idx, errno);
		}
		//--------------------------------------------------------------------------------------------
		REF_RELEASE(s);

		socket_map_.erase(it);

		//PRINTF_DEBUG("remove socket count = %d", socket_map_.size());
	}
}

void SocketMgr::CloseSocket(Socket* s)
{
	if (s->status_ == socket_status_closing || s->status_ == socket_status_connected)
	{
		s->status_ = socket_status_closed;
		s->OnDisconnect();

		RemoveSocket(s->GetConnectIdx());

		s->CloseSocket();
	}
}

void SocketMgr::Disconnect(int32 conn_idx)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;
		s->Disconnect();
	}
}

bool SocketMgr::Send(int32 conn_idx, const void* content, uint32 len)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;
		s->Send(content, len);
		return true;
	}
	else
	{
		return false;
	}
}

bool SocketMgr::SendMsg(int32 conn_idx, const void* content, uint32 len)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;
		s->SendMsg(content, len);
		return true;
	}
	else
	{
		return false;
	}
}

char* SocketMgr::GetIpAddress(int32 conn_idx)
{
	char* ip_addr = NULL;

	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;

		ip_addr = inet_ntoa(s->GetRemoteStruct().sin_addr);
	}

	return ip_addr;
}

#endif
