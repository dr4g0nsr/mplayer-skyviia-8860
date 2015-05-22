/*!
    @file
   @verbatim
   $Id: DMFTagInfo.h 58500 2009-02-18 19:45:46Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFTAGINFO_H_
#define _DMFTAGINFO_H_

#include "DivXInt.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"

typedef enum {
    DMF_TAG_UNKNNOWN               = 0x00, 
    DMF_TAG_ID                     = 0x1254C367,
    DMF_TAG_SIMPLETAG              = 0x67C8,
    DMF_TAG_TARGETS                = 0x63C0,
    DMF_TAG_TARGETTYPE             = 0x63CA,
    DMF_TAG_TARGETTYPEVALUE        = 0x68CA,
    DMF_TAG_COPYRIGHT              = 0x4489,
    DMF_TAG_DESCRIPTION            = 0x4490,
    DMF_TAG_URL                    = 0x4491,
    DMF_TAG_JPEG                   = 0x4492,
    DMF_TAG_DISPLAY_SETTINGS       = 0x4493,
    DMF_TAG_DISPLAY_ORIGIN         = 0x4494,
    DMF_TAG_NAME                   = 0x45A3,
    DMF_TAG_LANGUAGE               = 0x447A,
    DMF_TAG_ORIGINAL               = 0x4484,
    DMF_TAG_BINARY                 = 0x4485,
    DMF_TAG_STRING                 = 0x4487,
    DMF_TAG_TRACKUID               = 0x63C5,
    DMF_TAG_EDITIONUID             = 0x63C9,
    DMF_TAG_CHAPTERUID             = 0x63C4,
    DMF_TAG_ATTACHMENTUID          = 0x63C6,
} DMF_TAG_DATA_TYPE_e;

typedef enum {
    DMF_TARGET_UNKNNOWN = 0x00,
    DMF_TARGET_COLLECTION,
    DMF_TARGET_SEASON,
    DMF_TARGET_MOVIE,
    DMF_TARGET_PART,
    DMF_TARGET_TRACK,
    DMF_TARGET_EDITION,
    DMF_TARGET_CHAPTER,
    DMF_TARGET_ATTACHMENT,
    DMF_TARGET_SCENE,
    DMF_TARGET_SHOT,
    NUM_DMF_TARGET
} DMF_TAG_TARGET_e;

/*!
    This structure is used to identify the size, type and other attributes of a tag/metadata 
    within a file.
 **/
typedef struct _DMFTagInfo_t
{
    uint64_t                    size;                /*!< Size of the tag in bytes*/
    DMF_TAG_DATA_TYPE_e         type;                /*!< Type of the tag */
    DMF_TAG_TARGET_e            target;              /*!< Target type */
    int32_t                     numTracks;
    int32_t                     numChapters;
    int32_t                     numAttachments;
    int32_t                     numEditions;
    int32_t                     index;               /*!< The data index of this item */
    int32_t                     simpleTagIndex;      /*!< The simple tag tag->simpleTag->data */
    int32_t                     tagIndex;            /*!< The tag index for this tag information */
    int32_t                     titleIndex;          /*!< Title index reference, -1 if not relevant */
    DMFBlockType_t              trackType;           /*!< Track type, BLOCK_TYPE_UNKNOWN if not relevant */
    int32_t                     trackIndex;          /*!< Track index, index of trackType */
    int32_t                     chapter;             /*!< Chapter reference, -1 if not relevant */

} DMFTagInfo_t;

#endif /* _DMFTAGINFO_H_ */
