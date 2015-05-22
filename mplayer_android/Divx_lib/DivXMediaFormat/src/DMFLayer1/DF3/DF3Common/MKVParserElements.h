/*!
    @file
@verbatim
$Id: MKVParserElements.h 59996 2009-05-06 19:54:33Z ashivadas $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVPARSERELEMENTS_H_
#define _MKVPARSERELEMENTS_H_

#include "DivXString.h"
#include "DF3/DF3Read/EBMLMap.h"

typedef enum {
    MKVELEMENT_INVALID_ID                     = 0,
    MKVELEMENT_EBML_ID                        = 0x1A45DFA3,
    MKVELEMENT_EBMLVERSION_ID                 = 0x4286,
    MKVELEMENT_EBMLREADVERSION_ID             = 0x42F7,
    MKVELEMENT_EBMLMAXIDLEN_ID                = 0x42F2,
    MKVELEMENT_EBMLMAXSIZELEN_ID              = 0x42F3,
    MKVELEMENT_DOCTYPE_ID                     = 0x4282,
    MKVELEMENT_DOCTYPEVERSION_ID              = 0x4287,
    MKVELEMENT_DOCTYPEREADVERSION_ID          = 0x4285,
    MKVELEMENT_SEGMENT_ID                     = 0x18538067,
    MKVELEMENT_SEGMENTUID_ID                  = 0x73A4,
    MKVELEMENT_SEGMENT_FILENAME_ID            = 0x7384,
    MKVELEMENT_PREVUID_ID                     = 0x3CB923,
    MKVELEMENT_PREVFILENAME_ID                = 0x3C83AB,
    MKVELEMENT_NEXTUID_ID                     = 0x3EB923,
    MKVELEMENT_NEXTFILENAME_ID                = 0x3E83BB,
    MKVELEMENT_TIMECODESCALE_ID               = 0x2AD7B1,
    MKVELEMENT_DURATION_ID                    = 0x4489,
    MKVELEMENT_TITLE_ID                       = 0x7BA9,
    MKVELEMENT_MUXINGAPP_ID                   = 0x4D80,
    MKVELEMENT_WRITINGAPP_ID                  = 0x5741,
    MKVELEMENT_DATEUTC_ID                     = 0x4461,
    MKVELEMENT_SEGMENTINFO_ID                 = 0x1549A966,
    MKVELEMENT_CRC_ID                         = 0xBF,
    MKVELEMENT_VOID_ID                        = 0xEC,
    MKVELEMENT_TRACKS_ID                      = 0x1654AE6B,
    MKVELEMENT_TRACKENTRY_ID                  = 0xAE,
    MKVELEMENT_TRACKNUMBER_ID                 = 0xD7,
    MKVELEMENT_TRACKTYPE_ID                   = 0x83,
    MKVELEMENT_FLAGENABLED_ID                 = 0xB9,
    MKVELEMENT_FLAGDEFAULT_ID                 = 0x88,
    MKVELEMENT_FLAGFORCED_ID                  = 0x55AA,
    MKVELEMENT_FLAGLACING_ID                  = 0x9C,
    MKVELEMENT_MINCACHE_ID                    = 0x6DE7,
    MKVELEMENT_MAXCACHE_ID                    = 0x6DF8,
    MKVELEMENT_DEFAULTDURATION_ID             = 0x23E383,
    MKVELEMENT_TRACKTIMECODESCALE_ID          = 0x23314F,
    MKVELEMENT_NAME_ID                        = 0x536E,
    MKVELEMENT_LANGUAGE_ID                    = 0x22B59C,
    MKVELEMENT_CODECID_ID                     = 0x86,
    MKVELEMENT_MAXBLOCKADDITIONID_ID          = 0x55EE,
    MKVELEMENT_MEDIADESCRIPTION_ID            = 0xEA,
    MKVELEMENT_CODECDECODEALL_ID              = 0x2C,
    MKVELEMENT_CODECPRIVATE_ID                = 0x63A2,
    MKVELEMENT_CODECNAME_ID                   = 0x258688,
    MKVELEMENT_ATTACHMENTLINK_ID              = 0x7446,
    MKVELEMENT_VIDEO_ID                       = 0xE0,
    MKVELEMENT_AUDIO_ID                       = 0xE1,
    MKVELEMENT_SUBTITLE_ID                    = 0xE2,
    MKVELEMENT_CONTENTENCODINGS_ID            = 0x6D80,
    MKVELEMENT_TRACKUID_ID                    = 0x73C5,
    MKVELEMENT_PIXELWIDTH_ID                  = 0xB0,
    MKVELEMENT_PIXELHEIGHT_ID                 = 0xBA,
    MKVELEMENT_FLAGINTERLACED_ID              = 0x9A,
    MKVELEMENT_PIXELCROPBOTTOM_ID             = 0x54AA,
    MKVELEMENT_PIXELCROPTOP_ID                = 0x54BB,
    MKVELEMENT_PIXELCROPLEFT_ID               = 0x54CC,
    MKVELEMENT_PIXELCROPRIGHT_ID              = 0x54DD,
    MKVELEMENT_DISPLAYWIDTH_ID                = 0x54B0,
    MKVELEMENT_DISPLAYHEIGHT_ID               = 0x54BA,
    MKVELEMENT_DISPLAYUNIT_ID                 = 0x54B2,
    MKVELEMENT_FILELACING_ID                  = 0x9C,
    MKVELEMENT_SAMPLING_FREQUENCY_ID          = 0xB5,
    MKVELEMENT_OUTPUTSAMPLINGFREQUENCY_ID     = 0x78B5,
    MKVELEMENT_CHANNELS_ID                    = 0x9F,
    MKVELEMENT_BITDEPTH_ID                    = 0x6264,
    MKVELEMENT_CONTENTCOMPRESSION_ID          = 0x5034,
    MKVELEMENT_CONTENTCOMPALGO_ID             = 0x4254,
    MKVELEMENT_CONTENTCOMPSETTINGS_ID         = 0x4255,
    MKVELEMENT_CLUSTER_ID                     = 0x1F43B675,
    MKVELEMENT_BLOCKGROUP_ID                  = 0xA0,
    MKVELEMENT_BLOCK_ID                       = 0xA1,
    MKVELEMENT_REFERENCEBLOCK_ID              = 0xFB,
    MKVELEMENT_BLOCKDURATION_ID               = 0X9B,
    MKVELEMENT_SIMPLEBLOCK_ID                 = 0xA3,
    MKVELEMENT_TIMECODE_ID                    = 0xE7,
    MKVELEMENT_POSITION_ID                    = 0xA7,
    MKVELEMENT_PREVSIZE_ID                    = 0xAB,
    MKVELEMENT_CUES_ID                        = 0x1C53BB6B,
    MKVELEMENT_CUEPOINT_ID                    = 0xBB,
    MKVELEMENT_CUETIME_ID                     = 0xB3,
    MKVELEMENT_CUETRACKPOSITIONS_ID           = 0xB7,
    MKVELEMENT_CUECLUSTERPOSITION_ID          = 0xF1,
    MKVELEMENT_CUETRACK_ID                    = 0xF7,
    MKVELEMENT_CUEBLOCKNUMBER_ID              = 0x5378,
    MKVELEMENT_EDITIONENTRY_ID                = 0x45B9,
    MKVELEMENT_EDITIONUID2_ID                 = 0x45BC,
    MKVELEMENT_EDITIONFLAGHIDDEN_ID           = 0x45BD,
    MKVELEMENT_EDITIONFLAGDEFAULT_ID          = 0x45DB,
    MKVELEMENT_EDITIONFLAGORDERED_ID          = 0x45DD,
    MKVELEMENT_CHAPTERATOM_ID                 = 0xB6,
    MKVELEMENT_CHAPTERUID2_ID                 = 0x73C4,
    MKVELEMENT_CHAPTERTIMESTART_ID            = 0x91,
    MKVELEMENT_CHAPTERTIMEEND_ID              = 0x92,
    MKVELEMENT_CHAPTERFLAGHIDDEN_ID           = 0x98,
    MKVELEMENT_CHAPTERFLAGENABLED_ID          = 0x4598,
    MKVELEMENT_CHAPTERSEGMENTUID_ID           = 0x6E67,
    MKVELEMENT_CHAPTERSEGMENTEDITIONUID_ID    = 0x6EBC,
    MKVELEMENT_CHAPTERS_ID                    = 0x1043A770,
    MKVELEMENT_CHAPTERTRACKS_ID               = 0x8F,
    MKVELEMENT_CHAPTERDISPLAY_ID              = 0x80,
    MKVELEMENT_CHAPTERTRACKNUMBER_ID          = 0x89,
    MKVELEMENT_CHAPSTRING_ID                  = 0x85,
    MKVELEMENT_CHAPLANGUAGE_ID                = 0x437C,
    MKVELEMENT_CHAPCOUNTRY_ID                 = 0x437E,
    MKVELEMENT_ATTACHMENTS_ID                 = 0x1941A469,
    MKVELEMENT_ATTACHEDFILE_ID                = 0x61A7,
    MKVELEMENT_FILEDESCRIPTION_ID             = 0x467E,
    MKVELEMENT_FILENAME_ID                    = 0x466E,
    MKVELEMENT_FILEMIMETYPE_ID                = 0x4660,
    MKVELEMENT_FILEDATA_ID                    = 0x465C,
    MKVELEMENT_FILEUID_ID                     = 0x46AE,
    MKVELEMENT_TAGS_ID                        = 0x1254C367,
    MKVELEMENT_TAG_ID                         = 0x7373,
    MKVELEMENT_SIMPLETAG_ID                   = 0x67C8,
    MKVELEMENT_TARGETS_ID                     = 0x63C0,
    MKVELEMENT_TARGETTYPE_ID                  = 0x63CA,
    MKVELEMENT_TARGETTYPEVALUE_ID             = 0x68CA,
    MKVELEMENT_TAGSTRING_ID                   = 0x4487,
    MKVELEMENT_TAG_TITLE_ID                   = 0x4488,
    MKVELEMENT_TAG_COPYRIGHT_ID               = 0x4489,
    MKVELEMENT_TAG_DESCRIPTION_ID             = 0x4490,
    MKVELEMENT_TAG_URL_ID                     = 0x4491,
    MKVELEMENT_TAG_JPEG_ID                    = 0x4492,
    MKVELEMENT_TAG_DISPLAY_SETTINGS_ID        = 0x4493,
    MKVELEMENT_TAG_DISPLAY_ORIGIN_ID          = 0x4494,
    MKVELEMENT_TAG_DIVX_UID_ID                = 0x4495,
    MKVELEMENT_TAG_GLOBAL_REFERENCE_ID        = 0x4496,
    MKVELEMENT_TAG_RECORDER_SETTINGS_ID       = 0x4497,
    MKVELEMENT_TAG_SIGNIFICANCE_ID            = 0x4498,
    MKVELEMENT_TAG_VIEWED_SEGMENTS_ID         = 0x4499,
    MKVELEMENT_TAGNAME_ID                     = 0x45A3,
    MKVELEMENT_TAGLANGUAGE_ID                 = 0x447A,
    MKVELEMENT_TAGORIGINAL_ID                 = 0x4484,
    MKVELEMENT_TAGBINARYVALUE_ID              = 0x4485,
    MKVELEMENT_TAGTRACKUID_ID                 = 0x63C5,
    MKVELEMENT_EDITIONUID_ID                  = 0x63C9,
    MKVELEMENT_CHAPTERUID_ID                  = 0x63C4,
    MKVELEMENT_ATTACHMENTUID_ID               = 0x63C6,
    MKVELEMENT_SEEKHEAD_ID                    = 0x114D9B74,
    MKVELEMENT_SEEK_ID                        = 0x4DBB,
    MKVELEMENT_SEEKPOSITION_ID                = 0x53AC,
    MKVELEMENT_SEEKID_ID                      = 0x53AB,
    MKVELEMENT_SEEKI2_ID                      = 0x53AB,
    MKVELEMENT_ATTACHMENT_ID                  = 0x1941A469,
    MKVELEMENT_TRACKSDATA_ID                  = 0xDA,
    MKVELEMENT_TRACKSDATA_VER_ID              = 0xDB,
    MKVELEMENT_TRACKSDATA_PSIZE_ID            = 0xDC,
    MKVELEMENT_DRMINFO_ID                     = 0xDD,
    MKVELEMENT_TRACKSDATA_PAYLOAD_ID          = 0xDE,
    MKVELEMENT_CONTENTENCODING_ID             = 0x6240,
    MKVELEMENT_CONTENTENCODINGORDER_ID        = 0x5031,
    MKVELEMENT_CONTENTENCODINGSCOPE_ID        = 0x5032,
    MKVELEMENT_CONTENTENCODINGCOMPRESSION_ID  = 0x5034,
    MKVELEMENT_CONTENTENCODINGENCRYPTION_ID   = 0x5035,
    MKVELEMENT_ENHANCEDTRACK_UID_ID           = 0xC0,
    MKVELEMENT_ENHANCEDTRACK_SEGUID_ID        = 0xC1,
    MKVELEMENT_ENHANCEDTRACK_FILENAME_ID      = 0xC2,
    MKVELEMENT_MASTERTRACK_SEGUID_ID          = 0xC4,
    MKVELEMENT_MASTERTRACK_FILENAME_ID        = 0xC5,
    MKVELEMENT_ENHANCEDTRACK_FLAG_ID          = 0xC6,
    MKVELEMENT_MASTERTRACK_UID_ID             = 0xC7,
    MKVELEMENT_PREVIOUS_REF_FRAME_ID          = 0xC8,
    MKVELEMENT_PREVIOUS_REF_FRAME_OFFSET      = 0xC9,
    MKVELEMENT_PREVIOUS_REF_FRAME_TIMECODE    = 0xCA,
    MKVELEMENT_NEXT_REF_FRAME_ID              = 0xCB,
    MKVELEMENT_NEXT_REF_FRAME_OFFSET          = 0xCC,
    MKVELEMENT_NEXT_REF_FRAME_TIMECODE        = 0xCD,

    MKVELEMENT_HIERARCHICAL_INDEX_ID              = 0x2AD1D3,
    MKVELEMENT_HIERARCHICAL_INDEX_POINT_ID        = 0x30A1D9,
    MKVELEMENT_HIERARCHICAL_INDEX_TIME_ID         = 0x4A10,
    MKVELEMENT_HIERARCHICAL_INDEX_POSITIONS_ID    = 0x4A11,
    MKVELEMENT_HIERARCHICAL_INDEX_TRACK_ID        = 0x4A12,
    MKVELEMENT_HIERARCHICAL_INDEX_CUEPOS_ID       = 0x4A13,


    // The following are not EBML Element ID's, used by the write side API
    MKVELEMENT_FIXUPS_ID                = 0x0C0B1213, //mlst -- multichildlist
    DF3WRITE_INSTACE_DATA_ID            = 0xFFFFFFFF,
    DF3WRITE_WRITECACHE_INST_ID         = 0xFFFFFFFE

}MKVELEMENT_ID_e;

typedef enum _MKVFORMAT_TAG_e_
{
    MKV_FORMAT_TAG_MP3 = 0x0055,
    MKV_FORMAT_TAG_MP2 = 0x0050,
    MKV_FORMAT_TAG_AC3 = 0x2000,
    MKV_FORMAT_TAG_AAC = 0x00FF
}MKVFORMAT_TAG_e;


/*! MKV Map Element definition */
typedef struct
{
    const MKVELEMENT_ID_e id;
    const EBMLDataType_e dataType;
    const DivXString *name;
} MKVElement_t;

