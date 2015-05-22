/*!
    @file
@verbatim
$Id: RingBuffer.h 49283 2008-02-11 16:48:26Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include "DivXInt.h"
#include "DivXMem.h"

typedef struct ring_buffer_t
{
    uint8_t *frameBuffer;
    uint32_t totalBufferSize;
    uint32_t bytesInBuffer;
    uint8_t *readPtr;
    uint8_t *writePtr;
    DivXMem  hMem;
}   ring_buffer_t;

#ifdef __cplusplus
extern "C" {
#endif

void *ring_buffer_new( uint32_t bufferSize,
                       DivXMem  hMem );

void ring_buffer_free( void *hRingBuffer );

int ring_buffer_putbytes( void    *hRingBuffer,
                          const uint8_t *data,
                          uint32_t size );

int ring_buffer_getbytes( void    *hRingBuffer,
                          uint8_t *actualData,
                          uint32_t size );

int ring_buffer_peekbytes( void    *hRingBuffer,
                           uint8_t *data,
                           uint32_t size );

uint32_t ring_buffer_getbufferedcount( void *hRingBuffer );

#ifdef __cplusplus
}
#endif

#endif /*_RINGBUFFER_H */
