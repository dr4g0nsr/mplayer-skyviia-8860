/*!
    @file
@verbatim
$Id: DF2IndexHeader.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2INDEXHEADER_H_
#define _DF2INDEXHEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AVI_INDEX_OF_INDEXES 0x00 /* when each entry in aIndex
                                     array points to an index chunk */
#define AVI_INDEX_OF_CHUNKS  0x01 /* when each entry in aIndex;
                                     array points to a chunk in the file */
#define AVI_INDEX_IS_DATA    0x80 /* when each entry in aIndex is really the data */

/* bIndexSubtype codes for INDEX_OF_CHUNKS
    */
#define AVI_INDEX_2FIELD     0x01 /* when fields within frames are also indexed */

#ifdef __cplusplus
}
#endif
#endif /* _L1DF2READ_H_ */
