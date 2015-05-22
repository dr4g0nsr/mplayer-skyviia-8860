/*!
    @file
   @verbatim
   $Id: DMFMenuQueryIDs.h 58955 2009-03-17 23:15:27Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMENUQUERYIDS_H_
#define _DMFMENUQUERYIDS_H_

#include "DMFQueryIDsType.h"

#define DMF_QID_CONTAINER_MENUBLOCK_COUNT                     QIDVAL( 0x1005, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Get number of menu blocks in container */
#define DMF_QID_CONTAINER_MENUBLOCK_ID                        QIDVAL( 0x1006, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< [n] Get HMenuBlock for origin:0 menu (see L2API.h) */
#define DMF_QID_CONTAINER_MENUBLOCK_RAW                       QIDVAL( 0x1007, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Read raw menu block, starting with 'MENU' and including all top-level menu blocks plus trailing word alignment padding */
#define DMF_QID_CONTAINER_TITLE_MEDS_ID                       QIDVAL( 0x1008, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< [HTitle] Get/set media source (MEDS) ID for title */

#define DMF_QID_MENUMEDIA_VIDEOMEDIA_FILENAME                 QIDVAL( 0x1100, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< File name of VideoMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_VIDEOMEDIA_TITLENUMBER              QIDVAL( 0x1101, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Title number of AudioMedia in TitleMedia */
#define DMF_QID_MENUMEDIA_AUDIOMEDIA_FILENAME                 QIDVAL( 0x1102, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< File name of AudioMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_AUDIOMEDIA_LOCALE                   QIDVAL( 0x1103, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Locale of AudioMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_AUDIOMEDIA_COUNTRYCODE              QIDVAL( 0x1104, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Country code of AudioMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_AUDIOMEDIA_MEDIADESC                QIDVAL( 0x1105, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Media descriptor of AudioMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_VIDEOFRAMECOUNT                     QIDVAL( 0x1106, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< The total number of video frames in MenuMedia */
#define DMF_QID_MENUMEDIA_FILE_OFFSET                         QIDVAL( 0x1107, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 ) /* !< Gets the offset of the menu media*/
#define DMF_QID_MENUMEDIA_VID_ABSOLUTE_TRACK_NUM              QIDVAL( 0x1107, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/
#define DMF_QID_MENUMEDIA_AUD_ABSOLUTE_TRACK_NUM              QIDVAL( 0x1108, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/
#define DMF_QID_MENUMEDIA_SUB_ABSOLUTE_TRACK_NUM              QIDVAL( 0x1109, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/

#define DMF_QID_ACTION_FORMAT                                 QIDVAL( 0x1320, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Get/set the action structure/union */

