/*!
    @file
   @verbatim
   $Id: DMFAudioStreamInfo1.h 52605 2008-04-23 06:10:09Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFAUDIOSTREAMINFO1_H_
#define _DMFAUDIOSTREAMINFO1_H_

#include "DivXInt.h"

/*!
    The version 1 audio stream info header.  Is marked
    by the:

    DMFStreamHeaders_t streamTime = DMF_AUD_STREAM_INFO1;
 */

#define DMF_AUD_STREAM_INFO1 0x11
#define MAX_EXTRA_DATA_SIZE 22

/*! This structure defines the generic audio stream header
 */
typedef struct DMFAudioStreamInfo1_t
{
    uint8_t type;         /*!< must be DMF_AUD_STREAM_INFO1 */
    int16_t wFormatTag;
    int16_t nChannels;
    int32_t nSamplesPerSec;
    int32_t nAvgBytesPerSec;
    int16_t nBlockAlign;
    int16_t wBitsPerSample;
    int16_t delay;
    int16_t cbSize;
    uint8_t extra[MAX_EXTRA_DATA_SIZE];
}
DMFAudioStreamInfo1_t;

/*! laceing types */
typedef enum DMFLaceType1_e
{
    DMF_LACE_TYPE_NONE = 0,
    DMF_LACE_TYPE_FIXED,
    DMF_LACE_TYPE_XIPH,
    DMF_LACE_TYPE_EBML,
    DMF_LACE_TYPE_MIXED
}DMFLaceType1_e;

/*! lacing info per stream */
typedef struct DMFLaceInfo1_t
{
    DMFLaceType1_e type;
    uint8_t        framesPerBlock;
} DMFLaceInfo1_t;

#endif /* _DMFAUDIOSTREAMINFO1_H_ */
