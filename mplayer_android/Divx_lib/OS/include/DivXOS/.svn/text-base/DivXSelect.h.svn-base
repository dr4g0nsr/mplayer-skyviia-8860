/*!
    @file
@verbatim

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXSELECT_H_
#define _DIVXSELECT_H_

/*********************************************************************************************
 *  Includes
 */

#include "DivXTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************************************
 *  Defines
 */

/*! Max wait objects */
#define MAX_DIVX_WAIT_OBJECTS 64

/*! Infinite timeout for select */
#define DIVX_SELECT_INFINITE_TIMEOUT -1

/*! Define DivX FD Events  */
#define DIVX_FD_EVENT_READ  (1 << 0)
#define DIVX_FD_EVENT_WRITE (1 << 1)

/*! File descriptor type */
enum DivXFileDescType
{
     DIVX_FILEDESC_TYPE_SOCKET,
     DIVX_FILEDESC_TYPE_KEYPAD
};

/*********************************************************************************************
 *  Typedefs
 */

/*! Structure for file descriptors */
typedef struct _DivXFileDesc
{
    /*! File descriptor to select and wait on */
    int32_t                         descriptor;
    /*! Event to wait on */
    int32_t                         events;
    /*! Type of file descriptor */
    enum DivXFileDescType           type;
}
DivXFileDesc;

/*********************************************************************************************
 *  External facing APIs
 */

/*!
 * Select and waits until one of the specified objects is in the signaled state, or until timeout
 *
 * @param pFileDesc        (IN)     - Array of DivXFileDesc elements
 * @param fdCount          (IN)     - Count of DivXFileDesc elements
 * @param fdTimeout        (IN)     - Upper limit for wait, -ve implies infinite timeout
 *
 * @return DivXError (OUT)  - Returns an error code
 */
DivXError DivXSelect(
    const struct _DivXFileDesc *pFileDesc,
    int32_t                    fdCount,
    int32_t                    fdTimeout);

#ifdef __cplusplus
}
#endif


#endif