/* MENU Structure data for Query */
#define DMF_QID_NUM_CONTAINER_MENUS                           DMF_QID_CONTAINER_MENUBLOCK_COUNT /* !< Number of Menus in Container */
#define DMF_QID_NUM_MENUS_GLOBALMENUS                         QIDVAL( 0x4101, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of GlobalMenus in Menus */
#define DMF_QID_NUM_MENUS_LANGUAGEGROUP                       QIDVAL( 0x4102, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of LanguageGroups in Menus */
#define DMF_QID_NUM_LANGUAGEGROUP_MENU                        QIDVAL( 0x4103, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Menu in LanguageGroup */
#define DMF_QID_NUM_GLOBALMENUS_MENU                          QIDVAL( 0x4104, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Menu in GlobalMenus */
#define DMF_QID_NUM_MENU_MENUMEDIA                            QIDVAL( 0x4105, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of MenuMedia in Menu */
#define DMF_QID_NUM_TITLE_TITLEMEDIA                          QIDVAL( 0x4106, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of TitleMedia in Title */
#define DMF_QID_NUM_MENUMEDIA_AUDIOMEDIA                      QIDVAL( 0x4107, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of AudioMedia in MenuMedia */
#define DMF_QID_NUM_MENU_BUTTON                               QIDVAL( 0x4109, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Button in Menu */
#define DMF_QID_NUM_MENU_ENTERACTION                          QIDVAL( 0x410A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of EnterAction in Menu */
#define DMF_QID_NUM_MENU_EXITACTION                           QIDVAL( 0x410B, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of ExitAction in Menu */
#define DMF_QID_NUM_BUTTON_SELECTACTION                       QIDVAL( 0x410C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of SelectAction in Menu */
#define DMF_QID_NUM_CHAPTERS_CHAPTER                          QIDVAL( 0x4110, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Chapter in Chapters */
#define DMF_QID_NUM_PLAY_CHAPTER                              QIDVAL( 0x4111, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Chapter in Play */
#define DMF_QID_NUM_TITLE_CHAPTERS                            QIDVAL( 0x4116, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Chapters in Title */
#define DMF_QID_NUM_TRANSLATION_LOOKUPS                       QIDVAL( 0x4118, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Lookups in Translation Table */
#define DMF_QID_NUM_LOOKUP_ENTRIES                            QIDVAL( 0x4119, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of entries in Translation lookup */
#define DMF_QID_TRANSLATION_LOOKUP_ID                         QIDVAL( 0x4120, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< ID of Translation Lookup */
#define DMF_QID_TRANSLATION_LANGUAGE                          QIDVAL( 0x4121, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Translation Language String */
#define DMF_QID_TRANSLATION_STRING_SIZE                       QIDVAL( 0x4122, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Translation String size */
#define DMF_QID_TRANSLATION_STRING                            QIDVAL( 0x4123, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Translation String */
#define DMF_QID_CHAPTERS_CHAPTER_TRANSLATIONID                QIDVAL( 0x4126, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Translation ID of Chapter */
#define DMF_QID_CONTAINER_TITLE_TRANSLATIONID                 QIDVAL( 0x4128, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Translation ID of Title */
#define DMF_QID_TITLEMEDIA_AUDIOMEDIA_TRANSLATIONID           QIDVAL( 0x412A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Translation ID of Audio Media */
#define DMF_QID_TITLEMEDIA_SUBTITLEMEDIA_TRANSLATIONID        QIDVAL( 0x412C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Translation ID of Subtitle Media */

#define DMF_QID_MENUS_GLOBALMENUS_START                       QIDVAL( 0x4003, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Start of GlobalMenus in Menus */
#define DMF_QID_MENUS_LANGUAGEGROUP_LANGUAGE                  QIDVAL( 0x4004, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Language of LanguageGroup in Menus */
#define DMF_QID_MENUS_LANGUAGEGROUP_STARTMENU                 QIDVAL( 0x4005, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Start menu of LanguageGroup in Menus */
#define DMF_QID_MENUS_LANGUAGEGROUP_ROOTMENU                  QIDVAL( 0x4006, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Root menu of LanguageGroup in Menus */
#define DMF_QID_GLOBALMENUS_MENU_NAME                         QIDVAL( 0x4007, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Name of Menu in GlobalMenus */
#define DMF_QID_GLOBALMENUS_MENU_TYPE                         QIDVAL( 0x4008, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Type of Menu in GlobalMenus */
#define DMF_QID_GLOBALMENUS_MENU_STARTBUTTON                  QIDVAL( 0x4009, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Start button of Menu in GlobalMenus */
#define DMF_QID_LANGUAGEGROUP_MENU_NAME                       QIDVAL( 0x400A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Name of Menu in LanguageGroup */
#define DMF_QID_LANGUAGEGROUP_MENU_TYPE                       QIDVAL( 0x400B, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Type of Menu in LanguageGroup */
#define DMF_QID_LANGUAGEGROUP_MENU_STARTBUTTON                QIDVAL( 0x400C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Start button of Menu in LanguageGroup */
#define DMF_QID_MENU_BUTTON_NAME                              QIDVAL( 0x400D, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Name of Button in Menu */
#define DMF_QID_MENU_BUTTON_OVERLAYFRAME                      QIDVAL( 0x400E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Overlay frame of Button in Menu */
#define DMF_QID_BUTTON_NAV_LEFT                               QIDVAL( 0x400F, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Left of Nav in Button */
#define DMF_QID_BUTTON_NAV_RIGHT                              QIDVAL( 0x4020, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Right of Nav in Button */
#define DMF_QID_BUTTON_NAV_UP                                 QIDVAL( 0x4021, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Up of Nav in Button */
#define DMF_QID_BUTTON_NAV_DOWN                               QIDVAL( 0x4022, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Down of Nav in Button */
#define DMF_QID_MENUMEDIA_OVERLAYMEDIA_FILENAME               QIDVAL( 0x4023, DMF_QIDT_NOSTREAM, DMF_QIDT_STR8 )  /* !< File name of OverlayMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_OVERLAYMEDIA_TITLENUMBER            QIDVAL( 0x4024, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Title number of OverlayMedia in MenuMedia */
#define DMF_QID_MENUMEDIA_OVERLAYMEDIA_SOURCETRACK            QIDVAL( 0x4025, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Source track of OverlayMedia in MenuMedia */
#define DMF_QID_BUTTON_POSITION_LEFT                          QIDVAL( 0x4026, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Left of Position in Button */
#define DMF_QID_BUTTON_POSITION_TOP                           QIDVAL( 0x4027, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Top of Position in Button */
#define DMF_QID_BUTTON_POSITION_WIDTH                         QIDVAL( 0x4028, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Width of Position in Button */
#define DMF_QID_BUTTON_POSITION_HEIGHT                        QIDVAL( 0x4029, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Height of Position in Button */
#define DMF_QID_MENUS_DEFAULTLANG                             QIDVAL( 0x4030, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Height of Position in Button */
#define DMF_QID_MENUS_DEFAULTMENUID                           QIDVAL( 0x4031, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Height of Position in Button */

