/*!
    @file
@verbatim
$Id: ExitingState.h 58819 2009-03-05 00:16:10Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _EXITINGSTATE_H_
#define _EXITINGSTATE_H_
#include "PBStates.h"
#include "PlaybackState.h"

CLASS(ExitingState,PBState)
VTABLE(ExitingState,PBState)
METHODS
    ExitingState ExitingState_Con(ExitingState this, pPBStateEngine pStateEngine);
END_CLASS

Transition_t ExitingExecute(PBState this, PBCache_t* pbCache);
BEGIN_VTABLE(ExitingState, PBState)
    IMETHOD(super.PlaybackState, Execute)     = ExitingExecute;
END_VTABLE

#endif /* _EXITINGSTATE_H_ */
