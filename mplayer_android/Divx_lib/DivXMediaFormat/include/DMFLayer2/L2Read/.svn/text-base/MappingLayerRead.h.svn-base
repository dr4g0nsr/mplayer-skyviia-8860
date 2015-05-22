/*!
    @file
@verbatim
$Id: MappingLayerRead.h 60057 2009-05-09 23:20:24Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

/** @defgroup LAYER2 DMF Layer 2 API
 *  Description of DivX Media Format API (DMF)
 *  @{
 */

#ifndef _MAPPINGLAYERREAD_H_
#define _MAPPINGLAYERREAD_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockNode.h"
#include "DivXFileStruct.h"
#include "DMFBlockType.h"
#include "DMFContainerHandle.h"
#include "DMFModuleInitStruct.h"
#include "DMFErrors.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
    Initializes the module.  This must be called before all other
    functions.  Passing in NULL will load default modules and is
    recommended unless a special use case is required.

    @param init - Modules to load if NULL defaults are used.

    @return The normal return code is DIVX_ERR_SUCCESS or error code.
    @retval DMF_ERR_LAYER1_MODULE_INIT

 */
DivXError L2car_Init( DMFModuleInitStruct *init );

//#ifdef __SYMBIAN32__
/*!
    Creates a container for opened file for reading. This will check with all loaded modules
    to determine the best one to use for the for the file.

    @param hContainer (OUT) - Receives the handle to the instance.
    @param file (IN) - Handle to an opened file to be used in container.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_CreateContainerForFile( DMFContainerHandle *phContainer,
					PDivXFileExternalObj file,
					DivXMem 			 hMem);
//#endif // __SYMBIAN32__

/*!
    Opens a container for reading.  This will check with all loaded modules
    to determine the best one to use for the for the file.

    @param hContainer (OUT) - Receives the handle to the instance.
    @param filename (IN) - Name of container to open.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_OpenContainer( DMFContainerHandle *hContainer,
                               const DivXString   *filename,
                               DivXMem             hMem );

/*!
    Opens a container for reading.  This will use the module specified by the
    moduleID parameter.

    @param hContainer (OUT) - Receives the handle to the instance.
    @param moduleID (IN) - The ID of the module to use.  See L1Modules.h for possible values.
    @param filename (IN) - Name of container to open.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_OpenContainerModule( DMFContainerHandle *hContainer,
                                     int32_t             moduleID,
                                     const DivXString   *filename,
                                     DivXMem             hMem );

/*!
    Closes the container.

    @param hContainer (IN) - Handle to instance.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_CloseContainer( DMFContainerHandle hContainer );

/*!
    Used to query the container for information.  All possible query values can be found in
    the DMFQueryIds.h file.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param value (OUT) - The variant to receive the queried value.
    @param index (IN) - The index of the value to query.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_GetInfo( DMFContainerHandle hContainer,
                         int32_t            queryValueID,
                         DMFVariant        *value,
                         int32_t            index );

/*!
    Used to set the container for information.  All possible query values can be found in
    the DMFQueryIds.h file.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param inputValue (IN) - A input variant for the query.  This value is used only if the query needs information.
    @param index (IN) - The index of the value to query.
    @param outputValue (OUT) - A variant to receive a value.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetInfo( DMFContainerHandle hContainer,
                         int32_t            queryValueID,
                         DMFVariant        *inputValue,
                         int32_t            index,
                         DMFVariant        *outputValue );

/*!
    Used to query the container for Metadata information.  All possible query values can be found in
    the DMFQueryIds.h file.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param value (OUT) - The variant to receive the queried value.
    @param index (IN) - The index of the value to query.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_GetMetaInfo( DMFContainerHandle hContainer,
                             int32_t            queryValueID,
                             DMFVariant        *value,
                             int32_t            index );

/*!
    Sets the current title to read from.

    @param hContainer (IN) - Handle to instance.
    @param nTitle (IN) - Number of the title to set, zero based.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetTitle( DMFContainerHandle hContainer,
                          int32_t            nTitle );

/*!
    Sets a stream active to be returned by the L2car_ReadNextMenuMediaBlock function call.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Block type of the stream to set active.
    @param nStreamOfType (IN) - The stream number of blockType to set active. (zero based)
    @param bActive (IN) - DIVX_TRUE: sets stream to read from. DIVX_FALSE: disables stream for reading.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetTitleActiveStream( DMFContainerHandle hContainer,
                                      DMFBlockType_t     blockType,
                                      int32_t            nStreamOfType,
                                      DivXBool           bActive );

/*!
    Reads a block from the file.  Will read only from streams that have been set active by the
    L2car_SetMenuActiveStream.

    @param hContainer (IN) - Handle to instance.
    @param pBlockType (OUT) - Read block type.
    @param pnStream (OUT) - Read stream number of type.
    @param pBlockNode (OUT) - Block node to read data into.
    @param userData (IN) - Reserved

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_ReadNextBlock( DMFContainerHandle hContainer,
                               DMFBlockType_t    *pBlockType,
                               uint32_t          *pnStream,
                               DMFBlockNode_t    *pBlockNode,
                               void              *userData );

/*!
    Reads a block from the current title for a certain stream.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Type of the block to read.
    @param nStreamOfType (IN) - The number of the stream to read.
    @param pBlockNode (IN) - BlockNode to read the data into.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)

    @note Streeam does not have to be active for this to be used.
 */
