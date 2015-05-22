/*!
    @file
@verbatim
$Id: L4Render.h 59445 2009-04-07 07:20:11Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4RENDER_H_
#define _L4RENDER_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"

#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"
#include "DMFBlockNode.h"
#include "ProgressBarParams.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _L4RenderInst_t * L4RenderHandle;
/*typedef struct _StoppedScreen *HSS;*/

/*! Events that can occur in the L4Playback
*/
typedef enum {
    L4RENDER_NO_EVENT = -1,
    L4RENDER_STOP_EVENT = 0,
    L4RENDER_SWITCH_VIDEO_EVENT,
    L4RENDER_SWITCH_AUDIO_EVENT,
    L4RENDER_SWITCH_SUBTITLE_EVENT,
    L4RENDER_SEEK_FORWARD_EVENT,
    L4RENDER_SEEK_BACK_EVENT,
    L4RENDER_FF_EVENT,
    L4RENDER_RW_EVENT,
    L4RENDER_PLAY_PAUSE_EVENT,
    L4RENDER_PAUSE_EVENT,
    L4RENDER_PLAY_EVENT,
    L4RENDER_QUIT,
    L4RENDER_NEXT_TITLE,
    L4RENDER_PREVIOUS_TITLE,
    L4RENDER_NEXT_CHAPTER,
    L4RENDER_PREVIOUS_CHAPTER,
    L4RENDER_SEEK_AUTOCHAPTER,
    L4RENDER_TIMESTAMP,
    L4RENDER_INFO,
    NUM_L4RENDER_EVENTS
} L4RenderEvents_e;

/*! Function pointer video init
*/
typedef DivXError (*pfnL4VideoInit)( DMFVideoStreamInfo1_t* , int32_t currentVideoIdx, uint8_t* privateData, uint32_t privateDataSize, void* userData);

/*! Function pointer audio init
*/
typedef DivXError (*pfnL4AudioInit)( DMFAudioStreamInfo1_t* pAudioStruct, int32_t currentAudioIdx, uint8_t* privateData, uint32_t privateDataSize, void* userData);

/*! Function pointer subtitle init
*/
typedef DivXError (*pfnL4SubtitleInit)( DMFSubtitleStreamInfo1_t* pSubtitleStruct, int32_t currentSubtitleIdx, void* userData);

typedef DivXError (*pfnL4ProgressBarInit)( DMFVideoStreamInfo1_t* pVideoStruct, void* userData, void* font);

/*! Function for calling is ready
*/
typedef DivXError (*pfnL4IsReady)(DMFBlockType_t blockType, void* userData);

/*! Function for calling flush
*/
typedef DivXError (*pfnL4Flush)(void* userData);

/*! Function to queue
*/
typedef DivXError (*pfnL4Queue)(DMFBlockNode_t* pBlockNode, void* userData);

typedef DivXError (*pfnL4QueueBitmap)(DMFBlockNode_t* pBlockNode, 
                                      uint8_t* bitmap, 
                                      uint32_t size, 
                                      void* userData, 
                                      uint32_t vidWidth, 
                                      uint32_t vidHeight, 
                                      uint32_t subWidth, 
                                      uint32_t subHeight,
                                      uint16_t left, 
                                      uint16_t top, 
                                      uint16_t right,
                                      uint16_t bottom);
/*! Function for heartbeat
*/
typedef DivXError (*pfnL4Heartbeat)(void* userData, uint32_t ms);

/*! Function for handling events
*/
typedef DivXError (*pfnL4Event)(void* userData, L4RenderEvents_e* pEvent, uint64_t *pnData);

/* ! show the progress bar
*/
typedef DivXError (*pfnL4ShowProgressBar)(ProgressBarParams* pParams, void* userData, void** ppBmp, void* hDivXFont);

/* ! show the stop screen
*/
typedef DivXError (*pfnL4LoadStoppedScreen)(void* userData, void **hStoppedScreen);

