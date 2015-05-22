/*!
    @file
@verbatim
$Id: libPlaybackState.h 56375 2008-10-08 19:39:38Z eleppert $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBPLAYBACKSTATE_H_
#define _LIBPLAYBACKSTATE_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "Layer3CommonPublished.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Enumeration for DMF modes
    DMFMenu - we are playing a regular menu
    DMFFeature - we are playing a regular title
    DMFForcedMenu - we are playing a non-negotiable menu (FBI warnings, etc..)
 */

typedef enum DMFModeType
{
    DMFPlayingMenu,
    DMFPlayingFeature,
    DMFPlayingForced,
    DMFOpeningMenu,
    DMFOpeningFeature,
    DMFOpeningForced,
    DMFUpdateMenu,
    DMFUpdateFeature,
    DMFUpdateForced
} DMFMode;

/*! Enumeration for player state
    DMFPlaying - we are playing a stream
    DMFPaused - a stream is open, but idle
    DMFStopped - no open stream
    DMFInvalid - the player is confused
 */

typedef enum DMFStateType
{
    DMFPlaying,
    DMFPaused,
    DMFStopped,
    DMFTrickPlay,
    DMFInvalid
} DMFState;

/*! handle to a playback state (handles one container) */
typedef struct PlaybackStateType *PlaybackStateHandle;