extern const MKVElement_t MKVELEMENT_UNKNOWN;

// EBML
extern const MKVElement_t MKVELEMENT_EBML;
extern const MKVElement_t MKVELEMENT_DOCTYPE;
extern const MKVElement_t MKVELEMENT_DOCTYPEVERSION;
extern const MKVElement_t MKVELEMENT_DOCTYPEREADVERSION;

// SEGMENT and miscellaneous
extern const MKVElement_t MKVELEMENT_SEGMENT;
extern const MKVElement_t MKVELEMENT_CRC;
extern const MKVElement_t MKVELEMENT_VOID;

// SEGMENTINFO
extern const MKVElement_t MKVELEMENT_TITLE;
extern const MKVElement_t MKVELEMENT_SEGMENTINFO;
extern const MKVElement_t MKVELEMENT_TIMECODESCALE;
extern const MKVElement_t MKVELEMENT_DURATION;
extern const MKVElement_t MKVELEMENT_SEGMENTUID;
extern const MKVElement_t MKVELEMENT_PREVUID;
extern const MKVElement_t MKVELEMENT_NEXTUID;
extern const MKVElement_t MKVELEMENT_PREVFILENAME;
extern const MKVElement_t MKVELEMENT_NEXTFILENAME;
extern const MKVElement_t MKVELEMENT_MUXINGAPP;
extern const MKVElement_t MKVELEMENT_WRITINGAPP;
extern const MKVElement_t MKVELEMENT_SEGMENT_FILENAME;

