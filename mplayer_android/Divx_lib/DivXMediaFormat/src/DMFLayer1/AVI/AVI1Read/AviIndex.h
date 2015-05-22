/*!
    @file
@verbatim
$Id: AviIndex.h 57969 2009-01-14 01:23:14Z fchan $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIINDEX_H_
#define _AVIINDEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DMFBlockType.h"
#include "./AVI/AVI1Read/AviIndexHandle.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVICommon/DivXIndexEntry.h"

typedef DivXError ( *AVIIndexProcess )( AVIIndexHandle aviIndexHandle );
typedef AVI1IndexEntry *( *AVIIndexGet )( AVIIndexHandle aviIndexHandle, uint64_t index );
typedef DivXError ( *AVIIndexGetMoviOffset )( AVIIndexHandle aviIndexHandle, uint16_t nRiffExt,
                                              uint64_t *moviOffset, uint32_t *moviSize );
typedef DivXError ( *AVIIndexNextMoviOffset )( AVIIndexHandle aviIndexHandle,
                                               uint64_t *moviOffset );
typedef DivXError ( *AVIIndexSet )( AVIIndexHandle aviIndexHandle, uint64_t index,
                                    uint64_t offset );
typedef DivXError ( *AVIIndexDeInit )( AVIIndexHandle aviIndexHandle );
typedef DivXBool ( *AVIIndexIsKeyFrame )( AVIIndexHandle aviIndexHandle, uint32_t nBlock );
typedef DivXError ( *AVIIndexGetNumEntrys )( AVIIndexHandle aviIndexHandle,
                                             uint32_t *pNumEntries );
typedef DivXError ( *AVIIndexGetChunkOffset )( AVIIndexHandle aviIndexHandle,
                                               uint32_t nCurIndexPos, DMFBlockType_t blockType,
                                               int32_t nStreamOfType, uint64_t *chunkOffset );

typedef DivXError ( *AddDivXIndexEntry )( AVIIndexHandle   pIndexInst,
                                          DivXIndexEntry_t *pDivXEntry );

/* The following structure provides a place holder for all needed index get/set functions
   and data */
typedef struct _AVIIndex_t_
{
    AVIIndexProcess        pfIndexProcess;
    AVIIndexGet            pfIndexGet;
    AVIIndexGetMoviOffset  pfIndexGetMoviOffset;
    AVIIndexNextMoviOffset pfIndexNextMoviOffset;
    AVIIndexGetChunkOffset pfIndexGetChunkOffset;
    AVIIndexSet            pfIndexSet;
    AVIIndexDeInit         pfIndexDeInit;
    AVIIndexGetNumEntrys   pfIndexGetNumEntrys;
    AVIIndexIsKeyFrame     pfIndexIsKeyFrame;
    AddDivXIndexEntry      pfAddDivXIndexEntry;
    /* DivXIndexEntry_t        *pDivXIndexEntries; */
    uint32_t numDivXIndexEntries;
    uint64_t indexOffset;
    void    *indexData;
    DivXMem  hMem;
    uint32_t maxEntries;

    // Only used in write with min mem usage turned off
    uint32_t           nKeyFrames;     // EC TODO init to 0
    uint32_t           nMaxKeyEntries; // EC TODO init to 0
    uint32_t          *pKeyFrameList;  // EC TODO init to NULL, this is the signal we are using min
    // max mem usage end

    DivXIndexEntry_t  *pDivXIndexEntries;
} AviIndex_t;

#ifdef __cplusplus
}
#endif

#endif /* _AVIINDEX_H_ */
