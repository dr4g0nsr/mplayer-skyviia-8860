/*!
    @file
@verbatim
$Id: MKVCache_private.h 60063 2009-05-11 22:42:32Z snaderi $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVCACHEPRIVATE_H_
#define _MKVCACHEPRIVATE_H_

#include "DivXMem.h"
#include "DivXError.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"
#include "DMFAttachmentInfo.h"
#include "DMFProperties.h"
#include "DMFBlockType.h"
#include "DF3/DF3Common/MKVTypes.h"
#include "DF3/DF3Common/MKVParserElements.h"
#include "DivXLinkedList.h"
#include "DF3Index.h"
#include "DF3Lacing.h"
#include "DF3HierarchicalIndex.h"
#include "DF3TagInfo.h"

#define MAX_TRACK_LACE_ENTRIES 256

/*
@NOTE
-Instance structure is never accessed directly except direct sets/gets.
-Private gets/sets of MKVCacheHandle return the value.
-Public Count gets ie mkvCache_GetNumMKVTracks returns count as int32_t not error code.
-Public Offset gets returns offset as uint64_t not error code.
*/

/*! Chapter data */

#define MKVCHAPTER_FLAGS_HIDDEN             0x01
#define MKVCHAPTER_FLAGS_ENABLED            0x02
#define MKVCHAPTER_FLAGS_DEFAULT            0x04
#define MKVCHAPTER_FLAGS_ORDERED            0x08

typedef LinkedList_t MKVChapterDisplayList_t;
typedef MKVChapterDisplayList_t*  MKVChapterDisplayListHandle;

/*! An item of the chapterDisplayList in chapterItem */
typedef struct _MKVChapterDisplayItem_t
{
    DivXString *pChapterString;
    DivXString *pChapterLanguage;
    DivXString *pChapterCountry;
} MKVChapterDisplayItem_t;

typedef LinkedList_t MKVChapterTrackList_t;
typedef MKVChapterTrackList_t*  MKVChapterTrackListHandle;

/*! An item of the chapterTrackList in chapterItem */
typedef struct _MKVChapterTrackItem_t
{
    uint64_t uiTrackNum;
} MKVChapterTrackItem_t;

typedef LinkedList_t MKVChapterList_t;
typedef MKVChapterList_t*  MKVChapterListHandle;

/*! An item of the chapterList in editionEntry or chapterItem */
typedef struct _MKVChapterItem_t
{
    uint64_t uiUID;
    uint32_t flags;

    uint64_t uiTimeStart;
    uint64_t uiTimeEnd;
    int64_t iTimeOffset;

    uint64_t uiSegmentUID;
    uint64_t uiSegmentEditionUID;

    uint32_t currChapterDisplayIdx;
    MKVChapterDisplayListHandle chapterDisplayList;

    uint32_t currChapterTrackIdx;
    MKVChapterTrackListHandle chapterTrackList;

    uint32_t currChapterIdx;
    MKVChapterListHandle chapterList;
} MKVChapterItem_t;

/*! An item of the chapterEditionList in Title */
typedef struct _MKVChapterEditionItem_t
{
    uint64_t uiUID;
    uint32_t flags;

    double duration;
    uint32_t numChapters;
    uint32_t currChapterIdx;

    MKVChapterItem_t* pCurrChapter;

    MKVChapterListHandle chapterList;
} MKVChapterEditionItem_t;

typedef LinkedList_t MKVChapterEditionList_t;
typedef MKVChapterEditionList_t*  MKVChapterEditionListHandle;

/*! Video specific track data */
typedef struct _MKVVideoTrack
{
    DivXBool pixelWidthFound;
    uint16_t pixelWidth;
    DivXBool pixelHeightFound;
    uint16_t pixelHeight;
    DivXBool displayWidthFound;
    uint16_t displayWidth;
    DivXBool displayHeightFound;
    uint16_t displayHeight;
    DivXBool flagInterlacedFound;
    DivXBool flagInterlaced;
}MKVVideoTrack_t;

