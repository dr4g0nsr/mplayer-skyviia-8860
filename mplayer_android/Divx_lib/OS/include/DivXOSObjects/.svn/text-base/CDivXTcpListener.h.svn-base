// $Id: CDivXTcpListener.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include "DivXInt.h"
#include "CDivXNetBase.h"
#include "CDivXTcpConn.h"
#include "CDivXMutex.h"

namespace DivXOS
{
    /*! @brief TcpListener class object */
    class CDivXTcpListener : public CDivXNetBase
    {
        public:

            CDivXTcpListener(DivXMem hMem);
            virtual ~CDivXTcpListener();

            /*! listen for incoming connections on a random port */
            int Listen() { return Listen(0); }

            /*! listen for incoming connections on the specified port */
            int Listen(uint16_t port);

            /*! accept an incoming connection, NET_OK for success, NET_DISCONNECTED if async did not yet connect, NET_ error code for failure */
            int Accept(CDivXTcpConn *pTcpConn, bool async = false, bool keepalive = false);

            /*! return port number we are listening on (0 if not listening) */
            int GetPort() { return cachedPort; }

        private:

            /*! listening socket */
            void *listenSocket;

            /*! listening socket mutex */
            CDivXMutex listenSocketMutex;

            /*! cached listening port */
            uint16_t cachedPort;

            /*! cached FIONBIO state */
            int cachedFIONBIO;

            /*! Memory handler, set to NULL for default maligned memory */
            DivXMem hMem;
    };
}

#endif
