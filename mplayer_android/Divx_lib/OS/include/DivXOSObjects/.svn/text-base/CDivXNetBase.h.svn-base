/*!
    @file
@verbatim
$Id: CDivXNetBase.h 49106 2008-02-07 18:42:23Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _CDIVXNETBASE_H_
#define _CDIVXNETBASE_H_

#include "DivXInt.h"

/*lint -esym( 1510, NetBase ) NetBase has no destructor by design */
namespace DivXOS
{
  /*! @brief NetBase class object */
  class CDivXNetBase
  {
      public:

      protected:

          /*! enable blocking socket */
          int EnableBlocking(void *&sock, int &cachedFIONBIO);

          /*! disable blocking socket */
          int DisableBlocking(void *&sock, int &cachedFIONBIO);

      private:
  };

  /*! \name Return results */
  /*! \{ */
  #define NET_OK                  0x0000
  #define NET_FAIL                0x0001
  #define NET_NOTIMPL             0x0002
  #define NET_INVALID_PARAM       0x0003
  #define NET_OUTOFMEMORY         0x0004
  #define NET_ACCESS_DENIED       0x0005
  #define NET_INVALID_CALL        0x0006
  #define NET_HOST_NOT_FOUND      0x0007
  #define NET_BUFFER_FULL         0x0008
  #define NET_SOCKET_ERROR        0x0009
  #define NET_DISCONNECTED        0x000A
  #define NET_VALUE_NOT_SET       0x000B
  /*! \} */

  /*! Helper function for detecting success */
  #define NET_SUCCESS(x) (x == NET_OK)
  /*! Helper function for detecting failure */
  #define NET_FAILED(x) (x != NET_OK)

}

#endif
