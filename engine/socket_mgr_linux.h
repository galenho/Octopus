/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _SOCKETMGR_LINUX_H_
#define _SOCKETMGR_LINUX_H_

#include "socket_defines.h"
#ifndef WIN32

#define THREAD_EVENT_SIZE 4096
#define SOCKET_HOLDER_SIZE 30000
#include "thread_base.h"
#include "rwlock.h"
#include "safe_list.h"
#include <sys/eventfd.h>

class Socket;
class SocketIOThread;

struct SocketEvent
{
	uint32 customized_events;
	Socket* s;

	SocketEvent()
	{
		customized_events = 0;
		s = NULL;
	}
};

class SocketMgr : public Singleton<SocketMgr>
{
public:
	friend class SocketIOThread;

	SocketMgr();

	~SocketMgr();

	uint32 MakeGeneralConnID();
	int GetEpollFd();

	bool Init();
	bool Close();

	void WakeUp();

	void HandleDelayEvent();
	int  EventLoop(int64 timeout_time);
	void Update(int64 cur_time);

	void Accept( SOCKET aSocket, 
				 sockaddr_in& address, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize,
				 int sub_type);

	uint32 Connect( const string& ip, uint16 port,
					const HandleInfo onconnected_handler,
					const HandleInfo onclose_handler,
					const HandleInfo onrecv_handler,
					uint32 sendbuffersize, 
					uint32 recvbuffersize);

	uint32 ConnectEx( const string& ip, uint16 port,
					const HandleInfo onconnected_handler,
					const HandleInfo onclose_handler,
					const HandleInfo onrecv_handler,
					uint32 sendbuffersize, 
					uint32 recvbuffersize);

	bool AcceptUDP(sockaddr_in& remote_address,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		int32  sub_type,
		uint16& local_port);

	uint32 ConnectUDP(const string& ip, uint16 port, uint16& local_port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		int sub_type,
		bool is_server_build = false);

	void Disconnect(int32 conn_idx);
	void CloseSocket(Socket* s);

	bool Send(int32 conn_idx, const void* content, uint32 len);
	bool SendMsg(int32 conn_idx, const void* content, uint32 len);

	void AddSocket(Socket* s);
	void RemoveSocket(int32 conn_idx);

	char* GetIpAddress(int32 conn_idx);

public:
	hash_map<uint32, Socket*> socket_map_;
	int epoll_fd_;
	SafeList<SocketEvent> event_queue_;

private:
	uint32 auto_conn_idx_;  //用conn_idx来做为socket_map_的Key, 因为端口事件检测使用的是指针ptr形式的

	// epoll event struct
	struct epoll_event events[THREAD_EVENT_SIZE];

	Socket* wakeup_s_;
	bool is_wakeup_doing_;
};

#endif

#endif //_SOCKETMGR_LINUX_H_