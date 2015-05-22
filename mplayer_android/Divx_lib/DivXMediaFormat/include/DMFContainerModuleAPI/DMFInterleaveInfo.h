/*!
   @file DMFInterleaveInfo.h
   @verbatim
   $Id: DMFInterleaveInfo.h 58500 2009-02-18 19:45:46Z jbraness $

   Copyright (c) 2006-2008 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/


#ifndef _DMFINTERLEAVE_INFO_H_
#define _DMFINTERLEAVE_INFO_H_

#include "DivXTime.h"

typedef enum DMFStreamPriority
{
    DMFSTREAM_PRIORITY_LOWEST,   /* absolute lowest priority */
    DMFSTREAM_PRIORITY_LEVEL_4,  /* lower priority then level 3 */
    DMFSTREAM_PRIORITY_LEVEL_3,  /* lower priority then level 2 */
    DMFSTREAM_PRIORITY_LEVEL_2,  /* lower priority then level 1 */
    DMFSTREAM_PRIORITY_LEVEL_1,  /* lower priority then level highest */
    DMFSTREAM_PRIORITY_HIGHEST,  /* absolute highest priority */
    DMFSTREAM_NUM_PRIORITIES
} DMFStreamPriority_t;

typedef struct _DMFInterleaveInfo_t
{
    DivXTime preload;                  /* preload in divx time units */
    DivXBool bAudioAlwaysFixedBlocks;  /* If the stream should be re-blocked by the interleaver */
    DMFStreamPriority_t priority;      /* Priority of the stream, higher pri stream will be written first */
} DMFInterleaveInfo_t;

#endif /* _DMFINTERLEAVE_INFO_H_ */
