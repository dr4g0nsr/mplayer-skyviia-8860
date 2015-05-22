/*!
    @file
@verbatim
$Id: TimeResetState.h 58819 2009-03-05 00:16:10Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/


#ifndef _TIMERESETSTATE_H_
#define _TIMERESETSTATE_H_
#include "PlaybackState.h"

/* idle state declaration */
CLASS(TimeResetState, PBState)
VTABLE(TimeResetState, PBState)
METHODS
    TimeResetState TimeResetState_Con(TimeResetState this, pPBStateEngine pStateEngine);
END_CLASS

/* overrides for idlestate */
Transition_t TimeResetExecute(TimeResetState this, PBCache_t* pbCache);
BEGIN_VTABLE(TimeResetState, PBState)
    IMETHOD(super.PlaybackState, Execute) = TimeResetExecute;
END_VTABLE

#endif /* _TIMERESETSTATE_H_*/
