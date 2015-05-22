/*!
    @file
@verbatim
$Id: L4ReadMedia.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4READMEDIA_H_
#define _L4READMEDIA_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "L4ReadContainer.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct L4ReadMediaInst_t* L4ReadMediaHandle;

//typedef struct _L4ReadMediaInst_t * L4ReadMediaHandle;
//typedef void * L4ReadMediaHandle; /** The handle to L4ReadState. */

/*!
    Retrieve the resource handle

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return The normal return code is L1_ERR_SUCCESS
*/
L3ResourcesHandle L4ReadMedia_GetL3ResourcesHandle( L4ReadMediaHandle handle );

/**
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return The normal return code is L1_ERR_SUCCESS
*/
DivXError L4ReadMedia_New( L4ReadMediaHandle *pHandle, DivXMem  hMem );

/**
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is L1_ERR_SUCCESS
*/
DivXError L4ReadMedia_Delete( L4ReadMediaHandle handle );


//DivXError L4ReadMedia_LoadContainer( L4ReadMediaHandle handle, const DivXString *fileName );

//DivXError L4ReadMedia_SetTitle( L4ReadHandle handle, int32_t nTitle, L4ReadTitleHandle *phReadTitle );

/**
    Loads media

    @param handle   (IN) - A handle to instance.
    @param fileName (IN) - File to load

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadMedia_LoadMedia( L4ReadMediaHandle handle, const DivXString *fileName );

/**
    Opens a container

    @param handle   (IN) - A handle to instance.
    @param fileName (IN) - File to load

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadMedia_OpenContainer( L4ReadMediaHandle handle, int32_t nContainer, L4ReadContainerHandle *phReadContainer );


#ifdef __cplusplus
}
#endif

#endif //_L4READMEDIA_H_
