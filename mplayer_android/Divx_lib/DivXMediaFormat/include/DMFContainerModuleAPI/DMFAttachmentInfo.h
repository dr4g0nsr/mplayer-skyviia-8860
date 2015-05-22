/*!
    @file
   @verbatim
   $Id: DMFAttachmentInfo.h 58500 2009-02-18 19:45:46Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFATTACHMENTINFO_H_
#define _DMFATTACHMENTINFO_H_

#include "DivXInt.h"
#include "DivXString.h"

#define ATTACHMENT_MAX_FILENAME_LEN         256
#define ATTACHMENT_MAX_MIMETYPE_LEN         256
#define ATTACHMENT_MAX_FILEDESCRIPTION_LEN  1000


typedef struct DMFTrackAttachmentLinkInfo_t
{
    uint32_t    uiAttachmentLinkCount;      /*!< Number of links attached to a track */
    uint32_t   *uiAttachmentLinks;          /*!< Array of attachment link indices */
} DMFTrackAttachmentLinkInfo_t;

typedef struct DMFAttachmentInfo_t
{
    uint64_t    uiUID;                                          /*!< The file UID */
    DivXString  sFilename[ATTACHMENT_MAX_FILENAME_LEN];         /*!< File name */
    DivXString  sMimeType[ATTACHMENT_MAX_MIMETYPE_LEN];         /*!< Mime type */
    DivXString  sFileDesc[ATTACHMENT_MAX_FILEDESCRIPTION_LEN];  /*!< File description */
    uint32_t    uiFileSize;                                     /*!< File size */
    uint64_t    uiOffset;                                       /*!< Offset in file */
    uint8_t    *pFileData;                                      /*!< File contents */
} DMFAttachmentInfo_t;

#endif /* _DMFATTACHMENTINFO_H_ */
