/*!
    @file
@verbatim
$Id: avcContainerReadStructures.h 59012 2009-03-20 19:15:35Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVCCONTAINERREADSTRUCTURES_H_
#define _AVCCONTAINERREADSTRUCTURES_H_

#include "DivXFile.h"
#include "DivXTime.h"
#include "DivXTypes.h"

#define MAX_NALU_SIZE 10000000

typedef struct _Hrd_Parameters_t
{
    uint8_t     cpb_cnt_minus1;                         // 0-31
    uint8_t     bit_rate_scale;                         // 0-15
    uint8_t     cpb_size_scale;                         // 0-15
    uint32_t    bit_rate_value_minus1[32];              // 0 - (2^32 - 2)
    uint32_t    cpb_size_value_minus1[32];              // 0 - (2^32 - 2)
    uint8_t     cbr_flag[32];                           // 0-1
    uint8_t     initial_cpb_removal_delay_lenth_minus1; // 0-31
    uint8_t     cpb_removal_delay_length_minus1;        // 0-31
    uint8_t     dpb_output_delay_length_minus1;         // 0-31
    uint8_t     time_offset_length;                     // 0-31
} Hrd_Parameters_t;

// video usability information
typedef struct _Vui_Parameters_t
{
    uint8_t     aspect_ratio_info_present_flag;         // 0-1
    uint8_t     aspect_ratio_idc;                       // 0-255
    uint16_t    sar_width;                              // 0-65535
    uint16_t    sar_height;                             // 0-65535

    uint8_t     overscan_info_present_flag;             // 0-1
    uint8_t     overscan_appropriate_flag;              // 0-1

    uint8_t     video_signal_type_present_flag;         // 0-1
    uint8_t     video_format;                           // 0-7
    uint8_t     video_full_range_flag;                  // 0-1
    uint8_t     colour_description_present_flag;        // 0-1
    uint8_t     colour_primaries;                       // 0-255
    uint8_t     transfer_characteristics;               // 0-255
    uint8_t     matrix_coefficients;                    // 0-255

    uint8_t     chroma_loc_info_present_flag;           // 0-1
    uint8_t     chroma_sample_loc_type_top_field;       // 0-5
    uint8_t     chroma_sample_loc_type_bottom_field;    // 0-5

    uint8_t     timing_info_present_flag;               // 0-1
    uint32_t    num_units_in_tick;                      // 0 - (2^32 - 1)
    uint32_t    time_scale;                             // 0 - (2^32 - 1)
    uint8_t     fixed_frame_rate_flag;                  // 0-1

    uint8_t     nal_hrd_parameters_present_flag;        // 0-1; hypothetical reference decoder parameters not supported
    Hrd_Parameters_t    Nal_Hrd_Parameters;
    uint8_t     vcl_hrd_parameters_present_flag;        // 0-1; hypothetical reference decoder parameters not supported
    Hrd_Parameters_t    Vcl_Hrd_Parameters;

    uint8_t     pic_struct_present_flag;                // 0-1
    uint8_t     bitstream_restriction_flag;             // 0-1
    uint8_t     motion_vectors_over_pic_boundaries_flag;// 0-1
    uint8_t     max_bytes_per_pic_denom;                // 0-16
    uint8_t     max_bytes_per_mb_denom;                 // 0-16
    uint8_t     log2_max_mv_length_horizontal;          // 0-16
    uint8_t     log2_max_mv_length_vertical;            // 0-16
    uint8_t     num_reorder_frames;                     // 0 - max_dec_frame_buffering
    uint8_t     max_dec_frame_buffering;                // num_ref_frames - MaxDpbSize

} Vui_Parameters_t;

typedef struct _Seq_Parameter_Set_Data_t
{
    uint8_t     profile_idc;
    uint8_t     constraint_flags;
    uint8_t     level_idc;
    
    uint8_t     seq_parameter_set_id;                   // 0-31

    uint8_t     chroma_format_idc;                      // 0-3
    uint8_t     separate_colour_plane_flag;             // 0-1

    uint8_t     bit_depth_luma_minus8;                  // 0-6
    uint8_t     bit_depth_chroma_minus8;                // 0-6
    uint8_t     qpprime_y_zero_transform_bypass_flag;   // 0-1
    uint8_t     seq_scaling_matrix_present_flag;        // 0-1; parsing sps w/ scaling matrix not currently supported
    
    uint8_t     log2_max_frame_num_minus4;              // 0-12
    
    uint8_t     pic_order_cnt_type;                     // 0-2
    uint8_t     log2_max_pic_order_cnt_lsb_minus4;      // 0-12
    uint8_t     delta_pic_order_always_zero_flag;       // 0-1
    int32_t     offset_for_non_ref_pic;                 // (-2^31) - (2^31 - 1)
    int32_t     offset_for_top_to_bottom_field;         // (-2^31) - (2^31 - 1)
    uint8_t     num_ref_frames_in_pic_order_cnt_cycle;  // 0-255
    int32_t     offset_for_ref_frame[256];              // (-2^31) - (2^31 - 1)

    uint8_t     num_ref_frames;                         // 0-16
    uint8_t     gaps_in_frame_num_value_allowed_flag;   // 0-1

    uint32_t    pic_width_in_mbs_minus1;
    uint32_t    pic_height_in_map_units_minus1;
    
    uint8_t     frame_mbs_only_flag;                    // 0-1
    uint8_t     mb_adaptive_frame_field_flag;           // 0-1
    uint8_t     direct_8x8_inference_flag;              // 0-1
    uint8_t     frame_cropping_flag;                    // 0-1

    uint16_t    frame_crop_left_offset;                 // valid values vary
    uint16_t    frame_crop_right_offset;                // valid values vary
    uint16_t    frame_crop_top_offset;                  // valid values vary
    uint16_t    frame_crop_bottom_offset;               // valid values vary

    uint8_t     vui_parameters_present_flag;            // 0-1
    Vui_Parameters_t    vui;

} Seq_Parameter_Set_Data_t;

/*
    This is the internal structure for the avc container.
 */
typedef struct AVCContainer_t
{
    DivXFile            hFile;
    DivXBool            bStreamSetActive;
    DivXBool            bTitleSet;
    
    int64_t             fileSize;
    uint64_t            fileOffset;

    uint8_t             fileBuffer[MAX_NALU_SIZE];
    uint32_t            bufferSize;                  // number of bytes loaded in fileBuffer
    uint32_t            bufferOffset;                 // index of read pointer in fileBuffer

    uint32_t            nalTotalBytes;               // total number of bytes used by NAL and start code prefix

    uint32_t            blockNumber;

    uint32_t            width;
    uint32_t            height;

    uint32_t            rate;
    uint32_t            scale;

    Seq_Parameter_Set_Data_t    sps_data;

    uint8_t            *spsBuffer;
    uint32_t            spsSize;
    uint8_t            *ppsBuffer;
    uint32_t            ppsSize;
} AVCContainer_t;


typedef struct _AspectRatioValues_t
{
    int32_t numerator;
    int32_t denominator;
}AspectRatioValues_t;

static AspectRatioValues_t aspect_ratio_idc[] = 
{
    {0,0}, /* unspecified */
    {1,1},
    {12,11},
    {10,11},
    {16,11},
    {40,33},
    {24,11},
    {20,11},
    {32,11},
    {80,33},
    {18,11},
    {15,11},
    {64,33},
    {160,99},
    {4,3},
    {3,2},
    {2,1},
    {0,0}, /* reserved */
    {0,0}  /*extended SAR */
};

#endif /* _AVCCONTAINERREADSTRUCTURES_H_ */
