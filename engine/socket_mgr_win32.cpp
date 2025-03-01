#include "network.h"
#include "timer.h"
#include "guard.h"
#include "tcp_listen_socket.h"
#include "udp_listen_socket.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "kcp_socket.h"
#include "http_socket.h"
#include "web_socket.h"

#ifdef WIN32
void HandleAcceptTCPComplete(TCPListenSocket* s, SOCKET aSocket)
{
	int addr_len = sizeof(sockaddr_in);

	sockaddr_in* addr_local = NULL;
	int addr_len_local = addr_len;
	sockaddr_in* addr_client = NULL;
	int addr_len_client = addr_len;

	s->accept_addrs_(s->buff_, 0,
		addr_len + 16, addr_len + 16,
		(LPSOCKADDR*)&addr_local, &addr_len_local,
		(LPSOCKADDR*)&addr_client, &addr_len_client);

	// 这里一定要设置, 因为使用了AcceptEx
	SocketOps::SetUpdateAcceptContext(aSocket, s->GetFd());

	SocketMgr::get_instance()->Accept(aSocket,
		*addr_client,
		s->onconnected_handler_,
		s->onclose_handler_,
		s->onrecv_handler_,
		s->sendbuffersize_,
		s->recvbuffersize_,
		s->sub_type_);


	// 再投递一个AcceptEx
	s->PostAccept();
}

void HandleAcceptUDPComplete(UDPListenSocket* s, uint32 len)
{
	//PRINTF_INFO("HandleAcceptUDPComplete fd = %d", s->socket_);

	if (len == 8)
	{
		uint8* buffer_start = (uint8*)(s->buff_);

		char src_conn_str[8] = "connect";
		char dst_conn_str[8];
		memcpy(dst_conn_str, (uint8*)buffer_start, 8);
		if (strcmp(src_conn_str, dst_conn_str) == 0)
		{
			// 模拟tcp的accept, 创建一个新的一个udp socket
			uint16 out_port = 0;
			bool ret = SocketMgr::get_instance()->AcceptUDP(s->client_addr_,
				s->onconnected_handler_,
				s->onclose_handler_,
				s->onrecv_handler_,
				s->sendbuffersize_,
				s->recvbuffersize_,
				s->sub_type_,
				out_port);

			// 回发一个连接包给UDP客户端, 告诉客户端新的端口号
			if (ret)
			{
				RepServerPort rep_msg;
				rep_msg.port = out_port;
				sendto(s->socket_, (char*)&rep_msg, sizeof(rep_msg), 0, (sockaddr*)&s->client_addr_, sizeof(s->client_addr_));
			}
		}
	}

	// 再投递一个AcceptEx
	s->PostAccept();
}

void HandleConnectComplete(Socket* s, uint32 len, bool is_success)
{
	//PRINTF_INFO("HandleConnectComplete fd = %d, conn_idx = %d, status = %d, len = %d", s->GetFd(), s->GetConnectIdx(), s->status_, len);

	if (is_success)
	{
		s->status_ = socket_status_connected;

		// 这里一定要设置, 因为使用了ConnectEx
		SocketOps::SetUpdateConnectContext(s->GetFd());

		SocketMgr::get_instance()->AddSocket(s); //加入socket列表
		bool ret = s->PostReadEvent();
		if (ret) //投递一个读成功
		{
			if (s->GetSocketType() == SOCKET_TYPE_TCP && s->GetSubType() == TCP_TYPE_WEB_SOCKET)
			{
				// 等待验证完头信息后再OnConnect
			}
			else
			{
				s->OnConnect(true);
			}
		}
		else
		{
			//PRINTF_ERROR("PostReadEvent = false fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

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
		s->OnConnect(false);
	}
}

void HandleReadComplete(Socket* s, uint32 len)
{
	//PRINTF_INFO("HandleReadComplete fd = %d, conn_idx = %d, status = %d, len = %d", s->GetFd(), s->GetConnectIdx(), s->status_, len);

	// 释放引用-1
	REF_RELEASE(s);

	if (s->status_ != socket_status_connected && s->status_ != socket_status_closing)
	{
		return;
	}

	if (len > 0)
	{
		s->GetReadBuffer().IncrementWritten(len);
		s->OnRead();

		bool ret = s->PostReadEvent();
		if (!ret)
		{
			//PRINTF_ERROR("PostReadEvent is fail, fd = %d, conn_idx = %d", s->GetFd(), s->GetConnectIdx());
			SocketMgr::get_instance()->CloseSocket(s);
		}
		else
		{
			//PRINTF_INFO("PostReadEvent is success, fd = %d, conn_idx = %d", s->GetFd(), s->GetConnectIdx());
		}
	}
	else
	{
		if (s->GetSocketType() == SOCKET_TYPE_TCP)
		{
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
				closesocket(s->GetFd()); //直接调用关闭closesocket

				//PRINTF_ERROR("closesocket fd = %d", fd);

				s->status_ = socket_status_closed;
				s->OnConnect(false);
			}
		}
	}
}

