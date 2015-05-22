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

#ifndef MPLAYER_DEMUXER_H
#define MPLAYER_DEMUXER_H

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stream/stream.h"
#ifdef CONFIG_ASS
#include "libass/ass_mp.h"
#endif

#ifdef CONFIG_DRM_ENABLE
// WT, 100615, include drm api header
#include "libmpwmdrm/DxDrmClient.h"
#endif

#ifdef HAVE_BUILTIN_EXPECT
#define likely(x) __builtin_expect ((x) != 0, 1)
#define unlikely(x) __builtin_expect ((x) != 0, 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#ifdef DEMUX_THREAD
#include <pthread.h>
#endif

//#define MAX_FRAME_SIZE	0x180000	// 1.5M
#define MAX_FRAME_SIZE	0x100000	// 1M
#define ROUND_UP(x, y)	(((unsigned int)(x) + ((y)-1)) & ~((y)-1)) // must be power of 2

#include "salloc.h"

#define MAX_PACKS 4096
#define MAX_PACK_BYTES 0x2000000

#define DEMUXER_TYPE_UNKNOWN 0
#define DEMUXER_TYPE_MPEG_ES 1
#define DEMUXER_TYPE_MPEG_PS 2
#define DEMUXER_TYPE_AVI 3
#define DEMUXER_TYPE_AVI_NI 4
#define DEMUXER_TYPE_AVI_NINI 5
#define DEMUXER_TYPE_ASF 6
#define DEMUXER_TYPE_MOV 7
#define DEMUXER_TYPE_VIVO 8
#define DEMUXER_TYPE_TV 9
#define DEMUXER_TYPE_FLI 10
#define DEMUXER_TYPE_REAL 11
#define DEMUXER_TYPE_Y4M 12
#define DEMUXER_TYPE_FILM 14
#define DEMUXER_TYPE_ROQ 15
#define DEMUXER_TYPE_MF 16
#define DEMUXER_TYPE_AUDIO 17
#define DEMUXER_TYPE_OGG 18
#define DEMUXER_TYPE_RAWAUDIO 20
#define DEMUXER_TYPE_RTP 21
#define DEMUXER_TYPE_RAWDV 22
#define DEMUXER_TYPE_PVA 23
#define DEMUXER_TYPE_SMJPEG 24
#define DEMUXER_TYPE_XMMS 25
#define DEMUXER_TYPE_RAWVIDEO 26
#define DEMUXER_TYPE_MPEG4_ES 27
#define DEMUXER_TYPE_GIF 28
#define DEMUXER_TYPE_MPEG_TS 29
#define DEMUXER_TYPE_H264_ES 30
#define DEMUXER_TYPE_MATROSKA 31
#define DEMUXER_TYPE_REALAUDIO 32
#define DEMUXER_TYPE_MPEG_TY 33
#define DEMUXER_TYPE_LMLM4 34
#define DEMUXER_TYPE_LAVF 35
#define DEMUXER_TYPE_NSV 36
#define DEMUXER_TYPE_VQF 37
#define DEMUXER_TYPE_AVS 38
#define DEMUXER_TYPE_AAC 39
#define DEMUXER_TYPE_MPC 40
#define DEMUXER_TYPE_MPEG_PES 41
#define DEMUXER_TYPE_MPEG_GXF 42
#define DEMUXER_TYPE_NUT 43
#define DEMUXER_TYPE_LAVF_PREFERRED 44
#define DEMUXER_TYPE_RTP_NEMESI 45
#define DEMUXER_TYPE_MNG 46
#define DEMUXER_TYPE_AAC_ADIF 47    //HONDA_ADIF 
#define DEMUXER_TYPE_AC3 48
#define DEMUXER_TYPE_SKYMPEG_TS 49
#define DEMUXER_TYPE_APE 50
// This should always match the higest demuxer type number.
// Unless you want to disallow users to force the demuxer to some types
#define DEMUXER_TYPE_MIN 0
#define DEMUXER_TYPE_MAX 49//48

