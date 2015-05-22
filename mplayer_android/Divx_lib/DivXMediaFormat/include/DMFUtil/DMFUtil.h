/*!
    @file
@verbatim
$Id: DMFUtil.h 58926 2009-03-14 20:56:01Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/
#ifndef DMFUTIL_H_
#define DMFUTIL_H_

#include "DivXTypes.h"
#include "DivXTime.h"
#include "DivXString.h"


#ifdef __cplusplus
extern "C" {
#endif

#define FourCCToI32( a ) * ( (uint32_t *)( a ) )

void DivXTimeToString( DivXTime time, DivXString *buffer, int32_t size );

DivXBool IsVideoCodecSupported( uint32_t compressionCode );

DivXBool IsAudioCodecSupported( uint32_t compressionCode );

DivXBool IsSubtitleCodecSupported( uint32_t compressionCode );

void CompressCodeToCodecName( uint32_t compressionCode, DivXString *buffer, int32_t bufferSize );

#ifdef __cplusplus
}
#endif
#endif