/* The following 4 items were moved to DMFQueryIDs.h */
//#define DMF_QID_CHAPTERS_CHAPTER_NAME                         QIDVAL( 0x404B, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Name of Chapter in Chapters */
//#define DMF_QID_CHAPTERS_CHAPTER_COUNT                        QIDVAL( 0x404C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Count of Chapter in Chapters */
//#define DMF_QID_CHAPTERS_CHAPTER_STARTTIME                    QIDVAL( 0x504C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< Start time of Chapter in Chapters */
//#define DMF_QID_CHAPTERS_CHAPTER_ENDTIME                      QIDVAL( 0x504D, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< End time of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_STARTFRAME                   QIDVAL( 0x406E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< Start frame of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_ENDFRAME                     QIDVAL( 0x406F, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< End frame of Chapter in Chapters */

#define DMF_QID_PLAY_CHAPTER_NAME                             QIDVAL( 0x4080, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Name of Chapter in Play */
#define DMF_QID_PLAY_CHAPTER_COUNT                            QIDVAL( 0x4081, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Count of Chapter in Play */
#define DMF_QID_PLAY_CHAPTER_STARTTIME                        QIDVAL( 0x4082, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< Start time of Chapter in Play */
#define DMF_QID_PLAY_CHAPTER_ENDTIME                          QIDVAL( 0x4083, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< End time of Chapter in Play */
#define DMF_QID_PLAY_CHAPTER_STARTFRAME                       QIDVAL( 0x4084, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< Start frame of Chapter in Play */
#define DMF_QID_PLAY_CHAPTER_ENDFRAME                         QIDVAL( 0x4085, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< End frame of Chapter in Play */

/* The following QIDs are all new ObjectID queries */

#define DMF_QID_CONTAINER_TITLE_OBJECTID                      QIDVAL( 0x4200, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Title */
#define DMF_QID_GLOBALMENUS_MENU_OBJECTID                     QIDVAL( 0x4201, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Menu */
#define DMF_QID_LANGUAGEGROUP_MENU_OBJECTID                   QIDVAL( 0x4202, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Menu */
#define DMF_QID_MENU_BUTTON_OBJECTID                          QIDVAL( 0x4203, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Button */
#define DMF_QID_PLAY_CHAPTER_OBJECTID                         QIDVAL( 0x4204, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Chapter */
#define DMF_QID_CHAPTERS_CHAPTER_OBJECTID                     QIDVAL( 0x4205, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Object ID for Chapter */

/* The Following QIDs are all new NUM queries */

