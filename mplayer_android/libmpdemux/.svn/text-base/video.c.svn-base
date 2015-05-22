/*
 * video frame reading
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <stdio.h>
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "demux_ty_osd.h"
#include "stheader.h"
#include "parse_es.h"
#include "mpeg_hdr.h"

/* sub_cc (closed captions)*/
#include "sub_cc.h"

/* biCompression constant */
#define BI_RGB        0L

#ifdef CONFIG_LIVE555
#include "demux_rtp.h"
#endif

static mp_mpeg_header_t picture;
int sky_telecine = 0;
int sky_repeatfield_cnt = 0;
int sky_repeatfield_flag = 0;
static int telecine=0;
static float telecine_cnt=-2.5;

//int bidx = 0;
int use_PP_buf = 0;
unsigned char *VideoBuffer[3] = {NULL, NULL, NULL};

//Robert 20100712 new VideoBuffer Idx
unsigned int VideoBufferIdx = 0;
#define RESERVED_BUFFER_SIZE	4//3//4//3
unsigned int sky_vd_reserved_size = RESERVED_BUFFER_SIZE;
//Robert 20100503
//extern int skydvb_need_workaround;
extern int sky_mpeg12_progressive_sequence;
//Fuchun 2010.07.01
extern int pic_struct_present_flag;
//Polun 2011-07-01 ++s
//extern int interlace_mbaff;
int video_1080_mbaff_flag = 0;
//Polun 2011-07-01 ++e
unsigned int get_h264_iframe = 0; //Polun 2011-10-05 fixed h264 ts file first frame not I frame.
unsigned int h264_check_sps = 0;//Polun 2011-11-30
float ts_h264_next_inc_pts =0.0; //polun 2012-01-17 fixed FJ train_window_20110525.ts playback not smooth.
extern int h264_frame_field; //Polun 2011-12-09
extern int maxFrameNum; //Polun 2011-12-13 for H264 get frame num. 
extern int mb_adaptive_frame_field_flag; //Polun 2011-12-30  MBAFF flag.


#include "libmpcodecs/sky_vdec_2.h"
extern vdec_shm_t *svsd_state;

// Raymond 2009/12/15
int check_fast_ts_startcode = 0;	//Barry 2010-05-10

//Barry 2010-07-02
int ts_avc1_pts_queue_cnt = 0;
float *ts_avc1_pts_queue;
extern int num_ref_frames;
extern int h264_reorder_num;
extern int h264_frame_mbs_only;

//Barry 2010-04-21
int FAST_MPEG_PS_FLAG = 0;
int check_fast_ps_startcode = 0;
int FAST_MPEG_PS_SEEK = 0;		//Barry 2010-04-22
//Barry 2010-08-06
float fast_ts_pre_pts = 0.0;
int check_fast_ts = 0, fast_ts_pts_cnt = 0;
int first_time_len = 0;		//Barry 2010-08-11
static int frame_split = 1;	//Barry 2010-08-18
static int h264_header_packet = 0;	//Barry 2010-11-10
int h264_fast_ts_seek = 0;	//Barry 2010-11-23
//Barry 2010-12-15
float check_h264_pts=0.0;
int h264_fast_ts_check = 0;
unsigned char *h264_sps_buf = NULL;
int h264_sps_len = 0;
//Barry 2011-03-30
int hddvd_check = 0;
extern double correct_sub_pts;
static int pre_picture_coding_type=0;

//Barry 2011-05-25
unsigned int ts_h264_field_num = 0;
unsigned int ts_h264_field_offset[8];
int h264_need_merge_more = 0;	//Barry 2011-07-22

//Barry 2011-08-11
extern int rewind_mult;
extern float MPEG_ES_START_PTS;

#ifdef HW_TS_DEMUX	// carlos add 2010-07-27
extern int hwtsdemux;
#endif // end of HW_TS_DEMUX

extern unsigned int dvd_btn_addr_idx;
int dvdnav_should_do_alpha=0;
#include "stream/stream_dvdnav.h"
extern DvdnavState   dvdnavstate;
extern char video_names[];

//rywu20090601++ same as definition in demux_real.c
//NOTE : once you modify its structure, PLEASE update both files, demux_real.c and this file.
typedef struct dp_hdr_s {
    uint32_t chunks;	// number of chunks
    uint32_t timestamp; // timestamp from packet header
    uint32_t len;	// length of actual data
    uint32_t chunktab;	// offset to chunk offset array
} dp_hdr_t;
#define DP_HDR_T_SIZE sizeof(dp_hdr_t)

typedef struct _rv_info_ {
    //RV header size.
    //Only 1st frame contains both sequence and picture header.
    //Others contains picture header.
    uint32_t rv_header_len;

    //You can add some other information here.
    uint32_t useless_slice_num;	//rywu20090629
    uint32_t frame_padding_len;//rywu20090701
} rv_info;
#define RV_INFO_BLOCK_SIZE sizeof(rv_info)
//rywu20090601--

typedef enum {
    VIDEO_MPEG12,
    VIDEO_MPEG4,
    VIDEO_H264,
    VIDEO_VC1,
    VIDEO_OTHER
} video_codec_t;

int  picture_coding_type = 0;    //JF 1031 : local to global

#ifdef CMV_WORKAROUND
/*JF 1024****************************************************************************/


/* Start codes. */
#define SEQ_END_CODE                   0x000001b7
#define SEQ_START_CODE                0x000001b3
#define GOP_START_CODE                0x000001b8
#define PICTURE_START_CODE          0x00000100
#define SLICE_MIN_START_CODE      0x00000101  
#define SLICE_MAX_START_CODE     0x000001af
#define EXT_START_CODE                0x000001b5
#define USER_START_CODE              0x000001b2

#define MB_PTYPE_VLC_BITS 6


#define MB_TYPE_INTRA       0x0001
#define MB_TYPE_QUANT      0x00010000


#define MB_TYPE_16x16      0x0008
#define MB_TYPE_16x8       0x0010


#define MB_TYPE_P0L0       0x1000
#define MB_TYPE_P1L0       0x2000
#define MB_TYPE_L0         (MB_TYPE_P0L0 | MB_TYPE_P1L0)

#define MB_TYPE_CBP        0x00020000

#define MB_TYPE_ZERO_MV   0x20000000
#define MB_TYPE_INTERLACED 0x0080


#define IS_QUANT(a)      ((a)&MB_TYPE_QUANT)
#define IS_INTRA(a)      ((a)&7)
#define HAS_CBP(a)        ((a)&MB_TYPE_CBP)

#define IS_DIR(a, part, list) ((a) & (MB_TYPE_P0L0<<((part)+2*(list))))
#define USES_LIST(a, list) ((a) & ((MB_TYPE_P0L0|MB_TYPE_P1L0)<<(2*(list)))) 


#define MV_VLC_BITS 9

#define MB_PAT_VLC_BITS 9

#define MBINCR_VLC_BITS 9
#define INIT_VLC_LE         2
#define INIT_VLC_USE_NEW_STATIC 4


#define DC_VLC_BITS 9
#define TEX_VLC_BITS 9

#define MT_FIELD 1
#define MT_FRAME 2
#define MT_16X8  2
#define MT_DMV   3


#define  u8    uint8_t
#define  u32  uint32_t 
#define  u16  uint16_t 

#define  i8    int8_t 
#define  i16   int16_t 
#define  i32   int32_t 


typedef struct GetBitContext {
    const u8 *buffer, *buffer_end;
    int index;
    int size_in_bits;
} GetBitContext;

   GetBitContext gb;	



typedef struct PutBitContext {

    uint32_t bit_buf;
    int bit_left;
    uint8_t *buf, *buf_ptr, *buf_end;
    int size_in_bits;
	
} PutBitContext;


PutBitContext  pb;

typedef i16 DCTELEM;


DCTELEM   blocks[6][64];
DCTELEM   * block;



#define VLC_TYPE    i16     

typedef struct VLC {
    int bits;
    VLC_TYPE (*table)[2];                   
    int table_size, table_allocated;
} VLC;



typedef struct RL_VLC_ELEM {
    i16 level;
    i8 len;
    u8 run;

} RL_VLC_ELEM;



/* run length table */
#define MAX_RUN    64
#define MAX_LEVEL  64

/** RLTable. */
typedef struct RLTable {
    int n;                         
    int last;                      
    const u16 (*table_vlc)[2];
    const i8 *table_run;
    const i8 *table_level;
    u8 *index_run[2];         
    i8 *max_level[2];          
    i8 *max_run[2];            
    VLC vlc;                      
    RL_VLC_ELEM *rl_vlc[32];       
} RLTable;

   GetBitContext gb_cp;

u8 ff_mpeg12_static_rl_table_store[2][2][2*MAX_RUN + MAX_LEVEL + 3];       





static VLC mv_vlc;
static VLC mbincr_vlc;
static VLC mb_ptype_vlc;

static VLC mb_pat_vlc;

static VLC dc_lum_vlc;
static VLC dc_chroma_vlc;


const u8 mpeg12_vlc_dc_lum_bits[12] = {
    3, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 9,
};


const u16 mpeg12_vlc_dc_lum_code[12] = {
    0x4, 0x0, 0x1, 0x5, 0x6, 0xe, 0x1e, 0x3e, 0x7e, 0xfe, 0x1fe, 0x1ff,
};


const u8  mpeg12_vlc_dc_chroma_bits[12] = {
    2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10,
};


const u16  mpeg12_vlc_dc_chroma_code[12] = {
    0x0, 0x1, 0x2, 0x6, 0xe, 0x1e, 0x3e, 0x7e, 0xfe, 0x1fe, 0x3fe, 0x3ff,
};


static const uint8_t mpeg12_mbPatTable[64][2] = {
    {0x1, 9},
    {0xb, 5},
    {0x9, 5},
    {0xd, 6},
    {0xd, 4},
    {0x17, 7},
    {0x13, 7},
    {0x1f, 8},
    {0xc, 4},
    {0x16, 7},
    {0x12, 7},
    {0x1e, 8},
    {0x13, 5},
    {0x1b, 8},
    {0x17, 8},
    {0x13, 8},
    {0xb, 4},
    {0x15, 7},
    {0x11, 7},
    {0x1d, 8},
    {0x11, 5},
    {0x19, 8},
    {0x15, 8},
    {0x11, 8},
    {0xf, 6},
    {0xf, 8},
    {0xd, 8},
    {0x3, 9},
    {0xf, 5},
    {0xb, 8},
    {0x7, 8},
    {0x7, 9},
    {0xa, 4},
    {0x14, 7},
    {0x10, 7},
    {0x1c, 8},
    {0xe, 6},
    {0xe, 8},
    {0xc, 8},
    {0x2, 9},
    {0x10, 5},
    {0x18, 8},
    {0x14, 8},
    {0x10, 8},
    {0xe, 5},
    {0xa, 8},
    {0x6, 8},
    {0x6, 9},
    {0x12, 5},
    {0x1a, 8},
    {0x16, 8},
    {0x12, 8},
    {0xd, 5},
    {0x9, 8},
    {0x5, 8},
    {0x5, 9},
    {0xc, 5},
    {0x8, 8},
    {0x4, 8},
    {0x4, 9},
    {0x7, 3},
    {0xa, 5},
    {0x8, 5},
    {0xc, 6}
};


const uint8_t mpeg12_mbMotionVectorTable[17][2] = {
{ 0x1, 1 },
{ 0x1, 2 },
{ 0x1, 3 },
{ 0x1, 4 },
{ 0x3, 6 },
{ 0x5, 7 },
{ 0x4, 7 },
{ 0x3, 7 },
{ 0xb, 9 },
{ 0xa, 9 },
{ 0x9, 9 },
{ 0x11, 10 },
{ 0x10, 10 },
{ 0xf, 10 },
{ 0xe, 10 },
{ 0xd, 10 },
{ 0xc, 10 },
};


static  const u8 mpeg12_mbAddrIncrTable[36][2] = {
    {0x1, 1},
    {0x3, 3},
    {0x2, 3},
    {0x3, 4},
    {0x2, 4},
    {0x3, 5},
    {0x2, 5},
    {0x7, 7},
    {0x6, 7},
    {0xb, 8},
    {0xa, 8},
    {0x9, 8},
    {0x8, 8},
    {0x7, 8},
    {0x6, 8},
    {0x17, 10},
    {0x16, 10},
    {0x15, 10},
    {0x14, 10},
    {0x13, 10},
    {0x12, 10},
    {0x23, 11},
    {0x22, 11},
    {0x21, 11},
    {0x20, 11},
    {0x1f, 11},
    {0x1e, 11},
    {0x1d, 11},
    {0x1c, 11},
    {0x1b, 11},
    {0x1a, 11},
    {0x19, 11},
    {0x18, 11},
    {0x8, 11}, 
    {0xf, 11}, 
    {0x0, 8}, 
};



static const uint8_t table_mb_ptype[7][2] = {
    { 3, 5 }, 
    { 1, 2 }, 
    { 1, 3 },
    { 1, 1 }, 
    { 1, 6 }, 
    { 1, 5 }, 
    { 2, 5 }, 
};

static const uint32_t ptype2mb_type[7] = {
                    MB_TYPE_INTRA,
                    MB_TYPE_L0 | MB_TYPE_CBP | MB_TYPE_ZERO_MV | MB_TYPE_16x16,
                    MB_TYPE_L0,
                    MB_TYPE_L0 | MB_TYPE_CBP,
                    MB_TYPE_QUANT | MB_TYPE_INTRA,
                    MB_TYPE_QUANT | MB_TYPE_L0 | MB_TYPE_CBP | MB_TYPE_ZERO_MV | MB_TYPE_16x16,
                    MB_TYPE_QUANT | MB_TYPE_L0 | MB_TYPE_CBP,
};


static const uint16_t mpeg1_vlc[113][2] = {
 { 0x3, 2 }, { 0x4, 4 }, { 0x5, 5 }, { 0x6, 7 },
 { 0x26, 8 }, { 0x21, 8 }, { 0xa, 10 }, { 0x1d, 12 },
 { 0x18, 12 }, { 0x13, 12 }, { 0x10, 12 }, { 0x1a, 13 },
 { 0x19, 13 }, { 0x18, 13 }, { 0x17, 13 }, { 0x1f, 14 },
 { 0x1e, 14 }, { 0x1d, 14 }, { 0x1c, 14 }, { 0x1b, 14 },
 { 0x1a, 14 }, { 0x19, 14 }, { 0x18, 14 }, { 0x17, 14 },
 { 0x16, 14 }, { 0x15, 14 }, { 0x14, 14 }, { 0x13, 14 },
 { 0x12, 14 }, { 0x11, 14 }, { 0x10, 14 }, { 0x18, 15 },
 { 0x17, 15 }, { 0x16, 15 }, { 0x15, 15 }, { 0x14, 15 },
 { 0x13, 15 }, { 0x12, 15 }, { 0x11, 15 }, { 0x10, 15 },
 { 0x3, 3 }, { 0x6, 6 }, { 0x25, 8 }, { 0xc, 10 },
 { 0x1b, 12 }, { 0x16, 13 }, { 0x15, 13 }, { 0x1f, 15 },
 { 0x1e, 15 }, { 0x1d, 15 }, { 0x1c, 15 }, { 0x1b, 15 },
 { 0x1a, 15 }, { 0x19, 15 }, { 0x13, 16 }, { 0x12, 16 },
 { 0x11, 16 }, { 0x10, 16 }, { 0x5, 4 }, { 0x4, 7 },
 { 0xb, 10 }, { 0x14, 12 }, { 0x14, 13 }, { 0x7, 5 },
 { 0x24, 8 }, { 0x1c, 12 }, { 0x13, 13 }, { 0x6, 5 },
 { 0xf, 10 }, { 0x12, 12 }, { 0x7, 6 }, { 0x9, 10 },
 { 0x12, 13 }, { 0x5, 6 }, { 0x1e, 12 }, { 0x14, 16 },
 { 0x4, 6 }, { 0x15, 12 }, { 0x7, 7 }, { 0x11, 12 },
 { 0x5, 7 }, { 0x11, 13 }, { 0x27, 8 }, { 0x10, 13 },
 { 0x23, 8 }, { 0x1a, 16 }, { 0x22, 8 }, { 0x19, 16 },
 { 0x20, 8 }, { 0x18, 16 }, { 0xe, 10 }, { 0x17, 16 },
 { 0xd, 10 }, { 0x16, 16 }, { 0x8, 10 }, { 0x15, 16 },
 { 0x1f, 12 }, { 0x1a, 12 }, { 0x19, 12 }, { 0x17, 12 },
 { 0x16, 12 }, { 0x1f, 13 }, { 0x1e, 13 }, { 0x1d, 13 },
 { 0x1c, 13 }, { 0x1b, 13 }, { 0x1f, 16 }, { 0x1e, 16 },
 { 0x1d, 16 }, { 0x1c, 16 }, { 0x1b, 16 },
 { 0x1, 6 }, 
 { 0x2, 2 },
};

static const u16  mpeg2_vlc[113][2] = {
  {0x02, 2}, {0x06, 3}, {0x07, 4}, {0x1c, 5},
  {0x1d, 5}, {0x05, 6}, {0x04, 6}, {0x7b, 7},
  {0x7c, 7}, {0x23, 8}, {0x22, 8}, {0xfa, 8},
  {0xfb, 8}, {0xfe, 8}, {0xff, 8}, {0x1f,14},
  {0x1e,14}, {0x1d,14}, {0x1c,14}, {0x1b,14},
  {0x1a,14}, {0x19,14}, {0x18,14}, {0x17,14},
  {0x16,14}, {0x15,14}, {0x14,14}, {0x13,14},
  {0x12,14}, {0x11,14}, {0x10,14}, {0x18,15},
  {0x17,15}, {0x16,15}, {0x15,15}, {0x14,15},
  {0x13,15}, {0x12,15}, {0x11,15}, {0x10,15},
  {0x02, 3}, {0x06, 5}, {0x79, 7}, {0x27, 8},
  {0x20, 8}, {0x16,13}, {0x15,13}, {0x1f,15},
  {0x1e,15}, {0x1d,15}, {0x1c,15}, {0x1b,15},
  {0x1a,15}, {0x19,15}, {0x13,16}, {0x12,16},
  {0x11,16}, {0x10,16}, {0x05, 5}, {0x07, 7},
  {0xfc, 8}, {0x0c,10}, {0x14,13}, {0x07, 5},
  {0x26, 8}, {0x1c,12}, {0x13,13}, {0x06, 6},
  {0xfd, 8}, {0x12,12}, {0x07, 6}, {0x04, 9},
  {0x12,13}, {0x06, 7}, {0x1e,12}, {0x14,16},
  {0x04, 7}, {0x15,12}, {0x05, 7}, {0x11,12},
  {0x78, 7}, {0x11,13}, {0x7a, 7}, {0x10,13},
  {0x21, 8}, {0x1a,16}, {0x25, 8}, {0x19,16},
  {0x24, 8}, {0x18,16}, {0x05, 9}, {0x17,16},
  {0x07, 9}, {0x16,16}, {0x0d,10}, {0x15,16},
  {0x1f,12}, {0x1a,12}, {0x19,12}, {0x17,12},
  {0x16,12}, {0x1f,13}, {0x1e,13}, {0x1d,13},
  {0x1c,13}, {0x1b,13}, {0x1f,16}, {0x1e,16},
  {0x1d,16}, {0x1c,16}, {0x1b,16},
  {0x01,6}, 
  {0x06,4}, 
};


static const i8 mpeg1_run[111] = {
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  2,  2,  2,  2,  2,  3,
  3,  3,  3,  4,  4,  4,  5,  5,
  5,  6,  6,  6,  7,  7,  8,  8,
  9,  9, 10, 10, 11, 11, 12, 12,
 13, 13, 14, 14, 15, 15, 16, 16,
 17, 18, 19, 20, 21, 22, 23, 24,
 25, 26, 27, 28, 29, 30, 31,
};



static const i8 mpeg1_level[111] = {
  1,  2,  3,  4,  5,  6,  7,  8,
  9, 10, 11, 12, 13, 14, 15, 16,
 17, 18, 19, 20, 21, 22, 23, 24,
 25, 26, 27, 28, 29, 30, 31, 32,
 33, 34, 35, 36, 37, 38, 39, 40,
  1,  2,  3,  4,  5,  6,  7,  8,
  9, 10, 11, 12, 13, 14, 15, 16,
 17, 18,  1,  2,  3,  4,  5,  1,
  2,  3,  4,  1,  2,  3,  1,  2,
  3,  1,  2,  3,  1,  2,  1,  2,
  1,  2,  1,  2,  1,  2,  1,  2,
  1,  2,  1,  2,  1,  2,  1,  2,
  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,
};

RLTable rl_mpeg1 = {
    111,
    111,
    mpeg1_vlc,
    mpeg1_run,
    mpeg1_level,
};



RLTable rl_mpeg2 = {
    111,
    111,
    mpeg2_vlc,
    mpeg1_run,
    mpeg1_level,
};


#if 0 //JF 1108
static const uint8_t  non_linear_qscale[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8,10,12,14,16,18,20,22,
    24,28,32,36,40,44,48,52,
    56,64,72,80,88,96,104,112,
};
#endif


//JF 1024    
unsigned char *data_2;

static int motion_cmv =0;

static int  first_entry = 1;

static int   ret_error = 0;		   

			  



       int last_mv_00 =0;  
       int last_mv_01 =0;  
       int last_mv_10 =0;  
       int last_mv_11 =0;  

       int mv_01  = 0;
       int mv_11  = 0;     


#define INIT_2D_VLC_RL(rl, static_size)\
{\
    static RL_VLC_ELEM rl_vlc_table[static_size];\
    INIT_VLC_STATIC(&rl.vlc, TEX_VLC_BITS, rl.n + 2,\
             &rl.table_vlc[0][1], 4, 2,\
             &rl.table_vlc[0][0], 4, 2, static_size);\
\
    rl.rl_vlc[0]= rl_vlc_table;\
    init_2d_vlc_rl(&rl);\
}


static void init_2d_vlc_rl(RLTable *rl)
{
    int i;

    for(i=0; i<rl->vlc.table_size; i++){
        int code= rl->vlc.table[i][0];
        int len = rl->vlc.table[i][1];
        int level, run;

        if(len==0){ 
            run= 65;
            level= MAX_LEVEL;
        }else if(len<0){ 
            run= 0;
            level= code;
        }else{
            if(code==rl->n){ 
                run= 65;
                level= 0;
            }else if(code==rl->n+1){ 
                run= 0;
                level= 127;
            }else{
                run=   rl->table_run  [code] + 1;
                level= rl->table_level[code];
            }
        }
        rl->rl_vlc[0][i].len= len;
        rl->rl_vlc[0][i].level= level;
        rl->rl_vlc[0][i].run= run;
    }
}



int init_vlc_sparse(VLC *vlc, int nb_bits, int nb_codes,
             const void *bits, int bits_wrap, int bits_size,
             const void *codes, int codes_wrap, int codes_size,
             const void *symbols, int symbols_wrap, int symbols_size,
             int flags);




#define init_vlc(vlc, nb_bits, nb_codes,\
                 bits, bits_wrap, bits_size,\
                 codes, codes_wrap, codes_size,\
                 flags)\
                 init_vlc_sparse(vlc, nb_bits, nb_codes,\
                 bits, bits_wrap, bits_size,\
                 codes, codes_wrap, codes_size,\
                 NULL, 0, 0, flags)
                 




#define INIT_VLC_STATIC(vlc, bits, a,b,c,d,e,f,g, static_size)\
{\
    static VLC_TYPE table[static_size][2];\
    (vlc)->table= table;\
    (vlc)->table_allocated= static_size;\
    init_vlc(vlc, bits, a,b,c,d,e,f,g, INIT_VLC_USE_NEW_STATIC);\
}



/////////////////////////////////////////////////////



#define NEG_USR32(a,s) (((u32)(a))>>(32-(s)))


#define NEG_SSR32(a,s) ((( i32)(a))>>(32-(s)))


#define AV_RB32(x)  ((((const u8*)(x))[0] << 24) | (((const u8*)(x))[1] << 16) | (((const u8*)(x))[2] <<  8) |  ((const u8*)(x))[3])




#define OPEN_READER(name, gb)\
        unsigned int name##_index= (gb)->index;\
        int name##_cache= 0;\

#define CLOSE_READER(name, gb)\
        (gb)->index= name##_index;\







#define UPDATE_CACHE(name, gb)\
        name##_cache= AV_RB32( ((const u8 *)(gb)->buffer)+(name##_index>>3) ) << (name##_index&0x07);\


#define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)


#define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache, num)


#define SKIP_COUNTER(name, gb, num)\
        name##_index += (num);\


#define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)


#define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);


#define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\




#define GET_CACHE(name, gb)\
        ((u32)name##_cache)



///////////////////////////////////////////////////////////////////////


static inline int get_xbits(GetBitContext *s, int n){
    register int sign;
    register i32 cache;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    cache = GET_CACHE(re,s);
    sign=(~cache)>>31;
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)

    return (NEG_USR32(sign ^ cache, n) ^ sign) - sign;
}



static inline void skip_bits(GetBitContext *s, int n){
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
}


static inline unsigned int show_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
    return tmp;
}

static inline unsigned int get_bits1(GetBitContext *s){
#if 1 
    unsigned int index= s->index;
    u8 result= s->buffer[ index>>3 ];

    result<<= (index&0x07);
    result>>= 8 - 1;

    index++;
    s->index= index;

    return result;
#else
    return get_bits(s, 1);
#endif
}



static inline unsigned int get_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
    return tmp;
}


#define GET_VLC(code, name, gb, table, bits, max_depth)\
{\
    int n, nb_bits;\
    unsigned int index;\
\
    index= SHOW_UBITS(name, gb, bits);\
    code = table[index][0];\
    n    = table[index][1];\
\
    if(max_depth > 1 && n < 0){\
        LAST_SKIP_BITS(name, gb, bits)\
        UPDATE_CACHE(name, gb)\
\
        nb_bits = -n;\
\
        index= SHOW_UBITS(name, gb, nb_bits) + code;\
        code = table[index][0];\
        n    = table[index][1];\
        if(max_depth > 2 && n < 0){\
            LAST_SKIP_BITS(name, gb, nb_bits)\
            UPDATE_CACHE(name, gb)\
\
            nb_bits = -n;\
\
            index= SHOW_UBITS(name, gb, nb_bits) + code;\
            code = table[index][0];\
            n    = table[index][1];\
        }\
    }\
    SKIP_BITS(name, gb, n)\
}