/*! Initializes a state management for a single container

    @param (OUT) Playback state handle

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsInit( PlaybackStateHandle *handle,
                   DivXMem              Pool );

/*! Sets the DMF mode for a container

    @param handle (IN) Playback state handle
    @param (IN) new DMF state for the container

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetMode( PlaybackStateHandle handle,
                      DMFMode             mode );

/*! Retrieves the DMF mode for a container

    @param handle (IN) Playback state handle
    @param (OUT) DMF state for the container

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetMode( PlaybackStateHandle handle,
                      DMFMode            *mode );

/*! Sets the Playback state for a container

    @param handle (IN) Playback state handle
    @param (IN) new playback state for the container

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetState( PlaybackStateHandle handle,
                       DMFState            state );

/*! Retrieves the playback state of a container

    @param handle (IN) Playback state handle
    @param (OUT) playback state for the container

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetState( PlaybackStateHandle handle,
                       DMFState           *state );

/*! Saves the active menu object id in a DMF container

    @param handle (IN) Playback state handle
    @param (IN)object id of new active menu

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetMenuId( PlaybackStateHandle handle,
                        int32_t             menuId );

/*! Retrieves the active menu in a DMF container

    @param handle (IN) Playback state handle
    @param (OUT) object Id of currently active menu

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetMenuId( PlaybackStateHandle handle,
                        int32_t            *menuId );

/*! Sets the active title id for a DMF container

    @param handle (IN) Playback state handle
    @param (IN) object id for current title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetCurrentTitle( PlaybackStateHandle handle,
                              int16_t             index );

/*! Sets the current title.

    @param handle (IN) Playback state handle
    @param (IN) title index number

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetCurrentTitle( PlaybackStateHandle handle,
                              int16_t            *index );

/*! Sets the current title.

    @param handle (IN) Playback state handle
    @param (IN) title index number
    @param (IN) title object id

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetTitleId( PlaybackStateHandle handle,
                         int16_t             title,
                         int32_t             titleId );

/*! Retrieves the active title for a container

    @param handle (IN) Playback state handle
    @param (OUT) object id for current title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetTitleId( PlaybackStateHandle handle,
                         int16_t             title,
                         int32_t            *titleId );

/*! Sets the active title id for a DMF container

    @param handle (IN) Playback state handle
    @param (IN) object id for current title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetButtonId( PlaybackStateHandle handle,
                          int32_t             buttonId );

/*! Retrieves the active title for a container

    @param handle (IN) Playback state handle
    @param (OUT) object id for current title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetButtonId( PlaybackStateHandle handle,
                          int32_t            *buttonId );

/*! Sets the active title id for a DMF container

    @param handle (IN) Playback state handle
    @param (IN) object id for current title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetChapterId( PlaybackStateHandle handle,
                           int16_t             title,
                           int32_t             chapterId );

/*! Retrieves the active title for a container

    @param handle      (IN) Playback state handle
    @param title       (IN) object id for current title
    @param chapterId  (OUT) returned chapter id

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetChapterId( PlaybackStateHandle handle,
                           int16_t             title,
                           int32_t            *chapterId );

/*! Sets the active menu in a DMF container

    @param handle (IN) Playback state handle
    @param menu   (IN) Model Handle of new active menu

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetMenuHandle( PlaybackStateHandle handle,
                            MenuL3Handle   menu );

/*! Retrieves the active menu in a DMF container

    @param handle    (IN) Playback state handle
    @param menu     (OUT) Model Handle of currently active menu

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetMenuHandle( PlaybackStateHandle handle,
                            MenuL3Handle  *menu );

/*! Sets the active title (menu title or feature title)

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Title index
    @param title     (IN) State object handle for title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetTitleHandle( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             TitleL3Handle   title );

/*! Retrieves the active title for a container

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Title index
    @param chapter  (OUT) State object handle for title

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetTitleHandle( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             TitleL3Handle  *title );

/*! Sets the active title (menu title or feature title)

    @param handle   (IN) Playback state handle
    @param titleIdx (IN) Title index
    @param chapter  (IN) State object handle for chapter

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetChapterHandle( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               ChapterL3Handle   chapter );

/*! Retrieves the active title for a container

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Title index
    @param chapter  (OUT) State object handle for chapter

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetChapterHandle( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               ChapterL3Handle  *chapter );

/*! Sets the active title (menu title or feature title)

    @param handle   (IN) Playback state handle
    @param titleIdx (IN) Title index
    @param frame    (IN) Frame to set

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetParseFrame( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            uint32_t            frame );

/*! Retrieves the active title for a container

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Model handle for current title
    @param frame    (OUT) Returns frame

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetParseFrame( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            uint32_t           *frame );

/*! Sets the active title (menu title or feature title)

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Model handle for current title
    @param frame     (IN) Sets frame decode

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetDecodeFrame( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             uint32_t            frame );

/*! Retrieves the active title for a container

    @param handle    (IN) Playback state handle
    @param titleIdx (OUT) Title index

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetDecodeFrame( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             uint32_t           *frame );

/*! Sets the active title (menu title or feature title)

    @param handle   (IN) Playback state handle
    @param titleIdx (IN) Title index
    @param frame    (IN) Frame to set

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetRenderFrame( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             uint32_t            frame );

/*! Retrieves the active title for a container

    @param handle    (IN) Playback state handle
    @param titleIdx  (IN) Title index
    @param frame    (OUT) Returns frame

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetRenderFrame( PlaybackStateHandle handle,
                             int16_t             titleIdx,
                             uint32_t           *frame );

/*! Sets the active button for the current menu

    @param handle (IN) Playback state handle
    @param button (IN) Model handle for active button

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetButtonHandle( PlaybackStateHandle handle,
                              ButtonL3Handle   button );

/*! Retrieves the active button for the current menu

    @param handle  (IN) Playback state handle
    @param button (OUT) Model handle for active button

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetButtonHandle( PlaybackStateHandle handle,
                              ButtonL3Handle  *button );

/*! Sets the Menu Language for a User

    @param handle      (IN) user state handle
    @param menuLanguage(IN) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetMenuLanguage( PlaybackStateHandle handle,
                              const DivXString   *menuLanguage );

/*! Retrieves the current menu language for a user

    @param handle        (IN) Playback state handle
    @param menuLanguage (OUT) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetMenuLanguage( PlaybackStateHandle handle,
                              DivXString         *menuLanguage );

/*! Sets the audio language for a user

    @param handle        (IN) Playback state handle
    @param titleIdx      (IN) Title index
    @param audioLanguage (IN) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetAudioLanguage( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               const DivXString   *audioLanguage );

/*! Retrieves the audio language for a user

    @param handle         (IN) Playback state handle
    @param titleIdx       (IN) Title index
    @param audioLanguage (OUT) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetAudioLanguage( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               DivXString         *audioLanguage );

/*! Sets the subtitle language for a user

    @param handle           (IN) Playback state handle
    @param titleIdx         (IN) Title index
    @param overlayLanguage (OUT) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetSubtitleLanguage( PlaybackStateHandle handle,
                                  int16_t             titleIdx,
                                  const DivXString   *overlayLanguage );

/*! Retrieves the subtitle language for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param subtitleLanguage (OUT) 2 character language code

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetSubtitleLanguage( PlaybackStateHandle handle,
                                  int16_t             titleIdx,
                                  DivXString         *overlayLanguage );

/*! Sets the Video Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param videoTrack        (IN) Index for video track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetVideoTrack( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            int32_t             videoTrack );

/*! Retrieves the Video Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param videoTrack       (OUT) Index for video track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetVideoTrack( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            int32_t            *videoTrack );

/*! Sets the Audio Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param audioTrack        (IN) Index for audio track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetAudioTrack( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            int32_t             audioTrack );

/*! Retrieves the Audio Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param audioTrack       (OUT) Index for audio track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetAudioTrack( PlaybackStateHandle handle,
                            int16_t             titleIdx,
                            int32_t            *audioTrack );

/*! Sets the Subtitle Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param overlayTrack      (IN) Index for overlay track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsSetSubtitleTrack( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               int32_t             overlayTrack );

/*! Retrieves the Subtitle Track for a user

    @param handle            (IN) Playback state handle
    @param titleIdx          (IN) Title index
    @param overlayTrack     (OUT) Index for overlay track

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsGetSubtitleTrack( PlaybackStateHandle handle,
                               int16_t             titleIdx,
                               int32_t            *overlayTrack );

/*! Deletes the container state

    @param handle (IN) Playback state handle to close

    @return DivXError        (OUT) - Returns a DivXError error code (See DivXError.h)
 */
DivXError dpsClose( PlaybackStateHandle handle );

#ifdef __cplusplus
}
#endif

#endif /* _LIBPLAYBACKSTATE_H_ */
