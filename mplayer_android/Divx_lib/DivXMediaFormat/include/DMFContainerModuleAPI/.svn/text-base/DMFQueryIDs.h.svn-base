/*!
    @file
   @verbatim
   $Id: DMFQueryIDs.h 60063 2009-05-11 22:42:32Z snaderi $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFQUERYIDS_H_
#define _DMFQUERYIDS_H_

#include "DMFQueryIDsType.h"

/**
    Some queries provide iteration through lists by returning a count
    and providing a related query which gets a handle for a specified
    item within the list. All iterations start with 0. Handles should
    never be 0. Queries which take an iterator are marked [n] at the
    start of the comments. Queries which take a handle are marked [HStream],
    etc.

    Title-related operations are dependent on the sequence:
    1. SetTitle(HTitle)
    2-n. Title operations other than SetTitle()
 **/

/*!
    Data identifiers used in queries
    To provide consistency in naming, some aliases have been added.
 **/
#define DMF_QID_NUM_CONTAINER_TITLE                           QIDVAL( 0x1000, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of titles in container (does not include menus or titles within menu) */

#define DMF_QID_CONTAINER_TITLE_NAME                          QIDVAL( 0x1001, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )
#define DMF_QID_CONTAINER_TITLE_ID                            QIDVAL( 0x1004, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< [n] Get HTitle for origin:0 title (see L2API.h) */
#define DMF_QID_CONTAINER_FILE_NAME                           QIDVAL( 0x1002, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )
#define DMF_QID_CONTAINER_PREV_NAME                           QIDVAL( 0x1003, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )
#define DMF_QID_CONTAINER_NEXT_NAME                           QIDVAL( 0x1005, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )
#define DMF_QID_CONTAINER_MUX_APP                             QIDVAL( 0x1006, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )
#define DMF_QID_CONTAINER_WRITE_APP                           QIDVAL( 0x1007, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )


#define DMF_QID_CONTAINER_TITLE_RIFF_OFFSET                   QIDVAL( 0x1009, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< [HTitle] Get riff offset for a title */

#define DMF_QID_NUM_TITLE_STREAMS                             QIDVAL( 0x1050, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of title streams */
#define DMF_QID_NUM_TITLE_STREAMS_PROP                        QIDVAL( 0x1051, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of Streams in a Title */
#define DMF_QID_NUM_ACTIVE_TITLE_STREAMS                      QIDVAL( 0x1052, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Number of active title streams */

#define DMF_QID_TITLEMEDIA_PREV_SYNC_POINT                    QIDVAL( 0x105E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Gets the previous key frame */
#define DMF_QID_TITLEMEDIA_NEXT_SYNC_POINT                    QIDVAL( 0x105F, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 ) /* !< Gets the next key frame */
#define DMF_QID_TITLEMEDIA_FILE_OFFSET                        QIDVAL( 0x1060, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 ) /* !< Gets the offset of the title */
#define DMF_QID_TITLEMEDIA_VID_ABSOLUTE_TRACK_NUM             QIDVAL( 0x1060, DMF_QIDT_VID, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/
#define DMF_QID_TITLEMEDIA_AUD_ABSOLUTE_TRACK_NUM             QIDVAL( 0x1061, DMF_QIDT_AUD, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/
#define DMF_QID_TITLEMEDIA_SUB_ABSOLUTE_TRACK_NUM             QIDVAL( 0x1062, DMF_QIDT_SUB, DMF_QIDT_INT32 ) /* !< Absolute track number of stream based on order in file*/
#define DMF_QID_TITLEMEDIA_PREV_SYNC_TIME                     QIDVAL( 0x1063, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 ) /* !< Gets the previous sync point in time */
#define DMF_QID_TITLEMEDIA_NEXT_SYNC_TIME                     QIDVAL( 0x1064, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 ) /* !< Gets the next sync point in time */

#define DMF_QID_TITLEDATA_METADATA                            QIDVAL( 0x1123, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Get/set metadata */
#define DMF_QID_TITLEDATA_MENU_INFO                           QIDVAL( 0x1125, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Get/set raw menu info */
#define DMF_QID_TITLEDATA_STREAM_BLOCKTYPE                    QIDVAL( 0x1129, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< [HStream] Get stream type to be cast as DivXBlockType_t */

#define DMF_QID_NUM_TITLEMEDIA_VIDEOMEDIA                     QIDVAL( 0x4505, DMF_QIDT_VID, DMF_QIDT_INT16 ) /* !< Number of vid tracks up to max tracks. */
#define DMF_QID_NUM_TITLEMEDIA_AUDIOMEDIA                     QIDVAL( 0x4108, DMF_QIDT_AUD, DMF_QIDT_INT16 ) /* !< Number of aud tracks up to max tracks. */
#define DMF_QID_NUM_TITLEMEDIA_SUBTITLEMEDIA                  QIDVAL( 0x4112, DMF_QIDT_SUB, DMF_QIDT_INT16 ) /* !< Number of sub tracks up to max tracks. */

