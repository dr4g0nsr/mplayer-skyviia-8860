/*!
    @file
@verbatim
$Id: DF3Tags.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3TAGS_H_
#define _DF3TAGS_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DivXLinkedList.h"
#include "DF3/DF3Common/MKVTypes.h"
#include "DF3/DF3Common/MKVParserElements.h"
#include "DMFTagInfo.h"
#include "DF3TagInfo.h"
#include "DF3/DF3Common/DMFInputStream.h"
#include "MKVCache_private.h"

typedef struct _DF3Tags_t* DF3TagsHandle;
/*typedef struct _MKVCacheInst_t * MKVCacheHandle;*/

/*! Creates a new DF3 Index

    @param pHandle          (IN/OUT)- A pointer to a DF3TagsHandle that will
                                      receive the instance.
    @param hMKVCache        (IN)    - MKVCache handle for performing lookups related to target data
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3Tags_Create( DF3TagsHandle* phDF3Tags, MKVCacheHandle hMKVCache, DivXMem hMem);

/*! Deletes a created DF3 Index

    @param pHandle          (IN)   - DF3TagsHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3Tags_Delete( DF3TagsHandle hDF3Tags);

/*! Indicates a new time entry is being created

    @param pHandle          (IN)    - DF3TagsHandle
    @param cuePointOffset   (IN)    - Position within the file for the cue point

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError DF3TagsNewEntry( DF3TagsHandle handle, uint64_t cuePointOffset);

DivXError DF3Tags_AddTag(DF3TagsHandle handle);

DivXError DF3Tags_AddTargets(DF3TagsHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint32_t val);

DivXError DF3Tags_AddSimpleTag(DF3TagsHandle handle);

DivXError DF3Tags_AddSimpleTagItem(DF3TagsHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint8_t val);

DivXError DF3Tags_SetFirst(DF3TagsHandle hDF3Tags);

DivXError DF3Tags_SetNext(DF3TagsHandle hDF3Tags);

DivXError DF3Tags_GetTagInfo(DF3TagsHandle hDF3Tags, DF3TagsInfo_t* pTagInfo);

/*!
    Retrieve the tag size and other information

    @param handle         (IN) -  A handle to the instance

    @param queryID        (IN) -  Target type to retrieve

    @param index          (IN) -  Index of target type to retrieve

    @param value          (OUT) - Value of target

    @return DivXError
*/
DivXError DF3Tags_GetTargetByTypeIdx(DF3TagsHandle hDF3Tags, int32_t queryID, int32_t index, int32_t *value);

#endif //_DF3TAGS_H_
