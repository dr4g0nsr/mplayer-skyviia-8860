/*!
    @file
@verbatim
$Id: DMFInputStream.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/


#ifndef _DMFINPUTSTREAM2_H_
#define _DMFINPUTSTREAM2_H_

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

typedef struct __DMFInputStream * DMFInputStreamHandle;

DivXError DMFInputStream_Create(DMFInputStreamHandle *streamHandle, DMFStreamType_e eStreamType, DivXMem hMem);
DivXError DMFInputStream_Destroy(DMFInputStreamHandle streamHandle);

DivXError DMFInputStream_GetInfo(DMFInputStreamHandle streamHandle, int32_t iQueryID, DMFVariant *Variant);
DivXError DMFInputStream_SetInfo(DMFInputStreamHandle streamHandle, int32_t iQueryID, DMFVariant Variant);

DivXError DMFInputStream_Open(DMFInputStreamHandle streamHandle, const DivXString *sInputFileName);
DivXError DMFInputStream_Close(DMFInputStreamHandle streamHandle);
DivXError DMFInputStream_Seek(DMFInputStreamHandle streamHandle, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *location);
DivXError DMFInputStream_Tell(DMFInputStreamHandle streamHandle, uint64_t *location);
DivXError DMFInputStream_Read(DMFInputStreamHandle streamHandle, void *buffer, uint32_t bytesToRead, uint32_t *bytesRead);

#ifdef __cplusplus
}
#endif

#endif
