/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/


#ifndef _SOCKET_OPS_H_
#define _SOCKET_OPS_H_

namespace SocketOps
{
	// Create file descriptor for socket i/o operations.
	SOCKET CreateTCPFileDescriptor();

	// Create file descriptor for socket i/o operations.
	SOCKET CreateUDPFileDescriptor();

	// Disable blocking send/recv calls.
	bool Nonblocking(SOCKET fd);

	// Enable blocking send/recv calls.
	bool Blocking(SOCKET fd);

	// Disable nagle buffering algorithm
	bool DisableBuffering(SOCKET fd);

	// Enables nagle buffering algorithm
	bool EnableBuffering(SOCKET fd);

	// Set internal buffer size to socket.
	bool SetRecvBufferSize(SOCKET fd, uint32 size);

	// Set internal buffer size to socket.
	bool SetSendBufferSize(SOCKET fd, uint32 size);

	// Closes socket completely.
	void CloseSocket(SOCKET fd);

	// Sets SO_REUSEADDR
	void ReuseAddr(SOCKET fd);

#ifdef WIN32
	// Set update connect context.
	bool SetUpdateConnectContext(SOCKET fd);

	// Set update accept context.
	bool SetUpdateAcceptContext(SOCKET connFd, SOCKET listenFd);
#endif
};

#endif