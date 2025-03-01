#include "lua_network.h"
#include "lua_fix.h"

int lua_tcpserver_new(lua_State* L)
{
	tcpserver_t* t = (tcpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new TCPServer();

	luaL_getmetatable(L, "tcpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_tcpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	bool ret = t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	lua_pushboolean(L, ret);
	return 1;
}

int lua_tcpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	t->server->Close();

	return 0;
}

int lua_tcpserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_tcpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_tcpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	if (t->server) 
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_tcpclient_new(lua_State* L)
{
	tcpclient_t* t = (tcpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new TCPClient();

	luaL_getmetatable(L, "tcpclient");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_tcpclient_connectex(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnt");
	}
	else if (count == 10)
	{
		check_param(L, 11, "usnfffnntt");
	}
	else
	{
		check_param(L, 11, "usnfffnnttt");
	}

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	t->client->ConnectEx(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);

	return 0;
}

int lua_tcpclient_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpclient_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->client->Disconnect(conn_idx);

	return 0;
}

int lua_tcpclient_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->client->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);
	return 1;
}

int lua_tcpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_udpserver_new(lua_State* L)
{
	udpserver_t* t = (udpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new UDPServer();

	luaL_getmetatable(L, "udpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_udpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	bool ret = t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	lua_pushboolean(L, ret);
	return 1;
}

int lua_udpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	t->server->Close();

	return 0;
}

int lua_udpserver_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_udpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_udpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	if (t->server)
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}
//-------------------------------------------------------------------------------------
int lua_udpclient_new(lua_State* L)
{
	udpclient_t* t = (udpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new UDPClient();

	luaL_getmetatable(L, "udpclient");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_udpclient_connect(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnt");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnntt");
	}
	else
	{
		check_param(L, 11, "usnfffnnttt");
	}

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	uint16 local_port = 0;
	t->client->Connect(ip, port, local_port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);

	return 0;
}

int lua_udpclient_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpclient_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpclient_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->client->Disconnect(conn_idx);

	return 0;
}

int lua_udpclient_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->client->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);
	return 1;
}

int lua_udpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_kcpserver_new(lua_State* L)
{
	kcpserver_t* t = (kcpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new KCPServer();

	luaL_getmetatable(L, "kcpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_kcpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	bool ret = t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	lua_pushboolean(L, ret);
	return 1;
}

int lua_kcpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");
	t->server->Close();

	return 0;
}

int lua_kcpserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_kcpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_kcpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_kcpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	kcpserver_t* t = (kcpserver_t*)luaL_checkudata(L, 1, "kcpserver");
	if (t->server)
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}
//-------------------------------------------------------------------------------------
int lua_kcpclient_new(lua_State* L)
{
	kcpclient_t* t = (kcpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new KCPClient();

	luaL_getmetatable(L, "kcpclient");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_kcpclient_connect(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnt");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnntt");
	}
	else
	{
		check_param(L, 11, "usnfffnnttt");
	}

	kcpclient_t* t = (kcpclient_t*)luaL_checkudata(L, 1, "kcpclient");
	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	uint16 local_port = 0;
	t->client->Connect(ip, port, local_port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);

	return 0;
}

int lua_kcpclient_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	kcpclient_t* t = (kcpclient_t*)luaL_checkudata(L, 1, "kcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_kcpclient_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	kcpclient_t* t = (kcpclient_t*)luaL_checkudata(L, 1, "kcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->client->Disconnect(conn_idx);

	return 0;
}

int lua_kcpclient_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	kcpclient_t* t = (kcpclient_t*)luaL_checkudata(L, 1, "kcpclient");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->client->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);
	return 1;
}

