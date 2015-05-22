/*!
    @file
@verbatim
$Id: L1DF3ReadEntry.h 60057 2009-05-09 23:20:24Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1DF3READENTRY_H_
#define _L1DF3READENTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXMem.h"
#include "DivXString.h"
#include "DMFModuleInfo.h"
#include "./DF3/DF3Read/L1DF3ReadEntry.h"
#include "./DF3/DF3Read/DF3Interface.h"

/*!
    Initialize for read.

    @param data (IN) - Pointer to a DMFModuleInitStruct struct containing init settings.

    @return The normal return code is 0 for success.
 */
int L1DF3r_Init( const void *data );

/*! Retrieves module info for given Module

    @param ModuleID (IN) - Unique module ID
    @return pointer to DMFModuleInfo associated with ModuleID
    @retval NULL if Module could be found
*/
DMFModuleInfo *L1DF3r_GetDMFModuleInfo( int32_t ModuleID );

/*! Retrieves the DMFModuleAttributes from module

   @return DMFModuleAttributes (OUT) - Returns the DMFModuleAttributes struct
                                       describing modules capabilities
 */
DMFModuleAttributes *L1DF3r_GetCaps( void );

/*! Checks if the container is supported

   @param SupportLevel (OUT) - Pointer to support level of the container
   @param Filename     (IN)  - File name of the container to open

   @return DivXError   (OUT) - See DivXError.h for more details
 */
DivXError L1DF3r_IsContainerSupported( int32_t          *SupportLevel,
                                       const DivXString *Filename );

/*!
    Initialize for read.

    @param data (IN) - Pointer to a _DMFModuleInitStruct struct containing init settings.

    @return DivXError   (OUT) - See DivXError.h for more details
 */
int L1DF3r_Init( const void *data );


/*!
    Retrieve module info

    @return DMFModuleInfo struct containing modules read attributes
*/
DMFModuleInfo *L1DF3r_GetDMFModuleInfo( int32_t ModuleID );

/*!
    Retrieve modules attributes

    @return DMFModuleAttributes struct containing modules read attributes
*/
DMFModuleAttributes *L1DF3r_GetCaps( void );

/*! Checks if the container is supported

   @param SupportLevel (OUT) - Pointer to support level of the container
   @param Filename     (IN)  - File name of the container to open

   @return DivXError   (OUT) - See DivXError.h for more details
 */
DivXError L1DF3r_IsContainerSupported( int32_t          *SupportLevel,
                                       const DivXString *Filename );

/*!
    Will open supported containers and create instance of the module.

    @param ContainerHandle (IN/OUT) - A pointer to a DF2ReadInstanceHandle that will receive module handle.
    @param filename   (IN) - The name of the file to open.
    @param hMem       (IN) - Handle to a DivXMem module.  If NULL will use standard malloc and free.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_OpenContainer( DF3Interface_h *ContainerHandle,
                                const DivXString  *filename,
                                DivXMem            hMem );

/*!
    Will close and free up the resources for the container module.

    @param hContainer (IN) - Container to close.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_CloseContainer( DF3Interface_h hContainer );

/*!
    Will set the current title.  This action will preform a minimal
    parse of the title being set.

    @param hContainer (IN) - Container to operate on.
    @param TitleIndex (IN) - Index of the title to set, zero based.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_SetTitle( DF3Interface_h hContainer,
                           int32_t           TitleIndex );

/*!
    Queries the module for information.  See DMFQueryIDs.h for possible
    query IDs.

    @param hContainer   (IN) - handle to container to operate on
    @param QueryValueID (IN) - Determines the property to be retrieved
    @param Value        (IN)/(OUT) - pointer to multi type input/output
    @param Index        (IN) - used to identifiy a particular stream

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
*/
DivXError L1DF3r_GetInfo( DF3Interface_h hContainer,
                          int32_t           QueryValueID,
                          DMFVariant       *Value,
                          int32_t           Index );