void HandleWriteComplete(Socket* s, uint32 len)
{
	//PRINTF_INFO("HandleWriteComplete fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

	// 释放引用-1
	REF_RELEASE(s);
	s->is_sending_ = false;

	if (s->status_ != socket_status_connected)
	{
		return;
	}
	else
	{
		s->GetWriteBuffer().Remove(len);
		if (s->GetWriteBuffer().GetSize() > 0) //继续写
		{
			bool ret = s->PostSendEvent();
			if (!ret)
			{
				SocketMgr::get_instance()->CloseSocket(s);
			}
		}
	}
}

void HandleClose(Socket* s)
{
	//PRINTF_INFO("HandleClose fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

	// 释放引用-1
	REF_RELEASE(s);

	if (s->status_ == socket_status_connected)
	{
		s->status_ = socket_status_closing;

		if (s->GetSocketType() == SOCKET_TYPE_TCP)
		{
			int ret = ::shutdown(s->GetFd(), SD_SEND);
			if (ret == SOCKET_ERROR)
			{
				int last_error = WSAGetLastError();
				printf("shutdown error = %d\n", last_error);
			}
		}
		else
		{
			s->CloseSocket();
		}
	}
}

//-------------------------------------------------------------------------------

initialiseSingleton(SocketMgr);
SocketMgr::SocketMgr()
{
	auto_conn_idx_ = 1;
	completion_port_ = NULL;
	wakeup_event_.SetEvent(SOCKET_IO_THREAD_WAKEUP);
	shutdown_event_.SetEvent(SOCKET_IO_THREAD_SHUTDOWN);
}

SocketMgr::~SocketMgr()
{
	
}

uint32 SocketMgr::MakeGeneralConnID()
{
	int32 conn_idx = INVALID_INDEX;
	while(true)
	{
		hash_map<uint32, Socket*>::iterator it = socket_map_.find(auto_conn_idx_);
		if (it != socket_map_.end())
		{
			++auto_conn_idx_;
			if (auto_conn_idx_ == 1000000)
			{
				auto_conn_idx_ = 1; //从头开始
			}
		}
		else
		{
			break;
		}
	}
	
	conn_idx = auto_conn_idx_;
	++auto_conn_idx_;

	return conn_idx;
}

bool SocketMgr::Init()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);

	return true;
}

bool SocketMgr::Close()
{
	PostQueuedCompletionStatus(completion_port_, 0, (ULONG_PTR)0, &shutdown_event_.overlap_);

	return true;
}

void SocketMgr::Update(int64 cur_time)
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

