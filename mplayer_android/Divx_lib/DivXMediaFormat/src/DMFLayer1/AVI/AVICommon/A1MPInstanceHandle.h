/*!
    @file
@verbatim
$Id: A1MPInstanceHandle.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _A1MPINSTANCEHANDLE_H_
#define _A1MPINSTANCEHANDLE_H_

typedef int ( *A1MPRead )( void *handleRead, uint64_t offset, uint8_t *buffer, uint32_t length,
                           uint32_t *bytesRead, void *userData, uint32_t hint );

typedef int ( *A1MPSeek )( void *handle, uint64_t fileLocation, uint32_t flags );

typedef struct A1MPInstanceDataStruct *A1MPInstanceHandle;

#endif /* _A1MPINSTANCEHANDLE_H_ */
