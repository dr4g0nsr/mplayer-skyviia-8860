/*
 * Demultiplexer for MPEG2 Transport Streams.
 *
 * Written by Nico <nsabbi@libero.it>
 * Kind feedback is appreciated; 'sucks' and alike is not.
 * Originally based on demux_pva.c written by Matteo Giani and FFmpeg (libavformat) sources
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "parse_es.h"
#include "stheader.h"
#include "ms_hdr.h"
#include "mpeg_hdr.h"
#include "demux_ts.h"

//#define DEBUG_TIMER
#ifdef DYNAMIC_CALCULATED_PTS
static char check_fps_count = 0;
char need_do_dynamic_pts = 0;
#define MAX_CHECK_DYNAMIC_COUNT 5
#endif /* end of DYNAMIC_CALCULATED_PTS */
typedef struct {
	int32_t atype, vtype, stype;	//types
	int32_t apid, vpid, spid;	//stream ids
	char slang[4], alang[4];	//languages
	uint16_t prog;
	off_t probe;
} tsdemux_init_t;
#ifdef CHECK_AUDIO_IS_VALID
//#define MAX_CHECK_AUDIO_EXIST	5000 /* check audio or video exist, 5000 * 184 * priv->pmt_cnt * priv->pmt[ii].es_cnt */
#define MAX_CHECK_AUDIO_EXIST	6000 /* check audio or video exist, 5000 * 184 * priv->pmt_cnt * priv->pmt[ii].es_cnt */
#define MAX_CHECK_SUB_EXIST MAX_CHECK_AUDIO_EXIST*1000
#define check_video_status check_audio_is_exist
#define check_sub_status check_audio_is_exist
#define FIRST_VIDEO_PMT_NUMBER	0
#define FIRST_VIDEO_PMT_ES_NUMBER	1
#define FIRST_AUDIO_PMT_NUMBER	2
#define FIRST_AUDIO_PMT_ES_NUMBER	3
#define FIRST_SUB_PMT_NUMBER	4
#define FIRST_SUB_PMT_ES_NUMBER	5
#define NOW_VIDEO_PMT_NUMBER	6
#define NOW_VIDEO_PMT_ES_NUMBER	7
#define NOW_AUDIO_PMT_NUMBER	8
#define NOW_AUDIO_PMT_ES_NUMBER	9
#define NOW_SUB_PMT_NUMBER		10
#define NOW_SUB_PMT_ES_NUMBER	11
#ifndef CHECK_EXIST_FLAGS
static int inline check_audio_is_exist(int pid, int check_count, demuxer_t *demuxer);
#endif /* end of  ifndef CHECK_EXIST_FLAGS */
static int inline find_out_stream(ts_priv_t *priv, tsdemux_init_t *param, demuxer_t *demuxer);
static int inline select_stream(ts_priv_t *priv, tsdemux_init_t *param, demuxer_t *demuxer, int type);
#endif // end of CHECK_AUDIO_IS_VALID
#define MIN_PES_LENGTH	6	
#define MIN_PES_SUB_LENGTH	15


#ifdef BUGGY_FIXED_DTS_PROBLEM
unsigned char Check_Audio_PTS_Zero = 1;
unsigned char Got_Special_DTS_CASE = 0;
#endif // end of BUGGY_FIXED_DTS_PROBLEM
#ifdef STREAM_DIO_AUTO
extern void dio_enable(stream_t *s);
#endif /* end of STREAM_DIO_AUTO */		
#ifdef FAST_TSDEMUX	//Barry 2010-11-23
void h264bsdDecodeExpGolombUnsigned(unsigned char *pStrmData, unsigned int *codeNum);
#endif /* end of FAST_TSDEMUX */

int ts_prog;
int ts_keep_broken=0;
off_t ts_probe = 0;
int audio_substream_id = -1;
extern char *dvdsub_lang, *audio_lang;	//for -alang
extern int dvdsub_id;
extern unsigned int no_osd; 

extern unsigned int dts_packet;
//Barry 2010-08-11
extern int ts_avc1_pts_queue_cnt;
extern float *ts_avc1_pts_queue;
extern int num_ref_frames;
extern int h264_reorder_num;
extern int h264_frame_mbs_only;
extern int check_fast_ts_startcode;
int encrypted_ts_check = 1;	//Barry 2010-10-12
//Barry 2010-11-11
static int check_audio_bps_len_cnt = 0;
static float check_audio_bps_diff_cnt = 0;
extern int h264_fast_ts_seek;	//Barry 2010-11-23
//Barry 2011-04-24
extern int hddvd_check;
double pre_packet_diff_pts = 0.0;

#ifdef ADJUST_STREAM_PTS	//Barry 2010-01-07
extern int first_set_sub_pts;
extern double correct_sub_pts;
#endif
extern unsigned int sky_hwac3;

static double last_video_pts = 0.0;

double ts_file_duration = 0.0; //Polun 2012-01-05 fixed mantis 6771 duration issue

#ifdef SUPPORT_QT_BD_ISO_ENHANCE
static int found_keyframe_seek = 1;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
extern int use_PP_buf;
extern unsigned int VideoBufferIdx;
extern int vol_blksz;

#ifdef QT_SUPPORT_DVBT //maybe not used
typedef struct {
	es_stream_type_t type;
	ts_section_t section;
} TS_pids_t;
#endif /* end of QT_SUPPORT_DVBT */

#ifdef SUPPORT_SS_FINE_TUNE
static off_t per_second_pos = 0;
static double first_video_pts = 0.0;
static double first_audio_pts = 0.0;
static int first_video_pid = 0;
static int first_audio_pid = 0;
#define MAX_SEEK_LOOP_TIME 50
#define THE_SEEK_FINE_TUNE_PTS 2.0
#define CHECK_LAST_OFFSET		0x200000 //2M
#define CHECK_LAST_OFFSET_NETWORK 	0x80000 //512K
#define KEEP_RUNNING	1
#define CHECK_PTS_BY_AUDIO	2
#define CHECK_PTS_BY_VIDEO	3
/* for probe */
#define PTS_MAX_NUMBER	95443.717689f
//#define PTS_MAX_NUMBER	((double)((unsigned long long )1<<34 - 1) / 90000.0 )
/* for SW SS */
#define SS_CHECK_PTS	2
#endif /* end of SUPPORT_SS_FINE_TUNE */
#ifdef BD_CLIP_BASE_PTS
#if 0 /* move the below code to libmpdemux/demux_ts.h */
#include "libbluray/bluray.h"
#include "libbluray/bdnav/mpls_parse.h"
struct bd_priv_t {
	int iso;
	int title;
	int chapter;
	int m2ts;
	int clip_idx;
	uint64_t clip_len;
	uint64_t clip_pos;
	char *device;
	BLURAY *bd;
	BLURAY_TITLE_INFO *title_info;
	MPLS_PL *play_list;
	uint64_t *clip_list;
	UDF_FILE_BLOCK *segments;
};
#endif
extern double bd_now_base_pts;
extern double *list_base_time[3] ;
static void correct_bd_iso_timebar(demuxer_t *demuxer);
static inline void adjust_bd_pts(demux_packet_t *dp, demuxer_t *demuxer);
static inline void adjust_bd_pts(demux_packet_t *dp, demuxer_t *demuxer)
{
	struct bd_priv_t *bdpriv = demuxer->stream->priv;
	int in_number = 0;
	int out_number = 0;
	/* We don't need handle continuous pts Ex: Inception.iso */
	if (bdpriv && bdpriv->clip_idx > 0 && bdpriv->timebar_type != BDISO_TIMEBRR_TYPE_CON_TIME)
	{
		if (bdpriv->clip_pos < 0xA00000)
		{
			/*
			 * 1. dp->pts < list_base_time[0][bdpriv->clip_idx] ==> mean previous file packet (Case 2)
			 * 2. dp->pts <= list_base_time[0][bdpriv->clip_idx] + 10.0 ==> mean current file packet (case 3)
			 */
			if (bdpriv->timebar_type == BDISO_TIMEBRR_TYPE_FIXED_INTIME)
			{
				bdiso_debug("@@@ In [%s][%d] type[%d] clip_idx[%d] pts[%f] ", __func__, __LINE__, bdpriv->timebar_type, bdpriv->clip_idx, dp->pts);
				if (dp->pts < (list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + 10.0))
				{
					/* Mean current clip packet */
					dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-1];
				}
				else
				{
					/* Mean previous clip packet */
					if (bdpriv->clip_idx > 1)
						dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx-1] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-2];
				}
				bdiso_debug(" ###newpts [%f]###\n", dp->pts);
			}
			else
			{
				/* bdpriv->timebar_type is BDISO_TIMEBRR_TYPE_RANDOM_TIME */
				if (dp->pts < list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx])
				{
					bdiso_debug("@@@ In [%s][%d] dp->pts is [%f] previous_out [%f]@@@", __func__, __LINE__, dp->pts, list_base_time[BD_ISO_OUT_TIME][bdpriv->clip_idx-1]);
					/* Previous clip packet */
					/* BEST OF EUROPE ITALY.ISO will be */
					if (dp->pts < (list_base_time[BD_ISO_OUT_TIME][bdpriv->clip_idx-1] - 0.001))
					{
						if (bdpriv->clip_idx > 1)
							dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx-1] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-2];
					}
					else
							dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-1];
						
					bdiso_debug("new pts[%f] ###\n", dp->pts);
				}
				else
				{
					/* Some file, the out_time will be over defined out_time, like TRUE TREASURES EUROPE.ISO, over time is 0.02418*/
					if (dp->pts < (list_base_time[BD_ISO_OUT_TIME][bdpriv->clip_idx-1] + 0.05))
					{
						if (dp->pts <= (list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + 10.0))
						{
							bdiso_debug("@@@ In [%s][%d] dp->pts[%f] - [%f] + [%f] will be [%f] @@@", __func__, __LINE__, dp->pts, list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx], list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-1], dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-1]);
							/* Case 3, this is current clip packet */
							dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-1];
							bdiso_debug("new pts[%f] ###\n", dp->pts);
						}
						else
						{
							bdiso_debug("@@@ In [%s][%d] dp->pts[%f] - [%f] + [%f] @@@", __func__, __LINE__, dp->pts, list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx-1], list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-2]);
							/* Case 3, this is previous clip packet */
							if (bdpriv->clip_idx > 1)
								dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx-1] + list_base_time[BD_ISO_BASE_TIME][bdpriv->clip_idx-2];
							bdiso_debug("new pts[%f] ###\n", dp->pts);
						}
					}
					else
					{
						//printf("#### Carlos in [%s][%d] clip_count is [%d] ####\n", __func__, __LINE__, bdpriv->title_info->clip_count);
						//printf("#### Carlos in [%s][%d] bdpriv [%p] bdpriv->title_info[%p] ####\n", __func__, __LINE__, bdpriv, bdpriv->title_info);
						if (bdpriv->clip_idx == bdpriv->title_info->clip_count && bdpriv->clip_idx > 2)
						{
							in_number = bdpriv->clip_idx - 2;
							out_number = bdpriv->clip_idx - 3;
							/* Special case, video has 0.5 sec, but audio has 30sec, so we need check BD_ISO_IN_TIME */
							if (list_base_time[BD_ISO_IN_TIME][in_number] > dp->pts)
							{
								in_number = bdpriv->clip_idx - 1;
								out_number = bdpriv->clip_idx - 1;
							}
						}
						else
						{
							in_number = bdpriv->clip_idx ;
							out_number = bdpriv->clip_idx-1;
						}
						bdiso_debug("@@@ In [%s][%d] dp->pts is [%f] list_base_time[0][%d][%f] base[%f]@@@", __func__, __LINE__, dp->pts, bdpriv->clip_idx, list_base_time[BD_ISO_IN_TIME][in_number], list_base_time[BD_ISO_BASE_TIME][out_number]);
						dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][in_number]+ list_base_time[BD_ISO_BASE_TIME][out_number];
						bdiso_debug("new pts[%f] ###\n", dp->pts);
					}
				}
			}

		}
		else
		{
			if (bdpriv->clip_idx == bdpriv->title_info->clip_count && bdpriv->clip_idx > 1)
			{
				in_number = bdpriv->clip_idx - 1;
				out_number = bdpriv->clip_idx - 2;
			}
			else
			{
				in_number = bdpriv->clip_idx ;
				out_number = bdpriv->clip_idx - 1;
			}
			//printf("@@@ In [%s][%d] clip_idx[%d] dp->pts [%f] in_time[%f] out_time[%f] @@@", __func__, __LINE__, bdpriv->clip_idx, dp->pts, list_base_time[BD_ISO_IN_TIME][in_number],list_base_time[BD_ISO_BASE_TIME][out_number]);
			dp->pts = dp->pts - list_base_time[BD_ISO_IN_TIME][in_number] + list_base_time[BD_ISO_BASE_TIME][out_number];
			//printf("## new->pts[%f]##\n", dp->pts);
		}
	}
}
#endif /* end of BD_CLIP_BASE_PTS */

#ifdef HW_TS_DEMUX
#include <pthread.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h> // for mapp
#include "stream/dvb/dmx.h"
#include "stream/skydvbin.h"
#include <signal.h>
#include <fcntl.h>
#include "pthread-macro.h"
/* Hwts Define */
#define	DMX_BUFFER_FLUSHING		(1<<0)
/* move below define to stream/dvb/dmx.h */
#if 0
#define DMX_METER_BUFFER		_IOR('o', 54, struct dmx_buffer_meter)
#define DMX_DVR_MAP_MEM			_IOWR('o', 58, struct dvr_map_info)
#define DMX_PAUSE_FILTER		_IO('o', 59)
#define DMX_RESUME_FILTER		_IO('o', 60)
#define DMX_SET_AXI_SPEED       _IO('o', 61)
#endif

#define OVER_QUEUE_USLEEP_TIME	8000
#define MAX_QUEUE    50 
#define MAX_VIDEO_QUEUE	150
#define WRITE_FINISHED      2
/* BUFFER_COUNT * TS_BUF_LENGTH ==> 1MB is better */
#define BUFFER_COUNT	4
#define TS_BUF_LENGTH     0x40000 //256KB
#define DEBUG_COUNT		600
#define DVR_DEVICE	1
#define TS_DEVICE	2
#define TS_SEEK_START	1
#define TS_SEEK_END		0
#define PES_BUF_LENGTH	1024*1024	// 1MB, 20100811, fixed for Shanghai demo, HD20081210031522.ts
#define MAX_RETRY	1000
#define MAX_FILL_VIDEOBUFFER_COUNT		3
#define START_DELAY_QUEUE	15
#define MAX_KERNEL_BUFFER_QUEUE	START_DELAY_QUEUE*2
#define FILTER_SUB             0x04
#define FILTER_AUDIO	0x02
#define FILTER_VIDEO	0x01
//#define UNKNOWN			0
#define FILTER_DVB		0x08
#define ONETIME_MAXREAD_VIDEO_PACKET	3	/* change this value from 5 to 3, for speed up */
#define ONETIME_MAXREAD_AUDIO_PACKET	3	/* change this value from 5 to 3, for speed up */
#define ONETIME_MAXREAD_SUB_PACKET     5
/* For QT BD-iso enhance performance */
#define ONETIME_BD_MAXREAD_AUDIO_PACKET	5
#define ONETIME_BD_MAXREAD_VIDEO_PACKET	1//5
#define ONETIME_BD_MAXREAD_SUB_PACKET	5
//#define WRITE_STOP_CHECK /* Enable this define, avoid kernel space flush video or audio buffer */
//#define HWTS_USED_FRAMEBUFFER /* Enable this define, Kernel need increase uncached buffer */ // move this define to config.h
#define KEEP_FILTER_RUNNING	/* If enable this define, using pause and resume to replace stop and start when operator seek function */
#define HWTS_WAKEUP /* 2010-10-09 add, reduce cpu when buffer is full */
#define DROP_GARBAGE_DATA /* Deep Blue-1080P-H264-DTS6ch-TS.ts has 3 bytes garbage, payload_size is correct pes-raw data */

#define USE_PP_BUF3_CNTS 0
#define MAX_QUICK_BD_STARTUP_CNTS 60

#define CALCULATE_PTS(a, data)	 {	\
			if (data[7] & 0x80) \
			{	\
				int64_t pts;	\
				pts = (int64_t)(data[9] & 0x0E) << 29 ;	\
				pts |= data[10]         << 22 ;	\
				pts |= (data[11] & 0xFE) << 14 ;	\
				pts |= data[12]         <<  7 ;	\
				pts |= (data[13] & 0xFE) >>  1 ;	\
				a = pts / 90000.0f;	\
			}	\
			else	\
				a = MP_NOPTS_VALUE;	\
			}

#define GET_VIDEO_FORMAT(a, ptr)	\
	{	\
		if (ptr)	\
		{	\
			if (ptr->format == VIDEO_H264)	\
				a = DMX_VIDEO_H264;	\
			else if (ptr->format == VIDEO_VC1)	\
				a = DMX_VIDEO_VC1;	\
			else if (ptr->format == VIDEO_MPEG4)	\
				a = DMX_VIDEO_MPEG4;	\
			else	\
				a = DMX_VIDEO_MPEG2;	/*default set to mpeg2 */ \
		}	\
	}
struct dmx_buffer_meter {
	int			queue_pkt_cnt;
	__u32		spare_size;	
	__u32		flags;
} __attribute__((packed));
#ifndef HWTS_USED_FRAMEBUFFER
struct dvr_map_info
{
       __u32                           *dvr_map_addr;
       __u32                           map_size;
}__attribute__((packed));
#endif /* end of if not define HWTS_USED_FRAMEBUFFER */

/* Carlos add local function */
static void skydvb_write_stream(demuxer_t * demuxer);
static int open_ts_device(ts_priv_t * priv, int type);
static int hw_ts_parse(demuxer_t *demuxer, ES_stream_t *es, ts_priv_t* priv, demux_stream_t *current_ds, int retry_count);
static inline int check_packets(int fd);
static inline int add_audio_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, sh_audio_t *sh, int count, demuxer_t *demuxer, char filter_type);
static inline int add_video_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, sh_video_t *sh, int count, demuxer_t *demuxer, TS_stream_info *si, char filter_type, int fe_fd);
#ifdef DVB_SUBTITLES
static inline int add_sub_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, int count, demuxer_t *demuxer, char filter_type);
#endif // end of DVB_SUBTITLES

static void hw_io_write( int signo, siginfo_t *info, void *context );
/* Extern value or function */
extern int dvb_set_tsout_filt(int fd, uint16_t pid, dmx_pes_type_t pestype, int video_flag);
extern int dvb_demux_stop(int fd);
extern int hwtsdemux;
extern int skydroid;
/* Carlos add static parameter */
static char ts_seek_status = TS_SEEK_END;	
static pthread_mutex_t ts_seekmtx;
static pthread_cond_t fillcond;
static pthread_mutex_t fillmtx;
static int self_write=1;
static int wait_late = 0; // add by carlos 2010-09-10, record we do not write data ready, need write again
unsigned char read_toggle = 0;
unsigned char write_toggle = 0;
static int dvr_fd = 0;
static unsigned char *ts_buf = NULL;
int write_toggle_len[BUFFER_COUNT]={0};
static unsigned char *buf_bk = NULL;
static pthread_t skydvb_write_thread;
static char start_write_thread = HWTS_WRITE_STREAM_SLEEP;
#define THREAD_STATUS_IDLE		0x00
#define THREAD_STATUS_RUNNING	0x01
#define THREAD_STATUS_END		0x02
#define MAX_WAITING_THREAD_READY_COUNT	100	
static char status_write_thread = THREAD_STATUS_IDLE;
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
static pthread_mutex_t ts_switch;
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
#ifdef SUPPORT_NETWORKING_HWTS
extern int hwtsnet;
#endif /* end of SUPPORT_NETWORKING_HWTS */
#endif // end of HW_TS_DEMUX

#ifdef DVBT_USING_NORMAL_METHOD
extern int skydvb_thread_running;
#endif // end of DVBT_USING_NORMAL_METHOD

//#define IS_AUDIO(x) (((x) == AUDIO_MP2) || ((x) == AUDIO_A52) || ((x) == AUDIO_LPCM_BE) || ((x) == AUDIO_AAC) || ((x) == AUDIO_DTS) || ((x) == AUDIO_TRUEHD) || ())
#define IS_AUDIO(x) (((x) == AUDIO_MP2) || ((x) == AUDIO_A52) || ((x) == AUDIO_BPCM) || ((x) == AUDIO_AAC) || ((x) == AUDIO_DTS) || ((x) == AUDIO_TRUEHD) || ((x) == AUDIO_LPCM_BE))
#define IS_VIDEO(x) (((x) == VIDEO_MPEG1) || ((x) == VIDEO_MPEG2) || ((x) == VIDEO_MPEG4) || ((x) == VIDEO_H264) || ((x) == VIDEO_AVC)  || ((x) == VIDEO_VC1))
#ifdef DVB_SUBTITLES
#define IS_SUB(x) (((x) == SPU_DVD) || ((x) == SPU_DVB) || ((x) == SPU_TELETEXT) || ((x) == SPU_PGS))	//Fuchun 2010.09.28
//#define IS_SUB(x) (((x) == SPU_DVD) || ((x) == SPU_DVB) || ((x) == SPU_PGS))
#else
#define IS_SUB(x) (((x) == SPU_DVD) || ((x) == SPU_DVB) || ((x) == SPU_TELETEXT))
#endif
#define MAX_CHECK_COUNT	1*1024*1024	 // reduce check ts packet size, if don't find ts start code over 1MB, we make sure the data is wrong
#define MPLAYER_NOT_READY_TYPE_HANDLE	// Set the not_support area, add by carlos 2010-10-20
#define SUPPORT_ERROR_TYPE_CHECK /* Fixed error type from PMT table, set to audio type to DTS when pmt described AC52 : (TS)? å??…_hotel california_1080i_?¨_DTS??ts and CHD?”ç?.?³ä??ƒç?@HDTV DTSè¯•æœºç¢?VOL 1.ts */
#ifdef CHECK_EXIST_FLAGS
static inline void check_exist_from_pmt(ts_priv_t *priv, int pid, uint8_t flag);
#endif // end of CHECK_EXIST_FLAGS

static int ts_parse(demuxer_t *demuxer, ES_stream_t *es, unsigned char *packet, int probe);

static uint8_t get_packet_size(const unsigned char *buf, int size)
{
	int i;

	if (size < (TS_FEC_PACKET_SIZE * NUM_CONSECUTIVE_TS_PACKETS))
		return 0;

	for(i=0; i<NUM_CONSECUTIVE_TS_PACKETS; i++)
	{
		if (buf[i * TS_PACKET_SIZE] != 0x47)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "GET_PACKET_SIZE, pos %d, char: %2x\n", i, buf[i * TS_PACKET_SIZE]);
			goto try_fec;
		}
	}
	return TS_PACKET_SIZE;

try_fec:
	for(i=0; i<NUM_CONSECUTIVE_TS_PACKETS; i++)
	{
		if (buf[i * TS_FEC_PACKET_SIZE] != 0x47){
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "GET_PACKET_SIZE, pos %d, char: %2x\n", i, buf[i * TS_PACKET_SIZE]);
			goto try_philips;
		}
	}
	return TS_FEC_PACKET_SIZE;

 try_philips:
	for(i=0; i<NUM_CONSECUTIVE_TS_PACKETS; i++)
	{
		if (buf[i * TS_PH_PACKET_SIZE] != 0x47)
		return 0;
	}
	return TS_PH_PACKET_SIZE;
}

static int parse_avc_sps(uint8_t *buf, int len, int *w, int *h);
static inline uint8_t *pid_lang_from_pmt(ts_priv_t *priv, int pid);

static void ts_add_stream(demuxer_t * demuxer, ES_stream_t *es)
{
	int i;
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;

	if(priv->ts.streams[es->pid].sh)
		return;

	if((IS_AUDIO(es->type) || IS_AUDIO(es->subtype)) && priv->last_aid+1 < MAX_A_STREAMS)
	{
		sh_audio_t *sh = new_sh_audio_aid(demuxer, priv->last_aid, es->pid);
		if(sh)
		{
			const char *lang = pid_lang_from_pmt(priv, es->pid);
			sh->needs_parsing = 1;
			sh->format = IS_AUDIO(es->type) ? es->type : es->subtype;
			sh->ds = demuxer->audio;

			priv->ts.streams[es->pid].id = priv->last_aid;
			priv->ts.streams[es->pid].sh = sh;
			priv->ts.streams[es->pid].type = TYPE_AUDIO;
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nADDED AUDIO PID %d, type: %x stream n. %d\r\n", es->pid, sh->format, priv->last_aid);
			if (lang && lang[0])
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_AID_%d_LANG=%s\n", es->pid, lang);
				//sh->lang = lang;
				sh->lang = strdup(lang); /* Fixed free invalid address problem, carlos add 2010-11-09 */
			}
			priv->last_aid++;
		}

		if(es->extradata && es->extradata_len)
		{
			sh->wf = malloc(sizeof (WAVEFORMATEX) + es->extradata_len);
			sh->wf->cbSize = es->extradata_len;
			memcpy(sh->wf + 1, es->extradata, es->extradata_len);
		}
		return;
	}

	if((IS_VIDEO(es->type) || IS_VIDEO(es->subtype)) && priv->last_vid+1 < MAX_V_STREAMS)
	{
		sh_video_t *sh = new_sh_video_vid(demuxer, priv->last_vid, es->pid);
		if(sh)
		{
			sh->format = IS_VIDEO(es->type) ? es->type : es->subtype;
			sh->ds = demuxer->video;

			priv->ts.streams[es->pid].id = priv->last_vid;
			priv->ts.streams[es->pid].sh = sh;
			priv->ts.streams[es->pid].type = TYPE_VIDEO;
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nADDED VIDEO PID %d, type: %x stream n. %d\r\n", es->pid, sh->format, priv->last_vid);
			priv->last_vid++;


			if(sh->format == VIDEO_AVC && es->extradata && es->extradata_len)
			{
				int w = 0, h = 0;
				sh->bih = calloc(1, sizeof(BITMAPINFOHEADER) + es->extradata_len);
				sh->bih->biSize= sizeof(BITMAPINFOHEADER) + es->extradata_len;
				sh->bih->biCompression = sh->format;
				memcpy(sh->bih + 1, es->extradata, es->extradata_len);
				mp_msg(MSGT_DEMUXER,MSGL_DBG2, "EXTRADATA(%d BYTES): \n", es->extradata_len);
				for(i = 0;i < es->extradata_len; i++)
					mp_msg(MSGT_DEMUXER,MSGL_DBG2, "%02x ", (int) es->extradata[i]);
				mp_msg(MSGT_DEMUXER,MSGL_DBG2,"\n");
				if(parse_avc_sps(es->extradata, es->extradata_len, &w, &h))
				{
					sh->bih->biWidth = w;
					sh->bih->biHeight = h;
				}
			}
		}
		return;
	}

#ifdef DVB_SUBTITLES
	if(IS_SUB(es->type) && priv->last_sid+1 < MAX_S_STREAMS)	//Fuchun 2010.09.28
	{
		sh_sub_t *sh = new_sh_sub_sid(demuxer, priv->last_sid, es->pid);
 		if (sh) {
			const char *lang = pid_lang_from_pmt(priv, es->pid);
			/* Record the subtitle format, 2011-01-17 */
			sh->format = es->type;
			switch (es->type) {
			case SPU_DVD:
				sh->type = 'v'; break;
			case SPU_PGS:
				sh->type = 'p'; break;
			case SPU_TELETEXT:
				sh->type = 'd'; break;
			case SPU_DVB:
				sh->type = 'b'; break;
        		}
			if (lang && lang[0])
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_SID_%d_LANG=%s\n", es->pid, lang);
				//sh->lang = lang;
				sh->lang = strdup(lang); /* Fixed free invalid address problem, carlos add 2010-11-09 */
			}
			priv->ts.streams[es->pid].id = priv->last_aid;
			priv->ts.streams[es->pid].sh = sh;
			priv->ts.streams[es->pid].type = TYPE_SUB;
			priv->last_sid++;
		}
		return;
	}
#endif //end of DVB_SUBTITLES
}

static int ts_check_file(demuxer_t * demuxer)
{
	const int buf_size = (TS_FEC_PACKET_SIZE * NUM_CONSECUTIVE_TS_PACKETS);
	unsigned char buf[TS_FEC_PACKET_SIZE * NUM_CONSECUTIVE_TS_PACKETS], done = 0, *ptr;
	uint32_t _read, i, count = 0, is_ts;
	int cc[NB_PID_MAX], last_cc[NB_PID_MAX], pid, cc_ok, c, good, bad;
	uint8_t size = 0;
	off_t pos = 0;
	off_t init_pos;

	mp_msg(MSGT_DEMUX, MSGL_V, "Checking for MPEG-TS...\n");

	init_pos = stream_tell(demuxer->stream);
	is_ts = 0;
	while(! done)
	{
		i = 1;
		c = 0;

		while(((c=stream_read_char(demuxer->stream)) != 0x47)
			&& (c >= 0)
			&& (i < MAX_CHECK_SIZE)
			&& ! demuxer->stream->eof
		) i++;


		if(c != 0x47)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "THIS DOESN'T LOOK LIKE AN MPEG-TS FILE!\n");
			is_ts = 0;
			done = 1;
			continue;
		}

		pos = stream_tell(demuxer->stream) - 1;
		buf[0] = c;
		_read = stream_read(demuxer->stream, &buf[1], buf_size-1);

		if(_read < buf_size-1)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "COULDN'T READ ENOUGH DATA, EXITING TS_CHECK\n");
			stream_reset(demuxer->stream);
			return 0;
		}

		size = get_packet_size(buf, buf_size);
		if(size)
		{
            if(size == TS2_PACKET_SIZE)
                ts_version = TS_2;
            else
				ts_version = TS_1;

			done = 1;
			is_ts = 1;
		}

		if(pos - init_pos >= MAX_CHECK_SIZE)
		{
			done = 1;
			is_ts = 0;
		}
	}

	mp_msg(MSGT_DEMUX, MSGL_V, "TRIED UP TO POSITION %"PRIu64", FOUND %x, packet_size= %d, SEEMS A TS? %d\n", (uint64_t) pos, c, size, is_ts);
	stream_seek(demuxer->stream, pos);

	if(! is_ts)
	  return 0;

	//LET'S CHECK continuity counters
	good = bad = 0;
	for(count = 0; count < NB_PID_MAX; count++)
	{
		cc[count] = last_cc[count] = -1;
	}

	for(count = 0; count < NUM_CONSECUTIVE_TS_PACKETS; count++)
	{
		ptr = &(buf[size * count]);
		pid = ((ptr[1] & 0x1f) << 8) | ptr[2];
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "BUF: %02x %02x %02x %02x, PID %d, SIZE: %d \n",
		ptr[0], ptr[1], ptr[2], ptr[3], pid, size);

		if((pid == 8191) || (pid < 16))
			continue;

		cc[pid] = (ptr[3] & 0xf);
		cc_ok = (last_cc[pid] < 0) || ((((last_cc[pid] + 1) & 0x0f) == cc[pid]));
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "PID %d, COMPARE CC %d AND LAST_CC %d\n", pid, cc[pid], last_cc[pid]);
		if(! cc_ok)
			//return 0;
			bad++;
		else
			good++;

		last_cc[pid] = cc[pid];
	}

	mp_msg(MSGT_DEMUX, MSGL_V, "GOOD CC: %d, BAD CC: %d\n", good, bad);

	/* fixed Final Fantasy XIII Final Trailer HD 1080P-Chinese Subtitles.ts choose demxuer lavf , mantis: 5570*/
	//if(good >= bad)
	if(good >= bad || ((good+bad) >= (NUM_CONSECUTIVE_TS_PACKETS/2)))
			return size;
	else
	{
			mp_msg(MSGT_DEMUX, MSGL_INFO, "Check Mpeg-ts failed, Good CC: %d, BAD CC: %d\n", good, bad);
			return 0;
	}
}


static inline int32_t progid_idx_in_pmt(ts_priv_t *priv, uint16_t progid)
{
	int x;

	if(priv->pmt == NULL)
		return -1;

	for(x = 0; x < priv->pmt_cnt; x++)
	{
		if(priv->pmt[x].progid == progid)
			return x;
	}

	return -1;
}


static inline int32_t progid_for_pid(ts_priv_t *priv, int pid, int32_t req)		//finds the first program listing a pid
{
	int i, j;
	pmt_t *pmt;


	if(priv->pmt == NULL)
		return -1;


	for(i=0; i < priv->pmt_cnt; i++)
	{
		pmt = &(priv->pmt[i]);

		if(pmt->es == NULL)
			return -1;

		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].pid == pid)
			{
				if((req == 0) || (req == pmt->progid))
					return pmt->progid;
			}
		}

	}
	return -1;
}

static inline int32_t prog_pcr_pid(ts_priv_t *priv, int progid)
{
	int i;

	if(priv->pmt == NULL)
		return -1;
	for(i=0; i < priv->pmt_cnt; i++)
	{
		if(priv->pmt[i].progid == progid)
			return priv->pmt[i].PCR_PID;
	}
	return -1;
}


static inline int pid_match_lang(ts_priv_t *priv, uint16_t pid, char *lang)
{
	uint16_t i, j;
	pmt_t *pmt;

	if(priv->pmt == NULL)
		return -1;

	for(i=0; i < priv->pmt_cnt; i++)
	{
		pmt = &(priv->pmt[i]);

		if(pmt->es == NULL)
			return -1;

		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].pid != pid)
				continue;

			mp_msg(MSGT_DEMUXER, MSGL_V, "CMP LANG %s AND %s, pids: %d %d\n",pmt->es[j].lang, lang, pmt->es[j].pid, pid);
			if(strncmp(pmt->es[j].lang, lang, 3) == 0)
			{
				return 1;
			}
		}

	}

	return -1;
}

//stripped down version of a52_syncinfo() from liba52
//copyright belongs to Michel Lespinasse <walken@zoy.org> and Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
int mp_a52_framesize(uint8_t * buf, int *srate)
{
	int rate[] = {	32,  40,  48,  56,  64,  80,  96, 112,
			128, 160, 192, 224, 256, 320, 384, 448,
			512, 576, 640
	};
	uint8_t halfrate[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3};
	int frmsizecod, bitrate, half;

	if((buf[0] != 0x0b) || (buf[1] != 0x77))	/* syncword */
		return 0;

	if(buf[5] >= 0x60)		/* bsid >= 12 */
		return 0;

	half = halfrate[buf[5] >> 3];

	frmsizecod = buf[4] & 63;
	if(frmsizecod >= 38)
		return 0;

	bitrate = rate[frmsizecod >> 1];

	switch(buf[4] & 0xc0)
	{
		case 0:	/* 48 KHz */
			*srate = 48000 >> half;
			return 4 * bitrate;
		case 0x40:	/* 44.1 KHz */
			*srate = 44100 >> half;
			return 2 * (320 * bitrate / 147 + (frmsizecod & 1));
		case 0x80: /* 32 KHz */
			*srate = 32000 >> half;
			return 6 * bitrate;
	}

	return 0;
}

//second stage: returns the count of A52 syncwords found
static int a52_check(char *buf, int len)
{
	int cnt, frame_length, ok, srate;

	cnt = ok = 0;
	if(len < 8)
		return 0;

	while(cnt < len - 7)
	{
		if(buf[cnt] == 0x0B && buf[cnt+1] == 0x77)
		{
			frame_length = mp_a52_framesize(&buf[cnt], &srate);
			if(frame_length>=7 && frame_length<=3840)
			{
				cnt += frame_length;
				ok++;
			}
			else
			    cnt++;
		}
		else
			cnt++;
	}

	mp_msg(MSGT_DEMUXER, MSGL_V, "A52_CHECK(%d input bytes), found %d frame syncwords of %d bytes length\n", len, ok, frame_length);
	return ok;
}