#define DMF_QID_MAX_SUPPORTED_VID_STREAMS                     QIDVAL( 0x8002, DMF_QIDT_VID, DMF_QIDT_INT16 ) /* !< Max video tracks that could be parsed by the system. */
#define DMF_QID_MAX_SUPPORTED_AUD_STREAMS                     QIDVAL( 0x8003, DMF_QIDT_AUD, DMF_QIDT_INT16 ) /* !< Max audio tracks that could be parsed by the system. */
#define DMF_QID_MAX_SUPPORTED_SUB_STREAMS                     QIDVAL( 0x8004, DMF_QIDT_SUB, DMF_QIDT_INT16 ) /* !< Max subtitle tracks that could be parsed by the system. */

/***********************************************************/
/*!
    These track count querys are only for querying the total number of tracks in the file.
    It will return counts over the number of supported tracks if present.
 **/
/***********************************************************/
#define DMF_QID_TOTAL_NUM_TITLEMEDIA_VIDEOMEDIA               QIDVAL( 0x8005, DMF_QIDT_VID, DMF_QIDT_INT16 ) /* !< Number total number of vid tracks in the file (includes unsupported tracks) */
#define DMF_QID_TOTAL_NUM_TITLEMEDIA_AUDIOMEDIA               QIDVAL( 0x8006, DMF_QIDT_AUD, DMF_QIDT_INT16 ) /* !< Number total number of aud tracks in the file (includes unsupported tracks) */
#define DMF_QID_TOTAL_NUM_TITLEMEDIA_SUBTITLEMEDIA            QIDVAL( 0x8007, DMF_QIDT_SUB, DMF_QIDT_INT16 ) /* !< Number total number of sub tracks in the file (includes unsupported tracks) */

#define DMF_QID_TITLEDATA_VIDEOFRAMECOUNT                     QIDVAL( 0x450B, DMF_QIDT_VID, DMF_QIDT_INT32 ) /* !< Total number of frames */
#define DMF_QID_TITLEDATA_VIDEO_FRAME_FROM_TIME               QIDVAL( 0x450C, DMF_QIDT_VID, DMF_QIDT_INT32 ) /* !< The frame number cooresponding to the time */
#define DMF_QID_TITLE_DURATION                                QIDVAL( 0x450E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< The total duration of the title in DivXTime. */

#define DMF_QID_VID_STREAM_FORMAT                             QIDVAL( 0x8010, DMF_QIDT_VID, DMF_QIDT_UINT8P )  /* !< Stream Structure DivXVidStreamFormat_t */
#define DMF_QID_AUD_STREAM_FORMAT                             QIDVAL( 0x8011, DMF_QIDT_AUD, DMF_QIDT_UINT8P )  /* !< Stream Structure DivXAudStreamFormat_t */
#define DMF_QID_SUB_STREAM_FORMAT                             QIDVAL( 0x8012, DMF_QIDT_SUB, DMF_QIDT_UINT8P )  /* !< Stream Structure DivXSubStreamFormat_t */

#define DMF_QID_GET_CONTAINER_TYPE                            QIDVAL( 0x8013, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< The type of the container returns DMFContainerType */

#define DMF_QID_TITLE_SUGGESTED_BUFFER_SIZE                   QIDVAL( 0x8014, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Gets the suggested buffer size for reading */
#define DMF_QID_MENU_SUGGESTED_BUFFER_SIZE                    QIDVAL( 0x8015, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Gets the suggested buffer size for reading */

#define DMF_QID_STREAM_NAME                                       QIDVAL( 0x8019, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_VID_STREAM_NAME                                   QIDVAL( 0x8016, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_AUD_STREAM_NAME                                   QIDVAL( 0x8017, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_SUB_STREAM_NAME                                   QIDVAL( 0x8018, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_VID_CODEC_ID                                      QIDVAL( 0x801a, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_AUD_CODEC_ID                                      QIDVAL( 0x801b, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_SUB_CODEC_ID                                      QIDVAL( 0x801c, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_VID_CODEC_NAME                                    QIDVAL( 0x801d, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_AUD_CODEC_NAME                                    QIDVAL( 0x801e, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)
#define DMF_QID_SUB_CODEC_NAME                                    QIDVAL( 0x801f, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)

