/*!
    @file
@verbatim
$Id: avir1ParseInternal.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1PARSEINTERNAL_H_
#define _AVI1PARSEINTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#include "./AVI/AVICommon/L1StreamName.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVICommon/AVIChunkPosInfo.h"
#include "./AVI/AVI1Read/AVI1StreamLocationInfo.h"
#include "./AVI/AVI1Read/avirIndex.h"
#include "./AVI/AVI1Read/ReadBuffer.h"

#include "./AVI/AVI1ReadMenu/AVI1MenuParse.h"
#include "./AVI/AVI1Read/avir1Parse.h"
#include "./AVI/AVI1Read/avir1Internal.h"

DivXError AVI1OpenFunction( const DivXString *sFilename,
                            int               iOpenFlags,
                            DivXFile         *pHandle,
                            DivXMem           hMem );

DivXError AVI1CloseFunction( void *handleRead );

DivXError AVI1OpenContainerEx( const DivXString    *filename,
                               AVI1ContainerHandle *handle,
                               void                *userData,
                               DivXMem              hMem );

DivXError AVI1CreateInstance( AVI1InstanceData **pInst,
                              DivXMem            hMem );

void AVI1InitStreamLocation( AVI1StreamLocationInfo *pInfo );

void AVI1ResetStreamLocation( AVI1StreamLocationInfo *pInfo );

DivXError ScanRiff( AVI1InstanceData *pInst,
                    AVIData          *pData,
                    uint32_t          riffIndex,
                    RDBufferHandle    handleBuffer,
                    void             *userData );

DivXError AVI1ReadNextBlockCommon( AVI1ContainerHandle handle,
                                   AVIData            *pData,
                                   DMFBlockType_t     *pBlockType,
                                   uint32_t           *pnStream,
                                   DMFBlockNode_t     *pBlockNode,
                                   void               *userData );

DivXError AVI1CommonReadNextBlockByStream( AVI1ContainerHandle handle,
                                           AVIData            *pAviData,
                                           DMFBlockType_t      blockType,
                                           uint32_t            nStream,
                                           DMFBlockNode_t     *pBlockNode,
                                           void               *userData );

DivXError AVI1ReadBlockFromIndexEntry( AVI1ContainerHandle handle,
                                       AVIData            *pAviData,
                                       AVI1IndexEntry     *pIndexEntry,
                                       unsigned char      *pBuffer,
                                       uint32_t            bufferSize );

DivXError AVI1GetDRMVideoBlockInfo( AVI1InstanceData *pInst,
                                    AVIData          *pData,
                                    uint32_t          blockNum,
                                    uint32_t          bUseIndex,
                                    uint32_t          bReverse4CC,
                                    DivXTime         *pBlockTime,
                                    DivXTime         *pDuration,
                                    uint32_t         *pIndexNum,
                                    uint64_t         *pBlockOffset,
                                    uint32_t         *pSize );

int GetBlockNumberFromIndexEntry( AVI1ContainerHandle handle,
                                  DMFBlockType_t      blockType,
                                  uint32_t            nStream,
                                  uint32_t            nAbsoluteIndexNum );

DivXError AVI1SetBlock( AVI1InstanceData *pInst,
                        AVIData          *pData,
                        DMFBlockType_t    blockType,
                        uint32_t          nStream,
                        uint32_t         *pNumBlock,
                        DivXTime         *pTime,
                        uint32_t         *pBlockSize,
                        DivXBool          bSkipDRM,
                        DivXBool          titleRiff,
                        DivXBool          bKeyFrameOnly );

AVI1StreamLocationInfo *AVI1GetStreamLocationInfo( AVIData       *pAviData,
                                                   DMFBlockType_t blockType,
                                                   int32_t        nStreamOfType );

DivXError GetNextBlockIndexEntryByStream( AVI1ContainerHandle handle,
                                          AVIData            *pAviData,
                                          AVIChunkPosInfo_t  *pChunkPos,
                                          DMFBlockType_t      blockType,
                                          int32_t             nStreamOfType );

DivXError AVI1CalculateTimestamps( AVI1InstanceData  *pInst,
                                   AVIData           *pData,
                                   AVIChunkPosInfo_t *pChunkPos,
                                   DivXTime          *pAbsoluteTime,
                                   DivXTime          *pPeriod );

DivXError AVI1UpdateStreamLoction( AVI1InstanceData  *pInst,
                                   AVIData           *pData,
                                   AVIChunkPosInfo_t *pChunkPos );

DivXError GetNextActiveBlockIndexEntry( AVI1ContainerHandle handle,
                                        AVIData            *pAviData,
                                        AVIChunkPosInfo_t  *pChunkPos );

DivXError DMFHttpParmsCpy( DMFHttpAuth *dest,
                           DMFHttpAuth *src,
                           DivXMem      hMem );

#ifdef __cplusplus
}
#endif

#endif /* _AVI1PARSEINTERNAL_H_ */
