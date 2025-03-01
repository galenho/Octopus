/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "socket_defines.h"
#include "timer.h"
#include "scheduler.h"
#include "sequence_buffer.h"

enum SocketStatus
{
	socket_status_closed		= 1,
	socket_status_listening		= 2,
	socket_status_connecting	= 3,
	socket_status_connected		= 4,
	socket_status_closing		= 5
};

class Socket : public SocketBase
{
public:

#ifndef WIN32
	Socket( SOCKET wakeup_fd );
#endif

	Socket(
		SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	~Socket();

	virtual void Update(int64 cur_time);

	virtual void OnRead();
	virtual bool Send(const void* buff, uint32 len);
	virtual bool SendMsg(const void* buff, uint32 len);

	void Accept(sockaddr_in* address);
	virtual void Disconnect();

	void OnConnect(bool is_success);
	void OnDisconnect();

	void CloseSocket();

	string GetRemoteIP();
	inline uint32 GetRemotePort()
	{
		return ntohs(m_client.sin_port);
	}
	inline SOCKET GetFd()
	{
		return fd_;
	}
	inline uint32 GetConnectIdx()
	{
		return conn_idx_;
	}
	inline SocketType GetSocketType()
	{
		return socket_type_;
	}
	inline int32 GetSubType()
	{
		return sub_type_;
	}
	inline sockaddr_in& GetRemoteStruct()
	{
		return m_client;
	}
	inline in_addr GetRemoteAddress()
	{
		return m_client.sin_addr;
	}
	inline SequenceBuffer& GetReadBuffer()
	{
		return readBuffer;
	}
	inline SequenceBuffer& GetWriteBuffer()
	{
		return writeBuffer;
	}

#ifdef WIN32
public:
	OverlappedStruct connect_event_;
	OverlappedStruct read_event_;
	OverlappedStruct write_event_;
	OverlappedStruct close_event_;
#endif

#ifdef WIN32
public:
	bool PostReadEvent();
	bool PostSendEvent();
#else
public:
	bool PostEvent(uint32 events);
	bool WriteData();
#endif

public:
	SequenceBuffer readBuffer;
	uint32 recvbuffersize_;

	SequenceBuffer writeBuffer;
	
	SocketStatus status_;
	bool is_sending_;		// 是否正在发送中
	bool is_client_;

	SOCKET fd_;
	uint32 conn_idx_;
	sockaddr_in m_client;

	HandleInfo onconnected_handler_;
	HandleInfo onclose_handler_;
	HandleInfo onrecv_handler_;

	bool isCloseSocket_;
};

#endif // _SOCKET_H_
