/*!
    @file
@verbatim
$Id: DF3SimpleIndex.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3SIMPLEINDEX_H_
#define _DF3SIMPLEINDEX_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DF3Index.h"

#define MAX_SIMPLE_INDEX_SIZE 0x1000
#define MAX_SIMPLE_INDEX_TRACKS 0x10 // Only allow 16 tracks 

/*! Track index entry
    Specifies an index entry for a track
*/
typedef struct _DF3SimpleIndexTrackEntry_t_
{
    uint64_t trackNum;         /* track number            */
    uint64_t clusterOffset;    /* offset of cluster       */
    uint64_t blockNumber;      /* block number in cluster */
}DF3SimpleIndexTrackEntry_t;

/*! Time index entry
    Specifies an index entry for a seek time
*/
typedef struct _DF3SimpleIndexTimeEntry_t_
{
    DivXTime                   entryTime;       /* time value in this segment */
    uint8_t                    numTrackEntries; /* number of entries          */
    DF3SimpleIndexTrackEntry_t trackEntries[MAX_SIMPLE_INDEX_TRACKS];    /* track entries              */
}DF3SimpleIndexTimeEntry_t;

/*! Index Instance
    Represents an index for one Segment
*/
typedef struct _DF3SimpleIndex_t_
{
    uint32_t                  myType;                /* Set this to the expected DF3_INDEX_DATA_TYPE */
    int32_t                   SegmentId;             /* segment "title" id */
    DivXMem                   hMem;                  /* memory handle      */
    uint64_t                  gapMs;                 /* Gap in ms */
    uint32_t                  numTimeEntries;        /* number of entries  */
    DF3SimpleIndexTimeEntry_t *timeEntries;           /* time entries       */
    uint64_t                  cueOffset;             /* offset of Cue data */
    uint64_t                  segmentOffset;         /* offset of Segment  */
}DF3SimpleIndex_t;


/*! Creates a new DF3 Index

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param gapMs            (IN)    - Gap in ms
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexCreate( DF3IndexHandle* ppDF3Index, uint64_t gapMs, DivXMem hMem);

/*! Deletes a created DF3 Index

    @param pHandle          (IN)   - DF3IndexHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexDelete( DF3IndexHandle pDF3Index);

/*! Indicates a new time entry is being created

    @param pHandle          (IN)    - DF3IndexHandle
    @param cuePointOffset   (IN)    - Cue point offset within the file

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexNewEntry( DF3IndexHandle pDF3Index, uint64_t cuePointOffset);

/*! Sets a new Index Time Entry

    @param pHandle          (IN)    - DF3IndexHandle
    @param time           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexSetTime( DF3IndexHandle pDF3Index, uint64_t time);

/*! Sets a new cue point track

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexNewTrackEntry( DF3IndexHandle pDF3Index);

/*! Sets a cue point block number

    @param pHandle          (IN)    - DF3IndexHandle
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexSetBlock( DF3IndexHandle pDF3Index, uint64_t nBlock);

/*! Sets a cue point track

    @param pHandle          (IN)    - DF3IndexHandle 
    @param track            (IN)    - Track
    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexSetTrack( DF3IndexHandle pDF3Index, uint64_t track);

/*! Sets a track position

    @param pHandle          (IN)    - DF3IndexHandle
    @param pos              (IN)    - File position

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexSetTrackPosition( DF3IndexHandle pDF3Index, uint64_t pos);

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
DivXError DF3SimpleIndexGetOffset( DF3IndexHandle pDF3Index, DivXTime *pTime, uint32_t track, uint64_t *pOffset, uint64_t uiTimecodeScale, float fTrackTimecodeScale, DF3_SI_SeekType seekType);

/*! Gets the next offset

    @param pHandle          (IN)    - DF3IndexHandle
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexNextOffset( DF3IndexHandle pDF3Index, uint64_t *pOffset);

/*! Determines if the frame is a reference frame

    @param pHandle          (IN)    - DF3IndexHandle
    @param refBlock         (OUT)   - Reference block

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SimpleIndexIsReferenceBlock( DF3IndexHandle pDF3Index, DivXBool* refBlock);

#endif //DF3INDEX