static off_t ts_detect_streams(demuxer_t *demuxer, tsdemux_init_t *param)
{
	int video_found = 0, audio_found = 0, sub_found = 0, i, num_packets = 0, req_apid, req_vpid, req_spid;
	int is_audio, is_video, is_sub, has_tables;
	int32_t p, chosen_pid = 0;
	off_t pos=0, ret = 0, init_pos, end_pos;
	ES_stream_t es;
	unsigned char tmp[TS_FEC_PACKET_SIZE];
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;
	struct {
		char *buf;
		int pos;
	} pes_priv1[8192], *pptr;
	char *tmpbuf;
#ifdef FIX_NOSOUND_SELECT_OTHER_VIDEO
	int nosound = 0;
#endif // end of FIX_NOSOUND_SELECT_OTHER_VIDEO
#ifdef SUPPORT_SS_FINE_TUNE
	double last_check_pts = 0.0;
#ifdef TS_STREAM_CHECK_RANGE
	extern unsigned int ts_check_cache;
	char quick_ts_check = 0;
#endif /* end of TS_STREAM_CHECK_RANGE */ // add by carlos, 2011-03-15
	first_video_pid = 0;
	first_audio_pid = 0;
	per_second_pos = 0;
	first_video_pts = 0.0;
#endif /* end of SUPPORT_SS_FINE_TUNE */
	priv->last_pid = 8192;		//invalid pid

#ifdef DVB_SUBTITLES /* Fuchun support ts subtitle 2010-10-05 */
#else // else of DVB_SUBTITLES
	param->spid = DO_NOT_PLAY;	//Barry 2010-08-09 
#endif // end of DVB_SUBTITLES	
	req_apid = param->apid;
	req_vpid = param->vpid;
	req_spid = param->spid;

#ifdef FIX_NOSOUND_SELECT_OTHER_VIDEO
	if(req_apid == DO_NOT_PLAY)
	{
		req_apid = DO_NOT_CARE;
		nosound = 1;
	}
#endif // end of FIX_NOSOUND_SELECT_OTHER_VIDEO

	has_tables = 0;
	memset(pes_priv1, 0, sizeof(pes_priv1));
	init_pos = stream_tell(demuxer->stream);
	mp_msg(MSGT_DEMUXER, MSGL_V, "PROBING UP TO %"PRIu64", PROG: %d\n", (uint64_t) param->probe, param->prog);

#ifdef TS_STREAM_CHECK_RANGE
	printf("#### Carlos in [%s][%d] ts_check_cache is [%x] ####\n", __func__, __LINE__, ts_check_cache);
	if (ts_check_cache != 0xFFFFFFFF && (ts_check_cache > 0x10000) && (ts_check_cache < 0x800000))
	{
		quick_ts_check = 1;
		end_pos = init_pos + ts_check_cache;
	}
	else
#endif /* end of TS_STREAM_CHECK_RANGE */
		end_pos = init_pos + (param->probe ? param->probe : TS_MAX_PROBE_SIZE);
//#define MAX_END_POS_RANGE	0x40000	
	//end_pos = init_pos + 1000000; //==> ok
	//end_pos = init_pos + 500000; // ==> ok
	//end_pos = init_pos + 0x100000	; // 1MB , ok 
	//end_pos = init_pos + 0x80000	; // 512KB, ok
//	end_pos = init_pos + MAX_END_POS_RANGE ; // 256, ok
//	end_pos = init_pos + 0x20000	; // 128, failed
	printf("@@@@ In [%s][%d] init_pos is [%lld] end_pos [%lld]@@@@\n", __func__, __LINE__, init_pos, end_pos);
	while(1)
	{
		pos = stream_tell(demuxer->stream);
		if(pos > end_pos || demuxer->stream->eof)
		{
			debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d] pos[%lld] end_pos[%lld] demuxer->stream->eof[%d]\n", __func__, __LINE__, param->apid, param->apid, num_packets, pos, end_pos, demuxer->stream->eof);
			break;
		}

		if(ts_parse(demuxer, &es, tmp, 1))
		{
			//Non PES-aligned A52 audio may escape detection if PMT is not present;
			//in this case we try to find at least 3 A52 syncwords
			if((es.type == PES_PRIVATE1) && (! audio_found) && req_apid > DO_NOT_PLAY)
			{
				pptr = &pes_priv1[es.pid];
				if(pptr->pos < 64*1024)
				{
					tmpbuf = realloc(pptr->buf, pptr->pos + es.size);
					if(tmpbuf != NULL)
					{
						pptr->buf = tmpbuf;
						memcpy(&(pptr->buf[ pptr->pos ]), es.start, es.size);
						pptr->pos += es.size;
						if(a52_check(pptr->buf, pptr->pos) > 2)
						{
							param->atype = AUDIO_A52;
							param->apid = es.pid;
							es.type = AUDIO_A52;
						}
					}
				}
			}

			is_audio = IS_AUDIO(es.type) || ((es.type==SL_PES_STREAM) && IS_AUDIO(es.subtype));
			is_video = IS_VIDEO(es.type) || ((es.type==SL_PES_STREAM) && IS_VIDEO(es.subtype));
			is_sub   = IS_SUB(es.type);
 
#ifdef TS_STREAM_CHECK_RANGE
			if (!quick_ts_check)
			{
#endif /* end of TS_STREAM_CHECK_RANGE */			
#ifdef SUPPORT_SS_FINE_TUNE
				if(is_video && (first_video_pts == 0.0))
				{
					if (!first_video_pid)
						first_video_pid = es.pid;
					if(first_video_pid == es.pid)
					{
						first_video_pts = es.pts;
						seek_printf("@@@ In [%s][%d] pts is [%f] pid [%d]@@@\n", __func__, __LINE__, es.pts, es.pid);
					}
				}
				else if(is_audio && (first_audio_pts == 0.0))
				{
					if (!first_audio_pid)
						first_audio_pid = es.pid;
					if(first_audio_pid == es.pid)
					{
						first_audio_pts = es.pts;
						seek_printf("@@@ In [%s][%d] pts is [%f] pid [%d]@@@\n", __func__, __LINE__, es.pts, es.pid);
					}
				}
				else
				{
					/* Nothing */
				}
#endif /* end of SUPPORT_SS_FINE_TUNE */
#ifdef TS_STREAM_CHECK_RANGE
			}
#endif /* end of TS_STREAM_CHECK_RANGE */

			if((! is_audio) && (! is_video) && (! is_sub))
				continue;
			if(is_audio && req_apid == DO_NOT_PLAY)
				continue;

			if(is_video)
			{
    				chosen_pid = (req_vpid == es.pid);
				if((! chosen_pid) && (req_vpid > 0))
					continue;
			}
			else if(is_audio)
			{
				if(req_apid > 0)
				{
					chosen_pid = (req_apid == es.pid);
					if(! chosen_pid)
						continue;
				}
				else if(param->alang[0] > 0 && es.lang[0] > 0)
				{
					if(pid_match_lang(priv, es.pid, param->alang) == -1)
						continue;

					chosen_pid = 1;
					param->apid = req_apid = es.pid;
				}
			}
			else if(is_sub)
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_SUBTITLE_ID=%d\n", es.pid);
				if (es.lang[0] > 0)
					mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_SID_%d_LANG=%s\n", es.pid, es.lang);
				chosen_pid = (req_spid == es.pid);
				if((! chosen_pid) && (req_spid > 0))
					continue;
			}

			if(req_apid < 0 && (param->alang[0] == 0) && req_vpid < 0 && req_spid < 0)
				chosen_pid = 1;

			if((ret == 0) && chosen_pid)
			{
				ret = stream_tell(demuxer->stream);
			}

			p = progid_for_pid(priv, es.pid, param->prog);
			if(p != -1)
			{
				has_tables++;
				if(!param->prog && chosen_pid)
					param->prog = p;
			}

			if((param->prog > 0) && (param->prog != p))
			{
				if(audio_found)
				{
					/* carlos 20100810, some ts file have many program that they only have audio stream, so we need play other program */
					//if(is_video && (req_vpid == es.pid))
					if(is_video)
					{
						/* user setup the video pid */
						if (req_vpid > 0 && (req_vpid == es.pid))
						{
							if(priv->pmt_cnt)
							{
								p = progid_for_pid(priv, es.pid, param->prog);
								if (p != -1 )
								{
									/* We need make sure the vpid and vpid are the same program */
									param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
									param->vpid = es.pid;
									video_found = 1;
									debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->apid, num_packets);
									break;
								}
								param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
								param->vpid = es.pid;
								video_found = 1;
							}
							/* Sometimes, the file isn't include pmt table , 20100809, fixed file name : attenuated.ts*/
							else
							{
								param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
								param->vpid = es.pid;
								video_found = 1;
								debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->vpid, num_packets);
								break;
							}
						}
						/* Sometime, the program don't have video program, we save someone video pid, and search some program later */
						else if (req_vpid == DO_NOT_CARE)
						{
							param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
							param->vpid = es.pid;
							video_found = 1;
						}
						else
						{
							/* req_vpid == -2 , It's no-video*/
						}
					}
				}

				if(video_found)
				{
					if(is_audio && (req_apid == es.pid))
					{
						if(priv->pmt_cnt)
						{
							p = progid_for_pid(priv, es.pid, param->prog);
							if (p != -1 )
							{
								/* We need make sure the apid and vpid are the same program */
								param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
								param->apid = es.pid;
								audio_found = 1;
								debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->vpid, num_packets);
								break;
							}
						}
						/* Sometimes, the file isn't include pmt table , 20100809, fixed file name : attenuated.ts*/
						else
						{
							param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
							param->apid = es.pid;
							audio_found = 1;
							debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->vpid, num_packets);
							break;
						}
					}
				}


				continue;
			}


			mp_msg(MSGT_DEMUXER, MSGL_DBG2, "TYPE: %x, PID: %d, PROG FOUND: %d\n", es.type, es.pid, param->prog);

			if(is_video)
			{
				if((req_vpid == DO_NOT_CARE) || (req_vpid == es.pid))
				{
					param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
					param->vpid = es.pid;
					video_found = 1;
					debug2_printf("found video in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->vpid, num_packets);
				}
			}


			if(((req_vpid == DO_NOT_PLAY) || (num_packets >= NUM_CONSECUTIVE_AUDIO_PACKETS * 4)) && audio_found && !param->probe)
			//if(((num_packets >= NUM_CONSECUTIVE_AUDIO_PACKETS)) && audio_found && !param->probe)
			{
				//novideo or we have at least 348 audio packets (64 KB) without video (TS with audio only)
				param->vtype = 0;
				debug2_printf("break in [%s][%d] apid[%d] vpid[%d] num_packets[%d]\n", __func__, __LINE__, param->apid, param->vpid, num_packets);
				break;
			}

			if(is_sub)
			{
				if((req_spid == DO_NOT_CARE) || (req_spid == es.pid))
				{
					p = progid_for_pid(priv, es.pid, param->prog);
					if (p != -1 )
					{
						/* We need make sure the spid and vpid are the same program */
						param->stype = es.type;
						param->spid = es.pid;
						sub_found = 1;
					}
				}
			}

			if(is_audio)
			{
				if((req_apid == DO_NOT_CARE) || (req_apid == es.pid))
				{
					if(priv->pmt_cnt)
					{
						p = progid_for_pid(priv, es.pid, param->prog);
						if (p != -1 )
						{
							/* We need make sure the apid and vpid are the same program */
							param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
							param->apid = es.pid;
							audio_found = 1;
						}
						/* Carlos add for PMT does not include audio pid : MPEG1_1280x720.ts, 2010-10-26  */
						else
						{
							param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
							param->apid = es.pid;
						}
					}
					/* Sometimes, the file isn't include pmt table , 20100809, fixed file name : attenuated.ts*/
					else
					{
						param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
						param->apid = es.pid;
						audio_found = 1;
					}
				}
			}

			if(audio_found && (param->apid == es.pid) && (! video_found))
				num_packets++;

			//if((has_tables==0) && (video_found && audio_found) && (pos >= 1000000))
			if((has_tables==0) && (video_found && audio_found) && (pos >= TS_MAX_PROBE_SIZE)) // using the TS_MAX_PROBE_SIZE to search pmt table
			{
				debug2_printf("break in [%s][%d] has_tables == 0, video_found && audio_found num_packets[%d]\n", __func__, __LINE__, num_packets);
				break;
			}
		}
	}
#ifdef TS_STREAM_CHECK_RANGE
	if (!quick_ts_check)
#endif /* end of TS_STREAM_CHECK_RANGE */			
#ifdef SUPPORT_SS_FINE_TUNE
		if (first_video_pid)
		{
#ifdef CONFIG_BLURAY	
			if (bluray_device)
			{
				if(stream_control(demuxer->stream, STREAM_CTRL_GET_TIME_LENGTH, &last_check_pts) == STREAM_UNSUPPORTED)
				{
					seek_printf("$$$$ This BD ISO do not support STREAM_CTRL_GET_TIME_LENGTH$$$$\n");
				}
				else
				{
					per_second_pos = (off_t)((double)(demuxer->stream->end_pos) / last_check_pts);
					seek_printf("$$$$ Total time is [%f] end_pos[%llx] per_second_pos is [%lld]$$$$\n", last_check_pts, demuxer->stream->end_pos, per_second_pos);
					/* 
					 * Carlos add 2011-07-20, fixed the below file, initial time is wrong .
					 * "¿¿¿¿¿¿¿¿¿¿-¿¿¿¿/Broken Blade 3 - Mark of the Assassins Dagger.iso" 
					 * "¿¿¿¿¿¿¿¿¿¿-¿¿¿¿/Broken Blade 4 The Land of Disaster.iso"
					 */
					correct_bd_iso_timebar(demuxer);
				}
			}
			else
#endif /* end of CONFIG_BLURAY */		
			{
				off_t new_offset = 0;
				off_t check_ret = 0;
				char do_find = KEEP_RUNNING;
				int ret_len = 0;
				int32_t prog_id = 0;
				int check_size = 0;

				if(demuxer->stream->type == STREAMTYPE_STREAM)
					check_size = CHECK_LAST_OFFSET_NETWORK;
				else
					check_size = CHECK_LAST_OFFSET;

				if(demuxer->stream->end_pos >= check_size)
					new_offset = demuxer->stream->end_pos - check_size;
				else
					new_offset = demuxer->stream->end_pos / 2 ;

				seek_printf("### In [%s][%d] we demuxer->stream->end_pos [%lld] change offset to [%lld] ###\n", __func__, __LINE__, demuxer->stream->end_pos, new_offset);
				check_ret = stream_seek(demuxer->stream, new_offset);

				seek_printf("$$$$$$$$$$$$$$$$$$$$$$In [%s][%d] pid is [%d] demuxer->stream->bd_ofs[%lld]$$$$$$$$$$$$$$$$$\n", __func__, __LINE__, first_video_pid, demuxer->stream->bd_ofs);
				if (check_ret == (off_t) -1)
					mplayer_debug("$$$$ In [%s][%d] lseek failed errno[%d] [%s] new_offset is [%lld]$$$$\n", __func__, __LINE__, errno, strerror(errno), new_offset);

				seek_printf("$$$$ In [%s][%d] lseek success new_offset is [%lld]$$$$\n", __func__, __LINE__, check_ret);

				prog_id = progid_for_pid(priv, first_audio_pid, 0);
				do
				{
					ret_len = ts_parse(demuxer, &es, tmp, 1);
					if (es.pts != 0.0)
					{
						if (es.pid == first_video_pid) 
						{
							last_check_pts = es.pts;
							do_find = CHECK_PTS_BY_VIDEO;
						}
						else if (es.pts != 0.0)
						{
							seek_printf("======= [%s][%d] es.pid [%d] es.pts[%f] check_es.pid [%d] first_audio_pid[%d] first_audio_pid[%d]=======\n", __func__, __LINE__, es.pid, es.pts, progid_for_pid(priv, es.pid, 0), progid_for_pid(priv, first_audio_pid, 0), first_audio_pid);
							if (es.pid == first_audio_pid)
							{
								last_check_pts = es.pts;
								do_find = CHECK_PTS_BY_AUDIO;
							}
							else if (IS_AUDIO(es.type) && (prog_id == progid_for_pid(priv, es.pid, 0)))
							{
								last_check_pts = es.pts;
								do_find = CHECK_PTS_BY_AUDIO;
							}
							else
							{
								/* Nothing */
							}
						}
						else
						{
							/* Nothing */
						}
					}
#if 0
					//if (demuxer->stream->eof || ret_len <= 0 || (last_check_pts != 0.0))
					if (demuxer->stream->eof || ret_len < 0 ) /* Some file as m2ts also turn on transport scrambling control bit */
					{
						seek_printf("#### We leave [%s][%d], eof[%d], ret_len[%d] last_check_pts[%f] ####\n", __func__, __LINE__, demuxer->stream->eof, ret_len, last_check_pts);
						do_find = 0;
						if(demuxer->stream->eof)
							demuxer->stream->eof = 0;
					}
#endif
				}
				//while(do_find == KEEP_RUNNING);
				while(demuxer->stream->eof == 0);
				if (demuxer->stream->eof) 
				{
					printf("#### We leave [%s][%d], eof[%d], ret_len[%d] last_check_pts[%f] ####\n", __func__, __LINE__, demuxer->stream->eof, ret_len, last_check_pts);
					demuxer->stream->eof = 0;
				}

				if (do_find == CHECK_PTS_BY_VIDEO)
				{
					if (last_check_pts != 0.0 && first_video_pts != 0.0)
					{
						if(last_check_pts < first_video_pts)
						{
							mplayer_debug("#### last_check_pts is [%f] first_video_pts is [%f], PTS_MAX_NUMBER[%f] new last_check_pts is [%f] ###\n", last_check_pts, first_video_pts, PTS_MAX_NUMBER, last_check_pts+PTS_MAX_NUMBER);
							last_check_pts += PTS_MAX_NUMBER;
						}
						if (demuxer->stream->end_pos > (1<<20))
							//per_second_pos = (off_t)((double)(demuxer->stream->end_pos - CHECK_LAST_OFFSET) / (last_check_pts - first_video_pts + (double)2) ) ;
							per_second_pos = (off_t)((double)(demuxer->stream->end_pos - CHECK_LAST_OFFSET) / (last_check_pts - first_video_pts + (double)((CHECK_LAST_OFFSET >> 20) * 2)) ) ;
						else
							per_second_pos = (off_t)((double)(demuxer->stream->end_pos /2) / (last_check_pts - first_video_pts)) ;
						seek_printf("$$$$ last_check_pts is [%f] total time is [%f] per_second_pos is [%lld]$$$$\n", last_check_pts, last_check_pts - first_video_pts, per_second_pos);
						mplayer_debug("$$ The start pid [%d] pts is [%f] end pts is [%f] total duration is [%f] $$\n", first_video_pid, first_video_pts, last_check_pts, last_check_pts - first_video_pts);
                                          ts_file_duration = last_check_pts - first_video_pts; //Polun 2012-01-05 fixed mantis 6771 duration issue
					}
				}
				else if (do_find == CHECK_PTS_BY_AUDIO)
				{
					if (last_check_pts != 0.0 && first_audio_pts != 0.0)
					{
						if(last_check_pts < first_audio_pts)
						{
							mplayer_debug("#### last_check_pts is [%f] first_audio_pts is [%f], PTS_MAX_NUMBER[%f] new last_check_pts is [%f] ###\n", last_check_pts, first_audio_pts, PTS_MAX_NUMBER, last_check_pts+PTS_MAX_NUMBER);
							last_check_pts += PTS_MAX_NUMBER;
						}
						if (demuxer->stream->end_pos > CHECK_LAST_OFFSET)
							//per_second_pos = (off_t)((double)(demuxer->stream->end_pos - CHECK_LAST_OFFSET) / (last_check_pts - first_audio_pts + (double)(CHECK_LAST_OFFSET / 2) ) ) ;
							per_second_pos = (off_t)((double)(demuxer->stream->end_pos - CHECK_LAST_OFFSET) / (last_check_pts - first_audio_pts + (double)((CHECK_LAST_OFFSET >> 20) * 3)) ) ;
						else
							per_second_pos = (off_t)((double)(demuxer->stream->end_pos /2) / (last_check_pts - first_audio_pts)) ;
						seek_printf("$$$$ last_check_pts is [%f] total time is [%f] per_second_pos is [%lld]$$$$\n", last_check_pts, last_check_pts - first_audio_pts, per_second_pos);
						mplayer_debug("$$ The start pid [%d] pts is [%f] end pts is [%f] total duration is [%f] $$\n", first_audio_pid, first_audio_pts, last_check_pts, last_check_pts - first_audio_pts);
                                          ts_file_duration = last_check_pts - first_audio_pts; //Polun 2012-01-05 fixed mantis 6771 duration issue
					}
				}
				else
				{
					/* Can't find the total time */
					seek_printf("### Can't find the audio or video pts ###\n");
				}
			}
		}
#endif /* end of SUPPORT_SS_FINE_TUNE */
	{
		int jj = 0;
		int yy = 0;
		if (priv->pmt_cnt) 
		{
			for(jj = 0 ; jj < priv->pmt_cnt; jj++)
			{
				for(yy = 0 ; yy < priv->pmt[jj].es_cnt; yy++)
#ifdef CHECK_EXIST_FLAGS
					mplayer_debug("## program [%d] pid [%d] type [%.8x] lang[%s]exist_flag[%d]##\n", priv->pmt[jj].progid, priv->pmt[jj].es[yy].pid, priv->pmt[jj].es[yy].type, priv->pmt[jj].es[yy].lang, priv->pmt[jj].es[yy].exist_flag);
#else // else of CHECK_EXIST_FLAGS
					mplayer_debug("## program [%d] pid [%d] type [%.8x] lang[%s]##\n", priv->pmt[jj].progid, priv->pmt[jj].es[yy].pid, priv->pmt[jj].es[yy].type, priv->pmt[jj].es[yy].lang);
#endif // end of CHECK_EXIST_FLAGS
				mplayer_debug("\n");
			}
		}
	}
#ifdef CHECK_AUDIO_IS_VALID
#if 1 /* Carlos add 20100729, Video, Audio and Sub-title need in the same program */
	{
		/* using the first meet video program */
		parse_debug_printf("@@@@@@ In original mode [%s][%d], video_found[%d] audio_found [%d]@@@@@\n", __func__, __LINE__, video_found, audio_found);
		if (req_vpid == DO_NOT_CARE)
		{
			parse_debug_printf("#### In [%s][%d] video don't care, req_apid[%d] video_found[%d] req_spid [%d]###\n", __func__, __LINE__, req_apid, video_found, video_found, req_spid);
			if (video_found)
			{
				if (req_apid == DO_NOT_CARE)
				{
					if (req_spid > 0)
					{
						param->apid = req_apid;
						select_stream(priv, param, demuxer, TYPE_SUB);
					}
					else
						/* normal case, UI play mode */
						find_out_stream(priv, param, demuxer);
				}
				else if (req_apid > 0)
				{
					/* audio user defined, need use audio program */
					#if 1	//Barry 2011-06-14
					if (!select_stream(priv, param, demuxer, TYPE_AUDIO))
					{
						find_out_stream(priv, param, demuxer);
					}
					#else
					select_stream(priv, param, demuxer, TYPE_AUDIO);
					#endif
				}
#ifdef DVB_SUBTITLES				
				else if (req_spid > 0)
				{
					param->apid = req_apid;
					/* sub user defined, need use sub program */
					select_stream(priv, param, demuxer, TYPE_SUB);
					
				}
#endif // end of DVB_SUBTITLES				
				else
				{
					/* Video only, nothing */
                                   //Polun 2011-10-14 fixed if file only support hwac3 audio track, continu play no sound. 
                                   if(sky_hwac3) 
                                       find_out_stream(priv, param, demuxer);
				}
			}
		}
		else if (req_vpid > 0)
		{
			parse_debug_printf("#### In [%s][%d] video user defined [%d] req_apid[%d]video_found[%d]###\n", __func__, __LINE__, req_vpid, req_apid, video_found);
			if (video_found)
			{
				/* Video user define*/
				if (req_apid == DO_NOT_CARE)
				{
					/* video user defined, need use video program */
					select_stream(priv, param, demuxer, TYPE_VIDEO);
				}
				else if (req_apid > 0)
				{
					/* video user defined, need use video program, check audio program */
					select_stream(priv, param, demuxer, TYPE_VIDEO);
				}
				else
				{
					/* Video only, nothing*/
				}
			}
			else
			{
				/* Video user define*/
				if (req_apid == DO_NOT_CARE)
				{
					/* video user defined, need use video program */
					select_stream(priv, param, demuxer, TYPE_VIDEO);
				}
				else if (req_apid > 0)
				{
					/* video user defined, need use video program, check audio program */
					select_stream(priv, param, demuxer, TYPE_VIDEO);
				}
				else
				{
					/* Can't find the video */
					param->vtype = 0;
					param->vpid = 0;
				}
			}
			
		}
		else
		{
			/* Audio only , nothing */
			select_stream(priv, param, demuxer, TYPE_AUDIO);
			parse_debug_printf("#### Audio only mode in [%s][%d] video_found[%d] audio_found [%d]###\n", __func__, __LINE__, video_found, audio_found);
		}
		audio_found = param->apid > 0 ? 1 : 0;
		video_found = param->vpid > 0 ? 1 : 0;
#ifdef DVB_SUBTITLES		
		sub_found = param->spid > 0 ? 1 : 0;
		parse_debug_printf("### Carlos in [%s][%d] audio_found[%d]i[%d][%d], video_found[%d][%d] sub_found[%d][%d]@@@\n", __func__, __LINE__, audio_found, param->apid, param->atype, video_found, param->vpid, sub_found, param->spid);
#endif // end of DVB_SUBTITLES		
	}
#endif // end of 1
#endif // end of CHECK_AUDIO_IS_VALID
#ifdef FIX_NOSOUND_SELECT_OTHER_VIDEO
	if(nosound)
	{
		param->atype = 0;
		param->apid = DO_NOT_PLAY;
		//req_apid = DO_NOT_PLAY;
		audio_found = 0;
	}
	debug2_printf("==== video_found [%d] audio_found [%d] param->vpid[%d] apid[%d]=====\n", video_found, audio_found, param->vpid, param->apid);
#endif // end of FIX_NOSOUND_SELECT_OTHER_VIDEO

	for(i=0; i<8192; i++)
	{
		if(pes_priv1[i].buf != NULL)
		{
			free(pes_priv1[i].buf);
			pes_priv1[i].buf = NULL;
			pes_priv1[i].pos = 0;
		}
	}

	if(video_found)
	{
		if(param->vtype == VIDEO_MPEG1)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO MPEG1(pid=%d) ", param->vpid);
		else if(param->vtype == VIDEO_MPEG2)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO MPEG2(pid=%d) ", param->vpid);
		else if(param->vtype == VIDEO_MPEG4)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO MPEG4(pid=%d) ", param->vpid);
		else if(param->vtype == VIDEO_H264)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO H264(pid=%d) ", param->vpid);
		else if(param->vtype == VIDEO_VC1)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO VC1(pid=%d) ", param->vpid);
		else if(param->vtype == VIDEO_AVC)
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "VIDEO AVC(NAL-H264, pid=%d) ", param->vpid);

#ifdef TS_STREAM_CHECK_RANGE
		if (!quick_ts_check)
		{
#endif /* end of TS_STREAM_CHECK_RANGE */
#ifdef SUPPORT_SS_FINE_TUNE
#ifdef CONFIG_BLURAY
			if ((bluray_device == NULL) && per_second_pos && (first_video_pid != param->vpid))
#else /* else of CONFIG_BLURAY */
				if (per_second_pos && (first_video_pid != param->vpid))
#endif /* end of CONFIG_BLURAY */		
				{
					off_t new_offset = 0;
					off_t check_ret = 0;
					char do_find = 1;
					int ret_len = 0;
					check_ret = stream_seek(demuxer->stream, init_pos);
					first_video_pid = param->vpid;
					first_video_pts = 0.0;
					seek_printf("$$$$$$$$$$$$$$$$$$$$$$In [%s][%d] pid is [%d] $$$$$$$$$$$$$$$$$\n", __func__, __LINE__, first_video_pid);
					if (!check_ret)
						mplayer_debug("$$$$ In [%s][%d] seek failed errno[%d] [%s] new_offset is [%lld]$$$$\n", __func__, __LINE__, errno, strerror(errno), init_pos);

					seek_printf("$$$$ In [%s][%d] seek success new_offset is [%lld]$$$$\n", __func__, __LINE__, init_pos);
					while(do_find)
					{
						ret_len = ts_parse(demuxer, &es, tmp, 1);
						if ((es.pid == first_video_pid) && (es.pts != 0.0))
						{
							first_video_pts = es.pts;
							seek_printf("#### We in [%s][%d], ret_len[%d] first_video_pts[%f] es.pts [%f]####\n", __func__, __LINE__, ret_len, first_video_pts, es.pts);
						}

						if (demuxer->stream->eof || ret_len <= 0 || (first_video_pts != 0.0))
						{
							seek_printf("#### We leave [%s][%d], eof[%d], ret_len[%d] first_video_pts[%f] ####\n", __func__, __LINE__, demuxer->stream->eof, ret_len, first_video_pts);
							do_find = 0;
							demuxer->stream->eof = 0;
						}
					}
				}
				else
				{
					mplayer_debug("$$$ In [%s][%d] bluray_device [%s] per_second_pos[%lld] first_video_pid [%d] param->vpid[%d] $$$\n", __func__, __LINE__, bluray_device, per_second_pos, first_video_pid, param->vpid);
				}
#endif /* end of SUPPORT_SS_FINE_TUNE */		
#ifdef TS_STREAM_CHECK_RANGE
		}
#endif /* end of TS_STREAM_CHECK_RANGE */
	}
	else
	{
		param->vtype = UNKNOWN;
		//WE DIDN'T MATCH ANY VIDEO STREAM
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "NO VIDEO! ");
	}

#ifdef TS_STREAM_CHECK_RANGE
	if (!quick_ts_check)
#endif /* end of TS_STREAM_CHECK_RANGE */
#ifdef SUPPORT_SS_FINE_TUNE
		if (param->apid > 0)
			first_audio_pid = param->apid;
#endif /* end of SUPPORT_SS_FINE_TUNE */		
	if(param->atype == AUDIO_MP2)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO MPA(pid=%d)", param->apid);
	else if(param->atype == AUDIO_A52)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO A52(pid=%d)", param->apid);
	else if(param->atype == AUDIO_DTS)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO DTS(pid=%d)", param->apid);
	else if(param->atype == AUDIO_LPCM_BE)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO LPCM(pid=%d)", param->apid);
	else if(param->atype == AUDIO_BPCM)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO BPCM(pid=%d)", param->apid);
	else if(param->atype == AUDIO_AAC)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO AAC(pid=%d)", param->apid);
	else if(param->atype == AUDIO_TRUEHD)
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "AUDIO TRUEHD(pid=%d)", param->apid);
	else
	{
		audio_found = 0;
		param->atype = UNKNOWN;
		//WE DIDN'T MATCH ANY AUDIO STREAM, SO WE FORCE THE DEMUXER TO IGNORE AUDIO
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "NO AUDIO! ");
	}

	if(IS_SUB(param->stype))
#ifdef DVB_SUBTITLES
		mp_msg(MSGT_DEMUXER, MSGL_INFO, " SUB %s(pid=%d) ", (param->stype==SPU_DVD ? "DVD" : param->stype==SPU_DVB ? "DVB" : param->stype==SPU_PGS ? "PGS" : "Teletext"), param->spid);
#else
		mp_msg(MSGT_DEMUXER, MSGL_INFO, " SUB %s(pid=%d) ", (param->stype==SPU_DVD ? "DVD" : param->stype==SPU_DVB ? "DVB" : "Teletext"), param->spid);
#endif
	else
	{
		param->stype = UNKNOWN;
		mp_msg(MSGT_DEMUXER, MSGL_INFO, " NO SUBS (yet)! ");
	}
	if(video_found || audio_found)
	{
		if(!param->prog)
		{
			p = progid_for_pid(priv, video_found ? param->vpid : param->apid, 0);
			if(p != -1)
				param->prog = p;
		}

		if(demuxer->stream->eof && (ret == 0))
			ret = init_pos;
		mp_msg(MSGT_DEMUXER, MSGL_INFO, " PROGRAM N. %d\n", param->prog);
	}
	else
		mp_msg(MSGT_DEMUXER, MSGL_INFO, "\n");


	for(i=0; i<8192; i++)
	{
		if(priv->ts.pids[i] != NULL)
		{
			priv->ts.pids[i]->payload_size = 0;
			priv->ts.pids[i]->pts = priv->ts.pids[i]->last_pts = 0;
			priv->ts.pids[i]->last_cc = -1;
			priv->ts.pids[i]->is_synced = 0;
		}
	}

	return ret;
}

static int parse_avc_sps(uint8_t *buf, int len, int *w, int *h)
{
	int sps, sps_len;
	unsigned char *ptr;
	mp_mpeg_header_t picture;
	if(len < 6)
		return 0;
	sps = buf[5] & 0x1f;
	if(!sps)
		return 0;
	sps_len = (buf[6] << 8) | buf[7];
	if(!sps_len || (sps_len > len - 8))
		return 0;
	ptr = &(buf[8]);
	picture.display_picture_width = picture.display_picture_height = 0;
	h264_parse_sps(&picture, ptr, len - 8);
	if(!picture.display_picture_width || !picture.display_picture_height)
		return 0;
	*w = picture.display_picture_width;
	*h = picture.display_picture_height;
	return 1;
}

static demuxer_t *demux_open_ts(demuxer_t * demuxer)
{
	int i;
	uint8_t packet_size;
	sh_video_t *sh_video;
	sh_audio_t *sh_audio;
#ifdef DVB_SUBTITLES
	sh_sub_t *sh_sub;
#endif //end of DVB_SUBTITLES
	off_t start_pos;
	tsdemux_init_t params;
	ts_priv_t * priv = demuxer->priv;
#ifdef DEBUG_TIMER
	unsigned int check_time = 0;
#endif /* end of DEBUG_TIMER */	
#ifdef DYNAMIC_CALCULATED_PTS
	check_fps_count = 0;
	need_do_dynamic_pts = 0;
#endif /* end of DYNAMIC_CALCULATED_PTS */

	//Fuchun 2010.03.16
	mpeg_fast = 0;
	encrypted_ts_check = 1;	//Barry 2010-10-12

	mp_msg(MSGT_DEMUX, MSGL_V, "DEMUX OPEN, AUDIO_ID: %d, VIDEO_ID: %d, SUBTITLE_ID: %d,\n",
		demuxer->audio->id, demuxer->video->id, demuxer->sub->id);

	demuxer->type= DEMUXER_TYPE_MPEG_TS;


	stream_reset(demuxer->stream);

	packet_size = ts_check_file(demuxer);
	if(!packet_size)
	    return NULL;

	priv = calloc(1, sizeof(ts_priv_t));
	if(priv == NULL)
	{
		mp_msg(MSGT_DEMUX, MSGL_FATAL, "DEMUX_OPEN_TS, couldn't allocate enough memory for ts->priv, exit\n");
		return NULL;
	}

	for(i=0; i < 8192; i++)
	{
	    priv->ts.pids[i] = NULL;
	    priv->ts.streams[i].id = NOT_EXIST;
	}
	priv->pat.progs = NULL;
	priv->pat.progs_cnt = 0;
	priv->pat.section.buffer = NULL;
	priv->pat.section.buffer_len = 0;

	priv->pmt = NULL;
	priv->pmt_cnt = 0;

	priv->keep_broken = ts_keep_broken;
	priv->ts.packet_size = packet_size;


	demuxer->priv = priv;
	if(demuxer->stream->type != STREAMTYPE_FILE)
		demuxer->seekable = 1;
	else
		demuxer->seekable = 1;


	params.atype = params.vtype = params.stype = UNKNOWN;
	params.apid = demuxer->audio->id;
	params.vpid = demuxer->video->id;
	params.spid = demuxer->sub->id;
	params.prog = ts_prog;
	params.probe = ts_probe;

	if(dvdsub_lang != NULL)
	{
		strncpy(params.slang, dvdsub_lang, 3);
		params.slang[3] = 0;
	}
	else
		memset(params.slang, 0, 4);

	if(audio_lang != NULL)
	{
		strncpy(params.alang, audio_lang, 3);
		params.alang[3] = 0;
	}
	else
		memset(params.alang, 0, 4);

#ifdef HW_TS_DEMUX
	extern int upnp;
	extern int ts_easy_audio;
	if (upnp)
	{
		printf("===  UPNP (MPEG-TS): need enable + hwts + quick_bd + ts_easy_audio ===\n");
		hwtsnet = 1;
		//quick_bd = 1;
		//ts_easy_audio = 1;
		if (!hwtsdemux)
			hwtsdemux = 1;
	}
#endif

#ifdef DEBUG_TIMER
	check_time = GetTimer();

	printf("-------In [%s][%d] type is [%x] STREAMTYPE_STREAM [%x] ------\n", __func__, __LINE__, demuxer->stream->type, STREAMTYPE_STREAM);
#endif /* end of DEBUG_TIMER */	
	start_pos = ts_detect_streams(demuxer, &params);

#ifdef DEBUG_TIMER
	check_time = (GetTimer() - check_time) / 1000;
	printf("#### In [%s][%d] This time using [%3d] ms ###\n", __func__, __LINE__, check_time);
#endif /* end of DEBUG_TIMER */	

	demuxer->sub->id = params.spid;
	priv->prog = params.prog;

	if(params.vtype != UNKNOWN)
	{
		ts_add_stream(demuxer, priv->ts.pids[params.vpid]);
		sh_video = priv->ts.streams[params.vpid].sh;
		demuxer->video->id = priv->ts.streams[params.vpid].id;
		sh_video->ds = demuxer->video;
		sh_video->format = params.vtype;
		demuxer->video->sh = sh_video;
	}

	if(params.atype != UNKNOWN)
	{
		ES_stream_t *es = priv->ts.pids[params.apid];

		if(!IS_AUDIO(es->type) && !IS_AUDIO(es->subtype) && IS_AUDIO(params.atype)) es->subtype = params.atype;
		ts_add_stream(demuxer, priv->ts.pids[params.apid]);
		sh_audio = priv->ts.streams[params.apid].sh;
		demuxer->audio->id = priv->ts.streams[params.apid].id;
		sh_audio->ds = demuxer->audio;
		sh_audio->format = params.atype;
		demuxer->audio->sh = sh_audio;
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
//     if (hwtsdemux)
       {
           int32_t get_idx = progid_idx_in_pmt(priv, params.prog);
           int jj = 0;
           if (get_idx != -1)
           {
               for (jj = 0 ; jj < priv->pmt[get_idx].es_cnt; jj++)
               {
                   if (priv->pmt[get_idx].es[jj].pid != params.apid)
                   {
					   //if (hwtsdemux)
					   {
#ifdef CHECK_EXIST_FLAGS
						   if ( (priv->pmt[get_idx].es[jj].exist_flag & THE_PID_IS_EXIST) && IS_AUDIO(priv->pmt[get_idx].es[jj].type) && !(priv->pmt[get_idx].es[jj].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA)))
#else // else of CHECK_EXIST_FLAGS
						   if (IS_AUDIO(priv->pmt[get_idx].es[jj].type))
#endif // end of CHECK_EXIST_FLAGS						   
						   {
							   ts_add_stream(demuxer, priv->ts.pids[priv->pmt[get_idx].es[jj].pid]);
							   switch_debug_printf("### In [%s][%d] add audio pid [%d] ###\n", __func__, __LINE__, priv->pmt[get_idx].es[jj].pid);
						   }
					   }
                       if (IS_SUB(priv->pmt[get_idx].es[jj].type))
                       {
                           ts_add_stream(demuxer, priv->ts.pids[priv->pmt[get_idx].es[jj].pid]);
                           switch_debug_printf("### In [%s][%d] add sub pid [%d] ###\n", __func__, __LINE__, priv->pmt[get_idx].es[jj].pid);
                       }
                   }
               }
           }
       }
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE

	}

