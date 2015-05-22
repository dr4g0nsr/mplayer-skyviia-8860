/*!
    @file
@verbatim
$Id: L4Menus.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4MENUS_H_
#define _L4MENUS_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXMem.h"
#include "DivXTime.h"
#include "DivXLinkedList.h"

#include "DMFBlockNode.h"

#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _L4MenuActiveTracks_t
{
    int16_t vidTrackId;
    int32_t vidStartBlock;
    int16_t audTrackId;
    int32_t audStartBlock;
    int16_t subTrackId;
    int32_t subStartBlock;
}L4MenuActiveTracks_t;

/*!
This enumeration defines the events that can be sent to the L4 API
*/
typedef enum
{
    L4_MENU_EVENT_UP,
    L4_MENU_EVENT_DOWN,
    L4_MENU_EVENT_LEFT,
    L4_MENU_EVENT_RIGHT,
    L4_MENU_EVENT_ROOT_MENU,
    L4_MENU_EVENT_TITLE_MENU,
    L4_MENU_EVENT_CHAPTER_MENU,
    L4_MENU_EVENT_AUDIO_MENU,
    L4_MENU_EVENT_SUBTITLE_MENU,
    L4_MENU_EVENT_MOUSE_OVER,
    L4_MENU_EVENT_MOUSE_CLICK,
    L4_MENU_EVENT_ENTER,
    NUM_L4_MENU_EVENTS
} L4MenuEvent_e;

/*!
This enumeration defines the modes of playback
*/
typedef enum
{
    L4_MODE_OPENING_MENU,
    L4_MODE_PLAYING_MENU,
    L4_MODE_OPENING_TITLE,
    L4_MODE_PLAYING_TITLE,
} L4Mode_e;

typedef struct _L4MenusInst_t* L4MenusHandle;

typedef struct _MenuOverlayCache L4MenusOverlayCache;

/*!
    Creates a new L4Menus object

    @param handle       (OUT) - A handle to instance.
    @param hMem         (IN)  - The memory handle for L4Menus
    
    @return DivXError
*/
DivXError L4Menus_New(L4MenusHandle* handle, DivXMem hMem);

/*!
    Destroys a created L4Menus object

    @param handle  (IN) - A handle to instance.
    @param hMem         (IN)  - The memory handle for L4Menus
    
    @return DivXError
*/
DivXError L4Menus_Delete(L4MenusHandle handle, DivXMem hMem);

/*!
    Intializes a L4 Menus object

    @param handle       (OUT) - A handle to instance.
    @param hL3Container (IN)  - The L3 Container handle
    @param hL3Resources (IN)  - The L3 Resources handle
    @param pMenusExist  (OUT) - Menus exist flag
    
    @return DivXError
*/
DivXError L4Menus_Init(L4MenusHandle handle, void* hL3Container, void* hL3Resources, DivXBool* pMenusExist);

/*!
    Open a menu

    @param handle       (OUT) - A handle to instance.
    
    @return DivXError
*/
DivXError L4Menus_OpenMenu( L4MenusHandle handle );

/*!
    Close the menu

    @param handle       (OUT) - A handle to instance.
    
    @return DivXError
*/
DivXError L4Menus_CloseMenu( L4MenusHandle handle );

/*!
    Retrieves the menu total duration

    @param handle       (IN)  - A handle to instance.
    @param pTime        (OUT) - Retrieved duration
    
    @return DivXError
*/
DivXError L4Menus_GetTotalDuration(L4MenusHandle handle, DivXTime* pTime);

/*!
    Sets the media finished event for L4 Menus

    @param handle       (IN)  - A handle to instance.
    
    @return DivXError
    */
DivXError L4Menus_HandleMediaFinishedEvent(L4MenusHandle handle);

/*!
    Reads a menu block

    @param handle       (OUT) - A handle to instance.
    
    @return DivXError
*/
DivXError L4Menus_ReadNextBlock( L4MenusHandle handle, DMFBlockType_t *pBlockType, uint32_t *pnStream, DMFBlockNode_t *pBlockNode);

/*!
    Seek to a given position

    @param handle     (IN)     - A handle to instance.
    @param pTime      (IN/OUT) - Time to seek to and actual time seeked to returned
    
    @return DivXError
*/
DivXError L4Menus_Seek(L4MenusHandle handle, DivXTime* pTime);

