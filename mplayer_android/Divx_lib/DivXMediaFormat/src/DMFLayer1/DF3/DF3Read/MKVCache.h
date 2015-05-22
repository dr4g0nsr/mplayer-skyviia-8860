/*!
    @file
@verbatim
$Id: MKVCache.h 60063 2009-05-11 22:42:32Z snaderi $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVCACHE_H_
#define _MKVCACHE_H_

#include "MKVCache_private.h"

#include "DivXMem.h"
#include "DivXError.h"
#include "DivXBool.h"
#include "DMFTagInfo.h"

/*! Creates a new cache instance

    @param pHandle          (IN/OUT)- A pointer to a MKVCacheHandle that will
                                      receive the instance.
    @param hMem             (IN)    - Memory handle to use.

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_New(MKVCacheHandle *pHandle, DivXMem hMem);

/*! Delete a cache instance

    @param handle           (IN)    - MKVCacheHandle to be deleted

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_Delete(MKVCacheHandle handle);

// -------------------------------------------------------------------------------------
//                   DMF Facing API
// -------------------------------------------------------------------------------------

/*! Checks if track is active for doing read next block

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param tracknum         (IN)    - Track Number
    @param pbActive         (IN)    - Active if DIVX_TRUE

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFIsActiveTrack( MKVCacheHandle handle, uint32_t tracknum, DivXBool* pbActive);

/*! Sets track active for doing read next block

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param tracknum         (IN)    - Track Number
    @param pbActive         (IN)    - Active if DIVX_TRUE

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFSetActiveTrack( MKVCacheHandle handle, DMFBlockType_t blockType, uint32_t tracknum, DivXBool bActive);

/*! Get VideoStreamInfo for the segment

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param tracknum         (IN)    - Track number
    @param info             (IN/OUT)- Pointer to DMFVideoStreamInfo1_t to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetVideoStreamInfo( MKVCacheHandle handle, uint32_t tracknum, DMFVideoStreamInfo1_t *info);

/*! Get AudioStreamInfo for the segment

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param tracknum         (IN)    - Track number
    @param info             (IN/OUT)- Pointer to DMFAudioStreamInfo1_t to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetAudioStreamInfo( MKVCacheHandle handle, uint32_t tracknum, DMFAudioStreamInfo1_t *info);

/*! Get SubtitleStreamInfo for the segment

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param tracknum         (IN)    - Track number
    @param info             (IN/OUT)- Pointer to DMFSubtitleStreamInfo1_t to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetSubtitleStreamInfo( MKVCacheHandle handle, uint32_t tracknum, DMFSubtitleStreamInfo1_t *info);

/*! Get container properties

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pDMFContainerProp    (IN/OUT)- Pointer to DMFContainerProp to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetContainerProp(MKVCacheHandle handle, DMFContainerProp_t* pDMFContainerProp);

/*! Get title properties

    @param handle           (IN)    - Pointer to MKVCacheHandle
    @param index            (IN)    - Title number
    @param pDMFTitleProp    (IN/OUT)- Pointer to DMFTitleProp to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetTitleProp(MKVCacheHandle handle, uint32_t index, DMFTitleProp_t* pDMFTitleProp);

/*! Get title media properties

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param index                (IN)    - Title number
    @param pDMFTitleMediaProp   (IN/OUT)- Pointer to DMFTitleMediaProp to be populated

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetTitleMediaProp(MKVCacheHandle handle, uint32_t index, DMFTitleMediaProp_t* pDMFTitleMediaProp);

/*! Get title index

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param index                (IN)    - Current title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetTitleIndex(MKVCacheHandle handle, uint32_t* index);

/*! Get size of private video codec data

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pDataSize            (IN)    - Size of private codec data
    @param blockType            (IN)    - Track type to query
    @param index                (IN)    - Track number to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetCodecPrivateDataSize(MKVCacheHandle handle, uint32_t* pDataSize, DMFBlockType_t blockType, uint32_t index);

/*! Get private video codec data

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pData                (IN)    - Private codec data
    @param blockType            (IN)    - Track type to query
    @param index                (IN)    - Track number to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetCodecPrivateData(MKVCacheHandle handle, uint8_t* pData, DMFBlockType_t blockType, uint32_t index);


/*! Get size of private video codec data

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pDataSize            (IN)    - Size of private codec data
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
DivXError mkvCache_DMFGetPrivateCodecDataSize(MKVCacheHandle handle, uint32_t* pDataSize, uint32_t index);
*/

/*! Get private video codec data

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pData                (IN)    - Private codec data
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
DivXError mkvCache_DMFGetPrivateCodecData(MKVCacheHandle handle, uint8_t* pData, uint32_t index);
*/


