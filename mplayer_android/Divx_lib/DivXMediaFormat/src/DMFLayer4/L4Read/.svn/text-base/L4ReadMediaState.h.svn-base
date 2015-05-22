/*!
    @file
@verbatim
$Id: L4ReadMediaState.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4ReadMediaState_H_
#define _L4ReadMediaState_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"

typedef struct L4ReadMediaState_t* L4ReadMediaStateHandle; /** The handle to L4ReadState. */

#ifdef __cplusplus
extern "C" {
#endif

/*!
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return DivXError
*/
DivXError L4ReadMediaState_New( L4ReadMediaStateHandle *pHandle, DivXMem  hMem );

/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return DivXError
*/
DivXError L4ReadMediaState_Delete( L4ReadMediaStateHandle handle );

/*!
    Gets the active container, returns the index

    @param handle (IN) - A handle to instance.

    @return int32_t - Active container index
*/
int32_t L4ReadMediaState_GetActiveContainer( L4ReadMediaStateHandle handle );

/*!
    Sets the active container

    @param handle     (IN) - A handle to instance.
    @param nContainer (IN) - The index of the active container

    @return DivXError
*/
void L4ReadMediaState_SetActiveContainer( L4ReadMediaStateHandle handle, int32_t nContainer );

#ifdef __cplusplus
}
#endif

#endif //_L4ReadMediaState_H_