#define DEMUXER_TYPE_DEMUXERS (1<<16)
// A virtual demuxer type for the network code
#define DEMUXER_TYPE_PLAYLIST (2<<16)


#define MP_NOPTS_VALUE (-1LL<<63) //both int64_t and double should be able to represent this exactly


// DEMUXER control commands/answers
#define DEMUXER_CTRL_NOTIMPL -1
#define DEMUXER_CTRL_DONTKNOW 0
#define DEMUXER_CTRL_OK 1
#define DEMUXER_CTRL_GUESS 2
#define DEMUXER_CTRL_GET_TIME_LENGTH 10
#define DEMUXER_CTRL_GET_PERCENT_POS 11
#define DEMUXER_CTRL_SWITCH_AUDIO 12
#define DEMUXER_CTRL_RESYNC 13
#define DEMUXER_CTRL_SWITCH_VIDEO 14
#define DEMUXER_CTRL_IDENTIFY_PROGRAM 15
#define DEMUXER_CTRL_CORRECT_PTS 16
#define DEMUXER_CTRL_PROGRAM_LIST 17
#ifdef AVI_NOT_SORT
#define DEMUXER_CTRL_SWITCH_SUB 18	//Fuchun 2011.03.01 just for AVI now.
#endif
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
#define DEMUXER_CTRL_ADD_AUDIO_STREAM	19
#define DEMUXER_CTRL_ADD_SUBTITLE_STREAM	20
#endif /* end of SUPPORT_SKYDVB_DYNAMIC_PID */ 

#define SEEK_ABSOLUTE (1 << 0)
#define SEEK_FACTOR   (1 << 1)
#define SEEK_CHAPTER  (1 << 2)

#define MP_INPUT_BUFFER_PADDING_SIZE 8

//Fuchun 2009.12.02
extern int speed_mult;
extern int FRtoFF;	//Barry 2010-12-24
extern int is_mjpeg;
extern int read_nextframe;	//Fuchun 2010.03.09
extern int mpeg_fast;
extern unsigned char* videobuffer;
extern int dvd_fast;		//Fuchun 2010.04.20
extern int not_supported_profile_level;		//Barry 2010-06-08
extern int FR_to_end;
extern int mjpeg_skip1;
extern int mjpeg_skip2;
extern int sky_vdec_vdref_num;
extern int wait_video_or_audio_sync;
extern int ad_real_channels;  //Fuchun 2011.05.17
extern int change_flag;

//Fuchun 2010.12.20 define for A/V quick sync
#define AVSYNC_NORMAL				1
#define AVSYNC_VIDEO_CATCH_AUDIO	2
#define AVSYNC_AUDIO_CATCH_VIDEO	4
#define AVSYNC_NOSOUND			8
#define AVSYNC_NOVIDEO				16
#define AVSYNC_BEGINNING			32
#define AVSYNC_NOACT				AVSYNC_NOSOUND|AVSYNC_NOVIDEO

#define FAST_TSDEMUX
#define FAST_MPEG_PS_DEMUX
/* 2010-12-15, Jeffrey say disable this flag because some file caused kernel panic : kernel parse start code error, TODO: mpeg2 */
/* 2010-12-31, Carlos enable this flag. Used it need with -quick_bd */
#define HWTS_READ_FRAME	//Barry 2010-11-29 instead of FAST_TSDEMUX video read frame
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
extern int quick_bd;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */
#define ADJUST_STREAM_PTS	//Barry 2011-01-06

#define MAX_QUICKBD_FIRST_HDR_LEN 256
#define IPC_THREAD_IDLE		0x00
#define IPC_THREAD_RUNNING	0x01
#define IPC_THREAD_END		0x02
#define IPC_THREAD_STOP		0x04