int lua_kcpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	kcpclient_t* t = (kcpclient_t*)luaL_checkudata(L, 1, "kcpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_httpserver_new(lua_State* L)
{
	httpserver_t* t = (httpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new HTTPServer();

	luaL_getmetatable(L, "httpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_httpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	bool ret = t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	lua_pushboolean(L, ret);
	return 1;
}

int lua_httpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	t->server->Close();

	return 0;
}

int lua_httpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_httpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_httpserver_clear(lua_State* L)
{
	check_param(L, 2, "un");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Clear(conn_idx);

	return 0;
}

int lua_httpserver_set_error(lua_State* L)
{
	check_param(L, 4, "unns");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	int code = (uint32)lua_tointeger(L, 3);
	string msg = lua_tostring(L, 4);
	t->server->SetError(conn_idx, code, msg);

	return 0;
}

int lua_httpserver_set_body(lua_State* L)
{
	check_param(L, 3, "uns");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	string body = lua_tostring(L, 3);
	t->server->SetBody(conn_idx, body);

	return 0;
}

int lua_httpserver_response(lua_State* L)
{
	check_param(L, 2, "un");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Response(conn_idx);

	return 0;
}

int lua_httpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	httpserver_t* t = (httpserver_t*)luaL_checkudata(L, 1, "httpserver");
	if (t->server)
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_httpclient_new(lua_State* L)
{
	httpclient_t* t = (httpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new HTTPAccessor();

	luaL_getmetatable(L, "httpclient");
	lua_setmetatable(L, -2);
	return 1;
}

int lua_httpclient_init(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	bool ret = t->client->Init();
	lua_pushboolean(L, ret);

	return 1;
}

int lua_httpclient_close(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	t->client->Close();

	return 0;
}

int lua_httpclient_settimeout(lua_State* L)
{
	check_param(L, 2, "un");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	uint32 time = (uint32)lua_tointeger(L, 2);
	t->client->SetTimeout(time);

	return 0;
}

int lua_httpclient_get(lua_State* L)
{
	check_param(L, 4, "usfs");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");

	const char* url = lua_tostring(L, 2);
	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 3);
	handler.fun_id = fun_id;
	const char* custom_http_head = lua_tostring(L, 4);

	t->client->Get(url, handler, custom_http_head);

	return 0;
}

int lua_httpclient_post(lua_State* L)
{
	check_param(L, 5, "ussfs");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");

	const char* url = lua_tostring(L, 2);
	const char* data = lua_tostring(L, 3);
	
	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 4);
	handler.fun_id = fun_id;
	const char* custom_http_head = lua_tostring(L, 5);

	t->client->Post(url, data, handler, custom_http_head);

	return 0;
}

int lua_httpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_websocketserver_new(lua_State* L)
{
	websocketserver_t* t = (websocketserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new WebSocketServer();

	luaL_getmetatable(L, "websocketserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_websocketserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count >= 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count >= 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count >= 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	bool ret = t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	lua_pushboolean(L, ret);
	return 1;
}

int lua_websocketserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");
	t->server->Close();

	return 0;
}

int lua_websocketserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_websocketserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_websocketserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");
	int32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_websocketserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	websocketserver_t* t = (websocketserver_t*)luaL_checkudata(L, 1, "websocketserver");
	if (t->server)
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}



int luaopen_lua_tcpserver(lua_State* L)
{
	luaL_newmetatable(L, "tcpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_tcpserver_methods, 0);
	luaL_newlib(L, lua_tcpserver_functions);

	return 1;
}

int luaopen_lua_tcpclient(lua_State* L)
{
	luaL_newmetatable(L, "tcpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_tcpclient_methods, 0);
	luaL_newlib(L, lua_tcpclient_functions);

	return 1;
}

int luaopen_lua_udpserver(lua_State* L)
{
	luaL_newmetatable(L, "udpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_udpserver_methods, 0);
	luaL_newlib(L, lua_udpserver_functions);

	return 1;
}

int luaopen_lua_udpclient(lua_State* L)
{
	luaL_newmetatable(L, "udpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_udpclient_methods, 0);
	luaL_newlib(L, lua_udpclient_functions);

	return 1;
}

int luaopen_lua_kcpserver(lua_State* L)
{
	luaL_newmetatable(L, "kcpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_kcpserver_methods, 0);
	luaL_newlib(L, lua_kcpserver_functions);

	return 1;
}

int luaopen_lua_kcpclient(lua_State* L)
{
	luaL_newmetatable(L, "kcpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_kcpclient_methods, 0);
	luaL_newlib(L, lua_kcpclient_functions);

	return 1;
}

int luaopen_lua_httpserver(lua_State* L)
{
	luaL_newmetatable(L, "httpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_httpserver_methods, 0);
	luaL_newlib(L, lua_httpserver_functions);

	return 1;
}

int luaopen_lua_httpclient(lua_State* L)
{
	luaL_newmetatable(L, "httpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_httpclient_methods, 0);
	luaL_newlib(L, lua_httpclient_functions);

	return 1;
}

int luaopen_lua_websocketserver(lua_State* L)
{
	luaL_newmetatable(L, "websocketserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_websocketserver_methods, 0);
	luaL_newlib(L, lua_websocketserver_functions);

	return 1;
}