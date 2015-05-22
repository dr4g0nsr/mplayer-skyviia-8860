/*!
    @file
@verbatim
$Id: DF3Index.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3INDEX_H_
#define _DF3INDEX_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DivXTime.h"

/*! Index data handle
*/
//typedef void* DF3IndexDataHandle;

/*! The myType for DF3IndexData_t */
#define DF3_INDEX_DATA_TYPE 0x101

/*! This is a generic header that will work for all types derived from DF3IndexData */
typedef struct _DF3IndexData_t
{
    uint32_t myType; /* The structures type */
    /* More data */
} DF3IndexData_t;

typedef enum _DF3_SI_SeekType{
    DF3_SI_Exact = 0,
    DF3_SI_Reverse,
    DF3_SI_Forward
}DF3_SI_SeekType;

typedef DF3IndexData_t * DF3IndexDataHandle;
typedef struct _DF3Index_t *DF3IndexHandle;

/*! Function pointer for DF3IndexCreate 
*/
typedef DivXError (*pfnDF3IndexCreate)( DF3IndexHandle* ppDF3Index, uint64_t gapMs, DivXMem hMem);

/*! Function pointer for DF3IndexDelete 
*/
typedef DivXError (*pfnDF3IndexDelete)( DF3IndexHandle pDF3Index);

/*! Function pointer for DF3SetIndexGap
*/
typedef DivXError (*pfnDF3IndexSetGap) (DF3IndexHandle pDF3Index, uint64_t gapMs);

/*! Function pointer for DF3GetIndexGap
*/
typedef DivXError (*pfnDF3IndexGetGap) (DF3IndexHandle pDF3Index, uint64_t* gapMs);

/*! Function pointer for DF3IndexNewEntry 
*/
typedef DivXError (*pfnDF3IndexNewEntry)( DF3IndexHandle pDF3Index, uint64_t cuePointOffset);

/*! Function pointer for DF3SimpleSetTime
*/
typedef DivXError (*pfnDF3IndexSetTime)( DF3IndexHandle pDF3Index, uint64_t time);

/*! Function pointer for DF3SimpleNewTrack 
*/
typedef DivXError (*pfnDF3IndexNewTrackEntry)( DF3IndexHandle pDF3Index);

/*! Function pointer for DF3IndexSetBlock 
*/
typedef DivXError (*pfnDF3IndexSetBlock)( DF3IndexHandle pDF3Index, uint64_t nBlock);

/*! Function pointer for DF3IndexSetTrack 
*/
typedef DivXError (*pfnDF3IndexSetTrack)( DF3IndexHandle pDF3Index, uint64_t track);

/*! Function pointer for DF3IndexSetTrackPosition 
*/
typedef DivXError (*pfnDF3IndexSetTrackPosition)( DF3IndexHandle pDF3Index, uint64_t pos);

/*! Function pointer for DF3IndexGetOffset 
*/
typedef DivXError (*pfnDF3IndexGetOffset)( DF3IndexHandle pDF3Index, DivXTime *pTime, uint32_t track, uint64_t *pOffset, uint64_t uiTimecodeScale, float fTrackTimecodeScale, DF3_SI_SeekType seekType);

/*! Function pointer for DF3IndexNextOffset 
*/
typedef DivXError (*pfnDF3IndexNextOffset)( DF3IndexHandle pDF3Index, uint64_t *pOffset);

/*! Function pointer for DF3IndexIsReferenceBlock 
*/
typedef DivXError (*pfnDF3IndexIsReferenceBlock)( DF3IndexHandle pDF3Index, DivXBool* refBlock);

/*! DF3 Index contains function pointers and the data structure
*/
typedef struct _DF3Index_t
{
    pfnDF3IndexCreate                   fnDF3IndexCreate;
    pfnDF3IndexDelete                   fnDF3IndexDelete;
    pfnDF3IndexSetGap                   fnDF3IndexSetGap;
    pfnDF3IndexGetGap                   fnDF3IndexGetGap;
    pfnDF3IndexNewEntry                 fnDF3IndexNewEntry;
    pfnDF3IndexSetBlock                 fnDF3IndexSetBlock;
    pfnDF3IndexSetTrack                 fnDF3IndexSetTrack;
    pfnDF3IndexSetTrackPosition         fnDF3IndexSetTrackPosition;
    pfnDF3IndexGetOffset                fnDF3IndexGetOffset;
    pfnDF3IndexNextOffset               fnDF3IndexNextOffset;
    pfnDF3IndexIsReferenceBlock         fnDF3IndexIsReferenceBlock;
    pfnDF3IndexSetTime                  fnDF3IndexSetTime;
    pfnDF3IndexNewTrackEntry            fnDF3IndexNewTrackEntry;
    DF3IndexData_t*                     df3IndexData;
} DF3Index_t;


/*! Creates a new DF3 Index

    @param pHandle          (IN/OUT)- A pointer to a DF3Index_t* that will
                                      receive the instance.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexCreate(DF3Index_t** handle, DivXMem hMem);

/*! Deletes a created DF3 Index

    @param pHandle          (IN)   - DF3Index_t*

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexDelete( DF3IndexHandle pDF3Index);

/*! Sets the gap size in ms

    @param pHandle          (IN)   - DF3Index_t*
    @param gapMs            (IN)   - Gap size in ms

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexSetGap( DF3IndexHandle pDF3Index, uint64_t gapMs);

/*! Gets the gap size in ms

    @param pHandle          (IN)   - DF3Index_t*
    @param gapMs            (IN)   - Gets the gap size in ms

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexGetGap( DF3IndexHandle pDF3Index, uint64_t* gapMs);

/*! Sets a new cue point

    @param pHandle          (IN)    - DF3Index_t*
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3SetTime( DF3IndexHandle pDF3Index, uint64_t time);

/*! Sets a new cue point track

    @param pHandle          (IN)    - DF3Index_t*
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3NewTrackEntry( DF3IndexHandle pDF3Index, uint32_t track);

/*! Sets a cue point block number

    @param pHandle          (IN)    - DF3Index_t*
    @param nBlock           (IN)    - Block index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexSetBlock( DF3IndexHandle pDF3Index, uint32_t nBlock);

/*! Sets a cue point track

    @param pHandle          (IN)    - DF3Index_t* 
    @param track            (IN)    - Track
    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexSetTrack( DF3IndexHandle pDF3Index, uint32_t track);

/*! Sets a track position

    @param pHandle          (IN)    - DF3Index_t*
    @param pos              (IN)    - File position

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexSetTrackPosition( DF3IndexHandle pDF3Index, uint64_t pos);

/*! Gets a offset based on time

    @param pHandle          (IN)    - DF3Index_t* 
    @param pTime            (IN/OUT - Time to set to (in) and actual time (out)
    @param track            (IN)    - Track entry to retrieve
    @param pOffset          (OUT)   - Retrieved offset

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexGetOffset( DF3IndexHandle pDF3Index, DivXTime *pTime, uint32_t track, uint64_t *pOffset);

/*! Gets the next offset

    @param pHandle          (IN)    - DF3Index_t*
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexNextOffset( DF3IndexHandle pDF3Index, uint64_t *pOffset);

/*! Determines if the frame is a reference frame

    @param pHandle          (IN)    - DF3Index_t*
    @param refBlock         (OUT)   - Reference block

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3IndexIsReferenceBlock( DF3IndexHandle pDF3Index, DivXBool* refBlock);

#endif //DF3INDEX
