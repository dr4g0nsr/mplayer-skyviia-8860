/*!
    @file
   @verbatim
   $Id: DMFProperties.h 59666 2009-04-15 02:41:53Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFPROPERTIES_H_
#define _DMFPROPERTIES_H_

#include "DivXInt.h"
#include "GUIDGen.h"

/*!
    This file defines a number of structures used by higher layers to more efficiently
    retrieve info from the menu and title data.  Each structure is versioned indepedently.
 **/

/*!
    The following enumeration defines the track type of the current track
 */
typedef enum _DMFTrackType_e
{
    DMF_TRACK_TYPE_INVALID,
    DMF_TRACK_TYPE_TRICK,
    DMF_TRACK_TYPE_MASTER
} DMFTrackType_e;

/*!
    The following enumeration defines the menu type for each language group or global menus.
 */
typedef enum _DMFDrmSupportLevel_e
{
    DMFDrmLevel0, /* No support for DRM */
    DMFDrmLevel1, /* DRM Level 1 */
    DMFDrmLevel2, /* DRM Level 2 */
    DMFDrmLevel3, /* DRM Level 3 */
    DMFDrmLevel4, /* DRM Level 4 (fictitious) */
    DMFDrmLevel5  /* DRM Level 5 (fictitious) */
} DMFDrmSupportLevel_e;

/*!
    The following enumeration defines the menu type for each language group or global menus.
 */
typedef enum _DMFMenuType_t
{
    DMFMenuTypeRoot,
    DMFMenuTypeTitle,
    DMFMenuTypeChapter,
    DMFMenuTypeAudio,
    DMFMenuTypeSubtitle,
    DMFMenuTypeForce,
    DMFMenuTypeOther,
    DMFMenuTypeUnknown = -1
} DMFMenuType;

#define DMF_SUBTITLE_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFSubtitleMediaProp_t
{
    uint8_t        structVersion;
    int16_t        titlenumber;
    DivXString     locale[4];
    DivXString     countrycode[4];
    DivXString     mediadesc[4];
    DivXString     name[512];
    uint32_t       translationId;
} DMFSubtitleMediaProp_t;

#define DMF_CHAPTER_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFChapterProp_t
{
    uint8_t  structVersion;
    int32_t  objectid;
    int16_t  name;
    int16_t  count;
    uint32_t translationId;
    uint64_t startFrame;
    uint64_t endFrame;
    uint64_t startTime;
    uint64_t endTime;
    uint8_t  isHidden;
    uint8_t  isEnabled;
} DMFChapterProp_t;

#define DMF_PLAYLIST_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFPlaylistProp_t
{
    uint8_t  structVersion;
    uint32_t chapterCount;
    double   duration;
    uint8_t  isHidden;
    uint8_t  isDefault;
    uint8_t  isOrdered;
} DMFPlaylistProp_t;

#define DMF_POSITION_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFPositionProp_t
{
    uint8_t structVersion;
    int32_t left;
    int32_t top;
    int32_t width;
    int32_t height;
} DMFPositionProp_t;

#define DMF_OVERLAY_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFOverlayMediaProp_t
{
    uint8_t structVersion;
    int16_t titlenumber;
    int32_t frame;
} DMFOverlayMediaProp_t;

#define DMF_NAV_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFNavProp_t
{
    uint8_t structVersion;
    int16_t left;
    int16_t right;
    int16_t up;
    int16_t down;
} DMFNavProp_t;

#define DMF_BUTTON_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFButtonProp_t
{
    uint8_t    structVersion;
    int16_t    objectid;
    DivXString name[3];
    int16_t    overlayframe;
} DMFButtonProp_t;

#define DMF_AUDIO_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFAudioMediaProp
{
    uint8_t        structVersion;
    uint32_t       titlenumber;
    DivXString     locale[4];
    DivXString     countrycode[4];
    DivXString     mediadesc[4];
    DivXString     name[512];
    uint32_t       translationId; /* use DMF_QID_GET_XXX_STREAMNAME_PROP to get STRN*/
} DMFAudioMediaProp_t;

#define DMF_VIDEO_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFVideoMediaProp
{
    uint8_t        structVersion;
    int16_t        titlenumber;
    DMFTrackType_e trackType;    // Track type is master/trick/etc.
} DMFVideoMediaProp_t;

