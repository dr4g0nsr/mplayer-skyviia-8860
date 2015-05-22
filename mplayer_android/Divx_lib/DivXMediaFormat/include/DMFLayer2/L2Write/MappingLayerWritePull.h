/*!
    @file
@verbatim
$Id: MappingLayerWritePull.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MAPPINGLAYERWRITEPULL_H_
#define _MAPPINGLAYERWRITEPULL_H_

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
    Inits the layer for writing.

    @param init (IN) - Modules to load if NULL defaults are used.
 */
DivXError L2caw_PullInit( DMFModuleInitStruct *init );

/*!
    Creates a container for use in the Pull Model.

    @param hContainer (OUT) - Receives the handle to the instance.
    @param ModuleID (IN) - ID to the container to create.
    @param filename (IN) - Name of container to open.
    @param hMem (IN) - Handle to a DivXMem instance.  NULL will map to standard malloc/free.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullCreateContainer( DMFContainerHandle *hContainer,
                                     int32_t             ModuleID,
                                     const DivXString   *filename,
                                     DivXMem             hMem );

/*!
    Closes a container that is used in the pull model.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullCloseContainer( DMFContainerHandle hContainer );

/*!
    Callback is called to Query for info.

    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param value (OUT) - The variant to receive the queried value.
    @param index (IN) - The index of the value to query.
    @param privateGetInfoData (IN) - Private data for the callback.  Gets set when the callback is registered.

    @return The normal return code is 0, representing success or error code.
 */
typedef DivXError ( *fnPullGetInfo )( int32_t QueryValueID, DMFVariant *value, int32_t index,
                                      void *privateGetInfoData );

/*!
    Register GetInfo call back with the DMFMapping Layer.  This gives the ability for the Mapping Layer to query
    for info about the container.

    Most cases used to query data from the High Level Model.

    @param hContainer (IN) - Handle to instance.
    @param getInfo (IN) - Pointer to the GetInfo function.
    @param privateGetInfoData (IN) - Private data for the callback.  Will get passed into the GetInfo callback.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullRegisterGetInfoCallback( DMFContainerHandle hContainer,
                                             fnPullGetInfo      getInfo,
                                             void              *privateGetInfoData );

/*!
    Callback is called to Set info.

    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param value (OUT) - The variant to receive the queried value.
    @param index (IN) - The index of the value to query.
    @param privateGetInfoData (IN) - Private data for the callback.  Gets set when the callback is registered.

    @return The normal return code is 0, representing success or error code.
 */
typedef DivXError ( *fnPullSetInfo )( int32_t QueryValueID, DMFVariant *Value, int32_t Index,
                                      void *privateSetInfoData );

/*!
    Register SetInfo call back with the DMFMapping Layer.  This gives the ability for the Mapping Layer to set
    info on the container.

    Most cases used to build a High Level model from a container object.

    @param hContainer (IN) - Handle to instance.
    @param setInfo (IN) - Pointer to the SetInfo function.
    @param privateGetInfoData (IN) - Private data for the callback.  Will get passed into the SetInfo callback.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullRegisterSetInfoCallback( DMFContainerHandle hContainer,
                                             fnPullSetInfo      setInfo,
                                             void              *privateSetInfoData );

/*!
    Signals the system to stop writing the container.

    @param hContainer (IN) - Handle to instance.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullSignalInterupt( DMFContainerHandle hContainer );

/*!
    Call back is called on initializations of each new DMFType.

    @param type (IN) - The type that is being intialized.
    @param nType (IN) - The number of the type that is being intialized.
    @param pPrivateData (IN) - The private data for the callback. Gets registered in the L2caw_PullWriteContainer.

    @return The normal return code is 0, representing success or error code.
 */
typedef DivXError ( *fnPullInitCallback )( DMFTypes_t type, int32_t nType, void *pPrivateData );

/*!
    Call back is called to get data from input sources (video blocks, audio blocks, menu chunks)

    @param blockType (IN) - The block type that is being requested.
    @param nStreamOfBlockType (IN) - The stream number of the requested block type.
    @param ppBlockNode (OUT) - The actual block data.
    @param pPrivateData (IN) - The private data for the callback. Gets registered in the L2caw_PullWriteContainer.

    @return The normal return code is 0, representing success or error code.
 */
typedef DivXError ( *fnPullGetDataCallback )( DMFBlockType_t blockType,
                                              int32_t nStreamOfBlockType,
                                              DMFBlockNode_t **ppBlockNode, void *pPrivateData );

/*!
    Call back is called on uninitializations of each DMFType.

    @param pPrivateData (IN) - The private data for the callback. Gets registered in the L2caw_PullWriteContainer.

    @return The normal return code is 0, representing success or error code.
 */
typedef DivXError ( *fnPullDeInitCallback )( void *pPrivateData );

/*!
    Sets Metadata info on the container.

    @param hContainer (IN) - Handle to instance.
    @param queryValueID (IN) - A the ID to the query.  See DMFQueryIds.h for all possible queries.
    @param inputValue (IN) - A input variant for the query.  This value is used only if the query needs information.
    @param index (IN) - The index of the value to query.
    @param outputValue (OUT) - A variant to receive a value.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullSetMetaInfo( DMFContainerHandle hContainer,
                                 int32_t            queryValueID,
                                 DMFVariant        *inputValue,
                                 int32_t            index,
                                 DMFVariant        *outputValue );

/*!
    Writes the container.  Uses the callbacks to request the data from the input sources.

    @param hContainer (IN) - Handle to instance.
    @param initCallback (IN) - Pointer to a function that gets called when a new type initialized.
    @param getDataCallback (IN) - Pointer to a function that gets called to request a block.
    @param deInitCallback (IN) - Pointer to a function that gets called to deinitialize a type.
    @param privateData (IN) - The private data that will get passed to each callback.

    @return The normal return code is 0, representing success or error code.
 */
DivXError L2caw_PullWriteContainer( DMFContainerHandle    hContainer,
                                    fnPullInitCallback    initCallback,
                                    fnPullGetDataCallback getDataCallback,
                                    fnPullDeInitCallback  deInitCallback,
                                    void                 *privateData );

#ifdef __cplusplus
}
#endif

#endif /* _MAPPINGLAYERWRITEPULL_H_ */
