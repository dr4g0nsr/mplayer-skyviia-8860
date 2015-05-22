/*!
    @file
@verbatim
$Id: L1AVIStructureConversions.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1AVISTRUCTURECONVERSIONS_H_
#define _L1AVISTRUCTURECONVERSIONS_H_


#include "AVIStructures.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"

#ifdef __cplusplus
extern "C" {
#endif

void GenericVideoFormatToStrf( DMFVideoStreamInfo1_t *pVidFormat,
                               AVIStreamFormatVideo  *strf );

void GenericVideoFormatToStrh( DMFVideoStreamInfo1_t *pVidFormat,
                               AVIStreamHeader       *strh );

void StrfToGenericVideoFormat( AVIStreamHeader       *strh,
                               AVIStreamFormatVideo  *strf,
                               DMFVideoStreamInfo1_t *pVidFormat );

void GenericAudioFormatToStrf( DMFAudioStreamInfo1_t *pAudFormat,
                               AVIStreamFormatAudio  *strf );

void GenericAudioFormatToStrh( DMFAudioStreamInfo1_t *pAudFormat,
                               AVIStreamHeader       *strh );

void StrfToGenericAudioFormat( AVIStreamHeader       *strh,
                               AVIStreamFormatAudio  *strf,
                               DMFAudioStreamInfo1_t *pAudFormat );

void GenericSubtitleFormatToStrf( DMFSubtitleStreamInfo1_t *pSubFormat,
                                  AVIStreamFormatSubtitle  *strf );

void GenericSubtitleFormatToStrh( DMFSubtitleStreamInfo1_t *pSubFormat,
                                  AVIStreamHeader          *strh );

void StrfToGenericSubtitleFormat( AVIStreamHeader          *strh,
                                  AVIStreamFormatSubtitle  *strf,
                                  DMFSubtitleStreamInfo1_t *pSubFormat );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _L1AVISTRUCTURECONVERSIONS_H_ */
