/*!
    @file
@verbatim
$Id: DF3Parser.h 60376 2009-05-27 00:17:44Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3PARSER_H_
#define _DF3PARSER_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DF3Lacing.h"
#include "MKVCache.h"
#include "MKVParser.h"

typedef struct _DF3ParserInst_t
{
    DivXMem                    hMem;
    DMFInputStreamHandle       hInputStream;
    MKVCacheHandle             hMKVCache;
    MKVParserHandle            hMKVParser;
    DF3LacingHandle            hLacing; // Lacing object handle

    // parser state variables
    DivXTime uiLastTimecode[NUM_BLOCK_TYPE];
    //DivXTime uiLastDeliveredTimecode[50];
    uint64_t uiLastDeliveredOffset[50];
    DivXBool bStreamResetPosition[50];
    uint64_t uiLastOffset; // This will always fall on a UID even during delacing (delacing
                           // stores it's own uiOffset variable until it is finished)
    uint64_t uiLastClusterEnd; // End position for the current cluster

    DivXBool bSkipNextVidBlock;  // used for reading interlaced bitstreams where an audio block might be interleaved between video fields

    // State information for retrieving a reference frame
    DivXBool findRefFrame;
    DivXTime findRefTime;

    // Previous Reference Offset Exists
    DivXBool bPreviousReferenceOffsetExists;
    DivXBool bPrevRefRead;
    uint64_t uiPreviousRefOffset;
    DivXTime tPreviousRefTime;
    DivXTime tPrevRefSyncTime; // Set this flag to indicate a previous sync request was made
}DF3ParserInst_t;


#define MKV_KEYFRAME_BIT     0x80

#define DF3_KEYFRAME_FLAG    0x10

typedef DF3ParserInst_t* DF3ParserHandle;

// CHAPTER - playlist support
typedef enum _DF3_Playlist_State{
    DF3_Playlist_NoChange = 0,
    DF3_Playlist_Seek,
    DF3_Playlist_EOS,
    DF3_Playlist_Error
}DF3_Playlist_State;

/*! Creates a new parser instance

    @param pHandle          (IN/OUT)- A pointer to a df3Parser handle that will
                                      receive the instance.
    @param hInputStream     (IN)    - The input stream to read from.
    @param hMKVCache        (IN)    - A handle to the cache.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_New( DF3ParserHandle *pHandle, DMFInputStreamHandle *hInputStream, MKVCacheHandle hMKVCache, DivXMem hMem );

/*! Deletes a parser instance

    @param handle          (IN) - A DF3ParserHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_Delete( DF3ParserHandle handle );

/*! Initial high level parse operation.  Confirms the file is an EBML mkv file
    of the correct version and finds all segment elements.

    @param pHandle          (IN) - A DF3ParserHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ParseDocument( DF3ParserHandle handle );

/*! Parses a specific segment.  Gathers track information and cluster 
    locations.

    @param handle          (IN) - A DF3ParserHandle
    @param nSegment         (IN) - Indicates which segment to parse

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ParseSegment( DF3ParserHandle handle, int32_t nSegment );

/*! Parses the cues list.

    @param handle          (IN) - A DF3ParserHandle
    @param offset          (IN) - Offset of the cues list
    @param size            (IN) - Size of the cues list

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ParseCues( DF3ParserHandle handle, uint64_t offset, uint64_t size);

/*! Parses the hierarchical index

    @param handle          (IN) - A DF3ParserHandle
    @param offset          (IN) - Offset of the hierarchical index
    @param size            (IN) - Size of the hierarchical index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ParseHierarchicalIndex( DF3ParserHandle handle, uint64_t offset, uint64_t size);

/*! Reads the next block by stream from the parser

    @param handle          (IN) - A DF3ParserHandle
    @param pBlockType       (OUT) - Returns the block type retrieved
    @param pnStream         (OUT) - Returns the stream type retrieved
    @param pBlockNode       (OUT) - Data from the block
    @param userData         (OUT) - Additional user defined data

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ReadNextBlockByStream( DF3ParserHandle handle, DMFBlockType_t blockType, uint32_t nStream, DMFBlockNode_t *pBlockNode);

/*! Reads the next block from the parser

    @param handle          (IN) - A DF3ParserHandle
    @param pBlockType       (OUT) - Returns the block type retrieved
    @param pnStream         (OUT) - Returns the stream type retrieved
    @param pBlockNode       (OUT) - Data from the block
    @param userData         (OUT) - Additional user defined data

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ReadNextBlock( DF3ParserHandle handle, DMFBlockType_t *pBlockType, uint32_t *pnStream, DMFBlockNode_t *pBlockNode, void *userData );

/*! Reads the next block from the parser

    @param handle          (IN) - A DF3ParserHandle
    @param pBlockType       (OUT) - Returns the block type retrieved
    @param pnStream         (OUT) - Returns the stream type retrieved
    @param pBlockNode       (OUT) - Data from the block
    @param userData         (OUT) - Additional user defined data
    @param byStream         (IN)  - Is this a by stream read

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_ReadNextBlockGeneric( DF3ParserHandle handle, DMFBlockType_t *pBlockType, uint32_t *pnStream, DMFBlockNode_t *pBlockNode, void *userData, DivXBool byStream );

/*! This call back function is passed to the MKVParser to handle skipping parts of the 
    file, such as the Cluster list

    @param elemId            (IN) - The element id
    @param elemStartOffset   (IN) - The start offset of the element
    @param elemDataOffset    (IN) - The data offset of the element
    @param elemDataSize      (IN) - The size of the data
    @param pNewElemDataSize (OUT) - The returned size of the element data (this may encompass more than one element)

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError NotifyFromMKVParser(void* handle, MKVELEMENT_ID_e elementId, uint64_t elemStartOffset, uint64_t elemDataOffset, uint64_t elemDataSize, uint64_t* pNewElemDataSize );

/* Seek to a block in the file based on a time code and track number

    @param handle           (IN) - A DF3ParserHandle
    @param tracknum         (IN) - The track number to look for
    @param pOffset          (OUT)- The location where the offset will be stored

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_FindPosition(DF3ParserHandle handle, DMFBlockType_t blockType, uint32_t tracknum, DivXTime *pTime, uint64_t *pOffset);

/* Seek to a block in the file by reading the cue list

    @param handle           (IN) - A DF3ParserHandle
    @param blockType        (IN) - Blocktype being seeked
    @param tracknum         (IN) - The track number to look for
    @param pTime            (IN/OUT) - In: Time being requested to seek to; Out: Actual time seeked to
    @param pOffset          (OUT)- The location where the offset will be stored

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_FindPositionFromCueList(DF3ParserHandle handle, uint64_t cuesOffset, DMFBlockType_t blockType, uint32_t tracknum, DivXTime *pTime, uint64_t *pOffset);

/* Find the next/prev sync point by reading the cue list

    @param handle           (IN) - A DF3ParserHandle
    @param bNextSync        (IN) - DIVX_TRUE if looking for the next sync point
    @param tracknum         (IN) - The track number to look for
    @param currTime         (IN) - Current time
    @param pTime            (OUT) - Sync point time

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_GetSyncPointFromCueList(DF3ParserHandle handle, DivXBool bNextSync, uint64_t cuesOffset, uint32_t tracknum, DivXTime currTime, DivXTime *pTime);

/* Set the block back to the previous reference frame from that last reference frame read

    @param handle           (IN)     - A DF3ParserHandle
    @param pTime            (OUT)    - The time set to 

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Parser_SetToPreviousRefFrame(DF3ParserHandle handle, DivXTime *pTime);

#endif
/* _DF3PARSER_H_ */
