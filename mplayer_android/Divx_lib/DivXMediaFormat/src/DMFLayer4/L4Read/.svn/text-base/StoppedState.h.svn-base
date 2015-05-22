/*!
    @file
@verbatim
$Id: StoppedState.h 58819 2009-03-05 00:16:10Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _STOPPEDSTATE_H_
#define _STOPPEDSTATE_H_
#include "PlaybackState.h"
#include "PBStates.h"

CLASS(StoppedState ,PBState)
VTABLE(StoppedState ,PBState)
METHODS
    StoppedState  StoppedState_Con(StoppedState this, pPBStateEngine pStateEngine);
    Transition_t  StoppedUpdate(StoppedState this, PBCache_t* pbCache);
END_CLASS

Transition_t StoppedExecute(StoppedState this, PBCache_t* pbCache);
Transition_t StoppedPlayPause(StoppedState this, PBCache_t* pbCache);

BEGIN_VTABLE(StoppedState, PBState)
    IMETHOD(super.PlaybackState, Execute)    = StoppedExecute;
    IMETHOD(super.PlaybackState, PLAY_PAUSE) = StoppedPlayPause;
END_VTABLE


#endif /*_STOPPEDSTATE_H*/