/*! Get private video codec data for SPS

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pDataSize            (IN)    - Private codec data size
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetPrivateCodecDataSeqSize(MKVCacheHandle handle, uint32_t* pDataSize, uint32_t index);

/*! Get private video codec data for SPS

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pData                (IN)    - Private codec data
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetPrivateCodecDataSeq(MKVCacheHandle handle, uint8_t* pData, uint32_t index);

/*! Get private video codec data for PPS

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pDataSize            (IN)    - Private codec data size
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetPrivateCodecDataPicSize(MKVCacheHandle handle, uint32_t* pDataSize, uint32_t index);

/*! Get private video codec data for PPS

    @param handle               (IN)    - Pointer to MKVCacheHandle
    @param pData                (IN)    - Private codec data
    @param index                (IN)    - Video track to query

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetPrivateCodecDataPic(MKVCacheHandle handle, uint8_t* pData, uint32_t index);

/*! Create a new title

    @param handle               (IN)    - Pointer to MKVCacheHandle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_CreateNewTitle(MKVCacheHandle handle);

/*! Sets the current track position in time

    @param handle               (IN) - Pointer to MKVCacheHandle
    @param blockType            (IN) - Block type to set position on
    @param nStream              (IN) - Stream index to set position on
    @param pTime            (IN/OUT) - Time to set position to (returns time actually set)
    @param offset              (OUT) - Offset into the file

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_FindPosition(MKVCacheHandle handle, 
                               DMFBlockType_t blockType, 
                               int32_t nStream, 
                               DivXTime *pTime,
                               uint64_t *offset);

/*! Gets the previous sync point

    @param handle               (IN) - Pointer to MKVCacheHandle
    @param blockType            (IN) - Block type to set position on
    @param curTime              (IN) - Current time
    @param pTime               (OUT) - Time of sync point

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetPreviousSyncPoint(MKVCacheHandle handle,
                                   DMFBlockType_t      blockType,
                                   DivXTime            curTime,
                                   DivXTime           *pTime );

/*! Gets the next sync point

    @param handle               (IN) - Pointer to MKVCacheHandle
    @param blockType            (IN) - Block type to set position on
    @param curTime              (IN) - Current time
    @param pTime               (OUT) - Time of sync point

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNextSyncPoint(MKVCacheHandle handle,
                                   DMFBlockType_t      blockType,
                                   DivXTime            curTime,
                                   DivXTime           *pTime );

/*! Get the number of attachments in a title
    @param handle               (IN) - Pointer to MKVCacheHandle
    @param pCount               (OUT) - Number of attachments

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetAttachmentCount(MKVCacheHandle handle, uint32_t *pCount);

/*! Get the size of an attachment
    @param handle               (IN) - Pointer to MKVCacheHandle
    @param uiAttachIdx          (IN) - Index of attachment to get size of
    @param pSize                (OUT) - Pointer to size 

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetAttachmentSize(MKVCacheHandle handle, uint32_t uiAttachIdx, uint32_t *pSize);

/*! Get an attachments in a title
    @param handle               (IN) - Pointer to MKVCacheHandle
    @param uiAttachIdx          (IN) - Index of attachment to get
    @param pAttachmentInfo      (IN) - Pointer to attachment info to fill

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetAttachmentInfo(MKVCacheHandle handle, uint32_t uiAttachIdx, DMFAttachmentInfo_t *pAttachmentInfo);

/*! Get the number of attachment links for a track
    @param handle               (IN) - Pointer to MKVCacheHandle
    @param blockType            (IN) - Track type 
    @param uiTrackIdx           (IN) - Track index
    @param puiCount              (OUT) - Number of attachment links a track has

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetAttachmentLinkCount(MKVCacheHandle handle, DMFBlockType_t blockType, uint32_t uiTrackIdx, uint32_t *puiCount);

/*! Get the attachment index for a track's attachment link
    @param handle               (IN) - Pointer to MKVCacheHandle
    @param blockType            (IN) - Track type 
    @param uiTrackIdx           (IN) - Track index
    @param pAttachmentLinks     (IN/OUT) - Pointer to array to fill

    @return DivXError           (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetAttachmentLinks(MKVCacheHandle handle, DMFBlockType_t blockType, uint32_t uiTrackIdx, DMFTrackAttachmentLinkInfo_t *pAttachmentLinkInfo);

// -------------------------------------------------------------------------------------------
//                       MKVCache Parser Functions
// -------------------------------------------------------------------------------------------

/*! Get the number of segments

    @param handle          (IN)    - Handle to MKVCache instance

    @return int32_t         (OUT)   - Number of segments
*/
uint32_t mkvCache_GetNumTitles(MKVCacheHandle handle);

/*! Get the size of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param titleIdx        (IN)    - Index of title

    @return uint64_t        (OUT)   - Size of the segment
*/
uint64_t mkvCache_GetSegmentSize(MKVCacheHandle handle, uint32_t titleIdx);

/*! Sets the size of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param titleIdx        (IN)    - Index of title
    @param titleIdx        (IN)    - Index of title

    @return DivXError     (OUT)   - 0, for success or error code
*/
DivXError mkvCache_SetSegmentSize(MKVCacheHandle handle, uint32_t titleIdx, uint64_t segmentSize);

/*! Get the offset for a specific segment number

    @param handle          (IN)    - Handle to MKVCache instance
    @param uint32_t        (IN)    - Title index

    @return uint64_t       (OUT)   - Offset of title (segment)
*/
uint64_t mkvCache_GetTitleOffset(MKVCacheHandle handle, uint32_t titleIdx);

/*! Get the offset for the first cluster

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t        (OUT)   - Offset of first cluster
*/
uint64_t mkvCache_GetClusterStartOffset(MKVCacheHandle handle);

/*! Get the time code scale for the track

    @param handle          (IN)    - Handle to MKVCache instance

    @return float         (OUT)   - Time code scale for the track
*/
float mkvCache_GetTrackTimecodeScale(MKVCacheHandle handle, DMFBlockType_t blockType);

/*! Get the default duration for the specified track

    @param handle          (IN)    - Handle to MKVCache instance
    @param type            (IN)    - The block type
    @param iTrackNum       (IN)    - The track number

    @return uint64_t        (OUT)   - Default duration for the track
*/
uint64_t mkvCache_GetDefaultDuration(MKVCacheHandle handle, DMFBlockType_t type, uint32_t iTrackNum);

/*! Get the number of the segment being held in the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return int32_t        (OUT)   - Segment number
*/
int32_t mkvCache_GetSegmentInCache(MKVCacheHandle handle);

