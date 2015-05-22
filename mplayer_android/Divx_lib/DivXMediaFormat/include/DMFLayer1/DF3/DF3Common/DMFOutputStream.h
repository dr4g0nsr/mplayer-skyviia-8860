/*!
    @file
@verbatim
$Id: DMFOutputStream.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DMFOUTPUTSTREAMCOMMON_H_
#define _DMFOUTPUTSTREAMCOMMON_H_


#include "DivXString.h"
#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXFile.h"

#include "DMFVariant.h"
#include "DMFQueryIDs.h"

#include "DMFStreamTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct __DMFOutputStream * DMFOutputStreamHandle;

DivXError DMFOutputStream_Create(DMFOutputStreamHandle *streamHandle, DMFStreamType_e eStreamType, DivXMem hMem);
DivXError DMFOutputStream_Destroy(DMFOutputStreamHandle streamHandle);

DivXError DMFOutputStream_GetInfo(DMFOutputStreamHandle streamHandle, int32_t iQueryID, DMFVariant *Variant);
DivXError DMFOutputStream_SetInfo(DMFOutputStreamHandle streamHandle, int32_t iQueryID, DMFVariant Variant);

DivXError DMFOutputStream_Open(DMFOutputStreamHandle streamHandle, const DivXString *sInputFileName);
DivXError DMFOutputStream_Close(DMFOutputStreamHandle streamHandle);
DivXError DMFOutputStream_Seek(DMFOutputStreamHandle streamHandle, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *location);
DivXError DMFOutputStream_Tell(DMFOutputStreamHandle streamHandle, uint64_t *location);
DivXError DMFOutputStream_Write(DMFOutputStreamHandle streamHandle, const void *buffer, uint32_t bytesToWrite, uint32_t *bytesWritten); 

DivXError DMFOutputStream_CreateFile(DMFOutputStreamHandle streamHandle, const DivXString *sInputFileName);

#ifdef __cplusplus
}
#endif

#endif /* _DMFOUTPUTSTREAMCOMMON_H_ */
