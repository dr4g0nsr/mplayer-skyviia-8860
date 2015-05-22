/*!
    @file
@verbatim
$Id: mp3ContainerConstants.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3CONTAINERCONSTANTS_H_
#define _MP3CONTAINERCONSTANTS_H_

#include "DivXTypes.h"

#define MP3_SYNC1 0xF
#define MP3_SYNC2 0xF
#define MP3_SYNC3 0x7

#define MP3_SYNC_WORD_SIZE            11
#define MP3_FRAME_HEADER_SIZE_BYTES   4
#define MP3_CRC_CHECK_SIZE_BYTES      2
#define MAX_MP3_FRAME_SIZE            4096

/*
    mp3_layer_table[Layer Index]
 */
extern const int mp3_layer_table[];

/*
    mp3_version_map[Version ID]
 */
extern const int mp3_version_table[];

/*
    mp3_version_strings[Version ID]
 */
extern const char mp3_verion_string_names[4][10];

/*
    sampling_frequency_map[id_bit][sampling_frequency_bits]
 */
extern const int32_t mp3_sampling_frequency_table[4][4];

/*
    bitrate_map[version_bits][layer][bitrate_bits]
 */
extern const int32_t mp3_bitrate_table[4][3][16];

/*
    mp3_channel_mode_strings[Channel Mode]
 */
extern const char mp3_channel_mode_strings[4][16];

extern const int32_t mp3_coefficients[2][3];

extern const int32_t mp3_slot_sizes[3];

extern const int32_t m_dwSamplesPerFrames[2][3];

#endif /* _MP3CONTAINERCONSTANTS_H_ */
