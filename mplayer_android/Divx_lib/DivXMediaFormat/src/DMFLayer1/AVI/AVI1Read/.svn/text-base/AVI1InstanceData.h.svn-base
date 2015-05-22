/*!
    @file
@verbatim
$Id: AVI1InstanceData.h 57969 2009-01-14 01:23:14Z fchan $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1INSTANCEDATA_H_
#define _AVI1INSTANCEDATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXFile.h"
#include <DivXFileStruct.h>
#include "DivXError.h"
#include "DivXString.h"
#include "./AVI/AVI1Read/ReadBuffer.h"
#include "./AVI/AVI1Read/AVIData.h"
#include "./AVI/AVI1Read/MenuModuleEntryPoints.h"
#include "DMFWebProxy.h"
#include "DMFHttpAuth.h"

/* These are the open/read/close function pointer definitions for callback */
typedef DivXError ( *AVI1OpenFN )( const DivXString *sFilename, int iOpenFlags,
                                   DivXFile *pHandle, DivXMem hMem );

typedef DivXError ( *AVI1ReadFN )( void *handleRead, uint64_t offset, uint8_t *buffer,
                                   uint32_t length, uint32_t *bytesRead, void *userData,
                                   uint32_t hint );

typedef DivXError ( *AVI1SeekFN )( void *handle, uint64_t fileLocation, uint32_t flags );

typedef DivXError ( *AVI1CloseFN )( void *handleRead );

/* These are progress/bitrate access function pointers */
typedef DivXError ( *AVI1GetProgressFN )( void *handle, int8_t *progress );
typedef DivXError ( *AVI1GetBitrateFN )( void *handle, int32_t *bitrate );
typedef DivXError ( *AVI1GetFileSizeFN )( void *handle, uint64_t *fileSize );

typedef void *HttpFileHandle;


typedef struct __AVI1InstanceData__
{
    /* AVI1OpenFN      pOpen; */
    AVI1ReadFN       pRead;
    AVI1CloseFN      pClose;
    AVI1SeekFN       pSeek;
    void            *userData;

    AVI1GetProgressFN pGetProgress;
    AVI1GetBitrateFN  pGetBitrate;
    AVI1GetFileSizeFN  pGetFileSize;

    DMFWebProxy webProxy;   /* Only used in progressive network playback */
    DMFHttpAuth httpAuth;   /* Only used in progressive network playback */
/* Should use anonymous union instead of casting file handle to HttpFileHandle, but gcc 2.95 doesn't allow it */
#if defined WIN32 || (__GNUC__ >= 3)    
    union {
        DivXFile fileHandle;
        HttpFileHandle httpFileHandle;
    };                      /* Anonymous union for pointers to different handle types */
#else
    DivXFile fileHandle; // will need to use a cast for HttpFileHandle assignments
#endif
    DivXString*      fileName;
    RDBufferHandle   handleBuffer;
    uint8_t         *readBuffer;
    uint8_t         *menuBuffer;

    A1MPInstanceHandle menuInst;

    int32_t bUseIndex;

    uint32_t keyFrameGap;

    uint64_t menuOffset;
    uint64_t metadataOffset;

    AVIData titleRiffs;
    AVIData menuRiffs;

    DivXMem hMem;

    MenuModuleEntryPoints_t *menuEntryPoints;

    uint64_t curOffsetInfoMovi;
    DivXBool bRemoteFile;
    DivXBool bUseCacheLessIndex;
    DivXBool bMinMemUsage;
} AVI1InstanceData;

#ifdef __cplusplus
}
#endif

#endif /* _AVI1INSTANCEDATA_H_ */
