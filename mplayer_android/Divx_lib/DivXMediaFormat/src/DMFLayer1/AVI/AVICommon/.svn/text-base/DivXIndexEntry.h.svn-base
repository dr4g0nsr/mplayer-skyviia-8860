/*!
    @file
@verbatim
$Id: DivXIndexEntry.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DIVXINDEXENTRY_H_
#define _DIVXINDEXENTRY_H_

#include "DivXInt.h"
#include "DivXTime.h"

typedef struct _DivXIndexEntry_t
{
    uint64_t vidChunkOffsetIntoMovi;
    uint32_t nVidBlock;
    int32_t  audTimeDiffFromVid[8];
    uint64_t audChunkOffsetIntoMovi[8];
    uint64_t subChunkOffsetIntoMovi[8];
    uint32_t nAVIX;
    uint32_t prevOffsetIntoMovi;
    DivXTime prevAbsoluteTime;
    DivXBool previouslyPoppedCache;
} DivXIndexEntry_t;

#endif /* _DIVXINDEXENTRY_H_ */