/*! Audio specific track data */
typedef struct _MKVAudioTrack
{
    DivXBool nChannelsFound;
    uint32_t nChannels;
    DivXBool samplingFreqFound;
    uint32_t samplingFreq;
    DivXBool samplingFreqOutputFound;
    uint32_t samplingFreqOutput;
    DivXBool wBitsPerSampleFound;

    uint32_t wFormatTag;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint32_t nBlockAlign;
    uint32_t wBitsPerSample;
    uint32_t delay;
    uint32_t cbSize;
    uint8_t extra[MAX_EXTRA_DATA_SIZE];
}MKVAudioTrack_t;

/*! Subtitle specific track data */
typedef struct _MKVVSubTrack
{
    uint32_t temp;
}MKVSubTrack_t;

#define MAX_TIME_CODE_ARRAY 4

/*! An item of the attachmentList in Title*/
typedef struct _AttachmentLinkItem_t
{
    uint64_t        uiUID;
} AttachmentLinkItem_t;

typedef LinkedList_t AttachmentLinkList_t;
typedef AttachmentLinkList_t* AttachmentLinkListHandle;


/*! MKVTracks stores all track types under a given title */
typedef struct _MKVTracks_t
{
    DivXBool                bActive;       //! Is this track active or not
    uint32_t                uiTrackIndex;  //! The MKV uiTrackIndex for this track type
    uint32_t                uiTrackID;     //! The MKV uiTrackID for this track
    uint64_t                uiUID;         //! The unique identifier for this track
    DivXBool                bDuplicateUID; //! Screens for multiple unique identifiers
    
    DMFBlockType_t          trackType;      //! Track type for this track (may come at any time in track entry)
    MKV_CODEC_TYPE_e        codecType;      //! The codec type for this track
    uint16_t                iCodecDataSize; //! Found the codec data
    uint8_t                *codecPrivateData;  //! Maximum codec data we can store before knowing the codec type

    DivXBool                bInterlaced;           //! Is this track interlaced?
    DivXBool                bCombineFields;        //! Combine interlaced fields when reading this track
    uint32_t                uiBitsToFieldData;     //! Number of bits to skip in the slice_header to get from pic_parameter_set_id to field_pic_flag

    DivXBool                bLacing;               //! Lacing is enabled for this track
    DivXBool                bFoundDefaultDuration; //! Detects whether default duration is set
    uint64_t                uiDefaultDuration;     //! Default duration for this track

    DivXBool                bFoundTrackTimecodeScale; //! Detects existence of timecode scale data
    float                   fTrackTimecodeScale;      //! cluster/block timecodes get multiplied by this

    DivXBool                videoTrackFound;        //! Detects video track specific data exists
    MKVVideoTrack_t         videoTrack;             //! Video track specific data
    DivXBool                audioTrackFound;        //! Detects audio track specific data exists
    MKVAudioTrack_t         audioTrack;             //! Audio track specific data
    DivXBool                subTrackFound;          //! Detects sub track specific data exists
    MKVSubTrack_t           subTrack;               //! Subtitle track specific data

    uint64_t                uiLastOffset;           //! Keeps track of byStream track reading

    DivXTime                uiLastClusterTimecode; //! Keeps track of byStream track reading last cluster time
    uint64_t                uiLastClusterEnd;      //! Keeps track of byStream track reading last cluster end position
    uint64_t                uiTimecodeScale;       //! Keeps track of time code scale by track

    // The following are for calculating default duration based on the gap
    uint64_t                timeCodeArrayIdx;      //! Keep track of the timeCodes being read in
    uint64_t                timeCodeArray[MAX_TIME_CODE_ARRAY];      //! Stores data for calculating duration from block gaps

    AttachmentLinkListHandle attachmentLinkList;

    // Object for retrieving laced blocks
    DF3LacingHandle         hLacing;

    // Type of track this is (trick/master/etc.)
    DMFTrackType_e          dmfTrackType;

    // Trick Track UID
    uint64_t                trickTrackUID;

    // Trick Track Filename
    DivXString*             trickTrackFilename;

    // Master Track UID
    uint64_t                masterTrackUID;

    // Master Filename
    DivXString*             masterFilename;

    // Trick Track Segment UID
    CDivXUUID                trickSegmentUID;

    // Master Segment UID
    CDivXUUID                masterSegmentUID;

    // Media Description
    uint64_t                mediaDescription;

    // Track Name
    DivXString*             trackName;

    // Track Language
    DivXString*             trackLang;

    DivXString*             codecName;

}MKVTrack_t;

