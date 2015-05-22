/*!
    @file
@verbatim
$Id: L4ReadTitle.h 58719 2009-03-02 04:55:45Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4READTITLE_H_
#define _L4READTITLE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "./L4Read/L4ReadStream.h"
#include "DMFTagInfo.h"

#include "DMFBlockNode.h"

#include "L3Common/Layer3CommonPublished.h"


typedef struct _L4ReadTitleInst_t* L4ReadTitleHandle;

/*!
    Create an instance.

    @param pHandle       (OUT) - A pointer to receive instance.
    @param hL3Container  (IN)  - Container handle to retrieve title from
    @param hMem          (IN)  - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitle_New( L4ReadTitleHandle *pHandle, void* hL3Container, void* hL3Resources, DivXMem  hMem );


/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitle_Delete( L4ReadTitleHandle handle );

/*!
    Returns the title media handle

    @param handle (IN) - A handle to instance.

    @return TitleMediaL3Handle
*/
TitleMediaL3Handle L4ReadTitle_GetL3TitleMedia( L4ReadTitleHandle handle );

/*!
    Returns the title handle

    @param handle (IN) - A handle to instance.

    @return TitleL3Handle
*/
TitleL3Handle L4ReadTitle_GetL3Title( L4ReadTitleHandle handle );

/*!
    Get the number of streams for a certain type.

    @param handle (IN) - A handle to instance.

    @return The number of streams.
*/
int32_t L4ReadTitle_GetNumStreamsByType( L4ReadTitleHandle handle, DMFBlockType_t type );

/*!
    Returns the currently active stream(s) // TODO EC - Must update to handle multiple active vid streams

    @param handle     (IN) - A handle to instance.
    @param blockType  (IN) - The block type (AUD,VID,SUB)
    @param pIndex     (IN) - The returned index // TODO EC - Will change to an array parameter

    @return The number of streams.
*/
DivXError L4ReadTitle_GetActiveStreams(L4ReadTitleHandle handle, DMFBlockType_t blockType, int32_t* pIndex);

/*!
    Set the stream type active

    @param handle     (IN) - A handle to instance.
    @param blockType  (IN) - The block type (AUD,VID,SUB)
    @param bActive    (IN) - Set active if DIVX_TRUE, deactivated if DIVX_FALSE

    @return DivXError.
*/
DivXError L4ReadTitle_SetStreamTypeActive(L4ReadTitleHandle handle, DMFBlockType_t blockType, DivXBool bActive);

/*!
    Sets a specific type index stream active

    @param handle (IN) - A handle to instance.

    @return The number of streams.
*/
DivXError L4ReadTitle_SetStreamNumActive(L4ReadTitleHandle handle, DMFBlockType_t blockType, uint32_t nActive);

/*!
    Sets all stream active that are selected

    @param handle (IN) - A handle to instance.

    @return The number of streams.
*/
DivXError L4ReadTitle_SetStreamsActive( L4ReadTitleHandle handle);

/*!
    Returns whether a stream of blockType is active

    @param handle    (IN) - A handle to instance.
    @param blockType (IN) - The block type
    @param index     (IN) - The index to set

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXBool L4ReadTitle_IsStreamActive(L4ReadTitleHandle handle, DMFBlockType_t blockType);

/*!
    Checks if the Master Track is active

    @param handle      (IN) - A handle to instance.
    @param titleIdx    (IN) - Title index for the trick track
    @param vidTrackIdx (IN) - Video track index for the trick track

    @return DivXError
*/
DivXError L4ReadTitle_SetMasterTrackActive( L4ReadTitleHandle handle, uint32_t* titleIdx, uint32_t* vidTrackIdx);

/*!
    Checks if the Trick Track is active

    @param handle      (IN) - A handle to instance.
    @param titleIdx    (IN) - Title index for the trick track
    @param vidTrackIdx (IN) - Video track index for the trick track

    @return DivXError
*/
DivXError L4ReadTitle_GetTrickTrackActive( L4ReadTitleHandle handle, uint32_t* titleIdx, uint32_t* vidTrackIdx);

DivXError L4ReadTitle_SetTrickTrackActive( L4ReadTitleHandle handle, uint32_t vidTrackIdx);

/*!
    Sets the trick streams active

    @param handle (IN) - A handle to instance.

    @return DivXError
*/
DivXError L4ReadTitle_SetTrickStreamsActive( L4ReadTitleHandle handle );

/*!
    Adds a stream to the playback container.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4ReadTitle_AddStream( L4ReadTitleHandle handle, DMFBlockType_t type, int32_t nStream );

/*!
    Loads a title

    @param handle (IN) - A handle to instance.
    @param nTitle (IN) - The title index to load

    @return DivXError
*/
DivXError L4ReadTitle_LoadTitle( L4ReadTitleHandle handle, int32_t nTitle );

/*!
    Initialize the read block

    @param handle     (IN) - A handle to instance.
    @param pBlockNode (IN) - Block node to initialize

    @return DivXError
*/
DivXError L4ReadTitle_InitReadBlock( L4ReadTitleHandle handle, DMFBlockNode_t *pBlockNode );

/*!
    DeInitialize the read block

    @param handle     (IN) - A handle to instance.
    @param pBlockNode (IN) - Block node to deinitialize

    @return DivXError
*/
DivXError L4ReadTitle_DeInitReadBlock( L4ReadTitleHandle handle, DMFBlockNode_t *pBlockNode );

/*!
    Read a block

    @param handle      (IN) - A handle to instance.
    @param pBlocktype (OUT) - The block read from title
    @param pnStream    (IN) - The stream number of the block read from the title

    @return DivXError
*/
DivXError L4ReadTitle_ReadNextBlock( L4ReadTitleHandle handle, DMFBlockType_t *pBlockType, uint32_t *pnStream, DMFBlockNode_t *pBlockNode);

