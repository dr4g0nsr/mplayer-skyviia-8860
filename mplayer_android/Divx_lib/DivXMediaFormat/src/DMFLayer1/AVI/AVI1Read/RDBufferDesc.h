/*!
    @file
@verbatim
$Id: RDBufferDesc.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _RDBUFFERDESC_H_
#define _RDBUFFERDESC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RDBufferDesc_
{
    uint64_t fileSize;
    uint64_t fileLocation;

    uint32_t     bufferSize;
    uint8_t     *buffer;
    RDBRead      fnRead;
    RDBSeek      fnSeek;
    void        *handleRead;
} RDBufferDesc;

#ifdef __cplusplus
}
#endif

#endif /* _RDBUFFERDESC_H_ */
