/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "clog.h"
#include "sequence_buffer.h"
#include "socket_defines.h"
#include "socket_ops.h"
#include "singleton.h"
#include "socket.h"
#include "listen_socket.h"
#include "tcp_listen_socket.h"
#include "http_listen_socket.h"
#include "web_listen_socket.h"
#include "udp_listen_socket.h"
#include "kcp_listen_socket.h"

#ifdef WIN32
#include "socket_mgr_win32.h"
#else
#include "socket_mgr_linux.h"
#endif

#endif //__NETWORK_H__