// Holds one packet/frame/whatever
typedef struct demux_packet {
  int len;
  double pts;
  double endpts;
  double stream_pts;
  off_t pos;  // position in index (AVI) or file (MPG)
  unsigned char* buffer;
  int flags; // keyframe, etc
  int refcount;   //refcounter for the master packet, if 0, buffer can be free()d
  int free;	// need to free or not -  Raymond 2009/04/18
  struct demux_packet* master; //pointer to the master packet if this one is a cloned one
  struct demux_packet* next;
} demux_packet_t;

typedef struct {
  int buffer_pos;          // current buffer position
  int buffer_size;         // current buffer size
  unsigned char* buffer;   // current buffer, never free() it, always use free_demux_packet(buffer_ref);
  double pts;              // current buffer's pts
  int pts_bytes;           // number of bytes read after last pts stamp
  int eof;                 // end of demuxed stream? (true if all buffer empty)
  off_t pos;                 // position in the input stream (file)
  off_t dpos;                // position in the demuxed stream
  int pack_no;		   // serial number of packet
  int flags;               // flags of current packet (keyframe etc)
  int non_interleaved;     // 1 if this stream is not properly interleaved,
                           // so e.g. subtitle handling must do explicit reads.
//---------------
#ifdef DEMUX_THREAD
  pthread_mutex_t dsmtx;  // mutex to protect demux queue
#endif
  int packs;              // number of packets in buffer
  int bytes;              // total bytes of packets in buffer
  demux_packet_t *first;  // read to current buffer from here
  demux_packet_t *last;   // append new packets from input stream to here
  demux_packet_t *current;// needed for refcounting of the buffer
  int id;                 // stream ID  (for multiple audio/video streams)
  struct demuxer *demuxer; // parent demuxer structure (stream handler)
// ---- asf -----
  demux_packet_t *asf_packet;  // read asf fragments here
  int asf_seq;
// ---- mov -----
  unsigned int ss_mul,ss_div;
// ---- stream header ----
  void* sh;
} demux_stream_t;

typedef struct demuxer_info {
  char *name;
  char *author;
  char *encoder;
  char *comments;
  char *copyright;
} demuxer_info_t;

#define MAX_A_STREAMS 256
#define MAX_V_STREAMS 256
#define MAX_S_STREAMS 256

struct demuxer;

extern int correct_pts;
extern int user_correct_pts;

extern demux_packet_t *dp_last; //memory leak for ts and dvbts

/**
 * Demuxer description structure
 */
typedef struct demuxer_desc {
  const char *info; ///< What is it (long name and/or description)
  const char *name; ///< Demuxer name, used with -demuxer switch
  const char *shortdesc; ///< Description printed at demuxer detection
  const char *author; ///< Demuxer author(s)
  const char *comment; ///< Comment, printed with -demuxer help

  int type; ///< DEMUXER_TYPE_xxx
  int safe_check; ///< If 1 detection is safe and fast, do it before file extension check

  /// Check if can demux the file, return DEMUXER_TYPE_xxx on success
  int (*check_file)(struct demuxer *demuxer); ///< Mandatory if safe_check == 1, else optional
  /// Get packets from file, return 0 on eof
  int (*fill_buffer)(struct demuxer *demuxer, demux_stream_t *ds); ///< Mandatory
  /// Open the demuxer, return demuxer on success, NULL on failure
  struct demuxer* (*open)(struct demuxer *demuxer); ///< Optional
  /// Close the demuxer
  void (*close)(struct demuxer *demuxer); ///< Optional
  // Seek
  void (*seek)(struct demuxer *demuxer, float rel_seek_secs, float audio_delay, int flags); ///< Optional
  // Control
  int (*control)(struct demuxer *demuxer, int cmd, void *arg); ///< Optional
} demuxer_desc_t;

typedef struct demux_chapter
{
  uint64_t start, end;
  char* name;
} demux_chapter_t;

typedef struct demux_attachment
{
  char* name;
  char* type;
  void* data;
  unsigned int data_size;
} demux_attachment_t;