/*! Commits the temporarily stored track to the appropriate place
    in memory

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_CommitTrack(MKVCacheHandle handle);

/*! Sets the current title's time code scale

    @param handle           (IN)    - Handle to MKVCache instance
    @param uiTimecodeScale  (IN)    - The time code scale for this title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTimecodeScale(MKVCacheHandle handle, uint64_t uiTimecodeScale);

/*! Sets the current title duration

    @param handle           (IN)    - Handle to MKVCache instance
    @param dDuration        (IN)    - The duration of the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetDuration(MKVCacheHandle handle, double dDuration);

/*! Sets the current title duration

    @param handle           (IN)    - Handle to MKVCache instance
    @param pdDuration       (OUT)   - The duration of the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetDuration(MKVCacheHandle handle, double* pdDuration);

/*! Sets the current title segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (IN)    - The segment UID of the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetSegmentUID(MKVCacheHandle handle, const unsigned char *uid);

/*! Get the current title segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (OUT)   - The segment UID of the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetSegmentUID(MKVCacheHandle handle, unsigned char *uid);

/*! Sets the title's previous segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (IN)    - The previous segment UID for the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetPrevUID(MKVCacheHandle handle, const unsigned char *uid);

/*! Sets the muxing app

    @param handle           (IN)    - Handle to MKVCache instance
    @param fileName         (IN)    - The muxing application

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetMuxingApp(MKVCacheHandle handle, DivXString* fileName);

/*! Sets the writing app

    @param handle           (IN)    - Handle to MKVCache instance
    @param fileName         (IN)    - The writing application

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetWritingApp(MKVCacheHandle handle, DivXString* fileName);

DivXError mkvCache_SetNextFileName(MKVCacheHandle handle, DivXString* nextFileName);

DivXError mkvCache_SetCodecName(MKVCacheHandle handle, DivXString* codecName);

DivXError mkvCache_SetPrevFileName(MKVCacheHandle handle, DivXString* prevFileName);

/*! Sets the filename

    @param handle           (IN)    - Handle to MKVCache instance
    @param fileName         (IN)    - The filename

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetFileName(MKVCacheHandle handle, DivXString* fileName);

/*! Sets the title's name
    @param handle           (IN)    - Handle to MKVCache instance
    @param titleString      (IN)    - The titles name

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTitle(MKVCacheHandle handle, DivXString* titleString);


/*! Get the title's previous segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (OUT)   - The previous segment UID for the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetPrevUID(MKVCacheHandle handle, unsigned char *uid);

/*! Sets the title's next segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (IN)    - The next segment UID for the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNextUID(MKVCacheHandle handle, const unsigned char *uid);

/*! Get the title's next segment UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uid              (OUT)   - The next segment UID for the title

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNextUID(MKVCacheHandle handle, unsigned char *uid);

/*! Sets the current track time code scale

    @param handle               (IN)    - Handle to MKVCache instance
    @param fTrackTimecodeScale  (IN)    - The time code scale for this track

    @return DivXError           (OUT)   - Returns errors:
                                            DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTrackTimecodeScale(MKVCacheHandle handle, float fTrackTimecodeScale);

/*! Sets the current track type

    @param handle          (IN)    - Handle to MKVCache instance
    @param mkvBlockType    (IN)    - The MKV block type to set to

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentTrackType(MKVCacheHandle handle, MKV_TRACK_TYPE_e mkvBlockType);

/*! Sets the current track language

    @param handle          (IN)    - Handle to MKVCache instance
    @param langString      (IN)    - The language string to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentTrackLanguage(MKVCacheHandle handle, DivXString* langString);

/*! Sets the current track name

    @param handle          (IN)    - Handle to MKVCache instance
    @param trackName       (IN)    - The track name value to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentTrackName(MKVCacheHandle handle, DivXString* trackName);

/*! Sets the UID of the track

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiUID           (IN)    - Unique identifier

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentUID(MKVCacheHandle handle, uint64_t uiUID);

/*! Indicate which segment number is being held in the cache

    @param handle          (IN)    - Handle to MKVCache instance
    @param iSegmentNum     (IN)    - Title number (segment number)

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTitleIndex(MKVCacheHandle handle, int32_t iSegmentNum);

/*! Create a new title for cache

    @param handle           (IN)   - Handle to cache
    @param offset           (IN)   - Offset location of title
    @param offset           (IN)   - Offset location of title data

    @return DivXError      (OUT)   - Error codes
    DIVX_ERR_INVALID_ARG
    DIVX_ERR_INSUFFICIENT_MEM
*/
DivXError mkvCache_AddNewTitle(MKVCacheHandle handle, uint64_t offset, uint64_t dataOffset);

/*! Retrieve the segment which is being held in the cache

    @param handle          (IN)    - Handle to MKVCache instance
    @param piSegmentNum    (IN)    - Segment number pointer

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetTitleIndex(MKVCacheHandle handle, int32_t *piSegmentNum);

/*! Sets the number of audio tracks based on segment index

    @param handle          (IN)    - Handle to MKVCache instance
    @param nSegment        (IN)    - Segment index
    @param iNumAudioTracks (IN)    - Number of audio tracks to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNumAudioTracks(MKVCacheHandle handle, uint32_t iNumAudioTracks);

/*! Gets the number of audio tracks based on segment index

    @param handle           (IN)    - Handle to MKVCache instance
    @param nSegment         (IN)    - Segment index
    @param piNumAudioTracks (IN)    - Number of audio tracks retrieved

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNumAudioTracks(MKVCacheHandle handle, uint32_t* piNumAudioTracks);

/*! Sets the number of Video tracks based on segment index

    @param handle          (IN)    - Handle to MKVCache instance
    @param nSegment        (IN)    - Segment index
    @param iNumVideoTracks (IN)    - Number of Video tracks to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNumVideoTracks(MKVCacheHandle handle, uint32_t iNumVideoTracks);

/*! Gets the number of Video tracks based on segment index

    @param handle           (IN)    - Handle to MKVCache instance
    @param nSegment         (IN)    - Segment index
    @param piNumVideoTracks (IN)    - Number of Video tracks retrieved

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNumVideoTracks(MKVCacheHandle handle, uint32_t* piNumVideoTracks);


/*! Sets the number of Subtitle tracks based on segment index

    @param handle          (IN)    - Handle to MKVCache instance
    @param nSegment        (IN)    - Segment index
    @param iNumVideoTracks (IN)    - Number of Subtitle tracks to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNumSubtitleTracks(MKVCacheHandle handle, uint32_t iNumSubtitleTracks);

/*! Gets the number of Subtitle tracks based on segment index

    @param handle           (IN)    - Handle to MKVCache instance
    @param nSegment         (IN)    - Segment index
    @param piNumVideoTracks (IN)    - Number of Subtitle tracks retrieved

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNumSubtitleTracks(MKVCacheHandle handle, uint32_t* piNumSubtitleTracks);


/*! Sets the number of HD Subtitle tracks based on segment index

    @param handle          (IN)    - Handle to MKVCache instance
    @param nSegment        (IN)    - Segment index
    @param iNumVideoTracks (IN)    - Number of HD Subtitle tracks to set

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNumHDSubtitleTracks(MKVCacheHandle handle, uint32_t iNumHDSubtitleTracks);

/*! Gets the number of HD Subtitle tracks based on segment index

    @param handle           (IN)    - Handle to MKVCache instance
    @param nSegment         (IN)    - Segment index
    @param piNumVideoTracks (IN)    - Number of HD Subtitle tracks retrieved

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNumHDSubtitleTracks(MKVCacheHandle handle, uint32_t* piNumHDSubtitleTracks);

/*! Returns whether or not the current title has a hierarchical index

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXBool      (OUT)    - Returns DIVX_TRUE if the title has a hierarchical index
*/
DivXBool mkvCache_HasHierarchicalIndex(MKVCacheHandle handle);

