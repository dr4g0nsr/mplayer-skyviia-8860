/*!
    @file
@verbatim
$Id: AVI1StreamLocationInfo.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1STREAMLOCATIONINFO_H_
#define _AVI1STREAMLOCATIONINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXTime.h"
#include "DivXError.h"

typedef struct _StreamLocationInfo_
{
    DivXTime time;
    uint32_t blockNum;
    uint32_t indexNum;
    /* uint64_t        blockOffset; */
    uint64_t absOffset;
    uint32_t blockSize;
    uint32_t blockDuration;
    uint64_t bytesBeforeBlock;

    uint32_t nextIndexNum;
    uint64_t nextBlockOffset;
    uint32_t nextBlockSize;
    DivXTime nextStartTime;

    DivXBool bStreamActive;
    uint32_t nAbsoluteIndexNum;

    DivXTime curTime;
    uint16_t nCurRiffExt;
} AVI1StreamLocationInfo;

#ifdef __cplusplus
}
#endif

#endif /* _AVI1STREAMLOCATIONINFO_H_ */
