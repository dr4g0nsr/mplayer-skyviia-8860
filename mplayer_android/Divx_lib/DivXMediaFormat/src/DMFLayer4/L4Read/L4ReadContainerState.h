/*!
    @file
@verbatim
$Id: L4ReadContainerState.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4READCONTAINERSTATE_H_
#define _L4READCONTAINERSTATE_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct L4ReadContainerStateInst_t* L4ReadContainerStateHandle; /** The handle to L4ReadState. */

/*!
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadContainerState_New( L4ReadContainerStateHandle *pHandle, DivXMem  hMem );

/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadContainerState_Delete( L4ReadContainerStateHandle handle );

/*!
    Retrieves the active title

    @param handle (IN) - A handle to instance.

    @return int32_t - Active title
*/
int32_t L4ReadContainerState_GetActiveTitle( L4ReadContainerStateHandle handle );

/**
    Sets the active title for the read container state

    @param handle     (IN) - A handle to instance.
    @param nContainer (IN) - Active title for the read container state

    @return DivXError
*/
DivXError L4ReadContainerState_SetActiveTitle( L4ReadContainerStateHandle handle, int32_t nContainer );

#ifdef __cplusplus
}
#endif

#endif //_L4READCONTAINERSTATE_H_