DivXError L2car_ReadNextBlockByStream( DMFContainerHandle hContainer,
                                       DMFBlockType_t     blockType,
                                       int32_t            nStreamOfType,
                                       DMFBlockNode_t    *pBlockNode );

/*DivXError L2car_ReadNextBlockByStream( DMFContainerHandle hContainer,
                                       DMFBlockType_t     blockType,
                                       int32_t            nStreamOfType,
                                       DMFBlockNode_t    *pBlockNode ){};*/

/*!
    Sets the current block position.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Type of the block to read.
    @param nStreamOfType (IN) - The number of the stream to read.
    @param pTime (IN/OUT) - The time of the block to seek to.  Returns the actual time seeked to.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetPosition( DMFContainerHandle hContainer,
                             DMFBlockType_t     blockType,
                             int32_t            nStreamOfType,
                             DivXTime          *pTime );

/*!
    Sets the current menu.

    @param hContainer (IN) - Handle to instance.
    @param nMenu (IN) - The number of the menu to seek to.  (Zero based)

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetMenu( DMFContainerHandle hContainer,
                         int32_t            nMenu );

/*!
    Retreives the actual menu chunk.

    @param hContainer (IN) - Handle to instance.
    @param bufferSize (IN) - Size of the buffer.
    @param buffer    (IN) - The actual buffer to receive the menu chunk.
    @param bytesRead (OUT) - The actual amount of data read into the buffer.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_GetMenu( DMFContainerHandle hContainer,
                         int32_t            bufferSize,
                         void              *buffer,
                         int32_t           *bytesRead );

/*!
    Sets the current menu media.  This must be done to read blocks from a menu.

    @param hContainer (IN) - Handle to instance.
    @param nMenuMedia (IN) - Sets the current menu media.  (Zero based)

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetMenuMedia( DMFContainerHandle hContainer,
                              int32_t            nMenuMedia );

/*!
    Sets the current block inside the current menu media.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Type of the block to read.
    @param nStreamOfType (IN) - The number of the stream to read.
    @param pTime (IN/OUT) - The time of the block to seek to.  Returns the actual time seeked to.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)

 */
DivXError L2car_SetMenuMediaPosition( DMFContainerHandle hContainer,
                                      DMFBlockType_t     blockType,
                                      int32_t            nStreamOfType,
                                      DivXTime          *pTime );

/*!
    Sets a stream active to be returned by the L2car_ReadNextMenuMediaBlock function call.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Block type of the stream to set active.
    @param nStreamOfType (IN) - The stream number of blockType to set active. (zero based)
    @param bActive (IN) - DIVX_TRUE: sets stream to read from. DIVX_FALSE: disables stream for reading.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_SetMenuActiveStream( DMFContainerHandle hContainer,
                                     DMFBlockType_t     blockType,
                                     int32_t            nStreamOfType,
                                     DivXBool           bActive );

/*!
    Reads a block from the file.  Will read only from streams that have been set active by the
    L2car_SetMenuActiveStream.

    @param hContainer (IN) - Handle to instance.
    @param pBlockType (OUT) - Read block type.
    @param pnStream (OUT) - Read stream number of type.
    @param pBlockNode (OUT) - Block node to read data into.
    @param userData (IN) - Reserved

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L2car_ReadNextMenuMediaBlock( DMFContainerHandle hContainer,
                                        DMFBlockType_t    *pBlockType,
                                        uint32_t          *pnStream,
                                        DMFBlockNode_t    *pBlockNode,
                                        void              *userData );

/*!
    Reads a block from the current menu media.

    @param hContainer (IN) - Handle to instance.
    @param blockType (IN) - Type of the block to read.
    @param nStreamOfType (IN) - The number of the stream to read.
    @param pBlockNode (IN) - BlockNode to read the data into.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)

    @note Streeam does not have to be active for this to be used.
 */
DivXError L2car_ReadNextMenuMediaBlockByStream( DMFContainerHandle hContainer,
                                                DMFBlockType_t     blockType,
                                                int32_t            nStreamOfType,
                                                DMFBlockNode_t    *pBlockNode );

#ifdef __cplusplus
}
#endif

#endif /* MAPPINGLAYERREAD */
/**  @}  */
