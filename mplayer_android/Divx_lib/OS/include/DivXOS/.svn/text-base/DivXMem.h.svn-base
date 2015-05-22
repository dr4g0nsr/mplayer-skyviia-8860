/*!
    @file
@verbatim
$Id: DivXMem.h 58738 2009-03-03 02:14:18Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXMEM_H_
#define _DIVXMEM_H_

#include "DivXTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
typedef struct _MemoryManager *DivXMem;

/*! Create memory manager over memory pool

  @param totalBytes (IN)  - Total number of bytes for this block
  @return DivXMem   (OUT) - Memory block handle

*/
DivXMem DivXMemInit(uint32_t totalBytes);

/*! Returns the initialized memory from the system back to the memory pool.

    A typical use of this API would be:
    DivXMem handle = DivXMemInit(0x10000); // Intializes a memory pool
    DivXMemAlloc(handle, 100); // Allocates memory from the pool
    DivXMemAlloc(handle, 12); // Allocates memory from the pool
    DivXMemDeInit(handle); // On close of lib/API

    @param handle (IN)      - Handle to the memory manager
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXMemDeInit(DivXMem handle);

/*! This function returns allocated memory.  On a CE device this memory allocation should
    be taken from the DivXMemInit pre-allocated region.
    Alloc occurs only on the memory pool defined in a memory Init.

    @param numBytes   (IN)  - Number of bytes to allocate from the block
    @param handle     (IN)  - Handle to the memory block
    @return void*     (OUT) - Pointer to the allocated memory (NULL == Error)

*/
#if defined __mips__
//#define MEMDEBUG
#endif
#if defined MEMDEBUG

void AddAllocation(void *buf, uint32_t numBytes, char *file, int lineNo);
void RemoveAllocation(void *buf);
void DumpAllocation(void *buf, uint32_t numBytes, char *file, int lineNo);


#define DivXMemAlloc(numBytes, handle) DivXMemAllocDebug(numBytes, handle, __FILE__, __LINE__)
void *DivXMemAllocDebug(uint32_t numBytes, DivXMem handle, char *file, int lineNo);
#else
void DumpAllocation(void);

void *DivXMemAlloc(uint32_t numBytes, DivXMem handle);
#endif


/*! This function frees allocated memory.  On a CE device this memory free should return allocated
    memory to the pre-allocated region created by DivXMemInit.  In a non-CE application the allocated
	and de-allocated data should still be tracked to conform to the CE standard, ensuring interoperability
	of this API amongst all applications.

    @note This should not be used in most CE devices.

    @param ptr   (IN)       - Memory to free
    @param handle     (IN)  - Handle to the memory block
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXMemFree(void *ptr, DivXMem handle);

/*! This function sets the pointed to memory

    @note 

    @param ptr        (IN)  - Memory to set
    @param val        (IN)  - Value to set
    @param size       (IN)  - Size of memory to set
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
void *DivXMemSet(void *ptr, uint8_t val, uint32_t size);

/*! This function copies memory

    @note 

    @param dest        (IN)  - Destination memory
    @param src         (IN)  - Source memory
    @param size        (IN)  - Size of memory to copy
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
void *DivXMemCpy(void *dest, const void *src, uint32_t size);

/*! This function compares memory

    @note 

    @param dest        (IN)  - Destination memory
    @param src         (IN)  - Source memory
    @param size        (IN)  - Size of memory to compare
    @return DivXBool  (OUT)  - If dest and src are the same for size, 
                               then DIVX_TRUE, else DIVX_FALSE
*/
DivXBool DivXMemCmp(void* dest, const void* src, uint32_t size);


#ifdef __cplusplus
}
#endif


#endif
