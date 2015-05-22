/*!
    @file
@verbatim
$Id: ac3FrameHeader.h 56514 2008-10-15 20:32:30Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AC3FRAMEHEADER_H_
#define _AC3FRAMEHEADER_H_

#include "DivXTypes.h"

/**
    ac3 header

    This is from the ATSC A/52A standard.

 **/
typedef struct ac3_frame_header_t
{
    int   CRC1;
    int   fscod;
    int   framesizecod;
    int   samples_words;
    int   bitrate;
    int   sampling_frequency;
    int   bsi;
    int   bsmode;
    int   acmode;
    int   mode;
    int   lfeon;
    float N;
} ac3_frame_header_t;

#endif /* _AC3FRAMEHEADER_H_ */