/*!
    Read a block by it's stream type and index

    @param handle      (IN) - A handle to instance.
    @param blocktype   (IN) - Block type to read
    @param nStream     (IN) - Index of type of stream to read
    @param pBlockNode (OUT) - Block read

    @return DivXError
*/
DivXError L4ReadTitle_ReadNextBlockByStream( L4ReadTitleHandle handle, DMFBlockType_t blockType, uint32_t nStream, DMFBlockNode_t *pBlockNode);

/*!
    Seek to time

    @param handle      (IN) - A handle to instance.
    @param ptime       (IN) - Time to seek to

    @return DivXError
*/
DivXError L4ReadTitle_Seek( L4ReadTitleHandle handle, DivXTime *pTime);

/*!
    Seek to time

    @param handle      (IN) - A handle to instance.
    @param blocktype   (IN) - Block type to read
    @param nStream     (IN) - Index of type of stream to read
    @param ptime       (IN) - Time to seek to

    @return DivXError
*/
DivXError L4ReadTitle_SeekByStream( L4ReadTitleHandle handle, DMFBlockType_t blockType, uint32_t nStream, DivXTime *pTime);


/*!
    Set the next trick block

    @param handle      (IN) - A handle to instance.
    @param time        (IN) - Time to get next trick block from
    @param trickMode   (IN) - Sets the trick mode

    @return DivXError
*/
DivXError L4ReadTitle_SetNextTrickBlock( L4ReadTitleHandle handle, DivXTime* time, int32_t trickMode );

/*!
    Get the number of streams in the title

    @param handle      (IN) - A handle to instance.

    @return int32_t - The number of streams
*/
int32_t L4ReadTitle_GetNumStreams( L4ReadTitleHandle handle );

/*!
    Get the stream from the title

    @param handle      (IN) - A handle to instance.
    @param nStream     (IN) - Stream index to retrieve

    @return L4ReadStreamHandle - Stream handle that is returned
*/
L4ReadStreamHandle L4ReadTitle_GetStream( L4ReadTitleHandle handle, int32_t nStream );

/*!
    Get the number of streams by type

    @param handle      (IN) - A handle to instance.
    @param type        (IN) - Block type to get number of streams for

    @return int32_t - Number of stream of this type
*/
int32_t L4ReadTitle_GetNumStreamsByType( L4ReadTitleHandle handle, DMFBlockType_t type );

/*!
    Get stream by type

    @param handle      (IN) - A handle to instance.
    @param type        (IN) - Block type to get number of streams for
    @param nStream     (IN) - Stream index

    @return L4ReadStreamHandle - Stream retrieved
*/
L4ReadStreamHandle L4ReadTitle_GetStreamByType( L4ReadTitleHandle handle, DMFBlockType_t type, int32_t nStream );

/*!
    Returns the size of the format specific data for the currently selected title

    @param handle         (IN) - A handle to the instance
    @param blocktype      (IN) - Block type to get codec private data of
    @param nStream        (IN) - Stream index
    @param pLen          (OUT) - Length of buffer returned (use GetFormatSize to pre-initialize buffer)

    @return DivXError
*/
DivXError L4ReadTitle_GetStreamPrivateDataSize( L4ReadTitleHandle handle, DMFBlockType_t blockType, int32_t nStream, uint32_t* pLen);

/*!
    Returns the format specific data

    @param handle         (IN) - A handle to the instance
    @param blocktype      (IN) - Block type to get codec private data of
    @param nStream        (IN) - Stream index
    @param pBuffer       (OUT) - Buffer of data to be returned
    @param len           (OUT) - Length of buffer returned (use GetFormatSize to pre-initialize buffer)

    @return DivXError
*/
DivXError L4ReadTitle_GetStreamPrivateData( L4ReadTitleHandle handle, DMFBlockType_t blockType, int32_t nStream, uint8_t* pBuffer, uint32_t* len);

/*!
    Return the tags to the first entry

    @param handle         (IN) - A handle to the instance

    @return DivXError
*/
DivXError L4ReadTitle_FirstTag( L4ReadTitleHandle handle);

/*!
    Keep grabbing tags until we have no more

    @param handle         (IN) - A handle to the instance

    @return DivXError
*/
DivXError L4ReadTitle_NextTag( L4ReadTitleHandle handle);

/*!
    Retrieve the tag size and other information

    @param handle         (IN) - A handle to the instance
    @param tagInfo       (OUT) - information about the tag including size and index information

    @return DivXError
*/
DivXError L4ReadTitle_GetTagInfo( L4ReadTitleHandle handle, DMFTagInfo_t* tagInfo );


/*!
    Retrieve the tag size and other information

    @param handle         (IN) -  A handle to the instance

    @param type           (IN) -  Enum of target type to retrieve

    @param index          (IN) -  Index of target type to retrieve

    @param value          (OUT) - Value of target

    @return DivXError
*/
DivXError L4ReadTitle_GetTargetByTypeIdx( L4ReadTitleHandle handle,  DMF_TAG_TARGET_e type, 
                                         int32_t index, int32_t *value);
/*!
    Retrieve the tag

    @param handle         (IN)  - A handle to the instance
    @param tagData        (OUT) - Actual tag data

    @return DivXError
*/
DivXError L4ReadTitle_GetTag( L4ReadTitleHandle handle, uint8_t* tagData );

DivXError L4ReadTitle_GetInfo(L4ReadTitleHandle handle, int32_t QueryValueID, DMFVariant *Value, int32_t Index);



#ifdef __cplusplus
}
#endif

#endif //_L4ReadTitle_H_
