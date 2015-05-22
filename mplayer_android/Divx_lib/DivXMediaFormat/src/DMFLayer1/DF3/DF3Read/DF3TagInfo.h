/*!
    @file
@verbatim
$Id: DF3TagInfo.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3TAGINFO_H_
#define _DF3TAGINFO_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DMFTagInfo.h"

/*!
    This structure is used to identify the size, type and other attributes of a tag/metadata 
    within a file.
 **/
typedef struct _DF3TagsInfo_t
{
    uint64_t                    size;                /*!< Size of the tag in bytes*/
    uint64_t                    offset;              /*!< The offset in the file for the data */
    int32_t                     index;               /*!< Index of this tag data */
    int32_t                     simpleTagIndex;      /*!< The Simple tag index tag->simpleTag->data */
    int32_t                     tagIndex;            /*!< The tag index this data is part of */
    DMF_TAG_DATA_TYPE_e         type;                /*!< Type of the tag */
    DMF_TAG_TARGET_e            target;              /*!< Target type */
    int32_t                     numTracks;           /*!< Number of track indexes*/
    int32_t                     numChapters;         /*!< Number of chapter indexes*/
    int32_t                     numAttachments;      /*!< Number of attachment indexes*/
    int32_t                     numEditions;         /*!< Number of editions indexes */
    int32_t                     titleIndex;          /*!< Title index reference, -1 if not relevant */
    DMFBlockType_t              trackType;           /*!< Track type, BLOCK_TYPE_UNKNOWN if not relevant */
    int32_t                     trackIndex;          /*!< Track index, index of trackType */
    int32_t                     chapter;             /*!< Chapter reference, -1 if not relevant */
} DF3TagsInfo_t;


#endif
