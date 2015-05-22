/*!
    @file
@verbatim
$Id: mp3ContainerReadStructures.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3CONTAINERREADSTRUCTURES_H_
#define _MP3CONTAINERREADSTRUCTURES_H_

#include "DivXFile.h"
#include "DivXTime.h"
#include "DivXTypes.h"
#include "./MP3/MP3Common/mp3FrameHeader.h"
#include "./MP3/MP3Common/mp3ContainerConstants.h"

#define UNKNOWN_BITRATE  0
#define CONSTANT_BITRATE 1
#define VARIABLE_BITRATE 2

/*
    This is the internal structure for the mp3 container.
 */
typedef struct Mp3Container_t
{
    DivXFile           hFile;
    int                curBlock;
    unsigned char      curFrameHeader[MP3_FRAME_HEADER_SIZE_BYTES];
    unsigned char      curCRCCheck[MP3_CRC_CHECK_SIZE_BYTES];
    uint8_t            variableBitrateFlag;
    int32_t            firstFrameOffset;
    int32_t            nSamplesPerFrame;
    mp3_frame_header_t mp3FrameHeader;
    DivXBool           bStreamSetActive;
    DivXTime           curTime;
    double             avgFrameSize;
} Mp3Container_t;

#endif /* _MP3CONTAINERREADSTRUCTURES_H_ */