#define DMF_QID_VID_MIME_TYPE                                 QIDVAL( 0x8016, DMF_QIDT_VID, DMF_QIDT_DSTRP )  /* !< Codec mime type */
#define DMF_QID_AUD_MIME_TYPE                                 QIDVAL( 0x8017, DMF_QIDT_AUD, DMF_QIDT_DSTRP )  /* !< Codec mime type */
#define DMF_QID_SUB_MIME_TYPE                                 QIDVAL( 0x8018, DMF_QIDT_SUB, DMF_QIDT_DSTRP )  /* !< Codec mime type */

/***********************************************************/

/*!
   The following QIDs are used to retrieve the L3 model prop
   structs.  These QIDs deprecate most of the individual
   property QIDs above, including GET_NUM...
 */
/***********************************************************/

#define DMF_QID_GET_SUBTITLE_MEDIA_PROP                            QIDVAL( 0x8500,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Retrieves the SubtitleMedia Structure */
#define DMF_QID_SET_SUBTITLE_MEDIA_PROP                            QIDVAL( 0x8502,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Sets the SubtitleMedia Structure */

#define DMF_QID_GET_HDSUBTITLE_MEDIA_PROP                          QIDVAL( 0x8503,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Retrieves the SubtitleMedia Structure */
#define DMF_QID_SET_HDSUBTITLE_MEDIA_PROP                          QIDVAL( 0x8504,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Sets the SubtitleMedia Structure */

#define DMF_QID_GET_OVERLAY_MEDIA_PROP                             QIDVAL( 0x8514,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the OverlayMedia Structure */
#define DMF_QID_SET_OVERLAY_MEDIA_PROP                             QIDVAL( 0x8516,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the OverlayMedia Structure */

#define DMF_QID_GET_AUDIO_MEDIA_PROP                               QIDVAL( 0x8528,\
                                                                           DMF_QIDT_AUD, DMF_QIDT_UINT8P )      /* !< Retrieves the AudioMedia Structure */
#define DMF_QID_SET_AUDIO_MEDIA_PROP                               QIDVAL( 0x852A,\
                                                                           DMF_QIDT_AUD, DMF_QIDT_UINT8P )      /* !< Sets the AudioMedia Structure */

#define DMF_QID_GET_VIDEO_MEDIA_PROP                               QIDVAL( 0x852C,\
                                                                           DMF_QIDT_VID, DMF_QIDT_UINT8P )      /* !< Retrieves the VideoMedia Structure */
#define DMF_QID_SET_VIDEO_MEDIA_PROP                               QIDVAL( 0x852E,\
                                                                           DMF_QIDT_VID, DMF_QIDT_UINT8P )      /* !< Sets the VideoMedia Structure */

#define DMF_QID_GET_TITLE_MEDIA_PROP                               QIDVAL( 0x8530,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the TitleMedia Structure */
#define DMF_QID_SET_TITLE_MEDIA_PROP                               QIDVAL( 0x8532,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the TitleMedia Structure */

#define DMF_QID_GET_STREAMS_PROP                                   QIDVAL( 0x854C,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Streams Structure */
#define DMF_QID_SET_STREAMS_PROP                                   QIDVAL( 0x854E,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Streams Structure */

/*
 #define DMF_QID_GET_SUBTITLE_DATA_PROP                             QIDVAL(0x8550,DMF_QIDT_UINT8P) //!< Retrieves the SubtitleData Structure
 #define DMF_QID_SET_SUBTITLE_DATA_PROP                             QIDVAL(0x8552,DMF_QIDT_UINT8P) //!< Sets the SubtitleData Structure

 #define DMF_QID_GET_AUDIO_DATA_PROP                                QIDVAL(0x8554,DMF_QIDT_UINT8P) //!< Retrieves the AudioData Structure
 #define DMF_QID_SET_AUDIO_DATA_PROP                                QIDVAL(0x8556,DMF_QIDT_UINT8P) //!< Sets the AudioData Structure

 #define DMF_QID_GET_VIDEO_DATA_PROP                                QIDVAL(0x8558,DMF_QIDT_UINT8P) //!< Retrieves the VideoData Structure
 #define DMF_QID_SET_VIDEO_DATA_PROP                                QIDVAL(0x855A,DMF_QIDT_UINT8P) //!< Sets the VideoData Structure
 */

#define DMF_QID_GET_CONTAINER_PROP                                 QIDVAL( 0x8564,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Container Structure */
#define DMF_QID_SET_CONTAINER_PROP                                 QIDVAL( 0x8566,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Container Structure */

#define DMF_QID_GET_TITLE_PROP                                     QIDVAL( 0x8560,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Title Structure */
#define DMF_QID_SET_TITLE_PROP                                     QIDVAL( 0x8562,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Title Structure */