/*! Function for flushing decoders
*/
typedef DivXError (*pfnL4Cleanup)(DivXBool bVideo, DivXBool bAudio, DivXBool bSub, DivXBool bClear, void* userData);

/*! Function to set timer
*/
typedef DivXError (*pfnL4SetTimer)(DivXTime time, void* userData);

/*! Function to get time
*/
typedef DivXError (*pfnL4GetTimer)(DivXTime *time, void* userData);

/*! Function to Pause
*/
typedef DivXError (*pfnL4Pause)(DivXBool bPause, void* userData);

/*!
    Create an instance.

    @param pHandle               (OUT) - A pointer to receive instance.
    @param hMem                  (IN)  - Memory handle

    // Private data (required for video codecs, audio codecs in some cases)
    @param privateData           (IN)  - The private data block buffer to be used (use NULL if none)
    @param privateDataAllocSize  (IN)  - The allocated space for the private data (used for error check)

    // Callback functions and data
    @param pfnVidInit            (IN)  - Initialization function for video
    @param pfnAudInit            (IN)  - Initialization function for audio
    @param pfnSubInit            (IN)  - Initialization function for subtitles
    @param pfnIsReady            (IN)  - Is ready function for render stream readiness
    @param pfnCleanup            (IN)  - Cleans up render
    @param pfnSetTimer           (IN)  - Sets the render playback timer
    @param pfnPause              (IN)  - Render pause (pauses Vid, Aud, Sub and Timer)
    @param pfnFlush              (IN)  - Flushes the render object
    @param pfnQueue              (IN)  - Queues data to the render object
    @param pfnHeartbeat          (IN)  - Function used for any periodic events required by renderer
    @param pfnEvent              (IN)  - Triggers render resets based on user events
    @param userData              (IN)  - Data required by callback functions

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_New( L4RenderHandle *pHandle, DivXMem  hMem, uint8_t*          privateData,
                                                                uint32_t          privateDataAllocSize,
                                                                pfnL4VideoInit    pfnVidInit, 
                                                                pfnL4AudioInit    pfnAudInit, 
                                                                pfnL4SubtitleInit pfnSubInit,
                                                                pfnL4ProgressBarInit pfnProgBarInit,
                                                                pfnL4IsReady      pfnIsReady, 
                                                                pfnL4Cleanup      pfnCleanup, 
                                                                pfnL4SetTimer     pfnSetTimer,
                                                                pfnL4GetTimer     pfnGetTimer,
                                                                pfnL4Pause        pfnPause,
                                                                pfnL4Flush        pfnFlush,
                                                                pfnL4Queue        pfnQueue,
                                                                pfnL4QueueBitmap  pfnQueueBitmap,
                                                                pfnL4ShowProgressBar pfnShowProgressBar,
                                                                pfnL4LoadStoppedScreen  pfnLoadStoppedScreen,
                                                                pfnL4Heartbeat    pfnHeartbeat,
                                                                pfnL4Event        pfnEvent,
                                                                void*             userData);

/*!
    Delete an instance.

    @param handle (OUT) - The instance to delete

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_Delete( L4RenderHandle handle);

/*!
    Returns a buffer to be used for private data (used for private codec data in 
    audio and video initialization) 

    @param handle (IN) - The instance

    @return The normal return code is DIVX_ERR_SUCCESS
*/
uint8_t* L4Render_GetPrivateData( L4RenderHandle handle);

/*!
    Returns the total allocated space for the buffer

    @param handle (IN) - The instance

    @return The normal return code is DIVX_ERR_SUCCESS
*/
uint32_t L4Render_GetPrivateDataAllocSize( L4RenderHandle handle);