/*! An item of the seekList in Title */
typedef struct _SeekItem_t
{
    MKVELEMENT_ID_e seekID;
    uint64_t        seekPos;
} SeekItem_t;

typedef LinkedList_t SeekList_t;
typedef SeekList_t*  SeekListHandle;

/*! An item of the attachmentList in Title*/
typedef struct _AttachmentItem_t
{
    uint64_t        uiUID;
    DivXString     *psFilename;
    DivXString     *psFileDesc;
    DivXString     *psMimeType;
    uint32_t        uiFileSize;
    uint8_t        *pFileData;
    uint64_t        uiOffset;
} AttachmentItem_t;

typedef LinkedList_t AttachmentList_t;
typedef AttachmentList_t* AttachmentListHandle;

typedef struct _MKVTitle_t
{
    // Number of track types found (if found)
    DivXBool numVideoMediaFound;
    DivXBool numAudioMediaFound;
    DivXBool numSubtitleMediaFound;
    DivXBool numHDSubtitleMediaFound;
    uint16_t numVideoMedia;
    uint16_t numAudioMedia;
    uint16_t numSubtitleMedia;
    uint16_t numHDSubtitleMedia;

    // Additional data needed by the container properties
    DivXBool numEditionsFound;
    int32_t numEditions;
    DivXBool numChaptersFound;
    int32_t numChapters;
    DivXBool numStreamsFound;
    int32_t numStreams;
    DivXBool objectidFound;
    int32_t objectid;
    DivXBool translationIdFound;
    int32_t translationId;
    DivXBool countFound;
    int32_t count;

    uint64_t uiTimecodeScale;   //! The timecode scale converted from ns (MKV native) to 0.1us (DivXTime)

    unsigned char SegmentUID[16];
    unsigned char PrevUID[16];
    unsigned char NextUID[16];

    // Temporary track information (used for storage before track type is known)
    DivXBool       useTempTrack;
    DMFBlockType_t trackType;
    MKVTrack_t     tempTrack;

    // Lookup of tracks by type and index
    uint32_t   curTrackOfType[NUM_BLOCK_TYPE];  // index of the currently active track
    uint32_t   numTracksOfType[NUM_BLOCK_TYPE]; // counts of tracks of each type
    MKVTrack_t* tracks[NUM_BLOCK_TYPE];         // array of tracks organized by track type

    // Linear lookup of tracks
    uint32_t     totalTracks;
    uint32_t     numTracks;
    uint32_t     trackIdx;
    MKVTrack_t** pTracks; // Allocate for the total number of tracks in this title

    // Store seekhead data temporarily
    uint64_t        seekPosTemp;
    MKVELEMENT_ID_e seekIdTemp;

    // Store seekhead information in linked list
    SeekListHandle seekList;

    // Store attachment information in linked list
    AttachmentListHandle attachmentList;

    // Temporary attachment information
    AttachmentItem_t *tempAttachment;

    // Store cue point information in an index
    DF3Index_t* pDF3Index;

    // Store tags information in a DF3 Tags object
    struct _DF3Tags_t* hDF3Tags;

    // Store chapter information in linked list
    MKVChapterEditionListHandle chapterEditionList;

    // the current chapter edition
    uint32_t currChapterEditionIdx;

    // the current chapter edition duration
    double currChapterEditionDuration;

    // Indicates whether the ordered flag of an edition is used
    DivXBool bEnableOrderedEditions;

    // Store the start offset and size of the cues and for this title
    uint64_t cuesStartOffset;
    uint64_t cuesOffset;
    uint64_t cuesSize;

    // Store the start offset and size of the hierarchical index for this title
    uint64_t hierarchicalIndexOffset;
    uint64_t hierarchicalIndexSize;

    // Store the start of the cluster for this title
    uint64_t clusterOffset;

    // Store the size of the segment
    uint64_t segmentSize;

    // Store the start of this title
    uint64_t titleOffset;

    // Store the data offset of this title
    uint64_t titleDataOffset;

    // Store the total duration for this title
    double duration;

    // Which way are we seeking?
    DivXBool bSeekForward;

    // DRM header buffer
    uint8_t* pDrmHeader;

    // DRM Header buffer size
    uint32_t uiDrmHeaderSize;

    // DRM Header version
    uint32_t uiDrmVersion;

    // Default duration data exists for all relevant tracks
    DivXBool durationDataExists; // Used to flag when all aud/vid tracks have a default duration set

    DivXString* titleName;
    DivXString* prevFileName;
    DivXString* nextFileName;
    DivXString* muxingApp;
    DivXString* writingApp;
    DivXString* fileName;
} MKVTitle_t;

