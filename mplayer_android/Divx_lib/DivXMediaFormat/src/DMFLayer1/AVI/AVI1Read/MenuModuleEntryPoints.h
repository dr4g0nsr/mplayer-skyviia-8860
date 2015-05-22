/*!
    @file
@verbatim
$Id: MenuModuleEntryPoints.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MENUMODULEENTRYPOINTS_H_
#define _MENUMODULEENTRYPOINTS_H_

#include "DivXInt.h"
#include "DMFVariant.h"
#include "DMFProperties.h"
#include "DMFActionFormat.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVICommon/A1MPInstanceHandle.h"
#include "./AVI/AVI1ReadMenu/AVI1MPQueryStateData.h"
#include "AVI/AVI1Read/AVI1ContainerHandle.h"

typedef int ( *pfn_A1MPCreateInstance )( void *, uint64_t, A1MPRead, A1MPSeek,
                                         A1MPInstanceHandle *, uint8_t *, int32_t, void *,
                                         DivXMem );
typedef int ( *pfn_A1MPReleaseInstance )( A1MPInstanceHandle, DivXMem );
typedef int ( *pfn_A1MPSetMenuSystemVersion )( A1MPInstanceHandle, uint32_t );
typedef int ( *pfn_A1MPScanRiff )( A1MPInstanceHandle );
typedef int ( *pfn_A1MPSetQueryStateIndexes )( A1MPInstanceHandle, int, int );
typedef int ( *pfn_A1MPGetTitleInfo )( A1MPInstanceHandle, uint32_t, TitleInfo * );
typedef int ( *pfn_A1MPGetTitleNumChapters )( A1MPInstanceHandle, uint32_t, uint32_t * );
typedef int ( *pfn_A1MPGetQueryStatePtr )( A1MPInstanceHandle, QueryStateData ** );
typedef int ( *pfn_A1MPGetNumMenus )( A1MPInstanceHandle, int32_t * );
typedef int ( *pfn_A1MPGetMenuMediaOffsets )( A1MPInstanceHandle, uint64_t *, uint32_t * );
typedef int ( *pfn_A1MPGetTitleOffsets )( A1MPInstanceHandle, uint64_t *, uint32_t * );
typedef int ( *pfn_A1MPGetInfo )( AVI1ContainerHandle, int32_t, DMFVariant *, int32_t );
typedef int ( *pfn_A1MPSetInfo )( AVI1ContainerHandle, int, DMFVariant *, int, DMFVariant * );

typedef struct _MenuModuleEntryPoints_t
{
    pfn_A1MPCreateInstance       fnCreateInstance;
    pfn_A1MPReleaseInstance      fnReleaseInstance;
    pfn_A1MPSetMenuSystemVersion fnSetMenuSystemVersion;
    pfn_A1MPScanRiff             fnScanRiff;
    pfn_A1MPSetQueryStateIndexes fnSetQueryStateIndexes;
    pfn_A1MPGetTitleInfo         fnGetTitleInfo;
    pfn_A1MPGetTitleNumChapters  fnGetTitleNumChapters;
    pfn_A1MPGetQueryStatePtr     fnGetQueryStatePtr;
    pfn_A1MPGetNumMenus          fnGetNumMenus;
    pfn_A1MPGetMenuMediaOffsets  fnGetMenuMediaOffsets;
    pfn_A1MPGetTitleOffsets      fnGetTitleOffsets;
    pfn_A1MPGetInfo              fnGetInfo;
    pfn_A1MPSetInfo              fnSetInfo;
} MenuModuleEntryPoints_t;

#endif /* _MENUMODULEENTRYPOINTS_H_ */
