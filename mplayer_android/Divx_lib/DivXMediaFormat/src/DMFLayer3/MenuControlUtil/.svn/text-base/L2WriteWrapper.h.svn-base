/*!
    @file
@verbatim
$Id: L2WriteWrapper.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L2WRITEWRAPPER_H_
#define _L2WRITEWRAPPER_H_

#include "DivXInt.h"
#include "aviin.h"
#include "video_info.h"
#include "DMFBlockType.h"
#include "DMFContainerHandle.h"
#include "ActiveTracks.h"
#include "MappingLayerWritePush.h"

int32_t L2wwCreateOutputFile(DivXString *outputName, DMFContainerHandle *phContainer);
int32_t L2wwInitTitle(DMFContainerHandle hContainer);
int32_t L2wwAddVideoStream(DMFContainerHandle hContainer, video_info_t *vidInfo);
int32_t L2wwAddAudioStream(DMFContainerHandle hContainer, audio_info_t *vidInfo);
int32_t L2wwAddSubtitleStream(DMFContainerHandle hContainer, video_info_t *vidInfo);
int32_t L2wwGetRelativeFromTrackId( int32_t trackId, DMFBlockType_t blockType );
int32_t L2wwPushVideoBlock(DMFContainerHandle hContainer, uint8_t *buffer, int32_t size, int32_t flags);
int32_t L2wwPushAudioBlock(DMFContainerHandle hContainer, int32_t trackId, uint8_t *buffer, int32_t size);
int32_t L2wwPushSubtitleBlock(DMFContainerHandle hContainer, DivXBool bMenuMode, int32_t trackId, uint8_t *buffer, int32_t size);
int32_t L2wwWritePass(DMFContainerHandle hContainer);
int32_t L2wwClose(DMFContainerHandle hContainer);

#endif //_L2WRITEWRAPPER_H_

