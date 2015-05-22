/*!
    @file
@verbatim
$Id: DMFNetworkStream.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _DMFNETWORKSTREAM_H_
#define _DMFNETWORKSTREAM_H_

#include "DMFStreamCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError DMFInputStream_Network_Open(DMFStreamCommonInstanceData *streamData, const DivXString *sInputFilename, DMFStreamDirection_e direction);
DivXError DMFInputStream_Network_Close(DMFStreamCommonInstanceData *streamData);

DivXError DMFInputStream_Network_Read(DMFStreamCommonInstanceData *streamData, void *pBuf, uint32_t uiCount, uint32_t *puiBytesRead);
DivXError DMFInputStream_Network_Seek(DMFStreamCommonInstanceData *streamData, int64_t iOffset, DivXSeekLocation_e origin, uint64_t *puiLocation);
DivXError DMFInputStream_Network_Tell(DMFStreamCommonInstanceData *streamData, uint64_t *puiLocation);

#ifdef __cplusplus
}
#endif

#endif
/* _DMFNETWORKSTREAM_H_ */