typedef struct demuxer {
  const demuxer_desc_t *desc;  ///< Demuxer description structure
  off_t filepos; // input stream current pos.
  off_t movi_start;
  off_t movi_end;
  stream_t *stream;
  double stream_pts;       // current stream pts, if applicable (e.g. dvd)
  double reference_clock;
  char *filename; ///< Needed by avs_check_file
  int synced;  // stream synced (used by mpeg)
  int type;    // demuxer type: mpeg PS, mpeg ES, avi, avi-ni, avi-nini, asf
  int file_format;  // file format: mpeg/avi/asf
  int seekable;  // flag
//Robert 20100712 new VideoBuffer Idx
  int need_double_copy; //need copy to videobuffer before decode
  //
  demux_stream_t *audio; // audio buffer/demuxer
  demux_stream_t *video; // video buffer/demuxer
  demux_stream_t *sub;   // dvd subtitle buffer/demuxer

  // stream headers:
  void* a_streams[MAX_A_STREAMS]; // audio streams (sh_audio_t)
  void* v_streams[MAX_V_STREAMS]; // video sterams (sh_video_t)
  void *s_streams[MAX_S_STREAMS];   // dvd subtitles (flag)

  // pointer to teletext decoder private data, if demuxer stream contains teletext
  void *teletext;

  demux_chapter_t* chapters;
  int num_chapters;

  demux_attachment_t* attachments;
  int num_attachments;

  void* priv;  // fileformat-dependent data
  char** info;

#ifdef CONFIG_DRM_ENABLE
  // WT, 100615, add drm params
  HDxDrmStream	drmStream;
  DxBool 			drmFileOpened;
  DxStatus 		lastResult;
  DxBool 			clientInitialized;
  DxBool 			fileIsDrmProtected;
  #endif
#ifdef SUPPORT_DIVX_DRM
  char divx_drm_notsupport;
#endif /* end of SUPPORT_DIVX_DRM */
} demuxer_t;

typedef struct {
  int progid;        //program id
  int aid, vid, sid; //audio, video and subtitle id
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
  int aformat, vformat, sformat; // audio format and subtitle format
#endif /* end of SUPPORT_SKYDVB_DYNAMIC_PID */
} demux_program_t;

typedef struct {
   int id;
   char lang[4];
} program_list_item_t;

typedef struct {
  int aid_num;
  int sid_num;
  program_list_item_t *a_list;
  program_list_item_t *s_list;
} demux_program_list_t;

static inline demux_packet_t* new_demux_packet(int len){
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  if(dp)
  {
  dp->len=len;
  dp->next=NULL;
  dp->pts=MP_NOPTS_VALUE;
  dp->endpts=MP_NOPTS_VALUE;
  dp->stream_pts = MP_NOPTS_VALUE;
  dp->pos=0;
  dp->flags=0;
  dp->refcount=1;
  dp->free = 1;		// Raymond 2009/04/18
  dp->master=NULL;
  dp->buffer=NULL;
#if 1	//Barry 2010-10-25
  if (len > 0)
  {
  	dp->buffer = (unsigned char *)malloc(len + MP_INPUT_BUFFER_PADDING_SIZE);
	if (!dp->buffer){
		dp->len = 0;
		printf("@@@@ In [%s][%d] alloc dp buffer failed. size:%x @@@@\n", __func__, __LINE__);
	}
  }
#else
  if (len > 0 && (dp->buffer = (unsigned char *)malloc(len + MP_INPUT_BUFFER_PADDING_SIZE)))
    memset(dp->buffer + len, 0, 8);
  else
    dp->len = 0;
#endif
  }
  else
  	printf("@@@@ In [%s][%d] alloc dp failed  @@@@\n", __func__, __LINE__);
  return dp;
}