#define DMF_QID_NUM_MENUMEDIA_VIDEOMEDIA                      QIDVAL( 0x4503, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_NUM_MENUMEDIA_OVERLAYMEDIA                    QIDVAL( 0x4504, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )

#define DMF_QID_NUM_BUTTON_NAV                                QIDVAL( 0x4506, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_NUM_BUTTON_POSITION                           QIDVAL( 0x4507, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_NUM_ENTERACTION_ACTION                        QIDVAL( 0x4508, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_NUM_EXITACTION_ACTION                         QIDVAL( 0x4509, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_NUM_SELECTACTION_ACTION                       QIDVAL( 0x450A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )

/* The following QIDs are all new type queries */

#define DMF_QID_GLOBALMENUS_MENU_NAME_SIZE                    QIDVAL( 0x6000, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_GLOBALMENUS_MENU_TYPE_SIZE                    QIDVAL( 0x6002, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_GLOBALMENUS_MENU_STARTBUTTON_SIZE             QIDVAL( 0x6004, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_LANGUAGEGROUP_MENU_NAME_SIZE                  QIDVAL( 0x6006, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_LANGUAGEGROUP_MENU_TYPE_SIZE                  QIDVAL( 0x6008, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_LANGUAGEGROUP_MENU_STARTBUTTON_SIZE           QIDVAL( 0x600A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */

#define DMF_QID_MENUMEDIA_VIDEOMEDIA_FILENAME_SIZE            QIDVAL( 0x600E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_MENUMEDIA_AUDIOMEDIA_FILENAME_SIZE            QIDVAL( 0x6010, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */

#define DMF_QID_MENU_BUTTON_NAME_SIZE                         QIDVAL( 0x6014, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_MENUMEDIA_OVERLAYMEDIA_FILENAME_SIZE          QIDVAL( 0x6016, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_CHAPTERS_CHAPTER_NAME_SIZE                    QIDVAL( 0x603A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_PLAY_CHAPTER_NAME_SIZE                        QIDVAL( 0x603C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */

#define DMF_QID_MENUS_LANGUAGEGROUP_STARTMENU_SIZE            QIDVAL( 0x6050, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */
#define DMF_QID_MENUS_LANGUAGEGROUP_ROOTMENU_SIZE             QIDVAL( 0x6052, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< String size */

/* The following QID's are all menu parser informational sets */

#define DMF_QID_SET_CONTAINER_TITLE_IDX                       QIDVAL( 0x6200, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title */
#define DMF_QID_SET_CONTAINER_MENUS_IDX                       QIDVAL( 0x6201, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menus */
#define DMF_QID_SET_TITLE_STREAMS_IDX                         QIDVAL( 0x6206, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title - streams */
#define DMF_QID_SET_STREAMS_STREAMDATA_IDX                    QIDVAL( 0x6207, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of streams - stream data */
#define DMF_QID_SET_TITLE_CHAPTERS_IDX                        QIDVAL( 0x6208, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title - chapters */
#define DMF_QID_SET_MENUS_GLOBALMENUS_IDX                     QIDVAL( 0x6209, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menus - global menus */
#define DMF_QID_SET_MENUS_LANGUAGEGROUP_IDX                   QIDVAL( 0x620A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menus - language group */
#define DMF_QID_SET_GLOBALMENUS_MENU_IDX                      QIDVAL( 0x6210, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of global menus - menu */
#define DMF_QID_SET_LANGUAGEGROUP_MENU_IDX                    QIDVAL( 0x6211, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of language group - menu */
#define DMF_QID_SET_MENU_MENUMEDIA_IDX                        QIDVAL( 0x6212, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu - menu media */
#define DMF_QID_SET_TITLE_TITLEMEDIA_IDX                      QIDVAL( 0x6213, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title - title media */
#define DMF_QID_SET_TITLEMEDIA_VIDEOMEDIA_IDX                 QIDVAL( 0x6214, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title media - video media */
#define DMF_QID_SET_MENUMEDIA_VIDEOMEDIA_IDX                  QIDVAL( 0x6215, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu media - video media */
#define DMF_QID_SET_MENUMEDIA_AUDIOMEDIA_IDX                  QIDVAL( 0x6216, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu media - audio media */
#define DMF_QID_SET_TITLEMEDIA_AUDIOMEDIA_IDX                 QIDVAL( 0x6217, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title media - audio media */
#define DMF_QID_SET_MENU_BUTTON_IDX                           QIDVAL( 0x6218, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title media - audio media */
#define DMF_QID_SET_MENU_ENTERACTION_IDX                      QIDVAL( 0x6219, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu - button */
#define DMF_QID_SET_MENU_EXITACTION_IDX                       QIDVAL( 0x621A, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu - enter action */
#define DMF_QID_SET_BUTTON_NAV_IDX                            QIDVAL( 0x621B, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu - exit action */
#define DMF_QID_SET_MENUMEDIA_OVERLAYMEDIA_IDX                QIDVAL( 0x621C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of menu media - nav */
#define DMF_QID_SET_BUTTON_POSITION_IDX                       QIDVAL( 0x621D, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of button - position */

