/*!
    @file
@verbatim
$Id: DF3Interface.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3INTERFACE_H_
#define _DF3INTERFACE_H_

#include "DF3/DF3Common/DMFInputStream.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DivXInt.h"
#include "DivXError.h"


typedef struct _DF3Interface * DF3Interface_h;

/*! 
    @note Opens a DF3 container

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param filename          (IN) - File name to open
    @param hMem              (IN) - Memory handle

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_OpenContainer( DF3Interface_h *ContainerHandle,
                                const DivXString  *filename,
                                DivXMem            hMem );

/*! 
    @note Closes a DF3 container

    @param hContainer        (IN) - Pointer to DF3Interface handle

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_CloseContainer( DF3Interface_h ContainerHandle );

/*! 
    @note Sets the current working title of the DF3 file

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param TitleIndex        (IN) - The title index to select

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_SetTitle( DF3Interface_h hContainer,
                           int32_t           TitleIndex );

/*! 
    @note Retrieves queries from the DF3 container

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param QueryValueID      (IN) - Query ID to retrieve
    @param Value            (OUT) - Retrieved query data
    @param Index             (IN) - Index of queried item to retrieve

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_GetInfo( DF3Interface_h hContainer,
                          int32_t           QueryValueID,
                          DMFVariant       *Value,
                          int32_t           Index );

/*! 
    @note Set the titles active streams

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param blockType         (IN) - Stream type to set active/inactive
    @param nStream           (IN) - Stream to set active/inactive
    @param bActive           (IN) - Active (DIVX_TRUE) or inactive (DIVX_FALSE) setting

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_SetTitleActiveStream( DF3Interface_h hContainer,
                                     DMFBlockType_t    blockType,
                                     int32_t           nStream,
                                     DivXBool          bActive );

/*! 
    @note Retrieves the next active block from the active title

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param pBlockType       (OUT) - Returns the block type retrieved
    @param pnStream         (OUT) - Returns the stream type retrieved
    @param pBlockNode       (OUT) - Data from the block
    @param userData         (OUT) - Additional user defined data

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_ReadNextBlock( DF3Interface_h     hContainer,
                                DMFBlockType_t   *pBlockType,
                                uint32_t         *pnStream,
                                DMFBlockNode_t   *pBlockNode,
                                void             *userData );

/*! 
    @note Retrieves the next block by stream and index

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param blockType         (IN) - Block type to retrieve
    @param nStream           (IN) - Index of this block type
    @param pBlockNode       (OUT) - Data from the block

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_ReadNextBlockByStream(   DF3Interface_h    hContainer,
                                        DMFBlockType_t    blockType,
                                        uint32_t          nStream,
                                        DMFBlockNode_t   *pBlockNode);

/*! 
    @note Sets data to the file (used primarily in menus)

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param QueryValueID      (IN) - Query ID to retrieve
    @param InputValue       (OUT) - Retrieved query data
    @param Index             (IN) - Index of queried item to retrieve
    @param OutputValue      (OUT) - Retrieved output value

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_SetInfo( DF3Interface_h hContainer,
                          int32_t           QueryValueID,
                          DMFVariant       *InputValue,
                          int32_t           Index,
                          DMFVariant       *OutputValue );

/*! 
    @note Sets the current position for ReadNextBlock and ReadNextBlockByStream

    @param hContainer        (IN) - Pointer to DF3Interface handle
    @param blockType         (IN) - Block type to set position on
    @param nStream           (IN) - Stream index to set position on
    @param pTime         (IN/OUT) - Time to set position to (returns time actually set)

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3r_SetPosition( DF3Interface_h hContainer,
                              DMFBlockType_t    blockType,
                              int32_t           nStream,
                              DivXTime         *pTime );

#endif
/* _DF3INTERFACE_H_ */
