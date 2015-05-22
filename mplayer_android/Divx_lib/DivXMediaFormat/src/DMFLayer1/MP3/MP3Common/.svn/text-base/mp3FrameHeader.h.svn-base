/*!
    @file
@verbatim
$Id: mp3FrameHeader.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3FRAMEHEADER_H_
#define _MP3FRAMEHEADER_H_

#include "DivXTypes.h"

/*

   MPEG Audio Layer I/II/III frame header
   found in the MPEG file.

   11 Bits Frame Sync
   2 Version ID
   2 Layer Index
   1 Protection Bit
   4 Bitrate Index
   2 Sampliing Rate Index
   1 Padding Bit
   1 Private Bit
   2 Channel Mode
   2 Mode Extension
   1 Copyright Bit
   1 Original Bit
   2 Emphasis

 */

typedef struct mp3_frame_header_t
{
    int32_t id_bit;
    int32_t layer;
    int32_t protection_bit;
    int32_t bitrate_bits;
    int32_t bitrate;
    int32_t sampling_frequency_bits;
    int32_t sampling_frequency;
    int32_t mode;
    int32_t mode_extension;
    int32_t padding_bit;
    int32_t private_bit;
    int32_t copyright;
    int32_t original_copy;
    int32_t emphasis;
    int32_t frameSize;
} mp3_frame_header_t;

#endif /* _MP3FRAMEHEADER_H_ */