#define DMF_QID_GET_TRACK_PROP                                     QIDVAL( 0x8580,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Retrieves the Track Structure */
#define DMF_QID_SET_TRACK_PROP                                     QIDVAL( 0x8582,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )      /* !< Sets the Track Structure */

#define DMF_QID_GET_TITLE_VIDEO_INDEX                              QIDVAL( 0x8567,\
                                                                           DMF_QIDT_VID, DMF_QIDT_UINT8P )

#define DMF_QID_GET_VID_STREAMNAME_PROP                            QIDVAL( 0x8569,\
                                                                           DMF_QIDT_VID, DMF_QIDT_UINT8P )      /* !< Retrieves the Video Stream Name Prop */
#define DMF_QID_SET_VID_STREAMNAME_PROP                            QIDVAL( 0x8570,\
                                                                           DMF_QIDT_VID, DMF_QIDT_UINT8P )      /* !< Sets the Video Stream Name Prop */
#define DMF_QID_GET_AUD_STREAMNAME_PROP                            QIDVAL( 0x8571,\
                                                                           DMF_QIDT_AUD, DMF_QIDT_UINT8P )      /* !< Retrieves the Audio Stream Name Prop */
#define DMF_QID_SET_AUD_STREAMNAME_PROP                            QIDVAL( 0x8572,\
                                                                           DMF_QIDT_AUD, DMF_QIDT_UINT8P )      /* !< Sets the Audio Stream Name Prop */
#define DMF_QID_GET_SUB_STREAMNAME_PROP                            QIDVAL( 0x8573,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Retrieves the Subtitle Stream Name Prop */
#define DMF_QID_SET_SUB_STREAMNAME_PROP                            QIDVAL( 0x8574,\
                                                                           DMF_QIDT_SUB, DMF_QIDT_UINT8P )      /* !< Set the Subtitle Stream Name Prop */
#define DMF_QID_SET_TOOL_VERSION                                   QIDVAL( 0x8575,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Sets the tool version (gets written to AVI 2.0 files) */
#define DMF_QID_GET_TOOL_VERSION                                   QIDVAL( 0x8576,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Gets the tool version (gets written to AVI 2.0 files) */
#define DMF_QID_GET_FORMAT_VERSION                                 QIDVAL( 0x8577,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Sets the format version (gets written to AVI 2.0 files)*/
#define DMF_QID_PROFILE_NAME                                       QIDVAL( 0x8578,\
                                                                           DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP ) /* !< Sets the profile name (gets written to AVI 2.0 files)*/

/***********************************************************/
/*!
   The following QIDs are used to retrieve/set the AVI 2.0 information
 */
/***********************************************************/
#define DMF_QID_GET_AVI_RIFF_SIZE                          QIDVAL( 0x4101, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Number in bytes of the AVI RIFF, MRIF any starting RIFF */
#define DMF_QID_SET_AVI_RIFF_SIZE                          QIDVAL( 0x4102, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Number in bytes of the AVI RIFF, MRIF any starting RIFF */

#define DMF_QID_GET_AVIX_STEP_SIZE                         QIDVAL( 0x4103, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Number in bytes of the AVIX step size, 0 indicates no AVIX */
#define DMF_QID_SET_AVIX_STEP_SIZE                         QIDVAL( 0x4104, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Number in bytes of the AVIX step size, 0 indicates no AVIX */

#define DMF_QID_GET_INDX_TYPE                              QIDVAL( 0x4105, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get AVI 2.0 Index State:  idx1, idx1+indx, idx1+indx+ix##, indx, indx+ix## */
#define DMF_QID_SET_INDX_TYPE                              QIDVAL( 0x4106, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set AVI 2.0 Index State:  idx1, idx1+indx, idx1+indx+ix##, indx, indx+ix## */

#define DMF_QID_GET_STD_INDEX_NUM_ENTRIES                  QIDVAL( 0x4107, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get AVI 2.0 max num entries in each CHUNK('ix##') */
#define DMF_QID_SET_STD_INDEX_NUM_ENTRIES                  QIDVAL( 0x4108, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set AVI 2.0 max num entries in each CHUNK('ix##') */

#define DMF_QID_GET_INDX_GAP                               QIDVAL( 0x4109, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get AVI 2.0 num ms of video between CHUNK('ix##') */
#define DMF_QID_SET_INDX_GAP                               QIDVAL( 0x410A, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set AVI 2.0 num ms of video between CHUNK('ix##') */

#define DMF_QID_GET_INDEX_POS                              QIDVAL( 0x410B, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get AVI 2.0 index position (set where the index is in the movi list) */
#define DMF_QID_SET_INDEX_POS                              QIDVAL( 0x410C, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set AVI 2.0 index position (set where the index is in the movi list) */