// TRACKS
extern const MKVElement_t MKVELEMENT_TRACKS;
extern const MKVElement_t MKVELEMENT_NAME;
extern const MKVElement_t MKVELEMENT_LANGUAGE;
extern const MKVElement_t MKVELEMENT_TRACKENTRY;
extern const MKVElement_t MKVELEMENT_TRACKSDATA;
extern const MKVElement_t MKVELEMENT_TRACKSDATAVER;
extern const MKVElement_t MKVELEMENT_TRACKSDATASIZE;
extern const MKVElement_t MKVELEMENT_TRACKSDATAPAYLOAD;
extern const MKVElement_t MKVELEMENT_TRACKNUMBER;
extern const MKVElement_t MKVELEMENT_TRACKTYPE;
extern const MKVElement_t MKVELEMENT_TRACKTIMECODESCALE;
extern const MKVElement_t MKVELEMENT_TRACKUID;
extern const MKVElement_t MKVELEMENT_ATTACHMENTLINK;
extern const MKVElement_t MKVELEMENT_DEFAULTDURATION;
extern const MKVElement_t MKVELEMENT_VIDEO;
extern const MKVElement_t MKVELEMENT_AUDIO;
extern const MKVElement_t MKVELEMENT_FILELACING;
extern const MKVElement_t MKVELEMENT_CODECPRIVATE;
extern const MKVElement_t MKVELEMENT_CODECID;
extern const MKVElement_t MKVELEMENT_CODECNAME;
extern const MKVElement_t MKVELEMENT_MEDIADESCRIPTION;

