/*!
    @file
@verbatim
$Id: AVIStructures.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVISTRUCTURES_H_
#define _AVISTRUCTURES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"

typedef struct _AviStructureHeader_
{
    char     cId[4];
    uint32_t size;
    char     cType[4];
} AVIStructureHeader;

typedef AVIStructureHeader RiffHeader;
typedef AVIStructureHeader ListHeader;

typedef struct __AVIHeader__
{
    int32_t dwMicroSecPerFrame;    /* set to 0 ? */
    int32_t dwMaxBytesPerSec;      /* maximum transfer rate */
    int32_t dwPaddingGranularity;  /* pad to multiples of this size, normally 2K */
    int32_t dwFlags;
    int32_t dwTotalFrames;         /* number of frames in first RIFF 'AVI ' chunk */
    int32_t dwInitialFrames;
    int32_t dwStreams;
    int32_t dwSuggestedBufferSize;

    int32_t dwWidth;
    int32_t dwHeight;

    int32_t dwReserved[4];
} AVIHeader;

typedef struct _AVIStreamHeader_
{
    char    cId[4];
    int32_t size;
    char    ccType[4];
    char    ccHandler[4];
    int32_t dwFlags;    /* Contains AVITF_* flags */
    int16_t wPriority;
    int16_t wLanguage;
    int32_t dwInitialFrames;
    int32_t dwScale;
    int32_t dwRate; /* dwRate / dwScale == samples/second */
    int32_t dwStart;
    int32_t dwLength;  /* In units above... */
    int32_t dwSuggestedBufferSize;
    int32_t dwQuality;
    int32_t dwSampleSize;
    int16_t rcFrame_left;
    int16_t rcFrame_top;
    int16_t rcFrame_right;
    int16_t rcFrame_bottom;
} AVIStreamHeader;

typedef struct _AviChunkHeader_
{
    char     cId[4];
    uint32_t size;
} AVIChunkHeader;

typedef struct _AVIStreamFormatVideo_
{
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    int16_t  biPlanes;
    int16_t  biBitCount;
    char     cCompression[4];
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} AVIStreamFormatVideo, AVIStreamFormatSubtitle;

typedef struct _AVIStreamFormatAudio_
{
    int16_t wFormatTag;         /* format type */
    int16_t nChannels;          /* number of channels (i.e. mono, stereo...) */
    int32_t nSamplesPerSec;     /* sample rate */
    int32_t nAvgBytesPerSec;    /* for buffer estimation */
    int16_t nBlockAlign;        /* block size of data */
    int16_t wBitsPerSample;     /* number of bits per sample of mono data */
    int16_t cbSize;             /* the count in bytes of the size of extra information (after cbSize) */
    #define AVI_STRF_MAX_EXTRA 22
    uint8_t extra[AVI_STRF_MAX_EXTRA]; /* any extra information, e.g. mp3 needs this */
} AVIStreamFormatAudio;

typedef struct __AVI1IndexEntry__
{
    char     cId[4];
    uint32_t dwFlags;
    uint32_t dwOffset;
    uint32_t dwSize;
} AVI1IndexEntry;

typedef struct _AVIStreamInfoDRM_
{
    int32_t      version;
    uint32_t     sizeDRMInfo;
    uint8_t     *drmInfo; /* encrpyted drm info. */
    uint64_t     drmOffset;
} AVIStreamInfoDRM;
#define DRM_INFO_CONSTANTSIZE ( sizeof( uint32_t ) + sizeof( int32_t ) )

typedef struct _DivXIdChunkHeader_
{
    char     cId[4];
    uint32_t size;
    uint32_t chunkId;
} DivXIdChunkHeader;

typedef struct MediaSourceType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  Reserved;
    uint64_t RiffOffset;
} MediaSource;

typedef struct MediaTrackType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    uint32_t ObjectID;
    uint32_t MediaSourceID;
    char     TrackID[4];
    int32_t  StartTime;
    int32_t  EndTime;
    int32_t  LanguageCodeLen;
    int16_t  LanguageCode[2];
    int32_t  TranslationLookupID;
    int32_t  TypeLen;
    int16_t  Type[20];
} MediaTrack;

#define MTConstantSize ( sizeof( MediaTrack ) -\
                        ( sizeof( int16_t ) *\
                          20 ) - ( 2 * sizeof( int32_t ) ) - ( sizeof( int16_t ) * 2 ) )

typedef struct DivxMediaManagerType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    uint64_t Offset;
    char     VersionId[3];
    uint32_t MenuVersion;
    int32_t  ObjectID;
    int32_t  StartingMenuID;
    uint32_t DefaultMenuLanguageLen;
    uint16_t cDefLang[16 ];
} DivxMediaManagerChunkHeader;
/* wchar default language string follows */