#define DMF_QID_SET_ENTERACTION_ACTION_IDX                    QIDVAL( 0x6224, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of enter action - action */
#define DMF_QID_SET_EXITACTION_ACTION_IDX                     QIDVAL( 0x6225, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of exit action - action */
#define DMF_QID_SET_SELECTACTION_ACTION_IDX                   QIDVAL( 0x6226, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of select action - action */

#define DMF_QID_SET_TITLEMEDIA_SUBTITLEMEDIA_IDX              QIDVAL( 0x6227, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of title media - subtitle media */
#define DMF_QID_SET_SELECTACTION_PLAY_IDX                     QIDVAL( 0x6228, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of select action - play */
#define DMF_QID_SET_CHAPTERS_CHAPTER_IDX                      QIDVAL( 0x6229, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of chapters - chapter */
#define DMF_QID_SET_PLAY_CHAPTER_IDX                          QIDVAL( 0x6230, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of play - chapter */
#define DMF_QID_SET_TABLE_LOOKUP_IDX                          QIDVAL( 0x6237, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of Translation table - lookup */
#define DMF_QID_SET_TABLE_LOOKUP_ENTRY_IDX                    QIDVAL( 0x4238, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Current parse index of Translation Entry Idx */

/*!
    DMF_QID_MENUTITLE_* are all related to getting menu media (i.e. titles) from containers supporting menus (DMF)
    They are used in conjunction with L2SetMenuMedia(), which takes a handle returned by DMF_QID_MENUTITLE_ID
    Note that menu title here refers to an MRIF within a menu block.
 **/
#define DMF_QID_MENUTITLE_COUNT                               QIDVAL( 0x6400, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Number of titles in menu block of container */
#define DMF_QID_MENUTITLE_ID                                  QIDVAL( 0x6401, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< [n] Get title ID for origin:0 title index */
#define DMF_QID_MENUTITLE_STREAM_COUNT                        QIDVAL( 0x6402, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Get total stream count for menu title */
#define DMF_QID_MENUTITLE_VIDEO_STREAM_COUNT                  QIDVAL( 0x6410, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Get number of video streams in menu title */
#define DMF_QID_MENUTITLE_VIDEO_L1VIDEOINFO                   QIDVAL( 0x6412, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< [HStream] Get/set info for video stream handle (DivXVideoStreamInfo1_t* defined in DivXVideoStreamInfo1.h) */
#define DMF_QID_MENUTITLE_AUDIO_STREAM_COUNT                  QIDVAL( 0x6420, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Get number of audio streams in menu title */
#define DMF_QID_MENUTITLE_AUDIO_L1AUDIOINFO                   QIDVAL( 0x6422, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< [HStream] Get/set info for audio stream handle.  DivXAudioStreamInfo1_t* (defined in DivXAudioStreamInfo1.h) passed as data. */
#define DMF_QID_MENUTITLE_SUBTITLE_STREAM_COUNT               QIDVAL( 0x6430, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )  /* !< Get number of subtitle streams in menu title */
#define DMF_QID_MENUTITLE_SUBTITLE_L1VIDEOINFO                QIDVAL( 0x6432, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< [HStream] Get/set info for overlay stream handle. DivXVideoStreamInfo1_t* (defined in DivXVideoStreamInfo1.h) */
#define DMF_QID_MENUTITLE_STREAMTYPE                          QIDVAL( 0x6441, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< [HStream] Get stream type in MRIF to be cast as L1ChunkType (DEPRECATED) */
#define DMF_QID_MENUTITLE_STREAM_BLOCKTYPE                    QIDVAL( 0x6442, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< [HStream] Get stream type in MRIF to be cast as DivXBlockType_t */