typedef MKVTitle_t* MKVTitleHandle;

/*! Menu data placeholder */
typedef struct _MKVMenu_t
{
    DivXBool MKVMenuFound;
} MKVMenu_t;

/*! Cache structure, stores all titles */
typedef struct _MKVCacheInst_t
{
    DivXMem  hMem;

    // Titles
    LinkedList_t *pTitleList;

    // Title iterator
    LinkedListIterHandle hTitleIter;

    // The current title
    MKVTitle_t* pCurrTitle;

    // Menus (not implemented)
    LinkedList_t *pMenuList;

    // Use hierarchical index
    DivXBool bUseHierarchicalIndex;

    // Gap in ms
    uint64_t gapMs;

    // Use delacing
    DivXBool bDelace;

    // Tags offset
    uint64_t tagsOffset;

    // Full permissions
    DivXBool bFullPermissions;

}MKVCacheInst_t;
typedef struct _MKVCacheInst_t * MKVCacheHandle;


// -------------------------------------------------------------------------------------------
//                       MKVCache Internal Functions
// -------------------------------------------------------------------------------------------

/*! Get the current track to store to

    @param handle          (IN)    - Handle to MKVCache instance

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetCurrentTrack(MKVCacheHandle handle);

/*! Get the current track for a particular block type

    @param handle          (IN)    - Handle to MKVCache instance
    @param blockType       (IN)    - The block type aud, vid, sub

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetCurrentTrackByType(MKVCacheHandle handle, DMFBlockType_t blockType);

/*! Get the track based on uiTrackID (MKV specific)

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackID       (IN)    - uiTrackID (1...X)

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetTrack(MKVCacheHandle handle, uint64_t uiTrackID);

/*! Retrieve the track based on it's UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackUID      (IN)    - uiTrackUID (some unique identifier ID)

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetTrackFromUID(MKVCacheHandle handle, uint64_t uiTrackUID);

/*! Retrieve the track based on it's UID and title index

    @param handle          (IN)    - Handle to MKVCache instance
    @param titleIdx        (IN)    - Title index
    @param uiTrackUID      (IN)    - uiTrackUID (some unique identifier ID)

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetTrackFromTitleAndUID(MKVCacheHandle handle, uint32_t titleIdx, uint64_t uiTrackUID);

/*! Get the track based on track index (DMF based)

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackIdx      (IN)    - uiTrackIdx (0...X)

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetTrackByIndex(MKVCacheHandle handle, uint32_t uiTrackIdx);

/*! Get the track based on trackIdx and block type(DMF specific)

    @param handle          (IN)    - Handle to MKVCache instance
    @param blockType       (IN)    - The block type aud, vid, sub
    @param trackIdx        (IN)    - trackIdx (0...X)

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTrack_t* mkvCache_GetTrackByType(MKVCacheHandle handle, DMFBlockType_t blockType, uint32_t trackIdx);

/*! Get the number of tracks based on type

    @param handle          (IN)    - Handle to MKVCache instance
    @param blockType       (IN)    - The block type aud, vid, sub

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
uint32_t mkvCache_GetNumTrackByType(MKVCacheHandle handle, DMFBlockType_t blockType);

/*! Set the track based on uiTrackID (MKV specific)

    @param handle          (IN)    - Handle to MKVCache instance
    @param blockType       (IN)    - The block type aud, vid, sub
    @param num             (IN)    - The number of tracks to set

    @return DivXError      (OUT)   - DivXError
*/
DivXError mkvCache_SetNumTrackByType(MKVCacheHandle handle, DMFBlockType_t blockType, uint32_t num);

/*! Create a title list

    @param handle          (IN)    - Handle to MKVCache instance
    @param ppTitleList     (OUT)   - Created title list

    @return DivXError      (OUT)   - Error codes
    DIVX_ERR_INVALID_ARG
    DIVX_ERR_INSUFFICIENT_MEM
*/
DivXError mkvCache_CreateTitleList(DivXMem hMem, LinkedList_t** ppTitleList);

