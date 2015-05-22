/*!
    @file
@verbatim
$Id: L4ReadTitleState.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4READTITLESTATE_H_
#define _L4READTITLESTATE_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct L4ReadTitleState_t* L4ReadTitleStateHandle; /** The handle to L4ReadState. */

/**
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitleState_New( L4ReadTitleStateHandle *pHandle, DivXMem  hMem );

/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitleState_Delete( L4ReadTitleStateHandle handle );

/*!
    Activate streams

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXBool L4ReadTitleState_GetStreamTypeActive( L4ReadTitleStateHandle handle, DMFBlockType_t type);

/*!
    Activate streams

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
void L4ReadTitleState_SetStreamTypeActive( L4ReadTitleStateHandle handle, DMFBlockType_t type, DivXBool bActive);

/*!
    Activate streams

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
int32_t L4ReadTitleState_GetActiveStreamNum( L4ReadTitleStateHandle handle, DMFBlockType_t type );

/*!
    Activate streams

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitleState_SetActiveStreamNum( L4ReadTitleStateHandle handle, DMFBlockType_t type, int32_t nStream );

#ifdef __cplusplus
}
#endif

#endif //_L4READTITLESTATE_H_