// Raymond 2009/04/18	- no free for video packet
static inline demux_packet_t* new_demux_video_packet(int len){
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  dp->len=len;
  dp->next=NULL;
  // still using 0 by default in case there is some code that uses 0 for both
  // unknown and a valid pts value
  dp->pts=correct_pts ? MP_NOPTS_VALUE : 0;
  dp->endpts=MP_NOPTS_VALUE;
  dp->stream_pts = MP_NOPTS_VALUE;
  dp->pos=0;
  dp->flags=0;
  dp->refcount=1;
  dp->free = 0;		// Raymond 2009/04/18
  dp->master=NULL;
  dp->buffer=NULL;
  if (len > 0 && (dp->buffer = (unsigned char *)salloc(len + 8)))
    memset(dp->buffer + len, 0, 8);
  else
    dp->len = 0;
  return dp;
}

// Raymond 2009/06/01	- no free and given video buffer
static inline demux_packet_t* new_video_packet(int len, unsigned char *buf)
{
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  dp->len=len;
  dp->next=NULL;
  // still using 0 by default in case there is some code that uses 0 for both
  // unknown and a valid pts value
  dp->pts=correct_pts ? MP_NOPTS_VALUE : 0;
  dp->endpts=MP_NOPTS_VALUE;
  dp->stream_pts = MP_NOPTS_VALUE;
  dp->pos=0;
  dp->flags=0;
  dp->refcount=1;
  dp->free = 0;		// Raymond 2009/04/18
  dp->master=NULL;
  dp->buffer=NULL;
  if ( len > 0 && buf !=NULL )
  {
  	dp->buffer = buf;
	memset(dp->buffer + len, 0, 8);	
  }
  else
    dp->len = 0;
  return dp;
}

static inline void resize_demux_packet(demux_packet_t* dp, int len)
{
	//check for no change at all - Raymond 2009/04/18
	if (len == dp->len)
		return;
	
  if(len > 0)
  {
     dp->buffer=(unsigned char *)realloc(dp->buffer,len+8);
  }
  else
  {
     if(dp->buffer) 
	 if(dp->free == 1)	// Raymond 2009/04/18	
	 	free(dp->buffer);
     dp->buffer=NULL;
  }
  dp->len=len;

#if 1	//Barry 2010-10-25
  if (!dp->buffer)
     dp->len = 0;
#else
  if (dp->buffer)
     memset(dp->buffer + len, 0, 8);
  else
     dp->len = 0;
#endif
}

static inline demux_packet_t* clone_demux_packet(demux_packet_t* pack){
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  while(pack->master) pack=pack->master; // find the master
  memcpy(dp,pack,sizeof(demux_packet_t));
  dp->next=NULL;
  dp->refcount=0;
  dp->master=pack;
  pack->refcount++;
  return dp;
}

static inline void free_demux_packet(demux_packet_t* dp){
  if(dp == NULL)
	  return;

  if (dp->master==NULL){  //dp is a master packet
    dp->refcount--;
    if (dp->refcount==0){
      if (dp->buffer) 
      	{
      		if(dp->free == 1)	// Raymond 2009/04/18
	  		free(dp->buffer);

		dp->buffer = NULL;
      	}
      free(dp);
	  dp = NULL;
    }
    return;
  }
  // dp is a clone:
  free_demux_packet(dp->master);
  /* Make sure dp is not NULL point */
  if (dp)
  {
	  free(dp);
	  dp = NULL;
  }
}

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

static inline void *realloc_struct(void *ptr, size_t nmemb, size_t size) {
  if (nmemb > SIZE_MAX / size) {
    free(ptr);
    return NULL;
  }
  return realloc(ptr, nmemb * size);
}

demux_stream_t* new_demuxer_stream(struct demuxer *demuxer,int id);
demuxer_t* new_demuxer(stream_t *stream,int type,int a_id,int v_id,int s_id,char *filename);
void free_demuxer_stream(demux_stream_t *ds);
void free_demuxer(demuxer_t *demuxer);

void ds_add_packet(demux_stream_t *ds,demux_packet_t* dp);
void ds_read_packet(demux_stream_t *ds, stream_t *stream, int len, double pts, off_t pos, int flags);

