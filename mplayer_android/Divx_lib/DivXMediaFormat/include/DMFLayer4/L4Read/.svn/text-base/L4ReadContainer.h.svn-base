/*!
    @file
@verbatim
$Id: L4ReadContainer.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4READCONTAINER_H_
#define _L4READCONTAINER_H_

#include "DivXError.h"
#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "./L4Read/L4ReadTitle.h"
#include "./L3Read/Layer3ReadPublished.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _L4ReadContainerInst_t * L4ReadContainerHandle;

/**
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hMem    (IN) - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadContainer_New( L4ReadContainerHandle *pHandle, L3ResourcesHandle hL3Resources, DivXMem  hMem );

/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadContainer_Delete( L4ReadContainerHandle handle );

/*!
    Loads a container based on the file name

    @param handle   (IN) - A handle to instance.
    @param fileName (IN) - File name to be opened

    @return DivXError
*/
DivXError L4ReadContainer_LoadContainer( L4ReadContainerHandle handle, const DivXString *fileName );

/*!
    Closes a container

    @param handle   (IN) - A handle to instance.

    @return DivXError
*/
DivXError L4ReadContainer_CloseContainer( L4ReadContainerHandle handle );

/*!
    Returns the L3Resources handle as a void* for use elsewhere in the API 
    TODO EC - Change to a private header file (for use within the API)

    @param handle   (IN) - A handle to instance.

    @return DivXError
*/
void* L4ReadContainer_GetL3ResourcesHandle( L4ReadContainerHandle handle );

/*!
    Returns the L3Container handle as a void* for use elsewhere in the API 
    TODO EC - Change to a private header file (for use within the API)

    @param handle   (IN) - A handle to instance.

    @return DivXError
*/
void* L4ReadContainer_GetL3ContainerHandle( L4ReadContainerHandle handle );

/*!
    Returns the number of titles in this container

    @param handle         (IN) - A handle to instance.
    @param pnTitle       (OUT) - Number of titles

    @return DivXError
*/
DivXError L4ReadContainer_NumTitle( L4ReadContainerHandle handle, int32_t* pnTitle);

/*!
    Returns the currently active title

    @param handle         (IN) - A handle to instance.
    @param pCurTitle       (OUT) - Current active title

    @return DivXError
*/
DivXError L4ReadContainer_GetCurTitle(L4ReadContainerHandle handle, int32_t* pCurTitle);

/*!
    Opens the title based on nTitle and returns an L4ReadTitleHandle

    @param handle        (IN) - A handle to instance.
    @param nTitle        (IN) - Title to open
    @param hL4ReadTitle (OUT) - Returned handle to ReadTitle

    @return DivXError
*/
DivXError L4ReadContainer_OpenTitle( L4ReadContainerHandle handle, int32_t nTitle, L4ReadTitleHandle *hL4ReadTitle );


/*!
    Set the frame gap used when building an index

    @param L4ReadContainerHandle(IN) - A handle to instance.
    @param frameGap             (IN) - size of gap

    @return DivXError
*/
DivXError L4ReadContainer_SetFrameGap( L4ReadContainerHandle handle, int32_t frameGap );

/*!
    Set the index scheme to cacheless or normal parse

    @param L4ReadContainerHandle(IN) - A handle to instance.
    @param cachelessIndex             (IN) - use cachelss indexing is DIVX_TRUE

    @return DivXError
*/
DivXError L4ReadContainer_SetIndexScheme(L4ReadContainerHandle pInst, DivXBool cachelessIndex);

#ifdef __cplusplus
}
#endif

#endif //_L4READCONTAINER_H_
