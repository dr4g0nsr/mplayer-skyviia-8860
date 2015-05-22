/*!
    @file
@verbatim
$Id: avir1Query.h 59996 2009-05-06 19:54:33Z ashivadas $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIR1QUERY_H_
#define _AVIR1QUERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXError.h"
#include "AVI1ContainerHandle.h"
#include "./AVI/AVI1Read/AVIData.h"
#include "./AVI/AVICommon/L1StreamName.h"
#include "./AVI/AVICommon/AVIStructures.h"

/*!
    Gets the number of titles in the file.
 */
DivXError AVI1GetNumTitles( AVI1ContainerHandle handle,
                            uint32_t           *num );

/*!
    Gets the number of video streams loaded into the api, or the number of streams,
    available for parsing.
 */
DivXError AVI1GetNumVideoStreams( AVI1ContainerHandle handle,
                                  uint16_t            titleIndex,
                                  uint16_t           *num );

/*!
    Gets the number of audio streams loaded into the api, or the number of streams,
    available for parsing.
 */
DivXError AVI1GetNumAudioStreams( AVI1ContainerHandle handle,
                                  uint16_t            titleIndex,
                                  uint16_t           *num );

/*!
    Gets the number of subtitle streams loaded into the api, or the number of streams,
    available for parsing.
 */
DivXError AVI1GetNumSubtitleStreams( AVI1ContainerHandle handle,
                                     uint16_t            titleIndex,
                                     uint16_t           *num );

/*!
    Gets the total number of video streams in the file including the streams that are
    past the max number of streams allowed.
 */
DivXError AVI1GetNumVideoStreamsAbsolute( AVI1ContainerHandle handle,
                                          uint16_t            titleIndex,
                                          uint16_t           *num );

/*!
    Gets the total number of audio streams in the file including the streams that are
    past the max number of streams allowed.
 */
DivXError AVI1GetNumAudioStreamsAbsolute( AVI1ContainerHandle handle,
                                          uint16_t            titleIndex,
                                          uint16_t           *num );

/*!
    Gets the total number of subtitle streams in the file including the streams that are
    past the max number of streams allowed.
 */
DivXError AVI1GetNumSubtitleStreamsAbsolute( AVI1ContainerHandle handle,
                                             uint16_t            titleIndex,
                                             uint16_t           *num );

/*!
    Gets the streams header information.
 */
DivXError AVI1GetVideoInfo( AVI1ContainerHandle   handle,
                            uint16_t              titleIndex,
                            uint16_t              streamIndex,
                            AVIStreamHeader      *strh,
                            AVIStreamFormatVideo *strf,
                            L1StreamName         *strn );

/*!
    Gets the streams header information.
 */
DivXError AVI1GetAudioInfo( AVI1ContainerHandle   handle,
                            uint16_t              titleIndex,
                            uint16_t              streamIndex,
                            AVIStreamHeader      *strh,
                            AVIStreamFormatAudio *strf,
                            L1StreamName         *strn );

/*!
    Gets the streams header information.
 */
DivXError AVI1GetSubtitleInfo( AVI1ContainerHandle      handle,
                               uint16_t                 titleIndex,
                               uint16_t                 streamIndex,
                               AVIStreamHeader         *strh,
                               AVIStreamFormatSubtitle *strf,
                               L1StreamName            *strn );

int32_t AVI1GetLargestSuggestedBufferSize( AVIData *pData );
/*!
    Retreives the current title number.
 */
void AVI1GetCurrentTitleIndex( AVI1ContainerHandle handle,
                                    uint32_t           *index );

/*!
    Querys file for DRM 1 chunk, ie the strd chunk.
 */
DivXError AVI1HasDRM1( AVI1ContainerHandle handle,
                       uint16_t            titleIndex,
                       uint16_t           *hasDRM );

/*!
    Retreives the size of the DRM chunk.
 */
DivXError AVI1GetTitleDRM1Size( AVI1ContainerHandle handle,
                                uint32_t           *pDataSize );

/*!
    Retreives the version of the DRM chunk.
 */
DivXError AVI1GetTitleDRM1Version( AVI1ContainerHandle pInst,
                                   uint32_t           *pDataVersion );

/*!
    Retreives the DRM 1 chunk.
 */
DivXError AVI1GetTitleVideoDRM1Chunk( AVI1ContainerHandle handle,
                                      uint8_t            *pChunk,
                                      uint32_t            sizeBuffer,
                                      uint32_t           *pDataSize );

/*!
    Sets the DRM 1 chunk.
 */
DivXError AVI1SetTitleVideoDRM1Chunk( AVI1ContainerHandle handle,
                                      uint8_t            *pChunk,
                                      uint32_t            sizeData );

/*!
    Retreives the metadata riff offset.
 */
uint64_t AVI1GetMetadataRiffOffset( AVI1ContainerHandle handle );

#ifdef __cplusplus
}
#endif

#endif /* _AVIR1QUERY_H_ */