// VIDEO TRACK
extern const MKVElement_t MKVELEMENT_PIXELWIDTH;
extern const MKVElement_t MKVELEMENT_PIXELHEIGHT;
extern const MKVElement_t MKVELEMENT_DISPLAYWIDTH;
extern const MKVElement_t MKVELEMENT_DISPLAYHEIGHT;
extern const MKVElement_t MKVELEMENT_FLAGINTERLACED;

// AUDIO TRACK
extern const MKVElement_t MKVELEMENT_CHANNELS;
extern const MKVElement_t MKVELEMENT_SAMPLING_FREQUENCY;
extern const MKVElement_t MKVELEMENT_OUTPUTSAMPLINGFREQUENCY;
extern const MKVElement_t MKVELEMENT_BITDEPTH;

// CLUSTER
extern const MKVElement_t MKVELEMENT_CLUSTER;
extern const MKVElement_t MKVELEMENT_BLOCKGROUP;
extern const MKVElement_t MKVELEMENT_BLOCK;
extern const MKVElement_t MKVELEMENT_SIMPLEBLOCK;
extern const MKVElement_t MKVELEMENT_TIMECODE;
extern const MKVElement_t MKVELEMENT_REFERENCEBLOCK;
extern const MKVElement_t MKVELEMENT_POSITION;

