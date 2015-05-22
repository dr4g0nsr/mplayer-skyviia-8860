/*!
    @file
@verbatim
$Id: PlaybackState.h 59648 2009-04-14 18:13:32Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#include "Object.h"
#include "./L4Read/L4Playback.h"
#include "PBStates.h"

#ifndef _PLAYBACKSTATE_H_
#define _PLAYBACKSTATE_H_

#define SUB_WIDTH   1080
#define SUB_HEIGHT  200

typedef struct PBStateEngine* pPBStateEngine;

OBJ_BEGIN_INTERFACE(PlaybackState)
    Transition_t (*NO_EVENT_HANDLER)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*STOP_EVENT_HANDLER)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*QUIT)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SEEK_FORWARD)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SEEK_BACK)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SWITCH_VIDEO)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SWITCH_AUDIO)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SWITCH_SUBTITLE)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*PLAY_PAUSE)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*PAUSE)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*PLAY)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*NEXT_TITLE)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*PREVIOUS_TITLE)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*FF_EVENT)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*RW_EVENT)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*SEEK_AUTOCHAPTER)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*NEXT_CHAPTER)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*PREV_CHAPTER)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*Execute)(PlaybackState this, PBCache_t* pbCache);
    Transition_t (*TOGGLE_PROGBAR)(PlaybackState this, PBCache_t* pbCache);
    void    (*__OnEntry)(PlaybackState this, PBCache_t* pbCache);
    void    (*__OnExit)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogLevel1)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogLevel2)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogEntryLevel1)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogEntryLevel2)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogEntryNoLog)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogExitLevel1)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogExitLevel2)(PlaybackState this, PBCache_t* pbCache);
    void    (*LogExitNoLog)(PlaybackState this, PBCache_t* pbCache);
OBJ_END_INTERFACE;

CLASS(PBState, Object) 
    IMPLEMENTS(PlaybackState);
    /* function pointer to current logging method */
    void (*pLogFunc)(PBState this, PBCache_t* pbCache);
    void (*pLogEntry)(PBState this, PBCache_t* pbCache);
    void (*pLogExit)(PBState this, PBCache_t* pbCache);
    Transition_t (*pPBSReadBlock)(PBState this, PBCache_t* pbCache);
    Transition_t (*pPBSHandleData)(PBState this, PBCache_t* pbCache);
    Transition_t (*pPBSHeartBeat)(PBState this, PBCache_t* pbCache);
    pPBStateEngine pStateEngine;
VTABLE(PBState, Object)
    EXTENDS(PlaybackState);
METHODS
    PBState State_Con(PBState this, pPBStateEngine pStateEngine);
    void    PBSSetLogLevel(PBState this, PBLogLevel_e logLevel);
END_CLASS

PBState PBState_Con(PBState this, pPBStateEngine pStateEngine);


#endif /*_PLAYBACKSTATE_H_*/
