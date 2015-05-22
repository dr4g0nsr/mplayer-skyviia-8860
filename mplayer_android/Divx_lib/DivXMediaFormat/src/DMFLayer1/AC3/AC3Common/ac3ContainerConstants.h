/*!
    @file
@verbatim
$Id: ac3ContainerConstants.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AC3CONTAINERCONSTANTS_H_
#define _AC3CONTAINERCONSTANTS_H_

#include "DivXTypes.h"

#define AC3_FRAME_HEADER_SIZE_BYTES 7
#define MAX_AC3_FRAME_SIZE  4096

/* Marks the beginning of an ac3 frame */
#define AC3_SYNC_WORD 0xB77

#define AC3_SYNC1 0xB
#define AC3_SYNC2 0x7
#define AC3_SYNC3 0x7

/*
    Enough room for 5 second of audio at 320 kbps
    320 kbits/s * 5 second * 1024 * 1/8 + some padding
 */
#define AC3_MAX_INTERLEAVE_BUFFER_SIZE  255500

/*
    This is a magic number from the ATSC A/52A standard
 */
#define AC3_AUDIO_SAMPLES_PER_CHANNEL 1536

/**
    Define all the max values for the arrays.
 **/
#define MODE_MAP_MAX                7
#define AC3_BITRATES_MAX            37
#define AC3_WORDSPERSYNCFRAME_MAX   39
#define SAMPLING_MAP_AC3_MAX        3

/**
    ac3_bitrates[frmsizecod]
    This is from the ATSC A/52A standard
 **/
extern const int ac3_bitrates[38];

/**
    ac3_WordsperSyncframe[frmsizecod][fscod]

    This is from the ATSC A/52A standard.
 **/
extern const int ac3_WordsperSyncframe[38][3];

/** ac3_sampling_map[fscod] **/
extern const int ac3_sampling_map[4];

/** ac3 mode map **/
extern const int ac3_mode_map[8];

#endif /* _AC3CONTAINERCONSTANTS_H_ */
