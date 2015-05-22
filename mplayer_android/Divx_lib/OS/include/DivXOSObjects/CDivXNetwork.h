/*!
    @file
@verbatim
$Id: CDivXNetwork.h 49106 2008-02-07 18:42:23Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _CDIVXNETWORK_H_
#define _CDIVXNETWORK_H_

#include "DivXInt.h"
#include "DivXError.h"

namespace DivXOS
{
  /*! @brief DivXNetwork class object */
  class CDivXNetwork
  {
      public:

          CDivXNetwork(){};
          virtual ~CDivXNetwork(){};

          /*! initialize network connection*/
          static DivXError Init(void);

          /*! shutdown network connection*/
          static DivXError DeInit(void);

      private:
  };
}

#endif