#define GET_RL_VLC(level, run, name, gb, table, bits, max_depth, need_update)\
{\
    int n, nb_bits;\
    unsigned int index;\
\
    index= SHOW_UBITS(name, gb, bits);\
    level = table[index].level;\
    n     = table[index].len;\
\
    if(max_depth > 1 && n < 0){\
        SKIP_BITS(name, gb, bits)\
        if(need_update){\
            UPDATE_CACHE(name, gb)\
        }\
\
        nb_bits = -n;\
\
        index= SHOW_UBITS(name, gb, nb_bits) + level;\
        level = table[index].level;\
        n     = table[index].len;\
    }\
    run= table[index].run;\
    SKIP_BITS(name, gb, n)\
}


static inline int get_vlc2(GetBitContext *s, VLC_TYPE (*table)[2], int bits, int max_depth)
{
    int code;

    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)

    GET_VLC(code, re, s, table, bits, max_depth)

    CLOSE_READER(re, s)
    return code;
}



#ifndef bswap_32
static inline const uint32_t bswap_32(uint32_t x)
{
    x= ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
    x= (x>>16) | (x<<16);
    return x;
}
#endif



#if HAVE_BIGENDIAN         
#define be2me_32(x) (x)
#else
#define be2me_32(x) bswap_32(x)
#endif



static inline void init_put_bits(PutBitContext *s, uint8_t *buffer, int buffer_size)
{
    if(buffer_size < 0) {
        buffer_size = 0;
        buffer = NULL;
    }

    s->size_in_bits= 8*buffer_size;
    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;

    s->buf_ptr = s->buf;
    s->bit_left=32;
    s->bit_buf=0;

}
 

static inline int put_bits_count(PutBitContext *s)
{
    return (s->buf_ptr - s->buf) * 8 + 32 - s->bit_left;
}


static inline void put_bits(PutBitContext *s, int n, unsigned int value)
{
    unsigned int bit_buf;
    int bit_left;

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;


    if (n < bit_left) {
        bit_buf = (bit_buf<<n) | value;
        bit_left-=n;
    } else {
        bit_buf<<=bit_left;
        bit_buf |= value >> (n - bit_left); 

        if (3 & (intptr_t) s->buf_ptr) {
			
            s->buf_ptr[0] = bit_buf >> 24;
            s->buf_ptr[1] = bit_buf >> 16;
            s->buf_ptr[2] = bit_buf >>  8;
            s->buf_ptr[3] = bit_buf      ;

        } else

        *(uint32_t *)s->buf_ptr = be2me_32(bit_buf);
		
        //printf("bitbuf = %08x\n", bit_buf);
        s->buf_ptr+=4;
        bit_left+=32 - n;
        bit_buf = value;
    }


    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}



int ps_a,ps_b;


	void mpeg2_slice(int a, int  b, uint8_t *ptt)    
{ 
   unsigned short  tmp;
   int p_s_A =a;
   int p_s_B =b;
  
   if( a == 0)
   {
     init_put_bits(&pb, ptt,    10240);   
     gb_cp.index = 0;
   }
   else
   {
         while(   (  p_s_A  -  gb_cp.index  )  >= 16)
		 tmp = get_bits(&gb_cp, 16);

	  if(  (p_s_A  -  gb_cp.index )  >  0)	
	  tmp = 	 get_bits(&gb_cp, p_s_A  -  gb_cp.index );
           

   }   	


   
   while(  (p_s_B - p_s_A )  >=  16)
    	{
        tmp = get_bits(&gb_cp, 16);
        put_bits(&pb, 16, tmp);     
	  p_s_A  += 16;		
   	}


   if(  (p_s_B - p_s_A )  >  0)
     {

        tmp = get_bits(&gb_cp, p_s_B - p_s_A);
        put_bits(&pb, p_s_B - p_s_A , tmp);     

	  		

    }




}

         

/*JF 1024****************************************************************************/



/*JF 1024****************************************************************************/

//JF 1108    int  mb_skiped  =0;
//JF 1031    int pict_type;
    int ext_type;	

    int CMV_Diff_X;
    int CMV_Diff_Y;


     int   mpeg_f_code_0_0 = 0;;
     int   mpeg_f_code_0_1 = 0;
	
     int   c_m_v = 0;	
     int   intra_vlc_format = 0;
    
     int   picture_structure = 0;
     int   frame_pred_frame_dct = 0;	

     int  q_scale_type = 0;	 

//JF 1108    int interlaced_dct;   

//JF 1108    static int extra_cnt =0;
//JF 1108    int qscale;     

    int mb_x,mb_y;

    static int mb_skip_run = 0 ;


//JF 1108    int resync_mb_x,resync_mb_y;

    u16  mb_width;
    u16  mb_height;



//JF 1110      char mv_S[60];
    char mv_S[45];
    int need_modify_mv=0;	


//JF 1024    unsigned char *data_2;
    const uint8_t *buf_end_2;
    const uint8_t *buf_ptr_2;
    const uint8_t *after_PIC_SC;
    const uint8_t *after_Slice_SC;
    int input_size_2;
    int len2;	


    int  dist = 0 ;



void init_rl_2(RLTable *rl, u8 static_store[2][2*MAX_RUN + MAX_LEVEL + 3])
{
    i8 max_level[MAX_RUN+1], max_run[MAX_LEVEL+1];
    u8 index_run[MAX_RUN+1];
    int last, run, level, start, end, i;

    /* If table is static, we can quit if rl->max_level[0] is not NULL */
    if(static_store && rl->max_level[0])
        return;

    /* compute max_level[], max_run[] and index_run[] */
    for(last=0;last<2;last++) {
        if (last == 0) {
            start = 0;
            end = rl->last;
        } else {
            start = rl->last;
            end = rl->n;
        }

        memset(max_level, 0, MAX_RUN + 1);
        memset(max_run, 0, MAX_LEVEL + 1);
        memset(index_run, rl->n, MAX_RUN + 1);
        for(i=start;i<end;i++) {
            run = rl->table_run[i];
            level = rl->table_level[i];
            if (index_run[run] == rl->n)
                index_run[run] = i;
            if (level > max_level[run])
                max_level[run] = level;
            if (run > max_run[level])
                max_run[level] = run;
        }

            rl->max_level[last] = static_store[last];

		
        memcpy(rl->max_level[last], max_level, MAX_RUN + 1);
		

            rl->max_run[last] = static_store[last] + MAX_RUN + 1;

		
        memcpy(rl->max_run[last], max_run, MAX_LEVEL + 1);
		

            rl->index_run[last] = static_store[last] + MAX_RUN + MAX_LEVEL + 2;
		
        memcpy(rl->index_run[last], index_run, MAX_RUN + 1);
		
    }
}






void  mpeg12_init_vlcs(void)
{
    static int done = 0;

    if (!done) {
        done = 1;


        INIT_VLC_STATIC(&dc_lum_vlc, DC_VLC_BITS, 12,mpeg12_vlc_dc_lum_bits, 1, 1,mpeg12_vlc_dc_lum_code, 2, 2, 512);
        INIT_VLC_STATIC(&dc_chroma_vlc,  DC_VLC_BITS, 12,mpeg12_vlc_dc_chroma_bits, 1, 1,mpeg12_vlc_dc_chroma_code, 2, 2, 514);
        INIT_VLC_STATIC(&mbincr_vlc, MBINCR_VLC_BITS, 36,&mpeg12_mbAddrIncrTable[0][1], 2, 1,&mpeg12_mbAddrIncrTable[0][0], 2, 1, 538);
        INIT_VLC_STATIC(&mb_pat_vlc, MB_PAT_VLC_BITS, 64,&mpeg12_mbPatTable[0][1], 2, 1,&mpeg12_mbPatTable[0][0], 2, 1, 512);
        INIT_VLC_STATIC(&mb_ptype_vlc, MB_PTYPE_VLC_BITS, 7,&table_mb_ptype[0][1], 2, 1, &table_mb_ptype[0][0], 2, 1, 64);
        INIT_VLC_STATIC(&mv_vlc, MV_VLC_BITS, 17,&mpeg12_mbMotionVectorTable[0][1], 2, 1,&mpeg12_mbMotionVectorTable[0][0], 2, 1, 518);

        init_rl_2(&rl_mpeg1, ff_mpeg12_static_rl_table_store[0]);
        init_rl_2(&rl_mpeg2, ff_mpeg12_static_rl_table_store[1]);

        INIT_2D_VLC_RL(rl_mpeg1, 680);
        INIT_2D_VLC_RL(rl_mpeg2, 674);

    }


}

static int find_start_code(const uint8_t **pbuf_ptr, const uint8_t *buf_end)
{
    const uint8_t *buf_ptr= *pbuf_ptr;

    buf_ptr++;               
    buf_end -= 2;          

    while (buf_ptr < buf_end) {
        if(*buf_ptr==0){
            while(buf_ptr < buf_end && buf_ptr[1]==0)
                buf_ptr++;

            if(buf_ptr[-1] == 0 && buf_ptr[1] == 1){
                *pbuf_ptr = buf_ptr+3;
                return buf_ptr[2] + 0x100;
            }
        }
        buf_ptr += 2;
    }
    buf_end += 2; 

    *pbuf_ptr = buf_end;
    return -1;
}


#define INT_BIT 32



static void encode_motion( int val, int f_or_b_code)
{
    if (val == 0) {
        /* zero vector */
        put_bits(&pb,mpeg12_mbMotionVectorTable[0][1], mpeg12_mbMotionVectorTable[0][0]);
    } else {
        int code, sign, bits;
        int bit_size = f_or_b_code - 1;
        int range = 1 << bit_size;
        /* modulo encoding */
        int l= INT_BIT - 5 - bit_size;
        val= (val<<l)>>l;

        if (val >= 0) {
            val--;
            code = (val >> bit_size) + 1;
            bits = val & (range - 1);
            sign = 0;
        } else {
            val = -val;
            val--;
            code = (val >> bit_size) + 1;
            bits = val & (range - 1);
            sign = 1;
        }

        put_bits(&pb,mpeg12_mbMotionVectorTable[code][1],mpeg12_mbMotionVectorTable[code][0]);

        put_bits(&pb, 1, sign);
        if (bit_size > 0) {
            put_bits(&pb, bit_size, bits);
        }
    }
	
}


static int mpeg_decode_motion_vector( int fcode, int pred)
{
    int code, sign, val, l, shift;

    code = get_vlc2(&gb, mv_vlc.table, MV_VLC_BITS, 2); 

    if (code == 0) {
        return pred;
    }
    if (code < 0) {
        return 0xffff;
    }

    sign = get_bits1(&gb);
    shift = fcode - 1;
    val = code;


    if (shift) {
        val = (val - 1) << shift;
        val |= get_bits(&gb, shift);
        val++;
    }

    if (sign)
        val = -val;

    val += pred;

      l= 32 - 5 - shift;
      val = (val<<l)>>l;

    return val;
}


#if 0 //JF 1108
static inline int get_qscale(void)    
{
    int qscale = get_bits(&gb, 5);
    if (q_scale_type) {    
        return non_linear_qscale[qscale];
    } else {
        return qscale << 1;
    }
}
#endif

static inline int decode_dc(GetBitContext *gb, int component)
{
    int code, diff;

    if (component == 0) {
        code = get_vlc2(gb, dc_lum_vlc.table, DC_VLC_BITS, 2);
    } else {
        code = get_vlc2(gb, dc_chroma_vlc.table, DC_VLC_BITS, 2);
    }
    if (code < 0){
//JF 1108        fprintf(stderr, " [decode_dc] invalid dc code at\n");
        return 0xffff;
    }

#if 1  //JF 1108
    if (code == 0) {
        return 0;
    } else {
        return get_xbits(gb, code);
    }

#else
    if (code == 0) {
        diff = 0;
    } else {
        diff = get_xbits(gb, code);
    }
    return diff;
#endif

}	



#define ESCAPE_LEVEL    127

#if 1 //JF 1108
static inline void mpeg2_decode_block_non_intra(void) 
{

    int   level , run;   
    RLTable *rl = &rl_mpeg1; 


    { 

        int v;
        OPEN_READER(re, &gb);


        /* special case for the first coef. no need to add a second vlc table */
        UPDATE_CACHE(re, &gb);
        v= SHOW_UBITS(re, &gb, 2);                                                                
		
        if (v & 2) {
            LAST_SKIP_BITS(re, &gb, 2);
        }

	  
        for(;;) {
            UPDATE_CACHE(re, &gb);

            GET_RL_VLC(level, run, re, &gb, rl->rl_vlc[0], TEX_VLC_BITS, 2,0);
            
            if(level == ESCAPE_LEVEL){
                break;
            } else if(level != 0) {
		   
                LAST_SKIP_BITS(re, &gb, 1);
            } else {

		   LAST_SKIP_BITS(re, &gb, 6);
                UPDATE_CACHE(re, &gb);
 		   SKIP_BITS(re, &gb, 12);

            }
			
        }
        CLOSE_READER(re, &gb);
    }

}

#else

static inline int mpeg2_decode_block_non_intra(DCTELEM *block,int n) 
{
    int level, i, j, run; 
    RLTable *rl = &rl_mpeg1; 

    int mismatch; 

    mismatch = 1; 

    { 

        int v;
        OPEN_READER(re, &gb);
        i = -1;        

        /* special case for the first coef. no need to add a second vlc table */
        UPDATE_CACHE(re, &gb);
        v= SHOW_UBITS(re, &gb, 2);                                                                
		
        if (v & 2) {
            LAST_SKIP_BITS(re, &gb, 2);
            i++;
        }

	  
        for(;;) {
            UPDATE_CACHE(re, &gb);

            GET_RL_VLC(level, run, re, &gb, rl->rl_vlc[0], TEX_VLC_BITS, 2,0);
            
            if(level == ESCAPE_LEVEL){
                break;
            } else if(level != 0) {
                i += run;

		   level = SHOW_SBITS(re, &gb, 1) ;  
		   level = SHOW_SBITS(re, &gb, 1);
		   
                LAST_SKIP_BITS(re, &gb, 1);
            } else {

                run = SHOW_UBITS(re, &gb, 6)+1;
		   LAST_SKIP_BITS(re, &gb, 6);
                UPDATE_CACHE(re, &gb);
                level = SHOW_SBITS(re, &gb, 12); 
		   SKIP_BITS(re, &gb, 12);

                i += run;
            }
            if (i > 63){

		   fprintf(stderr, " ¡inon_intra¡jac-tex damaged at %d %d\n", mb_x, mb_y);	
                       return -1;
            }

			
        }
        CLOSE_READER(re, &gb);
    }

    return 0;
}

#endif


#if 1 //JF 1108
static inline void mpeg2_decode_block_intra(int n)
{
    int level,run;    
    int component;
	
    RLTable *rl;

    if (n < 4){
        component = 0;
    }else{
        component = (n&1) + 1;
    }
	
   if ( decode_dc(&gb, component) >=  0xffff)
{
fprintf(stderr,"[decode_block_intra] diff >= 0xffff  !\n ");
return -1;
}	

      if (intra_vlc_format)
        rl = &rl_mpeg2;
      else
        rl = &rl_mpeg1;

	
    {
        OPEN_READER(re, &gb);

        for(;;) {
            UPDATE_CACHE(re, &gb);
			
            GET_RL_VLC(level, run, re, &gb, rl->rl_vlc[0], TEX_VLC_BITS, 2, 0);

            if(level == 127){
                break;
            } else if(level != 0) {

           LAST_SKIP_BITS(re, &gb, 1);

            } else {

                LAST_SKIP_BITS(re, &gb, 6);
                UPDATE_CACHE(re, &gb);

		   SKIP_BITS(re, &gb, 12);

            }

        }
        CLOSE_READER(re, &gb);
    }


}

#else
static inline int mpeg2_decode_block_intra(DCTELEM *block,int n)
{
    int level, dc, diff, i, j, run;
    int component;
	
    RLTable *rl;
    const int db_qscale= qscale;
    int mismatch;

    if (n < 4){
        component = 0;
    }else{
        component = (n&1) + 1;
    }
	
    diff = decode_dc(&gb, component);   


    if (diff >= 0xffff)
{
fprintf(stderr,"[decode_block_intra] diff >= 0xffff  !\n ");
return -1;
}	


    i = 0;

      if (intra_vlc_format)
        rl = &rl_mpeg2;
      else
        rl = &rl_mpeg1;

	
    {
        OPEN_READER(re, &gb);

        for(;;) {
            UPDATE_CACHE(re, &gb);
			
            GET_RL_VLC(level, run, re, &gb, rl->rl_vlc[0], TEX_VLC_BITS, 2, 0);

            if(level == 127){
                break;
            } else if(level != 0) {

	     SHOW_SBITS(re, &gb, 1) ;
	     SHOW_SBITS(re, &gb, 1) ; 
           LAST_SKIP_BITS(re, &gb, 1);
            i += run;	
            } else {
                run = SHOW_UBITS(re, &gb, 6) + 1; 
                LAST_SKIP_BITS(re, &gb, 6);
                UPDATE_CACHE(re, &gb);
                level = SHOW_SBITS(re, &gb, 12); 
		   SKIP_BITS(re, &gb, 12);
                i += run;
            }
            if (i > 63){
                fprintf(stderr,"\n ¡iintra¡jac-tex damaged at X=%d Y=%d n=%d \n", mb_x, mb_y,n); 
                return -1;    
            }

        }
        CLOSE_READER(re, &gb);
    }






    return 0;
}

#endif

static int mpeg_decode_mb_P( DCTELEM block[6][64])
{
    int val;
    int motion_type;
    int  i,mb_type;
    const int mb_block_count = 6;


    if (mb_skip_run--         != 0) {     
		
//JF1031        if( pict_type ==  1 ){             
             if( picture_coding_type == 1){
            printf("\n [decode_mb] skiped MB in I frame at %d %d\n", mb_x, mb_y);
            return -1;
        }

//JF 1108   mb_skiped = 1;  

       last_mv_00 =0;
       last_mv_01 =0;
       last_mv_10 =0;
       last_mv_11 =0;
//JF 1108       mv_S[mb_x] = 'S';         
  
         return 0;
		
    } 		


        mb_type = get_vlc2(&gb, mb_ptype_vlc.table, MB_PTYPE_VLC_BITS, 1);

        if (mb_type < 0){

            fprintf(stderr," [decode_mb_P] invalid mb type in P Frame at %d %d\n", mb_x, mb_y);
            return -1;
        }

        mb_type = ptype2mb_type[ mb_type ];  

    if (IS_INTRA(mb_type)) {

        if ( (picture_structure == 3 )  && ( !frame_pred_frame_dct)) 
        {
//JF 1108            interlaced_dct = get_bits1(&gb);
              skip_bits(&gb,1);   
        }

        if (IS_QUANT(mb_type))
{
//JF 1108             qscale = get_qscale();
              skip_bits(&gb, 5);	  
}

         if (c_m_v) {

           ps_b   =  gb.index;
           mpeg2_slice(ps_a,ps_b, buf_ptr_2);
           need_modify_mv  = 1 ; 

          if (picture_structure  != 3)   
               skip_bits(&gb, 1); 
              
 	      last_mv_10 = last_mv_00 = mpeg_decode_motion_vector(mpeg_f_code_0_0, last_mv_00);  
            last_mv_11 = last_mv_01 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_01)  ;  		
            mv_S[mb_x] = 'C';  
            skip_bits(&gb, 1); 
           ps_a   =  gb.index;
        }
	  else
	 {  
         last_mv_10 = last_mv_11 = last_mv_00 = last_mv_01 = 0;
//JF 1108         mv_S[mb_x] = 'R';  
	 }


                for(i=0;i<mb_block_count;i++) {
					
//JF 1108                    if (mpeg2_decode_block_intra(&block[i][0], i) < 0)
//JF 1108                        return -1;
                      mpeg2_decode_block_intra(i);
                }
 
    }



else{
	
        if (mb_type & MB_TYPE_ZERO_MV){


            if ( (picture_structure == 3 )  && ( !frame_pred_frame_dct)) 
            {
//JF 1108            interlaced_dct = get_bits1(&gb);
	         skip_bits(&gb,1);
           }				
			
           if (IS_QUANT(mb_type))
//JF 1108             qscale = get_qscale();   
               skip_bits(&gb, 5);	            

	     if( picture_structure != 3)
                  mb_type |= MB_TYPE_INTERLACED;
                  last_mv_10 = last_mv_11 = last_mv_00 = last_mv_01 = 0;
//JF 1108                  mv_S[mb_x] = '0';  
        }
         else
         {
       int    p_or_f_after_c = 0;  

            if (frame_pred_frame_dct)
                motion_type = MT_FRAME;
            else{
                motion_type = get_bits(&gb, 2);
            }

            if ( (picture_structure == 3)  &&  ( !frame_pred_frame_dct)  && HAS_CBP(mb_type) )
		{
//JF 1108                interlaced_dct = get_bits1(&gb);
		   skip_bits(&gb,1);
            }
	
           if (IS_QUANT(mb_type))
//JF 1108             qscale = get_qscale(); 
             skip_bits(&gb, 5);             

      ps_b   =  gb.index;
        switch(motion_type) 
         {
         case MT_FRAME:

        mv_S[mb_x] = 'P';  

	 if (  (  mb_x  !=  0  )    &&     (  mv_S[mb_x-1] == 'C' )	) 
	   {
               need_modify_mv  = 1 ; 
              mpeg2_slice(ps_a,ps_b, buf_ptr_2); 
               p_or_f_after_c = 1; 
		  	   
	   }
	  else
	  {
	      p_or_f_after_c = 0;		 
	  }



                        if (picture_structure == 3) {
 
                            mb_type |= MB_TYPE_16x16; 
                           
                            last_mv_10 = last_mv_00= mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_00);
                            last_mv_11 = last_mv_01= mpeg_decode_motion_vector(mpeg_f_code_0_1,last_mv_01);	

                       if(p_or_f_after_c)
				{
				 encode_motion(last_mv_00,mpeg_f_code_0_0); 	
				 encode_motion(last_mv_01,mpeg_f_code_0_1);
                       	}		   
							
                        } else {

							
                            mb_type |= MB_TYPE_16x8 | MB_TYPE_INTERLACED;

#if 1  //JF 1109 2011
                              val = get_bits1(&gb);
				    last_mv_00 =  mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_00);				  
				    last_mv_01 =  mpeg_decode_motion_vector(mpeg_f_code_0_1,last_mv_01);	

                              int val2 = get_bits1(&gb);
				    last_mv_10 =  mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_10);	
				    last_mv_11 =  mpeg_decode_motion_vector(mpeg_f_code_0_1,last_mv_11);						
					
                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_00,mpeg_f_code_0_0); 	
				      encode_motion(last_mv_01,mpeg_f_code_0_1);

				      put_bits(&pb,1,val2);
				      encode_motion(last_mv_10,mpeg_f_code_0_0); 	
				      encode_motion(last_mv_11,mpeg_f_code_0_1);
					  
                       	     }		   
#else
                              val = get_bits1(&gb);
				    last_mv_00 = mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_00);				  
				    last_mv_01 = mpeg_decode_motion_vector(mpeg_f_code_0_1,last_mv_01);	
                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_00,mpeg_f_code_0_0); 	
				      encode_motion(last_mv_01,mpeg_f_code_0_1);
                       	     }		   
                              val = get_bits1(&gb);
				    last_mv_10 =  mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_10);	
				    last_mv_11 =   mpeg_decode_motion_vector(mpeg_f_code_0_1,last_mv_11);		
                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_10,mpeg_f_code_0_0); 	
				      encode_motion(last_mv_11,mpeg_f_code_0_1);
                       	     }		   
#endif

                        }
                            if(p_or_f_after_c)
                               ps_a  =  gb.index;	 

                        break;
                    case MT_FIELD:
        mv_S[mb_x] = 'F';  
	 if (  (  mb_x  !=  0  )    &&     (  mv_S[mb_x-1] == 'C' )	)  
	   {
               need_modify_mv  = 1 ;  
             mpeg2_slice(ps_a,ps_b, buf_ptr_2);  
              p_or_f_after_c = 1; 	
	   }
	  else
	  {
	      p_or_f_after_c = 0;		 
	  }
                        if (picture_structure == 3) {
                            mb_type |= MB_TYPE_16x8 | MB_TYPE_INTERLACED;

#if 1   //JF 1109 2011
                             val = get_bits1(&gb);
				   last_mv_00 = mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_00);
                             mv_01 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_01 >> 1);	  
				   last_mv_01 =  mv_01	<<  1;		

                             int val2 = get_bits1(&gb);
				   last_mv_10 = mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_10);
                             mv_11 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_11 >> 1);	  
  				   last_mv_11 =  mv_11	<<  1;	

                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_00,mpeg_f_code_0_0); 	   
				      encode_motion(mv_01 ,mpeg_f_code_0_1); 

				      put_bits(&pb,1,val2);
				      encode_motion(last_mv_10  ,mpeg_f_code_0_0); 	
				      encode_motion(mv_11  ,mpeg_f_code_0_1);
				  
                       	     }		   

#else
                             val = get_bits1(&gb);
				   last_mv_00 = mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_00);
                             mv_01 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_01 >> 1);	  
				   last_mv_01 =  mv_01	<<  1;		

                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_00,mpeg_f_code_0_0); 	   
				      encode_motion(mv_01 ,mpeg_f_code_0_1); 
                       	     }		   

                             val = get_bits1(&gb);
				   last_mv_10 = mpeg_decode_motion_vector(mpeg_f_code_0_0,last_mv_10);
                             mv_11 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_11 >> 1);	  
  				   last_mv_11 =  mv_11	<<  1;		

                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_10  ,mpeg_f_code_0_0); 	
				      encode_motion(mv_11  ,mpeg_f_code_0_1);
				  
                       	     }		   
	 
#endif
							
                        } else {
                            mb_type |= MB_TYPE_16x16 | MB_TYPE_INTERLACED;
				  val = get_bits1(&gb);
				  last_mv_10 = last_mv_00 = mpeg_decode_motion_vector(mpeg_f_code_0_0, last_mv_00);	
                           last_mv_11 = last_mv_01 = mpeg_decode_motion_vector(mpeg_f_code_0_1, last_mv_01);	
                            if(p_or_f_after_c)
				     {
				      put_bits(&pb,1,val);
				      encode_motion(last_mv_00,mpeg_f_code_0_0); 	
				      encode_motion(last_mv_01,mpeg_f_code_0_1);
                       	     }		   
			  
                        }

                            if(p_or_f_after_c)
                               ps_a  =  gb.index;	 


                        break;
#if 0  //JF 1110
                    case MT_DMV:

               fprintf(stderr,"[decode_mb_P] ......D\n");

               return -1;
