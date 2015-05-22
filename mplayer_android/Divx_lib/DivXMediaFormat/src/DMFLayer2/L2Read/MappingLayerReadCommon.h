/*!
    @file
@verbatim
$Id: MappingLayerReadCommon.h 56119 2008-09-25 07:07:20Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MAPPINGLAYERREADCOMMON_H_
#define _MAPPINGLAYERREADCOMMON_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFContainerHandle.h"

#ifdef __cplusplus
extern "C"
{
#define DEFAULT_VAL( v )  = v
#else
#define DEFAULT_VAL( v )
#endif

/* / Get top-level menu block, exclusive of menu media (MRIFs).
   / This is just an alternate entry for L2GetInfo(container,DMF_QID_CONTAINER_MENUBLOCK_RAW,...) */
    DivXError L2GetMenu( DMFComponentContainerHandle containerHandle, int32_t bufferSize, void *buffer,
                         int32_t * bytesRead );

#ifdef __cplusplus
}
#endif

#endif
