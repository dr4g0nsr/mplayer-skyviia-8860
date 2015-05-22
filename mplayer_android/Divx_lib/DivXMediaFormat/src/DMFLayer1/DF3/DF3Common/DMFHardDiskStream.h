/*!
    @file
@verbatim
$Id: DMFHardDiskStream.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _DMFHARDDISKSTREAM_H_
#define _DMFHARDDISKSTREAM_H_

#include "DivXError.h"
#include "DivXString.h"

#include "DMFStreamCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError DMFStream_HardDisk_Open(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFilename, DMFStreamDirection_e direction);
DivXError DMFStream_HardDisk_Close(DMFStreamCommonInstanceData *streamData);
DivXError DMFStream_HardDisk_Seek(DMFStreamCommonInstanceData *streamData, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *puiLocation);
DivXError DMFStream_HardDisk_Tell(DMFStreamCommonInstanceData *streamData, uint64_t *puiLocation);

DivXError DMFStream_HardDisk_Read(DMFStreamCommonInstanceData *streamData, void *pBuf, uint32_t uiCount, uint32_t *puiBytesRead);
DivXError DMFStream_HardDisk_Write(DMFStreamCommonInstanceData *streamData, const void *buffer, uint32_t bytesToWrite, uint32_t *bytesWritten);


DivXError DMFStream_HardDisk_CreateFile(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFileName);

#ifdef __cplusplus
}
#endif

#endif
/* _DMFHARDDISKSTREAM_H_ */