#ifdef DVB_SUBTITLES
	if(params.stype != UNKNOWN)
	{
		ES_stream_t *es = priv->ts.pids[params.spid];

		if(!IS_SUB(params.stype)) es->subtype = params.stype;
		ts_add_stream(demuxer, priv->ts.pids[params.spid]);
		sh_sub = priv->ts.streams[params.spid].sh;
		demuxer->sub->id = priv->ts.streams[params.spid].id;
		demuxer->sub->sh = sh_sub;
	}
#endif // end of DVB_SUBTITLES

	mp_msg(MSGT_DEMUXER,MSGL_V, "Opened TS demuxer, audio: %x(pid %d), video: %x(pid %d)...POS=%"PRIu64", PROBE=%"PRIu64"\n", params.atype, demuxer->audio->id, params.vtype, demuxer->video->id, (uint64_t) start_pos, ts_probe);


	start_pos = (start_pos <= priv->ts.packet_size ? 0 : start_pos - priv->ts.packet_size);
	demuxer->movi_start = start_pos;
	demuxer->reference_clock = MP_NOPTS_VALUE;
	stream_reset(demuxer->stream);
	stream_seek(demuxer->stream, start_pos);	//IF IT'S FROM A PIPE IT WILL FAIL, BUT WHO CARES?

	priv->last_pid = 8192;		//invalid pid

	for(i = 0; i < 3; i++)
	{
		priv->fifo[i].pack  = NULL;
		priv->fifo[i].offset = 0;
	}
	priv->fifo[0].ds = demuxer->audio;
	priv->fifo[1].ds = demuxer->video;
	priv->fifo[2].ds = demuxer->sub;

	priv->fifo[0].buffer_size = 1536;
	priv->fifo[1].buffer_size = 32767;
	priv->fifo[2].buffer_size = 32767;

	priv->pat.section.buffer_len = 0;
	for(i = 0; i < priv->pmt_cnt; i++)
		priv->pmt[i].section.buffer_len = 0;

	demuxer->filepos = stream_tell(demuxer->stream);

#ifdef HW_TS_DEMUX
	if (hwtsdemux && ((params.apid > 0) || (params.vpid > 0)))
	{
		status_write_thread = THREAD_STATUS_IDLE;
		/* Carlos add 2010-12-08, fixed the side effect of fixed playback mosaic when using samba server v753 */
#ifdef STREAM_DIO_AUTO
		if (dio == 0)
			dio_enable(demuxer->stream);
#endif // end of STREAM_DIO_AUTO		
		if (dio)
		{
			i = open_ts_device(priv, TS_DEVICE);
			if (i >= 0)
			{
				priv->prog = 0 ; // carlos add 2010-07-20 : we need set the default value to 0, avoid some file can't play when using hwts
				pthread_mutex_init(&ts_seekmtx, NULL);
				pthread_mutex_init(&fillmtx, NULL);
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
				pthread_mutex_init(&ts_switch, NULL);
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
				/* The offset must align to block size */
#ifdef CONFIG_BLURAY
				if((start_pos + demuxer->stream->bd_ofs) & ~(vol_blksz-1))
					mplayer_debug("%s:%d ******* position not aligned %llx *****************\n", __func__, __LINE__, (start_pos + demuxer->stream->bd_ofs));
				lseek(demuxer->stream->fd, (start_pos + demuxer->stream->bd_ofs) & ~(vol_blksz-1), SEEK_SET);
#else // else of CONFIG_BLURAY
				if(start_pos & ~(vol_blksz-1))
					mplayer_debug("%s:%d ******* position not aligned %llx *****************\n", __func__, __LINE__, start_pos);
				lseek(demuxer->stream->fd, start_pos & ~(vol_blksz-1), SEEK_SET);
#endif // end of CONFIG_BLURAY
				demuxer->stream->pos = start_pos & ~(vol_blksz-1);
				debug2_printf("@@@@ Carlos in [%s][%d] dio [%d] start pos is [%d] @@@@\n", __func__, __LINE__, dio, lseek(demuxer->stream->fd, 0, SEEK_CUR));
				/* call write thread */
				if (pthread_create(&skydvb_write_thread, NULL, (void *)skydvb_write_stream, demuxer) == 0)
				{
					/* postpone the time of setup start_write_thread */
					//start_write_thread = HWTS_WRITE_STREAM_START;
				}
				else
				{
					hwtsdemux = 0;
					mp_msg(MSGT_DEMUX, MSGL_ERR, "demux ts: open ts device failed, switch to sw ts\n");
				}
			}
		}
		else
		{
#ifdef SUPPORT_NETWORKING_HWTS
			if (hwtsnet)
			{
				i = open_ts_device(priv, TS_DEVICE);
				if (i >= 0)
				{
					priv->prog = 0 ; // carlos add 2010-07-20 : we need set the default value to 0, avoid some file can't play when using hwts
					pthread_mutex_init(&ts_seekmtx, NULL);
					pthread_mutex_init(&fillmtx, NULL);
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
					pthread_mutex_init(&ts_switch, NULL);
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
					/* The offset must be 512 alignment */
					debug2_printf("@@@@ Carlos in [%s][%d] dio [%d] start pos is [%d] @@@@\n", __func__, __LINE__, dio, stream_tell(demuxer->stream));
					/* call write thread */
					if (pthread_create(&skydvb_write_thread, NULL, (void *)skydvb_write_stream, demuxer) == 0)
					{
						/* postpone the time of setup start_write_thread */
						//start_write_thread = HWTS_WRITE_STREAM_START;
					}
					else
					{
						start_write_thread = HWTS_WRITE_STREAM_SLEEP;
						mplayer_debug("## pthread_create failed , failed reason is [%s] ##\n", strerror(errno));
					}
					mplayer_debug("@@@ Enable Dio failed, using HW TS net demuxer @@@\n");
				}
				else
				{
					hwtsdemux = 0;
					mp_msg(MSGT_DEMUX, MSGL_ERR, "demux ts: enable Dio failed, open ts device failed, switch to sw ts\n");
				}
			}
			else
#endif /* end of SUPPORT_NETWORKING_HWTS */			
			{
				/* Enable dio failed, we switch to sw ts parse */
				hwtsdemux = 0;
				mplayer_debug("@@@ Enable Dio failed, switch to SW TS demuxer @@@\n");
			}
		}
		
	}
#endif // end of  HW_TS_DEMUX

#ifdef HW_TS_DEMUX
	/* We need check HWTS is workable or not, else we need switch to SW, maybe initial failed on skydvb_write_stream */
	if (hwtsdemux)
	{
		i = 0;
		do
		{
			if (status_write_thread != THREAD_STATUS_IDLE)
			{
				if (status_write_thread == THREAD_STATUS_END)
				{
					mp_msg(MSGT_DEMUX, MSGL_ERR, "demux ts: enable Dio failed, open ts device failed, switch to sw ts\n");
					hwtsdemux = 0;
					status_write_thread = THREAD_STATUS_IDLE;
					if	(pthread_join(skydvb_write_thread, 0) != 0)
						mplayer_debug("=== pthread_join failed, reason is [%s] ==\n", strerror(errno));
					pthread_mutex_destroy(&ts_seekmtx);
					pthread_mutex_destroy(&fillmtx);
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
					pthread_mutex_destroy(&ts_switch);
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
					/* Close device */
					for(i = 0; i < 3; i++)
					{
						if(priv->fifo[i].pfd > 0)
							close(priv->fifo[i].pfd);
					}
					/* Move close(dvr_fd) from skydvb_write_stream to here, by carlos 2010-09-10*/
					if (dvr_fd)
					{
						close(dvr_fd);
						dvr_fd = 0;
					}

				}
					break;

			}
			else
			{
				i++;
				if (i > MAX_WAITING_THREAD_READY_COUNT)
				{
					mplayer_debug("@@@ In [%s][%d] over MAX_WAITING_THREAD_READY_COUNT [%d] @@@\n", __func__, __LINE__, MAX_WAITING_THREAD_READY_COUNT);
					break;
				}
				usleep(10000);
			}

		}while(1);

	}
#endif /* end of HW_TS_DEMUX */

	if (priv->pmt_cnt)	//Fuchun 20110712 note bpcm channels in sh_audio_t
	{
		int jj = 0;
		int yy = 0;
		for(jj = 0 ; jj < priv->pmt_cnt; jj++)
		{
			for(yy = 0 ; yy < priv->pmt[jj].es_cnt; yy++)
			{
				if(priv->pmt[jj].es[yy].type == AUDIO_BPCM)
				{
					int ii = 0;
					for(ii = 0; ii < MAX_A_STREAMS; ii++)
					{
						if(demuxer->a_streams[ii] && ((sh_audio_t *)demuxer->a_streams[ii])->aid == priv->pmt[jj].es[yy].pid)
						{
//							printf("=== In[%s][%d]  ii:%d pid:%d type:0x%x channels:%d ===\n", __func__, __LINE__, ii, priv->pmt[jj].es[yy].pid, priv->pmt[jj].es[yy].type, priv->pmt[jj].es[yy].bpcm_channels);
							((sh_audio_t *)demuxer->a_streams[ii])->bpcm_channels = priv->pmt[jj].es[yy].bpcm_channels;
						}
					}
				}
			}
		}
	}


	encrypted_ts_check = 0;	//Barry 2010-10-12
	return demuxer;
}

static void demux_close_ts(demuxer_t * demuxer)
{
	uint16_t i;
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;

	if(priv)
	{
		if(priv->pat.section.buffer)
			free(priv->pat.section.buffer);
		if(priv->pat.progs)
			free(priv->pat.progs);

		if(priv->pmt)
		{
			for(i = 0; i < priv->pmt_cnt; i++)
			{
				if(priv->pmt[i].section.buffer)
					free(priv->pmt[i].section.buffer);
				if(priv->pmt[i].es)
					free(priv->pmt[i].es);
			}
			free(priv->pmt);
		}

		//Barry 2010-08-09
		for(i = 0; i < NB_PID_MAX;i ++)
		{
			if(priv->ts.pids[i] != NULL)
			{
				if (priv->ts.pids[i]->extradata != NULL)
					free(priv->ts.pids[i]->extradata);
				if (priv->ts.pids[i]->section.buffer != NULL)
					free(priv->ts.pids[i]->section.buffer);
				if (priv->ts.pids[i]->start != NULL)
					free(priv->ts.pids[i]->start);
				free(priv->ts.pids[i]);
			}
		}
#ifdef HW_TS_DEMUX
		if(hwtsdemux)
		{
			/* Stop Thread */
			if(start_write_thread & HWTS_WRITE_STREAM_START)
			{
			#if 1 // carlos change exit condition, 2010-06-23
				status_write_thread = THREAD_STATUS_IDLE;
				pwake_up(&fillcond, &fillmtx); /* for quick leave mplayer when received q or enter, add by carlos 2010-11-16 */
			#else // carlos 2010-06-23
				if (status_write_thread != THREAD_STATUS_RUNNING)
				{
					if (pthread_cancel(skydvb_write_thread) != 0)
						mplayer_debug("=== pthread_cancel failed, reason is [%s] ==\n", strerror(errno));
				}
			#endif // carlos 2010-06-23
				if	(pthread_join(skydvb_write_thread, 0) != 0)
					mplayer_debug("=== pthread_join failed, reason is [%s] ==\n", strerror(errno));
			}
			pthread_mutex_destroy(&ts_seekmtx);
			pthread_mutex_destroy(&fillmtx);
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
			pthread_mutex_destroy(&ts_switch);
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE			
			/* Close device */
			for(i = 0; i < 3; i++)
			{
				if(priv->fifo[i].pfd > 0)
					close(priv->fifo[i].pfd);
			}
			/* Move close(dvr_fd) from skydvb_write_stream to here, by carlos 2010-09-10*/
			if (dvr_fd)
			{
				close(dvr_fd);
				dvr_fd = 0;
			}

#ifdef SUPPORT_QT_BD_ISO_ENHANCE
	quick_bd = 0;
#endif
		}
#endif // end of HW_TS_DEMUX
		free(priv);
	}
	demuxer->priv=NULL;
}


#define getbits mp_getbits

static int mp4_parse_sl_packet(pmt_t *pmt, uint8_t *buf, uint16_t packet_len, int pid, ES_stream_t *pes_es)
{
	int i, n, m, mp4_es_id = -1;
	uint64_t v = 0;
	uint32_t pl_size = 0;
	int deg_flag = 0;
	mp4_es_descr_t *es = NULL;
	mp4_sl_config_t *sl = NULL;
	uint8_t au_start = 0, au_end = 0, rap_flag = 0, ocr_flag = 0, padding = 0,  padding_bits = 0, idle = 0;

	pes_es->is_synced = 0;
	mp_msg(MSGT_DEMUXER,MSGL_V, "mp4_parse_sl_packet, pid: %d, pmt: %pm, packet_len: %d\n", pid, pmt, packet_len);
	if(! pmt || !packet_len)
		return 0;

	for(i = 0; i < pmt->es_cnt; i++)
	{
		if(pmt->es[i].pid == pid)
			mp4_es_id = pmt->es[i].mp4_es_id;
	}
	if(mp4_es_id < 0)
		return -1;

	for(i = 0; i < pmt->mp4es_cnt; i++)
	{
		if(pmt->mp4es[i].id == mp4_es_id)
			es = &(pmt->mp4es[i]);
	}
	if(! es)
		return -1;

	pes_es->subtype = es->decoder.object_type;

	sl = &(es->sl);
	if(!sl)
		return -1;

	//now es is the complete es_descriptor of out mp4 ES stream
	mp_msg(MSGT_DEMUXER,MSGL_DBG2, "ID: %d, FLAGS: 0x%x, subtype: %x\n", es->id, sl->flags, pes_es->subtype);

	n = 0;
	if(sl->au_start)
		pes_es->sl.au_start = au_start = getbits(buf, n++, 1);
	else
		pes_es->sl.au_start = (pes_es->sl.last_au_end ? 1 : 0);
	if(sl->au_end)
		pes_es->sl.au_end = au_end = getbits(buf, n++, 1);

	if(!sl->au_start && !sl->au_end)
	{
		pes_es->sl.au_start = pes_es->sl.au_end = au_start = au_end = 1;
	}
	pes_es->sl.last_au_end = pes_es->sl.au_end;


	if(sl->ocr_len > 0)
		ocr_flag = getbits(buf, n++, 1);
	if(sl->idle)
		idle = getbits(buf, n++, 1);
	if(sl->padding)
		padding = getbits(buf, n++, 1);
	if(padding)
	{
		padding_bits = getbits(buf, n, 3);
		n += 3;
	}

	if(idle || (padding && !padding_bits))
	{
		pes_es->payload_size = 0;
		return -1;
	}

	//(! idle && (!padding || padding_bits != 0)) is true
	n += sl->packet_seqnum_len;
	if(sl->degr_len)
		deg_flag = getbits(buf, n++, 1);
	if(deg_flag)
		n += sl->degr_len;

	if(ocr_flag)
	{
		n += sl->ocr_len;
		mp_msg(MSGT_DEMUXER,MSGL_DBG2, "OCR: %d bits\n", sl->ocr_len);
	}

	if(packet_len * 8 <= n)
		return -1;

	mp_msg(MSGT_DEMUXER,MSGL_DBG2, "\nAU_START: %d, AU_END: %d\n", au_start, au_end);
	if(au_start)
	{
		int dts_flag = 0, cts_flag = 0, ib_flag = 0;

		if(sl->random_accesspoint)
			rap_flag = getbits(buf, n++, 1);

		//check commented because it seems it's rarely used, and we need this flag set in case of au_start
		//the decoder will eventually discard the payload if it can't decode it
		//if(rap_flag || sl->random_accesspoint_only)
			pes_es->is_synced = 1;

		n += sl->au_seqnum_len;
		if(packet_len * 8 <= n+8)
			return -1;
		if(sl->use_ts)
		{
			dts_flag = getbits(buf, n++, 1);
			cts_flag = getbits(buf, n++, 1);
		}
		if(sl->instant_bitrate_len)
			ib_flag = getbits(buf, n++, 1);
		if(packet_len * 8 <= n+8)
			return -1;
		if(dts_flag && (sl->ts_len > 0))
		{
			n += sl->ts_len;
			mp_msg(MSGT_DEMUXER,MSGL_DBG2, "DTS: %d bits\n", sl->ts_len);
		}
		if(packet_len * 8 <= n+8)
			return -1;
		if(cts_flag && (sl->ts_len > 0))
		{
			int i = 0, m;

			while(i < sl->ts_len)
			{
				m = FFMIN(8, sl->ts_len - i);
				v |= getbits(buf, n, m);
				if(sl->ts_len - i > 8)
					v <<= 8;
				i += m;
				n += m;
				if(packet_len * 8 <= n+8)
					return -1;
			}

			pes_es->pts = (double) v / (double) sl->ts_resolution;
			mp_msg(MSGT_DEMUXER,MSGL_DBG2, "CTS: %d bits, value: %"PRIu64"/%d = %.3f\n", sl->ts_len, v, sl->ts_resolution, pes_es->pts);
		}


		i = 0;
		pl_size = 0;
		while(i < sl->au_len)
		{
			m = FFMIN(8, sl->au_len - i);
			pl_size |= getbits(buf, n, m);
			if(sl->au_len - i > 8)
				pl_size <<= 8;
			i += m;
			n += m;
			if(packet_len * 8 <= n+8)
				return -1;
		}
		mp_msg(MSGT_DEMUXER,MSGL_DBG2, "AU_LEN: %u (%d bits)\n", pl_size, sl->au_len);
		if(ib_flag)
			n += sl->instant_bitrate_len;
	}

	m = (n+7)/8;
	if(0 < pl_size && pl_size < pes_es->payload_size)
		pes_es->payload_size = pl_size;

	mp_msg(MSGT_DEMUXER,MSGL_V, "mp4_parse_sl_packet, n=%d, m=%d, size from pes hdr: %u, sl hdr size: %u, RAP FLAGS: %d/%d\n",
		n, m, pes_es->payload_size, pl_size, (int) rap_flag, (int) sl->random_accesspoint_only);

	return m;
}

//this function parses the extension fields in the PES header and returns the substream_id, or -1 in case of errors
static int parse_pes_extension_fields(unsigned char *p, int pkt_len)
{
	int skip;
	unsigned char flags;

	if(!(p[7] & 0x1))	//no extension_field
		return -1;
	skip = 9;
	if(p[7] & 0x80)
	{
		skip += 5;
		if(p[7] & 0x40)
			skip += 5;
	}
	if(p[7] & 0x20)	//escr_flag
		skip += 6;
	if(p[7] & 0x10)	//es_rate_flag
		skip += 3;
	if(p[7] & 0x08)//dsm_trick_mode is unsupported, skip
	{
		skip = 0;//don't let's parse the extension fields
	}
	if(p[7] & 0x04)	//additional_copy_info
		skip += 1;
	if(p[7] & 0x02)	//pes_crc_flag
		skip += 2;
	if(skip >= pkt_len)	//too few bytes
		return -1;
	flags = p[skip];
	skip++;
	if(flags & 0x80)	//pes_private_data_flag
		skip += 16;
	if(skip >= pkt_len)
		return -1;
	if(flags & 0x40)	//pack_header_field_flag
	{
		unsigned char l = p[skip];
		skip += l;
	}
	if(flags & 0x20)	//program_packet_sequence_counter
		skip += 2;
	if(flags & 0x10)	//p_std
		skip += 2;
	if(skip >= pkt_len)
		return -1;
	if(flags & 0x01)	//finally the long desired pes_extension2
	{
		unsigned char l = p[skip];	//ext2 flag+len
		skip++;
		if((l == 0x81) && (skip < pkt_len))
		{
			int ssid = p[skip];
			mp_msg(MSGT_IDENTIFY, MSGL_V, "SUBSTREAM_ID=%d (0x%02X)\n", ssid, ssid);
			return ssid;
		}
	}

	return -1;
}

static int pes_parse2(unsigned char *buf, uint16_t packet_len, ES_stream_t *es, int32_t type_from_pmt, pmt_t *pmt, int pid)
{
	unsigned char  *p;
	uint32_t       header_len;
	int64_t        pts;
	uint32_t       stream_id;
	uint32_t       pkt_len, pes_is_aligned;
	int ii = 0;

	//Here we are always at the start of a PES packet
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2(%p, %d): \n", buf, (uint32_t) packet_len);

	if(packet_len == 0 || packet_len > 184)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2, BUFFER LEN IS TOO SMALL OR TOO BIG: %d EXIT\n", packet_len);
		return 0;
	}

	p = buf;
	pkt_len = packet_len;


	mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: HEADER %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);
	if (p[0] || p[1] || (p[2] != 1))
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: error HEADER %02x %02x %02x (should be 0x000001) \n", p[0], p[1], p[2]);
		return 0 ;
	}


	packet_len -= 6;
	if(packet_len==0)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: packet too short: %d, exit\n", packet_len);
		return 0;
	}

	es->payload_size = (p[4] << 8 | p[5]);
	pes_is_aligned = (p[6] & 4);

	stream_id  = p[3];


	if (p[7] & 0x80)
	{ 	/* pts available */
		pts  = (int64_t)(p[9] & 0x0E) << 29 ;
		pts |=  p[10]         << 22 ;
		pts |= (p[11] & 0xFE) << 14 ;
		pts |=  p[12]         <<  7 ;
		pts |= (p[13] & 0xFE) >>  1 ;

		es->pts = pts / 90000.0f;
	}
	else
		es->pts = 0.0f;


	header_len = p[8];


	if (header_len + 9 > pkt_len) //9 are the bytes read up to the header_length field
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "demux_ts: illegal value for PES_header_data_length (0x%02x)\n", header_len);
		return 0;
	}

	if(stream_id==0xfd)
	{
		int ssid = parse_pes_extension_fields(p, pkt_len);
		if((audio_substream_id!=-1) && (ssid != audio_substream_id))
			return 0;

#ifdef DVB_SUBTITLES
		if(ssid == 0x72 && type_from_pmt != AUDIO_DTS && type_from_pmt != SPU_PGS)	//Fuchun 2010.09.28
#else
		if(ssid == 0x72 && type_from_pmt != AUDIO_DTS)
#endif
			/* Our audio decoder do not support AUDIO_TRUEHD, we need set the default value to UNKNOWN, avoid to cause mplayer exit when select thie audio stream, carlos add 2010-10-18 */
			//es->type  = type_from_pmt = AUDIO_TRUEHD;
			//es->type  = type_from_pmt = NOT_SUPPORT;
			es->type  = type_from_pmt = UNKNOWN;
	}

	p += header_len + 9;
	packet_len -= header_len + 3;

	if(es->payload_size)
		es->payload_size -= header_len + 3;

#ifdef DROP_GARBAGE_DATA
	/* Deep Blue-1080P-H264-DTS6ch-TS.ts has 3 bytes garbage, payload_size is correct pes-raw data */
	if (es->payload_size && es->payload_size < packet_len)
		packet_len = es->payload_size;
#endif // end of DROP_GARBAGE_DATA		

	es->is_synced = 1;	//only for SL streams we have to make sure it's really true, see below
	if (stream_id == 0xbd)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG3, "pes_parse2: audio buf = %02X %02X %02X %02X %02X %02X %02X %02X, 80: %d\n",
			p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[0] & 0x80);

		/*
		* we check the descriptor tag first because some stations
		* do not include any of the A52 header info in their audio tracks
		* these "raw" streams may begin with a byte that looks like a stream type.
		*/

#ifdef DVB_SUBTITLES
		if(type_from_pmt == SPU_PGS)	//Fuchun 2010.09.28
		{
			es->start = p;
			es->size  = packet_len;
			es->type  = SPU_PGS;
			es->payload_size -= packet_len;
			return 1;
		}
#endif //end of DVB_SUBTITLES
		if(
			(type_from_pmt != NOT_SUPPORT) && (
			(type_from_pmt == AUDIO_A52) ||		 /* A52 - raw */
			(p[0] == 0x0B && p[1] == 0x77)		/* A52 - syncword */
			)
		)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "A52 RAW OR SYNCWORD\n");
			es->start = p;
			es->size  = packet_len;
			es->type  = AUDIO_A52;
			es->payload_size -= packet_len;
#if 1	//Barry 2010-08-05
			if ( (p[0] == 0x0B && p[1] == 0x77) && (((p[5]>>3) & 0x1F) > 10) && (((p[5]>>3) & 0x1F) <= 16))
			{
				if (pmt)
				{
					for(ii = 0 ; ii < pmt->es_cnt ; ii++)
						if (pmt->es[ii].pid ==pid)
						{
							if (pmt->es[ii].type != NOT_SUPPORT)
							{
								mplayer_debug("demux_ts [%d] : Audio [%d] is E-AC3, Not supported!, type_from_pmt[%x]\n", __LINE__, pid, type_from_pmt);
								pmt->es[ii].type = NOT_SUPPORT;
							}

						}
				}
				es->type =  NOT_SUPPORT;
			}
#ifdef SUPPORT_ERROR_TYPE_CHECK
			else
			{
				
				if ((p[0] == 0x7f) && (p[1] == 0xfe) && (p[2] == 0x80) && (p[3] == 0x01))
				{
					if (pmt)
					{
						for(ii = 0 ; ii < pmt->es_cnt ; ii++)
							if (pmt->es[ii].pid ==pid)
								pmt->es[ii].type = AUDIO_DTS;
					}
					es->type =  AUDIO_DTS;
					mp_msg(MSGT_DEMUX, MSGL_V, ": Audio is AUDIO_DTS, Not AC52! [%s][%d]\n", __func__, __LINE__);
				}
			}
#endif // end of SUPPORT_ERROR_TYPE_CHECK			
#endif

			return 1;
		}
#ifdef MPLAYER_NOT_READY_TYPE_HANDLE
		else if (type_from_pmt == NOT_SUPPORT)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "NOT SUPPORT\n");
			es->is_synced = 0;
			return 0;
		}
#endif // end of MPLAYER_NOT_READY_TYPE_HANDLE
		else if (type_from_pmt == AUDIO_DTS)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "AUDIO_DTS\n");
			es->start   = p;
			es->size    = packet_len;
			es->type    = AUDIO_DTS;
			es->payload_size -= packet_len;

			return 1;
		}
		else if (type_from_pmt == AUDIO_BPCM)
		{
#if 1	//Barry 2011-07-04
			if ((p[2] >> 4) != 0x3 && pmt)	//Barry 2010-10-26 support bpcm 2 channels
			{
				for(ii = 0 ; ii < pmt->es_cnt ; ii++)
				{
					if (pmt->es[ii].pid == pid)
					{
						pmt->es[ii].bpcm_channels = p[2];
				#if 1	//Barry 2011-09-22 fix mantis:5600, 5977 set bpcm, 6 ch, not support
						if (skydroid)
						{
							es->type =  NOT_SUPPORT;
							pmt->es[ii].type = NOT_SUPPORT;
						}
						else
							es->type = AUDIO_BPCM;
				#endif
					}
				}
			}
			else
				es->type = AUDIO_BPCM;
#else
			if ((p[2] >> 4) != 0x3 && pmt)	//Barry 2010-10-26 support bpcm 2 channels
			{
				for(ii = 0 ; ii < pmt->es_cnt ; ii++)
				{
					if (pmt->es[ii].pid ==pid)
					{
						es->type =  NOT_SUPPORT;
						pmt->es[ii].type = NOT_SUPPORT;
						//mplayer_debug("== we set  PCM to not support pid[%d] ii[%d]==\n", pid, ii);
					}
				}
			}
			else
				es->type = AUDIO_BPCM;
#endif

			es->start   = p;
			es->size    = packet_len;
			es->payload_size -= packet_len;
			return 1;
		}
		else if (pes_is_aligned && ((p[0]&0xf0) == 0xa0))
		{
			int pcm_offset;

			for (pcm_offset=0; ++pcm_offset < packet_len-1 ; )
			{
				if (p[pcm_offset] == 0x01 && p[pcm_offset+1] == 0x80)
				{ 	/* START */
					pcm_offset += 2;
					break;
				}
			}

			es->start   = p + pcm_offset;
			es->size    = packet_len - pcm_offset;
			es->type    = AUDIO_LPCM_BE;
			es->payload_size -= packet_len;

			return 1;
		}
		/* SPU SUBS */
		else if(type_from_pmt == SPU_DVB ||
		((p[0] == 0x20) && pes_is_aligned)) // && p[1] == 0x00))
		{
			es->start = p;
			es->size  = packet_len;
			es->type  = SPU_DVB;
			es->payload_size -= packet_len;

			return 1;
		}
		else if (pes_is_aligned && ((p[0] & 0xE0) == 0x20))	//SPU_DVD
		{
			//DVD SUBS
			es->start   = p+1;
			es->size    = packet_len-1;
			es->type    = SPU_DVD;
			es->payload_size -= packet_len;

			return 1;
		}
		else if (pes_is_aligned && (p[0] & 0xF8) == 0x80)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "A52 WITH HEADER\n");
			es->start   = p+4;
			es->size    = packet_len - 4;
			es->type    = AUDIO_A52;
			es->payload_size -= packet_len;

			return 1;
		}
		else
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "PES_PRIVATE1\n");
			es->start   = p;
			es->size    = packet_len;
			es->type    = (type_from_pmt == UNKNOWN ? PES_PRIVATE1 : type_from_pmt);
			es->payload_size -= packet_len;

			return 1;
		}
	}
	else if(((stream_id >= 0xe0) && (stream_id <= 0xef)) || (stream_id == 0xfd && type_from_pmt != UNKNOWN))
	{
		es->start   = p;
		es->size    = packet_len;
		if(type_from_pmt != UNKNOWN)
		    es->type    = type_from_pmt;
		else
		{
			/* Carlos add for PMT does not include audio pid : MPEG1_1280x720.ts, 2010-10-26 */
			if ( ((p[0] | p[1] | p[2])  == 0x0) && (p[3] == 0x01) && (p[4] == 0x09))
			{
				/*
				 * Example : 00 00 00 01 09 30 || 00 00 00 01 09 50  from MPEG1_1280x720.ts
				 */
				es->type    = VIDEO_H264;
			}
			else
			{
				/*
				 * Example : 00 00 01 B3 from attenuated.ts
				 */
				es->type    = VIDEO_MPEG2;
			}
		}
#ifdef DROP_GARBAGE_DATA
		if(es->payload_size && (es->payload_size > packet_len)) /* Sometimes es->payload_size is smaller than packet_len , avoid overflowa, carlos add on 2010-11-19 */
#else // else of DROP_GARBAGE_DATA
		if(es->payload_size)
#endif // end of DROP_GARBAGE_DATA
			es->payload_size -= packet_len;

		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: M2V size %d\n", es->size);
		return 1;
	}
	else if ((stream_id == 0xfa))
	{
		int l;

		es->is_synced = 0;
		if(type_from_pmt != UNKNOWN)	//MP4 A/V or SL
		{
			es->start   = p;
			es->size    = packet_len;
			es->type    = type_from_pmt;

			if(type_from_pmt == SL_PES_STREAM)
			{
				//if(pes_is_aligned)
				//{
					l = mp4_parse_sl_packet(pmt, p, packet_len, pid, es);
					mp_msg(MSGT_DEMUX, MSGL_DBG2, "L=%d, TYPE=%x\n", l, type_from_pmt);
					if(l < 0)
					{
						mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: couldn't parse SL header, passing along full PES payload\n");
						l = 0;
					}
				//}

				es->start   += l;
				es->size    -= l;
			}

			if(es->payload_size)
				es->payload_size -= packet_len;
			return 1;
		}
	}
	else if ((stream_id & 0xe0) == 0xc0)
	{
		es->start   = p;
		es->size    = packet_len;

		if(type_from_pmt != UNKNOWN)
			es->type = type_from_pmt;
		else
			es->type    = AUDIO_MP2;

		es->payload_size -= packet_len;

		return 1;
	}
	else if (type_from_pmt != -1)	//as a last resort here we trust the PMT, if present
	{
		es->start   = p;
		es->size    = packet_len;
		es->type    = type_from_pmt;
		es->payload_size -= packet_len;

		return 1;
	}
	else
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: unknown packet, id: %x\n", stream_id);
	}

	es->is_synced = 0;
	return 0;
}




static int ts_sync(stream_t *stream)
{
	int c=0;
	unsigned int read_count = 0;

	mp_msg(MSGT_DEMUX, MSGL_DBG3, "TS_SYNC \n");

	//while(((c=stream_read_char(stream)) != 0x47) && ! stream->eof);
	/* Carlos reduce check time when we can't find the ts packet start code over MAX_CHECK_COUNT size , 2010-10-06 */
	while(((c=stream_read_char(stream)) != 0x47) && ! stream->eof)
	{
		read_count++;
		if (read_count > MAX_CHECK_COUNT)
		{
			debug_printf("#### exit [%s][%d] read_count is [%d] ####\n", __func__, __LINE__, read_count);
			break;
		}
	}

	if(c == 0x47)
		return c;
	else
		return 0;
}

#ifdef QT_SUPPORT_DVBT
static void ts_dump_streams(ts_priv_t *priv)
{
	int i;

	for(i = 0; i < 3; i++)
	{
		if((priv->fifo[i].pack != NULL) && (priv->fifo[i].offset != 0))
		{
			resize_demux_packet(priv->fifo[i].pack, priv->fifo[i].offset);
			ds_add_packet(priv->fifo[i].ds, priv->fifo[i].pack);
			priv->fifo[i].offset = 0;
			priv->fifo[i].pack = NULL;
		}
	}
}
#endif /* end of QT_SUPPORT_DVBT */


static inline int32_t prog_idx_in_pat(ts_priv_t *priv, uint16_t progid)
{
	int x;

	if(priv->pat.progs == NULL)
			return -1;

	for(x = 0; x < priv->pat.progs_cnt; x++)
	{
		if(priv->pat.progs[x].id == progid)
			return x;
	}

	return -1;
}


static inline int32_t prog_id_in_pat(ts_priv_t *priv, uint16_t pid)
{
	int x;

	if(priv->pat.progs == NULL)
		return -1;

	for(x = 0; x < priv->pat.progs_cnt; x++)
	{
		if(priv->pat.progs[x].pmt_pid == pid)
			return priv->pat.progs[x].id;
	}

	return -1;
}

static int collect_section(ts_section_t *section, int is_start, unsigned char *buff, int size)
{
	uint8_t *ptr;
	uint16_t tlen;
	int skip, tid;

	mp_msg(MSGT_DEMUX, MSGL_V, "COLLECT_SECTION, start: %d, size: %d, collected: %d\n", is_start, size, section->buffer_len);
	if(! is_start && !section->buffer_len)
		return 0;

	if(is_start)
	{
		if(! section->buffer)
		{
			section->buffer = (uint8_t*) malloc(4096+256);
			if(section->buffer == NULL)
				return 0;
		}
		section->buffer_len = 0;
	}

	if(size + section->buffer_len > 4096+256)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "COLLECT_SECTION, excessive len: %d + %d\n", section->buffer_len, size);
		return 0;
	}

	memcpy(&(section->buffer[section->buffer_len]), buff, size);
	section->buffer_len += size;

	if(section->buffer_len < 3)
		return 0;

	skip = section->buffer[0];
	if(skip + 4 > section->buffer_len)
		return 0;

	ptr = &(section->buffer[skip + 1]);
	tid = ptr[0];
	tlen = ((ptr[1] & 0x0f) << 8) | ptr[2];
	mp_msg(MSGT_DEMUX, MSGL_V, "SKIP: %d+1, TID: %d, TLEN: %d, COLLECTED: %d\n", skip, tid, tlen, section->buffer_len);
	if(section->buffer_len < (skip+1+3+tlen))
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "DATA IS NOT ENOUGH, NEXT TIME\n");
		return 0;
	}

	return skip+1;
}

static int parse_pat(ts_priv_t * priv, int is_start, unsigned char *buff, int size)
{
	int skip;
	unsigned char *ptr;
	unsigned char *base;
	int entries, i;
	uint16_t progid;
	struct pat_progs_t *tmp;
	ts_section_t *section;

	section = &(priv->pat.section);
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;

	ptr = &(section->buffer[skip]);
	//PARSING
	priv->pat.table_id = ptr[0];
	if(priv->pat.table_id != 0)
		return 0;
	priv->pat.ssi = (ptr[1] >> 7) & 0x1;
	priv->pat.curr_next = ptr[5] & 0x01;
	priv->pat.ts_id = (ptr[3]  << 8 ) | ptr[4];
	priv->pat.version_number = (ptr[5] >> 1) & 0x1F;
	priv->pat.section_length = ((ptr[1] & 0x03) << 8 ) | ptr[2];
	priv->pat.section_number = ptr[6];
	priv->pat.last_section_number = ptr[7];

	//check_crc32(0xFFFFFFFFL, ptr, priv->pat.buffer_len - 4, &ptr[priv->pat.buffer_len - 4]);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PAT: section_len: %d, section %d/%d\n", priv->pat.section_length, priv->pat.section_number, priv->pat.last_section_number);

	entries = (int) (priv->pat.section_length - 9) / 4;	//entries per section

	for(i=0; i < entries; i++)
	{
		int32_t idx;
		base = &ptr[8 + i*4];
		progid = (base[0] << 8) | base[1];

		if((idx = prog_idx_in_pat(priv, progid)) == -1)
		{
			int sz = sizeof(struct pat_progs_t) * (priv->pat.progs_cnt+1);
			tmp = realloc_struct(priv->pat.progs, priv->pat.progs_cnt+1, sizeof(struct pat_progs_t));
			if(tmp == NULL)
			{
				mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PAT: COULDN'T REALLOC %d bytes, NEXT\n", sz);
				break;
			}
			priv->pat.progs = tmp;
			idx = priv->pat.progs_cnt;
			priv->pat.progs_cnt++;
		}

		priv->pat.progs[idx].id = progid;
		priv->pat.progs[idx].pmt_pid = ((base[2]  & 0x1F) << 8) | base[3];
		mp_msg(MSGT_DEMUX, MSGL_V, "PROG: %d (%d-th of %d), PMT: %d\n", priv->pat.progs[idx].id, i+1, entries, priv->pat.progs[idx].pmt_pid);
		mp_msg(MSGT_IDENTIFY, MSGL_V, "PROGRAM_ID=%d (0x%02X), PMT_PID: %d(0x%02X)\n",
			progid, progid, priv->pat.progs[idx].pmt_pid, priv->pat.progs[idx].pmt_pid);
	}

	return 1;
}


