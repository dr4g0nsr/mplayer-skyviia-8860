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

#ifndef PORTABLENETWORK_H
#define PORTABLENETWORK_H

#include "DivXInt.h"
#include "DivXBool.h"

#define UDP_PACKET_SIZE 2048
#define MAX_FD_SET_SIZE 10
#define MAX_IP_LEN 16
#define MAX_HOST_LEN 512
#define PN_INVALID_SOCKET (-1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PNAddr
{
    uint16_t port;
    int8_t host[MAX_HOST_LEN];
}
PNAddr;

typedef enum _PNError
{
    PNOK,
    PNFail,
    PNWouldBlock,
}
PNError;

typedef enum _PNParam
{
    PNSockDGram,
    PNSockStream,
}
PNParam;

/*! used for windows wsa startup */
PNError PNCreate();

/*! used for windows wsa cleanup */
PNError PNDestroy();

/*! open a network socket */
PNError PNOpen(int32_t *sock, PNParam param);

/*! close a network socket */
PNError PNClose(int32_t sock);

/*! bind to specific port, NULL ip for any interface */
PNError PNBind(int32_t sock, uint16_t *port, uint8_t *ip);

/*! setup listening queue */
PNError PNListen(int32_t sock, uint32_t num);

/*! accept incoming connection */
PNError PNAccept(int32_t listenSock, int32_t *acceptSock, PNAddr *addr);

/*! connect a reliable data stream */
PNError PNConnect(int32_t sock, PNAddr *addr);

/*! receive reliable data */
PNError PNRecv(int32_t sock, void* buf, int32_t *size);

/*! send reliable data */
PNError PNSend(int32_t sock, void* buf, int32_t *size);

/*! sets socket to blocking */
PNError PNSetBlocking(int32_t sock, DivXBool blocking);

/*! wait for file descriptor to be readable */
PNError PNWaitReadable(int32_t sock, uint32_t msTimeout, DivXBool *ready);

/*! wait for file descriptor to be writable */
PNError PNWaitWritable(int32_t sock, uint32_t msTimeout, DivXBool *ready);

/*! suspend execution for specified amount of time */
PNError PNSleep(uint32_t msTime);

#ifdef __cplusplus
}
#endif

#endif

