/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#include "Object.h"
#include "PlayState.h"
#include "PausedState.h"
#include "ExitingState.h"
#include "TimeResetState.h"
#include "StoppedState.h"

#ifndef _PBSTATEENGINE_H_
#define _PBSTATEENGINE_H_

CLASS(PBStateEngine, Object)
    struct PlayState    playState;
    struct PausedState  pausedState;
    struct ExitingState exitingState;
    struct TimeResetState timeResetState;
    struct StoppedState stoppedState;
    States_e            curState;
    PBCache_t*          ppbCache;
    void (*pLogFunc)(PBStateEngine this, PBCache_t* pbCache);
VTABLE(PBStateEngine, Object)
METHODS
    PBStateEngine PBSECon(PBStateEngine this);
    DivXError     PBSESetCache(PBStateEngine this, PBCache_t* ppBCache);
    void          PBSESetCurState(PBStateEngine this, States_e state);
    PBState       PBSEGetCurStateObj(PBStateEngine this, States_e state);
    DivXError     PBSEGetEvent(PBStateEngine this, L4PlaybackHandle pInst, L4RenderEvents_e *peEvent);
    Transition_t  PBSEExecuteEvent(PBStateEngine this, L4RenderEvents_e renderEvent);
    void    PBSELogLevel1(PBStateEngine this, PBCache_t* pbCache);
    void    PBSELogLevel2(PBStateEngine this, PBCache_t* pbCache);
    void    PBSELogLevelOff(PBStateEngine this, PBCache_t *pbCache);
    void          PBSESetLogLevel(PBStateEngine this, PBLogLevel_e logLevel);
    DivXError     PBSEOpenLogFile(PBStateEngine this, DivXString* fileName);
END_CLASS

/* should be in pbstates.h */
DivXError InitPBCache(PBStateEngine this, PBCache_t* ppbCache, L4PlaybackHandle pInst, DMFBlockNode_t* pBlockNode, DivXBool exitOnFinish);

#endif /* _PBSTATEENGINE_H_ */
