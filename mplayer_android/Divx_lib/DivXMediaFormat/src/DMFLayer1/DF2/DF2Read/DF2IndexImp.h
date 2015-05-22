/*!
    @file
@verbatim
$Id: DF2IndexImp.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2INDEXIMP_H_
#define _DF2INDEXIMP_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "./AVI/AVI1Read/AviIndex.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./DF2/DF2Read/DF2IndexEntry.h"
#include "./DF2/DF2Read/DF2IndexData.h"
#include "./AVI/AVI1Read/DivXIndexFifo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AVI1IndexCache_t
{
    DivXIndexFifo hDivXList;
} AVI1IndexCache_t;

int32_t df2Index_GetNumDivXIndexEntries( AviIndex_t *pIndexInst );

DivXIndexEntry_t *df2Index_GetDF2IndexEntry( AviIndex_t *pIndexInst,
                                            uint32_t    nEntry );

DivXError df2Index_AddDivXIndexEntry( AVIIndexHandle   hIndex,
                                      DivXIndexEntry_t *pDivXEntry );

DivXError df2Index_CalculateTrackPositions( AVI1InstanceData *pInst,
                                            AVIData          *pData,
                                            RDBufferHandle    handle,
                                            uint64_t          indexOffset );

DivXIndexEntry_t *df2Index_SearchForEntry( AviIndex_t *pIndexInst,
                                          uint32_t    nVidBlock,
                                          uint32_t    *pLowValue,
                                          uint32_t    *pHighValue);

DivXIndexEntry_t *df2Index_GetEntryFromVidBlock( AVIIndexHandle aviIndexHandle,
                                                uint32_t       nVidBlockNum,
                                                uint32_t      *pLowValue,
                                                uint32_t      *pHighValue);

DivXIndexEntry_t *df2Index_GetEntryFromTime( AVIIndexHandle aviIndexHandle,
                                            AVIData       *pData,
                                            DivXTime       origTime );

DivXIndexEntry_t *df2Index_SearchForNextEntry( AviIndex_t *pIndexInst,
                                              uint32_t    nVidBlock );

DivXIndexEntry_t *df2Index_SearchForPrevEntry( AviIndex_t *pIndexInst,
                                              uint32_t    nVidBlock );

DivXError df2Index_ProcessIdx1Index( AVI1InstanceData *pInst,
                                     AVIData          *pData,
                                     AVIXOffsetData_t *pAVIX );

DivXError df2Index_ProcessIndexOfChunks( AVI1InstanceData *pInst,
                                         AVIData          *pData,
                                         AVIXOffsetData_t *pAVIX );

DivXError df2Index_ProcessIndexOfIndexes( AVI1InstanceData *pInst,
                                          AVIData          *pData,
                                          AVIXOffsetData_t *pAVIX );

DivXError df2Index_GetExactVideoChunkOffset( AVI1InstanceData *pInst,
                                             AVIData          *pData,
                                             uint32_t          nVidBlock,
                                             uint64_t         *pAudChunkOffset );

DivXError df2Index_GetExactAudioChunkOffsetFromTime( AVI1InstanceData *pInst,
                                                     AVIData          *pData,
                                                     DivXTime          origTime,
                                                     int32_t           nStreamOfType,
                                                     uint64_t         *pAudChunkOffset );

DivXError df2Index_GetDivXIndexEntryFromTime( AVIData         *pData,
                                              DivXTime         origTime,
                                              DivXIndexEntry_t *pDivXIndexEntry );

/* Reads up all the Super-Indexes for a RIFF*/
DivXError df2Index_ReadSuperIndexes( AVI1InstanceData *pInst,
                                     AVIData          *pData,
                                     DF2IndexData_t   *pIndexData );

/* Reads up all the Super-Indexes for a stream*/
DivXError df2Index_ReadSuperIndexesForStream( AVI1InstanceData       *pInst,
                                              int32_t                 nStreams,
                                              int32_t                *pCount,
                                              int32_t                *indxLocations,
                                              DF2SuperIndexEntries_t *pSuperIndexEnries );

/* Reads up all the Super-Indexes entries for a specific Super-Index*/
DivXError df2Index_ReadSuperIndexEntries( AVI1InstanceData   *pInst,
                                          int32_t             nEntries,
                                          uint64_t            location,
                                          DF2SuperIndexEntry *pSuperIndexEntry,
                                          int32_t            *pSuperIndexEntries );

DivXError df2Index_SetDivXIndexAudFramePosition( AVI1InstanceData *pInst,
                                                 AVIData          *pData,
                                                 int32_t           nStream,
                                                 uint64_t          stdIndexOffset );

DivXError df2Index_GetDivXIndexEntry( AVIIndexHandle   aviIndexHandle,
                                      uint32_t         nVidBlockNum,
                                      DivXIndexEntry_t *pDivXIndexEntry );

DivXError df2Index_SetDivXIndexSubFramePosition( AVI1InstanceData *pInst,
                                                 AVIData          *pData,
                                                 int32_t           nStream);

DivXError df2Index_ReadBaseStandardIndex( AVI1InstanceData *pInst,
                                          uint64_t          ixOffset,
                                          uint32_t         *ixEntries,
                                          uint32_t         *ixEntriesSize,
                                          uint64_t         *ixEntriesOffset,
                                          uint64_t         *ixBaseOffset );

DivXError AviIndexGetRiffIndex( AviIndex_t *pIndexInst,
                                uint64_t    offset,
                                uint16_t   *pnRiff );

int32_t df2GetFrameNumberFromTime( AVIData* pData, DivXTime time );

#ifdef __cplusplus
}
#endif
#endif /* _DF2INDEXIMP_H_ */
