/*!
    @file
@verbatim
$Id: MappingLayerWritePush.h 54060 2008-07-07 16:57:17Z jmurray $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MAPPINGLAYERWRIREPUSH_H_
#define _MAPPINGLAYERWRITEPUSH_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFContainerHandle.h"
#include "DMFModuleInitStruct.h"

#include "DMFTypes.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*!
 */
DivXError L2caw_PushInit( DMFModuleInitStruct *init );

/*!
    Creates a container for use in the push model.

    @param hContainer (OUT) - Receives the handle to the instance.
    @param moduleID (IN) - The ID of the module to use.  See DMFContainerModules.h for possible values.
    @param filename (IN) - Name of container to open.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushCreateContainer( DMFContainerHandle *hContainer,
                                     int32_t             moduleID,
                                     const DivXString   *filename,
                                     DivXMem             hMem );

/*!
    Closes the container.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushCloseContainer( DMFContainerHandle hContainer );

/*!
    Used to query the container for Metadata information.  All possible query values can be found in
    the DMFQueryIds.h file.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param value (OUT) - The variant to receive the queried value.
    @param index (IN) - The index of the value to query.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushGetInfo( DMFContainerHandle hContainer,
                             int32_t            queryValueID,
                             DMFVariant        *value,
                             int32_t            index );

/*!
    Sets info on the container.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param inputValue (IN) - A input variant for the query.  This value is used only if the query needs information.
    @param index (IN) - The index of the value to query.
    @param outputValue (OUT) - A variant to receive a value.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushSetInfo( DMFContainerHandle hContainer,
                             int32_t            queryValueID,
                             DMFVariant        *inputValue,
                             int32_t            index,
                             DMFVariant        *outputValue );

/*!
    Sets Metadata info on the container.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param inputValue (IN) - A input variant for the query.  This value is used only if the query needs information.
    @param index (IN) - The index of the value to query.
    @param outputValue (OUT) - A variant to receive a value.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_SetMetaInfo( DMFContainerHandle hContainer,
                             int32_t            queryValueID,
                             DMFVariant        *inputValue,
                             int32_t            index,
                             DMFVariant        *outputValue );

/*!
    Adds a title to the container.

    @param hContainer (IN) - Handle to instance.
    @param nTitle (OUT) - Current count of title added.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushAddTitle( DMFContainerHandle hContainer,
                              int32_t           *nTitle );

/*!
    Adds a stream to the title.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - BlockType of the stream to add.
    @param pFormat (IN) - Format of the stream.  Valid formats are DMFVideoStreamInfo1_t, DMFAudioStreamInfo1_t, DMFSubtitleStreamInfo1_t.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushAddStream( DMFContainerHandle hContainer,
                               DMFBlockType_t     blockType,
                               void              *pFormat );

/*!
    Adds a stream to the menu.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - BlockType of the stream to add.
    @param pFormat (IN) - Format of the stream.  Valid formats are DMFVideoStreamInfo1_t, DMFAudioStreamInfo1_t, DMFSubtitleStreamInfo1_t.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushAddMenuMediaStream( DMFContainerHandle hContainer,
                                        DMFBlockType_t     blockType,
                                        void              *pFormat );

/*!
    Pushes a block to a title.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - BlockType of the stream.
    @param nStreamOfType (IN) - The stream number of type.
    @param pBlockNode (IN) - The actual block data.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushWriteNextBlock( DMFContainerHandle hContainer,
                                    DMFBlockType_t     blockType,
                                    int32_t            nStreamOfType,
                                    DMFBlockNode_t    *pBlockNode );

/*!
    Adds a menu to the container.  This is the actual menu chunk.

    @param hContainer (IN) - Handle to instance.
    @param headerLength (IN) - The number of bytes of the headerData.
    @param headerData (IN) - The actual header data.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushAddMenu( DMFContainerHandle hContainer,
                             int32_t            headerLength,
                             void              *headerData );

/*!
    Adds menu media to the container.  This is the actual MRIFs.

    @param hContainer (IN) - Handle to instance.
    @param nMenuMedia (OUT) - The count of the added menu media.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushAddMenuMedia( DMFContainerHandle hContainer,
                                  int32_t           *nMenuMedia );

/*!
    Pushes a block to the mrif.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - BlockType of the stream.
    @param nStreamOfType (IN) - The stream number of type.
    @param pBlockNode (IN) - The actual block data.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushWriteNextMenuMediaBlock( DMFContainerHandle hContainer,
                                             DMFBlockType_t     blockType,
                                             int32_t            nStreamOfType,
                                             DMFBlockNode_t    *pBlockNode );

/*!
    Call to notify that the container that it is about to receive
    blocks.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushInitContainer( DMFContainerHandle hContainer );

/*!
    Call to initialize a title.  Called before pushing to a
    title.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushInitTitle( DMFContainerHandle hContainer );

/*!
    Call to initialize a menu.  Called before pushing to a
    menu.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushInitMenu( DMFContainerHandle hContainer );

/*!
    Asks the container what type Title or Menu that it would
    like to receive next.  Should be called to determine what
    to push to the container next.

    @param hContainer (IN) - Handle to instance.
    @param neededType (IN/OUT) - Pass in the current type. Returns the needed type by the system.
    @param numOfType (IN) - The needed number of the type.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushGetNextNeededType( DMFContainerHandle hContainer,
                                       DMFTypes_t        *neededType,
                                       int32_t           *numOfType );

/*!
    Signals the container that there are no more of what it is requesting.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushNoMoreNeededType( DMFContainerHandle hContainer );

/*!
    Resets the GetNextNeededType.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushResetNeededType( DMFContainerHandle hContainer );

/*!
    Asks the container what the next needed block is.  Should be
    called to determine what to push to the container next.

    @param hContainer (IN) - Handle to instance.
    @param pBlockType (OUT) - The needed block type.
    @param pStreamNumOfType (OUT) - The needed number of the block type. (Zero based)

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushGetNextNeededBlock( DMFContainerHandle hContainer,
                                        DMFBlockType_t    *pBlockType,
                                        int32_t           *pStreamNumOfType );

/*!
    Called to signal the container that there are no more blocks for the stream.  This
    will stop the container requesting blocks for this stream.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - The block type of the stream.
    @param nStreamOfType (IN) - The stream number of the type. (Zero based)

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushEndOfStream( DMFContainerHandle hContainer,
                                 DMFBlockType_t     blockType,
                                 int32_t            nStreamOfType );

/*!
    Called after every loop.  This is call will actually write the block to the container if ready.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushWritePass( DMFContainerHandle hContainer );

/*!
    Called after a title has completed.  Signals container
    to finish up the title.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushTitleFinished( DMFContainerHandle hContainer );

/*!
    Called after a menu has completed.  Signals container
    to finish up the menu.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PushMenuFinished( DMFContainerHandle hContainer );

DivXError L2caw_SetTitle( DMFContainerHandle hContainer, int32_t nTitle );
#ifdef __cplusplus
}
#endif

#endif /* _MAPPINGLAYERWRITEPUSH_H_ */
