/*!
    @file
@verbatim
$Id: DF3HybridIndex_private.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3HYBRIDINDEX_PRIVATE_H_
#define _DF3HYBRIDINDEX_PRIVATE_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DF3Index.h"
#include "DivXLinkedList.h"

typedef LinkedList_t TimeEntryList_t;
typedef TimeEntryList_t* TimeEntryListHandle;

typedef LinkedList_t TrackEntryList_t;
typedef TrackEntryList_t* TrackEntryListHandle;


/*! Track index entry
    Specifies an index entry for a track
*/
typedef struct _DF3HybridIndexTrackEntry_t_
{
    uint64_t trackNum;         /* track number            */
    uint64_t clusterOffset;    /* offset of cluster       */
    uint64_t blockNumber;      /* block number in cluster */
}DF3HybridIndexTrackEntry_t;

/*! Time index entry
    Specifies an index entry for a seek time
*/
typedef struct _DF3HybridIndexTimeEntry_t_
{
    DivXTime                      entryTime;       /* time value in this segment */
    uint8_t                       numTrackEntries; /* number of entries          */
    //DF3HybridIndexTrackEntry_t trackEntries[MAX_HYBRID_INDEX_TRACKS];    /* track entries              */
    uint32_t                      trackEntryListIndex;    /* time entry index */
    uint32_t                      trackEntryListNum;       /* time entry num */
    TrackEntryListHandle          trackEntriesList;
    uint64_t cuePointOffset;   /* Location of the cue point */
}DF3HybridIndexTimeEntry_t;

/*! Index Instance
    Represents an index for one Segment
*/
typedef struct _DF3HybridIndex_t_
{
    uint32_t                       myType;                /* Set this to the expected DF3_INDEX_DATA_TYPE */
    int32_t                        SegmentId;             /* segment "title" id */
    DivXMem                        hMem;                  /* memory handle      */
    uint64_t                       gapMs;                 /* Gap in ms */
    uint64_t                       lastEntryTime;         /* Last entry time in ms */
    uint64_t                       cueOffset;             /* offset of Cue data */
    uint64_t                       segmentOffset;         /* offset of Segment  */
    uint32_t                       numTimeEntries;        /* Number of time entries */
    DF3HybridIndexTrackEntry_t* pTrackEntry;
    DF3HybridIndexTimeEntry_t*  pTimeEntry;            /* time entry       */
    //DF3HybridIndexTimeEntry_t  *timeEntries;           /* time entries       */
    TimeEntryListHandle            timeEntryList;         /* time entries as a linked list */
    uint32_t                       timeEntryListIndex;    /* time entry index */
    uint32_t                       timeEntryListNum;      /* time entry num */
    DivXBool                       storeEntry;            /* Indicates if the temp should be stored */
    DivXBool                       storeNextEntry;        /* Indicates if the next entry should be stored */
    LinkedListIterHandle           timeIter;
    LinkedListIterHandle           trackIter;
    LinkedListIterHandle           timeEntryIter;
}DF3HybridIndex_t;

typedef DF3HybridIndex_t* DF3HybridIndexHandle;

/*! Delete entire list

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_CreateTimeEntryList(DF3HybridIndexHandle handle, DivXMem hMem);

/*! Delete entire list

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_DeleteTimeEntryList(DF3HybridIndexHandle handle);

/*! Adds a new time entry to the timeEntry list

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_AddNewTimeEntryToList(DF3HybridIndexHandle handle, DF3HybridIndexTimeEntry_t** pEntry);

/*! Adds a new time entry to the trackEntry list

    @param pHandle          (IN/OUT)- A pointer to a DF3IndexHandle that will
                                      receive the instance.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_AddNewTrackEntryToList(DF3HybridIndexHandle handle, DF3HybridIndexTrackEntry_t** pEntry);

/*! Resets time entry to retrieve from list, back to 0

    @param pHandle          (IN)    - DF3HybridIndex_t structure

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_ResetTimeEntryList(DF3HybridIndexHandle handle);

/*! Resets track entry to retrieve from list, back to 0

    @param pHandle          (IN)    - DF3HybridIndex_t structure

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_ResetTrackEntryList(DF3HybridIndexHandle handle);

/*! Gets a time entry from the list

    @param pHandle          (IN)    - DF3HybridIndex_t structure
    @param index            (IN)    - Index to retrieve
    @param pEntry          (OUT)    - Retrieved time entry object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_GetTimeEntryFromList(DF3HybridIndexHandle handle, uint32_t index, DF3HybridIndexTimeEntry_t *pEntry);

/*! Gets a track entry from the list

    @param pHandle          (IN)    - DF3HybridIndex_t structure
    @param timeIdx          (IN)    - Time entry index
    @param trackIdx         (IN)    - Track entry index
    @param pEntry          (OUT)    - Retrieved track entry object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_GetTrackEntryFromList(DF3HybridIndexHandle handle, uint32_t timeIdx, uint32_t trackIdx, DF3HybridIndexTrackEntry_t *pEntry);

/*! Gets a track entry from the list

    @param pHandle          (IN)    - DF3HybridIndex_t structure
    @param pEntry          (OUT)    - Retrieved track entry object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_GetNextTrackEntryFromList(DF3HybridIndexHandle handle, DF3HybridIndexTrackEntry_t *pEntry);

/*! Gets a track entry from the list

    @param pHandle          (IN)    - TimeEntryListHandle LinkedList_t*
    @param trackIdx         (IN)    - Track entry index
    @param pEntry          (OUT)    - Retrieved track entry object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3HybridIndex_GetTrackEntryFromList2(TimeEntryListHandle handle, uint32_t trackIdx, DF3HybridIndexTrackEntry_t *pEntry);


#endif //_DF3HYBRIDINDEX_PRIVATE_H_