static inline int32_t es_pid_in_pmt(pmt_t * pmt, uint16_t pid)
{
	uint16_t i;

	if(pmt == NULL)
		return -1;

	if(pmt->es == NULL)
		return -1;

	for(i = 0; i < pmt->es_cnt; i++)
	{
		if(pmt->es[i].pid == pid)
			return (int32_t) i;
	}

	return -1;
}


static uint16_t get_mp4_desc_len(uint8_t *buf, int *len)
{
	//uint16_t i = 0, size = 0;
	int i = 0, j, size = 0;

	mp_msg(MSGT_DEMUX, MSGL_DBG2, "PARSE_MP4_DESC_LEN(%d), bytes: ", *len);
	j = FFMIN(*len, 4);
	while(i < j)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, " %x ", buf[i]);
		size |= (buf[i] & 0x7f);
		if(!(buf[i] & 0x80))
			break;
		size <<= 7;
		i++;
	}
	mp_msg(MSGT_DEMUX, MSGL_DBG2, ", SIZE=%d\n", size);

	*len = i+1;
	return size;
}


static uint16_t parse_mp4_slconfig_descriptor(uint8_t *buf, int len, void *elem)
{
	int i = 0;
	mp4_es_descr_t *es;
	mp4_sl_config_t *sl;

	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_SLCONFIG_DESCRIPTOR(%d)\n", len);
	es = (mp4_es_descr_t *) elem;
	if(!es)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}
	sl = &(es->sl);

	sl->ts_len = sl->ocr_len = sl->au_len = sl->instant_bitrate_len = sl->degr_len = sl->au_seqnum_len = sl->packet_seqnum_len = 0;
	sl->ocr = sl->dts = sl->cts = 0;

	if(buf[0] == 0)
	{
		i++;
		sl->flags = buf[i];
		i++;
		sl->ts_resolution = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->ocr_resolution = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->ts_len = buf[i];
		i++;
		sl->ocr_len = buf[i];
		i++;
		sl->au_len = buf[i];
		i++;
		sl->instant_bitrate_len = buf[i];
		i++;
		sl->degr_len = (buf[i] >> 4) & 0x0f;
		sl->au_seqnum_len = ((buf[i] & 0x0f) << 1) | ((buf[i+1] >> 7) & 0x01);
		i++;
		sl->packet_seqnum_len = ((buf[i] >> 2) & 0x1f);
		i++;

	}
	else if(buf[0] == 1)
	{
		sl->flags = 0;
		sl->ts_resolution = 1000;
		sl->ts_len = 32;
		i++;
	}
	else if(buf[0] == 2)
	{
		sl->flags = 4;
		i++;
	}
	else
	{
		sl->flags = 0;
		i++;
	}

	sl->au_start = (sl->flags >> 7) & 0x1;
	sl->au_end = (sl->flags >> 6) & 0x1;
	sl->random_accesspoint = (sl->flags >> 5) & 0x1;
	sl->random_accesspoint_only = (sl->flags >> 4) & 0x1;
	sl->padding = (sl->flags >> 3) & 0x1;
	sl->use_ts = (sl->flags >> 2) & 0x1;
	sl->idle = (sl->flags >> 1) & 0x1;
	sl->duration = sl->flags & 0x1;

	if(sl->duration)
	{
		sl->timescale = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->au_duration = (buf[i] << 8) | buf[i+1];
		i += 2;
		sl->cts_duration = (buf[i] << 8) | buf[i+1];
		i += 2;
	}
	else	//no support for fixed durations atm
		sl->timescale = sl->au_duration = sl->cts_duration = 0;

	mp_msg(MSGT_DEMUX, MSGL_V, "MP4SLCONFIG(len=0x%x), predef: %d, flags: %x, use_ts: %d, tslen: %d, timescale: %d, dts: %"PRIu64", cts: %"PRIu64"\n",
		len, buf[0], sl->flags, sl->use_ts, sl->ts_len, sl->timescale, (uint64_t) sl->dts, (uint64_t) sl->cts);

	return len;
}

static int parse_mp4_descriptors(pmt_t *pmt, uint8_t *buf, int len, void *elem);

static uint16_t parse_mp4_decoder_config_descriptor(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i = 0, j;
	mp4_es_descr_t *es;
	mp4_decoder_config_t *dec;

	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DECODER_CONFIG_DESCRIPTOR(%d)\n", len);
	es = (mp4_es_descr_t *) elem;
	if(!es)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}
	dec = (mp4_decoder_config_t*) &(es->decoder);

	dec->object_type = buf[i];
	dec->stream_type =  (buf[i+1]>>2) & 0x3f;

	if(dec->object_type == 1 && dec->stream_type == 1)
	{
		 dec->object_type = MP4_OD;
		 dec->stream_type = MP4_OD;
	}
	else if(dec->stream_type == 4)
	{
		if(dec->object_type == 0x6a)
			dec->object_type = VIDEO_MPEG1;
		if(dec->object_type >= 0x60 && dec->object_type <= 0x65)
			dec->object_type = VIDEO_MPEG2;
		else if(dec->object_type == 0x20)
			dec->object_type = VIDEO_MPEG4;
		else if(dec->object_type == 0x21)
			dec->object_type = VIDEO_AVC;
		/*else if(dec->object_type == 0x22)
			fprintf(stderr, "TYPE 0x22\n");*/
		else dec->object_type = UNKNOWN;
	}
	else if(dec->stream_type == 5)
	{
		if(dec->object_type == 0x40)
			dec->object_type = AUDIO_AAC;
		else if(dec->object_type == 0x6b)
			dec->object_type = AUDIO_MP2;
		else if(dec->object_type >= 0x66 && dec->object_type <= 0x69)
			dec->object_type = AUDIO_MP2;
		else
			dec->object_type = UNKNOWN;
	}
	else
		dec->object_type = dec->stream_type = UNKNOWN;

	if(dec->object_type != UNKNOWN)
	{
		//update the type of the current stream
		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].mp4_es_id == es->id)
			{
				pmt->es[j].type = SL_PES_STREAM;
			}
		}
	}

	if(len > 13)
		parse_mp4_descriptors(pmt, &buf[13], len-13, dec);

	mp_msg(MSGT_DEMUX, MSGL_V, "MP4DECODER(0x%x), object_type: 0x%x, stream_type: 0x%x\n", len, dec->object_type, dec->stream_type);

	return len;
}

static uint16_t parse_mp4_decoder_specific_descriptor(uint8_t *buf, int len, void *elem)
{
	int i;
	mp4_decoder_config_t *dec;

	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DECODER_SPECIFIC_DESCRIPTOR(%d)\n", len);
	dec = (mp4_decoder_config_t *) elem;
	if(!dec)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}

	mp_msg(MSGT_DEMUX, MSGL_DBG2, "MP4 SPECIFIC INFO BYTES: \n");
	for(i=0; i<len; i++)
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "%02x ", buf[i]);
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "\n");

	if(len > MAX_EXTRADATA_SIZE)
	{
		mp_msg(MSGT_DEMUX, MSGL_ERR, "DEMUX_TS, EXTRADATA SUSPICIOUSLY BIG: %d, REFUSED\r\n", len);
		return len;
	}
	memcpy(dec->buf, buf, len);
	dec->buf_size = len;

	return len;
}

static uint16_t parse_mp4_es_descriptor(pmt_t *pmt, uint8_t *buf, int len)
{
	int i = 0, j = 0, k, found;
	uint8_t flag;
	mp4_es_descr_t es, *target_es = NULL, *tmp;

	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4ES: len=%d\n", len);
	memset(&es, 0, sizeof(mp4_es_descr_t));
	while(i < len)
	{
		es.id = (buf[i] << 8) | buf[i+1];
		mp_msg(MSGT_DEMUX, MSGL_V, "MP4ES_ID: %d\n", es.id);
		i += 2;
		flag = buf[i];
		i++;
		if(flag & 0x80)
			i += 2;
		if(flag & 0x40)
			i += buf[i]+1;
		if(flag & 0x20)		//OCR, maybe we need it
			i += 2;

		j = parse_mp4_descriptors(pmt, &buf[i], len-i, &es);
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4ES, types after parse_mp4_descriptors: 0x%x, 0x%x\n", es.decoder.object_type, es.decoder.stream_type);
		if(es.decoder.object_type != UNKNOWN && es.decoder.stream_type != UNKNOWN)
		{
			found = 0;
			//search this ES_ID if we already have it
			for(k=0; k < pmt->mp4es_cnt; k++)
			{
				if(pmt->mp4es[k].id == es.id)
				{
					target_es = &(pmt->mp4es[k]);
					found = 1;
				}
			}

			if(! found)
			{
				tmp = realloc_struct(pmt->mp4es, pmt->mp4es_cnt+1, sizeof(mp4_es_descr_t));
				if(tmp == NULL)
				{
					fprintf(stderr, "CAN'T REALLOC MP4_ES_DESCR\n");
					continue;
				}
				pmt->mp4es = tmp;
				target_es = &(pmt->mp4es[pmt->mp4es_cnt]);
				pmt->mp4es_cnt++;
			}
			memcpy(target_es, &es, sizeof(mp4_es_descr_t));
			mp_msg(MSGT_DEMUX, MSGL_V, "MP4ES_CNT: %d, ID=%d\n", pmt->mp4es_cnt, target_es->id);
		}

		i += j;
	}

	return len;
}

static void parse_mp4_object_descriptor(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i, j = 0, id;

	i=0;
	id = (buf[0] << 2) | ((buf[1] & 0xc0) >> 6);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_OBJECT_DESCRIPTOR: len=%d, OD_ID=%d\n", len, id);
	if(buf[1] & 0x20)
	{
		i += buf[2] + 1;	//url
		mp_msg(MSGT_DEMUX, MSGL_V, "URL\n");
	}
	else
	{
		i = 2;

		while(i < len)
		{
			j = parse_mp4_descriptors(pmt, &(buf[i]), len-i, elem);
			mp_msg(MSGT_DEMUX, MSGL_V, "OBJD, NOW i = %d, j=%d, LEN=%d\n", i, j, len);
			i += j;
		}
	}
}


static void parse_mp4_iod(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i, j = 0;
	mp4_od_t *iod = &(pmt->iod);

	iod->id = (buf[0] << 2) | ((buf[1] & 0xc0) >> 6);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_IOD: len=%d, IOD_ID=%d\n", len, iod->id);
	i = 2;
	if(buf[1] & 0x20)
	{
		i += buf[2] + 1;	//url
		mp_msg(MSGT_DEMUX, MSGL_V, "URL\n");
	}
	else
	{
		i = 7;
		while(i < len)
		{
			j = parse_mp4_descriptors(pmt, &(buf[i]), len-i, elem);
			mp_msg(MSGT_DEMUX, MSGL_V, "IOD, NOW i = %d, j=%d, LEN=%d\n", i, j, len);
			i += j;
		}
	}
}

static int parse_mp4_descriptors(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int tag, descr_len, i = 0, j = 0;

	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DESCRIPTORS, len=%d\n", len);
	if(! len)
		return len;

	while(i < len)
	{
		tag = buf[i];
		j = len - i -1;
		descr_len = get_mp4_desc_len(&(buf[i+1]), &j);
		mp_msg(MSGT_DEMUX, MSGL_V, "TAG=%d (0x%x), DESCR_len=%d, len=%d, j=%d\n", tag, tag, descr_len, len, j);
		if(descr_len > len - j+1)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "descriptor is too long, exit\n");
			return len;
		}
		i += j+1;

		switch(tag)
		{
			case 0x1:
				parse_mp4_object_descriptor(pmt, &(buf[i]), descr_len, elem);
				break;
			case 0x2:
				parse_mp4_iod(pmt, &(buf[i]), descr_len, elem);
				break;
			case 0x3:
				parse_mp4_es_descriptor(pmt, &(buf[i]), descr_len);
				break;
			case 0x4:
				parse_mp4_decoder_config_descriptor(pmt, &buf[i], descr_len, elem);
				break;
			case 0x05:
				parse_mp4_decoder_specific_descriptor(&buf[i], descr_len, elem);
				break;
			case 0x6:
				parse_mp4_slconfig_descriptor(&buf[i], descr_len, elem);
				break;
			default:
				mp_msg(MSGT_DEMUX, MSGL_V, "Unsupported mp4 descriptor 0x%x\n", tag);
		}
		i += descr_len;
	}

	return len;
}

static ES_stream_t *new_pid(ts_priv_t *priv, int pid)
{
	ES_stream_t *tss;

	tss = malloc(sizeof(ES_stream_t));
	if(! tss)
		return NULL;
	memset(tss, 0, sizeof(ES_stream_t));
	tss->pid = pid;
	tss->last_cc = -1;
	tss->type = UNKNOWN;
	tss->subtype = UNKNOWN;
	tss->is_synced = 0;
	tss->extradata = NULL;
	tss->extradata_alloc = tss->extradata_len = 0;
	priv->ts.pids[pid] = tss;

	return tss;
}


static int parse_program_descriptors(pmt_t *pmt, uint8_t *buf, uint16_t len, uint16_t pid)
{
	uint16_t i = 0, k, olen = len;
#ifdef CHECK_EXIST_FLAGS	
	int32_t pid_idx = 0;
#endif /* end of CHECK_EXIST_FLAGS */	

	while(len > 0)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "PROG DESCR, TAG=%x, LEN=%d(%x)\n", buf[i], buf[i+1], buf[i+1]);
		if(buf[i+1] > len-2)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "ERROR, descriptor len is too long, skipping\n");
			return olen;
		}

		if(buf[i] == 0x1d)
		{
			if(buf[i+3] == 2)	//buggy versions of vlc muxer make this non-standard mess (missing iod_scope)
				k = 3;
			else
				k = 4;		//this is standard compliant
			parse_mp4_descriptors(pmt, &buf[i+k], (int) buf[i+1]-(k-2), NULL);
		}
#ifdef CHECK_EXIST_FLAGS		
		else if (buf[i] == 0x9)
		{
			pid_idx = es_pid_in_pmt(pmt, pid);
			if (pid_idx != -1)
			{
				if (!(pmt->es[pid_idx].exist_flag & THE_PID_IS_CA))
				{
					parse_debug_printf("Program CA Descriptor, pid [%d] CA_system_ID: %02X%02X\n", pid, buf[i+2], buf[i+3]);
					pmt->es[pid_idx].exist_flag |= THE_PID_IS_CA;
				}
			}
		}
#endif /* end of CHECK_EXIST_FLAGS */		
		else
		{
		}


		len -= 2 + buf[i+1];
		i += 2 + buf[i+1];
	}

	return olen;
}

static int parse_descriptors(struct pmt_es_t *es, uint8_t *ptr)
{
	int j, descr_len, len;

	j = 0;
	len = es->descr_length;
	while(len > 2)
	{
		descr_len = ptr[j+1];
		mp_msg(MSGT_DEMUX, MSGL_V, "...descr id: 0x%x, len=%d\n", ptr[j], descr_len);
		if(descr_len > len)
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "INVALID DESCR LEN for tag %02x: %d vs %d max, EXIT LOOP\n", ptr[j], descr_len, len);
			return -1;
		}


		if(ptr[j] == 0x6a || ptr[j] == 0x7a)	//A52 Descriptor
		{
			if(es->type == 0x6)
			{
				es->type = AUDIO_A52;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "DVB A52 Descriptor\n");
			}
		}
		else if(ptr[j] == 0x7b)	//DVB DTS Descriptor
		{
			if(es->type == 0x6)
			{
				es->type = AUDIO_DTS;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "DVB DTS Descriptor\n");
			}
		}
		else if(ptr[j] == 0x56) // Teletext
		{
			if(descr_len >= 5) {
				//if ((ptr+2 >= 'a') && (ptr+2 <= 'z') && (ptr+3 >= 'a') && (ptr+3 <= 'z')) /* fixed warning message */
				if ((ptr[2] >= 'a') && (ptr[2] <= 'z') && (ptr[3] >= 'a') && (ptr[3] <= 'z'))
				{
					memcpy(es->lang, ptr+2, 3);
					es->lang[3] = 0;
				}
			}
			es->type = SPU_TELETEXT;
		}
		else if(ptr[j] == 0x59)	//Subtitling Descriptor
		{
			uint8_t subtype;

			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Subtitling Descriptor\n");
			if(descr_len < 8)
			{
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "Descriptor length too short for DVB Subtitle Descriptor: %d, SKIPPING\n", descr_len);
			}
			else
			{
				memcpy(es->lang, &ptr[j+2], 3);
				es->lang[3] = 0;
				subtype = ptr[j+5];
				if(
						(subtype >= 0x10 && subtype <= 0x13) ||
						(subtype >= 0x20 && subtype <= 0x23)
				  )
				{
					es->type = SPU_DVB;
					//page parameters: compo page 2 bytes, ancillary page 2 bytes
				}
				else
					es->type = UNKNOWN;
			}
		}
		else if(ptr[j] == 0x50)	//Component Descriptor
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Component Descriptor\n");
			memcpy(es->lang, &ptr[j+5], 3);
			es->lang[3] = 0;
		}
		else if(ptr[j] == 0xa)	//Language Descriptor
		{
			memcpy(es->lang, &ptr[j+2], 3);
			es->lang[3] = 0;
			mp_msg(MSGT_DEMUX, MSGL_V, "Language Descriptor: %s\n", es->lang);
		}
		else if(ptr[j] == 0x5)	//Registration Descriptor (looks like e fourCC :) )
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Registration Descriptor\n");
			if(descr_len < 4)
			{
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "Registration Descriptor length too short: %d, SKIPPING\n", descr_len);
			}
			else
			{
				char *d;
				memcpy(es->format_descriptor, &ptr[j+2], 4);
				es->format_descriptor[4] = 0;

				d = &ptr[j+2];
				if(d[0] == 'A' && d[1] == 'C' && d[2] == '-' && d[3] == '3')
				{
					es->type = AUDIO_A52;
				}
				else if(d[0] == 'D' && d[1] == 'T' && d[2] == 'S' && d[3] == '1')
				{
					es->type = AUDIO_DTS;
					dts_packet = 1;
				}
				else if(d[0] == 'D' && d[1] == 'T' && d[2] == 'S' && d[3] == '2')
				{
					es->type = AUDIO_DTS;
					dts_packet = 1;
				}
				else if(d[0] == 'V' && d[1] == 'C' && d[2] == '-' && d[3] == '1')
				{
					es->type = VIDEO_VC1;
				}
				else
					es->type = UNKNOWN;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "FORMAT %s\n", es->format_descriptor);
			}
		}
		else if(ptr[j] == 0x1e)
		{
			es->mp4_es_id = (ptr[j+2] << 8) | ptr[j+3];
			mp_msg(MSGT_DEMUX, MSGL_V, "SL Descriptor: ES_ID: %d(%x), pid: %d\n", es->mp4_es_id, es->mp4_es_id, es->pid);
		}
		/* Add from MingYu parse */
		else if(ptr[j] == 0x2)
		{
			int multiple_frame_rate_flag, frame_rate_code, MPEG_1_only_flag;
			mp_msg(MSGT_DEMUX, MSGL_V, "video stream Descriptor\n");
			multiple_frame_rate_flag = (ptr[j+2] >> 7);
			frame_rate_code = (ptr[j+2] & 0x7f) >> 3;
			MPEG_1_only_flag = (ptr[j+2] & 0x04) >> 2;
			mp_msg(MSGT_DEMUX, MSGL_V, "multiple_frame_rate_flag: %d %f %d\n", multiple_frame_rate_flag,
					(float)frame_rate_code, MPEG_1_only_flag);
			if (multiple_frame_rate_flag == 0) {
				//get_mp2_framerate(frame_rate_code);
			}
			if (MPEG_1_only_flag == 0) {
				//int profile_and_level_indication = ptr[j+3];
				//memcpy(es->format_descriptor, &ptr[j+3], 1);
			}
		}
		else if(ptr[j] == 0x3)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "audio stream Descriptor\n");
		}
		else if(ptr[j] == 0x9)
		{
			//mplayer_debug("CA Descriptor, CA_system_ID: %02X%02X\n", ptr[j+2], ptr[j+3]); 
#ifdef CHECK_EXIST_FLAGS			
			es->exist_flag |= THE_PID_IS_CA;
#endif /* end of CHECK_EXIST_FLAGS */			
		}
		else if(ptr[j] == 0x1b) // 27
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "MPEG-4 video Descriptor\n");
		}
		else if(ptr[j] == 0x1c) // 28
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "MPEG-4 audio Descriptor\n");
		}
		else if(ptr[j] == 0x28) // 40
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "AVC video descriptor\n");
		}
		else if(ptr[j] == 0x81)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "AC-3 audio descriptor\n");
		}
		else
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Unknown descriptor 0x%x, SKIPPING\n", ptr[j]);

		len -= 2 + descr_len;
		j += 2 + descr_len;
	}

	return 1;
}

static int parse_sl_section(pmt_t *pmt, ts_section_t *section, int is_start, unsigned char *buff, int size)
{
	int tid, len, skip;
	uint8_t *ptr;
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;

	ptr = &(section->buffer[skip]);
	tid = ptr[0];
	len = ((ptr[1] & 0x0f) << 8) | ptr[2];
	mp_msg(MSGT_DEMUX, MSGL_V, "TABLEID: %d (av. %d), skip=%d, LEN: %d\n", tid, section->buffer_len, skip, len);
	if(len > 4093 || section->buffer_len < len || tid != 5)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "SECTION TOO LARGE or wrong section type, EXIT\n");
		return 0;
	}

	if(! (ptr[5] & 1))
		return 0;

	//8 is the current position, len - 9 is the amount of data available
	parse_mp4_descriptors(pmt, &ptr[8], len - 9, NULL);

	return 1;
}

//static int parse_pmt(ts_priv_t * priv, uint16_t progid, uint16_t pid, int is_start, unsigned char *buff, int size)
static int parse_pmt(ts_priv_t * priv, uint16_t progid, uint16_t pid, int is_start, unsigned char *buff, int size, int probe)
{
	unsigned char *base, *es_base;
	pmt_t *pmt;
	int32_t idx, es_count, section_bytes;
	uint8_t m=0;
	int skip;
	pmt_t *tmp;
	struct pmt_es_t *tmp_es;
	ts_section_t *section;
	ES_stream_t *tss;
	uint16_t current_propid = 0;

	/* Fixed mplayer don't check program_id problem, carlos add 2011-01-19 */
	if (is_start && (progid != 0) && buff && ((buff[0] + 4) <= size) &&  ( size >= (buff[0] + 4 + (((buff[buff[0]+2] & 0x0f) << 8) | buff[buff[0]+3]))))
	{
		current_propid = (((buff[buff[0]+4] & 0xff) << 8) | buff[buff[0]+5]);
		parse_debug_printf("### In [%s][%d] skip [%d] tid [%d] tlen[%d] current_propid [%d] progid [%d]\n", __func__, __LINE__, buff[0], buff[buff[0]+1], (((buff[buff[0]+2] & 0x0f) << 8) | buff[buff[0]+3]), current_propid, progid);
		if (current_propid != progid && current_propid >0 && current_propid < 8191)
		{
			parse_debug_printf("#### In [%s][%d] current_propid [%d] progid [%d] ###\n", __func__, __LINE__, current_propid, progid);
			progid = current_propid;
		}
	}


	idx = progid_idx_in_pmt(priv, progid);

	if(idx == -1)
	{
		int sz = (priv->pmt_cnt + 1) * sizeof(pmt_t);
		tmp = realloc_struct(priv->pmt, priv->pmt_cnt + 1, sizeof(pmt_t));
		if(tmp == NULL)
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PMT: COULDN'T REALLOC %d bytes, NEXT\n", sz);
			return 0;
		}
		priv->pmt = tmp;
		idx = priv->pmt_cnt;
		memset(&(priv->pmt[idx]), 0, sizeof(pmt_t));
		priv->pmt_cnt++;
		priv->pmt[idx].progid = progid;
	}

	pmt = &(priv->pmt[idx]);

	section = &(pmt->section);
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;

	base = &(section->buffer[skip]);

	mp_msg(MSGT_DEMUX, MSGL_V, "FILL_PMT(prog=%d), PMT_len: %d, IS_START: %d, TS_PID: %d, SIZE=%d, M=%d, ES_CNT=%d, IDX=%d, PMT_PTR=%p\n",
		progid, pmt->section.buffer_len, is_start, pid, size, m, pmt->es_cnt, idx, pmt);

	pmt->table_id = base[0];
	if(pmt->table_id != 2)
		return -1;
	pmt->ssi = base[1] & 0x80;
	pmt->section_length = (((base[1] & 0xf) << 8 ) | base[2]);
	pmt->version_number = (base[5] >> 1) & 0x1f;
	pmt->curr_next = (base[5] & 1);
	pmt->section_number = base[6];
	pmt->last_section_number = base[7];
	pmt->PCR_PID = ((base[8] & 0x1f) << 8 ) | base[9];
	pmt->prog_descr_length = ((base[10] & 0xf) << 8 ) | base[11];
	if(pmt->prog_descr_length > pmt->section_length - 9)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT, INVALID PROG_DESCR LENGTH (%d vs %d)\n", pmt->prog_descr_length, pmt->section_length - 9);
		return -1;
	}

	if(pmt->prog_descr_length)
		parse_program_descriptors(pmt, &base[12], pmt->prog_descr_length, pid);

	es_base = &base[12 + pmt->prog_descr_length];	//the beginning of th ES loop

	section_bytes= pmt->section_length - 13 - pmt->prog_descr_length;
	es_count  = 0;

	while(section_bytes >= 5)
	{
		int es_pid, es_type;

		es_type = es_base[0];
		es_pid = ((es_base[1] & 0x1f) << 8) | es_base[2];

		idx = es_pid_in_pmt(pmt, es_pid);
		if(idx == -1)
		{
			int sz = sizeof(struct pmt_es_t) * (pmt->es_cnt + 1);
			tmp_es = realloc_struct(pmt->es, pmt->es_cnt + 1, sizeof(struct pmt_es_t));
			if(tmp_es == NULL)
			{
				mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PMT, COULDN'T ALLOCATE %d bytes for PMT_ES\n", sz);
				continue;
			}
			pmt->es = tmp_es;
			idx = pmt->es_cnt;
			memset(&(pmt->es[idx]), 0, sizeof(struct pmt_es_t));
			pmt->es_cnt++;
		}
#if 1	//Barry 2010-07-22
		else
		{
			section_bytes -= 5 + pmt->es[idx].descr_length;
			es_base += 5 + pmt->es[idx].descr_length;
			mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT, Skip repetitive INDEX = %d,  PID = %d\n", idx, es_pid);
			//continue; // carlos replace continue to break, don't parse duplicate pmt information
			break;
		}
#endif

		pmt->es[idx].descr_length = ((es_base[3] & 0xf) << 8) | es_base[4];


		if(pmt->es[idx].descr_length > section_bytes - 5)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT, ES_DESCR_LENGTH TOO LARGE %d > %d, EXIT\n",
				pmt->es[idx].descr_length, section_bytes - 5);
			return -1;
		}


		pmt->es[idx].pid = es_pid;
		if(es_type != 0x6)
			pmt->es[idx].type = UNKNOWN;
		else
			pmt->es[idx].type = es_type;

		parse_descriptors(&pmt->es[idx], &es_base[5]);

		switch(es_type)
		{
			case 0:
				pmt->es[idx].type = 0x01;
				break;
			case 1:
				pmt->es[idx].type = VIDEO_MPEG1;
				break;
			case 2:
				pmt->es[idx].type = VIDEO_MPEG2;
				break;
			case 3:
			case 4:
				pmt->es[idx].type = AUDIO_MP2;
				break;
			case 6:
				if(pmt->es[idx].type == 0x6)	//this could have been ovrwritten by parse_descriptors
					pmt->es[idx].type = UNKNOWN;
				break;
			case 0x10:
				pmt->es[idx].type = VIDEO_MPEG4;
				break;
			case 0x0f:
			case 0x11:
				pmt->es[idx].type = AUDIO_AAC;
				break;
			case 0x1b:
				pmt->es[idx].type = VIDEO_H264;
				break;
			case 0x12:
				pmt->es[idx].type = SL_PES_STREAM;
				break;
			case 0x13:
				pmt->es[idx].type = SL_SECTION;
				break;
			case 0x81:
				pmt->es[idx].type = AUDIO_A52;
				break;
			case 0x83:  /* carlos add , 2010-10-16, this is AC3 type, 2010-11-04 marked, disable this feature, TODO: wait audio decoder support it */
				//pmt->es[idx].type = AUDIO_A52;
				if (sky_hwac3)
					pmt->es[idx].type = AUDIO_A52;
				else
				{
					if (!skydroid)
						pmt->es[idx].type = AUDIO_TRUEHD;	// TrueHD
					else
						pmt->es[idx].type = NOT_SUPPORT;	// TrueHD
				}
				mplayer_debug("### Found 0x83, 131 A52 TrueHD###\n");
				break;
			case 0x84: /* Barry add, 2011-07-26, AC-3 Plus*/
				if (sky_hwac3)
					pmt->es[idx].type = AUDIO_A52;
				else
				{
					if (!skydroid)
						pmt->es[idx].type = AUDIO_AC3PLUS;	//AC-3 Plus
					else
						pmt->es[idx].type = NOT_SUPPORT;	//AC-3 Plus
				}
				mplayer_debug("### Found 0x84, AC-3 Plus###\n");
				break;
			case 0x80: // PCM data
				pmt->es[idx].type = AUDIO_BPCM; // We set the default PCM to BPCM 
				//pmt->es[idx].type = NOT_SUPPORT;
				break;
#ifdef MPLAYER_NOT_READY_TYPE_HANDLE
			case 0xA1: /* carlos note, 2010-10-16, this is E-AC-3 type */
				if (sky_hwac3)
					pmt->es[idx].type = AUDIO_A52;
				else
				{
					if (!skydroid)
						pmt->es[idx].type = AUDIO_EAC3;	//E-AC-3
					else
						pmt->es[idx].type = NOT_SUPPORT;	//E-AC-3
				}
				mplayer_debug("### Found 0xA1, E-AC-3 ###\n");
				break;
#endif // end of MPLAYER_NOT_READY_TYPE_HANDLE
			case 0x8A:
			case 0x82:
			case 0x85:
			case 0x86:
				pmt->es[idx].type = AUDIO_DTS;
				dts_packet = 1;
				break;
#ifdef DVB_SUBTITLES
			case 0x90:	//Fuchun 2010.09.28
				pmt->es[idx].type = SPU_PGS;
				break;
#endif
			case 0xEA:
				pmt->es[idx].type = VIDEO_VC1;
				break;
			default:
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "UNKNOWN ES TYPE=0x%x\n", es_type);
				pmt->es[idx].type = UNKNOWN;
		}

		tss = priv->ts.pids[es_pid];			//an ES stream
		if(tss == NULL)
		{
			if (!probe)
			{
				int ii = 0;
				char found_change = 0;
				for(ii = 0 ; ii < pmt->es_cnt; ii++)
				{
					if (pmt->es[ii].type == pmt->es[idx].type)
					{
						printf("$$$$ In [%s][%d] will change type[%x] pid from [%d] to [%d] $$$$\n", __func__, __LINE__, pmt->es[idx].type, pmt->es[ii].pid, pmt->es[idx].pid);

						priv->ts.streams[pmt->es[idx].pid].sh = priv->ts.streams[pmt->es[ii].pid].sh;
						priv->ts.streams[pmt->es[ii].pid].sh = NULL;
						priv->ts.streams[pmt->es[idx].pid].id = priv->ts.streams[pmt->es[ii].pid].id;
						priv->ts.streams[pmt->es[ii].pid].id = NOT_EXIST;

						priv->ts.pids[pmt->es[idx].pid] = priv->ts.pids[pmt->es[ii].pid];
						priv->ts.pids[pmt->es[ii].pid] = NULL;
					
						pmt->es[ii].pid = pmt->es[idx].pid;
						found_change = 1;
						break;
					}
				}
				if (found_change)
				{
					/* For VOD dynmaic change video / audio pid */
					pmt->es_cnt--;
					tmp_es = realloc_struct(pmt->es, pmt->es_cnt + 1, sizeof(struct pmt_es_t));
					pmt->es = tmp_es;
				}
			}
			else
			{
				tss = new_pid(priv, es_pid);
				if(tss)
					tss->type = pmt->es[idx].type;
			}
		}

		section_bytes -= 5 + pmt->es[idx].descr_length;
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT(%d INDEX %d), STREAM: %d, FOUND pid=0x%x (%d), type=0x%x, ES_DESCR_LENGTH: %d, bytes left: %d\n",
			progid, idx, es_count, pmt->es[idx].pid, pmt->es[idx].pid, pmt->es[idx].type, pmt->es[idx].descr_length, section_bytes);


		es_base += 5 + pmt->es[idx].descr_length;

		es_count++;
	}

	mp_msg(MSGT_DEMUX, MSGL_V, "----------------------------\n");
	return 1;
}

static pmt_t* pmt_of_pid(ts_priv_t *priv, int pid, mp4_decoder_config_t **mp4_dec)
{
	int32_t i, j, k;

	if(priv->pmt)
	{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			if(priv->pmt[i].es && priv->pmt[i].es_cnt)
			{
				for(j = 0; j < priv->pmt[i].es_cnt; j++)
				{
					if(priv->pmt[i].es[j].pid == pid)
					{
						//search mp4_es_id
						if(priv->pmt[i].es[j].mp4_es_id)
						{
							for(k = 0; k < priv->pmt[i].mp4es_cnt; k++)
							{
								if(priv->pmt[i].mp4es[k].id == priv->pmt[i].es[j].mp4_es_id)
								{
									*mp4_dec = &(priv->pmt[i].mp4es[k].decoder);
									break;
								}
							}
						}

						return &(priv->pmt[i]);
					}
				}
			}
		}
	}

	return NULL;
}


static inline int32_t pid_type_from_pmt(ts_priv_t *priv, int pid)
{
	int32_t pmt_idx, pid_idx, i, j;

	pmt_idx = progid_idx_in_pmt(priv, priv->prog);

	if(pmt_idx != -1)
	{
		pid_idx = es_pid_in_pmt(&(priv->pmt[pmt_idx]), pid);
		if(pid_idx != -1)
			return priv->pmt[pmt_idx].es[pid_idx].type;
	}
	//else
	//{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			pmt_t *pmt = &(priv->pmt[i]);
			for(j = 0; j < pmt->es_cnt; j++)
				if(pmt->es[j].pid == pid)
					return pmt->es[j].type;
		}
	//}

	return UNKNOWN;
}


static inline uint8_t *pid_lang_from_pmt(ts_priv_t *priv, int pid)
{
	int32_t pmt_idx, pid_idx, i, j;
	int32_t prog;

	prog = progid_for_pid(priv, pid, 0);
	//pmt_idx = progid_idx_in_pmt(priv, priv->prog);
	pmt_idx = progid_idx_in_pmt(priv, prog);

	if(pmt_idx != -1)
	{
		pid_idx = es_pid_in_pmt(&(priv->pmt[pmt_idx]), pid);
		if(pid_idx != -1)
			return priv->pmt[pmt_idx].es[pid_idx].lang;
	}
	else
	{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			pmt_t *pmt = &(priv->pmt[i]);
			for(j = 0; j < pmt->es_cnt; j++)
				if(pmt->es[j].pid == pid)
					return pmt->es[j].lang;
		}
	}

	return NULL;
}