/*! Destroys a title list

    @param handle          (IN)    - Handle to MKVCache instance
    @param pTitleList      (OUT)   - Created title list

    @return DivXError      (OUT)   - Error codes
*/
DivXError mkvCache_DestroyTitleList(DivXMem hMem, LinkedList_t* pTitleList);

/*! Create a title to add to the title list

    @param titleList        (IN)   - Handle to MKVTitleList_t

    @return DivXError      (OUT)   - Error codes
    DIVX_ERR_INVALID_ARG
    DIVX_ERR_INSUFFICIENT_MEM
*/
DivXError mkvCache_AddNewTitleToList(MKVCacheHandle handle);

/*! Get the number of titles

    @param handle          (IN)    - Handle to MKVCache instance

    @return uint32_t      (OUT)    - Number of titles
*/
uint32_t mkvCache_GetNumTitle(MKVCacheHandle handle);

/*! Get the current title to store to

    @param handle          (IN)    - Handle to MKVCache instance

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTitle_t* mkvCache_GetCurrentTitle(MKVCacheHandle handle);

/*! Get the current title to store to

    @param handle          (IN)    - Handle to MKVCache instance
    @param pTitle          (IN)    - Set the title as current

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
DivXError mkvCache_SetCurrentTitle(MKVCacheHandle handle, MKVTitle_t* pTitle);

/*! Get the current title to store to

    @param handle          (IN)    - Handle to MKVCache instance
    @param titleIdx        (IN)    - Title index to retrieve

    @return MKVTrack_t*    (OUT)   - Pointer to track structure
*/
MKVTitle_t* mkvCache_GetTitle(MKVCacheHandle handle, uint32_t titleIdx);

/*! Retrieve the title based on it's UID

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiTrackUID      (IN)    - uiTitleUID (some unique identifier ID)
    @param pTitleIdx      (OUT)    - Returns the index of the title found

    @return MKVTitle_t*    (OUT)   - Pointer to title structure
*/
MKVTitle_t* mkvCache_GetTitleFromUID(MKVCacheHandle handle, CDivXUUID* pTitleUUID, uint32_t* pTitleIdx);

//-----------------------------------------------------------------------
//                        Seek Head
//-----------------------------------------------------------------------

/*! Create seek list object

    @param hMem            (IN)    - DivXMem handle

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
SeekListHandle mkvCache_CreateSeekList(DivXMem hMem);

/*! Delete list object (this function deletes all items in list as well)

    @param handle          (IN)    - Handle to SeekList

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_DeleteSeekList(SeekListHandle handle);

/*! Set seek item parameters

    @param handle          (IN)    - Handle to SeekList
    @param id              (IN)    - ID of item
    @param pos             (IN)    - Seek id position in file

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_SetSeekItem(SeekListHandle handle, MKVELEMENT_ID_e id, uint64_t pos);


//-----------------------------------------------------------------------
//                        Chapters
//-----------------------------------------------------------------------

/*! Create chapter edition list object

    @param hMem            (IN)    - DivXMem handle

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
MKVChapterEditionListHandle mkvCache_CreateChapterEditionList(DivXMem hMem);

/*! Delete list object (this function deletes all items in list as well)

    @param handle          (IN)    - Handle to ChapterEditionList

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_DeleteChapterEditionList(MKVChapterEditionListHandle handle);

/*! Create a ChapterEdition item

    @param handle          (IN)    - Handle to ChapterEditionList
    @param pCurrEdition   (OUT)    - Pointer to receive the new chapter edition
                                     can be NULL

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_AddChapterEditionItem(MKVChapterEditionListHandle handle, MKVChapterEditionItem_t** pCurrEdition);

/*! Create chapter list object

    @param hMem            (IN)    - DivXMem handle

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
MKVChapterListHandle mkvCache_CreateChapterList(DivXMem hMem);

/*! Delete list object (this function deletes all items in list as well)

    @param handle          (IN)    - Handle to ChapterList

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_DeleteChapterList(MKVChapterListHandle handle);

/*! Create chapter display list object

    @param hMem            (IN)    - DivXMem handle

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
MKVChapterDisplayListHandle mkvCache_CreateChapterDisplayList(DivXMem hMem);

/*! Delete list object (this function deletes all items in list as well)

    @param handle          (IN)    - Handle to ChapterDisplayList

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_DeleteChapterDisplayList(MKVChapterDisplayListHandle handle);

/*! Create chapter track list object

    @param hMem            (IN)    - DivXMem handle

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
MKVChapterTrackListHandle mkvCache_CreateChapterTrackList(DivXMem hMem);

/*! Delete list object (this function deletes all items in list as well)

    @param handle          (IN)    - Handle to ChapterTrackList

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_DeleteChapterTrackList(MKVChapterTrackListHandle handle);

/*! Create a chapter atom item

    @param handle          (IN)    - Handle to MKVCache instance
    @param pCurrChapter   (OUT)    - Pointer to receive the new chapter atom pointer
                                     can be NULL

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_AddChapterItem(MKVChapterListHandle handle, MKVChapterItem_t** pCurrChapter);

/*! Create a chapter display item

    @param handle          (IN)    - Handle to MKVCache instance
    @param pCurrDisplay   (OUT)    - Pointer to receive the new chapter display pointer
                                     can be NULL

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_AddChapterDisplayItem(MKVChapterDisplayListHandle handle, MKVChapterDisplayItem_t** pCurrDisplay);

/*! Create a chapter track item

    @param handle          (IN)    - Handle to MKVCache instance
    @param pCurrTrack     (OUT)    - Pointer to receive the new chapter track pointer
                                     can be NULL

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_AddChapterTrackItem(MKVChapterTrackListHandle handle, MKVChapterTrackItem_t** pCurrTrack);

/*! Get the current chapter edition for the current title

    @param handle                       (IN)    - Handle to MKVCache instance

    @return MKVChapterEditionItem_t*   (OUT)    - pointer to a chapter edition structure
*/
MKVChapterEditionItem_t* mkvCache_GetCurrentChapterEdition(MKVCacheHandle handle);

