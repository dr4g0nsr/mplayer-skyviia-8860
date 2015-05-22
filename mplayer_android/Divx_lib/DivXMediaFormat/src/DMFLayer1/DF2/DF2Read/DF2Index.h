/*!
    @file
@verbatim
$Id: DF2Index.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2INDEX_H_
#define _DF2INDEX_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DMFBlockType.h"
#include "./AVI/AVICommon/AVIStructures.h"

#include "./AVI/AVI1Read/avir1Internal.h"
#include "./AVI/AVI1Read/AviIndexHandle.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./DF2/DF2Read/AVIXOffsetData.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError DF2IndexInit( AVIIndexHandle   *aviIndexHandle,
                        AVI1InstanceData *pInst,
                        AVIData          *pData,
                        AVIXOffsetData_t *avixOffset,
                        int32_t           nAVIX,
                        int32_t           minMemUsage,
                        DivXMem           hMem );

DivXError DF2IndexProcess( AVIIndexHandle aviIndexHandle );

AVI1IndexEntry *DF2IndexGet( AVIIndexHandle aviIndexHandle,
                             uint64_t       index );

DivXError DF2GetMoviOffset( AVIIndexHandle aviIndexHandle,
                            uint16_t       nRiffExt,
                            uint64_t      *moviOffset,
                            uint32_t      *moviSize );

DivXError DF2GetChunkOffset( AVIIndexHandle aviIndexHandle,
                             uint32_t       nCurIndexPos,
                             DMFBlockType_t blockType,
                             int32_t        nStreamOfType,
                             uint64_t      *chunkOffset );

DivXError DF2NextMoviOffset( AVIIndexHandle aviIndexHandle,
                             uint64_t      *moviOffset );

DivXError DF2IndexSet( AVIIndexHandle aviIndexHandle,
                       uint64_t       index,
                       uint64_t       offset );

DivXError DF2IndexDeInit( AVIIndexHandle aviIndexHandle );

DivXBool DF2IndexIsKeyFrame( AVIIndexHandle aviIndexHandle,
                             uint32_t       nBlock );

/* GLS - The following apears to be a duplicate of part of the AVIIndex.h, Jason Braness had added this code on 11jun2007 , deleted 19jun2007 GLS
 #include "DivXInt.h"
 #include "DivXMem.h"
 #include "DivXError.h"
 #include "./AVI/AVI1Read/AviIndexHandle.h"
 #include "./AVI/AVICommon/AVIStructures.h"
 #include "./AVI/AVICommon/DivXIndexEntry.h"

   typedef DivXError       (*AVIIndexProcess)(AVIIndexHandle aviIndexHandle);
   typedef AVI1IndexEntry* (*AVIIndexGet)(AVIIndexHandle aviIndexHandle, uint64_t index);
   typedef DivXError       (*AVIIndexGetMoviOffset)(AVIIndexHandle aviIndexHandle, uint16_t nRiffExt, uint64_t* moviOffset);
   typedef DivXError       (*AVIIndexNextMoviOffset)(AVIIndexHandle aviIndexHandle, uint64_t* moviOffset);
   typedef DivXError       (*AVIIndexSet)(AVIIndexHandle aviIndexHandle, uint64_t index, uint64_t offset);
   typedef DivXError       (*AVIIndexDeInit)(AVIIndexHandle aviIndexHandle);
   typedef DivXBool        (*AVIIndexIsKeyFrame)(AVIIndexHandle aviIndexHandle, uint32_t nBlock);
   typedef DivXError       (*AVIIndexGetNumEntrys)(AVIIndexHandle aviIndexHandle, uint32_t *pNumEntries);

   // The following structure provides a place holder for all needed index get/set functions
   // and data
   typedef struct _DF2EnhancedIndex_t_
   {
    AVIIndexProcess         pfIndexProcess;
    AVIIndexGet             pfIndexGet;
    AVIIndexGetMoviOffset   pfIndexGetMoviOffset;
    AVIIndexNextMoviOffset  pfIndexNextMoviOffset;
    AVIIndexSet             pfIndexSet;
    AVIIndexDeInit          pfIndexDeInit;
    AVIIndexGetNumEntrys    pfIndexGetNumEntrys;
    AVIIndexIsKeyFrame      pfIndexIsKeyFrame;
    //DivXIndexEntry_t        *pDivXIndexEntries;
    uint32_t                numDivXIndexEntries;
    uint64_t                indexOffset;
    void*                   indexData;
    DivXMem                 hMem;
    int32_t                 maxEntries;
   } _DF2EnhancedIndex_t;
 */
#ifdef __cplusplus
}
#endif
#endif /* _DF2INDEX_H_ */
