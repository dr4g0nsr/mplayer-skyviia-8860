/*!
    @file
@verbatim
$Id: ReadBuffer.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _READBUFFER_H_
#define _READBUFFER_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "./AVI/AVI1Read/ReadNonBuffer.h"

#ifdef USE_BUFFERED_READ

#ifdef __cplusplus
extern "C" {
#endif

typedef int ( *RDBRead )( void *handleRead, uint64_t offset, uint8_t *buffer, uint32_t length,
                          uint32_t *bytesRead, void *userData, uint32_t hint );

typedef int ( *RDBSeek )( void *handle, uint64_t fileLocation, uint32_t flags );

typedef void *RDBufferHandle;

DivXError RDBCreate( RDBufferHandle *outHandle,
                     uint8_t        *buffer,
                     int32_t         bufferSize,
                     RDBRead         fnRead,
                     RDBSeek         fnSeek,
                     void           *handleRead,
                     DivXMem         hMem );

DivXError RDBFree( RDBufferHandle handle,
                   DivXMem        hMem );

DivXError RDBGetBytes( RDBufferHandle handle,
                       uint64_t       location,
                       uint32_t       length,
                       uint8_t       *buffer,
                       void          *userData,
                       uint32_t       hint );

#ifdef __cplusplus
}
#endif

#endif /*USE_BUFFERED_READ*/

#endif /* _READBUFFER_H_ */
