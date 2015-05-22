/*!
    @file
   @verbatim
   $Id: DMFVideoStreamInfo1.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFVIDEOSTREAMINFO1_H_
#define _DMFVIDEOSTREAMINFO1_H_

#include "DivXInt.h"

/*!
    The version 1 video stream info header.  Is marked
    by the:

    DMFStreamHeaders_t streamTime = DMF_VID_STREAM_INFO1;
 */

#define DMF_VID_STREAM_INFO1 0x01

typedef struct DMFVideoStreamInfo1_t
{
    uint8_t  type;                    /*!< Must be DMF_VID_STREAM_INFO1 */
    uint32_t dwRate;
    uint32_t dwScale;
    int32_t  biWidth;
    int32_t  biHeight;
    int16_t  biBitCount;
    uint32_t biCodec;             /*!< The codec used for the compression. (for DMF is DIVX) */
    uint32_t biCompression;       /*!< The compression used. (for DMF 5,6 is DX50) */
    uint32_t biSizeImage;
    uint32_t dwInterlaceFlags;
    uint32_t dwPictAspectRatioX;
    uint32_t dwPictAspectRatioY;
}
DMFVideoStreamInfo1_t;

#endif /* _DMFVIDEOSTREAMINFO1_H_ */
