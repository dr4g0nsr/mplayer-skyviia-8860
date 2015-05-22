/*!
    @file
@verbatim
$Id: CDivXTcpConn.h 49106 2008-02-07 18:42:23Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _CDIVXTCPCONN_H_
#define _CDIVXTCPCONN_H_

#include "DivXInt.h"

#include "CDivXNetConn.h"
#include "CDivXMutexRW.h"


namespace DivXOS
{
  /*! @brief TcpConn class object */
  class CDivXTcpConn : public CDivXNetBase, public CDivXNetConn
  {
      public:

          CDivXTcpConn(DivXMem hMem);
          virtual ~CDivXTcpConn();

          /*! initialize connection using specified socket */
          int Init(void *connSocket);

          /*! connect to the specified host and port, with optional keep alive */
          virtual int Connect(const char *host, uint16_t port, bool keepalive);

          /*! connect to the specified host and port */
          virtual int Connect(const char *host, uint16_t port);

          /*! disconnect */
          virtual int Disconnect();

          /*! receive the specified number of bytes */
          virtual int Recv(void *buffer, uint32_t *bytes, bool async = false);

          /*! send the specified number of bytes */
          virtual int Send(void *buffer, uint32_t *bytes, bool async = false);

          /*! enable / disable no delay transfers */
          virtual int SetNoDelay(bool bNoDelay);

          /*! query if connection is closed */
          virtual bool IsClosed();

      private:

          /*! update cached IP / port */
          void UpdateCache();

          /*! connection socket */
          void *connSocket;

          /*! connection socket mutex */
          CDivXMutexRW connSocketMutex;

          /*! cached FIONBIO state */
          int cachedFIONBIO;

          /*! Memory handler, set to NULL for aligned memory */
          DivXMem hMem;
  };
}

#endif
