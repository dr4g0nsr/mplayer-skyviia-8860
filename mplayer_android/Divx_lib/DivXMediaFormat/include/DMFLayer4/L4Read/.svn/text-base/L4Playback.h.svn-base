/*!
    @file
@verbatim
$Id: L4Playback.h 60170 2009-05-15 16:41:21Z ashivadas $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4PLAYBACK_H_
#define _L4PLAYBACK_H_

#include "L4Menus.h"
#include "L4Render.h"
#include "DMFTagInfo.h"
#include "DMFProperties.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup LAYER4 DMF Layer 4 API
 *  Description of DivX Media Format API (DMF) Layer 4 Playback API
 *  @{
 */

typedef  enum {
    START, TIMERESET, PLAY, QUEUE, PAUSED, IDLE, EXITING, EXIT
}PLAYERSTATE;


typedef enum L4Error
{
    L4_WARN_INVALID_MENU_VERSION    = 2, /*! non-fatal error code that the menu is invalid */
    L4_WARN_NONFATAL                = 1,
    L4_ERR_SUCCESS                  = 0,
    L4_ERR_FAILURE                  = DIVX_ERR_FAILURE,         ///< General undefined error
    L4_ERR_INVALID_ARG              = DIVX_ERR_INVALID_ARG,     ///< Supplied argument invalid
    L4_ERR_OPEN_FAILURE             = DIVX_ERR_OPEN_FAILURE,    ///< Open file or container failed
    L4_ERR_READ_FAILURE             = DIVX_ERR_READ_FAILURE,    ///< General read failure
    L4_ERR_WRITE_FAILURE            = DIVX_ERR_WRITE_FAILURE,   ///< General write failure
    L4_ERR_INVALID_STATE            = DIVX_ERR_INVALID_STATE,   ///< Invalid state
    L4_ERR_OUT_OF_RANGE             = DIVX_ERR_OUT_OF_RANGE,    ///< Error code out of range
    L4_ERR_INVALID_REQUEST          = DIVX_ERR_INVALID_REQUEST, ///< Invalid request
    L4_ERR_EOF                      = DIVX_ERR_EOF,             ///< End of file reached while reading
    L4_ERR_INSUFFICIENT_MEM         = DIVX_ERR_INSUFFICIENT_MEM,///< Insufficient memory 
    L4_ERR_INVALID_HTTP_SERVER      = -11,                      ///< Http server returns invalid response
    L4_ERR_NO_DATA_STAT_FILES       = -12,                      ///< Existing mode cannot be used
    L4_ERR_BLOCK_BUFFER_TOO_SMALL   = -13,                      ///< Block buffer is too small
    L4_ERR_DATA_NOT_READY           = -14,                      ///< Progressive read data not ready
    L4_ERR_EOS                      = -21,                      ///< End of stream detected
    L4_ERR_CODEC_UNSUPPORTED        = -22                       ///< Codec type not supported
} L4Error_t;

typedef struct _L4PlaybackInst_t * L4PlaybackHandle;

/*!
    Create an instance.

    @param pHandle (OUT) - A pointer to receive instance.
    @param hRender (IN)  - The rendering interface for L4
    @param hMem    (IN)  - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_New( L4PlaybackHandle *pHandle, L4RenderHandle hRender, DivXMem  hMem);

/*!
    Delete an instance.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_Delete( L4PlaybackHandle handle );

/*!
    Intializes the media for playback
    This API gets the track info for all tracks of each media type and
    initializes the media decoders to the current track's info.

    @param handle (IN) - A handle to the instance
    @param 
*/
DivXError L4Playback_InitMedia(L4PlaybackHandle handle, DMFBlockNode_t* pBlockNode, uint32_t titleIndex);