/*! Set the offset of the hierarchical index of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param offset          (IN)    - Offset of cues

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetHierarchicalIndexOffset(MKVCacheHandle handle, uint64_t offset);

/*! Set the size of the hierarchical index of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param size            (IN)    - Size of cues

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetHierarchicalIndexSize(MKVCacheHandle handle, uint64_t size);

/*! Set the start of the Cues offset

    @param handle          (IN)    - Handle to MKVCache instance
    @param size            (IN)    - Offset of cues

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCuesStartOffset(MKVCacheHandle handle, uint64_t offset);

/*! Set the offset of the start of the cues list of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param offset          (IN)    - Offset of start of cues

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCuesOffset(MKVCacheHandle handle, uint64_t offset);

/*! Set the size of the cues list of the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param size            (IN)    - Size of cues

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCuesSize(MKVCacheHandle handle, uint64_t size);

/*! Set the offset of the first cluster in the segment

    @param handle          (IN)    - Handle to MKVCache instance
    @param offset          (IN)    - Offset of cluster

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetStartCluster(MKVCacheHandle handle, uint64_t offset);

/*! Get the track index for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackID       (IN)    - MKV track ID in cluster
    @param pTrackIndex    (OUT)    - Track type pointer

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFTrackIndex( MKVCacheHandle handle, uint32_t uiTrackID, uint32_t* pTrackIndex);

/*! Get the track index for the last TrackEntry, from the Track UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackUID      (IN)    - MKV track UID (unique identifier)
    @param pTrackIndex    (OUT)    - Track type pointer

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFTrackIndexFromUID( MKVCacheHandle handle, uint64_t uiTrackUID, uint32_t* pTrackIndex);

/*! Get the track type for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackID       (IN)    - MKV track ID in cluster
    @param pBlockType     (OUT)    - Block type

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFTrackType(MKVCacheHandle handle, uint32_t uiTrackID, DMFBlockType_t* pBlockType);

/*! Get the track type for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackUID      (IN)    - MKV track UID in cluster (unique identifier)
    @param pBlockType     (OUT)    - Block type

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFTrackTypeFromUID(MKVCacheHandle handle, uint32_t uiTrackUID, DMFBlockType_t* pBlockType);

/*! Get the track type for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param titleIdx        (IN)    - Title index
    @param uiTrackUID      (IN)    - MKV track UID in cluster (unique identifier)
    @param pBlockType     (OUT)    - Block type

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFTrackIndexFromTitleAndUID( MKVCacheHandle handle, uint32_t titleIdx, uint64_t uiTrackUID, uint32_t* pTrackIndex);

/*! Retrieve the time code scale for the current track

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Time code scale
*/
uint64_t mkvCache_GetTimecodeScale(MKVCacheHandle handle);

/*! Retrieve the hierarchical index offset from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Hierarchical index offset
*/
uint64_t mkvCache_GetHierarchicalIndexOffset(MKVCacheHandle handle);

/*! Retrieve the hierarchical index size from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Hierarchical index size
*/
uint64_t mkvCache_GetHierarchicalIndexSize(MKVCacheHandle handle);

/*! Retrieve the start of the cues offset from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Cues list offset
*/
uint64_t mkvCache_GetCuesStartOffset(MKVCacheHandle handle);

/*! Retrieve the cues offset from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Cues list offset
*/
uint64_t mkvCache_GetCuesOffset(MKVCacheHandle handle);

/*! Retrieve the cues size from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Cues list size
*/
uint64_t mkvCache_GetCuesSize(MKVCacheHandle handle);

/*! Retrieve the offset of where segment (title) data begins

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Title data offset
*/
uint64_t mkvCache_GetTitleDataOffset(MKVCacheHandle handle);

/*! Retrieve the start cluster offset from the cache

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint64_t       (OUT)   - Start cluster offset
*/
uint64_t mkvCache_GetStartClusterOffset(MKVCacheHandle handle);

