/*!
    @file
@verbatim
$Id: DivXFifo.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXFIFO_H_
#define _DIVXFIFO_H_

#include "DivXMem.h"

#ifdef __cplusplus
extern "C"
{
#endif

// This define is crucial for setting the type of this FIFO
// Since this is a C implementation and we'd like to support 
// multiple types, this define allows for the code to be identical
// between implementations except for this define.
#ifdef DIVX_ARCH_64BIT
#define DIVX_FIFO_TYPE uint64_t 
#else
#define DIVX_FIFO_TYPE uint32_t 
#endif

/*! This structure carries all DivXFifo info for a given Queue and should
    only be modified by the DivQueue API.
*/
typedef struct _DivXFifo_t
{
	// Place holder for the data 
    DIVX_FIFO_TYPE *buffer;
    uint32_t headPtr;
    uint32_t tailPtr;
    uint32_t walkPtr;
    uint32_t maxSize;
    /*
#ifdef DIVX_ARCH_64BIT
    uint32_t blank;
#endif*/
} DivXFifo_t;

/*! Hide the fact that this is a pointer from the user (treating it
    as a handle.
*/
typedef DivXFifo_t* DivXFifo;

// The following API allows for an abstraction layer for Queue

/*! Calculates the expected memory usage of this API

    @note This calculates the total expected memory usage based on the 
           max size entered.

    @param maxSize (IN)  - Maximum size for the FIFO
    @return size   (OUT) - Size in bytes of memory expected to be used 
                           by the FIFO
*/
uint32_t DivXFifoMemUsage(const uint32_t maxSize);

/*! This function initializes a generic fifo that utilizes the DivXMem
    API for memory access.

    @note maxSize should match what was used in DivXFifoMemUsage
          A NULL handle will generate the FIFO from heap

    @param maxSize   (IN)  - Maximum size for the FIFO
    @param handle    (IN)  - handle to the memory manager
    @return DivXFifo (OUT) - Returns a handle to the fifo
*/
DivXFifo DivXFifoInit(const uint32_t maxSize, DivXMem handle);

/*! Puts the walk pointer back to the beginning and returns the data there

    @note Fifo Handle must be initialized by DivXFifoInit

    @param fifo       (IN)   - Handle to a FIFO 
    @param type*      (OUT)  - Returns the value at the start of the fifo
    @return DivXError (OUT)  - Returns an error code
*/
DivXError DivXFifoStart(DivXFifo fifo, DIVX_FIFO_TYPE* type);

/*! Puts the walk pointer to the next element in the fifo

    @note Fifo Handle must be initialized by DivXFifoInit

    @param fifo       (IN)   - Handle to a FIFO 
    @param type*      (OUT)  - Returns the value at the next element of the fifo
    @return DivXError (OUT)  - Returns an error code
*/
DivXError DivXFifoNext(DivXFifo fifo, DIVX_FIFO_TYPE* type);

/*! This function pushes a void* of data of size elemSize (defined in Init).

    @note Fifo Handle must be initialized by DivXFifoInit

    @param fifo       (IN)   - Handle to a FIFO 
    @param type       (IN)   - Value to be stored in FIFO
    @return DivXError (OUT)  - Returns an error code
*/
DivXError DivXFifoPush(DivXFifo, DIVX_FIFO_TYPE type);

/*! This function pops a DIVX_FIFO_TYPE of data of size elemSize (defined in Init).

    @note Fifo Handle must be initialized by DivXFifoInit

    @param fifo       (IN)   - Handle to a FIFO 
    @param type*      (OUT)  - Value of popped element in FIFO
    @return DivXError (OUT)  - Returns an error code
*/
DivXError DivXFifoPop(DivXFifo, DIVX_FIFO_TYPE* type);

/*! This function returns the current size of the queue.

    @note Fifo Handle must be initialized by DivXFifoInit

    @param fifo       (IN)   - Handle to a FIFO 
    @param size*      (OUT)  - Returns the current size of the FIFO
    @return DivXError (OUT)  - Returns an error code
  */
DivXError DivXFifoSize(DivXFifo, uint32_t* size);


#ifdef __cplusplus
}
#endif

#endif

