/*!
    @file
@verbatim
$Id: L1Avi1ReadInternal.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1AVI1READINTERNAL_H_
#define _L1AVI1READINTERNAL_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockNode.h"
#include "DMFBlockType.h"
#include "AVI1ContainerHandle.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError L1avir_OpenContainerUnitTests( AVI1ContainerHandle *ContainerHandle,
                                         void              *mockFile,
                                         DivXMem            hMem );

DivXError L1avir_GetInfoInt8( AVI1ContainerHandle ContainerHandle,
                              int32_t           QueryValueID,
                              int8_t           *Int8Value,
                              int32_t           Index );

DivXError L1avir_GetInfoInt16( AVI1ContainerHandle ContainerHandle,
                               int32_t           QueryValueID,
                               int16_t          *Int16Value,
                               int32_t           Index );

DivXError L1avir_GetInfoInt32( AVI1ContainerHandle ContainerHandle,
                               int32_t           QueryValueID,
                               int32_t          *Int32Value,
                               int32_t           Index );

DivXError L1avir_GetInfoUInt32( AVI1ContainerHandle ContainerHandle,
                               int32_t           QueryValueID,
                               uint32_t         *UInt32Value,
                               int32_t           Index );

DivXError L1avir_GetInfoInt64( AVI1ContainerHandle ContainerHandle,
                               int32_t           QueryValueID,
                               int64_t          *Int64Value,
                               int32_t           Index );

DivXError L1avir_GetInfoFloat( AVI1ContainerHandle ContainerHandle,
                               int32_t           QueryValueID,
                               double           *DoubleValue,
                               int32_t           Index );

DivXError L1avir_GetInfoString( AVI1ContainerHandle ContainerHandle,
                                int32_t           QueryValueID,
                                DivXString       *StringValue,
                                int32_t           MaxLength,
                                int32_t           Index );

#ifdef __cplusplus
}
#endif

#endif /* _L1AVI1READINTERNAL_H_ */
