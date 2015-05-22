/*!
    @file
@verbatim
$Id: DF2FieldIndexChunk.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2FIELDINDEXCHUNK_H_
#define _DF2FIELDINDEXCHUNK_H_

#include "DF2IndexHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DF2FieldIndexEntry
{
    uint32_t dwOffset;
    uint32_t dwSize;           /* size of all fields (bit 31 set for NON-keyframes) */
    uint32_t dwOffsetField2;   /* offset to second field */
} DF2FieldIndexEntry;

/*! The AVI Field Index Chunk is  the same as the Standard Index Chunk except that it contains
    the locations of each field in the frame.

    See ODML AVI File Format Extensions document:
    The field wLongsPerEntry is set to 3 because fo the addition of the dwOffsetField2
    field in the array.  This is indicated by setting the sub type to AVI_INDEX_2FIELD.
 */
typedef struct _DF2FieldIndexChunk
{
    char                cId[4]; /* 'ix##' */
    uint32_t            cb;
    uint16_t            wLongsPerEntry; /* must be sizeof(aIndex[0])/sizeof(uint32_t) */
    uint8_t             bIndexSubType; /* must be 0 */
    uint8_t             bIndexType; /* must be AVI_INDEX_OF_CHUNKS */
    uint32_t            nEntriesInUse; /* */
    char                cChunkId[4]; /* '##dc' or '##db' or '##wb' etc... */
    uint64_t            qwBaseOffset; /* all dwOffsets in aIndex arrray are relative to this */
    uint16_t            dwReserved3; /* Must be 0 */
    DF2FieldIndexEntry *fieldIndexEntry;   /* Field Index Entry array  fieldIndexEntry[wLongsPerEntry] */
} DF2FieldIndexChunk;

#ifdef __cplusplus
}
#endif
#endif /* _DF2FIELDINDEXCHUNK_H_ */
