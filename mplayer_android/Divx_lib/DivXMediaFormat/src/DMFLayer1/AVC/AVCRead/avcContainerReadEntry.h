/*!
    @file
@verbatim
$Id: avcContainerReadEntry.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVCCONTAINERREADENTRY_H_
#define _AVCCONTAINERREADENTRY_H_

#include "DivXString.h"
#include "DMFFunctionPointers.h"
#include "DMFModuleAttributes.h"

#ifdef __cplusplus
extern "C"
{
#endif

int32_t L1avcr_Init( const void *data );

DMFModuleAttributes *L1avcr_GetCaps( void );

DivXError L1avcr_IsContainerSupported( int32_t    *SupportLevel,
                                       const DivXString *Filename );

#ifdef __cplusplus
}
#endif

#endif /* _AVCCONTAINERREADENTRY_H_ */
