/*
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

#ifndef MPLAYER_DEMUX_TS_H
#define MPLAYER_DEMUX_TS_H

#include <stdint.h>

int mp_a52_framesize(uint8_t *buf, int *srate);

/* Carlos move below define and function from demux_ts.c, because dvb need the defines item */
#define AUTO_CHECK /* We will parse PAT and PMT table to set the video and audio format when enable this define , used by demux_skyts.c*/

#include "aviheader.h"
/* For BD-ISO */
#include "udf.h"
#include "libbluray/bluray.h"
#include "libbluray/bdnav/mpls_parse.h"
//#define see_message(fmt, args...) printf(fmt, ##args)
#define see_message            mplayer_debug
//#define SEE_GET_PACKET
//#define SEE_DVB_DEBUG
//#define OPEN_DEBUG
//#define SEE_PARSE_PMT_DEBUG
//#define RUNTIME_SWITCH_AUDIO_OR_SUBTITLE_STREAM_DEBUG
//#define SKYDVB_DYNAMIC_PID_DEBUG
//#define SEEK_FINE_TUNE_DEBUG
//#define BDISO_DEBUG
//#define CHECK_AAC_HEADER_DEBUG
//#define DYNAMIC_DEBUG_MESSAGE

#ifdef SEE_GET_PACKET
#define debug_printf	see_message
#else // else of SEE_GET_PACKET
#define debug_printf(fmt, args...) {}
#endif // end of SEE_GET_PACKET

#ifdef OPEN_DEBUG
#define debug2_printf see_message
#else // else of OPEN_DEBUG
#define debug2_printf(fmt, args...) {}
#endif // end of OPEN_DEBUG

#ifdef SEE_DVB_DEBUG
#define dvb_debug_printf see_message
#else // else of SEE_DVB_DEBUG
#define dvb_debug_printf(fmt, args...) {}
#endif // end of SEE_DVB_DEBUG

#ifdef SEE_PARSE_PMT_DEBUG
#define parse_debug_printf see_message
#else // else of SEE_PARSE_PMT_DEBUG
#define parse_debug_printf(fmt, args...) {}
#endif // end of SEE_PARSE_PMT_DEBUG

#ifdef RUNTIME_SWITCH_AUDIO_OR_SUBTITLE_STREAM_DEBUG
#define switch_debug_printf see_message
#else // else of RUNTIME_SWITCH_AUDIO_OR_SUBTITLE_STREAM_DEBUG
#define switch_debug_printf(fmt, args...) {}
#endif // end of RUNTIME_SWITCH_AUDIO_OR_SUBTITLE_STREAM_DEBUG

#ifdef SKYDVB_DYNAMIC_PID_DEBUG
#define skydvb_printf see_message
#else // else of SKYDVB_DYNAMIC_PID_DEBUG
#define skydvb_printf(fmt, args...)	{}
#endif // end of SKYDVB_DYNAMIC_PID_DEBUG

#ifdef SEEK_FINE_TUNE_DEBUG
#define seek_printf	see_message
#else // else of SEEK_FINE_TUNE_DEBUG
#define seek_printf(fmt, args...) {}
#endif // end of SEEK_FINE_TUNE_DEBUG

#ifdef BDISO_DEBUG
#define bdiso_debug	see_message
#else /* else of BDISO_DEBUG */
#define bdiso_debug(fmt, args...) {}
#endif /* end of BDISO_DEBUG */

#ifdef CHECK_AAC_HEADER_DEBUG
#define audio_check_debug see_message
#else /* else of CHECK_AAC_HEADER_DEBUG */
#define audio_check_debug(fmt, args...) {}
#endif /* end of CHECK_AAC_HEADER_DEBUG */

#ifdef DYNAMIC_DEBUG_MESSAGE
#define dynamic_debug see_message
#else /* else of DYNAMIC_DEBUG_MESSAGE */
#define dynamic_debug(fmt, args...) {}
#endif /* end of DYNAMIC_DEBUG_MESSAGE */

/* For DVB_T workaround, skydvb_need_workaround used */
#define INIT_STATUS			0x00
#define NEED_SEEK_CHANNEL	0x01
#define LOST_DVBT_SIGNAL	0x02
#define READY_TO_DISPLAY	0x04

#define TS_MAX_RETRY_COUNT	80 // 80 * 1000 => 0.8 sec
#define DVB_MAX_RETRY_COUNT	50 // 50 * 1000 => 0.5 sec
#define TS_PH_PACKET_SIZE 192
#define TS_FEC_PACKET_SIZE 204
#define TS_PACKET_SIZE 188
#define TS2_PACKET_SIZE      192 // .m2ts   //SkyMedi_Vincent06032009
#define NB_PID_MAX 8192

