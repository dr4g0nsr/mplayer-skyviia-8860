/*!
    @file
@verbatim
$Id: CDivXNetConn.h 49106 2008-02-07 18:42:23Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _CDIVXNETCONN_H_
#define _CDIVXNETCONN_H_

#include "DivXInt.h"
#include "DivXTypes.h"
#include "DivXString.h"
#include "CDivXNetBase.h"

namespace DivXOS
{
  /*! @brief NetConn class object */
  class CDivXNetConn
  {
      public:

          CDivXNetConn();
          virtual ~CDivXNetConn();

          /*! connect to the specified host and port */
          virtual int Connect(const char *host, uint16_t port) = 0;

          /*! disconnect */
          virtual int Disconnect() = 0;

          /*! receive the specified number of bytes */
          virtual int Recv(void *buffer, uint32_t *bytes, bool async = false) = 0;

          /*! send the specified number of bytes */
          virtual int Send(void *buffer, uint32_t *bytes, bool async = false) = 0;

          /*! enable / disable no delay transfers */
          virtual int SetNoDelay(bool bNoDelay) = 0;

          /*! query if connection is closed */
          virtual bool IsClosed() = 0;

          /*! update remote information */
          virtual int UpdateRemoteInfo(const char *localIpAddr, const char *ipAddr, uint16_t port, DivXMem hMem);

          /*! return MAC address (NET_DISCONNECTED if not connected) */
          int GetMAC(char *buff, int maxSize)
          {
              if(cachedMAC == 0) { return NET_DISCONNECTED; }

              DivXStringCpyChar(buff, cachedMAC, maxSize);

              return NET_OK;
          }

          /*! return IP address (NET_DISCONNECTED if not connected) */
          int GetIP(char *buff, int maxSize) 
          { 
              if(cachedIP == 0) { return NET_DISCONNECTED; } 

              DivXStringCpyChar(buff, cachedIP, maxSize); 

              return NET_OK;
          }

          /*! return port number we are connected to (0 if not connected) */
          int GetPort() { return cachedPort; }

          /*! receive an unsigned 16 bit integer */
          int Recv_uint16(uint16_t &value);

          /*! send an unsigned 16 bit integer */
          int Send_uint16(uint16_t value);

          /*! receive a generic type/length/value triplet */
          int RecvTLV(uint16_t &type, uint16_t &length, void *value);

          /*! send a generic type/length/value triplet */
          int SendTLV(uint16_t type, uint16_t length, void *value);

      protected:

          /*! get MAC address of the specified IP address */
          int GetIPsMAC(const char *ipAddr, char *macAddr);

          /*! cached MAC address */
          char *cachedMAC;

          /*! cached local IP address */
          char *cachedLocalIP;

          /*! cached IP address */
          char *cachedIP;

          /*! cached listening port */
          uint16_t cachedPort;
  };

  /*! Wildcard string that represents local MAC address */
  #define NETCONN_LOCAL_MAC "XX-XX-XX-XX-XX-XX"
}

#endif