#endif                     
                    default:
                        printf("\n [decode_mb_P] 00 motion_type at %d %d\n", mb_x, mb_y);
                        return -1;
                    }

     }

        if (HAS_CBP(mb_type)) {

           int cbp = get_vlc2(&gb, mb_pat_vlc.table, MB_PAT_VLC_BITS, 1);

	      if ( (cbp < 0) || (cbp == 0 ) ){		 

                printf(" [decode_mb_P] invalid cbp at %d %d\n", mb_x, mb_y);
                return -1;
            }

                {
                    cbp <<= 6 ;   

			for(i=0;i < 6 ;i++) {			
                        if ( cbp & (1<<11) ) {
//JF 1108				   if (  mpeg2_decode_block_non_intra(&block[i][0], i)   < 0 )
//JF 1108                                return -1;
				   mpeg2_decode_block_non_intra();
                        } else {
                            //block_last_index[i] = -1;
                        }
                        cbp+=cbp;
                    }
                }
			
        }else{
            // for(i=0;i<6;i++) 
            // block_last_index[i] = -1; 
        }
  }	 


    return 0;
}



static int mpeg_decode_slice_Pic(int mb_y_index, const uint8_t *buf, int buf_size)
{
    int ret;
    
     if (mb_y_index  >= mb_height){
	      printf("\n [ [ D_S_P ] ]: slice below image (%d >= %d) \n", mb_y_index, mb_height);		
        return -1;
     }	

   {
       int buffer_size= ( (buf_size*8)+7)>>3; 
       if(buffer_size < 0 || (buf_size*8) < 0) {
           buffer_size = buf_size = 0;
           buf = NULL;
       }

       gb.buffer= buf;
       gb.size_in_bits= (buf_size*8);
       gb.buffer_end= buf + buffer_size;
       gb.index=0;
	   

       gb_cp.buffer= buf;
       gb_cp.size_in_bits= (buf_size*8);
       gb_cp.buffer_end= buf + buffer_size;
       gb_cp.index=0;
       ps_a =0;
   }

       last_mv_00 =0;  
       last_mv_01 =0;  
       last_mv_10 =0;  
       last_mv_11 =0;  

//JF 1108      qscale = get_qscale();
      skip_bits(&gb, 5);

#if 0   //JF 1108
    if(qscale == 0){
        fprintf(stderr, " [ [ D_S_P ] ] : qscale == 0 !!! \n");
        return -1;
    }
#endif

//JF 1108    extra_cnt++;	
//JF 1108     int cnt = 0; 	
    while (get_bits1(&gb) != 0 ) {
//JF 1108         fprintf(stderr, "[D_S_P] [%d](%d)!!!  extra_bit_slice not  '0' ..SKIP 8 \n",extra_cnt,++cnt);	 	
        skip_bits(&gb, 8);
    }
    mb_x=0;

        for(;;) {
            int code = get_vlc2(&gb, mbincr_vlc.table, MBINCR_VLC_BITS, 2); 
            if (code < 0){
		    fprintf(stderr,"\n[ [D_S_P : 1 ] ]Error: first mb_incr damaged\n");		
                return -1;
            }

			
            if (code >= 33) {
                if (code == 33) {
                    mb_x += 33;
                }
            } else {
                mb_x += code;
                break;
            }
        }

    if( mb_x >= (unsigned) mb_width){
        fprintf(stderr,"\n[ [ D_S_P ] ]Error: initial skip overflow\n");
        return -1;
    }


//JF 1108     resync_mb_x= mb_x;
//JF 1108     resync_mb_y= mb_y = mb_y_index;
    mb_y = mb_y_index;

    mb_skip_run =  0;

//JF 1108    if(mb_skiped)     mb_skiped = 0;
 
    for(;;) {

//JF 1108           block=  &blocks[0][0];
//JF 1108	    memset(block, 0, 6 * 64 *  sizeof(DCTELEM)); 
		
        if(mpeg_decode_mb_P(block) < 0)
	     // Free DCT buffer		
            return -1;

        if (++mb_x >= mb_width) {
            mb_x = 0;
		mb_y++;
            if(mb_y >= mb_height){
                 goto eos;  
            }

        }

        if (mb_skip_run == -1) {
            mb_skip_run = 0;
            for(;;) {
                int code = get_vlc2(&gb, mbincr_vlc.table, MBINCR_VLC_BITS, 2);
                if (code < 0){
                    fprintf(stderr,"[ [ D_S_P ] ]Error : mb incr damaged\n");
                    return -1;
                }
                if (code >= 33) {
                    if (code == 33) {
                        mb_skip_run += 33; 
                    }else if(code == 35){  
                        if(mb_skip_run != 0 || show_bits(&gb, 15) != 0){
                            fprintf(stderr,"[ [ D_S_P : 2 ] ] slice mismatch\n");
                            return -1;
                        }
                        goto eos;
                    }
                 } else {   
                    mb_skip_run += code;
                    break;
                }
            }

        }


	  
    }

eos: 

//JF 1110   for(int kkk=0; kkk < 60 ; kkk++)
    for(int kkk=0; kkk < 45 ; kkk++) 
   	 mv_S[kkk] = 'X';

   buf  += gb.index/8 - 1;

    return 0; 
}


//JF 1024
void cmv_workaround_uninit(void)
{
  if ( first_entry == 0)
  	{
          free(data_2);

	    //JF 1031   for slave idle mode 
          first_entry = 1;
		
          printf("\ncmv_workaround_uninit !!!\n");
  	}	  
}

/*JF 1024****************************************************************************/
#endif

static inline void print_pts(int qcnt, float *q)
{
	int i;
	printf("qcnt:%d", qcnt);
	for(i = 0; i < qcnt; i++){
		printf(" %f", q[i]);
	}
	printf("\n");
}

static video_codec_t find_video_codec(sh_video_t *sh_video)
{
    demux_stream_t *d_video=sh_video->ds;
    int fmt = d_video->demuxer->file_format;

    if(
        (fmt == DEMUXER_TYPE_PVA) ||
        (fmt == DEMUXER_TYPE_MPEG_ES) ||
        (fmt == DEMUXER_TYPE_MPEG_GXF) ||
        (fmt == DEMUXER_TYPE_MPEG_PES) ||
        (
            (fmt == DEMUXER_TYPE_MPEG_PS || fmt == DEMUXER_TYPE_MPEG_TS) &&
            ((! sh_video->format) || (sh_video->format==0x10000001) || (sh_video->format==0x10000002))
        ) ||
        (d_video->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS && ((sh_video->format==0x10000001) || (sh_video->format==0x10000002))) ||
        (fmt == DEMUXER_TYPE_MPEG_TY)
#ifdef CONFIG_LIVE555
        || ((fmt == DEMUXER_TYPE_RTP) && demux_is_mpeg_rtp_stream(d_video->demuxer))
#endif
    )
        return VIDEO_MPEG12;
    else if((fmt == DEMUXER_TYPE_MPEG4_ES) ||
            ((fmt == DEMUXER_TYPE_MPEG_TS) && (sh_video->format==0x10000004)) ||
            ((fmt == DEMUXER_TYPE_MPEG_PS) && (sh_video->format==0x10000004)) ||
            ((fmt == DEMUXER_TYPE_SKYMPEG_TS ) && (sh_video->format==0x10000004)) // carlos add for DVB-T MPEG4 video format, 2010-08-18
           )
        return VIDEO_MPEG4;
    else if((fmt == DEMUXER_TYPE_H264_ES) ||
            ((fmt == DEMUXER_TYPE_MPEG_TS) && (sh_video->format==0x10000005)) ||
            ((d_video->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS) && (sh_video->format==0x10000005)) ||
            ((fmt == DEMUXER_TYPE_MPEG_PS) && (sh_video->format==0x10000005))
           )
        return VIDEO_H264;
    else if((fmt == DEMUXER_TYPE_MPEG_PS ||  fmt == DEMUXER_TYPE_MPEG_TS || fmt == DEMUXER_TYPE_SKYMPEG_TS) && // carlos add for DVB-T WVC1 video format, 2010-08-18
            (sh_video->format==mmioFOURCC('W', 'V', 'C', '1')))
        return VIDEO_VC1;
    else if (fmt == DEMUXER_TYPE_ASF && sh_video->bih && sh_video->bih->biCompression == mmioFOURCC('D', 'V', 'R', ' '))
        return VIDEO_MPEG12;
    else
        return VIDEO_OTHER;
}

#ifdef FAST_TSDEMUX	//Barry 2010-11-23 reset global variable
void reset_fast_ts_global_variable(void)
{
    frame_split = 1;
    h264_header_packet = 0;
    ts_avc1_pts_queue_cnt = 0;
    check_fast_ts_startcode = 0;
    check_fast_ts = 0;
    fast_ts_pts_cnt = 0;
    fast_ts_pre_pts = 0.0;
    h264_fast_ts_seek = 0;
    h264_need_merge_more = 0;
    get_h264_iframe = 0;//Polun 2011-10-05  fixed h264 first frame is not I frame
    h264_check_sps = 0;//Polun 2011-11-30
}
#endif

int video_read_properties(sh_video_t *sh_video)
{
    int next_nal = -1;  //SkyMedi_Vincent06032009
    demux_stream_t *d_video=sh_video->ds;
    video_codec_t video_codec = find_video_codec(sh_video);
    hddvd_check = 0;
    pre_picture_coding_type=0;

#ifdef DOLBY_DVD
    pre_picture_coding_type=0;
#endif

//#ifdef _SKY_VDEC_V1	// Raymond 2010/03/30
//printf("SallocVirtualBase=0x%x\n", SallocVirtualBase);
    VideoBuffer[0] = (unsigned char *) SallocVirtualBase;
    VideoBuffer[1] = (unsigned char *) SallocVirtualBase + (1 << 20);
    VideoBuffer[2] = (unsigned char *) SallocVirtualBase + (2 << 20);

//	bidx = 0;
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
    if (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
        videobuffer = VideoBuffer[1];
    else
#endif // end of HW_TS_DEMUX && HWTS_USED_FRAMEBUFFER
        videobuffer = VideoBuffer[0];
//	printf("videobuffer = %08X\n", (unsigned int)videobuffer);

    use_PP_buf = 0;

#if 0	//Barry 2010-11-22
    if (d_video->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS )
    {
#ifdef USE_PFM_ADDR_MODE
        use_PP_buf = 2;
#else
        use_PP_buf = 3;
#endif
    }
    else
    {
#if 0
        if( video_codec != VIDEO_OTHER ||
                d_video->demuxer->file_format == DEMUXER_TYPE_MOV ||
                d_video->demuxer->file_format == DEMUXER_TYPE_AVI 		// Raymond 2009/08/03
          )
        {
            use_PP_buf = 1;
        }
        // Raymond 2009/06/01
        else if (d_video->demuxer->file_format == DEMUXER_TYPE_MATROSKA )
        {
//		if(sh_video->format != mmioFOURCC('a','v','c','1') )
//			use_PP_buf = 1;
        }
#endif
    }
#endif

//printf("use_PP_buf = %d\n", use_PP_buf);

//#endif

// Determine image properties:
    switch(video_codec) {
    case VIDEO_OTHER: {
        if((d_video->demuxer->file_format == DEMUXER_TYPE_ASF) || (d_video->demuxer->file_format == DEMUXER_TYPE_AVI)) {
            // display info:
            // in case no strf chunk has been seen in avi, we have no bitmap header
            if(!sh_video->bih) return 0;
            sh_video->format=sh_video->bih->biCompression;
            sh_video->disp_w=sh_video->bih->biWidth;
            sh_video->disp_h=abs(sh_video->bih->biHeight);
        }
#if 1	//Fuchun 20110915
	if(d_video->demuxer->file_format == DEMUXER_TYPE_MATROSKA && h264_frame_mbs_only == 0)
	{
		sh_video->fps *= 2;
	}
#endif
        break;
    }
    case VIDEO_MPEG4: {
        int pos = 0, vop_cnt=0, units[3];
        videobuf_len=0;
        videobuf_code_len=0;
        mp_msg(MSGT_DECVIDEO,MSGL_V,"Searching for Video Object Start code... ");
        while(1) {
            int i=sync_video_packet(d_video);
            if(i<=0x11F) break; // found it!
            if(!i || !skip_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\n");
        if(!videobuffer) {
            videobuffer=(char*)memalign(8,VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
//	videobuffer=(char*)salloc(VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
            if (videobuffer) memset(videobuffer+VIDEOBUFFER_SIZE, 0, MP_INPUT_BUFFER_PADDING_SIZE);
            else {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_ShMemAllocFail);
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_V,"Searching for Video Object Layer Start code... ");
        while(1) {
            int i=sync_video_packet(d_video);
            mp_msg(MSGT_DECVIDEO,MSGL_V,"M4V: 0x%X\n",i);
            if(i>=0x120 && i<=0x12F) break; // found it!
            if(!i || !read_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
        pos = videobuf_len+4;
        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Can't read Video Object Layer Header\n");
            return 0;
        }
        mp4_header_process_vol(&picture, &(videobuffer[pos]));
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK! FPS SEEMS TO BE %.3f\nSearching for Video Object Plane Start code... ", sh_video->fps);
mp4_init:
        while(1) {
            int i=sync_video_packet(d_video);
            if(i==0x1B6) break; // found it!
            if(!i || !read_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
        pos = videobuf_len+4;
        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Can't read Video Object Plane Header\n");
            return 0;
        }
        mp4_header_process_vop(&picture, &(videobuffer[pos]));
        units[vop_cnt] = picture.timeinc_unit;
        vop_cnt++;
        //mp_msg(MSGT_DECVIDEO,MSGL_V, "TYPE: %d, unit: %d\n", picture.picture_type, picture.timeinc_unit);
        if(!picture.fps) {
            int i, mn, md, mx, diff;
            if(vop_cnt < 3)
                goto mp4_init;

            i=0;
            mn = mx = units[0];
            for(i=0; i<3; i++) {
                if(units[i] < mn)
                    mn = units[i];
                if(units[i] > mx)
                    mx = units[i];
            }
            md = mn;
            for(i=0; i<3; i++) {
                if((units[i] > mn) && (units[i] < mx))
                    md = units[i];
            }
            mp_msg(MSGT_DECVIDEO,MSGL_V, "MIN: %d, mid: %d, max: %d\n", mn, md, mx);
            if(mx - md > md - mn)
                diff = md - mn;
            else
                diff = mx - md;
            if(diff > 0) {
                picture.fps = ((float)picture.timeinc_resolution) / diff;
                mp_msg(MSGT_DECVIDEO,MSGL_V, "FPS seems to be: %f, resolution: %d, delta_units: %d\n", picture.fps, picture.timeinc_resolution, diff);
            }
        }
        if(picture.fps) {
            sh_video->fps=picture.fps;
            sh_video->frametime=1.0/picture.fps;
            mp_msg(MSGT_DECVIDEO,MSGL_INFO, "FPS seems to be: %f\n", picture.fps);
        }
        //Barry 2010-11-18
        sh_video->disp_w=picture.display_picture_width;
        sh_video->disp_h=picture.display_picture_height;

        int len;
        len=sprintf(video_names, "%s,%dx%d,%5.3f\0", "MPEG4",sh_video->disp_w,sh_video->disp_h,sh_video->fps);
        video_names[len] = '\0';

        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\n");
        sh_video->format=0x10000004;
        break;
    }
    case VIDEO_H264: {
        int pos = 0;
        videobuf_len=0;
        videobuf_code_len=0;
//+SkyMedi_Vincent06032009
        if(ts_version == TS_2)//+SkyMedi_Vincent08102009+
            while(1) {
                int i=sync_video_packet(d_video);
                if((i&~0x60) == 0x109 && i == 0x109)
                {
                    next_nal = demux_getc(d_video);
                    if(next_nal == 0x10)
                        break; // found it!
                }
                if(!i || !skip_video_packet(d_video)) {
                    mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                    return 0;
                }
            }
//SkyMedi_Vincent06032009+
        mp_msg(MSGT_DECVIDEO,MSGL_V,"Searching for sequence parameter set... ");
        while(1) {
            int i=sync_video_packet(d_video);
            if((i&~0x60) == 0x107 && i != 0x107) break; // found it!
            if(!i || !skip_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\n");
        if(!videobuffer) {
            videobuffer=(char*)memalign(8,VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
//	videobuffer=(char*)salloc(VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
            if (videobuffer) memset(videobuffer+VIDEOBUFFER_SIZE, 0, MP_INPUT_BUFFER_PADDING_SIZE);
            else {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_ShMemAllocFail);
                return 0;
            }
        }

//+SkyMedi_Vincent06032009
        if(ts_version == TS_2)
        {
            static unsigned char	header[7] = {0x0, 0x0, 0x0, 0x1, 0x9, 0x10, 0x00};
            memcpy(videobuffer,header,7);

            videobuf_len += 7;
        }
//SkyMedi_Vincent06032009+

        pos = videobuf_len+4;
        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Can't read sequence parameter set\n");
            return 0;
        }

        pic_struct_present_flag = 0;	//Fuchun 2010.08.11 reset global variable
        //Polun 2011-10-05 fixed BBC_HD.TS playback black screen.
        #if 0 
        h264_parse_sps(&picture, &(videobuffer[pos]), videobuf_len - pos);
        #else
        unsigned char *tempvideobuffer = malloc(videobuf_len);
        memcpy(tempvideobuffer, videobuffer, videobuf_len);
        h264_parse_sps(&picture, &(tempvideobuffer[pos]), videobuf_len - pos);
        free(tempvideobuffer);
        #endif
        sh_video->aspect = h264_aspect_info(&picture);	//Barry 2010-09-03

        if (!check_h264_sps_bank_size(&picture))
        {
            not_supported_profile_level = 1;
            return 0;
        }
        sh_video->disp_w=picture.display_picture_width;
        sh_video->disp_h=picture.display_picture_height;
        mp_msg(MSGT_DECVIDEO,MSGL_V,"Searching for picture parameter set... ");
        while(1) {
            int i=sync_video_packet(d_video);
            mp_msg(MSGT_DECVIDEO,MSGL_V,"H264: 0x%X\n",i);
            if((i&~0x60) == 0x108 && i != 0x108) break; // found it!
            if(!i || !read_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\nSearching for Slice... ");
#if 1
        if(!read_video_packet(d_video))
        {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Can't read picture parameter set\n");
            return 0;
        }
        while(1)
        {
            int i=sync_video_packet(d_video);
            if((i&~0x60) == 0x101 || (i&~0x60) == 0x102 || (i&~0x60) == 0x105) break;
            else if(pic_struct_present_flag && ((i&~0x60) == 0x106))
            {
                int interlace_flag;
                pos = videobuf_len + 4;
                if(!read_video_packet(d_video))
                {
                    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Can't read SEI\n");
                    return 0;
                }

                if(interlace_flag = h264_parse_sei(&videobuffer[pos], videobuf_len-pos-1))
                {
                	pic_struct_present_flag = 0;
#if 1	//Barry 2011-12-09
                     #if 0  //Polun 2011-12-30 only MBAFF to merage field to frame.
                     if ((!h264_frame_mbs_only) && (picture.fps == 25.0))
                     #else
                     if (mb_adaptive_frame_field_flag && (!h264_frame_mbs_only) && (picture.fps == 25.0))
                     #endif
			{
				h264_frame_mbs_only = 1;
			}
			else
			{
				if(interlace_flag == 2 && picture.fps) picture.fps *= 2;
			}
#else
                    if(interlace_flag == 2 && picture.fps) picture.fps *= 2;
#endif
                }
            }
            else if(!i || !read_video_packet(d_video))
            {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
      //Polun 2011-07-01 ++s
        #if 0
        if((interlace_mbaff == 1) && (picture.display_picture_width == 1920) && (picture.display_picture_height >= 1080)) 
        #else
        if((mb_adaptive_frame_field_flag == 1) && (picture.display_picture_width == 1920) && (picture.display_picture_height >= 1080)) 
        #endif
        {
	      printf("===video is 1920x1080 MBAFF ===\n");
             video_1080_mbaff_flag = 1 ;
        }
        else
        {
             video_1080_mbaff_flag = 0 ;
        }
      //Polun 2011-07-01 ++e
        if(ts_avc1_pts_queue) {
            free(ts_avc1_pts_queue);
            ts_avc1_pts_queue = NULL;
        }
        ts_avc1_pts_queue_cnt = 0;

        if (h264_sps_buf)
        {
            free(h264_sps_buf);
            h264_sps_buf = NULL;
        }
        //Barry 2010-07-02
        ts_avc1_pts_queue = malloc((h264_reorder_num*(2-h264_frame_mbs_only)+1)*sizeof(float));

        if(!ts_avc1_pts_queue)
        {
            printf(">>> ts_avc1_pts_queue alloc fail <<<\n");
            return 0;
        }

        memset(ts_avc1_pts_queue, 0, (h264_reorder_num*(2-h264_frame_mbs_only)+1)*sizeof(float));
#else
        while(1) {
            int i=sync_video_packet(d_video);
            if((i&~0x60) == 0x101 || (i&~0x60) == 0x102 || (i&~0x60) == 0x105) break; // found it!
            if(!i || !read_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                return 0;
            }
        }
#endif
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\n");
        sh_video->format=0x10000005;
        if(picture.fps) {
            sh_video->fps=picture.fps;
            sh_video->frametime=1.0/picture.fps;
            mp_msg(MSGT_DECVIDEO,MSGL_INFO, "FPS seems to be: %f\n", picture.fps);
//20100519 Robert remove this dirty hack
            /*
                 if (d_video->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS)
                 {
            //         sh_video->fps=60.0;
            //         sh_video->frametime=1.0/60.0;
                     sh_video->fps=picture.fps*2;
                     sh_video->frametime=1.0/(picture.fps*2);
                     mp_msg(MSGT_DECVIDEO,MSGL_INFO, "Reset FPS to : %f\n", sh_video->fps);
                 }
            */
        }

        int len;
        len=sprintf(video_names, "%s,%dx%d,%5.3f\0", "H264",sh_video->disp_w,sh_video->disp_h,sh_video->fps);
        video_names[len] = '\0';

        break;
    }
    case VIDEO_MPEG12: {
        if (d_video->demuxer->file_format == DEMUXER_TYPE_ASF) { // DVR-MS
            if(!sh_video->bih) return 0;
            sh_video->format=sh_video->bih->biCompression;
        }
mpeg_header_parser:
        // Find sequence_header first:
        videobuf_len=0;
        videobuf_code_len=0;
        sky_telecine=0;
        sky_repeatfield_cnt = 0;
        sky_repeatfield_flag = 0;
        telecine=0;
        telecine_cnt=-2.5;
        mp_msg(MSGT_DECVIDEO,MSGL_V,"Searching for sequence header... ");
        while(1) {
            int i=sync_video_packet(d_video);
            if(i==0x1B3) break; // found it!
            if(!i || !skip_video_packet(d_video)) {
                if( mp_msg_test(MSGT_DECVIDEO,MSGL_V) )  mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_MpegNoSequHdr);
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_V,"OK!\n");
        // ========= Read & process sequence header & extension ============
        if(!videobuffer) {
            videobuffer=(char*)memalign(8,VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
//	videobuffer=(char*)salloc(VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
            if (videobuffer) memset(videobuffer+VIDEOBUFFER_SIZE, 0, MP_INPUT_BUFFER_PADDING_SIZE);
            else {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_ShMemAllocFail);
                return 0;
            }
        }

        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_CannotReadMpegSequHdr);
            return 0;
        }
        if(mp_header_process_sequence_header (&picture, &videobuffer[4])) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_BadMpegSequHdr);
            goto mpeg_header_parser;
        }
        if(sync_video_packet(d_video)==0x1B5) { // next packet is seq. ext.
            int pos=videobuf_len;
            if(!read_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_CannotReadMpegSequHdrEx);
                return 0;
            }

            //Barry 2010-05-19
            if ( (videobuffer[pos+4] & 0x07) < 4 && (videobuffer[pos+4] & 0x07) > 0 )
            {
                printf("Unsupported High profile\n");
#if 0	//Barry 2010-06-14 disable
                not_supported_profile_level = 1;
                return 0;
#endif
            }

            if(mp_header_process_extension (&picture, &videobuffer[pos+4])) {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_BadMpegSequHdrEx);
                return 0;
            }
        }

        // display info:
        sh_video->format=picture.mpeg1?0x10000001:0x10000002; // mpeg video
        sh_video->fps=picture.fps * picture.frame_rate_extension_n / picture.frame_rate_extension_d;
        if(!sh_video->fps) {
            sh_video->frametime=0;
        } else {
            sh_video->frametime=1.0/picture.fps;
        }
        sh_video->disp_w=picture.display_picture_width;
        sh_video->disp_h=picture.display_picture_height;

#ifdef CMV_WORKAROUND  //JF 1031   for ES
        mb_width  = (picture.display_picture_width  + 15) / 16;
        mb_height = (picture.display_picture_height + 15) / 16;
#endif

        sh_video->aspect = mpeg12_aspect_info(&picture);	//Barry 2010-06-22

        // bitrate:
        if(picture.bitrate!=0x3FFFF) // unspecified/VBR ?
            sh_video->i_bps=picture.bitrate * 400 / 8;
        // info:
        mp_dbg(MSGT_DECVIDEO,MSGL_DBG2,"mpeg bitrate: %d (%X)\n",picture.bitrate,picture.bitrate);
        mp_msg(MSGT_DECVIDEO,MSGL_INFO,"VIDEO:  %s  %dx%d  (aspect %d)  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n",
               picture.mpeg1?"MPEG1":"MPEG2",
               sh_video->disp_w,sh_video->disp_h,
               picture.aspect_ratio_information,
               sh_video->fps,
               sh_video->i_bps * 8 / 1000.0,
               sh_video->i_bps / 1000.0 );
        
        if(picture.mpeg1)
        {
        	int len;
        	len=sprintf(video_names, "%s,%dx%d,%5.3f\0", "MPEG1",sh_video->disp_w,sh_video->disp_h,sh_video->fps);
        	video_names[len] = '\0';
        }
        else
        {
        	int len;
        	len=sprintf(video_names, "%s,%dx%d,%5.3f\0", "MPEG2",sh_video->disp_w,sh_video->disp_h,sh_video->fps);
        	video_names[len] = '\0';
        }
//printf("picture_structure=%d prog:%d  rff:%d prog_frame:%d tff:%d type:%d\n", picture.picture_structure, picture.progressive_sequence, picture.repeat_first_field, picture.progressive_frame, picture.top_field_first, picture.picture_type);

        sky_mpeg12_progressive_sequence = picture.progressive_sequence;

        break;
    }
    case VIDEO_VC1: {
        // Find sequence_header:
        videobuf_len=0;
        videobuf_code_len=0;
        mp_msg(MSGT_DECVIDEO,MSGL_INFO,"Searching for VC1 sequence header... ");
        while(1) {
            int i=sync_video_packet(d_video);
            if(i==0x10F) break; // found it!
            if(!i || !skip_video_packet(d_video)) {
                if( mp_msg_test(MSGT_DECVIDEO,MSGL_V) )  mp_msg(MSGT_DECVIDEO,MSGL_V,"NONE :(\n");
                mp_msg(MSGT_DECVIDEO,MSGL_ERR, "Couldn't find VC-1 sequence header\n");
                return 0;
            }
        }
        mp_msg(MSGT_DECVIDEO,MSGL_INFO,"found\n");
        if(!videobuffer) {
            videobuffer=(char*)memalign(8,VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
//	videobuffer=(char*)salloc(VIDEOBUFFER_SIZE + MP_INPUT_BUFFER_PADDING_SIZE);
            if (videobuffer) memset(videobuffer+VIDEOBUFFER_SIZE, 0, MP_INPUT_BUFFER_PADDING_SIZE);
            else {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_ShMemAllocFail);
                return 0;
            }
        }
        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_ERR, "Couldn't read VC-1 sequence header!\n");
            return 0;
        }

        while(1) {
            int i=sync_video_packet(d_video);
            if(i==0x10E) break; // found it!
            if(!i || !skip_video_packet(d_video)) {
                mp_msg(MSGT_DECVIDEO,MSGL_V,"Couldn't find VC-1 entry point sync-code:(\n");
                return 0;
            }
        }
        if(!read_video_packet(d_video)) {
            mp_msg(MSGT_DECVIDEO,MSGL_V,"Couldn't read VC-1 entry point sync-code:(\n");
            return 0;
        }

        if(mp_vc1_decode_sequence_header(&picture, &videobuffer[4], videobuf_len-4)) {
            sh_video->bih = calloc(1, sizeof(BITMAPINFOHEADER) + videobuf_len);
            if(sh_video->bih == NULL) {
                mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Couldn't alloc %d bytes for VC-1 extradata!\n", sizeof(BITMAPINFOHEADER) + videobuf_len);
                return 0;
            }
            sh_video->bih->biSize= sizeof(BITMAPINFOHEADER) + videobuf_len;
            memcpy(sh_video->bih + 1, videobuffer, videobuf_len);
            sh_video->bih->biCompression = sh_video->format;
            sh_video->bih->biWidth = sh_video->disp_w = picture.display_picture_width;
            sh_video->bih->biHeight = sh_video->disp_h = picture.display_picture_height;
            if(picture.fps > 0) {
                sh_video->frametime=1.0/picture.fps;
                sh_video->fps = picture.fps;
            }
            mp_msg(MSGT_DECVIDEO,MSGL_INFO,"VIDEO:  VC-1  %dx%d, %5.3f fps, header len: %d\n",
                   sh_video->disp_w, sh_video->disp_h, sh_video->fps, videobuf_len);
            int len;
            len=sprintf(video_names, "%s,%dX%d,%5.3f\0", "VC-1",sh_video->disp_w,sh_video->disp_h,sh_video->fps);
            video_names[len] = '\0';
        }
        break;
    }
    } // switch(file_format)

#ifdef FAST_TSDEMUX	// Raymond 2009/12/15
    reset_fast_ts_global_variable();	//Barry 2010-11-23 reset global variable
    if ( d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TS && (video_codec == VIDEO_MPEG12 || video_codec == VIDEO_H264) )	//Barry 2010-05-18
        d_video->buffer_pos = 0;
    else
#endif
        if (d_video->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS)
        {
            d_video->buffer_pos = 0;
#ifdef USE_PFM_ADDR_MODE
            printf("!!!! switch dmx_skyts_fill_data_type to 1\n");
            extern int dmx_skyts_fill_data_type;
            dmx_skyts_fill_data_type = 1;

            use_PP_buf = 2;
//            dvb_pfm_buffer_update(0xffffffff);
#endif
        }

#ifdef FAST_MPEG_PS_DEMUX	//Barry 2010-04-21
    if (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_PS)
    {
#if 1	//Barry 2010-12-13
        double probe_bps = 0.0;
        probe_bps = demuxer_get_time_length(d_video->demuxer);
        if (probe_bps > 0)
            probe_bps = ((double)d_video->demuxer->movi_end - d_video->demuxer->movi_start) / probe_bps;

        if ( (((int)probe_bps) << 3) > 40000000 )	// > 40 Mbps
#else
        if ( (sh_video->i_bps << 3) > 40000000)	// > 40 Mbps
#endif
        {
            FAST_MPEG_PS_FLAG = 1;
            mp_msg(MSGT_DECVIDEO,MSGL_V,"Enable Fast MPEG-PS demux\n");
        }
        if(FAST_MPEG_PS_FLAG)
        {
            d_video->buffer_pos = 0;
            d_video->pts = 0;
        }
    }
#endif

    return 1;
}

static void process_userdata(unsigned char* buf,int len) {
    int i;
    extern int subcc_force_off;
    extern int is_dvdnav;
    extern int dvdnav_globle_num_subtitle;
    extern int has_subcc;
    static int first_get_subcc = 0;
    extern unsigned int skydroid;

    /* if the user data starts with "CC", assume it is a CC info packet */
    if(len>2 && buf[0]=='C' && buf[1]=='C' && buf[2]==0x01 && buf[3]==0xF8) {
//    mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"video.c: process_userdata() detected Closed Captions!\n");
//printf("subcc_enabled[%d],has_subcc[%d],subcc_force_off[%d]\n",subcc_enabled,has_subcc,subcc_force_off);
	has_subcc = 1;
	if(dvdnav_globle_num_subtitle==0 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && subcc_force_off==0)
	{
		if(first_get_subcc <= 3 && skydroid)
		{
			subcc_enabled = 1;
			char callback_str[32];
			extern char subtitle_names[];
			sprintf(subtitle_names, "%s\0","SUBCC 100 cc");
			sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);

			if(first_get_subcc==3)
				ipc_callback(callback_str);
			first_get_subcc++;
		}
	}

        subcc_process_data(buf+2,len-2);
    }
    if( len > 2 && buf[ 0 ] == 'T' && buf[ 1 ] == 'Y' )
    {
#ifndef BUILD_MPLAYER_LITE	//Barry 2010-10-27
        ty_processuserdata( buf + 2, len - 2 );
#endif
        return;
    }
    if(verbose<2) return;
    fprintf(stderr, "user_data: len=%3d  %02X %02X %02X %02X '",
            len, buf[0], buf[1], buf[2], buf[3]);
    for(i=0; i<len; i++)
//    if(buf[i]>=32 && buf[i]<127) fputc(buf[i], stderr);
        if(buf[i]&0x60) fputc(buf[i]&0x7F, stderr);
    fprintf(stderr, "'\n");
}

// for dvdnav use
int video_restore_frame(sh_video_t* sh_video, unsigned char** start, unsigned char * data, int in_size)
{
    demux_stream_t *d_video = NULL;
    demuxer_t *demuxer = NULL;
    int nRet = 0;
    unsigned int VideoBufferOffset = 0;
    unsigned char* videobuffertemp = NULL;

    if ((sh_video == NULL) || (data == NULL) || (in_size < 0))
	return nRet; 

    if (sh_video->ds)
	d_video = sh_video->ds;
    if (d_video)
	demuxer = d_video->demuxer;
    else
	return nRet; 
    
#ifdef BITSTREAM_BUFFER_CONTROL
    unsigned char *bs_buf_start = VideoBuffer[0];
    //printf("use VideoBuffer[%d] = %x, VideoBufferIdx %x\n", (bs_buf_start == VideoBuffer[1]) ? 1 : 0, VIRTUAL_TO_UCM(bs_buf_start), VideoBufferIdx);
#endif

    // because eof we don't know previous frame size, skip max frame size
    //VideoBufferOffset = ROUND_UP(VideoBufferIdx + in_size, 8192);
    VideoBufferOffset = VideoBufferIdx;

#ifdef BITSTREAM_BUFFER_CONTROL
    if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferOffset), MAX_FRAME_SIZE)) {
	printf("Frame too large, reset to head\n");
	VideoBufferOffset = 0;
    }
    videobuffertemp = bs_buf_start + VideoBufferOffset;

    wait_buffer(VIRTUAL_TO_UCM(videobuffertemp), MAX_FRAME_SIZE);