#define DMF_QID_GET_SUPER_INDEX_NUM_ENTRIES                QIDVAL( 0x410D, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get AVI 2.0 the super index num entries */
#define DMF_QID_SET_SUPER_INDEX_NUM_ENTRIES                QIDVAL( 0x410E, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set AVI 2.0 the super index num entries */

/***********************************************************/
/*!
   The following QIDs are used to retrieve/set the internal index
 */
/***********************************************************/
#define DMF_QID_SET_ADVANCED_INDEX_GAP                     QIDVAL( 0x4400, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )


/***********************************************************/
/*!
   The following QIDs are used to retrieve/set the internal index
 */
/***********************************************************/
#define DMF_QID_GET_KEYFRAME_GAP                           QIDVAL( 0x4201, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Get gap between cached index entries */
#define DMF_QID_SET_KEYFRAME_GAP                           QIDVAL( 0x4202, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 ) /* !< Set gap between cached index entries */
#define DMF_QID_USE_INDEX                                  QIDVAL( 0x4203, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Parser will use cacheless indexing scheme is set to true */
#define DMF_QID_USE_CACHELESS_INDEX                        QIDVAL( 0x4204, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Parser will use cacheless indexing scheme is set to true */
#define DMF_QID_GET_MIN_MEM_USAGE                          QIDVAL( 0x4205, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Retrieves parser minimum memory usage setting */
#define DMF_QID_SET_MIN_MEM_USAGE                          QIDVAL( 0x4206, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Parser will use minimum memory if set to true */
#define DMF_QID_USE_HIERARCHICAL_INDEX                     QIDVAL( 0x4207, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Parser will use cacheless indexing scheme is set to true */

/***********************************************************/
/*!
   The following QIDs are used to set the srt access mode
 */
/***********************************************************/
#define DMF_QID_SET_SRT_OUTPUT_TEXT                     QIDVAL( 0x4300, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< SRT will output RLE chunks until this is set to true  */
#define DMF_QID_GET_SRT_OUTPUT_TEXT                     QIDVAL( 0x4301, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )  /* !< Checks if SRT will output RLE chunks until this is set to true  */

/***********************************************************/
/*!
    The following QIDs are used to setup for Progressive Playback
 */
/***********************************************************/
#define DMF_QID_SET_HTTP_AUTH                         QIDVAL( 0x2101, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Sets the HTTP Authorization structure */
#define DMF_QID_GET_HTTP_AUTH                         QIDVAL( 0x2102, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Gets the HTTP Authorization structure */
#define DMF_QID_SET_WEB_PROXY                         QIDVAL( 0x2201, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Sets the Web Proxy information */
#define DMF_QID_GET_WEB_PROXY                         QIDVAL( 0x2202, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Gets the Web Proxy information */
#define DMF_QID_GET_PROGRESS                          QIDVAL( 0x2301, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 )   /* !< Gets the percentage of file downloaded */
#define DMF_QID_GET_BITRATE                           QIDVAL( 0x2401, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Gets the bitrate */
#define DMF_QID_GET_FILE_SIZE                         QIDVAL( 0x2402, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )  /* !< Gets the file size */
#define DMF_QID_GET_WAIT_TIME                         QIDVAL( 0x2403, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 )   /* !< Gets the suggested wait time in DivXTime */

/***********************************************************/
/*!
   The following QIDs are used to retrieve the DRM information
 */
/***********************************************************/
#define DMF_QID_HAS_TITLE_DRM1                               QIDVAL( 0x8570, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 )
#define DMF_QID_GET_TITLE_DRM1_DATA_SIZE                     QIDVAL( 0x8571, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Retrieves the size of DRM 1 data in the current title */
#define DMF_QID_GET_TITLE_DRM1                               QIDVAL( 0x8572, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_TITLE_DRM1                               QIDVAL( 0x8573, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_TITLE_DRM1_VER                           QIDVAL( 0x8574, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_TITLE_DRM1_DATA_VER                      QIDVAL( 0x8575, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )

/***********************************************************/
/*!
   The following QIDs are used to retrieve the Metadata
 */
/***********************************************************/
#define DMF_QID_METADATA_TITLE_INFO                 QIDVAL( 0x9001, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Gets the title Info (see TitleInformation_t) */
#define DMF_QID_METADATA_BITSTREAM                  QIDVAL( 0x9002, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Gets the metadata bitstream (see MetadataBitstream_t) */
#define DMF_QID_METADATA_OFFSET                     QIDVAL( 0x9003, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 )   /* !< Gets the offset to the start of the metadata (the actual RIFF) */
#define DMF_QID_MENUMEDIA_OFFSET                    QIDVAL( 0x9004, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 )   /* !< Gets the offset to the start of menu media */