static int fill_packet(demuxer_t *demuxer, demux_stream_t *ds, demux_packet_t **dp, int *dp_offset, TS_stream_info *si)
{
	int ret = 0;
#ifdef DVB_SUBTITLES /* Fuchun support ts subtitle 2010-10-05 */
#else // else of DVB_SUBTITLES
	//Barry 2010-08-07
	if (ds == demuxer->sub)
	{
		free_demux_packet(*dp);
		*dp = NULL;
		*dp_offset = 0;
		return ret;
	}
#endif // end of DVB_SUBTITLES

	if((*dp != NULL) && (*dp_offset > 0))
	{
		ret = *dp_offset;
#ifdef BUGGY_FIXED_DTS_PROBLEM
		static int last_audio_len = 0;
		char do_it = 0;
		sh_audio_t *audio_t = (sh_audio_t *)(demuxer->audio->sh);

		if (ds == demuxer->audio && audio_t && audio_t->format == AUDIO_DTS)
		{
			if (Got_Special_DTS_CASE)
			{
				do_it = 1;
			}
			else
			{
				if ((!Check_Audio_PTS_Zero) && last_audio_len == ret)
				{
					Got_Special_DTS_CASE = 1;
					do_it = 1;
					mplayer_debug("#### In [%s][%d]: Special DTS packet, manually split one packet to two packets####\n", __func__, __LINE__);
				}
			}
			last_audio_len = ret ;
		}

		if (do_it)
		{
			demux_packet_t *dup_dp = NULL;
			dup_dp = new_demux_packet(ret / 2);
			memcpy(dup_dp->buffer, (*dp)->buffer + (ret /2) , ret / 2);
			dup_dp->len = ret / 2 ;
#ifdef BD_CLIP_BASE_PTS
			if (bd_now_base_pts > 0)
				adjust_bd_pts(*dp, demuxer);
#endif /* end of BD_CLIP_BASE_PTS */
			dup_dp->pts = (*dp)->pts;
			resize_demux_packet(*dp, ret/2);	//shrinked to the right size
			ds_add_packet(ds, *dp);
			ds_add_packet(ds, dup_dp);
		}
		else
#endif // end of BUGGY_FIXED_DTS_PROBLEM		
		{
#ifdef BD_CLIP_BASE_PTS
			if (bd_now_base_pts > 0)
				adjust_bd_pts(*dp, demuxer);
#endif /* end of BD_CLIP_BASE_PTS */
			resize_demux_packet(*dp, ret);	//shrinked to the right size
			ds_add_packet(ds, *dp);
		}
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "ADDED %d  bytes to %s fifo, PTS=%.3f\n", ret, (ds == demuxer->audio ? "audio" : (ds == demuxer->video ? "video" : "sub")), (*dp)->pts);
		if(si)
		{
			float diff = (*dp)->pts - si->last_pts;
			float dur;

#if 1	//Barry 2010-11-11
			if (ds == demuxer->audio && (((sh_audio_t *)(demuxer->audio->sh))->i_bps) == 0)
			{
				if (demuxer->audio->pack_no > 5 && demuxer->audio->pack_no < 16)
				{
					check_audio_bps_len_cnt += ret;
					check_audio_bps_diff_cnt += diff;
				}
				else if (demuxer->audio->pack_no == 16)
					((sh_audio_t *)(demuxer->audio->sh))->i_bps = (int)(check_audio_bps_len_cnt / check_audio_bps_diff_cnt);
			}
#endif

			if(abs(diff) > 1) //1 second, there's a discontinuity
			{
				si->duration += si->last_pts - si->first_pts;
				si->first_pts = si->last_pts = (*dp)->pts;
			}
			else
			{
				si->last_pts = (*dp)->pts;
			}
			si->size += ret;
			dur = si->duration + (si->last_pts - si->first_pts);

			if(dur > 0 && ds == demuxer->video)
			{
				ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
				if(dur > 1)	//otherwise it may be unreliable
					priv->vbitrate = (uint32_t) ((float) si->size / dur);
			}
		}
	}
	else
	{
		if (*dp && *dp_offset == 0)
		{
			free_demux_packet(*dp);
		}
	}

	*dp = NULL;
	*dp_offset = 0;

	return ret;
}

static int fill_extradata(mp4_decoder_config_t * mp4_dec, ES_stream_t *tss)
{
	uint8_t *tmp;

	mp_msg(MSGT_DEMUX, MSGL_DBG2, "MP4_dec: %p, pid: %d\n", mp4_dec, tss->pid);

	if(mp4_dec->buf_size > tss->extradata_alloc)
	{
		tmp = (uint8_t *) realloc(tss->extradata, mp4_dec->buf_size);
		if(!tmp)
			return 0;
		tss->extradata = tmp;
		tss->extradata_alloc = mp4_dec->buf_size;
	}
	memcpy(tss->extradata, mp4_dec->buf, mp4_dec->buf_size);
	tss->extradata_len = mp4_dec->buf_size;
	mp_msg(MSGT_DEMUX, MSGL_V, "EXTRADATA: %p, alloc=%d, len=%d\n", tss->extradata, tss->extradata_alloc, tss->extradata_len);

	return tss->extradata_len;
}

// 0 = EOF or no stream found
// else = [-] number of bytes written to the packet
static int ts_parse(demuxer_t *demuxer , ES_stream_t *es, unsigned char *packet, int probe)
{
	ES_stream_t *tss;
	uint8_t done = 0;
	int buf_size, is_start, pid, pid1, base;
	int len, cc, cc_ok, afc, retv = 0, is_video, is_audio, is_sub;
	ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
	stream_t *stream = demuxer->stream;
	char *p;
	demux_stream_t *ds = NULL;
	demux_packet_t **dp = NULL;
	int *dp_offset = 0, *buffer_size = 0;
	int32_t progid, pid_type, bad, ts_error;
	int junk = 0, rap_flag = 0;
	pmt_t *pmt;
	mp4_decoder_config_t *mp4_dec;
	TS_stream_info *si;

	while(! done)
	{
		bad = ts_error = 0;
		ds = (demux_stream_t*) NULL;
		dp = (demux_packet_t **) NULL;
		dp_offset = buffer_size = NULL;
		rap_flag = 0;
		mp4_dec = NULL;
		es->is_synced = 0;
		es->lang[0] = 0;
		si = NULL;

		junk = priv->ts.packet_size - TS_PACKET_SIZE;
		buf_size = priv->ts.packet_size - junk;

		if(stream_eof(stream))
		{
			if(! probe)
			{
#ifdef QT_SUPPORT_DVBT
				ts_dump_streams(priv);
#endif /* end of QT_SUPPORT_DVBT */
				demuxer->filepos = stream_tell(demuxer->stream);
			}

			return 0;
		}

		if(! ts_sync(stream))
		{
			mp_msg(MSGT_DEMUX, MSGL_INFO, "TS_PARSE: COULDN'T SYNC\n");
			return 0;
		}

		len = stream_read(stream, &packet[1], 3);
		if (len != 3)
			return 0;
		buf_size -= 4;

#if 1	 // carlos 2010-07-29
		//Barry 2010-07-29
		if ( (packet[1] == 0x47) && (packet[2] != 0x47) && demuxer)
		{
			pid1 = (((packet[2] & 0x1f) << 8) | packet[3]);
			if ( (demuxer->video->id == priv->ts.streams[pid1].id) ||
				(demuxer->audio->id == priv->ts.streams[pid1].id) )
			{
				//mp_msg(MSGT_DEMUX, MSGL_V, "demux_ts: Has 2 bytes sync start code [0x4747]\n");
				packet[1] = packet[2];
				packet[2] = packet[3];
				len = stream_read(stream, &packet[3], 1);
				if (len != 1)
					return 0;
			}
		}
#endif

		if (packet[1] & 0x80) /* change from if((packet[1]  >> 7) & 0x01)	*/ //transport error
			ts_error = 1;

		is_start = packet[1] & 0x40;
		pid = ((packet[1] & 0x1f) << 8) | packet[2];

#if 0	/* 2011-07-04 need marked the below condition for VOD dynamic changed video and audio pid */
		/* Carlos add for speed up SW TS, 2011-01-19 */
		if (!probe)
		{
#if 1
			int parse_next = 1;
			if (demuxer->video && demuxer->video->id == priv->ts.streams[pid].id)
				parse_next = 0;
			else if (demuxer->audio && demuxer->audio->id == priv->ts.streams[pid].id)
			{
				//mplayer_debug("=== In [%s][%d] audio->id is [%d] pid [%d] priv->id[%d]===\n", __func__, __LINE__, demuxer->audio->id, pid, priv->ts.streams[pid].id);
				parse_next = 0;
			}
#ifdef DVB_SUBTITLES
			else if (demuxer->sub->sh)
			{
				sh_sub_t *sh_sub = demuxer->sub->sh;
				if (sh_sub->sid == pid)
					parse_next = 0;
			}
#endif /* end of DVB_SUBTITLES */
			else
				parse_next = 1;
			if (parse_next)
			{
				stream_skip(stream, buf_size-1+junk);
				//printf("@@@@@ In [%s][%d] will go back pid [%d]  progid_for_pid(priv, pid, 0) [%d] priv->prog [%d]@@@@@\n", __func__, __LINE__, pid, progid_for_pid(priv, pid, 0), priv->prog);
				continue;
			}
#else
			/*
			 * Described with parameter
			 * priv->prog == 0 mean this file don't have PAT/PMT, or current pid do not list on PAT/PMT
			 * priv->prog != progid_for_pid(priv, pid, 0) mean current pid is related to playing pid
			 */
			if (priv->prog && priv->prog != progid_for_pid(priv, pid, 0))
			{
				stream_skip(stream, buf_size-1+junk);
				//printf("@@@@@ In [%s][%d] will go back pid [%d]  progid_for_pid(priv, pid, 0) [%d] priv->prog [%d]@@@@@\n", __func__, __LINE__, pid, progid_for_pid(priv, pid, 0), priv->prog);
				continue;
			}
#endif
		}
#endif		
		tss = priv->ts.pids[pid];			//an ES stream
		if(tss == NULL)
		{
			tss = new_pid(priv, pid);
			if(tss == NULL)
				continue;
		}


		cc = (packet[3] & 0xf);
		cc_ok = (tss->last_cc < 0) || ((((tss->last_cc + 1) & 0x0f) == cc));
		tss->last_cc = cc;

		bad = ts_error; // || (! cc_ok);
		if(bad)
		{
			if(priv->keep_broken == 0)
			{
				stream_skip(stream, buf_size-1+junk);
				continue;
			}

			is_start = 0;	//queued to the packet data
		}

		if(is_start)
			tss->is_synced = 1;

		// Carlos move from the front of check ts_error to here, avoid wrong ts data caused mischeck
		//Barry 2010-10-04
		//if ( ((packet[3] >> 7) == 1  && (IS_VIDEO(tss->type)) && encrypted_ts_check )
		if (((packet[3] & 0xc0) >> 6) >= 2 && encrypted_ts_check)
		{
			check_exist_from_pmt(priv, pid, THE_PID_IS_SCRAMBLE);
			stream_skip(stream, buf_size-1+junk);
			return 0;
		}

		if((!is_start && !tss->is_synced) || ((pid > 1) && (pid < 16)) || (pid == 8191))		//invalid pid
		{
			stream_skip(stream, buf_size-1+junk);
			continue;
		}


		afc = (packet[3] >> 4) & 3;
		if(! (afc % 2))	//no payload in this TS packet
		{
			stream_skip(stream, buf_size-1+junk);
			continue;
		}

		if(afc > 1)
		{
			int c;
			c = stream_read_char(stream);
			buf_size--;
			if(c < 0 || c > 183)	//broken from the stream layer or invalid
			{
				stream_skip(stream, buf_size-1+junk);
				continue;
			}

			//c==0 is allowed!
			if(c > 0)
			{
				uint8_t pcrbuf[188];
				int flags = stream_read_char(stream);
				int has_pcr;
				rap_flag = (flags & 0x40) >> 6;
				has_pcr = flags & 0x10;

				buf_size--;
				c--;
				stream_read(stream, pcrbuf, c);
				
				if(has_pcr)
				{
					int pcr_pid = prog_pcr_pid(priv, priv->prog);
					if(pcr_pid == pid)
					{
						uint64_t pcr, pcr_ext;

						pcr  = (int64_t)(pcrbuf[0]) << 25;
						pcr |=  pcrbuf[1]         << 17 ;
						pcr |= (pcrbuf[2]) << 9;
						pcr |=  pcrbuf[3]  <<  1 ;
						pcr |= (pcrbuf[4] & 0x80) >>  7;

						pcr_ext = (pcrbuf[4] & 0x01) << 8;
						pcr_ext |= pcrbuf[5];

						pcr = pcr * 300 + pcr_ext;

						demuxer->reference_clock = (double)pcr/(double)27000000.0;
					}
				}

				buf_size -= c;
				if(buf_size == 0)
					continue;
			}
		}

		//find the program that the pid belongs to; if (it's the right one or -1) && pid_type==SL_SECTION
		//call parse_sl_section()
		pmt = pmt_of_pid(priv, pid, &mp4_dec);
		if(mp4_dec)
		{
			fill_extradata(mp4_dec, tss);
			if(IS_VIDEO(mp4_dec->object_type) || IS_AUDIO(mp4_dec->object_type))
			{
				tss->type = SL_PES_STREAM;
				tss->subtype = mp4_dec->object_type;
			}
		}


		//TABLE PARSING

		base = priv->ts.packet_size - buf_size;

		priv->last_pid = pid;

		is_video = IS_VIDEO(tss->type) || (tss->type==SL_PES_STREAM && IS_VIDEO(tss->subtype));
		is_audio = IS_AUDIO(tss->type) || (tss->type==SL_PES_STREAM && IS_AUDIO(tss->subtype)) || (tss->type == PES_PRIVATE1);
		is_sub	= IS_SUB(tss->type);

		pid_type = pid_type_from_pmt(priv, pid);

		// PES CONTENT STARTS HERE
		if(! probe)
		{
#if 0		
#ifdef DVB_SUBTITLES
			if((is_video || is_audio || is_sub) && is_start && !priv->ts.streams[pid].sh)	//Fuchun 2010.09.28
#else
			if((is_video || is_audio) && is_start && !priv->ts.streams[pid].sh)
#endif
				ts_add_stream(demuxer, tss);
#else // else of 0
			if (is_start && !priv->ts.streams[pid].sh)
			{
				char do_add_stream = 0;
				if (is_audio)
				{
					if (progid_for_pid(priv, pid, 0) == priv->prog)
						do_add_stream = 1;
					switch_debug_printf("@@@ In [%s][%d] Audio : We got other audio stream [%d], now play pid is [%d][%d] do_add_stream[%d]@@@\n", __func__, __LINE__, pid, demuxer->audio->id, priv->ts.streams[pid].id, do_add_stream);
				}
#ifdef DVB_SUBTITLES
				else if (is_sub)
				{
					sh_sub_t *sh_sub = demuxer->sub->sh;

					if (sh_sub && (progid_for_pid(priv, pid, 0) == priv->prog))
						do_add_stream = 1;
					switch_debug_printf("@@@ In [%s][%d] Subtitle: We got other audio stream [%d], now play pid is [%d] do_add_stream[%d]@@@\n", __func__, __LINE__, pid, sh_sub ? sh_sub->sid : -1, do_add_stream);
				}
#endif /* end of DVB_SUBTITLES */				
				else
				{
					/* This stream is video, but we not support multi video stream switch now */
					do_add_stream = 0;
					switch_debug_printf("@@@ In [%s][%d] Video : We got the pid [%d], now play pid is [%d][%d] @@@\n", __func__, __LINE__, pid, demuxer->video->id, priv->ts.streams[pid].id);
				}
				/* We only add the same program audio to stream */
				if (do_add_stream)
					ts_add_stream(demuxer, tss);
				else
				{
					/* Need check next packet */
				}

			}
#endif // end of 0

			if((pid == demuxer->sub->id))	//or the lang is right
			{
				pid_type = SPU_DVD;
			}

			if(is_video && (demuxer->video->id == priv->ts.streams[pid].id))
			{
				ds = demuxer->video;

				dp = &priv->fifo[1].pack;
				dp_offset = &priv->fifo[1].offset;
				buffer_size = &priv->fifo[1].buffer_size;
				si = &priv->vstr;
			}
			else if(is_audio && (demuxer->audio->id == priv->ts.streams[pid].id))
			{
				ds = demuxer->audio;

				dp = &priv->fifo[0].pack;
				dp_offset = &priv->fifo[0].offset;
				buffer_size = &priv->fifo[0].buffer_size;
				si = &priv->astr;
			}
			else if(is_sub
				|| IS_SUB(pid_type))
			{
#ifdef DVB_SUBTITLES
				sh_sub_t *sh_sub = demuxer->sub->sh;

				if(sh_sub && sh_sub->sid == tss->pid)
#else
				//SUBS are infrequent, so the initial detection may fail
				// and we may need to add them at play-time
				if(demuxer->sub->id == -1)
				{
					uint16_t p;
					p = progid_for_pid(priv, tss->pid, priv->prog);

					if(p == priv->prog)
					{
						int asgn = 0;
						uint8_t *lang;

						if(dvdsub_lang)
						{
							if ((lang = pid_lang_from_pmt(priv, pid)))
								asgn = (strncmp(lang, dvdsub_lang, 3) == 0);
						}
						else		//no language specified with -slang
							asgn = 1;

						if(asgn)
						{
							demuxer->sub->id = tss->pid;
							mp_msg(MSGT_DEMUX, MSGL_INFO, "CHOSEN SUBs pid 0x%x (%d) FROM PROG %d\n", tss->pid, tss->pid, priv->prog);
						}
					}
				}

				if(demuxer->sub->id == tss->pid)
#endif
				{
					ds = demuxer->sub;

					dp = &priv->fifo[2].pack;
					dp_offset = &priv->fifo[2].offset;
					buffer_size = &priv->fifo[2].buffer_size;
				}
				else
				{
					stream_skip(stream, buf_size+junk);
					continue;
				}
			}

			//IS IT TIME TO QUEUE DATA to the dp_packet?
			if(is_start && (dp != NULL))
			{
				retv = fill_packet(demuxer, ds, dp, dp_offset, si);
			}

			if(dp && *dp == NULL)
			{
				if(*buffer_size > MAX_PACK_BYTES)
					*buffer_size = MAX_PACK_BYTES;
				*dp = new_demux_packet(*buffer_size);	//es->size
				
				dp_last = *dp;
				
				*dp_offset = 0;
				if(! *dp)
				{
					fprintf(stderr, "fill_buffer, NEW_ADD_PACKET(%d)FAILED\n", *buffer_size);
					continue;
				}
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "CREATED DP(%d)\n", *buffer_size);
			}
		}


		if(probe || !dp)	//dp is NULL for tables and sections
		{
			p = &packet[base];
		}
		else	//feeding
		{
			if(*dp_offset + buf_size > *buffer_size)
			{
				//*buffer_size = *dp_offset + buf_size + TS_FEC_PACKET_SIZE;
				*buffer_size = *dp_offset + (64 * 1024); // every time we add 64k
				resize_demux_packet(*dp, *buffer_size);
			}
			p = &((*dp)->buffer[*dp_offset]);
		}

		len = stream_read(stream, p, buf_size);
		if(len < buf_size)
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2,  "\r\nts_parse() couldn't read enough data: %d < %d\r\n", len, buf_size);
			continue;
		}
		stream_skip(stream, junk);

		if(pid  == 0)
		{
			if (probe)	/* Carlos add on 2011-01-19, reduce run times*/
				parse_pat(priv, is_start, p, buf_size);
			continue;
		}
		else if((tss->type == SL_SECTION) && pmt)
		{
			int k, mp4_es_id = -1;
			ts_section_t *section;
			for(k = 0; k < pmt->mp4es_cnt; k++)
			{
				if(pmt->mp4es[k].decoder.object_type == MP4_OD && pmt->mp4es[k].decoder.stream_type == MP4_OD)
					mp4_es_id = pmt->mp4es[k].id;
			}
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "MP4ESID: %d\n", mp4_es_id);
			for(k = 0; k < pmt->es_cnt; k++)
			{
				if(pmt->es[k].mp4_es_id == mp4_es_id)
				{
					section = &(tss->section);
					parse_sl_section(pmt, section, is_start, &packet[base], buf_size);
				}
			}
			continue;
		}
		else
		{
			/* 2011-07-04 need marked the below condition for VOD dynamic changed video and audio pid */
//			if (probe) /* Carlos add for speed up, because this case is only check when before playing video/audio/sub */
			{
				progid = prog_id_in_pat(priv, pid);
				if(progid != -1)
				{
					if(pid != demuxer->video->id && pid != demuxer->audio->id && pid != demuxer->sub->id)
					{
						//parse_pmt(priv, progid, pid, is_start, &packet[base], buf_size);
						parse_pmt(priv, progid, pid, is_start, &packet[base], buf_size, probe);
						continue;
					}
					else
						mp_msg(MSGT_DEMUX, MSGL_ERR, "Argh! Data pid %d used in the PMT, Skipping PMT parsing!\n", pid);
				}
			}
		}

		if(!probe && !dp)
			continue;

		if(is_start)
		{
			uint8_t *lang = NULL;

			mp_msg(MSGT_DEMUX, MSGL_DBG2, "IS_START\n");

			len = pes_parse2(p, buf_size, es, pid_type, pmt, pid);
			if(! len)
			{
				tss->is_synced = 0;
				continue;
			}
			es->pid = tss->pid;
			tss->is_synced |= es->is_synced || rap_flag;
			tss->payload_size = es->payload_size;

			if((is_sub || is_audio) && (lang = pid_lang_from_pmt(priv, es->pid)))
			{
				memcpy(es->lang, lang, 3);
				es->lang[3] = 0;
			}
			else
				es->lang[0] = 0;

			if(probe)
			{
#ifdef CHECK_EXIST_FLAGS
				check_exist_from_pmt(priv, es->pid, 0);
#endif // end of CHECK_EXIST_FLAGS
				if(es->type == UNKNOWN)
					return 0;

				tss->type = es->type;
				tss->subtype = es->subtype;

				return 1;
			}
			else
			{
#ifdef BUGGY_FIXED_DTS_PROBLEM
				if (es->pts == 0.0f)
				{
					if (Check_Audio_PTS_Zero && is_audio && (tss->type == AUDIO_DTS))
					{
						Check_Audio_PTS_Zero = 0;
						debug_printf("#### In [%s][%d] Check_Audio_PTS_Zero 0 ####\n", __func__, __LINE__);
					}
					es->pts = tss->pts = tss->last_pts;
				}
#else // else of BUGGY_FIXED_DTS_PROBLEM
				if(es->pts == 0.0f)
					es->pts = tss->pts = tss->last_pts;
#endif // end of BUGGY_FIXED_DTS_PROBLEM					
				else
					tss->pts = tss->last_pts = es->pts;

				mp_msg(MSGT_DEMUX, MSGL_DBG2, "ts_parse, NEW pid=%d, PSIZE: %u, type=%X, start=%p, len=%d\n",
					es->pid, es->payload_size, es->type, es->start, es->size);

				demuxer->filepos = stream_tell(demuxer->stream) - es->size;
				memmove(p, es->start, es->size);
				*dp_offset += es->size;
				(*dp)->flags = 0;
				(*dp)->pos = stream_tell(demuxer->stream);
				(*dp)->pts = es->pts;

				if(retv > 0)
				{
					//Fuchun 2010.03.01
					if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2) && mpeg_fast == 0)
  						mpeg_fast = 1;
				
					return retv;
				}
				else
					continue;
			}
		}
		else
		{
			uint16_t sz;

			es->pid = tss->pid;
			es->type = tss->type;
			es->subtype = tss->subtype;
#ifdef SUPPORT_SS_FINE_TUNE
			if (probe != SS_CHECK_PTS)
				es->pts = tss->pts = tss->last_pts;
#endif
			es->start = &packet[base];


			if(tss->payload_size > 0)
			{
				sz = FFMIN(tss->payload_size, buf_size);
				tss->payload_size -= sz;
				es->size = sz;
			}
			else
			{
				if(is_video)
				{
					sz = es->size = buf_size;
				}
				else
				{
					continue;
				}
			}


			if(! probe)
			{
				*dp_offset += sz;

				if(*dp_offset >= MAX_PACK_BYTES)
				{
					(*dp)->pts = tss->last_pts;
					retv = fill_packet(demuxer, ds, dp, dp_offset, si);
					return 1;
				}

				continue;
			}
			else
			{
				memcpy(es->start, p, sz);

				if(es->size)
					return es->size;
				else
					continue;
			}
		}
	}

	return 0;
}


void skip_audio_frame(sh_audio_t *sh_audio);

static void reset_fifos(demuxer_t *demuxer, int a, int v, int s)
{
	ts_priv_t* priv = demuxer->priv;
	if(a)
	{
		if(priv->fifo[0].pack != NULL)
		{
			free_demux_packet(priv->fifo[0].pack);
			priv->fifo[0].pack = NULL;
		}
		priv->fifo[0].offset = 0;
	}

	if(v)
	{
		if(priv->fifo[1].pack != NULL)
		{
			free_demux_packet(priv->fifo[1].pack);
			priv->fifo[1].pack = NULL;
		}
		priv->fifo[1].offset = 0;
	}

	if(s)
	{
		if(priv->fifo[2].pack != NULL)
		{
			free_demux_packet(priv->fifo[2].pack);
			priv->fifo[2].pack = NULL;
		}
		priv->fifo[2].offset = 0;
	}
	demuxer->reference_clock = MP_NOPTS_VALUE;
}

#ifdef FAST_TSDEMUX	//Barry 2010-11-23
unsigned int ts_skip_bits = 0;
void h264bsdDecodeExpGolombUnsigned(unsigned char *pStrmData, unsigned int *codeNum)
{
	unsigned int bits = 0;
	if (ts_skip_bits == 0)
		bits = (pStrmData[0]<<24) | (pStrmData[1]<<16) | (pStrmData[2]<<8) | (pStrmData[3]);
	else if (ts_skip_bits <= 7)
	{
		bits = ((pStrmData[0] & ((1<<(8-ts_skip_bits))-1))<<24) | (pStrmData[1]<<16) | (pStrmData[2]<<8) | (pStrmData[3]);
		bits <<= ts_skip_bits;
		bits |= (pStrmData[4] >> (8-ts_skip_bits));
	}
//	else
//		mplayer_debug("%s: ts_skip_bits[%d] more than 8 bits, need to check!\n", __func__, ts_skip_bits);

//	mplayer_debug("====>   ts_skip_bits=%d, bits=%.8X\n", ts_skip_bits, bits);

	/* first bit is 1 -> code length 1 */
	if (bits >= 0x80000000)
	{
		ts_skip_bits += 1;
		*codeNum = 0;
	}
	/* second bit is 1 -> code length 3 */
	else if (bits >= 0x40000000)
	{
		ts_skip_bits += 3;
		*codeNum = 1 + ((bits >> 29) & 0x1);
	}
	/* third bit is 1 -> code length 5 */
	else if (bits >= 0x20000000)
	{
		ts_skip_bits += 3;
		*codeNum = 3 + ((bits >> 27) & 0x3);
	}
	/* fourth bit is 1 -> code length 7 */
	else if (bits >= 0x10000000)
	{
		ts_skip_bits += 7;
		*codeNum = 7 + ((bits >> 25) & 0x7);
	}
	/* other code lengths */
//	else
//		mplayer_debug("%s: Other code lengths, need to check!\n", __func__);
}
#endif
//Polun 2011-12-13 for H264 get frame num. 
unsigned int h264bsdGetBits(unsigned char * pStrmData, unsigned int numBits)
{

    unsigned int out;

   if (ts_skip_bits == 0)
   {   
        out = (pStrmData[0]<<24) | (pStrmData[1]<<16) | (pStrmData[2]<<8) | (pStrmData[3]);
        out >>=  (32 - numBits);
   }
   else if (ts_skip_bits <= 7)
   {
        	out = ((pStrmData[0] & ((1<<(8-ts_skip_bits))-1))<<24) | (pStrmData[1]<<16) | (pStrmData[2]<<8) | (pStrmData[3]);
        	out <<= ts_skip_bits;
        	out |= (pStrmData[4] >> (8-ts_skip_bits));
              out >>= (32 - numBits);
    }

    return out;
}


static void demux_seek_ts(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
	demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
	ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
	int i, video_stats;
	off_t newpos;
	int found_keyframe = 0;
	int ii = 0;
#ifdef DVB_SUBTITLES
	demux_stream_t *d_sub=demuxer->sub;
	sh_sub_t *sh_sub=d_sub->sh;
#endif // end of DVB_SUBTITLES
#ifdef HW_TS_DEMUX
	extern int FFFR_to_normal;
#endif /* end of HW_TS_DEMUX */	

#if defined(ADJUST_STREAM_PTS) && defined(SUPPORT_SS_FINE_TUNE)	//Barry 2010-01-07
	if ( d_video && (flags & 1) )	//for -ss option
	{
		if (correct_sub_pts == 0.0 && first_video_pts > 0 && first_set_sub_pts == 1)
		{
			correct_sub_pts = first_video_pts;
			first_set_sub_pts = 0;
		}
	}
#endif
#if 0 /* Carlos fixed on 2011-01-10 */
	// FIXME: 20110107 workaround, fix bdiso can't seek chapter problem
	if (bluray_device && per_second_pos != 0.0)
		per_second_pos = 0.0;
#endif
	//================= seek in MPEG-TS ==========================
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
	found_keyframe_seek = 0;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
//mplayer_debug(" #####  in seek  VideoBufferIdx=0x%x\n", VideoBufferIdx);

#ifdef QT_SUPPORT_DVBT
	ts_dump_streams(demuxer->priv);
#endif /* end of QT_SUPPORT_DVBT */
	reset_fifos(demuxer, sh_audio != NULL, sh_video != NULL, demuxer->sub->id > 0);
	//mplayer_debug("==before flush vq:[%d] aq:[%d] ==\n", d_video->packs, d_audio->packs);

	demux_flush(demuxer);

	//Barry 2010-08-11
	//if (ts_avc1_pts_queue_cnt)
	if (ts_avc1_pts_queue_cnt && !speed_mult)	//Barry 2010-11-25
	{
		ts_avc1_pts_queue_cnt = 0;
		memset(ts_avc1_pts_queue, 0, (h264_reorder_num*(2-h264_frame_mbs_only)+1)*sizeof(float));
	}
	if (check_fast_ts_startcode)
		check_fast_ts_startcode = 0;

#ifdef HW_TS_DEMUX
	if (hwtsdemux)
	{
		ts_seek_status = TS_SEEK_START;	
		/* Stop filter, will flush data in kernel space */
		pthread_mutex_lock(&ts_seekmtx); // carlos 2010-06-24 avoid kernel panic when use seek
#ifdef HWTS_WAKEUP		
		pwake_up(&fillcond, &fillmtx);
#endif // end of HWTS_WAKEUP		
		/* Avoid write data when seek function */
		for (ii = 0; ii < BUFFER_COUNT ; ii++)
			write_toggle_len[ii] = 0;
		read_toggle = write_toggle = 0;

		debug_printf("=======start seek [%s][%d]===\n", __func__, __LINE__);
#ifdef KEEP_FILTER_RUNNING
		if (sh_audio)
		{
			if (ioctl(priv->fifo[0].pfd, DMX_PAUSE_FILTER) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[0].pfd, errno);
			}
		}
		debug_printf(" #[%s]: do audio filter I/O  #\n", __func__);
		if (sh_video)
		{
			if (ioctl(priv->fifo[1].pfd, DMX_PAUSE_FILTER) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[1].pfd, errno);
			}
		}
		debug_printf(" #[%s]: do video filter I/O  #\n", __func__);
#ifdef DVB_SUBTITLES
		if (sh_sub)
		{
			if (ioctl(priv->fifo[2].pfd, DMX_PAUSE_FILTER) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[2].pfd, errno);
			}
		}
		debug_printf(" #[%s]: do sub filter I/O  #\n", __func__);
#endif // end of DVB_SUBTITLES		
#else // else of KEEP_FILTER_RUNNING
		if (sh_audio)
			dvb_demux_stop(priv->fifo[0].pfd);
		if (sh_video)
			dvb_demux_stop(priv->fifo[1].pfd);
#ifdef DVB_SUBTITLES
		if (sh_sub)
			dvb_demux_stop(priv->fifo[2].pfd);
#endif // end of DVB_SUBTITLES			
#endif // end of KEEP_FILTER_RUNNING
		debug_printf("=======start seek [%s][%d] close audio and video ok===\n", __func__, __LINE__);
		if ( !(flags & 1) )	//Barry 2011-10-12
			change_axi_speed(d_video, priv, IN_FF_FR);
	}
#endif // end of HW_TS_DEMUX
	debug_printf("rel_seek_secs is [%f]\n", rel_seek_secs);
	debug_printf("audio_delay is [%f]\n", audio_delay);
	debug_printf("flags is [%d]\n", flags);

	video_stats = (sh_video != NULL);
	if(video_stats)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "IBPS: %d, vb: %d\r\n", sh_video->i_bps, priv->vbitrate);
		debug_printf("IBPS: %d, vb: %d\r\n", sh_video->i_bps, priv->vbitrate);
		if(priv->vbitrate)
#ifdef HW_TS_DEMUX
		{
			if (hwtsdemux && (flags & SEEK_ABSOLUTE))
			{
				video_stats = 0;
				debug_printf("@@@@@@@@@@@@@@@@@@@@@@ set video_stats = 0 @@@@@@@@@@@@@@@@\n");
			}
			else
				video_stats = priv->vbitrate;
		}
#else // else of HW_TS_DEMUX
			video_stats = priv->vbitrate;
#endif // end of HW_TS_DEMUX
		else
			video_stats = sh_video->i_bps;
	}

	newpos = (flags & SEEK_ABSOLUTE) ? demuxer->movi_start : demuxer->filepos;
	debug_printf ("==== newpos is [%d] - [%d]===\n", newpos, flags & SEEK_FACTOR);
	debug_printf("==after flush vq:[%d] aq:[%d] ==\n", d_video->packs, d_audio->packs);
	if(flags & SEEK_FACTOR) // float seek 0..1
	{
	//	newpos+=(demuxer->movi_end-demuxer->movi_start)*rel_seek_secs;
#ifdef SUPPORT_SS_FINE_TUNE
		if (per_second_pos != 0.0)
			newpos += per_second_pos * rel_seek_secs;
		else
		{
#endif /* end of SUPPORT_SS_FINE_TUNE */
			if(priv->pmt_cnt)
				newpos += (demuxer->movi_end-demuxer->movi_start)*rel_seek_secs*priv->pmt_cnt;
			else
				newpos += (demuxer->movi_end-demuxer->movi_start)*rel_seek_secs;
#ifdef SUPPORT_SS_FINE_TUNE
		}
#endif /* end of SUPPORT_SS_FINE_TUNE */
	}
	else
	{
#ifdef SUPPORT_SS_FINE_TUNE
		if (per_second_pos != 0.0)
			newpos += per_second_pos * rel_seek_secs;
		else
		{
#endif /* end of SUPPORT_SS_FINE_TUNE */
			// time seek (secs)
			if(! video_stats) // unspecified or VBR
			{
				//	newpos += 2324*75*rel_seek_secs; // 174.3 kbyte/sec
				if(priv->pmt_cnt)
					newpos += 2324*75*rel_seek_secs*priv->pmt_cnt;
				else
					newpos += 2324*75*rel_seek_secs;
			}
			else
			{
				//	newpos += video_stats*rel_seek_secs;
				if(priv->pmt_cnt)
					newpos += video_stats*rel_seek_secs*priv->pmt_cnt;
				else
					newpos += video_stats*rel_seek_secs;
			}
#ifdef SUPPORT_SS_FINE_TUNE
		}
#endif /* end of SUPPORT_SS_FINE_TUNE */
	}

	debug_printf ("==== newpos is [%d] part2===\n", newpos);
	//Fuchun 2010.03.16
	if (!FR_to_end && speed_mult < 0 && newpos < 0){
#if 0
		demuxer->stream->eof=1;
		return;
#else
//		speed_mult = 0;
		FR_to_end = 1;
		newpos = demuxer->movi_start;
#endif
	}

	if(newpos < demuxer->movi_start)
  		newpos = demuxer->movi_start;	//begininng of stream

	stream_seek(demuxer->stream, newpos);

	for(i = 0; i < 8192; i++)
		if(priv->ts.pids[i] != NULL)
			priv->ts.pids[i]->is_synced = 0;
#ifdef SUPPORT_SS_FINE_TUNE
	/* Using -ss */
#ifdef CONFIG_BLURAY
	if (flags & SEEK_ABSOLUTE && per_second_pos && (bluray_device == NULL))	
	/* bd-iso don't using SS_FINE_TUNE method */
#else /* else of CONFIG_BLURAY */
	if (flags & SEEK_ABSOLUTE && per_second_pos)	
#endif /* end of CONFIG_BLURAY */	
	{
		ES_stream_t es;
		unsigned char tmp[TS_FEC_PACKET_SIZE];
		double wish_pts = first_video_pts + rel_seek_secs;
		double diff_pts = 0.0;
		off_t new_offset = 0;
		int max_count = 0;
		char over_range = 0;
		off_t current_pos = newpos;

		if (wish_pts > PTS_MAX_NUMBER)
			over_range = 1;
		do
		{
			if(ts_parse(demuxer, &es, tmp, SS_CHECK_PTS) >= 0)
			{
				if(((es.pid == first_video_pid) || (es.pid == first_audio_pid) ) && (es.pts != 0.0))
				{
					if (over_range && (es.pts < first_video_pts))
						diff_pts = wish_pts - (es.pts + PTS_MAX_NUMBER);
					else
						diff_pts = wish_pts - es.pts;
					seek_printf("$$$$ In [%s][%d] wish_pts [%f] es.pts [%f] diff_pts [%f]es.pid [%d]$$$$$$\n", __func__, __LINE__, wish_pts, es.pts, diff_pts, es.pid);
					
					if (fabs(diff_pts) > THE_SEEK_FINE_TUNE_PTS)
					{
						seek_printf("###### diff_pts [%f] per_second_pos[%lld] is [%lld], new_offset[%lld]\n", diff_pts, per_second_pos, new_offset, (off_t)(diff_pts / (double)2 * (double)per_second_pos));
						new_offset = (off_t)(diff_pts / 2.0 * (double)per_second_pos);
						seek_printf("###### new_offset is [%lld]", new_offset);
						newpos += new_offset;
						seek_printf("###### newpos is [%lld]", newpos);
						stream_seek(demuxer->stream, newpos);
						seek_printf("###### now position is [%lld]\n", stream_tell(demuxer->stream));
						es.pts = 0.0;
					}
					if (max_count > MAX_SEEK_LOOP_TIME)
					{
						mplayer_debug("@@@ Sorry, we can't seek to accurate at you wish pts [%f] now pts is [%f]@@@\n", wish_pts, es.pts);
						break;
					}
					max_count++;
				}
				else
					es.pts = 0.0;
			}
			if(demuxer->stream->eof)
			{
				seek_printf("#### We leave [%s][%d], eof[%d]###\n", __func__, __LINE__, demuxer->stream->eof);
				break;
			}
		} while(((fabs(diff_pts) > THE_SEEK_FINE_TUNE_PTS) && (diff_pts > 0.0))|| (es.pts == 0.0));

		seek_printf("@@@@@ In [%s][%d] diff_pts is [%f] es.pts [%f] @@@\n", __func__, __LINE__, diff_pts, es.pts);
		if (demuxer->stream->eof)
		{
			demuxer->stream->eof = 0;
			stream_seek(demuxer->stream, current_pos);
			mplayer_debug("Got eof, reset to current_pos\n");
		}
		FFFR_to_normal = 1;
	}