#else
    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE-1)<<20))
    {
	VideoBufferIdx = 0;
    }
    videobuffertemp = VideoBuffer[0] + VideoBufferOffset;
#endif
    if (memcpy((void *)videobuffertemp, (void *)data, in_size) != NULL)
    {
	videobuffer = videobuffertemp;
	//VideoBufferIdx = VideoBufferOffset;
	VideoBufferIdx = ROUND_UP(VideoBufferOffset + in_size, 8192);
	*start=videobuffer;
	nRet = 1;
    }

    return nRet;
}

int video_read_frame(sh_video_t* sh_video,float* frame_time_ptr,unsigned char** start,int force_fps) {
    demux_stream_t *d_video=sh_video->ds;
    demuxer_t *demuxer=d_video->demuxer;
    float frame_time=1;
    float pts1=d_video->pts;
    float pts=0.0, pts2=0.0;
    float fps;
    picture_coding_type=0;     //JF 1031 : local to global
    int in_size=0;
    video_codec_t video_codec = find_video_codec(sh_video);
    extern int uvcnt;
    ts_h264_field_num = 0;
    memset(&ts_h264_field_offset[0], 0, 8*sizeof(unsigned int));
    float avc1_next_pts = 0.0;	//Barry 2011-09-07

//	int next_nal = -1;  //SkyMedi_Vincent06032009

    dp_hdr_t *dp_hdr;//rywu20090601
    //rywu20090629++
    rv_pic_header *rv_phdr;
    rv_info *rv_extra_info;
    unsigned int value32;
//	unsigned char *buf;
//	unsigned int buf_len;
//	unsigned int buf_offset;
    //rywu20090629--
    int new_ft=0;
#ifdef STREAM_DIO_AUTO
    extern int dio;
#endif // end of  STREAM_DIO_AUTO

#ifdef BITSTREAM_BUFFER_CONTROL
    unsigned char *bs_buf_start = VideoBuffer[0];
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))
    if (demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
        bs_buf_start = VideoBuffer[1];
#endif
    //printf("use VideoBuffer[%d] = %x, VideoBufferIdx %x\n", (bs_buf_start == VideoBuffer[1]) ? 1 : 0, VIRTUAL_TO_UCM(bs_buf_start), VideoBufferIdx);
#endif

    *start=NULL;
//	extern int hwtsdemux;
#if 0	// Raymond 2009/04/17
//	if( VideoBuffer[0] != NULL )
    {
        videobuffer = VideoBuffer[bidx];
        // Raymond 2009/04/28
        bidx++;
        if(bidx == 3)
            bidx = 0;
    }
#endif

    uvcnt++;
#ifdef HWTS_READ_FRAME	//Barry 2010-11-29
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
    if((demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && quick_bd && hwtsdemux && use_PP_buf != 3)
#else /* else of SUPPORT_QT_BD_ISO_ENHANCE */
    if((demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && hwtsdemux)
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
    {
        int i=0;
        unsigned char *p = NULL;

        if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG TS hwts %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
        demuxer->need_double_copy = 0;
        videobuf_len = 0;

        //Barry 2010-12-15
        if (video_codec == VIDEO_H264)
        {
            if (!h264_fast_ts_check)
            {
                while(1)
                {
                    in_size=ds_get_packet(d_video,start);
                    if(in_size<0) return -1; // EOF
                    /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                    if (*start == NULL)
                    {
                        printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                        return 0;
                    }

                    p = *start;
                    pts = d_video->pts;
                    for (i=0; i<in_size-5; i++)
                    {
                        if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] &0x1F) == 7) )	//SPS
                        {
                            h264_fast_ts_check++;
                            break;
                        }
                    }
                    if (in_size < 64 && !h264_fast_ts_seek && check_h264_pts == pts && (i < (in_size-5)))
                        h264_fast_ts_seek = 1;
                    check_h264_pts = pts;
                    if (i < (in_size-5))
                        break;
                }
            }
            else
            {
                in_size=ds_get_packet(d_video,start);
                if(in_size<0) return -1; // EOF
                /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                if (*start == NULL)
                {
                    printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                    return 0;
                }
            }

            if (h264_fast_ts_check==2 && in_size < 64)
            {
                in_size=ds_get_packet(d_video,start);
                if(in_size<0) return -1; // EOF
                /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                if (*start == NULL)
                {
                    printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                    return 0;
                }
            }
        }
        else
        {
            in_size=ds_get_packet(d_video,start);

            if(in_size<0) return -1; // EOF

            /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
            if (*start == NULL)
            {
                printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                return 0;
            }
        }

#ifdef BITSTREAM_BUFFER_CONTROL
        if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
            //printf("Frame too large, reset to head\n");
            VideoBufferIdx = 0;
        }
        videobuffer = bs_buf_start + VideoBufferIdx;

        wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#else
//Robert 20100712 new VideoBuffer Idx
//next Idx
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
        if (hwtsdemux)
        {
#ifdef STREAM_DIO_AUTO
            if (dio)
            {
                /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                /* fixed for HD-DVD_Shinobi_clip01.ts, 2010-11-24, change reserved from 4.5 MB to 3.5 MB */
                if (ROUND_UP(VideoBufferIdx + in_size, 8192) > (((RESERVED_BUFFER_SIZE + 1 - 1)<<20) - (1 << 19)))
                    VideoBufferIdx = 0;
            }
            else
#endif // end of STREAM_DIO_AUTO			
            {
                if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE - 1)<<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                    VideoBufferIdx = 0;
            }
        }
        else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
        {
            if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE<<20) - (1<<19)))
            {
                VideoBufferIdx = 0;
            }
        }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
        if (hwtsdemux)
            videobuffer = VideoBuffer[1] + VideoBufferIdx;
        else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
            videobuffer = VideoBuffer[0] + VideoBufferIdx;
#endif // BITSTREAM_BUFFER_CONTROL

        pts = d_video->pts;
        videobuf_len += in_size;

        if(video_codec == VIDEO_H264)
        {
            //Barry 2010-12-15
            if (h264_fast_ts_seek)
            {
                if (h264_fast_ts_check < 2)	//find keyframe
                {
                    p = *start;
                    unsigned int bits;
                    extern unsigned int ts_skip_bits;

                    for (i=0; i<in_size; i++)
                    {
                        if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] & ~0x60) == 1) )
                        {
                            ts_skip_bits = 0;
                            h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits);	//first_mb_in_slice
                            h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits);	//sliceType
//							printf("=====> slice type = %d\n", bits);
                            if (bits == 2 || bits == 7)
                            {
                                h264_fast_ts_check++;
                                if (h264_sps_buf)
                                {
                                    memcpy(videobuffer, h264_sps_buf, h264_sps_len);
                                    free(h264_sps_buf);
                                    h264_sps_buf = NULL;
                                }
                                h264_sps_len++;
                                break;
                            }
                            else
                                return 0;
                        }
                    }
                    if (in_size < 64)
                    {
                        if (h264_sps_buf)
                        {
                            free(h264_sps_buf);
                            h264_sps_buf = NULL;
                        }
                        h264_sps_buf = malloc(in_size);
                        h264_sps_len = in_size;
                        memcpy(h264_sps_buf, *start, in_size);
                    }
                }
            }

            if (ts_avc1_pts_queue_cnt == 0)
                ts_avc1_pts_queue[0] = pts;
            else if (ts_avc1_pts_queue_cnt == 1)
            {
                if (pts < ts_avc1_pts_queue[0])
                {
                    ts_avc1_pts_queue[1] = ts_avc1_pts_queue[0];
                    ts_avc1_pts_queue[0] = pts;
                }
                else
                    ts_avc1_pts_queue[1] = pts;
            }
            else
            {
                for (i=0; i<ts_avc1_pts_queue_cnt; i++)
                {
                    if (ts_avc1_pts_queue[i] == 0)
                    {
                        ts_avc1_pts_queue[i] = pts;
                        break;
                    }
                    else if (pts < ts_avc1_pts_queue[i])
                    {
                        memcpy(ts_avc1_pts_queue+(i+1), ts_avc1_pts_queue+i, (ts_avc1_pts_queue_cnt-i)*sizeof(float));
                        ts_avc1_pts_queue[i] = pts;
                        break;
                    }
                }
                if (i == ts_avc1_pts_queue_cnt)
                    ts_avc1_pts_queue[i] = pts;
            }

            //20100728 Robert need keep last_pts for new AV
            if (sh_video->last_pts != MP_NOPTS_VALUE && ts_avc1_pts_queue[0] != MP_NOPTS_VALUE &&
                    sh_video->last_pts != 0.0 && ts_avc1_pts_queue[0] != 0.0)
            {
                new_ft = 1;
            }
        }
        else if(video_codec == VIDEO_MPEG12)
        {
            p = *start;
            for (i=0; i < in_size -3; i++)
            {
                if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0xB3 )
                    mp_header_process_sequence_header (&picture, &p[i+4]);
                else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0xB5 )
                    mp_header_process_extension (&picture, &p[i+4]);
                else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0 )	//Barry 2010-07-02
                    picture_coding_type=(p[i+5] >> 3) & 7;
                else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0x01 )
                    break;
                else
                    continue;

                // get mpeg fps:
                if(sh_video->fps!=picture.fps) if(!force_fps && !telecine)
                    {
                        mp_msg(MSGT_CPLAYER,MSGL_WARN,"Warning! FPS changed %5.3f -> %5.3f  (%f) [%d]  \n",sh_video->fps,picture.fps,sh_video->fps-picture.fps,picture.frame_rate_code);
                        sh_video->fps=picture.fps;
                        sh_video->frametime=1.0/picture.fps;
                    }

                // fix mpeg2 frametime:
                frame_time=(picture.display_time)*0.01f;
                picture.display_time=100;

                telecine_cnt*=0.9; // drift out error
                telecine_cnt+=frame_time-5.0/4.0;
                mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"\r telecine = %3.1f  %5.3f     \n",frame_time,telecine_cnt);

                if(telecine)
                {
                    frame_time=1;
                    if(telecine_cnt<-1.5 || telecine_cnt>1.5)
                    {
                        mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_LeaveTelecineMode);
                        telecine=0;
                    }
                }
                else if(telecine_cnt>-0.5 && telecine_cnt<0.5 && !force_fps)
                {
                    frame_time=1;	//Fuchun 2010.11.05
                    sh_video->fps=sh_video->fps*4/5;
                    sh_video->frametime=sh_video->frametime*5/4;
                    mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_EnterTelecineMode);
                    telecine=1;
                }
            }
        }
        else if(video_codec == VIDEO_VC1)
        {
            //Barry 2010-07-02
            if ( demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS )
            {
                double temp_pts = 0.0;
                ds_get_next_pts(d_video);
                if ( d_video->first && (d_video->pts > d_video->first->pts) )
                {
                    temp_pts = d_video->pts;
                    d_video->pts = d_video->first->pts;
                    d_video->first->pts = temp_pts;
                }
            }

            if (fast_ts_pre_pts == d_video->pts)
            {
                fast_ts_pts_cnt++;
                d_video->pts += (sh_video->frametime * fast_ts_pts_cnt);
            }
            else
            {
                fast_ts_pre_pts = d_video->pts;
                fast_ts_pts_cnt = 0;
            }
//			printf("d_video->pts = %f   sh_video->last_pts = %f\t", d_video->pts, sh_video->last_pts);

            //20100728 Robert need keep last_pts for new AV
            if (sh_video->last_pts != MP_NOPTS_VALUE && d_video->pts != MP_NOPTS_VALUE &&
                    sh_video->last_pts != 0.0 && d_video->pts != 0.0)
            {
                frame_time = d_video->pts - sh_video->last_pts;
                //Barry 2010-08-19 fix frame_time < 0 issue
                if (frame_time < 0)
                    frame_time = 0;
                new_ft = 1;
            }
//			printf("frame_time = %f\n", frame_time);
        }

#ifdef BITSTREAM_BUFFER_CONTROL
        if(videobuf_len > MAX_FRAME_SIZE)
            printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
        //Barry 2010-12-15
        if (h264_sps_len)
        {
            memcpy(videobuffer+h264_sps_len, *start, videobuf_len);
            *start=videobuffer;
            in_size=videobuf_len+h264_sps_len;
            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
            h264_sps_len = 0;
        }
        else
        {
            memcpy(videobuffer, *start, videobuf_len);
            *start=videobuffer;
            in_size=videobuf_len;
            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
        }
    }
    else
//=============================== use_PP_buf == 3
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
        if((demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && quick_bd && hwtsdemux && use_PP_buf == 3)
#else /* else of SUPPORT_QT_BD_ISO_ENHANCE */
        if((demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && hwtsdemux)
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
        {
            int i=0;
            unsigned char *p = NULL;
            int pp;
            unsigned int *lptr;// = *start;
            unsigned int new_addr, new_size;

            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG TS hwts %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
            demuxer->need_double_copy = 0;
            videobuf_len = 0;

            //Barry 2010-12-15
            if (video_codec == VIDEO_H264)
            {
                if (!h264_fast_ts_check)
                {
                    while(1)
                    {
                        in_size=ds_get_packet(d_video,start);
                        if(in_size<0) return -1; // EOF
                        /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                        if (*start == NULL)
                        {
                            printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                            return 0;
                        }

                        p = *start;
                        pts = d_video->pts;
                        for (i=0; i<in_size-5; i++)
                        {
                            if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] &0x1F) == 7) )	//SPS
                            {
                                h264_fast_ts_check++;
                                break;
                            }
                        }
                        if (in_size < 64 && !h264_fast_ts_seek && check_h264_pts == pts && (i < (in_size-5)))
                            h264_fast_ts_seek = 1;
                        check_h264_pts = pts;
                        if (i < (in_size-5))
                            break;
                    }
                }
                else
                {
                    in_size=ds_get_packet(d_video,start);
                    if(in_size<0) return -1; // EOF
                    /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                    if (*start == NULL)
                    {
                        printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                        return 0;
                    }
                }

                if (h264_fast_ts_check==2 && in_size < 64)
                {
                    in_size=ds_get_packet(d_video,start);
                    if(in_size<0) return -1; // EOF
                    /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                    if (*start == NULL)
                    {
                        printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                        return 0;
                    }
                }
            }
            else
            {
                in_size=ds_get_packet(d_video,start);

                if(in_size<0) return -1; // EOF

                /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                if (*start == NULL)
                {
                    printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                    return 0;
                }
            }

            pts = d_video->pts;
            videobuf_len += in_size;

            lptr = *start;
            if(video_codec == VIDEO_H264)
            {
                //Barry 2010-12-15
                if (lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] != 0xff008860)
                {
#ifdef BITSTREAM_BUFFER_CONTROL
                    if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
                        //printf("Frame too large, reset to head\n");
                        VideoBufferIdx = 0;
                    }
                    videobuffer = bs_buf_start + VideoBufferIdx;

                    wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#endif
                    if (h264_fast_ts_seek)
                    {
                        if (h264_fast_ts_check < 2)	//find keyframe
                        {
                            p = *start;
                            unsigned int bits;
                            extern unsigned int ts_skip_bits;

                            for (i=0; i<in_size; i++)
                            {
                                if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] & ~0x60) == 1) )
                                {
                                    ts_skip_bits = 0;
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits);	//first_mb_in_slice
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits);	//sliceType
//							printf("=====> slice type = %d\n", bits);
                                    if (bits == 2 || bits == 7)
                                    {
                                        h264_fast_ts_check++;
                                        if (h264_sps_buf)
                                        {
                                            memcpy(videobuffer, h264_sps_buf, h264_sps_len);
                                            free(h264_sps_buf);
                                            h264_sps_buf = NULL;
                                        }
                                        h264_sps_len++;
                                        break;
                                    }
                                    else
                                        return 0;
                                }
                            }
                            if (in_size < 64)
                            {
                                if (h264_sps_buf)
                                    free(h264_sps_buf);
                                h264_sps_buf = malloc(in_size);
                                h264_sps_len = in_size;
                                memcpy(h264_sps_buf, *start, in_size);
                            }
                        }
                    }
                }
//============================
                if (ts_avc1_pts_queue_cnt == 0)
                    ts_avc1_pts_queue[0] = pts;
                else if (ts_avc1_pts_queue_cnt == 1)
                {
                    if (pts < ts_avc1_pts_queue[0])
                    {
                        ts_avc1_pts_queue[1] = ts_avc1_pts_queue[0];
                        ts_avc1_pts_queue[0] = pts;
                    }
                    else
                        ts_avc1_pts_queue[1] = pts;
                }
                else
                {
                    for (i=0; i<ts_avc1_pts_queue_cnt; i++)
                    {
                        if (ts_avc1_pts_queue[i] == 0)
                        {
                            ts_avc1_pts_queue[i] = pts;
                            break;
                        }
                        else if (pts < ts_avc1_pts_queue[i])
                        {
                            memcpy(ts_avc1_pts_queue+(i+1), ts_avc1_pts_queue+i, (ts_avc1_pts_queue_cnt-i)*sizeof(float));
                            ts_avc1_pts_queue[i] = pts;
                            break;
                        }
                    }
                    if (i == ts_avc1_pts_queue_cnt)
                        ts_avc1_pts_queue[i] = pts;
                }

                //20100728 Robert need keep last_pts for new AV
                if (sh_video->last_pts != MP_NOPTS_VALUE && ts_avc1_pts_queue[0] != MP_NOPTS_VALUE &&
                        sh_video->last_pts != 0.0 && ts_avc1_pts_queue[0] != 0.0)
                {
                    new_ft = 1;
                }
            }
            else if(video_codec == VIDEO_MPEG12)
            {
                if (lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] != 0xff008860)
                {
#ifdef BITSTREAM_BUFFER_CONTROL
                    if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
                        //printf("Frame too large, reset to head\n");
                        VideoBufferIdx = 0;
                    }
                    videobuffer = bs_buf_start + VideoBufferIdx;

                    wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#endif
                }


                p = *start;
                for (i=0; i < in_size -3; i++)
                {
                    if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0xB3 )
                        mp_header_process_sequence_header (&picture, &p[i+4]);
                    else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0xB5 )
                        mp_header_process_extension (&picture, &p[i+4]);
                    else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0 )	//Barry 2010-07-02
                        picture_coding_type=(p[i+5] >> 3) & 7;
                    else if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] == 0x01 )
                        break;
                    else
                        continue;

                    // get mpeg fps:
                    if(sh_video->fps!=picture.fps) if(!force_fps && !telecine)
                        {
                            mp_msg(MSGT_CPLAYER,MSGL_WARN,"Warning! FPS changed %5.3f -> %5.3f  (%f) [%d]  \n",sh_video->fps,picture.fps,sh_video->fps-picture.fps,picture.frame_rate_code);
                            sh_video->fps=picture.fps;
                            sh_video->frametime=1.0/picture.fps;
                        }

                    // fix mpeg2 frametime:
                    frame_time=(picture.display_time)*0.01f;
                    picture.display_time=100;

                    telecine_cnt*=0.9; // drift out error
                    telecine_cnt+=frame_time-5.0/4.0;
                    mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"\r telecine = %3.1f  %5.3f     \n",frame_time,telecine_cnt);

                    if(telecine)
                    {
                        frame_time=1;
                        if(telecine_cnt<-1.5 || telecine_cnt>1.5)
                        {
                            mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_LeaveTelecineMode);
                            telecine=0;
                        }
                    }
                    else if(telecine_cnt>-0.5 && telecine_cnt<0.5 && !force_fps)
                    {
                        frame_time=1;	//Fuchun 2010.11.05
                        sh_video->fps=sh_video->fps*4/5;
                        sh_video->frametime=sh_video->frametime*5/4;
                        mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_EnterTelecineMode);
                        telecine=1;
                    }
                }
            }
            else if(video_codec == VIDEO_VC1)
            {
                if (lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] != 0xff008860)
                {
#ifdef BITSTREAM_BUFFER_CONTROL
                    if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
                        //printf("Frame too large, reset to head\n");
                        VideoBufferIdx = 0;
                    }
                    videobuffer = bs_buf_start + VideoBufferIdx;

                    wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#endif
                }
                //Barry 2010-07-02
                if ( demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS )
                {
                    double temp_pts = 0.0;
                    ds_get_next_pts(d_video);
                    if ( d_video->first && (d_video->pts > d_video->first->pts) )
                    {
                        temp_pts = d_video->pts;
                        d_video->pts = d_video->first->pts;
                        d_video->first->pts = temp_pts;
                    }
                }

                if (fast_ts_pre_pts == d_video->pts)
                {
                    fast_ts_pts_cnt++;
                    d_video->pts += (sh_video->frametime * fast_ts_pts_cnt);
                }
                else
                {
                    fast_ts_pre_pts = d_video->pts;
                    fast_ts_pts_cnt = 0;
                }