/***********************************************************/
/*! 
 * The following query ID's are used to interact with the interleaver, mainly as test functions
 */
/***********************************************************/
#define DMF_QID_INTERLEAVER_HANDLE                            QIDVAL( 0x8000, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< Handle to the DivXInterleaver */
#define DMF_QID_INTERLEAVE_INTERVAL                           QIDVAL( 0x8001, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32 )  /* !< The number of frames the interleave interval is */
#define DMF_QID_INTERLEAVE_AUD_STREAM_FIXED                   QIDVAL( 0x8002, DMF_QIDT_AUD, DMF_QIDT_INT8 )  /* !< Sets the stream to be fixed block sizes or not */
#define DMF_QID_INTERLEAVE_AUD_INFO                           QIDVAL( 0x8003, DMF_QIDT_AUD, DMF_QIDT_INT8 )
#define DMF_QID_INTERLEAVE_VID_INFO                           QIDVAL( 0x8003, DMF_QIDT_VID, DMF_QIDT_INT8 )
#define DMF_QID_INTERLEAVE_SUB_INFO                           QIDVAL( 0x8003, DMF_QIDT_SUB, DMF_QIDT_INT8 )
/***********************************************************/
/*! 
 * The following query ID's are used for DMFStream
 */
/***********************************************************/
#define DMF_QID_INPUTSTREAM_SET_HTTP_AUTH                       QIDVAL( 0xa000, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Set HTTP Auth struct for DMFInputStream */
#define DMF_QID_INPUTSTREAM_GET_HTTP_AUTH                       QIDVAL( 0xa001, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Get HTTP Auth struct for a DMFInputStream */
#define DMF_QID_STREAM_GET_STREAMTYPE                           QIDVAL( 0xa002, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Get DMFInputStream's type */
#define DMF_QID_STREAM_GET_FILENAME_LENGTH                      QIDVAL( 0xa003, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Get filename length */
#define DMF_QID_STREAM_GET_FILENAME                             QIDVAL( 0xa004, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP )   /* !< Get filename */
#define DMF_QID_STREAM_GET_FILESIZE                             QIDVAL( 0xa005, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )  /* !< Get file size */
#define DMF_QID_STREAM_SET_FILESIZE                             QIDVAL( 0xa006, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )  /* !< Set file size */
#define DMF_QID_STREAM_SET_BUFFER_MEM                           QIDVAL( 0xa007, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Set Mem buffer */
#define DMF_QID_STREAM_GET_BUFFER_MEM                           QIDVAL( 0xa008, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Get Mem beffer */
#define DMF_QID_STREAM_SET_BUFFER_MEM_SIZE                      QIDVAL( 0xa009, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )  /* !< Set Mem buffer size */
#define DMF_QID_STREAM_GET_BUFFER_MEM_SIZE                      QIDVAL( 0xa00A, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )  /* !< Get Mem beffer size */

/***********************************************************/
/*! 
 * The following query ID's are used for Video Codec Private Data
 */
/***********************************************************/
#define DMF_QID_VIDEO_CODEC_PRIVATE_DATA_SIZE           QIDVAL( 0xB000, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the codec private data size for a video track */
#define DMF_QID_VIDEO_CODEC_PRIVATE_DATA                QIDVAL( 0xB001, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieve the codec private data for a video track */

#define DMF_QID_AUDIO_CODEC_PRIVATE_DATA_SIZE           QIDVAL( 0xB002, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the codec private data size for an audio track */
#define DMF_QID_AUDIO_CODEC_PRIVATE_DATA                QIDVAL( 0xB003, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieve the codec private data for an audio track */

#define DMF_QID_SUBTITLE_CODEC_PRIVATE_DATA_SIZE        QIDVAL( 0xB004, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the codec private data size for a subtitle track */
#define DMF_QID_SUBTITLE_CODEC_PRIVATE_DATA             QIDVAL( 0xB005, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieve the codec private data for a subtitle track */



#define DMF_QID_VIDEO_MEDIA_DESCRIPTION              QIDVAL( 0xB500, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the media description for a video track */
#define DMF_QID_AUDIO_MEDIA_DESCRIPTION              QIDVAL( 0xB501, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the media description for a audio track */
#define DMF_QID_SUBTITLE_MEDIA_DESCRIPTION           QIDVAL( 0xB502, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the media description for a subtitle track */

/***********************************************************/
/*!
 * The following query ID's are used to retrieve track vitals opaquely
 */
