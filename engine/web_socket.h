/*
* 3D MMORPG Server
* Copyright (C) 2009-2023 RedLight Team
* author: galen
*/

#ifndef _WEB_SOCKET_H_
#define _WEB_SOCKET_H_

/*-------------------------------------------------------------------

 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+

1. FIN：表示这个数据是不是接收完毕，为1表示收到的数据是完整的，占1bit
2. RSV1～3：用于扩展，通常都为0，各占1bit
3. OPCODE：表示报文的类型，占4bit  
   (1). 0x00：标识一个中间数据包
   (2). 0x01：标识一个text数据包
   (3). 0x02：标识一个二进制数据包
   (4). 0x03～07：保留
   (5). 0x08：标识一个断开连接数据包
   (6). 0x09：标识一个ping数据包
   (7). 0x0A：标识一个pong数据包
   (8). 0x0B～F：保留
4. MASK：用于表示数据是否经常掩码处理，为1时，Masking-key即存在，占1bit
5. Payload len：表示数据长度，即Payload Data的长度，当Payload len为0～125时，表示的值就是Payload Data的真实长度；
   当Payload len为126时，报文其后的2个字节形成的16bits无符号整型数的值是Payload Data的真实长度（网络字节序，需转换）；
   当Payload len为127时，报文其后的8个字节形成的64bits无符号整型数的值是Payload Data的真实长度（网络字节序，需转换）；
6. Masking-key：掩码，当Mask为1时存在，占4字节32bit
7. Payload Data：表示数据

-----------------------------------------------------------------------*/

#include "tcp_socket.h"
#include "serialize_stream.h"

enum WSFrameType
{
	// 文本帧与二进制帧
	TEXT_FRAME	 = 0x01,
	BINARY_FRAME = 0x02,

	// 关闭连接
	CLOSE_FRAME = 0x08,

	// ping pong
	PING_FRAME = 0x09,
	PONG_FRAME = 0x0A,
};

class WebSocket : public TCPSocket
{
public:
	WebSocket(
		SOCKET fd,
		int32 conn_idx,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);

	virtual void OnRead();
	virtual bool SendMsg(const void* data, uint32 len);

	bool ParseHandshake(const char* data, uint32 len);
	void ResponseHandshake();

	int32 FetchFrameInfo(char* data, uint32 len, CSerializeStream& s, uint8& opcode);
	void  PackDataFrame(uint8 opcode, const char* data, uint32 len, CSerializeStream& s);

	bool SendPong();

public:
	string key_;
	string host_;
	bool is_handshake_;
};

#endif //_WEB_SOCKET_H_
