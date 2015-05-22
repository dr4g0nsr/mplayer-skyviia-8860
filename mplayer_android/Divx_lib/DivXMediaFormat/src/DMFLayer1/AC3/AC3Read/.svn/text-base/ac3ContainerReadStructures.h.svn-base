/*!
    @file
@verbatim
$Id: ac3ContainerReadStructures.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AC3CONTAINERREADSTRUCTURES_H_
#define _AC3CONTAINERREADSTRUCTURES_H_

#include "DivXTypes.h"
#include "DivXFile.h"
#include "./AC3/AC3Common/ac3ContainerConstants.h"

#define UNKNOWN_BITRATE  0
#define CONSTANT_BITRATE 1
#define VARIABLE_BITRATE 2

/*
    This is the internal structure for the mp3 container.
 */
typedef struct Ac3Container_t
{
    DivXFile           hFile;
    int                curBlock;
    char               curFrameHeader[AC3_FRAME_HEADER_SIZE_BYTES];
    uint8_t            variableBitrateFlag;
    int32_t            firstFrameOffset;
    uint32_t           nFrames; /* this is required to give correct stream info */
    uint32_t           nBytes; /* this is required to give correct stream info */
    ac3_frame_header_t ac3FrameHeader;
    DivXMem            hMem;
    DivXBool           bStreamSetActive;
    DivXTime           curTime;
} Ac3Container_t;

#endif /* _AC3CONTAINERREADSTRUCTURES_H_ */
