/*!
    @file
   @verbatim
   $Id: DMFSubtitleStreamInfo1.h 58719 2009-03-02 04:55:45Z jmurray $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFSUBTITLESTREAMINFO1_H_
#define _DMFSUBTITLESTREAMINFO1_H_

#include "DivXInt.h"

/*!
    The version 2 subtitle stream info header.  Is marked
    by the:

    DMFStreamHeaders_t streamTime = DMF_SUB_STREAM_INFO1;
 **/

#define DMF_SUB_STREAM_INFO1    0x21

/* Subtitle FOURCC codes */
#ifndef MAKE_FOURCC
#define MAKE_FOURCC(A, B, C, D) ( ((uint8_t) A) | (((uint8_t) B)<<8) | \
    (((uint8_t) C) <<16) | (((uint8_t) D)<<24) )
#endif

#ifndef FOURCC_UTF8
#define FOURCC_UTF8 MAKE_FOURCC('U','T','F','8')
#endif
#ifndef FOURCC_SSA
#define FOURCC_SSA  MAKE_FOURCC('S','S','A', 0 )
#endif
#ifndef FOURCC_ASS
#define FOURCC_ASS  MAKE_FOURCC('A','S','S', 0 )
#endif
#ifndef FOURCC_USF
#define FOURCC_USF  MAKE_FOURCC('U','S','F', 0 )
#endif
#ifndef FOURCC_BMP
#define FOURCC_BMP  MAKE_FOURCC('B','M','P', 0 )
#endif
#ifndef FOURCC_VSUB
#define FOURCC_VSUB MAKE_FOURCC('V','S','U','B')
#endif
#ifndef FOURCC_RGBA
#define FOURCC_RGBA MAKE_FOURCC('R','G','B','A')
#endif


typedef struct DMFSubtitleStreamInfo1_t
{
    uint8_t  type;             /*!< must be DMF_SUB_STREAM_INFO1 */
    uint32_t dwRate;
    uint32_t dwScale;
    int32_t  biWidth;
    int32_t  biHeight;
    int16_t  biBitCount;
    uint32_t biCodec;
    uint32_t biCompression;
    uint32_t biSizeImage;
}
DMFSubtitleStreamInfo1_t;

#endif /* _DMFSUBTITLESTREAMINFO1_H_ */
