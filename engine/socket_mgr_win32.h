/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*
*
* 核心类: 
*/
#ifndef _SOCKETMGR_WIN32_H_
#define _SOCKETMGR_WIN32_H_

#ifdef WIN32
#include "singleton.h"
#include "rwlock.h"

class Socket;
class TCPListenSocket;
class UDPListenSocket;
//----------------------------------------------------------------------------
void HandleAcceptTCPComplete(TCPListenSocket* s, SOCKET aSocket);
void HandleAcceptUDPComplete(UDPListenSocket* s, uint32 len);

void HandleConnectComplete(Socket* s, uint32 len, bool is_success);
void HandleReadComplete(Socket* s, uint32 len);
void HandleWriteComplete(Socket* s, uint32 len);

void HandleClose(Socket* s);

//----------------------------------------------------------------------------
class SocketMgr : public Singleton<SocketMgr>
{
public:
	SocketMgr();
	~SocketMgr();

	bool Init();
	bool Close();

	void Update(int64 cur_time);
	void EventLoop(int64 timeout_time);
	void WakeUp();

	HANDLE GetCompletionPort();

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

	bool Send(int32 conn_idx, const void* content, uint32 len);
	bool SendMsg(int32 conn_idx, const void* content, uint32 len);
	void Disconnect(int32 conn_idx);

	void Accept( SOCKET aSocket, 
		sockaddr_in& address, 
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize,
		int sub_type);

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
		int32 sub_type,
		bool is_server_build);

	void CloseSocket(Socket* s);

	void RemoveSocket(int32 conn_idx);

	uint32 MakeGeneralConnID();

	void AddSocket(Socket* s);

	char* GetIpAddress(int32 conn_idx);
	
public:
	hash_map<uint32, Socket*> socket_map_;
	HANDLE completion_port_;

	OverlappedStruct wakeup_event_;
	OverlappedStruct shutdown_event_;

private:
	uint32 auto_conn_idx_;  //用conn_idx来做为socket_map_的Key, 因为端口事件检测使用的是指针ptr形式的
};

#endif
#endif //_SOCKETMGR_WIN32_H_