/*!
    Sets information on the module.  See DMFQueryIDs.h for possible
    query IDs.

    @param hContainer   (IN) - Container to operate on
    @param QueryValueID (IN) - Determines the property to be set
    @param InputValue   (IN)/(OUT) - pointer to multi type input/output param
    @param Index        (IN) - used to identifiy a particular stream
    @param OutputValue  (OUT) - pointer ot multi type output

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
*/
DivXError L1DF3r_SetInfo( DF3Interface_h hContainer,
                          int32_t           QueryValueID,
                          DMFVariant       *InputValue,
                          int32_t           Index,
                          DMFVariant       *OutputValue );

/*!
    Will set the position for the specified stream by time.  Only seeks a single 
    stream, to maintain sync must seek all streams.

    @param hContainer (IN) - Container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pTime      (IN/OUT) - Pointer to the time to seek to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_SetPosition( DF3Interface_h hContainer,
                              DMFBlockType_t    blockType,
                              int32_t           nStream,
                              DivXTime         *pTime );

/*!
    Reads the next block for a specific stream.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_ReadNextBlockByStream( DF3Interface_h hContainer,
                                        DMFBlockType_t    blockType,
                                        int32_t           nStream,
                                        DMFBlockNode_t   *pBlockNode );

/*!
    Sets a stream active so that it will be returned through the ReadNextBlock function call.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param bActive    (IN) - DIVX_TRUE sets the stream to be read DIVX_FALSE sets the stream to not be read.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.
 */
DivXError L1DF3r_SetTitleActiveStream( DF3Interface_h hContainer,
                                       DMFBlockType_t    blockType,
                                       int32_t           nStream,
                                       DivXBool          bActive );

/*!
    Reads the next block from the current title.  Only streams that have been set active
    through the L1DF3r_SetTitleActiveStream will be read.

    @param hContainer (IN)  - Handle to container to operate on.
    @param pBlockType (OUT) - Pointer to the type of the block read.
    @param pnStream   (OUT) - Pointer to number of the stream read.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.
    @param userData   (IN) - Reserved.

    @return The normal return code is DIVX_ERR_SUCCESS or error code from DMFErrors.h.

    @note If no stream have been set active through the L3DF3r_SetTitleActiveStream() function
    then DMF_ERR_END_OF_STREAM will be returned on the first call to ReadNextBlock.
 */
DivXError L1DF3r_ReadNextBlock( DF3Interface_h hContainer,
                                DMFBlockType_t   *pBlockType,
                                uint32_t         *pnStream,
                                DMFBlockNode_t   *pBlockNode,
                                void             *userData );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
*/
DivXError L1DF3r_SetMenu( DF3Interface_h ContainerHandle,
                          int32_t           mnuIndex );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
 */
DivXError L1DF3r_SetMenuMedia( DF3Interface_h hContainer,
                               int32_t           menuMedia );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
 */
DivXError L1DF3r_SetMenuMediaBlock( DF3Interface_h hContainer,
                                    DMFBlockType_t    blockType,
                                    int32_t           nStream,
                                    DivXTime         *pTime );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
 */
DivXError L1DF3r_ReadNextMenuMediaBlockByStream( DF3Interface_h hContainer,
                                                 DMFBlockType_t    blockType,
                                                 int32_t           nStream,
                                                 DMFBlockNode_t   *pBlockNode );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
 */
DivXError L1DF3r_SetMenuActiveStream( DF3Interface_h hContainer,
                                      DMFBlockType_t    blockType,
                                      int32_t           nStream,
                                      DivXBool          bActive );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
 */
DivXError L1DF3r_ReadNextMenuMediaBlock( DF3Interface_h hContainer,
                                         DMFBlockType_t   *pBlockType,
                                         uint32_t         *pnStream,
                                         DMFBlockNode_t   *pBlockNode,
                                         void             *userData );

#ifdef __cplusplus
}
#endif
#endif /* _L1DF3READENTRY_H_ */
