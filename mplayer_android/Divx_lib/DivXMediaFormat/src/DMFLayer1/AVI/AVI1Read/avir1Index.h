/*!
    @file
@verbatim
$Id: avir1Index.h 57969 2009-01-14 01:23:14Z fchan $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIR1INDEX_H_
#define _AVIR1INDEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DMFBlockType.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./AVI/AVI1Read/AviIndex.h"

/* typedef void * AVIIndexInstance; */

DivXError AVI1IndexInit( AVIIndexHandle   *aviIndexHandle,
                         AVI1InstanceData *pInst,
                         AVIData          *pData );

DivXError AVI1IndexGetNumEntries( AVIIndexHandle aviIndexHandle,
                                  uint32_t      *pNumEntries );

DivXError AVI1GetMoviOffset( AVIIndexHandle aviIndexHandle,
                             uint16_t       nRiffExt,
                             uint64_t      *moviOffset,
                             uint32_t      *moviSize );

int32_t AVI1GetNextDivXIndexEntryBlockNum( AVIIndexHandle aviIndexHandle,
                                           uint32_t       nVidBlockNum );

uint32_t AVI1GetPreviousDivXIndexEntryBlockNum( AVIIndexHandle aviIndexHandle,
                                                uint32_t       nVidBlockNum );

AVI1IndexEntry *AVI1GetIndexEntry( AVIData *pAviData,
                                   uint32_t nIndexEntry );

DivXError AVI1IndexProcess( AVIIndexHandle aviIndexHandle );

AVI1IndexEntry *AVI1IndexGet( AVIIndexHandle aviIndexHandle,
                              uint64_t       index );

DivXError AVI1IndexSet( AVIIndexHandle aviIndexHandle,
                        uint64_t       index,
                        uint64_t       offset );

DivXError AVI1IndexDeInit( AVIIndexHandle aviIndexHandle );

DivXBool AVI1IndexIsKeyFrame( AVIIndexHandle aviIndexHandle,
                              uint32_t       nBlock );

#ifdef __cplusplus
}
#endif

#endif /* _AVIR1INDEX_H_ */