/*! Set the seek point

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetSeekPoint(MKVCacheHandle handle);

/*! Set the seek position

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiSeekPos       (IN)   - The seek position

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetSeekPosition(MKVCacheHandle handle, uint64_t uiSeekPos);

/*! Set the seek id

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiSeekPos       (IN)   - The seek id

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetSeekID(MKVCacheHandle handle, MKVELEMENT_ID_e uiSeekID);

/*! Set the tag

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTag(MKVCacheHandle handle);

/*! Set the simple tag

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetSimpleTag(MKVCacheHandle handle);

/*! Set the simple tag item

    @param handle          (IN)   - Handle to MKVCache instance
    @param type            (IN)   - The MKV Element for this item
    @param offset          (IN)   - Offset into the file
    @param size            (IN)   - Size in the file
    @param val             (IN)   - Value of the simple tag item (only applies to some simple tag items)

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetSimpleTagItem(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint8_t val);

/*! Set the targets

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTargetItem(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint32_t val);
#if 0
/*! Set a general tag item

    @param handle          (IN)   - Handle to MKVCache instance
    @param type            (IN)   - The MKVELEMENT_ID for this tag entry
    @param offset          (IN)   - Location in the file for this item
    @param size            (IN)   - Size of this item in the file

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagData(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint32_t size);

/*! Set the tag name

    @param handle          (IN)   - Handle to MKVCache instance
    @param psTagName       (IN)   - Tag name

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagName(MKVCacheHandle handle, DivXString *psTagName);

/*! Set the tag lang

    @param handle          (IN)   - Handle to MKVCache instance
    @param psTagLang       (IN)   - Tag lang

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagLanguage(MKVCacheHandle handle, DivXString *psTagLang);

/*! Set the tag original

    @param handle          (IN)   - Handle to MKVCache instance
    @param bTagOrig        (IN)   - Tag original

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagOriginal(MKVCacheHandle handle, DivXBool bTagOrig);

/*! Set the tag string

    @param handle          (IN)   - Handle to MKVCache instance
    @param psTagString     (IN)   - Tag string

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagString(MKVCacheHandle handle, DivXString *psTagString);

/*! Set the tag binary

    @param handle          (IN)   - Handle to MKVCache instance
    @param pucTagBin       (IN)   - Tag binary

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagBinary(MKVCacheHandle handle, unsigned char *pucTagBin);
#endif
/*! Sets the Target Type Val

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiTargetTypeVal (IN)   - Target type val

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTargetTypeValue(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint8_t val);

/*! Sets the Target Type

    @param handle          (IN)   - Handle to MKVCache instance
    @param psTargetType    (IN)   - Target type string

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTargetType(MKVCacheHandle handle, DivXString *psTargetType);

/*! Sets the Track UID

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiTrackUID      (IN)   - Track UID

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetTagTrackUID(MKVCacheHandle handle, uint64_t uiTrackUID);

/*! Sets the Edition UID

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiEditionUID    (IN)   - Edition UID

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetEditionUID(MKVCacheHandle handle, uint64_t uiEditionUID);

/*! Sets the Chapter UID

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiChapterUID    (IN)   - Chapter UID

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetChapterUID(MKVCacheHandle handle, uint64_t uiChapterUID);

/*! Sets the Attach UID

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiAttachUID    (IN)   - Attach UID

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetAttachmentUID(MKVCacheHandle handle, uint64_t uiAttachUID);

/*! Sets the Cue Point

    @param handle               (IN)   - Handle to MKVCache instance
    @param bHierarchicalIndex   (IN)   - True if this is a hierarchical index
    @param cuePointOffset       (IN)   - Offset location of the cue point

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCuePoint(MKVCacheHandle handle, DivXBool bHierarchicalIndex, uint64_t cuePointOffset);

/*! Sets the Track Position

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCueTrackPosition(MKVCacheHandle handle);

/*! Sets the Cue Time

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiCueTime       (IN)   - Cue Time

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCueTime(MKVCacheHandle handle, uint64_t uiCueTime);

/*! Sets the Cue Block Num

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiCueBlockNum   (IN)   - Cue Block Num

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCueBlockNumber(MKVCacheHandle handle, uint64_t uiCueBlockNum);

/*! Sets the Cue Track

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiCueTrack      (IN)   - Cue Track

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCueTrack(MKVCacheHandle handle, uint64_t uiCueTrack);

/*! Sets the Cue Cluster Position

    @param handle          (IN)   - Handle to MKVCache instance
    @param uiCueClusterPos (IN)   - Cue Cluster Position

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_SetCueClusterPosition(MKVCacheHandle handle, uint64_t uiCueClusterPos);

/*! Set an attachment link for a track entry

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiUID           (IN)    - UID of the attachment

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentLink(MKVCacheHandle handle, uint64_t uiUID);

// -------------------------------------------------------------------------------------------
//                       MKVCache Video Track Functions
// -------------------------------------------------------------------------------------------

/*! Set the default duration for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param type            (IN)    - default duration value (block period, in nanosec)

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetDefaultDuration(MKVCacheHandle handle, uint64_t uiDuration);

/*! Set the codec private data for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param size            (IN)    - Size of data
    @param data            (IN)    - Pointer to codec private data

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCodecPrivateData(MKVCacheHandle handle, uint32_t size, const uint8_t *data);

/*! Set the codec ID string for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param psTagName       (IN)    - codec ID string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCodecID(MKVCacheHandle handle, DivXString *psCodecID);

/*! Set the codec ID string for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param dsCodecID       (IN)    - codec ID string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCodecID(MKVCacheHandle handle, DivXString *psCodecID);

/*! Gets the CodecID string.

    @param pTrack          (IN)     - Track to get the codec id from
    @param dsCodecID       (IN/OUT) - string to copy codec ID string into
    @param buffSize        (IN)     - Size of string buffer

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetCodecID(MKVTrack_t *pTrack, DivXString *dsCodecID, int32_t buffSize);

/*! Set the media description ID for the last TrackEntry

    @param handle          (IN)    - Handle to MKVCache instance
    @param psTagName       (IN)    - codec ID string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetMediaDescriptions(MKVCacheHandle handle, uint64_t mediaDescriptionId);

// -------------------------------------------------------------------------------------------
//                       MKVCache Video Track Functions
// -------------------------------------------------------------------------------------------

/*! Set video track pixel width

    @param handle          (IN)    - Handle to MKVCache instance
    @param pixelWidth      (IN)    - Pixel width

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetVideoTrackPixelWidth(MKVCacheHandle handle, uint16_t pixelWidth);

/*! Set video track pixel height

    @param handle          (IN)    - Handle to MKVCache instance
    @param pixelHeight     (IN)    - Pixel height

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetVideoTrackPixelHeight(MKVCacheHandle handle, uint16_t pixelHeight);

/*! Set video track display width

    @param handle          (IN)    - Handle to MKVCache instance
    @param displayWidth      (IN)    - Display width

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetVideoTrackDisplayWidth(MKVCacheHandle handle, uint16_t displayWidth);

/*! Set video track display height

    @param handle          (IN)    - Handle to MKVCache instance
    @param displayHeight     (IN)    - Display height

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetVideoTrackDisplayHeight(MKVCacheHandle handle, uint16_t displayHeight);

/*! Set interlace flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param bSet            (IN)    - Value to set to

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetFlagInterlaced(MKVCacheHandle handle, DivXBool bSet);

// -------------------------------------------------------------------------------------------
//                       MKVCache Audio Track Functions
// -------------------------------------------------------------------------------------------

/*! Set audio track num channels parameter

    @param handle          (IN)    - Handle to MKVCache instance
    @param data            (IN)    - Num Channels

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAudioTrackNumChannels(MKVCacheHandle handle, uint32_t numChannels);

/*! Set audio track sampling frequency parameter

    @param handle          (IN)    - Handle to MKVCache instance
    @param samplingFreq    (IN)    - Sampling rate

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAudioTrackSamplingFreq(MKVCacheHandle handle, float samplingFreq);

/*! Set audio track output sampling frequency parameter

    @param handle          (IN)    - Handle to MKVCache instance
    @param samplingFreq    (IN)    - Sampling rate

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAudioTrackOutputSamplingFreq(MKVCacheHandle handle, float samplingFreq);


/*! Set audio track bits per sample paramater

    @param handle          (IN)    - Handle to MKVCache instance
    @param data            (IN)    - bits per sample

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAudioTrackBitsPerSample(MKVCacheHandle handle, uint32_t bitsPerSample);

//-----------------------------------------------------------------------
//                        Seek Head Data
//-----------------------------------------------------------------------

/*! Get seek position based on id and index

    @param handle          (IN)    - Handle to SeekList
    @param pId            (OUT)    - ID returned for this index
    @param index           (IN)    - Index of id to get

    @return uint64_t      (OUT)    - Position in file (0 is invalid)
*/
uint64_t mkvCache_GetSeekPos(SeekListHandle handle, MKVELEMENT_ID_e* pId, uint32_t index);