//			printf("d_video->pts = %f   sh_video->last_pts = %f\t", d_video->pts, sh_video->last_pts);

                //20100728 Robert need keep last_pts for new AV
                if (sh_video->last_pts != MP_NOPTS_VALUE && d_video->pts != MP_NOPTS_VALUE &&
                        sh_video->last_pts != 0.0 && d_video->pts != 0.0)
                {
                    frame_time = d_video->pts - sh_video->last_pts;
                    //Barry 2010-08-19 fix frame_time < 0 issue
                    if (frame_time < 0)
                        frame_time = 0;
                    new_ft = 1;
                }
//			printf("frame_time = %f\n", frame_time);
            }

            if (lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] != 0xff008860)
            {
                //Barry 2010-12-15
                if (h264_sps_len)
                {
                    memcpy(videobuffer+h264_sps_len, *start, videobuf_len);
                    *start=videobuffer;
                    in_size=videobuf_len+h264_sps_len;
                    VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                    h264_sps_len = 0;
                }
                else
                {
                    memcpy(videobuffer, *start, videobuf_len);
                    *start=videobuffer;
                    in_size=videobuf_len;
//			VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                    VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 1024*32);
                }

                if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
                    //printf("Frame too large, reset to head\n");
                    VideoBufferIdx = 0;
                }

            }
            else
            {
                new_addr = lptr[MAX_QUICKBD_FIRST_HDR_LEN/4+2];
                new_size = lptr[MAX_QUICKBD_FIRST_HDR_LEN/4+1];
                *start = new_addr;
                in_size = new_size;
            }
        }
        else


//===============================
#endif	//#if defined(HWTS_READ_FRAME))

#ifdef FAST_TSDEMUX	// Raymond 2009/12/15
//	if( demuxer->file_format==DEMUXER_TYPE_MPEG_TS && (video_codec == VIDEO_MPEG12 || video_codec == VIDEO_H264) )	//Barry 2010-05-18
            if((demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && (video_codec == VIDEO_MPEG12 || video_codec == VIDEO_H264) )	//Barry 2010-05-18
            {
                unsigned char *p = NULL;
                int i=0, read_size, j = 0, buf_pos = 0;
                int in_frame=0;
               unsigned int bits;
               extern unsigned int ts_skip_bits;

                demuxer->need_double_copy = 0;

                if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG TS MPEG12 H264 %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                // videobuf_len = 0;  //Polun 2011-10-05 remove fixed BBC_HD.TS playback black screen.
                if(video_codec == VIDEO_MPEG12) //Polun 2011-10-17  fixed mantis 6350
                    videobuf_len = 0;
                in_size=ds_get_packet(d_video,start);
                #if 1 //Polun 2011-11-30 fixed Deep Blue-1080P-H264-DTS6ch-TS.ts first frame is not I frame.
                unsigned int fcunt = 0;
                //if( !get_h264_iframe && demuxer->file_format == DEMUXER_TYPE_MPEG_TS && video_codec == VIDEO_H264 && speed_mult == 0 )
                if( !get_h264_iframe && demuxer->file_format == DEMUXER_TYPE_MPEG_TS && video_codec == VIDEO_H264) //Polun 2011-11-30 fixed Deep Blue-1080P-H264-DTS6ch-TS.ts FF or FR bug.
                {
                    do
                    {
                        p = *start;
                        for (i=0; i<in_size; i++)
                        {
                            if ( p[i]==0 && p[i+1]==0 && p[i+2]==1 && ((p[i+3] & ~0x60) == 0x01 ) )
                            {
                                // h.264 slice: has to be start MB 0 & type I (2, 4, 7 or 9)
                                unsigned char id = p[i+4];
                                if ( ( id >> 4 ) == 0x0b || ( id >> 2 ) == 0x25 || id == 0x88 || id == 0x8a )
				    {
				        get_h264_iframe = 1;
                                    break;
				    }
                             }
                             if ( p[i]==0 && p[i+1]==0 && p[i+2]==1 && ((p[i+3] & ~0x60) == 5 ) )
                             {
                                 // h.264 IDR picture start
                                 get_h264_iframe = 1;
                                 break;
                              }

                          }
                          if(get_h264_iframe == 1)
                          {
                              if(speed_mult == 0)
                                  printf("$$$$$$$$$$$$ h264 :[%s][%d] find an I-frame $$$$$$$$$$$$$\n",  __func__, __LINE__);
                              break;
                          }
                          else
                          {
                              in_size=ds_get_packet(d_video,start);
                              fcunt += 1;
                          }
                          //if(fcunt > 50)
                          if(fcunt > 180) //Polun 2012-01-03 fixed mantis 6778 hd_other_philips_formula_1.m2ts FFFR_to_normalspeed mosaic issue.
                          {
                              printf("$$$$$$$$$$$$ h264 :[%s][%d] didn't find an I-frame $$$$$$$$$$$$$\n",  __func__, __LINE__);
                              get_h264_iframe = 2;
                              break;
                          }   
                    }while(in_size);
                }
                #endif
                //Polun 2011-10-17 ++s fixed Final Fantasy XIII Final Trailer HD 1080P-Chinese Subtitles.mp4 start code error. 
                //if((!get_h264_iframe ||!fcunt) && demuxer->file_format == DEMUXER_TYPE_MPEG_TS && video_codec == VIDEO_H264 && speed_mult == 0) //Polun 2011-11-25 fixed internet TV no use hwtsdemux.
                if(!h264_check_sps && demuxer->file_format == DEMUXER_TYPE_MPEG_TS && video_codec == VIDEO_H264 && speed_mult == 0)//Polun 2011-11-30
                {
                    p = *start;
                    for (j=0; j<256; j++)
                    {
                        if(j >= (in_size -5))
                            break;
                        if (p[j]==0 && p[j+1]==0 && p[j+2]==0 && p[j+3]==1 && (p[j+4]== 0x67 ||p[j+4]== 0x47||p[j+4]== 0x27))  //find H.264 SPS
                        {
                            videobuf_len = 0;
                            break;
                        }   
                    }
                    h264_check_sps = 1;//Polun 2011-11-30
                } 
                //Polun 2011-10-17 ++e 
                //Polun 2011-12-09 ++s fixed MBAFF file need paser sei.
                //if(interlace_mbaff == 1 && h264_frame_mbs_only == 1)
                if (video_codec == VIDEO_H264 && sh_video->fps == 25.0)	//Barry 2011-12-09
                {
                    p = *start;
                    for (i=0; i<in_size-5; i++)
                    {
                        if(p[i]==0 && p[i+1]==0 && p[i+2]==1 && ((p[i+3] & ~0x60) == 0x06 ))
                        {
                             h264_video_sei(&p[i+4], in_size-4-1);
                         }    
                    }
                }
                //Polun 2011-12-09 ++e
                if(in_size<0) return -1; // EOF

                /* Avoid play DVB-T cause mplayer crash because sometimes *start is NULL point, add by carlos 2010-09-28*/
                if (*start == NULL)
                {
                    printf("@@@@ *start is NULL, in [%s][%d] in_size[%d]@@@@\n", __func__, __LINE__, in_size);
                    return 0;
                }
//printf("in_size=%d  VideoBufferIdx=%d\n", in_size, VideoBufferIdx);
//printf("VideoBuffer[0]=0x%x [1]=0x%x [2]=0x%x\n", VideoBuffer[0], VideoBuffer[1], VideoBuffer[2]);

                //Barry 2010-11-22
                if ( (sh_video->format==0x10000002 && ( ((sh_video->i_bps << 3) < 25000000) || ((sh_video->i_bps << 3) > 85000000) ))
                        || (video_codec == VIDEO_H264 && frame_split >= 1) )
                {
                    check_fast_ts = 0;
                    if (d_video->buffer_size != in_size)
                        buf_pos = in_size;
                    else
                        buf_pos = 0;
                }

#ifdef BITSTREAM_BUFFER_CONTROL
                if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), in_size)) {
                    //printf("Frame too large, reset to head\n");
                    VideoBufferIdx = 0;
                }
                videobuffer = bs_buf_start + VideoBufferIdx;

                wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#else
//Robert 20100712 new VideoBuffer Idx
//next Idx
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
                if (hwtsdemux)
                {
#ifdef STREAM_DIO_AUTO
                    if (dio)
                    {
                        /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                        /* fixed for HD-DVD_Shinobi_clip01.ts, 2010-11-24, change reserved from 4.5 MB to 3.5 MB */
                        //if (ROUND_UP(VideoBufferIdx + in_size, 8192) > (((RESERVED_BUFFER_SIZE + 2 - 1)<<20) - (1 << 19))) // reserved 1 MB for using hw_ts, carlos add 2010-07-23 // failed
                        if (ROUND_UP(VideoBufferIdx + in_size, 8192) > (((RESERVED_BUFFER_SIZE + 1 - 1)<<20) - (1 << 19)))
                            VideoBufferIdx = 0;
                    }
                    else
#endif // end of STREAM_DIO_AUTO			
                    {
                        if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE - 1)<<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                            VideoBufferIdx = 0;
                    }
                }
                else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                {
                    if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE<<20) - (1<<19)))
                    {
                        VideoBufferIdx = 0;
                    }
                }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
                if (hwtsdemux)
                    videobuffer = VideoBuffer[1] + VideoBufferIdx;
                else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                    videobuffer = VideoBuffer[0] + VideoBufferIdx;
                //printf("videobuffer = %08X, start = %08X\n", videobuffer, *start);
#endif // BITSTREAM_BUFFER_CONTROL

#if 1	//Barry 2010-04-07
                if (check_fast_ts_startcode)
                {
                    if (check_fast_ts_startcode == 3)
                    {
                        videobuffer[0] = 0;
                        videobuffer[1] = 0;
                        videobuffer[2] = 1;
                        videobuf_len += 3;
                    }
                    else if (check_fast_ts_startcode == 2)
                    {
                        videobuffer[0] = 0;
                        videobuffer[1] = 0;
                        videobuf_len += 2;
                    }
                    else if (check_fast_ts_startcode == 1)
                    {
                        videobuffer[0] = 0;
                        videobuf_len += 1;
                    }
                    check_fast_ts_startcode = 0;
                }

#if 1	//Barry 2010-08-11
                if (sh_video->format==0x10000002)
                    first_time_len = videobuf_len;
                else if (video_codec == VIDEO_H264)
                    first_time_len = videobuf_len;
#else
                memcpy(videobuffer+videobuf_len, *start, in_size);
#endif
                videobuf_len += in_size;
                pts = d_video->pts;		// Raymond 2009/12/31

#if 1	//Barry 2010-07-02
                if (video_codec == VIDEO_H264)
                {
#if 1		//Barry 2010-08-18
                    if (frame_split >= 1)
                    {
                        p = *start;
                        for (i=0; i < videobuf_len - 4; i++)
                        {
                            if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 0 && p[i+3] == 1 && p[i+4] == 9 )
                            {
                                in_frame = 1;
                                break;
                            }
                        }

#if 1	//Barry 2011-03-17
                        if ( (i == videobuf_len - 4) && (speed_mult < 0 || speed_mult >= 2) && !in_frame && frame_split > 1)
                        {
                        	memcpy(videobuffer+first_time_len, *start, in_size);
				first_time_len += in_size;

				in_size=ds_get_packet(d_video,start);
				if (d_video->buffer_size != in_size)
					buf_pos = in_size;
				else
					buf_pos = 0;

				if(in_size<0) return -1; // EOF
//Robert 20110425 Add chk_tailroom for second ds_get_packet
#ifdef BITSTREAM_BUFFER_CONTROL
                                if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx + first_time_len), in_size)) {
//printf("%s:%d !!! Frame too large, reset to head idx:%d pts:%f\n", __FUNCTION__, __LINE__, VideoBufferIdx, d_video->pts);
                                    unsigned int oldVideoBufferIdx = VideoBufferIdx;
                                    char *oldVideoBuffer = videobuffer;
                                    VideoBufferIdx = 0;
                                    videobuffer = bs_buf_start + VideoBufferIdx;
                                    wait_buffer(VIRTUAL_TO_UCM(videobuffer), first_time_len + in_size);
                                    memcpy(videobuffer, oldVideoBuffer, first_time_len); 
                                }
                                else
                                {
                                    videobuffer = bs_buf_start + VideoBufferIdx;
                                    wait_buffer(VIRTUAL_TO_UCM(videobuffer + first_time_len), in_size);
                                }

//                                wait_buffer(VIRTUAL_TO_UCM(videobuffer), in_size);
#endif

				pts = d_video->pts;
				p = *start;
				for (i=0; i < in_size - 4; i++)
				{
					if ( !in_frame && p[i] == 0 && p[i+1] == 0 && p[i+2] == 0 && p[i+3] == 1 && p[i+4] == 9 )
					{
						in_frame = 1;
						break;
					}
				}
//printf("[%s - %d]   first_time_len =%d    videobuf_len=%d\n", __func__, __LINE__, first_time_len, videobuf_len);
				if ( (videobuffer[0] == p[6]) && (videobuffer[1] == p[7]) && (videobuffer[2] == p[8]) && (videobuffer[3] == p[9]) && (videobuffer[4] == p[10]) )
				{
					first_time_len = 0;
					videobuf_len = 0;
				}
//printf("[%s - %d]   first_time_len =%d    videobuf_len=%d\n", __func__, __LINE__, first_time_len, videobuf_len);
//printf("[%s - %d]  i_pos=%d  %.2X %.2X %.2X %.2X %.2X\n", __func__, __LINE__, i, p[i+5], p[i+6], p[i+7], p[i+8], p[i+9]);
//printf("[%s - %d]    i_pos=%d     in_size=%d    in_frame=%d   first_time_len=%d,     videobuf_len=%d\n", __func__, __LINE__, i, in_size, in_frame, first_time_len, videobuf_len);

				for (j=i+1; j<in_size - 4; j++)
				{
					if (in_frame == 1 && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 && p[j+3] == 1 && p[j+4] == 9 )
					{
						check_fast_ts = 2;
						if (!buf_pos)
							d_video->buffer_pos = j;
						else
							d_video->buffer_pos = (d_video->buffer_size - buf_pos + j);
						memcpy(videobuffer+first_time_len, *start, j);
						videobuf_len += j;
						break;
					}
				}
				if (j == in_size-4)
				{
					memcpy(videobuffer+first_time_len, *start, in_size);
					videobuf_len += in_size;
				}
                        }
                       else
			  {
			  	for (j=i+1; j<videobuf_len - 4; j++)
				{
					if (in_frame == 1 && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 && p[j+3] == 1 && p[j+4] == 9 )
					{
						check_fast_ts = 1;
						if (!buf_pos)
							d_video->buffer_pos = j;
						else
							d_video->buffer_pos = (d_video->buffer_size - buf_pos + j);
						videobuf_len = j;
						break;
					}
				}
#if 1	//Barry 2011-04-23	/* try to fixed SAMPLE Red 2010 (mp4).ts */
				if ((in_frame == 1) && (j == (videobuf_len - 4)) && (frame_split == 2))
				{
					memcpy(videobuffer+first_time_len, *start, videobuf_len);

					while(1)
					{
						in_size=ds_get_packet(d_video,start);
						if (in_size < 0) 
							return -1; // EOF
						else
						{
							p = *start;
							for (i=0;i<in_size-4;i++)
							{
								if (p[i] == 0 && p[i+1] == 0 && p[i+2] == 0 && p[i+3] == 1 && p[i+4] == 9)
								{
									if (i>0)
									{
										memcpy(videobuffer+videobuf_len, *start, i);
										videobuf_len += i;
									}
									d_video->buffer_pos = i;
									check_fast_ts = 2;
									break;
								}
							}
							if (i == (in_size-4))
							{
								memcpy(videobuffer+videobuf_len, *start, in_size);
								videobuf_len += in_size;
							}
							else
							{
								break;
							}
						}
					}
				}
#endif
			  }
#else
                        for (j=i+1; j<videobuf_len - 4; j++)
                        {
                            if (in_frame == 1 && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 && p[j+3] == 1 && p[j+4] == 9 )
                            {
                                check_fast_ts = 1;
//						printf("frame_size=%d   videobuf_len=%d\n", j, videobuf_len);
                                if (!buf_pos)
                                    d_video->buffer_pos = j;
                                else
                                    d_video->buffer_pos = (d_video->buffer_size - buf_pos + j);
                                videobuf_len = j;
                                break;
                            }
                        }
#endif

                        if (frame_split == 1)
                        {
//					printf("videobuf_len=%d, j=%d\n", videobuf_len, j);
                            if (j == (videobuf_len - 4))
                                frame_split = 0;
                            else
                            {
                                frame_split++;
                                printf("H.264-TS: video frame need to split\n");
                            }
                        }
                    }
#endif

#if 1	//Barry 2011-09-07
                    avc1_next_pts = ds_get_next_pts(d_video);
                    if ((!frame_split) && (avc1_next_pts == pts) && (sh_video->frametime != 0) && (picture.fps != 0) //Polun 2011-09-26 for HD-h264.ts time not correction
				&& ((pts - ts_avc1_pts_queue[ts_avc1_pts_queue_cnt-1]) > (1.5 * sh_video->frametime)))
                    {
//				printf("####   [0] = %f   [1] = %f  [2] = %f\n", ts_avc1_pts_queue[0], ts_avc1_pts_queue[1], ts_avc1_pts_queue[2]);
//				printf("pts = %f    avc1_next_pts = %f\n", pts, avc1_next_pts);
				pts = (pts + ts_avc1_pts_queue[ts_avc1_pts_queue_cnt-1])/2.0;
//				printf("pts = %f    avc1_next_pts = %f\n\n", pts, avc1_next_pts);
                    }
#endif
                    //Polun 2011-12-09 ++s fixed MBAFF file merge two field to decoder .
                    if (check_fast_ts != 2)
                    {
                    		memcpy(videobuffer+first_time_len, *start, in_size);
				//printf("######  [%s - %d]  first size: %d\n", __func__, __LINE__, in_size);
                    }
                    //if(interlace_mbaff == 1 && h264_frame_mbs_only == 1 && h264_frame_field == 0 && sh_video->fps == 25.0)
                    if(video_codec == VIDEO_H264 && h264_frame_mbs_only == 1 && h264_frame_field == 0 && sh_video->fps == 25.0)	//Barry 2011-12-09
                    {
                    #if 1
                    		p = *start;
				unsigned int bits1, bits2;
				unsigned int h264_sliceType1 = 0;
                            unsigned int h264_sliceType2 = 0;
				unsigned int h264_frame_num1 = 0;
                            unsigned int h264_frame_num2 = 0;
                            unsigned int temp_cunt = 0;
				extern unsigned int ts_skip_bits;
                            for (i=0; i<in_size-5; i++)
                            {
                                if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] & ~0x60) == 1) )
                                {
                                    ts_skip_bits = 0;
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits1);	//first_mb_in_slice
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits1);	//sliceType
                                    h264_sliceType1 = bits1;
                                    #if 1 //Polun 2011-12-13 for H264 get frame num. 
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits1);	//pic_parameter_set_id
                                    /* log2(maxFrameNum) -> num bits to represent frame_num */
                                     temp_cunt = 0;
                                    while(maxFrameNum >> temp_cunt)
                                        temp_cunt++;
                                     temp_cunt--;
                                    h264_frame_num1 = h264bsdGetBits(&p[i+5], temp_cunt); //frame_num
                                    #endif
                                    break;
                                }
				}
				//printf("######  [%s - %d]  first size: %d    slice_type = %d\n", __func__, __LINE__, in_size, bits1);
			#endif
				//printf("@@@@@@@ first field pts = %f @@@@@@@@\n",pts);
				in_size=ds_get_packet(d_video,start);
				if(in_size<0) return -1; // EOF

			#if 1
				p = *start;
                            for (i=0; i<in_size-5; i++)
                            {
                                if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] & ~0x60) == 1) )
                                {
                                    ts_skip_bits = 0;
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits2);	//first_mb_in_slice
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits2);	//sliceType
                                    h264_sliceType2 = bits2;
                                    #if 1 //Polun 2011-12-13 for H264 get frame num. 
                                    h264bsdDecodeExpGolombUnsigned(&p[i+5], &bits2);	//pic_parameter_set_id
                                    /* log2(maxFrameNum) -> num bits to represent frame_num */
                                     temp_cunt = 0;
                                    while(maxFrameNum >> temp_cunt)
                                        temp_cunt++;
                                     temp_cunt--;
                                    h264_frame_num2 = h264bsdGetBits(&p[i+5], temp_cunt); //frame_num
                                    #endif
                                    break;
                                }
				}
			//printf("######  [%s - %d]  second size: %d     slice_type = %d\n", __func__, __LINE__, in_size, bits2);
			#endif
			//printf("######  [%s - %d]   h264_frame_num1 = %d     s h264_frame_num2 = = %d\n", __func__, __LINE__,  h264_frame_num1,  h264_frame_num2);
				//if (bits1 == bits2) //Polun 2011-12-13 for H264 get frame num. 
				if(h264_frame_num1 == h264_frame_num2 && h264_sliceType1 == h264_sliceType2)
				{
#ifdef BITSTREAM_BUFFER_CONTROL
					if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx + videobuf_len), in_size))
					{
						unsigned int oldVideoBufferIdx = VideoBufferIdx;
						char *oldVideoBuffer = videobuffer;
						VideoBufferIdx = 0;
						videobuffer = bs_buf_start + VideoBufferIdx;
						wait_buffer(VIRTUAL_TO_UCM(videobuffer), videobuf_len + in_size);
						memcpy(videobuffer, oldVideoBuffer, videobuf_len);
					}
					else
					{
						videobuffer = bs_buf_start + VideoBufferIdx;
						wait_buffer(VIRTUAL_TO_UCM(videobuffer + videobuf_len), in_size);
					}
#endif // BITSTREAM_BUFFER_CONTROL
					pts = d_video->pts;
					//printf("@@@@@@@ second field pts = %f @@@@@@@@\n",pts);
					memcpy(videobuffer+ videobuf_len, &d_video->buffer[0], in_size);
					videobuf_len += in_size;
				}
				else
				{
					d_video->buffer_pos = 0;
				}
                    }
                    //Polun 2011-12-09 ++e
                    if (ts_avc1_pts_queue_cnt == 0)
                        ts_avc1_pts_queue[0] = pts;
                    else if (ts_avc1_pts_queue_cnt == 1)
                    {
                        if (pts < ts_avc1_pts_queue[0])
                        {
                            ts_avc1_pts_queue[1] = ts_avc1_pts_queue[0];
                            ts_avc1_pts_queue[0] = pts;
                        }
                        else
                            ts_avc1_pts_queue[1] = pts;
                    }
                    else
                    {
                        for (i=0; i<ts_avc1_pts_queue_cnt; i++)
                        {
                            if (ts_avc1_pts_queue[i] == 0)
                            {
                                ts_avc1_pts_queue[i] = pts;
                                break;
                            }
                            else if (pts < ts_avc1_pts_queue[i])
                            {
                                memcpy(ts_avc1_pts_queue+(i+1), ts_avc1_pts_queue+i, (ts_avc1_pts_queue_cnt-i)*sizeof(float));
                                ts_avc1_pts_queue[i] = pts;
                                break;
                            }
                        }
                        if (i == ts_avc1_pts_queue_cnt)
                            ts_avc1_pts_queue[i] = pts;
                    }
//20100728 Robert need keep last_pts for new AV
                    if (sh_video->last_pts != MP_NOPTS_VALUE && ts_avc1_pts_queue[0] != MP_NOPTS_VALUE &&
                            sh_video->last_pts != 0.0 && ts_avc1_pts_queue[0] != 0.0)
                    {
                        new_ft = 1;
                    }

#if 1	//Barry 2010-11-10
                    while (1)
                    {
                        if (frame_split)
                            break;

                        in_size=ds_get_packet(d_video,start);
                        if(in_size<0) return -1; // EOF
                        pts2 = d_video->pts;
                        if (h264_header_packet)
                        {
                            pts = pts2;
                            h264_header_packet = 0;
                        }
                        if (pts2 == pts)
                        {
                            if (in_size < 64)
                            {
                                p = *start;
                                for (i=0; i<in_size; i++)
                                {
                                    if ( p[i]==0 && p[i+1]==0 && p[i+2]==0 && p[i+3]==1 && ((p[i+4] &0x1F) == 7) )	//SPS
                                    {
                                        h264_header_packet = 1;
                                        //Barry 2010-11-23
                                        if (!h264_fast_ts_seek)
                                            h264_fast_ts_seek = 1;

                                        d_video->buffer_pos = 0;
                                        break;
                                    }
                                }
                                if (d_video->buffer_pos == 0)
                                    break;
                            }
#if 1	//Barry 2011-01-20
                            else
                            {
                                p = *start;
                                for (i=0; i<in_size-5; i++)
                                {
                                    if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 0 && p[i+3] == 1 && p[i+4] == 9 )
                                    {
#ifdef BITSTREAM_BUFFER_CONTROL
                                        if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(videobuffer+videobuf_len), i)) {
                                            //printf("Frame too large, reset to head\n");
                                            wait_buffer(VIRTUAL_TO_UCM(bs_buf_start), videobuf_len);
                                            memcpy(bs_buf_start, videobuffer, videobuf_len);
                                            VideoBufferIdx = 0;
                                        }
                                        videobuffer = bs_buf_start + VideoBufferIdx;

                                        wait_buffer(VIRTUAL_TO_UCM(videobuffer + videobuf_len), i);
#endif
                                        memcpy(videobuffer+videobuf_len, *start, i);
                                        videobuf_len += i;
                                        d_video->buffer_pos = i;
                                        break;
                                    }
                                }
                                if (i != in_size - 5)
                                    break;
                            }
