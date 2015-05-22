/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

/** @defgroup DMFERROS DMF API Error Codes
 *  Description of DivX Media Format API (DMF)Error Codes
 *  @{
 */

#ifndef _DMFERRORS_H_
#define _DMFERRORS_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXString.h"

#ifdef __cplusplus
extern "C" {
#endif

/* macro to apply a mask to an enum */
#define MASK_ENUM(x, num, mask)    \
    x##_ENUM_IDX,            \
    x = num | mask, \
    x##_ENUM_IDX_AGAIN = x##_ENUM_IDX

/** marco to apply error mask */
#define DMF_ERR_MASK(x,num) MASK_ENUM(x,num,DMF_ERR_MASK_VALUE)

/** marco to apply warning mask */
#define DMF_WARN_MASK(x,num) MASK_ENUM(x,num,DMF_WARN_MASK_VALUE)

typedef struct DMFErrorDesc_t {
    int32_t           errorCode;      /* error number */
    const DivXString *errorString;    /* error string representation*/
    const DivXString *errorDesc;      /* error description */
}DMFErrorDesc_t;

typedef enum _DMFErrors
{
    DMF_WARN_MASK( 
        DMF_WARN_RENDER_RESET, 1
        ),
    DMF_WARN_MASK( 
        DMF_ERR_NO_INDEX_FOUND, 2
        ),
    DMF_WARN_MASK( 
        DMF_ERR_INVALID_MENU_VERSION, 3
        ),
    DMF_WARN_MASK( 
        DMF_WARN_NONFATAL, 4
        ),
    DMF_ERR_MASK( 
        DMF_ERR_UNEXPECTED_ERROR, 5
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_HANDLE, 6
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_STREAM_HANDLE, 7
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_STREAM, 8
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BLOCK_READ_FAILURE, 9
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BLOCK_BUFFER_TOO_SMALL, 10
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SEEK_FAILURE, 11
        ),
    DMF_ERR_MASK( 
        DMF_ERR_CREATE_INSTANCE, 12
        ),
    DMF_ERR_MASK( 
        DMF_ERR_CREATE_READ_BUFFER,13
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_FILE, 14
        ),
    DMF_ERR_MASK( 
        DMF_ERR_HDRL_NOT_FOUND, 15
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DUPLICATE_STRH_FOUND, 16
        ),
    DMF_ERR_MASK( 
        DMF_ERR_TOO_MANY_VID_STREAMS, 17
        ),
    DMF_ERR_MASK( 
        DMF_ERR_TOO_MANY_AUD_STREAMS, 18
        ),
    DMF_ERR_MASK( 
        DMF_ERR_TOO_MANY_SUB_STREAMS, 19
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BAD_STRUCTURE, 20
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INDEX_OUT_OF_RANGE, 21
        ),
    DMF_ERR_MASK( 
        DMF_ERR_GET_BLOCK_FAILED, 22
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SET_BLOCK_FAILED, 23
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SET_BY_TIME_FAILED, 24
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SET_BY_BLOCK_FAILED, 25
        ),
    DMF_ERR_MASK( 
        DMF_ERR_END_OF_STREAM, 26
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SET_TITLE_FAILED, 27
        ),
    DMF_ERR_MASK( 
        DMF_ERR_PROCESS_INDEX_FAILED, 28
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_BLOCK_AND_TIME, 29
        ),
    DMF_ERR_MASK( 
        DMF_ERR_SCAN_RIFF_FAILURE, 30
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NO_METADATA_PRESENT, 31
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NO_DRM_PRESENT, 32
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BUFFER_TOO_SMALL, 33
        ),
    DMF_ERR_MASK( 
        DMF_ERR_MENU_FUNC_NULL, 34
        ),
    DMF_ERR_MASK( 
        DMF_ERR_MENU_MODULE_NULL, 35
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NO_MENU_FOUND, 36
        ),
    DMF_ERR_MASK( 
        DMF_ERR_UNSUPPORTED_QID, 37
        ),
    DMF_ERR_MASK( 
        DMF_ERR_REMOTE_FILE_UNSUPPORTED, 38
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_QUERY_ID, 39
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_QUERY_DTYPE, 40
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_SUBTITLE, 41
        ),
    DMF_ERR_MASK( 
        DMF_ERR_AUDIO_FORMAT_BAD, 42
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_HTTP_SERVER, 43
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NO_DATA_STAT_FILES, 44
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DATA_NOT_READY, 45
        ),
    DMF_ERR_MASK( 
        DMF_ERR_GET_MODULE_ID, 46
        ),
    DMF_ERR_MASK( 
        DMF_ERR_CODEC_UNSUPPORTED, 47
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NULL_MODULE_FUNC, 48
        ),
    DMF_ERR_MASK( 
        DMF_ERR_UNSUPPORTED_CONTAINER , 49
        ),
    DMF_ERR_MASK( 
        DMF_ERR_OPEN_CONTAINER, 50
        ),
    DMF_ERR_MASK( 
        DMF_ERR_LAYER1_FUNC_ERR, 51
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BAD_MODULE_ID, 52
        ),
    DMF_ERR_MASK( 
        DMF_ERR_MEM_FREE, 53
        ),
    DMF_ERR_MASK( 
        DMF_ERR_LAYER1_MODULE_INIT, 54
        ),
    DMF_ERR_MASK( 
        DMF_ERR_UNDEFINED_ENTRY, 55
        ),
    DMF_ERR_MASK( 
        DMF_ERR_NULL_MODULE_ID, 56
        ),
    DMF_ERR_MASK( 
        DMF_ERR_BLOCK_BUFFER_NULL, 57
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DRM_BUFFER_NULL, 58
        ),
    DMF_ERR_MASK( 
        DMF_ERR_ATTACHMENT_LINK_INVALID, 59
        ),
    DMF_ERR_MASK( 
        DMF_ERR_UNSUPPORTED_SPS_DATA, 60
        ),
    DMF_ERR_MASK( 
        DMF_ERR_OUTSIDE_RANGE, 61
        ),
    DMF_ERR_MASK( 
        DMF_ERR_LACING_AUD_TYPE_INVALID, 62
        ),
    DMF_ERR_MASK( 
        DMF_ERR_AR_IDC_INVALID, 63
        ),
    DMF_ERR_MASK( 
        DMF_ERR_AR_EXTENDED_UNSUPPORTED, 64
        ),
    DMF_ERR_MASK( 
        DMF_ERR_AUTHORIZATION_ERROR, 65
        ),
    DMF_ERR_MASK( 
        DMF_ERR_RENTAL_EXPIRED, 66
        ),
    DMF_ERR_MASK( 
        DMF_ERR_STRH_NOT_FOUND, 67
        ),
    DMF_ERR_MASK( 
        DMF_ERR_STRF_NOT_FOUND, 68
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DUPLICATE_STRF_FOUND, 69
        ),
    DMF_ERR_MASK( 
        DMF_ERR_STRL_NOT_FOUND, 70
        ),
    DMF_ERR_MASK( 
        DMF_ERR_MOVI_NOT_FOUND, 71
        ),
    DMF_ERR_MASK( 
        DMF_ERR_FILE_CORRUPT, 72
        ),
    DMF_ERR_MASK( 
        DMF_ERR_FILE_OPEN_FAILURE, 73
        ),
    DMF_ERR_MASK( 
        DMF_ERR_STREAM_DNE, 74
        ),
    DMF_ERR_MASK( 
        DMF_ERR_INVALID_TITLE_INDEX, 75
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_TRACKS, 76
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_SEGMENTINFO, 77
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_CUES, 78
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_SEEKHEAD, 79
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_HIERARCHICAL, 80
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_TAGS, 81
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_ATTACHMENTS, 82
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DF3_PARSE_CHAPTERS, 83
        ),
    DMF_ERR_MASK( 
        DMF_ERR_FONT_NOT_FOUND, 84
        ),
    DMF_ERR_MASK( 
        DMF_ERR_RESOLUTION_UNSUPPORTED, 85
        ),
    DMF_ERR_MASK( 
        DMF_ERR_FRAMERATE_UNSUPPORTED, 86
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DECODING_FAILURE, 87
        ),
    DMF_ERR_MASK( 
        DMF_ERR_COMBINE_FIELDS_UNSUPPORTED, 88
        ),
    DMF_ERR_MASK( 
        DMF_ERR_DRMVER_UNSUPPORTED, 89
        ),
    DMF_ERR_MASK( 
        DMF_ERR_RENTAL_NO, 90
        )
} DMFErrors;

static const DMFErrorDesc_t DMFErrorStrings[] = {
    {
        DMF_ERR_NO_INDEX_FOUND,
        DIVX_STR("DMF_ERR_NO_INDEX_FOUND"),
        DIVX_STR("No index found in file.") },
    {
        DMF_ERR_INVALID_MENU_VERSION,
        DIVX_STR("DMF_ERR_INVALID_MENU_VERSION"),
        DIVX_STR("Invalid menu version.") },
    {
        DMF_WARN_NONFATAL,
        DIVX_STR("DMF_WARN_NONFATAL"),
        DIVX_STR("Unknown warning.") },
    {
        DMF_ERR_UNEXPECTED_ERROR,
        DIVX_STR("DMF_ERR_UNEXPECTED_ERROR"),
        DIVX_STR("Unknown error.") },
    {
        DMF_ERR_INVALID_HANDLE,
        DIVX_STR("DMF_ERR_INVALID_HANDLER"),
        DIVX_STR("Invalid container handle.") },
    {
        DMF_ERR_INVALID_STREAM_HANDLE,
        DIVX_STR("DMF_ERR_INVALID_STREAM_HANDLE"),
        DIVX_STR("Invalid stream handle.") },
    {
        DMF_ERR_INVALID_STREAM,
        DIVX_STR("DMF_ERR_INVALID_STREAM"),
        DIVX_STR("Invalid stream.") },
    {
        DMF_ERR_BLOCK_READ_FAILURE,
        DIVX_STR("DMF_ERR_BLOCK_READ_FAILURE"),
        DIVX_STR("Failed to read media block.") },
    {
        DMF_ERR_BLOCK_BUFFER_TOO_SMALL,
        DIVX_STR("DMF_ERR_BLOCK_BUFFER_TOO_SMALL"),
        DIVX_STR("Block buffer too small.") },
    {
        DMF_ERR_SEEK_FAILURE,
        DIVX_STR("DMF_ERR_SEEK_FAILURE"),
        DIVX_STR("Failed to execute seek.") },
    {
        DMF_ERR_CREATE_INSTANCE,
        DIVX_STR("DMF_ERR_CREATE_INSTANCE"),
        DIVX_STR("Failed to create instance.") },
    {
        DMF_ERR_CREATE_READ_BUFFER,
        DIVX_STR("DMF_ERR_CREATE_READ_BUFFER"),
        DIVX_STR("Failed to create read buffer, memory may be low.") },
    {
        DMF_ERR_INVALID_FILE,
        DIVX_STR("DMF_ERR_INVALID_FILE"),
        DIVX_STR("Invalid file.") },
    {
        DMF_ERR_HDRL_NOT_FOUND,
        DIVX_STR("DMF_ERR_HDRL_NOT_FOUND"),
        DIVX_STR("AVI file struture error, no hdrl found.") },
    {
        DMF_ERR_DUPLICATE_STRH_FOUND,
        DIVX_STR("DMF_ERR_DUPLICATE_STRH_FOUND"),
        DIVX_STR("AVI file struture error, duplicate strh's.") },
    {
        DMF_ERR_TOO_MANY_VID_STREAMS,
        DIVX_STR("DMF_ERR_TOO_MANY_VID_STREAMS"),
        DIVX_STR("Too many video streams.") },
    {
        DMF_ERR_TOO_MANY_AUD_STREAMS,
        DIVX_STR("DMF_ERR_TOO_MANY_AUD_STREAMS"),
        DIVX_STR("Too many audio streams.") },
    {
        DMF_ERR_TOO_MANY_SUB_STREAMS,
        DIVX_STR("DMF_ERR_TOO_MANY_SUB_STREAMS"),
        DIVX_STR("Too many subtitle streams.") },
    {
        DMF_ERR_BAD_STRUCTURE,
        DIVX_STR("DMF_ERR_BAD_STRUCTURE"),
        DIVX_STR("Bad file structure.") },
    {
        DMF_ERR_INDEX_OUT_OF_RANGE,
        DIVX_STR("DMF_ERR_INDEX_OUT_OF_RANGE"),
        DIVX_STR("Index out of range.") },
    {
        DMF_ERR_GET_BLOCK_FAILED,
        DIVX_STR("DMF_ERR_GET_BLOCK_FAILED"),
        DIVX_STR("Get block failed.") },
    {
        DMF_ERR_SET_BLOCK_FAILED,
        DIVX_STR("DMF_ERR_SET_BLOCK_FAILED"),
        DIVX_STR("Set block failed.") },
    {
        DMF_ERR_SET_BY_TIME_FAILED,
        DIVX_STR("DMF_ERR_SET_BY_TIME_FAILED"),
        DIVX_STR("Set by time failed.") },
    {
        DMF_ERR_SET_BY_BLOCK_FAILED,
        DIVX_STR("DMF_ERR_SET_BY_BLOCK_FAILED"),
        DIVX_STR("Set by block failed.") },
    {
        DMF_ERR_END_OF_STREAM,
        DIVX_STR("DMF_ERR_END_OF_STREAM"),
        DIVX_STR("End of stream.") },
    {
        DMF_ERR_SET_TITLE_FAILED,
        DIVX_STR("DMF_ERR_SET_TITLE_FAILED"),
        DIVX_STR("Set title failure.") },
    {
        DMF_ERR_PROCESS_INDEX_FAILED,
        DIVX_STR("DMF_ERR_PROCESS_INDEX_FAILED"),
        DIVX_STR("Failure when processing index.") },
    {
        DMF_ERR_INVALID_BLOCK_AND_TIME,
        DIVX_STR("DMF_ERR_INVALID_BLOCK_AND_TIME"),
        DIVX_STR("Invalid block time.") },
    {
        DMF_ERR_SCAN_RIFF_FAILURE,
        DIVX_STR("DMF_ERR_SCAN_RIFF_FAILURE"),
        DIVX_STR("AVI file struture error, failure when processing headers.") },
    {
        DMF_ERR_NO_METADATA_PRESENT,
        DIVX_STR("DMF_ERR_NO_METADATA_PRESENT"),
        DIVX_STR("No metadata is found in file.") },
    {
        DMF_ERR_NO_DRM_PRESENT,
        DIVX_STR("DMF_ERR_NO_DRM_PRESENT"),
        DIVX_STR("File does not contain DRM.") },
    {
        DMF_ERR_BUFFER_TOO_SMALL,
        DIVX_STR("DMF_ERR_BUFFER_TOO_SMALL"),
        DIVX_STR("Buffer too small for data.") },
    {
        DMF_ERR_MENU_FUNC_NULL,
        DIVX_STR("DMF_ERR_MENU_FUNC_NULL"),
        DIVX_STR("Menu function does not exist.") },
    {
        DMF_ERR_MENU_MODULE_NULL,
        DIVX_STR("DMF_ERR_MENU_MODULE_NULL"),
        DIVX_STR("Menu module not loaded.") },
    {
        DMF_ERR_NO_MENU_FOUND,
        DIVX_STR("DMF_ERR_NO_MENU_FOUND"),
        DIVX_STR("Menu not found.") },
    {
        DMF_ERR_UNSUPPORTED_QID,
        DIVX_STR("DMF_ERR_UNSUPPORTED_QID"),
        DIVX_STR("Query ID value not supported by module.") },
    {
        DMF_ERR_REMOTE_FILE_UNSUPPORTED,
        DIVX_STR("DMF_ERR_REMOTE_FILE_UNSUPPORTED"),
        DIVX_STR("Remote file module not loaded.") },
    {
        DMF_ERR_INVALID_QUERY_ID,
        DIVX_STR("DMF_ERR_INVALID_QUERY_ID"),
        DIVX_STR("Invalid query ID.") },
    {
        DMF_ERR_INVALID_QUERY_DTYPE,
        DIVX_STR("DMF_ERR_INVALID_QUERY_DTYPE"),
        DIVX_STR("Data type invalid for query.") },
    {
        DMF_ERR_INVALID_SUBTITLE,
        DIVX_STR("DMF_ERR_INVALID_SUBTITLE"),
        DIVX_STR("Invalid subtitle.") },
    {
        DMF_ERR_AUDIO_FORMAT_BAD,
        DIVX_STR("DMF_ERR_AUDIO_FORMAT_BAD"),
        DIVX_STR("Unsupported audio format.") },
    {
        DMF_ERR_INVALID_HTTP_SERVER,
        DIVX_STR("DMF_ERR_INVALID_HTTP_SERVER"),
        DIVX_STR("Invalid HTTP server.") },
    {
        DMF_ERR_NO_DATA_STAT_FILES,
        DIVX_STR("DMF_ERR_NO_DATA_STAT_FILES"),
        DIVX_STR("Remote file error, no statistics file.") },
    {
        DMF_ERR_DATA_NOT_READY,
        DIVX_STR("DMF_ERR_DATA_NOT_READY"),
        DIVX_STR("Data not ready to read.") },
    {
        DMF_ERR_GET_MODULE_ID,
        DIVX_STR("DMF_ERR_GET_MODULE_ID"),
        DIVX_STR("Failed to get module ID.") },
    {
        DMF_ERR_CODEC_UNSUPPORTED,
        DIVX_STR("DMF_ERR_CODEC_UNSUPPORTED"),
        DIVX_STR("Unsupported codec.") },
    {
        DMF_ERR_NULL_MODULE_FUNC,
        DIVX_STR("DMF_ERR_NULL_MODULE_FUNC"),
        DIVX_STR("Funtion does not exist in module.") },
    {
        DMF_ERR_UNSUPPORTED_CONTAINER,
        DIVX_STR("DMF_ERR_UNSUPPORTED_CONTAINER"),
        DIVX_STR("Unsupported container type.") },
    {
        DMF_ERR_OPEN_CONTAINER,
        DIVX_STR("DMF_ERR_OPEN_CONTAINER"),
        DIVX_STR("Failed to open container.") },
    {
        DMF_ERR_LAYER1_FUNC_ERR,
        DIVX_STR("DMF_ERR_LAYER1_FUNC_ERR"),
        DIVX_STR("Generic Layer 1 function error.") },
    {
        DMF_ERR_BAD_MODULE_ID,
        DIVX_STR("DMF_ERR_BAD_MODULE_ID"),
        DIVX_STR("Invalid module ID.") },
    {
        DMF_ERR_MEM_FREE,
        DIVX_STR("DMF_ERR_MEM_FREE"),
        DIVX_STR("Failed to free memory.") },
    {
        DMF_ERR_LAYER1_MODULE_INIT,
        DIVX_STR("DMF_ERR_LAYER1_MODULE_INIT"),
        DIVX_STR("Failed to initialize module.") },
    {
        DMF_ERR_UNDEFINED_ENTRY,
        DIVX_STR("DMF_ERR_UNDEFINED_ENTRY"),
        DIVX_STR("undefined module entry point.") },
    {
        DMF_ERR_NULL_MODULE_ID,
        DIVX_STR("DMF_ERR_NULL_MODULE_ID"),
        DIVX_STR("Module ID null.") },
    {
        DMF_ERR_BLOCK_BUFFER_NULL,
        DIVX_STR("DMF_ERR_BLOCK_BUFFER_NULL"),
        DIVX_STR("Block buffer NULL.") },
    {
        DMF_ERR_DRM_BUFFER_NULL,
        DIVX_STR("DMF_ERR_DRM_BUFFER_NULL"),
        DIVX_STR("DRM buffer NULL.") },
    {
        DMF_ERR_ATTACHMENT_LINK_INVALID,
        DIVX_STR("DMF_ERR_ATTACHMENT_LINK_INVALID"),
        DIVX_STR("Attachment link invalid.") },
    {
        DMF_ERR_UNSUPPORTED_SPS_DATA,
        DIVX_STR("DMF_ERR_UNSUPPORTED_SPS_DATA"),
        DIVX_STR("unsupported H.264 SPS data.") },
    {
        DMF_ERR_OUTSIDE_RANGE,
        DIVX_STR("DMF_ERR_OUTSIDE_RANGE"),
        DIVX_STR("Outside range.") },
    {
        DMF_ERR_LACING_AUD_TYPE_INVALID,
        DIVX_STR("DMF_ERR_LACING_AUD_TYPE_INVALID"),
        DIVX_STR("Invalid audio lacing type.  Audio format may not support lacing!") },
    {
        DMF_ERR_AR_IDC_INVALID,
        DIVX_STR("DMF_ERR_AR_IDC_INVALID"),
        DIVX_STR("Invalid H.264 aspect ratio index.") },
    {
        DMF_ERR_AR_EXTENDED_UNSUPPORTED,
        DIVX_STR("DMF_ERR_AR_EXTENDED_UNSUPPORTED"),
        DIVX_STR("Extended aspect ratio unsupported.") },
    {
        DMF_ERR_AUTHORIZATION_ERROR,
        DIVX_STR("DMF_ERR_AUTHORIZATION_ERROR"),
        DIVX_STR("DRM authorization failed.") },
    {
        DMF_ERR_RENTAL_EXPIRED,
        DIVX_STR("DMF_ERR_RENTAL_EXPIRED"),
        DIVX_STR("Rental period expired.") },
    {
        DMF_ERR_STRH_NOT_FOUND,
        DIVX_STR("DMF_ERR_STRH_NOT_FOUND"),
        DIVX_STR("AVI file struture error, no strf found.") },
    {
        DMF_ERR_DUPLICATE_STRF_FOUND,
        DIVX_STR("DMF_ERR_DUPLICATE_STRF_FOUND"),
        DIVX_STR("AVI file struture error, duplicate strf's.") },
    {
        DMF_ERR_STRL_NOT_FOUND,
        DIVX_STR("DMF_ERR_STRL_NOT_FOUND"),
        DIVX_STR("AVI file struture error, no strl found.") },
    {
        DMF_ERR_MOVI_NOT_FOUND,
        DIVX_STR("DMF_ERR_MOVI_NOT_FOUND"),
        DIVX_STR("AVI file struture error, no movi list found.") },
    {
        DMF_ERR_FILE_CORRUPT,
        DIVX_STR("DMF_ERR_FILE_CORRUPT"),
        DIVX_STR("File is corrupt.") },
    {
        DMF_ERR_FILE_OPEN_FAILURE,
        DIVX_STR("DMF_ERR_FILE_OPEN_FAILURE"),
        DIVX_STR("Failed to open file.") },
    {
        DMF_ERR_STREAM_DNE,
        DIVX_STR("DMF_ERR_STREAM_DNE"),
        DIVX_STR("Stream does not exist.") },
    {
        DMF_ERR_INVALID_TITLE_INDEX,
        DIVX_STR("DMF_ERR_INVALID_TITLE_INDEX"),
        DIVX_STR("Invalid title index.") },
    {
        DMF_ERR_DF3_PARSE_TRACKS,
        DIVX_STR("DMF_ERR_DF3_PARSE_TRACKS"),
        DIVX_STR("MKV parser error, in tracks element") },
    {
        DMF_ERR_DF3_PARSE_SEGMENTINFO,
        DIVX_STR("DMF_ERR_DF3_PARSE_SEGMENTINFO"),
        DIVX_STR("MKV parser error, in segmentinfo element") },
    {
        DMF_ERR_DF3_PARSE_CUES,
        DIVX_STR("DMF_ERR_DF3_PARSE_CUES"),
        DIVX_STR("MKV parser error, in cues element") },
    {
        DMF_ERR_DF3_PARSE_SEEKHEAD,
        DIVX_STR("DMF_ERR_DF3_PARSE_SEEKHEAD"),
        DIVX_STR("MKV parser error, in seekhead element") },
    {
        DMF_ERR_DF3_PARSE_HIERARCHICAL,
        DIVX_STR("DMF_ERR_DF3_PARSE_HIERARCHICAL"),
        DIVX_STR("MKV parser error, in Hierarchical Index element") },
    {
        DMF_ERR_DF3_PARSE_TAGS,
        DIVX_STR("DMF_ERR_DF3_PARSE_TAGS"),
        DIVX_STR("MKV parser error, in Tags element") },
    {
        DMF_ERR_DF3_PARSE_ATTACHMENTS,
        DIVX_STR("DMF_ERR_DF3_PARSE_ATTACHMENTS"),
        DIVX_STR("MKV parser error, in Attachments element") },
    {
        DMF_ERR_DF3_PARSE_CHAPTERS,
        DIVX_STR("DMF_ERR_DF3_PARSE_CHAPTERS"),
        DIVX_STR("MKV parser error, in Chapters element") },
    {
        DMF_ERR_FONT_NOT_FOUND,
        DIVX_STR("DMF_ERR_FONT_NOT_FOUND"),
        DIVX_STR("Failed to load font.") },
    {
        DMF_ERR_RESOLUTION_UNSUPPORTED,
        DIVX_STR("DMF_ERR_RESOLUTION_UNSUPPORTED"),
        DIVX_STR("Resolution unsupported.") },
    {
        DMF_ERR_FRAMERATE_UNSUPPORTED,
        DIVX_STR("DMF_ERR_FRAMERATE_UNSUPPORTED"),
        DIVX_STR("Framerate unsupported.") },
    {
        DMF_ERR_DECODING_FAILURE,
        DIVX_STR("DMF_ERR_DECODING_FAILURE"),
        DIVX_STR("Decoding failure.") },
    {
        DMF_ERR_COMBINE_FIELDS_UNSUPPORTED,
        DIVX_STR("DMF_ERR_COMBINE_FIELDS_UNSUPPORTED"),
        DIVX_STR("Combine fields requested on unsupported bitstream type.") },
    {
        DMF_ERR_DRMVER_UNSUPPORTED,
        DIVX_STR("DMF_ERR_DRMVER_UNSUPPORTED"),
        DIVX_STR("DRM version unsupported.") },
    {
        DMF_ERR_RENTAL_NO,
        DIVX_STR("DMF_ERR_RENTAL_NO"),
        DIVX_STR("View rental screen no selected.") }

};

#define DMF_ERR_STRINGS_COUNT (sizeof(DMFErrorStrings)/sizeof(DMFErrorStrings[0]))

const DivXString *DMFError_ToString( DMFErrors error );

const DivXString *DMFError_GetDescription( DMFErrors error );

#ifdef __cplusplus
}
#endif

#endif /* _DMFERRORS_H_ */
/**  @}  */