/* / These are for internal use only */

#define DMF_QID_INT_DMNU_FH                                       QIDVAL( 0x7f00,\
                                                                          DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )
#define DMF_QID_INT_DMNU_OFF                                      QIDVAL( 0x7f01,\
                                                                          DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )

/***********************************************************/

/*!
   The following QIDs are used to communicate with L1 MenuChunk creation code
 */
/***********************************************************/

#define DMF_QID_SET_MCBUILD_CHUNKBUFFER                     QIDVAL( 0x9101, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Sets a data buffer for the code to build a block in */
#define DMF_QID_SET_MCBUILD_SCRATCHBUFFER                   QIDVAL( 0x9102, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Sets a data buffer for the code to build a block in */
#define DMF_QID_SET_MCBUILD_MEDIAMGR                        QIDVAL( 0x9103, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_TRANSTBL_ID                     QIDVAL( 0x9104, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )
#define DMF_QID_SET_MCBUILD_TRANSLOOKUP_ID                  QIDVAL( 0x9105, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )
#define DMF_QID_SET_MCBUILD_TRANSENTRY                      QIDVAL( 0x9106, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_MEDIA_ID                        QIDVAL( 0x9107, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )

#define DMF_QID_SET_MCBUILD_MENUMEDIA_ID                    QIDVAL( 0x9108, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )

#define DMF_QID_SET_MCBUILD_MEDIASOURCE                     QIDVAL( 0x9109, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MCBUILD_MEDIATRACK                      QIDVAL( 0x9109, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_TITLE                           QIDVAL( 0x910A, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_CHAP                            QIDVAL( 0x910B, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_LANGMENU                        QIDVAL( 0x910C, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_MENU                            QIDVAL( 0x910D, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_ACTION                          QIDVAL( 0x910E, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_ASAC                            QIDVAL( 0x910F, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MCBUILD_STAC                            QIDVAL( 0x9110, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MCBUILD_MNTX                            QIDVAL( 0x9111, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MCBUILD_PLAY                            QIDVAL( 0x9112, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_BUTTON                          QIDVAL( 0x9113, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MCBUILD_MREC                            QIDVAL( 0x9114, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_SET_MCBUILD_DONE                            QIDVAL( 0x9115, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !<Param will always be NULL */

#define DMF_QID_SET_MCBUILD_BLOCK                           QIDVAL( 0x9116, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !<Raw block injector */

#define DMF_QID_SET_MCBUILD_RELINK_MEDIA                    QIDVAL( 0x9117, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !<Fixup */

#define DMF_QID_GET_MCBUILD_GETBLOCKSIZE                    QIDVAL( 0x9118, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

/***********************************************************/

/*!
   The following QIDs are used to retrieve the DRM information
 */
/***********************************************************/

#define DMF_QID_HAS_MENU_DRM1                                QIDVAL( 0x8580, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )

#define DMF_QID_GET_MENU_DRM1_DATA_SIZE                      QIDVAL( 0x8581, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Retrieves the size of DRM 1 data in the current menu */

#define DMF_QID_GET_MENU_DRM1                                QIDVAL( 0x8582, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_MENU_DRM1                                QIDVAL( 0x8583, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_GET_MENU_DPIC_DATA_SIZE                      QIDVAL( 0x8584, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Retrieves the size of the DPIC data in the current menu */

#define DMF_QID_GET_MENU_DPIC_DATA                           QIDVAL( 0x8585, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieves the DPIC data in the current menu */
#define DMF_QID_SET_MENU_DPIC_DATA                           QIDVAL( 0x8586, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Sets the DPIC data in the current menu */

