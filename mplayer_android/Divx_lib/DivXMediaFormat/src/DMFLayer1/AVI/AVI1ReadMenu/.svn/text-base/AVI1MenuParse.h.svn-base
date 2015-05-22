/*!
    @file
@verbatim
$Id: AVI1MenuParse.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1MENUPARSE_H_
#define _AVI1MENUPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "DMFProperties.h"
#include "DMFActionFormat.h"
#include "./AVI/AVI1ReadMenu/AVI1MPQueryStateData.h"
#include "./AVI/AVICommon/A1MPInstanceHandle.h"

typedef struct ActionMemberSwapTemplateType
{
    char     FourCC[4];
    uint32_t m[4];
} ActionMemberSwapTemplate;

typedef union
{
    PlayAction               play;
    PlayFromCurrentOffset    playCurrent;
    MenuTransitionAction     menuTrans;
    ButtonTransitionAction   buttonTrans;
    AudioSelectAction        audioSelect;
    SubtitleSelectAction     subtitleSelect;
    ActionMemberSwapTemplate swap;
} ActionCommandUnion;

typedef struct _ACTN_CMD_
{
    uint32_t           type;
    ActionCommandUnion cmd;
} ActionCommand;

#define ENTER_ACTION 0
#define EXIT_ACTION 1

#define MenuEnterAction     0
#define MenuExitAction      1
#define ButtonSelectAction  2

#define A1MP_BadInstanceHandle -1

int A1MPCreateInstance( void               *handleRead,
                        uint64_t            menuRiffOffset,
                        A1MPRead            fnread,
                        A1MPSeek            fnseek,
                        A1MPInstanceHandle *outputHandle,
                        uint8_t            *buffer,
                        int32_t             bufferSize,
                        void               *userData,
                        DivXMem             hMem );

int A1MPReleaseInstance( A1MPInstanceHandle handle,
                         DivXMem            hMem );

int A1MPSetMenuSystemVersion( A1MPInstanceHandle handle,
                              uint32_t           version );

int A1MPScanRiff( A1MPInstanceHandle handle );

int A1MPGetDivXMediaManagerInfo( A1MPInstanceHandle           handle,
                                 DivxMediaManagerChunkHeader *header );

int A1MPGetNumMenus( A1MPInstanceHandle handle,
                     int32_t           *iNum );

int A1MPGetMenuMediaOffsets( A1MPInstanceHandle handle,
                             uint64_t          *offsets,
                             uint32_t          *numMenuMedia );

int A1MPGetNumGlobalMenuGroups( A1MPInstanceHandle handle,
                                uint32_t          *iNum );

int A1MPGetGlobalGroupStartMenu( A1MPInstanceHandle handle,
                                 uint32_t           globalGroupIndex,
                                 uint32_t          *iNum );

#define A1MP_BadGroupIndex -2

int A1MPGetNumMenusInGlobalGroup( A1MPInstanceHandle handle,
                                  uint32_t           globalGroupIndex,
                                  uint32_t          *iNum );

int A1MPGetNumLanguageGroups( A1MPInstanceHandle handle,
                              uint32_t          *iNum );

int A1MPGetNumMenusInLanguageGroup( A1MPInstanceHandle handle,
                                    uint32_t           langGroupIndex,
                                    uint32_t          *iNum );

int A1MPGetLanguageGroupInfo( A1MPInstanceHandle handle,
                              uint32_t           index,
                              LanguageMenus     *langMenu );

#define A1MP_BadMenuIndex -3

int A1MPTranslationLangGroupMenuIndexToAbsolute( A1MPInstanceHandle handle,
                                                 uint32_t           langGroupIndex,
                                                 uint32_t           lgMenuIndex,
                                                 uint32_t          *absIndex );

int A1MPSetCurrentMenu( A1MPInstanceHandle handle,
                        uint32_t           menuNum );

int A1MPGetNumEnterActionsForMenu( A1MPInstanceHandle handle,
                                   uint32_t           menuNum,
                                   uint32_t          *iNum );

int A1MPGetCommandTypeInEnterAction( A1MPInstanceHandle handle,
                                     uint32_t           menuNum,
                                     uint32_t           commandIndex,
                                     uint32_t          *type );

int A1MPGetEnterActionCommand( A1MPInstanceHandle handle,
                               uint32_t           menuNum,
                               uint32_t           commandIndex,
                               ActionCommand     *action );

int A1MPGetNumExitActionsForMenu( A1MPInstanceHandle handle,
                                  uint32_t           menuNum,
                                  uint32_t          *iNum );

int A1MPGetCommandTypeInExitAction( A1MPInstanceHandle handle,
                                    uint32_t           menuNum,
                                    uint32_t           commandIndex,
                                    uint32_t          *type );

int A1MPGetExitActionCommand( A1MPInstanceHandle handle,
                              uint32_t           menuNum,
                              uint32_t           commandIndex,
                              ActionCommand     *action );

int A1MPGetMenuMediaStreamInfoById( A1MPInstanceHandle handle,
                                    uint32_t           iId,
                                    StreamInfo        *info );

int A1MPGetMenuInfo( A1MPInstanceHandle handle,
                     uint32_t           menuNum,
                     MenuInfo          *menu );

int A1MPGetNumMenuButtons( A1MPInstanceHandle handle,
                           uint32_t           menuNum,
                           uint32_t          *iNum );

int A1MPGetButtonInfo( A1MPInstanceHandle handle,
                       uint32_t           menuNum,
                       uint32_t           buttonNum,
                       ButtonInfo        *info );

int A1MPGetButtonPosition( A1MPInstanceHandle handle,
                           uint32_t           menuNum,
                           uint32_t           buttonNum,
                           uint32_t          *left,
                           uint32_t          *top,
                           uint32_t          *width,
                           uint32_t          *height );

int A1MPGetNumButtonCommands( A1MPInstanceHandle handle,
                              uint32_t           menuNum,
                              uint32_t           buttonNum,
                              uint32_t          *num );

int A1MPGetButtonCommandType( A1MPInstanceHandle handle,
                              uint32_t           menuNum,
                              uint32_t           buttonNum,
                              uint32_t           commandNum,
                              uint32_t          *type );

int A1MPGetButtonActionCommand( A1MPInstanceHandle handle,
                                uint32_t           menuNum,
                                uint32_t           buttonNum,
                                uint32_t           commandIndex,
                                ActionCommand     *action );

/* int A1MPGetMenuMediaStreamByIndex(A1MPInstanceHandle handle, uint8_t type, uint32_t index, StreamInfo *info ); */