int demux_fill_buffer(demuxer_t *demux,demux_stream_t *ds);
int ds_fill_buffer(demux_stream_t *ds);

static inline off_t ds_tell(demux_stream_t *ds){
  return (ds->dpos-ds->buffer_size)+ds->buffer_pos;
}

static inline int ds_tell_pts(demux_stream_t *ds){
  return (ds->pts_bytes-ds->buffer_size)+ds->buffer_pos;
}

int demux_read_data(demux_stream_t *ds,unsigned char* mem,int len);
int demux_pattern_3(demux_stream_t *ds, unsigned char *mem, int maxlen,
                    int *read, uint32_t pattern);

#define demux_peekc(ds) (\
     (likely(ds->buffer_pos<ds->buffer_size)) ? ds->buffer[ds->buffer_pos] \
     :((unlikely(!ds_fill_buffer(ds)))? (-1) : ds->buffer[ds->buffer_pos] ) )
#if 1
#define demux_getc(ds) (\
     (likely(ds->buffer_pos<ds->buffer_size)) ? ds->buffer[ds->buffer_pos++] \
     :((unlikely(!ds_fill_buffer(ds)))? (-1) : ds->buffer[ds->buffer_pos++] ) )
#else
static inline int demux_getc(demux_stream_t *ds){
  if(ds->buffer_pos>=ds->buffer_size){
    if(!ds_fill_buffer(ds)){
//      printf("DEMUX_GETC: EOF reached!\n");
      return -1; // EOF
    }
  }
//  printf("[%02X]",ds->buffer[ds->buffer_pos]);
  return ds->buffer[ds->buffer_pos++];
}
#endif

#if 1	// Raymond 2009/06/05
static inline void PUT16(int val, uint8_t *dest)
{
	dest[0] = ((uint16_t)(val) & 0xff00) >> 8;
	dest[1] = (uint16_t)(val) & 0xff;	
//	printf("val = %d, %02X, %02X\n", val, dest[0], dest[1]);
}

static inline void PUT32(int val, uint8_t *dest)
{
	dest[0] = ((uint32_t)(val) & 0xff000000) >> 24;
	dest[1] = ((uint32_t)(val) & 0xff0000) >> 16;
	dest[2] = ((uint32_t)(val) & 0xff00) >> 8;
	dest[3] = (uint32_t)(val) & 0xff;
//	printf("val = %d, %02X, %02X %02X, %02X\n", val, dest[0], dest[1], dest[2], dest[3]);
}

static inline void PUT32_L(int val, uint8_t *dest)  //modify by ethan little
{
	dest[3] = ((uint32_t)(val) & 0xff000000) >> 24;        
	dest[2] = ((uint32_t)(val) & 0xff0000) >> 16;        
	dest[1] = ((uint32_t)(val) & 0xff00) >> 8;        
	dest[0] = (uint32_t)(val) & 0xff;
}

#endif

void ds_free_packs(demux_stream_t *ds);
int ds_get_packet(demux_stream_t *ds,unsigned char **start);
int ds_get_packet_pts(demux_stream_t *ds, unsigned char **start, double *pts);
int ds_get_packet_sub(demux_stream_t *ds,unsigned char **start);
double ds_get_next_pts(demux_stream_t *ds);
int ds_parse(demux_stream_t *sh, uint8_t **buffer, int *len, double pts, off_t pos);
void ds_clear_parser(demux_stream_t *sh);

// This is defined here because demux_stream_t ins't defined in stream.h
stream_t* new_ds_stream(demux_stream_t *ds);

static inline int avi_stream_id(unsigned int id){
  unsigned char a,b;
  a = id - '0';
  b = (id >> 8) - '0';
  if(a>9 || b>9) return 100; // invalid ID
  return a*10+b;
}