typedef struct DivxMediaMenuType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  BackgroundVideoID;
    int32_t  BackgroundAudioID;
    int32_t  StartingMenuItemID;
    int32_t  EnterAction;
    int32_t  ExitAction;
    int32_t  MenuTypeLen;
    uint16_t MenuType[32];
    int32_t  MenuNameLen;
    uint16_t MenuName[32];
} DivXMediaMenu;

#define DMMConstantSize ( sizeof( DivXMediaMenu ) -\
                         ( 64 * sizeof( uint16_t ) ) - sizeof( int32_t ) )

typedef struct ButtonMenuType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  OverlayID;
    int32_t  UpAction;
    int32_t  DownAction;
    int32_t  LeftAction;
    int32_t  RightAction;
    int32_t  SelectAction;
    int32_t  ButtonNameLen;
    int16_t  ButtonName[64];
} ButtonMenu;

#define BMENConstantSize ( sizeof( ButtonMenu ) - ( 64 * sizeof( uint16_t ) ) )

typedef struct MenuRectangleType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  Left;
    int32_t  Top;
    int32_t  Width;
    int32_t  Height;
} MenuRectangle;

typedef struct LanguageMenusType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  LanguageCodeLen;
    int16_t  LanguageCode[2];
    int32_t  StartingMenuID;
    int32_t  RootMenuID;
} LanguageMenus;

typedef struct TranslationType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  LanguageCodeLen;
    int16_t  LanguageCode[2];
    int32_t  ValueLen;
} TranslationEntry;

typedef struct _StreamInfoStruct_
{
    uint64_t riffOffset;
    /* uint64_t    indexOffset;
       uint32_t    moviOffset;
       uint32_t    headerOffset; */
    uint8_t  cFourCC[4];
    uint8_t  typeIndex;
    uint8_t  trackIndex;
    uint8_t  trackType;
    uint64_t startFrame;
    uint64_t endFrame;
    uint64_t startTime;
    uint64_t endTime;
    uint32_t lookupId;
} StreamInfo;
#define VIDEO_TRACK 0
#define AUDIO_TRACK 1
#define SUBTITLE_TRACK 2

typedef struct SubtitleSelectActionType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  TitleID;
    char     TrackID[4];
} SubtitleSelectAction;

typedef struct AudioSelectActionType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  TitleID;
    char     TrackID[4];
} AudioSelectAction;

typedef struct PlayFromCurrentOffsetType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  TitleID;
    int32_t  MediaObjectID;
} PlayFromCurrentOffset;

typedef struct MenuTransitionActionType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  MenuID;
    int32_t  ButtonMenuID;
} MenuTransitionAction;

typedef struct ButtonTransitionActionType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  ButtonMenuID;
} ButtonTransitionAction;

typedef struct PlayActionType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  TitleID;
    int32_t  MediaObjectID;
} PlayAction;

typedef struct _MenuInfo_
{
    int32_t  ObjectID;
    int32_t  BackgroundVideoID;
    int32_t  BackgroundAudioID;
    int32_t  StartingMenuItemID;
    int32_t  MenuTypeLen;
    uint16_t MenuType[32];
    int32_t  MenuNameLen;
    uint16_t MenuName[32];
} MenuInfo;

typedef struct TitleType
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  TranslationLookupID;
    int32_t  DefaultAudioLen;
    int16_t  DefaultAudioTrack[4];
    int32_t  DefaultSubtitleLen;
    int16_t  DefaultSubtitleTrack[4];
} TitleInfo;

typedef struct _ChapterInfo_
{
    char     FourCC[4];
    uint32_t size;
    int32_t  ObjectId;
    int32_t  LookupId;
    uint64_t startFrame;
    uint64_t endFrame;
    uint64_t startTime;
    uint64_t endTime;
} ChapterInfo;

typedef struct _ChapterHeader_
{
    char     FourCC[4];
    uint32_t size;
    int32_t  ObjectId;
    int32_t  LookupId;
} ChapterHeader;

typedef struct _ButtonInfo_
{
    char     FourCC[4];
    uint32_t ChunkSize;
    int32_t  ObjectID;
    int32_t  OverlayID;
    int32_t  UpAction;
    int32_t  DownAction;
    int32_t  LeftAction;
    int32_t  RightAction;
    int32_t  SelectAction;
    int32_t  ButtonNameLen;
    int16_t  ButtonName[64];
} ButtonInfo;

#ifdef __cplusplus
}
#endif

#endif /* _AVISTRUCTURES_H_ */
