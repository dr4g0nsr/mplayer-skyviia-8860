/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#ifndef _AVC_READ_H_
#define _AVC_READ_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DMFVariant.h"
#include "DivXString.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "L1ContainerHandle.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AVCContainer_t *AVCContainerHandle;

/*!
    Opens an instance of the AVC Read Module.  The handle is created and passed back in.

    @param hContainer   (OUT) - A L1ContainerHandle is passed back in the value.
    @param fileName     (IN)  - This is the file name for the container to open.
    @return DivXError   (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_OpenContainer( AVCContainerHandle       *hContainer,
                                const DivXString        *fileName,
                                DivXMem                  hMem );

/*!
    Closes the instance of the module.

    @param hContainer   (IN)  - Container handle to close.
    @return DivXError   (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_CloseContainer( AVCContainerHandle hContainer );

/*!
    Sets the cur title position.  For AVC the only valid valid is 0.

    @param hContainer   (IN)  - Container to operate on.
    @param nTitle       (IN)  - Title number (zero based).
    @return DivXError   (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_SetTitle( AVCContainerHandle hContainer,
                           int32_t           nTitle );

/*!
    Sets the current position in the file.

    @param hContainer   (IN)     - Container to operate on.
    @param blockType    (IN)     - Block type of stream. (Should always be BLOCK_TYPE_VID).
    @param nStream      (IN)     - Stream num.  (Should always be zero).
    @param pNumBlock    (IN/OUT) - Block to seek to and the actual block position after seek is returned.
    @param pTime        (IN/OUT) - Time to seek to and the actual time after seek is returned.
    @return DivXError   (OUT)    - Returns a DivXError error code (See DivXError.h)

    @note Only pNumBlock or pTime is needed to seek.  If both are supplied with when nBlock takes
    precidence.
 */
DivXError L1avcr_SetPosition( AVCContainerHandle hContainer,
                              DMFBlockType_t    blockType,
                              int32_t           nStream,
                              DivXTime         *pTime );

/*!
    Sets the stream active for reading by ReadNextBlock.

    @param hContainer   (IN)     - Container to read from.
    @param blockType    (IN)     - Block type of stream. (Should always be BLOCK_TYPE_VID).
    @param nStream      (IN)     - Stream num.  (Should always be zero).
    @param bActive      (IN)     - Set to DIVX_TRUE to enable the stream for reading.
    @return DivXError   (OUT)    - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_SetTitleActiveStream( AVCContainerHandle hL1Container,
                                       DMFBlockType_t    blockType,
                                       int32_t           nStream,
                                       DivXBool          bActive );

/*!
    Reads from the container.

    @param hContainer   (IN)     - Container to read from.
    @param pBlockType   (OUT)    - Block type of stream read out. (Should always be BLOCK_TYPE_VID).
    @param pnStream     (OUT)    - Stream num  of stream read.  (Should always be zero).
    @param pBlockNode   (IN/OUT) - Pointer to block node where the block read is coppied to.
    @param userData     (IN/OUT) - Not used should be NULL.
    @return DivXError   (OUT)    - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_ReadNextBlock( AVCContainerHandle hL1Container,
                                DMFBlockType_t   *pBlockType,
                                uint32_t         *pnStream,
                                DMFBlockNode_t   *pBlockNode,
                                void             *userData );

/*!
    Reads the next block according to BlockType and int32_t.

    @param hContainer   (IN)     - Container to read from.
    @param blockType    (IN)     - Block type of stream. (Should always be BLOCK_TYPE_VID).
    @param nStream      (IN)     - Stream num.  (Should always be zero).
    @param pBlockNode   (IN/OUT) - Pointer to block node where the block read is coppied to.
    @return DivXError   (OUT)    - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_ReadNextBlockByStream( AVCContainerHandle hContainer,
                                        DMFBlockType_t    blockType,
                                        int32_t           nStream,
                                        DMFBlockNode_t   *pBlockNode );

/*!
    Get info from the container.  The info is returned in the DMFVariant.

    @param hContainer   (IN)     - Container to operate on.
    @param QueryValueID (IN)     - The Query Id.
    @param Value        (IN/OUT) - The value is passed out in the DMFVariant.
    @parm  Index        (IN)     - An index of the data to query.
    @return DivXError   (OUT)    - Returns a DivXError error code (See DivXError.h)
 */
DivXError L1avcr_GetInfo( AVCContainerHandle ContainerHandle,
                          int32_t           QueryValueID,
                          DMFVariant       *Value,
                          int32_t           Index );

#ifdef __cplusplus
}
#endif

#endif /* _AVC_READ_H_ */
