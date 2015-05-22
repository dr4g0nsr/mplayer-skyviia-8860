/*!
    @file
   @verbatim
   $Id: DMFTagsReader.h 58500 2009-02-18 19:45:46Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 */

#ifndef DMFPLAYER_H
#define DMFPLAYER_H

/*!
   Header files
 */
#include "portab.h"

#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DivXTime.h"

#include "DMFHttpAuth.h"

#include "./L4Read/L4Playback.h"
#include "./L4Read/L4Render.h"
#include "./L4Read/L4ReadContainer.h"

#include "DPR.h"

/*!
   Error codes for DMFPlayer
 */
typedef enum {
    DMFPLAYER_OK = 0x00,

    /*! DMFPLAYER_FAIL: The player failed, most likely an invalid file name/path or corrupt file */
    DMFPLAYER_FAIL,

    /*! DMFPLAYER_REINIT: The new playlist is already obtained and there is no need to call dmcMenuFinished. */
    DMFPLAYER_REINIT,

    /*! DMFPLAYER_FINISHED_PLAYOUT: The current playlist has been played out; call dmcMenuFinished next. */
    DMFPLAYER_FINISHED_PLAYOUT,

    /*! DMFPLAYER_END_OF_FILE: End of file obtained. Close menu controller and exit to PLAY_UI state. */
    DMFPLAYER_END_OF_FILE,

    NUM_DMFPLAYER_ERRORS
} DMFPLAYER_ERRORS_e;

/*!
   DMFPlayer states
 */
typedef enum
{
    STOPPED,                    /* Stopped. */
    OPEN_MEDIA,                 /* Open selected file pause for enter to play */
    REOPEN_MEDIA,               /* Reopen selected file */
    PLAY_MENU,                  /* Playing DMF menu. */
    PLAY_MAIN_FEATURE,          /* Playing Main Feature/non-DMF file. */
    TRICKPLAY,                  /* Trickplay mode (FF/REW). */
    PAUSED_AFTER_PLAY,          /* Paused just after PLAY_MAIN_FEATURE. */
    PAUSED_AFTER_TRICKPLAY,     /* Paused just after TRICKPLAY. */
    SHUTDOWN_PLAYER             /* Shutdown player and exit. */
} DMFPlayerState_e;

#ifdef REMOTE_FILE_SUPPORT
    /*!
     * Memory pool size
     */
    #define DMF_PLAYER_POOL_SIZE 100000000

    /*!
     * Block memory size
     */
    #define DMF_PLAYER_BLOCK_SIZE 1000000
#else
    /*!
     * Memory pool size
     */
    #define DMF_PLAYER_POOL_SIZE 1000000

    /*!
     * Block memory size
     */
    #define DMF_PLAYER_BLOCK_SIZE 200000
#endif

/*!
 * This structure keeps track of number of active audio and subtitle tracks as well
 * as memory pool handles, resource handles, and handles from the DMF API
 */
typedef struct _PlayerInfo
{
    DMFBlockNode_t Block;
    uint64_t totalVideoTime;

    uint64_t currentStreamTime;

    /*! Set this rate for trickplay: playRate = 1x for normal playback up to MAX_PLAY_RATE. */
    int16_t playRate;

    /*! Set the direction to AVIIN_FORWARD for FF; AVIIN_REVERSE for REW. */
    int8_t playDirection;

    uint32_t titleIndex;

    uint32_t numTitles;

    uint32_t bMenuMode;

    /* State of DMF player */
    DMFPlayerState_e state;

    /* Keep the trick play position here in ms*/
    DivXTime trickPos;

    /* Keep the trick play rate in multiples */
    int32_t trickRate;

    /* Keep the trick play direction 1 or -1 */
    int32_t trickDir;

    /* Keep for chapter */
    int32_t numChapters;
    int32_t curChapter;

    /* Play and exit command */
    DivXBool playAndExit;
} PlayerInfo;

typedef struct _DMF_t
{
    L4PlaybackHandle hL4PlaybackHandle;
    L4RenderHandle   hRender;
} DMF_t;

typedef struct _DMFPlayerSession
{
    /* DMF objects */
    DMF_t dmf;

    /* Playout parameters */
    PlayerInfo playInfo;

    /* DPR variables */
    struct _DPR_t dpr;

    /* Memory handle */
    DivXMem hMem;

} DMFPlayerSession;


DivXBool subtitleCompressionSupported(uint32_t val);


#endif /* DMFPLAYER_H */
