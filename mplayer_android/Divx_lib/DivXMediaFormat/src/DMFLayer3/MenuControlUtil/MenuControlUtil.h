/*!
    @file
@verbatim
$Id: MenuControlUtil.h 56375 2008-10-08 19:39:38Z eleppert $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MENUCONTROLUTIL_H_
#define _MENUCONTROLUTIL_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXString.h"
#include "ActiveTracks.h"
#include "libMenuController.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void * MenuControlUtil;

int32_t mcUtil_Init( MenuControlUtil *hMenuControlUtil, DivXMem hMem );

int32_t mcUtil_Open( MenuControlUtil hMenuControlUtil, const DivXString *sInputFileName);

int32_t mcUtil_OpenCurrentMenu( MenuControlUtil hMenuControlUtil );

int32_t mcUtil_OpenCurrentTitle( MenuControlUtil hMenuControlUtil );

int32_t mcUtil_GetMenuCurrentPlaybackState(MenuControlUtil hMenuControlUtil);

int32_t mcUtil_GetActiveTracks(MenuControlUtil hMenuControlUtil, DivXBool *bMenuMode, ActiveTracks_t *activeTracks, uint64_t *offset );

int32_t mcUtil_GetMenuActiveTracks(MenuControlUtil hMenuControlUtil, ActiveTracks_t *activeTracks, uint64_t *offset );

int32_t mcUtil_GetTitleActiveTracks(MenuControlUtil hMenuControlUtil, ActiveTracks_t *activeTracks, uint64_t *offset);

int32_t mcUtil_HandleEvent(MenuControlUtil hMenuControlUtil, DMCEvent eventToHandle);

int32_t mcUtil_SetMenuLanguage(MenuControlUtil hMenuControlUtil, const DivXString *lang);

int32_t mcUtil_SetAudioLanguage(MenuControlUtil hMenuControlUtil, const DivXString *lang, ActiveTracks_t *activeTracks);

int32_t mcUtil_SetSubtitleLanguage(MenuControlUtil hMenuControlUtil, const DivXString *lang, ActiveTracks_t *activeTracks);

int32_t mcUtil_Close( MenuControlUtil hMenuControlUtil);

#ifdef __cplusplus
}
#endif

#endif //_MENUCONTROLUTIL_H_