/*!
    @file
@verbatim
$Id: PlayState.h 58819 2009-03-05 00:16:10Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/


#ifndef _PLAYSTATE_H_
#define _PLAYSTATE_H_
#include "PlaybackState.h"
#include "PBStates.h"

/* play state declaration */
CLASS(PlayState, PBState)
VTABLE(PlayState, PBState)
METHODS
    PlayState PlayState_Con(PlayState this, pPBStateEngine pStateEngine);
    Transition_t PlayUpdateState(PBState this, PBCache_t* pbCache);
END_CLASS

Transition_t PlayExecute(PBState this, PBCache_t* pbCache);
BEGIN_VTABLE(PlayState, PBState)
    IMETHOD(super.PlaybackState, Execute) = PlayExecute;
END_VTABLE

#endif 