#define DMF_TITLE_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFTitleMediaProp_t
{
    uint8_t structVersion;
    int16_t numVideoMedia;
    int16_t numAudioMedia;
    int16_t numSubtitleMedia;
    int16_t numHDSubtitleMedia;
} DMFTitleMediaProp_t;

#define DMF_MENU_MEDIA_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFMenuMediaProp_t
{
    uint8_t structVersion;
    int16_t numVideoMedia;
    int16_t numAudioMedia;
    int16_t numOverlayMedia;
} DMFMenuMediaProp_t;

#define DMF_MENU_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFMenuProp_t
{
    uint8_t     structVersion;
    int32_t     numButton;
    int16_t     objectid;
    DMFMenuType type;
    int16_t     startbutton;
} DMFMenuProp_t;

#define DMF_LANGUAGE_GROUP_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFLanguageGroupProp_t
{
    uint8_t    structVersion;
    int32_t    numMenu;
    DivXString language[3];
    int16_t    startmenu;
    int16_t    rootmenu;
    int16_t    titleMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    chapterMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    audioMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    subtitleMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
} DMFLanguageGroupProp_t;

#define DMF_GLOBAL_MENUS_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFGlobalMenusProp_t
{
    uint8_t structVersion;
    int32_t numMenu;
    int32_t start;
    int16_t rootMenu;  /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t titleMenu; /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t chapterMenu;/* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t audioMenu; /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t subtitleMenu;/* This is the object ID set by menu type, only one allowed per global Menu set */
} DMFGlobalMenusProp_t;

#define DMF_MENUS_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFMenusProp_t
{
    uint8_t    structVersion;
    int16_t    defaultMenu;
    DivXString defaultLang[3];
    int32_t    numTranslationLookup;
    int32_t    numGlobalMenus;
    int32_t    numLanguageGroup;
} DMFMenusProp_t;

#define DMF_TITLE_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFTitleProp_t
{
    uint8_t  structVersion;
    int32_t  numPlaylists;
    int32_t  numChapters;
    int32_t  numStreams;
    int16_t  objectid;
    int16_t  count;
    uint32_t translationId;
} DMFTitleProp_t;

typedef struct _DMFTrickTrackProp_t
{
    CDivXUUID MasterTrackGUID;
    CDivXUUID MasterTitleGUID;
    uint32_t  masterTitleIndex;
    uint32_t  masterVidTrackIndex;
} DMFTrickTrackProp_t;

typedef struct _DMFMasterTrackProp_t
{
    CDivXUUID TrickTrackGUID;
    CDivXUUID TrickTrackTitleGUID;
    uint32_t  trickTitleIndex;
    uint32_t  trickVidTrackIndex;
} DMFMasterTrackProp_t;

typedef struct _DMFTrackProp_t
{
    DMFTrackType_e trackType;   // Track type is master/trick/etc.

} DMFTrackProp_t;

#define DMF_CONTAINER_PROP 0x10 /* Version 1.0 of the structure */

typedef struct _DMFContainerProp_t
{
    uint8_t structVersion;
    int32_t numTitle;
    int32_t numMenus;
} DMFContainerProp_t;

typedef enum _DMFLaceType_e
{
    LACETYPE_NONE = 0,
    LACETYPE_EBML,
    LACETYPE_FIXED,
    LACETYPE_XIPH
} DMFLaceType_e;

typedef struct _DMFLaceProp_t
{
    DMFLaceType_e laceType;
}DMFLaceProp_t;


typedef struct _DMFStreamNameProp_t
{
    DivXString locale[4];
    DivXString countrycode[3];
    DivXString mediadesc[3];
    DivXString name[512];
} DMFStreamNameProp_t;

typedef struct _DMFNameProp_t
{
    DivXString *locale;
    DivXString *countrycode;
    DivXString *name;
} DMFNameProp_t;

typedef enum
{
    DMF_TRICK_MODE_NONE    = 0x01,
    DMF_TRICK_MODE_FORWARD = 0x02,
    DMF_TRICK_MODE_REVERSE = 0x04,
} DMFTrickMode_e;


#endif /* _DMFPROPERTIES_H_ */
