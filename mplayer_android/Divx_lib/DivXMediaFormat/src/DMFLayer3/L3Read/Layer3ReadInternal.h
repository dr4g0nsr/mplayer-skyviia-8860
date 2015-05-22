/*!
    @file
@verbatim
$Id: Layer3ReadInternal.h 53755 2008-06-18 09:37:29Z bbeyeler $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/
#ifndef _LAYER3READINTERNAL_H_
#define _LAYER3READINTERNAL_H_

#include "./L3Common/Layer3Internal.h"
#include "./L3Read/Layer3ReadPublished.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Close Function Header*/
DivXError L3cmr_BaseCloseContainer( Container *in,
                                    DivXMem    mem );

DivXError L3cmr_GetIndexedActionFromSelectAction( int32_t              idx,
                                                  DivXMem              mem,
                                                  SelectActionL3Handle inHandle,
                                                  ActionL3Handle      *outHandle );

DivXError L3cmr_CreateVirtualTitles( L3ResourcesHandle res, ContainerL3Handle inHandle );

#ifdef __cplusplus
}
#endif

#endif /* _LAYER3READINTERNAL_H_ */
