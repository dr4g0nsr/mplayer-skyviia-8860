/*!
    @file
@verbatim
$Id: MKVParser_private.h 59996 2009-05-06 19:54:33Z ashivadas $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVPARSERPRIVATE_H_
#define _MKVPARSERPRIVATE_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DMFErrors.h"

#include "DF3/DF3Common/MKVTypes.h"
#include "DF3/DF3Common/EBMLElementReader.h"
#include "DF3/DF3Common/MKVParserElements.h"

#include "DF3/DF3Common/DMFInputStream.h"
#include "DF3/DF3Read/EBMLMap.h"
#include "DF3/DF3Read/MKVCache.h"
#include "DMFBlockNode.h"

/*! Function pointer definition for MKVParser Notification, this function allows the owner of
    MKVParser to jump elements within the file (for instance, with Cluster elements).
*/
//typedef DivXError ( *pfnNotifyMKVParser ) (void* userData, MKVELEMENT_ID_e elementId, uint64_t elemStartOffset, uint64_t elemDataOffset, uint64_t elemDataSize, uint64_t* newElemDataSize );


/*! Instance definition for MKVParser */
typedef struct _MKVParserInst_t
{
    int32_t                    DMFId;
    DivXMem                    hMem;
    DMFInputStreamHandle       hInputStream;
    MKVCacheHandle             hMKVCache;
    pfnNotifyMKVParser         fnNotify;
    void*                      userData;
    DivXBool                   topLevelClusters;
}MKVParserInst_t;

//typedef MKVParserInst_t* MKVParserHandle;


/*! Casts the MKVParser handle to the MKVParser instance structure

    @param pHandle              (IN)- A handle to the MKVParser object

    @return MKVParserInst_t*   (OUT)   - MKVParser instance pointer
*/
MKVParserInst_t *MKVParserCast( MKVParserHandle handle );

/*! This function aids in parsing the ebml file by reading both the id and data size in 
    one go.

    @param pMKVParser           (IN) - Pointer to the MKVParser instance
    @param elementId           (OUT) - The element id
    @param elementDataSize     (OUT) - Element data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ReadElementIdAndDataSize(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e *elementId, uint64_t *elementDataSize);

/*! 
  Notification callback used by the ParseEBML function to redirect new master list parsing
*/
typedef DivXError ( *pfnNotifyElement2 )         ( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Prints element information to stdout for debugging purposes TODO EC - Should dissappear on release

    @param element           (IN) - Element to print
    @param elementDataSize   (IN) - Data size to print
    @param depth             (IN) - Indentation to print at

    @return void
*/
void PrintElement(MKVElement_t element, uint64_t elementDataSize, int32_t depth);

/*! This function performs all EBML level parsing and hands off elements to be parsed by it's
    callback (fnNotify).

    @param pMKVParser        (IN) - MKVParser handle
    @param offset            (IN) - Offset to parse from
    @param sizeParent        (IN) - Total size of this master list
    @param fnNotify          (IN) - Callback for next level of MKV Parsing

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseEBML( MKVParserInst_t *pMKVParser, uint64_t offset, uint64_t sizeParent, pfnNotifyElement2 fnNotify );

/*! Parses Document children in the MKV format (used as a callback in ParseEBML)

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseDocumentChildren(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize);

/*! Parses EBML children in the MKV format (used as a callback in ParseEBML)

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseEBMLChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Segment children in the MKV format (used as a callback in ParseEBML)

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseSegmentChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Cues children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseCuesChildren(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize);

/*! Parses CuePoint children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseCuePointChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Cue Track Position children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseCueTrackPositionChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Attachments children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Attachments.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseAttachmentsChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses AttachedFile children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Attachments.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseAttachedFileChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Tags children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tags.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTagsChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Tag children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tags.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTagChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Simple Tag children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tags.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseSimpleTagChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Targets children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tags.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTargetsChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Tracks children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tracks.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTracksChildren(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize);

/*! Parses Tracks children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tracks.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTracksDataChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses TracksEntry children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tracks.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseTracksEntryChildren(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses VideoTrackEntry children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tracks.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseVideoTrackEntry( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses AudioTrackEntry children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Tracks.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseAudioTrackEntry( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses SeekHead children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Seekhead.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseSeekHeadChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses SeekChildren children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Seekhead.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseSeekChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses SegmentInfo children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_SegmentInfo.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseSegmentInfoChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Chapters children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Chapters.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseChaptersChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Chapter edition children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Chapters.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseEditionEntryChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Chapter atom children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Chapters.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseChapterAtomChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Chapter display children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Chapters.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseChapterDisplayChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Chapter track children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Chapters.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseChapterTrackChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );



/*! Parses Cues children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseHierarchicalIndexChildren(MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize);

/*! Parses CuePoint children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseHierarchicalIndexPointChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

/*! Parses Cue Track Position children in the MKV format (used as a callback in ParseEBML)

    See MKVParser_Cues.c

    @param pMKVParser        (IN) - MKVParser handle
    @param elementId         (IN) - The EBML element id
    @param elementStartOffset(IN) - The EBML start offset
    @param elementDataOffset (IN) - The EBML element data offset
    @param elementDataSize   (IN) -  The EBML data size

    @return DivXError       (OUT) - DivXError TODO EC - Error code definitions needed
*/
DivXError mkvParser_ParseHierarchicalIndexPositionChildren( MKVParserInst_t *pMKVParser, MKVELEMENT_ID_e elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t* pElemDataSize );

#endif
/* _MKVPARSERPRIVATE_H_ */