/*! Retrieves the DRM header from the cache

    @param handle          (IN)    - Handle to SeekList
    @param pData          (OUT)    - DRM data
    @param pLen           (OUT)    - Size of DRM data

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetDrmHeader(MKVCacheHandle handle, uint8_t *pData, uint32_t *pLen);


//-----------------------------------------------------------------------
//                        Chapter Data
//-----------------------------------------------------------------------

/*! Create a chapter edition entry

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_AddChapterEditionEntry(MKVCacheHandle handle);

/*! Set chapter edition UID parameter

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiEditionUID    (IN)    - UID

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterEditionUID(MKVCacheHandle handle, uint64_t uiEditionUID);

/*! Set chapter edition hidden flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiFlagHidden    (IN)    - 0 = not hidden, 1 = hidden

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterEditionFlagHidden(MKVCacheHandle handle, uint64_t uiFlagHidden);

/*! Set chapter edition default flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiFlagDefault   (IN)    - 0 = not default, 1 = default

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterEditionFlagDefault(MKVCacheHandle handle, uint64_t uiFlagDefault);

/*! Set chapter edition ordered flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiFlagOrdered   (IN)    - 0 = not ordered, 1 = ordered

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterEditionFlagOrdered(MKVCacheHandle handle, uint64_t uiFlagOrdered);

/*! Get properties of the current edition (playlist)

    @param handle          (IN)    - Handle to MKVCache instance
    @param pPlaylistProp   (IN)    - pointer to receive the playlist properties

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetEditionProperties(MKVCacheHandle handle, DMFPlaylistProp_t *pPlaylistProp);

/*! Get the number of editions (playlists) in the title

    @param handle          (IN)    - Handle to MKVCache instance
    @param piNumEditions   (IN)    - pointer to receive the number of editions

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetNumEditions(MKVCacheHandle handle, uint32_t* piNumEditions);

/*! Create a chapter atom entry

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_AddChapterAtom(MKVCacheHandle handle, MKVChapterItem_t *pParentChapter, MKVChapterItem_t **ppNewChapter);

/*! Set chapter atom UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiChapterUID    (IN)    - UID

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterUID2(MKVCacheHandle handle, uint64_t uiChapterUID);

/*! Set chapter atom start time

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTimeStart     (IN)    - starting time

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterTimeStart(MKVCacheHandle handle, uint64_t uiTimeStart);

/*! Set chapter atom end time

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTimeEnd       (IN)    - ending time

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterTimeEnd(MKVCacheHandle handle, uint64_t uiTimeEnd);

/*! Set chapter atom hidden flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiFlagHidden    (IN)    - 0 = not hidden, 1 = hidden

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterFlagHidden(MKVCacheHandle handle, uint64_t uiFlagHidden);

/*! Set chapter atom enabled flag

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiFlagEnabled   (IN)    - 0 = not enabled, 1 = enabled

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterFlagEnabled(MKVCacheHandle handle, uint64_t uiFlagEnabled);

/*! Set chapter atom segment UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiSegmentUID    (IN)    - UID

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterSegmentUID(MKVCacheHandle handle, uint64_t uiSegmentUID);

/*! Set chapter atom segment edition UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiEditionUID    (IN)    - UID

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterSegmentEditionUID(MKVCacheHandle handle, uint64_t uiEditionUID);

/*! Get the current chapter for the current title

    @param handle                (IN)    - Handle to MKVCache instance

    @return MKVChapterItem_t*   (OUT)    - pointer to a chapter structure
*/
MKVChapterItem_t* mkvCache_GetCurrentChapter(MKVCacheHandle handle);

/*! Get the next chapter for the current title

    @param handle                (IN)    - Handle to MKVCache instance

    @return MKVChapterItem_t*   (OUT)    - pointer to a chapter structure
*/
MKVChapterItem_t* mkvCache_GetNextChapter(MKVCacheHandle handle);

/*! Get the previous chapter for the current title

    @param handle                (IN)    - Handle to MKVCache instance

    @return MKVChapterItem_t*   (OUT)    - pointer to a chapter structure
*/
MKVChapterItem_t* mkvCache_GetPreviousChapter(MKVCacheHandle handle);

/*! Get the time offset for the current chapter

    @param handle                (IN)    - Handle to MKVCache instance

    @return int64_t             (OUT)    - time offset for current chapter
*/
int64_t mkvCache_GetChapterTimeOffset(MKVCacheHandle handle);