#define MAX_HEADER_SIZE 6			/* enough for PES header + length */
#define MAX_CHECK_SIZE	65535
/* 20100809, carlos change TS_MAX_PROBE_SIZE  from 2000000 to 5000000, for search pmt table */
/* 20101110, carlos change TS_MAX_PROBE_SIZE  from 5000000 to 6000000, for play file : Hokkaido-2008_1920x1080_H264.avi, the first audio offset is 0x5314F8 */
#define TS_MAX_PROBE_SIZE 6000000 /* do not forget to change this in cfg-common-opts.h, too */
//#define NUM_CONSECUTIVE_TS_PACKETS 64 /* Carlos change 32 to 64 for fixed Kung.Fu.Panda.Blu-ray.REMUX.H264.1080P.High@L4.1 TrueHD.DD51.SILUHD.disk1.ts */
/* Carlos change again in 2011-08-15, set NUM_CONSECUTIVE_TS_PACKETS to 32, using good and bad to check ts_file, mantis 5570 */
#define NUM_CONSECUTIVE_TS_PACKETS 32 /* Carlos change 32 to 64 for fixed Kung.Fu.Panda.Blu-ray.REMUX.H264.1080P.High@L4.1 TrueHD.DD51.SILUHD.disk1.ts */
#define NUM_CONSECUTIVE_AUDIO_PACKETS 348
#define MAX_A52_FRAME_SIZE 3840

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

#define TYPE_AUDIO 0x1
#define TYPE_VIDEO 0x2
#define TYPE_SUB   0x4

/* for check_hwts_packets using */
#define CHECK_AUDIO	0x00
#define CHECK_VIDEO	0x01
#define CHECK_SUB	0x02

#define CHECK_EXIST_FLAGS		/* Record exist or not : speed up parse and check exist, add by carlos 2010-11-09 */

#ifdef HW_TS_DEMUX
#define FIX_NOSOUND_SELECT_OTHER_VIDEO /* carlos fixed thumbnail display different video on 20100809 */
#define CHECK_AUDIO_IS_VALID /* carlos fixed check the audio is valid, 20100810 */
#define SELECT_FIRST_VIDEO_IN_PMT /* carlos fixed HD M2TS : BD_AVC_H264_Vali.m2ts */
#define SUPPORT_HWTS_CHANGE_AUDIO_OR_SUBTITLE /* 2010-10-20 add, support hwts change audio or subtitle stream */
#define ENABLE_HWTS_WHEN_USING_SKYDROID	/* 2010-11-23, enable this defined will auto enable hwts when we using android UI */
#define ENABLE_HARDWARE_TS_START_CODE	/* Enable hardware TS Start code function */
/* For check scramble and exist */
#define THE_PID_IS_EXIST	(1 << 0)
#define THE_PID_IS_SCRAMBLE	(1 << 1)
#define THE_PID_IS_CA		(1 << 2)
/* For check we already write data to hw demux */
#define HWTS_WRITE_STREAM_SLEEP	0x00
#define HWTS_WRITE_STREAM_START	(1 << 0)
#define HWTS_WRITE_STREAM_WRITE_OK	(1 << 1)

#endif // end of HW_TS_DEMUX

/* 2010-11-23 disable TS_SUPPORT_PCM_AUDIO. This defined value will using the other audio stream when we meet DTS audio */
//#define TS_SUPPORT_PCM_AUDIO /* Support the pcm audio when enable this define, 2010-10-21 */
/* We need disable this flag when Honda change the DTS decoder */
#define BUGGY_FIXED_DTS_PROBLEM	/* using buggy method to fixed dts no pts and duplicate len cause too many video queue, 2010-10-28 */
/*
 * Carlos add on 2011-01-05, for enable ss fine-tune
 * The method is using the  total size to divide by total duration time to get the block of the per-second.
 */
#define SUPPORT_SS_FINE_TUNE
#ifdef ENABLE_HARDWARE_TS_START_CODE
#define DYNAMIC_CALCULATED_PTS	/* 2011-08-11 dynamic calculated new pts, because the fps is smaller than 15 */
#endif /* end of ENABLE_HARDWARE_TS_START_CODE */

#define DO_NOT_PLAY		-2
#define DO_NOT_CARE		-1


#define BDISO_TIMEBRR_TYPE_FIXED_INTIME		0x01
#define BDISO_TIMEBRR_TYPE_CON_TIME			0x02
#define BDISO_TIMEBRR_TYPE_RANDOM_TIME		0x03

