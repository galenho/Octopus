#ifndef _UDP_CLIENT_IMPL_H_
#define _UDP_CLIENT_IMPL_H_

class Socket;
class UDPClient_impl
{
public:
	UDPClient_impl();

	~UDPClient_impl();

	uint32 Connect( const string& ip, uint16 port, uint16& local_port,
				  const HandleInfo onconnected_handler,
				  const HandleInfo onclose_handler,
				  const HandleInfo onrecv_handler,
				  uint32 sendbuffersize, 
				  uint32 recvbuffersize);

	void Disconnect(int32 conn_idx);
	bool SendMsg( int32 conn_idx, const void* msg, uint32 len );

	char* GetIpAddress( int32 conn_idx );
};

#endif //_UDP_CLIENT_IMPL_H_