/*!
    @file
@verbatim
$Id: avirIndex.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIRINDEX_H_
#define _AVIRINDEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXError.h"
#include "./AVI/AVI1Read/avir1Parse.h"
#include "./AVI/AVI1Read/avir1Internal.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./AVI/AVICommon/DivXIndexEntry.h"

/* This structure wraps the AVI1 index data needed by process index, which
   greatly simplifies the transition from non-abstrated to abstracted indexing. */
typedef struct __AVI1IndexData_t__
{
    AVI1InstanceData  *pInst;
    AVIData           *pData;
} AVI1IndexData_t;

/* Sets the index handle into the instance structure */
DivXError AVISetMenuIndexHandle( AVI1InstanceData *pInst,
                                 AVIIndexHandle    aviIndexHandle );

void AVISetTitleIndexHandle( AVI1InstanceData *pInst,
                                  AVIIndexHandle    aviIndexHandle );

/* Checks if the index handle has been set into the instance structure */
DivXBool AVIMenuIndexHandleSet( AVI1InstanceData *pInst );

DivXBool AVITitleIndexHandleSet( AVI1InstanceData *pInst );

/* These functions should be used in place of the legacy function ...ProcessIndex and ...GetIndexEntry */
DivXError       AviProcessIndex( AVIIndexHandle aviIndexHandle );

AVI1IndexEntry *AviGetIndexEntry( AVIIndexHandle aviIndexHandle,
                                  uint64_t       nIndexEntry );

DivXError       AviGetMoviOffset( AVIIndexHandle aviIndexHandle,
                                  uint64_t      *moviOffset,
                                  uint32_t      *moviSize );

DivXError       AviIndexSet( AVIIndexHandle aviIndexHandle,
                             uint64_t       index,
                             uint64_t       offset );

void       AviIndexDeInit( AVIIndexHandle aviIndexHandle );

DivXError       AviIndexGetNumEntries( AVIIndexHandle aviIndexHandle,
                                       uint32_t      *pNumEntrys );

DivXError AviIndexGetMoviOffset( AVIIndexHandle aviIndexHandle,
                                 uint16_t       nRiffExt,
                                 uint64_t      *moviOffset,
                                 uint32_t      *moviSize );

DivXError AviIndexGetChunkOffset( AVIIndexHandle aviIndexHandle,
                                  uint32_t       nCurIndexPos,
                                  DMFBlockType_t blockType,
                                  int32_t        nStreamOfType,
                                  uint64_t      *chunkOffset );

#ifdef __cplusplus
}
#endif
#endif /* _AVIRINDEX_H_ */
