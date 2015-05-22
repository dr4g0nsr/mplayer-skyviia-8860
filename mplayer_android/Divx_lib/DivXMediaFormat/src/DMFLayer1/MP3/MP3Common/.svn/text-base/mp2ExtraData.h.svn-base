/*!
    @file
@verbatim
$Id: mp2ExtraData.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP2EXTRADATA_H_
#define _MP2EXTRADATA_H_

#include "DivXTypes.h"

#define MP2_EXTRA_DATA_SIZE 22

#define ACM_MPEG_ID_MPEG1           ( 0x0010 )

/* extra data required in 'strf' for MPEG Layer 2 audio */
typedef struct mp2_strf_extra_data_s
{
    uint16_t fwHeadLayer;
    uint32_t dwHeadBitrate;
    uint16_t fwHeadMode;
    uint16_t fwHeadModeExt;
    uint16_t wHeadEmphasis;
    uint16_t fwHeadFlags;
    uint32_t dwPTSLow;
    uint32_t dwPTSHigh;
} mp2_strf_extra_data_t;

#endif /* _MP2EXTRADATA_H_ */
