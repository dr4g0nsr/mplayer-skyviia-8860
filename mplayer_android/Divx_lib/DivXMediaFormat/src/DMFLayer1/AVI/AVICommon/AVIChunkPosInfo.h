/*!
    @file
@verbatim
$Id: AVIChunkPosInfo.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVICHUNKPOSINFO_H_
#define _AVICHUNKPOSINFO_H_

#include "DivXInt.h"
#include "DMFBlockType.h"

typedef struct _AVIChunkPosInfo_t
{
    uint64_t       offsetIntoMovi;
    uint32_t       sizeChunk;
    DMFBlockType_t blockType;
    int32_t        nStreamOfType;
} AVIChunkPosInfo_t;

#endif /* _AVICHUNKPOSINFO_H_ */
