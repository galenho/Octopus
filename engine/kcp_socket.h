/*
* 3D MMORPG Server
* Copyright (C) 2009-2023 RedLight Team
* author: galen
*/

#ifndef _KCP_SOCKET_H_
#define _KCP_SOCKET_H_

#include "udp_socket.h"
#include "ikcp.h"

class KCPSocket : public UDPSocket
{
public:
	KCPSocket(SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	~KCPSocket();

	virtual void Update(int64 cur_time);
	virtual void OnRead();
	
	bool SendMsg(const void* buff, uint32 len);

private:
	static int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);
	void on_udp_package_send(const char* buf, int len);

public:
	ikcpcb* p_kcp_;
};

#endif //_KCP_SOCKET_H_