/*! Create a chapter display entry

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddChapterDisplay(MKVCacheHandle handle);

/*! Set chapter display string

    @param handle          (IN)    - Handle to MKVCache instance
    @param pChapterString  (IN)    - chapter string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterString(MKVCacheHandle handle, DivXString *pChapterString);

/*! Set chapter display language

    @param handle           (IN)    - Handle to MKVCache instance
    @param pChapterLanguage (IN)    - chapter language

    @return DivXError       (OUT)   - Returns errors:
                                         DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterLanguage(MKVCacheHandle handle, DivXString *pChapterLanguage);

/*! Set chapter display country

    @param handle           (IN)    - Handle to MKVCache instance
    @param pChapterCountry  (IN)    - chapter country

    @return DivXError       (OUT)   - Returns errors:
                                         DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterCountry(MKVCacheHandle handle, DivXString *pChapterCountry);


DivXError mkvCache_GetChapterDisplayCount(MKVCacheHandle handle, uint32_t *pCount);

//DivXError mkvCache_GetChapterDisplay(MKVCacheHandle handle, uint32_t nDisplay, MKVChapterDisplayItem_t *pCurrDisplay);
DivXError mkvCache_GetChapterDisplay(MKVCacheHandle handle, uint32_t nDisplay, MKVChapterDisplayItem_t **ppChapterDisplay);


/*! Create a chapter track entry

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddChapterTrack(MKVCacheHandle handle);

/*! Set chapter track number

    @param handle           (IN)    - Handle to MKVCache instance
    @param uiTrackNum       (IN)    - track number

    @return DivXError       (OUT)   - Returns errors:
                                         DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterTrackNumber(MKVCacheHandle handle, uint64_t uiTrackNum);

/*! Get the properties of the current chapter

    @param handle           (IN)    - Handle to MKVCache instance
    @param pChapterProp     (OUT)   - pointer to receive the chapter properties

    @return DivXError       (OUT)   - Returns errors:
                                         DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetChapterProperties(MKVCacheHandle handle, DMFChapterProp_t *pChapterProp);

/*! Get the properties of the current chapter

    @param handle           (IN)    - Handle to MKVCache instance
    @param nChapter         (IN)    - Chapter num.
    @param pChapterProp     (OUT)   - pointer to receive the chapter properties

    @return DivXError       (OUT)   - Returns errors:
                                         DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetChapterNonOrderedProperties(MKVCacheHandle handle, int32_t nChapter, DMFChapterProp_t *pChapterProp);

/*! Set the current chapter edition

    @param handle                    (IN)    - Handle to MKVCache instance
    @param currChapterEditionIdx     (IN)    - chapter edition index

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentChapterEdition(MKVCacheHandle handle, int32_t currChapterEditionIdx);

/*! Sets the current ordered chapter edition

    @param handle                    (IN)    - Handle to MKVCache instance
    @param currChapterEditionIdx     (IN)    - chapter edition index

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentOrderedChapterEdition(MKVCacheHandle handle, int32_t currChapterEditionIdx);

/*! Set the current chapter edition to the default edition

    @param handle                    (IN)    - Handle to MKVCache instance

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetDefaultChapterEdition(MKVCacheHandle handle);

/*! Set the current chapter in the current edition

    @param handle                    (IN)    - Handle to MKVCache instance
    @param currChapterIdx            (IN)    - chapter index

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentChapter(MKVCacheHandle handle, int32_t currChapterIdx);

/*! Set the next chapter as the current chapter

    @param handle                    (IN)    - Handle to MKVCache instance

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetNextChapterAsCurrent(MKVCacheHandle handle);

/*! Set the previous chapter as the current chapter

    @param handle                    (IN)    - Handle to MKVCache instance

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetPrevChapterAsCurrent(MKVCacheHandle handle);

/*! Set the current chapter based on the specified relative time

    @param handle                    (IN)    - Handle to MKVCache instance
    @param relativeTime              (IN)    - the relative time

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetChapterFromRelativeTime(MKVCacheHandle handle, DivXTime relativeTime);

/*! Set the current chapter display in the current chapter

    @param handle                    (IN)    - Handle to MKVCache instance
    @param currChapterDisplayIdx     (IN)    - chapter display index

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentChapterDisplay(MKVCacheHandle handle, int32_t currChapterDisplayIdx);

/*! Set the current chapter track in the current chapter

    @param handle                    (IN)    - Handle to MKVCache instance
    @param currChapterTrackIdx       (IN)    - chapter track index

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCurrentChapterTrack(MKVCacheHandle handle, int32_t currChapterTrackIdx);

/*! Returns whether or not the file is using an index

    @param handle                    (IN)    - Handle to MKVCache instance

    @return DivXBool                (OUT)    - DIVX_TRUE/DIVX_FALSE
*/
DivXBool mkvCache_HasIndex(MKVCacheHandle handle);

/*! Set the session to use hierarchical index

    @param handle                    (IN)    - Handle to MKVCache instance
    @param bUseHierarchicalIndex     (IN)    - Set to use the hierarchical index (DIVX_TRUE)

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetUseHierarchicalIndex(MKVCacheHandle handle, DivXBool bUseHierarchicalIndex);

/*! Get whether the session should use a hierarchical index

    @param handle                    (IN)    - Handle to MKVCache instance

    @return DivXBool                (OUT)    - Returns DIVX_TRUE if session will use a hierarchical index
*/
DivXBool mkvCache_GetUseHierarchicalIndex(MKVCacheHandle handle);