#endif /* end of SUPPORT_SS_FINE_TUNE */

#ifdef HW_TS_DEMUX
	if (hwtsdemux)
	{
#ifdef KEEP_FILTER_RUNNING
		int flag = 0;
		/* fixed mantis 4702, seek_chapter caused mosaic, kernel driver slow down the speed of AXI ==> flags & SEEK_CHAPTER*/
		if (speed_mult == 0 &&( (rel_seek_secs > 200.0 || rel_seek_secs < -200.0) || (flags & SEEK_CHAPTER) ) )
		{
			flag = 1; /* carlos add on 2011-05-25, flag = 1 mean need slow down AXI speed avoid BD-ISO had mosaic */
			FFFR_to_normal = 1; /* carlos add on 2011-06-03, set the normal AXI speed when seek 1200 and seek chapter */
#ifdef SUPPORT_QT_BD_ISO_ENHANCE		
			if ((flags & SEEK_CHAPTER) && bluray_device && sh_video && (sh_video->format == VIDEO_H264 || sh_video->format == VIDEO_AVC))
				change_axi_speed(d_video, priv, SLOW_DOWN_SPEED);
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */				
		}
		/* Reset Audio filter */
		if (sh_audio)
		{
			if ((i = ioctl(priv->fifo[0].pfd, DMX_RESUME_FILTER, flag)) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[0].pfd, errno);
			}
		}
		if (sh_video)
		{
			if ((i = ioctl(priv->fifo[1].pfd, DMX_RESUME_FILTER, flag)) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[1].pfd, errno);
			}
		}
#ifdef DVB_SUBTITLES
		if (sh_sub)
		{
			if ((i = ioctl(priv->fifo[2].pfd, DMX_RESUME_FILTER, flag)) < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_seek : ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d\r\n", priv->fifo[2].pfd, errno);
			}
		}
#endif // end of DVB_SUBTITLES		
#else // else of KEEP_FILTER_RUNNING
		struct dmx_pes_filter_params pesFilterParams;
		int fd = 0;

		pesFilterParams.input   = DMX_IN_FRONTEND;
		pesFilterParams.output  = DMX_OUT_TAP;
        pesFilterParams.flags   = DMX_IMMEDIATE_START;
		/* Restart ts_seek_status */
		//ts_seek_status = TS_SEEK_END;
		/* Reset Audio filter */
		if (sh_audio)
		{
			fd = priv->fifo[0].pfd;
#ifdef HAVE_DVB_HEAD
			pesFilterParams.pes_type = DMX_PES_AUDIO;
#else
			pesFilterParams.pesType = DMX_PES_AUDIO;
#endif
			pesFilterParams.pid     = sh_audio->aid;
			if ((i = ioctl(fd, DMX_SET_PES_FILTER, &pesFilterParams)) < 0)
				mplayer_debug("demux_reset : ERROR IN SETTING DMX_FILTER %i for fd %d: ERRNO: %d", (int)pesFilterParams.pid , fd, errno);
		}
		/* Reset Video filter */
		if (sh_video)
		{
			fd = priv->fifo[1].pfd;
#ifdef HAVE_DVB_HEAD
			pesFilterParams.pes_type = DMX_PES_VIDEO;
#else
			pesFilterParams.pesType = DMX_PES_VIDEO;
#endif
			pesFilterParams.pid     = sh_video->vid;
			if ((i = ioctl(fd, DMX_SET_PES_FILTER, &pesFilterParams)) < 0)
				mp_msg(MSGT_DEMUX, MSGL_V, "demux_reset : ERROR IN SETTING DMX_FILTER %d for fd %d: ERRNO: %d", (int)pesFilterParams.pid, fd, errno);
		}
		/* Reset Sub filter */
		if (sh_sub && sh_sub->sid > 0)
		{
			fd = priv->fifo[2].pfd;
#ifdef HAVE_DVB_HEAD
			pesFilterParams.pes_type = DMX_PES_SUBTITLE;
#else
			pesFilterParams.pesType = DMX_PES_SUBTITLE;
#endif
			pesFilterParams.pid     = sh_sub->sid;
			if ((i = ioctl(fd, DMX_SET_PES_FILTER, &pesFilterParams)) < 0)
				mp_msg(MSGT_DEMUX, MSGL_V, "demux_reset : ERROR IN SETTING DMX_FILTER %d for fd %d: ERRNO: %d", (int)pesFilterParams.pid, fd, errno);
		}
#endif // end of KEEP_FILTER_RUNNING
		self_write = 1; /* write_stream can start write_data */
		wait_late = 0;
		/* Restart ts_seek_status */
		ts_seek_status = TS_SEEK_END;
		pthread_mutex_unlock(&ts_seekmtx); // carlos 2010-06-24 avoid kernel panic when use seek
	}
#endif // end of HW_TS_DEMUX
	videobuf_code_len = 0;
	if(sh_video != NULL)
		ds_fill_buffer(d_video);
	if(!speed_mult && sh_audio != NULL)
		ds_fill_buffer(d_audio);

	//Barry 2011-01-20
	if (h264_fast_ts_seek && demuxer->stream->eof && speed_mult > 0)
		return;

	while(sh_video != NULL)
	{
//		if(sh_audio && !d_audio->eof && d_video->pts && d_audio->pts)
		if(!speed_mult && sh_audio && !d_audio->eof && d_video->pts && d_audio->pts)	//don't consider about audio pts when FF/FR
		{
			double a_pts=d_audio->pts;
			a_pts+=(ds_tell_pts(d_audio)-sh_audio->a_in_buffer_len)/(double)sh_audio->i_bps;
			if(d_video->pts > a_pts)
			{
				skip_audio_frame(sh_audio);  // sync audio
				continue;
			}
		}


		i = sync_video_packet(d_video);
//mplayer_debug("@@ sync_video_packet i=0x%.2x\n", i);

		//Barry 2011-01-26
		if ( (flags&SEEK_ABSOLUTE)	// seek absolute
			&& sh_video->format == VIDEO_H264 && !h264_fast_ts_seek
			&& d_video->buffer_size < 64 && ((i&0x1F) == 7) )
		{
			h264_fast_ts_seek = 1;
			d_video->buffer_pos = 0;
			break;
		}

		if((sh_video->format == VIDEO_MPEG1) || (sh_video->format == VIDEO_MPEG2))
		{
			if(i==0x1B3 || i==0x1B8) 
			{
#ifdef FAST_TSDEMUX
				found_keyframe = 4;
#endif
				break;
			} // found it!
		}
//		else if((sh_video->format == VIDEO_MPEG4) && (i==0x1B6))
		else if((sh_video->format == VIDEO_MPEG4) && (i==0x100))	//Fuchun 2010.08.31 0x100 = MPEG4 Header Video Object
			break;
		else if(sh_video->format == VIDEO_VC1 && (i==0x10E || i==0x10F))
			break;
		else	//H264
		{
#ifdef FAST_TSDEMUX	//Barry 2010-11-23
			if (h264_fast_ts_seek)
			{
//mplayer_debug("   h264_fast_ts_seek=%d d_video->buffer_pos=%d d_video->buffer[d_video->buffer_pos-5]=0x%.2x\n", h264_fast_ts_seek, d_video->buffer_pos, d_video->buffer[d_video->buffer_pos-5]);
				if ( d_video->buffer[d_video->buffer_pos-5] == 0x00 && ((i & ~0x60) == 0x101) )
				{
					{
						unsigned int bits = 0;
						ts_skip_bits = 0;
						h264bsdDecodeExpGolombUnsigned(&d_video->buffer[d_video->buffer_pos], &bits);	//first_mb_in_slice
						h264bsdDecodeExpGolombUnsigned(&d_video->buffer[d_video->buffer_pos], &bits);	//sliceType
//						mplayer_debug("=====> slice type = %d\n\n", bits);
						if (bits == 2 || bits == 7)
						{
							found_keyframe = 5;
							break;
						}
					}
				}
			}
			else
#endif

			if(d_video && (d_video->buffer_pos >= 5)) /* Avoid mplayer access wrong address cause SIGSEGV, carlos add 2010-10-05 */ 
			{
			       #if 0 //Polun 2011-11-30 fixed Deep Blue-1080P-H264-DTS6ch-TS.ts FF or FR bug.
				if(d_video->buffer[d_video->buffer_pos-5] == 0x00 && ((i & ~0x60) == 0x105 || (i & ~0x60) == 0x107)) 
				{
#ifdef FAST_TSDEMUX
					found_keyframe = 5;
#endif
					break;
				}
                            #else
                            {
#ifdef FAST_TSDEMUX
					found_keyframe = 5;
#endif
                                   extern int get_h264_iframe;
                                   get_h264_iframe = 0;
					break;
				}
                            #endif
			}
		}

		if(!i || !skip_video_packet(d_video)) break; // EOF?
	}
	if(found_keyframe) d_video->buffer_pos -= found_keyframe;		//Fuchun 2010.03.16
#if 1	//Barry 2011-04-25
//	printf("%.2X %.2X %.2X %.2X %.2X %.2X \n", d_video->buffer[d_video->buffer_pos-6], d_video->buffer[d_video->buffer_pos-5], d_video->buffer[d_video->buffer_pos-4], d_video->buffer[d_video->buffer_pos-3], d_video->buffer[d_video->buffer_pos-2], d_video->buffer[d_video->buffer_pos-1]);
//	printf("******* [%s - %d]   d_video->buffer_pos=%d, d_video->buffer_size=%d\n", __func__, __LINE__, d_video->buffer_pos, d_video->buffer_size);
	if ( (d_video->buffer_pos >=6 && d_video->buffer[d_video->buffer_pos-6] == 0 && d_video->buffer[d_video->buffer_pos-5] == 0 && d_video->buffer[d_video->buffer_pos-4] == 0 && d_video->buffer[d_video->buffer_pos-3] == 1 && d_video->buffer[d_video->buffer_pos-2] == 9) &&
		(d_video->buffer_size > 70 && !(d_video->buffer[65] == 0 && d_video->buffer[66] == 0 && d_video->buffer[67] == 0 && d_video->buffer[68] == 1 && d_video->buffer[69] == 0x67))
	   )
		d_video->buffer_pos -= 6;
#endif

#if 1	//Barry 2011-04-24
	if ( (flags & 1) && !hddvd_check && (sh_video->fps > 24) && (pre_packet_diff_pts > 0.5 && pre_packet_diff_pts < 0.51))
	{
		printf("[%s - %d]    pre_packet_diff_pts = %f     Real_FPS should be = 23.97 not %f\n", __func__, __LINE__, pre_packet_diff_pts, sh_video->fps);
		sh_video->fps = 23.97;
		sh_video->frametime = 1.0/sh_video->fps;
		hddvd_check = 1;
	}
#endif
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
	found_keyframe_seek = 1;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */	
}


static int demux_ts_fill_buffer(demuxer_t * demuxer, demux_stream_t *ds)
{
	ES_stream_t es;
	ts_priv_t *priv = (ts_priv_t *)demuxer->priv;

#ifdef HW_TS_DEMUX
	if (hwtsdemux)
	{
#ifdef SUPPORT_NETWORKING_HWTS
		if (hwtsnet)
			return hw_ts_parse(demuxer, &es, priv, ds, TS_MAX_RETRY_COUNT * 10);
		else
#endif /* end of SUPPORT_NETWORKING_HWTS */
			return hw_ts_parse(demuxer, &es, priv, ds, bluray_device? TS_MAX_RETRY_COUNT*2 : TS_MAX_RETRY_COUNT);
	}
	else
#endif // end of HW_TS_DEMUX
	return -ts_parse(demuxer, &es, priv->packet, 0);
}

#ifdef DVBT_USING_NORMAL_METHOD
int dvb_fill_ts_buffer(demuxer_t *demux, ts_priv_t *ts_priv,  demux_stream_t *ds)
{
	ES_stream_t es;
	if (skydvb_thread_running == -1)
		return 1;
	//dvb_debug_printf("## Carlos in [%s][%d], last_pid is [%d] ###\n", __func__, __LINE__, ts_priv->last_pid);
	return hw_ts_parse(demux, &es, ts_priv, NULL, DVB_MAX_RETRY_COUNT);
}

#endif // end of #ifdef DVBT_USING_NORMAL_METHOD

static int ts_check_file_dmx(demuxer_t *demuxer)
{
    return ts_check_file(demuxer) ? DEMUXER_TYPE_MPEG_TS : 0;
}

static int is_usable_program(ts_priv_t *priv, pmt_t *pmt)
{
	int j;

	for(j = 0; j < pmt->es_cnt; j++)
	{
		if(priv->ts.pids[pmt->es[j].pid] == NULL || priv->ts.streams[pmt->es[j].pid].sh == NULL)
			continue;
		if(
			priv->ts.streams[pmt->es[j].pid].type == TYPE_VIDEO ||
			priv->ts.streams[pmt->es[j].pid].type == TYPE_AUDIO
		)
			return 1;
	}

	return 0;
}

static int demux_ts_control(demuxer_t *demuxer, int cmd, void *arg)
{
	ts_priv_t* priv = (ts_priv_t *)demuxer->priv;
       demux_stream_t *d_video;
       sh_video_t     *sh_video;
       d_video = demuxer->video;
       sh_video = d_video->sh;

	switch(cmd)
	{
		case DEMUXER_CTRL_SWITCH_AUDIO:
		case DEMUXER_CTRL_SWITCH_VIDEO:
		{
			void *sh = NULL;
			int i, n;
			int reftype, areset = 0, vreset = 0;
			demux_stream_t *ds;

			if(cmd == DEMUXER_CTRL_SWITCH_VIDEO)
			{
				reftype = TYPE_VIDEO;
				ds = demuxer->video;
				vreset  = 1;
			}
			else
			{
				reftype = TYPE_AUDIO;
				ds = demuxer->audio;
				areset = 1;
			}
			n = *((int*)arg);
			if(n == -2)
			{
				reset_fifos(demuxer, areset, vreset, 0);
				ds->id = -2;
				ds->sh = NULL;
				ds_free_packs(ds);
				*((int*)arg) = ds->id;
				return DEMUXER_CTRL_OK;
			}

			if(n < 0)
			{
				for(i = 0; i < 8192; i++)
				{
					if(priv->ts.streams[i].id == ds->id && priv->ts.streams[i].type == reftype)
						break;
				}

				while(!sh)
				{
					i = (i+1) % 8192;
					if(priv->ts.streams[i].type == reftype)
					{
						if(priv->ts.streams[i].id == ds->id)	//we made a complete loop
							break;
						sh = priv->ts.streams[i].sh;
					}
				}
			}
			else	//audio track <n>
			{
				if (n >= 8192 || priv->ts.streams[n].type != reftype) return DEMUXER_CTRL_NOTIMPL;
				i = n;
						sh = priv->ts.streams[i].sh;
			}

			if(sh)
			{
#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
               /* STOP filter */
               if(hwtsdemux && (priv->fifo[0].pfd > 0) && (ds->id != priv->ts.streams[i].id))
               {
//                   extern int seek_sync_flag;
                   pthread_mutex_lock(&ts_switch);
                   dvb_demux_stop(priv->fifo[0].pfd);
                   switch_debug_printf("### In [%s][%d] switched to audio pid %d, ds->id[%d] id: %d, sh: %p, new type is [%x]\n", __func__, __LINE__, i, ds->id, priv->ts.streams[i].id, sh, priv->ts.pids[i]->type);
#ifdef BUGGY_FIXED_DTS_PROBLEM
				   if (priv->ts.pids[i]->type == AUDIO_DTS)
					   Check_Audio_PTS_Zero = 1;
				   else
					   Check_Audio_PTS_Zero = 0;
				   Got_Special_DTS_CASE = 0;
				   switch_debug_printf("#### In [%s][%d] set Check_Audio_PTS_Zero [%d] Got_Special_DTS_CASE [%d] ####\n", __func__, __LINE__, Check_Audio_PTS_Zero, Got_Special_DTS_CASE);
#endif // end of BUGGY_FIXED_DTS_PROBLEM		
//				   seek_sync_flag = 4; 
                   dvb_set_tsout_filt(priv->fifo[0].pfd, i, DMX_PES_AUDIO, 0);
                   pthread_mutex_unlock(&ts_switch);
               }
			   else
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
#ifdef BUGGY_FIXED_DTS_PROBLEM
			   {
				   Got_Special_DTS_CASE = 0;
//				   Check_Audio_PTS_Zero = 1; /* fixed CCIR3311.ts has audio noise */
				   if (priv->ts.pids[i]->type == AUDIO_DTS)
					   Check_Audio_PTS_Zero = 1;
				   else 
					   Check_Audio_PTS_Zero = 0;
				   switch_debug_printf("#### In [%s][%d] set Check_Audio_PTS_Zero [%d] Got_Special_DTS_CASE [%d] ####\n", __func__, __LINE__, Check_Audio_PTS_Zero, Got_Special_DTS_CASE);
			   }
#endif // end of BUGGY_FIXED_DTS_PROBLEM
				if(ds->id != priv->ts.streams[i].id)
					reset_fifos(demuxer, areset, vreset, 0);
				ds->id = priv->ts.streams[i].id;
				ds->sh = sh;
				ds_free_packs(ds);
				mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndemux_ts, switched to audio pid %d, id: %d, sh: %p\r\n", i, ds->id, sh);
			}

			*((int*)arg) = ds->id;
			return DEMUXER_CTRL_OK;
		}

		case DEMUXER_CTRL_IDENTIFY_PROGRAM:		//returns in prog->{aid,vid} the new ids that comprise a program
		{
			int i, j, cnt=0;
			int vid_done=0, aid_done=0;
			pmt_t *pmt = NULL;
			demux_program_t *prog = arg;

			if(priv->pmt_cnt < 2)
				return DEMUXER_CTRL_NOTIMPL;

			if(prog->progid == -1)
			{
				int cur_pmt_idx = 0;

				for(i = 0; i < priv->pmt_cnt; i++)
					if(priv->pmt[i].progid == priv->prog)
					{
						cur_pmt_idx = i;
						break;
					}

				i = (cur_pmt_idx + 1) % priv->pmt_cnt;
				while(i != cur_pmt_idx)
				{
					pmt = &priv->pmt[i];
					cnt = is_usable_program(priv, pmt);
					if(cnt)
						break;
					i = (i + 1) % priv->pmt_cnt;
				}
			}
			else
			{
				for(i = 0; i < priv->pmt_cnt; i++)
					if(priv->pmt[i].progid == prog->progid)
					{
						pmt = &priv->pmt[i]; //required program
						cnt = is_usable_program(priv, pmt);
					}
			}

			if(!cnt)
				return DEMUXER_CTRL_NOTIMPL;

			//finally some food
			prog->aid = prog->vid = -2;	//no audio and no video by default
			for(j = 0; j < pmt->es_cnt; j++)
			{
				if(priv->ts.pids[pmt->es[j].pid] == NULL || priv->ts.streams[pmt->es[j].pid].sh == NULL)
					continue;

				if(!vid_done && priv->ts.streams[pmt->es[j].pid].type == TYPE_VIDEO)
				{
					vid_done = 1;
					prog->vid = pmt->es[j].pid;
				}
				else if(!aid_done && priv->ts.streams[pmt->es[j].pid].type == TYPE_AUDIO)
				{
					aid_done = 1;
					prog->aid = pmt->es[j].pid;
				}
			}

			priv->prog = prog->progid = pmt->progid;
			return DEMUXER_CTRL_OK;
		}

		case DEMUXER_CTRL_PROGRAM_LIST:
		{
			int cur_pmt_idx = -1;
			int ii;
			pmt_t *pmt = NULL;
			demux_program_list_t *p_list = (demux_program_list_t *)arg;

			for(ii = 0; ii < priv->pmt_cnt; ii++)
				if(priv->pmt[ii].progid == priv->prog)
				{
					cur_pmt_idx = ii;
					break;
				}

			if (cur_pmt_idx != -1)
			{
				p_list->aid_num = 0;
				p_list->sid_num = 0;
				p_list->a_list = NULL;
				p_list->s_list = NULL;
				pmt = &priv->pmt[cur_pmt_idx];
				if ((pmt != NULL) && pmt->es_cnt > 0)
				{
					p_list->a_list = (demux_program_list_t *)malloc(sizeof(demux_program_list_t) * pmt->es_cnt);
					p_list->s_list = (demux_program_list_t *)malloc(sizeof(demux_program_list_t) * pmt->es_cnt);
					for(ii = 0; ii < pmt->es_cnt; ii++)
					{
						if (IS_AUDIO(pmt->es[ii].type))
						{
							p_list->a_list[p_list->aid_num].id = pmt->es[ii].pid;
							memcpy(p_list->a_list[p_list->aid_num].lang, pmt->es[ii].lang, 4);
							p_list->aid_num++;
							//mplayer_debug("apid: %d, lang: %s\n", pmt->es[ii].pid, pmt->es[ii].lang);
						} else if (IS_SUB(pmt->es[ii].type)) {
							p_list->s_list[p_list->sid_num].id = pmt->es[ii].pid;
							memcpy(p_list->s_list[p_list->sid_num].lang, pmt->es[ii].lang, 4);
							p_list->sid_num++;
							//mplayer_debug("spid: %d, lang: %s\n", pmt->es[ii].pid, pmt->es[ii].lang);
						}
					}
					return DEMUXER_CTRL_OK;
				}
			}
			return DEMUXER_CTRL_NOTIMPL;
		}
             //Polun 2012-01-05 fixed mantis 6771 duration issue
            case DEMUXER_CTRL_GET_TIME_LENGTH:
                if (sh_video == NULL)	//audio only
                    return DEMUXER_CTRL_DONTKNOW;
		if(ts_file_duration > 0){
                    *(double *)arg = ts_file_duration; 
                    return DEMUXER_CTRL_GUESS;
		}else if(sh_video->i_bps){
                    *(double *)arg = (double)(demuxer->movi_end - demuxer->movi_start) / sh_video->i_bps;
                    return DEMUXER_CTRL_GUESS;
		}
                return DEMUXER_CTRL_DONTKNOW;
		default:
			return DEMUXER_CTRL_NOTIMPL;
	}
}


const demuxer_desc_t demuxer_desc_mpeg_ts = {
  "MPEG-TS demuxer",
  "mpegts",
  "TS",
  "Nico Sabbi",
  "",
  DEMUXER_TYPE_MPEG_TS,
  0, // unsafe autodetect
  ts_check_file_dmx,
  demux_ts_fill_buffer,
  demux_open_ts,
  demux_close_ts,
  demux_seek_ts,
  demux_ts_control
};

#ifdef HW_TS_DEMUX
static void skydvb_write_stream(demuxer_t * demuxer)
{
#ifndef HWTS_USED_FRAMEBUFFER
	int mem_fd = 0;
	struct dvr_map_info st_map_info;
#endif // end of HWTS_USED_FRAMEBUFFER
	ts_priv_t * priv = demuxer->priv;
	extern int is_pause;
#ifdef WRITE_STOP_CHECK
	int do_stop = 0;
#endif // end of WRITE_STOP_CHECK
#ifdef STREAM_DIO_AUTO
//	extern int dio;
#endif // end of  STREAM_DIO_AUTO
	struct sigaction sig_act;
	int ret = 0;
	/* We open filter in skydvb_write_stream */
	demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
#ifdef DVB_SUBTITLES
	demux_stream_t *d_sub=demuxer->sub;
	sh_sub_t *sh_sub=d_sub->sh;
#endif // end of DVB_SUBTITLES
	int check_wait_count = 0;
	/* Carlos add init Global value */
	int ii = 0;
	int video_format = 0;
	char check_write_ok = 1;
	int check_pause = 0;
	self_write = 1;
	wait_late = 0;
	read_toggle = write_toggle = 0;
	last_video_pts = 0.0;

	for (ii = 0; ii < BUFFER_COUNT ; ii++)
		write_toggle_len[ii] = 0;

	mplayer_debug("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
	GET_VIDEO_FORMAT(video_format, sh_video);
#ifdef ENABLE_HARDWARE_TS_START_CODE
	video_format |= DMX_SCODE_ENABLE;
#endif /* end of ENABLE_HARDWARE_TS_START_CODE  */
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
#if 0	//Fuchun 2011.06.08 disable by Carlos, this cause DIE HARD 4.0 video freeze when FF4x or FR2x, fix it later. 
	if (quick_bd)
		video_format |= DMX_SCODE_ENABLE;
#endif
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
	//mplayer_debug("### Carlos in [%s][%d] , video format is [%x] video_format[%d]###\n", __func__, __LINE__, sh_video ? sh_video->format : NULL, video_format);
	//mplayer_debug("%s renice to %d\n", __func__, nice(-19));
	if (demuxer->stream->fd)
	{
		/* At first, we need make sure the demux_fd is open */
		dvr_fd = open_ts_device(priv, DVR_DEVICE);

		if (sh_video && sh_video->vid != DO_NOT_PLAY)
			ret = dvb_set_tsout_filt(priv->fifo[1].pfd, sh_video->vid, DMX_PES_VIDEO, video_format);
		if ((ret == 0)  && sh_audio && sh_audio->aid != DO_NOT_PLAY)
			ret = dvb_set_tsout_filt(priv->fifo[0].pfd, sh_audio->aid, DMX_PES_AUDIO, 0);
#ifdef DVB_SUBTITLES
		if ((ret == 0) && (!no_osd) && (dvdsub_id != DO_NOT_PLAY) && sh_sub && (sh_sub->sid > 0))
			ret = dvb_set_tsout_filt(priv->fifo[2].pfd, sh_sub->sid, DMX_PES_SUBTITLE, 0);
#endif // end of DVB_SUBTITLES
		
		if ((ret == 0) && dvr_fd)
		{
			status_write_thread = THREAD_STATUS_RUNNING;
#ifdef HWTS_USED_FRAMEBUFFER 
#if defined(_SKY_VDEC_V1) || defined(_SKY_VDEC_V2)
			debug_printf("SallocVirtualBase =%p, SallocPhysicalBase =%p\n", SallocVirtualBase, SallocPhysicalBase);
			mplayer_debug("SallocVirtualBase =%p, SallocPhysicalBase =%p\n", SallocVirtualBase, SallocPhysicalBase);
			//ts_buf = (unsigned char *) SallocVirtualBase + (3 << 20);
			//buf_bk = (unsigned char *) SallocPhysicalBase + (3 << 20);
			ts_buf = (unsigned char *) SallocVirtualBase;
			buf_bk = (unsigned char *) SallocPhysicalBase;
			debug_printf("virtual=%p, physical=%p\n", ts_buf, buf_bk);
			mplayer_debug("virtual=%p, physical=%p\n", ts_buf, buf_bk);
#endif
#else // else of HWTS_USED_FRAMEBUFFER
			memset(&st_map_info, 0, sizeof(struct dvr_map_info));
			st_map_info.map_size = TS_BUF_LENGTH*BUFFER_COUNT;
     		if (ioctl(dvr_fd, DMX_DVR_MAP_MEM, &st_map_info) < 0){
     			mplayer_debug("	DMX_DVR_MAP_MEM ioctl failed ..............\n");				
     		}
			
			if((mem_fd = open("/dev/mem", O_RDWR)) < 0) {
    				mplayer_debug(" Open device mem failed\n");
					goto byebye;
  			}
			ts_buf = mmap(0, st_map_info.map_size, (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, (unsigned long)st_map_info.dvr_map_addr);			
			mplayer_debug("phys_addr=%p, ts_buf=%p\n", st_map_info.dvr_map_addr, ts_buf);
			buf_bk = (unsigned char *)st_map_info.dvr_map_addr;
#endif // end of HWTS_USED_FRAMEBUFFER

			sig_act.sa_flags = SA_SIGINFO;
			sig_act.sa_sigaction = hw_io_write;
			if (sigaction(SIGIO, &sig_act, NULL))
				mplayer_debug("=== sigaction failed , errno is [%d] [%s]==\n", errno, strerror(errno));

			fcntl(dvr_fd, F_SETOWN, getpid());
			ret = fcntl(dvr_fd, F_GETFL);
			if (ret == -1)
				mplayer_debug("== F_GETFL failed [%s] [%d] ==\n", strerror(errno), errno);
			
			ret = fcntl(dvr_fd, F_SETFL, ret | FASYNC);
			if (ret == -1)
				mplayer_debug("== F_SETFL failed [%s] [%d] ==\n", strerror(errno), errno);

			start_write_thread |= HWTS_WRITE_STREAM_START;

			change_axi_speed(d_video, priv, NORMAL_SPPED);

			do
			{
				/* users cancel play, exit write_thread, carlos 2010-06-23, fixed cache handle issue */
				//mplayer_debug("%s : %d status_write_thread %d\n", __func__, __LINE__, status_write_thread);
				if (status_write_thread != THREAD_STATUS_RUNNING)
					break;

				if (check_write_ok && write_toggle > 2)
				{
					start_write_thread |= HWTS_WRITE_STREAM_WRITE_OK;
					check_write_ok = 0;
				}
#if 1
				/* Check pause */
				while(is_pause)
				{
					usleep(1000);
				}
#else
				
				do 
				{
					if (is_pause)
						check_pause = 1;
					if (check_pause)
					{
						if (is_pause)
							usleep(1000);
						else
						{
							check_pause = 0;
							self_write = 1;
						}
					}
				}while(check_pause);
#endif 

				/* Check Seek function */
#if 1
	#if 0
				if (ts_seek_status == TS_SEEK_START)
				{
					pthread_mutex_lock(&ts_seekmtx); // carlos 2010-07-16, wait open filter finished
					debug_printf("==== flush all data in lock/unlock , self_write[%d]===\n", self_write);
					ts_seek_status = TS_SEEK_END;
					self_write = 1;
					pthread_mutex_unlock(&ts_seekmtx); // carlos 2010-07-16
				}
	#endif 
#else
				while(ts_seek_status == TS_SEEK_START)
				{
					//mplayer_debug("=== wait ts_seek_status change , now is [%d] start is [%d]==\n", ts_seek_status, TS_SEEK_START);
					usleep(1);
				}
#endif 
#ifdef HWTS_WAKEUP
				if(write_toggle_len[read_toggle] != 0){
					if(pwait_event_timeout(&fillcond, &fillmtx, 1) == ETIMEDOUT){
						if (check_wait_count > 4 && check_wait_count < 10)
							mplayer_debug("%s:%d wait write_toggle_len[%d] timedout, video [%d] audio[%d] sub[%d] check_wait_count[%d]\n", __func__, __LINE__, read_toggle, check_packets(priv->fifo[1].pfd), check_packets(priv->fifo[0].pfd), check_packets(priv->fifo[2].pfd), check_wait_count);
						check_wait_count++;
					}
				}
#endif // end of HWTS_WAKEUP


				pthread_mutex_lock(&ts_seekmtx); // carlos 2010-07-23: Avoid buf_pos > buf_len when called stream_seek function
				if (write_toggle_len[read_toggle] == 0)
				{
#ifdef STREAM_DIO_AUTO
					if (dio)
					{
#if 0						
						//write_toggle_len[read_toggle]= stream_read(demuxer->stream, ((__u32)(buf_bk + TS_BUF_LENGTH * read_toggle) | 0xc0000000), TS_BUF_LENGTH);						
					  	write_toggle_len[read_toggle] = demuxer->stream->fill_buffer(s, mem, total); // read file to 'mem' directly
					  	s->pos += r;
#else						
						//write_toggle_len[read_toggle]= read(demuxer->stream->fd, ((__u32)(buf_bk + TS_BUF_LENGTH * read_toggle) | 0xc0000000) , TS_BUF_LENGTH);
						write_toggle_len[read_toggle]= demuxer->stream->fill_buffer(demuxer->stream, ((__u32)(buf_bk + TS_BUF_LENGTH * read_toggle) | 0xc0000000) , TS_BUF_LENGTH);

						if (write_toggle_len[read_toggle] >= 0 )
							demuxer->stream->pos += write_toggle_len[read_toggle];
#endif 						
					}
					else
						write_toggle_len[read_toggle]= stream_read(demuxer->stream, ts_buf + TS_BUF_LENGTH * read_toggle , TS_BUF_LENGTH);
					
#else // else of STREAM_DIO_AUTO
					write_toggle_len[read_toggle]= stream_read(demuxer->stream, ts_buf + TS_BUF_LENGTH * read_toggle , TS_BUF_LENGTH);
#endif // end of STREAM_DIO_AUTO
					/* The file is over, exit write_thread */
					if (write_toggle_len[read_toggle] <= 0)
					{
						mplayer_debug("The file is over, will exit [%s] len [%d]-[%d] pos[0x%"PRIX64"]\n", __func__, read_toggle, write_toggle_len[read_toggle], (int64_t)stream_tell(demuxer->stream));
						pthread_mutex_unlock(&ts_seekmtx); // carlos 2010-07-19 : Avoid buf_pos > buf_len when called stream_seek function
						break;
					}
					read_toggle = (read_toggle + 1 ) % BUFFER_COUNT;
					check_wait_count = 0;
				}
				pthread_mutex_unlock(&ts_seekmtx); // carlos 2010-07-19 : Avoid buf_pos > buf_len when called stream_seek function

				
#ifdef WRITE_STOP_CHECK /* Control write speed wait the kernel video or audio queue consumption*/
				do
				{
					/* Normal case */
					if (demuxer->video->id != -2 && demuxer->audio->id != -2 && demuxer->video->packs && demuxer->audio->packs)
						do_stop = (((demuxer->video->packs + demuxer->audio->packs) > MAX_QUEUE ) && (demuxer->audio->packs > 5))? 1 : 0;
					else if (demuxer->audio->id == -2)
						do_stop = (demuxer->video->packs > MAX_QUEUE) ? 1 : 0;
					/* Audio only */
					else
						do_stop = (demuxer->audio->packs > 20) ? 1 : 0;
					if(do_stop)
					{
						usleep(8000);
						if (count % (DEBUG_COUNT*3 - 1) == 0)
						mplayer_debug("--------------------usleep 80000, on WRITE_STOP_CHECK-------------, aq[%d] vq[%d]video[%d]audio[%d] normal[%d]\n", demuxer->audio->packs, demuxer->video->packs, demuxer->video->eof, demuxer->audio->eof, demuxer->stream->eof);
					}
					if ((status_write_thread != THREAD_STATUS_RUNNING) || ts_seek_status == TS_SEEK_START || demuxer->stream->eof)
					{
						debug_printf("#### get status_write_thread [%d] ts_seek_status[%d]####, exit \n", status_write_thread, ts_seek_status);
						break;
					}
					count++;
				}while(do_stop);
#endif  // end of WRITE_STOP_CHECK

		
				if(self_write ) 
				{
					if (ts_seek_status != TS_SEEK_START)
					{
						self_write=0;
						hw_io_write(0, NULL, NULL);
					}
					else
						debug_printf("===== self_write [%d] , ts_seek_status[%d] ===\n", self_write, ts_seek_status);
				}	
				/* Something wrong. detect stream->eof is true */
				if (demuxer->stream->eof)
					break;
			}while(1);
		}
		else
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "demux ts: We can't open dvr0 [%d] or ts_out failed [%d]##\n", dvr_fd, ret);
		}
		mplayer_debug("================================exit and set priv->prog = 2\n");
		/* We use prog to notify the source is finished */
		priv->prog = 2;
	}
#ifndef HWTS_USED_FRAMEBUFFER
byebye:
	if (ts_buf)
	{
	  	if (mem_fd) 
    			close(mem_fd);
		ts_buf = buf_bk;
		munmap(ts_buf, st_map_info.map_size);
		ts_buf = NULL;
	}
	else
		mplayer_debug("=== Alloc memory for hwtsdemux failed ===\n");
#endif // end of ifndef HWTS_USED_FRAMEBUFFER
	status_write_thread = THREAD_STATUS_END;
	mplayer_debug("%s:%d end\n", __func__, __LINE__);
}

int check_need_write_ts_video(void *priv, int is_play)
{
	ts_priv_t *ts_priv = (ts_priv_t *)priv;
	int fd = 0;
	struct dmx_buffer_meter st_buffer_params;
	int ret = 0;

	if (ts_priv)
	{
		if(is_play == 1)
		{
			fd = ts_priv->fifo[0].pfd;
			if (fd > 0)
			{
				if (ioctl(fd, DMX_METER_BUFFER, &st_buffer_params) != -1)
				{
					//printf("======Audio Carlos in [%s][%d] spare_size is [%x] , count [%d]=====\n", __func__, __LINE__, st_buffer_params.spare_size, st_buffer_params.queue_pkt_cnt);
					if(st_buffer_params.queue_pkt_cnt != 0)
					{
						/* Make sure at least one audio packet */
						ret = 1;
					}
				}
			}
		}
		else
		{
			fd = ts_priv->fifo[1].pfd;
			if (fd > 0)
			{
				if (ioctl(fd, DMX_METER_BUFFER, &st_buffer_params) != -1)
				{
					//printf("======Carlos in [%s][%d] spare_size is [%x] , count [%d]=====\n", __func__, __LINE__, st_buffer_params.spare_size, st_buffer_params.queue_pkt_cnt);
					if (0x6fffff  < st_buffer_params.spare_size)
					{
						/* Free size < 1MB, need write again */
						ret = 1;
					}
				}
			}
		}		
	}
	return ret;
}
/* For check hwdemux packets */
int check_hwts_packets(ts_priv_t *ts_priv, int fd)
{
	struct dmx_buffer_meter st_buffer_params;
	int ret = 0;
	int real_fd = 0;
	if (ts_priv)
	{
		real_fd = ts_priv->fifo[fd].pfd;
		if (real_fd > 0)
		{
			if (ioctl(real_fd , DMX_METER_BUFFER, &st_buffer_params) != -1)
			{
				//printf("======Carlos in [%s][%d] spare_size is [%x] , count [%d] real_fd[%d] fd[%d]=====\n", __func__, __LINE__, st_buffer_params.spare_size, st_buffer_params.queue_pkt_cnt, real_fd, fd);
				ret = st_buffer_params.queue_pkt_cnt;
			}
		}	
	}
	//printf("===== In [%s][%d] ts_priv[%p] , fd [%d] real_fd[%d], ret[%d] ====\n", __func__, __LINE__, ts_priv, fd, real_fd, ret);
	return ret;
}
int check_need_read_ts_video(void *priv)
{
	ts_priv_t *ts_priv = (ts_priv_t *)priv;
	int fd = 0;
	struct dmx_buffer_meter st_buffer_params;
	int ret = 0;

	if (ts_priv)
	{
		fd = ts_priv->fifo[1].pfd;
		if (fd > 0)
		{
			if (ioctl(fd, DMX_METER_BUFFER, &st_buffer_params) != -1)
			{
				//printf("======Carlos in [%s][%d] spare_size is [%x] , count [%d]=====\n", __func__, __LINE__, st_buffer_params.spare_size, st_buffer_params.queue_pkt_cnt);
				if (st_buffer_params.queue_pkt_cnt > 3)
				{
					/* packet cnt > 3, need ds_fill_buffer */
					ret = 1;
				}
			}
		}	
	}
	return ret;
}