/*!
    Initializes a block node

    @param handle      (IN) - A handle to instance.
    @param pBlockNode (OUT) - Block node to initialize

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_InitReadBlock( L4PlaybackHandle handle, DMFBlockNode_t *pBlockNode );

/*!
    DeInitialize the read block

    @param handle     (IN) - A handle to instance.
    @param pBlockNode (IN) - Block node to deinitialize

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_DeInitReadBlock( L4PlaybackHandle handle, DMFBlockNode_t *pBlockNode );

/*!
    Loads a file

    @param handle    (IN)     - A handle to instance.
    @param filename  (IN)     - File to open
    @param pMenuMode (IN/OUT) - Menu mode if set to DIVX_TRUE plays menus, if they exist
                                If menus exist returns DIVX_TRUE

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_LoadMedia( L4PlaybackHandle handle, const DivXString *fileName, DivXBool* pMenuMode);

/*!
    Closes a file

    @param handle   (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_CloseMedia( L4PlaybackHandle handle);

/*!
    Playback the loaded media
    This function will exit when user has requested or if exitOnFinished
    is set, when the media has finished playing out.  All user interactions
    are handled through the L4Render object.

    @param handle        (IN) - A handle to instance.
    @param exitOnFinish  (IN) - Flag to exit on finish

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_Play(L4PlaybackHandle handle, DivXBool exitOnFinish);

/*!
    Handle an event from the render API

    @param handle        (IN) - A handle to instance.
    @param renderEvent   (IN) - This is the render event to execute
    @param renderEvent   (IN) - This is the data along with render event 

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_HandleEvent(L4PlaybackHandle handle, L4RenderEvents_e* pRenderEvent, uint64_t nData, PLAYERSTATE *playerState);

/*!
    Gets the number of titles

    @param pHandle        (IN) - A handle to instance.
    @param pNumTitle     (OUT) - Number of titles

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetNumTitles( L4PlaybackHandle handle, uint32_t* pNumTitle );

/*!
    Returns the currently active title

    @param pHandle        (IN) - A handle to instance.
    @param pNumTitle     (OUT) - Number of titles

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetCurTitle( L4PlaybackHandle handle, uint32_t* pCurTitle );

/*!
    Sets the title based on the readTitle handle

    @param pHandle        (IN) - A pointer to receive instance.
    @param titleIndex     (IN) - Title index to open

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_SetTitle( L4PlaybackHandle handle, uint32_t titleIndex );

/*!
    Read the DRM data 

    @param handle         (IN)  - A pointer to receive instance.
    @param iTitleIndex    (IN)  - Index of title to open 
    @param nHeaderSize    (IN)  - Header buffer size 
    @param pDrmHeader     (OUT) - A pointer to buffer to receive DRM header

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetDrmData(L4PlaybackHandle handle, uint16_t iTitleIndex, DMFDrmSupportLevel_e *drmLevel, 
                                uint32_t nHeaderSize, uint8_t *pdrmHeader);


/*!
    Reads the next block 

    @param handle        (IN)  - A pointer to receive instance.
    @param pBlockType    (OUT) - The block type to be returned when a block is read
    @param pnStream      (OUT) - The stream number (index) to be returned when a block is read
    @param pBlockNode    (OUT) - The block node to be returned when the block is read

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_ReadNextBlock( L4PlaybackHandle handle, DMFBlockType_t *pBlockType, uint32_t *pnStream, DMFBlockNode_t *pBlockNode);

/*!
    Seeks the playback to a specific time

    @param handle        (IN)  - A pointer to receive instance.
    @param pTime         (IN/OUT) - The time to set, and the time actually set

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_Seek( L4PlaybackHandle handle, DivXTime *pTime);

/**
    Sets the trick mode.

    @param handle     (IN) - A handle to instance.
    @param pBlockType (IN) - The mode to switch to.
    
    @return The normal return code is DIVX_ERR_SUCCESS

    @note possible values:
            TRICK_MODE_NONE
            TRICK_MODE_FORWARD_SPEED1
            TRICK_MODE_FORWARD_SPEED2
            TRICK_MODE_FORWARD_SPEED3
            TRICK_MODE_FORWARD_SPEED4
            TRICK_MODE_REVERSE_SPEED1
            TRICK_MODE_REVERSE_SPEED2
            TRICK_MODE_REVERSE_SPEED3
            TRICK_MODE_REVERSE_SPEED4
*/
DivXError L4Playback_SetTrickMode( L4PlaybackHandle handle, DMFTrickMode_e mode );

