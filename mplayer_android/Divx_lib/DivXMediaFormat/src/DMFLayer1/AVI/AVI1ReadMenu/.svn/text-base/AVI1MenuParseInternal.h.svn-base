/*!
    @file
@verbatim
$Id: AVI1MenuParseInternal.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1MENUPARSEINTERNAL_H_
#define _AVI1MENUPARSEINTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVI1ReadMenu/A1PMemory.h"

int GetAVIStructureHeader( A1MPInstanceData   *pInst,
                           AVIStructureHeader *header,
                           uint64_t            offset );

int GetAVIChunkHeader( A1MPInstanceData *pInst,
                       AVIChunkHeader   *header,
                       uint64_t          offset );

int GetDivXIdChunkHeader( A1MPInstanceData  *pInst,
                          DivXIdChunkHeader *header,
                          uint64_t           offset );

int GetChunkOffsetFromListById( A1MPInstanceData *pInst,
                                uint64_t          startOffset,
                                uint32_t          length,
                                uint32_t          id,
                                uint32_t         *pOffset );

int GetMediaManagerChunkHeader( A1MPInstanceData            *pInst,
                                DivxMediaManagerChunkHeader *header,
                                uint64_t                     offset );

int GetLanguageMenusHeader( A1MPInstanceData *pInst,
                            uint64_t          offset,
                            LanguageMenus    *pHdr );

int GetMenuHeader( A1MPInstanceData *pInst,
                   uint64_t          offset,
                   DivXMediaMenu    *pHdr );

int GetButtonHeader(  A1MPInstanceData *pInst,
                      uint64_t          offset,
                      ButtonMenu       *pButton );

int GetPositionHeader( A1MPInstanceData *pInst,
                       MenuRectangle    *mr,
                       uint64_t          offset );

int GetMediaTrackHeader( A1MPInstanceData *pInst,
                         uint64_t          startOffset,
                         MediaTrack       *track );

int GetMediaSourceHeader( A1MPInstanceData *pInst,
                          uint64_t          startOffset,
                          MediaSource      *source );

int GetTranslationEntryHeader( A1MPInstanceData *pInst,
                               uint64_t          startOffset,
                               TranslationEntry *entry );

int GetTranslationLength( A1MPInstanceData *pInst,
                          uint32_t          lookupIndex,
                          uint32_t          entryIndex,
                          uint32_t         *byteLen );

int GetMediaChildrenList( A1MPInstanceData *pInst,
                          uint64_t          mediaOffset,
                          uint64_t         *start,
                          uint32_t         *len );

int SetCurrentMenu( A1MPInstanceData *pInst,
                    uint32_t          menuNum );

/* absolute offset */
int GetMenuChildrenList( A1MPInstanceData *pInst,
                         uint32_t          menuNum,
                         uint32_t         *listOffset,
                         uint32_t         *listSize );

int GetNumTopLevelActionCommands( A1MPInstanceHandle handle,
                                  uint32_t           menuNum,
                                  int                type,
                                  uint32_t          *iNum );

int GetCommandTypeInTopLevelAction( A1MPInstanceHandle handle,
                                    uint32_t           menuNum,
                                    uint32_t           actionType,
                                    uint32_t           commandIndex,
                                    uint32_t          *type );

int GetTopLevelActionOffset( A1MPInstanceData *pInst,
                             uint32_t          menuNum,
                             int               type,
                             uint32_t         *pOffset );

int GetNumActionCommands( A1MPInstanceData *pInst,
                          uint64_t          offsetToACTN,
                          uint32_t         *iNum );

int GetActionCommandOffset( A1MPInstanceData *pInst,
                            uint64_t          offsetToACTN,
                            uint32_t          commandIndex,
                            uint32_t         *commandOffset );

int GetActionCommandType( A1MPInstanceData *pInst,
                          uint64_t          offsetToACTN,
                          uint32_t          commandIndex,
                          uint32_t         *iType );

