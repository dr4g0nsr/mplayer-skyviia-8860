/*!
    @file
@verbatim
$Id: DF2IndexData.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2INDEXDATA_H_
#define _DF2INDEXDATA_H_

#include "./DF2/DF2Read/DF2IndexEntry.h"
#include "./DF2/DF2Common/DF2StdIndexChunk.h"
#include "./DF2/DF2Common/DF2SuperIndexChunk.h"
#include "./DF2/DF2Read/AVIXOffsetData.h"

#define MAX_SUPER_INDEX_ENTRIES 256

typedef struct __DF2SuperIndexEntries_t
{
    DF2SuperIndexEntry entry[MAX_SUPER_INDEX_ENTRIES];
} DF2SuperIndexEntries_t;

typedef struct __DF2IndexData_t
{
    AVI1InstanceData      *pInst;
    AVIData               *pData;
    AVIXOffsetData_t      *avixOffset;
    uint32_t               nIndex;
    uint32_t               absVidBlockNum;
    uint16_t               nCurIndex;
    //uint32_t               nKeyFrames;
    //uint32_t               nMaxKeyEntries;
    //uint32_t              *pKeyFrameList;
    //DivXIndexEntry_t       *pDivXIndexEntries;
    DivXBool               bIndx;
    uint32_t               indexType; /* Should be AVI_INDEX_OF_INDEXES or AVI_INDEX_OF_CHUNKS */
    uint32_t               vidIndxOffsets[1];
    uint32_t               audIndxOffsets[8];
    uint32_t               subIndxOffsets[8];
    DF2StdIndexChunk       vidIndx[1];
    DF2StdIndexChunk       audIndx[8];
    DF2StdIndexChunk       subIndx[8];
    int32_t                nVidSuperIndxEntries[1];
    DF2SuperIndexEntries_t vidSuperIndexEntries[1];
    int32_t                nAudSuperIndxEntries[8];
    DF2SuperIndexEntries_t audSuperIndexEntries[8];
    int32_t                nSubSuperIndxEntries[8];
    DF2SuperIndexEntries_t subSuperIndexEntries[8];
} DF2IndexData_t;

#endif /* _DF2INDEXDATA_H_ */
