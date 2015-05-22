/*!
    @file
@verbatim
$Id: L1StreamName.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

/**
   L1CommonStreamTypes.h - Layer 1 types common to all streams
 **/

#ifndef _L1STREAM_NAME_H_
#define _L1STREAM_NAME_H_

#ifndef STRN_VIDEO_NAME
#define STRN_VIDEO_NAME     "Video"
#define STRN_AUDIO_NAME     "Audio"
#define STRN_BITMAP_CAPTION_NAME    "Subtitle"
#endif

/* / Originally LibAVI/include/common/headers.h:AVI_header_strn_t */
typedef struct _L1StreamName
{
    int16_t  streamNameSize;    /* the number of bytes in the streamName, this is not to be read or written */
    uint8_t *streamName;        /* null terminated string */
} L1StreamName;

#endif /* _L1STREAM_NAME_H_ */
