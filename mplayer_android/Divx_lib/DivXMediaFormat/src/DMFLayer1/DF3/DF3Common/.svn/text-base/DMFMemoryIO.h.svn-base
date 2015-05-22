/*!
    @file
@verbatim
$Id: DMFMemoryIO.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DMFMEMORYIO_H_
#define _DMFMEMORYIO_H_

#include "DMFStreamCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct __DMFPrivateStruct * MemoryHandle;

DivXError DMFMemory_Open(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFileName, DMFStreamDirection_e direction);
DivXError DMFMemory_Close(DMFStreamCommonInstanceData *streamData);
DivXError DMFMemory_Seek(DMFStreamCommonInstanceData *streamData, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *location);
DivXError DMFMemory_Tell(DMFStreamCommonInstanceData *streamData, uint64_t *location);

DivXError DMFMemory_Write(DMFStreamCommonInstanceData *streamData, const void *buffer, uint32_t bytesToWrite, uint32_t *bytesWritten);
DivXError DMFMemory_Read(DMFStreamCommonInstanceData *streamData, void *buffer, uint32_t bytesToRead, uint32_t *bytesRead);

DivXError DMFMemory_GetInfo(DMFStreamCommonInstanceData *streamData, int32_t iQueryID, DMFVariant *Variant);
DivXError DMFMemory_SetInfo(DMFStreamCommonInstanceData *streamData, int32_t iQueryID, DMFVariant Variant);


#ifdef __cplusplus
}
#endif

#endif /*_DMFMEMORYIO_H_ */
