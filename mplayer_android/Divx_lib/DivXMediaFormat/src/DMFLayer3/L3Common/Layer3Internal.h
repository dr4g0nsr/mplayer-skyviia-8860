/*!
    @file
@verbatim
$Id: Layer3Internal.h 59666 2009-04-15 02:41:53Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/
#ifndef _LAYER3INTERNAL_H_
#define _LAYER3INTERNAL_H_

#include "DivXInt.h"

#include "DMFProperties.h"
#include "DMFActionFormat.h"

#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"
#include "DMFWebProxy.h"
#include "DMFHttpAuth.h"
#include "DivXLinkedList.h"

#include "./DMFTranslator/libTranslator.h"
#include "DMFContainerHandle.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* Read Enumerations */
#ifndef ActionEnumDefine
#define ActionEnumDefine
typedef enum
{
    AudioSelectEnum,
    SubtitleSelectEnum,
    PlayEnum,
    ResumeEnum,
    MenuTransitionEnum,
    ButtonSelectEnum,
} Action_e;
#endif

#ifndef _L3Resource
typedef struct _L3Resources
{
    TranslatorHandle libTransHdl;
    DivXMem          memHdl;
    DMFHttpAuth     *httpAuth;
    DMFWebProxy     *webProxy;
} L3Resources;
#endif

