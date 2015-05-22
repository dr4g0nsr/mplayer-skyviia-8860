/*!
    @file
@verbatim
$Id: DF3HierarchicalIndex.h 59490 2009-04-08 17:12:50Z ashivadas $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3HIERARCHICALINDEX_H_
#define _DF3HIERARCHICALINDEX_H_

#include "DivXMem.h"
#include "DivXError.h"
//#include "DF3Index.h"
#include "DMFBlockType.h"

#define MAX_HIERARCHICAL_INDEX_SIZE 256    // Only 100 hierarchical index points allowed
#define MAX_HIERARCHICAL_INDEX_TRACKS 0x18  // Only allow 24 tracks 

/*! Track index entry
    Specifies an index entry for a track
*/
typedef struct _DF3HierarchicalIndexTrackEntry_t_
{
    uint32_t       trackNum;
    uint32_t       clusterOffset;
}DF3HierarchicalIndexTrackEntry_t;

/*! Time index entry
    Specifies an index entry for a seek time
*/
typedef struct _DF3HierarchicalIndexTimeEntry_t_
{
    DivXTime                            entryTime;       /* time value in this segment */
    uint8_t                             numTrackEntries; /* number of entries          */
    DF3HierarchicalIndexTrackEntry_t    trackEntries[MAX_HIERARCHICAL_INDEX_TRACKS];    /* track entries              */
}DF3HierarchicalIndexTimeEntry_t;

/*! Index Instance
    Represents an index for one Segment
*/
typedef struct _DF3HierarchicalIndex_t_
{
    uint32_t                        myType;                /* Set this to the expected DF3_INDEX_DATA_TYPE */
    int32_t                         SegmentId;             /* segment "title" id */
    DivXMem                         hMem;                  /* memory handle      */
    uint64_t                        gapMs;                 /* Gap in ms */
    uint32_t                        numTimeEntries;        /* number of entries  */
    DF3HierarchicalIndexTimeEntry_t *timeEntries;          /* time entries       */
    uint64_t                        cueOffset;             /* offset of Cue data */
    uint64_t                        segmentOffset;         /* offset into Segment  */
}DF3HierarchicalIndex_t;


/*! Creates a new DF3 Index

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param gapMs            (IN)    - Gap in ms
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexCreate( DF3IndexHandle* ppDF3Index, uint64_t gapMs, DivXMem hMem);

/*! Deletes a created DF3 Index

    @param pHandle          (IN)   - DF3IndexHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexDelete( DF3IndexHandle pDF3Index);

/*! Indicates a new time entry is being created

    @param pHandle          (IN)    - DF3IndexHandle
    @param cuePointOffset   (IN)    - Cue point offset within the file

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexNewEntry( DF3IndexHandle pDF3Index, uint64_t cuePointOffset);

/*! Sets a new Index Time Entry

    @param pHandle          (IN)    - DF3IndexHandle
    @param time           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexSetTime( DF3IndexHandle pDF3Index, uint64_t time);

/*! Sets a new cue point track

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexNewTrackEntry( DF3IndexHandle pDF3Index);

/*! Sets a cue point block number

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexSetBlock( DF3IndexHandle pDF3Index, uint64_t nBlock);

/*! Sets a cue point track

    @param pHandle          (IN)    - DF3IndexHandle 
    @param track            (IN)    - Track
    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexSetTrack( DF3IndexHandle pDF3Index, uint64_t track);

/*! Sets a track position

    @param pHandle          (IN)    - DF3IndexHandle
    @param pos              (IN)    - File position

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexSetTrackPosition( DF3IndexHandle pDF3Index, uint64_t pos);

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
DivXError DF3HierarchicalIndexGetOffset( DF3IndexHandle pDF3Index, DivXTime *pTime, uint32_t track, uint64_t *pOffset, uint64_t uiTimecodeScale, float fTrackTimecodeScale, DF3_SI_SeekType seekType);

/*! Gets the next offset

    @param pHandle          (IN)    - DF3IndexHandle
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexNextOffset( DF3IndexHandle pDF3Index, uint64_t *pOffset);

/*! Determines if the frame is a reference frame

    @param pHandle          (IN)    - DF3IndexHandle
    @param refBlock         (OUT)   - Reference block

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3HierarchicalIndexIsReferenceBlock( DF3IndexHandle pDF3Index, DivXBool* refBlock);

#endif //DF3INDEX
