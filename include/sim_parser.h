#ifndef _HTTP_RESULT_H_
#define _HTTP_RESULT_H_

#include "common.h"

struct CodeMsg
{
	int status_code;
	std::string msg;
};

const static CodeMsg STATUS_OK = { 200, "OK" };
const static CodeMsg STATUS_NOT_FOUND = { 404, "Not Found" };
const static CodeMsg STATUS_METHOD_NOT_ALLOWED = { 405, "Method Not Allowed" };
const static CodeMsg STATUS_LENGTH_REQUIRED = { 411, "Length Required" };

const static int PARSE_REQ_LINE = 0;
const static int PARSE_REQ_HEAD = 1;
const static int PARSE_REQ_HEAD_OVER = 2;
const static int PARSE_REQ_BODY = 3;
const static int PARSE_REQ_OVER = 4;

const static int STATUS_BAD = -1;
const static int STATUS_OVER = 0;
const static int STATUS_NO_OVER = 1;

class RequestParam
{
public:
    /**
     * get param by name
     * when not found , return empty string
     */
    std::string get_param(std::string& name);

    /**
     * get params by name
     * when params in url like age=1&age=2, it will return [1, 2]
     */
    void get_params(std::string& name, std::vector<std::string>& params);

    /**
     * query_url : name=tom&age=3
     */
    int parse_query_url(std::string& query_url);

    int add_param_pair(const std::string& key, const std::string& value);

    void reset();

public:
    std::multimap<std::string, std::string> _params;
};

/**
 * parse for http protocol first line, like below:
 * GET /login?name=tom&age=1 HTTP/1.0
 */
class RequestLine
{
public:
    /**
     * return "GET" or "POST"
     */
    std::string get_method();
    /**
     * like /login
     */
    std::string get_request_uri();
    /**
     * like /login?name=tom&age=1
     */
    std::string get_request_url();
    /**
     * return "HTTP/1.0" or "HTTP/1.1"
     */
    std::string get_http_version();

    RequestParam& get_request_param();

    std::string to_string();
    /**
     * request_url : /sayhello?name=tom&age=3
     */
    int parse_request_url_params();

    void set_method(std::string m);

    void set_request_url(std::string url);

    void append_request_url(std::string p_url);

    void set_http_version(const std::string& v);

    void reset();

private:
    RequestParam _param;
    std::string _method;       // like GET/POST
    std::string _request_url;  // like /hello?name=aaa
    std::string _http_version; // like HTTP/1.1
};

class RequestBody
{
public:

    /**
     * when Content-Type is "application/x-www-form-urlencoded"
     * we will parse the request body , it will excepted like below
     *
     *     "name=tom&age=1"
     *
     */
    std::string get_param(std::string name);

    void get_params(std::string& name, std::vector<std::string>& params);

    void Decode();

    /**
     * get request body bytes
     */
    std::string* get_raw_string();

    RequestParam* get_req_params();

    void reset();

public:
    std::string _raw_string;
    RequestParam _req_params;
};

class Request
{
public:
    Request();
    ~Request();

    std::string get_param(std::string name);
    std::string get_body_param(std::string name);

    void get_params(std::string& name, std::vector<std::string>& params);

    std::string get_header(std::string name);

    /**
     * return like /login
     */
    std::string get_request_uri();

    /**
     * return like /login?name=tom&age=1
     */
    std::string get_request_url();

    RequestBody* get_body();

    std::string get_method();

    void add_header(std::string& name, std::string& value);

    void reset();

    void BuildLuaResult(lua_State* L);

public:
    bool _last_was_value;
    std::vector<std::string> _header_fields;
    std::vector<std::string> _header_values;
    int _parse_part;
    RequestLine _line;

private:
    std::map<std::string, std::string> _headers;
    RequestBody _body;
};

class Response
{
public:
    Response(CodeMsg status_code = STATUS_OK);

    void set_error(const int code, const std::string& msg);
    void set_head(const std::string& name, const std::string& value);
    void set_body(string& body);

    void gen_response();
    void clear();

private:
    CodeMsg _code_msg;

    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    std::stringstream _res_bytes;
};



#endif //_HTTP_RESULT_H_