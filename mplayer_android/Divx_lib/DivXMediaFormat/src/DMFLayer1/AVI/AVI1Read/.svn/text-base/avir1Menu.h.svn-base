/*!
    @file
@verbatim
$Id: avir1Menu.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIR1MENU_H_
#define _AVIR1MENU_H_

#include "./AVI/AVI1Read/avir1Parse.h"

#include "DivXInt.h"
#include "DivXError.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError AVI1MenuMediaReadNextBlock( AVI1ContainerHandle handle,
                                      DMFBlockType_t     *pBlockType,
                                      uint32_t           *pnStream,
                                      DMFBlockNode_t     *pBlockNode,
                                      void               *userData );

DivXError AVI1SetCurrentMenuMedia( AVI1InstanceData *pInst,
                                   uint32_t          mediaIndex );

DivXError AVI1GetMenuChunkOffset( AVI1ContainerHandle handle,
                                  uint64_t           *offset );

DivXError AVI1GetNumMenuMedia( AVI1ContainerHandle handle,
                               uint16_t           *num );

DivXError AVI1GetNumMenuMediaVideoStreams( AVI1ContainerHandle handle,
                                           uint16_t            menuMediaIndex,
                                           uint16_t           *num );

DivXError AVI1GetNumMenuMediaAudioStreams( AVI1ContainerHandle handle,
                                           uint16_t            menuMediaIndex,
                                           uint16_t           *num );

DivXError AVI1GetNumMenuMediaSubtitleStreams( AVI1ContainerHandle handle,
                                              uint16_t            menuMediaIndex,
                                              uint16_t           *num );

DivXError AVI1MenuMediaHasDRM( AVI1ContainerHandle handle,
                               uint16_t            menuMediaIndex,
                               uint16_t           *hasDRM );

DivXError AVI1GetMenuMediaVideoInfo( AVI1ContainerHandle   handle,
                                     uint16_t              menuMediaIndex,
                                     uint16_t              streamIndex,
                                     AVIStreamHeader      *strh,
                                     AVIStreamFormatVideo *strf );

DivXError AVI1GetMenuMediaAudioInfo( AVI1ContainerHandle   handle,
                                     uint16_t              menuMediaIndex,
                                     uint16_t              streamIndex,
                                     AVIStreamHeader      *strh,
                                     AVIStreamFormatAudio *strf,
                                     L1StreamName         *strn );

DivXError AVI1GetMenuMediaSubtitleInfo( AVI1ContainerHandle      handle,
                                        uint16_t                 menuMediaIndex,
                                        uint16_t                 streamIndex,
                                        AVIStreamHeader         *strh,
                                        AVIStreamFormatSubtitle *strf,
                                        L1StreamName            *strn );

DivXError AVI1SetMenuMediaBlock( AVI1ContainerHandle handle,
                                 DMFBlockType_t      blockType,
                                 uint32_t            nStream,
                                 uint32_t           *pNumBlock,
                                 DivXTime           *pTime,
                                 uint32_t           *pBlockSize,
                                 DivXBool            bKeyFrameOnly );

DivXError AVI1GetCurrentMenuMediaIndex( AVI1ContainerHandle handle,
                                        uint32_t           *index );

DivXError AVI1SetMenuMedia( AVI1ContainerHandle handle,
                            uint32_t            mediaIndex );

A1MPInstanceHandle AVI1GetMenuParserInstanceHandle( AVI1ContainerHandle handle );

DivXError AVI1GetMenuChunk( AVI1ContainerHandle handle,
                            uint8_t            *pChunk,
                            uint32_t           *sizeChunk );

DivXError AVI1GetMenuDRM1Size( AVI1ContainerHandle handle,
                               uint32_t           *pDataSize );

DivXError AVI1GetMenuVideoDRM1Chunk( AVI1ContainerHandle handle,
                                     uint8_t            *pChunk,
                                     uint32_t            sizeBuffer,
                                     uint32_t           *pDataSize );

DivXError AVI1MenuMediaReadNextBlockByStream( AVI1ContainerHandle handle,
                                              DMFBlockType_t      blockType,
                                              uint32_t            nStream,
                                              DMFBlockNode_t     *pBlockNode,
                                              void               *userData );

DivXError AVI1MenuSetActiveStream( AVI1ContainerHandle handle,
                                   DMFBlockType_t      blockType,
                                   uint32_t            nStream,
                                   DivXBool            bActive );

#ifdef __cplusplus
}
#endif

#endif /* _AVIR1MENU_H_ */
