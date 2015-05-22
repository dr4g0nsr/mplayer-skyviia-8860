/*!
    @file
@verbatim
$Id: libMenuControllerInternal.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBMENUCONTROLLERINTERNAL_H_
#define _LIBMENUCONTROLLERINTERNAL_H_

/* external interface */
#include "./DMFMenuController/libMenuController.h"

#include "DMFProperties.h"

#define ACTION_MENUTRANSITION   1
#define ACTION_BUTTONTRANSITION 2
#define ACTION_PLAYTITLE        3
#define ACTION_RESUMETITLE      4
#define ACTION_CHANGEAUDIO      5
#define ACTION_CHANGESUBTITLE   6

typedef struct _MenuActionType MenuAction;
typedef struct _MenuActionType
{
    uint8_t  Type;
    uint32_t Value1;
    uint32_t Value2;
    void    *pValue;
} MenuActionType;

typedef struct _MenuControlInstanceType MenuControlInstance;
typedef struct _MenuControlInstanceType
{
    ContainerL3Handle   Model;
    int16_t             currentTitle;
    int16_t             currentLanguageGroup;
    PlaybackStateHandle playState;
    DivXMem             MemPool;
    MenuAction          ActionList[6];
    uint8_t             ActionCount;
} MenuControlInstanceType;

/* internal-only structures and functions go here */
DivXError AddActionToActionList( MenuControlInstance *instance,
                                 L2ActionStruct       action );

DivXError ExecuteActions( MenuControlInstance *instance );

#endif /* _LIBMENUCONTROLLERINTERNAL_H_ */
