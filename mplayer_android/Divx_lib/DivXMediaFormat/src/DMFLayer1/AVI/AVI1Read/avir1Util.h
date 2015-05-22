/*!
    @file
@verbatim
$Id: avir1Util.h 56436 2008-10-11 01:07:01Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIR1UTIL_H_
#define _AVIR1UTIL_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXTime.h"
#include "DMFBlockType.h"
#include "L1ContainerHandle.h"
#include "./AVI/AVI1Read/AVIData.h"
#include "./AVI/AVI1Read/avir1Internal.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError ConvertSubTextTimeToDivXStreamTime( uint8_t  *cSubBuffer,
                                              DivXTime *startTime,
                                              DivXTime *endTime );

DivXError AVI1IsAudVBR( AVIData  *pData,
                        int32_t   nStream,
                        DivXBool *bIsVBR );

DMFBlockType_t AVI1GetBlockTypeFromChunkId( char *chunkId );

DivXError AVI1GetChunkIdDetail( char           *chunkId,
                                int32_t        *pIndex,
                                DMFBlockType_t *pBlockType );

DivXBool AVI1DoesStreamExist( AVIData       *pAviData,
                              DMFBlockType_t blockType,
                              int32_t        nStreamOfType );

DivXBool BlockTypesMatch( DMFBlockType_t blockType1,
                          DMFBlockType_t blockType2 );

int AVI1GetStreamNumOfTypeFromTrackId( AVIData       *pAviData,
                                       int            nAbsoluteIndex,
                                       DMFBlockType_t blockType );

DivXError parseStreamInfoString( const char *infoString,
                                 int16_t     infoStringSize,
                                 DivXString *locale,
                                 DivXString *countryCode,
                                 DivXString *mediaDesc,
                                 DivXBool    bVideo );

DivXBool IsVariableBitrateAudio( AVIData *pData,
                                 int32_t  nStream );

int32_t AVI1GetTrackIdFromStreamNum(AVIData       *pAviData, 
                                    int32_t        nStreamOfType, 
                                    DMFBlockType_t blockType);

DivXTime GetTimeFromVideoFrame(AVIData *pData, int32_t nFrame);

uint32_t GetVideoFrameFromTime(AVIData *pData, DivXTime dtTime);

uint32_t GetAudioFrameFromTime(AVIData *pData, DivXTime dtTime, int32_t nAudStreamOfType);

uint32_t GetAudioBytesFromTime(AVIData *pData, DivXTime dtTime, int32_t nAudStreamOfType);

DivXTime GetTimeFromAudioBytes(AVIData *pData, uint32_t nBytes, int32_t nAudStreamOfType);

DivXTime GetVideoFramePeriod(AVIData *pData);

DivXTime GetTimeFromAudioFrame(AVIData *pData, int32_t nFrame, int32_t nAudStreamOfType);

uint32_t GetAudioScale(AVIData *pData, int32_t nAudStreamOfType);

#ifdef __cplusplus
}
#endif

#endif /* _AVIR1UTIL_H_ */