#endif

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(videobuffer+videobuf_len), in_size)) {
                                //printf("Frame too large, reset to head\n");
                                wait_buffer(VIRTUAL_TO_UCM(bs_buf_start), videobuf_len);
                                memcpy(bs_buf_start, videobuffer, videobuf_len);
                                VideoBufferIdx = 0;
                            }
                            videobuffer = bs_buf_start + VideoBufferIdx;

                            wait_buffer(VIRTUAL_TO_UCM(videobuffer + videobuf_len), in_size);
#endif
                            memcpy(videobuffer+videobuf_len, *start, in_size);
                            videobuf_len += in_size;
                        }
                        else
                        {
#if 1	//Barry 2011-07-22
                        	p = *start;
                        	if (in_frame == 1 && (j == videobuf_len-4) && !(p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1 && p[4] == 9))
                        	{
					for (i=0;i<in_size-4;i++)
					{
						if (p[i] == 0 && p[i+1] == 0 && p[i+2] == 0 && p[i+3] == 1 && p[i+4] == 9)
						{
							memcpy(videobuffer+videobuf_len, *start, i);
							videobuf_len += i;
							d_video->buffer_pos = i;
							h264_need_merge_more = 1;
							frame_split = 1;
							break;
						}
					}
                        	}
				else
				{
					if (h264_need_merge_more == 1 && frame_split == 0)
						frame_split = 1;
                            	d_video->buffer_pos = 0;
				}
#else
                            d_video->buffer_pos = 0;
#endif
                            break;
                        }
                    }
#endif
                }
#endif

                if (sh_video->format==0x10000002)
                {
#if 1	//Barry 2010-08-11
                    p = *start;
                    for (i=0; i < videobuf_len - 3; i++)
                    {
                        if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] >= 1 && p[i+3] < 0xB0 )
                        {
                            in_frame = 1;
                            pts=d_video->pts;
                            break;
                        }
                    }

                    //Barry 2010-08-06
                    //if (( (sh_video->i_bps << 3) < 25000000) && in_frame == 1)
                    if ( (((sh_video->i_bps << 3) < 25000000) || ((sh_video->i_bps << 3) > 85000000)) && in_frame == 1)	//Barry 2010-11-22
                    {
                        for (j=i; j<videobuf_len - 3; j++)
                        {
//					printf("j=%d   %.2X %.2X %.2X %.2X\n", j, p[j], p[j+1], p[j+2], p[j+3]);
                            if ( (p[j] == 0 && p[j+1] == 0 && p[j+2] == 1) && (p[j+3] < 0x01 || p[j+3] >= 0xB0) )
                            {
                                if (!buf_pos)
                                    d_video->buffer_pos = j;
                                else
                                    d_video->buffer_pos = (d_video->buffer_size - buf_pos + j);
                                videobuf_len = j;
                                check_fast_ts = 1;	//Barry 2010-08-25
//						printf("======   buffer_size=%d, buf_pos=%d, j=%d   =======\n", d_video->buffer_size, buf_pos, j);
                                break;
                            }
                        }
                    }
#if 1	//Barry 2011-01-18
                    if ( p[0] == 0 && p[1] == 0 && p[2] == 1 && p[3] == 0xB7 &&
                            p[4] == 0 && p[5] == 0 && p[6] == 1 && p[7] == 0xB3 )
                    {
                        *start += 4;
                        if ( in_size == (videobuf_len - first_time_len) )
                        {
                            in_size -= 4;
                            memcpy(videobuffer+first_time_len, *start, in_size);
                        }
                        else
                        {
                            videobuf_len -= 4;
                            memcpy(videobuffer+first_time_len, *start, videobuf_len);
                        }
                    }
                    else
#endif
                    {
                        if ( in_size == (videobuf_len - first_time_len) )
                        {
                            memcpy(videobuffer+first_time_len, *start, in_size);
                        }
                        else
                        {
                            memcpy(videobuffer+first_time_len, *start, videobuf_len);
                        }
                    }
#endif

                    while (1)
                    {
                        //Barry 2010-08-06
                        if (check_fast_ts == 1)
                            break;

                        read_size = ds_get_packet(d_video,start);
                        if(read_size<0) return -1;	// EOF
                        /* DVB-T if have noise will cause read_size == 0 */
                        if (read_size < 3)
                        {
                            //printf("@@@@@@ read_size is [%d], need continue@@@@@ in [%s][%d] continue@@@@@\n", read_size, __func__, __LINE__);
                            continue;
                        }

#ifdef BITSTREAM_BUFFER_CONTROL
                        if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx + videobuf_len), read_size)) {
                            //printf("Frame too large, reset to head\n");
                            wait_buffer(VIRTUAL_TO_UCM(bs_buf_start), videobuf_len);
                            memcpy(bs_buf_start, videobuffer, videobuf_len);
                            VideoBufferIdx = 0;
                        }
                        videobuffer = bs_buf_start + VideoBufferIdx;

                        wait_buffer(VIRTUAL_TO_UCM(videobuffer + videobuf_len), read_size);
                        //printf("%d: idx %x buf %x size %x\n", __LINE__, VideoBufferIdx, VIRTUAL_TO_UCM(videobuffer), read_size);
#else
//Barry 2010-08-25 add for 3MB buffer boundary check avoid overflow
//next Idx
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
                        if (hwtsdemux)
                        {
#ifdef STREAM_DIO_AUTO
                            if (dio)
                            {
                                /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                                if (ROUND_UP(VideoBufferIdx + read_size + videobuf_len, 8192) > ((RESERVED_BUFFER_SIZE +2 -1)<<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                {
                                    memcpy(VideoBuffer[1], videobuffer, videobuf_len);
                                    VideoBufferIdx = 0;
                                }
                            }
                            else
#endif // end of STREAM_DIO_AUTO					
                            {
                                if (ROUND_UP(VideoBufferIdx + read_size + videobuf_len, 8192) > ((RESERVED_BUFFER_SIZE - 1)<<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                {
                                    memcpy(VideoBuffer[1], videobuffer, videobuf_len);
                                    VideoBufferIdx = 0;
                                }
                            }
                        }
                        else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                        {
                            if ( ROUND_UP(VideoBufferIdx + read_size + videobuf_len, 8192) > ((RESERVED_BUFFER_SIZE<<20) - (1<<19)) )
                            {
                                memcpy(VideoBuffer[0], videobuffer, videobuf_len);
                                VideoBufferIdx = 0;
                            }
                        }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
                        if (hwtsdemux)
                            videobuffer = VideoBuffer[1] + VideoBufferIdx;
                        else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                            videobuffer = VideoBuffer[0] + VideoBufferIdx;

#endif // BITSTREAM_BUFFER_CONTROL

                        p = d_video->buffer;
                        /* DVB-T if have noise will cause read_size == 0 */
                        if (p == NULL)
                        {
                            printf("@@@@@@ p is NULL continue start[%p]@@@@@ read_size[%d] in [%s][%d] return -1@@@@@\n", start, read_size, __func__, __LINE__);
                            continue;
                            return -1;
                        }

                        if (check_fast_ts_startcode)
                        {
                            if ( (check_fast_ts_startcode == 3 && (p[0] < 0x01 || p[0] >= 0xB0)) ||
                                    (check_fast_ts_startcode == 2 && p[0] == 1 && (p[1] < 0x01 || p[1] >= 0xB0)) ||
                                    (check_fast_ts_startcode == 1 && p[0] == 0 && p[1] == 1 && (p[2] < 0x01 || p[2] >= 0xB0))
                               )
                            {
                                videobuf_len -= check_fast_ts_startcode;
                                d_video->buffer_pos = 0;
                                break;
                            }
                            else if (check_fast_ts_startcode == 2 && p[0] == 0 && p[1] == 1 && (p[2] < 0x01 || p[2] >= 0xB0))
                            {
                                check_fast_ts_startcode = 1;
                                videobuf_len -= check_fast_ts_startcode;
                                d_video->buffer_pos = 0;
                                break;
                            }
                            else
                                check_fast_ts_startcode = 0;
                        }

                        for (i=0; i < read_size -3; i++)
                        {
                            if (!in_frame)
                            {
                                if ( p[i] == 0 && p[i+1] == 0 && p[i+2] == 1 && p[i+3] >= 1 && p[i+3] < 0xB0 )
                                    in_frame = 1;
                            }
                            else
                            {
                                if ( (p[i] == 0 && p[i+1] == 0 && p[i+2] == 1) && (p[i+3] < 0x01 || p[i+3] >= 0xB0) )
                                    break;
                            }
                        }
                        if ( i >= 0 && (i < read_size -3) )
                        {
                            memcpy(videobuffer+ videobuf_len, &d_video->buffer[0], i);
                            videobuf_len += i;
                            d_video->buffer_pos = i;
                            break;
                        }
                        else
                        {
                            memcpy(videobuffer+ videobuf_len, &d_video->buffer[0], read_size);
                            videobuf_len += read_size;
                            d_video->buffer_pos += read_size;
                            if ( p[read_size -3] == 0 && p[read_size -2] == 0 && p[read_size -1] == 1)
                                check_fast_ts_startcode = 3;
                            else if ( p[read_size -2] == 0 && p[read_size -1] == 0)
                                check_fast_ts_startcode = 2;
                            else if ( p[read_size -1] == 0)
                                check_fast_ts_startcode = 1;
                        }
                    }

                    for (i=0; i < videobuf_len -3; i++)
                    {
                        if ( videobuffer[i] == 0 && videobuffer[i+1] == 0 && videobuffer[i+2] == 1 && videobuffer[i+3] == 0xB3 )
			{
                            mp_header_process_sequence_header (&picture, &videobuffer[i+4]);
#ifdef CMV_WORKAROUND  //JF 1031 C			
                             mb_width  = (picture.display_picture_width  + 15) / 16;
                             mb_height = (picture.display_picture_height + 15) / 16;
#endif		
                    	  }						
				   
                        else if ( videobuffer[i] == 0 && videobuffer[i+1] == 0 && videobuffer[i+2] == 1 && videobuffer[i+3] == 0xB5 )
                            mp_header_process_extension (&picture, &videobuffer[i+4]);
                        else if ( videobuffer[i] == 0 && videobuffer[i+1] == 0 && videobuffer[i+2] == 1 && videobuffer[i+3] == 0 )	//Barry 2010-07-02
                            picture_coding_type=(videobuffer[i+5] >> 3) & 7;
                        else if ( videobuffer[i] == 0 && videobuffer[i+1] == 0 && videobuffer[i+2] == 1 && videobuffer[i+3] == 0x01 )
                            break;
                        else
                            continue;

                        // get mpeg fps:
                        if(sh_video->fps!=picture.fps) if(!force_fps && !telecine)
                            {
                                mp_msg(MSGT_CPLAYER,MSGL_WARN,"Warning! FPS changed %5.3f -> %5.3f  (%f) [%d]  \n",sh_video->fps,picture.fps,sh_video->fps-picture.fps,picture.frame_rate_code);
                                sh_video->fps=picture.fps;
                                sh_video->frametime=1.0/picture.fps;
                            }

                        // fix mpeg2 frametime:
                        frame_time=(picture.display_time)*0.01f;
                        picture.display_time=100;

                        telecine_cnt*=0.9; // drift out error
                        telecine_cnt+=frame_time-5.0/4.0;
                        mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"\r telecine = %3.1f  %5.3f     \n",frame_time,telecine_cnt);

                        if(telecine)
                        {
                            frame_time=1;
                            if(telecine_cnt<-1.5 || telecine_cnt>1.5)
                            {
                                mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_LeaveTelecineMode);
                                telecine=0;
                            }
                        }
                        else if(telecine_cnt>-0.5 && telecine_cnt<0.5 && !force_fps)
                        {
                            frame_time=1;	//Fuchun 2010.11.05
                            sh_video->fps=sh_video->fps*4/5;
                            sh_video->frametime=sh_video->frametime*5/4;
                            mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_EnterTelecineMode);
                            telecine=1;
                        }
                    }
                }

#ifdef BITSTREAM_BUFFER_CONTROL
                if(videobuf_len > MAX_FRAME_SIZE)
                    printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
                    *start=videobuffer;
                     in_size=videobuf_len;
  
#if 1	//Barry 2011-05-25
                if (svsd_state->tune_on_h264_workaround)
                {
			for (i=0;i<videobuf_len-5;i++)
			{
				if (videobuffer[i]==0 && videobuffer[i+1]==0 && videobuffer[i+2]==1)
				{
					j = videobuffer[i+3] & 0x1F;
					if (j == 1 || j == 5 || j == 20)	//h264 field
					{
						ts_h264_field_offset[ts_h264_field_num] = i;
						ts_h264_field_num++;
					}
				}
			}
                }
#endif
                VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                videobuf_len = 0; //Polun 2011-10-05 fixed BBC_HD.TS playback black screen.
#else
                memcpy(videobuffer, *start, in_size);
                *start = videobuffer;

                pts = d_video->pts;		// Raymond 2009/12/31
#endif
            }
            else
#endif //#ifdef FAST_TSDEMUX	// Raymond 2009/12/15

#ifdef FAST_MPEG_PS_DEMUX	//Barry 2010-04-21
                if(FAST_MPEG_PS_FLAG && demuxer->file_format==DEMUXER_TYPE_MPEG_PS && (sh_video->i_bps > 3750000) && ((! sh_video->format) || (sh_video->format==0x10000001) || (sh_video->format==0x10000002)))
                {
                    double next_pts = 0.0;
                    unsigned int len = 0, i = 0, flag = 0, pre_buffer_pos = 0;
                    unsigned char *p = NULL;

                    if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG PS %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                    demuxer->need_double_copy = 0;
//#endif
                    if(d_video->eof == 1)
                        return -1;
                    videobuf_len = 0;

#ifdef BITSTREAM_BUFFER_CONTROL
                    if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), MAX_FRAME_SIZE)) {
                        //printf("Frame too large, reset to head\n");
                        VideoBufferIdx = 0;
                    }
                    videobuffer = bs_buf_start + VideoBufferIdx;

                    wait_buffer(VIRTUAL_TO_UCM(videobuffer), MAX_FRAME_SIZE);
#else
//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE-1)<<20))
                    {
                        VideoBufferIdx = 0;
                    }
                    videobuffer = VideoBuffer[0] + VideoBufferIdx;
//#endif
#endif
                    while(1)
                    {
                        if(next_pts > 0 && next_pts != d_video->pts)
                        {
                            if(d_video->buffer_pos>=d_video->buffer_size)
                            {
                                if(!ds_fill_buffer(d_video))
                                {
                                    // EOF
                                    *start = NULL;
                                    return -1;
                                }
                            }

                            p = d_video->buffer;
                            if ( (check_fast_ps_startcode == 3 && (p[0] == 0 ||p[0] == 0xB3)) ||
                                    (check_fast_ps_startcode == 2 && p[0] == 1 && (p[1] == 0 ||p[1] == 0xB3)) ||
                                    (check_fast_ps_startcode == 1 && p[0] == 0 && p[1] == 1 && (p[2] == 0 ||p[2] == 0xB3))
                               )
                            {
                                videobuf_len -= check_fast_ps_startcode;
                                d_video->buffer_pos = 0;
                                break;
                            }
                            else
                            {
                                if (check_fast_ps_startcode == 2 && p[0] == 0 && p[1] == 1 && (p[2] == 0 ||p[2] == 0xB3))
                                {
                                    check_fast_ps_startcode = 1;
                                    videobuf_len -= check_fast_ps_startcode;
                                    d_video->buffer_pos = 0;
                                    break;
                                }
                                check_fast_ps_startcode = 0;
                            }

                            for( i = 0 ; i < d_video->buffer_size - 3 ; i++)
                            {
                                if ( (p[i] == 0 && p[i+1] == 0 && p[i+2] == 1) && (p[i+3] == 0 ||p[i+3] == 0xB3) )
                                {
                                    len = i;
                                    break;
                                }
                            }

                            if (i == d_video->buffer_size - 3)
                            {
//					printf("***************************   not finish   *****************\n\n");
//					printf("videobuf_len=%d, len=%d   frame_cnt=%d\n", videobuf_len, len, frame_cnt);
//					printf("AAA: check_fast_ps_startcode=%d    p[0 ~ 3]=0x%.2X %.2X %.2X %.2X\n", check_fast_ps_startcode, p[0], p[1], p[2], p[3]);
//					printf("final[ 4 bytes]=0x%.2X %.2X %.2X %.2X\n", videobuffer[videobuf_len-4], videobuffer[videobuf_len-3], videobuffer[videobuf_len-2], videobuffer[videobuf_len-1]);
                                if (d_video->buffer_pos == 0)
                                {
                                    memcpy(videobuffer + videobuf_len, &d_video->buffer[0], d_video->buffer_size);
                                    d_video->buffer_pos = d_video->buffer_size;
                                    videobuf_len += d_video->buffer_size;
                                    if(d_video->buffer_pos>=d_video->buffer_size)
                                    {
                                        if(!ds_fill_buffer(d_video))
                                        {
                                            // EOF
                                            *start = NULL;
                                            return -1;
                                        }
                                    }
                                    p = d_video->buffer;
                                    if (videobuffer[videobuf_len-3]==0 &&videobuffer[videobuf_len-2]==0 && videobuffer[videobuf_len-1]==1 && (p[0] == 0 ||p[0] == 0xB3))
                                        check_fast_ps_startcode = 3;
                                    else if (videobuffer[videobuf_len-2]==0 && videobuffer[videobuf_len-1]==0 && p[0] == 1 && (p[1] == 0 ||p[1] == 0xB3))
                                        check_fast_ps_startcode = 2;
                                    else if (videobuffer[videobuf_len-1]==0 && p[0] == 0 && p[1] == 1 && (p[2] == 0 ||p[2] == 0xB3))
                                        check_fast_ps_startcode = 1;
                                    videobuf_len -= check_fast_ps_startcode;
                                    d_video->buffer_pos = 0;
//						printf("pts appear too early!!   check_fast_ps_startcode=%d\n", check_fast_ps_startcode);
                                    break;
                                }
                            }
//				if (frame_cnt > 28)
//					printf("len=%d, i=%d, d_video->buffer_size=%d, buffer_pos=%d\n", len, i, d_video->buffer_size, d_video->buffer_pos);
                            memcpy(videobuffer+ videobuf_len, &d_video->buffer[0], len);
                            d_video->buffer_pos+=len;
                            videobuf_len += len;
                            break;
                        }
                        pre_buffer_pos = d_video->buffer_pos;
                        //Barry 2010-04-22
                        if (FAST_MPEG_PS_SEEK && !videobuf_len && d_video->buffer_pos==4)
                        {
                            videobuffer[0] = 0;
                            videobuffer[1] = 0;
                            videobuffer[2] = 1;
                            videobuffer[3] = d_video->buffer[d_video->buffer_pos-1];
                            videobuf_len += 4;
                            FAST_MPEG_PS_SEEK = 0;
                        }
//			if (frame_cnt > 28)
//			printf("d_video->buffer_pos=%d, d_video->buffer_size=%d\n", d_video->buffer_pos, d_video->buffer_size);
                        in_size = ds_get_packet(d_video,start);
//			if (frame_cnt > 28)
//			printf("in_size = %d    videobuf_len=%d   check_fast_ps_startcode=%d\n", in_size, videobuf_len, check_fast_ps_startcode);

                        if(in_size < 0)
                        {
                            d_video->eof = 1;
                            break;
                        }

                        p = *start;
                        if(videobuf_len == 0)
                        {
                            int check_packet_has_two_frame = 0;

                            if (check_fast_ps_startcode)
                            {
                                if (check_fast_ps_startcode == 3)
                                {
                                    videobuffer[0] = 0;
                                    videobuffer[1] = 0;
                                    videobuffer[2] = 1;
                                    videobuf_len += 3;
                                }
                                else if (check_fast_ps_startcode == 2)
                                {
                                    videobuffer[0] = 0;
                                    videobuffer[1] = 0;
                                    videobuf_len += 2;
                                    if ( p[0] == 1 && (p[1] == 0 ||p[1] == 0xB3) )
                                        check_packet_has_two_frame = 1;
                                }
                                else if (check_fast_ps_startcode == 1)
                                {
                                    videobuffer[0] = 0;
                                    videobuf_len += 1;
                                    if ( (p[0] == 0 && p[1] == 1) && (p[2] == 0 ||p[2] == 0xB3) )
                                        check_packet_has_two_frame = 1;
                                }
                                else if (check_fast_ps_startcode > 4)
                                {
                                    memcpy(videobuffer + videobuf_len, *start, in_size);
                                    videobuf_len += in_size;
                                    check_fast_ps_startcode = 0;
                                    continue;
                                }
                            }
                            else
                            {
                                if ( (p[0] == 0 && p[1] == 0 && p[2] == 1) && (p[3] == 0 ||p[3] == 0xB3) )
                                    check_packet_has_two_frame = 1;
                            }

                            if ( check_packet_has_two_frame ==1 )
                            {
                                if (in_size == 4)
                                {
                                    memcpy(videobuffer + videobuf_len, *start, in_size);
                                    videobuf_len += in_size;
                                    check_fast_ps_startcode = 0;
                                    continue;
                                }
                                else
                                {
                                    //one packet has two frame
                                    for( i = 4 ; i < in_size - 3 ; i++)
                                    {
                                        if ( (p[i] == 0 && p[i+1] == 0 && p[i+2] == 1) && (p[i+3] == 0 ||p[i+3] == 0xB3) )
                                        {
                                            if ( ( (p[0]<<24|p[1]<<16|p[2]<<8|p[3]) == 0x1B3 && flag == 0 && check_fast_ps_startcode == 0 ) ||
                                                    ( (p[0]<<16|p[1]<<8|p[2]) == 0x1B3 && flag == 0 && check_fast_ps_startcode == 1 ) ||
                                                    ( (p[0]<<8|p[1]) == 0x1B3 && flag == 0 && check_fast_ps_startcode == 2 ) ||
                                                    ( p[0] == 0xB3 && flag == 0 && check_fast_ps_startcode == 3 )
                                               )	//Barry 2010-04-23
                                            {
                                                flag = 2;
                                                continue;
                                            }
                                            else
                                            {
                                                len = i;
                                                break;
                                            }
                                        }
                                    }
                                    if (i < in_size - 3)
                                    {
                                        if ( (in_size < d_video->buffer_size) || (check_fast_ps_startcode != 0) )
                                            d_video->buffer_pos = len+pre_buffer_pos;
                                        in_size = len;
                                        flag = 1;
                                    }
                                }
                            }
                            check_fast_ps_startcode = 0;
                        }

                        //check start code boundary
                        if (in_size > 8)
                        {
                            if (p[in_size -4] == 0 && p[in_size -3] == 0 && p[in_size -2] == 1 && (p[in_size -1] == 0 || p[in_size -1] == 0xB3) )
                            {
                                d_video->buffer_pos = d_video->buffer_size - 4;
                                in_size -= 4;
                                flag = 1;
                            }
                        }

                        next_pts = ds_get_next_pts(d_video);
                        if(next_pts > 0 && next_pts != d_video->pts)
                        {
                            if (in_size < 50)
                            {
                                if (videobuf_len == 0 && p[0]==0 && p[1]==0 && p[2]==1 && p[3]==0xB3)
                                {
                                    memcpy(videobuffer + videobuf_len, *start, in_size);
                                    videobuf_len += in_size;
                                    next_pts = 0.0;
                                    continue;
                                }
                            }
                            else
                            {
                                //final packet maybe has next frame start code
                                for( i = in_size - 4 ; i > 4 ; i--)
                                {
                                    if ( (p[i] == 0 && p[i+1] == 0 && p[i+2] == 1) && (p[i+3] == 0 ||p[i+3] == 0xB3) )
                                    {
                                        d_video->buffer_pos = i;
                                        check_fast_ps_startcode = in_size - i;
                                        in_size = i;
                                        flag = 1;
                                        break;
                                    }
                                }
                            }

                            if (flag != 1)
                            {
                                if (p[in_size -3] == 0 && p[in_size -2] == 0 && p[in_size -1] == 1)
                                    check_fast_ps_startcode = 3;
                                else if (p[in_size -2] == 0 && p[in_size -1] == 0)
                                    check_fast_ps_startcode = 2;
                                else if (p[in_size -1] == 0)
                                    check_fast_ps_startcode = 1;
                            }
                        }

                        memcpy(videobuffer + videobuf_len, *start, in_size);
                        videobuf_len += in_size;
                        if (flag == 1)
                            break;
                    }

#ifdef BITSTREAM_BUFFER_CONTROL
                    if(videobuf_len > MAX_FRAME_SIZE)
                        printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
                    *start=videobuffer;
                    in_size=videobuf_len;
//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                    VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
//#endif
                }
                else
#endif
//20100519 Robert use original FAST_TSDEMUX
#if 0
                    if ( demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS )
                    {
do_it_again:
                        in_size=ds_get_packet(d_video,start);
                        if (in_size<0) return -1; // EOF
                        memcpy(videobuffer, *start, in_size);
#ifndef USE_PFM_ADDR_MODE  //PFM DATA Mode
                        {
                            int pp;
                            unsigned int *lptr = videobuffer;
//            if (!((videobuffer[0]==0 && videobuffer[1]==0 &&videobuffer[2]==1) &&(videobuffer[3]==0||videobuffer[3]==1||videobuffer[3]==0x21)))
//                if ((lptr[0]&0x00ffffff)!=0x00010000)
                            if ((lptr[0]&0x00ffffff)!=0x00010000 && (lptr[0]&0xffffffff)!=0x01000000)
                            {
                                printf("lptr[0]=0x%.8x\n", lptr[0]);
                                printf("FAST_TSDEMUX  demuxer->file_format=%d DEMUXER_TYPE_MPEG_TS=%d\n", demuxer->file_format, DEMUXER_TYPE_MPEG_TS);
                                printf("\n===================================\n");
                                for (pp=0; pp<20; pp++)
                                {
                                    printf("%.2x ", videobuffer[pp]);
                                }
                                printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                                //====================
                                lptr = *start;
                                if ((lptr[0]&0x00ffffff)!=0x00010000 && (lptr[0]&0xffffffff)!=0x01000000)
                                {
                                    printf("check again orig data lptr[0]=0x%.8x\n", lptr[0]);
                                    printf("===================================\n");
                                    for (pp=0; pp<20; pp++)
                                    {
                                        printf("%.2x ", *start[pp]);
                                    }
                                    printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                                }
                                else
                                {
                                    printf("\n====== orig data is ok ======\n");
                                }
                                //====================
//Robert 20100503
                                set_skydvb_wrokaround(NEED_SEEK_CHANNEL);

                                goto do_it_again;
                            }
                        }
                        *start = videobuffer;
#else	//PFM Addr Mode
                        {
                            int pp;
                            unsigned int *lptr = *start;
                            unsigned int new_addr, new_size;
                            new_addr = lptr[2];
                            new_size = lptr[1];

//            printf(" @ @ @ @  tag=0x%x addr=0x%x  size=%d\n", lptr[0], new_addr, new_size);
                            if (lptr[0] != 0xff008860)
                            {
                                if (use_PP_buf == 3)
                                {
                                    *start = videobuffer;
                                }
                                else
                                {
                                    goto do_it_again;
                                }
                            }
                            else
                            {
                                *start = new_addr;
                                in_size = new_size;
                            }
                        }
//        *start = videobuffer;

#endif
                        //=====================================

                        pts = d_video->pts;		// Raymond 2009/12/31
                    }
                    else
