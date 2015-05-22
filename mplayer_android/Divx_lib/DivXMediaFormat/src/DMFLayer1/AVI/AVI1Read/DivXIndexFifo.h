/*!
    @file
   @verbatim
   $Id: DivXIndexFifo.h 56354 2008-10-06 01:02:30Z sbramwell $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DIVXINDEXFIFO_H_
#define _DIVXINDEXFIFO_H_

#include "DivXTypes.h"
#include "./AVI/AVI1Read/AVI1IndexCacheEntry.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* This define is crucial for setting the type of this FIFO
   Since this is a C implementation and we'd like to support
   multiple types, this define allows for the code to be identical
   between implementations except for this define. */
#define DIVX_FIFO_TYPE uint32_t

/*! This structure carries all DivXIndexFifo info for a given Queue and should
    only be modified by the DivQueue API.
 */
typedef struct _DivXIndexFifo_t
{
    /* Place holder for the data */
    AVI1IndexCacheEntry_t *buffer;
    uint32_t               headPtr;
    uint32_t               tailPtr;
    uint32_t               walkPtr;
    uint32_t               maxSize;
} DivXIndexFifo_t;

/*! Hide the fact that this is a pointer from the user (treating it
    as a handle.
 */
typedef DivXIndexFifo_t *DivXIndexFifo;

/* The following API allows for an abstraction layer for Queue */

/*! Calculates the expected memory usage of this API

    @note This calculates the total expected memory usage based on the
           max size entered.

    @param maxSize (IN)  - Maximum size for the FIFO
    @return size   (OUT) - Size in bytes of memory expected to be used
                           by the FIFO
 */
uint32_t DivXIndexFifoMemUsage( const uint32_t maxSize );

/*! This function initializes a generic fifo that utilizes the DivXMem
    API for memory access.

    @note maxSize should match what was used in DivXIndexFifoMemUsage
          A NULL handle will generate the FIFO from heap

    @param maxSize   (IN)  - Maximum size for the FIFO
    @param handle    (IN)  - handle to the memory manager
    @return DivXIndexFifo (OUT) - Returns a handle to the fifo
 */
DivXIndexFifo DivXIndexFifoInit( const uint32_t maxSize,
                                 DivXMem        handle );

void DivXIndexFifoDeInit( DivXIndexFifo handle,
                               DivXMem       hMem );

/*! Puts the walk pointer back to the beginning and returns the data there

    @note Fifo Handle must be initialized by DivXIndexFifoInit

    @param fifo       (IN)   - Handle to a FIFO
    @param type*      (OUT)  - Returns the value at the start of the fifo
    @return DivXError (OUT)  - Returns an error code
 */
DivXError DivXIndexFifoStart( DivXIndexFifo fifo,
                              AVI1IndexCacheEntry_t * );

/*! Puts the walk pointer to the next element in the fifo

    @note Fifo Handle must be initialized by DivXIndexFifoInit

    @param fifo       (IN)   - Handle to a FIFO
    @param type*      (OUT)  - Returns the value at the next element of the fifo
    @return DivXError (OUT)  - Returns an error code
 */
DivXError DivXIndexFifoNext( DivXIndexFifo fifo,
                             AVI1IndexCacheEntry_t * );

/*! This function pushes a void* of data of size elemSize (defined in Init).

    @note Fifo Handle must be initialized by DivXIndexFifoInit

    @param fifo       (IN)   - Handle to a FIFO
    @param type*      (IN)   - Value to be stored in FIFO
    @return DivXError (OUT)  - Returns an error code
 */
DivXError DivXIndexFifoPush( DivXIndexFifo,
                             AVI1IndexCacheEntry_t );

/*! This function pops a AVI1IndexCacheEntry_t of data of size elemSize (defined in Init).

    @note Fifo Handle must be initialized by DivXIndexFifoInit

    @param fifo       (IN)   - Handle to a FIFO
    @param type*      (OUT)  - Value of popped element in FIFO
    @return DivXError (OUT)  - Returns an error code
 */
DivXError DivXIndexFifoPop( DivXIndexFifo,
                            AVI1IndexCacheEntry_t * );

/*! This function returns the current size of the queue.

    @note Fifo Handle must be initialized by DivXIndexFifoInit

    @param fifo       (IN)   - Handle to a FIFO
    @param size*      (OUT)  - Returns the current size of the FIFO
    @return DivXError (OUT)  - Returns an error code
 */
void DivXIndexFifoSize(           DivXIndexFifo,
                                       uint32_t *size );

#ifdef __cplusplus
}
#endif

#endif /* _DIVXINDEXFIFO_H_ */
