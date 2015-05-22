/*!
    @file
@verbatim
$Id: MKVParser.h 56013 2008-09-23 21:17:04Z snaderi $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVPARSER_H_
#define _MKVPARSER_H_

#include "DivXInt.h"
#include "DivXError.h"

#include "DF3/DF3Common/MKVParserElements.h"
#include "DF3/DF3Read/MKVCache.h"

/*! Function pointer definition for MKVParser Notification, this function allows the owner of
    MKVParser to jump elements within the file (for instance, with Cluster elements).
*/
typedef DivXError ( *pfnNotifyMKVParser ) (void* userData, MKVELEMENT_ID_e elementId, uint64_t elemStartOffset, uint64_t elemDataOffset, uint64_t elemDataSize, uint64_t* newElemDataSize );

//typedef struct _MKVParserInst_t * MKVParserHandle;
typedef struct _MKVParserInst_t * MKVParserHandle;

/*! Creates a new parser instance

    @param pHandle          (IN/OUT)- A pointer to a MKVParserHandle that will
                                      receive the instance.
    @param hInputStream     (IN)    - The input stream to read from.
    @param hMKVCache        (IN)    - A handle to the cache.
    @param pfnNotify        (IN)    - Notification callback (allows caller to skip areas in the file)
    @param userData         (IN)    - Used by the notify function
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_New( MKVParserHandle *pHandle, DMFInputStreamHandle *hInputStream, MKVCacheHandle hMKVCache, pfnNotifyMKVParser pfnNotify, void* userData, DivXMem hMem );

/*! Deletes a parser instance

    @param pHandle          (IN) - A pointer to a MKVParserHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_Delete( MKVParserHandle handle );

/*! Initial high level parse operation.  Confirms the file is an EBML mkv file
    of the correct version and finds all segment elements.

    @param pHandle          (IN) - A pointer to a MKVParserHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_ParseDocument( MKVParserHandle handle );

/*! Parses a specific segment.  Gathers track information and cluster 
    locations.

    @param pHandle          (IN) - A pointer to a MKVParserHandle
    @param nSegment         (IN) - Indicates which segment to parse

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_ParseSegment( MKVParserHandle handle, int32_t nSegment );

/*! Parses the cues list.

    @param pHandle          (IN) - A pointer to a MKVParserHandle
    @param offset           (IN) - Offset of the cues list
    @param size             (IN) - Size of the cues list

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_ParseCues( MKVParserHandle handle, uint64_t offset, uint64_t size );

/*! Parses the hierarchical index.

    @param pHandle          (IN) - A pointer to a MKVParserHandle
    @param offset           (IN) - Offset of the hierarchical index
    @param size             (IN) - Size of the hierarchical index

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvParser_ParseHierarchicalIndex( MKVParserHandle handle, uint64_t offset, uint64_t size );

/*! Returns whether the file being parsed has top level clusters

    @param pHandle          (IN) - A pointer to a MKVParserHandle

    @return DivXBool       (OUT)   - Returns DIVX_TRUE if there are top level clusters
*/
DivXBool mkvParser_HasTopLevelClusters( MKVParserHandle handle );
#endif
/* _EBMLPARSER_H_ */
