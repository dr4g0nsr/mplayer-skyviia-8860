/*!
    @file
@verbatim
$Id: AVIData.h 58712 2009-03-02 02:44:07Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIDATA_H_
#define _AVIDATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "./AVI/AVI1Read/avirDefaults.h"
#include "./AVI/AVI1Read/AviIndexHandle.h"
#include "./AVI/AVI1Read/AVI1StreamLocationInfo.h"
#include "./AVI/AVICommon/L1StreamName.h"
#include "./AVI/AVICommon/AVIStructures.h"

typedef struct __AVIData__
{
    uint32_t numRiffs;

    uint64_t    *offsets;

    uint16_t riffIndex;

    uint16_t numAudioStreams;
    uint16_t numVideoStreams;
    uint16_t numSubtitleStreams;

    uint16_t numAbsoluteAudioStreams;
    uint16_t numAbsoluteVideoStreams;
    uint16_t numAbsoluteSubtitleStreams;

    uint64_t moviOffset;
    uint32_t moviSize;

    AVIHeader aviHeader;

    AVIStreamHeader      videoStreamHeader;
    AVIStreamFormatVideo videoFormatHeader;
    AVIStreamInfoDRM     videoDRMHeader;
    L1StreamName         videoSTRN;
    uint8_t              *videoConnectedDRMHeader;
    int32_t              videoConnectedDRMHeaderSize;

    AVIStreamHeader      audioStreamHeaders[MAX_AUD_TRACKS];
    AVIStreamFormatAudio audioFormatHeaders[MAX_AUD_TRACKS];
    L1StreamName         audioSTRN[MAX_AUD_TRACKS];

    AVIStreamHeader         subtitleStreamHeaders[MAX_SUB_TRACKS];
    AVIStreamFormatSubtitle subtitleFormatHeaders[MAX_SUB_TRACKS];
    L1StreamName            subtitleSTRN[MAX_SUB_TRACKS];

    DivXBool bHasIndex;

    AVI1StreamLocationInfo videoLoc;

    AVI1StreamLocationInfo audioLoc[MAX_AUD_TRACKS];

    AVI1StreamLocationInfo subtitleLoc[MAX_SUB_TRACKS];

    uint16_t nRiffExt; /* The index of the current AVIX */

    /* This is a generic index handle used to
       generate, store and retrieve the index offset
       information within a given title or menu */
    AVIIndexHandle aviIndexHandle;

    /* This is a flag indicating the presence of 'indx' in the file
       If 'indx' exists we are to use DF2 Enhanced Indexing */
    int32_t bIndx;
    
    /* This is a flag indicating the presence of an index in the file */
    int32_t bIndexPresent;

    uint32_t idx1Offset;

    /* The following allows for there to be multiple pData objects linked together */
    struct __AVIData__ *pNext;

} AVIData;

#ifdef __cplusplus
}
#endif

#endif /* _AVIDATA_H_ */
