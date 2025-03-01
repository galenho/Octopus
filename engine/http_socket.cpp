#include "http_socket.h"
#include "socket_ops.h"

#ifndef WIN32
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

extern CLog* g_logger;

int ss_on_url(http_parser* p, const char* buf, size_t len)
{
	Request* req = &((HTTPSocket*)p->data)->request_;
	std::string url;
	url.assign(buf, len);
	req->_line.append_request_url(url);
	return 0;
}

int ss_on_header_field(http_parser* p, const char* buf, size_t len)
{
	Request* req = &((HTTPSocket*)p->data)->request_;
	if (req->_parse_part == PARSE_REQ_LINE)
	{
		if (p->method == 1)
		{
			req->_line.set_method("GET");
		}

		if (p->method == 3)
		{
			req->_line.set_method("POST");
		}

		int ret = req->_line.parse_request_url_params();
		if (ret != 0)
		{
			return ret;
		}

		if (p->http_major == 1 && p->http_minor == 0)
		{
			req->_line.set_http_version("HTTP/1.0");
		}
		if (p->http_major == 1 && p->http_minor == 1)
		{
			req->_line.set_http_version("HTTP/1.1");
		}
		req->_parse_part = PARSE_REQ_HEAD;
	}

	std::string field;
	field.assign(buf, len);
	if (req->_last_was_value)
	{
		req->_header_fields.push_back(field);
		req->_last_was_value = false;
	}
	else
	{
		req->_header_fields[req->_header_fields.size() - 1] += field;
	}

	return 0;
}

int ss_on_header_value(http_parser* p, const char* buf, size_t len)
{
	Request* req = &((HTTPSocket*)p->data)->request_;

	std::string value;
	value.assign(buf, len);
	if (!req->_last_was_value)
	{
		req->_header_values.push_back(value);
	}
	else
	{
		req->_header_values[req->_header_values.size() - 1] += value;
	}

	req->_last_was_value = true;
	return 0;
}

int ss_on_headers_complete(http_parser* p)
{
	Request* req = &((HTTPSocket*)p->data)->request_;
	if (req->_header_fields.size() != req->_header_values.size())
	{
		LOG_ERROR(g_logger, "header field size:%u != value size:%u", req->_header_fields.size(), req->_header_values.size());
		return -1;
	}

	for (size_t i = 0; i < req->_header_fields.size(); i++)
	{
		req->add_header(req->_header_fields[i], req->_header_values[i]);
	}

	req->_parse_part = PARSE_REQ_HEAD_OVER;

	return 0;
}

int ss_on_body(http_parser* p, const char* buf, size_t len)
{
	Request* req = &((HTTPSocket*)p->data)->request_;
	req->get_body()->get_raw_string()->append(buf, len);
	req->_parse_part = PARSE_REQ_BODY;
	return 0;
}

int ss_on_message_complete(http_parser* p)
{
	HTTPSocket* s = (HTTPSocket*)p->data;
	Request* req = &s->request_;
	std::string ct_header = req->get_header("Content-Type");

	// parse body params
	if (ct_header == "application/x-www-form-urlencoded")
	{
		req->get_body()->Decode(); //galen: 进行一次urlencode
		std::string* raw_str = req->get_body()->get_raw_string();
		if (raw_str->size() > 0)
		{
			req->get_body()->get_req_params()->parse_query_url(*raw_str);
		}
	}

	req->_parse_part = PARSE_REQ_OVER;

	// 回调一次成功响应
	HttpReadTask::Process(s->onrecv_handler_, s->GetConnectIdx(), req);

	// 还原相关变量
	req->reset();

	return 0;
}

HTTPSocket::HTTPSocket( 
	SOCKET fd, 
	int32 conn_idx, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
	: TCPSocket(fd, conn_idx, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize)
{
	sub_type_ = TCP_TYPE_HTTP;

	// 初始化http
	http_parser_settings_init(&settings_);
	settings_.on_url = ss_on_url;
	settings_.on_header_field = ss_on_header_field;
	settings_.on_header_value = ss_on_header_value;
	settings_.on_headers_complete = ss_on_headers_complete;
	settings_.on_body = ss_on_body;
	settings_.on_message_complete = ss_on_message_complete;

	http_parser_init(&parser_, HTTP_REQUEST);
	parser_.data = this;
}

void HTTPSocket::OnRead()
{
	uint32 packet_len = GetReadBuffer().GetSize();
	char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());

	http_parser_execute(&parser_, &settings_, buffer_start, packet_len);

	if (parser_.http_errno != 0) //有错误
	{
		std::string err_msg = http_errno_description(HTTP_PARSER_ERRNO(&parser_));
		LOG_ERROR(g_logger, "parse request error! msg:%s", err_msg.c_str());

		request_.reset();

		Disconnect();
	}

	GetReadBuffer().Remove(packet_len);
}
