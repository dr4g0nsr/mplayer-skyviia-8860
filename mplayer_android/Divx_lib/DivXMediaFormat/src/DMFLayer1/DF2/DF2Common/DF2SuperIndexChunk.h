/*!
    @file
@verbatim
$Id: DF2SuperIndexChunk.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2SUPERINDEXCHUNK_H_
#define _DF2SUPERINDEXCHUNK_H_

#include "DF2IndexHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DF2SuperIndexEntry
{
    uint64_t qwOffset;         /* absolute file offset, offset 0 is unused entry?? */
    uint32_t dwSize;           /* size of index chunk at this offset */
    uint32_t dwDuration;       /* time span in stream ticks */
} DF2SuperIndexEntry;

/*! The Super Index Chunk is an index of indexes and is always found in the 'indx' chunk of
    an AVI file.

    See ODML AVI File Format Extensions document:
    The bIndexSubType is set to the type of index that the Super Index points to.  If the index
    chunks are Standard Index Chunks, the nthe value should be 0.  If the index chunks are AVI
    Field Index Chunks, then the value should be AVI_INDEX_2FIELD.  This implies t hat a stream
    cannot mix Field and Standard Index Chunks.

 */
typedef struct _DF2SuperIndexChunk
{
    char                cId[4]; /* 'ix##' */
    uint32_t            cb;    /* size of this structure */
    uint16_t            wLongsPerEntry; /* must be 4 (size of each entry in aIndex array) */
    uint8_t             bIndexSubType; /* must be 0 or AVI_INDEX_2FIELD */
    uint8_t             bIndexType; /* must be AVI_INDEX_OF_INDEXES */
    uint32_t            nEntriesInUse; /* number of entries in aIndex array that are used */
    char                cChunkId[4]; /* '##dc' or '##db' or '##wb' etc... */
    uint32_t            dwReserved3[3]; /* Must be 0 */
    DF2SuperIndexEntry *superIndexEntry;   /* */
} DF2SuperIndexChunk;

#ifdef __cplusplus
}
#endif
#endif /* _L1DF2READ_H_ */
