/*!
    @file
@verbatim
$Id: DF2StdIndexChunk.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2STDINDEXCHUNK_H_
#define _DF2STDINDEXCHUNK_H_

#include "DF2IndexHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DF2StdIndexEntry
{
    uint32_t dwOffset;        /* qwBaseOffset + this is absolute file offset */
    uint32_t dwSize;          /* bit 31 is set if this is NOT a keyframe */
} DF2StdIndexEntry;

/*! The AVI Standard Index chunk contains information that indexes AVI frames.

    See ODML AVI File Format Extensions document:
    A single standard index chunk can only index data within a 4GB region.  The
    dwOffset field points to the start of the data itself, and not to the start of
    the RIFF chunk for that field.
 */
typedef struct _DF2StdIndexChunk
{
    char              cId[4];  /* 'ix##' */
    uint32_t          cb;
    uint16_t          wLongsPerEntry; /* must be sizeof(aIndex[0])/sizeof(uint32_t) */
    uint8_t           bIndexSubType; /* must be 0 */
    uint8_t           bIndexType; /* must be AVI_INDEX_OF_CHUNKS */
    uint32_t          nEntriesInUse; /* */
    char              cChunkId[4]; /* '##dc' or '##db' or '##wb' etc... */
    uint64_t          qwBaseOffset; /* all dwOffsets in aIndex arrray are relative to this */
    uint32_t          dwReserved3; /* Must be 0 */
    DF2StdIndexEntry *stdIndexEntry; /* uint32_t adw[wLongsPerEntry]; */
} DF2StdIndexChunk;

/* Use these actuall sizes of the chunk data on disc, not size of the structure which is non portable across platforms
or compilers */
#define DF2STDINDEXCHUNKSIZE 32
#define DF2STDINDEXENTRY 8

#ifdef __cplusplus
}
#endif
#endif /* _DF2STDINDEXCHUNK_H_ */
