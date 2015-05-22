/*!
    @file
@verbatim
$Id: AVIXOffsetData.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIXOFFSETDATA_H_
#define _AVIXOFFSETDATA_H_

typedef struct _AVIXOffsetData_t
{
    uint64_t avixOffset;
    uint64_t moviOffset;
    uint32_t moviSize;
    uint64_t idx1Offset;
    uint64_t numIdx1Entries;
    uint64_t avixIdx;
} AVIXOffsetData_t;

#endif /* _AVIXOFFSETDATA_H_ */