demuxer_t* demux_open(stream_t *stream,int file_format,int aid,int vid,int sid,char* filename);
void demux_flush(demuxer_t *demuxer);
int demux_seek(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags);
demuxer_t*  new_demuxers_demuxer(demuxer_t* vd, demuxer_t* ad, demuxer_t* sd);

// AVI demuxer params:
extern int index_mode;  // -1=untouched  0=don't use index  1=use (geneate) index
extern char *index_file_save, *index_file_load;
extern int force_ni;
extern int pts_from_bps;

extern int extension_parsing;

int demux_info_add(demuxer_t *demuxer, const char *opt, const char *param);
char* demux_info_get(demuxer_t *demuxer, const char *opt);
int demux_info_print(demuxer_t *demuxer);
int demux_control(demuxer_t *demuxer, int cmd, void *arg);

int demuxer_get_current_time(demuxer_t *demuxer);
double demuxer_get_time_length(demuxer_t *demuxer);
int demuxer_get_percent_pos(demuxer_t *demuxer);
int demuxer_switch_audio(demuxer_t *demuxer, int index);
int demuxer_switch_video(demuxer_t *demuxer, int index);
#ifdef AVI_NOT_SORT
int demuxer_switch_sub(demuxer_t *demuxer);
#endif

int demuxer_type_by_filename(char* filename);

void demuxer_help(void);
int get_demuxer_type_from_name(char *demuxer_name, int *force);

int demuxer_add_attachment(demuxer_t* demuxer, const char* name,
                           const char* type, const void* data, size_t size);

int demuxer_add_chapter(demuxer_t* demuxer, const char* name, uint64_t start, uint64_t end);
int demuxer_seek_chapter(demuxer_t *demuxer, int chapter, int mode, float *seek_pts, int *num_chapters, char **chapter_name);

/// Get current chapter index if available.
int demuxer_get_current_chapter(demuxer_t *demuxer);
/// Get chapter name by index if available.
char *demuxer_chapter_name(demuxer_t *demuxer, int chapter);
/// Get chapter display name by index.
char *demuxer_chapter_display_name(demuxer_t *demuxer, int chapter);
/// Get chapter start time and end time by index if available.
float demuxer_chapter_time(demuxer_t *demuxer, int chapter, float *end);
/// Get total chapter number.
int demuxer_chapter_count(demuxer_t *demuxer);
/// Get current angle index.
int demuxer_get_current_angle(demuxer_t *demuxer);
/// Set angle.
int demuxer_set_angle(demuxer_t *demuxer, int angle);
/// Get number of angles.
int demuxer_angles_count(demuxer_t *demuxer);

// get the index of a track
// lang is a comma-separated list
int demuxer_audio_track_by_lang(demuxer_t* demuxer, char* lang);
int demuxer_sub_track_by_lang(demuxer_t* demuxer, char* lang);

// find the default track
// for subtitles, it is the first track with default attribute
// for audio, additionally, the first track is selected if no track has default attribute set
int demuxer_default_audio_track(demuxer_t* d);
int demuxer_default_sub_track(demuxer_t* d);
int demux_avi_set_index_pos(demuxer_t* d);	//Fuchun 2010.08.24
#ifdef SUPPORT_DIVX_DRM
void demux_avi_save_idxpos(demuxer_t* d);  //Fuchun 2011.03.10
#endif /* end of SUPPORT_DIVX_DRM */
//extern void FFFR_to_normalspeed(void *this);
#ifdef AVI_NOT_SORT
void demux_avi_read_audio_index(demuxer_t *demux);
#endif

//Barry 2011-10-05
extern ssize_t svread(int fd, void *buf, size_t count);
extern void skyfb_set_thumnail_mode_enable(void);
extern void skyfb_set_display_status(int flag);
extern int svsd_ipc_drop_video_cnts(void);
extern int svsd_ipc_uninit_callback(unsigned int standard);

#ifdef STREAM_NI
void demux_switch_stream(demuxer_t *demuxer, demux_stream_t* ds);
#endif
#endif /* MPLAYER_DEMUXER_H */