/***********************************************************/

/*!
   The following QIDs are used to retrieve the L3 model prop
   structs.  These QIDs deprecate most of the individual
   property QIDs above, including GET_NUM...
 */
/***********************************************************/

#define DMF_QID_GET_CHAPTER_PROP                                   QIDVAL( 0x8504,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Chapter Structure */
#define DMF_QID_SET_CHAPTER_PROP                                   QIDVAL( 0x8506,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Chapter Structure */

#define DMF_QID_GET_ACTION_PROP                                    QIDVAL( 0x8508,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Action Structure */
#define DMF_QID_SET_ACTION_PROP                                    QIDVAL( 0x850A,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Action Structure */

#define DMF_QID_GET_SELECT_ACTION_PROP                             QIDVAL( 0x850C,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the SelectAction Structure */
#define DMF_QID_SET_SELECT_ACTION_PROP                             QIDVAL( 0x850E,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the SelectAction Structure */

#define DMF_QID_GET_POSITION_PROP                                  QIDVAL( 0x8510,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Position Structure */
#define DMF_QID_SET_POSITION_PROP                                  QIDVAL( 0x8512,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Position Structure */

#define DMF_QID_GET_NAV_PROP                                       QIDVAL( 0x8518,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Nav Structure */
#define DMF_QID_SET_NAV_PROP                                       QIDVAL( 0x851A,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Nav Structure */

#define DMF_QID_GET_EXIT_ACTION_PROP                               QIDVAL( 0x851C,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the ExitAction Structure */
#define DMF_QID_SET_EXIT_ACTION_PROP                               QIDVAL( 0x851E,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the ExitAction Structure */

#define DMF_QID_GET_ENTER_ACTION_PROP                              QIDVAL( 0x8520,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the EnterAction Structure */
#define DMF_QID_SET_ENTER_ACTION_PROP                              QIDVAL( 0x8522,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the EnterAction Structure */

#define DMF_QID_GET_BUTTON_PROP                                    QIDVAL( 0x8524,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Button Structure */
#define DMF_QID_SET_BUTTON_PROP                                    QIDVAL( 0x8526,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Button Structure */

#define DMF_QID_GET_MENU_MEDIA_PROP                                QIDVAL( 0x8534,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the MenuMedia Structure */
#define DMF_QID_SET_MENU_MEDIA_PROP                                QIDVAL( 0x8536,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the MenuMedia Structure */

#define DMF_QID_GET_MENU_PROP                                      QIDVAL( 0x8538,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Menu Structure */
#define DMF_QID_SET_MENU_PROP                                      QIDVAL( 0x853A,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Menu Structure */

#define DMF_QID_GET_LANGUAGE_GROUP_PROP                            QIDVAL( 0x853C,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the LanguageGroup Structure */
#define DMF_QID_SET_LANGUAGE_GROUP_PROP                            QIDVAL( 0x853E,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the LanguageGroup Structure */

#define DMF_QID_GET_GLOBAL_MENUS_PROP                              QIDVAL( 0x8540,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the GlobalMenus Structure */
#define DMF_QID_SET_GLOBAL_MENUS_PROP                              QIDVAL( 0x8542,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the GlobalMenus Structure */

#define DMF_QID_GET_CHAPTERS_PROP                                  QIDVAL( 0x8544,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Chapters Structure */
#define DMF_QID_SET_CHAPTERS_PROP                                  QIDVAL( 0x8546,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Chapters Structure */

#define DMF_QID_GET_MENUS_PROP                                     QIDVAL( 0x855C,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Menus Structure */
#define DMF_QID_SET_MENUS_PROP                                     QIDVAL( 0x855E,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Menus Structure */

#define DMF_QID_GET_MENUMEDIA_VIDEO_INDEX                          QIDVAL( 0x8568,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

#define DMF_QID_CHAPTER_NAME_COUNT                                 QIDVAL( 0x8568,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Num chapter display props */

#define DMF_QID_CHAPTER_NAME_PROP                                  QIDVAL( 0x8568,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Chapter display prop DMFChapterDisplay*/

#endif /* _DMFMENUQUERYIDS_H_ */