#define BD_ISO_IN_TIME		0x00
#define BD_ISO_OUT_TIME		0x01
#define BD_ISO_BASE_TIME	0x02
	/*
	 *	Case 1. Every clip has same in_time
	 *		Clip 1	11.333	~	636.133
	 *		Clip 2	11.333	~	5822.0
	 *		Clip 3	11.333	~	5883.0
	 *		Clip 4	11.333	~	600.5
	 *	Case 2. The pts will be continuous even changed clip.
	 *		Clip 1	11.133	~	200.0
	 *		Clip 2	200.0	~	764.0
	 *		Clip 3	764.0	~	899.0
	 *		Clip 4	899.0	~	1200.0
	 *	Case 3. No rule to check the pts.
	 *		Clip 1	600.0	~	636.133
	 *		Clip 2	4313.0	~	5822.0
	 *		Clip 3	5852.0	~	5883.0
	 *		Clip 4	600.0	~	600.5
	 */
/* The below struct from stream/stream_bd.c */
struct bd_priv_t {
	int iso;
	int title;
	int chapter;
	int m2ts;
	int clip_idx;
	uint64_t clip_len;
	uint64_t clip_pos;
	int segment_idx;
	uint64_t segment_len;
	uint64_t segment_pos;
	char timebar_type;
	char *device;
	BLURAY *bd;
	BLURAY_TITLE_INFO *title_info;
	MPLS_PL *play_list;
	uint64_t *clip_list;
	UDF_FILE_BLOCK *segments;
};


typedef enum
{
	UNKNOWN		= -1,
	VIDEO_MPEG1 	= 0x10000001,
	VIDEO_MPEG2 	= 0x10000002,
	VIDEO_MPEG4 	= 0x10000004,
	VIDEO_H264 	= 0x10000005,
	VIDEO_AVC	= mmioFOURCC('a', 'v', 'c', '1'),
	VIDEO_VC1	= mmioFOURCC('W', 'V', 'C', '1'),
	AUDIO_MP2   	= 0x50,
	AUDIO_A52   	= 0x2000,
	AUDIO_DTS	= 0x2001,
	AUDIO_LPCM_BE  	= 0x10001,
	AUDIO_BPCM	= mmioFOURCC('B', 'P', 'C', 'M'), // Blue ray PCM 
	AUDIO_AAC	= mmioFOURCC('M', 'P', '4', 'A'),
	AUDIO_TRUEHD	= mmioFOURCC('T', 'R', 'H', 'D'),
	AUDIO_AC3PLUS	= mmioFOURCC('A', 'C', '3', 'P'),
	AUDIO_EAC3	= mmioFOURCC('E', 'A', 'C', '3'),
	SPU_DVD		= 0x3000000,
	SPU_DVB		= 0x3000001,
	SPU_TELETEXT	= 0x3000002,
#ifdef DVB_SUBTITLES
	SPU_PGS		= 0x3000003,	//Fuchun 2010.09.28
#endif
	PES_PRIVATE1	= 0xBD00000,
	SL_PES_STREAM	= 0xD000000,
	SL_SECTION	= 0xD100000,
	MP4_OD		= 0xD200000,
	NOT_SUPPORT	= -2 , // Carlos add 2010-10-19
	NOT_EXIST = -3 , // Carlos add 2010-11-01
} es_stream_type_t;

typedef struct {
	uint8_t *buffer;
	uint16_t buffer_len;
} ts_section_t;

typedef struct {
	int size;
	unsigned char *start;
	uint16_t payload_size;
	es_stream_type_t type, subtype;
	double pts, last_pts;
	int pid;
	char lang[4];
	int last_cc;				// last cc code (-1 if first packet)
	int is_synced;
	ts_section_t section;
	uint8_t *extradata;
	int extradata_alloc, extradata_len;
	struct {
		uint8_t au_start, au_end, last_au_end;
	} sl;
} ES_stream_t;

typedef struct {
	void *sh;
	int id;
	int type;
} sh_av_t;

typedef struct MpegTSContext {
	int packet_size; 		// raw packet size, including FEC if present e.g. 188 bytes
	ES_stream_t *pids[NB_PID_MAX];
	sh_av_t streams[NB_PID_MAX];
} MpegTSContext;


typedef struct {
	demux_stream_t *ds;
	demux_packet_t *pack;
	int offset, buffer_size;
#ifdef HW_TS_DEMUX
	int pfd;
#endif // end of HW_TS_DEMUX
} av_fifo_t;

#define MAX_EXTRADATA_SIZE 64*1024
typedef struct {
	int32_t object_type;	//aka codec used
	int32_t stream_type;	//video, audio etc.
	uint8_t buf[MAX_EXTRADATA_SIZE];
	uint16_t buf_size;
	uint8_t szm1;
} mp4_decoder_config_t;

