/*!
@file DF3WriteInterface.h
@verbatim
$Id: DF3WriteInterface.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3WRITEINTERFACE_H_
#define _DF3WRITEINTERFACE_H_

#include "DivXError.h"
#include "DivXString.h"
#include "DivXMem.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFVariant.h"

typedef struct _DF3WriteInstanceData_t *DF3WriteInstanceHandle;

/*!
    Creates a container for Write.

    @param handle   (OUT) - Pointer handle to the container instance.
    @param filename (IN)  - Name of container to open for write.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_CreateContainer( DF3WriteInstanceHandle *handle,
                                  const DivXString  *filename,
                                  DivXMem            hMem );

/*!
    Opens an existing container for Edit.

    @param handle   (OUT) - Pointer handle to the container instance.
    @param filename (IN)  - Name of container to open for edit.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError DF3w_EditContainer( DF3WriteInstanceHandle *handle,
                                const DivXString  *filename,
                                DivXMem            hMem );

/*!
    Will close and free up the resources for the container module.

    @param handle (IN) - Container to close.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_INVALID_HANDLE
 */
DivXError DF3w_CloseContainer( DF3WriteInstanceHandle handle );

/*!
    Adds a title to the container.

    @param handle  (IN) - Handle to instance.
    @param TitleID (OUT) - Pointer to current count of title added.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_AddTitle( DF3WriteInstanceHandle handle,
                           int32_t          *TitleID );

DivXError   DF3w_SetTitle(DF3WriteInstanceHandle handle, 
                          int32_t           nTitle);


/*!
    Adds a menu to the container.  This is the actual menu chunk.

    @param handle (IN) - Handle to instance.
    @param headerLength (IN) - The number of bytes of the headerData.
    @param headerData (IN) - The actual header data.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_AddMenu( DF3WriteInstanceHandle handle,
                          int32_t           headerLength,
                          void             *headerData );

/*!
    Adds menu media to the container.  This is the actual MRIFs.

    @param handle (IN) - Handle to instance.
    @param menuMedia (OUT) - Pointer to the count of the added menu media.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_AddMenuMedia( DF3WriteInstanceHandle handle,
                               int32_t          *menuMedia );


/*!
    Adds a stream to the title.

    @param handle     (IN) - Handle to instance.
    @param blockType  (IN) - BlockType of the stream to add.
    @param data       (OUT) - Pointer to stream header data
    @param nStreamOfType (OUT) - Pointer to number of streams of blockType

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_AddStream( DF3WriteInstanceHandle handle,
                            DMFBlockType_t    blockType,
                            void             *data,
                            int32_t          *nStreamOfType );

/*!
    Adds a stream to the menu.

    @param ContainerHandle (IN) - Handle to instance.
    @param BlockType (IN) - BlockType of the stream to add.
    @param Data (IN) - Format of the stream.
    @param StreamHandle - (OUT) Pointer to number of streams of this blockType

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_AddMenuMediaStream( DF3WriteInstanceHandle ContainerHandle,
                                     DMFBlockType_t    BlockType,
                                     void             *Data,
                                     int32_t          *StreamHandle );

/*!
    Writes a block to the title.

    @param hContainer (IN) - Handle to instance
    @param blockType (IN) - BlockType of the stream.
    @param nStreamOfType (IN) - The stream number of type.
    @param pBlockNode (IN) - Pointer to the actual block data to write.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_WriteNextBlock( DF3WriteInstanceHandle hContainer,
                                 DMFBlockType_t    blockType,
                                 int32_t           nStreamOfType,
                                 DMFBlockNode_t   *pBlockNode );

/*!
    Writes a block to the mrif.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - BlockType of the stream.
    @param nStream (IN) - The stream number of type.
    @param pBlockNode (IN) - Pointer to the actual block data to write.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_WriteNextMenuMediaBlock( DF3WriteInstanceHandle hContainer,
                                          DMFBlockType_t    blockType,
                                          int32_t           nStream,
                                          DMFBlockNode_t   *pBlockNode );


/*!
    Called after a title has completed.  Signals container
    to finish up the title.

    @param handle (IN) - Handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_FinishTitle( DF3WriteInstanceHandle handle );


/*!
    Called after a MenuRiff has completed.  Signals container
    to finish up the MenuRiff.

    @param handle (IN) - Handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_FinishMenuRiff( DF3WriteInstanceHandle handle );

/*!
    Called after a MRIF has completed.  Signals container
    to finish up the MRIF.

    @param handle (IN) - Handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_FinishMRIF( DF3WriteInstanceHandle handle );

/*!
    Used to query the container for information.  All possible query values can be found in
    the L1QueryIds.h file.

    @param handle (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See L1QueryIds.h for all possible queries.
    @param inputValue   (OUT) - A pointer to the variant to receive the queried value.
    @param index (IN) - The index of the value to query.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_GetInfo( DF3WriteInstanceHandle handle,
                          int32_t           queryValueID,
                          DMFVariant       *inputValue,
                          int32_t           index );

/*!
    Sets info on the container.

    @param handle (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See L1QueryIds.h for all possible queries.
    @param inputValue (IP) - A input variant for the query.  This value is used only if the query needs information.
    @param index (IN) - The index of the value to query.
    @param outputValue (OUT) - A pointer to the variant to receive a value.

    @return The normal return code is DIVX_ERR_SUCCESS
 */
DivXError DF3w_SetInfo( DF3WriteInstanceHandle handle,
                          int32_t           queryValueID,
                          DMFVariant       *inputValue,
                          int32_t           index,
                          DMFVariant       *outputValue );



#endif /*_DF3WRITEINTERFACE_H_*/
