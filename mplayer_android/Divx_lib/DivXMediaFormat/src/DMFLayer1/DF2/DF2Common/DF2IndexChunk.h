/*!
    @file
@verbatim
$Id: DF2IndexChunk.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2INDEXCHUNK_H_
#define _DF2INDEXCHUNK_H_

#include "DF2IndexHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __DF2IndexChunk__
{
    char     cId[4];
    uint32_t cb;
    uint16_t wLongsPerEntry; /* size of each entry in aIndex array */
    uint8_t  bIndexSubType;    /* future use.  must be 0 */
    uint8_t  bIndexType;       /* one of AVI_INDEX_ * codes */
    uint32_t nEntriesInUse;  /* index of first unused member */
    char     cChunkId[4];      /* fcc of what is indexed */
    uint32_t dwReserved[3];  /* meaning differs for each index
                                type/subtype.  0 if unused */
    uint32_t *adw;            /* uint32_t adw[wLongsPerEntry]; */
} DF2IndexChunk;

#ifdef __cplusplus
}
#endif
#endif /* _L1DF2READ_H_ */