// CUES
extern const MKVElement_t MKVELEMENT_CUES;
extern const MKVElement_t MKVELEMENT_CUEPOINT;
extern const MKVElement_t MKVELEMENT_CUETIME;
extern const MKVElement_t MKVELEMENT_CUETRACKPOSITIONS;
extern const MKVElement_t MKVELEMENT_CUECLUSTERPOSITION;
extern const MKVElement_t MKVELEMENT_CUETRACK;
extern const MKVElement_t MKVELEMENT_CUEBLOCKNUMBER;

// ATTACHMENTS
extern const MKVElement_t MKVELEMENT_ATTACHMENTS;
extern const MKVElement_t MKVELEMENT_ATTACHEDFILE;
extern const MKVElement_t MKVELEMENT_FILEDESCRIPTION;
extern const MKVElement_t MKVELEMENT_FILENAME;
extern const MKVElement_t MKVELEMENT_FILEMIMETYPE;
extern const MKVElement_t MKVELEMENT_FILEDATA;
extern const MKVElement_t MKVELEMENT_FILEUID;

// TAGS
extern const MKVElement_t MKVELEMENT_TAGS;
extern const MKVElement_t MKVELEMENT_TAG;
extern const MKVElement_t MKVELEMENT_SIMPLETAG;
extern const MKVElement_t MKVELEMENT_TARGETS;
extern const MKVElement_t MKVELEMENT_TARGETTYPE;
extern const MKVElement_t MKVELEMENT_TARGETTYPEVALUE;
extern const MKVElement_t MKVELEMENT_TAGSTRING;
extern const MKVElement_t MKVELEMENT_TAGNAME;
extern const MKVElement_t MKVELEMENT_TAGLANGUAGE;
extern const MKVElement_t MKVELEMENT_TAGORIGINAL;
extern const MKVElement_t MKVELEMENT_TAGBINARY_VALUE;
extern const MKVElement_t MKVELEMENT_TAGTRACKUID;
extern const MKVElement_t MKVELEMENT_EDITIONUID;
extern const MKVElement_t MKVELEMENT_CHAPTERUID;
extern const MKVElement_t MKVELEMENT_ATTACHMENTUID;
extern const MKVElement_t MKVELEMENT_TAGTITLE;

