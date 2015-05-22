/*!
    @file
@verbatim
$Id: L1DF2Read.h 60057 2009-05-09 23:20:24Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1DF2READ_H_
#define _L1DF2READ_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockNode.h"
#include "DMFBlockType.h"
#include "L1ContainerHandle.h"

#include "DMFErrors.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __SYMBIAN32__
    /*!
    Opens container with previously opened file provided by platform specific descriptor
    Will open supported containers and create instance of the module.

    @param ContainerHandle (IN/OUT) - A pointer to a L1ContainerHandle that will receive module handle.
    @param filename   (IN) - The name of the file to open.
    @param hMem       (IN) - Handle to a DivXMem module.  If NULL will use standard malloc and free.

    @return The normal return code is L1_ERR_SUCCESS or error code.
    @retval L1_ERR_OPEN_FAILURE
    @retval L1_ERR_INSUFFICIENT_MEM
    @retval L1_ERR_READ_FAILURE
    @retval L1_ERR_INVALID_HANDLE
    */
DivXError L1DF2r_OpenContainerForFile( L1ContainerHandle *ContainerHandle,
									   PDivXFileExternalObj file,
                                       DivXMem            hMem );
#endif // __SYMBIAN32__


typedef struct _DF2ReadInstanceData *DF2ReadInstanceHandle; 

/*!
    Will open supported containers and create instance of the module.

    @param ContainerHandle (IN/OUT) - A pointer to a DF2ReadInstanceHandle that will receive module handle.
    @param filename   (IN) - The name of the file to open.
    @param hMem       (IN) - Handle to a DivXMem module.  If NULL will use standard malloc and free.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DIVX_ERR_OPEN_FAILURE
    @retval DIVX_ERR_INSUFFICIENT_MEM
    @retval DIVX_ERR_READ_FAILURE
    @retval DMF_ERR_INVALID_HANDLE
 */
DivXError L1DF2r_OpenContainer( DF2ReadInstanceHandle *ContainerHandle,
                                const DivXString  *filename,
                                DivXMem            hMem );

/*!
    Will close and free up the resources for the container module.

    @param hContainer (IN) - Container to close.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
 */
DivXError L1DF2r_CloseContainer( DF2ReadInstanceHandle hContainer );

/*!
    Will set the current title.  This action will preform a minimal
    parse of the title being set.

    @param hContainer (IN) - Container to operate on.
    @param TitleIndex (IN) - Index of the title to set, zero based.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DMF_ERR_SCAN_RIFF_FAILURE
 */
DivXError L1DF2r_SetTitle( DF2ReadInstanceHandle hContainer,
                           int32_t           TitleIndex );

