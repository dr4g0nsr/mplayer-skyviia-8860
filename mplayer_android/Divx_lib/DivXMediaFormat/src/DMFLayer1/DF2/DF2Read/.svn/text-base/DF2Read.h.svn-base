/*!
    @file
@verbatim
$Id: DF2Read.h 57969 2009-01-14 01:23:14Z fchan $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2READ_H_
#define _DF2READ_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFBlockType.h"
#include "./AVI/AVI1Read/AVIData.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"
#include "./DF2/DF2Read/DF2ReadInstanceData.h"
#include "./AVI/AVI1Read/AVI1ContainerHandle.h"
#include "./DF2/DF2Common/DF2StdIndexChunk.h"
#include "./DF2/DF2Common/DF2SuperIndexChunk.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError L1DF2r_CreateInstance( DF2ReadInstanceData **ppInst,
                                 DivXMem               hMem );

//#ifdef __SYMBIAN32__
DivXError df2r_OpenContainerForFile( void             *ContainerHandle,
									 PDivXFileExternalObj file,
                                     DivXMem          hMem );
//#endif // __SYMBIAN32__

DivXError df2r_OpenContainer( void             *ContainerHandle,
                              const DivXString *filename,
                              DivXMem           hMem );

//#ifdef __SYMBIAN32__
DivXError df2r_OpenContainerForFileEx( PDivXFileExternalObj file,
                                       void            **handle,
                                       AVI1OpenFN        pOpen,
                                       AVI1ReadFN        pRead,
                                       AVI1CloseFN       pClose,
                                       void             *userData,
                                       DivXMem           hMem );
//#endif // __SYMBIAN32__
DivXError df2r_OpenContainerEx( const DivXString *filename,
                                DF2ReadInstanceData  **handle,
                                AVI1OpenFN        pOpen,
                                AVI1ReadFN        pRead,
                                AVI1CloseFN       pClose,
                                void             *userData,
                                DivXMem           hMem );

DivXError df2r_CompleteOpenContainer( const DivXString    *filename,
                                      DF2ReadInstanceData *pAvi2,
                                      void                *userData,
                                      DivXMem              hMem,
                                      AVI1InstanceData    *pInst,
                                      DivXFile             fileHandle );

DivXError df2r_ReadAvixOffsets( RDBufferHandle       handle,
                                uint64_t            *avixOffset,
                                DF2ReadInstanceData *pAvi2,
                                void                *userData,
                                DivXMem              hMem );

DivXError df2r_ScanRiff( DF2ReadInstanceData *pDF2Data,
                         AVI1InstanceData *pInst,
                         AVIData          *pData,
                         uint32_t          riffIndex,
                         RDBufferHandle    handleBuffer,
                         void             *userData );

DivXError df2r_SetTitle( void    *pDF2Handle,
                         void    *handle,
                         uint32_t titleIndex );

DivXError df2r_MenuMedia( DF2ReadInstanceData    *pDF2Inst,
                          AVI1InstanceData    *pInst,
                          uint32_t mediaIndex );

DivXError df2r_SetBlock( void          *pInst,
                         AVIData       *pData,
                         DMFBlockType_t blockType,
                         uint32_t       nStream,
                         uint32_t      *pNumBlock,
                         DivXTime      *pTime,
                         DivXBool       bSkipDRM,
                         DivXBool       titleRiff,
                         DivXBool       bKeyFrameOnly );

DivXError df2Title_SetActiveStream( AVI1ContainerHandle handle,
                                    DMFBlockType_t      blockType,
                                    uint32_t            nStream,
                                    DivXBool            bActive );

DivXError readDF2StdIndexChunk( RDBufferHandle    handle,
                                uint64_t          location,
                                DF2StdIndexChunk *buffer,
                                void             *userData,
                                uint32_t          hint );

int32_t readDF2StdIndexEntry( RDBufferHandle    handle,
                              uint64_t          location,
                              DF2StdIndexEntry *buffer,
                              void             *userData,
                              uint32_t          hint );

int32_t readDF2SuperIndexEntry( RDBufferHandle      handle,
                             uint64_t            location,
                             DF2SuperIndexEntry *buffer,
                             void               *userData,
                             uint32_t            hint );

#ifdef __cplusplus
}
#endif
#endif /* _DF2READ_H_ */
