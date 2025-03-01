/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef LUA_NETWORK_H
#define LUA_NETWORK_H

#include "tcp_server.h"
#include "tcp_client.h"
#include "udp_server.h"
#include "udp_client.h"
#include "kcp_server.h"
#include "kcp_client.h"
#include "http_accessor.h"
#include "http_server.h"
#include "websocket_server.h"

//-------------------------------------------------------------------------------------
//					TCPServer接口
//-------------------------------------------------------------------------------------
int lua_tcpserver_new(lua_State* L);
int lua_tcpserver_start(lua_State* L);
int lua_tcpserver_close(lua_State* L);
int lua_tcpserver_sendmsg(lua_State* L);
int lua_tcpserver_disconnect(lua_State* L);
int lua_tcpserver_getipaddress(lua_State* L);
int lua_tcpserver_destroy(lua_State* L);

static const struct luaL_Reg lua_tcpserver_methods[] = {
	{ "start", lua_tcpserver_start },
	{ "close", lua_tcpserver_close },
	{ "send_msg", lua_tcpserver_sendmsg },
	{ "disconnect", lua_tcpserver_disconnect },
	{ "get_ip_address", lua_tcpserver_getipaddress },
	{ "__gc",  lua_tcpserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_tcpserver_functions[] = {
	{ "new", lua_tcpserver_new },
	{ NULL, NULL }
};

typedef struct
{
	TCPServer* server;
} tcpserver_t;

//-------------------------------------------------------------------------------------
//					TCPClient接口
//-------------------------------------------------------------------------------------
int lua_tcpclient_new(lua_State* L);
int lua_tcpclient_connectex(lua_State* L);
int lua_tcpclient_sendmsg(lua_State* L);
int lua_tcpclient_disconnect(lua_State* L);
int lua_tcpclient_getipaddress(lua_State* L);
int lua_tcpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_tcpclient_methods[] = {
	{ "connect", lua_tcpclient_connectex },
	{ "send_msg", lua_tcpclient_sendmsg },
	{ "disconnect", lua_tcpclient_disconnect },
	{ "get_ip_address", lua_tcpclient_getipaddress },
	{ "__gc",  lua_tcpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_tcpclient_functions[] = {
	{ "new", lua_tcpclient_new },
	{ NULL, NULL }
};

typedef struct {
	TCPClient* client;
} tcpclient_t;

//-------------------------------------------------------------------------------------
//					UDPServer接口
//-------------------------------------------------------------------------------------
int lua_udpserver_new(lua_State* L);
int lua_udpserver_start(lua_State* L);
int lua_udpserver_close(lua_State* L);
int lua_udpserver_send(lua_State* L);
int lua_udpserver_sendmsg(lua_State* L);
int lua_udpserver_disconnect(lua_State* L);
int lua_udpserver_getipaddress(lua_State* L);
int lua_udpserver_destroy(lua_State* L);

static const struct luaL_Reg lua_udpserver_methods[] = {
	{ "start", lua_udpserver_start },
	{ "close", lua_udpserver_close },
	{ "send", lua_udpserver_send },
	{ "send_msg", lua_udpserver_sendmsg },
	{ "disconnect", lua_udpserver_disconnect },
	{ "get_ip_address", lua_udpserver_getipaddress },
	{ "__gc",  lua_udpserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_udpserver_functions[] = {
	{ "new", lua_udpserver_new },
	{ NULL, NULL }
};

typedef struct
{
	UDPServer* server;
} udpserver_t;

//-------------------------------------------------------------------------------------
//					UDPClient接口
//-------------------------------------------------------------------------------------
int lua_udpclient_new(lua_State* L);
int lua_udpclient_connect(lua_State* L);
int lua_udpclient_send(lua_State* L);
int lua_udpclient_sendmsg(lua_State* L);
int lua_udpclient_disconnect(lua_State* L);
int lua_udpclient_getipaddress(lua_State* L);
int lua_udpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_udpclient_methods[] = {
	{ "connect", lua_udpclient_connect },
	{ "send", lua_udpclient_send },
	{ "send_msg", lua_udpclient_sendmsg },
	{ "disconnect", lua_udpclient_disconnect },
	{ "get_ip_address", lua_udpclient_getipaddress },
	{ "__gc",  lua_udpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_udpclient_functions[] = {
	{ "new", lua_udpclient_new },
	{ NULL, NULL }
};

typedef struct {
	UDPClient* client;
} udpclient_t;


//-------------------------------------------------------------------------------------
//					KCPServer接口
//-------------------------------------------------------------------------------------
int lua_kcpserver_new(lua_State* L);
int lua_kcpserver_start(lua_State* L);
int lua_kcpserver_close(lua_State* L);
int lua_kcpserver_sendmsg(lua_State* L);
int lua_kcpserver_disconnect(lua_State* L);
int lua_kcpserver_getipaddress(lua_State* L);
int lua_kcpserver_destroy(lua_State* L);

static const struct luaL_Reg lua_kcpserver_methods[] = {
	{ "start", lua_kcpserver_start },
	{ "close", lua_kcpserver_close },
	{ "send_msg", lua_kcpserver_sendmsg },
	{ "disconnect", lua_kcpserver_disconnect },
	{ "get_ip_address", lua_kcpserver_getipaddress },
	{ "__gc",  lua_kcpserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_kcpserver_functions[] = {
	{ "new", lua_kcpserver_new },
	{ NULL, NULL }
};

typedef struct
{
	KCPServer* server;
} kcpserver_t;

//-------------------------------------------------------------------------------------
//					KCPClient接口
//-------------------------------------------------------------------------------------
int lua_kcpclient_new(lua_State* L);
int lua_kcpclient_connect(lua_State* L);
int lua_kcpclient_sendmsg(lua_State* L);
int lua_kcpclient_disconnect(lua_State* L);
int lua_kcpclient_getipaddress(lua_State* L);
int lua_kcpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_kcpclient_methods[] = {
	{ "connect", lua_kcpclient_connect },
	{ "send_msg", lua_kcpclient_sendmsg },
	{ "disconnect", lua_kcpclient_disconnect },
	{ "get_ip_address", lua_kcpclient_getipaddress },
	{ "__gc",  lua_kcpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_kcpclient_functions[] = {
	{ "new", lua_kcpclient_new },
	{ NULL, NULL }
};

typedef struct {
	KCPClient* client;
} kcpclient_t;

//-------------------------------------------------------------------------------------
//					HttpServer接口
//-------------------------------------------------------------------------------------
int lua_httpserver_new(lua_State* L);
int lua_httpserver_start(lua_State* L);
int lua_httpserver_close(lua_State* L);
int lua_httpserver_disconnect(lua_State* L);
int lua_httpserver_getipaddress(lua_State* L);
int lua_httpserver_clear(lua_State* L);
int lua_httpserver_set_error(lua_State* L);
int lua_httpserver_set_body(lua_State* L);
int lua_httpserver_response(lua_State* L);
int lua_httpserver_destroy(lua_State* L);

static const struct luaL_Reg lua_httpserver_methods[] = {
	{ "start", lua_httpserver_start },
	{ "close", lua_httpserver_close },
	{ "disconnect", lua_httpserver_disconnect },
	{ "get_ip_address", lua_httpserver_getipaddress },

	{ "clear", lua_httpserver_clear },
	{ "set_error", lua_httpserver_set_error },
	{ "set_body", lua_httpserver_set_body },
	{ "response", lua_httpserver_response },

	{ "__gc",  lua_httpserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_httpserver_functions[] = {
	{ "new", lua_httpserver_new },
	{ NULL, NULL }
};

typedef struct
{
	HTTPServer* server;
} httpserver_t;


//-------------------------------------------------------------------------------------
//					HttpClient接口 (使用curl)
//-------------------------------------------------------------------------------------
int lua_httpclient_new(lua_State* L);
int lua_httpclient_init(lua_State* L);
int lua_httpclient_close(lua_State* L);
int lua_httpclient_settimeout(lua_State* L);
int lua_httpclient_get(lua_State* L);
int lua_httpclient_post(lua_State* L);
int lua_httpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_httpclient_methods[] = {
	{ "init", lua_httpclient_init },
	{ "close", lua_httpclient_close },
	{ "set_timeout", lua_httpclient_settimeout },
	{ "get", lua_httpclient_get },
	{ "post", lua_httpclient_post },
	{ "__gc",  lua_httpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_httpclient_functions[] = {
	{ "new", lua_httpclient_new },
	{ NULL, NULL }
};

typedef struct {
	HTTPAccessor* client;
} httpclient_t;


//-------------------------------------------------------------------------------------
//					WebSocketServer接口
//-------------------------------------------------------------------------------------
int lua_websocketserver_new(lua_State* L);
int lua_websocketserver_start(lua_State* L);
int lua_websocketserver_close(lua_State* L);
int lua_websocketserver_sendmsg(lua_State* L);
int lua_websocketserver_disconnect(lua_State* L);
int lua_websocketserver_getipaddress(lua_State* L);
int lua_websocketserver_destroy(lua_State* L);

static const struct luaL_Reg lua_websocketserver_methods[] = {
	{ "start", lua_websocketserver_start },
	{ "close", lua_websocketserver_close },
	{ "send_msg", lua_websocketserver_sendmsg },
	{ "disconnect", lua_websocketserver_disconnect },
	{ "get_ip_address", lua_websocketserver_getipaddress },
	{ "__gc",  lua_websocketserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_websocketserver_functions[] = {
	{ "new", lua_websocketserver_new },
	{ NULL, NULL }
};

typedef struct
{
	WebSocketServer* server;
} websocketserver_t;


//-------------------------------------------------------------------------------------
//							luaopen接口
//-------------------------------------------------------------------------------------
int luaopen_lua_tcpserver(lua_State* L);
int luaopen_lua_tcpclient(lua_State* L);
int luaopen_lua_udpserver(lua_State* L);
int luaopen_lua_udpclient(lua_State* L);
int luaopen_lua_kcpserver(lua_State* L);
int luaopen_lua_kcpclient(lua_State* L);
int luaopen_lua_httpserver(lua_State* L);
int luaopen_lua_httpclient(lua_State* L);
int luaopen_lua_websocketserver(lua_State* L);

#endif //LUA_NETWORK_H