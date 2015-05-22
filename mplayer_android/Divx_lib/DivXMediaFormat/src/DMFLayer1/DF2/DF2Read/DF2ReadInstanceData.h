/*!
    @file
@verbatim
$Id: DF2ReadInstanceData.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2READINSTANCEDATA_H_
#define _DF2READINSTANCEDATA_H_

#include "DivXInt.h"
#include "AVIXOffsetData.h"
#include "./DF2/DF2Read/AVIXOffsetData.h"
#include "./AVI/AVI1Read/AVI1InstanceData.h"

#define MAX_AVIX 100
#define MAX_ISFT 256
#define MAX_IDFV 256
#define MAX_IDPN 256

typedef struct _DF2ReadInstanceData
{
    AVI1InstanceData *avi1; /* Instance of the AVI1 structure, use this first if available */

    int32_t          nAVIX;
    AVIXOffsetData_t avix[MAX_AVIX];
    char             isftData[MAX_ISFT];
    char             idfvData[MAX_IDFV];
    char             idpnData[MAX_IDPN];
} DF2ReadInstanceData;

#endif /* _DF2READINSTANCEDATA_H_ */