/*!
    Sets the stream active

    @param handle       (IN) - A handle to instance.
    @param type         (IN) - block type
    @param index        (IN) - Index to set
    
    @return DivXError
*/
DivXError L4Menus_SetStreamNumActive(L4MenusHandle handle, DMFBlockType_t blockType, uint32_t index);

/*!
    Returns the currently active stream(s) 

    @param handle     (IN) - A handle to instance.
    @param blockType  (IN) - The block type (AUD,VID,SUB)
    @param pIndex     (IN) - The returned index

    @return The number of streams.
*/
DivXError L4Menus_GetActiveStreams(L4MenusHandle handle, DMFBlockType_t blockType, uint32_t* pIndex);

/*!
    Returns the number of streams by type

    @param handle       (IN) - A handle to instance.
    @param type         (IN) - block type
    
    @return DivXError
*/
uint32_t L4Menus_GetNumStreamsByType(L4MenusHandle handle, DMFBlockType_t type);

/*!
    Returns the video format for the given stream

    @param handle       (IN) - A handle to instance.
    @param videoStruct  (IN) - Video structure to be passed out
    @param nStream      (IN) - Stream to retrieve
    
    @return DivXError
*/
DivXError L4Menus_GetVideoFormat(L4MenusHandle handle, DMFVideoStreamInfo1_t *videoStruct, int32_t nStream);

/*!
    Returns the audio format for the given stream

    @param handle       (IN) - A handle to instance.
    @param audioStruct  (IN) - Video structure to be passed out
    @param nStream      (IN) - Stream to retrieve
    
    @return DivXError
*/
DivXError L4Menus_GetAudioFormat(L4MenusHandle handle, DMFAudioStreamInfo1_t *audioStruct, int32_t nStream);

/*!
    Returns the subtitle format for the given stream

    @param handle          (IN) - A handle to instance.
    @param subtitleStruct  (IN) - Video structure to be passed out
    @param nStream         (IN) - Stream to retrieve
    
    @return DivXError
*/
DivXError L4Menus_GetSubtitleFormat(L4MenusHandle handle, DMFSubtitleStreamInfo1_t *subtitleStruct, int32_t nStream);

/*!
    Check if reset is necessary

    @param handle   (IN) - A handle to instance.
    @param pEvent   (IN) - The event that occured in playback
    
    @return DivXError
*/
DivXBool L4Menus_Reset(L4MenusHandle handle);

/*!
    Set reset

    @param handle   (IN) - A handle to instance.
    @param val      (IN) - The reset state
    
    @return DivXError
*/
DivXError L4Menus_SetReset(L4MenusHandle handle, DivXBool val);

/*!
    Checks for an L4 Event. 
    This function should be called 

    @param handle   (IN) - A handle to instance.
    @param pEvent   (IN) - The event that occured in playback
    
    @return DivXError
*/
DivXError L4Menus_SetEvent(L4MenusHandle handle, L4MenuEvent_e* pEvent);

/*!
    Get mode

    @param handle  (IN) - A handle to instance.
    @param pMode   (IN) – Returns mode currently set to
    @param pIndex (OUT) - If opening feature index of title to open
    
    @return DivXError
*/
DivXError L4Menus_GetMode(L4MenusHandle handle, L4Mode_e* pMode, int16_t* pIndex);

/*!
Set mode (not required on initialization)
If the mode comes back as L4_MODE_MENUS, then we skip directly to file playback without calling SetTitle().  Any SetTitle that is called in the L4_MODE_MENUS mode will force it to exit.  After a SetTitle() call is made the API will allow the user to return to a menu by calling SetMode(L4_MODE_MENUS).


    @param handle  (IN) - A handle to instance.
    @param mode    (IN) – Mode to be set to  
  
    @return DivXError
*/
DivXError L4Menus_SetMode(L4MenusHandle handle, L4Mode_e mode);

/*!
    Retrieves the languages supported by this file

    @param handle    (IN)  - A handle to instance.
    @param pLangList (OUT) – A linked list of languages
  
    @return DivXError
*/
DivXError L4Menus_GetLanguages(L4MenusHandle handle, LinkedList_t* pLangList);

/*!
    Sets the language used for playback

    @param handle    (IN)  - A handle to instance.
    @param pLangList (OUT) – A linked list of languages
  
    @return DivXError
*/
DivXError L4Menus_SetLanguage(L4MenusHandle handle, DivXString* lang);

#ifdef __cplusplus
}
#endif

#endif //_L4MENUS_H_