/*! Set the ordered edition allowed flag in the current title

    @param handle                    (IN)    - Handle to MKVCache instance
    @param bEnableOrderedEditions    (IN)    - Set to enable ordered editions (DIVX_TRUE)

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetOrderedEditionFlag(MKVCacheHandle handle, DivXBool bEnableOrderedEditions);

/*! Set the gap size for the index

    @param handle                    (IN)    - Handle to MKVCache instance
    @param gapMs                     (IN)    - Set the gap size

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetIndexGap(MKVCacheHandle handle, uint64_t gapMs);

/*! Get the gap size for the index

    @param handle                    (IN)    - Handle to MKVCache instance
    @param *gapMs                    (IN)    - Gap size

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetIndexGap(MKVCacheHandle handle, uint64_t* gapMs);

/*! Sets the delace state

    @param handle                    (IN)    - Handle to MKVCache instance
    @param bDelace                   (IN)    - The delacing state (DIVX_TRUE/DIVX_FALSE)

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetDelace(MKVCacheHandle handle, DivXBool bDelace);

/*! Gets the delace state

    @param handle                    (IN)    - Handle to MKVCache instance
    @param pbDelace                 (OUT)    - Get the delacing state (DIVX_TRUE/DIVX_FALSE)

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetDelace(MKVCacheHandle handle, DivXBool* pbDelace);


/*! Get the combine fields state (will fields be returned in a single ReadNextBlock() call?)

    @param handle                    (IN)    - Handle to MKVCache instance
    @param pbCombine                (OUT)    - Get the combine fields state

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetCombineFields(MKVCacheHandle handle, DivXBool *pbCombine);

/*! Sets a track so interlaced fields are combined for each frame before being returned

    @param handle                    (IN)    - Handle to MKVCache instance
    @param bCombine                  (IN)    - Set DIVX_TRUE to combine interlaced fields

    @return DivXError                (OUT)   - Returns errors:
                                                  DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetCombineFields(MKVCacheHandle handle, DivXBool bCombine);

/*! Get if this track is interlaced

    @param handle          (IN)    - Handle to MKVCache instance
    @param pbInterlaced    (OUT)   - DIVX_TRUE if track is interlaced

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetIsInterlaced(MKVCacheHandle handle, DivXBool *pbInterlaced);

/*! Set if this track is interlaced

    @param handle          (IN)    - Handle to MKVCache instance
    @param bInterlaced     (IN)    - DIVX_TRUE is track is interlaced

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetIsInterlaced(MKVCacheHandle handle, DivXBool bInterlaced);

//-----------------------------------------------------------------------
//                        Attachment Data
//-----------------------------------------------------------------------

/*! Create an attachment entry

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_AddAttachmentEntry(MKVCacheHandle handle);

/*! Add an attachment entry to the attachment list

    @param handle          (IN)   - Handle to MKVCache instance

    @return DivXError     (OUT)   - DivXError TODO EC - Add error codes
*/
DivXError mkvCache_CommitAttachmentEntry(MKVCacheHandle handle);

/*! Set attachment's filename

    @param handle           (IN)    - Handle to MKVCache instance
    @param psFilename       (IN)    - Pointer to filename string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentFilename(MKVCacheHandle handle, DivXString *psFilename);

/*! Set attachment's file description

    @param handle           (IN)    - Handle to MKVCache instance
    @param psFileDesc       (IN)    - Pointer to description string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentFileDescription(MKVCacheHandle handle, DivXString *psFileDesc);

/*! Set attachment's MIME type

    @param handle           (IN)    - Handle to MKVCache instance
    @param psMimeType       (IN)    - Pointer to MIME type string

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentMimeType(MKVCacheHandle handle, DivXString *psMimeType);

/*! Set attachment's file size

    @param handle           (IN)    - Handle to MKVCache instance
    @param uiSize           (IN)    - File size

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentFileSize(MKVCacheHandle handle, uint32_t uiSize);

/*! Set attachment's file offset

    @param handle           (IN)    - Handle to MKVCache instance
    @param uiSize           (IN)    - File offset

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentFileOffset(MKVCacheHandle handle, uint64_t uiOffset);

/*! Set attachment's file UID

    @param handle           (IN)    - Handle to MKVCache instance
    @param uiUID            (IN)    - The file's UID

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetAttachmentFileUID(MKVCacheHandle handle, uint64_t uiUID);

//-----------------------------------------------------------------------
//                        Tags / Metadata
//-----------------------------------------------------------------------

/*! Set the tag iterator to the first tag

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTagFirst(MKVCacheHandle handle);

/*! Set the tag iterator to the next tag, returns failure if no more tags

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTagNext(MKVCacheHandle handle);

/*! Get the tag information for the current tag

    @param handle           (IN)    - Handle to MKVCache instance
    @param pTagInfo         (OUT)   - Retrieves the tag information:
                                        size - in bytes
                                        type - data type
                                        title - title this tag references
                                        chapter - chapter this tag references
                                        etc...

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetTagInfo(MKVCacheHandle handle, DF3TagsInfo_t* tagInfo);

/*!
    Retrieve the tag size and other information

    @param handle         (IN) -  A handle to the instance

    @param queryID        (IN) -  Target type to retrieve

    @param index          (IN) -  Index of target type to retrieve

    @param value          (OUT) - Value of target

    @return DivXError
*/

DivXError mkvCache_GetTargetByTypeIdx(MKVCacheHandle handle, int32_t queryID, int32_t index, int32_t *value);

/*! Retrieves the tag

    @param handle            (IN)    - Handle to MKVCache instance
    @param var              (OUT)    - Returns the tag data specified by the tag info

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_GetTag(MKVCacheHandle handle, DMFVariant* var);

/*! Retrieves the media description

    @param handle            (IN)  - Handle to MKVCache instance
    @param pMediaDescription (OUT) - Returns the media description
    @param blockType          (IN) - the block type required
    @param index              (IN) -  the index
    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DMFGetMediaDescription(MKVCacheHandle handle, uint64_t* pMediaDescription,  DMFBlockType_t blockType, uint32_t index);


//-----------------------------------------------------------------------
//                        Trick Track
//-----------------------------------------------------------------------

/*! Returns whether or not a video track has a companion visual search track

    @param handle            (IN)    - Handle to MKVCache instance
    @param index             (IN)    - Index of the video track
    @param pTrackProp        (IN)    - The properties on this track (master/trick)

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_TrackProp(MKVCacheHandle handle, uint32_t iVid, DMFTrackProp_t* pTrackProp);


#endif //MKVCACHE
