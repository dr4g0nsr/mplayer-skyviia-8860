/*!
    @file
@verbatim
$Id: libMenuController.h 56375 2008-10-08 19:39:38Z eleppert $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBMENUCONTROLLER_H_
#define _LIBMENUCONTROLLER_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "./libPlaybackState.h"
#include "./L3Read/Layer3ReadPublished.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Menu Controller handle definition */
typedef struct _MenuControlInstanceType *MenuControlHandle;

/*! event enumeration */
typedef enum DMCEventType
{
    DMCEventOpen,
    DMCEventUp,
    DMCEventDown,
    DMCEventLeft,
    DMCEventRight,
    DMCEventSelect,
    DMCEventMediaFinished,
    DMCEventRootMenu,
    DMCEventTitleMenu,
    DMCEventChapterMenu,
    DMCEventAudioMenu,
    DMCEventSubtitleMenu,
    DMCEventMouseOver,
    DMCEventMouseClick
} DMCEvent;

/*! Variant parameter for DMC, used to define mouse position (X,Y) */
typedef struct _DMCVariant
{
    union
    {
        struct mouseXY
        {
            int32_t X;
            int32_t Y;
        } mouseXY;
    } v;
} DMCVariant;

/*! 
    Note: Initializes Menu Controller with High Level Model

    @param instance (OUT) Instance Handle for this Menu Control session
    @param hModel    (IN)  Handle of High Level Model
    @param pS        (IN)  Playback state handle
    @param Pool      (IN)  Memory pool

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
*/
DivXError dmcInit( MenuControlHandle  *instance,
                   ContainerL3Handle   hModel,
                   PlaybackStateHandle pS,
                   DivXMem             Pool );

/*! 
    Executes an Event against a current Menu Control instance

    @param instance (IN)  Instance for this Menu control session
    @param dmfEvent (IN)  Enum of DMF event (user action or media state update)
    @param var     (OUT)  True if playback state has changed, false otherwise
    @param update  (OUT)  True if there has been a change in state

    @return DivXError (OUT) - Returns a DivXError error code (See DivXError.h)
*/
DivXError dmcEvent( MenuControlHandle instance,
                    DMCEvent          dmfEvent,
                    DMCVariant       *var,
                    DivXBool         *update );

/*! 
    Loads a menu and sets state ready for playback

    @param inst   (IN) MenuController handle
    @param menu   (IN) L3 Menu handle
    @param lgIdx  (IN) Language group index

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
*/
DivXError dmcInitMenu( MenuControlHandle inst,
                       MenuL3Handle      menu,
                       int16_t           lgIdx );

/*! 
    Loads a title and sets state ready for playback

    @param inst  (IN) Instance for this Menu Control Session
    @param title (IN) L3 handle of title to open
    @param tIdx  (IN) Title index

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
*/
DivXError dmcInitTitle( MenuControlHandle inst,
                        TitleL3Handle     title,
                        int16_t           tIdx );

/*! 
    Closes and Cleans up the Menu Control Instance

    @param instance (IN) instance to menu control session

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
*/
DivXError dmcClose( MenuControlHandle instance );

#ifdef __cplusplus
}
#endif

#endif /* _LIBMENUCONTROLLER_H_ */
