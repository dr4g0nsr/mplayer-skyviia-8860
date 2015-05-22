/*!
    @file
@verbatim
$Id: libPlaybackStateInternal.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBPLAYBACKSTATEINTERNAL_H_
#define _LIBPLAYBACKSTATEINTERNAL_H_

#include "./DMFPlaybackState/libPlaybackState.h"

typedef struct PlaybackStateType PlaybackState;
struct PlaybackStateType
{
    DivXMem  memPool;
    DMFMode  mode;
    DMFState state;

    DivXString menuLanguage[3];

    int32_t           menuId;
    MenuL3Handle menu;
    int32_t           buttonId;
    ButtonL3Handle button;
    int32_t           CurrentTitle;

    struct TitleInfoType
    {
        DivXString        audioLanguage[3];
        DivXString        subtitleLanguage[3];
        int32_t           videoTrackId;
        int32_t           audioTrackId;
        int32_t           subtitleTrackId;
        uint32_t          parseFrame;
        uint32_t          decodeFrame;
        uint32_t          renderFrame;
        int32_t           titleId;
        TitleL3Handle     title;
        int32_t           chapterId;
        ChapterL3Handle chapter;
    } TitleInfo[100];
};

#endif /* _LIBPLAYBACKSTATEINTERNAL_H_ */