/*!
    Returns the actual data block size

    @param handle (IN) - The instance

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_SetPrivateDataSize( L4RenderHandle handle, uint32_t dataSize);

/*!
    Cleans up decoders

    @param handle (OUT) - The instance to delete
    // TODO EC - Add parameters

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_Cleanup(L4RenderHandle handle, DivXBool bVideo, DivXBool bAudio, DivXBool bSub, DivXBool bClear);

/*!
    Sets the media timer 

    @param handle (IN) - The instance to delete
    @param time   (IN) - The time in DivX Time

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_SetTimer(L4RenderHandle handle, DivXTime time);

/*!
    Pauses the tracks

    @param handle (IN) - The instance to delete
    @param bPause (IN) - True for pause

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_Pause(L4RenderHandle handle, DivXBool bPause);

/*!
    Flushes all

    @param handle (IN) - The instance to delete

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_Flush(L4RenderHandle handle);

/*!
    Queues data to be rendered

    @param handle (IN) - The instance to delete

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_Queue( L4RenderHandle handle, DMFBlockNode_t* pBlockNode);

/*!
    Queues bitmap to be rendered


    @param pInst (IN) - L4Render handle
    @param bitmap (IN) - bitmap to queue for rendre
    @param size (IN) - size of the bitmap in bytes
    @param vidWidth (IN) - width of the video
    @param vidHeight (IN) - height of the video
    @param subWidth (IN) - width of the overlay (subtitle)
    @param subHeight (IN) - height of the overlay (subtitle)

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_QueueBitmap(L4RenderHandle handle, 
                               DMFBlockNode_t* pBlockNode, 
                               uint8_t* bitmap, 
                               uint32_t size, 
                               uint32_t vidWidth, 
                               uint32_t vidHeight, 
                               uint32_t subWidth, 
                               uint32_t subHeight,
			       uint16_t left,
			       uint16_t top,
                               uint16_t right,
                               uint16_t bottom);


/*!
    Check if block type is ready

    @param pHandle (IN) - A pointer to receive instance.

    @return DIVX_TRUE if ready
*/
DivXBool L4Render_Ready( L4RenderHandle handle, DMFBlockType_t blockType);

/*!
    Heartbeat

    @param pHandle (IN) - A pointer to receive instance.
    @param ms      (IN) - Suspends for number of ms

    @return DIVX_TRUE if ready
*/
DivXError L4Render_Heartbeat( L4RenderHandle handle, uint32_t ms );

/*!
    Event

    @param pHandle  (IN) - A pointer to receive instance.
    @param pEvent   (OUT) - Event received
    @param pnData   (OUT) - Data received

    @return DIVX_TRUE if ready
*/
DivXError L4Render_Event( L4RenderHandle handle, L4RenderEvents_e* pEvent, uint64_t *pnData);

/*!
    Video initialization

    @param pHandle (IN) - A pointer to receive instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_VideoInit( L4RenderHandle handle, DMFVideoStreamInfo1_t* pVideoStruct, uint8_t* privateData, uint32_t privateDataSize, int32_t currentVideoIdx);

/*!
    Audio initialization

    @param pHandle (IN) - A pointer to receive instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_AudioInit( L4RenderHandle handle, DMFAudioStreamInfo1_t* pAudioStruct, uint8_t* privateData, uint32_t privateDataSize, int32_t audioVideoIdx);

/*!
    Subtitle initialization

    @param pHandle (IN) - A pointer to receive instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_SubtitleInit( L4RenderHandle handle, DMFSubtitleStreamInfo1_t* pSubtitleStruct, int32_t currentSubtitleIdx);

/*!
    Progress bar initialization

    @param pHandle (IN) - A pointer to receive instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_ProgressBarInit( L4RenderHandle pInst, DMFVideoStreamInfo1_t* pVideoStruct, void* hDivXFont);

/*!
    Gets the media time 

    @param handle (IN) - The instance to delete
    @param time   (OUT) - The time in DivX Time

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_GetTimer(L4RenderHandle handle, DivXTime *time);

DivXError L4Render_ShowProgressBar( L4RenderHandle pInst, ProgressBarParams* pParams, uint8_t** ppBmp, void* hDivXFont);
/*!
    Gets the media time 

    @param handle (IN) - The instance
    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Render_LoadStoppedScreen(L4RenderHandle pInst, void **hStoppedScreen);
#ifdef __cplusplus
}
#endif

#endif //_L4RENDER_H_