/* Open ts device */
static int open_ts_device(ts_priv_t * priv, int type)
{

	unsigned char *ts_device[2];
	int dvr_fd = 0;
	int ret = 0;
	if (skydroid)
	{
		ts_device[0] = "/dev/dvb0.dvr0";
		ts_device[1] = "/dev/dvb0.demux0";
	}
	else
	{
		ts_device[0] = "/dev/dvb/adapter0/dvr0";
		ts_device[1] = "/dev/dvb/adapter0/demux0";
	}

	if (type == DVR_DEVICE)
	{
		dvr_fd = open(ts_device[0], O_WRONLY);	// ts demux
		ret = dvr_fd;
	}
	else if (type == TS_DEVICE)
	{
		priv->fifo[0].pfd = open(ts_device[1], O_RDWR | O_NONBLOCK); // Audio
		priv->fifo[1].pfd = open(ts_device[1], O_RDWR | O_NONBLOCK); // Video
		priv->fifo[2].pfd = open(ts_device[1], O_RDWR | O_NONBLOCK); // Subs
		if (priv->fifo[0].pfd < 0 || priv->fifo[1].pfd < 0 || priv->fifo[2].pfd < 0)
		{
			if (priv->fifo[0].pfd > 0)
				close(priv->fifo[0].pfd);
			if (priv->fifo[1].pfd > 0)
				close(priv->fifo[1].pfd);
			if (priv->fifo[2].pfd > 0)
				close(priv->fifo[2].pfd);
			ret = -1;
			mplayer_debug("== carlos open device failed fifo[0] is [%d], fifo[1] is [%d], fifo[2] is [%d] == \n", priv->fifo[0].pfd, priv->fifo[1].pfd, priv->fifo[2].pfd);
		}
		else
			ret = 0;
		
	}
	else
		ret = 0;
	return ret;
}

/* fill buffer when we used hw_ts_demux */
// carlos add 20100713
static int hw_ts_parse(demuxer_t *demuxer, ES_stream_t *es, ts_priv_t* priv, demux_stream_t *current_ds, int retry_count)
{
	char keep_doing = 1;
	int got_video_count = 0;
	int got_audio_count = 0;
	int fdmax = 0;
	int read_size = 0;
	int retval = 0;
	unsigned char pes_buf[PES_BUF_LENGTH];
	fd_set rfds;
	struct timeval tv;
	int count = 0 ;
	char filter_type  = 0;
	int pkt_video_read_count = 0;
	int pkt_audio_read_count = 0;
#ifdef DVB_SUBTITLES
	int pkt_sub_read_count = 0;
	int got_sub_count = 0;
	sh_sub_t *sh_sub=demuxer->sub->sh;
#endif // end of DVB_SUBTITLES

	if (current_ds == NULL)
		filter_type = FILTER_DVB;
	if (current_ds == demuxer->audio)
		filter_type = FILTER_AUDIO;
	else if (current_ds == demuxer->video)
		filter_type = FILTER_VIDEO;
	else if (current_ds == demuxer->sub)
		filter_type = FILTER_SUB;
	else
		filter_type = FILTER_DVB;
	do
	{
		/* Check need Audio or Video */
		if ((filter_type & FILTER_AUDIO) || (filter_type &FILTER_DVB))
			pkt_audio_read_count = check_packets(priv->fifo[0].pfd);
		if ((filter_type & FILTER_VIDEO) || (filter_type &FILTER_DVB))
			pkt_video_read_count = check_packets(priv->fifo[1].pfd);
#ifdef DVB_SUBTITLES
		/*
		 * Carlos marked the below condition because somefile one sec only have two video packet that 
		 *	caused too many subtitle packet. 2010-12-16 , fixed for Kenneth Copeland.ts 
		 * We update subtitle packet when everybody call hw_ts_parse.
		 */
		//if ((!no_osd) && (dvdsub_id != DO_NOT_PLAY) && sh_sub && (sh_sub->sid > 0 )&& (filter_type & FILTER_VIDEO))
		if ((!no_osd) && (dvdsub_id != DO_NOT_PLAY) && sh_sub && (sh_sub->sid > 0 ))
			pkt_sub_read_count = check_packets(priv->fifo[2].pfd);
#endif // end of DVB_SUBTITLES
		debug_printf("==== In [%s][%d] before do add packet, filter_type[%d] pkt_audio_read_count[%d], pkt_video_read_count[%d] pkt_sub_read_count[%d]====\n", __func__, __LINE__, filter_type, pkt_audio_read_count, pkt_video_read_count, pkt_sub_read_count);

		//Fuchun 2010.03.16, carlos 20100716 add for hwts_FF > 8
		if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2) && mpeg_fast == 0)
			mpeg_fast = 1;

#ifdef DVB_SUBTITLES
		if (pkt_audio_read_count||pkt_video_read_count||pkt_sub_read_count)
#else // else of DVB_SUBTITLES
		if (pkt_audio_read_count||pkt_video_read_count)
#endif // end of DVB_SUBTITLES
		{
			/* We have need packet, add packet it */
			if(pkt_audio_read_count)
			{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
				if (quick_bd)
					count = ONETIME_BD_MAXREAD_AUDIO_PACKET;
				else
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */				
					count = ONETIME_MAXREAD_AUDIO_PACKET;
				pkt_audio_read_count = (pkt_audio_read_count > count) ? count : pkt_audio_read_count;

				read_size = add_audio_packets(priv->fifo[0].pfd, pes_buf, PES_BUF_LENGTH, demuxer->audio, demuxer->audio->sh, pkt_audio_read_count, demuxer, filter_type);
				got_audio_count += pkt_audio_read_count;
				count = 0;
			}
			if(pkt_video_read_count)
			{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
				if (quick_bd)
				{
					count = ONETIME_BD_MAXREAD_VIDEO_PACKET;
					if ( chk_bufspace(demuxer) )
					{
						pkt_video_read_count = (pkt_video_read_count > count) ? count : pkt_video_read_count;
						read_size = add_video_packets(priv->fifo[1].pfd, pes_buf, PES_BUF_LENGTH, demuxer->video, demuxer->video->sh, pkt_video_read_count, demuxer, &priv->vstr, filter_type, priv->last_pid);
						got_video_count += pkt_video_read_count;
					}
				}
				else
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
				{
					count = ONETIME_MAXREAD_VIDEO_PACKET;

					pkt_video_read_count = (pkt_video_read_count > count) ? count : pkt_video_read_count;
					read_size = add_video_packets(priv->fifo[1].pfd, pes_buf, PES_BUF_LENGTH, demuxer->video, demuxer->video->sh, pkt_video_read_count, demuxer, &priv->vstr, filter_type, priv->last_pid);
					got_video_count += pkt_video_read_count;
				}
				count = 0;
			}
#ifdef DVB_SUBTITLES
			if (pkt_sub_read_count)
			{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
				if (quick_bd)
					count = ONETIME_BD_MAXREAD_SUB_PACKET;
				else
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
					count = ONETIME_MAXREAD_SUB_PACKET;
				/* If the subtitle format is SPU_TELETEXT, we need read 50 packets/per secound, 2011-01-17 */
				if (sh_sub && sh_sub->format == SPU_TELETEXT)
					count *= 10;	/* average is 50 packets per second */
				pkt_sub_read_count = (pkt_sub_read_count > count) ? count : pkt_sub_read_count;
				read_size = add_sub_packets(priv->fifo[2].pfd, pes_buf, PES_BUF_LENGTH, demuxer->sub, pkt_sub_read_count, demuxer, filter_type);
				got_sub_count += pkt_sub_read_count;
				count = 0;
			}
#endif // end of DVB_SUBTITLES
		}
		else
		{
			/* no video and audio data, useing select */
			FD_ZERO(&rfds);
			if (demuxer->audio->id != DO_NOT_PLAY)
				FD_SET(priv->fifo[0].pfd, &rfds);
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
			if (quick_bd)
			{
				if ( (found_keyframe_seek == 1)  && (demuxer->video->id != DO_NOT_PLAY))
				{
					if (chk_bufspace(demuxer) && demuxer->video->packs < 10 )
					{
						FD_SET(priv->fifo[1].pfd, &rfds);
					}
				}

			}
			else
			{
				if ( (found_keyframe_seek == 1)  && (demuxer->video->id != DO_NOT_PLAY))
					FD_SET(priv->fifo[1].pfd, &rfds);
			}

#else /* else of SUPPORT_QT_BD_ISO_ENHANCE */
			if (demuxer->video->id != DO_NOT_PLAY)
				FD_SET(priv->fifo[1].pfd, &rfds);
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */

#ifdef DVB_SUBTITLES
			if ( (!no_osd) && (dvdsub_id > 0) && sh_sub && (sh_sub->sid> 0) && priv->fifo[2].pfd > 0) 
				FD_SET(priv->fifo[2].pfd, &rfds);
#endif // end of DVB_SUBTITLES
			if ( (!no_osd) && (dvdsub_id > 0) && (demuxer->sub->id > 0) && priv->fifo[2].pfd > 0)
				fdmax = priv->fifo[2].pfd + 1;
			else if (demuxer->audio->id != DO_NOT_PLAY)
				fdmax = (priv->fifo[1].pfd > priv->fifo[0].pfd ? priv->fifo[1].pfd : priv->fifo[0].pfd) + 1 ;
			else
				fdmax = priv->fifo[0].pfd + 1 ;

			tv.tv_usec = 10000;
			tv.tv_sec = 0;
			retval = select(fdmax, &rfds, NULL, NULL, &tv);
			if (retval < 0) 
			{
#if 0
				if (check_read_status == 0)
				{
					mplayer_debug("Error Happens retval = 0x%08x!!\n", retval);
					check_read_status = 1;
				}
				usleep(1);
				mplayer_debug("Error Happens retval = 0x%08x!!errno[%d] [%s]\n", retval, errno, strerror(errno));
#endif
			}
			else if (retval == 0) 
			{   //Time out
				count++;
				/* We need read all data of kernel when file is over */
				if (priv->prog == 2)
				{
					/* Check Video status */
					if (demuxer->video->id == DO_NOT_PLAY)
						demuxer->video->eof = 1;
					else 
					{
						if (filter_type & FILTER_VIDEO && demuxer->video->packs  == 0)
							demuxer->video->eof = 1;
					}
					/* Check Audio status */
					if (demuxer->audio->id == DO_NOT_PLAY)
						demuxer->audio->eof = 1;
					else 
					{
						if (filter_type & FILTER_AUDIO && demuxer->audio->packs  == 0)
							demuxer->audio->eof = 1;
					}
					if (demuxer->video->eof == 1 && demuxer->audio->eof == 1)
						demuxer->stream->eof = 1;
					debug_printf("=============We need exit,vq[%d] aq[%d] audio_eof [%d], video_eof[%d] stream[%d]===\n", demuxer->video->packs, demuxer->audio->packs, demuxer->audio->eof, demuxer->video->eof, demuxer->stream->eof);
					break;
				}
			}
			else
			{
				count++;
				/* Check Video */
				if (FD_ISSET(priv->fifo[1].pfd, &rfds))
				{
//printf(" @@@@@@@@ ??? v packs:%d \n\n", demuxer->video->packs);
					if (quick_bd)
					{
						if (chk_bufspace(demuxer) /*&& demuxer->video->packs < 8*/)
						{
							read_size = add_video_packets(priv->fifo[1].pfd, pes_buf, PES_BUF_LENGTH, demuxer->video, demuxer->video->sh, 1, demuxer, &priv->vstr, filter_type, priv->last_pid);
							if (read_size > 0)
							{
								got_video_count++;
								count = 0;
							}
						}
					}
					else
					{
						read_size = add_video_packets(priv->fifo[1].pfd, pes_buf, PES_BUF_LENGTH, demuxer->video, demuxer->video->sh, 1, demuxer, &priv->vstr, filter_type, priv->last_pid);
						if (read_size > 0)
						{
							got_video_count++;
							count = 0;
						}
					}
				}
				/* Check Audio */
				if (FD_ISSET(priv->fifo[0].pfd, &rfds))
				{
					read_size = add_audio_packets(priv->fifo[0].pfd, pes_buf, PES_BUF_LENGTH, demuxer->audio, demuxer->audio->sh, 1, demuxer, filter_type);
					if (read_size > 0)
					{
						got_audio_count++;
						count = 0;
					}
				}
#ifdef DVB_SUBTITLES
				/* Check Subtitle*/
				if ((!no_osd) && (dvdsub_id > 0) && sh_sub && (sh_sub->sid > 0) && FD_ISSET(priv->fifo[2].pfd, &rfds))
				{
					read_size = add_sub_packets(priv->fifo[2].pfd, pes_buf, PES_BUF_LENGTH, demuxer->sub, 1, demuxer, filter_type);
					if (read_size > 0)
					{
						got_sub_count++;
						count = 0;
					}
				}
#endif // end of DVB_SUBTITLES
			}
		}
		if (filter_type & FILTER_AUDIO)
		{
			if(got_audio_count) 
				keep_doing = 0;
		}
		else if (filter_type & FILTER_VIDEO)
		{
			if(got_video_count) 
				keep_doing = 0;
		}
#ifdef DVB_SUBTITLES
		else if (filter_type & FILTER_SUB)
		{
			if(got_sub_count)
				keep_doing = 0;
		}
#endif // end of DVB_SUBTITLES
		else
		{
#ifdef DVBT_USING_NORMAL_METHOD
			/* for DVB-T check */
			if (skydvb_thread_running == -1)
				return 1;
#endif // end of DVBT_USING_NORMAL_METHOD
#ifdef DVB_SUBTITLES
			if (got_video_count || got_audio_count || got_sub_count)
#else // else of DVB_SUBTITLES
			if (got_video_count || got_audio_count )
#endif // end of DVB_SUBTITLES
				keep_doing = 0;
		}

	
		if(count > retry_count)
		{
			if (priv->fifo[0].pfd > 0)
				pkt_audio_read_count = check_packets(priv->fifo[0].pfd);
			if (priv->fifo[1].pfd > 0)
				pkt_video_read_count = check_packets(priv->fifo[1].pfd);
#ifdef DVB_SUBTITLES
			if (priv->fifo[2].pfd > 0)
				pkt_sub_read_count = check_packets(priv->fifo[2].pfd);
			/* If we wait 0.8 sec (10000 * 80 usec) no received data, we need exit */
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
			if (!quick_bd && filter_type)
				mplayer_debug("==== we finished %s read_size is [%d], we add video[%d] audio[%d] we want [%s] timeout [%d] ms count[%d] ka[%d] kv[%d] ks[%d]===\n", __func__, read_size, got_video_count, got_audio_count, filter_type & FILTER_VIDEO ? "video": filter_type & FILTER_AUDIO ? "audio" : "subtitle" , retry_count*10, count, pkt_audio_read_count, pkt_video_read_count, pkt_sub_read_count);
#else
			if (filter_type)
				mplayer_debug("==== we finished %s read_size is [%d], we add video[%d] audio[%d] we want [%s] timeout [%d] ms count[%d] ka[%d] kv[%d] ks[%d]===\n", __func__, read_size, got_video_count, got_audio_count, filter_type & FILTER_VIDEO ? "video": filter_type & FILTER_AUDIO ? "audio" : "subtitle" , retry_count*10, count, pkt_audio_read_count, pkt_video_read_count, pkt_sub_read_count);
#endif
#endif
			break;
		}
		
	}while(keep_doing);
	debug_printf("==== we finished %s, we add video[%d] audio[%d]===\n", __func__, got_video_count, got_audio_count);

	return read_size;
}

/* check kernel packet */
static inline int check_packets(int fd)
{
	int number = 0;
	struct dmx_buffer_meter	st_buffer_params;

	if (fd > 0)
	{
		if (ioctl(fd, DMX_METER_BUFFER, &st_buffer_params) != -1)
		{
			number = st_buffer_params.queue_pkt_cnt;
			/* number == 0 and st_buffer_params.flags is true when we meet Buffer overflow */
			if (!number && st_buffer_params.flags)
				number = 1;
		}
	}
	return number;
}

#ifdef DVB_SUBTITLES
/* Parse subtile data and append to subtitle queue */
static inline int add_sub_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, int count, demuxer_t *demuxer, char filter_type)
{
	int ret = 0;
	int offset = 0;

	demux_packet_t *dp = NULL;
	debug_printf("=========== in [%s], count is [%d] aq[%d] vq[%d]=======\n", __func__, count, demuxer->audio->packs, demuxer->video->packs);
	while(count > 0)
	{
		ret = read(fd, buf, len);
		if (ret > MIN_PES_SUB_LENGTH)
		{	
			offset = buf[8] + 8 + 1;
			if (filter_type == FILTER_DVB)
			{
				if (buf[3] != 0xbd)
				{
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					mplayer_debug("=== data failed [%x %x %x %x ] drop this sub packet==\n", buf[0], buf[1],  buf[2], buf[3]);
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					count--;
					continue;
				}
				else
				{
					if (ret < offset)
					{
						mplayer_debug("@@ Carlos in [%s][%d] ret [%d] offset[%d] ### \n", __func__, __LINE__, ret, offset);
						count--;
						continue;
					}
				}
			}
			dp = new_demux_packet(ret - offset);
			dp->flags = 0;
			/* Calculate pts */
			CALCULATE_PTS(dp->pts, buf);
			if (ret > len)
			{
				debug_printf("##### len is too large , ret [%d] len[%d], next is [%d] in [%s][%d]####\n", ret, len, ret-len, __func__, __LINE__);
				memcpy(dp->buffer, buf + offset, len - offset);
				ret = read(fd, dp->buffer + (len - offset), ret - len);
			}
			else
				memcpy(dp->buffer, buf + offset, ret - offset); 

			//dp->pts = es->pts;
			demuxer->filepos = stream_tell(demuxer->stream); // record the file position, for seek 

#ifdef BD_CLIP_BASE_PTS
			if (bd_now_base_pts > 0)
				adjust_bd_pts(dp, demuxer);
#endif
			ds_add_packet( ds, dp );
			count--;
		}
		else if (ret > 0)
		{
			mplayer_debug("@@@@ Carlos in [%s][%d], ret is [%d] pes_legnth[%d] pes_header_length [%d]@@@@\n", __func__, __LINE__, ret, (buf[4] << 8) |buf[5] , buf[8]);
			count--;
		}
		else
		{
			debug_printf("== In [%s][%d]read data failed , failed reason is [%s][%d]==\n", __func__, __LINE__, strerror(errno), ret);
			break;
		}
	}
	return ret;
}
#endif // end of DVB_SUBTITLES

/* Parse audio data and append to audio queue */
static inline int add_audio_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, sh_audio_t *sh, int count, demuxer_t *demuxer, char filter_type)
{
	int ret = 0;
	int offset = 0;
#ifdef BUGGY_FIXED_DTS_PROBLEM
	static int last_audio_len = 0;
#endif // end of BUGGY_FIXED_DTS_PROBLEM
	static double last_audio_pts = 0.0;

	demux_packet_t *dp = NULL;
	debug_printf("=========== in [%s], count is [%d] aq[%d] vq[%d]=======\n", __func__, count, demuxer->audio->packs, demuxer->video->packs);
	while(count > 0)
	{
		ret = read(fd, buf, len);
		if (ret > MIN_PES_LENGTH)
		{	
			offset = buf[8] + 8 + 1;
			if (filter_type == FILTER_DVB)
			{
				//if ((buf[3]==0xbd) ||(buf[3] == 0xfd) || ((buf[3] >= 0xc0) &&(buf[3] <= 0xdf)))
				if ( ((buf[3] < 0xc0) && (buf[3] != 0xbd)) || ((buf[3] > 0xdf) && (buf[3] != 0xfd)))
				{
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					mplayer_debug("=== data failed [%x %x %x %x ] drop this audio packet==\n", buf[0], buf[1],  buf[2], buf[3]);
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					count--;
					continue;
				}
				else
				{
					if (ret < offset)
					{
						mplayer_debug("@@ Carlos in [%s][%d] ret [%d] offset[%d] ### \n", __func__, __LINE__, ret, offset);
						count--;
						continue;
					}
				}
			}
			dp = new_demux_packet(ret - offset);
			dp->flags = 0;
			/* Calculate pts */
			CALCULATE_PTS(dp->pts, buf);
#ifdef BUGGY_FIXED_DTS_PROBLEM
			if (Check_Audio_PTS_Zero)
			{
				if ((dp->pts == MP_NOPTS_VALUE) && ((ret - offset) == last_audio_len))
				{
					Got_Special_DTS_CASE = 1;
					Check_Audio_PTS_Zero = 0;
					mplayer_debug("#### In [%s][%d]: Special DTS packet, manually split one packet to two packets####\n", __func__, __LINE__);
				}
				last_audio_len = ret - offset;
			}
			if (dp->pts != MP_NOPTS_VALUE)
				last_audio_pts = dp->pts;
			if (Got_Special_DTS_CASE)
			{
				/* TODO, we mark sure audio packet length is not over 1MB */
				int really_len = (ret - offset) / 2;
				/* split one packet to two packets */
				demux_packet_t *dup_dp = NULL;
				dup_dp = new_demux_packet(really_len);
				dup_dp->flags = 0;
				dup_dp->len = really_len;
				dp->len = really_len;
#ifdef BD_CLIP_BASE_PTS
				if (bd_now_base_pts > 0)
					adjust_bd_pts(dp, demuxer);
#endif /* end of BD_CLIP_BASE_PTS */
				if (dp->pts == MP_NOPTS_VALUE)
				{
					dup_dp->pts = last_audio_pts ;
					dp->pts = last_audio_pts ;
				}
				else
					dup_dp->pts = dp->pts;
				memcpy(dp->buffer, buf + offset, really_len);
				memcpy(dup_dp->buffer, buf + offset + really_len, really_len);
				demuxer->filepos = stream_tell(demuxer->stream); // record the file position, for seek 

				ds_add_packet( ds, dp );
				ds_add_packet( ds, dup_dp );
				count--;
				debug_printf ("#### Special case in [%s][%d] , DTS split case dup_dp->len[%d], pts[%f] pts2[%f]####\n", __func__, __LINE__, dup_dp->len, dp->pts, dup_dp->pts);
			}
			else
#endif // end of BUGGY_FIXED_DTS_PROBLEM
			{
#ifdef BD_CLIP_BASE_PTS
				if (bd_now_base_pts > 0)
					adjust_bd_pts(dp, demuxer);
#endif /* end of BD_CLIP_BASE_PTS */
				if (dp->pts != MP_NOPTS_VALUE)
					last_audio_pts = dp->pts;
#if 0					
				else
					dp->pts = last_audio_pts;
#endif					
				if (ret > len)
				{
					debug_printf("##### len is too large , ret [%d] len[%d], next is [%d] in [%s][%d]####\n", ret, len, ret-len, __func__, __LINE__);
					memcpy(dp->buffer, buf + offset, len - offset);
					ret = read(fd, dp->buffer + (len - offset), ret - len);
				}
				else
					memcpy(dp->buffer, buf + offset, ret - offset);
				demuxer->filepos = stream_tell(demuxer->stream); // record the file position, for seek 

				ds_add_packet( ds, dp );
				count--;
			}
		}
		else if (ret > 0)
		{
			mplayer_debug("@@@@ Carlos in [%s][%d], ret is [%d] pes_legnth[%d] pes_header_length [%d]@@@@\n", __func__, __LINE__, ret, (buf[4] << 8) |buf[5] , buf[8]);
			count--;
		}
		else
		{
			debug_printf("== In [%s][%d]read data failed , failed reason is [%s][%d]==\n", __func__, __LINE__, strerror(errno), ret);
			break;
		}
	}
	return ret;
}

/* Parse video data and append to video queue */
static inline int add_video_packets(int fd, unsigned char *buf, int len, demux_stream_t *ds, sh_video_t *sh, int count, demuxer_t *demuxer, TS_stream_info *si, char filter_type, int fe_fd)
{
	int ret = 0;
	int offset = 0;

	static int quick_bd_startup_cnt=0;
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
	extern unsigned char *VideoBuffer[3];
	unsigned int *lptr;// = dp->buffer;
#endif
#ifdef DYNAMIC_CALCULATED_PTS
	int req_count = count;
	int dup1_count = 1;
	int dup2_count = 1;
	double dup1_pts = 0.0;
	double dup2_pts = 0.0;
	double med_pts = 0.0;
	char special_case = 0;
	char do_dynamic_pts = 0;
#endif /* end of DYNAMIC_CALCULATED_PTS */

	demux_packet_t *dp = NULL;
	debug_printf("=========== in [%s], count is [%d] aq[%d] vq[%d]=======\n", __func__, count, demuxer->audio->packs, demuxer->video->packs);
#ifdef DYNAMIC_CALCULATED_PTS
	if (check_fps_count < MAX_CHECK_DYNAMIC_COUNT && need_do_dynamic_pts == 0)
	{
		check_fps_count++;
		if (need_do_dynamic_pts)
			check_fps_count = MAX_CHECK_DYNAMIC_COUNT;
	}
	if (need_do_dynamic_pts)
	{
		do_dynamic_pts = 1;
	}
#endif /* end of DYNAMIC_CALCULATED_PTS */
	//printf("==== In [%s][%d] sh->fps is [%f] ===\n", __func__, __LINE__, sh->fps);
	//while(count > 0 && demuxer->video->packs < MAX_VIDEO_QUEUE)
	while(count > 0)
	{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
		if (found_keyframe_seek == 0)
		{
			quick_bd_startup_cnt = 0;
			use_PP_buf = 0;
		}
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */		

		quick_bd_startup_cnt++;
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
		if ((quick_bd && quick_bd_startup_cnt> USE_PP_BUF3_CNTS))
		{
#ifdef BITSTREAM_BUFFER_CONTROL
			unsigned char *bs_buf_start = VideoBuffer[0];
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))
			if (demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
				bs_buf_start = VideoBuffer[1];
#endif
			if(chk_tailroom(demuxer,VIRTUAL_TO_UCM(bs_buf_start + VideoBufferIdx), 1024*1024))
			{
				//mplayer_debug("Frame too large, reset to head\n");
				VideoBufferIdx = 0;
			}
			videobuffer = bs_buf_start + VideoBufferIdx;
			wait_buffer(VIRTUAL_TO_UCM(videobuffer), 1024*1024);
#endif

			ret = read(fd, (void *)(0xc0000000 | VIRTUAL_TO_UCM(videobuffer)), len);
			if (quick_bd_startup_cnt <= MAX_QUICK_BD_STARTUP_CNTS)
			{
				memcpy(buf, videobuffer, ret);
			}
			else
			{
				memcpy(buf, videobuffer, MAX_QUICKBD_FIRST_HDR_LEN);
			}
		}
		else
#endif
		{
			ret = read(fd, buf, len);
		}

		if (ret > MIN_PES_LENGTH)
		{	
			offset = buf[8] + 8 + 1;
			if (filter_type & FILTER_DVB)
			{
				if ((buf[3] != 0xfa) && (buf[3] < 0xe0 || buf[3] > 0xef) ) /* 0xfa is mpeg4 */
//				if (!((buf[3] >= 0xe0) && (buf[3] <= 0xef))))
				{
					__u32 ucblocks = 0;
					if (fe_fd)
					{
						if (ioctl(fe_fd, FE_READ_UNCORRECTED_BLOCKS, &ucblocks) < 0)
							mplayer_debug("FE_READ_UNCORRECTED_BLOCKS ioctl failed, fd_fd [%d], errno[%d] [%s] filter_type[%x]\n", fe_fd, errno, strerror(errno), filter_type);
					}
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					mplayer_debug("=== data failed [%x %x %x %x ] drop this video packet ucblocks[%x]==\n", buf[0], buf[1],  buf[2], buf[3], ucblocks);
					mplayer_debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					count--;
					continue;
				}
				else
				{
					if (ret < offset)
					{
						mplayer_debug("@@ Carlos in [%s][%d] ret [%d] offset[%d]### \n", __func__, __LINE__, ret, offset);
						count--;
						continue;
					}
				}
			}

#ifdef SUPPORT_QT_BD_ISO_ENHANCE
			if ((quick_bd && quick_bd_startup_cnt > USE_PP_BUF3_CNTS))
			{
				if (quick_bd_startup_cnt <= MAX_QUICK_BD_STARTUP_CNTS)
				{
					if ((ret-offset) < MAX_QUICKBD_FIRST_HDR_LEN+4*3)
					{
						dp = new_demux_packet(MAX_QUICKBD_FIRST_HDR_LEN+4*3);
					}
					else
					{
						dp = new_demux_packet(ret - offset);
					}
					lptr = dp->buffer;
					lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] = 0xffffffff;
					use_PP_buf = 3;
				}
				else
				{
					dp = new_demux_packet(MAX_QUICKBD_FIRST_HDR_LEN+4*3);
					lptr = dp->buffer;
        			        lptr[(MAX_QUICKBD_FIRST_HDR_LEN/4)+2] = (unsigned int)(videobuffer) + offset;// - SallocVirtualBase + (unsigned int)SallocPhysicalBase;
		                	lptr[(MAX_QUICKBD_FIRST_HDR_LEN/4)+1] = ret - offset;
					lptr[MAX_QUICKBD_FIRST_HDR_LEN/4] = 0xff008860;
					use_PP_buf = 3;
				}

//				VideoBufferIdx = ROUND_UP(VideoBufferIdx + ret, 8192);
				VideoBufferIdx = ROUND_UP(VideoBufferIdx + ret, 1024*32);
			}
			else
#endif
			{
				dp = new_demux_packet(ret - offset);
			}
			dp->flags = 0;
			/* Calculate pts */
			CALCULATE_PTS(dp->pts, buf);
			//printf("$$$$ Carlos in [%s][%d], len [%d] pts is [%f] buf is [%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x] bd_now_base_pts[%f] first_video_pts[%f] last_video_pts[%f]$$$\n", __func__, __LINE__, ret - offset, dp->pts, buf[0], buf[1], buf[2], buf[3],  buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], bd_now_base_pts, first_video_pts, last_video_pts);
			/* Record last video pts and fill the last_video_pts to current pes packet if the current pes packet does not have pts */
#ifdef BD_CLIP_BASE_PTS
			if (bd_now_base_pts > 0)
				adjust_bd_pts(dp, demuxer);
#endif /* end of BD_CLIP_BASE_PTS */
#ifdef DYNAMIC_CALCULATED_PTS
			if (do_dynamic_pts || check_fps_count < MAX_CHECK_DYNAMIC_COUNT)
			{
				/* find the duplicated pts, read more data */
				if (last_video_pts == dp->pts || (dp->pts == MP_NOPTS_VALUE && last_video_pts != 0.0))
				{
					/* Mean the pts is the same with the last time */
					if (count == 1)
						count++;
					if (count == req_count)
						special_case = 1;
					if (dup1_pts == 0.0)
					{
						dup1_pts = last_video_pts;
						dup1_count++;
					}
					else if ((dup1_pts != dp->pts) && (dp->pts != MP_NOPTS_VALUE))
					{
						dup2_pts = dp->pts;
						dup2_count++;
					}
					else
					{
						dup1_count++;
					}
					dynamic_debug("=============find duplicate pts [%f] count [%d] dp->pts [%f] req_count [%d]dup1_count[%d] dup2[%d] dup1[%f] dup2[%f]========\n", last_video_pts, count, dp->pts, req_count, dup1_count, dup2_count, dup1_pts, dup2_pts);
				}
				else
				{
					if (med_pts == 0.0)
						med_pts = dp->pts;
				}
			}
#endif /* end of DYNAMIC_CALCULATED_PTS */			
			if (dp->pts == MP_NOPTS_VALUE)
				dp->pts = last_video_pts;
			else
				last_video_pts = dp->pts;

#ifdef SUPPORT_QT_BD_ISO_ENHANCE
			if ((quick_bd && quick_bd_startup_cnt > USE_PP_BUF3_CNTS))
			{
				if (quick_bd_startup_cnt <= MAX_QUICK_BD_STARTUP_CNTS)
				{
					memcpy(dp->buffer, videobuffer + offset, ret - offset); 
				}
				else
				{
					memcpy(dp->buffer, buf + offset, MAX_QUICKBD_FIRST_HDR_LEN-offset); 
				}
			}
			else
#endif
			{
				if (ret > len)
				{
					debug_printf("##### len is too large , ret [%d] len[%d], next is [%d] in [%s][%d]####\n", ret, len, ret-len, __func__, __LINE__);
					memcpy(dp->buffer, buf + offset, len - offset);
					ret = read(fd, dp->buffer + (len - offset), ret - len);
				}
				else
					memcpy(dp->buffer, buf + offset, ret - offset); 
			}
			demuxer->filepos = stream_tell(demuxer->stream); // record the file position, for seek

			//Barry 2011-04-24
			if (ds->last)
				pre_packet_diff_pts = dp->pts - ds->last->pts;

			ds_add_packet( ds, dp );
			if (si)
			{
				float diff = dp->pts - si->last_pts;
				float dur;

				if(abs(diff) > 1) //1 second, there's a discontinuity
				{
					si->duration += si->last_pts - si->first_pts;
					si->first_pts = si->last_pts = dp->pts;
				}
				else
				{
					si->last_pts = dp->pts;
				}
				si->size += ret - offset;
				dur = si->duration + (si->last_pts - si->first_pts);

				if(dur > 0 )
				{
					ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
					if(dur > 1)	//otherwise it may be unreliable
					{
						priv->vbitrate = (uint32_t) ((float) si->size / dur);
						//mplayer_debug("@@@@ set vbitrate [%d], size [%f], dur [%f], durxx [%f] last_pts[%f], first_pts[%f]video@@\n", priv->vbitrate, si->size, dur, si->duration, si->last_pts, si->first_pts);
					}
				}
			}
			count--;
		}
		else if (ret > 0)
		{
			__u32 ucblocks = 0;
			if (fe_fd && (filter_type & FILTER_DVB))
			{
				if (ioctl(fe_fd, FE_READ_UNCORRECTED_BLOCKS, &ucblocks) < 0)
					mplayer_debug("FE_READ_UNCORRECTED_BLOCKS ioctl failed, fd_fd [%d], errno[%d] [%s]\n", fe_fd, errno, strerror(errno));
			}
			mplayer_debug("@@@@ In [%s][%d], ret is [%d] pes_header_length [%d] ucblocks[%x]@@@@\n", __func__, __LINE__, ret, ret > 9 ? buf[8] : 0, ucblocks);
			count--;
		}
		else
		{
			debug_printf("== In [%s][%d]read data failed , failed reason is [%s][%d]==\n", __func__, __LINE__, strerror(errno), ret);
			break;
		}
	}
		
#ifdef DYNAMIC_CALCULATED_PTS	
	if (do_dynamic_pts || check_fps_count < MAX_CHECK_DYNAMIC_COUNT)
	// 2 dup
	{
		demux_packet_t *p = ds->first;
		double check1_per_packet_time = 0.0;
		double check2_per_packet_time = 0.0;
		int dup1_adjust = 0;
		int dup2_adjust = 0;
		//check_per_packet_time = (last_video_pts - dup_pts) / (double)dup_count;
		if (dup2_pts != 0.0)
		{
			if (dup2_pts < last_video_pts)
			{
				check1_per_packet_time = (dup2_pts - dup1_pts) / (double)dup1_count;
				check2_per_packet_time = (last_video_pts - dup2_pts) / (double)dup2_count;
			}
			else
			{
				/* not yet */
			}
		}
		else
		{
			/* one dup */
			if (dup1_pts != 0.0)
			{
				if (med_pts != 0.0 && med_pts > dup1_pts)
					check1_per_packet_time = (med_pts - dup1_pts) / (double)dup1_count;
				else
					check1_per_packet_time = (last_video_pts - dup1_pts) / (double)dup1_count;
			}
		}

		if (need_do_dynamic_pts == 0 && check1_per_packet_time != 0.0 && check1_per_packet_time > 0.0667) /* 0.0667 mean fps is 15 */
		{
			need_do_dynamic_pts = 1;
			printf("$$$ In [%s][%d] enable dynamic_pts check1_per_packet_time[%f]$$$\n", __func__, __LINE__, check1_per_packet_time);
		}
		//printf("#### In [%s][%d] dup1[%f - %d] dup2[%f -%d] last_video_pts[%f] pt_time 1[%f] pt_time2[%f]####\n", __func__, __LINE__, dup1_pts, dup1_count, dup2_pts, dup2_count, last_video_pts, check1_per_packet_time, check2_per_packet_time);
		while(p)
		{
			dynamic_debug("$$$$ Show really data : In [%s][%d] p [%p] pts[%f] len[%d] special_case[%d]$$$$", __func__, __LINE__, p, p->pts, p->len, special_case);
			if (dup1_pts != 0.0 && dup1_pts == p->pts)
			{
				if (dup2_pts != 0.0 || special_case)
					dup1_adjust++;
				p->pts += check1_per_packet_time * (double)(dup1_adjust);
				if (special_case)
				{
					//dup1_adjust--;
					special_case = 0;
				}
				dynamic_debug("-- new pts [%f]####\n", p->pts);
				dup1_adjust++;
			}
			else if (dup2_pts != 0.0 && dup2_pts == p->pts)
			{
				p->pts += check2_per_packet_time * (double)(dup2_adjust);
				dynamic_debug("-- new pts [%f]####\n", p->pts);
				dup2_adjust++;
			}
			else
			{
				dynamic_debug("\n");
			}
			p = p->next;
		}
	}