/*!
    Retrieves the number of streams by type

    @param handle           (IN)  - A pointer to receive instance.
    @param type             (IN)  - The type

    @return The normal return code is DIVX_ERR_SUCCESS
*/
int32_t L4Playback_GetNumStreamsByType( L4PlaybackHandle handle, DMFBlockType_t type );

/*!
    Retrieves the vid struct for a specific stream

    @param handle           (IN)  - A pointer to receive instance.
    @param videoStruct      (OUT) - Video struct to return 
    @param nStream          (IN)  - Stream to retrieve

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetVideoFormat(L4PlaybackHandle handle, DMFVideoStreamInfo1_t *videoStruct, int32_t nStream);

/*!
    Retrieves the aud struct for a specific stream

    @param handle           (IN)  - A pointer to receive instance.
    @param audioStruct      (OUT) - Audio struct to return 
    @param nStream          (IN)  - Stream to retrieve

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetAudioFormat(L4PlaybackHandle handle, DMFAudioStreamInfo1_t *audioStruct, int32_t nStream);

/*!
    Retrieves the sub struct for a specific stream

    @param handle           (IN)  - A pointer to receive instance.
    @param subtitleStruct   (OUT) - Subtitle struct to return 
    @param nStream          (IN)  - Stream to retrieve

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetSubtitleFormat(L4PlaybackHandle handle, DMFSubtitleStreamInfo1_t *subtitleStruct, int32_t nStream);

/*!
    Retrieves the number of streams

    @param handle           (IN)  - A pointer to receive instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
int32_t L4Playback_GetNumStreams( L4PlaybackHandle handle);

/*!
    Get the currently active streams for a given block type

    @param handle    (IN) - A handle to instance.
    @param blockType (IN) - The block type
    @param index     (IN) - The index to set

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetActiveStreams(L4PlaybackHandle handle, DMFBlockType_t blockType, int32_t* index);

/*!
    Sets a specific type index stream active

    @param handle    (IN) - A handle to instance.
    @param blockType (IN) - The block type
    @param index     (IN) - The index to set

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_SetStreamNumActive(L4PlaybackHandle handle, DMFBlockType_t blockType, uint32_t index);

/*!
    Returns whether a stream of blockType is active

    @param handle    (IN) - A handle to instance.
    @param blockType (IN) - The block type
    @param index     (IN) - The index to set

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXBool L4Playback_IsStreamActive(L4PlaybackHandle handle, DMFBlockType_t blockType);

/*!
    Sets all stream active that are selected

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_SetStreamsActive( L4PlaybackHandle handle);

/*!
    Sets the trick streams active

    @param handle (IN) - A handle to instance.

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetTrickStreamsActive( L4PlaybackHandle handle );

/*!
    Adds a stream to the playback container.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_AddStream( L4PlaybackHandle handle, DMFBlockType_t type, int32_t nStream );

/*!
    Retrieve the number of chapters

    @param handle           (IN)  - A pointer to receive instance.
    @param pnChapters       (OUT) - The returned number of chapters

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetNumChapters( L4PlaybackHandle handle, int32_t* pnChapters);

/*!
    Retrieves the current chapter

    @param handle           (IN)  - A pointer to receive instance.
    @param pnChapter        (OUT) - The current chapter

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetCurChapter( L4PlaybackHandle handle, int32_t* pnChapter);

/*!
    Sets the chapter

    @param handle            (IN) - A pointer to receive instance.
    @param nChapter          (IN) - Chapter to set to

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_SetChapter( L4PlaybackHandle handle, int32_t nChapter);

/*!
    Gets the count of chapter names

    @param handle            (IN) - A pointer to receive instance.
    @param nChapter          (IN) - Chapter to get name count from

    @return The normal return code is DIVX_ERR_SUCCESS
*/
int32_t L4Playback_GetChapterNameCount(L4PlaybackHandle pInst, uint32_t nChapter);

