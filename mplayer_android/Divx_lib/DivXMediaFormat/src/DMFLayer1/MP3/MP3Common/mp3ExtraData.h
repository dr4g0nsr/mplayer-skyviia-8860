/*!
    @file
@verbatim
$Id: mp3ExtraData.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3EXTRADATA_H_
#define _MP3EXTRADATA_H_

#include "DivXTypes.h"

#define MP3_EXTRA_DATA_SIZE 12

/* extra data required in 'strf' for MPEG Layer 3 audio */
typedef struct mp3_strf_extra_data_s
{
    uint16_t wID;
    uint32_t fdwFlags;
    int16_t  nBlockSize;
    int16_t  nFramesPerBlock;
    int16_t  nCodecDelay;
} mp3_strf_extra_data_t;

#endif /* _MP3EXTRADATA_H_ */