/* Read Structures */
typedef struct _SubtitleMedia
{
    uint8_t                  flag;
    void                    *L2Handle;
    DivXString              *filename;
    int16_t                  titlenumber;
    DivXString               locale[3];
    DivXString               countrycode[3];
    DivXString               mediadesc[4];
    DivXString               name[512];
    uint32_t                 translationId;
    int16_t                  nStreamOfType;
    DMFSubtitleStreamInfo1_t subtitleStruct;
} SubtitleMedia;
typedef struct _Chapter
{
    uint8_t  flag;
    void    *L2Handle;
    int32_t  objectid;
    int16_t  name;
    int16_t  count;
    uint32_t translationId;
    uint64_t startFrame;
    uint64_t endFrame;
    uint64_t startTime;
    uint64_t endTime;
    LinkedList_t *chapNames;  /*List of DMFNameProp_t, chapter names, may be localized*/
} Chapter;
typedef struct _Action
{
    uint8_t        flag;
    void          *L2Handle;
    L2ActionStruct actionStruct;
} Action;
typedef struct _SelectAction
{
    uint8_t flag;
    void   *L2Handle;
    int16_t numAction;
    Action *action;
} SelectAction;
typedef struct _Position
{
    uint8_t flag;
    void   *L2Handle;
    int16_t left;
    int16_t top;
    int16_t width;
    int16_t height;
} Position;
typedef struct _OverlayMedia
{
    uint8_t                  flag;
    void                    *L2Handle;
    int16_t                  titlenumber;
    int16_t                  nStreamOfType;
    DMFSubtitleStreamInfo1_t overlayStruct;
} OverlayMedia;
typedef struct _Nav
{
    uint8_t flag;
    void   *L2Handle;
    int16_t left;
    int16_t right;
    int16_t up;
    int16_t down;
} Nav;
typedef struct _ExitAction
{
    uint8_t flag;
    void   *L2Handle;
    int16_t numAction;
    Action *action;
} ExitAction;
typedef struct _EnterAction
{
    uint8_t flag;
    void   *L2Handle;
    int16_t numAction;
    Action *action;
} EnterAction;
typedef struct _DivXButton
{
    uint8_t       flag;
    void         *L2Handle;
    Nav          *nav;
    Position     *position;
    SelectAction *selectaction;
    int16_t       objectid;
    DivXString    name[3];
    int16_t       overlayframe;
} DivXButton;
typedef struct _AudioMedia
{
    uint8_t               flag;
    void                 *L2Handle;
    uint32_t              titlenumber;
    DivXString           *filename;
    DivXString            locale[4];
    DivXString            countrycode[4];
    DivXString            mediadesc[4];
    DivXString            name[512];
    uint32_t              translationId;
    int16_t               nStreamOfType;
    DMFAudioStreamInfo1_t audioStruct;
} AudioMedia;
typedef struct _VideoMedia
{
    uint8_t               flag;
    void                 *L2Handle;
    int16_t               titlenumber;
    DMFVideoStreamInfo1_t videoStruct;
} VideoMedia;
typedef struct _TitleMedia
{
    uint8_t        flag;
    void          *L2Handle;
    int16_t        numVideoMedia;
    VideoMedia    *videomedia;
    int16_t        numAudioMedia;
    AudioMedia    *audiomedia;
    int16_t        numSubtitleMedia;
    SubtitleMedia *subtitlemedia;
} TitleMedia;
typedef struct _MenuMedia
{
    uint8_t       flag;
    void         *L2Handle;
    int16_t       numVideoMedia;
    VideoMedia   *videomedia;
    int16_t       numAudioMedia;
    AudioMedia   *audiomedia;
    int16_t       numOverlayMedia;
    OverlayMedia *overlaymedia;
} MenuMedia;
typedef struct _Menu
{
    uint8_t      flag;
    void        *L2Handle;
    EnterAction *enteraction;
    ExitAction  *exitaction;
    int16_t      numButton;
    DivXButton  *button;
    MenuMedia   *menumedia;
    int16_t      objectid;
    DMFMenuType  type;
    int16_t      startbutton;
    int16_t      count;
} Menu;
typedef struct _LanguageGroup
{
    uint8_t    flag;
    void      *L2Handle;
    int16_t    numMenu;
    Menu      *menu;
    DivXString language[3];
    int16_t    startmenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    rootmenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    menuCount;
    int16_t    titleMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    chapterMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    audioMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
    int16_t    subtitleMenu; /* This is the object ID set by menu type, only one allowed per Language Group */
} LanguageGroup;
typedef struct _GlobalMenus
{
    uint8_t flag;
    void   *L2Handle;
    int16_t numMenu;
    Menu   *menu;
    int16_t start;
    int16_t rootMenu;  /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t titleMenu; /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t chapterMenu;/* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t audioMenu; /* This is the object ID set by menu type, only one allowed per global Menu set */
    int16_t subtitleMenu;/* This is the object ID set by menu type, only one allowed per global Menu set */
} GlobalMenus;
typedef struct _Chapters
{
    uint8_t  flag;
    void    *L2Handle;
    int16_t  numChapter;
    Chapter *chapter;
} Chapters;
typedef struct _Streams
{
    uint8_t  flag;
    void    *L2Handle;
    uint32_t translationId;
} Streams;
typedef struct _SubtitleData
{
    uint8_t                  flag;
    void                    *L2Handle;
    DMFSubtitleStreamInfo1_t subStruct;
} SubtitleData;
typedef struct _AudioData
{
    uint8_t               flag;
    void                 *L2Handle;
    DMFAudioStreamInfo1_t audioStruct;
} AudioData;
typedef struct _VideoData
{
    uint8_t               flag;
    void                 *L2Handle;
    DMFVideoStreamInfo1_t videoStruct;
} VideoData;
typedef struct _TranslationLookup
{
    uint8_t   flag;
    void     *L2Handle;
    int32_t   numLookupEntries;
    uint32_t *id;
} TranslationLookup;
typedef struct _Menus
{
    uint8_t            flag;
    void              *L2Handle;
    int16_t            defaultMenu;
    DivXString         defaultLang[3];
    uint8_t            translationLookupFlag;
    int16_t            numTranslationLookup;
    TranslationLookup *translationLookup;
    int16_t            numGlobalMenus;
    GlobalMenus       *globalmenus;
    int16_t            numLanguageGroup;
    LanguageGroup     *languagegroup;
} Menus;
typedef struct _Title
{
    uint8_t     flag;
    void       *L2Handle;
    TitleMedia *titlemedia;
    int16_t     numPlaylists;
    int16_t     numChapters;
    Chapters   *chapters;
    int16_t     numStreams;
    Streams    *streams;
    int16_t     objectid;
    int16_t     count;
    int16_t     numstreams;
    uint32_t    translationId;

    DivXBool    bVirtualTitleFlag;
    DivXBool    bVirtualTitleIsHidden;
    DivXBool    bVirtualTitleIsDefault;
    DivXBool    bVirtualTitleIsOrdered;
    int16_t     virtualTitlePlaylistNum;
    int16_t     virtualTitleBaseTitleNum;
    double      virtualTitleDuration;
} Title;
typedef struct _Container
{
    uint8_t      flag;
    DMFContainerHandle L2Handle;
    int16_t      numTitle;
    Title       *title;
    int32_t      numMenus;
    Menus       *menus;
    L3Resources *res;
} Container;