/*
   int A1MPGetNumPlayCommandsInEnterAction(A1MPInstanceHandle handle, uint32_t menuNum, uint32_t *iNum);

   int A1MPGetNumPlayCommandsInExitAction(A1MPInstanceHandle handle, uint32_t menuNum, uint32_t *iNum);
 */

int A1MPGetNumCommandsInEnterAction( A1MPInstanceHandle handle,
                                     uint32_t           menuNum,
                                     uint32_t          *iNum );

int A1MPGetNumCommandsInExitAction( A1MPInstanceHandle handle,
                                    uint32_t           menuNum,
                                    uint32_t          *iNum );

int A1MPGetNumTitleAudStreams( A1MPInstanceHandle handle,
                               uint32_t           titleId,
                               uint32_t          *numAud );

int A1MPGetTitleOffsets( A1MPInstanceHandle handle,
                         uint64_t          *pOffsets,
                         uint32_t          *numTitles );

int A1MPGetTitleInfo( A1MPInstanceHandle handle,
                      uint32_t           titleNum,
                      TitleInfo         *title );

int A1MPGetTitleNumChapters( A1MPInstanceHandle handle,
                             uint32_t           titleNum,
                             uint32_t          *numChaps );

int A1MPGetTitleChapterInfo( A1MPInstanceHandle handle,
                             uint32_t           titleNum,
                             uint32_t           chapNum,
                             ChapterInfo       *chap );

int A1MPGetNumTranslationLookups( A1MPInstanceHandle handle,
                                  uint32_t          *num );

int A1MPGetNumLookupEntries( A1MPInstanceHandle handle,
                             uint32_t           lookupIndex,
                             uint32_t          *num );

int A1MPGetLookupEntryLength( A1MPInstanceHandle handle,
                              uint32_t           lookupIndex,
                              uint32_t           entryIndex,
                              uint32_t          *byteLen );

int A1MPGetLookupEntry( A1MPInstanceHandle handle,
                        uint32_t           lookupIndex,
                        uint32_t           entryIndex,
                        DivXString        *lCode,
                        uint16_t          *buffer,
                        uint32_t           bufferLen );

int A1MPGetLookupId( A1MPInstanceHandle handle,
                     uint32_t           lookupIndex,
                     uint32_t          *Id );

int A1MPGetTitleMediaStreamInfoByIndex( A1MPInstanceHandle handle,
                                        uint32_t           titleIndex,
                                        uint32_t           streamIndex,
                                        StreamInfo        *info );

int A1MPSetQueryStateIndexes( A1MPInstanceHandle handle,
                              int                QueryValueID,
                              int                Index );

int A1MPGetQueryStatePtr( A1MPInstanceHandle handle,
                          QueryStateData   **ptr );

#ifdef __cplusplus
}
#endif

#endif /* _AVI1MENUPARSE_H_ */
