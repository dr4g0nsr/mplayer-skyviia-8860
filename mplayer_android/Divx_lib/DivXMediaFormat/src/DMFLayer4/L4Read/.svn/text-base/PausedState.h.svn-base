/*!
    @file
@verbatim
$Id: PausedState.h 58819 2009-03-05 00:16:10Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _PAUSEDSTATE_H_
#define _PAUSEDSTATE_H_
#include "PlaybackState.h"


CLASS(PausedState ,PBState)
VTABLE(PausedState ,PBState)
METHODS
    PausedState  PausedState_Con(PausedState this, pPBStateEngine pStateEngine);
    Transition_t PausedUpdate(PausedState this, PBCache_t* pbCache);
END_CLASS

Transition_t PausedExecute(PausedState this, PBCache_t* pbCache);
Transition_t PausedPlayPause(PausedState this, PBCache_t* pbCache);

BEGIN_VTABLE(PausedState, PBState)
    IMETHOD(super.PlaybackState, Execute)    = PausedExecute;
    IMETHOD(super.PlaybackState, PLAY_PAUSE) = PausedPlayPause;
END_VTABLE


#endif /*_PAUSEDSTATE_H_*/