// SEEKHEAD
extern const MKVElement_t MKVELEMENT_SEEKHEAD;
extern const MKVElement_t MKVELEMENT_SEEK;
extern const MKVElement_t MKVELEMENT_SEEKPOSITION;
extern const MKVElement_t MKVELEMENT_SEEKID;

// CHAPTERS
extern const MKVElement_t MKVELEMENT_CHAPTERS;
extern const MKVElement_t MKVELEMENT_EDITIONENTRY;
extern const MKVElement_t MKVELEMENT_EDITIONUID2;
extern const MKVElement_t MKVELEMENT_EDITIONFLAGHIDDEN;
extern const MKVElement_t MKVELEMENT_EDITIONFLAGDEFAULT;
extern const MKVElement_t MKVELEMENT_EDITIONFLAGORDERED;
extern const MKVElement_t MKVELEMENT_CHAPTERATOM;
extern const MKVElement_t MKVELEMENT_CHAPTERUID2;
extern const MKVElement_t MKVELEMENT_CHAPTERTIMESTART;
extern const MKVElement_t MKVELEMENT_CHAPTERTIMEEND;
extern const MKVElement_t MKVELEMENT_CHAPTERFLAGHIDDEN;
extern const MKVElement_t MKVELEMENT_CHAPTERFLAGENABLED;
extern const MKVElement_t MKVELEMENT_CHAPTERSEGMENTUID;
extern const MKVElement_t MKVELEMENT_CHAPTERSEGMENTEDITIONUID;
extern const MKVElement_t MKVELEMENT_CHAPTERTRACKS;
extern const MKVElement_t MKVELEMENT_CHAPTER_DISPLAY;
extern const MKVElement_t MKVELEMENT_CHAPTERTRACKNUMBER;
extern const MKVElement_t MKVELEMENT_CHAPSTRING;
extern const MKVElement_t MKVELEMENT_CHAPLANGUAGE;
extern const MKVElement_t MKVELEMENT_CHAPCOUNTRY;