void SocketMgr::EventLoop(int64 timeout_time)
{
	Update(getMSTime());

	HANDLE cp = completion_port_;
	DWORD bytes_transferred;
	void* ptr = NULL;
	OverlappedStruct* ov;
	LPOVERLAPPED overlapped;
	
	while (true)
	{
		int64 cur_time = getMSTime();
		int64 timeout = timeout_time - cur_time;
		if (timeout <= 0)
		{
			break;
		}

#ifdef X64
		int ret = GetQueuedCompletionStatus(cp, &bytes_transferred, (PULONG_PTR)&ptr, &overlapped, (uint32)timeout);
#else
		int ret = GetQueuedCompletionStatus(cp, &bytes_transferred, (LPDWORD)&ptr, &overlapped, (uint32)timeout);
#endif
		if (ret)
		{
			//(1) 如果函数从完成端口取出一个成功I/O操作的完成包，返回值为非0。
			//    函数在指向lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped的参数中存储相关信息。

			ov = CONTAINING_RECORD(overlapped, OverlappedStruct, overlap_);

			if (ptr == NULL)
			{
				continue;
			}

			if (ov->event_ == SOCKET_IO_EVENT_ACCEPT_TCP) // 监听端口Accept
			{
				TCPListenSocket* listen_socket = (TCPListenSocket*)ptr;
				HandleAcceptTCPComplete(listen_socket, ov->fd_);
			}
			else if (ov->event_ == SOCKET_IO_EVENT_ACCEPT_UDP) 
			{
				UDPListenSocket* listen_socket = (UDPListenSocket*)ptr;
				HandleAcceptUDPComplete(listen_socket, bytes_transferred);
			}
			else
			{
				Socket* s = (Socket*)ptr;
				REF_ADD(s);

				switch (ov->event_)
				{
				case SOCKET_IO_EVENT_CONNECT_COMPLETE:
					HandleConnectComplete(s, bytes_transferred, true);
					break;
				case SOCKET_IO_EVENT_READ_COMPLETE:
					HandleReadComplete(s, bytes_transferred);
					break;
				case SOCKET_IO_EVENT_WRITE_COMPLETE:
					HandleWriteComplete(s, bytes_transferred);
					break;
				case SOCKET_IO_EVENT_CLOSE:
					HandleClose(s);
					break;
				default:
					break;
				}

				REF_RELEASE(s);
			}
		}
		else
		{
			ov = CONTAINING_RECORD(overlapped, OverlappedStruct, overlap_);

			if (ov == NULL) // 第(2)种情况
			{
				//(2) 如果 *lpOverlapped为空并且函数没有从完成端口取出完成包，返回值则为0。
				//    函数则不会在lpNumberOfBytes and lpCompletionKey所指向的参数中存储信息。
				//	  调用GetLastError可以得到一个扩展错误信息。如果函数由于等待超时而未能出列完成包，GetLastError返回WAIT_TIMEOUT. 
				DWORD last_error = ::GetLastError();
				if (last_error == WAIT_TIMEOUT)
				{
					break;
				}
				else
				{
					break;
				}
			}
			else
			{
				if (ptr == NULL)
				{
					continue;
				}

				//---------------------------------------------------------------------------------
				if (bytes_transferred == 0) // 第(4)种情况
				{
					//(4) 如果关联到一个完成端口的一个socket句柄被关闭了，则GetQueuedCompletionStatus返回ERROR_SUCCESS,并且lpNumberOfBytes等于0
					if (ov->event_ == SOCKET_IO_EVENT_ACCEPT_TCP) // 监听端口Accept
					{
						
					}
					else if (ov->event_ == SOCKET_IO_EVENT_ACCEPT_UDP)
					{

					}
					else
					{
						Socket* s = (Socket*)ptr;
						REF_ADD(s);

						switch (ov->event_)
						{
						case SOCKET_IO_EVENT_CONNECT_COMPLETE:
							HandleConnectComplete(s, bytes_transferred, false);
							break;
						case SOCKET_IO_EVENT_READ_COMPLETE:
							HandleReadComplete(s, bytes_transferred);
							break;
						default:
							break;
						}
						REF_RELEASE(s);
					}
				}
				else // 第(3)种情况
				{
					//(3) 如果 *lpOverlapped不为空并且函数从完成端口出列一个失败I/O操作的完成包，返回值为0。
					//    函数在指向lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped的参数指针中存储相关信息。
					//    调用GetLastError可以得到扩展错误信息 
					if (overlapped)
					{
						break;
					}
				}
			}
		}
	}
}

void SocketMgr::WakeUp()
{
	PostQueuedCompletionStatus(completion_port_, 0, (ULONG_PTR)0, &wakeup_event_.overlap_);
}

HANDLE SocketMgr::GetCompletionPort()
{
	return completion_port_;
}

