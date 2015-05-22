/*!
    @file
@verbatim
$Id: mp3ContainerReadEntry.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3CONTAINERREADENTRY_H_
#define _MP3CONTAINERREADENTRY_H_

#include "DivXString.h"
#include "DMFFunctionPointers.h"
#include "DMFModuleAttributes.h"

#ifdef __cplusplus
extern "C"
{
#endif

int32_t L1mp3r_Init( const void *data );

DMFModuleAttributes *L1mp3r_GetCaps( void );

DivXError L1mp3r_IsContainerSupported( int32_t    *SupportLevel,
                                       const DivXString *Filename );

#ifdef __cplusplus
}
#endif

#endif /* _MP3CONTAINERREADENTRY_H_ */