// ENHANCED VISUAL SEARCH
extern const MKVElement_t MKVELEMENT_ENHANCEDTRACK_UID;
extern const MKVElement_t MKVELEMENT_ENHANCEDTRACK_SEGUID;
extern const MKVElement_t MKVELEMENT_ENHANCEDTRACK_FILENAME;
extern const MKVElement_t MKVELEMENT_MASTERTRACK_SEGUID;
extern const MKVElement_t MKVELEMENT_MASTERTRACK_FILENAME;
extern const MKVElement_t MKVELEMENT_MASTERTRACK_UID;

// HIERARCHICAL INDEX

extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX;
extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX_POINT;
extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX_TIME;
extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX_POSITIONS;
extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX_TRACK;
extern const MKVElement_t MKVELEMENT_HIERARCHICAL_INDEX_CUEPOS;



/*! Video private track data */
typedef struct _MKVVideoPrivateData_t
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
}MKVVideoPrivateData_t;

/*! Audio private track data */
typedef struct _MKVAudioPrivateData_t
{
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
}MKVAudioPrivateData_t;

/*! Profile IDC definitions for AVC */
typedef enum
{
    PROFILE_IDC_BASELINE = 66,
    PROFILE_IDC_MAIN     = 77,
    PROFILE_IDC_EXTENDED = 88,
    PROFILE_IDC_HIGH     = 100,
    PROFILE_IDC_HIGH10   = 110,
    PROFILE_IDC_HIGH422  = 122,
    PROFILE_IDC_HIGH444  = 144
} PROFILE_IDC_e;

/*! Level IDC definitions for AVC */
typedef enum
{
    LEVEL_IDC_1   = 1,
    LEVEL_IDC_1_1 = 11,
    LEVEL_IDC_1_2 = 12,
    LEVEL_IDC_1_3 = 13,
    LEVEL_IDC_2   = 2,
    LEVEL_IDC_2_1 = 21,
    LEVEL_IDC_2_2 = 22,
    LEVEL_IDC_3   = 3,
    LEVEL_IDC_3_1 = 31,
    LEVEL_IDC_3_2 = 32,
    LEVEL_IDC_4   = 4,
    LEVEL_IDC_4_1 = 41,
    LEVEL_IDC_4_2 = 42,
    LEVEL_IDC_5   = 5,
    LEVEL_IDC_5_1 = 51
} LEVEL_IDC_e;

/*! Video H264 private track data V_MPEG4/ISO/AVC */
typedef struct _MKVVideoH264PrivateData_t
{
    uint8_t  reserved1;              //  8 bits Reserved Field
    uint8_t  profile_idc;            //  8 bits Defines the profile of the bitstream.  See Table #.
    uint8_t  reserved2;              //  8 bits Reserved Field
    uint8_t  level_idc;              //  8 bits Defines the level of the bitstream. See Table #.
    uint8_t  reserved3;              //  6 bits Reserved Field
    uint8_t  size_nalu_minus_one;    //  2 bits Defines the NAL Unit Length - 1
    uint8_t  reserved4[3];           //  3 bits Reserved Field
    uint8_t  num_seq_parameters;     //  5 bits Num Sequence Parameter Sets
    uint8_t* seq_parameter_set;      //  variable Sequence Parameter Sets
    uint8_t  num_pic_parameters;     //  8   Num Picture Parameter Sets
    uint8_t* pic_parameter_set;      //  variable Picture Parameter Sets
}MKVVideoH264PrivateData_t;