#endif
                        if(video_codec == VIDEO_MPEG12) {
                            int in_frame=0;
                            //float newfps;
                            //videobuf_len=0;
//Robert 20100712 new VideoBuffer Idx
                            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG 1/2 %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                            demuxer->need_double_copy = 0;

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), MAX_FRAME_SIZE)) {
                                //printf("Frame too large, reset to head\n");
                                VideoBufferIdx = 0;
                            }
                            videobuffer = bs_buf_start+ VideoBufferIdx;

                            wait_buffer(VIRTUAL_TO_UCM(videobuffer), MAX_FRAME_SIZE);
#else
                            if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE-1)<<20))
                            {
                                VideoBufferIdx = 0;
                            }
                            videobuffer = VideoBuffer[0] + VideoBufferIdx;
#endif

#ifdef CONFIG_DVDNAV
                            static unsigned int last_VideoBufferIdx;
                            if(demuxer->stream->type == STREAMTYPE_DVDNAV)
                                videobuf_len = 0;
#endif /* CONFIG_DVDNAV */

                            while(videobuf_len<VIDEOBUFFER_SIZE-MAX_VIDEO_PACKET_SIZE) {
                                int i=sync_video_packet(d_video);
                                //void* buffer=&videobuffer[videobuf_len+4];
                                //Fuchun 2010.04.20
#if 0
                                if(dvd_fast == 1 && videobuf_len > 0)
                                {
                                    unsigned char *temp_buf = videobuffer;
                                    videobuf_len -= 4;
                                    memcpy(videobuffer, temp_buf+4, videobuf_len);
                                    dvd_fast = 0;
                                }
#endif

                                int start=videobuf_len+4;
                                if(in_frame) {
                                    if(i<0x101 || i>=0x1B0) { // not slice code -> end of frame
                                        if(!i)
                                        {
                                            if(demuxer->stream->type == STREAMTYPE_DVDNAV)
					    {
						//VideoBufferIdx = last_VideoBufferIdx;
					    }
                                            return -1; // EOF
                                        }
                                        break;
                                    }
                                } else {
                                    if(i==0x100) {
                                        pts=d_video->pts;
                                        d_video->pts=0;
                                    }
                                    if(i>=0x101 && i<0x1B0) in_frame=1; // picture startcode
                                    else if(!i)
                                    {
                                        if(demuxer->stream->type == STREAMTYPE_DVDNAV)
					{
					    //VideoBufferIdx = last_VideoBufferIdx;
					}
                                        return -1; // EOF
                                    }
                                }
                                if(!read_video_packet(d_video)) return -1; // EOF

                                // process headers:
                                switch(i) {
                                case 0x1B3:
                                    mp_header_process_sequence_header (&picture, &videobuffer[start]);
#ifdef CMV_WORKAROUND   //JF 1031
                                    mb_width  = (picture.display_picture_width  + 15) / 16;
                                    mb_height = (picture.display_picture_height + 15) / 16;
#endif
                                    break;
                                case 0x1B5:
                                    mp_header_process_extension (&picture, &videobuffer[start]);
                                    break;
                                case 0x1B2:
                                    process_userdata (&videobuffer[start], videobuf_len-start);
                                    break;
                                case 0x100:
                                    picture_coding_type=(videobuffer[start+1] >> 3) & 7;
                                    break;
                                }
                            }
                            fps = picture.fps * picture.frame_rate_extension_n / picture.frame_rate_extension_d;

//Robert 20100712 new VideoBuffer Idx
#ifdef CONFIG_DVDNAV
                            if(demuxer->stream->type == STREAMTYPE_DVDNAV)
                                last_VideoBufferIdx = VideoBufferIdx;
#endif

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(videobuf_len > MAX_FRAME_SIZE)
                                printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
                            *start=videobuffer;
                            in_size=videobuf_len;
                            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);

                            // get mpeg fps:
                            if(sh_video->fps!=fps) if(!force_fps && !telecine) {
                                if (fabs(sh_video->fps - fps) > 0.1)
                                {
                                    mp_msg(MSGT_CPLAYER,MSGL_WARN,"01 Warning! FPS changed %5.3f -> %5.3f  (%f) [%d]  \n",sh_video->fps,fps,sh_video->fps-fps,picture.frame_rate_code);
                                    sh_video->fps=fps;
                                    sh_video->frametime=1.0/fps;
                                }
//Robert 20100823 let dynamic frame time do this
//Robert 20100601 change display framerate
#ifdef _SKY_VDEC_V1
//            sky_set_framerate((int)(sh_video->fps*1000));
#endif
                                }

                            // fix mpeg2 frametime:
                            frame_time=(picture.display_time)*0.01f;
                            picture.display_time=100;
                            videobuf_len=0;

                            telecine_cnt*=0.9; // drift out error
                            telecine_cnt+=frame_time-5.0/4.0;
                            mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"\r telecine = %3.1f  %5.3f     \n",frame_time,telecine_cnt);

                            if(telecine) {
                                frame_time=1;
                                if(telecine_cnt<-1.5 || telecine_cnt>1.5) {
                                    mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_LeaveTelecineMode);
                                    telecine=0;
                                }
                            } else {
                                frame_time=1;	//Fuchun 2010.11.05
                                if(telecine_cnt>-0.5 && telecine_cnt<0.5 && !force_fps) {
                                    sh_video->fps=sh_video->fps*4/5;
                                    sh_video->frametime=sh_video->frametime*5/4;
                                    mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_EnterTelecineMode);
                                    telecine=1;
//Robert 20100823 let dynamic frame time do this
//Robert 20100603 change display framerate
                                }
                            }
//printf("sh_video->fps=%f sh_video->fps=%f sh_video->frametime=%f telecine_cnt:%d\n", sh_video->fps, sh_video->fps, sh_video->frametime, sky_telecine);
                        } else if(video_codec == VIDEO_MPEG4) {
//not yet testing...
//Robert 20100712 new VideoBuffer Idx
                            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ MPEG MPEG4 %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                            demuxer->need_double_copy = 0;
#ifdef BITSTREAM_BUFFER_CONTROL
                            if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), MAX_FRAME_SIZE)) {
                                //printf("Frame too large, reset to head\n");
                                VideoBufferIdx = 0;
                            }
                            videobuffer = bs_buf_start + VideoBufferIdx;

                            wait_buffer(VIRTUAL_TO_UCM(videobuffer), MAX_FRAME_SIZE);
#else
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)) // carlos add 2010-07-27
                            if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                            {
#ifdef STREAM_DIO_AUTO
                                if (dio)
                                {
                                    /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                                    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                        VideoBufferIdx = 0;
                                }
                                else
#endif // end of STREAM_DIO_AUTO		
                                {
                                    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE - 2) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                        VideoBufferIdx = 0;
                                }
                            }
                            else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                            {
                                if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE - 1) <<20))
                                {
                                    VideoBufferIdx = 0;
                                }
                            }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))	// carlos add 2010-07-27
                            if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                                videobuffer = VideoBuffer[1] + VideoBufferIdx;
                            else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                                videobuffer = VideoBuffer[0] + VideoBufferIdx;
#endif // BITSTREAM_BUFFER_CONTROL

                            while(videobuf_len<VIDEOBUFFER_SIZE-MAX_VIDEO_PACKET_SIZE) {
                                int i=sync_video_packet(d_video);
                                if(!i) return -1;
                                if(!read_video_packet(d_video)) return -1; // EOF
                                if(i==0x1B6) break;
                            }

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(videobuf_len > MAX_FRAME_SIZE)
                                printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
//Robert 20100712 new VideoBuffer Idx
                            *start=videobuffer;
                            in_size=videobuf_len;
                            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                            videobuf_len=0;
                        } else if(video_codec == VIDEO_H264) {
                            int in_picture = 0;
//Robert 20100712 new VideoBuffer Idx
                            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ H264 %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                            demuxer->need_double_copy = 0;
//not yet testing...
#ifdef BITSTREAM_BUFFER_CONTROL
                            if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), MAX_FRAME_SIZE)) {
                                //printf("Frame too large, reset to head\n");
                                VideoBufferIdx = 0;
                            }
                            videobuffer = bs_buf_start + VideoBufferIdx;

                            wait_buffer(VIRTUAL_TO_UCM(videobuffer), MAX_FRAME_SIZE);
#else
                            if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE-1)<<20))
                            {
                                VideoBufferIdx = 0;
                            }
                            videobuffer = VideoBuffer[0] + VideoBufferIdx;
#endif
                            while(videobuf_len<VIDEOBUFFER_SIZE-MAX_VIDEO_PACKET_SIZE) {
                                int i=sync_video_packet(d_video);
                                int pos = videobuf_len+4;
                                if(!i) return -1;
                                if(!read_video_packet(d_video)) return -1; // EOF
                                if((i&~0x60) == 0x107 && i != 0x107) {
                                    h264_parse_sps(&picture, &(videobuffer[pos]), videobuf_len - pos);
                                    if(picture.fps > 0) {
                                        sh_video->fps=picture.fps;
                                        sh_video->frametime=1.0/picture.fps;
                                    }
                                    i=sync_video_packet(d_video);
                                    if(!i) return -1;
                                    if(!read_video_packet(d_video)) return -1; // EOF
                                }

                                // here starts the access unit end detection code
                                // see the mail on MPlayer-dev-eng for details:
                                // Date: Sat, 17 Sep 2005 11:24:06 +0200
                                // Subject: Re: [MPlayer-dev-eng] [RFC] h264 ES parser problems
                                // Message-ID: <20050917092406.GA7699@rz.uni-karlsruhe.de>
                                if((i&~0x60) == 0x101 || (i&~0x60) == 0x102 || (i&~0x60) == 0x105)
                                    // found VCL NAL with slice header i.e. start of current primary coded
                                    // picture, so start scanning for the end now
                                    in_picture = 1;
                                if (in_picture) {
                                    i = sync_video_packet(d_video) & ~0x60; // code of next packet
//            if(i == 0x106 || i == 0x109) break; // SEI or access unit delim.
                                    //if(i == 0x106 || i == 0x109) //original code
                                    if(ts_version == TS_2)//+SkyMedi_Vincent08102009+
                                    {
                                        if(i == 0x141)//+SkyMedi_Vincent06032009
                                            break; // SEI or access unit delim.
                                    }
                                    else//+SkyMedi_Vincent08102009+
                                    {
                                        if(i == 0x106 || i == 0x109)
                                            break; // SEI or access unit delim.
                                    }
                                    if(i == 0x101 || i == 0x102 || i == 0x105) {
                                        // assuming arbitrary slice ordering is not allowed, the
                                        // first_mb_in_slice (golomb encoded) value should be 0 then
                                        // for the first VCL NAL in a picture
                                        if (demux_peekc(d_video) & 0x80)
                                            break;
                                    }
                                }
                            }

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(videobuf_len > MAX_FRAME_SIZE)
                                printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
                            *start=videobuffer;
                            in_size=videobuf_len;
//Robert 20100712 new VideoBuffer Idx
                            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                            videobuf_len=0;
                        }  else if(video_codec == VIDEO_VC1) {
//Robert 20100715 new fast still buggy
//Robert 20100712 new fast VC1
#if 1
//Robert 20100712 new VideoBuffer Idx
                            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ VC1 %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                            demuxer->need_double_copy = 0;

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), MAX_FRAME_SIZE)) {
                                //printf("Frame too large, reset to head\n");
                                VideoBufferIdx = 0;
                            }
                            videobuffer = bs_buf_start + VideoBufferIdx;

                            wait_buffer(VIRTUAL_TO_UCM(videobuffer), MAX_FRAME_SIZE);
#else
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))	// carlos add 2010-07-27
                            if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                            {
#ifdef STREAM_DIO_AUTO
                                if (dio)
                                {
                                    /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                                    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                        VideoBufferIdx = 0;
                                }
                                else
#endif // end of STREAM_DIO_AUTO		
                                {
                                    if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE -2) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                        VideoBufferIdx = 0;
                                }
                            }
                            else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                            {
                                if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE - 1) <<20))
                                {
                                    VideoBufferIdx = 0;
                                }
                            }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))	// carlos add 2010-07-27
                            if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                                videobuffer = VideoBuffer[1] + VideoBufferIdx;
                            else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                                videobuffer = VideoBuffer[0] + VideoBufferIdx;
#endif // BITSTREAM_BUFFER_CONTROL
                            int pre_video_buffer_size; //Polun 2011-07-06 mantis 5322
                            while(videobuf_len<VIDEOBUFFER_SIZE-MAX_VIDEO_PACKET_SIZE) {
                                int i=sync_video_packet(d_video);
                                pre_video_buffer_size = d_video->buffer_size; //Polun 2011-07-06 mantis 5322
                                if(!i) return -1;
                                if(!read_video_packet(d_video)) return -1; // EOF
#if 1	//Fuchun 20110629 fix video freeze when FF or FR.(PLANET_EARTH_BONUS_DISK.ISO)
                                if(speed_mult > 1 || speed_mult < 0)
                                {
						if((i==0x10D && videobuf_len >= pre_video_buffer_size) || (i==0x10C)) //Polun 2011-07-06 mantis 5322
                                                break;
                                }
                                else
                                {
						if(i==0x10D) break;
                                }
#else
                                if(i==0x10D) break;
#endif
                            }

#ifdef BITSTREAM_BUFFER_CONTROL
                            if(videobuf_len > MAX_FRAME_SIZE)
                                printf("Warning: frame size %x > %x \n", videobuf_len, MAX_FRAME_SIZE);
#endif
                            *start=videobuffer;
                            in_size=videobuf_len;
//Robert 20100712 new VideoBuffer Idx
//next Idx
                            VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
                            videobuf_len=0;

#else

//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                            demuxer->need_double_copy = 0;
//#endif
//under testing...buggy..
//		videobuf_len = 0;
                            unsigned int *lptr, pp, newoffset;
                            unsigned char *nptr;

resync_next_vc1_frame:
                            if (videobuf_len > 0)
                            {
//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                                if (ROUND_UP(VideoBufferIdx, 8192) > ((RESERVED_BUFFER_SIZE-1)<<20))
                                {
                                    VideoBufferIdx = 0;
                                }
                                videobuffer = VideoBuffer[0] + VideoBufferIdx;
//#endif
                                while(videobuf_len<VIDEOBUFFER_SIZE-MAX_VIDEO_PACKET_SIZE) {
                                    int i=sync_video_packet(d_video);
                                    if(!i) return -1;
                                    if(!read_video_packet(d_video)) return -1; // EOF
                                    if(i==0x10D) break;
                                }

                                *start=videobuffer;
                                in_size=videobuf_len;
                                videobuf_len=0;

//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1

//next Idx
                                VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
//#endif
                            }
                            else
                            {
                                newoffset = 0;
                                in_size=ds_get_packet(d_video,start);
                                if(in_size<0) return -1; // EOF

                                if (videobuf_len == 0)
                                {
                                    lptr = *start;

                                    if ((lptr[0]&0xffffffff) != 0x0d010000)
                                    {
                                        nptr = *start;
                                        for (pp=0; pp<64; pp++)
                                        {
//printf(" ...pp:%d %.2x %.2x %.2x %.2x\n",pp, nptr[pp], nptr[pp+1], nptr[pp+2], nptr[pp+3]);
                                            if ((nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1)&&((nptr[pp+3]&0xff)==0x0d))
                                            {
                                                newoffset = pp;
                                                break;
                                            }
                                        }
                                        if (pp == 64)
                                        {
                                            newoffset = -1;
                                        }

                                    }
//printf("pp=%d newoffset=%d\n", pp, newoffset);

                                    if (newoffset == -1)
                                    {
                                        printf("====== drop one VC1 frame ===== in_size:%d  buf_pos=%d  buf_size=%d\n", in_size, d_video->buffer_pos, d_video->buffer_size);
                                        goto resync_next_vc1_frame;
                                    }
                                }

//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
//next Idx

#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))	// carlos add 2010-07-27
                                if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                                {
#ifdef STREAM_DIO_AUTO
                                    if (dio)
                                    {
                                        /* RESERVED_BUFFER_SIZE + 2 mean HWTS using remain buffer because we don't use DIO & AIO, add by carlos 2010-11-17*/
                                        if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE + 2 - 1) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                            VideoBufferIdx = 0;
                                    }
                                    else
#endif // end of STREAM_DIO_AUTO			
                                    {
                                        if (ROUND_UP(VideoBufferIdx + in_size, 8192) > ((RESERVED_BUFFER_SIZE - 1) <<20)) // reserved 1 MB for using hw_ts, carlos add 2010-07-23
                                            VideoBufferIdx = 0;
                                    }
                                }
                                else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                                {
                                    if (ROUND_UP(VideoBufferIdx + in_size, 8192) > (RESERVED_BUFFER_SIZE<<20))
                                    {
                                        VideoBufferIdx = 0;
                                    }
                                }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))	// carlos add 2010-07-27
                                if(hwtsdemux && demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                                    videobuffer = VideoBuffer[1] + VideoBufferIdx;
                                else
#endif // end of defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER)
                                    videobuffer = VideoBuffer[0] + VideoBufferIdx;
//#endif
//printf("#### in_size=%d  videobuf_len=%d\n", in_size, videobuf_len);
                                if (videobuf_len > 0)
                                {
                                    videobuffer[0] = 0;
                                    videobuffer[1] = 0;
                                    videobuffer[2] = 0x1;
                                    videobuffer[3] = 0xd;
                                    memcpy(videobuffer+4, *start, in_size);
                                    videobuf_len = in_size+4;
                                }
                                else
                                {
                                    memcpy(videobuffer, *start, in_size);
                                    videobuf_len = in_size;
                                }

                                pts = d_video->pts;		// Raymond 2009/12/31

                                *start=videobuffer;
                                in_size=videobuf_len;

//Robert 20100712 new VideoBuffer Idx
//#ifdef _SKY_VDEC_V1
                                VideoBufferIdx = ROUND_UP(VideoBufferIdx + in_size, 8192);
//#endif
                                videobuf_len = 0;
                            }
#endif

                            //Barry 2010-07-02
                            if ( demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS )
                            {
                                double temp_pts = 0.0;
                                ds_get_next_pts(d_video);
                                if ( d_video->first && (d_video->pts > d_video->first->pts) )
                                {
                                    temp_pts = d_video->pts;
                                    d_video->pts = d_video->first->pts;
                                    d_video->first->pts = temp_pts;
                                }
                            }

//20100728 Robert need keep last_pts for new AV
                            if (sh_video->last_pts != MP_NOPTS_VALUE && d_video->pts != MP_NOPTS_VALUE &&
                                    sh_video->last_pts != 0.0 && d_video->pts != 0.0)
                            {
                                frame_time = d_video->pts - sh_video->last_pts;
                                //Barry 2010-08-19 fix frame_time < 0 issue
                                if (frame_time < 0)
                                    frame_time = 0;
                                new_ft = 1;
                            }

                        } else {

//Robert 20100712 new VideoBuffer Idx
                            if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ Others need double copy %d $$$$$$$$$$$$$$$$$$$$$\n", __LINE__);
                            demuxer->need_double_copy = 1;

                            // frame-based file formats: (AVI,ASF,MOV)
                            in_size=ds_get_packet(d_video,start);
                            if(in_size<0) return -1; // EOF

#if 1	// Raymond 2009/04/10
                            if( demuxer->file_format==DEMUXER_TYPE_MOV )	// Raymond 20109/03/16
                            {
                                extern int MOV_3GP5_flg;//Polun 2011-11-09 fixed mantis 6447 jpn_engSUB_sample.3gp no video output
                                if(sh_video->format == mmioFOURCC('a','v','c','1') )
                                {
                                    int k = 0;
                                    int len = 0;
                                    unsigned char *ps = *start;

                                    for( k = 0 ; k < in_size ; k += 4)
                                    {
                                        len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
//				printf("len = %d\n", len);
                                        ps[k] = ps[k+1] = ps[k+2] = 0;
                                        ps[k+3] = 0x1;
                                        if(len < 0) break;		//Fuchun 2010.02.23
                                        k += len;
                                    }
                                }
                                 //Polun 2011-11-09 ++s fixed mantis 6447 jpn_engSUB_sample.3gp no video output
                                else if(MOV_3GP5_flg && sh_video->format == mmioFOURCC('m','p','4','v') ) //Polun 2011-11-08 fixed mantis 6447 jpn_engSUB_sample.3gp no video output
                                {
                                     int dcunt;
                                     unsigned char* buf = *start;
                                     for (dcunt=0; dcunt<in_size-4; dcunt++)
                                     {
                                         if (buf[dcunt]==0 && buf[dcunt+1]==0 && buf[dcunt+2]==1)
                                         {
                                             //printf("mp4v start code: 00 00 01 %.2X\n",  buf[dcunt+3]);
                                             if(buf[dcunt+3] == 0xb0 ||buf[dcunt+3] == 0xb6 ||buf[dcunt+3] == 0x20 || buf[dcunt+3] == 0xb5)
                                             {
                                                 (*start)+= dcunt;
                                                  in_size -=dcunt;
                                                  break;
                                              }   
                                           }
                                      }
                                }
                                //Polun 2011-11-09 ++e
                            }
#endif

                            //rywu20090601++
                            if (demuxer->file_format == DEMUXER_TYPE_REAL)
                            {
                                //NOTE : d_video->buffer[] or **start indicates to data buffer.
                                //		 You may use it to show packet content.

                                //Each RV buffer has demux packet header(struct dp_hdr_s) at the begin of RV buffer,
                                //and some extra information padding to data buffer. So we have to get rid of
                                //unnecessary head/tail information data.

                                //get demuxer packet header from buffer
                                dp_hdr = (dp_hdr_t *)d_video->buffer;

                                //give correct information of this packet for Trinity
                                (*start)+= DP_HDR_T_SIZE;//shift to RV start address by decreasing dp_hdr_s header size
                                in_size	= dp_hdr->chunktab - DP_HDR_T_SIZE - RV_INFO_BLOCK_SIZE; //decrease extra info size at tail

                                //rywu20090629++
                                //react of shriking slices fields
                                rv_phdr = (rv_pic_header *)(d_video->buffer + DP_HDR_T_SIZE);
                                rv_extra_info = (rv_info *)(d_video->buffer + dp_hdr->chunktab - RV_INFO_BLOCK_SIZE);
                                in_size	-= rv_extra_info->frame_padding_len;//rywu20090701
                                if (rv_extra_info->useless_slice_num > 0)
                                {
                                    printf("rv_extra_info->useless_slice_num = %d\n", rv_extra_info->useless_slice_num);
#if 1	//Barry 2010-10-19
                                    //update num_slices
                                    PUT32(rv_phdr->num_slices, &value32);
                                    value32 -= rv_extra_info->useless_slice_num;
                                    PUT32(value32, (unsigned char *)(&rv_phdr->num_slices));
                                    int buf_offset = RV_PICTURE_HEADER_SIZE + (value32 << 3);
                                    //printf("rv_phdr->num_slices: %.8X   dest: %d    src:%d\n", rv_phdr->num_slices, buf_offset, buf_offset + (rv_extra_info->useless_slice_num << 3));

                                    memcpy(*start + buf_offset, *start + buf_offset + (rv_extra_info->useless_slice_num << 3),  in_size - (buf_offset + (rv_extra_info->useless_slice_num << 3)));
                                    in_size -= (rv_extra_info->useless_slice_num << 3);
#endif

                                    //update header content, length, and start address ,
//#if 0		// Raymond 2010/05/26 disable
                                    //	(*start)+=buf_offset;
                                    //	in_size-=buf_offset;
                                }
                                //rywu20090629--

                                //for debug
                                /*unsigned char *my_ps = *start;
                                int my_i;
                                printf("    buffer content --> \n");
                                for(my_i=0; my_i<32 && (in_size-my_i)>0; my_i++)
                                {
                                	if(my_i!=0 && !(my_i%16))
                                	{
                                		printf("\n");
                                	}
                                	printf(" %.2x", my_ps[my_i]);
                                }
                                printf("\n");
                                */
                            }
                            //rywu20090601--
                        }

end_fast_tsdemux:

    //if(!demuxer->need_double_copy)
    //printf("Addr %x Size %x Next VideoBufferIdx %x\n", VIRTUAL_TO_UCM(*start), in_size, VideoBufferIdx);

//------------------------ frame decoded. --------------------

    // Increase video timers:
    sh_video->num_frames+=frame_time;
    ++sh_video->num_frames_decoded;