uint32 SocketMgr::Connect( const string& ip, uint16 port,
						const HandleInfo onconnected_handler,
						const HandleInfo onclose_handler,
						const HandleInfo onrecv_handler,
						uint32 sendbuffersize, 
						uint32 recvbuffersize)
{
	int32 conn_idx = MakeGeneralConnID();
	TCPSocket *s = new TCPSocket( 
							0,
							conn_idx,
							onconnected_handler, 
							onclose_handler, 
							onrecv_handler,
							sendbuffersize, 
							recvbuffersize );

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
		AddSocket(s); //加入socket列表
		s->status_ = socket_status_connected;

		bool ret = s->PostReadEvent();
		if (ret) //投递一个读成功
		{
			s->OnConnect(true);
			return conn_idx;
		}
		else
		{
			s->CloseSocket();
			s->isCloseSocket_ = true;
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			RemoveSocket(s->GetConnectIdx());
			return INVALID_INDEX;
		}
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
	TCPSocket *s = new TCPSocket(
						0,
						conn_idx,
						onconnected_handler,
						onclose_handler,
						onrecv_handler,
						sendbuffersize,
						recvbuffersize);

	s->status_ = socket_status_connecting;

	bool ret = s->ConnectEx(ip.c_str(), port);
	
	if (!ret)
	{	
		delete s;
		s = NULL;
		return conn_idx;
	}
	else
	{
		return conn_idx;
	}
}

void SocketMgr::Accept(
	SOCKET aSocket, 
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

	s->Accept(&address);

	AddSocket(s); //加入socket列表

	s->status_ = socket_status_connected;
	bool ret = s->PostReadEvent();
	if (ret) //投递一个读成功
	{	
		if (s->GetSocketType() == SOCKET_TYPE_TCP && s->GetSubType() == TCP_TYPE_WEB_SOCKET)
		{
			// 等待验证完头信息后再OnConnect
		}
		else
		{
			s->OnConnect(true);
		}
	}
	else
	{
		s->CloseSocket();
		RemoveSocket(s->GetConnectIdx());
	}
}

bool SocketMgr::AcceptUDP(
	sockaddr_in& remote_address, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize, 
	int32 sub_type,
	uint16& local_port)
{
	// 分配一个临时的socket
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

uint32 SocketMgr::ConnectUDP(
	const string& ip, 
	uint16 port, 
	uint16& local_port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize, 
	int sub_type,
	bool is_server_build)
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
		AddSocket(s); //加入socket列表
		s->status_ = socket_status_connected;

		bool ret = s->PostReadEvent();
		if (ret) //投递一个读成功
		{
			if (is_server_build) //如果是服务器主动创建的
			{
				s->is_udp_connected_ = true;
				s->OnConnect(true);
			}
			else
			{
				// 发送一个连接包给UDP服务器端
				char conn_str[8] = "connect";
				s->Send(conn_str, sizeof(conn_str));
			}

			return conn_idx;
		}
		else
		{
			s->CloseSocket();
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			RemoveSocket(s->GetConnectIdx());
			return INVALID_INDEX;
		}
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
			//PRINTF_DEBUG("socket count = %d", socket_map_.size());
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
		REF_RELEASE(s);

		socket_map_.erase(it);
		//PRINTF_DEBUG("socket count = %d", socket_map_.size());
	}
}

void SocketMgr::CloseSocket(Socket* s)
{
	if (s->status_ == socket_status_closing || s->status_ == socket_status_connected)
	{
		SocketStatus old_status = s->status_;

		s->status_ = socket_status_closed;
		s->OnDisconnect();

		SOCKET fd = s->GetFd();
		RemoveSocket(s->GetConnectIdx());

		s->CloseSocket();
	}
}

bool SocketMgr::Send(int32 conn_idx, const void* content, uint32 len)
{
	//PRINTF_INFO("Send conn_idx = %d", conn_idx);

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
	//PRINTF_INFO("SendMsg conn_idx = %d", conn_idx);

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

void SocketMgr::Disconnect(int32 conn_idx)
{
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;
		s->Disconnect();
	}
}

char* SocketMgr::GetIpAddress( int32 conn_idx )
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