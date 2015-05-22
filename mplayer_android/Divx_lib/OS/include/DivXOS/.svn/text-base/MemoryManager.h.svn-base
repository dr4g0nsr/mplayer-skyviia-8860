/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "DivXInt.h"
#include "DivXBool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! opaque handle defined internally */
typedef struct _MemoryManager *MemoryManager;

/*! parameters for memory manager */
typedef struct _MemoryParams
{
    uint8_t *buf;       /*! buffer to use for dynamic memory */
    uint32_t bufSize;   /*! size of buffer to use */
    uint8_t align;      /*! set byte alignment */
}
MemoryParams;

/*!
    MemoryCreate - create memory manager for dynamic memory

    @param pParams (INP) - parameters used for memory manager
    @return NULL for error, otherwise valid handle
*/
MemoryManager MemoryCreate(MemoryParams *pParams);

/*!
    MemoryDestroy - shutdown memory manager

    @param manager (INP) - memory manager handle
    @return void
*/
void MemoryDestroy(MemoryManager manager);

/*!
    MemoryAlloc - allocate buffer from memory pool

    @param manager (INP) - memory manager handle
    @param size (INP) - size of buffer to allocate
    @return NULL for error, otherwise valid handle
*/
void *MemoryAlloc(MemoryManager manager, uint32_t size);

/*!
    MemoryFree - release buffer to memory pool

    @param manager (INP) - memory manager handle
    @param buf (INP) - buffer to be returned
    @return void
*/
void MemoryFree(MemoryManager manager, void *buf);

/*!
    MemoryReset - instruct memory manager to reclaim all memory

    @param manager (INP) - memory manager handle
    @return void
*/
void MemoryReset(MemoryManager manager);

/*!
    MemorySet - utility function for initializing memory

    @param buf (INP) - buffer to initialize
    @param val (INP) - value to initialize with
    @param size (INP) - length of buffer to set
    @return void
*/
void MemorySet(void *buf, uint8_t val, uint32_t size);

/*!
    MemoryCopy - utility function for copying memory

    @param dest (INP) - buffer to copy into
    @param src (INP) - buffer to copy from
    @param size (INP) - amount of buffer to copy
    @return void
*/
void MemoryCopy(void *dest, const void *src, uint32_t size);

/*!
    MemoryDebugAvailable - compute amount of abailable memory

    @param manager (INP) - memory manager handle
    @return void
*/
uint32_t MemoryDebugAvailable(MemoryManager manager);

/*!
    MemoryDebugValidate (INP) - validate unallocated memory

    @param manager (INP) - memory manager handle
    @return void
*/
void MemoryDebugValidate(MemoryManager manager);

#ifdef __cplusplus
}
#endif

#endif
