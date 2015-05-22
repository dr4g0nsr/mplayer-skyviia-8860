/*!
    @file
@verbatim
$Id: A1PMemory.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _A1PMEMORY_H_
#define _A1PMEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "./AVI/AVI1ReadMenu/AVI1MenuParse.h"

#define GLOBALMENU  1
#define LANGMENU    2
#define MAX_MENUS   1000

typedef struct _ObjectIDLookup_t
{
    int32_t  objectID;
    uint64_t offset;
}ObjectIDLookup_t;

struct A1MPInstanceDataStruct
{
    void            *handleRead;
    uint64_t         menuRiffOffset; /* offset from the beginning of the file */
    uint32_t         menuChunkOffset; /* offset from the beginning of the Riff */
    uint32_t         menuChunkSize;
    uint32_t         menuVersion;
    A1MPRead         fnread;
    A1MPSeek         fnseek;
    void            *userData;
    RDBufferHandle   handleBuffer;

    QueryStateData queryState;

    uint16_t numMenus;
    uint32_t menuOffsets[1100]; /* relative to chunk */
    uint8_t  menuFlags[1100];

    uint32_t mediaOffset; /* relative to chunk */
    uint32_t translationOffset; /* relative to chunk */

    DivxMediaManagerChunkHeader mediaManagerHeader;

    uint32_t      currentMenuIndex;
    uint32_t      currentMenuObjectID;
    DivXMediaMenu hdrMenu;
    uint32_t      diskSizeMenuHdr;
    uint64_t      currentMenuOffset;  /* absolute offset */

    LanguageMenus langGroupHeader;
    uint32_t      currentMenuGroupIndex;

    uint32_t numButtons;

    uint32_t enterActionOffset;
    uint32_t exitActionOffset;

    uint32_t   currentButtonIndex;
    uint32_t   buttonOffset;     /* relative to current menu */
    ButtonMenu buttonHeader;
    uint32_t   buttonHeaderDiskSize;

    uint32_t  currentTitleIndex;
    uint32_t  currentTitleOffset;
    TitleInfo titleHeader;
    uint32_t  numAudioStreams;
    uint32_t  numSubtitleStreams;

    uint32_t translationTableLength;
    uint32_t currentLookupId;
    uint32_t currentLookupOffset;
    uint32_t currentLookupIndex;
    uint32_t currentLookupLength;
    uint32_t currentEntryOffset;
    uint32_t currentEntryIndex;

    ObjectIDLookup_t objectIDLookup[MAX_MENUS*3];
    int32_t          nObjectIDLookups;
};

typedef struct A1MPInstanceDataStruct A1MPInstanceData;

A1MPInstanceData *A1MPAllocateInstanceData();

void A1MPFreeInstanceData( A1MPInstanceData *pInstance );

#ifdef __cplusplus
}
#endif

#endif /* _A1PMEMORY_H_ */
