/*!
    @file
@verbatim
$Id: Layer3CommonPublished.h 56375 2008-10-08 19:39:38Z eleppert $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LAYER3COMMONPUBLISHED_H_
#define _LAYER3COMMONPUBLISHED_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFProperties.h"
#include "DMFActionFormat.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"

#include "libTranslator.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* The following code mimics the XML structure defined in the DivX Media File XML */
/* Reference Guide.doc.  This code is based on that definition.*/

/*! Handle definition for L3 container */
typedef struct _Container* ContainerL3Handle;

/*! Handle definition for L3 title */
typedef struct _Title* TitleL3Handle;

/*! Handle definition for L3 menu */
typedef struct _Menu *MenuL3Handle;

/*! handle ptr for L3 menu */
typedef MenuL3Handle *MenuL3HandlePtr;

/*! Handle definition for resource */
typedef struct _L3Resources* L3ResourcesHandle;

/*! Handle definition for Menus*/
typedef struct _Menus *MenusL3Handle;

/*! Handle definition for Streams*/
typedef struct _Streams *StreamsL3Handle;

/*! Handle definition for Chapters*/
typedef struct _Chapters *ChaptersL3Handle;

/*! Handle definition for GlobalMenus*/
typedef struct _GlobalMenus *GlobalMenusL3Handle;

/*! Handle definition for LanguageGroup*/
typedef struct _LanguageGroup *LanguageGroupL3Handle;

/*! Handle definition for MenuMedia*/
typedef struct _MenuMedia* MenuMediaL3Handle;

/*! Handle definition for TitleMedia*/
typedef struct _TitleMedia* TitleMediaL3Handle;

/*! Handle definition for VideoMedia*/
typedef struct _VideoMedia *VideoMediaL3Handle;

/*! Handle definition for AudioMedia*/
typedef struct _AudioMedia *AudioMediaL3Handle;

/*! Handle definition for Button*/
typedef struct _DivXButton *ButtonL3Handle;

/*! Handle definition for EnterAction*/
typedef struct _EnterAction *EnterActionL3Handle;

/*! Handle definition for ExitAction*/
typedef struct _ExitAction *ExitActionL3Handle;

/*! Handle definition for Nav*/
typedef struct _Nav *NavL3Handle;

/*! Handle definition for OverlayMedia*/
typedef struct _OverlayMedia *OverlayMediaL3Handle;

/*! Handle definition for Position*/
typedef struct _Position *PositionL3Handle;

/*! Handle definition for SelectAction*/
typedef struct _SelectAction *SelectActionL3Handle;

/*! Handle definition for Play*/
typedef void *PlayL3Handle;

/*! Handle definition for Chapter*/
typedef struct _Chapter *ChapterL3Handle;

/*! Handle definition for Action*/
typedef struct _Action *ActionL3Handle;

/*! Handle definition for SubtitleMedia*/
typedef struct _SubtitleMedia *SubtitleMediaL3Handle;

/*! Handle definition for TranslationLookup*/
typedef struct _TranslationLookup *TranslationLookupL3Handle;

#ifdef __cplusplus
}
#endif

#endif /* _LAYER3COMMONPUBLISHED_H_ */