int GetActionCommand( A1MPInstanceData *pInst,
                      uint64_t          offsetToACTN,
                      uint32_t          commandIndex,
                      ActionCommand    *pCommand );

/* dep */
int GetNumPlayCommandsInTopLevelAction( A1MPInstanceHandle handle,
                                        uint32_t           menuNum,
                                        int                type,
                                        uint32_t          *iNum );

int SetCurrentButton( A1MPInstanceData *pInst,
                      uint32_t          buttonNum );

/* offset from button */
int GetButtonChildrenList( A1MPInstanceData *pInst,
                           uint32_t          buttonNum,
                           uint64_t         *offset,
                           uint32_t         *size );

/* offset from button */
int GetButtonActionOffset( A1MPInstanceData *pInst,
                           uint32_t          buttonNum,
                           uint64_t         *offset );

int GetButtonPosition( A1MPInstanceData *pInst,
                       uint32_t          buttonNum,
                       uint32_t         *left,
                       uint32_t         *top,
                       uint32_t         *width,
                       uint32_t         *height );

int GetPlayCommand( A1MPInstanceData *pInst,
                    uint64_t          commandOffset,
                    PlayAction       *pAct );

int GetAudioSelectCommand( A1MPInstanceData  *pInst,
                           uint64_t           commandOffset,
                           AudioSelectAction *pAct );

int GetSubtitleSelectCommand( A1MPInstanceData     *pInst,
                              uint64_t              commandOffset,
                              SubtitleSelectAction *pAct );

int GetPlayCurrentCommand( A1MPInstanceData      *pInst,
                           uint64_t               commandOffset,
                           PlayFromCurrentOffset *pAct );

int GetMenuTransCommand( A1MPInstanceData     *pInst,
                         uint64_t              commandOffset,
                         MenuTransitionAction *pAct );

int GetButtonTransCommand( A1MPInstanceData       *pInst,
                           uint64_t                commandOffset,
                           ButtonTransitionAction *pAct );

/*

   int GetPlayCommandParams(A1MPInstanceData *pInst, uint64_t offsetACTN, uint32_t commandIndex, PlayAction *pAction);
 */

int GetNumTranslationLookups( A1MPInstanceData *pInst,
                              uint32_t         *numTrans );

int GetNumLookupEntries( A1MPInstanceData *pInst,
                         uint32_t          lookupIndex,
                         uint32_t         *numEntries );

int GetTranslationEntryLength( A1MPInstanceData *pInst,
                               uint32_t          lookupIndex,
                               uint32_t          entryIndex,
                               uint32_t         *byteLen );

int GetTranslationEntry( A1MPInstanceData *pInst,
                         uint32_t          lookupIndex,
                         uint32_t          entryIndex,
                         DivXString       *lang,
                         uint8_t          *buffer,
                         uint32_t          max );

int GetLookupId( A1MPInstanceData *pInst,
                 uint32_t          lookupIndex,
                 uint32_t         *id );

int SetCurrentLookup( A1MPInstanceData *pInst,
                      uint32_t          lookupIndex );

int SetCurrentEntry( A1MPInstanceData *pInst,
                     uint32_t          entryIndex );

int SetCurrentTitle( A1MPInstanceData *pInst,
                     uint32_t          titleNum );

int AddObjectIDLookup(A1MPInstanceData *pInst, ObjectIDLookup_t *pObjectIDLookup);

/*
Looks up the offset based on objectID.
*/
uint64_t GetOffsetByObjectID(A1MPInstanceData *pInst, uint32_t objectID);

/*
Looks up the offset based on the current index number.
*/
uint64_t A1MPGetCurrentMenuMediaOffset( A1MPInstanceHandle handle );

/*
Looks up the offset based on the index number.
*/
uint64_t A1MPGetCurrentMenuMediaOffsetFromIndex( A1MPInstanceHandle handle, int32_t index );

#ifdef __cplusplus
extern "C"
}
#endif

#endif /* _AVI1MENUPARSEINTERNAL_H_ */
