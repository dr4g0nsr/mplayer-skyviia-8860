/*!
    @file
@verbatim
$Id: DMFOpticalDiskStream.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _DMFOPTICALDISKSTREAM_H_
#define _DMFOPTICALDISKSTREAM_H_

#include "DMFStreamCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError DMFStream_OpticalDisk_Open(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFilename, DMFStreamDirection_e direction);
DivXError DMFStream_OpticalDisk_Close(DMFStreamCommonInstanceData *streamData);
DivXError DMFStream_OpticalDisk_Seek(DMFStreamCommonInstanceData *streamData, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *puiLocation);
DivXError DMFStream_OpticalDisk_Tell(DMFStreamCommonInstanceData *streamData, uint64_t *puiLocation);

DivXError DMFStream_OpticalDisk_Read(DMFStreamCommonInstanceData *streamData, void *pBuf, uint32_t uiCount, uint32_t *puiBytesRead);
DivXError DMFStream_OpticalDisk_Write(DMFStreamCommonInstanceData *streamData, const void *buffer, uint32_t bytesToWrite, uint32_t *bytesWritten);

DivXError DMFStream_OpticalDisk_CreateFile(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFileName);

#ifdef __cplusplus
}
#endif

#endif
/* _DMFOPTICALDISKSTREAM_H_ */