/*!
    Queries the module for information.  See DMFQueryIDs.h for possible
    query IDs.

    @param hContainer   (IN) - handle to container to operate on
    @param QueryValueID (IN) - Determines the property to be retrieved
    @param Value        (IN)/(OUT) - pointer to multi type input/output
    @param Index        (IN) - used to identifiy a particular stream

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
*/
DivXError L1DF2r_GetInfo( DF2ReadInstanceHandle hContainer,
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
DivXError L1DF2r_SetInfo( DF2ReadInstanceHandle hContainer,
                          int32_t           QueryValueID,
                          DMFVariant       *InputValue,
                          int32_t           Index,
                          DMFVariant       *OutputValue );

/*!
    Will set the position for the specified stream by time.

    @param hContainer (IN) - Container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pTime      (IN/OUT) - Pointer to the time to seek to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INDEX_OUT_OF_RANGE
    @retval DMF_ERR_END_OF_STREAM
 */
DivXError L1DF2r_SetPosition( DF2ReadInstanceHandle hContainer,
                              DMFBlockType_t    blockType,
                              int32_t           nStream,
                              DivXTime         *pTime );

/*!
    Reads the next block for a specific stream.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DMF_ERR_END_OF_STREAM
    @retval DMF_ERR_BLOCK_READ_FAILURE
    @retval DMF_ERR_BLOCK_BUFFER_TO_SMALL
 */
DivXError L1DF2r_ReadNextBlockByStream( DF2ReadInstanceHandle hContainer,
                                        DMFBlockType_t    blockType,
                                        int32_t           nStream,
                                        DMFBlockNode_t   *pBlockNode );

/*!
    Sets a stream active so that it will be returned through the ReadNextBlock function call.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param bActive    (IN) - DIVX_TRUE sets the stream to be read DIVX_FALSE sets the stream to not be read.

    @return The normal return code is AVI1R_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DMF_ERR_INVALID_STREAM
    @retval DIVX_ERR_SUCCESS
 */
DivXError L1DF2r_SetTitleActiveStream( DF2ReadInstanceHandle hContainer,
                                       DMFBlockType_t    blockType,
                                       int32_t           nStream,
                                       DivXBool          bActive );

/*!
    Reads the next block from the current title.  Only streams that have been set active
    through the L1avir_SetTitleActiveStream will be read.

    @param hContainer (IN)  - Handle to container to operate on.
    @param pBlockType (OUT) - Pointer to the type of the block read.
    @param pnStream   (OUT) - Pointer to number of the stream read.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.
    @param userData   (IN) - Reserved.

    @return The normal return code is AVI1R_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DIVX_ERR_INVALID_ARG
    @retval DMF_ERR_END_OF_STREAM
    @retval DMF_ERR_BLOCK_READ_FAILURE
    @retval DMF_ERR_BLOCK_BUFFER_TOO_SMALL

    @note If no stream have been set active through the L1avir_SetTitleActiveStream() function
    then DMF_ERR_END_OF_STREAM will be returned on the first call to ReadNextBlock.
 */
DivXError L1DF2r_ReadNextBlock( DF2ReadInstanceHandle hContainer,
                                DMFBlockType_t   *pBlockType,
                                uint32_t         *pnStream,
                                DMFBlockNode_t   *pBlockNode,
                                void             *userData );

/*!
    This function is unused
    @return The normal return code is DMF_ERR_MENU_MODULE_NULL
*/
DivXError L1DF2r_SetMenu( DF2ReadInstanceHandle ContainerHandle,
                          int32_t           mnuIndex );

/*!
    Will set the current menu media.  This action will preform a minimal
    parse of the menu media being set.

    @param hContainer     (IN) -  Handle to container to operate on.
    @param menuMedia      (IN)  - Index of the menu media to set, zero based.

    @return The normal return code is AVI1R_ERR_SUCCESS or error code.
    @retval DMF_ERR_INDEX_OUT_OF_RANGE
    @retval DMF_ERR_END_OF_STREAM
    @retval DMF_ERR_SET_BLOCK_FAILED
 */
DivXError L1DF2r_SetMenuMedia( DF2ReadInstanceHandle hContainer,
                               int32_t           menuMedia );

/*!
    Will set the position for the specified stream by time.

    @param hContainer (IN) - Container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pTime      (IN/OUT) - Pointer to the time to seek to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INDEX_OUT_OF_RANGE
    @retval DMF_ERR_END_OF_STREAM
 */
DivXError L1DF2r_SetMenuMediaBlock( DF2ReadInstanceHandle hContainer,
                                    DMFBlockType_t    blockType,
                                    int32_t           nStream,
                                    DivXTime         *pTime );

/*!
    Reads the next block for a specific stream.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DMF_ERR_END_OF_STREAM
    @retval DMF_ERR_BLOCK_READ_FAILURE
    @retval DMF_ERR_BLOCK_BUFFER_TO_SMALL
 */
DivXError L1DF2r_ReadNextMenuMediaBlockByStream( DF2ReadInstanceHandle hContainer,
                                                 DMFBlockType_t    blockType,
                                                 int32_t           nStream,
                                                 DMFBlockNode_t   *pBlockNode );

/*!
    Sets a stream active so that it will be returned through the ReadNextMenuMediaBlock function call.

    @param hContainer (IN) - Handle to container to operate on.
    @param blockType  (IN) - The block type for the stream.
    @param nStream    (IN) - The index of the stream by type, zero based.
    @param bActive    (IN) - DIVX_TRUE sets the stream to be read DIVX_FALSE sets the stream to not be read.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DMF_ERR_INVALID_STREAM
 */
DivXError L1DF2r_SetMenuActiveStream( DF2ReadInstanceHandle hContainer,
                                      DMFBlockType_t    blockType,
                                      int32_t           nStream,
                                      DivXBool          bActive );

/*!
    Reads the next block from the current title.  Only streams that have been set active
    through the L1avir_SetTitleActiveStream will be read.

    @param hContainer (IN) - Handle to container to operate on.
    @param pBlockType (OUT) - Pointer to the type of the block read.
    @param pnStream   (OUT) - Pointer to the number of the stream read.
    @param pBlockNode (IN/OUT) - Pointer to a block node that contains allocated buffers to copy data to.
    @param userData (IN) - Reserved.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
    @retval DIVX_ERR_INVALID_ARG
    @retval DMF_ERR_END_OF_STREAM
    @retval DMF_ERR_BLOCK_READ_FAILURE
    @retval DMF_ERR_BLOCK_BUFFER_TO_SMALL

    @note If no stream have been set active through the L1avir_SetMenuActiveStream() function
    then DMF_ERR_END_OF_STREAM will be returned on the first call to ReadNextMenuMediaBlock.
 */
DivXError L1DF2r_ReadNextMenuMediaBlock( DF2ReadInstanceHandle hContainer,
                                         DMFBlockType_t   *pBlockType,
                                         uint32_t         *pnStream,
                                         DMFBlockNode_t   *pBlockNode,
                                         void             *userData );

#ifdef __cplusplus
}
#endif
#endif /* _L1DF2READ_H_ */
