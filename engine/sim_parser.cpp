#include "common.h"
#include "sim_parser.h"
#include "http_parser.h"
#include "clog.h"
#include "http_help.h"
#include "lua_fix.h"

#define MAX_REQ_SIZE 10485760

extern CLog* g_logger;

std::string RequestParam::get_param(std::string &name) 
{
    std::multimap<std::string, std::string>::iterator i = this->_params.find(name);
    if (i == _params.end()) 
	{
        return std::string();
    }
    return i->second;
}

void RequestParam::get_params(std::string &name, std::vector<std::string> &params) 
{
    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ret = this->_params.equal_range(name);
    for (std::multimap<std::string, std::string>::iterator it=ret.first; it!=ret.second; ++it) 
	{
        params.push_back(it->second);
    }
}

int RequestParam::parse_query_url(std::string &query_url) 
{
    std::stringstream query_ss(query_url);
    //LOG_DEBUG(g_logger, "start parse_query_url:%s", query_url.c_str());

    while(query_ss.good()) 
	{
        std::string key_value;
        std::getline(query_ss, key_value, '&');
        //LOG_DEBUG(g_logger, "get key_value:%s", key_value.c_str());

        std::stringstream key_value_ss(key_value);
        while(key_value_ss.good()) 
		{
            std::string key, value;
            std::getline(key_value_ss, key, '=');
            std::getline(key_value_ss, value, '=');
            _params.insert(std::pair<std::string, std::string>(key, value));
        }
    }
    return 0;
}
int RequestParam::add_param_pair(const std::string &key, const std::string &value) 
{
    _params.insert(std::pair<std::string, std::string>(key, value));
    return 0;
}

void RequestParam::reset()
{
	_params.clear();
}

//---------------------------------------------------------------------------------------------------------
std::string RequestLine::get_request_uri() 
{
    std::stringstream ss(this->get_request_url());
    std::string uri;
    std::getline(ss, uri, '?');
    return uri;
}

RequestParam& RequestLine::get_request_param() 
{
    return _param;
}

std::string RequestLine::to_string() 
{
    std::string ret = "method:";
    ret += _method;
    ret += ",";
    ret += "request_url:";
    ret += _request_url;
    ret += ",";
    ret += "http_version:";
    ret += _http_version;
    return ret;
}

int RequestLine::parse_request_url_params() 
{
    std::stringstream ss(_request_url);
    //LOG_DEBUG(g_logger, "start parse params which request_url:%s", _request_url.c_str());

    std::string uri;
    std::getline(ss, uri, '?');
    if(ss.good()) {
        std::string query_url;
        std::getline(ss, query_url, '?');

        _param.parse_query_url(query_url);
    }
    return 0;
}

std::string RequestLine::get_method() 
{
    return _method;
}

void RequestLine::set_method(std::string m) 
{
    _method = m;
}

std::string RequestLine::get_request_url() 
{
    return _request_url;
}

void RequestLine::set_request_url(std::string url) 
{
    _request_url = url;
}

void RequestLine::append_request_url(std::string p_url) 
{
    _request_url += p_url;
}

std::string RequestLine::get_http_version() 
{
    return _http_version;
}

void RequestLine::set_http_version(const std::string &v) 
{
    _http_version = v;
}

void RequestLine::reset()
{
	_param.reset();
	_request_url = "";
	_method = "";
	_http_version = "";
}


//---------------------------------------------------------------------------------------------------------
std::string RequestBody::get_param(std::string name) 
{
    return _req_params.get_param(name);
}

void RequestBody::get_params(std::string &name, std::vector<std::string> &params) 
{
    return _req_params.get_params(name, params);
}

std::string *RequestBody::get_raw_string() 
{
    return &_raw_string;
}

RequestParam *RequestBody::get_req_params() 
{
    return &_req_params;
}

void RequestBody::reset()
{
	_raw_string = "";
	_req_params.reset();
}

void RequestBody::Decode()
{
	_raw_string = UrlDecode(_raw_string);
}

//---------------------------------------------------------------------------------------------------------
Request::Request() 
{
    _parse_part = PARSE_REQ_LINE;
    _last_was_value = true; // add new field for first
}

Request::~Request() 
{

}

void Request::reset()
{
	_header_fields.clear();
	_header_values.clear();
	_headers.clear();

	_body.reset();
	_line.reset();

	_parse_part = PARSE_REQ_LINE;
}

