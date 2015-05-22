/*!
    @file
@verbatim
$Id: MKVTypes.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVTYPES_H_
#define _MKVTYPES_H_

typedef enum {
    TAG_LEVEL_1,
    TAG_LEVEL_2,
    TAG_LEVEL_3,
    TAG_LEVEL_4,
    TAG_LEVEL_3_PLUS,
    TAG_LEVEL_4_PLUS,
    NUM_TAG_LEVEL
} MKV_TAG_LEVEL_e;

typedef enum {
    MKV_TAGS = 0,
    MKV_TAG,
    MKV_TARGETS,
    MKV_TARGET_TYPE_VALUE,
    MKV_TARGET_TYPE,
    MKV_TRACK_UID,
    MKV_EDITION_UID,
    MKV_CHAPTER_UID,
    MKV_ATTACHMENT_UID,
    MKV_SIMPLE_TAG,
    MKV_TAG_NAME,
    MKV_TAG_LANGUAGE,
    MKV_TAG_DEFAULT,
    MKV_TAG_STRING,
    MKV_TAG_BINARY,
    MKV_TAG_TYPES_NUM
} MKV_TAG_TYPE_e;

/*! The Target Types define the target type's referring "area" */
typedef enum {
    MKV_TARGET_TYPE_COLLECTION = 70,
    MKV_TARGET_TYPE_SEASON     = 60,
    MKV_TARGET_TYPE_MOVIE      = 50,
    MKV_TARGET_TYPE_PART       = 40,
    MKV_TARGET_TYPE_CHAPTER    = 30,
    MKV_TARGET_TYPE_SCENE      = 20,
    MKV_TARGET_TYPE_SHOT       = 10,
    MKV_TARGET_TYPE_NUM 
} MKV_TARGET_TYPE_e;

typedef enum
{
    MKV_TRACK_TYPE_UNKNOWN     = 0x00,
    MKV_TRACK_TYPE_VID         = 0x01,
    MKV_TRACK_TYPE_AUD         = 0x02,
    MKV_TRACK_TYPE_COMPLEX     = 0x03,
    MKV_TRACK_TYPE_LOGO        = 0x10,
    MKV_TRACK_TYPE_SUB         = 0x11,
    MKV_TRACK_TYPE_BUTTON      = 0x12,
    MKV_TRACK_TYPE_CONTROL     = 0x20,
    MKV_TRACK_TYPE_NUM
} MKV_TRACK_TYPE_e;

typedef enum
{
    MKV_FLAG_KEYFRAME        = 0x80,
    MKV_FLAG_LACETYPE_NONE   = 0x00,
    MKV_FLAG_LACETYPE_XIPH   = 0x02,
    MKV_FLAG_LACETYPE_EBML   = 0x06,
    MKV_FLAG_LACETYPE_FIXED  = 0x04,
    MKV_FLAG_INVISIBLE       = 0x08,
    MKV_FLAG_DISCARDABLE     = 0x01,
    MKV_FLAGS_NUM
} MKV_FLAGS_e;

typedef enum
{
    MKV_MEDIA_DESCRPTION_UNSPECIFIED        = 0x00,
    MKV_MEDIA_DESCRPTION_MAINTRACK          = 0x01,
    MKV_MEDIA_DESCRPTION_UNPROTECTED        = 0x02,
    MKV_MEDIA_DESCRPTION_NUM
} MKV_MEDIA_DESCRPTION_e;

#define AVI_INDEX_FLAG_KEYFRAME 0x00000010L
#define DIVX_TIME_UNITS_TO_NANO 100
#define SECONDS_TO_NANO 1000000000

#endif //MKVTYPES
