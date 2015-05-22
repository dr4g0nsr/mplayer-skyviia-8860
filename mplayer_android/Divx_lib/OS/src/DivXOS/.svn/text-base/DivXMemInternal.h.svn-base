/**
 ** Copyright (C) 2006 DivXNetworks, all rights reserved.
 **
 ** DivXNetworks, Inc. Proprietary & Confidential
 **
 ** This source code and the algorithms implemented therein constitute
 ** confidential information and may comprise trade secrets of DivXNetworks
 ** or its associates, and any use thereof is subject to the terms and
 ** conditions of the Non-Disclosure Agreement pursuant to which this
 ** source code was originally received.
 **
 **/

#ifndef _DIVXMEMINTERNAL_H_
#define _DIVXMEMINTERNAL_H_

#include "DivXTypes.h"
#include "DivXMem.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _MemoryManager MemoryManager;

typedef struct _AllocEntry
{
    uint32_t offset;    //allocated position
    uint32_t size;      //allocated size
} AllocEntry;

/*! Adds a memory tracking node (mem block instance number and address)

    @note - uses static variables "DivXUsedMem" (linked list) and blockInstance (uint64_t)
            only enabled when _DEBUG flag defined

    @param address      (IN) - the address of the memory block being tracked
    @return DivXError   (OUT) - Error code -- DIVX_ERR_SUCCESS if the node was added
                                              DIVX_ERR_FAILURE if node could not be added
*/
DivXError DivXMemAddUsedMemNode(uint64_t address);

/*!  Removes a memory tracking node (mem block instance number and address) from the tracking list

    @note - uses static variables "DivXUsedMem" (linked list) and blockInstance (uint64_t)
            only enabled when _DEBUG flag defined

    @param address      (IN) - the address of the memory block being tracked
    @return DivXError   (OUT) - Error code -- DIVX_ERR_SUCCESS if the node was removed
                                              DIVX_ERR_FAILURE if node could not be removed

*/
DivXError DivXMemRemoveUsedMemNode(uint64_t address);


/*!  prints all memory tracking nodes in the memory tracking list

    @note - uses static variables "DivXUsedMem" (linked list) and blockInstance (uint64_t)
            only enabled when _DEBUG flag defined

    @return DivXError   (OUT) - Error code -- DIVX_ERR_SUCCESS if list was printed
                                              DIVX_ERR_FAILURE if memory tracking list could not be printed

*/
DivXError DivXMemCheckUsedMemList();

/*!  frees all structures (memory tracking list and all nodes in memory tracking list) associated
     with memory tracking

    @note - uses static variables "DivXUsedMem" (linked list) and blockInstance (uint64_t)
            only enabled when _DEBUG flag defined

    @return DivXError   (OUT) - Error code -- DIVX_ERR_SUCCESS if all memory tracking structures were freed
                                              DIVX_ERR_FAILURE if memory tracking structures could not be freed

*/
DivXError DivXMemCleanUpUsedMemList();


/*! find allocated buffer entry */
static DivXBool DivXMemFindAllocated(AllocEntry *alloced, uint32_t count, uint32_t offset, uint32_t *pIndex);

/*! find insertion position for available buffer */
static DivXBool DivXMemFindInsertion(AllocEntry *alloced, uint32_t count, uint32_t offset, uint32_t *pIndex);

/*! find available buffer */
static DivXBool DivXMemFindAvailable(MemoryManager *manager, uint32_t size, uint32_t *pOffset);

#ifdef __cplusplus
}
#endif


#endif