void Request::BuildLuaResult(lua_State* L)
{
    /*
    {
        ["method"] = "xxx",
        ["url"] = "xxx",
		["uri"] = "xxx",
        ["body"] = "xxx",
		["head_params"] = {
				["id"] = 2,
				["name"] = "Kof"
		},
		["params"] = {
				["id"] = 2,
				["name"] = "Kof"
		},
		["body_params"] = {
				["id"] = 2,
				["name"] = "Kof"
		}
    }
    */

	lua_newtable(L);//创建一个表格，放在栈顶
    //---------------------------------------------------------------------
    lua_pushstring(L, "method"); //key
    lua_pushstring(L, get_method().c_str()); //value
    lua_settable(L, -3);
    //---------------------------------------------------------------------
	lua_pushstring(L, "url"); //key
	lua_pushstring(L, get_request_url().c_str()); //value
	lua_settable(L, -3);
    //---------------------------------------------------------------------
	lua_pushstring(L, "uri"); //key
	lua_pushstring(L, get_request_uri().c_str()); //value
	lua_settable(L, -3);
    //---------------------------------------------------------------------
	lua_pushstring(L, "body"); //key
	lua_pushstring(L, get_body()->get_raw_string()->c_str()); //value
	lua_settable(L, -3);
    //---------------------------------------------------------------------
	lua_pushstring(L, "head_params"); //key
    lua_newtable(L);//创建一个表格
	
	{
		for (map<string, string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		{
			lua_pushstring(L, it->first.c_str()); //key
			lua_pushstring(L, it->second.c_str()); //value
			lua_settable(L, -3);
		}
	}

	lua_settable(L, -3);
    //---------------------------------------------------------------------
	lua_pushstring(L, "params"); //key
	lua_newtable(L);//创建一个表格
	{
		multimap<std::string, std::string>& params = _line.get_request_param()._params;
		for (multimap<std::string, std::string>::iterator it = params.begin(); it != params.end(); it++)
		{
			lua_pushstring(L, it->first.c_str()); //key
			lua_pushstring(L, it->second.c_str()); //value
			lua_settable(L, -3);
		}
	}

	lua_settable(L, -3);
	//---------------------------------------------------------------------
	lua_pushstring(L, "body_params"); //key
	lua_newtable(L);//创建一个表格
	{
		multimap<std::string, std::string>& params = _body.get_req_params()->_params;
		for (multimap<std::string, std::string>::iterator it = params.begin(); it != params.end(); it++)
		{
			lua_pushstring(L, it->first.c_str()); //key
			lua_pushstring(L, it->second.c_str()); //value
			lua_settable(L, -3);
		}
	}

	lua_settable(L, -3);
}

RequestBody *Request::get_body() 
{
    return &_body;
}

std::string Request::get_method() 
{
    return _line.get_method();
}

std::string Request::get_param(std::string name) 
{
	return _line.get_request_param().get_param(name);
}

std::string Request::get_body_param(std::string name) 
{
	return _body.get_param(name);
}

void Request::get_params(std::string &name, std::vector<std::string> &params) 
{
	if (_line.get_method() == "GET")
	{
		_line.get_request_param().get_params(name, params);
	}

	if (_line.get_method() == "POST")
	{
		_body.get_params(name, params);
	}
}

void Request::add_header(std::string &name, std::string &value) 
{
	this->_headers[name] = value;
}

std::string Request::get_header(std::string name) 
{
	return this->_headers[name];
}

std::string Request::get_request_uri() 
{
	return _line.get_request_uri();
}

std::string Request::get_request_url() 
{
	return _line.get_request_url();
}

//---------------------------------------------------------------------------------------------------------
Response::Response(CodeMsg status_code) 
{
    _code_msg = status_code;
}

void Response::set_error(const int code, const std::string& msg)
{
    _code_msg.status_code = code;
    _code_msg.msg = msg;
}

void Response::set_head(const std::string &name, const std::string &value) 
{
    _headers[name] = value;
}

void Response::set_body(string& body) 
{
	this->_body = body;
}

void Response::gen_response()
{
	_res_bytes << "HTTP/1.1" << " " << _code_msg.status_code << " " << _code_msg.msg << "\r\n";

	_res_bytes << "Server: portalserver/1.0\r\n";

	if (_headers.find("Content-Type") == _headers.end()) //默认值
	{
		_res_bytes << "Content-Type: text/html" << "\r\n";
	}

	if (_headers.find("Content-Length") == _headers.end())
	{
		_res_bytes << "Content-Length: " << _body.size() << "\r\n";
	}

	//std::string con_status = "Connection: close";
	//_res_bytes << con_status << "\r\n";

	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		_res_bytes << it->first << ": " << it->second << "\r\n";
	}

	// header end
	_res_bytes << "\r\n";
	_res_bytes << _body;
}

void Response::clear()
{
    _code_msg = STATUS_OK;
    _headers.clear();
    _body = "";
    _res_bytes.str("");
}