/*! Get the current non-ordered chapter edition for the current title

    @param handle                       (IN)    - Handle to MKVCache instance

    @return number of ordered chapters.
*/
MKVChapterEditionItem_t* mkvCache_GetCurrentNonOrderedChapterEdition(MKVCacheHandle handle);

/*! Get the num ordered chapter edition for the current title

    @param handle                       (IN)    - Handle to MKVCache instance

    @return number of ordered chapters.
*/
int32_t mkvCache_GetNumOrderedChapterEdition(MKVCacheHandle handle);

/*! Get the chapter for a specified index

    @param pEdition                     (IN)    - pointer to an edition
    @param chapterIdx                   (IN)    - chapter index

    @return MKVChapterItem_t*          (OUT)    - pointer to a chapter structure
*/
MKVChapterItem_t* mkvCache_GetChapter(MKVChapterEditionItem_t *pEdition, uint32_t chapterIdx);

/*! Get the current chapter display for the current title

    @param handle                       (IN)    - Handle to MKVCache instance

    @return MKVChapterDisplayItem_t*   (OUT)    - pointer to a chapter display structure
*/
MKVChapterDisplayItem_t* mkvCache_GetCurrentChapterDisplay(MKVCacheHandle handle);

/*! Get the current chapter track for the current title

    @param handle                     (IN)    - Handle to MKVCache instance

    @return MKVChapterTrackItem_t*   (OUT)    - pointer to a chapter track structure
*/
MKVChapterTrackItem_t* mkvCache_GetCurrentChapterTrack(MKVCacheHandle handle);

/*! Check if the chapter end times are present in the chapter editions

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_CheckEditionTimes(MKVCacheHandle handle);

/*! Check if the chapter end times are present in a chapter list

    @param handle          (IN)    - Handle to MKVCache instance
    @param uiNextTime      (IN)    - Start time of the next chapter

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_CheckChapterTimes(MKVChapterListHandle handle, DivXTime uiNextTime);

/*! Check if the current edition is ordered

    @param handle          (IN)    - Handle to MKVCache instance

    @return DivXBool      (OUT)    - DIVX_TRUE = ordered edition, DIVX_FALSE = non-ordered edition
*/
DivXBool mkvCache_IsOrderedEdition(MKVCacheHandle handle);

