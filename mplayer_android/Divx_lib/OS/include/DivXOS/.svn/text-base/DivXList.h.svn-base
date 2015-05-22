/*!
    @file
@verbatim
$Id: DivXList.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXLIST_H_
#define _DIVXLIST_H_

#include "DivXFifo.h"

#ifdef __cplusplus
extern "C"
{
#endif


// This define is crucial for setting the type of this LIST
// Since this is a C implementation and we'd like to support 
// multiple types, this define allows for the code to be identical
// between implementations except for this define.
#ifdef DIVX_ARCH_64BIT
#define DIVX_LIST_TYPE uint64_t 
#else
#define DIVX_LIST_TYPE uint32_t 
#endif

/*! This is a master structure for a given list.  It contains info for accessing the
    elements.
*/
typedef struct _DivXList_t
{
  DivXFifo fifo;
  /*
#ifdef DIVX_ARCH_64BIT
  uint32_t blank;
#endif*/
  struct _DivXListElem_t *first;
  struct _DivXListElem_t *last;
  struct _DivXListElem_t *curr;
  /*
#ifdef DIVX_ARCH_64BIT
  uint32_t blank2;
#endif*/
} DivXList_t;

/*! This structure carries all DivXList elements
*/
typedef struct _DivXListElem_t
{
  DIVX_LIST_TYPE data;
  struct _DivXListElem_t *next;
  struct _DivXListElem_t *last;
} DivXListElem_t;

/*! Hide the fact that this is a pointer from the user (treating it
    as a handle.
*/
typedef DivXList_t* DivXList;

// The following API allows for an abstraction layer for Queue

/*! Calculates the expected memory usage of this API

    @note Determines the amount of memory needed by the list

    @param maxSize      (IN)   - Maximum number of elements for this list
    @return memory size (OUT)  - Returns memory needed for max elements
*/
uint32_t DivXListMemUsage(uint32_t maxSize);

/*! This function initializes a generic fifo that utilizes the DivXMem
    API for memory access.

    @note maxSize should match what was used in DivXListMemUsage
          A NULL handle will generate the list from heap

    @param maxSize      (IN)   - Maximum number of elements for this list
    @param handle       (IN)   - Memory manager handle for allocating the list
    @return DivXList    (OUT)  - Handle to the created list
*/
DivXList DivXListInit(uint32_t maxSize, DivXMem handle);

/*! Puts the walk pointer back to the beginning

    @note A NULL handle will generate the list from heap

    @param handle       (IN)   - Memory manager handle for allocating the list
    @param data        (OUT)   - Data returned from the end of the list

    @return DivXList    (OUT)  - Handle to the list
*/
DivXError DivXListBegin(DivXList handle, DIVX_LIST_TYPE *data);

/*! Puts the walk pointer at the end

    @note 

    @param handle       (IN)  - Handle to the list
    @param data        (OUT)  - Data returned from the end of the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListEnd(DivXList handle, DIVX_LIST_TYPE *data);

/*! Increments the walk ptr

    @note 

    @param handle       (IN)   - Handle to the list
    @param data        (OUT)  - Data returned from the next element of the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListNext(DivXList handle, DIVX_LIST_TYPE* data);

/*! Decrements the walk ptr

    @note 

    @param handle       (IN)   - Handle to the list
    @param data        (OUT)  - Data returned from the last element of the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListLast(DivXList handle, DIVX_LIST_TYPE* data);

/*! This function removes the element located at the walk ptr location

    @note 

    @param handle    (IN)   - Handle to the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListRemove(DivXList handle);


/*! This function pushes a DIVX_LIST_TYPE of data of size elemSize (defined in Init) to the back of the list.

    @note 

    @param DivXList    (IN)   - Handle to the list
    @param data        (IN)   - Push the data onto the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListPushBack(DivXList, DIVX_LIST_TYPE data);

/*! This function pushes a DIVX_LIST_TYPE of data of size elemSize (defined in Init) to the front of the list.

    @note 

    @param DivXList    (IN)   - Handle to the list
    @param data        (IN)   - Push the data onto the front of the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListPushFront(DivXList, DIVX_LIST_TYPE data);

/*! This function inserts a new DIVX_LIST_TYPE object.

    @note 

    @param DivXList    (IN)   - Handle to the list
    @param data        (IN)   - Push the data onto the front of the list

    @return DivXError  (OUT)  - Error code (see DivXError.h)
*/
DivXError DivXListInsert(DivXList, DIVX_LIST_TYPE data);

/*! This function returns the current size of the list

    @note 

    @param DivXList    (IN)   - Handle to the list
    @param size        (IN)   - Current size of the list
    @return DivXError  (OUT)  - Error code (see DivXError.h)
  */
DivXError DivXListSize(DivXList, uint32_t *size);


#ifdef __cplusplus
}
#endif

#endif

