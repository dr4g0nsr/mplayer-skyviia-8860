/*!
    @file
   @verbatim
   $Id: AVI1IndexCacheEntry.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _AVII1NDEXCACHEENTRY_H_
#define _AVII1NDEXCACHEENTRY_H_

#include "DivXInt.h"
#include "DivXTime.h"
#include "DMFBlockType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AVI1IndexCacheEntry_t
{
    DMFBlockType_t blockType;
    uint32_t       nStreamOfType;
    uint32_t       offsetIntoMovi;
    uint32_t       offsetIntoIndex;
    DivXTime       absoluteTime;
} AVI1IndexCacheEntry_t;

#ifdef __cplusplus
}
#endif

#endif /* _AVII1NDEXCACHEENTRY_H_ */