/*! Get absolute time from playlist time

    @param handle          (IN)    - Handle to MKVCache instance
    @param pTime       (IN/OUT)    - pointer to playlist time, absolute time is returned

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_GetAbsoluteTimeFromPlaylistTime(MKVCacheHandle handle, DivXTime *pTime);

/*! Get playlist time from absolute time
    NOTE: the current chapter MUST be set to the correct chapter to get the correct absolute time

    @param handle          (IN)    - Handle to MKVCache instance
    @param pTime       (IN/OUT)    - pointer to absolute time, playlist time is returned

    @return DivXError     (OUT)    - DivXError // TODO EC - Create complete error code list
*/
DivXError mkvCache_GetPlaylistTimeFromAbsoluteTime(MKVCacheHandle handle, DivXTime *pTime);

//-----------------------------------------------------------------------
//                        Tags / Metadata
//-----------------------------------------------------------------------

/*! Create the tags list

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_CreateTagsList(MKVCacheHandle handle);

/*! Deletes the tags list

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_DeleteTagsList(MKVCacheHandle handle);

/*! Add tags

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddTags(MKVCacheHandle handle);

/*! Add tag

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddTag(MKVCacheHandle handle);

/*! Add targets

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddTargets(MKVCacheHandle handle);

DivXError mkvCache_AddTargetItem(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint32_t val);

/*! Add target element

    @param handle           (IN)    - Handle to MKVCache instance
    @param type             (IN)    - The target

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddTargetElement(MKVCacheHandle handle, MKV_TARGET_TYPE_e type);

/*! Add Simple Tag

    @param handle           (IN)    - Handle to MKVCache instance

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddSimpleTag(MKVCacheHandle handle);

/*! Add the tag element (only stores size, type and offset, retrieved later)

    @param handle           (IN)    - Handle to MKVCache instance
    @param type             (IN)    - Element type
    @param offset           (IN)    - File offset
    @param size             (IN)    - Size of element

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_AddSimpleTagItem(MKVCacheHandle handle, MKVELEMENT_ID_e type, uint64_t offset, uint64_t size, uint8_t val);

//-----------------------------------------------------------------------
//                        Trick Track
//-----------------------------------------------------------------------

/*! Gets the master track properties

    @param handle            (IN)    - Handle to MKVCache instance
    @param iVid              (IN)    - Index of the video track
    @param pMasterTrackProp       (OUT)    - Properties of the track

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_MasterTrackProp(MKVCacheHandle handle, uint32_t index, DMFMasterTrackProp_t* pMasterTrackProp);

/*! Gets the trick track properties

    @param handle            (IN)    - Handle to MKVCache instance
    @param iVid              (IN)    - Index of the video track
    @param pTrickTrackProp       (OUT)    - Properties of the track

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_TrickTrackProp(MKVCacheHandle handle, uint32_t index, DMFTrickTrackProp_t* pTrickTrackProp);

/*! Sets the visual search track uid

    @param handle            (IN)    - Handle to MKVCache instance
    @param bExists           (IN)    - The visual search track uid

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTrickTrackUID(MKVCacheHandle handle, uint64_t uid);

/*! Sets the master track uid

    @param handle            (IN)    - Handle to MKVCache instance
    @param uid               (IN)    - The master track uid

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetMasterUID(MKVCacheHandle handle, uint64_t uid);

/*! Sets the visual search track segment uid

    @param handle            (IN)    - Handle to MKVCache instance
    @param bExists           (IN)    - The visual search track uid

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTrickTrackSegmentUID(MKVCacheHandle handle, uint8_t* uid);

/*! Sets the master track segment uid

    @param handle            (IN)    - Handle to MKVCache instance
    @param uid               (IN)    - The master track uid

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetMasterSegmentUID(MKVCacheHandle handle, uint8_t* uid);

/*! Sets the visual search track filename

    @param handle            (IN)    - Handle to MKVCache instance
    @param sFilename         (IN)    - Filename

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetTrickTrackFilename(MKVCacheHandle handle, DivXString* sFilename);

/*! Sets the master track filename

    @param handle            (IN)    - Handle to MKVCache instance
    @param sFilename         (IN)    - Filename

    @return DivXError      (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvCache_SetMasterFilename(MKVCacheHandle handle, DivXString* sFilename);

#endif // _MKVCACHEPRIVATE_H
