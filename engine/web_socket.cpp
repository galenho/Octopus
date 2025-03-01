#include "web_socket.h"
#include "socket_ops.h"

#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

#include "http_help.h"
#include "openssl/sha.h"
#include "util.h"

const char* s_ack_handshake = "HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: %s\r\n\r\n";

extern CLog* g_logger;

WebSocket::WebSocket(
	SOCKET fd, 
	int32 conn_idx, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	: TCPSocket(fd, conn_idx, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	sub_type_ = TCP_TYPE_WEB_SOCKET;

	is_handshake_ = false;
}

void WebSocket::OnRead()
{
	//未握手
	if (!is_handshake_)
	{
		uint32 packet_len = GetReadBuffer().GetSize();
		char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());
		if (ParseHandshake(buffer_start, packet_len) == false)
		{
			Disconnect();
			return;
		}

		GetReadBuffer().Remove(packet_len);

		// 回复
		ResponseHandshake();

		is_handshake_ = true;

		OnConnect(true);

		return;
	}
	else
	{
		while (true)
		{
			char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());
			uint32 packet_len = GetReadBuffer().GetSize();
			if (packet_len == 0)
			{
				break;
			}

			uint8 opcode = 0;
			CSerializeStream s(g_buff, g_buff_len);
			int32 len = FetchFrameInfo(buffer_start, packet_len, s, opcode);
			if (len > 0)
			{
				if (opcode == TEXT_FRAME || opcode == BINARY_FRAME)
				{
					SocketReadTask::Process(onrecv_handler_, conn_idx_, s.get_buffer(), s.get_length());
				}
				else if (opcode == CLOSE_FRAME)
				{
					Disconnect();
				}
				else if (opcode == PING_FRAME)
				{
					SendPong();
				}
				else if (opcode == PONG_FRAME)
				{

				}
				else
				{

				}
				
				GetReadBuffer().Remove(len);
			}
			else
			{
				break;
			}
		}
		
	}
}

bool WebSocket::SendMsg(const void* data, uint32 len)
{
	CSerializeStream s(g_buff, g_buff_len);
	PackDataFrame(TEXT_FRAME, (const char*)data, len, s);
	bool ret = Socket::Send(s.get_buffer(), s.get_length());
	return ret;
}

bool WebSocket::ParseHandshake(const char* data, uint32 len)
{
	if (len < 2)
		return false;

	istringstream stream(data);
	string line;
	getline(stream, line);
	if (line.find("GET", 0) != 0)
		return false;

	map<string, string> key_map;
	while (std::getline(stream, line))
	{
		if (line == "\r" || line[line.size() - 1] != '\r') 
		{
			continue; //end
		}

		string::size_type pos = line.find(": ");
		string key = line.substr(0, pos);
		string value = line.substr(pos + 2, line.length() - 1 - (pos + 2));
		key_map[key] = value;
	}

	map<string, string>::iterator it_host = key_map.find("Host");
	if (it_host == key_map.end())
		return false;

	map<string, string>::iterator it_key = key_map.find("Sec-WebSocket-Key");
	if (it_key == key_map.end())
		return false;

	host_ = it_host->second;
	key_ = it_key->second;
	return true;
}

void WebSocket::ResponseHandshake()
{
	static const char* s_key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::string server_key = key_ + s_key;

	unsigned char sha1hash[20] = { 0 };
	SHA_CTX sha1ctx;
	SHA1_Init(&sha1ctx);
	SHA1_Update(&sha1ctx, server_key.c_str(), server_key.length());
	SHA1_Final(sha1hash, &sha1ctx);

	char accept_key[128] = { 0 };
	base64_encode((const char*)sha1hash, sizeof(sha1hash), accept_key);
	
	string response_str = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
	response_str += accept_key;
	response_str += "\r\n\r\n";
	bool ret = Socket::Send(response_str.c_str(), (uint32)response_str.length());
	if (ret)
	{
		AddRef(); //增加引用
	}
}

int32 WebSocket::FetchFrameInfo(char* data, uint32 len, CSerializeStream& s, uint8& opcode)
{
	if (len < 2)
	{
		return 0;
	}

	uint32 cursor = 0;

	// FIN, opcode
	uint8 onebyte = *((uint8*)data);
	cursor += 1;

	uint8 fin = onebyte >> 7;
	uint8 rsv1 = onebyte & 0x40;
	uint8 rsv2 = onebyte & 0x20;
	uint8 rsv3 = onebyte & 0x10;
	opcode = onebyte & 0x0F;
	
	if (fin == 0) //说明是不完整的帧，不支持
	{
		return -1;
	}

	// payload length
	onebyte = *((uint8*)(data + cursor));
	cursor += 1;

	uint8 mask = onebyte >> 7 & 0x01;
	uint8 length_type = onebyte & 0x7F;
	uint64 payload_length = 0;
	uint8 masking_key_[4];

	if (length_type < 126)
	{
		payload_length = length_type;
	}
	else if (length_type == 126)
	{
		if (len < cursor + 2)
		{
			return 0;
		}

		uint16 v = *((uint16*)(data + cursor));
		payload_length = (uint16)SwapEndian((uint8*)&v, sizeof(v));
		cursor += 2;
	}
	else if (length_type == 127)
	{
		if (len < cursor + 8)
		{
			return 0;
		}

		uint64 v = *((uint64*)(data + cursor));
		payload_length = (uint64)SwapEndian((uint8*)&v, sizeof(v));
		cursor += 8;
	}

	// masking key
	if (mask == 1)
	{
		if (len < cursor + 4 + (uint32)payload_length)
		{
			return 0;
		}

		uint8* p = (uint8*)(data + cursor);
		memcpy(masking_key_, p, 4);
		cursor += 4;

		for (uint64_t i = 0; i < payload_length; i++)
		{
			uint8 v = (*((uint8*)(data + cursor + i))) ^ masking_key_[i % 4];
			s << v;
		}

		cursor += (int32)payload_length;
	}
	else
	{
		if (len < cursor + payload_length)
		{
			return 0;
		}

		s.write_byte(data + cursor, (uint32)payload_length);
		cursor += (int32)payload_length;
	}

	return cursor;
}

void WebSocket::PackDataFrame(uint8 opcode, const char* data, uint32 len, CSerializeStream& s)
{
	uint8 fin = 1;
	uint8 rsv1 = 0;
	uint8 rsv2 = 0;
	uint8 rsv3 = 0;

	uint8 onebyte = 0;
	onebyte |= (fin << 7);
	onebyte |= (rsv1 << 6);
	onebyte |= (rsv2 << 5);
	onebyte |= (rsv3 << 4);
	onebyte |= (opcode & 0x0F);
	s << onebyte;

	onebyte = 0;

	// 这里服务器端发给客户端的包不带maskKey
	if (len < 126)
	{
		onebyte = (uint8)len;
		s << onebyte;
	}
	else if (len <= 65535)
	{
		onebyte = 126;
		s << onebyte;

		uint16 v = (uint16)len;
		v = (uint16)SwapEndian((uint8*)&v, sizeof(v));
		s << v;
	}
	else
	{
		onebyte = 127;
		s << onebyte;

		uint64 v = (uint64)len;
		v = (uint64)SwapEndian((uint8*)&v, sizeof(v));
		s << v;
	}

	if (len > 0)
	{
		s.write_byte((char*)data, len);
	}
}

bool WebSocket::SendPong()
{
	CSerializeStream s(g_buff, g_buff_len);
	PackDataFrame(PONG_FRAME, NULL, 0, s);
	bool ret = Socket::Send(s.get_buffer(), s.get_length());
	return ret;
}