/*!
    Gets the count of chapter names

    @param handle            (IN) - A pointer to receive instance.
    @param nChapter          (IN) - Chapter to get name count from
    @param nName             (IN) - Index of name
    @param ppNameProp       (OUT) - Returns the name prop

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_GetChapterNameProp(L4PlaybackHandle pInst, uint32_t nChapter, uint32_t nName, DMFNameProp_t **ppNameProp);

/*!
    Switchs active tracks, turning on a stream a turning of all others.

    @param handle  (IN) - A handle to instance.
    @param type    (IN) - The type of stream.
    @param nStream (IN) - The stream number to turn on.
    
    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Playback_SwitchTrack( L4PlaybackHandle handle, DMFBlockType_t type, uint32_t* pnStream );

/*!
    Returns the total duration of the current video track

    @param handle  (IN) - A handle to instance.
    @param type    (IN) - The type of stream.
    @param nStream (IN) - The stream number to turn on.
    
    @return The normal return code is L1_ERR_SUCCESS
*/
DivXError L4Playback_GetVideoTotalDuration(L4PlaybackHandle handle, DivXTime* pTotalDuration);

/*!
    Returns the current absolute time

    @param handle  (IN) - A handle to instance.
    
    @return DivXTime
*/
DivXTime L4Playback_GetCurAbsoluteTime( L4PlaybackHandle handle );

/*! PRIVATE FUNCTION
    Sets the current absolute time (private function)

    @param handle  (IN) - A handle to instance.
    
    @return DivXTime
*/
DivXError L4Playback_SetCurAbsoluteTime( L4PlaybackHandle handle, DivXTime time );