#if 0	
	if (0)
	{
		demux_packet_t *p = ds->first;
		while(p)
		{
			dynamic_debug("**************Modified data : In [%s][%d] p [%p] pts[%f] len[%d] ***************\n", __func__, __LINE__, p, p->pts, p->len);
			p = p->next;
		}
	}
#endif	
#endif /* end of DYNAMIC_CALCULATED_PTS */	
	return ret;
}

/* Write data to dvr function.*/
/* This function is handle Signo IO, system will callback */
static void hw_io_write(int signo, siginfo_t *info, void *context) 
{
	int write_count = 0, len;
	if(signo) 
	{
		if (wait_late)
			wait_late = 0;
		else
		{
			write_toggle_len[write_toggle] = 0;                             // unlock for read thread
			write_toggle = (write_toggle + 1) % BUFFER_COUNT;
#ifdef HWTS_WAKEUP
			if (pthread_mutex_trylock(&fillmtx) != 0) /* Avoid mutex dead lock */
			{
				//mplayer_debug("##### pthread_mutex_trylock failed status[%d]TS_SEEK_START[%d]######, \n", ts_seek_status, TS_SEEK_START);
			}
			else
			{
				pthread_mutex_unlock(&fillmtx);
				pwake_up(&fillcond, &fillmtx);
			}
#endif // end of HWTS_WAKEUP			
		}
	}
	len = write_toggle_len[write_toggle];

#ifdef SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
	if (pthread_mutex_trylock(&ts_switch) != 0)
	{
		self_write = 1;
		mplayer_debug("#### In [%s][%d], we detect run switch audio  set self_write to 1###\n", __func__, __LINE__);
		return;
	}
	else
	{
		pthread_mutex_unlock(&ts_switch);
	}
#endif // end of SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE
	if((!len) || (ts_seek_status == TS_SEEK_START))
	{
		self_write =1;
		return;
	}

	debug_printf("Enter [%s] len=0x%x\n", __func__, len);	

	while(len > 0)
	{
		if (ts_seek_status == TS_SEEK_START)
			return ;
		write_count = write(dvr_fd, buf_bk+TS_BUF_LENGTH * write_toggle, write_toggle_len[write_toggle]);
		if (write_count != len && write_count > 0)
			mplayer_debug("Write data to dvr device node failed, something wrong. Read_size is [%d], Write_size is [%d]\n", len, write_count);
		if (write_count <= 0)
		{
			//mplayer_debug("Write data to dvr device node failed, something wrong. Read_size is [%d], Write_size is [%d] [%d]-[%s]\n", len, write_count, errno, strerror(errno));
			write_count = 0;
			//self_write = 1;
			wait_late = 1;
			break;
		}
		else
		{
			//mplayer_debug("Write data to Read_size is [%d], Write_size is [%d] \n", len, write_count);
			len -= write_count;
		}
	}
	debug_printf("Leave [%s]\n", __func__);		
}

#ifdef DVB_SUBTITLES
/* For H264 FF 1~4 Speed, sync sub : restart sub filter */
void restart_sub_buffer(demux_stream_t *d_sub, void *priv, char operation_flag)
{
	sh_sub_t *sh_sub=d_sub->sh;
	ts_priv_t *ts_priv = (ts_priv_t *)priv;
	int fd = ts_priv->fifo[2].pfd;

	if (sh_sub)
	{
		debug_printf(" #[%s]: do restart sub filter #\n", __func__);
		if ((operation_flag & NEED_PAUSE_FILTER) && ioctl(fd, DMX_PAUSE_FILTER) < 0)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", fd, errno);
		}
		if ((operation_flag & NEED_RESUME_FILTER) && ioctl(fd, DMX_RESUME_FILTER, 0) < 0)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nERROR IN SETTING NEED_RESUME_FILTER for fd %d: ERRNO: %d\r\n", fd, errno);
		}
		debug_printf(" #[%s]: do restart sub filter #\n", __func__);
	}
	else
		debug_printf("=== No audio stream, don't need restart filter ===\n");
}
#endif // end of DVB_SUBTITLES

/* For H264 FF 1~4 Speed, sync audio : restart audio filter */
void restart_audio_buffer(demux_stream_t *d_audio, void *priv, char operation_flag)
{
	sh_audio_t *sh_audio=d_audio->sh;
	ts_priv_t *ts_priv = (ts_priv_t *)priv;
	int fd = ts_priv->fifo[0].pfd;

	if (sh_audio)
	{
		debug_printf(" #[%s]: do restart audio filter #\n", __func__);
		if ((operation_flag & NEED_PAUSE_FILTER) && ioctl(fd, DMX_PAUSE_FILTER) < 0)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", fd, errno);
		}
		if ((operation_flag & NEED_RESUME_FILTER) && ioctl(fd, DMX_RESUME_FILTER, 0) < 0)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "\r\nERROR IN SETTING NEED_RESUME_FILTER for fd %d: ERRNO: %d\r\n", fd, errno);
		}
		debug_printf(" #[%s]: do restart audio filter #\n", __func__);
	}
	else
		debug_printf("=== No audio stream, don't need restart filter ===\n");
}

char check_hwts_thread(void)
{
	return start_write_thread;
}
/* Change axi_speed function */
void change_axi_speed(demux_stream_t *d_video, void *priv, char operation_flag)
{
	sh_video_t *sh_video=d_video->sh;
	ts_priv_t *ts_priv = (ts_priv_t *)priv;
	int fd = ts_priv->fifo[1].pfd; //set video fd
	char speed = 0x1a;

	if (sh_video)
	{
		if (operation_flag & IN_FF_FR)
		{
			speed = 0x1e;
		}
		else if (ts_version == TS_1)
		{
			if (operation_flag & SLOW_DOWN_SPEED)
				speed = 0x16;
			else
			{
#if 1	//Barry 2011-10-12
				if (no_osd)
					speed = 0x10;
				else
#endif
					speed = 0x1A;
			}
		} /* The below condition are m2ts file */
		else if ((sh_video->format == VIDEO_H264) || (sh_video->format == VIDEO_AVC))
		{
			if (operation_flag & SLOW_DOWN_SPEED)
				speed = 0x05;
			else
				speed = 0x07;
		}
		else if (sh_video->format == VIDEO_VC1)
		{
			if (operation_flag & SLOW_DOWN_SPEED)
			{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
				if (quick_bd)
					speed = 0x05;
				else
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */				
					speed = 0x16;
			}
			else
			{
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
				if (quick_bd)
					speed = 0x10;
				else
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */				
					speed = 0x1A;
			}
		}
		else if (sh_video->format == VIDEO_MPEG2)
		{
			if (operation_flag & SLOW_DOWN_SPEED)
				speed = 0x16;
			else
				speed = 0x1A;
		}
		else
		{
			/* This video is mpeg4 */
			if (operation_flag & SLOW_DOWN_SPEED)
				speed = 0x16;
			else
				speed = 0x1A;
		}
	
		//fprintf(stderr, "@@@@#[%s][%d]: do operation_flag[%x] change axi_speed to [%x] DMX_SET_AXI_SPEED[%x] fd[%d] sh_video->format[%x]#\n", __func__, __LINE__, operation_flag, speed, DMX_SET_AXI_SPEED, fd, sh_video->format);
	}
	else
	{
		 speed = 0x1a;
		 debug_printf("=== No video stream, set axi speed to 0x1a ===\n");
	}
	/* We always need set the axi speed */
	if ( ioctl(fd, DMX_SET_AXI_SPEED, speed) < 0)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "\r\nERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d\r\n", fd, errno);
	}
	debug_printf(" #[%s]: do change axi_speed to [%x]#\n", __func__, speed);
}
#endif // end of HW_TS_DEMUX

#ifdef CHECK_AUDIO_IS_VALID
#ifndef CHECK_EXIST_FLAGS
static inline int check_audio_is_exist(int pid, int check_count, demuxer_t *demuxer)
{
	int ii = 0;
	int ret_len = 0;
	int ret = 0 ;
	ES_stream_t es;
	unsigned char tmp[TS_FEC_PACKET_SIZE];
	if (!check_count)
	{
		mplayer_debug("Set default check count to %d\n", MAX_CHECK_AUDIO_EXIST);
		check_count = MAX_CHECK_AUDIO_EXIST;
	}
	if (demuxer->stream->eof)
		demuxer->stream->eof = 0;
	stream_seek(demuxer->stream, 0);
	for(ii = 0 ; ii < check_count ; ii++)
	{
		ret_len = ts_parse(demuxer, &es, tmp, 1);
		if (ret_len <= 0 )
		{
			ret = 0;
			break;
		}
		if (es.pid == pid)
		{
			ret = 1;
			break;
		}
	}
	parse_debug_printf("=== finished %s ret_len is [%d]ret [%d] , ii [%d], check_count[%d]====\n", __func__, ret_len, ret, ii, check_count);
	return ret;
}
#endif // end of ifndef CHECK_EXIST_FLAGS

static int inline find_out_stream(ts_priv_t *priv, tsdemux_init_t *param, demuxer_t *demuxer)
{
	int ii = 0;
	int jj = 0;
	char got_video = 0;
	char got_audio = 0;
 	char got_sub = 0;
	int record[12]={-1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0}; 
	char is_first_time_video = 1;
	char is_first_time_audio = 1;
	char is_first_time_sub = 1;
	parse_debug_printf("###In [%s][%d] ###\n", __func__, __LINE__);

	if (priv->pmt_cnt)
	{
		for (ii = 0; ii < priv->pmt_cnt; ii++)
		{
			for(jj = 0 ;  jj < priv->pmt[ii].es_cnt; jj++)
			{
				if((!got_video) && IS_VIDEO(priv->pmt[ii].es[jj].type))
				{
#ifdef CHECK_EXIST_FLAGS
					//if (priv->pmt[ii].es[jj].exist_flag)
					if ( (priv->pmt[ii].es[jj].exist_flag & THE_PID_IS_EXIST) && !(priv->pmt[ii].es[jj].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA)))
#else // else of CHECK_EXIST_FLAGS
					if (check_video_status(priv->pmt[ii].es[jj].pid, MAX_CHECK_AUDIO_EXIST*(priv->pmt_cnt)*(priv->pmt[ii].es_cnt), demuxer))
#endif // end of CHECK_EXIST_FLAGS						
					{
						if (is_first_time_video)
						{
							record[FIRST_VIDEO_PMT_NUMBER] = ii;
							record[FIRST_VIDEO_PMT_ES_NUMBER] = jj;
							is_first_time_video = 0;
						}
						record[NOW_VIDEO_PMT_NUMBER] = ii; 
						record[NOW_VIDEO_PMT_ES_NUMBER] = jj;
						got_video = 1;
					}
					else
					{
						parse_debug_printf("###In [%s][%d] The video content is not exist pid [%d]###\n", __func__, __LINE__, priv->pmt[ii].es[jj].pid);
						priv->pmt[ii].es[jj].type = NOT_EXIST ;
					}
				}
				if((!got_audio) && IS_AUDIO(priv->pmt[ii].es[jj].type) && priv->pmt[ii].es[jj].type != AUDIO_TRUEHD)
				{
				    if (priv->pmt[ii].es[jj].type == AUDIO_BPCM && priv->pmt[ii].es[jj].bpcm_channels != 0)
				    {
				    }
					else
					{
#ifdef CHECK_EXIST_FLAGS
						//if (priv->pmt[ii].es[jj].exist_flag)
						if ((priv->pmt[ii].es[jj].exist_flag & THE_PID_IS_EXIST) && !(priv->pmt[ii].es[jj].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA)))
#else // else of CHECK_EXIST_FLAGS
						if (check_audio_is_exist(priv->pmt[ii].es[jj].pid, MAX_CHECK_AUDIO_EXIST*(priv->pmt_cnt)*(priv->pmt[ii].es_cnt), demuxer))
#endif // end of CHECK_EXIST_FLAGS							
						{
							if (is_first_time_audio)
							{
								record[FIRST_AUDIO_PMT_NUMBER] = ii;
								record[FIRST_AUDIO_PMT_ES_NUMBER] = jj;
								is_first_time_audio = 0;
							}
							record[NOW_AUDIO_PMT_NUMBER] = ii; 
							record[NOW_AUDIO_PMT_ES_NUMBER] = jj;
							got_audio= 1;
						}
						else
						{
							parse_debug_printf("###In [%s][%d] The audio content is not exist pid [%d]###\n", __func__, __LINE__, priv->pmt[ii].es[jj].pid);
							priv->pmt[ii].es[jj].type = NOT_EXIST ;
						}
					}
				}
#ifdef DVB_SUBTITLES						
				/* Support Subtitle, 2010-10-04 */
				if((!got_sub) && IS_SUB(priv->pmt[ii].es[jj].type))
				{
					if (is_first_time_sub)
					{
						record[FIRST_SUB_PMT_NUMBER] = ii;
						record[FIRST_SUB_PMT_ES_NUMBER] = jj;
						is_first_time_sub = 0;
					}
					record[NOW_SUB_PMT_NUMBER] = ii; 
					record[NOW_SUB_PMT_ES_NUMBER] = jj;
					got_sub= 1;
				}
#endif // end of DVB_SUBTITLES				
				/* We need keep search sub title */
				if (got_video && got_audio && got_sub)
					break;
			}
			/* TODO, We need check subtitle when we support display subtitle or teltext */
			if (got_video && got_audio)
				break;
			else if (got_video || got_audio)
			{
				got_video = 0;
				got_audio = 0;
				got_sub = 0;
				continue;
			}
			else
			{
				got_video = 0;
				got_audio = 0;
				got_sub = 0;
			}
		}
		/* assign apid and vpid to param */
		if (got_video && got_audio)
		{
			int got_other_audio = 0;
#ifdef SUPPORT_TS_SELECT_LOW_AUDIO
			extern int ts_easy_audio;
			if (ts_easy_audio)
			{
				int ii = 0; 
				if((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type == AUDIO_DTS) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt > 2))
				{		
					for(ii = 0 ; ii < priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt ; ii++)
					{
						parse_debug_printf("## Carlos in [%s][%d] count[%d] ii [%d] pid [%d] type [%d] ##\n", __func__, __LINE__, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt, ii, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type);
						if ( (ii == record[NOW_AUDIO_PMT_ES_NUMBER] ) || (ii == record[NOW_VIDEO_PMT_ES_NUMBER]))
							continue;
						else 
						{
							if (IS_AUDIO(priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type != AUDIO_DTS) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type > 0))
								/* Don't add NOT_SUPPORT and UNKNOWN */
							{
								got_other_audio = 1;
								break;
							}
						}
					}
				}

				if (got_other_audio)
				{
					param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid;
					param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   //Polun 2011-10-14 ++s fixed Taipei.101.FireWorks(80mbps.dtsHD-LPCM7.1-2.0).m2ts switch audio id 4352 fail
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].bpcm_channels;
                                   //Polun 2011-10-14 ++e
                            }
				else
				{
					param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].pid;
					param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type;
				}
			}
			else
#endif /* end of SUPPORT_TS_SELECT_LOW_AUDIO */			
			{
                            #if 0
                            param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].pid;
			       param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type;
                            #else
                            //Polun 2011-09-23 ++s for truehd audio not support switch next support audi track
				int ii = 0; 
				if((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type == AUDIO_TRUEHD) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt > 2))
				{		
					for(ii = 0 ; ii < priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt ; ii++)
					{
						parse_debug_printf("## Carlos in [%s][%d] count[%d] ii [%d] pid [%d] type [%d] ##\n", __func__, __LINE__, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt, ii, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type);
						if ( (ii == record[NOW_AUDIO_PMT_ES_NUMBER] ) || (ii == record[NOW_VIDEO_PMT_ES_NUMBER]))
							continue;
						else 
						{
							if (IS_AUDIO(priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type != AUDIO_TRUEHD) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type > 0))
								/* Don't add NOT_SUPPORT and UNKNOWN */
							{
                                                        if((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_BPCM)  && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].bpcm_channels != 0))
                                                            continue;
								got_other_audio = 1;
                                                        printf("!!!!!!!got_other_audio !!!!!!!!\n ");
								break;
							}
						}
					}
				}

				if (got_other_audio)
				{
					param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid;
					param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   //Polun 2011-10-14 ++s fixed Taipei.101.FireWorks(80mbps.dtsHD-LPCM7.1-2.0).m2ts switch audio id 4352 fail
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].bpcm_channels;
                                   //Polun 2011-10-14 ++e
                            }
				else
				{
				      param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].pid;
				      param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type;
                           }
			      //Polun 2011-09-23 ++e
			      #endif
			}
#if 1	//Barry 2011-07-04 select other audio track, if now audio track = BPCM and channel(bpcm_channels != 0) > 2
			if((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type == AUDIO_BPCM) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt > 2) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels != 0))
			{
				//if ((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels >> 4) != 0x3)
				//	printf("#####  BPCM   bpcm_channels = %d\n", priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels);
				for(ii = 0 ; ii < priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt ; ii++)
				{
					parse_debug_printf("## Carlos in [%s][%d] count[%d] ii [%d] pid [%d] type [%d] ##\n", __func__, __LINE__, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es_cnt, ii, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid, priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type);
					if ( (ii == record[NOW_AUDIO_PMT_ES_NUMBER] ) || (ii == record[NOW_VIDEO_PMT_ES_NUMBER]))
						continue;
					else 
					{
						//if (IS_AUDIO(priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type != AUDIO_DTS) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type > 0))
						/*
						 * remove type != AUDIO_DTS condition, because it's mistake. Remove this condition for fixing mantis 5876.
						 * HK_BD_001.ISO has BPCM 6 channels, it is not supported.
						 * Carlos modified on 2011-08-18.
						 */
						if (IS_AUDIO(priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type) && (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type > 0))
							/* Don't add NOT_SUPPORT and UNKNOWN */
						{
                                                 //Polun 2011-10-14 fixed Taipei.101.FireWorks(80mbps.dtsHD-LPCM7.1-2.0).m2ts switch audio id 4352 fail
                                                 #if 0
                                                 if (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_BPCM && priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels != 0)
                                                 #else
                                                 if ((priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_BPCM  && priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].bpcm_channels != 0) ||
                                                     #ifdef SUPPORT_TS_SELECT_LOW_AUDIO
                                                     (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_TRUEHD && !sky_hwac3) ||(priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_DTS && !sky_hwac3))
                                                     #else
                                                     (priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type == AUDIO_TRUEHD && !sky_hwac3))
                                                     #endif
                                                 #endif
                                                    continue;
							got_other_audio = 1;
							break;
						}
					}
				}
				if (got_other_audio)
				{
					param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].pid;
					param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   //Polun 2011-10-14 ++s fixed Taipei.101.FireWorks(80mbps.dtsHD-LPCM7.1-2.0).m2ts switch audio id 4352 fail
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].type;
                                   priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].bpcm_channels = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[ii].bpcm_channels;
                                   //Polun 2011-10-14 ++e
				}
				else
				{
					param->apid = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].pid;
					param->atype = priv->pmt[record[NOW_AUDIO_PMT_NUMBER]].es[record[NOW_AUDIO_PMT_ES_NUMBER]].type;
				}
			}
#endif

			param->vpid = priv->pmt[record[NOW_VIDEO_PMT_NUMBER]].es[record[NOW_VIDEO_PMT_ES_NUMBER]].pid;
			param->vtype = priv->pmt[record[NOW_VIDEO_PMT_NUMBER]].es[record[NOW_VIDEO_PMT_ES_NUMBER]].type;
			param->prog = priv->pmt[record[NOW_VIDEO_PMT_NUMBER]].progid;
			parse_debug_printf("@@@ We have all in [%s][%d], apid[%d], vpid [%d] ii[%d]jj [%d] @@@\n", __func__, __LINE__, param->apid, param->vpid, ii, jj);
#ifdef DVB_SUBTITLES						
			if (got_sub)
			{
				param->spid = priv->pmt[record[NOW_SUB_PMT_NUMBER]].es[record[NOW_SUB_PMT_ES_NUMBER]].pid;
				param->stype = priv->pmt[record[NOW_SUB_PMT_NUMBER]].es[record[NOW_SUB_PMT_ES_NUMBER]].type;
				parse_debug_printf("@@@ We have all in [%s][%d], apid[%d], vpid [%d], spid[%d] ii[%d]jj [%d] @@@\n", __func__, __LINE__, param->apid, param->vpid, param->spid, ii, jj);
			}
#endif // end of DVB_SUBTITLES
		}
		else if (got_video)
		{
			/* leave loop, no audio */
			param->vpid = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].pid;
			param->vtype = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].type;
			param->prog = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].progid;
			parse_debug_printf("@@@ We have video only in [%s][%d], apid[%d], vpid [%d] ii[%d]jj [%d] @@@\n", __func__, __LINE__, param->apid, param->vpid, ii, jj);
#ifdef DVB_SUBTITLES						
			if (record[FIRST_SUB_PMT_ES_NUMBER] > 0 && record[FIRST_SUB_PMT_ES_NUMBER] == record[FIRST_VIDEO_PMT_NUMBER])
			{
				param->spid = priv->pmt[record[FIRST_SUB_PMT_NUMBER]].es[record[FIRST_SUB_PMT_ES_NUMBER]].pid;
				param->stype = priv->pmt[record[FIRST_SUB_PMT_NUMBER]].es[record[FIRST_SUB_PMT_ES_NUMBER]].type;
				parse_debug_printf("@@@ We have usb in [%s][%d], spid[%d], vpid [%d] ii[%d]jj [%d] @@@\n", __func__, __LINE__, param->spid, param->vpid, ii, jj);
			}
#endif // end of DVB_SUBTITLES			
			param->apid = 0 ;
			param->atype = 0;
		}
		else if (got_audio)
		{
			/* leave loop, no video, we choose the first video program */
			if (record[FIRST_VIDEO_PMT_NUMBER]!= -1)
			{
				param->vpid = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].pid;
				param->vtype = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].type;
				param->prog = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].progid;

				param->apid = 0 ;
				param->atype = 0;
			}
			else
			{
				param->apid = priv->pmt[record[FIRST_AUDIO_PMT_NUMBER]].es[record[FIRST_AUDIO_PMT_ES_NUMBER]].pid;
				param->atype = priv->pmt[record[FIRST_AUDIO_PMT_NUMBER]].es[record[FIRST_AUDIO_PMT_ES_NUMBER]].type;
				param->prog = priv->pmt[record[FIRST_AUDIO_PMT_NUMBER]].progid;

				param->vpid = 0 ;
				param->vtype = 0;
			}
			parse_debug_printf("@@@ We have audio only in [%s][%d], apid[%d], vpid [%d] ii[%d]jj [%d]  firstvideo[%d]@@@\n", __func__, __LINE__, param->apid, param->vpid, ii, jj, record[FIRST_VIDEO_PMT_NUMBER]);
		}
		else
		{
			if (record[FIRST_VIDEO_PMT_NUMBER] > 0)
			{
				/* don't have video and audio, check first video */
				param->vpid = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].pid;
				param->vtype = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].es[record[FIRST_VIDEO_PMT_ES_NUMBER]].type;
				param->prog = priv->pmt[record[FIRST_VIDEO_PMT_NUMBER]].progid;
				parse_debug_printf("@@@ We don't anything in [%s][%d], apid[%d], vpid [%d] ii[%d]jj [%d]  firstvideo[%d]@@@\n", __func__, __LINE__, param->apid, param->vpid, ii, jj, record[FIRST_VIDEO_PMT_NUMBER]);
#ifdef DVB_SUBTITLES						
				if (record[FIRST_SUB_PMT_ES_NUMBER] > 0 && record[FIRST_SUB_PMT_ES_NUMBER] == record[FIRST_VIDEO_PMT_NUMBER])
				{
					param->spid = priv->pmt[record[FIRST_SUB_PMT_NUMBER]].es[record[FIRST_SUB_PMT_ES_NUMBER]].pid;
					param->stype = priv->pmt[record[FIRST_SUB_PMT_NUMBER]].es[record[FIRST_SUB_PMT_ES_NUMBER]].type;
					parse_debug_printf("@@@ We have usb in [%s][%d], spid[%d], vpid [%d] ii[%d]jj [%d] @@@\n", __func__, __LINE__, param->spid, param->vpid, ii, jj);
				}
#endif // end of DVB_SUBTITLES
				param->apid = 0 ;
				param->atype = 0;
			}
		}
	}
	else
	{
		/* no pmt_cnt */
		parse_debug_printf("@@@ No pmt_cnt in [%s][%d] @@@\n", __func__, __LINE__);
	}
	return 0;
}

/* chooice the audio / video / subtitle pid in the same program */
static int inline select_stream(ts_priv_t *priv, tsdemux_init_t *param, demuxer_t *demuxer, int type)
{
	int ii = 0;
	int jj = 0;
	int zz = 0;
	int check_pid = 0;
	int ret = 0;
	int found_prog= 0;

	check_pid = (type == TYPE_AUDIO) ? param->apid: (type == TYPE_VIDEO) ? param->vpid : param->spid;
	parse_debug_printf("###In [%s][%d]  check_pid [%d], type [%d]###\n", __func__, __LINE__, check_pid, type);

	if (priv->pmt_cnt)
	{
		for (ii = 0; ii < priv->pmt_cnt; ii++)
		{
			for(jj = 0 ;  jj < priv->pmt[ii].es_cnt; jj++)
			{
				if (priv->pmt[ii].es[jj].pid == check_pid)
				{
					param->prog = priv->pmt[ii].progid;
					found_prog = 1;
					if(type == TYPE_AUDIO)
						param->atype = priv->pmt[ii].es[jj].type;
#ifdef DVB_SUBTITLES						
					if(type == TYPE_SUB)
						param->stype = priv->pmt[ii].es[jj].type;
#endif // end of DVB_SUBTITLES						
					break;
				}
			}
			if (found_prog)
				break;
		}
		parse_debug_printf("## Carlos in [%s][%d] found the program check_pid[%d], type[%d], pro[%d] cnt[%d] ii [%d] jj [%d] found_prog[%d]##\n", __func__, __LINE__, check_pid, type, priv->pmt[ii].progid, priv->pmt_cnt, ii, jj, found_prog);
		if (found_prog)
		{
			if (type == TYPE_VIDEO)
			{
				param->atype = 0;
				param->apid = 0;
				param->stype = 0;
				param->spid = 0;
				/* We need choose the audio in this program */
				for(zz = 0 ; zz < priv->pmt[ii].es_cnt; zz++)
				{
					if((!param->atype) && IS_AUDIO(priv->pmt[ii].es[zz].type))
					{
#ifdef CHECK_EXIST_FLAGS
						if ((priv->pmt[ii].es[zz].exist_flag & THE_PID_IS_EXIST) && !(priv->pmt[ii].es[zz].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA))) 
#else // else of CHECK_EXIST_FLAGS
						if (check_audio_is_exist(priv->pmt[ii].es[zz].pid, MAX_CHECK_AUDIO_EXIST*(priv->pmt_cnt)*(priv->pmt[ii].es_cnt), demuxer))
#endif // end of CHECK_EXIST_FLAGS							
						{
							param->apid = priv->pmt[ii].es[zz].pid;
							param->atype= priv->pmt[ii].es[zz].type;
							ret = 1;
							parse_debug_printf("#### Set up the apid [%d] type [%d] in [%s][%d]####\n", param->apid, param->atype, __func__, __LINE__);
							//break;
						}
						else
						{
							parse_debug_printf("###In [%s][%d] The audio content is not exist pid [%d]###\n", __func__, __LINE__, priv->pmt[ii].es[zz].pid);
							priv->pmt[ii].es[zz].type = NOT_EXIST;
						}
					}
#ifdef DVB_SUBTITLES	
					/* Check Subtitle */
					if((!param->stype) && IS_SUB(priv->pmt[ii].es[zz].type))
					{
							param->spid = priv->pmt[ii].es[zz].pid;
							param->stype= priv->pmt[ii].es[zz].type;
							ret = 1;
					}
#endif // end of DVB_SUBTITLES					
				}
				if (!ret)
				{
					mplayer_debug("### In [%s][%d] Can't find the requested audio pid [%d] ###\n", __func__, __LINE__, param->vpid);
					param->vpid = 0;
					param->vtype = 0;
				}
			}
			else /* Check Audio or SUB */
			{
				if (param->vpid != DO_NOT_PLAY)
				{
					param->vtype = 0;
					param->vpid = 0;
#ifdef DVB_SUBTITLES	
					if (param->spid == DO_NOT_CARE)
					{
						param->stype = 0;
						param->spid = 0;
					}
#endif // end of DVB_SUBTITLES					
					if (param->apid == DO_NOT_CARE)
					{
						parse_debug_printf("#### Set up the apid and atype to 0 in [%s][%d]####\n", __func__, __LINE__);
						param->atype = 0;
						param->apid = 0;
					}
					/* We need choose the video in this program */
					for(zz = 0 ; zz < priv->pmt[ii].es_cnt; zz++)
					{
						if((!param->vtype) && IS_VIDEO(priv->pmt[ii].es[zz].type))
						{
#ifdef CHECK_EXIST_FLAGS
							if ((priv->pmt[ii].es[zz].exist_flag & THE_PID_IS_EXIST) && !(priv->pmt[ii].es[zz].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA)))
#else // else of CHECK_EXIST_FLAGS
							if (check_video_status(priv->pmt[ii].es[zz].pid, MAX_CHECK_AUDIO_EXIST*(priv->pmt_cnt)*(priv->pmt[ii].es_cnt), demuxer))
#endif // end of CHECK_EXIST_FLAGS 								
							{
								param->vpid = priv->pmt[ii].es[zz].pid;
								param->vtype= priv->pmt[ii].es[zz].type;
								parse_debug_printf("#### Set up the vpid [%d] type [%d] param->atype[%d]id[%d] in [%s][%d]####\n", param->vpid, param->vtype, param->atype, param->apid, __func__, __LINE__);
								ret = 1;
								//break;
							}
							else
							{
								parse_debug_printf("###In [%s][%d] The video content is not exist pid [%d]###\n", __func__, __LINE__, priv->pmt[ii].es[zz].pid);
								priv->pmt[ii].es[zz].type = NOT_EXIST;
							}
						}
						/* Check Audio */
						if((!param->atype) && IS_AUDIO(priv->pmt[ii].es[zz].type))
						{
#ifdef CHECK_EXIST_FLAGS
							if ((priv->pmt[ii].es[zz].exist_flag & THE_PID_IS_EXIST)  && !(priv->pmt[ii].es[zz].exist_flag & (THE_PID_IS_SCRAMBLE | THE_PID_IS_CA)))
#else // else of CHECK_EXIST_FLAGS
							if (check_audio_is_exist(priv->pmt[ii].es[zz].pid, MAX_CHECK_AUDIO_EXIST*(priv->pmt_cnt)*(priv->pmt[ii].es_cnt), demuxer))
#endif // end of CHECK_EXIST_FLAGS								
							{
								param->apid = priv->pmt[ii].es[zz].pid;
								param->atype= priv->pmt[ii].es[zz].type;
								ret = 1;
								parse_debug_printf("#### Set up the apid [%d] type [%d] in [%s][%d]####\n", param->apid, param->atype, __func__, __LINE__);
								//break;
							}
							else
							{
								parse_debug_printf("###In [%s][%d] The audio content is not exist pid [%d]###\n", __func__, __LINE__, priv->pmt[ii].es[zz].pid);
								priv->pmt[ii].es[zz].type = NOT_EXIST ;
							}
						}
#ifdef DVB_SUBTITLES						
						/* Check Subtitle */
						if( (param->spid == DO_NOT_CARE) && (!param->stype) && IS_SUB(priv->pmt[ii].es[zz].type))
						{
								param->spid = priv->pmt[ii].es[zz].pid;
								param->stype= priv->pmt[ii].es[zz].type;
						}
#endif // end of DVB_SUBTITLES						
					}
					if(!ret)
					{
						mplayer_debug("### In [%s][%d] Can't find the requested audio pid [%d] ###\n", __func__, __LINE__, param->apid);
						param->apid = 0;
						param->atype = 0;
					}
				}
			}
		}
		else
			mplayer_debug ("## Carlos in [%s][%d] can't find the program check_pid[%d], type[%d]##\n", __func__, __LINE__, check_pid, type);

	}
	return ret;
}
#endif // end of CHECK_AUDIO_IS_VALID
#ifdef CHECK_EXIST_FLAGS
static inline void check_exist_from_pmt(ts_priv_t *priv, int pid, uint8_t flag)
{
	int32_t pmt_idx, pid_idx, i, j;
	int32_t prog;

	prog = progid_for_pid(priv, pid, 0);
	//pmt_idx = progid_idx_in_pmt(priv, priv->prog);
	pmt_idx = progid_idx_in_pmt(priv, prog);

	if(pmt_idx != -1)
	{
		pid_idx = es_pid_in_pmt(&(priv->pmt[pmt_idx]), pid);
		if(pid_idx != -1)
		{
			if (flag)
			{
#ifdef SEE_PARSE_PMT_DEBUG			
				if (!(priv->pmt[pmt_idx].es[pid_idx].exist_flag & THE_PID_IS_SCRAMBLE))
					mplayer_debug("demux_ts :  pid %d is scrambled data\n", pid);
#endif /* end of SEE_PARSE_PMT_DEBUG */					
				priv->pmt[pmt_idx].es[pid_idx].exist_flag |= THE_PID_IS_SCRAMBLE;
			}
			else
			{
				if (!(priv->pmt[pmt_idx].es[pid_idx].exist_flag & THE_PID_IS_EXIST))
				{
					parse_debug_printf("### In [%s][%d] set pid [%d] exist_flag to 1###\n", __func__, __LINE__, pid);
					priv->pmt[pmt_idx].es[pid_idx].exist_flag |= THE_PID_IS_EXIST;
				}
			}
		}
	}
	else
	{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			pmt_t *pmt = &(priv->pmt[i]);
			for(j = 0; j < pmt->es_cnt; j++)
			{
				if(pmt->es[j].pid == pid)
				{
					if (flag)
					{
#ifdef SEE_PARSE_PMT_DEBUG					
						if (!(pmt->es[j].exist_flag & THE_PID_IS_SCRAMBLE))
							mplayer_debug("demux_ts :  pid %d is scrambled data\n", pid);
#endif /* end of SEE_PARSE_PMT_DEBUG */							
						pmt->es[j].exist_flag |= THE_PID_IS_SCRAMBLE;
					}
					else
					{
						parse_debug_printf("### In [%s][%d] set pid [%d] exist_flag to 1###\n", __func__, __LINE__, pid);
						pmt->es[j].exist_flag |= THE_PID_IS_EXIST;
					}
					break;
				}
			}
		}
	}

}
#endif // end of CHECK_EXIST_FLAGS
#ifdef SUPPORT_SS_FINE_TUNE
#ifdef CONFIG_BLURAY
static void correct_bd_iso_timebar(demuxer_t *demuxer)
{
	double check_time = 0.0;
	struct bd_priv_t *bdpriv = demuxer->stream->priv;
	int ii = 0;
	if (first_video_pts != 0.0)
		check_time = first_video_pts;
	else if (first_audio_pts != 0.0)
		check_time = first_audio_pts;
	else
		check_time = 0.0;

	if (check_time != 0.0)
	{
		if (bdpriv && bdpriv->title_info && bdpriv->title_info->clip_count > 0)
		{
			if (abs(list_base_time[BD_ISO_IN_TIME][0] - check_time) > 2.0)
			{
				printf("@@@ In [%s][%d] check_time is [%f] BD_ISO_IN_TIME[%f] we need correct basetime ###\n", __func__, __LINE__, check_time, BD_ISO_IN_TIME);
				list_base_time[BD_ISO_OUT_TIME][0] = list_base_time[BD_ISO_OUT_TIME][0] - list_base_time[BD_ISO_IN_TIME][0] + check_time;
				list_base_time[BD_ISO_BASE_TIME][0] = list_base_time[BD_ISO_OUT_TIME][0];
				list_base_time[BD_ISO_IN_TIME][0] = check_time;
				bdiso_debug("$$ In [%s][%d] clip[%d] in_time[%f] out_time[%f] base_time[%f] $$\n", __func__, __LINE__, 0, list_base_time[BD_ISO_IN_TIME][0], list_base_time[BD_ISO_OUT_TIME][0], list_base_time[BD_ISO_BASE_TIME][0]);
				for (ii = 1; ii < bdpriv->title_info->clip_count ; ii++)
				{
					list_base_time[BD_ISO_BASE_TIME][ii] = list_base_time[BD_ISO_OUT_TIME][ii] - list_base_time[BD_ISO_IN_TIME][ii] + list_base_time[BD_ISO_BASE_TIME][ii-1];
					bdiso_debug("$$ In [%s][%d] clip[%d] in_time[%f] out_time[%f] base_time[%f] $$\n", __func__, __LINE__, ii, list_base_time[BD_ISO_IN_TIME][ii], list_base_time[BD_ISO_OUT_TIME][ii], list_base_time[BD_ISO_BASE_TIME][ii]);
				}
			}
			else
				printf("@@@ In [%s][%d] check_time is [%f] BD_ISO_IN_TIME[%f] nothing ###\n", __func__, __LINE__, check_time, BD_ISO_IN_TIME);
		}
	}
	else
		printf("@@@ In [%s][%d] check_time is 0.0, nothing @@@\n", __func__, __LINE__);
}
#endif /* end of CONFIG_BLURAY */
#endif /* end of SUPPORT_SS_FINE_TUNE */