//20100728 Robert need keep last_pts for new AV
    if (new_ft == 0)
        frame_time*=sh_video->frametime;

    // override frame_time for variable/unknown FPS formats:
    if(!force_fps) switch(demuxer->file_format) {
        case DEMUXER_TYPE_GIF:
        case DEMUXER_TYPE_MATROSKA:
        case DEMUXER_TYPE_MNG:
            if(d_video->pts>0 && pts1>0 && d_video->pts>pts1)
                frame_time=d_video->pts-pts1;
            break;
        case DEMUXER_TYPE_TV:
        case DEMUXER_TYPE_MOV:
        case DEMUXER_TYPE_FILM:
        case DEMUXER_TYPE_VIVO:
        case DEMUXER_TYPE_OGG:
        case DEMUXER_TYPE_ASF: {
            double next_pts = ds_get_next_pts(d_video);
            double d= (next_pts != MP_NOPTS_VALUE) ? next_pts - d_video->pts : d_video->pts-pts1;
            if(!(speed_mult != 0 && is_mjpeg == 1))	//Fuchun 2010.03.29
                if(d>=0) {
                    if(d>0) {
                        if((int)sh_video->fps==1000)
                            mp_msg(MSGT_CPLAYER,MSGL_V,"\navg. framerate: %d fps             \n",(int)(1.0f/d));
                        sh_video->frametime=d; // 1ms
                        sh_video->fps=1.0f/d;
                    }
                    frame_time = d;
                } else {
                    mp_msg(MSGT_CPLAYER,MSGL_WARN,"\nInvalid frame duration value (%5.3f/%5.3f => %5.3f). Defaulting to %5.3f sec.\n",d_video->pts,next_pts,d,frame_time);
                    // frame_time = 1/25.0;
                }
        }
        break;
        case DEMUXER_TYPE_LAVF:
        case DEMUXER_TYPE_LAVF_PREFERRED:
            if((int)sh_video->fps==1000 || (int)sh_video->fps<=1) {
                double next_pts = ds_get_next_pts(d_video);
                double d= (next_pts != MP_NOPTS_VALUE) ? next_pts - d_video->pts : d_video->pts-pts1;
                if(d>=0) {
                    frame_time = d;
                }
            }
            break;

        case DEMUXER_TYPE_REAL:
        {
            double next_pts = ds_get_next_pts(d_video);
            double d = (next_pts != MP_NOPTS_VALUE) ? next_pts - d_video->pts : d_video->pts - pts1;

            frame_time = (d >= 0 && pts1 > 0) ? d : 0.001;
        }

        break;
        }
double check_new_pts=0.0;
    if(video_codec == VIDEO_MPEG12
            /*
            //20100519 Robert remove it
                 || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS
            */
      ) {
static int mpeg2_gop_pkt_cnts=0;

#ifdef CMV_WORKAROUND //JF 1019

    unsigned char  *  data_ptr = (unsigned char *) videobuffer; //JF 1024

{

    ret_error = 0;		   
    const uint8_t *buf_end;
    const uint8_t *buf_ptr;
    int ret, start_code, input_size;

    const uint8_t * buf_ptr_00;	

	
 if  ( ( c_m_v ) && (picture.display_picture_width < 721) && ( (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_ES) ) && (picture_coding_type == 2) )	
//JF 1101   if  ( ( (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_ES) ) && ( c_m_v ) && (picture_coding_type == 2) && (picture.display_picture_width < 720) )
//JF 1027  if  ( ( (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (sh_video->ds->demuxer->type == DEMUXER_TYPE_MPEG_ES) )  && (picture.display_picture_width < 720) ) 	
    	{

    buf_ptr = data_ptr ;
                                          
    buf_end = data_ptr + in_size;  	

    buf_ptr_00 = buf_ptr;
    need_modify_mv = 0;  


    //JF 1027
    if (first_entry)
      {
         first_entry = 0 ;
         mpeg12_init_vlcs();
         data_2 =  malloc( 102400 );  
         memset(data_2, 0, 102400 );  
         buf_ptr_2 = data_2;	                                  
         buf_end_2 = data_2 + 102400 ;  	
	  printf("\n @@! mpeg12_cmv_workaround_init_VLC ...\n");
	  
         //JF 1108           
         block=  &blocks[0][0];
	  
      }     


    for(;;) {

        start_code = find_start_code(&buf_ptr, buf_end);

 
 	
        if (start_code < 0) break;

      input_size = buf_end - buf_ptr;

        switch (start_code)
        {


        case EXT_START_CODE:        //B5
                           {
                             dist =  buf_ptr - buf_ptr_00;
                             dist = ( dist + (27 / 8) ) ; 
                           }
                    break;		   
					
	  case PICTURE_START_CODE:  //00
//JF 1108 			 extra_cnt =0;		   

                     //JF 1031 pict_type  =  picture_coding_type;

                     //JF 1027     if(  pict_type == 2   &&  c_m_v == 1)    
                     {
                     buf_ptr_2 = data_2;	
					 
			  memcpy(data_2, data_ptr, 4);		 

			  buf_ptr_2 +=  4; 		 
			  after_PIC_SC = buf_ptr;
                     } 
		   break;		
		   
	  default:

               //JF 1027    if(  pict_type == 2   &&  c_m_v == 1)   
                {
                if (start_code  >=  SLICE_MIN_START_CODE  &&    start_code  <=  SLICE_MAX_START_CODE)  
			{
		      int mb_y= start_code - SLICE_MIN_START_CODE;
                  if(  mb_y  == 0)            
                  	{
                      len2 = buf_ptr -  after_PIC_SC ;
		         memcpy(buf_ptr_2, after_PIC_SC, len2); 		  
                      buf_ptr_2 = buf_ptr_2 + len2;
                	}	  
                   else 
                   	{
                     memcpy(buf_ptr_2 , buf_ptr -4 , 4);   
			  buf_ptr_2 +=  4; 	 
		      }
                  need_modify_mv=0;
                  after_Slice_SC = buf_ptr;  
                   if(picture_structure == 3)    
                    {
                      ret = mpeg_decode_slice_Pic(mb_y, buf_ptr, input_size); 
                    }              
                   else
                    {
                    printf("\n\n  ¡´call mpeg_decode_slice_Field !!!!  \n");	 
                    ret = -1;    
                    }

                 if(ret < 0)
	      	 	 {
			  printf("ret < 0 !\n");	 
                     ret_error = 1;		   
			  goto process_end;
			  
                    } 	

                start_code = find_start_code(&buf_ptr, buf_end);

    	          if( start_code < 0)
                   {
                     //JF recover
                 }
                 else
                   {
                     buf_ptr  = buf_ptr  - 4 ; 
                   }

		if( need_modify_mv  == 0 ) 
 		{
		    len2 = buf_ptr -  after_Slice_SC ;
                 memcpy(buf_ptr_2, after_Slice_SC, len2); 	
		    buf_ptr_2 = buf_ptr_2 + len2;		
		}
	     else
	     	{
              ps_b  =  (buf_ptr -  after_Slice_SC) * 8 ;
              mpeg2_slice(ps_a,ps_b, buf_ptr); 
              int temp = pb.bit_left;

              switch(temp)
                 {
                     case  32:

				    buf_ptr_2 = 	pb.buf_ptr ;			
				 	 	
		 	  break;		 	                 
                     case  24:
                              buf_ptr_2 = 	pb.buf_ptr +  1 ;	
				    put_bits(&pb, 24 , 0); 								
		 	  break;		 	
			  case  16:
				    buf_ptr_2 = 	pb.buf_ptr +  2 ;	
				    put_bits(&pb, 16 , 0); 							
		 	  break;				  	
			  case    8:
				    buf_ptr_2 = 	pb.buf_ptr +  3 ;
				    put_bits(&pb, 8 , 0); 		
		 	  break;				  	
			  default:
#if 1  //JF 1110 2011
                                  buf_ptr_2 = 	pb.buf_ptr +  4 ;		
		                     put_bits(&pb, temp , 0);				
					 
#else
			     {
		            if (( temp <= 31 ) &&  (temp >=25 )	 )
		              {
		                     buf_ptr_2 = 	pb.buf_ptr +  1 ;
		                     put_bits(&pb, temp , 0);
		              }
		             if (( temp <= 23 ) &&  (temp >=17 )	 )
		               {
		                     buf_ptr_2 = 	pb.buf_ptr +  2 ;
		                     put_bits(&pb, temp , 0);							 
		                }
 		            if (( temp <= 15 ) &&  (temp >=9 )	 )
		               {
		                     buf_ptr_2 = 	pb.buf_ptr +  3 ;
		                    put_bits(&pb, temp , 0);						
							 
		               }
		 
		            if (( temp <= 7 ) &&  (temp >=1 )	 )
		               {
		                    buf_ptr_2 = 	pb.buf_ptr +  4 ;
		                    put_bits(&pb, temp , 0);			
							
		                }
		 
			  	}
			  
#endif //JF 1110 2011

		 	  break;				  	

                 }
			  

				
	     	}




                }
             }
			  
	  	   break; 
       }		


		
    } 

process_end:


         //JF 1027    if(  (ret_error  ==  0 )  && ( pict_type == 2 )  && ( c_m_v == 1))
	  if(  ret_error  ==  0 )	 	
         {  

           data_2[dist] =   ( (unsigned char) data_2[dist] ) & 0xdf  ;
	    len2 = buf_ptr_2 - data_2 ; 	  
          memcpy(   videobuffer, data_2, len2); 
          //JF 1024       len = len2;
          in_size = len2;
        }	


  } 

}   

#endif    


        if(demuxer->stream->type == STREAMTYPE_DVDNAV)
        {
            if(picture_coding_type == 1 || picture_coding_type == 2)
                dvdnav_should_do_alpha = 1;
            else if(picture_coding_type == 3)
                dvdnav_should_do_alpha = 0;
	    sh_video->frame_type = picture_coding_type;
        }
mpeg2_gop_pkt_cnts++;

#if 1	//Barry 2011-08-11
       #if 0  //Polun 2011-11-11 fixed mantis 6481 playback Formula.m2v Press FR key¡Atimebar display 00:00:00 second 
	if (demuxer->file_format==DEMUXER_TYPE_MPEG_ES && !pts && !sh_video->i_pts)
	{
		if (speed_mult >= 2)
			sh_video->pts += (sh_video->last_pts+1.0);
		else if (speed_mult < 0)
			sh_video->pts += (sh_video->last_pts+(-1.0*rewind_mult));
		else
		{
			if (MPEG_ES_START_PTS > 0.0)
			{
				sh_video->pts = MPEG_ES_START_PTS;
				MPEG_ES_START_PTS = 0.0;
			}
			else
				sh_video->pts+=frame_time;
		}
	}
       #else
	if (MPEG_ES_START_PTS > 0.0)
	{
		sh_video->pts = MPEG_ES_START_PTS;
		MPEG_ES_START_PTS = 0.0;
	}
       #endif
	else
        	sh_video->pts+=frame_time;
#else
        sh_video->pts+=frame_time;
#endif
check_new_pts = sh_video->pts;
        if(picture_coding_type==1)
            d_video->flags |= 1;

if(picture_coding_type<=2 && sh_video->i_pts) {
               if(demuxer->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
               {
                   if (picture_coding_type == 1 && pts > 0)
                   {
                       if (pts - sh_video->last_i_pts < 2)
                       {
                           double new_fts = sh_video->fps;
                           if (mpeg2_gop_pkt_cnts> 5)
                           {
                               new_fts = 1/((pts - sh_video->last_i_pts)/mpeg2_gop_pkt_cnts);
//printf("new_fts=%f sh_video->fps=%f\n", new_fts, sh_video->fps);
                               if (new_fts - sh_video->fps > 0.01)
                               {
                                   if (new_fts - sh_video->fps > 0.08)
                                       sh_video->fps += 0.05;
                                   else
                                       sh_video->fps += fabs(new_fts - sh_video->fps)/20;//0.01;
                                   sh_video->frametime=1.0/sh_video->fps;
//printf("0 fps=%f new_fts=%f  abs:%f\n", fps, new_fts, fabs(new_fts - sh_video->fps));
                               }
                               else if (new_fts - sh_video->fps < -0.01)
                               {
                                   if (new_fts - sh_video->fps < -0.08)
                                       sh_video->fps -= 0.05;
                                   else
                                       sh_video->fps -= fabs(new_fts - sh_video->fps)/20;//0.01;
                                   sh_video->frametime=1.0/sh_video->fps;
//printf("1 fps=%f new_fts=%f  new sh_video->fps = %f\n", fps, new_fts, sh_video->fps);
                               }
                               else
                               {
//printf("fps=%f new_fts=%f\n", fps, new_fts);
                                   if ( fabs(fps-new_fts)<=0.01)
                                       sh_video->fps = fps;
                                   sh_video->frametime=1.0/sh_video->fps;
                               }
                           }
                           mpeg2_gop_pkt_cnts = 0;
                       }

                       sh_video->last_i_pts = pts;
                       sh_video->pts = pts; ///
                       
                   }

                       if (pre_picture_coding_type == 1 && picture_coding_type == 1)
                       {
                               sh_video->pts=pts;
                               sh_video->i_pts=pts;
//                               printf("&&&&&&&  [%s - %d]  sh_pts = %f\n", _func_, _LINE_, sh_video->pts);
                       }
                       else
                       {
#if 0
                               sh_video->pts=sh_video->i_pts;
#endif
                               sh_video->i_pts=pts;
                       }
               }
               else
               {
                       sh_video->pts=sh_video->i_pts;
                       sh_video->i_pts=pts;
               }
       } else {
           if(pts) {
               if(picture_coding_type<=2)
               {
                   sh_video->i_pts=pts;
                   if (picture_coding_type == 1 && pts > 0)
                   {
                       if (pts - sh_video->last_i_pts < 2)
                       {
                           double new_fts = sh_video->fps;
                           if (mpeg2_gop_pkt_cnts> 5)
                           {
                               new_fts = 1/((pts - sh_video->last_i_pts)/mpeg2_gop_pkt_cnts);
//printf("new_fts=%f sh_video->fps=%f\n", new_fts, sh_video->fps);
                               if (new_fts - sh_video->fps > 0.01)
                               {
                                   if (new_fts - sh_video->fps > 0.08)
                                       sh_video->fps += 0.05;
                                   else
                                       sh_video->fps += fabs(new_fts - sh_video->fps)/20;//0.01;
                                   sh_video->frametime=1.0/sh_video->fps;
//printf("0 fps=%f new_fts=%f  abs:%f\n", fps, new_fts, fabs(new_fts - sh_video->fps));
                               }
                               else if (new_fts - sh_video->fps < -0.01)
                               {
                                   if (new_fts - sh_video->fps < -0.08)
                                       sh_video->fps -= 0.05;
                                   else
                                       sh_video->fps -= fabs(new_fts - sh_video->fps)/20;//0.01;
                                   sh_video->frametime=1.0/sh_video->fps;
//printf("1 fps=%f new_fts=%f  new sh_video->fps = %f\n", fps, new_fts, sh_video->fps);
                               }
                               else
                               {
//printf("fps=%f new_fts=%f\n", fps, new_fts);
                                   if ( fabs(fps-new_fts)<=0.01)
                                       sh_video->fps = fps;
                                   sh_video->frametime=1.0/sh_video->fps;
                               }
                           }
                           mpeg2_gop_pkt_cnts = 0;
                       }
                       sh_video->last_i_pts = pts;
                   }

                   if(demuxer->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && picture_coding_type == 1)
                   {
                           sh_video->pts = pts;
                           if (!pre_picture_coding_type)
                               pre_picture_coding_type=picture_coding_type;
//                                       printf("&&&&&&&  [%s - %d]  sh_pts = %f\n", _func_, _LINE_, sh_video->pts);
                   }
               }
               else
               {
                    if(demuxer->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                    {
                    }
                    else
                    {
                    //original
                    sh_video->pts=pts;
                    }
               }
           }
       }
       pre_picture_coding_type = picture_coding_type;

        //20110110 Robert need keep last_pts for new AV
		extern unsigned int no_osd;
        if(demuxer->file_format==DEMUXER_TYPE_MPEG_PS && demuxer->stream->type != STREAMTYPE_DVDNAV && !no_osd)
        {
        	if (sh_video->last_pts != MP_NOPTS_VALUE && sh_video->pts != MP_NOPTS_VALUE && sh_video->last_pts != 0.0 && sh_video->pts != 0.0 &&
			//(sh_video->pts - sh_video->last_pts) > 0 && (sh_video->pts - sh_video->last_pts) < 1)	//Barry 2011-02-18
			(sh_video->pts - sh_video->last_pts) > 0 )	//Barry 2011-02-18
				frame_time = sh_video->pts - sh_video->last_pts;

//20100613 Robert fix wrong video pts in MPEG_PS, ex: VOB
/*
            if (sh_video->i_pts==0 && sh_video->last_i_pts == sh_video->pts)// && (sh_video->pts - sh_video->i_pts) > 1)
            {
                if (check_new_pts > sh_video->pts)
                {
                    sh_video->pts = check_new_pts;
//printf(" ???02 sh_video->i_pts:%f sh_video->pts:%f check_new_pts:%f\n", sh_video->i_pts, sh_video->pts, check_new_pts);
                }
            }
*/
        }
        else if(demuxer->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
        {
//        	if (sh_video->last_pts != MP_NOPTS_VALUE && sh_video->pts != MP_NOPTS_VALUE && sh_video->last_pts != 0.0 && sh_video->pts != 0.0 &&
//			//(sh_video->pts - sh_video->last_pts) > 0 && (sh_video->pts - sh_video->last_pts) < 1)	//Barry 2011-02-18
//			(sh_video->pts - sh_video->last_pts) > 0 )	//Barry 2011-02-18
//				frame_time = sh_video->pts - sh_video->last_pts;

		if (ds_get_next_pts(d_video) != MP_NOPTS_VALUE && sh_video->pts != MP_NOPTS_VALUE && (ds_get_next_pts(d_video) - sh_video->pts) > 0
			 && (sh_video->pts == pts || sh_video->pts == sh_video->i_pts))
			frame_time = ds_get_next_pts(d_video) - sh_video->pts;
	#if 1	//Barry 2011-06-16
		if ( (frame_time > 2*sh_video->frametime) && (frame_time < 1.0) )
			frame_time = sh_video->frametime;
		if (frame_time < 0)
			frame_time = sh_video->frametime;
	#endif
//20100613 Robert fix wrong video pts in DVD
/*
            if (sh_video->i_pts > 0 && (sh_video->pts - sh_video->i_pts) > 1)
            {
                sh_video->pts = sh_video->i_pts;
//printf(" ???01 sh_video->i_pts:%f sh_video->pts:%f check_new_pts:%f\n", sh_video->i_pts, sh_video->pts, check_new_pts);
            }
            else if (check_new_pts > sh_video->pts)
            {
                sh_video->pts = check_new_pts;
            }
*/
//            if (sh_video->i_pts > 0 && (sh_video->pts - sh_video->i_pts) > 1)
/*
            if (sh_video->i_pts==0 && sh_video->last_i_pts == sh_video->pts)// && (sh_video->pts - sh_video->i_pts) > 1)
            {
                if (check_new_pts > sh_video->pts)
                {
                    sh_video->pts = check_new_pts;
printf(" ???02 sh_video->i_pts:%f sh_video->pts:%f check_new_pts:%f\n", sh_video->i_pts, sh_video->pts, check_new_pts);
                }
            }
*/
//printf(" --- check_new_pts:%f sh_video->pts:%f  lipts:%f  ipts:%f\n", check_new_pts, sh_video->pts, sh_video->last_i_pts, sh_video->i_pts);
        }


        //Barry 2010-08-06
        //if (check_fast_ts)	//Barry 2010-08-25 disable
        if(demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS)
        {
            if (sh_video->pts == fast_ts_pre_pts)
            {
                fast_ts_pts_cnt++;
                sh_video->pts += (sh_video->frametime * fast_ts_pts_cnt);
            }
            else
            {
                fast_ts_pre_pts = sh_video->pts;
                fast_ts_pts_cnt = 0;
            }
//		printf("sh_video->pts=%3.4f\n", sh_video->pts);
        }
    } else
    {
#ifdef FAST_TSDEMUX	//Barry 2010-07-02
        if( (demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && video_codec == VIDEO_H264 )
        {
            int i;
            if ( ts_avc1_pts_queue_cnt < (h264_reorder_num * (2-h264_frame_mbs_only)) )
            {
                sh_video->pts = 0;
                ts_avc1_pts_queue_cnt++;
	    }
            else
            {
		//print_pts(ts_avc1_pts_queue_cnt, ts_avc1_pts_queue);

                //Barry 2010-09-17
                //if ( ts_avc1_pts_queue_cnt && ((ts_avc1_pts_queue[0] - sh_video->last_pts) < -10) )
                if ( ts_avc1_pts_queue_cnt && sh_video->last_pts != 0.0 && ( ((ts_avc1_pts_queue[0] - sh_video->last_pts) < -10) || ((ts_avc1_pts_queue[0] - sh_video->last_pts) > 60)) )	//Barry 2011-07-14 fix mantis: 5440
                {
                    ts_avc1_pts_queue_cnt = 1;
                    sh_video->pts = 0;
                }
                else
                {
                    ts_avc1_pts_queue_cnt++;
                    sh_video->pts = ts_avc1_pts_queue[0];
                    for (i=0; i < ts_avc1_pts_queue_cnt-1; i++)
                        ts_avc1_pts_queue[i] = ts_avc1_pts_queue[i+1];
                    ts_avc1_pts_queue[ts_avc1_pts_queue_cnt-1] = 0;
                    ts_avc1_pts_queue_cnt--;

                    //Barry 2010-08-16
                    if (fast_ts_pre_pts == sh_video->pts)
                    {
                        fast_ts_pts_cnt++;
                        #if 0 //polun 2012-01-17 fixed FJ train_window_20110525.ts playback not smooth.
                        sh_video->pts += (sh_video->frametime * fast_ts_pts_cnt);
                        #else
                        sh_video->pts += (ts_h264_next_inc_pts * fast_ts_pts_cnt);
                        #endif
                    }
                    else
                    {
#if 1	//Barry 2011-03-30
                        if (fast_ts_pre_pts != 0.0 && fast_ts_pts_cnt > 8 && !hddvd_check)
                        {
                        	float hddvd_frametime = 0.0;
				hddvd_frametime = (sh_video->pts - fast_ts_pre_pts)/(float)(fast_ts_pts_cnt+1);
                        	if (hddvd_frametime > 0.041 && hddvd_frametime < 0.042)
                        	{
                        		printf("[%s - %d]    Real_FPS should be = 23.97 not %f\n", __func__, __LINE__, sh_video->fps);
					sh_video->fps = 23.97;
					sh_video->frametime = 1.0/sh_video->fps;
				}
                        	hddvd_check = 1;
			   }
#endif
                        fast_ts_pre_pts = sh_video->pts;
                        ts_h264_next_inc_pts = (avc1_next_pts -sh_video->pts)/(ts_avc1_pts_queue_cnt + 2) ; //polun 2012-01-17 fixed FJ train_window_20110525.ts playback not smooth.
                        //printf("sh_video->pts=%3.4f  ts_h264_next_inc_pts =%3.4f  ts_avc1_pts_queue_cnt = %d fast_ts_pts_cnt = %d\n", sh_video->pts,ts_h264_next_inc_pts,ts_avc1_pts_queue_cnt,fast_ts_pts_cnt);
                        fast_ts_pts_cnt = 0;
                    }
                }
            }

            //Barry 2010-08-16
            if (sh_video->last_pts != MP_NOPTS_VALUE && sh_video->pts != MP_NOPTS_VALUE &&
                    sh_video->last_pts != 0.0 && sh_video->pts != 0.0)
            {
                frame_time=sh_video->pts - sh_video->last_pts;
                //printf("new frame_time=%f  lp:%f  q0:%f\n", frame_time, sh_video->last_pts, ts_avc1_pts_queue[0]);
                if (frame_time < 0)
                    frame_time = 0;
            }

	    //20110715 charleslin fixed mantis 5393
	    //printf("%s:%d fps:%f h264_frame_mbs_only:%d\n", __func__, __LINE__, sh_video->fps, h264_frame_mbs_only);
	    if(sh_video->fps >= 50 && h264_frame_mbs_only){
		static unsigned int fcnt = 0;
		if(((frame_time + 0.01) / sh_video->frametime) >= 2.0)
		{
			//printf("%s:%d frame_time:%f sh_video->frametime:%f\n", __func__, __LINE__, frame_time, sh_video->frametime);
			fcnt++;
		}
		if(fcnt >= 10){
			sh_video->frametime = frame_time;
			sh_video->fps = 1.0/sh_video->frametime;
			if(fcnt == 10){
				printf("%s:%d incorrect frame time, modify it\n", __func__, __LINE__);
				printf("%s:%d fps:%f sh_video->frametime:%f\n", __func__, __LINE__, sh_video->fps, sh_video->frametime);
			}
		}
	    }
        }
        else
#endif

#if 1	//Barry 2011-03-30
        if( (demuxer->file_format==DEMUXER_TYPE_MPEG_TS || demuxer->file_format==DEMUXER_TYPE_SKYMPEG_TS) && video_codec == VIDEO_VC1 )
        {
		sh_video->pts=d_video->pts;
        	if (fast_ts_pre_pts == d_video->pts)
        	{
        		fast_ts_pts_cnt++;
			sh_video->pts += (sh_video->frametime * fast_ts_pts_cnt);
			if (frame_time <= 0.0 || frame_time > 0.3)
				frame_time = sh_video->frametime;
        	}
		else
		{
			if ((correct_sub_pts != 0.0) && (correct_sub_pts != fast_ts_pre_pts) && (fast_ts_pre_pts != 0.0) && fast_ts_pts_cnt > 8 && !hddvd_check)
			{
				float hddvd_frametime = 0.0;
				hddvd_frametime = (d_video->pts - fast_ts_pre_pts)/(float)(fast_ts_pts_cnt+1);
                        	if (hddvd_frametime > 0.041 && hddvd_frametime < 0.042)
                        	{
                        		printf("[%s - %d]    Real_FPS should be = 23.97 not %f\n", __func__, __LINE__, sh_video->fps);
					sh_video->fps = 23.97;
					sh_video->frametime = 1.0/sh_video->fps;
					frame_time = sh_video->frametime;
				}
				hddvd_check = 1;
			}
			else
			{
				if ( hddvd_check && (frame_time <= 0.0 || frame_time > 0.1) )
					frame_time = sh_video->frametime;
                             //Polun 2011-06-17 ++s  for mantis4973 frametime not correction  
                             if(frame_time >=  (1.98 * sh_video->frametime))
                                   frame_time = sh_video->frametime;
                             if(frame_time < 0)
                                   frame_time = sh_video->frametime;
                             //Polun 2011-06-17 ++e 
				fast_ts_pre_pts = d_video->pts;
				fast_ts_pts_cnt = 0;
			}
		}
        	
        }
        else
#endif
            sh_video->pts=d_video->pts;
    }

#if 0	//Barry 2011-01-18 video_read_frame debug message
    int jj;
    unsigned char* buf = *start;

    //for (jj=0;jj<64;jj++)
    for (jj=0; jj<in_size-4; jj++)
    {
        if (buf[jj]==0 && buf[jj+1]==0 && buf[jj+2]==1)
            printf("00 00 01 %.2X\n",  buf[jj+3]);
        //printf("%.2X ", buf[jj]);
    }
    printf("===>  in_size = %d\n\n", in_size);
//	printf("sh_video->fps=%f,      sh_video->pts=%f\n", sh_video->fps, sh_video->pts);
#endif
    if(frame_time_ptr) *frame_time_ptr=frame_time;
    sky_telecine = telecine;
    return in_size;
}