#define MAX_CONTAINER_TITLE 100
#define MAX_CONTAINER_MENUS 1
#define MAX_TITLE_DESCRIPTION 1
#define MAX_TITLE_TITLEMEDIA 1
#define MAX_TITLE_CHAPTERS 100
#define MAX_TITLE_STREAMS 100
#define MAX_TITLE_TITLEDATA 1
#define MAX_MENUS_GLOBALMENUS 1
#define MAX_MENUS_LANGUAGEGROUP 100
#define MAX_GLOBALMENUS_MENU 100
#define MAX_LANGUAGEGROUP_MENU 100
#define MAX_TITLEDATA_VIDEODATA 1
#define MAX_TITLEDATA_AUDIODATA 100
#define MAX_TITLEDATA_SUBTITLEDATA 100
#define MAX_STREAMS_STREAMDATA 1
#define MAX_CHAPTERS_CHAPTER 100
#define MAX_GLOBALMENUS_MENU 100
#define MAX_LANGUAGEGROUP_MENU 100
#define MAX_MENU_ENTERACTION 1
#define MAX_MENU_EXITACTION 1
#define MAX_MENU_BUTTON 100
#define MAX_MENU_MENUMEDIA 1
#define MAX_MENU_ENTERACTION 1
#define MAX_MENU_EXITACTION 1
#define MAX_MENU_BUTTON 100
#define MAX_MENU_MENUMEDIA 1
#define MAX_MENUMEDIA_VIDEOMEDIA 1
#define MAX_MENUMEDIA_AUDIOMEDIA 100
#define MAX_MENUMEDIA_OVERLAYMEDIA 100
#define MAX_TITLEMEDIA_VIDEOMEDIA 1
#define MAX_TITLEMEDIA_AUDIOMEDIA 100
#define MAX_TITLEMEDIA_SUBTITLEMEDIA 100
#define MAX_AUDIOMEDIA_DESCRIPTION 1
#define MAX_AUDIOMEDIA_DESCRIPTION 1
#define MAX_BUTTON_NAV 1
#define MAX_BUTTON_POSITION 1
#define MAX_BUTTON_SELECTACTION 1
#define MAX_ENTERACTION_ACTION 100
#define MAX_EXITACTION_ACTION 1
#define MAX_SELECTACTION_ACTION 1
#define MAX_PLAY_CHAPTER 1
#define MAX_PLAY_CHAPTER 1
#define MAX_PLAY_CHAPTER 1
#define MAX_CHAPTER_DESCRIPTION 1
#define MAX_CHAPTER_DESCRIPTION 1
#define MAX_SUBTITLEMEDIA_DESCRIPTION 1
#define MAX_DESCRIPTION_TRANSLATION 1
#define MAX_DESCRIPTION_TRANSLATION 1
#define MAX_DESCRIPTION_TRANSLATION 1
#define MAX_DESCRIPTION_TRANSLATION 1

#ifdef __cplusplus
}
#endif

#endif /* _LAYER3INTERNAL_H_ */