/*! Codec table definitions AUD/VID/SUB */
typedef enum
{
    MKV_CODEC_TYPE_UNKNOWN = 0,
    MKV_CODEC_VID_MS_VCM = 1,
    MKV_CODEC_VID_UNCOMPRESSED,
    MKV_CODEC_VID_MPEG4_ISO_SP,  /*! Existing DivX Codec */
    MKV_CODEC_VID_MPEG4_ISO_ASP,
    MKV_CODEC_VID_MPEG4_ISO_AP,
    MKV_CODEC_VID_MPEG4_ISO_AVC, /*! Support for H.264 */
    MKV_CODEC_VID_MPEG4_V3,
    MKV_CODEC_VID_MPEG1,
    MKV_CODEC_VID_MPEG2,
    MKV_CODEC_VID_REAL_RV10,
    MKV_CODEC_VID_REAL_RV20,
    MKV_CODEC_VID_REAL_RV30,
    MKV_CODEC_VID_REAL_RV40,
    MKV_CODEC_VID_QUICKTIME,
    MKV_CODEC_VID_THEORA,

    MKV_CODEC_AUD_MPEG_L3,
    MKV_CODEC_AUD_MPEG_L2,
    MKV_CODEC_AUD_MPEG_L1,
    MKV_CODEC_AUD_PCM_INT_BE,
    MKV_CODEC_AUD_PCM_INT_LE,
    MKV_CODEC_AUD_PCM_FLOAT,
   	MKV_CODEC_AUD_MPC,
    MKV_CODEC_AUD_AC3,
    MKV_CODEC_AUD_AC3_BSID9,
    MKV_CODEC_AUD_AC3_BSID10,
    MKV_CODEC_AUD_DTS,
    MKV_CODEC_AUD_VORBIS,
    MKV_CODEC_AUD_FLAC,
    MKV_CODEC_AUD_REAL_14_4,
    MKV_CODEC_AUD_REAL_28_8,
    MKV_CODEC_AUD_REAL_COOK,
    MKV_CODEC_AUD_REAL_SIPR,
    MKV_CODEC_AUD_REAL_RALF,
    MKV_CODEC_AUD_REAL_ATRC,
    MKV_CODEC_AUD_MS_ACM,
    MKV_CODEC_AUD_AAC,
    MKV_CODEC_AUD_AAC_MPEG2_MAIN,
    MKV_CODEC_AUD_AAC_MPEG2_LC,
    MKV_CODEC_AUD_AAC_MPEG2_LC_SBR,
    MKV_CODEC_AUD_AAC_MPEG2_SSR,
    MKV_CODEC_AUD_AAC_MPEG4_MAIN,
    MKV_CODEC_AUD_AAC_MPEG4_LC,
    MKV_CODEC_AUD_AAC_MPEG4_LC_SBR,
    MKV_CODEC_AUD_AAC_MPEG4_SSR,
    MKV_CODEC_AUD_AAC_MPEG4_LTP,
    MKV_CODEC_AUD_QUICKTIME_QDMC,
    MKV_CODEC_AUD_QUICKTIME_QDM2,
    MKV_CODEC_AUD_TTA,
    MKV_CODEC_AUD_WAVPACK,

	MKV_CODEC_SUB_UTF8,
	MKV_CODEC_SUB_SSA,
	MKV_CODEC_SUB_ASS,
	MKV_CODEC_SUB_USF,
	MKV_CODEC_SUB_BMP,
	MKV_CODEC_SUB_VOBSUB,

	MKV_CODEC_BTN_VOBBTN,

    NUM_MKV_CODEC_TYPE
} MKV_CODEC_TYPE_e;

/*! MKV Codec type definition */
typedef struct
{
    const MKV_CODEC_TYPE_e codecType;	// codec ID enum
	const uint16_t formatTag;			// waveinfo formatTag field, audio only
    const DivXBool supported;           // Is this codec type supported
    const DivXString *name;				// MKV codec name
} MKVCodecType_t;

extern const char H264_CODEC_FOURCC[];

extern const MKVCodecType_t MKVCodecTypeMap[NUM_MKV_CODEC_TYPE];

const DivXString* MKVElementName(MKVELEMENT_ID_e id);

#define DEFAULT_CHANNELS 1
#define DEFAULT_STAMPLE_RATE 8000

#endif
/* _MKVPARSERELEMENTS_H_ */
