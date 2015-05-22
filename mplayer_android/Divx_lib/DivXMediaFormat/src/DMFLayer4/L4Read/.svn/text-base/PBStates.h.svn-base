/*!
    @file
@verbatim
$Id: PBStates.h 60394 2009-05-27 02:15:21Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#include "DivXError.h"
#include "L4Read/L4Playback.h"
#include "DivXFile.h"
#include "ProgressBar.h"

#ifndef _PBSTATES_H_
#define _PBSTATES_H_

typedef enum States_e
{
    PLAY_STATE = 0,
    PAUSED_STATE,
    TIMERESET_STATE,
    EXITING_STATE,
    STOPPED_STATE,
    EXIT_STATE
} States_e;

//  Playback events begin here,
//  currently called HandleEvent in L4Read 
typedef enum Actions_e
{
    NO_EVENT = 0,
    STOP_EVENT,
    QUIT_EVENT,
    SEEK_FORWARD_EVENT,
    SEEK_BACK_EVENT,
    SWITCH_VIDEO_EVENT,
    SWITCH_AUDIO_EVENT,
    SWITCH_SUBTITLE_EVENT,
    PLAY_PAUSE_EVENT,
    PAUSE_EVENT,
    PLAY_EVENT,
    NEXT_TITLE_EVENT,
    PREVIOUS_TITLE_EVENT,
    FF_EVENT_EVENT,
    RW_EVENT_EVENT,
    SEEK_AUTOCHAPTER_EVENT,
    HEARTBEAT_EVENT,
    GETEVENT_EVENT,
    NEXT_CHAPTER_EVENT,
    PREV_CHAPTER_EVENT,
    INFO_EVENT,
    UNKNOWN_EVENT
} Actions_e;

/* transitions indicate a state and action to invoke on the stateengine -- could this be an
    internal representation?  probably, but should always be invoked on exit state -- for now
    use as a return value from a state */
typedef struct Transition_t
{
    States_e    state;
    DivXError   err;
} Transition_t;

typedef enum PBLogLevel_e
{
    PB_LOG_OFF = 0,
    PB_LOG_1,
    PB_LOG_2
}PBLogLevel_e;

typedef enum PBOverlay_e
{
    PB_OVERLAY_NONE = 0,
    PB_OVERLAY_PROGBAR,
    PB_OVERLAY_SUBTITLE,
    PB_OVERLAY_SUBSWITCH,
    PB_OVERLAY_AUDSWITCH,
    PB_OVERLAY_CHAPSWITCH,
    PB_OVERLAY_BADAUD
}PBOverlay_e;

typedef struct PBOverlayInfo_t
{
    PBOverlay_e overlayType;
    uint32_t    biCompression; /* 4cc for compression used to init overlay */
    uint32_t    trackIndex; /* usefull if associated witha track */
    void*       extraData;  /* overlay specific data */
    DivXBool    showing;
    DivXTime    startTime;
    DivXTime    endTime;
    int32_t     count;
}PBOverlayInfo_t;

/* state engine variables -- used as input for now */
typedef struct PBCache_t
{
    DivXBool            exitOnFinish;
    DMFBlockNode_t*     pBlockNode;
    L4PlaybackHandle    pInst;
    DivXBool            dataReady;
    DMFBlockType_t      blockType;
    DivXTime            renderTime;
    int32_t             queueError;
    DivXTime            blockPeriod;
    DivXTime            videoTime;
    DivXTime            audioTime;
    Transition_t        __inputTransition;
    uint32_t            startCount;
    uint32_t            playCount; // arbitrary value chosen to allow decoders a good chance to get enough data to start decoding
    L4Mode_e            mode;
    uint64_t            nData;
    uint32_t            nStream;
    L4RenderEvents_e    renderEvent;
    DivXBool            bIdleState;
    DivXFile            logHandle;
    PBLogLevel_e        logLevel;
    PBOverlayInfo_t     curOverlayInfo;
    PBOverlayInfo_t     prevOverlayInfo;
    HPB                 hProgressBar;
    uint8_t*            pOverlayBuffer;
    DivXTime            preloadTime;
    uint32_t            trickPlayAdvanceLoad;

} PBCache_t;


#endif /* _PBSTATES_H_ */