/*!
    Checks for an L4 Event. 
    This function should be called 

    @param handle  (IN) - A handle to instance.
    @param pEvent  (IN) - The event that occured in playback
    
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetEvent(L4PlaybackHandle handle, L4MenuEvent_e* pMenuEvent);

/*!
    Get mode

    @param handle  (IN) - A handle to instance.
    @param pMode   (IN) – Returns mode currently set to
    
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetMode(L4PlaybackHandle handle, L4Mode_e* pMode);

/*!
Set mode (not required on initialization)
If the mode comes back as L4_MODE_MENUS, then we skip directly to file playback without calling SetTitle().  Any SetTitle that is called in the L4_MODE_MENUS mode will force it to exit.  After a SetTitle() call is made the API will allow the user to return to a menu by calling SetMode(L4_MODE_MENUS).


    @param handle  (IN) - A handle to instance.
    @param mode    (IN) – Mode to be set to  
  
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetMode(L4PlaybackHandle handle, L4Mode_e mode);

/*!
    Retrieves the languages supported by this file

    @param handle    (IN)  - A handle to instance.
    @param pLangList (OUT) – A linked list of languages
  
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetLanguages(L4PlaybackHandle handle, LinkedList_t* pLangList);

/*!
    Sets the language used for playback

    @param handle    (IN)  - A handle to instance.
    @param pLangList (OUT) – A linked list of languages
  
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetLanguage(L4PlaybackHandle handle, DivXString* lang);

/*!
    Returns the size of the format specific data for the currently selected title

    @param handle         (IN) - A handle to the instance
    @param blocktype      (IN) - Block type to get codec private data of
    @param nStream        (IN) - Stream index
    @param pLen          (OUT) - Length of buffer returned (use GetFormatSize to pre-initialize buffer)

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetStreamPrivateDataSize( L4PlaybackHandle handle, DMFBlockType_t blockType, int32_t nStream, uint32_t* pLen);

/*!
    Returns the format specific data

    @param handle         (IN) - A handle to the instance
    @param blocktype      (IN) - Block type to get codec private data of
    @param nStream        (IN) - Stream index
    @param pBuffer       (OUT) - Buffer of data to be returned
    @param len           (OUT) - Length of buffer returned (use GetFormatSize to pre-initialize buffer)

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetStreamPrivateData( L4PlaybackHandle handle, DMFBlockType_t blockType, int32_t nStream, uint8_t* pBuffer, uint32_t* len);

/*!
    Return the tags to the first entry

    @param handle         (IN) - A handle to the instance

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_FirstTag( L4PlaybackHandle handle);

/*!
    Keep grabbing tags until we have no more

    @param handle         (IN) - A handle to the instance

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_NextTag( L4PlaybackHandle handle);

/*!
    Retrieve the tag size and other information

    @param handle         (IN) - A handle to the instance

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetTagInfo( L4PlaybackHandle handle, DMFTagInfo_t* tagInfo );

/*!
    Retrieve the tag size and other information

    @param handle         (IN) -  A handle to the instance

    @param type           (IN) -  Enum of target type to retrieve

    @param index          (IN) -  Index of target type to retrieve

    @param value          (OUT) - Value of target

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetTargetByTypeIdx( L4PlaybackHandle handle,  DMF_TAG_TARGET_e type, 
                                         int32_t index, int32_t *value);
/*!
    Retrieve the tag

    @param handle         (IN) - A handle to the instance

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/

DivXError L4Playback_GetTag( L4PlaybackHandle handle, uint8_t* tagData );

/*!
    Enable/Disable auto chapters

    @param handle         (IN)  - A handle to the instance
    @param bEnable        (IN)  - True = enable, false = disable

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_EnableAutoChapters( L4PlaybackHandle handle, DivXBool bEnable );

/*!
    Enable/Disable auto chapter override

    @param handle         (IN)  - A handle to the instance
    @param bEnable        (IN)  - True = enable, false = disable

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_EnableAutoChapterOverride( L4PlaybackHandle handle, DivXBool bEnable );

/*!
    Set auto chapter spacing percent

    @param handle         (IN)  - A handle to the instance
    @param spacingPercent (IN)  - Chapter spacing percent

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetAutoChapterSpacing( L4PlaybackHandle handle, uint8_t spacingPercent );

/*!
    Get number of auto chapters 

    @param handle         (IN)   - A handle to the instance
    @param pnChapters     (OUT)  - Number of auto chapters 

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetNumAutoChapters(L4PlaybackHandle handle, int32_t* pnChapters);

/*!
    Seeek to autochapter specified

    @param handle         (IN)  - A handle to the instance
    @param nAutoChapter   (IN)  - Chapter numer to seek to 

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetAutoChapter(L4PlaybackHandle handle, int32_t nAutoChapter);

/*!
    Set frame gap for indexes

    @param handle         (IN)  - A handle to the instance
    @param frameGap (IN)  - minimum number of frames between indexed key frames

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetFrameGap( L4PlaybackHandle handle, int32_t frameGap );
/*!
    Set the index scheme to cacheless or normal parse

    @param handle         (IN)  - A handle to the instance
    @param frameGap (IN)  - use cachelss indexing is DIVX_TRUE
    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetIndexScheme( L4PlaybackHandle pInst, DivXBool cachelessIndex );


/* JM refactor JM todo should this be private? */
DivXError L4Playback_Reset(L4PlaybackHandle pInst, DivXTime resetTime);

/*!
    Subtitle activation wrapper -- also finds linked fonts and loads them into font handle

    @param handle         (IN)  - A handle to the instance
    @param index          (OUT) - Actual tag data

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_SetSubStreamNumActive(L4PlaybackHandle handle, int32_t index);

/*!
    Getinfo for L4Playback

    @param handle         (IN)  - A handle to the instance
    @param queryID        (IN)  - Query ID to query
    @param index          (IN)  - Actual index

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
*/
DivXError L4Playback_GetInfo(L4PlaybackHandle handle, int32_t QueryValueID, DMFVariant *Value, int32_t Index);

#ifdef __cplusplus
}
#endif

#endif //_L4PLAYBACK_H_
/**  @}  */