/***********************************************************/
#define DMF_QID_TRACK_VITALS                            QIDVAL( 0xAE00, DMF_QIDT_VID, DMF_QIDT_UINT8P ) /* !< Retrieve the track vital information */
#define DMF_QID_TRACK_VITALS_SIZE                       QIDVAL( 0xAE01, DMF_QIDT_VID, DMF_QIDT_UINT32 ) /* !< Retrieve the size of the track vitals structure */
/***********************************************************/
/*!
 * The following query ID's are used to set master track
 */
/***********************************************************/
#define DMF_QID_MASTERTRACK                             QIDVAL( 0xE100, DMF_QIDT_VID, DMF_QIDT_UINT8P ) /* !< Set the master track info under trackentry */
#define DMF_QID_ENHANCEDTRACK                           QIDVAL( 0xE000, DMF_QIDT_VID, DMF_QIDT_UINT8P ) /* !< Set the enhanced track info under trackentry */

/***********************************************************/
/*! 
 * The following query ID's are used for Chapter information
 */
/***********************************************************/
#define DMF_QID_CHAPTERS_BEGIN                              QIDVAL( 0x404B, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 )  /* !< Signal the start of a new chapter set, ignore value */
#define DMF_QID_CHAPTERS_CHAPTER_NAME                       QIDVAL( 0x404B, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P ) /* !< Name of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_COUNT                      QIDVAL( 0x404C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT16 ) /* !< Count of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_STARTTIME                  QIDVAL( 0x504C, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< Start time of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_ENDTIME                    QIDVAL( 0x504D, DMF_QIDT_NOSTREAM, DMF_QIDT_INT64 ) /* !< End time of Chapter in Chapters */
#define DMF_QID_EDITION_ORDERED                             QIDVAL( 0x504E, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 ) /* !< 1 if the edition is ordered */
#define DMF_QID_CHAPTERS_CHAPTER_HIDDEN                     QIDVAL( 0x504F, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 ) /* !< End time of Chapter in Chapters */
#define DMF_QID_CHAPTERS_CHAPTER_LANG                       QIDVAL( 0x5050, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 ) 
#define DMF_QID_CHAPTERS_CHAPTER_COUNTRY                    QIDVAL( 0x5051, DMF_QIDT_NOSTREAM, DMF_QIDT_INT8 )
/***********************************************************/
/*! 
 * The following query ID's are used for lace information
 */
/***********************************************************/
#define DMF_QID_LACEINFO                                    QIDVAL( 0x5000, DMF_QIDT_AUD, DMF_QIDT_UINT8P )
#define DMF_QID_DELACE_BLOCKS                               QIDVAL( 0x5001, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Set the read next block to delaced (true/false) */
#define DMF_QID_EDTD                                        QIDVAL( 0x6000, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )

/***********************************************************/
/*! 
 * The following query ID's are used for subtitles
 */
/***********************************************************/

#define DMF_QID_GET_VID_ATTACHMENT_LINK_COUNT               QIDVAL( 0x7000, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_AUD_ATTACHMENT_LINK_COUNT               QIDVAL( 0x7001, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_SUB_ATTACHMENT_LINK_COUNT               QIDVAL( 0x7002, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_VID_ATTACHMENT_LINKS                    QIDVAL( 0x7003, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )
#define DMF_QID_GET_AUD_ATTACHMENT_LINKS                    QIDVAL( 0x7004, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )
#define DMF_QID_GET_SUB_ATTACHMENT_LINKS                    QIDVAL( 0x7005, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )

/***********************************************************/
/*! 
 * The following query ID's are used for file attachments
 */
/***********************************************************/
#define DMF_QID_GET_ATTACHMENT_COUNT                        QIDVAL( 0x7100, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_ATTACHMENT_SIZE                         QIDVAL( 0x7101, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )
#define DMF_QID_GET_ATTACHMENTINFO                          QIDVAL( 0x7102, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_ATTACHMENT                              QIDVAL( 0x7103, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )
#define DMF_QID_SET_SUB_ATTACHMENT_LINK                     QIDVAL( 0x7104, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )
#define DMF_QID_GET_SUB_ATTACHMENT_LINK                     QIDVAL( 0x7105, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT64 )
/***********************************************************/
/*! 
 * The following query ID's are used for Playlist information
 */
/***********************************************************/
#define DMF_QID_PLAYLISTS_GET_COUNT                         QIDVAL( 0x50A0, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Count of playlists in title */
#define DMF_QID_PLAYLISTS_GET_PROP                          QIDVAL( 0x50A1, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Get the properties structure of a playlist */
#define DMF_QID_PLAYLISTS_SET_PLAYLIST_IDX                  QIDVAL( 0x50A2, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Set current playlist */
#define DMF_QID_PLAYLISTS_SET_ALLOW_ORDERED_FLAG            QIDVAL( 0x50A3, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8 )   /* !< Set the allowed ordered playback flag for a title */

