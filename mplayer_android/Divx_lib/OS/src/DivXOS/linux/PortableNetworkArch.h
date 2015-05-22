/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define SYSTEM_STARTUP

#define SYSTEM_CLEANUP

#define SYSTEM_SLEEP(x)  usleep(x * 1000); 

#define SOCKET_CLOSE(x)  close(sock);  

#define GET_LAST_ERROR   errno 

#define WOULD_BLOCK_ERROR EAGAIN

#define SOCKET_IOCTL     ioctl

#define SNPRINTF  snprintf
