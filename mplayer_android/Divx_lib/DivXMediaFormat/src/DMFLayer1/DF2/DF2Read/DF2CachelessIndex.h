/*!
    @file
@verbatim
$Id: DF2CachelessIndex.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2CACHELESSINDEX_H_
#define _DF2CACHELESSINDEX_H_

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

DivXError df2Index_UseCacheless(AVI1InstanceData *pInst);

DivXError df2Index_ProcessIndexOfIndexesCacheless( AVI1InstanceData *pInst,
                                                   AVIData          *pData );

DivXError df2Index_FindVideoChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, uint64_t *pVidChunkOffset, int32_t *pnVidBlock );

DivXError df2Index_FindAudioChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, int32_t nStreamOfType, uint64_t *pChunkOffset);

DivXError df2Index_FindSubtitleChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, int32_t nStreamOfType, uint64_t *pSubChunkOffset );

DivXError df2Index_FindVideoChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, uint64_t *pVidChunkOffset, int32_t *pnVidBlock );

DivXError df2Index_FindPreviousVideoChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, uint64_t *pVidChunkOffset, int32_t *pnVidBlock );

DivXError df2Index_FindNextVideoChunkOffset( AVI1InstanceData *pInst, AVIData *pData, DivXTime *pTime, uint64_t *pVidChunkOffset, int32_t *pnVidBlock );

#ifdef __cplusplus
}
#endif
#endif /* _DF2CACHELESSINDEX_H_ */
