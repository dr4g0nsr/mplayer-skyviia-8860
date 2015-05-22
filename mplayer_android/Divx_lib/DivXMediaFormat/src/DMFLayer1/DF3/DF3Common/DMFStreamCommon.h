/*!
    @file
@verbatim
$Id: DMFStreamCommon.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/


#ifndef _DMFSTREAMCOMMON_H_
#define _DMFSTREAMCOMMON_H_

#include "DivXString.h"
#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXFile.h"

#include "DMFVariant.h"
#include "DMFQueryIDs.h"

#include "./DF3/DF3Common/DMFStreamTypes.h"

typedef enum
{
    StreamRead,
    StreamWrite

} DMFStreamDirection_e;


typedef struct _DMFStreamCommonInstanceData
{
    DMFStreamType_e streamType;
    DivXMem hMem;
    
    union 
    {
        void        *p;
        DivXFile    hFileHandle;
    } InternalHandle_u;

    DivXString      *sFilename;
    uint64_t        fileSize;
    uint32_t        openFlags;

} DMFStreamCommonInstanceData;

typedef DivXError (*fnOpen) (DMFStreamCommonInstanceData *streamData, const DivXString *sInputFileName, DMFStreamDirection_e direction);
typedef DivXError (*fnClose)(DMFStreamCommonInstanceData *streamData);
typedef DivXError (*fnSeek) (DMFStreamCommonInstanceData *streamData, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *location);
typedef DivXError (*fnTell) (DMFStreamCommonInstanceData *streamData, uint64_t *location);
typedef DivXError (*fnGetInfo) (DMFStreamCommonInstanceData *streamData, int32_t iQueryID, DMFVariant *Variant);
typedef DivXError (*fnSetInfo) (DMFStreamCommonInstanceData *streamData, int32_t iQueryID, DMFVariant Variant);

typedef struct _DMFStreamCommon
{
    DMFStreamCommonInstanceData data;

    fnOpen pFnOpen;
    fnClose pFnClose;
    fnSeek pFnSeek;
    fnTell pFnTell;

    fnGetInfo pFnGetInfo;
    fnSetInfo pFnSetInfo;

} DMFStreamCommon;

#ifdef __cplusplus
extern "C" {
#endif

DivXError DMFStreamCommon_Create(DMFStreamCommon *stream, DMFStreamType_e eStreamType, DivXMem hMem);
DivXError DMFStreamCommon_Destroy(DMFStreamCommon *stream);

DivXError DMFStreamCommon_GetInfo(DMFStreamCommon *stream, int32_t iQueryID, DMFVariant *Variant);
DivXError DMFStreamCommon_SetInfo(DMFStreamCommon *stream, int32_t iQueryID, DMFVariant Variant);


DivXError DMFStreamCommon_Open(DMFStreamCommon *stream, const DivXString *sInputFileName, DMFStreamDirection_e direction);
DivXError DMFStreamCommon_Close(DMFStreamCommon *stream);
DivXError DMFStreamCommon_Seek(DMFStreamCommon *stream, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *location);
DivXError DMFStreamCommon_Tell(DMFStreamCommon *stream, uint64_t *location);

#ifdef __cplusplus
 }
#endif


#endif /* _DMFSTREAMCOMMON_H_ */
