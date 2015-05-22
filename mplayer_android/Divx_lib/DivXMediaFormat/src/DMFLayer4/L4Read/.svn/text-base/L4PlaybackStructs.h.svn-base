/*!
    @file
@verbatim
$Id: L4PlaybackStructs.h 60084 2009-05-12 18:17:16Z jmurray $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#include "./L4Read/L4ReadTitle.h"
#include "./L4Read/L4ReadContainer.h"
#include "./L4Read/L4ReadMediaState.h"
#include "DivXFont.h"

#ifndef _L4PLAYBACKSTRUCTS_H_
#define _L4PLAYBACKSTRUCTS_H_

#define DPROVERLAY_QUEUE_FULL 0x11

#if defined WIN32
/* Suspend execution on Windows for 1 ms */
#define SUSPENDEXECUTIONTIME 1
#else
/* On non-windows platform, eg Sigma, do not suspend by setting to 0 ms */
#define SUSPENDEXECUTIONTIME 0
#endif

typedef enum{
    L4_REINIT_QUEUE,
    L4_REINIT_FINISHED
} L4ReInitState;

typedef struct _L4PlaybackTimerInfo_t
{
    DivXTime    renderTime; /* the absolute start time of the last video block submitted for render */
    DivXTime    duration;   /* the duration of the last video block submitted for render */
}L4PlaybackTimerInfo_t;

typedef struct _L4ActiveTracks_t
{
    int32_t    video;
    int32_t    audio;
    int32_t    subtitle;
} L4ActiveTracks_t;

typedef struct _L4PlaybackInst_t
{
    DivXMem                hMem;
    DivXBool               bByStreamMode;
    DMFTrickMode_e         trickMode;
    DMFTrickMode_e         lastTrickMode;
    DivXTime               dtCurAbsoluteTime;
    L4MenusHandle          hL4Menus;
    L4ReadTitleHandle      hL4ReadTitle;
    L4ReadContainerHandle  hL4Container;
    L4ReadMediaStateHandle hL4ReadMediaState;
    L3ResourcesHandle      hL3Resources;
    L4ReInitState          reInitState;
    DivXTime               reInitStartTime;
    uint32_t               numVideoTracks;
    int32_t                currentVideoIdx;
    uint32_t               numAudioTracks;
    int32_t                currentAudioIdx;
    uint32_t               numSubtitleTracks;
    int32_t                currentSubtitleIdx;
    L4RenderHandle         hRender;
    DMFBlockNode_t*        pBlockNode;
    DivXBool               bMenuMode;
    DivXBool               bTrickTrack;
    DMFHttpAuth            httpAuth;
    DivXBool               bEnableAutoChapters;
    DivXBool               bAutoChaptersOverride;
    uint8_t                autoChapterSpacingPercent;
    DivXTime               autoChapterDuration;
    uint32_t               numAutoChapters;
    uint32_t               currAutoChapter;
    float                  trickRate; /* Multiplier for playback */
    uint64_t               lastTime; /* Fake time for fast playback */
    DivXFont               hDivXFont; /* handle to the current subs font */
    void*                  pFont;     /* the ttf loaded into mem, DivXFont does not memcpy so need to maintain here */
    DivXBool               overlayInitialized;
    DivXBool               audioValidated;
    L4PlaybackTimerInfo_t  timerInfo;
    L4ActiveTracks_t       activeTracks; /* when trickplay mode is enabled the active track's indices are stored here */
}L4PlaybackInst_t;

//#define LOGFILEDATA

#if ! defined FRAME_LOG
#define LOGBLOCK(block,playerTime)
#define LOGDATA(format, ...) 
#define LOGSDETAILDATA(format, ...)
#else
#if defined LOGFILEDATA
/* Set maco to log the position in the file where the data is being written */
#define LOGPOSITION fprintf(frameLog,"%s:%d -",__FILE__,__LINE__)
#else
#define LOGPOSITION
#endif

#define LOGDATA(format, ...) LOGPOSITION; fprintf (frameLog, format, ## __VA_ARGS__)

#if defined WIN32
#define llu  I64u
#define lld  I64d
#endif
/* Define a macro which is defined as a function. Allows blank definition so no time overhead if FRAME_LOG not defined */
#define LOGBLOCK(pBlockNode, playerTime) LogBlock(pBlockNode, playerTime)
static void LogBlock(DMFBlockNode_t *pBlockNode, DivXTime playerTime)
{
    char *typeString;

    switch(pBlockNode->type)
    {
    case BLOCK_TYPE_VID:
        typeString = "VIDEO";
        break;
    case BLOCK_TYPE_AUD:
        typeString = "AUDIO";
        break;
    case BLOCK_TYPE_SUB:
        typeString = "SUBTITLE";
        break;
    default:
        typeString = "OTHER";
        break;
    }

    fprintf(frameLog, "%s size=%d period=%lld absoluteTime=%lld playerTime=%lld\n", typeString, pBlockNode->blockSize, pBlockNode->blockPeriod / DIVX_TIME_MILLISECOND, pBlockNode->absoluteStartTime / DIVX_TIME_MILLISECOND, playerTime);

}
#endif


#endif /* _L4PLAYBACKSTRUCTS_H_ */