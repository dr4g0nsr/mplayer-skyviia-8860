/*!
    @file
@verbatim
$Id: DF3HybridIndex.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3HYBRIDINDEX_H_
#define _DF3HYBRIDINDEX_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DF3Parser.h"
#include "DF3Index.h"
#include "DivXLinkedList.h"

#define MAX_HYBRID_INDEX_TRACKS 0x100 // Only 100 hex available for storage
#define HYBRID_INDEX_TIME_BETWEEN_STORED_CUES 0 // Only store cue points at a regular time interval (ms)


/*! Creates a new DF3 Index

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param gapMs            (IN)    - Required gap between index entries
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexCreate( DF3IndexHandle* ppDF3Index, uint64_t gapMs, DivXMem hMem);

/*! Deletes a created DF3 Index

    @param pHandle          (IN)   - DF3IndexHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexDelete( DF3IndexHandle pDF3Index);

/*! Indicates a new time entry is being created

    @param pHandle          (IN)    - DF3IndexHandle
    @param cuePointOffset   (IN)    - Position within the file for the cue point

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexNewEntry( DF3IndexHandle pDF3Index, uint64_t cuePointOffset);

/*! Sets a new Index Time Entry

    @param pHandle          (IN)    - DF3IndexHandle
    @param time           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexSetTime( DF3IndexHandle pDF3Index, uint64_t time);

/*! Sets a new cue point track

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexNewTrackEntry( DF3IndexHandle pDF3Index);

/*! Sets a cue point block number

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexSetBlock( DF3IndexHandle pDF3Index, uint64_t nBlock);

/*! Sets a cue point track

    @param pHandle          (IN)    - DF3IndexHandle 
    @param track            (IN)    - Track
    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexSetTrack( DF3IndexHandle pDF3Index, uint64_t track);

/*! Sets a track position

    @param pHandle          (IN)    - DF3IndexHandle
    @param pos              (IN)    - File position

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexSetTrackPosition( DF3IndexHandle pDF3Index, uint64_t pos);

/*! Gets a offset based on time

    @param pHandle              (IN)    - DF3IndexHandle 
    @param pTime                (IN/OUT)- Time to set to (in) and actual time (out)
    @param track                (IN)    - Track entry to retrieve
    @param pOffset              (OUT)   - Retrieved offset
    @param uiTimecodeScale      (IN)    - Timecode scale of the title
    @param fTrackTimecodeScale  (IN)    - Timecode scale of the track
    @param bForward             (IN)    - Forward or reverse get (previous/next)

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexGetOffset( DF3IndexHandle pDF3Index, DivXTime *pTime, uint32_t track, uint64_t *pOffset, uint64_t uiTimecodeScale, float fTrackTimecodeScale, DF3_SI_SeekType seekType);

/*! Gets the next offset

    @param pHandle          (IN)    - DF3IndexHandle
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexNextOffset( DF3IndexHandle pDF3Index, uint64_t *pOffset);

/*! Determines if the frame is a reference frame

    @param pHandle          (IN)    - DF3IndexHandle
    @param refBlock         (OUT)   - Reference block

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HybridIndexIsReferenceBlock( DF3IndexHandle pDF3Index, DivXBool* refBlock);

#endif //DF3INDEX