/***********************************************************/
/*! 
 * The following query ID's are used for Trick Track / Master Track information
 */
/***********************************************************/
#define DMF_QID_GET_TRICK_TRACK_PROP                        QIDVAL( 0x5511, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Count of playlists in title */
#define DMF_QID_SET_TRICK_TRACK_PROP                        QIDVAL( 0x5512, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Count of playlists in title */

#define DMF_QID_GET_MASTER_TRACK_PROP                       QIDVAL( 0x5513, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Count of playlists in title */
#define DMF_QID_SET_MASTER_TRACK_PROP                       QIDVAL( 0x5514, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Count of playlists in title */


/***********************************************************/
/*! 
 * The following query ID's are used for tag information
 */
/***********************************************************/
#define DMF_QID_TAGS_SET_TAG                                QIDVAL( 0x5501, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Count of playlists in title */
#define DMF_QID_TAGS_SET_TAG_TARGET                         QIDVAL( 0x5502, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Count of playlists in title */
#define DMF_QID_TAGS_SET_SIMPLE_TAG                         QIDVAL( 0x5503, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Count of playlists in title */



/***********************************************************/
/*! 
 * The following query ID's are used for TAG Swinformation
 */
/***********************************************************/
#define DMF_QID_TAGS_SET_FIRST                                  QIDVAL(0x50B0, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32)  /* !< Sets the internal iterator to the beginning */
#define DMF_QID_TAGS_SET_NEXT                                   QIDVAL(0x50B1, DMF_QIDT_NOSTREAM, DMF_QIDT_INT32)  /* !< Sets the next item in the list of tags */
#define DMF_QID_TAGS_GET_INFO                                   QIDVAL(0x50B2, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8)  /* !< Gets the info about the current tag (size, type, etc...) */
#define DMF_QID_TAGS_GET_TAG_TARGET_TRACK                       QIDVAL(0x50B3, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32)  /* !< Gets the tag itself*/
#define DMF_QID_TAGS_GET_TAG_TARGET_EDITION                     QIDVAL(0x50B4, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32)  /* !< Gets the tag itself*/
#define DMF_QID_TAGS_GET_TAG_TARGET_CHAPTER                     QIDVAL(0x50B5, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32)  /* !< Gets the tag itself*/
#define DMF_QID_TAGS_GET_TAG_TARGET_ATTACHMENT                  QIDVAL(0x50B6, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32)  /* !< Gets the tag itself*/
#define DMF_QID_TAGS_GET_TAG                                    QIDVAL(0x50B7, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8)  /* !< Gets the tag itself*/


#define DMF_QID_SET_FILE_NAME                                   QIDVAL(0x50B8, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)  /* !< Sets the filename on a write container.*/

#define DMF_QID_MKV_SET_WRITE_BLOCK_DURATIONS                   QIDVAL(0x50B9, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)  /* !< Flag to write MKV block durations.*/
#define DMF_QID_MKV_SET_WRITE_HIERARCHICAL_INDEX                QIDVAL(0x50BA, DMF_QIDT_NOSTREAM, DMF_QIDT_DSTRP)  /* !< Flag to write MKV hierarchical index.*/
#define DMF_QID_MKV_SET_INTERLACED                              QIDVAL(0x50BB, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8)  /* !< Flag to write MKV flag interlaced true.*/

/***********************************************************/
/*! 
 * The following query ID's are used for AVC bitstream information
 */
/***********************************************************/
#define DMF_QID_SPS_SIZE                                QIDVAL( 0xB300, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the SPS size for AVC bitstream */
#define DMF_QID_SPS_DATA                                QIDVAL( 0xB301, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieve the SPS data for AVC bitstream */

#define DMF_QID_PPS_SIZE                                QIDVAL( 0xB302, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT32 )  /* !< Retrieve the PPS size for AVC bitstream */
#define DMF_QID_PPS_DATA                                QIDVAL( 0xB303, DMF_QIDT_NOSTREAM, DMF_QIDT_UINT8P )  /* !< Retrieve the PPS data for AVC bitstream */


/***********************************************************/
/*! 
 * The following query ID's are used for interlace MKV/AVC clips
 */
/***********************************************************/
#define DMF_QID_MKV_IS_INTERLACED                              QIDVAL(0x5100, DMF_QIDT_VID, DMF_QIDT_UINT8)  /* !< Flag indicates interlaced or not.*/
#define DMF_QID_MKV_COMBINE_FIELDS                             QIDVAL(0x5101, DMF_QIDT_VID, DMF_QIDT_UINT8)  /* !< Flag indicates ReadNextBlock should combine fields into single block.*/

#endif /* _DMFQUERYIDS_H_ */

/**
   @}
 **/
