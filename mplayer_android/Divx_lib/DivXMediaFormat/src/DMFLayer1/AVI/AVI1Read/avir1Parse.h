/*!
    @file
@verbatim
$Id: avir1Parse.h 59517 2009-04-08 22:04:49Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1PARSE_H_
#define _AVI1PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DMFErrors.h"
#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "DivXString.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#include "./AVI/AVI1Read/AVI1ContainerHandle.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"

#include "./AVI/AVI1Read/avirIndex.h"
#include "./AVI/AVICommon/L1StreamName.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVI1Read/ReadBuffer.h"

#include "./AVI/AVI1ReadMenu/AVI1MenuParse.h"

#define AVI_INDEX_FLAG_KEYFRAME 0x00000010L
#define DEFAULT_KEYFRAME_GAP  0
#define DEFAULT_MAX_KEYFRAMES 5000

int AVI1ReadCallBack( void     *handleRead,
                      uint64_t  offset,
                      uint8_t  *buffer,
                      uint32_t  length,
                      uint32_t *bytesRead,
                      void     *userData,
                      uint32_t  hint );

/* AVI1R_InsufficientMem
   AVI1R_InvalidFile */
DivXError AVI1OpenContainer( const DivXString    *filename,
                             AVI1ContainerHandle *handle,
                             DivXMem              hMem );

DivXError CompleteOpenContainer( const DivXString    *filename,
                                 AVI1ContainerHandle *handle,
                                 void                *userData,
                                 DivXMem              hMem,
                                 AVI1InstanceData    *pInst,
                                 DivXFile             fileHandle );

DivXBool IsRemoteFile( const DivXString *filename );

DivXError AVI1CloseContainer( AVI1ContainerHandle handle );

/*
   AVI1R_ERR_SUCCESS
   AVI1R_ERR_INDEX_OUT_OF_RANGE
   AVI1R_ERR_END_OF_STREAM
 */
DivXError AVI1SetTitleBlock( AVI1ContainerHandle handle,
                             DMFBlockType_t      blockType,
                             uint32_t            nStream,
                             uint32_t           *pNumBlock,
                             DivXTime           *pTime,
                             uint32_t           *pBlockSize,
                             DivXBool            bKeyFrameOnly );

void AVI1GetCurrentTitleIndex( AVI1ContainerHandle handle,
                                    uint32_t           *index );

DivXError AVI1GetCurrentTitleOffset( AVI1ContainerHandle handle,
                                     uint64_t           *offset );

DivXError AVI1SetTitle( AVI1ContainerHandle handle,
                        uint32_t            titleIndex );

DivXError AVI1GetMetadataBitstream( AVI1ContainerHandle handle,
                                    uint8_t            *pGraph,
                                    uint32_t           *sizeGraph,
                                    uint8_t            *pNode,
                                    uint32_t           *sizeNode );

DivXError AVI1GetTitleDRM1Size( AVI1ContainerHandle handle,
                                uint32_t           *pDataSize );

DivXError AVI1GetTitleVideoDRM1Chunk( AVI1ContainerHandle handle,
                                      uint8_t            *pChunk,
                                      uint32_t            sizeBuffer,
                                      uint32_t           *pDataSize );

uint64_t AVI1GetMetadataRiffOffset( AVI1ContainerHandle handle );

DivXError AVI1GetPreviousKeyFrame( AVI1ContainerHandle handle,
                                   DMFBlockType_t      blockType,
                                   int32_t             nBlockOfType,
                                   int32_t            *pFoundBlockNum,
                                   DivXTime           *pTime );

DivXError AVI1GetNextKeyFrame( AVI1ContainerHandle handle,
                               DMFBlockType_t      blockType,
                               int32_t             nBlockOfType,
                               int32_t            *pFoundBlockNum,
                               DivXTime           *pTime);

DivXError AVI1GetPreviousSyncPoint( AVI1ContainerHandle handle,
                                    DMFBlockType_t      blockType,
                                    DivXTime            curTime,
                                    DivXTime           *pTime );

DivXError AVI1GetNextSyncPoint( AVI1ContainerHandle handle,
                                DMFBlockType_t      blockType,
                                DivXTime            curTime,
                                DivXTime           *pTime );

DivXError AVI1GetNearestKeyFrame( AVI1ContainerHandle handle,
                                  DMFBlockType_t      blockType,
                                  int32_t             nBlockOfType,
                                  int32_t            *pFoundBlockNum );

DivXError AVI1TitleReadNextBlockByStream( AVI1ContainerHandle handle,
                                          DMFBlockType_t      blockType,
                                          uint32_t            nStream,
                                          DMFBlockNode_t     *pBlockNode,
                                          void               *userData );

DivXError AVI1TitleSetActiveStream( AVI1ContainerHandle handle,
                                    DMFBlockType_t      blockType,
                                    uint32_t            nStream,
                                    DivXBool            bActive );

DivXError AVI1TitleReadNextBlock( AVI1ContainerHandle handle,
                                  DMFBlockType_t     *pBlockType,
                                  uint32_t           *pnStream,
                                  DMFBlockNode_t     *pBlockNode,
                                  void               *userData );

DivXError AVI1UpdateStreamPosition( AVI1InstanceData *pInst,
                                    AVIData          *pAVIData );

#ifdef __cplusplus
}
#endif
#endif /* _AVI1PARSE_H_ */
