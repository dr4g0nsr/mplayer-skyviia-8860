/*!
    @file
   @verbatim
   $Id: DMFModuleAttributes.h 56354 2008-10-06 01:02:30Z sbramwell $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMODULEATTRIBUTES_H_
#define _DMFMODULEATTRIBUTES_H_

#include "DivXInt.h"

/*! Module attribute definition structure
**/

typedef struct _DMFModuleAttributes
{
    uint32_t CanSeekForward : 1,
CanSeekReverse: 1,
CanRead: 1,
CanWrite: 1,
CanReadMenu: 1,
CanWriteMenu: 1,
MenuAddSequenceDependent: 1,        /*!< AddMenu / AddTitle sequence is significant */
MenuAddBeforeTitles: 1,             /*!< AddMenu must come before */
_unused: 24;                        /*!< Changed from __unused, the Xcode compiler does not like the __, 23mar2007, GLS */
}
DMFModuleAttributes;

#endif /* _DMF_MODULE_ATTRIBUTES_H_ */
