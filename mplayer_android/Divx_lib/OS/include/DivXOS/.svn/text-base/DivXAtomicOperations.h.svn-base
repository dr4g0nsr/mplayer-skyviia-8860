/*!
    @file
   @verbatim
   $Id: DivXAtomicOperations.h 56462 2008-10-13 20:48:09Z jbraness $

   Copyright (c) 2008 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DIVX_ATOMIC_OPERATIONS_H_
#define _DIVX_ATOMIC_OPERATIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXTypes.h"

/*! Performs and Atomic Increment Operation

    @note             pInt must be aligned to a 32 bit boundary

    @param pInt       (IN)  - The 32-bit aligned address of an int32_t to increment

    @return int32_t (OUT) - The incremented value */
int32_t DivXAtomicIncrement(int32_t*pInt);

/*! Performs and Atomic Decrement Operation

    @note             pInt must be aligned to a 32 bit boundary

    @param pInt       (IN)  - The 32-bit aligned address of an int32_t to decrement

    @return int32_t (OUT) - The incremented value */
int32_t DivXAtomicDecrement(int32_t *pInt);

/*! Performs and Atomic Add Operation

    @note             pInt must be aligned to a 32 bit boundary

    @param amount     (IN)  - An int32_t to add to pInt
    @param pInt       (IN)  - The 32-bit aligned address of an int32_t as an Addend

    @return int32_t (OUT) - The incremented value */
int32_t DivXAtomicAdd(int32_t amount, int32_t *pInt);

#ifdef __cplusplus
}
#endif

#endif /*_DIVX_ATOMIC_OPERATIONS_H_*/