typedef struct {
	//flags
	uint8_t flags;
	uint8_t au_start;
	uint8_t au_end;
	uint8_t random_accesspoint;
	uint8_t random_accesspoint_only;
	uint8_t padding;
	uint8_t use_ts;
	uint8_t idle;
	uint8_t duration;

	uint32_t ts_resolution, ocr_resolution;
	uint8_t ts_len, ocr_len, au_len, instant_bitrate_len, degr_len, au_seqnum_len, packet_seqnum_len;
	uint32_t timescale;
	uint16_t au_duration, cts_duration;
	uint64_t ocr, dts, cts;
} mp4_sl_config_t;

typedef struct {
	uint16_t id;
	uint8_t flags;
	mp4_decoder_config_t decoder;
	mp4_sl_config_t sl;
} mp4_es_descr_t;

typedef struct {
	uint16_t id;
	uint8_t flags;
	mp4_es_descr_t *es;
	uint16_t es_cnt;
} mp4_od_t;

typedef struct {
	uint8_t skip;
	uint8_t table_id;
	uint8_t ssi;
	uint16_t section_length;
	uint16_t ts_id;
	uint8_t version_number;
	uint8_t curr_next;
	uint8_t section_number;
	uint8_t last_section_number;
	struct pat_progs_t {
		uint16_t id;
		uint16_t pmt_pid;
	} *progs;
	uint16_t progs_cnt;
	ts_section_t section;
} pat_t;

typedef struct {
	uint16_t progid;
	uint8_t skip;
	uint8_t table_id;
	uint8_t ssi;
	uint16_t section_length;
	uint8_t version_number;
	uint8_t curr_next;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t PCR_PID;
	uint16_t prog_descr_length;
	ts_section_t section;
	uint16_t es_cnt;
	struct pmt_es_t {
		uint16_t pid;
		uint32_t type;	//it's 8 bit long, but cast to the right type as FOURCC
		uint16_t descr_length;
		uint8_t format_descriptor[5];
		uint8_t lang[4];
		uint16_t mp4_es_id;
		uint16_t bpcm_channels;
#ifdef CHECK_EXIST_FLAGS		
		uint8_t exist_flag; // record exist or not
#endif // end of CHECK_EXIST_FLAGS		
	} *es;
	mp4_od_t iod, *od;
	mp4_es_descr_t *mp4es;
	int od_cnt, mp4es_cnt;
} pmt_t;

typedef struct {
	uint64_t size;
	float duration;
	double first_pts;
	double last_pts;
} TS_stream_info;

typedef struct {
	MpegTSContext ts;
	int last_pid;
	av_fifo_t fifo[3];	//0 for audio, 1 for video, 2 for subs
	pat_t pat;
	pmt_t *pmt;
	uint16_t pmt_cnt;
	uint32_t prog;
	uint32_t vbitrate;
	int keep_broken;
	int last_aid;
	int last_vid;
#ifdef DVB_SUBTITLES
	int last_sid;	//Fuchun 2010.09.28
#endif
	char packet[TS_FEC_PACKET_SIZE];
	TS_stream_info vstr, astr;
} ts_priv_t;

#ifdef HW_TS_DEMUX
#define NEED_PAUSE_FILTER 0x01
#define NEED_RESUME_FILTER	0x02
#define NEED_RESET_FILTER	(NEED_PAUSE_FILTER | NEED_RESUME_FILTER)
#define SLOW_DOWN_SPEED	0x01
#define NORMAL_SPPED	0x02
#define IN_FF_FR		0x04
char check_hwts_thread(void);
/* For H264 FF 1~4 Speed, sync audio : restart audio filter */
void restart_audio_buffer(demux_stream_t *d_audio, void *priv, char operation_flag);
/* For H264 FF 1~4 Speed, sync sub : restart sub filter */
void restart_sub_buffer(demux_stream_t *d_sub, void *priv, char operation_flag);
int check_need_write_ts_video(void *priv, int is_play);
int check_need_read_ts_video(void *priv);
void change_axi_speed(demux_stream_t *d_video, void *priv, char operation_flag);

#ifdef DVBT_USING_NORMAL_METHOD
int dvb_fill_ts_buffer(demuxer_t *demux, ts_priv_t *ts_priv,  demux_stream_t *ds);
#endif // end of DVBT_USING_NORMAL_METHOD
int check_hwts_packets(ts_priv_t *ts_priv, int fd);
#endif // end of HW_TS_DEMUX

/* provide for skydvb */
extern int skydvb_need_workaround;
int get_skydvb_wrokaround(void);
void set_skydvb_wrokaround(int value);

#endif /* MPLAYER_DEMUX_TS_H */
