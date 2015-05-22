/*!
    @file
@verbatim
$Id: avir1DivXIndex.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIR1DIVXINDEX_H_
#define _AVIR1DIVXINDEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXError.h"
#include "./AVI/AVI1Read/AviIndex.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVICommon/DivXIndexEntry.h"

DivXError divxIndex_AddDivXIndexEntry( AVIIndexHandle   hIndex,
                                       DivXIndexEntry_t *pDivXEntry );

DivXIndexEntry_t *divxIndex_GetEntryPtr( AVIIndexHandle aviIndexHandle,
                                         uint32_t       nVidBlock );

DivXError divxIndex_CalculateTrackPositions( AVI1InstanceData *pInst,
                                             AVIData          *pData,
                                             RDBufferHandle    handle,
                                             uint64_t          indexOffset );

DivXIndexEntry_t *divxIndex_SearchForEntry( AviIndex_t *pIndexInst,
                                            uint32_t    nVidBlock );

DivXIndexEntry_t *divxIndex_GetEntryFromVidBlock( AVIIndexHandle aviIndexHandle,
                                                  uint32_t       nVidBlockNum );

DivXIndexEntry_t *divxIndex_GetEntryFromTime( AVIIndexHandle aviIndexHandle,
                                              AVIData       *pData,
                                              DivXTime       origTime );

DivXIndexEntry_t *divxIndex_SearchForNextEntry( AviIndex_t *pIndexInst,
                                                uint32_t    nVidBlock );

DivXIndexEntry_t *divxIndex_SearchForPrevEntry( AviIndex_t *pIndexInst,
                                                uint32_t    nVidBlock );

DivXError divxIndex_ProcessIndex( AVI1InstanceData *pInst,
                                  AVIData          *pData );

DivXError divxIndex_ProcessIdx1Index( AVI1InstanceData *pInst,
                                     AVIData          *pData );

DivXError GetDivXIndexEntry( AVIIndexHandle    aviIndexHandle,
                             uint32_t          nVidBlockNum,
                             DivXIndexEntry_t *pDivXIndexEntry );

DivXError divxIndex_GetExactVideoChunkOffset( AVI1InstanceData *pInst,
                                              AVIData          *pData,
                                              uint32_t          nVidBlock,
                                              uint64_t         *pAudChunkOffset );

DivXError divxIndex_GetExactAudioChunkOffsetFromTime( AVI1InstanceData *pInst,
                                                      AVIData          *pData,
                                                      DivXTime          origTime,
                                                      int32_t           nStreamOfType,
                                                      uint64_t         *pAudChunkOffset );

DivXError GetDivXIndexEntryFromTime( AVIData          *pData,
                                     DivXTime          origTime,
                                     DivXIndexEntry_t *pDivXIndexEntry );

// Both of the functions below are needed when min mem usage is turned off
DivXError divxIndex_InitKeyframeList( AviIndex_t *pIndexData, uint32_t numEntries );

void divxIndex_AddToKeyFrameList(AviIndex_t *pIndexData, int32_t nVidBlock);

#ifdef __cplusplus
}
#endif

#endif /* _AVIR1DIVXINDEX_H_ */
