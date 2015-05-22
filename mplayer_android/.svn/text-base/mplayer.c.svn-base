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

/// \file
/// \ingroup Properties Command2Property OSDMsgStack

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

//#define DEBUG_VIDEO_CONTENT /* Carlos add this debug flag for calculate video frame checksum , 2010-12-27 */

#if defined(__MINGW32__) || defined(__CYGWIN__)
#define _UWIN 1  /*disable Non-underscored versions of non-ANSI functions as otherwise int eof would conflict with eof()*/
#include <windows.h>
#endif
#include <string.h>
#include <unistd.h>

// #include <sys/mman.h>
#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/ioctl.h>
#include <sys/wait.h>
#else
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGPIPE	13	/* broken pipe */
#endif

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/resource.h>

#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>

#include <errno.h>

#include <sys/shm.h>

#include "mp_msg.h"

#define HELP_MP_DEFINE_STATIC
#include "help_mp.h"

#include "m_option.h"
#include "m_config.h"
#include "m_property.h"

#include "cfg-mplayer-def.h"

#include "libavutil/intreadwrite.h"
#include "libavutil/avstring.h"

#include "subreader.h"

#include "libvo/video_out.h"
#include "libvo/sky_api.h"

#include "libvo/font_load.h"
#include "libvo/sub.h"

#ifdef CONFIG_X11
#include "libvo/x11_common.h"
#endif

#include "libao2/audio_out.h"

#include "codec-cfg.h"

#include "edl.h"
#include "mplayer.h"
#include "spudec.h"
#include "vobsub.h"
#include "access_mpcontext.h"

#include "osdep/getch2.h"
#include "osdep/timer.h"

#include "gui/interface.h"

#include "input/input.h"
#include "pthread-macro.h"

#ifdef SUPPORT_DIVX_DRM
#include "libmpdemux/aviheader.h"
#include "drm_mplayer.h"
int DRM_Open = 0;
//extern DRM_chunk *dd_chunk;
int slow_down_speed = 0;
#endif /* end of SUPPORT_DIVX_DRM */
//#define MP_MALLOC_HOOK
#ifdef MP_MALLOC_HOOK
#include <malloc.h>
#include <execinfo.h>

/* Prototypes for our hooks.  */
static void mp_init_hook(void);
static void *mp_malloc_hook(size_t, const void *);
static void *mp_realloc_hook(void *ptr, size_t size, const void *caller);
static void *mp_memalign_hook(size_t alignment, size_t size, const void *caller);
static void mp_free_hook(void *ptr, const void *caller);

/* Variables to save original hooks. */
static void *(*old_malloc_hook)(size_t, const void *);
static void *(*old_realloc_hook)(void *ptr, size_t size, const void *caller);
static void *(*old_memalign_hook)(size_t alignment, size_t size, const void *caller);
static void (*old_free_hook)(void *ptr, const void *caller);

/* Override initializing hook from the C library. */
void (*__malloc_initialize_hook) (void) = mp_init_hook;

/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */

static inline int fls(int x)
{
    int r = 32;

    if (!x)
        return 0;
    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }
    return r;
}

static inline void mp_save_hook(void)
{
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_memalign_hook = __memalign_hook;
    old_free_hook = __free_hook;
}

static inline void mp_restore_hook(void)
{
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __memalign_hook = old_memalign_hook;
    __free_hook = old_free_hook;
}

static inline void mp_new_hook(void)
{
    __malloc_hook = mp_malloc_hook;
    __realloc_hook = mp_realloc_hook;
    __memalign_hook = mp_memalign_hook;
    __free_hook = mp_free_hook;
}

static void mp_init_hook(void)
{
    mp_save_hook();
    mp_new_hook();
    /*
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_memalign_hook = __memalign_hook;
    old_free_hook = __free_hook;

    __malloc_hook = mp_malloc_hook;
    __realloc_hook = mp_realloc_hook;
    __memalign_hook = mp_memalign_hook;
    __free_hook = mp_free_hook;
    */
}

static void *mp_malloc_hook(size_t size, const void *caller)
{
    void *result;
    char **strings;

    /* Restore all old hooks */
    //__malloc_hook = old_malloc_hook;
    mp_restore_hook();

    /* Call recursively */
    result = malloc(size);

    /* Save underlying hooks */
    //old_malloc_hook = __malloc_hook;

    /* printf() might call malloc(), so protect it too. */
    //if(fls(size) >= 18)
    printf("malloc(%u) called from %p returns %p\n", (unsigned int) size, caller, result);

    /* Restore our own hooks */
    //__malloc_hook = mp_malloc_hook;
    mp_new_hook();

    return result;
}

static void *mp_realloc_hook(void *ptr, size_t size, const void *caller)
{
    void *result;

    /* Restore all old hooks */
    //__realloc_hook = old_realloc_hook;
    mp_restore_hook();

    /* Call recursively */
    result = realloc(ptr, size);

    /* Save underlying hooks */
    //old_realloc_hook = __realloc_hook;

    /* printf() might call realloc(), so protect it too. */
    //if(fls(size) >= 18)
    printf("realloc(%p, %u) called from %p returns %p\n", ptr, (unsigned int) size, caller, result);

    /* Restore our own hooks */
    //__realloc_hook = mp_realloc_hook;
    mp_new_hook();

    return result;
}

static void *mp_memalign_hook(size_t alignment, size_t size, const void *caller)
{
    void *result;

    /* Restore all old hooks */
    //__memalign_hook = old_memalign_hook;
    mp_restore_hook();

    /* Call recursively */
    result = memalign(alignment, size);

    /* Save underlying hooks */
    //old_memalign_hook = __memalign_hook;

    /* printf() might call memalign(), so protect it too. */
    //if(fls(size) >= 18)
    printf("memalign(%u, %u) called from %p returns %p\n", (unsigned int) alignment, (unsigned int) size, caller, result);

    /* Restore our own hooks */
    //__memalign_hook = mp_memalign_hook;
    mp_new_hook();

    return result;
}

static void mp_free_hook(void *ptr, const void *caller)
{
    /* Restore all old hooks */
    //__free_hook = old_free_hook;
    mp_restore_hook();

    /* Call recursively */
    free(ptr);

    /* Save underlying hooks */
    //old_free_hook = __free_hook;

    /* printf() might call free(), so protect it too. */
    printf("free(%p) called from %p\n", ptr, caller);

    /* Restore our own hooks */
    //__free_hook = mp_free_hook;
    mp_new_hook();
}

#endif

#ifdef USE_INSTRUMENT_FUNCTIONS
//===================
struct layout {
    struct layout *next;        /*! Holds the previous value of EBP (Frame Pointer)*/
    void *return_address;       /*! Holds the previous value of EIP (Return Address)*/
};
/*
 * If build static mode, need set START_ADDR to 0x8000, END_ADDR to 0x550000
 * static : from 0x8134 ~ 0x54f9a0
 * normal : from 0xa9c8 ~ 0x492380
 */
#define START_ADDR 0xa000
#define END_ADDR 0x4a0000
unsigned int g_mplayer_func_parent_list[END_ADDR - START_ADDR];
unsigned int g_mplayer_func_child_list[END_ADDR - START_ADDR];
FILE *fp;

#define ADVANCE_STACK_FRAME(next) (struct layout *) ( next )
#define FIRST_FRAME_POINTER  __builtin_frame_address (0)
#define CURRENT_STACK_FRAME  ({ char __csf; &__csf; })

void __cyg_profile_func_enter( void *func_address, void *call_site )
__attribute__ ((no_instrument_function));
void __cyg_profile_func_exit ( void *func_address, void *call_site )
__attribute__ ((no_instrument_function));
void __cyg_profile_func_enter( void *this, void *callsite )
{
    /* Function Entry Address */
    {
        fprintf(fp, "E%p\n", (int *)this);
        //fprintf(stderr, "E%p  Call=%p START_ADDR=%p\n", (int *)this, (int *)callsite, START_ADDR);
        g_mplayer_func_parent_list[(unsigned int)callsite - START_ADDR]++;
        g_mplayer_func_child_list[(unsigned int)this - START_ADDR]++;
    }
}
void __cyg_profile_func_exit( void *this, void *callsite )
{
    fprintf(fp, "X%p\n", (int *)this);
}

/* Constructor and Destructor Prototypes */

void main_constructor( void )
__attribute__ ((no_instrument_function, constructor));

void main_destructor( void )
__attribute__ ((no_instrument_function, destructor));

/* Output trace file pointer */
void main_constructor( void )
{
    fp = fopen( "/data/tmpfsmisc/mplayer.trace", "w" );
    if (fp == NULL) exit(-1);
}

void main_destructor( void )
{
    fclose(fp);
}
//===================
#endif // end of USE_INSTRUMENT_FUNCTIONS

//#ifdef HW_TS_DEMUX	Barry 2010-10-18 disable
#include "libmpdemux/demux_ts.h"
//#endif // end of HW_TS_DEMUX

demux_packet_t *dp_last=NULL;
int slave_mode=0;
int player_idle_mode=0;
int quiet=0;
int enable_mouse_movements=0;
float start_volume = -1;

#include "osdep/priority.h"

char *heartbeat_cmd;

// update video count, used to print debug message, set -1 in reset_all_global_variable() to enable it
int uvcnt;

#define ROUND(x) ((int)((x)<0 ? (x)-0.5 : (x)+0.5))

/*  carlos add 2010-08-23, for 138E_12354_43000_20071022_100M.trp, had audio packet but decode audio filaed */
//#define CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO	//Todo
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
int setdvb_by_pid= 0;
int stop_tuner = 0;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
int quick_bd = 0;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */

#ifdef HAVE_RTC
#ifdef __linux__
#include <linux/rtc.h>
#else
#include <rtc.h>
#define RTC_IRQP_SET RTCIO_IRQP_SET
#define RTC_PIE_ON   RTCIO_PIE_ON
#endif /* __linux__ */
#endif /* HAVE_RTC */

#include "stream/tv.h"
#include "stream/stream_radio.h"
#ifdef CONFIG_DVBIN
#include "stream/dvbin.h"
#endif
#include "stream/cache2.h"

//**************************************************************************//
//             Playtree
//**************************************************************************//
#include "playtree.h"
#include "playtreeparser.h"

//**************************************************************************//
//             Config
//**************************************************************************//
#include "parser-cfg.h"
#include "parser-mpcmd.h"

m_config_t* mconfig;

//**************************************************************************//
//             Config file
//**************************************************************************//

static int cfg_inc_verbose(m_option_t *conf) {
    ++verbose;
    return 0;
}

static int cfg_include(m_option_t *conf, char *filename) {
    return m_config_parse_config_file(mconfig, filename);
}

#include "get_path.h"

//**************************************************************************//
//**************************************************************************//
//             Input media streaming & demultiplexer:
//**************************************************************************//

//carlos add debug message
unsigned int yyy;
unsigned int xxx = 0;
#define CHECK_TIME(string) { \
					yyy = GetTimer();	\
					if (!xxx)	\
						xxx = yyy;	\
					printf("#### Check in [%s] [%s][%d] diff time is [%3d] ms####\n", string, __func__, __LINE__, (yyy-xxx)/1000);	\
					xxx = yyy;	\
					}
static int max_framesize=0;
#define MAX_CHECK_AUDIO	50
#define MAX_CHECK_VIDEO	500

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#ifdef CONFIG_DVDREAD
#include "stream/stream_dvd.h"
#endif
#include "stream/stream_dvdnav.h"
//+Skyviia_Vincent07202010
#ifdef CONFIG_DVDNAV
// vo color mode:
// -1 : no settings
//  0 : Y
//  1 : YUV
//  2 : RGB
//  3 : BGR
//  4 : YUYV
int     dvdnav_color_spu_flg = -1;
int     dvdnav_audio_need_uninit = 0;
int dvdnav_video_first = 0;

// enable/disable SPU menu button
int     dvdnav_color_spu = 1;
int     oldsx=0,oldsy=0,oldex=0,oldey=0;
int     vo_need_dvdnav_menu = 0;
unsigned int    disp2_addr;
DvdnavState   dvdnavstate;
extern int skyfd;
extern int skyfb_fd;    //Polun 2011-06-01 ++
extern unsigned int dvdnav_btn_virtaddr[2];
extern unsigned int dvd_btn_addr_idx;
extern int dvdnav_should_do_alpha;
extern int dvdnav_globle_num_subtitle;
int dvdnav_return_menu=0;
int dvdnav_sub_menu_select=0; //0:inital 1:menu select first 2:cmd switch first,
int dvdnav_audio_menu_select=0; //0:inital 1:menu select first 2:cmd switch first,
int dvdnav_rel_seek=1;	//Skyviia_Vincent03142011 set default value to 1
int dvdnav_seek_result = 0; //0:ok 1:Request to seek behind end  //Skyviia_Vincent02222011
int dvdnav_movie_sub_id=0; //Skyviia_Vincent03292011 mantis:3824
int dvdnav_movie_audio_id=0;
int resampleao_fbflg = 0;   //Polun 2011-06-13 add resampleao_fbflg 
extern int vd_get_is_deint(void);
int dvdnav_decode_cnts_in_cell_change = 0;
int same_pts_cnt=0;
//int dvdnav_button_on=0;
int dvdnav_resume = 0;
int dvdnav_error_file=0;
int dvdnav_first_menu=0;
int dvdnav_auto_action=0;
int dvdnav_btn_num = 0; // Mantis:5236 LASTMAN.iso
int dvdnav_pgcN_change = 0; // Mantis:5389 The Bucket List. sids always are zero.
int dvdnav_menupage_chg=0; // Mantis:5403 LASTMAN.iso after selected scene the button can't control
int dvdnav_toomanypkg=0;
int dvdnav_sub_reg=0;
int dvdnav_clear_subcc_buf=0; //Mantis:5988 seek_chapter needs to clear osd buf
int dvdnav_is_delay_cell=0; //Mantis:6099,6001
int menu_rsm_cmd = 0; //Mantis:5545
#endif /* CONFIG_DVDNAV */
//Skyviia_Vincent07202010+
extern int sky_telecine;
//Polun 2011-09-01 for waiting_for_superman dvd dvdnav_audio != current_id  
extern int do_switch_audio_flag;
extern int mkv_indexnull_flag;

float first_frame_pts = 0.0f ; //Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv -ss can't AVSYNC

#include "libmpcodecs/dec_audio.h"
#include "libmpcodecs/dec_video.h"
#include "libmpcodecs/mp_image.h"
#include "libmpcodecs/vf.h"
#include "libmpcodecs/vd.h"

#include "mixer.h"

#include "mp_core.h"

//**************************************************************************//
//**************************************************************************//

// Common FIFO functions, and keyboard/event FIFO code
#include "mp_fifo.h"
int noconsolecontrols=0;
//**************************************************************************//

// Not all functions in mplayer.c take the context as an argument yet
static MPContext mpctx_s = {
    .osd_function = OSD_PLAY,
    .begin_skip = MP_NOPTS_VALUE,
    .play_tree_step = 1,
    .global_sub_pos = -1,
    .set_of_sub_pos = -1,
    .file_format = DEMUXER_TYPE_UNKNOWN,
    .loop_times = -1,
#ifdef CONFIG_DVBIN
    .last_dvb_step = 1,
#endif
};

#ifdef READ_PERF_TEST
int cpy = 0;
int spd = 0;
#endif

static MPContext *mpctx = &mpctx_s;

int fixed_vo=0;

unsigned int audio_s32=0;		// Honda 2010/04/30
// benchmark:
double video_time_usage=0;
double video_read_time_usage=0;	// Raymond 2009/07/22
double vout_time_usage=0;
static double audio_time_usage=0;
unsigned int total_time_usage_start=0;	//Barry 2010-04-07
static int total_frame_cnt=0;
static int drop_frame_cnt=0; // total number of dropped frames
static int frame_cnt = 0;

static int frame_drop_lock = 0;

int benchmark=0;
int not_supported_profile_level = 0;// 1: video format not support    2:audio format not support   3:video file without audio track	//Barry 2010-05-19
// options:
#define DEFAULT_STARTUP_DECODE_RETRY 8 //64
int auto_quality=0;
static int output_quality=0;

float playback_speed=1.0;

int use_gui=0;

#ifdef CONFIG_GUI
int enqueue=0;
#endif

static int list_properties = 0;

int osd_level=1;
// if nonzero, hide current OSD contents when GetTimerMS() reaches this
unsigned int osd_visible;
int osd_duration = 1000;

int term_osd = 1;
static char* term_osd_esc = "\x1b[A\r\x1b[K";
static char* playing_msg = NULL;
// seek:
static double seek_to_sec;
static off_t seek_to_byte=0;
static off_t step_sec=0;
static int loop_seek=0;

static m_time_size_t end_at = { .type = END_AT_NONE, .pos = 0 };

// A/V sync:
int autosync=0; // 30 might be a good default value.

// may be changed by GUI:  (FIXME!)
float rel_seek_secs=0;
int abs_seek_pos=0;

// codecs:
char **audio_codec_list=NULL; // override audio codec
char **video_codec_list=NULL; // override video codec
char **audio_fm_list=NULL;    // override audio codec family
char **video_fm_list=NULL;    // override video codec family

// demuxer:
extern char *demuxer_name; // override demuxer
extern char *audio_demuxer_name; // override audio demuxer
extern char *sub_demuxer_name; // override sub demuxer

// streaming:
int audio_id=-1;
int video_id=-1;
int dvdsub_id=-1;
// this dvdsub_id was selected via slang
// use this to allow dvdnav to follow -slang across stream resets,
// in particular the subtitle ID for a language changes
int dvdsub_lang_id;
int vobsub_id=-1;
char* audio_lang=NULL;
char* dvdsub_lang=NULL;
static char* spudec_ifo=NULL;
char* filename=NULL; //"MI2-Trailer.avi";
int forced_subs_only=0;
int file_filter=1;
int sub_on=1; //0:off 1:on

int nosync=0;
//Robert 20101130 PAUSE command lock/unlock flag
int set_pause_lock=0;

//Robert 20100617 check video queue & audio buffers, prevent CPU Busy...
//cvq : current video queue buffers
//btw : bytes_to_write (audio buffers space)
//Robert 20101026 set cvq default = 5
int cvq_threshold=5, btw_threshold=6000;
int last_audio_bytes_to_write= 0;
double last_audio_pts = 0.0;

double sky_vdec_wait_sync_vpts=0.0;
extern int sky_vpts_total_cnts;
//Robert 20100712 new SVSD Framework
#include "libmpcodecs/sky_vdec_2.h"

//#define NEW_SVSD_CMD 1

int sky_vdec_vdref_num=0;
double sky_vdec_vq_ref_pts=0.0;
int last_svsd_video_cnts = 0;
int svsd_shm_id = NULL;
volatile unsigned char *svsd_shm_array=NULL;
vdec_shm_t *svsd_state = NULL;
vdec_svread_buf_t sky_nrb;

int sky_mpeg12_progressive_sequence=0;
extern int last_pulldown32_vft_diff;
extern unsigned int sky_disp_rate;
extern int sky_is_output_interlace;
extern int sky_vft_idx;
extern double sky_get_vft_total(int skip, int num);
extern double sky_vpts_history[MAX_SKY_VFT_BUFFER];
extern vdec_init_t vdec_init_info;
extern vdec_init2_t vdec_init2_info;

//#endif




int is_first_loop = 1;

// cache2:
int stream_cache_size=-1;
#ifdef CONFIG_STREAM_CACHE
extern int cache_fill_status;

float stream_cache_min_percent=20.0;
float stream_cache_seek_min_percent=50.0;
#else
#define cache_fill_status 0
#endif

int upnp = 0;
int upnp_duration = 0;
/* 2011-01-20 add samba parameter */
int samba = 0;

// dump:
static char *stream_dump_name="stream.dump";
int stream_dump_type=0;

// A-V sync:
static float default_max_pts_correction=-1;//0.01f;
static float max_pts_correction=0;//default_max_pts_correction;
static float c_total=0;
float audio_delay=0;
static int ignore_start=0;

static int softsleep=0;

double force_fps=0;
static int force_srate=0;
static int audio_output_format=-1; // AF_FORMAT_UNKNOWN
//Robert 20100615 enable framedrop by default
int frame_dropping=1;//0; // option  0=no drop  1= drop vo  2= drop decode
int check_framedrop_flag = 0;
static int play_n_frames=-1;
static int play_n_frames_mf=-1;

// screen info:
char** video_driver_list=NULL;
char** audio_driver_list=NULL;

// sub:
char *font_name=NULL;
char *sub_font_name=NULL;
extern int font_fontconfig;
float font_factor=0.75;
char **sub_name=NULL;

#ifdef VO_SUB_THREAD
float sub_delay=0.0;
#else
float sub_delay=0;
#endif

float sub_fps=0;
int   sub_auto = 1;
char *vobsub_name=NULL;
/*DSP!!char *dsp=NULL;*/
int   subcc_enabled=0;
int suboverlap_enabled = 1;
int   subcc_force_off = 0;
int   has_subcc=0;
int   subcc_reg=0; //Mantis:5940

#include "libass/ass_mp.h"

char* current_module=NULL; // for debugging

////////////////////////
#define  jfueng_2011_0311     
//for solve long period  without audio packet(攻殼機動隊.VOB) 
//can disable "#define jfueng_jfueng_2011_0311" for  roll back quickly
////////////////////////

#ifdef  jfueng_2011_0311
            unsigned int  audio_format_ref=0;
            extern int audio_long_period;
            int a_long_quiet_SP_yes =0;
            int a_long_quiet_SP_cnt=0;
#endif

#if 1		// Raymond 2010/03/09
//Fuchun 2009.12.02	FF/FB
int speed_mult = 0;
int FRtoFF = 0;	//Barry 2010-12-24
int audio_speed = 0;
float audio_start_pts = 0.0;
int get_audio_pts_first_flag = 1;
int Can_FF_FB = 1;
int rel_audio_id = -5;
int is_mjpeg = 0;
extern int Only_One_I;	//Fuchun 2010.02.24
int mpeg_fast = 0;		//Fuchun 2010.03.01
extern int mov_keyframe_size;	//Fuchun 2010.03.01
int read_nextframe = 1;	//Fuchun 2010.03.09
int dvd_fast = 0;		//Fuchun 2010.03.30
double last_rewind_pts = 0.0;
int rewind_mult = 1;
int wait_video_or_audio_sync = AVSYNC_NORMAL;
int dvbt_wait_sync = 0;		//0: normal, 1: just has video, 2: just has audio
int seek_sync_flag = 0;	//0: normal, 1: want to A/V sync after seek, 2: want to A/V sync at beginning, 3: for DVBT sync, 4: for system burst or something, 5: for dvdnav
int FR_to_end = 0;
int decode_num_cnt = 0;
int top_field_frame = 1;
unsigned int avsync_timeout_num = 150;
unsigned int avsync_timeout_cnt = 0;
unsigned int read_frame_time = 0;
unsigned int aserver_crash = 0;
int last_audio_id = -1;
int FFFR_to_normal = 0;	//Skyviia_Vincent03142011 mantis:3400 Death note black screen
double correct_last_dvdpts = 0.0;	//Fuchun 2011.04.26
int correct_frame_cnt = 0;
int audio_seek_flg = 0;//Polun 2011-07-27 + for audio play skip key time bar updata. 
int hit_eof_cunt = 0; //Polun 2011-08-02 +  mantis 5136
extern int Dvdnav_cmd_exit; //Polun 2012-02-03 fixed mantis6790 playback to eof signal 11 issue  
//Fuchun 2010.03.31	Subtitle
int vo_need_osd = 0;	//Fuchun 2009.12.15
int sub_visible = 1;
int first_osd = 1;		//Fuchun 2010.03.24
double last_sub_pts = -1.0;	//Fuchun 2010.03.24
int last_sub_lines = 0;
unsigned long long last_spu_pts = 0;
double correct_sub_pts = 0.0;
int rel_global_sub_pos = -5;
int sub_change_success = 0;
char subtitle_names[1024];
int audio_change_success = 0;
char audio_names[1024];
char video_names[64];
unsigned int no_osd = 0;
int sub_temp_utf8 = 0;
int display_height = 0;
int display_width = 0;
#ifdef DVB_SUBTITLES
int pgs_subtitle_flag = 0;
#endif
int teletext_subtitle_flag = 0;
int first_set_sub_pts = 1;
int recalcul_apts = 1;
int select_sub_id = -1;

int num_reorder_frames = 0;
int queue_frames = 0;

// Raymond 2009/07/07
int scanf_per_frame = 0;
int usleep_cnt = 0;

// Raymond 2009/08/03
int print_video_read_time = 0;
int print_video_decode_time = 0;
int print_audio_decode_time = 0;

//Robert 20110421 check h264 and need drop B
static int sky_video_fmt_is_h264 = 0;
static int sky_h264_need_check_dropB = 0;
//Robert 20100604
extern int h264_frame_mbs_only;

extern float video_aspect_ratio;	//Barry 2010-06-21
extern int avc1_nal_bytes;			//Barry 2010-07-09
int switch_audio_thread_status = 0;	//Barry 2011-02-17
int is_skynet = 0;     //Barry 2011-03-14
int audio_not_support = 0;	//Barry 2011-05-09
extern int not_support_audio_id;
#ifdef BD_CLIP_BASE_PTS
extern double bd_now_base_pts;
#endif
#ifdef DYNAMIC_CALCULATED_PTS
extern char need_do_dynamic_pts;
#endif /* end of DYNAMIC_CALCULATED_PTS */
int pre_isprogressive_flag = -1;
int AAC_MAIN_PROFILE = 0;	//Barry 2011-05-22
int set_disp_off = 0;	//Barry 2011-09-19
#ifdef DTS_CERTIFICATION	//Barry 2011-11-02
extern int DTS_in_WAV;
#endif

double AV_delay = 0.0;
int sky_avsync_method = 0;
extern int sky_repeatfield_cnt;

int bidx = 0;
extern unsigned char *VideoBuffer[3];

// Raymond 2009/09/25
extern unsigned int thumbnail_mode;

// Raymond 2009/04/01
extern int init_sky(void);
extern void uninit_sky(void);

extern void salloc_times_reset(void);
extern void salloc_uninit(void);

extern unsigned int get_thumbnail(char *callback_str); //SkyViia_Vincent01272010

#define CHECK_AAC_HEADER /* 2011-05-12 TS file will check aac_audio header before initial mp3 decoder */
#ifdef CHECK_AAC_HEADER
static int check_aac_header(unsigned char *buffer, int len, int mode);
#endif /* end of CHECK_AAC_HEADER */
#define ENABLE_KEY_EVENT_FUNCTION_MODE /* move key event progress to function, add by carlos 2010-10-14 */
#ifdef ENABLE_KEY_EVENT_FUNCTION_MODE
int parse_and_run_command(void);
#endif // end of ENABLE_KEY_EVENT_FUNCTION_MODE


void FFFR_to_normalspeed(void *this);

int slave_mode_quit = 0; //SkyViia_Vincent03232010

//Robert 20100503
#ifdef HAS_DVBIN_SUPPORT
//extern int skydvb_need_workaround;
#endif

#endif

unsigned int dts_packet = 0;
// ---

#ifdef CONFIG_MENU
#include "m_struct.h"
#include "libmenu/menu.h"
extern vf_info_t vf_info_menu;
static vf_info_t* libmenu_vfs[] = {
    &vf_info_menu,
    NULL
};
static vf_instance_t* vf_menu = NULL;
int use_menu = 0;
static char* menu_cfg = NULL;
static char* menu_root = "main";
#endif


#ifdef HAVE_RTC
static int nortc = 1;
static char* rtc_device;
#endif

edl_record_ptr edl_records = NULL; ///< EDL entries memory area
edl_record_ptr next_edl_record = NULL; ///< only for traversing edl_records
short edl_decision = 0; ///< 1 when an EDL operation has been made.
FILE* edl_fd = NULL; ///< fd to write to when in -edlout mode.
int use_filedir_conf;
int use_filename_title;

static unsigned int initialized_flags=0;
#include "mpcommon.h"
#include "command.h"

#include "metadata.h"


/*
IPC_control_Start__()
*/
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include "domain_socket.h"

extern int  code_domain;
int is_pause=0;
int start_play=PLAYER_STATUS_IDLE;
int my_current_pts=0;
int get_curpos_on=IPC_THREAD_IDLE;
int get_buffering_on=IPC_THREAD_IDLE;
int buffering_pthread_flag=0;
int duration_sec_cur=0;
int duration_sec_old=0;
int print_callback_str = 1;
unsigned int        ipcdomain = 0;	//for option using
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cbcond;
static pthread_mutex_t cbmtx;
mp_cmd_t* domainsocket_cmd=NULL;
pthread_t    ipc_thread_curpos_id = 0;
pthread_t    ipc_thread_buffering_id = 0;
int seek_by_time_done=0;

void ipc_callback(char *datastr);
void ipc_callback_error(void);
void ipc_callback_stop(void);
void ipc_callback_fast(void);
void ipc_callback_audio_fast(void);
void ipc_callback_curpos(void);
void ipc_callback_buffering(void);
void ipc_callback_duration(void);
void ipc_callback_audio_sub(void);
#ifdef SWITCH_AUDIO_THREAD
extern pthread_t thread_switch_audio;
extern int switch_audio_current_id;
extern void do_switch_audio_pthread(MPContext * mpctx);
#endif
/*
IPC_control_End__()
*/

//+SkyViia_Vincent06212010
int force_quit = 0;
int entry_while=0;
//SkyViia_Vincent06212010+

#define mp_basename2(s) (strrchr(s,'/')==NULL?(char*)s:(strrchr(s,'/')+1))

const void *mpctx_get_video_out(MPContext *mpctx)
{
    return mpctx->video_out;
}

const void *mpctx_get_audio_out(MPContext *mpctx)
{
    return mpctx->audio_out;
}

void *mpctx_get_demuxer(MPContext *mpctx)
{
    return mpctx->demuxer;
}

void *mpctx_get_playtree_iter(MPContext *mpctx)
{
    return mpctx->playtree_iter;
}

void *mpctx_get_mixer(MPContext *mpctx)
{
    return &mpctx->mixer;
}

int mpctx_get_global_sub_size(MPContext *mpctx)
{
    return mpctx->global_sub_size;
}

int mpctx_get_osd_function(MPContext *mpctx)
{
    return mpctx->osd_function;
}

static int is_valid_metadata_type (metadata_t type) {
    switch (type)
    {
        /* check for valid video stream */
    case META_VIDEO_CODEC:
    case META_VIDEO_BITRATE:
    case META_VIDEO_RESOLUTION:
    {
        if (!mpctx->sh_video)
            return 0;
        break;
    }

    /* check for valid audio stream */
    case META_AUDIO_CODEC:
    case META_AUDIO_BITRATE:
    case META_AUDIO_SAMPLES:
    {
        if (!mpctx->sh_audio)
            return 0;
        break;
    }

    /* check for valid demuxer */
    case META_INFO_TITLE:
    case META_INFO_ARTIST:
    case META_INFO_ALBUM:
    case META_INFO_YEAR:
    case META_INFO_COMMENT:
    case META_INFO_TRACK:
    case META_INFO_GENRE:
    {
        if (!mpctx->demuxer)
            return 0;
        break;
    }

    default:
        break;
    }

    return 1;
}

static char *get_demuxer_info (char *tag) {
    char **info = mpctx->demuxer->info;
    int n;

    if (!info || !tag)
        return NULL;

    for (n = 0; info[2*n] != NULL ; n++)
        if (!strcasecmp (info[2*n], tag))
            break;

    return info[2*n+1] ? strdup (info[2*n+1]) : NULL;
}

char *get_metadata (metadata_t type) {
    char *meta = NULL;
    sh_audio_t * const sh_audio = mpctx->sh_audio;
    sh_video_t * const sh_video = mpctx->sh_video;

    if (!is_valid_metadata_type (type))
        return NULL;

    switch (type)
    {
    case META_NAME:
    {
        return strdup (mp_basename2 (filename));
    }

    case META_VIDEO_CODEC:
    {
        if (sh_video->format == 0x10000001)
            meta = strdup ("mpeg1");
        else if (sh_video->format == 0x10000002)
            meta = strdup ("mpeg2");
        else if (sh_video->format == 0x10000004)
            meta = strdup ("mpeg4");
        else if (sh_video->format == 0x10000005)
            meta = strdup ("h264");
        else if (sh_video->format >= 0x20202020)
        {
            meta = malloc (8);
            sprintf (meta, "%.4s", (char *) &sh_video->format);
        }
        else
        {
            meta = malloc (8);
            sprintf (meta, "0x%08X", sh_video->format);
        }
        return meta;
    }

    case META_VIDEO_BITRATE:
    {
        meta = malloc (16);
        sprintf (meta, "%d kbps", (int) (sh_video->i_bps * 8 / 1024));
        return meta;
    }

    case META_VIDEO_RESOLUTION:
    {
        meta = malloc (16);
        sprintf (meta, "%d x %d", sh_video->disp_w, sh_video->disp_h);
        return meta;
    }

    case META_AUDIO_CODEC:
    {
        if (sh_audio->codec && sh_audio->codec->name)
            meta = strdup (sh_audio->codec->name);
        return meta;
    }

    case META_AUDIO_BITRATE:
    {
        meta = malloc (16);
        sprintf (meta, "%d kbps", (int) (sh_audio->i_bps * 8/1000));
        return meta;
    }

    case META_AUDIO_SAMPLES:
    {
        meta = malloc (16);
        sprintf (meta, "%d Hz, %d ch.", sh_audio->samplerate, sh_audio->channels);
        return meta;
    }

    /* check for valid demuxer */
    case META_INFO_TITLE:
        return get_demuxer_info ("Title");

    case META_INFO_ARTIST:
        return get_demuxer_info ("Artist");

    case META_INFO_ALBUM:
        return get_demuxer_info ("Album");

    case META_INFO_YEAR:
        return get_demuxer_info ("Year");

    case META_INFO_COMMENT:
        return get_demuxer_info ("Comment");

    case META_INFO_TRACK:
        return get_demuxer_info ("Track");

    case META_INFO_GENRE:
        return get_demuxer_info ("Genre");

    default:
        break;
    }

    return meta;
}

/// step size of mixer changes
int volstep = 3;

#ifdef CONFIG_DVDNAV
static void mp_dvdnav_context_free(MPContext *ctx) {
    // free stored decoded image
    if (ctx->nav_smpi) free_mp_image(ctx->nav_smpi);
    ctx->nav_smpi = NULL;
    if (ctx->nav_buffer) free(ctx->nav_buffer);
    ctx->nav_buffer = NULL;
    ctx->nav_start = NULL;
    ctx->nav_in_size = 0;
}
#endif

#include "cfg-mplayer.h"

void uninit_player(unsigned int mask) {

    //Fuchun 2009.12.03
    if(speed_mult != 0)
    {
    	if(mask == INITIALIZED_ALL || mask == (INITIALIZED_ALL-(INITIALIZED_GUI+INITIALIZED_INPUT+(fixed_vo?INITIALIZED_VO:0))))
		speed_mult = 0;
	else
		FFFR_to_normalspeed(NULL);
    }
    if(audio_speed != 0)
        audio_speed = 0;
   //Polun 2011-09-01 for waiting_for_superman dvd dvdnav_audio != current_id  
   if(do_switch_audio_flag != -1)
         do_switch_audio_flag = -1;


    extern unsigned int ori_display_scale;
    if(ori_display_scale != 0xFFFFFFFF) // someone has store ori_display_scale value
        skyfb_reset_display_scale();

#ifdef HW_TS_DEMUX
    if (vo_need_dvdnav_menu || skydroid == 0 || skyqt == 1 || no_osd == 1)
    {
        //don't need to workaround
    }
    else if((mpctx->demuxer) && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux) 
    {
        skyfb_trick_play_workaround(0);
    }
#endif

    if(mask == INITIALIZED_ALL || mask == (INITIALIZED_ALL-(INITIALIZED_GUI+INITIALIZED_INPUT+(fixed_vo?INITIALIZED_VO:0))))
    {
        //if(!no_osd) //Polun 2011-07-13 for slideshow background music
        if(!no_osd  &&  mpctx->sh_video)
        {
            skyfb_set_display_status(0);
            set_disp_off = 1;
        }
#ifdef SUPPORT_DIVX_DRM
		if (DRM_Open > 0)
		{
			//printf("=========   DRMUninit\n");
			DRMUninit();    //DRM_debug
			DRM_Open = 0;
		}
		if (slow_down_speed)
		{
			skyfb_trick_play_workaround(0);
			slow_down_speed = 0;
		}
#endif /* end of SUPPORT_DIVX_DRM */
    }

    mask &= initialized_flags;

    mp_msg(MSGT_CPLAYER,MSGL_DBG2,"\n*** uninit(0x%X)\n",mask);
    if(mask&INITIALIZED_ACODEC) {
        initialized_flags&=~INITIALIZED_ACODEC;
        current_module="uninit_acodec";
        if(mpctx->sh_audio) uninit_audio(mpctx->sh_audio);
#ifdef CONFIG_GUI
        if (use_gui) guiGetEvent(guiSetAfilter, (char *)NULL);
#endif
        mpctx->sh_audio=NULL;
        mpctx->mixer.afilter = NULL;
    }

    if(mask&INITIALIZED_VCODEC) {
        initialized_flags&=~INITIALIZED_VCODEC;
        current_module="uninit_vcodec";
        if(mpctx->sh_video) uninit_video(mpctx->sh_video);
        mpctx->sh_video=NULL;
#ifdef CONFIG_MENU
        vf_menu=NULL;
#endif
    }



    if(mask&INITIALIZED_DEMUXER) {
        initialized_flags&=~INITIALIZED_DEMUXER;
        current_module="free_demuxer";
        if(mpctx->demuxer) {
            mpctx->stream=mpctx->demuxer->stream;
            free_demuxer(mpctx->demuxer);

            if(dp_last && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
            {
                if (no_osd)
                    free_demux_packet(dp_last);
                else	//Barry 2010-10-26
                    dp_last = NULL;
            }
        }
        mpctx->demuxer=NULL;
    }

    // kill the cache process:
    if(mask&INITIALIZED_STREAM) {
        initialized_flags&=~INITIALIZED_STREAM;
        current_module="uninit_stream";
        if(mpctx->stream) free_stream(mpctx->stream);
        mpctx->stream=NULL;
    }

    if(mask&INITIALIZED_VO) {
        initialized_flags&=~INITIALIZED_VO;
        current_module="uninit_vo";
        mpctx->video_out->uninit();
        mpctx->video_out=NULL;
#ifdef CONFIG_DVDNAV
        mp_dvdnav_context_free(mpctx);
#endif
    }
    //Polun 2011-07-13 for slideshow background music removie
   //Polun 2011-06-01 ++s
  /* if (resampleao_fbflg == 1) //Polun 2011-06-13 modify resampleao_fbflg 
   {
       if(skyfb_fd != -1) 
	{
		close(skyfb_fd);
		skyfb_fd = -1;
		printf("skyfb_fd != -1 close skyfb_fd\n");
       }
       resampleao_fbflg = 0;
    }*/
   //Polun 2011-06-01 ++e

	if(mask == INITIALIZED_ALL || mask == (INITIALIZED_ALL-(INITIALIZED_GUI+INITIALIZED_INPUT+(fixed_vo?INITIALIZED_VO:0))))
	{
#ifdef CONFIG_ICONV		//Fuchun 2010.06.08
		subcp_close();
		if(temp_sub)
		{
			int i;
			for(i = 0; i < temp_sub->lines; i++)
				free(temp_sub->text[i]);
			free(temp_sub);
			temp_sub = NULL;
		}
#endif
	}

    // Must be after libvo uninit, as few vo drivers (svgalib) have tty code.
    if(mask&INITIALIZED_GETCH2) {
        initialized_flags&=~INITIALIZED_GETCH2;
        current_module="uninit_getch2";
        mp_msg(MSGT_CPLAYER,MSGL_DBG2,"\n[[[uninit getch2]]]\n");
        // restore terminal:
        getch2_disable();
    }

    if(mask&INITIALIZED_VOBSUB) {
        initialized_flags&=~INITIALIZED_VOBSUB;
        current_module="uninit_vobsub";
        if(vo_vobsub) vobsub_close(vo_vobsub);
        vo_vobsub=NULL;
    }

    if (mask&INITIALIZED_SPUDEC) {
        initialized_flags&=~INITIALIZED_SPUDEC;
        current_module="uninit_spudec";
        spudec_free(vo_spudec);
        vo_spudec=NULL;
    }

    if(mask&INITIALIZED_AO) {
        initialized_flags&=~INITIALIZED_AO;
        current_module="uninit_ao";
        if (mpctx->edl_muted) mixer_mute(&mpctx->mixer);
        if (mpctx->audio_out) mpctx->audio_out->uninit(mpctx->eof?0:1);
        mpctx->audio_out=NULL;
    }

#ifdef CONFIG_GUI
    if(mask&INITIALIZED_GUI) {
        initialized_flags&=~INITIALIZED_GUI;
        current_module="uninit_gui";
        guiDone();
    }
#endif

    if(mask&INITIALIZED_INPUT) {
        initialized_flags&=~INITIALIZED_INPUT;
        current_module="uninit_input";
        mp_input_uninit();
#ifdef CONFIG_MENU
        if (use_menu)
            menu_uninit();
#endif
    }

#ifdef DVBT_USING_NORMAL_METHOD
    //+SkyViia_Vincent06172010
    if( ((mask&INITIALIZED_AO) && (mask&INITIALIZED_ACODEC) && (mask&INITIALIZED_VCODEC) && (mask&INITIALIZED_VO))   || (mask&INITIALIZED_SKYVDEC)) {
        if (mask&INITIALIZED_DEMUXER)
        {
            current_module="uninit_sky";
            uninit_sky();	// Raymond 2009/11/05
        }
        else
        {
            extern int start_to_display;
            start_to_display = 0;
        }
    }
#else // else of DVBT_USING_NORMAL_METHOD
    //+SkyViia_Vincent06172010
    if( ((mask&INITIALIZED_AO) && (mask&INITIALIZED_ACODEC) && (mask&INITIALIZED_VCODEC) && (mask&INITIALIZED_VO)) || (mask&INITIALIZED_SKYVDEC)) {
        current_module="uninit_sky";
        uninit_sky();	// Raymond 2009/11/05
    }
#endif // end of DVBT_USING_NORMAL_METHOD
    //SkyViia_Vincent06172010+
    current_module="end of uninit_player";
}

void exit_player_with_rc(enum exit_reason how, int rc)
{
// WT, 100714, DRM stop event **add here?**
#if 0 //Barry 2011-12-02 move to demuxer.c free_demuxer() fix Segmentation fault bug
//#ifdef CONFIG_DRM_ENABLE
    if (mpctx->demuxer->drmFileOpened)
    {
        int er;
        er = DxDrmStream_HandleConsumptionEvent(mpctx->demuxer->drmStream, DX_EVENT_STOP);
        if (er != DX_SUCCESS)
            printf("DX:ERROR - stop event failed due to error %d\n", er);
        else
            printf("DX:DRM stop\n");
    }
#endif
    if (mpctx->user_muted && !mpctx->edl_muted) mixer_mute(&mpctx->mixer);
    uninit_player(INITIALIZED_ALL);
#if defined(__MINGW32__) || defined(__CYGWIN__)
    timeEndPeriod(1);
#endif
#ifdef CONFIG_X11
#ifdef CONFIG_GUI
    if ( !use_gui )
#endif
        vo_uninit();	// Close the X11 connection (if any is open).
#endif
//extern int vo_get_osd_thread_status(void);
//printf("vo_get_osd_thread_status=%d\n", vo_get_osd_thread_status());
#ifdef CONFIG_FREETYPE
    current_module="uninit_font";
    if (sub_font && sub_font != vo_font) free_font_desc(sub_font);
    sub_font = NULL;
    if (vo_font) free_font_desc(vo_font);
    vo_font = NULL;
    done_freetype();
#endif

    free_osd_list();

#ifdef CONFIG_ASS
    ass_library_done(ass_library);
    ass_library = NULL;
#endif

    current_module="exit_player";

// free mplayer config
    if(mconfig)
        m_config_free(mconfig);
    mconfig = NULL;

    if(mpctx->playtree_iter)
        play_tree_iter_free(mpctx->playtree_iter);
    mpctx->playtree_iter = NULL;
    if(mpctx->playtree)
        play_tree_free(mpctx->playtree, 1);
    mpctx->playtree = NULL;


    if(edl_records != NULL) free(edl_records); // free mem allocated for EDL
    edl_records = NULL;
    switch(how) {
    case EXIT_QUIT:
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_ExitingHow,MSGTR_Exit_quit);
	fflush(stdout);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_EXIT=QUIT\n");

#ifdef jfueng_2011_0311
           // printf("\n JF ::  reset  2  \n");          
	     audio_long_period = 0;	   
           a_long_quiet_SP_yes =0;
           a_long_quiet_SP_cnt=0;
#endif

        break;
    case EXIT_EOF:
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_ExitingHow,MSGTR_Exit_eof);
	fflush(stdout);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_EXIT=EOF\n");
        break;
    case EXIT_ERROR:
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_ExitingHow,MSGTR_Exit_error);
	fflush(stdout);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_EXIT=ERROR\n");
        break;
    default:
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_EXIT=NONE\n");
    }
    mp_msg(MSGT_CPLAYER,MSGL_DBG2,"max framesize was %d bytes\n",max_framesize);

    exit(rc);
}

void exit_player(enum exit_reason how)
{
    exit_player_with_rc(how, 1);
}

#ifndef __MINGW32__
static void child_sighandler(int x) {
    pid_t pid;
    while((pid=waitpid(-1,NULL,WNOHANG)) > 0);
}
#endif

#ifdef CONFIG_CRASH_DEBUG
static char *prog_path;
static int crash_debug = 0;
#endif

static void exit_sighandler(int x) {
    static int sig_count=0;

    printf("%s:%d mplayer got signal:%d(%s) count:%d tid:%d\n", __func__, __LINE__, x, strsignal(x), sig_count, syscall(SYS_gettid));

    if (x == SIGPIPE)
    {
        printf("%s:%d mplayer ignore SIGPIPE\n", __func__, __LINE__);
        return;
    }
#ifdef CONFIG_CRASH_DEBUG
    if (!crash_debug || x != SIGTRAP)
#endif
        ++sig_count;

//	skyts_dump_to_file();
#ifdef SUPPORT_DIVX_DRM
   if (slow_down_speed)
   {
       skyfb_trick_play_workaround(0);
       slow_down_speed = 0;
   }
#endif /* end of SUPPORT_DIVX_DRM */   

//    if(initialized_flags==0 && sig_count>1) exit(1);
    if(sig_count>1)
    {
        mp_msg(MSGT_CPLAYER,MSGL_FATAL,"\n" MSGTR_IntBySignal,x,
           current_module?current_module:"unknown"
          );
        printf("MPlayer: force exit(1)\n");
        getch2_disable();
        exit(1);
    }

//Robert 20100601 check stream_cache & do cache_uninit()
    if (stream_cache_size>0 && (mpctx->stream) && mpctx->stream->cache_pid > 0)
    {
        printf("=== mpctx->stream->cache_pid=%d stream_cache_size=%d ==\n", mpctx->stream->cache_pid, stream_cache_size);
        cache_uninit(mpctx->stream);
    }

    if (mpctx->sh_audio)
    {
        uninit_player(INITIALIZED_ACODEC|INITIALIZED_AO);
    }
    if (mpctx->sh_video)
    {
        uninit_player(INITIALIZED_VCODEC|INITIALIZED_VO);
    }
    if(sig_count==5)
    {
        /* We're crashing bad and can't uninit cleanly :(
         * by popular request, we make one last (dirty)
         * effort to restore the user's
         * terminal. */
        getch2_disable();
        exit(1);
    }
    if(sig_count==6) exit(1);
    if(sig_count>6) {
        // can't stop :(
#ifndef __MINGW32__
        kill(getpid(),SIGKILL);
#endif
    }
    mp_msg(MSGT_CPLAYER,MSGL_FATAL,"\n" MSGTR_IntBySignal,x,
           current_module?current_module:"unknown"
          );
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SIGNAL=%d\n", x);
#ifdef USE_INSTRUMENT_FUNCTIONS
    if (x==3 || x == SIGSEGV || x == SIGKILL)
    {
        int i;
        printf("\n========================================\n");
        for (i=0; i<END_ADDR - START_ADDR; i+=4)
        {
            if (g_mplayer_func_parent_list[i] != 0 || g_mplayer_func_child_list[i] != 0)
            {
                printf("P Addr: 0x%.8x (%d)   C Addr: 0x%.8x (%d)\n", i+0xb000, g_mplayer_func_parent_list[i], i+0xb000, g_mplayer_func_child_list[i]);
            }

            g_mplayer_func_parent_list[i] = 0;
            g_mplayer_func_child_list[i] = 0;
        }
    }
#endif

    if(sig_count<=1)
        switch(x) {
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
        case SIGKILL:
            async_quit_request = 1;
            return;  // killed from keyboard (^C) or killed [-9]
        case SIGILL:
#if CONFIG_RUNTIME_CPUDETECT
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGILL_RTCpuSel);
#else
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGILL);
#endif
        case SIGFPE:
        case SIGSEGV:
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGSEGV_SIGFPE);
        default:
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGCRASH);
#ifdef CONFIG_CRASH_DEBUG
            if (crash_debug) {
                int gdb_pid;
                mp_msg(MSGT_CPLAYER, MSGL_INFO, "Forking...\n");
                gdb_pid = fork();
                mp_msg(MSGT_CPLAYER, MSGL_INFO, "Forked...\n");
                if (gdb_pid == 0) { // We are the child
                    char spid[20];
                    snprintf(spid, sizeof(spid), "%i", getppid());
                    getch2_disable(); // allow terminal to work properly with gdb
                    if (execlp("gdb", "gdb", prog_path, spid, "-ex", "bt", NULL) == -1)
                        mp_msg(MSGT_CPLAYER, MSGL_ERR, "Couldn't start gdb\n");
                } else if (gdb_pid < 0)
                    mp_msg(MSGT_CPLAYER, MSGL_ERR, "Couldn't fork\n");
                else {
                    waitpid(gdb_pid, NULL, 0);
                }
                if (x == SIGTRAP) return;
            }
#endif
        }
    getch2_disable();
    exit(1);
}

//#include "cfg-mplayer.h"

static void parse_cfgfiles( m_config_t* conf )
{
    char *conffile;
    int conffile_fd;
    if (!disable_system_conf &&
            m_config_parse_config_file(conf, MPLAYER_CONFDIR "/mplayer.conf") < 0)
        exit_player(EXIT_NONE);
    if ((conffile = get_path("")) == NULL) {
        mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_NoHomeDir);
    } else {
#ifdef __MINGW32__
        mkdir(conffile);
#else
        mkdir(conffile, 0777);
#endif
        free(conffile);
        if ((conffile = get_path("config")) == NULL) {
            mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_GetpathProblem);
        } else {
            if ((conffile_fd = open(conffile, O_CREAT | O_EXCL | O_WRONLY, 0666)) != -1) {
                mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_CreatingCfgFile, conffile);
                write(conffile_fd, default_config, strlen(default_config));
                close(conffile_fd);
            }
            if (!disable_user_conf &&
                    m_config_parse_config_file(conf, conffile) < 0)
                exit_player(EXIT_NONE);
            free(conffile);
        }
    }
}

#define PROFILE_CFG_PROTOCOL "protocol."

static void load_per_protocol_config (m_config_t* conf, const char *const file)
{
    char *str;
    char protocol[strlen (PROFILE_CFG_PROTOCOL) + strlen (file) + 1];
    m_profile_t *p;

    /* does filename actually uses a protocol ? */
    str = strstr (file, "://");
    if (!str)
        return;

    sprintf (protocol, "%s%s", PROFILE_CFG_PROTOCOL, file);
    protocol[strlen (PROFILE_CFG_PROTOCOL)+strlen(file)-strlen(str)] = '\0';
    p = m_config_get_profile (conf, protocol);
    if (p)
    {
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingProtocolProfile, protocol);
        m_config_set_profile(conf,p);
    }
}

#define PROFILE_CFG_EXTENSION "extension."

static void load_per_extension_config (m_config_t* conf, const char *const file)
{
    char *str;
    char extension[strlen (PROFILE_CFG_EXTENSION) + 8];
    m_profile_t *p;

    /* does filename actually have an extension ? */
    str = strrchr (filename, '.');
    if (!str)
        return;

    sprintf (extension, PROFILE_CFG_EXTENSION);
    strncat (extension, ++str, 7);
    p = m_config_get_profile (conf, extension);
    if (p)
    {
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingExtensionProfile, extension);
        m_config_set_profile(conf,p);
    }
}

#define PROFILE_CFG_VO "vo."
#define PROFILE_CFG_AO "ao."

static void load_per_output_config (m_config_t* conf, char *cfg, char *out)
{
    char profile[strlen (cfg) + strlen (out) + 1];
    m_profile_t *p;

    sprintf (profile, "%s%s", cfg, out);
    p = m_config_get_profile (conf, profile);
    if (p)
    {
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingExtensionProfile, profile);
        m_config_set_profile(conf,p);
    }
}

/**
 * Tries to load a config file
 * @return 0 if file was not found, 1 otherwise
 */
static int try_load_config(m_config_t *conf, const char *file)
{
    struct stat st;
    if (stat(file, &st))
        return 0;
    mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingConfig, file);
    m_config_parse_config_file (conf, file);
    return 1;
}

static void load_per_file_config (m_config_t* conf, const char *const file)
{
    char *confpath;
    char cfg[PATH_MAX];
    char *name;

    if (strlen(file) > PATH_MAX - 14) {
        mp_msg(MSGT_CPLAYER, MSGL_WARN, "Filename is too long, can not load file or directory specific config files\n");
        return;
    }
    sprintf (cfg, "%s.conf", file);

    name = strrchr(cfg, '/');
    if (HAVE_DOS_PATHS) {
        char *tmp = strrchr(cfg, '\\');
        if (!name || tmp > name)
            name = tmp;
        tmp = strrchr(cfg, ':');
        if (!name || tmp > name)
            name = tmp;
    }
    if (!name)
        name = cfg;
    else
        name++;

    if (use_filedir_conf) {
        char dircfg[PATH_MAX];
        strcpy(dircfg, cfg);
        strcpy(dircfg + (name - cfg), "mplayer.conf");
        try_load_config(conf, dircfg);

        if (try_load_config(conf, cfg))
            return;
    }

    if ((confpath = get_path (name)) != NULL)
    {
        try_load_config(conf, confpath);

        free (confpath);
    }
}

/* When libmpdemux performs a blocking operation (network connection or
 * cache filling) if the operation fails we use this function to check
 * if it was interrupted by the user.
 * The function returns a new value for eof. */
static int libmpdemux_was_interrupted(int eof) {
    mp_cmd_t* cmd;
    if((cmd = mp_input_get_cmd(0,0,0)) != NULL) {
        switch(cmd->id) {
        case MP_CMD_QUIT:
            exit_player_with_rc(EXIT_QUIT, (cmd->nargs > 0)? cmd->args[0].v.i : 0);
        case MP_CMD_PLAY_TREE_STEP: {
            eof = (cmd->args[0].v.i > 0) ? PT_NEXT_ENTRY : PT_PREV_ENTRY;
            mpctx->play_tree_step = (cmd->args[0].v.i == 0) ? 1 : cmd->args[0].v.i;
        }
        break;
        case MP_CMD_PLAY_TREE_UP_STEP: {
            eof = (cmd->args[0].v.i > 0) ? PT_UP_NEXT : PT_UP_PREV;
        }
        break;
        case MP_CMD_PLAY_ALT_SRC_STEP: {
            eof = (cmd->args[0].v.i > 0) ?  PT_NEXT_SRC : PT_PREV_SRC;
        }
        break;
        }
        mp_cmd_free(cmd);
    }
    return eof;
}

#define mp_basename(s) (strrchr(s,'\\')==NULL?(mp_basename2(s)):(strrchr(s,'\\')+1))

static int playtree_add_playlist(play_tree_t* entry)
{
    play_tree_add_bpf(entry,filename);

#ifdef CONFIG_GUI
    if (use_gui) {
        if (entry) {
            import_playtree_playlist_into_gui(entry, mconfig);
            play_tree_free_list(entry,1);
        }
    } else
#endif
    {
        if(!entry) {
            entry = mpctx->playtree_iter->tree;
            if(play_tree_iter_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
                return PT_NEXT_ENTRY;
            }
            if(mpctx->playtree_iter->tree == entry ) { // Loop with a single file
                if(play_tree_iter_up_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
                    return PT_NEXT_ENTRY;
                }
            }
            play_tree_remove(entry,1,1);
            return PT_NEXT_SRC;
        }
        play_tree_insert_entry(mpctx->playtree_iter->tree,entry);
        play_tree_set_params_from(entry,mpctx->playtree_iter->tree);
        entry = mpctx->playtree_iter->tree;
        if(play_tree_iter_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
            return PT_NEXT_ENTRY;
        }
        play_tree_remove(entry,1,1);
    }
    return PT_NEXT_SRC;
}

void add_subtitles(char *filename, float fps, int noerr)
{
    sub_data *subd;
#ifdef CONFIG_ASS
    ass_track_t *asst = 0;
#endif

    if (filename == NULL || mpctx->set_of_sub_size >= MAX_SUBTITLE_FILES) {
        return;
    }

    subd = sub_read_file(filename, fps);
#ifdef CONFIG_ASS
    if (ass_enabled)
#ifdef CONFIG_ICONV
        asst = ass_read_stream(ass_library, filename, sub_cp);
#else
        asst = ass_read_stream(ass_library, filename, 0);
#endif
    if (ass_enabled && subd && !asst)
        asst = ass_read_subdata(ass_library, subd, fps);

    if (!asst && !subd)
#else
    if(!subd)
#endif
        mp_msg(MSGT_CPLAYER, noerr ? MSGL_WARN : MSGL_ERR, MSGTR_CantLoadSub,
               filename_recode(filename));

#ifdef CONFIG_ASS
    if (!asst && !subd) return;
    mpctx->set_of_ass_tracks[mpctx->set_of_sub_size] = asst;
#else
    if (!subd) return;
#endif
    mpctx->set_of_subtitles[mpctx->set_of_sub_size] = subd;
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_FILE_SUB_ID=%d\n", mpctx->set_of_sub_size);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_FILE_SUB_FILENAME=%s\n",
           filename_recode(filename));
    ++mpctx->set_of_sub_size;
    mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AddedSubtitleFile, mpctx->set_of_sub_size,
           filename_recode(filename));
}

// FIXME: if/when the GUI calls this, global sub numbering gets (potentially) broken.
void update_set_of_subtitles(void)
// subdata was changed, set_of_sub... have to be updated.
{
    sub_data ** const set_of_subtitles = mpctx->set_of_subtitles;
    int i;
    if (mpctx->set_of_sub_size > 0 && subdata == NULL) { // *subdata was deleted
        for (i = mpctx->set_of_sub_pos + 1; i < mpctx->set_of_sub_size; ++i)
            set_of_subtitles[i-1] = set_of_subtitles[i];
        set_of_subtitles[mpctx->set_of_sub_size-1] = NULL;
        --mpctx->set_of_sub_size;
        if (mpctx->set_of_sub_size > 0) subdata = set_of_subtitles[mpctx->set_of_sub_pos=0];
    }
    else if (mpctx->set_of_sub_size > 0 && subdata != NULL) { // *subdata was changed
        set_of_subtitles[mpctx->set_of_sub_pos] = subdata;
    }
    else if (mpctx->set_of_sub_size <= 0 && subdata != NULL) { // *subdata was added
        set_of_subtitles[mpctx->set_of_sub_pos=mpctx->set_of_sub_size] = subdata;
        ++mpctx->set_of_sub_size;
    }
}

void init_vo_spudec(void) {
    if (vo_spudec)
        spudec_free(vo_spudec);
    initialized_flags &= ~INITIALIZED_SPUDEC;
    vo_spudec = NULL;

    // we currently can't work without video stream
    if (!mpctx->sh_video)
        return;

    if (spudec_ifo) {
        unsigned int palette[16], width, height;
        current_module="spudec_init_vobsub";
        if (vobsub_parse_ifo(NULL,spudec_ifo, palette, &width, &height, 1, -1, NULL) >= 0)
            vo_spudec=spudec_new_scaled(palette, width, height, NULL, 0);
    }

#ifdef CONFIG_DVDREAD
    if (vo_spudec==NULL && mpctx->stream->type==STREAMTYPE_DVD) {
        current_module="spudec_init_dvdread";
        vo_spudec=spudec_new_scaled(((dvd_priv_t *)(mpctx->stream->priv))->cur_pgc->palette,
                                    mpctx->sh_video->disp_w, mpctx->sh_video->disp_h,
                                    NULL, 0);
    }
#endif

#ifdef CONFIG_DVDNAV
    if (vo_spudec==NULL && mpctx->stream->type==STREAMTYPE_DVDNAV) {
        unsigned int *palette = mp_dvdnav_get_spu_clut(mpctx->stream);
        current_module="spudec_init_dvdnav";
        vo_spudec=spudec_new_scaled(palette, mpctx->sh_video->disp_w, mpctx->sh_video->disp_h, NULL, 0);
    }
#endif

    if (vo_spudec==NULL) {
        sh_sub_t *sh = (sh_sub_t *)mpctx->d_sub->sh;
        current_module="spudec_init_normal";
        vo_spudec=spudec_new_scaled(NULL, mpctx->sh_video->disp_w, mpctx->sh_video->disp_h, sh->extradata, sh->extradata_len);
        spudec_set_font_factor(vo_spudec,font_factor);
    }

    if (vo_spudec!=NULL) {
        initialized_flags|=INITIALIZED_SPUDEC;
        mp_property_do("sub_forced_only", M_PROPERTY_SET, &forced_subs_only, mpctx);
    }
}

/*
 * In Mac OS X the SDL-lib is built upon Cocoa. The easiest way to
 * make it all work is to use the builtin SDL-bootstrap code, which
 * will be done automatically by replacing our main() if we include SDL.h.
 */
#if defined(__APPLE__) && defined(CONFIG_SDL)
#ifdef CONFIG_SDL_SDL_H
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#endif

/**
 * \brief append a formatted string
 * \param buf buffer to print into
 * \param pos position of terminating 0 in buf
 * \param len maximum number of characters in buf, not including terminating 0
 * \param format printf format string
 */
static void saddf(char *buf, unsigned *pos, int len, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    *pos += vsnprintf(&buf[*pos], len - *pos, format, va);
    va_end(va);
    if (*pos >= len ) {
        buf[len] = 0;
        *pos = len;
    }
}

/**
 * \brief append time in the hh:mm:ss.f format
 * \param buf buffer to print into
 * \param pos position of terminating 0 in buf
 * \param len maximum number of characters in buf, not including terminating 0
 * \param time time value to convert/append
 */
static void sadd_hhmmssf(char *buf, unsigned *pos, int len, float time) {
    long tenths = 10 * time;
    int f1 = tenths % 10;
    int ss = (tenths /  10) % 60;
    int mm = (tenths / 600) % 60;
    int hh = tenths / 36000;
    if (time <= 0) {
        saddf(buf, pos, len, "unknown");
        return;
    }
    if (hh > 0)
        saddf(buf, pos, len, "%2d:", hh);
    if (hh > 0 || mm > 0)
        saddf(buf, pos, len, "%02d:", mm);
    saddf(buf, pos, len, "%02d.%1d", ss, f1);
}

/**
 * \brief print the status line
 * \param a_pos audio position
 * \param a_v A-V desynchronization
 * \param corr amount out A-V synchronization
 */
static void print_status(float a_pos, float a_v, float corr)
{
    sh_video_t * const sh_video = mpctx->sh_video;
    int width;
    char *line;
    unsigned pos = 0;
    get_screen_size();
    if (screen_width > 0)
        width = screen_width;
    else
        width = 80;
#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(__OS2__)
    /* Windows command line is broken (MinGW's rxvt works, but we
     * should not depend on that). */
    width--;
#endif
    line = malloc(width + 1); // one additional char for the terminating null

    // Audio time
    if (mpctx->sh_audio) {
        saddf(line, &pos, width, "A:%6.1f Aq:%d ", a_pos, mpctx->d_audio->packs);
        if (!sh_video) {
            float len = demuxer_get_time_length(mpctx->demuxer);
            saddf(line, &pos, width, "(");
            sadd_hhmmssf(line, &pos, width, a_pos);
            saddf(line, &pos, width, ") of %.1f (", len);
            sadd_hhmmssf(line, &pos, width, len);
            saddf(line, &pos, width, ") ");
        }
    }

    // Video time
    if (sh_video)
        saddf(line, &pos, width, "V:%6.1f Vq:%d Sq:%d ", sh_video->pts, mpctx->d_video->packs, mpctx->d_sub->packs);

    // A-V sync
    if (mpctx->sh_audio && sh_video)
        saddf(line, &pos, width, "A-V:%7.3f ct:%7.3f ", a_v, corr);

    // Video stats
    if (sh_video)
        saddf(line, &pos, width, "%3d/%3d ",
              (int)sh_video->num_frames,
              (int)sh_video->num_frames_decoded);

    // CPU usage
    if (sh_video) {
        if (sh_video->timer > 0.5)
            saddf(line, &pos, width, "%2d%% %2d%% %4.1f%% ",
                  (int)(100.0*video_time_usage*playback_speed/(double)sh_video->timer),
                  (int)(100.0*video_read_time_usage*playback_speed/(double)sh_video->timer),		// Raymond 2009/08/03
//        (int)(100.0*vout_time_usage*playback_speed/(double)sh_video->timer),
                  (100.0*audio_time_usage*playback_speed/(double)sh_video->timer));
        else
            saddf(line, &pos, width, "??%% ??%% ??,?%% ");
    } else if (mpctx->sh_audio) {
        if (mpctx->delay > 0.5)
            saddf(line, &pos, width, "%4.1f%% ",
                  100.0*audio_time_usage/(double)mpctx->delay);
        else
            saddf(line, &pos, width, "??,?%% ");
    }

    // VO stats
    if (sh_video)
        saddf(line, &pos, width, "%d %d ", drop_frame_cnt, output_quality);

#ifdef CONFIG_STREAM_CACHE
    // cache stats
    if (stream_cache_size > 0)
        saddf(line, &pos, width, "%d%% ", cache_fill_status);
#endif

    // other
    if (playback_speed != 1)
        saddf(line, &pos, width, "%4.2fx ", playback_speed);

    // end
    if (erase_to_end_of_line) {
        line[pos] = 0;
        mp_msg(MSGT_STATUSLINE, MSGL_STATUS, "%s%s\r", line, erase_to_end_of_line);
    } else {
        memset(&line[pos], ' ', width - pos);
        line[width] = 0;
        mp_msg(MSGT_STATUSLINE, MSGL_STATUS, "%s\r", line);
    }
    free(line);
}

/**
 * \brief build a chain of audio filters that converts the input format
 * to the ao's format, taking into account the current playback_speed.
 * \param sh_audio describes the requested input format of the chain.
 * \param ao_data describes the requested output format of the chain.
 */
int build_afilter_chain(sh_audio_t *sh_audio, ao_data_t *ao_data)
{
    int new_srate;
    int result;
    if (!sh_audio)
    {
#ifdef CONFIG_GUI
        if (use_gui) guiGetEvent(guiSetAfilter, (char *)NULL);
#endif
        mpctx->mixer.afilter = NULL;
        return 0;
    }
    if(af_control_any_rev(sh_audio->afilter,
                          AF_CONTROL_PLAYBACK_SPEED | AF_CONTROL_SET,
                          &playback_speed)) {
        new_srate = sh_audio->samplerate;
    } else {
        new_srate = sh_audio->samplerate * playback_speed;
        if (new_srate != ao_data->samplerate) {
            // limits are taken from libaf/af_resample.c
            if (new_srate < 8000)
                new_srate = 8000;
            if (new_srate > 192000)
                new_srate = 192000;
            playback_speed = (float)new_srate / (float)sh_audio->samplerate;
        }
    }
    result =  init_audio_filters(sh_audio, new_srate,
                                 &ao_data->samplerate, &ao_data->channels, &ao_data->format);
    mpctx->mixer.afilter = sh_audio->afilter;
#ifdef CONFIG_GUI
    if (use_gui) guiGetEvent(guiSetAfilter, (char *)sh_audio->afilter);
#endif
    return result;
}


typedef struct mp_osd_msg mp_osd_msg_t;
struct mp_osd_msg {
    /// Previous message on the stack.
    mp_osd_msg_t* prev;
    /// Message text.
    char msg[128];
    int  id,level,started;
    /// Display duration in ms.
    unsigned  time;
};

/// OSD message stack.
static mp_osd_msg_t* osd_msg_stack = NULL;

/**
 *  \brief Add a message on the OSD message stack
 *
 *  If a message with the same id is already present in the stack
 *  it is pulled on top of the stack, otherwise a new message is created.
 *
 */

void set_osd_msg(int id, int level, int time, const char* fmt, ...) {
    mp_osd_msg_t *msg,*last=NULL;
    va_list va;
    int r;

    // look if the id is already in the stack
    for(msg = osd_msg_stack ; msg && msg->id != id ;
            last = msg, msg = msg->prev);
    // not found: alloc it
    if(!msg) {
        msg = calloc(1,sizeof(mp_osd_msg_t));
        msg->prev = osd_msg_stack;
        osd_msg_stack = msg;
    } else if(last) { // found, but it's not on top of the stack
        last->prev = msg->prev;
        msg->prev = osd_msg_stack;
        osd_msg_stack = msg;
    }
    // write the msg
    va_start(va,fmt);
    r = vsnprintf(msg->msg, 128, fmt, va);
    va_end(va);
    if(r >= 128) msg->msg[127] = 0;
    // set id and time
    msg->id = id;
    msg->level = level;
    msg->time = time;

}

/**
 *  \brief Remove a message from the OSD stack
 *
 *  This function can be used to get rid of a message right away.
 *
 */

void rm_osd_msg(int id) {
    mp_osd_msg_t *msg,*last=NULL;

    // Search for the msg
    for(msg = osd_msg_stack ; msg && msg->id != id ;
            last = msg, msg = msg->prev);
    if(!msg) return;

    // Detach it from the stack and free it
    if(last)
        last->prev = msg->prev;
    else
        osd_msg_stack = msg->prev;
    free(msg);
}

/**
 *  \brief Remove all messages from the OSD stack
 *
 */

static void clear_osd_msgs(void) {
    mp_osd_msg_t* msg = osd_msg_stack, *prev = NULL;
    while(msg) {
        prev = msg->prev;
        free(msg);
        msg = prev;
    }
    osd_msg_stack = NULL;
}

/**
 *  \brief Get the current message from the OSD stack.
 *
 *  This function decrements the message timer and destroys the old ones.
 *  The message that should be displayed is returned (if any).
 *
 */

static mp_osd_msg_t* get_osd_msg(void) {
    mp_osd_msg_t *msg,*prev,*last = NULL;
    static unsigned last_update = 0;
    unsigned now = GetTimerMS();
    unsigned diff;
    char hidden_dec_done = 0;

    if (osd_visible) {
        // 36000000 means max timed visibility is 1 hour into the future, if
        // the difference is greater assume it's wrapped around from below 0
        if (osd_visible - now > 36000000) {
            osd_visible = 0;
            vo_osd_progbar_type = -1; // disable
#if DONT_NEED_OSDTYPE
            vo_osd_changed(OSDTYPE_PROGBAR);
#endif
            if (mpctx->osd_function != OSD_PAUSE)
                mpctx->osd_function = OSD_PLAY;
        }
    }

    if(!last_update) last_update = now;
    diff = now >= last_update ? now - last_update : 0;

    last_update = now;

    // Look for the first message in the stack with high enough level.
    for(msg = osd_msg_stack ; msg ; last = msg, msg = prev) {
        prev = msg->prev;
        if(msg->level > osd_level && hidden_dec_done) continue;
        // The message has a high enough level or it is the first hidden one
        // in both cases we decrement the timer or kill it.
        if(!msg->started || msg->time > diff) {
            if(msg->started) msg->time -= diff;
            else msg->started = 1;
            // display it
            if(msg->level <= osd_level) return msg;
            hidden_dec_done = 1;
            continue;
        }
        // kill the message
        free(msg);
        if(last) {
            last->prev = prev;
            msg = last;
        } else {
            osd_msg_stack = prev;
            msg = NULL;
        }
    }
    // Nothing found
    return NULL;
}

/**
 * \brief Display the OSD bar.
 *
 * Display the OSD bar or fall back on a simple message.
 *
 */

void set_osd_bar(int type,const char* name,double min,double max,double val) {

    if(osd_level < 1) return;

    if(mpctx->sh_video) {
        osd_visible = (GetTimerMS() + 1000) | 1;
        vo_osd_progbar_type = type;
        vo_osd_progbar_value = 256*(val-min)/(max-min);
#if DONT_NEED_OSDTYPE
        vo_osd_changed(OSDTYPE_PROGBAR);
#endif
        return;
    }

    set_osd_msg(OSD_MSG_BAR,1,osd_duration,"%s: %d %%",
                name,ROUND(100*(val-min)/(max-min)));
}

/**
 * \brief Display text subtitles on the OSD
 */
void set_osd_subtitle(subtitle *subs) {
    int i;
    vo_sub = subs;
    vo_osd_changed(OSDTYPE_SUBTITLE);
    if (!mpctx->sh_video) {
        // reverse order, since newest set_osd_msg is displayed first
        for (i = SUB_MAX_TEXT - 1; i >= 0; i--) {
            if (!subs || i >= subs->lines || !subs->text[i])
                rm_osd_msg(OSD_MSG_SUB_BASE + i);
            else {
                // HACK: currently display time for each sub line except the last is set to 2 seconds.
                int display_time = i == subs->lines - 1 ? 180000 : 2000;
                set_osd_msg(OSD_MSG_SUB_BASE + i, 1, display_time, "%s", subs->text[i]);
            }
        }
    }
}

/**
 * \brief Update the OSD message line.
 *
 * This function displays the current message on the vo OSD or on the term.
 * If the stack is empty and the OSD level is high enough the timer
 * is displayed (only on the vo OSD).
 *
 */

static void update_osd_msg(void) {
    mp_osd_msg_t *msg;
    static char osd_text[128] = "";
    static char osd_text_timer[128];

    // we need some mem for vo_osd_text
    vo_osd_text = (unsigned char*)osd_text;

    // Look if we have a msg
    if((msg = get_osd_msg())) {
        if(strcmp(osd_text,msg->msg)) {
            strncpy((char*)osd_text, msg->msg, 127);
//            if(mpctx->sh_video) vo_osd_changed(OSDTYPE_OSD); else		//Fuchun 2010.06.15 disable, show information on console
            if(term_osd) mp_msg(MSGT_CPLAYER,MSGL_STATUS,"%s%s\n",term_osd_esc,msg->msg);
        }
        return;
    }

    if(mpctx->sh_video) {
        // fallback on the timer
        if(osd_level>=2) {
            int len = demuxer_get_time_length(mpctx->demuxer);
            int percentage = -1;
            char percentage_text[10];
            int pts = demuxer_get_current_time(mpctx->demuxer);

            if (mpctx->osd_show_percentage)
                percentage = demuxer_get_percent_pos(mpctx->demuxer);

            if (percentage >= 0)
                snprintf(percentage_text, 9, " (%d%%)", percentage);
            else
                percentage_text[0] = 0;

            if (osd_level == 3)
                snprintf(osd_text_timer, 63,
                         "%c %02d:%02d:%02d / %02d:%02d:%02d%s",
                         mpctx->osd_function,pts/3600,(pts/60)%60,pts%60,
                         len/3600,(len/60)%60,len%60,percentage_text);
            else
                snprintf(osd_text_timer, 63, "%c %02d:%02d:%02d%s",
                         mpctx->osd_function,pts/3600,(pts/60)%60,
                         pts%60,percentage_text);
        } else
            osd_text_timer[0]=0;

        // always decrement the percentage timer
        if(mpctx->osd_show_percentage)
            mpctx->osd_show_percentage--;

        if(strcmp(osd_text,osd_text_timer)) {
            strncpy(osd_text, osd_text_timer, 63);
#if DONT_NEED_OSDTYPE
            vo_osd_changed(OSDTYPE_OSD);
#endif
        }
        return;
    }

    // Clear the term osd line
    if(term_osd && osd_text[0]) {
        osd_text[0] = 0;
        printf("%s\n",term_osd_esc);
    }
}

///@}
// OSDMsgStack
//Barry 2011-06-29
short no_audio_channels = 0;
int no_audio_samplerate = 0;
int no_audio_format = 0;

void reinit_audio_chain(void) {
    if (!mpctx->sh_audio)
        return;

    no_audio_channels = 0;
    no_audio_samplerate = 0;
    no_audio_format = 0;

    current_module="init_audio_codec";
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
    if(!init_best_audio_codec(mpctx->sh_audio,audio_codec_list,audio_fm_list)) {
        goto init_error;
    }
    initialized_flags|=INITIALIZED_ACODEC;
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
   //Polun 2011-06-01 ++s
   if (skyfb_fd == -1)
    {
	if( skydroid == 0 )
	{	
		skyfb_fd = open("/dev/fb0", O_RDWR);
		printf("open /dev/fb\n");
	}
	else
	{
		skyfb_fd = open("/dev/graphics/fb0", O_RDWR);
		printf("open /dev/graphics/fb\n");
	}
       resampleao_fbflg = 1; //Polun 2011-06-13 modify resampleao_fbflg 
    }
    struct hdmiSupportAudioInfo_t hdmisupaf;
    int numcount;
    int hdmisupportmax = force_srate;
    hdmisupaf.bNum = 0;
    ioctl(skyfb_fd, SKYFB_HDMI_GET_AUDIO_INFO, &hdmisupaf);
    for(numcount = 0 ; numcount < hdmisupaf.bNum; numcount++ )
    {
    //printf("!!!!!!!!  reinit_audio_chain  [%s - %d]  bNum = %d  bType = 0x%02x,bSRate = 0x%02x,bRes = 0x%02x \n", __func__, __LINE__,numcount,hdmisupaf.bAudInfo[numcount].bType,hdmisupaf.bAudInfo[numcount].bSRate,hdmisupaf.bAudInfo[numcount].bRes);
        if(hdmisupaf.bAudInfo[numcount].bSRate > 0x3f)
        {
            hdmisupportmax = 192000;
        }
         else if(hdmisupaf.bAudInfo[numcount].bSRate > 0x1f)
        {
        	if (hdmisupportmax < 176400)
			hdmisupportmax = 176400;
        }
         else if(hdmisupaf.bAudInfo[numcount].bSRate > 0x0f)
        {
        	if (hdmisupportmax < 96000)
			hdmisupportmax = 96000;
        }
         else if(hdmisupaf.bAudInfo[numcount].bSRate > 0x07)
        {
        	if (hdmisupportmax < 88200)
			hdmisupportmax = 88200;
        }
         else if(hdmisupaf.bAudInfo[numcount].bSRate > 0x03)
        {
        	if (hdmisupportmax < 48000)
			hdmisupportmax = 48000;
        }
        else if(hdmisupaf.bAudInfo[numcount].bSRate > 0x01)
        {
        	if (hdmisupportmax < 44100)
			hdmisupportmax = 44100;
        }
        else
        {
        	if (hdmisupportmax < 32000)
			hdmisupportmax = 32000;
        }
    printf(" hdmi audio support max sample rate = %d\n",hdmisupportmax);
    }
   //Polun 2011-06-01 ++e
    current_module="af_preinit";
    //Barry 2010-09-17
    if (mpctx->sh_audio->samplerate < 32000)
        force_srate = 32000;
     //Polun 2011-06-01 ++s
    if ( (!mpctx->sh_video) &&(mpctx->sh_audio->samplerate > hdmisupportmax))
        force_srate = hdmisupportmax;
     //Polun 2011-06-01 ++e

    ao_data.samplerate=force_srate;
    ao_data.channels=0;
    ao_data.format=audio_output_format;
    // first init to detect best values
    if(!init_audio_filters(mpctx->sh_audio,   // preliminary init
                           // input:
                           mpctx->sh_audio->samplerate,
                           // output:
                           &ao_data.samplerate, &ao_data.channels, &ao_data.format)) {
        mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_AudioFilterChainPreinitError);
        exit_player(EXIT_ERROR);
    }

    if(!(initialized_flags&INITIALIZED_AO))	//Fuchun 2011.01.25 add condition
    {
        current_module="ao2_init";
        mpctx->audio_out = init_best_audio_out(audio_driver_list,
                                               0, // plugin flag
                                               ao_data.samplerate,
                                               ao_data.channels,
                                               ao_data.format, 0);
        if(!mpctx->audio_out) {
            mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CannotInitAO);
            goto init_error;
        }
        initialized_flags|=INITIALIZED_AO;
        mp_msg(MSGT_CPLAYER,MSGL_INFO,"AO: [%s] %dHz %dch %s (%d bytes per sample)\n",
               mpctx->audio_out->info->short_name,
               ao_data.samplerate, ao_data.channels,
               af_fmt2str_short(ao_data.format),
               af_fmt2bits(ao_data.format)/8 );
        mp_msg(MSGT_CPLAYER,MSGL_V,"AO: Description: %s\nAO: Author: %s\n",
               mpctx->audio_out->info->name, mpctx->audio_out->info->author);
        if(strlen(mpctx->audio_out->info->comment) > 0)
            mp_msg(MSGT_CPLAYER,MSGL_V,"AO: Comment: %s\n", mpctx->audio_out->info->comment);
    }

    // init audio filters:
    current_module="af_init";
    if(!build_afilter_chain(mpctx->sh_audio, &ao_data)) {
        mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_NoMatchingFilter);
        goto init_error;
    }
    mpctx->mixer.audio_out = mpctx->audio_out;
    mpctx->mixer.volstep = volstep;
    return;

init_error:
    if (mpctx->sh_audio && mpctx->sh_audio->wf)
    {
    	no_audio_channels = mpctx->sh_audio->wf->nChannels;
    	no_audio_samplerate = mpctx->sh_audio->samplerate;
    	no_audio_format = mpctx->sh_audio->format;
    }
    if (!no_audio_channels && mpctx->sh_audio->channels)
	no_audio_channels = mpctx->sh_audio->channels;
    if (!no_audio_samplerate && mpctx->sh_audio->samplerate)
	no_audio_samplerate = mpctx->sh_audio->samplerate;
    if (!no_audio_format && mpctx->sh_audio->format)
	no_audio_format = mpctx->sh_audio->format;

    uninit_player(INITIALIZED_ACODEC|INITIALIZED_AO); // close codec and possibly AO
    mpctx->sh_audio=mpctx->d_audio->sh=NULL; // -> nosound
    mpctx->d_audio->id = -2;
}


///@}
// Command2Property


// Return pts value corresponding to the end point of audio written to the
// ao so far.
static double written_audio_pts(sh_audio_t *sh_audio, demux_stream_t *d_audio)
{
    double buffered_output;
    // first calculate the end pts of audio that has been output by decoder
    double a_pts = sh_audio->pts;
//if (a_pts == MP_NOPTS_VALUE)
//printf("sh_audio->pts=%f\n", sh_audio->pts);
    //Polun 2011-11-18 ++s fixed mantis 6542 fansub-gits.2.0.720p.bluray.mkv sh_audio->samplesize = 0 cause UI timeout(Encountered error in file).
    if ((!sh_audio->o_bps) && (sh_audio->samplesize == 0) && (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA))
    {
       sh_audio->samplesize = 2;
	sh_audio->o_bps = sh_audio->channels * sh_audio->samplerate * sh_audio->samplesize;
    }
    //Polun 2011-11-18 ++e
    if (a_pts != MP_NOPTS_VALUE)
        // Good, decoder supports new way of calculating audio pts.
        // sh_audio->pts is the timestamp of the latest input packet with
        // known pts that the decoder has decoded. sh_audio->pts_bytes is
        // the amount of bytes the decoder has written after that timestamp.
        a_pts += sh_audio->pts_bytes / (double) sh_audio->o_bps;
    else {
        // Decoder doesn't support new way of calculating pts (or we're
        // being called before it has decoded anything with known timestamp).
        // Use the old method of audio pts calculation: take the timestamp
        // of last packet with known pts the decoder has read data from,
        // and add amount of bytes read after the beginning of that packet
        // divided by input bps. This will be inaccurate if the input/output
        // ratio is not constant for every audio packet or if it is constant
        // but not accurately known in sh_audio->i_bps.

        a_pts = d_audio->pts;
        // ds_tell_pts returns bytes read after last timestamp from
        // demuxing layer, decoder might use sh_audio->a_in_buffer for bytes
        // it has read but not decoded
        if (sh_audio->i_bps)
            a_pts += (ds_tell_pts(d_audio) - sh_audio->a_in_buffer_len) /
                     (double)sh_audio->i_bps;
    }
    // Now a_pts hopefully holds the pts for end of audio from decoder.
    // Substract data in buffers between decoder and audio out.

    // Decoded but not filtered
    a_pts -= sh_audio->a_buffer_len / (double)sh_audio->o_bps;

    // Data buffered in audio filters, measured in bytes of "missing" output
    buffered_output = af_calc_delay(sh_audio->afilter);

    // Data that was ready for ao but was buffered because ao didn't fully
    // accept everything to internal buffers yet
#ifdef NEW_AUDIO_FILL_BUFFER
#ifdef QT_SUPPORT_DVBT
    if(mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
    {
        buffered_output += sh_audio->a_out_buffer_len;
    }
    else
#endif /* end of QT_SUPPORT_DVBT */	
    {
        int r_len = sh_audio->r_a_out_buffer_len, w_len = sh_audio->w_a_out_buffer_len, total_out_size = sh_audio->a_out_buffer_size;
        buffered_output += w_len < r_len ? AUDIO_OUT_QUEUE_NUM*total_out_size - r_len + w_len : w_len - r_len;
    }
#else
    buffered_output += sh_audio->a_out_buffer_len;
#endif

    // Filters divide audio length by playback_speed, so multiply by it
    // to get the length in original units without speedup or slowdown
    a_pts -= buffered_output * playback_speed / ao_data.bps;
    /*
    printf("=== apts: %f  pts_bytes:%d (+%f)  abl:%d (%f) ao:%d a_pts:%f real:%f\n",
    sh_audio->pts, sh_audio->pts_bytes, (sh_audio->pts) + (sh_audio->pts_bytes)/(double) sh_audio->o_bps,
    sh_audio->a_buffer_len, (sh_audio->pts) + (sh_audio->pts_bytes - sh_audio->a_buffer_len)/(double) sh_audio->o_bps,
    sh_audio->a_out_buffer_len, a_pts,  a_pts - playback_speed * mpctx->audio_out->get_delay());
    */
    return a_pts;
}

// Return pts value corresponding to currently playing audio.
double playing_audio_pts(sh_audio_t *sh_audio, demux_stream_t *d_audio,
                         const ao_functions_t *audio_out)
{
    return written_audio_pts(sh_audio, d_audio) - playback_speed *
           audio_out->get_delay();
}

static int check_framedrop(double frame_time) {
    // check for frame-drop:

    current_module = "check_framedrop";

    //if (frame_drop_lock == 1)
    if (frame_drop_lock == 1 || switch_audio_thread_status == 1 || switch_audio_thread_status == 2 || !(wait_video_or_audio_sync&AVSYNC_NORMAL))
        return 0;

    if (mpctx->d_audio->pts == MP_NOPTS_VALUE || mpctx->d_video->pts == MP_NOPTS_VALUE)
    {
        return 0;
    }

#ifdef QT_SUPPORT_DVBT
    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS || quick_bd ==1)
#else /* else of QT_SUPPORT_DVBT */
    if (quick_bd ==1)
#endif /* end of QT_SUPPORT_DVBT */
    {
		if (sky_h264_need_check_dropB)
		{
			if (AV_delay > 1.2)
			{
				//            printf("TS need sync %f\n", (float)AV_delay);
				wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
				return 0;
			}
			else if (AV_delay < -0.15)
			{
				//            printf("TS need sync %f\n", (float)AV_delay);
				wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO;
				return 0;
			}
			
		}
		else
		{
			if (AV_delay > 0.2)
			{
				//            printf("TS need sync %f\n", (float)AV_delay);
				wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
				return 0;
			}
			else if (AV_delay < -0.15)
			{
				//            printf("TS need sync %f\n", (float)AV_delay);
				wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO;
				return 0;
			}
		}
    }


    if (mpctx->sh_audio && !mpctx->d_audio->eof && mpctx->audio_out) {
        static int dropped_frames = 0;
        float delay = playback_speed*mpctx->audio_out->get_delay();
        float d = delay-mpctx->delay;
        float need_drop_range;
        float need_sync_range;

//	    ++total_frame_cnt;
        // we should avoid dropping too many frames in sequence unless we
        // are too late. and we allow 100ms A-V delay here:
#if 1
        if (d < -dropped_frames*frame_time-0.100 &&
#else
        if (sky_vdec_wait_sync_vpts > 0.0)
        {
            check_framedrop_flag = 0;
            return 0;
        }
        else if ((AV_delay - dropped_frames*frame_time) > 0.06 &&
#endif
                mpctx->osd_function != OSD_PAUSE) {

            if (h264_frame_mbs_only == 1)
            {
                if (sky_nrb.qlen[2] == 0)
                    need_drop_range = 0.02;//0.10;
                else
                    need_drop_range = 0.06;//1;//0.20;
            }
            else
            {
                if (sky_nrb.qlen[2] == 0)
                    need_drop_range = 0.06;//0.06;//0.15;
                else
                    need_drop_range = 0.12;//0.1;//0.26;
            }
            need_sync_range = need_drop_range*20;

//            if ((mpctx->sh_video->disp_w >= 1920) && (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA || mpctx->demuxer->type == DEMUXER_TYPE_MOV || mpctx->demuxer->type == DEMUXER_TYPE_AVI))
            if (sky_h264_need_check_dropB == 1)
            {
                need_sync_range = 1.2;
            }

            if(AV_delay > need_sync_range && seek_sync_flag == 0 && sky_vdec_wait_sync_vpts == 0.0)
            {
//printf("\nMPlayer:  set wait_video_or_audio_sync=1, AV_delay=%f dropped_frames=%d need_sync_range=%f qlen[2]=%d\n", AV_delay, dropped_frames, need_sync_range, sky_nrb.qlen[2]);
                ++drop_frame_cnt;
                ++dropped_frames;
                wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                return 0;
            }
            else
//		if ((sky_nrb.qlen[2] == 0 && AV_delay > need_drop_range) || (sky_nrb.qlen[2] > 0 && AV_delay > need_drop_range))
//		if (AV_delay+(dropped_frames*frame_time) > need_drop_range)
//Robert 20101118  need - queued frames for drop checking
                if (AV_delay+((dropped_frames-sky_nrb.qlen[2])*frame_time) > need_drop_range)
//		if (AV_delay > need_drop_range)
                {
                    ++drop_frame_cnt;
                    ++dropped_frames;
//			check_framedrop_flag = 1;
//			wait_video_or_audio_sync = 1;	//Fuchun 2010.11.04
//printf("\nMPlayer:  set wait_video_or_audio_sync=1, AV_delay=%f dropped_frames=%d need_drop_range=%f qlen[2]=%d\n", AV_delay, dropped_frames, need_drop_range, sky_nrb.qlen[2]);
                    check_framedrop_flag = 0;
                    return frame_dropping;
                }
                else
                {
                    dropped_frames = 0;
                }
        } else
        {
            dropped_frames = 0;
        }
    }
    return 0;
}

static int generate_video_frame(sh_video_t *sh_video, demux_stream_t *d_video)
{
    unsigned char *start;
    int in_size;
    int hit_eof=0;
    double pts, pts1;
    demuxer_t *demuxer=d_video->demuxer;
    static int found_Iframe = 0;	//Fuchun 20110915	

    extern int uvcnt;
    uvcnt++;
    if(!uvcnt) printf("$$$$$$$$$$$$$$$$$$ generate_video_frame $$$$$$$$$$$$$$$$$$$$$\n");

    while (1) {
        int drop_frame = 0;
        if(speed_mult == 0)
            drop_frame = check_framedrop(sh_video->frametime);
        void *decoded_frame;
        current_module = "decode video";
        // XXX Time used in this call is not counted in any performance
        // timer now, OSD is not updated correctly for filter-added frames
        if (vf_output_queued_frame(sh_video->vfilter))
            break;
        current_module = "video_read_frame";

        pts1 = d_video->pts;
        in_size = ds_get_packet_pts(d_video, &start, &pts);

//Robert 20100615 update next_frame_time for display queue
        /*	//Fuchun 2010.07.15 disable, move to decode_video
        	if (pts1>0 && pts>0 && pts > pts1)
        	{
        	    sh_video->next_frame_time = pts - pts1;
                }
        */

        if (in_size < 0) {
            // try to extract last frames in case of decoder lag
            in_size = 0;
            pts = 1e300;
            //hit_eof = 1;
            //Polun 2011-08-02 ++s  mantis 5136
            if( demuxer->file_format == DEMUXER_TYPE_LAVF_PREFERRED && sh_video->format  == mmioFOURCC('H','2','6','4') &&  hit_eof_cunt < 2)
            {
                hit_eof_cunt += 1;
            }
            else
            {
                hit_eof = 1;
                hit_eof_cunt = 0 ;
            } 
            //Polun 2011-08-02 ++e
        }
        if (in_size > max_framesize)
            max_framesize = in_size;

#if 1	// Raymond 2010/03/16
	if( demuxer->file_format==DEMUXER_TYPE_MATROSKA)	// Raymond 2009/05/11
	{
		if(sh_video->format == mmioFOURCC('a','v','c','1') )
		{
			int k = 0;
			int len = 0, len_3byte = 0;
			unsigned char *ps = start;

#if 1	//Barry 2010-07-09
			for( k = 0 ; k < in_size ; k += 4)
			{
				len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
//				printf("len = %d\n", len);
				if ( ((k+len) > in_size) && avc1_nal_bytes == 3 )
				{
					len_3byte = ((int)ps[k] << 16) + ((int)ps[k+1] << 8) + ((int)ps[k+2]);
//					printf("in_size=%d, len=%d, len_3byte=%d\n", in_size, len, len_3byte);

					if ((k+len_3byte) <= in_size)
					{
						if((speed_mult < 0 || speed_mult >= 2) && !h264_frame_mbs_only)	//Fuchun 20110915
						{
							if(((ps[k+3]&~0x60) == 0x05) || ((ps[k+3]&~0x60) == 0x07))
							{
								found_Iframe = 1;
							}
						}
						ps[k] = ps[k+1] = 0;
						ps[k+2] = 0x1;
						k += (len_3byte - 1);
					}
				}
				else
				{
					if((speed_mult < 0 || speed_mult >= 2) && !h264_frame_mbs_only)	//Fuchun 20110915
					{
						if(((ps[k+4]&~0x60) == 0x05) || ((ps[k+4]&~0x60) == 0x07))
						{
							found_Iframe = 1;
						}
					}
					ps[k] = ps[k+1] = ps[k+2] = 0;
					ps[k+3] = 0x1;
					k += len;
				}
				if(len < 0) break;		//Fuchun 2010.02.23
			}
#else
			for( k = 0 ; k < in_size ; k += 4)
			{
				len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
//				printf("len = %d\n", len);
				ps[k] = ps[k+1] = ps[k+2] = 0;
				ps[k+3] = 0x1;
				if(len < 0) break;		//Fuchun 2010.02.23
				k += len;
			}
#endif
		}
	}
#endif

#if 1	//Fuchun 20110915
	if((speed_mult < 0 || speed_mult >= 2) && !h264_frame_mbs_only && (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA))
	{
		if(top_field_frame == 1 || found_Iframe == 0)
		{
			top_field_frame = 0;
			read_nextframe = 0;
		}
		else if(top_field_frame == 0)
		{
			top_field_frame = 1;
			read_nextframe = 1;
			found_Iframe = 0;
		}
	}
	else
	{
		top_field_frame = 1;
		read_nextframe = 1;
		found_Iframe = 0;
	}
#else
	read_nextframe = 1;		//Fuchun 2010.03.29
#endif

        current_module = "decode video";
        decoded_frame = decode_video(sh_video, start, in_size, drop_frame, pts);
        //Polun 2011-06-08 ++s  
        if((speed_mult == 0) && ( demuxer->file_format==DEMUXER_TYPE_MATROSKA) && (sh_video->fps == 25) && ((sh_video->next_frame_time < 0.002) || (sh_video->next_frame_time > 0.070)))
        {
              sh_video->next_frame_time = 0;
              printf("!!!!!!!!!!!!!next_frame_time not correction set it to 0\n");
        }
        //Polun 2011-06-08 ++e
#if 1	// Raymond 2010/03/18
//printf("vpts: %f  cur: %f   diff: %f\n", sh_video->pts, sky_nrb.cur_pts, sh_video->pts - sky_nrb.cur_pts);
        if (sky_nrb.cur_pts > 0.5 && sh_video->pts - sky_nrb.cur_pts < 1.0)
        {
            update_subtitles(sh_video, sky_nrb.cur_pts, mpctx->d_sub, 0);
        }
        else
        {
            update_subtitles(sh_video, sh_video->pts, mpctx->d_sub, 0);
        }
        update_teletext(sh_video, mpctx->demuxer, 0);
        update_osd_msg();
        if (hit_eof)
            return 0;
        else
            break;
#else
        if (decoded_frame) {
            update_subtitles(sh_video, sh_video->pts, mpctx->d_sub, 0);
            update_teletext(sh_video, mpctx->demuxer, 0);
            update_osd_msg();
            current_module = "filter video";
            if (filter_video(sh_video, decoded_frame, sh_video->pts))
                break;
        } else if (drop_frame)
            return -1;
        if (hit_eof)
            return 0;
#endif
    }
    return 1;
}

#ifdef HAVE_RTC
int rtc_fd = -1;
#endif

static float timing_sleep(float time_frame)
{
#ifdef HAVE_RTC
    if (rtc_fd >= 0) {
        // -------- RTC -----------
        current_module="sleep_rtc";
        while (time_frame > 0.000) {
            unsigned long rtc_ts;
            if (read(rtc_fd, &rtc_ts, sizeof(rtc_ts)) <= 0)
                mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_LinuxRTCReadError, strerror(errno));
            time_frame -= GetRelativeTime();
        }
    } else
#endif
    {
        // assume kernel HZ=100 for softsleep, works with larger HZ but with
        // unnecessarily high CPU usage
        float margin = softsleep ? 0.011 : 0;
        current_module = "sleep_timer";

        while (time_frame > margin) {
            usec_sleep(1000000 * (time_frame - margin));
            time_frame -= GetRelativeTime();
        }
        if (softsleep) {
            current_module = "sleep_soft";
            if (time_frame < 0)
                mp_msg(MSGT_AVSYNC, MSGL_WARN, MSGTR_SoftsleepUnderflow);
            while (time_frame > 0)
                time_frame-=GetRelativeTime(); // burn the CPU
        }
    }
    return time_frame;
}

static void select_subtitle(MPContext *mpctx, int is_dvd) {
    // find the best sub to use
    int vobsub_index_id = vobsub_get_index_by_id(vo_vobsub, vobsub_id);
    mpctx->global_sub_pos = -1; // no subs by default
    if (vobsub_index_id >= 0) {
        // if user asks for a vobsub id, use that first.
        mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_VOBSUB] + vobsub_index_id;
    } else if (dvdsub_id >= 0 && mpctx->global_sub_indices[SUB_SOURCE_DEMUX] >= 0) {
        // if user asks for a dvd sub id, use that next.
        mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + dvdsub_id;
    } else if (mpctx->global_sub_indices[SUB_SOURCE_SUBS] >= 0) {
        // if there are text subs to use, use those.  (autosubs come last here)
        mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_SUBS];
    } else if (dvdsub_id == -1 && mpctx->global_sub_indices[SUB_SOURCE_DEMUX] >= 0) {
        // finally select subs by language and container hints
        if (dvdsub_id == -1 && dvdsub_lang)
            dvdsub_id = demuxer_sub_track_by_lang(mpctx->demuxer, dvdsub_lang);
        if (dvdsub_id == -1)
        {
		if( skydroid == 0 && is_dvd == 0)	//Fuchun 20110810 no subtitle by default for QT
			dvdsub_id = -1;
		else
			dvdsub_id = demuxer_default_sub_track(mpctx->demuxer);
        }
        if (dvdsub_id >= 0)
            mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + dvdsub_id;
    }
    // rather than duplicate code, use the SUB_SELECT handler to init the right one.
#if 1	//Fuchun 2011.05.03 no subtitle by default for QT
	if( skydroid == 0 && is_dvd == 0)
	{
		if(select_sub_id == -1)
		mpctx->global_sub_pos=-1;
		else
		{
			mpctx->global_sub_pos = select_sub_id;
			select_sub_id = -1;
		}
		mp_property_do("sub", M_PROPERTY_SET, &mpctx->global_sub_pos, mpctx);
	}
	else
	{
		mpctx->global_sub_pos--;
		mp_property_do("sub",M_PROPERTY_STEP_UP,NULL, mpctx);
	}
#else
    mpctx->global_sub_pos--;
    mp_property_do("sub",M_PROPERTY_STEP_UP,NULL, mpctx);
#endif

    //Fuchun 2010.11.25
    char *ret = NULL;
    char callback_str[1024];
    mp_property_do("sub",M_PROPERTY_PRINT,&ret, mpctx);
    if(ret != NULL)
        free(ret);
    {
        if(sub_change_success == 1)
            sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
        else if(sub_change_success == 2)
            sprintf(callback_str, "SUBTITLE: partial %s\0", subtitle_names);
        else
            sprintf(callback_str, "SUBTITLE: %s\0", subtitle_names);

        if (mpctx->stream->type !=STREAMTYPE_DVDNAV )
        ipc_callback(callback_str);
        sub_change_success = 0;
    }
}

#ifdef CONFIG_DVDNAV
#ifndef FF_B_TYPE
#define FF_B_TYPE 3
#endif
/// store decoded video image
static mp_image_t * mp_dvdnav_copy_mpi(mp_image_t *to_mpi,
                                       mp_image_t *from_mpi) {
    mp_image_t *mpi;

    if (!from_mpi) return NULL;
    /// Do not store B-frames
    if (from_mpi->pict_type == FF_B_TYPE)
        return to_mpi;

    if (to_mpi &&
            to_mpi->w == from_mpi->w &&
            to_mpi->h == from_mpi->h &&
            to_mpi->imgfmt == from_mpi->imgfmt)
        mpi = to_mpi;
    else {
        if (to_mpi)
            free_mp_image(to_mpi);
        if (from_mpi->w == 0 || from_mpi->h == 0)
            return NULL;
        mpi = alloc_mpi(from_mpi->w,from_mpi->h,from_mpi->imgfmt);
    }

    copy_mpi(mpi,from_mpi);
    return mpi;
}

//
// set audio in dvdnav
//
static int mp_dvdnav_audio_handle() {
    int dvdnav_audio_num=mp_dvdnav_get_audio(mpctx->stream);
    int current_id=mpctx->demuxer->audio->id;
    int dvdnav_audio;
//printf("++++++dvdnav_audio_menu_select[%d],current_id[%d],audio_id[%d],dvdsub_id[%d],mpctx->d_sub->id[%d]\n",dvdnav_audio_menu_select,current_id,audio_id,dvdsub_id,mpctx->d_sub->id);
//printf("+++mpctx->demuxer->audio->id[%d],audio_id[%d],dvdnav_audio[%d]\n",mpctx->demuxer->audio->id,audio_id,dvdnav_audio);

    //+mantis:4672,4564 
    if(mpctx->demuxer->audio->id == -2 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
	mpctx->demuxer->audio->id = 128;
    //mantis:4672,4564+

    if(dvdnav_audio_menu_select == 1)
    {
    	dvdnav_audio=mp_dvdnav_aid_from_audio_num(mpctx->stream,dvdnav_audio_num);
    	dvdnav_audio=mp_dvdnav_reg_nav_aid(mpctx->stream, dvdnav_audio);
    	if(dvdnav_audio==-1) return 0;					// fail seek
    	if(current_id==dvdnav_audio) return 0;				// no change
	if(mpctx->demuxer->audio->id != -2)//Mantis:4638 Toy.story3 FF in menu fail
	audio_id = demuxer_switch_audio(mpctx->demuxer, dvdnav_audio);	// switch audio

       //Polun 2011-09-01 ++s for waiting_for_superman dvd dvdnav_audio != current_id  	
	if(do_switch_audio_flag == -1 &&  mpctx->demuxer->audio->id != -2)
       {   
            printf("No switch audio but current_id !=dvdnav_audio reinit_audio_chain\n");
            if((current_id & 0x0F)==(dvdnav_audio &0x0F)) return 0;		// if unchanged audio codec then no reinit audio

            if(audio_id == -2 || (audio_id > -1 && mpctx->demuxer->audio->id != current_id && current_id != -2))
                uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);
            if(audio_id > -1 && mpctx->demuxer->audio->id != current_id) {
	         sh_audio_t *sh2 = NULL;
	     if (mpctx->demuxer->audio->id >= 0)
                sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];
            if(sh2) {
                sh2->ds = mpctx->demuxer->audio;
                mpctx->sh_audio = sh2;
                reinit_audio_chain();
            }
        }  
        //Polun 2011-09-01 ++e
    }

    }

//    if(dvdnavstate.aid[dvdnavstate.new_aid_map] != mpctx->demuxer->audio->id)
//        mpctx->demuxer->audio->id = dvdnavstate.aid[dvdnavstate.new_aid_map];
}
#if 0
static int mp_dvdnav_audio_handle() {

    int dvdnav_audio_num=mp_dvdnav_get_audio(mpctx->stream);
    int current_id=mpctx->demuxer->audio->id;
    int dvdnav_audio;

    dvdnav_audio=mp_dvdnav_aid_from_audio_num(mpctx->stream,dvdnav_audio_num);
    dvdnav_audio=mp_dvdnav_reg_nav_aid(mpctx->stream, dvdnav_audio);
    if(dvdnav_audio==-1) return 0;					// fail seek
    if(current_id==dvdnav_audio) return 0;				// no change
    audio_id = demuxer_switch_audio(mpctx->demuxer, dvdnav_audio);	// switch audio
    if((current_id & 0x0F)==(dvdnav_audio &0x0F)) return 0;		// if unchanged audio codec then no reinit audio

    if(audio_id == -2 || (audio_id > -1 && mpctx->demuxer->audio->id != current_id && current_id != -2))
        uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);
    if(audio_id > -1 && mpctx->demuxer->audio->id != current_id) {
	sh_audio_t *sh2 = NULL;
	if (mpctx->demuxer->audio->id >= 0)
            sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];
        if(sh2) {
            sh2->ds = mpctx->demuxer->audio;
            mpctx->sh_audio = sh2;
            reinit_audio_chain();
        }
    }
    return 1;
}
#endif

static void mp_dvdnav_reset_stream (MPContext *ctx) {
    if (ctx->sh_video ) {
        /// clear video pts
        if(speed_mult >= 0)
        {
//printf("----mp_dvdnav_reset_stream----speed_mult[%d]\n",speed_mult);
            ctx->d_video->pts = 0.0f;
            ctx->sh_video->pts = 0.0f;
            ctx->sh_video->i_pts = 0.0f;
            ctx->sh_video->last_pts = 0.0f;
        }
        ctx->sh_video->num_buffered_pts = 0;
        ctx->sh_video->num_frames = 0;
        ctx->sh_video->num_frames_decoded = 0;
        ctx->sh_video->timer = 0.0f;
        ctx->sh_video->stream_delay = 0.0f;
//        ctx->sh_video->timer = 0;
        ctx->demuxer->stream_pts = MP_NOPTS_VALUE;

    }

    if (ctx->sh_audio) {
        /// free audio packets and reset
        ds_free_packs(ctx->d_audio);
        audio_delay -= ctx->sh_audio->stream_delay;
        ctx->delay =- audio_delay;
//        ctx->audio_out->reset();
        resync_audio_stream(ctx->sh_audio);
    }

    int audio_should_update=0;
    if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
    {
	int i;
	//printf("+++++mpctx->demuxer->audio->id[%d]\n",mpctx->demuxer->audio->id);
	for(i = 0; i < 8; i++)
	{
		if(mpctx->demuxer->audio->id == dvdnavstate.aid[i])
			audio_should_update = 0;
		//printf("--[%d]\n",dvdnavstate.aid[i]);
	}

    }


    /* audio handle */
    //if(speed_mult ==0 && dvdnav_audio_menu_select == 1){
    if(speed_mult ==0){
    //if(speed_mult ==0 && (dvdnav_audio_menu_select == 1 || audio_should_update)){
    	int dvdnav_audio_num=mp_dvdnav_get_audio(mpctx->stream);
    	int dvdnav_audio;
	int i;
    	
//printf("dvdnav_audio_num[%d]\n",dvdnav_audio_num);
    	dvdnav_audio=mp_dvdnav_aid_from_audio_num(ctx->stream,dvdnav_audio_num);
//printf("a1====dvdnav_audio[%d]\n",dvdnav_audio);
    	dvdnav_audio=mp_dvdnav_reg_nav_aid(ctx->stream, dvdnav_audio);
	for(i = 0; i < 8; i++)
	{
		if(dvdnavstate.aid[i] == dvdnav_audio)
		{
			dvdnavstate.cur_audio_num = i;
			break;
		}
	}
	if(i == 8)
		dvdnavstate.cur_audio_num = -1;
    	mpctx->demuxer->audio->id = dvdnav_audio;
//printf("a2====dvdnav_audio[%d]\n",dvdnav_audio);
    }

    audio_delay = 0.0f;
    /* subtitle handle */
    mpctx->global_sub_size = mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + mp_dvdnav_number_of_subs(mpctx->stream);
    //if (dvdsub_lang && dvdsub_id == dvdsub_lang_id) {	//Mantis:3122 CG.iso
    if(speed_mult ==0 && dvdnav_sub_menu_select == 1){
mplayer_dvdnav_debug("## DVDNAV In [%s][%d]-before, dvdsub_lang_id[%d],dvdsub_id[%d],mpctx->d_sub->id[%d]##\n", __func__, __LINE__,dvdsub_lang_id,dvdsub_id,mpctx->d_sub->id);
        dvdsub_lang_id = mp_dvdnav_sid_from_lang(ctx->stream, dvdsub_lang);
        if (dvdsub_lang_id != dvdsub_id) {
            dvdsub_id = dvdsub_lang_id;
            select_subtitle(ctx, 1);
        }
mplayer_dvdnav_debug("## DVDNAV In [%s][%d]-after, dvdsub_lang_id[%d],dvdsub_id[%d],mpctx->d_sub->id[%d]##\n", __func__, __LINE__,dvdsub_lang_id,dvdsub_id,mpctx->d_sub->id);
    }

    /// clear all EOF related flags
    ctx->d_video->eof = ctx->d_audio->eof = ctx->stream->eof = 0;
}

//
// set dvdnav subtile
//
static void mp_dvdnav_spu_handle() {
	int i;
	
    mpctx->global_sub_size=mp_dvdnav_number_of_subs(mpctx->stream);
//    if(!mp_dvdnav_spu_has_changed(mpctx->stream)) return;		// is unchanged dvdnav subtitle
    int dvdnav_spu=mp_dvdnav_get_spu(mpctx->stream);
    if (!mpctx->d_sub) return;
    if(dvdnav_spu<0) dvdnav_spu=-2;
    dvdnav_spu=dvdnav_reg_nav_sid(mpctx->stream, dvdnav_spu);
    if (mpctx->d_sub) {
        if((dvdsub_id < 0 && dvdnav_sub_menu_select == 1) || dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
        {
mplayer_dvdnav_debug("## DVDNAV In [%s][%d]-before, dvdsub_lang_id[%d],dvdnav_spu[%d],dvdsub_id[%d],mpctx->d_sub->id[%d]##\n", __func__, __LINE__,dvdsub_lang_id,dvdnav_spu,dvdsub_id,mpctx->d_sub->id);
#if 0 //Mantis:6066
            if(!skydroid && dvdnav_spu != dvdnavstate.sid[dvdnav_spu])
            {
            	/* Looking for dvdnavstate.sid mapping sid */
				for(i=0;i<dvdnav_globle_num_subtitle;i++)
				{
					if(dvdnav_spu == dvdnavstate.sid[i])
					{
						dvdsub_id = i;
						mpctx->d_sub->id = i;
					}
				}
            }
            else
#endif
            {
            	dvdsub_id = dvdnav_spu;
            	mpctx->d_sub->id = dvdnav_spu;
            }
mplayer_dvdnav_debug("## DVDNAV In [%s][%d]-after, dvdsub_lang_id[%d],dvdnav_spu[%d],dvdsub_id[%d],mpctx->d_sub->id[%d]##\n", __func__, __LINE__,dvdsub_lang_id,dvdnav_spu,dvdsub_id,mpctx->d_sub->id);
        }
    }
    return;
}

//
// set dvd menu buttons
//
static void mp_dvdnav_highlight_handle(int stream_is_change) {
    nav_highlight_t highlight;
    unsigned int *spu_clut;
    static int first_change=1;

    if (!dvdnav_color_spu) return;					// is disable dvdnav color spu
    if (dvdnav_color_spu_flg==-1) 					// is unsettings vo color mode
    {

        if (mpctx->video_out) 					// query vo color mode
        {
//            ((vf_instance_t *)mpctx->sh_video->vfilter)->control(mpctx->sh_video->vfilter, VFCTRL_COLORSPU_SUPPORT, &dvdnav_color_spu_flg);
            if(dvdnav_color_spu_flg==-1)
            {
                if (mpctx->video_out->control(VOCTRL_COLORSPU_SUPPORT,&dvdnav_color_spu_flg)!=VO_TRUE)
                {
                    dvdnav_color_spu_flg=0;
                }
            }
//            if (!dvdnav_color_spu_flg)
//	        mp_msg(MSGT_CPLAYER,MSGL_ERR,"Color SPU does not support this vo!");
            dvdnav_color_spu_flg = 2;
        }
    }
    spu_clut = mp_dvdnav_get_spu_clut(mpctx->stream);
    mp_dvdnav_get_highlight(mpctx->stream,&highlight);
static int button_type=2; // 0:hide 1:show
char callback_str[42];
static int callback_playing_delay_cnt=0;	//Skyviia_Vincent07062011 mantis:5303 5313
//printf("dvdnav_title_stat[%d],dvdnav_state_change[%d],button_type[%d],btn_on[%d]\n",dvdnavstate.dvdnav_title_state, dvdnavstate.dvdnav_state_change, button_type, dvdnavstate.dvdnav_button_on);
    if (highlight.sx==0 && highlight.sy==0 && highlight.ex==0 && highlight.ey==0) 	// is no button
    {
	if(button_type == 1 || button_type == 2)
	{
		dvdnavstate.dvdnav_button_on=0;
		if(callback_playing_delay_cnt>5)
		{
			dvdnavstate.dvdnav_state_change = 1;
			button_type = 0;
			strcpy(callback_str,"status: playing\0");
			ipc_callback(callback_str);
			callback_playing_delay_cnt=0;

                    if(skydroid && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                    {
                    	char callback_str[1024];
                    	extern char subtitle_names[];
			extern int sub_change_success;

                    	mp_property_print("sub", mpctx);
                    	if(subcc_enabled && mpctx->d_sub->id == -2 && subcc_reg)
                    	{
                    		sprintf(subtitle_names, "%s\0","SUBCC 100 cc");
				sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
                    	}
                    	else if(sub_change_success == 1)
				sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
			else if(sub_change_success == 2)
				sprintf(callback_str, "SUBTITLE: partial %s\0", subtitle_names);
			else
				sprintf(callback_str, "SUBTITLE: %s\0", subtitle_names);
			sub_change_success = 0;
                    	ipc_callback(callback_str);
                    }
		}
//		dvdnav_button_on=0;
		//if(dvdnavstate.dvdnav_state_change == 0)
		if(callback_playing_delay_cnt<=5)
			callback_playing_delay_cnt++;
	}

        if (stream_is_change || (spudec_visible(vo_spudec) && !mpctx->global_sub_size))
        {
            spudec_update_palette(vo_spudec,spu_clut);
            spudec_dvdnav_mode(vo_spudec, 0, dvdnav_color_spu_flg);	/* spu menu button off */ //◎◎vincent_note◎◎:設定dvdnav_color_spu color mode, yuv RGB
            spudec_dvdnav_area(vo_spudec,highlight.sx,highlight.sy,highlight.ex,
                               highlight.ey,highlight.palette); /* set spu button area & palette */

            if(vo_spudec) spudec_reset(vo_spudec);
//	        vo_osd_changed(OSDTYPE_SPU);

            if(vo_need_dvdnav_menu == 1)
                first_change = 1;

            vo_need_dvdnav_menu = 0;
        }
    } else
    {
		if(callback_playing_delay_cnt)
		{
			callback_playing_delay_cnt = 0;
			button_type = 0;
		}

		if(button_type == 0 || button_type == 2)
		{
//		int is_i_mode=0;
//		is_i_mode = vd_get_is_deint();
////printf("is_i_mode[%d].dvdnavstate.dvdnav_title_state[%d]\n",is_i_mode,dvdnavstate.dvdnav_title_state);
//		if(is_i_mode == 1 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
//		{
//			dvdnavstate.dvdnav_button_on=0; //work around for SPIDERWICK 2893 seconds system crash
//			dvdnavstate.dvdnav_state_change = 0;
//		}
//		else
		if( (duration_sec_cur < 300 && duration_sec_cur > 0 &&
			dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE) || 
			dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU
		  )
		{
			dvdnavstate.dvdnav_button_on=1;
			dvdnavstate.dvdnav_state_change = 1;
			button_type = 1;
			strcpy(callback_str,"status: menu\0");
			ipc_callback(callback_str);
		}
		else if(dvdnav_resume) //"The Last Mimsy" special case for resume, because there has button at movie state
		{
			dvdnav_resume = 0;
			button_type = 1;
			strcpy(callback_str,"status: playing\0");
			ipc_callback(callback_str);
		}
		}
//printf("dvdnavstate.dvdnav_title_state[%d],dvdnav_button_on[%d]\n",dvdnavstate.dvdnav_title_state,dvdnavstate.dvdnav_button_on);
//printf("dvdnavstate.dvdnav_state_change[%d],button_type[%d]\n",dvdnavstate.dvdnav_state_change,button_type);
//printf("highlight.sx[%d], highlight.sy[%d], highlight.ex[%d], highlight.ey[%d]\n",highlight.sx, highlight.sy, highlight.ex, highlight.ey);
        //Menu button started
        spudec_update_palette(vo_spudec,spu_clut);
        if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU) //Mantis:5199 in 3352 seconds subtitle show too long
        {
        spudec_dvdnav_mode(vo_spudec, 1, dvdnav_color_spu_flg);	/* spu menu button on */
        	vo_need_dvdnav_menu = 1;
        }
        spudec_dvdnav_area(vo_spudec,highlight.sx,highlight.sy,highlight.ex,
                           highlight.ey,highlight.palette); /* set spu button area & palette */
        vo_osd_changed(OSDTYPE_SPU);

        if(vo_need_dvdnav_menu == 0)
            first_change = 1;

        vo_need_dvdnav_menu = 1;

//        if (!vo_spudec || !spudec_visible(vo_spudec)) {		// if unsucessful spu button visible

//	        osd_set_nav_box(highlight.sx,highlight.sy,highlight.ex,highlight.ey);	// then use simple box
//	        vo_osd_changed(OSDTYPE_DVDNAV);
//	    } else
//	    {

//	        osd_set_nav_box(0,0,0,0);				// id sucessful spu button visible the
//	        vo_osd_changed(OSDTYPE_DVDNAV);				// then hide simple box
//	    }
    }

    if(first_change)
    {
        //ioctl(skyfd, SKY_VDEC_DO_ALPHABLEND, &vo_need_dvdnav_menu);

        first_change = 0;
    }
}

//
// query: dvdnav stream change (cell, title, audio or subtitle)
//
static int mp_dvdnav_is_stream_change() {
    if (!mp_dvdnav_stream_has_changed(mpctx->stream)) return 0;
    return 1;
}

//#define ENABLE_DVDNAV_CELL_STATE

#ifdef ENABLE_DVDNAV_CELL_STATE
#define mplayer_dvdnav_cell_state(fmt, args...)	printf(fmt, ##args)
#else /* else of ENABLE_DVDNAV_CELL_STATE */
#define mplayer_dvdnav_cell_state(fmt, args...)		{}
#endif /* end of ENABLE_DVDNAV_CELL_STATE */

//
// video frame preprocess: before decode_video()
//
/// Restore last decoded DVDNAV (still frame)
static mp_image_t *mp_dvdnav_restore_smpi(int *in_size,
        unsigned char **start,
        mp_image_t *decoded_frame)
{
    int delay_cell_change = 0, maybe_reinit_audio = 0;
    int dvd_audio_nums = mp_dvdnav_number_of_audios(mpctx->stream);
    static int sh_audio_null_cnt = 0; //for dolby DVD 2Ch Tests P.1 -> 2-ch Compression item
    float time_len;
    static int duration_sec_cur_old = 0; //Skyviia_Vincent11162011 Don't send the same of the duration

//printf("maybe_reinit_audio[%d]\n",maybe_reinit_audio);
//printf("*in_size[%d],mpctx->d_video->eof[%d], mpctx->d_audio->eof[%d]\n",*in_size,mpctx->d_video->eof, mpctx->d_audio->eof);
//printf(" !!!! mpctx->d_audio->pts:%f  d_packs:%d\n", mpctx->d_audio->pts, mpctx->d_audio->packs);
//printf("mpctx->stream->eof[%d],mpctx->d_video->eof[%d], mpctx->d_audio->eof[%d]\n",mpctx->stream->eof,mpctx->d_video->eof, mpctx->d_audio->eof);
//printf("2-mpctx->sh_video->pts[%f], len[%f],state[%d]\n",mpctx->sh_video->pts, len2,dvdnavstate.dvdnav_title_state);
//if(mpctx->sh_audio == NULL)
//	printf("mpctx->sh_audio-----NULL\n");
//printf("dvdnav_is_delay_cell[%d],dvdnav_toomanypkg[%d]\n",dvdnav_is_delay_cell,dvdnav_toomanypkg);
 static float len1;
if(dvdnav_toomanypkg && (mp_dvdnav_cell_has_changed(mpctx->stream, 0) == 0))
{
 len1 = demuxer_get_time_length(mpctx->demuxer);
}

//printf("1-mpctx->sh_video->pts[%f], len1[%f],state[%d]\n",mpctx->sh_video->pts, len1,dvdnavstate.dvdnav_title_state);

    static int cell_delay_cnt = 0;
    static int insize_null_cnt = 0; //星際寶貝2_史迪奇有問題 select first menu hang
    if (mp_dvdnav_cell_has_changed(mpctx->stream, 0) == 1) {
          mplayer_dvdnav_cell_state("## 1-1 ##\n");
          if (mpctx->sh_audio && mpctx->sh_video && mpctx->d_audio->packs > 800)
          {
//printf(" === 001 !!!! v pts: %f  mpctx->d_audio->pts:%f  d_packs:%d\n", mpctx->d_video->pts, mpctx->d_audio->pts, mpctx->d_audio->packs);
                seek_sync_flag = 0;
                cell_delay_cnt = mpctx->d_audio->packs;
          }
          if (cell_delay_cnt > 1)
          {
                  mplayer_dvdnav_cell_state("## 1-2 ##\n");
                  delay_cell_change = 1;
                  cell_delay_cnt--;
          }

          float len; //Mantis:5657 Harry Potter and the Goblet of Fire
          len = demuxer_get_time_length(mpctx->demuxer);

          if (!dvdnav_toomanypkg && mpctx->stream->eof == 1 && mpctx->d_video->eof==0 && mpctx->d_audio->eof == 1
	    && mpctx->sh_video->pts < len && len!=0 && mpctx->sh_video->pts!=0)
          {
	    mplayer_dvdnav_cell_state("## 1-3 ##\n");
	    delay_cell_change = 1;
	    dvdnav_is_delay_cell = 0;
          }
          else if (dvdnav_toomanypkg && mpctx->stream->eof == 1 && mpctx->d_video->eof==0 && mpctx->d_audio->eof == 1
	    && mpctx->sh_video->pts < len1 && len1!=0 && mpctx->sh_video->pts!=0)
          {
	    mplayer_dvdnav_cell_state("## 1-3-1 ##\n");
	    delay_cell_change = 1;
	    dvdnav_is_delay_cell = 1;
          }
          
//          if(dvdnav_is_delay_cell == 2)
//          	if (mpctx->stream->eof == 1 && mpctx->d_video->eof==0 && mpctx->d_audio->eof == 1
//	    && mpctx->sh_video->pts < len3 && len3!=0 && mpctx->sh_video->pts!=0)
//          	{
//	    		mplayer_dvdnav_cell_state("## 1-5 ##\n");
//	    		delay_cell_change = 1;
//	    		//dvdnav_is_delay_cell = 0;
//          	}

    }

    if (mp_dvdnav_vts_has_changed(mpctx->stream, 0) == 1)
    {
	mplayer_dvdnav_cell_state("## 1-4 ##\n");
	if ((*in_size > 0) && mpctx->sh_audio && (mpctx->d_video->eof + mpctx->d_audio->eof == 1))
	{
	    mplayer_dvdnav_cell_state("## 1-5 ##\n");
	    // 22_20070508 dvd
	    delay_cell_change = 1;
	}
	else if ((*in_size < 0) && mpctx->sh_audio && (mpctx->d_audio->eof == 0))
	{
	    mplayer_dvdnav_cell_state("## 1-6,insize_null_cnt[%d] ##\n",insize_null_cnt);
	    delay_cell_change = 1;
if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && seek_sync_flag==0)
{
insize_null_cnt++;
}
if(insize_null_cnt > 100)
	delay_cell_change = 0;
	}
	else if ((mpctx->stream->eof == 1) && mpctx->sh_audio && (mpctx->d_audio->eof == 0))
	//else if ((mpctx->stream->eof == 1) && (mpctx->d_audio->eof == 0))
	{
	    mplayer_dvdnav_cell_state("## 1-7 ##\n");
	    // dolby dvd
	    delay_cell_change = 1;
	}
	if ((speed_mult == 0) && (dvd_audio_nums > 0))
	{
	    mplayer_dvdnav_cell_state("## 1-8 ##\n");
	    maybe_reinit_audio = 1;
	}
	if (delay_cell_change == 0)
	{
		mplayer_dvdnav_cell_state("## 1-9 ##\n");
		if(mpctx->d_audio->packs != 0)
			delay_cell_change = 1;
		else
		mp_dvdnav_vts_has_changed(mpctx->stream, 1);

		//+Mantis:4937. select scene selection chapter 1 fail
		time_len = demuxer_get_time_length(mpctx->demuxer);
		if(dvdnavstate.dvdnav_state_change == 1 && time_len == 0)
			delay_cell_change = 1;
		else 
			delay_cell_change = 0;
		//Mantis:4937+. select scene selection chapter 1 fail

#if 0
		if(mpctx->sh_audio==NULL)
			sh_audio_null_cnt++;
		else
			sh_audio_null_cnt = 0;

		if(sh_audio_null_cnt == 2) //give one chance for null sh_audio
		{
			mp_dvdnav_vts_has_changed(mpctx->stream, 1);
			sh_audio_null_cnt = 0;
		}
		else if(sh_audio_null_cnt == 0) //mpctx->sh_audio != NULL
		{
			mp_dvdnav_vts_has_changed(mpctx->stream, 1);
		}
		else //sh_audio_null_cnt should be 1
		{
			printf("give one chance for null sh_audio\n");
			delay_cell_change = 1;
		}
#endif
	}
    }

    if (mp_dvdnav_cell_has_changed_now(mpctx->stream, 0) == 1) {

	mplayer_dvdnav_cell_state("## 1-10 ##\n");
	if(mpctx->audio_out ) //Mantis:5596 武林外傳爆音
	{
		mpctx->audio_out->reset();
	}

	//+Mantis:4937. select scene selection chapter 1 fail
	time_len = demuxer_get_time_length(mpctx->demuxer);
	if(dvdnavstate.dvdnav_state_change == 1 && time_len==0 )
		delay_cell_change = 1;
	else 
	delay_cell_change = 0;
	//Mantis:4937+. select scene selection chapter 1 fail

	sh_audio_null_cnt = 0;
	if(mpctx->d_audio->packs<4096) //Mantis:5781 Dirty prety things.iso
		mp_dvdnav_cell_has_changed_now(mpctx->stream, 1);
	mp_dvdnav_vts_has_changed(mpctx->stream, 1);
    }

//printf("delay_cell_change[%d],dvdnav_title_state[%d]\n",delay_cell_change,dvdnavstate.dvdnav_title_state);
    /// a change occured in dvdnav stream
    if ( delay_cell_change==0 && (mp_dvdnav_cell_has_changed(mpctx->stream, 0) == 1)) {
    	mplayer_dvdnav_cell_state("## 1-11,maybe_reinit_audio[%d] ##\n",maybe_reinit_audio);

	duration_sec_cur = demuxer_get_time_length(mpctx->demuxer);
	if(speed_mult == 0 && duration_sec_cur_old != duration_sec_cur)
    {
        duration_sec_cur_old = duration_sec_cur;
		ipc_callback_duration();
    }

dvdnav_toomanypkg = 0;
if(dvdnav_is_delay_cell == 2)
	dvdnav_is_delay_cell = 0;
else
dvdnav_is_delay_cell--;

if(insize_null_cnt)
  insize_null_cnt = 0;
        cell_delay_cnt = 0;
        // suspend read from dvdnav stream, and set auto_wait
        mp_dvdnav_read_wait(mpctx->stream, 1, 1);
        mp_dvdnav_context_free(mpctx);
        mp_dvdnav_reset_stream(mpctx);
        // enable read from dvdnav stream
        mp_dvdnav_read_wait(mpctx->stream, 0, 1);

	if (speed_mult == 0 && maybe_reinit_audio)
	{
	    //puts("dvdnav_audio_need_uninit");
	    dvdnav_audio_need_uninit = 1;
	}

	if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && ipcdomain && skydroid)
	{
		// call_back chapterinfo if cur_chapter has been changed
		int cur_chapter;
		static int last_chapter;
		char callback_str[256];
		if (mpctx->demuxer)
        		cur_chapter = demuxer_get_current_chapter(mpctx->demuxer);	
		if(last_chapter != cur_chapter)
		{
			stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS, &mpctx->demuxer->num_chapters);
			last_chapter = cur_chapter;
			sprintf(callback_str,"ChapterInfo=%02d,%02d\0",cur_chapter+1,mpctx->demuxer->num_chapters);
			ipc_callback(callback_str);
		}
	}

        // reset cell change flag
        mp_dvdnav_cell_has_changed(mpctx->stream,1);
        //memset(disp2_addr, 0x00, (mpctx->sh_video->disp_w*mpctx->sh_video->disp_h)*4);
//		memset(disp2_addr, 0x00, (720*480*4));

	memset(dvdnav_btn_virtaddr[dvd_btn_addr_idx], 0x00, DVDNAV_BUTTON_SIZE);
        dvdnavstate.dvdnav_cell_change = 1;
//printf("\n=== in cell change  sky_vpts_total_cnts=%d\n", sky_vpts_total_cnts);

        dvdnav_decode_cnts_in_cell_change = sky_vpts_total_cnts;
        if (seek_sync_flag == 6)
        {
                seek_sync_flag = 5;
        }
    }

    if (seek_sync_flag == 6 && (mp_dvdnav_cell_has_changed(mpctx->stream, 0) == 0))
		seek_sync_flag = 5;

//Robert 20110627 skip WAIT while DVDNAV_TITLE_STATE_MOVIE, fix SpiderMan sec 604 & 3460
    if (mp_dvdnav_is_wait(mpctx->stream))
    {
        if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
        {
                mp_dvdnav_skip_wait(mpctx->stream);
        }
//printf(" mp_dvdnav_is_wait(mpctx->stream) = %d seek_sync_flag=%d\n", mp_dvdnav_is_wait(mpctx->stream), seek_sync_flag);
    }

    if (*in_size < 0 && (speed_mult == 0 || speed_mult == 1)) {
        float len;

        dvdnav_should_do_alpha = 1;

        /// Display still frame, if any
        if (mpctx->nav_smpi && !mpctx->nav_buffer)
            decoded_frame = mpctx->nav_smpi;

        /// increment video frame : continue playing after still frame
        len = demuxer_get_time_length(mpctx->demuxer);
//printf("mpctx->sh_video->pts[%f], len[%f],state[%d]\n",mpctx->sh_video->pts, len,dvdnavstate.dvdnav_title_state);
        if (mpctx->sh_video->pts >= len &&
                mpctx->sh_video->pts > 0.0 && len > 0.0)
        {
            mp_dvdnav_skip_still(mpctx->stream);
            mp_dvdnav_skip_wait(mpctx->stream);
        }
//        mpctx->sh_video->pts += 1 / mpctx->sh_video->fps;	//Fuchun 2011.04.21 move to below
//printf("dvdnav_btn_num[%d]\n",dvdnav_btn_num);
	//+Mantis:4937. select scene selection chapter 1 fail
	if( dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE || 
	   (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU && dvdnav_btn_num == 1) ||
	(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU && dvdnav_btn_num == 0 && duration_sec_cur ==0) //星際寶貝STITCH_THE_MOVIE.iso
	  )
	{
		static int still_cnt=0;
		
		if(mpctx->sh_video->pts==0.0 && len==0.0)
			still_cnt++;
		else if(len==0.0)
			still_cnt++;
//if(mp_dvdnav_is_stillframe(mpctx->stream))
//printf("----still---\n");
		if(still_cnt > 20 && mp_dvdnav_is_stillframe(mpctx->stream))
		{
			if(dvdnav_btn_num == 1)
			{
				int button = -1;
				mp_dvdnav_handle_input(mpctx->stream,MP_CMD_DVDNAV_AUTOACTION,&button);
				dvdnav_btn_num = 0;
			}
			else
			{
				mp_dvdnav_skip_still(mpctx->stream);

			}
			still_cnt=0;
		}
	}
	//Mantis:4937+. select scene selection chapter 1 fail
	int skip_wait_no_restore=0; //Toy.Story.3 menu flick
	//if(mp_dvdnav_is_wait(mpctx->stream) && !(mpctx->d_audio->packs || mpctx->d_video->packs) && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
		if(mp_dvdnav_is_wait(mpctx->stream) && !(mpctx->d_audio->packs || mpctx->d_video->packs) )
		{
			//Mantis:3963 Toy.Story.3 menu stage screen lock
			mp_dvdnav_skip_wait(mpctx->stream);
		if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
			skip_wait_no_restore=1;
		}

        if (mpctx->nav_buffer) {
            // Set start, in_size and copy frame data from stored frame
	    if (video_restore_frame(mpctx->sh_video, start, mpctx->nav_buffer, mpctx->nav_in_size) == 1)
	    {
		if(skip_wait_no_restore)
		{
			*in_size = 1;
			skip_wait_no_restore = 0;
		}
		else
		*in_size = mpctx->nav_in_size;
	    }
        }
		if(*in_size > 0 && !decoded_frame)      //Fuchun 2011.04.21 add condition
			 mpctx->sh_video->pts += 1 / mpctx->sh_video->fps;
		
        dvdnavstate.dvdnav_cell_change = 2;
    }
    //Polun 2011-07-21 for jumanji DVD iso FF to end can't return menu
    //else if(*in_size < 0 && speed_mult == 1 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && ((duration_sec_cur-my_current_pts) < 3))
    else if(*in_size < 0 && speed_mult >= 1 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && ((duration_sec_cur-my_current_pts) < 3)) 
    {
    	//Mantis:5277 Thomas Crown Affair FF 2X to end hang
    	FFFR_to_normalspeed(NULL);
    }

    return decoded_frame;
}

//
// video frame postprocess: after decode_video()
//
/// Save last decoded DVDNAV (still frame)
static void *mp_dvdnav_save_smpi(int in_size,
                                 unsigned char *start,
                                 mp_image_t *decoded_frame)
{

    if(in_size > 0 && in_size != mpctx->nav_in_size)
    {
	if (mpctx->nav_buffer)
	    free(mpctx->nav_buffer);

	mpctx->nav_buffer = malloc(in_size);
	mpctx->nav_start = start;
	mpctx->nav_in_size = mpctx->nav_buffer ? in_size : -1;
	if (mpctx->nav_buffer)
	    memcpy(mpctx->nav_buffer,start,in_size);
    }

    if (decoded_frame && mpctx->nav_smpi != decoded_frame)
        mpctx->nav_smpi = mp_dvdnav_copy_mpi(mpctx->nav_smpi,decoded_frame);

#if 0
    float len;

    if (in_size<0) {
        if (mpctx->nav_buffer)
        {
            // free buffer
            free(mpctx->nav_buffer);
            mpctx->nav_buffer=NULL;
            mpctx->nav_in_size=0;
        }
        // get duration of part
        len = demuxer_get_time_length(mpctx->demuxer);
        if ((mpctx->sh_video->pts>=len || !mpctx->nav_smpi) &&
                mpctx->sh_video->pts>0.0 && len>0.0)
        {

            // clear still frame in dvdnav
            mp_dvdnav_skip_still(mpctx->stream);
            // clear wait in dvdnav
            mp_dvdnav_skip_wait(mpctx->stream);
        }

        // increment video frame
        mpctx->sh_video->pts+=1/mpctx->sh_video->fps;
    } else {
//        if (mpctx->dup_frame) {
//            if (mpctx->libmpeg2_count)
//		        mpctx->libmpeg2_count--;
//            if (!mpctx->libmpeg2_count) {
//                // clear buffers
//                if (mpctx->nav_buffer)
//		            free(mpctx->nav_buffer);
//                mpctx->nav_buffer=NULL;
//                mpctx->nav_in_size=0;
//
//                if (!decoded_frame && !mpctx->nav_smpi) {
//                    mp_msg(MSGT_CPLAYER,MSGL_ERR,
//                        "Can't decode still frame.\n"
//                        "Please, you try play dvdnav to -vc ffmpeg12 option.\n");
//                    mpctx->eof=1;
//                }
//            }
//        // no duplicate mode
//        } else
        {
            // clear buffer
            if (mpctx->nav_buffer)
                free(mpctx->nav_buffer);
            mpctx->nav_in_size=in_size;

            // allocate buffer and store read video frame data
            mpctx->nav_buffer = malloc(in_size);
            mpctx->nav_start = start;
            if (mpctx->nav_buffer)
                memcpy(mpctx->nav_buffer,start,in_size);
            else
                mpctx->nav_in_size=-1;
        }
        // if decoded OK and no stored image then store image
        if (decoded_frame && mpctx->nav_smpi != decoded_frame)
        {
            mpctx->nav_smpi = mp_dvdnav_copy_mpi(mpctx->nav_smpi,decoded_frame);
        }
    }
#endif
    return decoded_frame;
}
#endif /* CONFIG_DVDNAV */

static void adjust_sync_and_print_status(int between_frames, float timing_error)
{
    current_module="av_sync";
    static unsigned int time1 = 0;

    if(mpctx->sh_audio && !switch_audio_thread_status && mpctx->audio_out) {
        double a_pts, v_pts;
        extern int is_dvdnav;

        if (autosync)
            /*
             * If autosync is enabled, the value for delay must be calculated
             * a bit differently.  It is set only to the difference between
             * the audio and video timers.  Any attempt to include the real
             * or corrected delay causes the pts_correction code below to
             * try to correct for the changes in delay which autosync is
             * trying to measure.  This keeps the two from competing, but still
             * allows the code to correct for PTS drift *only*.  (Using a delay
             * value here, even a "corrected" one, would be incompatible with
             * autosync mode.)
             */
            a_pts = written_audio_pts(mpctx->sh_audio, mpctx->d_audio) - mpctx->delay;
        else
            a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);

//20110707 Robert fix DVD my_current_pts no change issue while FFx2
        if (speed_mult != 0)
            a_pts = 0;
        
        last_audio_pts = a_pts;

		if (mpctx->stream->type ==STREAMTYPE_DVDNAV )
		{
//		        last_audio_pts = a_pts;
#if 1	//Fuchun 2011.04.25
			if(dvdnav_error_file)
			{
				my_current_pts = mpctx->sh_video->pts;
			}
			else
			{
				#if 0	//Barry 2011-06-27 disable
			static float tmp_stream_pts=0;
			if(tmp_stream_pts == mpctx->demuxer->stream_pts)
			{
				same_pts_cnt++;	
			}
				#endif

			if(speed_mult > 1 || speed_mult < 0)
			{
				if(mpctx->demuxer->stream_pts != MP_NOPTS_VALUE)
				{
					my_current_pts = mpctx->demuxer->stream_pts;
					correct_last_dvdpts = mpctx->demuxer->stream_pts;
				}
				else
					my_current_pts = correct_last_dvdpts;
			}
				else if(abs((int)mpctx->demuxer->stream_pts-(int)a_pts) > 10)
				//else if(abs((int)mpctx->demuxer->stream_pts-(int)a_pts) > 10 && (same_pts_cnt < 3 || speed_mult == 1))
			{
				if(correct_last_dvdpts == 0 || correct_last_dvdpts != mpctx->demuxer->stream_pts)
				{
					correct_frame_cnt = 0;
					correct_last_dvdpts = mpctx->demuxer->stream_pts;
				}
				my_current_pts = (mpctx->demuxer->stream_pts + correct_frame_cnt/mpctx->sh_video->fps);
			}
			else
	#if 1	//Barry 2011-06-27
			{
				if(mpctx->d_audio->eof)
					my_current_pts = mpctx->sh_video->pts;
				else
                                   {
                                           if (a_pts < 0.1 && last_audio_pts == a_pts)
                                           {
                                                   same_pts_cnt++;
                                                   if (same_pts_cnt > 1000)	//fix mantis: 5125
								my_current_pts = (mpctx->demuxer->stream_pts + correct_frame_cnt/mpctx->sh_video->fps);
                                                   else
								my_current_pts = a_pts;
                                           }
                                           else if ((wait_video_or_audio_sync & AVSYNC_VIDEO_CATCH_AUDIO) && (last_audio_pts == a_pts))	//Barry 2011-07-11 fix mantis: 5388, 5391
                                           {
                                           	my_current_pts = mpctx->sh_video->pts;
                                           }
                                           else
                                           {
                                                   same_pts_cnt = 0;
					my_current_pts = a_pts;
                                           }
                                   }

				if(correct_last_dvdpts != 0.0)
					correct_last_dvdpts = 0.0;
			}
	#else
					 my_current_pts = a_pts;
	#endif
				#if 0	//Barry 2011-06-27 disable
			tmp_stream_pts = mpctx->demuxer->stream_pts;
				#endif
			}
#else
			if(mpctx->d_audio->eof || speed_mult != 0)
				my_current_pts = mpctx->sh_video->pts;
			else
				my_current_pts = a_pts;
#endif
		}

#if 1
        {
            //static double keep_last_vpts=0.0;
            static int drop_message=0;
            double x;

            svread(0, &sky_nrb, 0);
            v_pts = sky_nrb.cur_pts;
//if (sky_nrb.qlen[2] == 0)
//printf(" qlen[2] = %d  v_pts:%f\n", sky_nrb.qlen[2], v_pts);
#if 0	//Fuchun 20110725 fix video be locked when playback from http
#ifdef QT_SUPPORT_DVBT
		if (mpctx->stream->type != STREAMTYPE_STREAM && mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
#else /* else of QT_SUPPORT_DVBT */
		if (mpctx->stream->type != STREAMTYPE_STREAM)
#endif /* end of QT_SUPPORT_DVBT */
#endif	/*end of 0*/
		{
			if (speed_mult == 0)	//Barry 2010-09-17
			{
				static int vsync_number = 0;
				if (sky_vdec_wait_sync_vpts > 0.0)
				{
					time1 = GetTimer() - time1;
					vsync_number += (unsigned int)((float)time1/1000)*90;
					time1 = GetTimer();
					if (a_pts >= sky_vdec_wait_sync_vpts || mpctx->d_audio->eof || vsync_number > 0)
					{
						printf("@@@ Hay  got sync!!! unlock display\n");
						sky_set_vsync_status(0);
						sky_set_vsync_status(1);
						sky_vdec_wait_sync_vpts = 0.0;
						time1 = 0;
					}
					else if(seek_sync_flag != 0 || !(wait_video_or_audio_sync & AVSYNC_NORMAL))
					{
						printf("@@@ Coerce!!! unlock display\n");
						sky_set_vsync_status(0);
						sky_set_vsync_status(1);
						sky_vdec_wait_sync_vpts = 0.0;
						time1 = 0;
					}
				}
				else if (sky_vdec_wait_sync_vpts == 0.0 && sky_nrb.qlen[3] <= -(90000/10))
				{
#ifdef DYNAMIC_CALCULATED_PTS
					if (!need_do_dynamic_pts)
					{
#endif /* end of DYNAMIC_CALCULATED_PTS */
						time1 = GetTimer();
						vsync_number = sky_nrb.qlen[3];
						sky_get_next_pts(sky_nrb.cur_pts, &sky_vdec_wait_sync_vpts);
						printf("@@@ Hay  sky_nrb.cur_pts:%f wait:%f vsync_number:%d  lock display\n", sky_nrb.cur_pts, sky_vdec_wait_sync_vpts, vsync_number);
						sky_set_vsync_status(-1);
						seek_sync_flag = 0;
						wait_video_or_audio_sync = AVSYNC_NORMAL;
#ifdef DYNAMIC_CALCULATED_PTS
					}
#endif /* end of DYNAMIC_CALCULATED_PTS */
				}
			}
			else
			{
				if(sky_vdec_wait_sync_vpts > 0.0)
				{
					printf("@@@ Coerce!!! unlock display\n");
					sky_set_vsync_status(0);
					sky_set_vsync_status(1);
					sky_vdec_wait_sync_vpts = 0.0;
					time1 = 0;
				}
			}
		}

            sky_vdec_vq_ref_pts = sky_nrb.cur_pts;
            sky_vdec_vdref_num=sky_nrb.qlen[0];

            if (mpctx->stream->type==STREAMTYPE_DVDNAV)
            {
//                if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                {
                    static double t_vid_latency = 0.30;

                    if (mpctx->sh_video->pts > sky_nrb.cur_pts)
                    {
                        double output_fps = 29.97;
//                        t_vid_latency = mpctx->sh_video->pts - sky_nrb.cur_pts;
//                        t_vid_latency += 0.12;
                        if (mpctx->sh_video->fps<= 24)
                        {
                            if ((vdec_init_info.pulldown32_2 || vdec_init_info.pulldown32) && sky_telecine)
                            {
                                    output_fps = 29.97;
                            }
                            else
                            {
                                    output_fps = mpctx->sh_video->fps;
                            }
                        }
                        else
                        {
                            output_fps = mpctx->sh_video->fps;
                        }
                        t_vid_latency = (1 / mpctx->sh_video->fps)*((svsd_state->queue_length) + 3)
                                        + ((1 / output_fps) * (sky_nrb.qlen[2]+0.5) );

                    }
//printf("t_vid_latency=%f  n:%d\n", t_vid_latency, svsd_state->queue_length + sky_nrb.qlen[2]);

                    AV_delay = last_audio_pts - audio_delay - mpctx->sh_video->pts + t_vid_latency;//0.30;//0.38;
//sub_delay = -AV_delay;
                    if ( fabs(last_audio_pts - mpctx->sh_video->pts)<5)
                    {
//                                AV_delay = last_audio_pts - mpctx->sh_video->pts + t_vid_latency;//0.30;//0.38;
                    }
                    else
                    {
                            if (AV_delay <= -5)
                                    AV_delay = -5;
                            else if (AV_delay >= 5)
                                    AV_delay = 5;
//printf("last_audio_pts:%f mpctx->sh_video->pts:%f\n", last_audio_pts, mpctx->sh_video->pts);
//                                AV_delay = 0;//a_pts - audio_delay - mpctx->sh_video->pts;
                    }

                    if (mpctx->sh_video->pts == mpctx->sh_video->last_i_pts)
                    {
//                        if (speed_mult == 0 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                        if (/*dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && */!switch_audio_thread_status && speed_mult == 0 && sky_vdec_wait_sync_vpts == 0.0 
                        && seek_sync_flag == 0 && wait_video_or_audio_sync  == AVSYNC_NORMAL)

                        {
                            double quick_vft_diff;

                            if (AV_delay > 0.05)//3)//0.06)
                            {
                                double AV_range_limit = 0.12;
//                                last_pulldown32_vft_diff = (-1)*(3000*0.06);//-10;
                                if (vd_get_is_deint() == 0)
                                {
                                        AV_range_limit = 0.10;
//printf("AV_range_limit = %f\n", AV_range_limit);
                                }
                                if (AV_delay > 0.5)
                                        quick_vft_diff = (-1)*(90000*0.5)/35;
                                else if (AV_delay > AV_range_limit)
                                        quick_vft_diff = (-1)*(90000*0.5)/150;
                                else
                                        quick_vft_diff = (-1)*(90000*AV_delay)/600;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//printf("\n===>UP new last_pulldown32_vft_diff = %d\n", last_pulldown32_vft_diff);

                            }
                            else if (AV_delay < -0.06)//3)//-0.06)
                            {
//                                last_pulldown32_vft_diff = (3000*0.06);

                                if (AV_delay < -0.5)
                                        quick_vft_diff = (90000*0.5)/30;
                                else if (AV_delay < -0.13)
                                        quick_vft_diff = (90000*0.5)/150;
                                else
                                        quick_vft_diff = (-1)*(90000*AV_delay)/600;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//printf("\n===>DOWN new last_pulldown32_vft_diff = %d\n", last_pulldown32_vft_diff);
                            }
                            else
                            {
//                                last_pulldown32_vft_diff = 0;
                                        quick_vft_diff = (-1)*(90000*AV_delay)/900;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//24p could also use this check
                                if (sky_is_output_interlace || sky_disp_rate==24)
                                {
                                    if (sky_nrb.qlen[3] < 1500)
                                        last_pulldown32_vft_diff = -5;
                                    else
                                        last_pulldown32_vft_diff = 5;
                                }

                            }

                        }
                        else
                        {
                                last_pulldown32_vft_diff = 0;
                        }
#ifdef CONFIG_DEBUG_AVSYNC
if (seek_sync_flag > 0 || wait_video_or_audio_sync != AVSYNC_NORMAL)
printf("### seek flag:%d WAIT:%d ridx:%d  widx:%d   vft diff:%f\n", seek_sync_flag, wait_video_or_audio_sync, sky_nrb.ft_ridx, sky_nrb.ft_widx, t_vid_latency);
else
printf("\n qlen[2]:%d [3]:%d ridx:%d  widx:%d  vftdif:%f cur:%f vft_diff=%d\n", sky_nrb.qlen[2], sky_nrb.qlen[3], sky_nrb.ft_ridx, sky_nrb.ft_widx, t_vid_latency, sky_nrb.cur_pts, last_pulldown32_vft_diff);
#endif
                    }
                    else
                    {
                            if ((AV_delay < 0.06 && last_pulldown32_vft_diff < -10) || 
                                 (AV_delay > -0.07 && last_pulldown32_vft_diff > 10))
                            {
                                    last_pulldown32_vft_diff = (int)((-1)*(90000*AV_delay)/900);
//                                    last_pulldown32_vft_diff = (3000*0.01);
                            }

                            if ((sky_is_output_interlace || sky_disp_rate==24) && (AV_delay > -0.06 && AV_delay < 0.06))
                            {
                                    if (sky_nrb.qlen[3] < 1500)
                                        last_pulldown32_vft_diff = -5;
                                    else
                                        last_pulldown32_vft_diff = 5;
                            }

                    }

                    if (speed_mult != 0)
                    {
                        if (speed_mult == 1)
                        {
                            if (vd_get_is_deint() == 1)
                                    last_pulldown32_vft_diff = -1500;
                            else
                                    last_pulldown32_vft_diff = 0;
                        }
                        else
                        {
                            last_pulldown32_vft_diff = 0;
                        }
                            
                    }

                }
            }
            else //if (mpctx->stream->type != STREAMTYPE_DVDNAV)
            {
                last_pulldown32_vft_diff = 0;

                if(sky_vdec_wait_sync_vpts != 0.0)
                {
                    AV_delay = a_pts - audio_delay - sky_vdec_wait_sync_vpts;
                    sky_avsync_method = 1;
                }
                else if ((wait_video_or_audio_sync&AVSYNC_NORMAL) && !seek_sync_flag)
                {
                    AV_delay = a_pts - audio_delay - v_pts;
                    sky_avsync_method = 2;
                }
                else
                {
                    AV_delay = a_pts - audio_delay - mpctx->sh_video->pts;
                    sky_avsync_method = 0;
                }
//=======
#if 1
                    if (!switch_audio_thread_status && speed_mult == 0 && /*--resync_count <= 0 &&*/ sky_vdec_wait_sync_vpts == 0.0 
                        && seek_sync_flag == 0 && wait_video_or_audio_sync  == AVSYNC_NORMAL)
                    {
//                        resync_count = 16;
                        if (speed_mult == 0)// && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                        {
                            double quick_vft_diff;
                            double up_range_limit = 0.05;
                            if (sky_disp_rate==24)
                                    up_range_limit = 0.02;

                            if (AV_delay > up_range_limit)
                            {
//                                last_pulldown32_vft_diff = (-1)*(3000*0.06);//-10;
                                if (sky_disp_rate == 24 && AV_delay > 0.05)
                                {
                                    quick_vft_diff = (-1)*(90000*0.5)/35;
                                }
                                else if (AV_delay > 0.5)
                                {
                                    if (sky_is_output_interlace)
                                        quick_vft_diff = (-1)*(90000*0.5)/35;//65;
                                    else
                                        quick_vft_diff = (-1)*(90000*0.5)/35;
                                }
//else if (AV_delay > 0.09)
//quick_vft_diff = (-1)*(90000*0.5)/30;
                                else if (AV_delay > 0.12)
                                {
//                                    if (sky_is_output_interlace)
                                        quick_vft_diff = (-1)*(90000*AV_delay)/150;
//                                    else
//                                        quick_vft_diff = (-1)*(90000*0.5)/150;
                                }
                                else
                                        quick_vft_diff = (-1)*(90000*AV_delay)/600;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//printf("\n===>UP 1 new last_pulldown32_vft_diff = %d\n", last_pulldown32_vft_diff);

                            }
                            else if (AV_delay < -0.07)
                            {
//                                last_pulldown32_vft_diff = (3000*0.06);

                                if (AV_delay < -0.5)
                                        quick_vft_diff = (90000*0.5)/30;
                                else if (AV_delay < -0.13)
                                        quick_vft_diff = (90000*0.5)/150;
                                else
                                        quick_vft_diff = (-1)*(90000*AV_delay)/600;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//printf("\n===>DOWN new last_pulldown32_vft_diff = %d\n", last_pulldown32_vft_diff);
                            }
                            else
                            {
//                                last_pulldown32_vft_diff = 0;
                                        quick_vft_diff = (-1)*(90000*AV_delay)/900;//150;//60;//30;
                                last_pulldown32_vft_diff = (int)quick_vft_diff;
//                                if (  abs(sky_nrb.qlen[3] - 1500) < 200)
                                if (sky_is_output_interlace || sky_disp_rate==24)
                                {
                                    if (sky_nrb.qlen[3] < 1500)
                                        last_pulldown32_vft_diff = -5;
                                    else
                                        last_pulldown32_vft_diff = 5;
                                }
                            }

                        }
                        else
                        {
                                last_pulldown32_vft_diff = 0;
                        }
#ifdef CONFIG_DEBUG_AVSYNC
if (seek_sync_flag > 0 || wait_video_or_audio_sync != AVSYNC_NORMAL)
printf("### seek flag:%d WAIT:%d ridx:%d  widx:%d   vft diff:%f\n", seek_sync_flag, wait_video_or_audio_sync, sky_nrb.ft_ridx, sky_nrb.ft_widx, t_vid_latency);
else
printf("\n qlen[2]:%d [3]:%d ridx:%d  widx:%d  vftdif:%f cur:%f vft_diff=%d\n", sky_nrb.qlen[2], sky_nrb.qlen[3], sky_nrb.ft_ridx, sky_nrb.ft_widx, t_vid_latency, sky_nrb.cur_pts, last_pulldown32_vft_diff);
#endif
                    }
                    else
                    {
                            if (sky_vdec_wait_sync_vpts != 0.0)
                            {
                                   last_pulldown32_vft_diff = 0;
                            }
                            else if ((AV_delay < 0.06 && last_pulldown32_vft_diff < -10) || 
                                 (AV_delay > -0.06 && last_pulldown32_vft_diff > 10))
                            {
                                    last_pulldown32_vft_diff = (int)((-1)*(90000*AV_delay)/900);
//                                    last_pulldown32_vft_diff = (3000*0.01);
                            }
                            
                            if (switch_audio_thread_status)
                            {
                                    last_pulldown32_vft_diff = 0;
                            }
                            else if (speed_mult == 1)
                            {
                                if (vd_get_is_deint() == 1)
                                    last_pulldown32_vft_diff = -1500;
                                else
                                    last_pulldown32_vft_diff = 0;
                            }
                            else
                            {
                                    last_pulldown32_vft_diff = 0;
                            }

                    }
#endif
//=======
            }
//printf("last_pulldown32_vft_diff = %d\n", last_pulldown32_vft_diff);
//printf("AV_delay=%f   new=%f\n", AV_delay, sky_vpts_history[(sky_vft_idx - (sky_nrb.ft_widx-sky_nrb.ft_ridx) - svsd_state->queue_length + 1 + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER]-a_pts);
//printf("  ridx:%d   widx:%d\n", sky_nrb.ft_ridx, sky_nrb.ft_widx);

//printf("ft:%d  a_pts:%f  v_pts:%f (%f = %f)  sh_vpts:%f AV_delay=%f\n", between_frames, a_pts, v_pts, v_pts+sky_get_vft_total(svsd_state->queue_length, sky_vdec_vdref_num), sky_get_vft_total(svsd_state->queue_length, sky_vdec_vdref_num), mpctx->sh_video->pts, AV_delay);
/*
static double last_a_pts = 0.0;
if (last_a_pts != a_pts)
printf("sky_met:%d a_pts:%f v_pts:%f sh:%f ridx:%d widx:%d qlen:%d\n", sky_avsync_method, a_pts, v_pts, mpctx->sh_video->pts, sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_nrb.qlen[2]);
if (last_a_pts != a_pts)
        last_a_pts = a_pts;
*/
#endif

            if (AV_delay>0.5 && drop_frame_cnt>50 && drop_message==0) {
                ++drop_message;
//		mp_msg(MSGT_AVSYNC,MSGL_WARN,MSGTR_SystemTooSlow);
                printf("\n****** System is too slow... ******\n\n");
            }
            if (autosync)
                x = AV_delay*0.1f;
            else
                /* Do not correct target time for the next frame if this frame
                 * was late not because of wrong target time but because the
                 * target time could not be met */
                x = (AV_delay + timing_error * playback_speed) * 0.1f;
            if (x < -max_pts_correction)
                x = -max_pts_correction;
            else if (x> max_pts_correction)
                x = max_pts_correction;
            if (default_max_pts_correction >= 0)
                max_pts_correction = default_max_pts_correction;
            else
                max_pts_correction = mpctx->sh_video->frametime*0.10; // +-10% of time
            if (!between_frames) {
                mpctx->delay+=x;	//Fuchun 2010.08.19 disable
                c_total+=x;
            }
            if(!quiet)
            {
//Robert 20101217 reduce print_status cpu loading
                static double last_vpts=0.0, last_apts=0.0;
                if (mpctx->d_video && (last_vpts != mpctx->d_video->pts || last_apts != mpctx->d_audio->pts))
                {
                    print_status(a_pts - audio_delay, AV_delay, c_total);
                    last_vpts = mpctx->d_video->pts;
                    last_apts = mpctx->d_audio->pts;
                }
            }
        }
#if 0 // check av-sync debug 
	if(seek_sync_flag != 0 || wait_video_or_audio_sync != 1)
		printf("@@@ [%d, %d] a_pts[%f]   v_pts[%f]   AV_delay[%f] @@@\n", seek_sync_flag, wait_video_or_audio_sync, a_pts, v_pts, AV_delay);
#endif		
        //Fuchun 2010.07.05
        //if(seek_sync_flag == 2 || (seek_sync_flag == 1 && h264_frame_mbs_only == 0))
        if ( (seek_sync_flag == 2) && (v_pts > 0.0) )	//Barry 2010-10-20
        {
            if(AV_delay > 0.05 && AV_delay < 10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO|AVSYNC_NOVIDEO|AVSYNC_BEGINNING;
                avsync_timeout_cnt = 0;
            }
            else if(AV_delay < -0.05 && AV_delay > -10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO|AVSYNC_NOSOUND|AVSYNC_BEGINNING;
                avsync_timeout_cnt = 0;
            }
        }
        else if(seek_sync_flag == 1)
        {
            if(AV_delay > 0.1 && AV_delay < 10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                avsync_timeout_cnt = 0;
            }
            else if(AV_delay < -0.1 && AV_delay > -10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO|AVSYNC_NOSOUND;
                avsync_timeout_cnt = 0;
            }
        }
        else if(seek_sync_flag == 3)
        {
            if(AV_delay > 0.1 && AV_delay < 10)
            {
                seek_sync_flag = 0;
                dvbt_wait_sync = 1;
                avsync_timeout_cnt = 0;
            }
            else if(AV_delay < -0.1 && AV_delay > -10)
            {
                seek_sync_flag = 0;
                dvbt_wait_sync = 2;
                avsync_timeout_cnt = 0;
            }
        }
        else if(seek_sync_flag == 4)
        {
            if(AV_delay > 0 && AV_delay < 10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                avsync_timeout_cnt = 0;
            }
            else if(AV_delay < 0 && AV_delay > -10)
            {
                seek_sync_flag = 0;
                wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO|AVSYNC_NOSOUND;
                avsync_timeout_cnt = 0;
            }
        }
        else if(seek_sync_flag == 5)	//must be DVDNAV
        {
//if (fabs(sky_nrb.cur_pts - a_pts)<0.5 || svsd_state->queue_length == 0 || mpctx->audio_out->get_delay() < 0.08)
//if (svsd_state->queue_length == 0 || mpctx->audio_out->get_delay() < 0.08)
            //if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
            //if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && mp_dvdnav_is_stillframe(mpctx->stream)==0) //mantis:5195 失落的帝國Atlantis2 Milo's Return.iso 
            if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && duration_sec_cur>10) //300壯士華納片頭
            {
//printf("wait_video_or_audio_sync[%d]\n",wait_video_or_audio_sync);
                    seek_sync_flag = 0;
                    wait_video_or_audio_sync = AVSYNC_NORMAL;
                    avsync_timeout_cnt = 0;
                    sky_set_display_drop_cnt(1);//2);
                    dvdnav_video_first = 1;
#if 0
                if (a_pts < 0)
                {
//                        seek_sync_flag = 7;
//                        seek_sync_flag = 0;
                        seek_sync_flag = 0;
                        wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                        avsync_timeout_cnt = 0;
                }
                else
        {
            if(AV_delay > 0 && AV_delay < 10)
            {
                seek_sync_flag = 0;
                            if (AV_delay > 0.1)
                wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                            else
                                    wait_video_or_audio_sync = AVSYNC_NORMAL;
                avsync_timeout_cnt = 0;
            }
            else if(AV_delay < 0 && AV_delay > -10)
            {
                seek_sync_flag = 0;
                            if (AV_delay < -0.1)
                wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO|AVSYNC_NOSOUND;
                            else
                                    wait_video_or_audio_sync = AVSYNC_NORMAL;
                avsync_timeout_cnt = 0;
            }
        }
#endif
            }
            else
            {
                    if(AV_delay > 0 && AV_delay < 10)
                    {
                            seek_sync_flag = 0;
                            wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                            avsync_timeout_cnt = 0;
                    }
                    else if(AV_delay < 0 && AV_delay > -10)
                    {
                            seek_sync_flag = 0;
                            wait_video_or_audio_sync = AVSYNC_AUDIO_CATCH_VIDEO|AVSYNC_NOSOUND;
                            avsync_timeout_cnt = 0;
                    }
            }
        }
    } else {
        // No audio:

        if (switch_audio_thread_status)
        {
//printf("switch_audio_thread_status=%d last_pulldown32_vft_diff = %d\n", switch_audio_thread_status, last_pulldown32_vft_diff);
                    last_pulldown32_vft_diff = 0;
        }

	if (mpctx->stream->type ==STREAMTYPE_DVDNAV )
			my_current_pts = mpctx->sh_video->pts; //the cur pts using v-pts while no audio 
//printf("mpctx->sh_video->pts[%f]\n",mpctx->sh_video->pts);
	seek_sync_flag = 0;
        if (speed_mult == 0)	//Barry 2010-09-17
        {
            if (sky_vdec_wait_sync_vpts > 0.0)
            {
//			if (a_pts >= sky_vdec_wait_sync_vpts)
                {
                    printf("@@@ Hay  got sync!!! unlock display\n");
                    sky_set_vsync_status(0);
                    sky_set_vsync_status(1);
                    sky_vdec_wait_sync_vpts = 0.0;
                    time1 = 0;
                }
            }
        }
        else
        {
            if(sky_vdec_wait_sync_vpts > 0.0)
            {
                printf("@@@ Coerce!!! unlock display\n");
                sky_set_vsync_status(0);
                sky_set_vsync_status(1);
                sky_vdec_wait_sync_vpts = 0.0;
                time1 = 0;
            }

        }

        if (!quiet)
        {
//Robert 20101217 reduce print_status cpu loading
            static double last_noa_vpts=0.0;
            if (mpctx->d_video && last_noa_vpts != mpctx->d_video->pts )
            {
                print_status(0, 0, 0);
                last_noa_vpts = mpctx->d_video->pts;
            }
        }

#if 0	//Robert 20101118 not yet support nrb.timeout
//#ifdef _SKY_VDEC_V1	//Barry 2010-08-05
        if(mpctx->sh_video && (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS))
        {
            svread(0, &sky_nrb, 0);

            if (no_osd)	//thumbnail mode
            {
                if (sky_nrb.timeout_current > 15)
                {
                    //printf("thumbnail    === tr timeout : (%d, %d)\n", sky_nrb.timeout_current, sky_nrb.timeout_total);
                    rel_seek_secs += 1;
                }
            }
            else		//nosound
            {
                if (sky_nrb.timeout_current > 5)
                {
                    //printf("nosound    === tr timeout : (%d, %d)\n", sky_nrb.timeout_current, sky_nrb.timeout_total);
                    rel_seek_secs += 1;
                }
            }
        }
#endif
    }
}

static int fill_audio_out_buffers(void)
{
    unsigned int t;
    double tt;
    int playsize;
    int playflags=0;
    int audio_eof=0;
    int bytes_to_write;
    int decode_len;
    sh_audio_t * const sh_audio = mpctx->sh_audio;
    current_module="play_audio";
#ifdef SUPPORT_DIVX_DRM
	static int first_check = 1;
#endif /* end of SUPPORT_DIVX_DRM */

#ifdef CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
    //if (mpctx->d_audio->eof)
    if (mpctx->demuxer->audio->id == -2 || !mpctx->audio_out)
        return 1;
#endif // end of CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
	if (!mpctx->audio_out)
		return 1;

    while (1) {
        //int sleep_time;
        // all the current uses of ao_data.pts seem to be in aos that handle
        // sync completely wrong; there should be no need to use ao_data.pts
        // in get_space()
        ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
        bytes_to_write = mpctx->audio_out->get_space();

        break;
//20100910 Robert can skip this now
#if 0
        if (mpctx->sh_video || (bytes_to_write >= ao_data.outburst))
        {
            break;
        }
//printf(" ???\n");
#if 1	//Fuchun 2009.12.02
        if(speed_mult != 0 && !mpctx->sh_video)
        {
            if(mpctx->stream->eof)
                mpctx->eof = PT_NEXT_ENTRY;
            break;
        }
#endif
        // handle audio-only case:
        // this is where mplayer sleeps during audio-only playback
        // to avoid 100% CPU use
        sleep_time = (ao_data.outburst - bytes_to_write) * 1000 / ao_data.bps;
        if (sleep_time < 10) sleep_time = 10; // limit to 100 wakeups per second
        usec_sleep(sleep_time * 1000);
#endif
    }

    last_audio_bytes_to_write = bytes_to_write;
//printf(" mpctx->audio_out->get_space() = %d  get_delay:%f\n", mpctx->audio_out->get_space(), mpctx->audio_out->get_delay());
    if (bytes_to_write < 2000*4)
    {
        return 1;
    }
#if 0
    else if (bytes_to_write < 6000*4)
    {
        frame_dropping = 1;
    }
#endif
//20101213 Robert don't sync while audio & video is not empty
//    else if ((mpctx->sh_video) && ((bytes_to_write >= 15840*4 && AV_delay > 0.15 && mpctx->demuxer->video->bytes > 1024*1024*2) || mpctx->demuxer->video->packs > 2000 || mpctx->demuxer->video->bytes > 1024*1024*6))
    else if ((mpctx->sh_video) && ((bytes_to_write >= 15840*4 && AV_delay > 0.15 && mpctx->demuxer->video->bytes > 1024*1024*3) || mpctx->demuxer->video->packs > 2000 || (mpctx->demuxer->video->bytes > 1024*1024*6 && AV_delay > 0.033*(sky_nrb.qlen[2]+1))))
    {
        if (sky_nrb.qlen[2] == 0)
        {
		if(mpctx->demuxer->type != DEMUXER_TYPE_ASF)
			wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO|AVSYNC_NOVIDEO;
		return 1;
        }
    }

    if ( !(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && bytes_to_write < 4224*6 && (mpctx->sh_video) && (svsd_state->queue_length < 2))
    {
        return 1;
    }

    //Barry 2011-07-07
    if (mpctx->stream->type == STREAMTYPE_DVDNAV && mp_dvdnav_is_stop(mpctx->stream))
    {
    	mpctx->eof = 1;
	return 1;
    }

    while (bytes_to_write) {
        playsize = bytes_to_write;
        if (playsize > MAX_OUTBURST)
            playsize = MAX_OUTBURST;

        // Fill buffer if needed:
        current_module="decode_audio";
        t = GetTimer();
//printf("## bf decode sh_audio->a_out_buffer_len=%d playsize=%d\n", sh_audio->a_out_buffer_len, playsize);
#ifdef NEW_AUDIO_FILL_BUFFER
        if ((sh_audio->w_a_out_buffer_len < sh_audio->r_a_out_buffer_len
                ? AUDIO_OUT_QUEUE_NUM*sh_audio->a_out_buffer_size - sh_audio->r_a_out_buffer_len + sh_audio->w_a_out_buffer_len
                : sh_audio->w_a_out_buffer_len - sh_audio->r_a_out_buffer_len) < playsize)
        {
#if 1
            if (playsize > 1024*16)
                decode_len = playsize;
            else
                decode_len = 1024*16;
#ifdef SUPPORT_DIVX_DRM
			/* C04, C05 av not sync because decode_len is too large, we need reduce decode_len to 4096. */
			/* 0x30355844 = DX50, for DivX certification only */
			if ( mpctx->sh_video && (mpctx->sh_video->format == 0x30355844) && sh_audio->wf && (sh_audio->wf->nChannels == 2) && ((sh_audio->wf->nSamplesPerSec == 16000) || (sh_audio->wf->nSamplesPerSec == 22050)) )
			{
				decode_len = 4096;
				if (first_check)
				{
					printf("@@@@ We set decode_len is [%d] @@@@ [%s][%d] \n", decode_len, __func__, __LINE__);
					first_check = 0;
				}
			}
#endif /* end of SUPPORT_DIVX_DRM */ 
#else
            decode_len = playsize;
#endif
//		if (decode_audio(sh_audio, playsize) < 0) // EOF or error


#ifdef  jfueng_2011_0311
            int re_val =0;
            if(audio_long_period)
            	{
                a_long_quiet_SP_yes = 1; 
		   a_long_quiet_SP_cnt = 0;		
            	}		
	     else
	     	{
                 a_long_quiet_SP_cnt++;
		    if (a_long_quiet_SP_cnt > 100)    a_long_quiet_SP_yes = 0;		//OLD ::500 OK!
				
		 }
			
            if(a_long_quiet_SP_yes)
            	{
            	 //printf("\n  decode...1024*4......    ");
              re_val =   decode_audio(sh_audio, 1024* 4) ;  
            	}	  
	     else
	     	{
            	 //printf("\n  decode...decode_len......    ");	     	
       	 re_val =   decode_audio(sh_audio,decode_len) ;	
	     	}

              //JF  printf("%d  ← Mplayer::decode_audio",re_val);

            if ( re_val ==  0)   
		{
		    //JF  frame_drop_lock = 1;       
		    break;
            	}  
	      if ( re_val   < 0 ) // EOF or error 		
	       {
			if (mpctx->d_audio->eof) {
				audio_eof = 1;
				if ((sh_audio->w_a_out_buffer_len < sh_audio->r_a_out_buffer_len
					? AUDIO_OUT_QUEUE_NUM*sh_audio->a_out_buffer_size - sh_audio->r_a_out_buffer_len + sh_audio->w_a_out_buffer_len
					: sh_audio->w_a_out_buffer_len - sh_audio->r_a_out_buffer_len)  == 0)
			      {
				
			         // mingyu 2011-3-10
			        if (mpctx->stream->type !=STREAMTYPE_DVDNAV )
			        {				

					//Barry 2010-12-29
					if (mpctx->d_video->eof && mpctx->d_audio->eof)
						mpctx->eof = 1;
			        }
					
			      return 0;
                         }
			}

#else
            if (decode_audio(sh_audio, decode_len) < 0) // EOF or error
            {
                if (mpctx->d_audio->eof) {
                    audio_eof = 1;
                    if ((sh_audio->w_a_out_buffer_len < sh_audio->r_a_out_buffer_len
                            ? AUDIO_OUT_QUEUE_NUM*sh_audio->a_out_buffer_size - sh_audio->r_a_out_buffer_len + sh_audio->w_a_out_buffer_len
                            : sh_audio->w_a_out_buffer_len - sh_audio->r_a_out_buffer_len)  == 0)
		    {
			// mingyu 2011-3-10
			if (mpctx->stream->type !=STREAMTYPE_DVDNAV )
			{
			    //Barry 2010-12-29
			    if (mpctx->d_video->eof && mpctx->d_audio->eof)
				mpctx->eof = 1;
			}

                        return 0;
                    }
                }

#endif
                else if(aserver_crash && mpctx->sh_video) // only reinit audio if video is available, otherwise mplayer may crash
                {
                    mpctx->sh_audio->initialized = 0;
                    uninit_player(INITIALIZED_ACODEC);
                    usleep(1500);
                    aserver_crash = 0;
                    return 1;
                }
            }
        }

        if (benchmark)
        {
            t = GetTimer() - t;
            tt = t*0.000001f;
            audio_time_usage+=tt;

            // Raymond 2009/08/03
            if(print_audio_decode_time && t > 0)
                printf("\t\t\t\t\tA_Dec = %3d ms\n", t/1000);
        }

        int w_len = sh_audio->w_a_out_buffer_len, total_out_size = sh_audio->a_out_buffer_size;
        if (playsize > (w_len< sh_audio->r_a_out_buffer_len ? AUDIO_OUT_QUEUE_NUM*total_out_size - sh_audio->r_a_out_buffer_len + w_len
                        : w_len - sh_audio->r_a_out_buffer_len))
        {
            playsize = (w_len< sh_audio->r_a_out_buffer_len ? AUDIO_OUT_QUEUE_NUM*total_out_size - sh_audio->r_a_out_buffer_len + w_len
                        : w_len - sh_audio->r_a_out_buffer_len);
            if (audio_eof)
                playflags |= AOPLAY_FINAL_CHUNK;
        }
        if (!playsize)
        {
#ifdef CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
#ifdef QT_SUPPORT_DVBT
            if (mpctx->demuxer->file_format == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS)
#else /* else of QT_SUPPORT_DVBT */			
            if (mpctx->demuxer->file_format == DEMUXER_TYPE_MPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */			
            {
                audio_eof = 1;
                wait_video_or_audio_sync = AVSYNC_NORMAL;
                mpctx->d_audio->eof = 1;
                mpctx->demuxer->audio->id = -2;
                frame_dropping = 0;
                seek_sync_flag = 0;
                printf("@@@@ decode audio failed, we got playsize zero in [%s] [%d], turn off audio @@@@@\n", __func__, __LINE__);
            }
            break;
#endif	//end of CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
        }

        // play audio:
        current_module="play_audio";

        // Is this pts value actually useful for the aos that access it?
        // They're obviously badly broken in the way they handle av sync;
        // would not having access to this make them more broken?
        ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;

        //Fuchun 2010.07.23 A/V quickly sync after seek
        if(((wait_video_or_audio_sync&AVSYNC_NOSOUND) /*&& !(wait_video_or_audio_sync&AVSYNC_BEGINNING)*/)
                || (seek_sync_flag == 1 || seek_sync_flag == 3 || seek_sync_flag == 4))
        {
            sh_audio->r_a_out_buffer_len = sh_audio->w_a_out_buffer_len;
            mpctx->delay = 0;
            return 1;
        }
        else if(audio_speed >= 3 || audio_speed < 0)
        {
            sh_audio->r_a_out_buffer_len = sh_audio->w_a_out_buffer_len;
            mpctx->delay = 0;
            return 1;
        }
        //else if((seek_sync_flag == 2 && AV_delay > 0.0) || (seek_sync_flag == 5))
		else if((seek_sync_flag == 2 ) || (seek_sync_flag == 5))
		{
			/* Fixed some ts file audio in and out when playback at begining ex: Ghost Rider.ts, 007-Casino Royale_clip01.ts*/
			if (AV_delay < 0.0)
			{
				sh_audio->r_a_out_buffer_len = sh_audio->w_a_out_buffer_len;
				mpctx->delay = 0;
			}
            return 1;
        }
        else
        {
#if 0
            //FUchun 2010.12.06 avoid a_pts skip too large when A/V quickly sync
            if(((((wait_video_or_audio_sync&AVSYNC_NOSOUND) && (wait_video_or_audio_sync&AVSYNC_BEGINNING)) || seek_sync_flag == 2) && !mpctx->mixer.muted)
                    || ((wait_video_or_audio_sync&AVSYNC_NORMAL) && seek_sync_flag == 0 && mpctx->mixer.muted))
            {
                mixer_mute(&mpctx->mixer);
                mpctx->user_muted = mpctx->mixer.muted;
            }
#endif
            if((AUDIO_OUT_QUEUE_NUM*total_out_size - sh_audio->r_a_out_buffer_len) < playsize)
            {
                int memmove_len = playsize - (AUDIO_OUT_QUEUE_NUM*total_out_size - sh_audio->r_a_out_buffer_len);
                memmove(&sh_audio->a_out_buffer[AUDIO_OUT_QUEUE_NUM*total_out_size], sh_audio->a_out_buffer, memmove_len);
            }
	    if ((mpctx->d_audio->eof) && (last_audio_bytes_to_write > playsize))
	    {
		// if data size not enough, will cause some output garbage
		//printf("data size not enough: %d %d\n", last_audio_bytes_to_write, playsize);
	    } else {
//20110706 Robert
	        if (wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO)
	        {
	            memset(&sh_audio->a_out_buffer[sh_audio->r_a_out_buffer_len], 0x00, playsize);
		    playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[sh_audio->r_a_out_buffer_len], playsize, playflags);
		    playsize = 0;
		}
		else
		{
		    playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[sh_audio->r_a_out_buffer_len], playsize, playflags);
		}
	    }
        }

        if (playsize > 0) {
            bytes_to_write -= playsize;
#if 0
            if (bytes_to_write > 5000)
            {
                int new_playsize = 0;
                if (bytes_to_write > (sh_audio->a_out_buffer_len - playsize))
                {
                    new_playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[playsize], (sh_audio->a_out_buffer_len - playsize), playflags);
                }
                else
                {
                    new_playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[playsize], bytes_to_write, playflags);
                }
                if (new_playsize > 0)
                {
                    playsize += new_playsize;
                }
            }
#endif
            last_audio_bytes_to_write = playsize;

            sh_audio->r_a_out_buffer_len += playsize;
            if (sh_audio->r_a_out_buffer_len >= AUDIO_OUT_QUEUE_NUM*total_out_size)
            {
                sh_audio->r_a_out_buffer_len = sh_audio->r_a_out_buffer_len - AUDIO_OUT_QUEUE_NUM*total_out_size;
            }
            mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
        }
        else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
            // Sanity check to avoid hanging in case current ao doesn't output
            // partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
            mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
			printf("@@@@@@@@@@@@@@@@@@@@@ in [%s][%d] reset audio index @@@@@@@@\n", __func__, __LINE__);
            sh_audio->r_a_out_buffer_len = sh_audio->w_a_out_buffer_len;
        }
#else	//else of NEW_AUDIO_FILL_BUFFER
        if (sh_audio->a_out_buffer_len < playsize)
        {
#if 1
            if (playsize > 1024*16)
                decode_len = playsize;
            else
                decode_len = 1024*16;

#else
        decode_len = playsize;
#endif
//	if (decode_audio(sh_audio, playsize) < 0) // EOF or error
            if (decode_audio(sh_audio, decode_len) < 0) // EOF or error
            {
                if (mpctx->d_audio->eof)
                {
                    audio_eof = 1;
                    if (sh_audio->a_out_buffer_len == 0)
                    {
			// mingyu 2011-3-10
			if (mpctx->stream->type !=STREAMTYPE_DVDNAV )
			{
			    //Barry 2010-12-29
			    if (mpctx->d_video->eof && mpctx->d_audio->eof)
				mpctx->eof = 1;
			}

                        return 0;
                    }
                }
                else if(aserver_crash && mpctx->sh_video) // only reinit audio if video is available, otherwise mplayer may crash
                {
                    mpctx->sh_audio->initialized = 0;
                    uninit_player(INITIALIZED_ACODEC);
                    usleep(1500);
                    aserver_crash = 0;
                    return 1;
                }
            }
        }

        if (benchmark)
        {
            t = GetTimer() - t;
            tt = t*0.000001f;
            audio_time_usage+=tt;

            // Raymond 2009/08/03
            if(print_audio_decode_time && t > 0)
            {
                printf("\tA_Dec = %3d ms, dec len = %d  ", t/1000, decode_len);
                printf("btw=%d  get_space=%d\n", bytes_to_write, mpctx->audio_out->get_space());
//		printf("\t\t\t\t\tA_Dec = %3d ms, dec len = %d\n", t/1000, decode_len);
            }
        }

        if (playsize > sh_audio->a_out_buffer_len) {
            playsize = sh_audio->a_out_buffer_len;
            if (audio_eof)
                playflags |= AOPLAY_FINAL_CHUNK;
        }
        if (!playsize)
        {
#ifdef CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
#ifdef QT_SUPPORT_DVBT
            if (mpctx->demuxer->file_format == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS)
#else /* else of QT_SUPPORT_DVBT */		
            if (mpctx->demuxer->file_format == DEMUXER_TYPE_MPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */			
            {
                audio_eof = 1;
                wait_video_or_audio_sync = AVSYNC_NORMAL;
                mpctx->d_audio->eof = 1;
                mpctx->demuxer->audio->id = -2;
                frame_dropping = 0;
                seek_sync_flag = 0;
                printf("@@@@ decode audio failed, we got playsize zero in [%s] [%d], turn off audio @@@@@\n", __func__, __LINE__);
            }
#endif // end of CHECK_AUDIO_DECODE_FAILED_CLOSE_AUDIO
            break;
        }

        // play audio:
        current_module="play_audio";

        // Is this pts value actually useful for the aos that access it?
        // They're obviously badly broken in the way they handle av sync;
        // would not having access to this make them more broken?
        ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;

//Fuchun 2010.07.23 A/V quickly sync at beginning or after seek
        if(((wait_video_or_audio_sync&AVSYNC_NOSOUND) /*&& !(wait_video_or_audio_sync&AVSYNC_BEGINNING)*/)
                || (seek_sync_flag == 1 || seek_sync_flag == 3 || seek_sync_flag == 4))
        {
            sh_audio->a_out_buffer_len = 0;
            mpctx->delay = 0;
            return 1;
        }
        else if(audio_speed >= 3 || audio_speed < 0)
        {
            sh_audio->a_out_buffer_len = 0;
            mpctx->delay = 0;
            return 1;
        }
        else if(seek_sync_flag == 2)
        {
            return 1;
        }
        else
        {
#if 0
            if(((((wait_video_or_audio_sync&AVSYNC_NOSOUND) && (wait_video_or_audio_sync&AVSYNC_BEGINNING)) || seek_sync_flag == 2) && !mpctx->mixer.muted)
                    || ((wait_video_or_audio_sync&AVSYNC_NORMAL) && seek_sync_flag == 0 && mpctx->mixer.muted))
            {
                mixer_mute(&mpctx->mixer);
                mpctx->user_muted = mpctx->mixer.muted;
            }
#endif
            playsize = mpctx->audio_out->play(sh_audio->a_out_buffer, playsize, playflags);
        }

        if (playsize > 0) {
            bytes_to_write -= playsize;

            if (bytes_to_write > 5000)
            {
                int new_playsize = 0;
                if (bytes_to_write > (sh_audio->a_out_buffer_len - playsize))
                {
                    new_playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[playsize], (sh_audio->a_out_buffer_len - playsize), playflags);
                }
                else
                {
                    new_playsize = mpctx->audio_out->play(&sh_audio->a_out_buffer[playsize], bytes_to_write, playflags);
                }
                if (new_playsize > 0)
                {
                    playsize += new_playsize;
                }
            }

            //last_audio_bytes_to_write = bytes_to_write - playsize;
            last_audio_bytes_to_write -= playsize;	//Barry 2010-09-17


            sh_audio->a_out_buffer_len -= playsize;
            if (sh_audio->a_out_buffer_len>0)
            {
                memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
                        sh_audio->a_out_buffer_len);
            }
            mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
        }
        else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
            // Sanity check to avoid hanging in case current ao doesn't output
            // partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
            mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
            sh_audio->a_out_buffer_len = 0;
        }
#endif	//end of NEW_AUDIO_FILL_BUFFER
        bytes_to_write = 0;
    }
    return 1;
}

float total_audio_out_bytes=0.0, total_new_out=0.0;
//Robert 20100324 apply dynamic resample
static int fill_audio_out_buffers_with_resamp(int mute)
{
#if 0
    return fill_audio_out_buffers();
#else
    unsigned int t;
    double tt;
    int playsize;//, newsize, played_size;
    int playflags=0;
    int audio_eof=0;
    int bytes_to_write;
    sh_audio_t * const sh_audio = mpctx->sh_audio;
//    static int status_cnts=0;
//    float check_diff_time=0.0;
//    static float total_audio_out_bytes=0.0, total_new_out=0.0;
//    struct timeval tv, tv1;
//    static struct timeval *tv2;

    current_module="play_audio";

//20100806 Robert fix DVB-T blocking issue when unplug cable in playing
//    if (mpctx->d_audio->packs == 0 && dvb_get_aq_count() == 0)
    if (mpctx->d_audio->packs == 0)
    {
        return 1;
    }
    /*
        if (tv2 == 0)
        {
            tv2 = malloc(sizeof(struct timeval));
            gettimeofday(tv2, 0);
        }
    */
    while (1) {
        // all the current uses of ao_data.pts seem to be in aos that handle
        // sync completely wrong; there should be no need to use ao_data.pts
        // in get_space()
        ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
        if (mpctx->audio_out)
            bytes_to_write = mpctx->audio_out->get_space();
        else
            printf("### In [%s][%d] can't access get_space function ###\n", __func__, __LINE__);
        if (mpctx->sh_video || bytes_to_write >= ao_data.outburst)
            break;

#if 1	//Fuchun 2009.12.02
        if(speed_mult != 0 && !mpctx->sh_video)
        {
            if(mpctx->stream->eof)
                mpctx->eof = PT_NEXT_ENTRY;
            break;
        }
#endif

        // handle audio-only case:
        // this is where mplayer sleeps during audio-only playback
        // to avoid 100% CPU use
        usec_sleep(10000); // Wait a tick before retry
    }

//    bytes_to_write >>= 2;	// Raymond 2009/08/03
    last_audio_bytes_to_write = bytes_to_write;

    if (bytes_to_write < 2000*4)
//    if (bytes_to_write < 1000)
    {
//        printf("\n\n========\n\n======= bytes_to_write = %d\n\n======\n", bytes_to_write);
//        printf("bytes_to_write = %d\n", bytes_to_write);
        return 1;
    }
#if 0
    else if (bytes_to_write < 6000)
    {
        frame_dropping = 1;
    }
#endif
//    else if ((mpctx->sh_video) && ((bytes_to_write >= 15840 && AV_delay > 0.15 && mpctx->demuxer->video->bytes > 1024*1024*2)|| (mpctx->demuxer->video->packs > 2000 || mpctx->demuxer->video->bytes > 1024*1024*6) ))
    else if ((mpctx->sh_video) && ((bytes_to_write >= 15840*4 && AV_delay > 0.15 && mpctx->demuxer->video->bytes > 1024*1024*3) || mpctx->demuxer->video->packs > 2000 || (mpctx->demuxer->video->bytes > 1024*1024*6 && AV_delay > 0.033*(sky_nrb.qlen[2]+1))))
    {
//        wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO|AVSYNC_NOVIDEO;
//        return 1;
        if (sky_nrb.qlen[2] == 0)
        {
            wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO|AVSYNC_NOVIDEO;
            return 1;
        }
    }

    if ( !(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && bytes_to_write < 4224*6 && (mpctx->sh_video) && (svsd_state->queue_length < 2))
    {
        return 1;
    }

    while (bytes_to_write) {
        playsize = bytes_to_write;
        if (playsize > MAX_OUTBURST)
            playsize = MAX_OUTBURST;
        if (playsize >= 1024*16)
        {
            playsize = 1024*16;
        }
        bytes_to_write -= playsize;

        // Fill buffer if needed:
        current_module="decode_audio";
        t = GetTimer();
        if (decode_audio_with_resamp(sh_audio, playsize, mpctx->demuxer, mute) < 0) // EOF or error
        {
            if (mpctx->d_audio->eof) {
                audio_eof = 1;
                if (sh_audio->a_out_buffer_len == 0)
                    return 0;
            }
            else if(aserver_crash && mpctx->sh_video) // only reinit audio if video is available, otherwise mplayer may crash
            {
                mpctx->sh_audio->initialized = 0;
                uninit_player(INITIALIZED_ACODEC);
                usleep(1500);
                aserver_crash = 0;
                return 1;
            }
        }

        if (benchmark)
        {
            t = GetTimer() - t;
            tt = t*0.000001f;
            audio_time_usage+=tt;

            // Raymond 2009/08/03
//	if(print_audio_decode_time && t > 0)
            if(print_audio_decode_time && t > 20000)
                printf("\t\t\t\t\tA_Dec = %3d ms\n", t/1000);
        }

//20100519 Robert remove audio debug message
        /*
                gettimeofday(&tv1, 0);
                check_diff_time = (float)(tv1.tv_sec - tv2->tv_sec);
                if (tv1.tv_usec > tv2->tv_usec)
                {
                    check_diff_time += (float)(tv1.tv_usec - tv2->tv_usec)/1000000;
                }
                else
                {
                    check_diff_time -= 1.0;
                    check_diff_time += (float)(1000000 - tv2->tv_usec + tv1.tv_usec)/1000000;
                }

                if ( ++status_cnts == (4*5*4))
                {
                    if (check_diff_time > 1)
                        printf("audio sample rate: %f (bytes/4)/sec  new= %f\n", (total_audio_out_bytes/4)/check_diff_time, (total_new_out/4)/check_diff_time);
                    status_cnts = 0;
                }
                if (check_diff_time > 30)
                {
        printf("status reset\n");
                    total_audio_out_bytes = 0;
                    total_new_out = 0;
                    gettimeofday(tv2, 0);
                }
        */

        if (playsize > sh_audio->a_out_buffer_len) {
            playsize = sh_audio->a_out_buffer_len;
            if (audio_eof)
                playflags |= AOPLAY_FINAL_CHUNK;
        }
        if (!playsize)
            break;

        // play audio:
        current_module="play_audio";

        // Is this pts value actually useful for the aos that access it?
        // They're obviously badly broken in the way they handle av sync;
        // would not having access to this make them more broken?
        ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
        if (mute == 1)
        {
            sh_audio->a_out_buffer_len = 0;
            mpctx->delay = 0;
            return 1;
        }
        else if(dvbt_wait_sync != 0 || (seek_sync_flag == 1 || seek_sync_flag == 3 || seek_sync_flag == 4))
        {
            sh_audio->a_out_buffer_len = 0;
            mpctx->delay = 0;
            return 1;
        }
#if 0 /* never meet seek_sync_flag = 2 in resample case */		
        else if(seek_sync_flag == 2 && AV_delay > 0.0)
        {
            return 1;
        }
#endif 		
        else
        {
            playsize = mpctx->audio_out->play(sh_audio->a_out_buffer, playsize, playflags);
        }
        if (playsize > 0) {
            sh_audio->a_out_buffer_len -= playsize;
            if (sh_audio->a_out_buffer_len>0)
                memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize], sh_audio->a_out_buffer_len);
            mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
        }
        else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
            // Sanity check to avoid hanging in case current ao doesn't output
            // partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
            mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
            sh_audio->a_out_buffer_len = 0;
        }
    }
    return 1;
#endif
}

static int sleep_until_update_original(float *time_frame, float *aq_sleep_time)
{
    int frame_time_remaining = 0;
    current_module="calc_sleep_time";

    *time_frame -= GetRelativeTime(); // reset timer

    if (mpctx->sh_audio && !mpctx->d_audio->eof && mpctx->audio_out) {
        float delay = mpctx->audio_out->get_delay();

        mp_dbg(MSGT_AVSYNC, MSGL_DBG2, "delay=%f\n", delay);

        if (autosync) {
            /*
             * Adjust this raw delay value by calculating the expected
             * delay for this frame and generating a new value which is
             * weighted between the two.  The higher autosync is, the
             * closer to the delay value gets to that which "-nosound"
             * would have used, and the longer it will take for A/V
             * sync to settle at the right value (but it eventually will.)
             * This settling time is very short for values below 100.
             */
            float predicted = mpctx->delay / playback_speed + *time_frame;
            float difference = delay - predicted;
            delay = predicted + difference / (float)autosync;
        }

        *time_frame = delay - mpctx->delay / playback_speed;

        // delay = amount of audio buffered in soundcard/driver
        if (delay > 0.25) delay=0.25;
        else if (delay < 0.10) delay=0.10;
        if (*time_frame > delay*0.6) {
            // sleep time too big - may cause audio drops (buffer underrun)
            frame_time_remaining = 1;
            *time_frame = delay*0.5;
        }
    } else {
        // If we're lagging more than 200 ms behind the right playback rate,
        // don't try to "catch up".
        // If benchmark is set always output frames as fast as possible
        // without sleeping.
        if (*time_frame < -0.2 || benchmark)
            *time_frame = 0;
    }

    *aq_sleep_time += *time_frame;


    //============================== SLEEP: ===================================

    // flag 256 means: libvo driver does its timing (dvb card)
    if (*time_frame > 0.001 && !(vo_flags&256))
        *time_frame = timing_sleep(*time_frame);
    return frame_time_remaining;
}

static int sleep_until_update(float *time_frame, float *aq_sleep_time)
{
    int frame_time_remaining = 0;
    current_module="calc_sleep_time";
    *time_frame -= GetRelativeTime(); // reset timer

    float delay = 0.0;

    if (mpctx->sh_audio && !mpctx->d_audio->eof && mpctx->audio_out) {
        //float delay = mpctx->audio_out->get_delay();
        delay = mpctx->audio_out->get_delay();
        mp_dbg(MSGT_AVSYNC, MSGL_DBG2, "delay=%f\n", delay);

        if (autosync) {
            /*
             * Adjust this raw delay value by calculating the expected
             * delay for this frame and generating a new value which is
             * weighted between the two.  The higher autosync is, the
             * closer to the delay value gets to that which "-nosound"
             * would have used, and the longer it will take for A/V
             * sync to settle at the right value (but it eventually will.)
             * This settling time is very short for values below 100.
             */
            float predicted = mpctx->delay / playback_speed + *time_frame;
            float difference = delay - predicted;
            delay = predicted + difference / (float)autosync;
        }
        *time_frame = delay - mpctx->delay / playback_speed;
//20101213 Robert don't sleep while audio is not enough
//modify this later with get_delay()
//printf("get_space = %d\n", mpctx->audio_out->get_space());
#if 0
        if (mpctx->audio_out->get_space() > 4224*5)
        {
            *time_frame = 0.0;
        }
#endif
//printf("*time_frame=%f  mpctx->sh_video->next_frame_time=%f drop=%d\n", *time_frame, mpctx->sh_video->next_frame_time, check_framedrop_flag);
//printf("*time_frame=%f  next_f_time=%f drop=%d delay=%f\n", *time_frame, mpctx->sh_video->next_frame_time, check_framedrop_flag, delay);
        // delay = amount of audio buffered in soundcard/driver
        if (delay > 0.25) delay=0.25;
        else if (delay < 0.10) delay=0.10;

        if (*time_frame > delay*0.6) {
            // sleep time too big - may cause audio drops (buffer underrun)
#if 1
            double a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);

            svread(0, &sky_nrb, 0);

            sky_vdec_vq_ref_pts = sky_nrb.cur_pts;

            frame_drop_lock = 0;

#if 1
//if (svsd_state->queue_length > 0 && ((sky_vpts_history[(sky_vft_idx - (sky_nrb.qlen[0]) - svsd_state->queue_length + 1 + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER]-a_pts) > 0.04))
//            if (((sky_vpts_history[(sky_vft_idx - (sky_nrb.qlen[0]) - svsd_state->queue_length + 1 + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER]-a_pts) > 0.04))
//            if (((sky_nrb.cur_pts-a_pts) > 0.04))

//printf("sky_nrb.cur_pts:%f a_pts:%f v-a=%f\n", (float)sky_nrb.cur_pts, (float)a_pts, (float)(sky_nrb.cur_pts-a_pts));

            if (((sky_nrb.cur_pts-a_pts) > -0.01))
            {
//printf("sky_nrb.cur_pts:%f a_pts:%f v-a=%f\n", (float)sky_nrb.cur_pts, (float)a_pts, (float)(sky_nrb.cur_pts-a_pts));
                if (sky_nrb.cur_pts - a_pts > 5)
                {
                    frame_time_remaining = 0;
                }
                else
                {
                    frame_time_remaining = 1;
                    frame_drop_lock = 1;
//printf("frame_drop_lock=1 frame_time_remaining=1\n");
                }
            }
            else
#endif
            {
                frame_time_remaining = 0;
            }
            *time_frame = delay*0.5;
#else
            frame_time_remaining = 1;
            *time_frame = delay*0.5;
#endif

        }
        else
        {
            frame_time_remaining = 0;
            frame_drop_lock = 0;
//printf("AV_delay=%f  sky_nrb.qlen[2]=%d\n", (float)AV_delay, sky_nrb.qlen[2]);

//Robert 20110420 fine tune this later, check need drop b frame for H.264 with 1080P
            if ((sky_h264_need_check_dropB == 1) && (AV_delay > 0.20))
            {
                extern int sky_need_drop_b;
//                if (sky_nrb.qlen[2] == 0)
                {
//printf("AV_delay=%f  sky_nrb.qlen[2]=%d\n", (float)AV_delay, sky_nrb.qlen[2]);
                    sky_need_drop_b = 1;
//printf("MPlayer: sky_need_drop_b = 1\n");
                }
            }

        }

//20101213 Robert don't sleep too long
        if (*time_frame > 0.02)
            *time_frame = 0.015;//0.008;//0.02;

        *aq_sleep_time += *time_frame;

//printf("===> *time_frame=%f   AV_delay=%f\n", *time_frame, AV_delay);
        //============================== SLEEP: ===================================
        // flag 256 means: libvo driver does its timing (dvb card)

        if (*time_frame > 0.001 && !(vo_flags&256) && chk_bufspace(mpctx->demuxer) == 0)
        {
//printf("*time_frame=%f\n", *time_frame);
                *time_frame = timing_sleep(*time_frame);
        }
        else
        {
            if (chk_bufspace(mpctx->demuxer))
            {
                if (cvq_threshold > 2)
                {
                    if (delay >= 0.2 && svsd_state->queue_length > 2)
                    {
            //timing_sleep(0.005);
//printf(" usec_sleep(1003)");
//printf("=delay:%f queue_length=%d *time_f=%f  AV_=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        usec_sleep(1003);
                    }
                    else
                    {
//printf(" *** 01 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        if (delay >= 0.10 && sky_nrb.qlen[2] > 0 && svsd_state->queue_length >= 3)
                        {
                            usleep( 2000);
                        }
                        else
                        {
//printf(" *** 01 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        }
                    }
                }
                else
                {
//printf("cvq_threshold = %d delay=%f svsd_state->queue_length=%d\n", cvq_threshold, delay, svsd_state->queue_length);
                    if (delay >= 0.2 && svsd_state->queue_length>0)
                    {
//printf(" usec_sleep(1004)");
//printf("=delay:%f queue_length=%d *time_f=%f  AV_=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        usec_sleep(1004);
                    }
                    else
                    {
//printf(" *** 02 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                    }
                }
            }
            else
            {
                if (delay >= 0.2)
                {
//printf(" === 03 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                    usleep(2000);
                }
                else
                {
                    if (delay >= 0.10 && sky_nrb.qlen[2] > 0 && svsd_state->queue_length >= 3)
                            usleep( 2000);
                    
//printf(" === 05 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                }
            }
        //printf("~~no sleep display q:%d  svsd q:%d (%d:%d) audio %d apts:%f vpts:%f cur_pts:%f\n", sky_nrb.qlen[2], svsd_state->queue_length, svsd_state->send_count, svsd_state->recv_count, mpctx->audio_out->get_space(), mpctx->d_audio->pts, mpctx->sh_video->pts, sky_nrb.cur_pts);
        //printf("~~  time_frame %f  delay  %f  mpctx->delay %f\n", *time_frame, mpctx->audio_out->get_delay(), mpctx->delay);
        }

    } else {
        // If we're lagging more than 200 ms behind the right playback rate,
        // don't try to "catch up".
        // If benchmark is set always output frames as fast as possible
        // without sleeping.
        if (*time_frame < -0.2 || benchmark)
        {
            *time_frame = 0;
        }
        else
        {
            if (mpctx->sh_video)
                usleep(1000);
        }

//20101213 Robert don't sleep too long
        if (*time_frame > 0.02)
            *time_frame = 0.015;//0.008;//0.02;

        *aq_sleep_time += *time_frame;

        //============================== SLEEP: ===================================
        // flag 256 means: libvo driver does its timing (dvb card)
        if (*time_frame > 0.001 && !(vo_flags&256) && svsd_state->queue_length > 0)
        {
            *time_frame = timing_sleep(*time_frame);
        }
        
        frame_drop_lock = 0;
    }
//Robert 20110429 prevent audio/video locking
    if (seek_sync_flag==2)
    {
        frame_time_remaining = 0;
    }

    return frame_time_remaining;
}

static int sleep_until_update_dvdnav(float *time_frame, float *aq_sleep_time)
{
    int frame_time_remaining = 0;
    float delay_check_range = 0.0;
    float delay_value = 0.0;
    current_module="calc_sleep_time";
    *time_frame -= GetRelativeTime(); // reset timer

    float delay = 0.0;
    extern int dvdloop_lock_display;

    if (mpctx->sh_audio && !mpctx->d_audio->eof && mpctx->audio_out) {
        //float delay = mpctx->audio_out->get_delay();
        delay = mpctx->audio_out->get_delay();
        mp_dbg(MSGT_AVSYNC, MSGL_DBG2, "delay=%f\n", delay);

        if (autosync) {
            /*
             * Adjust this raw delay value by calculating the expected
             * delay for this frame and generating a new value which is
             * weighted between the two.  The higher autosync is, the
             * closer to the delay value gets to that which "-nosound"
             * would have used, and the longer it will take for A/V
             * sync to settle at the right value (but it eventually will.)
             * This settling time is very short for values below 100.
             */
            float predicted = mpctx->delay / playback_speed + *time_frame;
            float difference = delay - predicted;
            delay = predicted + difference / (float)autosync;
        }

        *time_frame = delay - mpctx->delay / playback_speed;

//20101213 Robert don't sleep while audio is not enough
//modify this later with get_delay()
//printf("get_space = %d\n", mpctx->audio_out->get_space());
#if 0
        if (mpctx->audio_out->get_space() > 4224*5)
        {
            *time_frame = 0.0;
        }
#endif
//printf("*time_frame=%f  mpctx->sh_video->next_frame_time=%f drop=%d\n", *time_frame, mpctx->sh_video->next_frame_time, check_framedrop_flag);
        // delay = amount of audio buffered in soundcard/driver
        if (delay > 0.25) delay=0.25;
        else if (delay < 0.10) delay=0.10;

        if (*time_frame > delay*0.6) {
            // sleep time too big - may cause audio drops (buffer underrun)
#if 1
            double a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);

            svread(0, &sky_nrb, 0);

            sky_vdec_vq_ref_pts = sky_nrb.cur_pts;

            frame_drop_lock = 0;

#if 1
//if (svsd_state->queue_length > 0 && ((sky_vpts_history[(sky_vft_idx - (sky_nrb.qlen[0]) - svsd_state->queue_length + 1 + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER]-a_pts) > 0.04))
//            if (((sky_vpts_history[(sky_vft_idx - (sky_nrb.qlen[0]) - svsd_state->queue_length + 1 + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER]-a_pts) > 0.04))
//            if (((sky_nrb.cur_pts-a_pts) > 0.04))

//printf("sky_nrb.cur_pts:%f a_pts:%f v-a=%f\n", (float)sky_nrb.cur_pts, (float)a_pts, (float)(sky_nrb.cur_pts-a_pts));
//printf("## sky_nrb.cur_pts:%f v-a=%f mpts-cur:%f\n", (float)sky_nrb.cur_pts, (float)(sky_nrb.cur_pts-a_pts), mpctx->sh_video->pts - sky_nrb.cur_pts);
//printf("## sky_nrb.cur_pts:%f  mpts-cur:%f\n", (float)sky_nrb.cur_pts, (float)( mpctx->sh_video->pts - sky_nrb.cur_pts));
/*
            if (sky_avsync_method == 0)
            {
                    delay_value = mpctx->sh_video->pts - a_pts;
                    delay_check_range = -0.14;//0.10;
            }
            else if (sky_avsync_method == 2)
            {
                    delay_value = sky_nrb.cur_pts - a_pts;
                    delay_check_range = -0.01;
            }
            else if (sky_avsync_method == 1)
            {
                    delay_value = sky_vdec_wait_sync_vpts - a_pts;
                    delay_check_range = -0.01;
            }
*/
//            if (delay_value > delay_check_range)
#ifdef CONFIG_DEBUG_AVSYNC
            if (seek_sync_flag > 0)//seek_sync_flag == 6 || seek_sync_flag == 5)
            {
                    printf("\n#### seek flag : %d,   cur_pts=%f  sh vpts:%f  a_pts:%f\n", seek_sync_flag, sky_nrb.cur_pts, mpctx->sh_video->pts, a_pts);
                    printf("#### *time_frame=%f  delay=%f   mpctx->delay=%f\n", *time_frame, delay, mpctx->delay);
            }
#endif
//            if ((seek_sync_flag >= 5) && ((sky_nrb.cur_pts-a_pts) > -0.01))
            if ((dvdloop_lock_display == 0) && (seek_sync_flag > 0) && (fabs(sky_nrb.cur_pts-mpctx->sh_video->pts)<0.5) && (AV_delay < -0.06))
            {
                if (delay_value > 5)
                {
                    frame_time_remaining = 0;
                }
                else
                {
                    frame_time_remaining = 1;
                    frame_drop_lock = 1;
#ifdef CONFIG_DEBUG_AVSYNC
printf("mpctx->audio_out->get_delay()=%f\n", mpctx->audio_out->get_delay());
#endif
                }
            }
            else
#endif
            {
                frame_time_remaining = 0;
            }
            *time_frame = delay*0.5;
#else
            frame_time_remaining = 1;
            *time_frame = delay*0.5;
#endif

        }
        else
        {
            frame_time_remaining = 0;
            frame_drop_lock = 0;
//printf("AV_delay=%f  sky_nrb.qlen[2]=%d\n", (float)AV_delay, sky_nrb.qlen[2]);

//Robert 20110420 fine tune this later, check need drop b frame for H.264 with 1080P
            if ((sky_h264_need_check_dropB == 1) && (AV_delay > 0.20))
            {
                extern int sky_need_drop_b;
//                if (sky_nrb.qlen[2] == 0)
                {
//printf("AV_delay=%f  sky_nrb.qlen[2]=%d\n", (float)AV_delay, sky_nrb.qlen[2]);
                    sky_need_drop_b = 1;
//printf("MPlayer: sky_need_drop_b = 1\n");
                }
            }

        }

//20101213 Robert don't sleep too long
        if (*time_frame > 0.02)
            *time_frame = 0.015;//0.008;//0.02;

        *aq_sleep_time += *time_frame;

//printf("===> *time_frame=%f   AV_delay=%f\n", *time_frame, AV_delay);
        //============================== SLEEP: ===================================
        // flag 256 means: libvo driver does its timing (dvb card)

        if (seek_sync_flag >= 5)
        {
                if (!quiet)
                {
                        printf("\n ######Don't sleep while SEEK sync flag :%d *time_frame=%f\n", seek_sync_flag, *time_frame);
                }
                *time_frame = 0;
                frame_drop_lock = 0;
                frame_time_remaining = 0;
                return 0;
        }

        if (*time_frame > 0.001 && !(vo_flags&256) && chk_bufspace(mpctx->demuxer) == 0)
        {
//printf("*time_frame=%f\n", *time_frame);
                *time_frame = timing_sleep(*time_frame);
        }
        else
        {
            if (chk_bufspace(mpctx->demuxer))
            {
                if (cvq_threshold > 2)
                {
                    if (delay >= 0.2 && svsd_state->queue_length > 2)
                    {
            //timing_sleep(0.005);
//printf(" usec_sleep(1003)");
//printf("=delay:%f queue_length=%d *time_f=%f  AV_=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        usec_sleep(1003);
                    }
                    else
                    {
//printf(" *** 01 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                    }
                }
                else
                {
//printf("cvq_threshold = %d delay=%f svsd_state->queue_length=%d\n", cvq_threshold, delay, svsd_state->queue_length);
                    if (delay >= 0.2 && svsd_state->queue_length>0)
                    {
//printf(" usec_sleep(1004)");
//printf("=delay:%f queue_length=%d *time_f=%f  AV_=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                        usec_sleep(1004);
                    }
                    else
                    {
//printf(" *** 02 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                    }
                }
            }
            else
            {
                if (delay >= 0.2)
                {
//printf(" === 03 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                    usleep(2000);
                }
                else
                {
//printf(" === 05 =delay:%f queue_length=%d *time_frame=%f  AV_delay=%f\n", (float)delay, svsd_state->queue_length, (float)*time_frame, (float)AV_delay);
                }
            }
        //printf("~~no sleep display q:%d  svsd q:%d (%d:%d) audio %d apts:%f vpts:%f cur_pts:%f\n", sky_nrb.qlen[2], svsd_state->queue_length, svsd_state->send_count, svsd_state->recv_count, mpctx->audio_out->get_space(), mpctx->d_audio->pts, mpctx->sh_video->pts, sky_nrb.cur_pts);
        //printf("~~  time_frame %f  delay  %f  mpctx->delay %f\n", *time_frame, mpctx->audio_out->get_delay(), mpctx->delay);
        }

    } else {
        // If we're lagging more than 200 ms behind the right playback rate,
        // don't try to "catch up".
        // If benchmark is set always output frames as fast as possible
        // without sleeping.
        if (*time_frame < -0.2 || benchmark)
        {
            *time_frame = 0;
        }
        else
        {
            if (mpctx->sh_video)
                usleep(1000);
        }

//20101213 Robert don't sleep too long
        if (*time_frame > 0.02)
            *time_frame = 0.015;//0.008;//0.02;

        *aq_sleep_time += *time_frame;

        //============================== SLEEP: ===================================
        // flag 256 means: libvo driver does its timing (dvb card)
        if (*time_frame > 0.001 && !(vo_flags&256) && svsd_state->queue_length > 0)
        {
            *time_frame = timing_sleep(*time_frame);
        }
        
        frame_drop_lock = 0;
    }
//Robert 20110429 prevent audio/video locking
    if (seek_sync_flag==2)
    {
        frame_time_remaining = 0;
    }

    return frame_time_remaining;
}

int reinit_video_chain(void) {
    sh_video_t * const sh_video = mpctx->sh_video;
    double ar=-1.0;

    //Fuchun 2010.10.20
    display_height = 0;
    display_width = 0;
    //================== Init VIDEO (codec & libvo) ==========================
    if(!fixed_vo || !(initialized_flags&INITIALIZED_VO)) {
        current_module="preinit_libvo";

        //shouldn't we set dvideo->id=-2 when we fail?
        vo_config_count=1;	//Fuchun 2009.10.20
        //if((mpctx->video_out->preinit(vo_subdevice))!=0){
        if(!(mpctx->video_out=init_best_video_out(video_driver_list))) {
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_ErrorInitializingVODevice);
            goto err_out;
        }
        initialized_flags|=INITIALIZED_VO;
#ifdef HW_TS_DEMUX
        if (vo_need_dvdnav_menu || skydroid == 0 || skyqt == 1 || no_osd == 1)
        {
            //don't need to workaround
        }
        else if((mpctx->demuxer) && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux) 
        {
//printf("got here!!!!!!!\n");
            skyfb_trick_play_workaround(0);
        }
#endif
    }

    if(stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
        mpctx->sh_video->stream_aspect = ar;
    current_module="init_video_filters";
    {
        char* vf_arg[] = { "_oldargs_", (char*)mpctx->video_out , NULL };
        sh_video->vfilter=(void*)vf_open_filter(NULL,"vo",vf_arg);
    }
#ifdef CONFIG_MENU
    if(use_menu) {
        char* vf_arg[] = { "_oldargs_", menu_root, NULL };
        vf_menu = vf_open_plugin(libmenu_vfs,sh_video->vfilter,"menu",vf_arg);
        if(!vf_menu) {
            mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CantOpenLibmenuFilterWithThisRootMenu,menu_root);
            use_menu = 0;
        }
    }
    if(vf_menu)
        sh_video->vfilter=(void*)vf_menu;
#endif

#ifdef CONFIG_ASS
    if(ass_enabled) {
        int i;
        int insert = 1;
        if (vf_settings)
            for (i = 0; vf_settings[i].name; ++i)
                if (strcmp(vf_settings[i].name, "ass") == 0) {
                    insert = 0;
                    break;
                }
        if (insert) {
            extern vf_info_t vf_info_ass;
            const vf_info_t* libass_vfs[] = {&vf_info_ass, NULL};
            char* vf_arg[] = {"auto", "1", NULL};
            vf_instance_t* vf_ass = vf_open_plugin(libass_vfs,sh_video->vfilter,"ass",vf_arg);
            if (vf_ass)
                sh_video->vfilter=(void*)vf_ass;
            else
                mp_msg(MSGT_CPLAYER,MSGL_ERR, "ASS: cannot add video filter\n");
        }
    }
#endif

    sh_video->vfilter=(void*)append_filters(sh_video->vfilter);

#ifdef CONFIG_ASS
    if (ass_enabled)
    {
        ((vf_instance_t *)sh_video->vfilter)->control(sh_video->vfilter, VFCTRL_INIT_EOSD, ass_library);
        ((vf_instance_t *)sh_video->vfilter)->config(sh_video->vfilter, display_width, display_height, display_width, display_height, 0, 0);	//Fuchun 2010.10.20
    }
#endif

#if 1	//Fuchun 2010.03.01
    if(sh_video)
    {
        if((mpctx->demuxer->type == DEMUXER_TYPE_MOV && is_mjpeg == 0 && mov_keyframe_size < 10)
                || mpctx->demuxer->type == DEMUXER_TYPE_AVI_NINI
#ifdef QT_SUPPORT_DVBT				
                || mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS
#endif /* end of QT_SUPPORT_DVBT */				
//			|| (mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED && stream_cache_size > 0)		//use stream_cache_size > 0 to detect streamming
                || mpctx->stream->type == STREAMTYPE_STREAM
//			|| ((mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED || mpctx->demuxer->type == DEMUXER_TYPE_LAVF) && mpctx->sh_video->bih->biCompression != mmioFOURCC('F','L','V','1'))
                || !mpctx->demuxer->seekable)		//Fuchun 2010.03.18
        {
            Can_FF_FB = 0;
             if(mpctx->demuxer->type == DEMUXER_TYPE_MOV && is_mjpeg == 0 && mov_keyframe_size < 10 
                && mpctx->demuxer->seekable == 1 )  //Polun 2011-08-17
                 mpctx->demuxer->seekable = 0; //Polun 2011-08-15 for mantis5721 no support seek set  seekable to 0.
            if(mpctx->demuxer->type == DEMUXER_TYPE_MOV)
                printf("===== MOV: mov_keyframe_size = %d\n", mov_keyframe_size);
            else if(mpctx->demuxer->type == DEMUXER_TYPE_AVI_NINI)
                printf("===== This file not has index_table(AVI_NINI).\n");
            else if(mpctx->stream->type == STREAMTYPE_STREAM)
                printf("===== This is Stream.\n");
            else if(!mpctx->demuxer->seekable)
                printf("===== The seekable of this file is 0.\n");
#ifdef QT_SUPPORT_DVBT
            if(mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
                Can_FF_FB = -1;
                printf("===== The DVB-T can't FF/FR!!\n");
            }
            else
#endif
                printf("===== Just can FF to 2x!\n");
        }
        else
            Can_FF_FB = 1;
    }
#endif

    current_module="init_video_codec";

    mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
    init_best_video_codec(sh_video,video_codec_list,video_fm_list);
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");

    if(!sh_video->initialized) {
        if(!fixed_vo) uninit_player(INITIALIZED_VO);
        goto err_out;
    }

    initialized_flags|=INITIALIZED_VCODEC;

    if (sh_video->codec)
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_CODEC=%s\n", sh_video->codec->name);

    sh_video->last_pts = MP_NOPTS_VALUE;
    sh_video->num_buffered_pts = 0;
    sh_video->next_frame_time = 0;

    if(auto_quality>0) {
        // Auto quality option enabled
        output_quality=get_video_quality_max(sh_video);
        if(auto_quality>output_quality) auto_quality=output_quality;
        else output_quality=auto_quality;
        mp_msg(MSGT_CPLAYER,MSGL_V,"AutoQ: setting quality to %d.\n",output_quality);
        set_video_quality(sh_video,output_quality);
    }

    // ========== Init display (sh_video->disp_w*sh_video->disp_h/out_fmt) ============

    current_module="init_vo";

    return 1;

err_out:
    mpctx->sh_video = mpctx->d_video->sh = NULL;
    return 0;
}

static double update_video(int *blit_frame)
{
    sh_video_t * const sh_video = mpctx->sh_video;
    unsigned int t2 = 0;
    static display_change = 0;

#if 1
    if (mpctx->stream->type !=STREAMTYPE_DVDNAV )
    {
        videobuffer = VideoBuffer[bidx];
        bidx++;
        if(bidx == 3)
            bidx = 0;
    }

//	printf("update_video: videobuffer = %08X\n", videobuffer);
#endif

    //--------------------  Decode a frame: -----------------------
    double frame_time;

    *blit_frame = 0; // Don't blit if we hit EOF
    t2=GetTimer();	// Raymond 2009/07/22
    if (!correct_pts) {
        unsigned char* start=NULL;
        void *decoded_frame = NULL;
        int drop_frame=0;
        int in_size;
        static double keep_last_vft = 0.0;

#if 1	// Raymond 2009/07/07
        if(scanf_per_frame)
        {
            unsigned char choice;
            printf("Next frame ? (y) : ");
            scanf("%c",&choice);
            printf("\r\n");
        }

        if( usleep_cnt > 0 )
            usleep(usleep_cnt);
#endif

        current_module = "video_read_frame";
        frame_time = sh_video->next_frame_time;

        in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
                                   &start, force_fps);

	 //Barry 2011-08-11
	 if (in_size > 0 && mpctx->demuxer && mpctx->sh_video && !mpctx->sh_audio)
	 {
		 if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES) && (speed_mult >=2 || speed_mult < 0))
		 	my_current_pts = mpctx->sh_video->pts;
	 }

//20100728 Robert need last_pts for new AV
      if (in_size > 0)
      {
        if (keep_last_vft > 0.0)
        {
            sh_video->next_frame_time += keep_last_vft;
            keep_last_vft = 0.0;
        }
        keep_last_vft = 0.0;
        if (sh_video->last_pts != 0.0 && sh_video->last_pts == sh_video->pts)
        {
            if (!quiet)
            {
                printf("!!!!! got duplicate pts frame it!!! pts=%f \n", sh_video->pts);
            }

            if(mpctx->stream->type != STREAMTYPE_DVDNAV)
            {
                sh_video->next_frame_time = 0.0;
                frame_time = 0.0;
            }
//            sky_set_sync_frame_ridx();	//Fuchun 2011.04.20 disable
        }
        else
        {
            sh_video->last_pts = sh_video->pts;
        }
      }
      else if (in_size == 0)
      {
          keep_last_vft = sh_video->next_frame_time;
      }

       #if 0  //Polun 2011-10-24  fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv -ss can't AVSYNC move after reinit_audio.
	//Fuchun 2011.03.10
	//Robert 2011.03.13 add mpctx->sh_audio checking for -nosound
	//Charles 2011.03.22 don't do this for STREAMTYPE_STREAM (iTV)
	if(recalcul_apts && mpctx->sh_audio && mpctx->stream->type != STREAMTYPE_STREAM)
	{
		if(mpctx->demuxer->type == DEMUXER_TYPE_ASF && (mpctx->sh_audio->format == 0x160 || mpctx->sh_audio->format == 0x161))
		{
			float a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
			audio_delay = a_pts - sh_video->pts -0.033;
			printf("#### ASF with wma(0x%x)  set audio_delay:%f ####\n", mpctx->sh_audio->format, audio_delay);
		}
		recalcul_apts = 0;
	}
       #endif
        //Barry 2010-05-18
#ifdef CONFIG_DVDNAV
        if (mpctx->demuxer->video->eof && mpctx->demuxer->audio->eof && mpctx->stream->type != STREAMTYPE_DVDNAV)
#else
        if (mpctx->demuxer->video->eof && mpctx->demuxer->audio->eof)
#endif
            mpctx->eof = 1;

        read_nextframe = 1;		//Fuchun 2010.03.09

        if(speed_mult < 0 && mpeg_fast == 1)
        {
            if(last_rewind_pts != 0.0 && last_rewind_pts <= sh_video->pts)
            {
                //printf("last_rewind_pts %f sh_video->pts %f\n", last_rewind_pts, sh_video->pts);
                rewind_mult *= 2;	//skip large
            }
            else
            {
                //printf("last_rewind_pts %f sh_video->pts %f\n", last_rewind_pts, sh_video->pts);
                last_rewind_pts = sh_video->pts;
                rewind_mult = 1;
            }
        }
        else if(last_rewind_pts != 0.0 || rewind_mult != 1)
        {
            last_rewind_pts = 0.0;
            rewind_mult = 1;
        }

        if ( mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS
                || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS )	//Barry 2010-10-13
        {
            if(first_set_sub_pts == 1 && sh_video->pts > 0.051)
            {
                correct_sub_pts = sh_video->pts;
                first_set_sub_pts = 0;
            }
        }

#ifdef _SKY_VDEC_V2	//Fuchun 2010.09.23 for H264 field frame type
        if((speed_mult < 0 || speed_mult >= 2) && !h264_frame_mbs_only && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES
                || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS))
        {
            if(top_field_frame == 1)
            {
                top_field_frame = 0;
                mpeg_fast = 0;
            }
            else if(top_field_frame == 0)
            {
                top_field_frame = 1;
                mpeg_fast = 1;
            }
        }
        else
            top_field_frame = 1;
#endif

        // Raymond 2009/08/03

        if (benchmark)
        {
            unsigned int t3 = 0;
            t3 = GetTimer() - t2;
            if(print_video_read_time)
            {
#ifdef DEBUG_VIDEO_CONTENT
                if (1)
                {
                    unsigned int checksum = 0;
                    unsigned int lll = 0;
                    for(lll = 0 ; lll < in_size ; lll++)
                        checksum += start[lll];
                    printf("[%5d]VRead len = %6d -%3d ms, sh_video->pts[%f] checksum [%d]\n", total_frame_cnt, in_size, t3/1000, sh_video->pts, checksum);
                }
                else
#endif // end of DEBUG_VIDEO_CONTENT		
                {
//                    if (t3/1000 > 5)
                    printf("[%5d]VRead len = %6d -%3d ms, sh_video->pts[%f]\n", total_frame_cnt, in_size, t3/1000, sh_video->pts);
                }
            }
//if (t2/1000 > 20)
//printf("[%5d]VRead len = %6d -%3d ms\n", total_frame_cnt, in_size, t3/1000);

            video_read_time_usage += t3 * 0.000001;		// Raymond 2009/07/22
        }

        if((speed_mult < 0 || speed_mult >= 2) && decode_num_cnt == 0)
        {
            read_frame_time = GetTimer() - t2;
        }

#ifdef CONFIG_DVDNAV
//printf("0-mpctx->d_video[%d],mpctx->d_audio[%d],mpctx->stream[%d]\n",mpctx->d_video->eof,mpctx->d_audio->eof,mpctx->stream->eof);
        if (mpctx->stream->type == STREAMTYPE_DVDNAV)
        {
            if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
            {
                if(mpctx->d_video->eof == 0 & mpctx->d_audio->eof == 0 & mpctx->stream->eof == 0)
                    dvdnavstate.dvdnav_stream_state = 0;
                else if(mpctx->d_video->eof == 1 & mpctx->d_audio->eof == 0 & mpctx->stream->eof == 1)
                    dvdnavstate.dvdnav_stream_state = 1;
                else if(mpctx->d_video->eof == 0 & mpctx->d_audio->eof == 1 & mpctx->stream->eof == 1)
                    dvdnavstate.dvdnav_stream_state = 2;
                else if(mpctx->d_video->eof == 1 & mpctx->d_audio->eof == 1 & mpctx->stream->eof == 1)
                    dvdnavstate.dvdnav_stream_state = 3;

		if(dvdnav_audio_need_uninit == 1)
                {

//                    if (audio_id == -2 || audio_id > -1)	
                    {
                    uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);
                    }

                    sh_audio_t *sh2 = NULL;
		    if (mpctx->demuxer->audio->id >= 0)
			    sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];

                    if (sh2) {
				dvdnav_audio_need_uninit = 0;
				if ( ((sh2->format == 0x2001 && !svsd_state->dec_cap.dts) || (sh2->format == 0x2000 && !svsd_state->dec_cap.dolby)) && !sky_hwac3 )
				{
					char callback_str[256];
					sprintf(audio_names, "No Audio can switch\0");
					sprintf(callback_str, "AUDIO: %s\0", audio_names);
					ipc_callback(callback_str);
					if (sh2->format == 0x2001)
						strcpy(callback_str,"status: playing 2\0");
					else
						strcpy(callback_str,"status: playing 4\0");
					ipc_callback(callback_str);
					audio_not_support = 1;
					if (mpctx->d_audio->packs > 1 && audio_not_support)
						ds_free_packs(mpctx->d_audio);
					not_support_audio_id = mpctx->demuxer->audio->id;
				}
				else
				{
					if (audio_not_support)
						audio_not_support = 0;
					sh2->ds = mpctx->demuxer->audio;
					mpctx->sh_audio = sh2;
					reinit_audio_chain();
				}
                    }
                    else
                    	mpctx->d_audio->sh = NULL;
                }
		
            }
            else if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
            {
		if(dvdnav_audio_need_uninit == 1)
                {
                    uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);

//printf("a_id[%d]  a_id2[%d]\n",mpctx->demuxer->audio->id, mpctx->d_audio->id);
//printf("dvdnavstate.dvdnav_title_state[%d]\n",dvdnavstate.dvdnav_title_state);

                    sh_audio_t *sh2 = NULL;
		    if (mpctx->demuxer->audio->id >= 0)
			    sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];

                    if (sh2) {
				dvdnav_audio_need_uninit = 0;
				if ( ((sh2->format == 0x2001 && !svsd_state->dec_cap.dts) || (sh2->format == 0x2000 && !svsd_state->dec_cap.dolby)) && !sky_hwac3 )
				{
					char callback_str[256];
					sprintf(audio_names, "No Audio can switch\0");
					sprintf(callback_str, "AUDIO: %s\0", audio_names);
					ipc_callback(callback_str);
					if (sh2->format == 0x2001)
						strcpy(callback_str,"status: playing 2\0");
					else
						strcpy(callback_str,"status: playing 4\0");
					ipc_callback(callback_str);
					audio_not_support = 1;
					if (mpctx->d_audio->packs > 1 && audio_not_support)
						ds_free_packs(mpctx->d_audio);
					not_support_audio_id = mpctx->demuxer->audio->id;
				}
				else
				{
					if (audio_not_support)
						audio_not_support = 0;
					sh2->ds = mpctx->demuxer->audio;
					mpctx->sh_audio = sh2;
					reinit_audio_chain();
				}

				if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && duration_sec_cur > 0)
				{
					char callback_str[64];

					mp_property_print("switch_audio", mpctx);
					sprintf(callback_str, "AUDIO: %s\0", audio_names);
					ipc_callback(callback_str);
				}
                    }
                    else 
                    	mpctx->d_audio->sh = NULL;
                }

                if(mpctx->d_video->eof == 0 & mpctx->d_audio->eof == 0 & mpctx->stream->eof == 0)
                    dvdnavstate.dvdnav_stream_state = 0;
            }
        }

        /// wait, still frame or EOF
        if (mpctx->stream->type == STREAMTYPE_DVDNAV && in_size < 0) {
           //Polun 2012-02-03 ++s fixed mantis6790 playback to eof signal 11 issue  
           #if 1
           if(Dvdnav_cmd_exit ){
            printf("%s:%d  dvdnav cmd exit \n", __func__, __LINE__);
            return -1; 
           }
           #endif
           //Polun 2012-02-03 ++e
	    if(0)
	    {
		// move code to behide mp_dvdnav_restore_smpi
		dvdnavstate.dvdnav_cell_change = 0;
		//Skyviia_Vincent08262010
		//todo: in playing mode to change to menu the in_size will be -1 why??
		//mark for testing
		//if (mp_dvdnav_is_eof(mpctx->stream)) return -1;

		if (mpctx->d_video) mpctx->d_video->eof = 0;
		if (mpctx->d_audio) mpctx->d_audio->eof = 0;
		mpctx->stream->eof = 0;
	    }
	    if (mp_dvdnav_is_stop(mpctx->stream)) return -1;

        } else
#endif
            if (in_size < 0)
                return -1;
        if (in_size > max_framesize)
            max_framesize = in_size; // stats
        sh_video->timer += frame_time;
        if (mpctx->sh_audio)
            mpctx->delay -= frame_time;
        // video_read_frame can change fps (e.g. for ASF video)
        vo_fps = sh_video->fps;

        if(drop_frame == 0 && speed_mult == 0 && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU)
            drop_frame = check_framedrop(frame_time);
#ifdef DVB_SUBTITLES
        /* carlos add for TS subtitle, according to Fuchun comment, 2010-10-05 */
#if 0	//Fuchun 2010.10.15 disable
        if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                && correct_sub_pts > 0 && (mpctx->d_sub->sh ? ((sh_sub_t *)mpctx->d_sub->sh)->type : 'v') == 'p')
            correct_sub_pts = 0;
#endif
        if (dvdsub_id >= 0 && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) 
#ifdef QT_SUPPORT_DVBT
         || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */
                && ((mpctx->d_sub->sh ? ((sh_sub_t *)mpctx->d_sub->sh)->type : 'v') == 'p' || (mpctx->d_sub->sh ? ((sh_sub_t *)mpctx->d_sub->sh)->type : 'v') == 'b'))
        {
            if (sky_nrb.cur_pts > 0.5 && sh_video->pts - sky_nrb.cur_pts < 1.0)
            {
                update_subtitles(sh_video, sky_nrb.cur_pts, mpctx->d_sub, 0);	//Fuchun 2010.10.15 for PGS
            }
            else
            {
                update_subtitles(sh_video, sh_video->pts, mpctx->d_sub, 0);	//Fuchun 2010.10.15 for PGS
            }
        }
        else
#endif // end of DVB_SUBTITLES		
        {
            if (sky_nrb.cur_pts > 0.5 && sh_video->pts - sky_nrb.cur_pts < 1.0 && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU)
            {
//printf(" == 01 cur_pts :%f  corr:%f   sh_pts:%f corr+timer=%f\n", sky_nrb.cur_pts, correct_sub_pts, sh_video->pts, correct_sub_pts+sh_video->timer);
//printf("last_sub_pts:%f  a:%f\n", last_sub_pts, sky_nrb.cur_pts - AV_delay);                        

                update_subtitles(sh_video, sky_nrb.cur_pts-correct_sub_pts, mpctx->d_sub, 0);	//Fuchun 2010.06.09 add correct_sub_pts
            }
            else
            {
//printf(" == 02 cur_pts :%f  corr:%f   sh_pts:%f\n", sky_nrb.cur_pts, correct_sub_pts, sh_video->pts);
                update_subtitles(sh_video, sh_video->pts-correct_sub_pts, mpctx->d_sub, 0);	//Fuchun 2010.06.09 add correct_sub_pts
            }
        }
        update_teletext(sh_video, mpctx->demuxer, 0);
        update_osd_msg();

        current_module = "decode_video";
#ifdef CONFIG_DVDNAV
        if (mpctx->stream->type==STREAMTYPE_DVDNAV )
	{
            // call dvdnav video preprocess
            decoded_frame = mp_dvdnav_restore_smpi(&in_size,&start,decoded_frame);

	    if (in_size > 0)
	    {
		if (mpctx->d_video) mpctx->d_video->eof = 0;
		//if (mpctx->d_audio) mpctx->d_audio->eof = 0;
		mpctx->stream->eof = 0;
	    }
	}
        /// still frame has been reached, no need to decode
        // if stored image then do not call decode_video()
        if (in_size > 0 && !decoded_frame)
#endif
        {
            decoded_frame = decode_video(sh_video, start, in_size, drop_frame,
                                         sh_video->pts);

            *blit_frame = 1;

        }
#ifdef CONFIG_DVDNAV
        if (mpctx->stream->type==STREAMTYPE_DVDNAV)
        {
	    if (sh_video->frame_type == 1) // only store i frame
	    {
		// save back last still frame for future display
		decoded_frame=mp_dvdnav_save_smpi(in_size,start,decoded_frame);
	    }
        }
#endif
        current_module = "filter_video";
//        *blit_frame = 1;
//	*blit_frame = (decoded_frame && filter_video(sh_video, decoded_frame,
//						    sh_video->pts));
    }
    else {
        int res = generate_video_frame(sh_video, mpctx->d_video);

        if((speed_mult < 0 || speed_mult >= 2) && decode_num_cnt == 0)
        {
            read_frame_time = GetTimer() - t2;
        }

        if (!res)
            return -1;
#if 0// Raymond 2010/03/18 disable for pts zero issue, but MKV / avc1 has problem - FIX ME!!
        ((vf_instance_t *)sh_video->vfilter)->control(sh_video->vfilter,
                VFCTRL_GET_PTS, &sh_video->pts);
#endif
        if (sh_video->pts == MP_NOPTS_VALUE) {
            mp_msg(MSGT_CPLAYER, MSGL_ERR, "pts after filters MISSING\n");
            sh_video->pts = sh_video->last_pts;
        }
        if (sh_video->last_pts == MP_NOPTS_VALUE)
            sh_video->last_pts= sh_video->pts;
        else if (sh_video->last_pts > sh_video->pts) {
            sh_video->last_pts = sh_video->pts;
            if (!speed_mult)	//Barry 2010-09-30
                mp_msg(MSGT_CPLAYER, MSGL_INFO, "pts value < previous\n");
        }
        frame_time = sh_video->pts - sh_video->last_pts;
        sh_video->last_pts = sh_video->pts;
        sh_video->timer += frame_time;
        if(mpctx->sh_audio)
            mpctx->delay -= frame_time;
        *blit_frame = res > 0;
    }

    ++total_frame_cnt;	// Raymond 2009/04/27
    ++correct_frame_cnt;

    if (mpctx->demuxer->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && frame_cnt < 2)
    {                                                                                                                                    
	    mpctx->num_buffered_frames = 0; 
	    mpctx->delay = 0;               
	    mpctx->time_frame = 0;          
	    frame_cnt++;                    
    }

    return frame_time;
}

static void pause_loop(void)
{
    mp_cmd_t* cmd;

//+SkyMedi_Vincent03232010
    if(ipcdomain) //using domain socket option
    {
        char callback_str[42];
        is_pause = 1;

        strcpy(callback_str,"status: pause\0");
        ipc_callback(callback_str);
    }
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
    if (hwtsdemux || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
    if (hwtsdemux )
#endif /* end of QT_SUPPORT_DVBT */	
        is_pause = 1;
#endif // end of HW_TS_DEMUX
//SkyMedi_Vincent03232010+

    sky_set_vsync_status(-2);	//Fuchun 2010.10.25

    if (!quiet) {
        // Small hack to display the pause message on the OSD line.
        // The pause string is: "\n == PAUSE == \r" so we need to
        // take the first and the last char out
        if (term_osd && !mpctx->sh_video) {
            char msg[128] = MSGTR_Paused;
            int mlen = strlen(msg);
            msg[mlen-1] = '\0';
            set_osd_msg(OSD_MSG_PAUSE, 1, 0, "%s", msg+1);
            update_osd_msg();
        } else
            mp_msg(MSGT_CPLAYER,MSGL_STATUS,MSGTR_Paused);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_PAUSED\n");
    }
#ifdef CONFIG_GUI
    if (use_gui)
        guiGetEvent(guiCEvent, (char *)guiSetPause);
#endif
    if (mpctx->video_out && mpctx->sh_video && vo_config_count)
        mpctx->video_out->control(VOCTRL_PAUSE, NULL);

    if (mpctx->audio_out && mpctx->sh_audio)
        mpctx->audio_out->pause(); // pause audio, keep data if possible

    while ( (cmd = mp_input_get_cmd(20, 1, 1)) == NULL || cmd->pausing == 4 || domainsocket_cmd) {
        if(domainsocket_cmd)
            cmd = domainsocket_cmd;

        if (cmd) {
            if(!domainsocket_cmd)
                cmd = mp_input_get_cmd(0,1,0);

            run_command(mpctx, cmd);

            if(!domainsocket_cmd)
                mp_cmd_free(cmd);
            else
            {
#if 1
                if(cmd->id == MP_CMD_FRAME_STEP || cmd->id == MP_CMD_DVDNAV)
                    break;
                else
                {
                    domainsocket_cmd = NULL;

                    if(cmd->id == MP_CMD_PAUSE || cmd->id == MP_CMD_SEEK || cmd->id == MP_CMD_STOP)
                        break;
                    else
                        continue;
                }
#else
                domainsocket_cmd = NULL;
                //Barry 2010-11-09
                if (cmd->id != MP_CMD_FRAME_STEP)
                    continue;
                break;
#endif
            }
            continue;
        }
        if (mpctx->sh_video && mpctx->video_out && vo_config_count)
            mpctx->video_out->check_events();
#ifdef CONFIG_GUI
        if (use_gui) {
            guiEventHandling();
            guiGetEvent(guiReDraw, NULL);
            if (guiIntfStruct.Playing!=2 || (rel_seek_secs || abs_seek_pos))
                break;
        }
#endif
#ifdef CONFIG_MENU
        if (vf_menu)
            vf_menu_pause_update(vf_menu);
#endif
        if(mpctx->sh_video)
            ioctl(skyfd, SKY_VDEC_SET_USE_TICK, SKY_VDEC_VIDEO);
        usec_sleep(20000);
    }

#ifdef SWITCH_AUDIO_THREAD
	if(skydroid)
	{
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
		if ((hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
		if (hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */
		{
			if(cmd->id == MP_CMD_FRAME_STEP)
				ds_fill_buffer(mpctx->d_audio);	//avoid ts audio queue full
			
			if(cmd->id != MP_CMD_FRAME_STEP
				|| (cmd->id == MP_CMD_FRAME_STEP && check_need_write_ts_video(mpctx->demuxer->priv, (cmd->id != MP_CMD_FRAME_STEP ? 1 : 0))))
			{
				is_pause = 0;
				usleep(2000);
			}
		}
#endif // end of HW_TS_DEMUX
		if(last_audio_id != audio_id && cmd->id != MP_CMD_FRAME_STEP)	//Fuchun 2011.02.10
		{
			if (!switch_audio_thread_status)
			{
				//printf("=========>  audio_id=%d,    mpctx->demuxer->audio->id=%d,   current_id=%d\n", audio_id, mpctx->demuxer->audio->id, current_id);
				switch_audio_thread_status = 2;
				switch_audio_current_id = last_audio_id;
				if (pthread_create(&thread_switch_audio, NULL, (void *)do_switch_audio_pthread, mpctx) == 0)
				{
					printf("### switch_audio: pthread_create ###\n");
				}
				else
					printf("### switch_audio: pthread_create failed , failed reason is [%s] ###\n", strerror(errno));
			}
			else
				printf("### switch_audio: switch_audio_thread_status == 1 (running)###");
		}
	}
	else
	{
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
		if (hwtsdemux || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
		if (hwtsdemux)
#endif /* end of QT_SUPPORT_DVBT */
			is_pause = 0;
#endif // end of HW_TS_DEMUX
	}
#else	//else of SWITCH_AUDIO_THREAD
	if(skydroid)
	{
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
		if ((hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
		if (hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */
		{
			if(cmd->id == MP_CMD_FRAME_STEP)
				ds_fill_buffer(mpctx->d_audio);	//avoid ts audio queue full
			
			if(cmd->id != MP_CMD_FRAME_STEP
				|| (cmd->id == MP_CMD_FRAME_STEP && check_need_write_ts_video(mpctx->demuxer->priv, (cmd->id != MP_CMD_FRAME_STEP ? 1 : 0))))
			{
				is_pause = 0;
				usleep(2000);
			}
		}
#endif // end of HW_TS_DEMUX
		
		if(last_audio_id != audio_id && cmd->id != MP_CMD_FRAME_STEP)	//Fuchun 2011.02.10
		{
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
			if ((hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
			if (hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
#endif /* end of QT_SUPPORT_DVBT */
			{
				while(!check_need_write_ts_video(mpctx->demuxer->priv, (cmd->id != MP_CMD_FRAME_STEP ? 1 : 0)))
				{
					if(check_need_read_ts_video(mpctx->demuxer->priv))
					{
						demux_fill_buffer(mpctx->demuxer, mpctx->d_video);
					}
					usleep(1000);
				}
			}
#endif
			if (audio_id == -2 || audio_id > -1)
				uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);

			sh_audio_t *sh2 = NULL;
			if (mpctx->demuxer->audio->id >= 0)
				sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];
			if (sh2)
			{
				sh2->ds = mpctx->demuxer->audio;
				mpctx->sh_audio = sh2;
				reinit_audio_chain();
			}
		}
	}
	else
	{
#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
		if (hwtsdemux || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
#else /* else of QT_SUPPORT_DVBT */
		if (hwtsdemux)
#endif /* end of QT_SUPPORT_DVBT */
			is_pause = 0;
#endif // end of HW_TS_DEMUX
	}
#endif	//end of SWITCH_AUDIO_THREAD
	
    sky_set_vsync_status(1);		//Fuchun 2010.10.25

// WT, 100714, DRM resume event
#ifdef CONFIG_DRM_ENABLE
    if (mpctx->demuxer->drmFileOpened)
    {
        int er;
        er = DxDrmStream_HandleConsumptionEvent(mpctx->demuxer->drmStream, DX_EVENT_RESUME);
        if (er != DX_SUCCESS)
            printf("DX:ERROR - resume event failed\n");
        else
            printf("DX:DRM resume\n");
    }
#endif
    if (cmd && cmd->id == MP_CMD_PAUSE) {
        cmd = mp_input_get_cmd(0,1,0);
        mp_cmd_free(cmd);
    }
    mpctx->osd_function=OSD_PLAY;
    if (mpctx->audio_out && mpctx->sh_audio) {
        if (mpctx->eof) // do not play remaining audio if we e.g.  switch to the next file
            mpctx->audio_out->reset();
        else
            mpctx->audio_out->resume(); // resume audio
    }
    if (mpctx->video_out && mpctx->sh_video && vo_config_count)
        mpctx->video_out->control(VOCTRL_RESUME, NULL); // resume video
    (void)GetRelativeTime(); // ignore time that passed during pause
#ifdef CONFIG_GUI
    if (use_gui) {
        if (guiIntfStruct.Playing == guiSetStop)
            mpctx->eof = 1;
        else
            guiGetEvent(guiCEvent, (char *)guiSetPlay);
    }
#endif
}


// Find the right mute status and record position for new file position
static void edl_seek_reset(MPContext *mpctx)
{
    mpctx->edl_muted = 0;
    next_edl_record = edl_records;

    while (next_edl_record) {
        if (next_edl_record->start_sec >= mpctx->sh_video->pts)
            break;

        if (next_edl_record->action == EDL_MUTE)
            mpctx->edl_muted = !mpctx->edl_muted;
        next_edl_record = next_edl_record->next;
    }
    if ((mpctx->user_muted | mpctx->edl_muted) != mpctx->mixer.muted)
        mixer_mute(&mpctx->mixer);
}


// Execute EDL command for the current position if one exists
static void edl_update(MPContext *mpctx)
{
    if (!next_edl_record)
        return;

    if (!mpctx->sh_video) {
        mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_EdlNOsh_video);
        free_edl(edl_records);
        next_edl_record = NULL;
        edl_records = NULL;
        return;
    }

    if (mpctx->sh_video->pts >= next_edl_record->start_sec) {
        if (next_edl_record->action == EDL_SKIP) {
            mpctx->osd_function = OSD_FFW;
            abs_seek_pos = 0;
            rel_seek_secs = next_edl_record->length_sec;
            mp_msg(MSGT_CPLAYER, MSGL_DBG4, "EDL_SKIP: start [%f], stop "
                   "[%f], length [%f]\n", next_edl_record->start_sec,
                   next_edl_record->stop_sec, next_edl_record->length_sec);
            edl_decision = 1;
        }
        else if (next_edl_record->action == EDL_MUTE) {
            mpctx->edl_muted = !mpctx->edl_muted;
            if ((mpctx->user_muted | mpctx->edl_muted) != mpctx->mixer.muted)
                mixer_mute(&mpctx->mixer);
            mp_msg(MSGT_CPLAYER, MSGL_DBG4, "EDL_MUTE: [%f]\n",
                   next_edl_record->start_sec );
        }
        next_edl_record = next_edl_record->next;
    }
}


// style & SEEK_ABSOLUTE == 0 means seek relative to current position, == 1 means absolute
// style & SEEK_FACTOR == 0 means amount in seconds, == 2 means fraction of file length
// return -1 if seek failed (non-seekable stream?), 0 otherwise
static int seek(MPContext *mpctx, double amount, int style)
{
    current_module = "seek";
    //Polun 2011-07-04 ++s for h.264 if run svsd_ipc_drop_video_cnts(), will have some issue.
   // if (!mpctx->demuxer->seekable) //Polun 2011-0-21 for Prestige.mkv seek long time cause UI timeout
    if (!mpctx->demuxer->seekable || (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA && mkv_indexnull_flag ==1 && speed_mult == 0 && (amount > 1.0f || amount < -1.0f)))
    {
        mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekFile);
        return -1;
     }
    //Polun 2011-07-04 ++e
    //Polun 2011-07-27 ++s for audio play skip key time bar updata. 
    if (!mpctx->sh_video && mpctx->sh_audio)  
    {
         audio_seek_flg = 1 ;
    } 
     //Polun 2011-07-27 ++e
	//Barry 2010-08-24 move to here, before demux_seek()
    svsd_ipc_drop_video_cnts();
    sky_repeatfield_cnt = 0;

#if 0	//Robert 20101118 not yet support nrb.timeout
    //Barry 2010-08-05
    if (sky_nrb.timeout_current > 5  && (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS))
    {
        if (!Vd_Reinit(mpctx->sh_video))
            printf("========>   Vd_Reinit fail\n");
    }
    else
#endif
        sky_set_reset_display_queue(0);

    if (sky_vdec_wait_sync_vpts > 0.0)
    {
        sky_set_vsync_status(0);
        sky_set_vsync_status(1);
        sky_vdec_wait_sync_vpts = 0.0;
    }

    if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video && !nosync && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU)
    {
        seek_sync_flag = 1;	//Fuchun 2010.07.05
        wait_video_or_audio_sync = AVSYNC_NORMAL;
    }

    if (demux_seek(mpctx->demuxer, amount, audio_delay, style) == 0)
    {
        if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video && !nosync && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU)
            seek_sync_flag = 0;	//Fuchun 2010.07.05
        return -1;
    }
    if(speed_mult == 0 && num_reorder_frames != 0) queue_frames = num_reorder_frames;		//Fuchun 2010.05.17

    last_sub_pts = -1.0;	//Fuchun 2010.06.10
    last_spu_pts = 0;


    if(dvd_device != NULL) dvd_fast = 1;	//Fuchun 2010.03.30
    if(speed_mult == 0)	//Fuchun 2010.03.30
        mpctx->startup_decode_retry = DEFAULT_STARTUP_DECODE_RETRY;
    if (mpctx->sh_video) {
        current_module = "seek_video_reset";
        if (vo_config_count)
            mpctx->video_out->control(VOCTRL_RESET, NULL);
        mpctx->num_buffered_frames = 0;
        mpctx->delay = 0;
        mpctx->time_frame = 0;
        // Not all demuxers set d_video->pts during seek, so this value
        // (which is used by at least vobsub and edl code below) may
        // be completely wrong (probably 0).
        // 20110523 charleslin For MPEG-PS, MPEG-1 *groupies*.mpeg, don't update sh_video->pts if d_video->pts is not valid
	if(mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS &&
	   (mpctx->d_video->pts == 0 || mpctx->d_video->pts == MP_NOPTS_VALUE)){
	  ; // do nothing
	}else{
          //printf("set sh->pts to ds->pts %f\n", mpctx->d_video->pts);
        mpctx->sh_video->pts = mpctx->d_video->pts;
	}
        update_subtitles(mpctx->sh_video, mpctx->sh_video->pts-correct_sub_pts, mpctx->d_sub, 1);	//Fuchun 2010.06.09 add correct_sub_pts
        update_teletext(mpctx->sh_video, mpctx->demuxer, 1);
    }

    if (mpctx->sh_audio && mpctx->audio_out) {
        current_module = "seek_audio_reset";
        mpctx->audio_out->reset(); // stop audio, throwing away buffered data
        if (!mpctx->sh_video)
            update_subtitles(NULL, mpctx->sh_audio->pts, mpctx->d_sub, 1);
    }

    if (vo_vobsub && mpctx->sh_video) {
        current_module = "seek_vobsub_reset";
        vobsub_seek(vo_vobsub, mpctx->sh_video->pts);
    }

#ifdef CONFIG_ASS
    if (ass_enabled && mpctx->d_sub->sh && ((sh_sub_t *)mpctx->d_sub->sh)->ass_track)
        ass_flush_events(((sh_sub_t *)mpctx->d_sub->sh)->ass_track);
#endif

    edl_seek_reset(mpctx);

    c_total = 0;
    max_pts_correction = 0.1;
    audio_time_usage = 0;
    video_time_usage = 0;
    vout_time_usage = 0;
    drop_frame_cnt = 0;

//svread(0, &sky_nrb, 0);
//printf("\n===> end of seek, d_aud pts:%f  d_v pts:%f sh_v:%f cur:%f rid:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->sh_video->pts, sky_nrb.cur_pts, sky_nrb.ft_ridx, sky_nrb.ft_widx);
    current_module = "end of seek";
    return 0;
}

//charleskao 10022009
int subtitle_up(void)
{
    demux_stream_t *const d_sub = mpctx->d_sub;
    const int global_sub_size = mpctx->global_sub_size;

    char lang[] = "unknown";
    int language;
    int i;

    if((mpctx->stream->type==STREAMTYPE_DVD) || (mpctx->stream->type == STREAMTYPE_DVDNAV))
    {
        for(i=0; i<5; i++) //some sid isn't continuous
        {
            mpctx->global_sub_pos += 2;
            mpctx->global_sub_pos = (mpctx->global_sub_pos % (global_sub_size + 1)) - 1;

            dvdsub_id = mpctx->global_sub_pos - mpctx->global_sub_indices[SUB_SOURCE_DEMUX];
            d_sub->id = dvdsub_id;
            d_sub->sh = mpctx->demuxer->s_streams[d_sub->id];

            //update_subtitles(mpctx->sh_video, d_sub, 1);
            update_subtitles(mpctx->sh_video, mpctx->sh_video->pts, d_sub, 1);

            //return subtitle language ID
            language = dvd_lang_from_sid(mpctx->stream, dvdsub_id);
            lang[0] = language >> 8;
            lang[1] = language;
            lang[2] = 0;
            printf("subup-dvdsub_id = %d, language = %s\n",dvdsub_id,lang);
            if(language)
                return language;
        }
    }
    return 0;
}

int subtitle_down(void)
{
    demux_stream_t *const d_sub = mpctx->d_sub;
    const int global_sub_size = mpctx->global_sub_size;

    char lang[] = "unknown";
    int language;
    int i;

    if((mpctx->stream->type==STREAMTYPE_DVD) || (mpctx->stream->type == STREAMTYPE_DVDNAV))
    {
        for(i=0; i<5; i++) //some sid isn't continuous
        {
            mpctx->global_sub_pos += global_sub_size + 1;
            mpctx->global_sub_pos = (mpctx->global_sub_pos % (global_sub_size + 1)) - 1;

            dvdsub_id = mpctx->global_sub_pos - mpctx->global_sub_indices[SUB_SOURCE_DEMUX];
            d_sub->id = dvdsub_id;
            d_sub->sh = mpctx->demuxer->s_streams[d_sub->id];

            //update_subtitles(mpctx->sh_video, d_sub, 1);
            update_subtitles(mpctx->sh_video, mpctx->sh_video->pts, d_sub, 1);

            //return subtitle language ID
            language = dvd_lang_from_sid(mpctx->stream, dvdsub_id);
            lang[0] = language >> 8;
            lang[1] = language;
            lang[2] = 0;
            printf("subdown-dvdsub_id = %d, language = %s\n",dvdsub_id,lang);

            if(language)
                return language;
        }
    }

    return 0;
}

int audio_next(void)
{
    char lang[] = "unknown";
    int language;
    int current_id;

    current_id = mpctx->demuxer->audio->id;
    audio_id = demuxer_switch_audio(mpctx->demuxer, -1);

    if (audio_id == -2
            || (audio_id > -1
                && mpctx->demuxer->audio->id != current_id && current_id != -2))
        uninit_player(INITIALIZED_AO | INITIALIZED_ACODEC);

    if (audio_id > -1 && mpctx->demuxer->audio->id != current_id) {
	sh_audio_t *sh2 = NULL;
	if (mpctx->demuxer->audio->id >= 0)
	    sh2 = mpctx->demuxer->a_streams[mpctx->demuxer->audio->id];

        if (sh2) {
            sh2->ds = mpctx->demuxer->audio;
            mpctx->sh_audio = sh2;
            reinit_audio_chain();
        }

        //return audio language ID
        language = dvd_lang_from_aid(mpctx->stream, audio_id);
        lang[0] = language >> 8;
        lang[1] = language;
        lang[2] = 0;
        printf("audio_id = %d, language = %s\n",audio_id,lang);
        return language;
    }
    return 0;
}

int subtitle_current(void)
{
    char lang[] = "unknown";
    int language;

    language = dvd_lang_from_sid(mpctx->stream, dvdsub_id);
    lang[0] = language >> 8;
    lang[1] = language;
    lang[2] = 0;

    printf("dvdsub_id = %d, language = %s\n",dvdsub_id,lang);
    return language;
}

int audio_lan_current(void)
{
    char lang[] = "unknown";
    int language;

    language = dvd_lang_from_aid(mpctx->stream, audio_id);
    lang[0] = language >> 8;
    lang[1] = language;
    lang[2] = 0;

    printf("audio_id = %d, language = %s\n",audio_id,lang);
    return language;
}

int subtitle_get(int *lang_ptr)
{
    int i;
    char lang[] = "unknown";
    int id;
    dvd_priv_t *d;
    int language;

    d = mpctx->stream->priv;
    for (i = 0; i < d->nr_of_subtitles; i++)
    {
        language = d->subtitles[i].language;
        *(lang_ptr+i) = language;
        id = d->subtitles[i].id;
        lang[0] = language >> 8;
        lang[1] = language;
        lang[2] = 0;
        printf("dvdsub_id = %d, language = %s\n",id,lang);
    }

    return d->nr_of_subtitles;
}

int audio_lan_get(int *lang_ptr)
{
    int i;
    char lang[] = "unknown";
    int id;
    dvd_priv_t *d;
    int language;

    d = mpctx->stream->priv;
    for(i=0; i<d->nr_of_channels; i++)
    {
        language = d->audio_streams[i].language;
        *(lang_ptr+i) = language;
        id = d->audio_streams[i].id;
        lang[0] = language >> 8;
        lang[1] = language;
        lang[2] = 0;
        printf("audio_id = %d, language = %s\n",id,lang);
    }

    return d->nr_of_channels;
}

extern int is_dvdnav;	//Barry 2011-01-06
//+SkyViia_Vincent03232010
void ipc_handler()
{
    int		server_sockfd, client_sockfd;
    int		server_len, client_len;
    struct	sockaddr_un serv_address;
    struct	sockaddr_un client_address;
    unsigned char   sdata[1024];
    pGeneralPkt	recv_pkt;
    GeneralPkt	send_pkt;
    int 	ret;
    int     current_pos;
    int	    should_send=0;
    int 	i;
    int		language;
    int 	lang_cnt[12];
    int 	class_type;
    int 	class_value;
    float  	f_current_volume;
    int  	current_volume;
    int     err_cnt=0;
    int cmd_id = -1;
    char callback_str[1024];
    fd_set FD_rcv,rset;
    int 	reuse=1;

    /*  Remove any old socket and create an unnamed socket for the server.  */
    printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_sockfd == -1)
    {
        printf("%s - Cannot create socket: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }

    /*  Name the socket.  */
    serv_address.sun_family = AF_UNIX;
    server_len = sizeof(serv_address.sun_family) +
                 sprintf(serv_address.sun_path, " mplayersock");
    serv_address.sun_path[0] = 0;   //Use abstract name as domain socket

	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int sbuf, rbuf, len; //Skyviia_Vincent03292011 
    len = sizeof(sbuf);
    sbuf = 0; //default will be 2048
    setsockopt(server_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, len);
    len = sizeof(rbuf);
    rbuf = 0; //default will be 256
    setsockopt(server_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, len);

    len = sizeof(sbuf);
    getsockopt(server_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, &len);
    len = sizeof(rbuf);
    getsockopt(server_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
    printf("MPLAYER: send/receive socket buffer space %d/%d bytes\n", sbuf, rbuf);

#if 0
    ret = bind(server_sockfd, (struct sockaddr *)&serv_address, server_len);
    if(ret == -1)
    {
        printf("%s - Cannot bind: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }
#else
    // 20110926 charleslin - previous mplayer may not quit yet, retry it.
    for(i=0; i<100; i++){
           ret = bind(server_sockfd, (struct sockaddr *)&serv_address, server_len);
	   if(ret == 0){
		   break; //bind success
	   }else if(ret == -1){
                   if(errno == EADDRINUSE){
                           if((i % 10) == 0) printf("%s - %s, %dth retry\n", __func__, strerror(errno), i);
                           usleep(100 * 1000);
                           continue;
                   }
                   break; //bind fail
           }
    }
    if(ret != 0){
           printf("%s - Cannot bind: %s\n", __func__, strerror(errno));
           return;
    }
#endif

    /*  Create a connection queue and wait for clients.  */
    listen(server_sockfd, 5);

    sprintf(callback_str, "status: ready\0");
    ipc_callback(callback_str);

#if 1
    while(1) {
        /*  Accept a connection.  */
        client_len = sizeof(client_address);
        //client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
        if(client_sockfd == -1)
        {
            printf("%s - accept error: %s\n",__FUNCTION__ , strerror(errno));
            if (errno == EINTR)
            {
                //to do
                printf("accept(errno == EINTR)\n");
            }
            continue;
        }

        memset(&sdata,0,sizeof(sdata));
        client_len = recv(client_sockfd, sdata, sizeof(sdata),0);
        if(client_len <= 0)
        {
            printf("%s - recv error: %s,client_len[%d]\n",__FUNCTION__ , strerror(errno),client_len);
            if (errno == EINTR)
            {
                //to do
                printf("recv(errno == EINTR)\n");
            }
            close(client_sockfd);
            continue;
        }

        if(sdata[0] == SET || sdata[0] == GET)
        {
            recv_pkt = (pGeneralPkt)sdata;
            class_type = recv_pkt->Subtype&0xf0;
            class_value = recv_pkt->Subtype&0x0f;

            if(recv_pkt->Control == SET)
            {
                switch(class_type)
                {
                case PLAY_PROC:
                {
                    switch(class_value)
                    {
                    case PLAY_PAUSE:
                        code_domain = 112; //pause
                        break;
                    case PLAY_STOP:
                        if(!entry_while)
                        {
                            exit_player_with_rc(EXIT_QUIT,0);
                        }
                        else
                        {
                            mpctx->eof = 1;
                            force_quit = 1;
                        }
                        printf("stop mpctx->eof[%d]\n",mpctx->eof);
//		    					//+SkyViia_Vincent06172010
//		    					if(pthread_mutex_lock(&mut))
//    	    						printf("Cannot lock condition mutex-1\n");
//		    					usleep(8000);
////	    						code_domain = 113; //stop
//		    					close(client_sockfd);
//		    					close(server_sockfd);
//		    					exit_player_with_rc(EXIT_QUIT,0);
//		    					pthread_mutex_unlock(&mut);
//		    					//SkyViia_Vincent06172010+
                        break;
                    case PLAY_NORMAL_SPEED:
                        code_domain = 256; //Play speed normal
                        break;
                    }
                    break;
                }
                case SEEK_PROC:
                {
                    switch(class_value)
                    {
                    case SEEK_SEC:
                        //todo
                        printf("Not supported\n");
                        break;
                    case SEEK_FF_10_SEC:
                        code_domain = 272; //FF 10 secs
                        break;
                    case SEEK_FB_10_SEC:
                        code_domain = 273; //Rev 10secs
                        break;
                    }
                    break;
                }
                case VOLUME_PROC:
                {
                    switch(class_value)
                    {
                    case VOLUME_MUTE:
                        code_domain = 109; //mute
                        break;
                    case VOLUME_UP:
                        code_domain = 42; //increase volume.
                        break;
                    case VOLUME_DOWN:
                        code_domain = 47; //Decrease volume.
                        break;
                    }
                    break;
                }
                case FF_PROC:
                {
                    switch(class_value)
                    {
                    case FF_SPEED2X:
                        code_domain = 125; //double current playback speed.
                        break;
                    case FF_SPEED4X:
                        //todo
                        printf("Not supported\n");
                        break;
                    case FF_SPEED8X:
                        //todo
                        printf("Not supported\n");
                        break;
                    case FF_SPEED16X:
                        //todo
                        printf("Not supported\n");
                        break;
                    }
                    break;
                }
                case FB_PROC:
                {
                    switch(class_value)
                    {
                    case FB_SPEED2X:
                        code_domain = 123;	//Fuchun 2009.12.15
                        break;
                    case FB_SPEED4X:
                        //todo
                        printf("Not supported\n");
                        break;
                    case FB_SPEED8X:
                        //todo
                        printf("Not supported\n");
                        break;
                    case FB_SPEED16X:
                        //todo
                        printf("Not supported\n");
                        break;
                    }
                    break;
                }
                case SUB_PROC:	//charleskao 10022009
                {
                    switch(class_value)
                    {
                    case SUB_UP:
                        language = subtitle_up();
                        send_pkt.Data[0] = language >> 8;
                        send_pkt.Data[1] = language;
                        send_pkt.Data[2] = 0;
                        ret = send(client_sockfd,send_pkt.Data, 4, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        break;
                    case SUB_DOWN:
                        language = subtitle_down();
                        send_pkt.Data[0] = language >> 8;
                        send_pkt.Data[1] = language;
                        send_pkt.Data[2] = 0;
                        ret = send(client_sockfd,send_pkt.Data, 4, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        break;
                    }
                    break;
                }
                case AUDIO_PROC:	//charleskao 10022009
                {
                    switch(class_value)
                    {
                    case AUDIO_UP:
                        if(pthread_mutex_lock(&mut))
                            printf("Cannot lock condition mutex-1\n");
                        usleep(30000);

                        language = audio_next();
                        send_pkt.Data[0] = language >> 8;
                        send_pkt.Data[1] = language;
                        send_pkt.Data[2] = "\0";
                        ret = send(client_sockfd,send_pkt.Data, 2, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

                        pthread_mutex_unlock(&mut);
                        break;
                    }
                    break;
                }
                case CMD_PROC:
                {
                    switch(class_value)
                    {
                    case CMD_INST:
                        printf("recv_pkt->CmdInst[%s]\n",recv_pkt->CmdInst);
                        domainsocket_cmd = mp_input_parse_cmd(recv_pkt->CmdInst);
                        printf("domainsocket_cmd-id[%d]\n",domainsocket_cmd->id);
                        cmd_id = domainsocket_cmd->id;
                        break;
                    }
                    break;
                }
                default:
                    break;
                } //switch(class_type)
            }
            else if(recv_pkt->Control == GET)
            {
                memset(&send_pkt.Data,0,sizeof(send_pkt.Data));
                switch(class_type)
                {
                case GET_PLAYER_STATUS_PROC:
                {
                    switch(class_value)
                    {
                    case PLAYER_STATUS_PLAYING:
                        send_pkt.Control = GET;
                        send_pkt.Subtype = GET_PLAYER_STATUS_PROC | PLAYER_STATUS_PLAYING;

                        if(!start_play)
                        {
                            send_pkt.Data[0] = PLAYER_STATUS_STOP;  //stop
                        }
                        else
                        {
                            if(is_pause)
                                send_pkt.Data[0] = PLAYER_STATUS_PAUSE; //pause
                            else
                                send_pkt.Data[0] = PLAYER_STATUS_PLAYING; //playing
                        }

                        send_pkt.DataSize = sizeof(send_pkt.Data);

                        should_send = 1;
                        break;

                    default:
                        //todo
                        break;
                    }
                }
                break;

                case GET_CURRENT_POS_PROC:
                {
                    switch(class_value)
                    {
                    case GET_CURRENT_POS_SEC:
#ifdef ADJUST_STREAM_PTS	//Barry 2011-01-06
                        if ((!is_dvdnav) && mpctx && mpctx->sh_video && mpctx->demuxer &&
                                (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS))
                            current_pos = my_current_pts - (int)correct_sub_pts;
                        else
                            current_pos = my_current_pts;
#else
                        current_pos = my_current_pts;
#endif
                        send_pkt.Control = GET;
                        send_pkt.Subtype = GET_CURRENT_POS_PROC | GET_CURRENT_POS_SEC;

                        send_pkt.Data[0] = (char)(current_pos & 0x000000FF);
                        send_pkt.Data[1] = (char)(current_pos >> 8);
                        send_pkt.Data[2] = (char)(current_pos >> 16);
                        send_pkt.Data[3] = (char)(current_pos >> 24);

                        send_pkt.DataSize = sizeof(send_pkt.Data);

                        should_send = 1;
                        break;
                    case GET_CURRENT_POS_SEC_CALL_BACK_ON:
                        printf("== callback get curpos: on ==\n");
                        if(get_curpos_on == IPC_THREAD_IDLE || get_curpos_on == IPC_THREAD_END)
                            ipc_callback_thread_start(&ipc_thread_curpos_id, &get_curpos_on, ipc_callback_curpos);
                        should_send = 0;
                        break;
                    case GET_CURRENT_POS_SEC_CALL_BACK_OFF:
                        if (get_curpos_on == IPC_THREAD_RUNNING) {
                            printf("== callback get curpos: off ==\n");
                            ipc_callback_thread_stop(&ipc_thread_curpos_id, &get_curpos_on);
                        } else {
                            printf("== callback get curpos: isn't on ==\n");
                        }
                        should_send = 0;
                        break;
                    default:
                        //todo
                        break;
                    }
                }
                break;

                case GET_BUFFERING_STATUS_PROC:
                {
                    switch(class_value)
                    {
                    case GET_BUFFERING_PERCENTAGE:
                    {
                        int fill_status = cache_fill_status;
                        send_pkt.Control = GET;
                        send_pkt.Subtype = GET_BUFFERING_STATUS_PROC | GET_BUFFERING_PERCENTAGE;

                        send_pkt.Data[0] = (char)((fill_status >> 0) & 0xFF);
                        send_pkt.Data[1] = (char)((fill_status >> 8) & 0xFF);
                        send_pkt.Data[2] = (char)((fill_status >> 16) & 0xFF);
                        send_pkt.Data[3] = (char)((fill_status >> 24) & 0xFF);

                        send_pkt.DataSize = sizeof(send_pkt.Data);

                        should_send = 1;
                        break;
                    }
                    case GET_BUFFERING_CALL_BACK_ON:
                    {
                        printf("== callback get stream buffering status: on ==\n");
                        if(get_buffering_on == IPC_THREAD_IDLE || get_buffering_on == IPC_THREAD_END)
                        {
	    			pthread_mutex_init(&cbmtx,NULL);
	    			pthread_cond_init(&cbcond,NULL);
                            ipc_callback_thread_start(&ipc_thread_buffering_id, &get_buffering_on, ipc_callback_buffering);
                        }
                        should_send = 0;
                        break;
                    }
                    case GET_BUFFERING_CALL_BACK_OFF:
                    {
                        if (get_buffering_on == IPC_THREAD_RUNNING) {
                            printf("== callback get stream buffering status: off ==\n");
                            buffering_pthread_flag = 1;
                            ipc_callback_thread_stop(&ipc_thread_buffering_id, &get_buffering_on);
                        } else {
                            printf("== callback get stream buffering status: isn't on ==\n");
                        }
                        should_send = 0;
                        break;
                    }
                    default:
                        //todo
                        break;
                    }
                }
                break;

                case GET_DURATION_PROC:
                {
                    switch(class_value)
                    {
                    case GET_DURATION_SEC:
                        duration_sec_cur = duration_sec_old = demuxer_get_time_length(mpctx->demuxer);

                        send_pkt.Data[0] = (char)(duration_sec_cur & 0x000000FF);
                        send_pkt.Data[1] = (char)(duration_sec_cur >> 8);
                        send_pkt.Data[2] = (char)(duration_sec_cur >> 16);
                        send_pkt.Data[3] = (char)(duration_sec_cur >> 24);

                        send_pkt.DataSize = sizeof(send_pkt.Data);

                        should_send = 1;
                        break;

                    default:
                        //todo
                        break;
                    }
                }
                break;

                case GET_AUDIO_LANG_PROC:
                {
                    switch(class_value)
                    {
                    case GET_AUDIO_MULTILANG:
                        //todo
                        printf("Not supported\n");
                        should_send = 1;
                        break;

                    default:
                        //todo
                        break;
                    }
                }
                break;

                case GET_SUBINFO_PROC:
                {
                    switch(class_value)
                    {
                    case GET_CURRENT_SUB:
                        language = subtitle_current();
                        send_pkt.Data[0] = language >> 8;
                        send_pkt.Data[1] = language;
                        send_pkt.Data[2] = 0;
                        ret = send(client_sockfd,send_pkt.Data, 4, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        break;

                    case GET_SUB:
                        language = subtitle_get(lang_cnt);
                        for(i=0; i<language; i++)
                        {
                            send_pkt.Data[0] = lang_cnt[i] >> 8;
                            send_pkt.Data[1] = lang_cnt[i];
                            send_pkt.Data[2] = 0;
                            ret = send(client_sockfd,send_pkt.Data, 4, 0);
                            if(ret == -1)
                                printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        }
                        break;

                    default:
                        break;
                    }
                    break;
                }

                case GET_AUDIOINFO_PROC:
                {
                    switch(class_value)
                    {
                    case GET_CURRENT_AUDIO_LAN:
                        language = audio_lan_current();
                        send_pkt.Data[0] = language >> 8;
                        send_pkt.Data[1] = language;
                        send_pkt.Data[2] = 0;
                        ret = send(client_sockfd,send_pkt.Data, 4, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        break;

                    case GET_AUDIO_LAN:
                        language = audio_lan_get(lang_cnt);
                        for(i=0; i<language; i++)
                        {
                            send_pkt.Data[0] = lang_cnt[i] >> 8;
                            send_pkt.Data[1] = lang_cnt[i];
                            send_pkt.Data[2] = 0;
                            ret = send(client_sockfd,send_pkt.Data, 4, 0);
                            if(ret == -1)
                                printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        }
                        break;
                    case GET_CURRENT_AUDIO_VOLUME:
                        mixer_getbothvolume(&mpctx->mixer, (float*)&f_current_volume);
                        current_volume = (int)f_current_volume;

                        send_pkt.Data[0] = (char)(current_volume & 0x000000FF);
                        send_pkt.Data[1] = (char)(current_volume >> 8);
                        send_pkt.Data[2] = (char)(current_volume >> 16);
                        send_pkt.Data[3] = (char)(current_volume >> 24);
                        ret = send(client_sockfd,send_pkt.Data, 4, 0);
                        if(ret == -1)
                            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                        break;
                    default:
                        break;
                    }
                    break;
                }

                default:
                    printf("<%s>:%s-->Error: invalid_arg\n", __FILE__, __FUNCTION__);
                    break;
                }//switch class

                if(should_send)
                {
                    ret = send(client_sockfd,send_pkt.Data, 4, 0);
                    if(ret == -1)
                        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
                    should_send = 0;
                }
            }
        }
        else if(sdata != NULL || client_len !=0)
        {
            printf("Cmd str[%s]\n",sdata);
#if 0
            if(!strcmp(sdata, "switch_audio"))
            {
                if((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
                {
                    printf("==== TS can't switch audio now!!!\n");
                    cmd_id = -1;
//					printf("==== audio_change_success %d!!!\n", audio_change_success);
                    //sprintf(callback_str, "AUDIO: unknow\0");
                    sprintf(callback_str, "AUDIO: (%d) %s\0",1,"audio");
                    ipc_callback(callback_str);
                }
                else
                {
                    domainsocket_cmd = mp_input_parse_cmd(sdata);
                    printf("domainsocket_cmd-id[%d]\n",domainsocket_cmd->id);
                    cmd_id = domainsocket_cmd->id;
                }
            }
            else
#endif
            {
                domainsocket_cmd = mp_input_parse_cmd(sdata);
//				printf("domainsocket_cmd-id[%d]\n",domainsocket_cmd->id);
                cmd_id = domainsocket_cmd->id;

                if(cmd_id == MP_CMD_STOP && (mpctx->eof != 0 ||  entry_while == 0))
                {
                    ipc_callback_stop();
                }
                else if(cmd_id == MP_CMD_STOP && mpctx->eof == 0)	//Barry 2010-11-18
                {
                    mpctx->startup_decode_retry = 0;
                }

            }
//			printf("===== cmd_id %d	sdata [%s]\n", cmd_id, sdata);
        }
        else
        {
            printf("invalid input data\n");
            close(client_sockfd);
            continue;
            //err_cnt++;
            //if(err_cnt > 5)
            //return;
        }

        while((cmd_id == MP_CMD_SUB_POS || cmd_id == MP_CMD_SUB_SCALE || cmd_id == MP_CMD_SUB_DELAY
                || cmd_id == MP_CMD_SET_SUBCP || cmd_id == MP_CMD_SUB_COLOR_NUM) && sub_change_success == 0)
            usec_sleep(20000);
        if((cmd_id == MP_CMD_SUB_POS || cmd_id == MP_CMD_SUB_SCALE || cmd_id == MP_CMD_SUB_DELAY
                || cmd_id == MP_CMD_SET_SUBCP || cmd_id == MP_CMD_SUB_COLOR_NUM) && sub_change_success != 0)
        {
            /*			ret = send(client_sockfd, subtitle_names, strlen(subtitle_names), 0);
            			if(ret == -1)
            				printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

            			if(cmd_id == MP_CMD_SUB_SELECT)
            			{
            				if(sub_change_success == 1)
            					sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
            				else if(sub_change_success == 2)
            					sprintf(callback_str, "SUBTITLE: partial %s\0", subtitle_names);
            				else
            					sprintf(callback_str, "SUBTITLE: %s\0", subtitle_names);
            			}
            			else
            */
            {
                sprintf(callback_str, "SUBTITLE: %s\0", sdata);
            }
            ipc_callback(callback_str);
            sub_change_success = 0;
            cmd_id = -1;
        }

//		while(cmd_id == MP_CMD_SWITCH_AUDIO && audio_change_success == 0) usec_sleep(20000);
//		if(cmd_id == MP_CMD_SWITCH_AUDIO && audio_change_success == 1)
//		{
///*			ret = send(client_sockfd, audio_names, strlen(audio_names), 0);
//			if(ret == -1)
//				printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
//*/
//			sprintf(callback_str, "AUDIO: %s\0", audio_names);
//			ipc_callback(callback_str);
//			audio_change_success = 0;
//			cmd_id = -1;
//		}

        close(client_sockfd);
    }
#endif // if 0
//////////////////////////////////////////////////////

}

void ipc_callback(char *datastr)
{
    int     sockfd;
    struct	sockaddr_un serv_addr;
    int     result;
    void    *send_ptr;
    int		server_len;

    if(print_callback_str)
        printf("ipc_callback(%s)\n",datastr);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    server_len = sizeof(serv_addr.sun_family) +
                 sprintf(serv_addr.sun_path, " mplayersock_callback");

    serv_addr.sun_path[0] = 0;   //Use abstract name as domain socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("%s - Cannot create socket: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }

    result = connect(sockfd, (struct sockaddr *)&serv_addr, server_len);
    if(result == -1)
    {
        printf("%s - Cannot connect socket: %s\n",__FUNCTION__ , strerror(errno));
        close(sockfd);
        return;
    }

    send_ptr = datastr;
    result = send(sockfd,send_ptr, strlen(datastr), 0);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    close(sockfd);
}

void ipc_callback_error(void)
{
    char callback_str[24];

    if (get_curpos_on == IPC_THREAD_RUNNING)
        ipc_callback_thread_stop(&ipc_thread_curpos_id, &get_curpos_on);

    if (get_buffering_on == IPC_THREAD_RUNNING)
    {
        buffering_pthread_flag = 1;
        ipc_callback_thread_stop(&ipc_thread_buffering_id, &get_buffering_on);
    }

    start_play = PLAYER_STATUS_STOP;

    strcpy(callback_str,"status: error: 0\0");
    ipc_callback(callback_str);
}

void ipc_callback_seek_by_time_done(void)
{
    char callback_str[48];
    sprintf(callback_str,"status: seektimedone\0");
    ipc_callback(callback_str);
    seek_by_time_done = 0;
}

void ipc_callback_stop(void)
{
    char callback_str[24];
    //Barry 2011-01-06
    if (get_curpos_on == IPC_THREAD_RUNNING)
        ipc_callback_thread_stop(&ipc_thread_curpos_id, &get_curpos_on);

    if (get_buffering_on == IPC_THREAD_RUNNING)
    {
        buffering_pthread_flag = 1;
        ipc_callback_thread_stop(&ipc_thread_buffering_id, &get_buffering_on);
    }

    start_play = PLAYER_STATUS_STOP;

    strcpy(callback_str,"status: stop\0");
    ipc_callback(callback_str);
}
//SkyViia_Vincent03232010+

void ipc_callback_fast(void)
{
    char callback_str[24];
    int speed_int = 1, i;

    start_play = PLAYER_STATUS_PLAYING;

    if(speed_mult > 0)
    {
        for(i = 0; i < speed_mult; i++) speed_int *= 2;
        sprintf(callback_str,"status: speed %d\0", speed_int);
    }
    else if(speed_mult < 0)
    {
        for(i = 0; i < ((-1)*speed_mult); i++) speed_int *= 2;
        sprintf(callback_str,"status: speed %d\0", (-1)*speed_int);
    }
    else
        strcpy(callback_str,"status: speed 1\0");

    ipc_callback(callback_str);
}

void ipc_callback_audio_fast(void)
{
    char callback_str[48];
    float speed_float = 1.0, i;

    start_play = PLAYER_STATUS_PLAYING;

    if(playback_speed == 1.0)
    {
        if(audio_speed > 0)
        {
            for(i = 1; i < audio_speed; i++)
                speed_float *= 2.0;
            sprintf(callback_str,"status: audio speed %2.2f\0", speed_float);
        }
        else if(audio_speed < 0)
        {
            if(audio_speed == -1)
                sprintf(callback_str,"status: audio speed -1.50\0");
            else
            {
                for(i = 1; i < ((-1)*audio_speed); i++)
                    speed_float *= 2.0;
                sprintf(callback_str,"status: audio speed %2.2f\0", (-1.0)*speed_float);
            }
        }
        else
            strcpy(callback_str,"status: audio speed 1.00\0");
    }
    else
        sprintf(callback_str,"status: audio speed %2.2f\0", playback_speed);
    ipc_callback(callback_str);
}

void ipc_callback_curpos(void)
{
    char callback_str[24];
    int current_pos=0,old_pos=0;
    int pre_FR_staus = 0;    //Polun 2011-07-19
    
    printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
    get_curpos_on = IPC_THREAD_RUNNING;

    while(get_curpos_on == IPC_THREAD_RUNNING)
    {
        if (!mpctx || (video_id != -2 && !mpctx->sh_video && mpctx->d_audio->eof) || !mpctx->demuxer )
		break;
#ifdef ADJUST_STREAM_PTS	//Barry 2011-01-06
        if ((!is_dvdnav) && mpctx && mpctx->sh_video && mpctx->demuxer &&
                (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS))
            current_pos = my_current_pts - (int)correct_sub_pts;
        else
        {
		current_pos = my_current_pts;
		if (!mpctx->sh_video && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
		{
			//printf("### In [%s][%d] my_current_pts is [%d]  current_pos[%d] old_pos[%d] correct_sub_pts[%d]###\n", __func__, __LINE__, my_current_pts, current_pos, old_pos, (int)correct_sub_pts);
			current_pos = my_current_pts - (int)correct_sub_pts;
		}
        }
        //Polun 2011-07-18 ++s for FR current_pos to 0
        //if(current_pos == 0 && pre_FR_staus == 1)
        if(current_pos == 0 && pre_FR_staus == 1 && old_pos <= (abs(speed_mult)*2)-1)//Polun 2011-11-11 fixed mantis 6481 playback Formula.m2v Press FR key，timebar display 00:00:00 second 
        {
        	old_pos = 0;
              #if 0 //Polun 2012-01-10 fixed mantis 6780 playerback youtobe eof handle 5s issue.
        	sprintf(callback_str,"POSITION: %d\0", current_pos);
              #else
              if(current_pos <= duration_sec_cur || duration_sec_cur == 0)
        	     sprintf(callback_str,"POSITION: %d\0", current_pos);
              else
                   sprintf(callback_str,"POSITION: %d\0", duration_sec_cur);
              #endif
		print_callback_str = 0;
		ipc_callback(callback_str);
		print_callback_str = 1;
        }
        //Polun 2011-07-18 ++e for FR current_pos to 0
#else
        current_pos = my_current_pts;
#endif
        if(old_pos != current_pos && current_pos != 0)
        {
        	if (mpctx->sh_video)
        	{
                     #if 0 //Polun 2012-01-10 fixed mantis 6780 playerback youtobe eof handle 5s issue.
        		   sprintf(callback_str,"POSITION: %d\0", current_pos);
                     #else
                     if(current_pos <= duration_sec_cur || duration_sec_cur == 0 )
        		   sprintf(callback_str,"POSITION: %d\0", current_pos);
                     else
                        sprintf(callback_str,"POSITION: %d\0", duration_sec_cur);
                     #endif
			print_callback_str = 0;
			if (!(((speed_mult < 0) &&(current_pos > old_pos) || (speed_mult > 0 && current_pos < old_pos) )) && current_pos >= 0)
				ipc_callback(callback_str);
			else
				fprintf(stderr, "#### In [%s][%d] current_pos [%d] , old_pos [%d] it's failed, speed_mult is [%d]###\n", __func__, __LINE__, current_pos, old_pos, speed_mult);
			print_callback_str = 1;
			if(current_pos != 0)
				old_pos = current_pos;
        	}
		else
		{
			//audio-only case:
			//if (!((current_pos < old_pos) && !audio_speed))        //Polun 2011-07-19
			//Polun 2011-07-27 add (audio_seek_flg == 1 && current_pos < old_pos) for audio play skip key time bar updata.
			if (!((current_pos < old_pos) && !audio_speed) || (pre_FR_staus == 1 && current_pos != old_pos) || (audio_seek_flg == 1 && current_pos < old_pos))
			{
                           #if 0 //Polun 2012-01-10 fixed mantis 6780 playerback youtobe eof handle 5s issue.
                		 sprintf(callback_str,"POSITION: %d\0", current_pos);
                           #else
                           if(current_pos <= duration_sec_cur || duration_sec_cur == 0)
                		 sprintf(callback_str,"POSITION: %d\0", current_pos);
                           else
                              sprintf(callback_str,"POSITION: %d\0", duration_sec_cur);
                           #endif
				print_callback_str = 0;
				ipc_callback(callback_str);
				print_callback_str = 1;
				//Polun 2011-07-27 ++s for audio play skip key time bar updata.
				if(audio_seek_flg == 1 && current_pos < old_pos)
					audio_seek_flg = 0;
				//Polun 2011-07-27 ++e
				if(current_pos != 0)
					old_pos = current_pos;
			}
		}
        }
        //Polun 2011-07-19 ++s
        if((audio_speed < 0) || (speed_mult < 0) )
            pre_FR_staus = 1 ;
        else
            pre_FR_staus = 0 ;
        //Polun 2011-07-19 ++e
        usleep(500000);
    }
    printf("### In [%s][%d] exit ###\n", __func__, __LINE__);
    get_curpos_on = IPC_THREAD_END;
}

void ipc_callback_buffering(void)
{
    char callback_str[24];
    int prev_percent = 0;
    int curr_percent;

    printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
    get_buffering_on = IPC_THREAD_RUNNING;

    while(get_buffering_on == IPC_THREAD_RUNNING)
    {
        curr_percent = cache_fill_status;
        if(curr_percent != prev_percent) {
            if(stream_cache_size > 0)
                sprintf(callback_str, "BUFFERING: %d\0", curr_percent);
            else
                sprintf(callback_str, "BUFFERING: %d\0", -1);

            print_callback_str = 0;
            ipc_callback(callback_str);
            print_callback_str = 1;
            prev_percent = curr_percent;
        }
        //usleep(500000);
        pwait_event_timeout(&cbcond, &cbmtx, 1);
    }
    printf("### In [%s][%d] exit ###\n", __func__, __LINE__);
    get_buffering_on = IPC_THREAD_END;
}

void ipc_callback_duration(void)
{
    char callback_str[24];

    sprintf(callback_str,"DURATION: %d\0", duration_sec_cur);
    ipc_callback(callback_str);
}

#if 1	//Barry 2011-04-20
void ipc_callback_scan_type(void)
{
	char callback_str[24];
	if (pre_isprogressive_flag == -1)
	{
		// 20110711 charleslin allow zoom in/out for 720x576 interlaced video
#if 0	//Fuchun 20110808 revise condition
		if (svsd_state->isprogressive || mpctx->sh_video->disp_w <= 720)
#else
		if(!sky_room_disable(mpctx->sh_video))
#endif
			strcpy(callback_str,"video: progressive\0");
		else
			strcpy(callback_str,"video: interlaced\0");
		ipc_callback(callback_str);
		pre_isprogressive_flag = svsd_state->isprogressive;
	}
	else
	{
		if (pre_isprogressive_flag != svsd_state->isprogressive)
		{
#if 0	//Fuchun 20110808 revise condition
			if (svsd_state->isprogressive)
#else
			if(!sky_room_disable(mpctx->sh_video))
#endif
				strcpy(callback_str,"video: progressive\0");
			else
				strcpy(callback_str,"video: interlaced\0");
			ipc_callback(callback_str);
			pre_isprogressive_flag = svsd_state->isprogressive;
		}
	}
}
#endif

void ipc_callback_thread_start(pthread_t *tid,  int *tstatus, void *(*start_routine)(void*))
{
    if(*tstatus == IPC_THREAD_IDLE || *tstatus == IPC_THREAD_END)
        pthread_create(tid, NULL, start_routine, NULL);
}

void ipc_callback_thread_stop(pthread_t *tid,  int *tstatus)
{
    int timeout = 0;

    printf("@@@ In [%s][%d] @@@\n", __func__, __LINE__);
    if (*tstatus != IPC_THREAD_RUNNING) {
        return;
    }

    if (buffering_pthread_flag == 1)
    {
	pwake_up_all(&cbcond, &cbmtx);
    }

    *tstatus = IPC_THREAD_STOP;
    while(*tstatus != IPC_THREAD_END && timeout < MAX_TIME_OUT_COUNT)
    {
        usleep(10000);
        timeout++;
    }

    pthread_cancel(*tid);
    pthread_join(*tid, NULL);

    if(buffering_pthread_flag)
    {
	pthread_cond_destroy(&cbcond);
	pthread_mutex_destroy(&cbmtx);
	buffering_pthread_flag = 0;
    }
    *tstatus = IPC_THREAD_END;
    printf("@@@ In [%s][%d] timeout is [%d] max [%d] @@@\n", __func__, __LINE__, timeout, MAX_TIME_OUT_COUNT);
}

void ipc_callback_audio_sub(void)
{
  if(!skydroid)
  	return;

  char callback_str[1024];
  extern char subtitle_names[];

  mp_property_print("switch_audio", mpctx);
  sprintf(callback_str, "AUDIO: %s\0", audio_names);
  ipc_callback(callback_str);

  mp_property_print("sub", mpctx);
  if(sub_change_success == 1)
	sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
  else if(sub_change_success == 2)
	sprintf(callback_str, "SUBTITLE: partial %s\0", subtitle_names);
  else
	sprintf(callback_str, "SUBTITLE: %s\0", subtitle_names);
  sub_change_success = 0;
  ipc_callback(callback_str);
}

//=============================
//Robert 20100712 add SVSD ipc routine

int svsd_ipc_client_sockfd = -1;
int svsd_ipc_server_sockfd = -1;
int svsd_ipc_back_sockfd = -1;
int svsd_cmd_idx = 0;
fd_set FD_rcv;
struct timeval tv_svsd;

int svsd_ipc_init_socket(void)
{
    struct	sockaddr_un cli_addr;/*, serv_addr;*/
    int     result;
    int		/*server_len,*/ client_len;
    int retry_cnts = 0;

    /* If novideo, don't init svsd */
    if (video_id == -2 )
    {
        if (svsd_shm_id == 0)
            svsd_shm_id = shmget((key_t) 2688, 1024+1024*8+128, 0666);
        if (svsd_shm_array == NULL)
            svsd_shm_array = (unsigned char *)shmat(svsd_shm_id, (void *)0, 0);
        svsd_state = (vdec_shm_t *)svsd_shm_array;
        return 0;
    }
//20100730 fix svsd socket not close issue
    if (svsd_ipc_client_sockfd != -1)
    {
        printf("%s - last svsd_ipc_client_sockfd = %d\n", __FUNCTION__, svsd_ipc_client_sockfd);
        close(svsd_ipc_client_sockfd);
        svsd_ipc_client_sockfd = -1;
    }

    if (svsd_shm_id == 0)
        svsd_shm_id = shmget((key_t) 2688, 1024+1024*8+128, 0666);
    if (svsd_shm_array == NULL)
        svsd_shm_array = (unsigned char *)shmat(svsd_shm_id, (void *)0, 0);
    svsd_state = (vdec_shm_t *)svsd_shm_array;
    printf("mplayer: svsd_shm_id=%d svsd_shm_array=0x%x  val:%d\n", svsd_shm_id, svsd_shm_array, svsd_state->queue_length);

svsd_init_retry:
    //printf("reset decode addr and length\n");
    svsd_state->decode_addr = 0;
    svsd_state->decode_length = 0;
    svsd_state->send_count = 0;
#ifdef NEW_SVSD_CMD
    svsd_state->send_cmd_id= 0;	//send cmd idx
#endif

    svsd_ipc_client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(svsd_ipc_client_sockfd == -1)
    {
        printf("%s - Cannot create socket: %s\n",__FUNCTION__ , strerror(errno));
        usec_sleep(5000);
        if (retry_cnts++ < 3)
            goto svsd_init_retry;
//		return;
    }
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sun_family = AF_UNIX;
    client_len = sizeof(cli_addr.sun_family) +
                 sprintf(cli_addr.sun_path, " svsdsock");

    cli_addr.sun_path[0] = 0;   //Use abstract name as domain socket

    fcntl( svsd_ipc_client_sockfd, F_SETFL, fcntl(svsd_ipc_client_sockfd, F_GETFL) | O_NONBLOCK );
#ifdef NEW_SVSD_CMD
    int sbuf, rbuf, len;
    len = sizeof(sbuf);
    getsockopt(svsd_ipc_client_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, &len);
    len = sizeof(rbuf);
    getsockopt(svsd_ipc_client_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
    printf("SVSD: send/receive socket buffer space %d/%d bytes\n", sbuf, rbuf);

    len = sizeof(sbuf);
    sbuf = 16;
    setsockopt(svsd_ipc_client_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, len);
    len = sizeof(rbuf);
    rbuf = 1024;
    setsockopt(svsd_ipc_client_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, len);
#endif
    retry_cnts = 0;

svsd_init_connect_retry:

    result = connect(svsd_ipc_client_sockfd, (struct sockaddr *)&cli_addr, client_len);
    if(result == -1)
    {
        printf("%s - Cannot connect socket: %s\n",__FUNCTION__ , strerror(errno));
        usec_sleep(5000);
        if (retry_cnts++ < 5)
        {
            goto svsd_init_connect_retry;
        }
        exit(0);
//		return;
    }

    svsd_ipc_init_video();
}

int svsd_ipc_init_video(void)
{
    int     result;
    int send_buf_len = 8;
    char send_buf[send_buf_len];
    char recv_buf[4];

    /*
        if (svsd_shm_id == 0)
    	    svsd_shm_id = shmget((key_t) 2688, 1024+1024*8+128, 0666);
        if (svsd_shm_array == NULL)
    	    svsd_shm_array = (unsigned char *)shmat(svsd_shm_id, (void *)0, 0);
    printf("mplayer: svsd_shm_id=%d svsd_shm_array=0x%x  val:%d\n", svsd_shm_id, svsd_shm_array, svsd_state->queue_length);
    */

    svsd_state->send_count = 0;
#ifdef NEW_SVSD_CMD
    svsd_state->send_cmd_id = 0;	//send cmd idx
#endif

    send_buf[0] = 0x10;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));
    printf("mplayer: send init to svsd\n");

#ifndef NEW_SVSD_CMD
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    /*
            FD_ZERO(&FD_rcv);
            FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
    	while (1)
    	{
    	        tv_svsd.tv_usec = 10000;
            	tv_svsd.tv_sec = 0;
            FD_ZERO(&FD_rcv);
            FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
    		result = select(svsd_ipc_client_sockfd+1, &FD_rcv, NULL, NULL, &tv_svsd);
    		if (result > 0)
    			break;
    	}
    */
    result = recv(svsd_ipc_client_sockfd, recv_buf, 4, MSG_DONTWAIT);
#else

    int last_svsd_idx = svsd_state->recv_cmd_id;
    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id += 1;
    svsd_cmd_idx++;

    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    while (last_svsd_idx == svsd_state->recv_cmd_id)
    {
        usleep(500);
//printf("MPlayer: [24]=%d\n", svsd_state->recv_cmd_id);
    }

#endif

    printf("mplayer: got svsd back, ret=%d\n", recv_buf[0]);
    return recv_buf[0];
}

int svsd_ipc_get_video_cnts(void)
{
    int     result;
    int send_buf_len = 8;
    char send_buf[send_buf_len];
    char recv_buf[4];

    send_buf[0] = 0x11;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    while (1)
    {
        tv_svsd.tv_usec = 10000;
        tv_svsd.tv_sec = 0;
        FD_ZERO(&FD_rcv);
        FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
        result = select(svsd_ipc_client_sockfd+1, &FD_rcv, NULL, NULL, &tv_svsd);
        if (result > 0)
            break;
        else if (result == 0)
        {
            printf("mplayer: get video cnts timeout...\n");
        }
    }
    result = recv(svsd_ipc_client_sockfd, recv_buf, 4, MSG_DONTWAIT);
//printf("mplayer: got svsd back, ret=%d\n", recv_buf[0]);
    return recv_buf[0];
}

int svsd_ipc_drop_video_cnts(void)
{
    int     result;
    int send_buf_len = 8;
    char send_buf[send_buf_len];
    char recv_buf[4];
//    int timeout_cnts = 5;

    send_buf[0] = 0x13;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));
#ifndef NEW_SVSD_CMD
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    fsync(svsd_ipc_client_sockfd);
    if(result == -1)
    {
        if (!quiet)
            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
    }
#else
    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id+= 1;
    svsd_cmd_idx++;

    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

#endif

    /*
            FD_ZERO(&FD_rcv);
            FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
    	while (1)
    	{
    	        tv_svsd.tv_usec = 10000;
            	tv_svsd.tv_sec = 0;
            FD_ZERO(&FD_rcv);
            FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
    		result = select(svsd_ipc_client_sockfd+1, &FD_rcv, NULL, NULL, &tv_svsd);
    		if (result > 0)
    			break;
    		else if (result == 0)
    		{
    			if (--timeout_cnts<=0)
    				break;
    		}
    	}

    	if (timeout_cnts > 0)
    	{
    	        result = recv(svsd_ipc_client_sockfd, recv_buf, 4, MSG_DONTWAIT);
    	}
    	else
    	{
    printf("mplayer: svsd_ipc_drop_video_cnts timeout...\n");
    	}
    */
//printf("mplayer: got svsd back, ret=%d\n", recv_buf[0]);
    return recv_buf[0];
}

int svsd_ipc_reset(void)
{
    int     result;
    int send_buf_len = 8;
    char send_buf[send_buf_len];
//    char recv_buf[4];

    send_buf[0] = 0x15;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));
//printf("mplayer: set SVSD reset\n");
#ifndef NEW_SVSD_CMD
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
#else
    int last_svsd_idx = svsd_state->recv_cmd_id;

    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id += 1;
    svsd_cmd_idx++;

    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    while (last_svsd_idx == svsd_state->recv_cmd_id)
    {
        usleep(500);
//printf("MPlayer: [24]=%d\n", svsd_state->recv_cmd_id);
    }

#endif
    close(svsd_ipc_client_sockfd);
    svsd_ipc_client_sockfd = -1;

//        result = recv(svsd_ipc_client_sockfd, recv_buf, 4, 0);
//printf("mplayer: got svsd back, ret=%d\n", recv_buf[0]);
//return recv_buf[0];
}


int svsd_ipc_callback(vdec_dec_t *pvdec)
{
//    int     svsd_ipc_client_sockfd;
//    struct	sockaddr_un serv_addr;
    int     result;
    int send_buf_len = 8+sizeof(vdec_dec_t);
    char send_buf[send_buf_len];

retry:
#ifdef NEW_SVSD_CMD
//int last_svsd_idx = svsd_state->recv_cmd_id;
    svsd_cmd_idx = svsd_state->send_cmd_id;
#endif
//    char recv_buf[4];

//printf("svsd_ipc_callback()\n");
    memcpy(&send_buf[8], pvdec, sizeof(vdec_dec_t));
    send_buf[0] = 0x12;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));
#ifndef NEW_SVSD_CMD
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    fsync(svsd_ipc_client_sockfd);

    if(result == -1)
    {
        if (!quiet)
        {
            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
            //printf("svsd_ipc_callback() send result=%d svsd_ipc_client_sockfd=%d\n", result, svsd_ipc_client_sockfd);
        }
//Robert 20110423 svsd reconnection
		if (svsd_ipc_client_sockfd != -1)
		{
			extern int ss_trak_pos;
			printf("%s - last svsd_ipc_client_sockfd = %d\n", __FUNCTION__, svsd_ipc_client_sockfd);
			close(svsd_ipc_client_sockfd);
			svsd_ipc_client_sockfd = -1;
			svsd_ipc_init_socket();
			reinit_video_chain();
			rel_seek_secs += 0.1;
			ss_trak_pos = 1;
			return 0;
//	goto retry;
		}
    }
//printf("svsd_ipc_callback() send result=%d svsd_ipc_client_sockfd=%d\n", result, svsd_ipc_client_sockfd);

#else
    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id += 1;
    svsd_cmd_idx++;

    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

#endif
    svsd_state->send_count += 1;
    //printf("send %d recv %d\n", svsd_state->send_count, svsd_state->recv_count);
    /*
    	while (last_svsd_idx == svsd_state->recv_cmd_id)
    	{
    		usleep(500);
    //printf("MPlayer: [24]=%d\n", svsd_state->recv_cmd_id);
    	}
    */

//        result = recv(svsd_ipc_client_sockfd, recv_buf, 4, 0);
//printf("mplayer: got svsd back, ret=%d\n", recv_buf[0]);
//return recv_buf[0];
    return 0;
}

#ifdef _SKY_VDEC_V2
/*sky_video_init()*/
int svsd_ipc_init_callback(vdec_init_t *pvdec)
{
    printf("mplayer: set SVSD init_callback\n");
    int     result=0;
    int init_timeout_cnts = 0;
    int send_buf_len = 8+sizeof(vdec_init_t);
    char send_buf[send_buf_len];
    int recv_buf_len = sizeof(int)+sizeof(vdec_alloc_t);
    char recv_buf[recv_buf_len];

    memcpy(&send_buf[8], pvdec, sizeof(vdec_init_t));

    send_buf[0] = 0x16;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));

#ifndef NEW_SVSD_CMD

    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    fsync(svsd_ipc_client_sockfd);


    if(result == -1)
    {
        if (!quiet)
            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
    }
#else
    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id += 1;
    svsd_cmd_idx++;


    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

#endif


    while (1)
    {
        tv_svsd.tv_usec = 10000;
        tv_svsd.tv_sec = 0;
        FD_ZERO(&FD_rcv);
        FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
        result = select(svsd_ipc_client_sockfd+1, &FD_rcv, NULL, NULL, &tv_svsd);
        if (result > 0)
            break;
        else if (result == 0)
        {
            printf("mplayer: init_callback timeout...\n");
            usleep(1000);
//Robert 20101005 add timeout, fix later
            if (init_timeout_cnts++ > 20)
            {
                exit(1);
            }
        } else /*-1*/
        {
            fprintf(stderr, "select error [%d]-[%s]in [%s][%d]\n", errno, strerror(errno), __func__, __LINE__);
            if (errno != EINTR)/* Avoid received SIGIO cause svsd hang issue */
                exit(1);

        }
    }


    result = recv(svsd_ipc_client_sockfd, recv_buf,recv_buf_len, MSG_DONTWAIT);
    if(result == -1)
    {
        if (!quiet)
            printf("%d - %s - Cannot receive: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
    }
    memcpy(&result, &recv_buf[0],sizeof(int));
    memcpy(&(pvdec->streamBuf), &recv_buf[0+4], sizeof(vdec_alloc_t));
    printf("mplayer: pvdec->streamBuf.virtAddr:%p,pvdec->streamBuf.phyAddr:0x%08x\n",pvdec->streamBuf.virtAddr,pvdec->streamBuf.phyAddr);

#ifdef HAS_DEC_CAPABILITY
    struct dec_cap_t *deccap = &(svsd_state->dec_cap);
    printf("%s: dolby:%d dts:%d divx:%d rmvb:%d drm:%d\n", __func__, deccap->dolby, deccap->dts, deccap->divx, deccap->rmvb, deccap->drm);
#endif

    return result;
}

/*sky_video_uninit()*/
int svsd_ipc_uninit_callback(unsigned int standard)
{
//printf("mplayer: set SVSD uninit_callback\n");
    int     result, timeout_cnts = 0;

    int send_buf_len = 8+sizeof(unsigned int);
    char send_buf[send_buf_len];
    int recv_buf_len = sizeof(int);
    char recv_buf[recv_buf_len];

    /* If novideo, don't init svsd */
    if (video_id == -2 )
        return 0;

    memcpy(&send_buf[8], &standard, sizeof(unsigned int));
    send_buf[0] = 0x17;
    memcpy(&send_buf[4], &send_buf_len, sizeof(int));

#ifndef NEW_SVSD_CMD
    result = send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
    fsync(svsd_ipc_client_sockfd);


    if(result == -1)
    {
        if (!quiet)
            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
    }
#else
    memcpy(&svsd_shm_array[128+1024*(svsd_cmd_idx%8)], &send_buf[0], send_buf_len);
    svsd_state->send_cmd_id += 1;
    svsd_cmd_idx++;


    result = send(svsd_ipc_client_sockfd, send_buf, 16, MSG_DONTWAIT);
    if(result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

#endif


    while (1)
    {
        tv_svsd.tv_usec = 30000;
        tv_svsd.tv_sec = 0;
        FD_ZERO(&FD_rcv);
        FD_SET(svsd_ipc_client_sockfd, &FD_rcv);
        result = select(svsd_ipc_client_sockfd+1, &FD_rcv, NULL, NULL, &tv_svsd);
        if (result > 0)
            break;
        else if (result == 0)
        {
            printf("mplayer: uninit_callback timeout...\n");
#if 0	/* We only send once uninit code to svsd */
            memcpy(&send_buf[8], &standard, sizeof(unsigned int));
            send_buf[0] = 0x17;
            memcpy(&send_buf[4], &send_buf_len, sizeof(int));
            send(svsd_ipc_client_sockfd, send_buf, send_buf_len, MSG_DONTWAIT);
            fsync(svsd_ipc_client_sockfd);
#endif
            if (timeout_cnts++ > 5)
                break;
        } else /*-1*/
        {

            fprintf(stderr, "select error [%d]-[%s]in [%s][%d]\n", errno, strerror(errno), __func__, __LINE__);
            if (errno != EINTR) /* Avoid received SIGIO cause svsd hang issue */
//			exit(1);
                break;
        }
    }


    result = recv(svsd_ipc_client_sockfd, recv_buf,recv_buf_len, MSG_DONTWAIT);
//	printf("mplayer: set SVSD uninit_callback recv\n");
    if(result == -1)
    {
        if (!quiet)
            printf("%d - %s - Cannot receive: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
    }
    memcpy(&result, &recv_buf[0],recv_buf_len);
    return result;

}
#endif


/*#endif*/
//=============================

int sub_source_by_global_pos(MPContext * mpctx, int pos)
{
    int source = -1;
    int top = -1;
    int i;
    for (i = 0; i < SUB_SOURCES; i++) {
        int j = mpctx->global_sub_indices[i];
        if ((j >= 0) && (j > top) && (pos >= j)) {
            source = i;
            top = j;
        }
    }
    return source;
}

//Fuchun 2010.08.10 reset global variable
void reset_all_global_variable(void)
{
    same_pts_cnt = 0;
    Can_FF_FB = 1;
    rel_audio_id = -5;
    is_mjpeg = 0;
    mpeg_fast = 0;
    read_nextframe = 1;
    dvd_fast = 0;
    last_rewind_pts = 0;
    rewind_mult = 1;
    wait_video_or_audio_sync = AVSYNC_NORMAL;
    seek_sync_flag = 0;
    FR_to_end = 0;
    decode_num_cnt = 0;
    vo_need_osd = 0;
    sub_visible = 1;
    last_sub_pts = -1.0;
    last_spu_pts = 0;
    correct_sub_pts = 0.0;
    rel_global_sub_pos = -5;
    sub_change_success = 0;
    audio_change_success = 0;
    num_reorder_frames = 0;
    queue_frames = 0;
    sub_temp_utf8 = 0;
    top_field_frame = 1;
    audio_start_pts = 0.0;
    get_audio_pts_first_flag = 1;
    not_supported_profile_level = 0;
    AAC_MAIN_PROFILE = 0;
#ifdef DVB_SUBTITLES
    pgs_subtitle_flag = 0;
#endif
    set_pause_lock = 0;
    uvcnt = 0;
    first_set_sub_pts= 1;
    recalcul_apts = 1;
    // reset get_buffering_on may stop ipc_callback_buffering thread, comment it
    //get_buffering_on = IPC_THREAD_IDLE;
    get_curpos_on = IPC_THREAD_IDLE;
    avsync_timeout_cnt = 0;
    read_frame_time = 0;
    aserver_crash = 0;
    last_audio_id = -1;
    not_support_audio_id = -2;
    is_skynet = 0;
    frame_cnt = 0;
    start_play=PLAYER_STATUS_IDLE;
    pre_isprogressive_flag = -1;
    audio_not_support = 0;
    AAC_MAIN_PROFILE = 0;
#ifdef BD_CLIP_BASE_PTS
    bd_now_base_pts = 0.0;
#endif
    memset(video_names,0,64);
    correct_last_dvdpts = 0.0;
    correct_frame_cnt = 0;
    my_current_pts = 0; //Polun 2011-07-26 for audio playback press next key to next file time bar no updata.
    hit_eof_cunt = 0; //Polun 2011-08-02 + mantis 5136
    //Polun 2011-08-08 ++s mantis 5678
    mpctx->num_buffered_frames = 0;
    mpctx->delay = 0.0;
    mpctx->time_frame = 0.0;
    //Polun 2011-08-08 ++e mantis 5678
    mkv_indexnull_flag = 0;
    set_disp_off = 0;
    first_frame_pts = 0.0f;
#ifdef DTS_CERTIFICATION	//Barry 2011-11-02
    DTS_in_WAV = 0;
#endif
   Dvdnav_cmd_exit = 0;//Polun 2012-02-03 fixed mantis6790 playback to eof signal 11 issue  
}

//Skyviia_Vincent11192010 reset some options after second times loadfile in slave mode.
void reset_option_variable(void)
{
    seek_to_sec = 0;
}

/* This preprocessor directive is a hack to generate a mplayer-nomain.o object
 * file for some tools to link against. */
#ifndef DISABLE_MAIN

int main(int argc,char* argv[]) {


    char * mem_ptr;
    int reset_option=0; //Skyviia_Vincent11192010
// movie info:

    /* Flag indicating whether MPlayer should exit without playing anything. */
    int opt_exit = 0;

//float a_frame=0;    // Audio

    int i;

    int gui_no_filename=0;

    char callback_str[42];
	ANNOUNCE_VALUE(check_time);
//int width, height;
#ifdef USE_INSTRUMENT_FUNCTIONS
    memset(g_mplayer_func_parent_list, 0, sizeof(g_mplayer_func_parent_list));
    memset(g_mplayer_func_child_list, 0, sizeof(g_mplayer_func_child_list));
#endif
#ifdef MEMTRACE_DEBUG
    setenv("MALLOC_TRACE", "./memleak.log", 1);
    mtrace();
#endif
#ifdef COREDUMP_DEBUG //IMPORTANT: Debug only! Do NOT enable this code to server!!!
    /* use /proc/sys/kernel/core_pattern to assign core dump file location */
    printf("Core dump feature is enabled! Don't forget to disable this feature before commit!\n");
    struct rlimit corelmt = { RLIM_INFINITY, RLIM_INFINITY };
    setrlimit(RLIMIT_CORE, &corelmt);
#endif
    printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));

    InitTimer();
    srand(GetTimerMS());

    mp_msg_init();

    // Create the config context and register the options
    mconfig = m_config_new();
    m_config_register_options(mconfig,mplayer_opts);
    mp_input_register_options(mconfig);

    // Preparse the command line
    m_config_preparse_command_line(mconfig,argc,argv);

#if (defined(__MINGW32__) || defined(__CYGWIN__)) && defined(CONFIG_WIN32DLL)
    set_path_env();
#endif

#ifdef CONFIG_TV
    stream_tv_defaults.immediate = 1;
#endif

    if (argc > 1 && argv[1] &&
            (!strcmp(argv[1], "-gui") || !strcmp(argv[1], "-nogui"))) {
        use_gui = !strcmp(argv[1], "-gui");
    } else if ( argv[0] )
    {
        char *base = strrchr(argv[0], '/');
        if (!base)
            base = strrchr(argv[0], '\\');
        if (!base)
            base = argv[0];
        if(strstr(base, "gmplayer"))
            use_gui=1;
    }

    parse_cfgfiles(mconfig);

#ifdef CONFIG_GUI
    if ( use_gui ) cfg_read();
#endif

    mpctx->playtree = m_config_parse_mp_command_line(mconfig, argc, argv);
    if(mpctx->playtree == NULL)
        opt_exit = 1;
    else {
        mpctx->playtree = play_tree_cleanup(mpctx->playtree);
        if(mpctx->playtree) {
            mpctx->playtree_iter = play_tree_iter_new(mpctx->playtree,mconfig);
            if(mpctx->playtree_iter) {
                if(play_tree_iter_step(mpctx->playtree_iter,0,0) != PLAY_TREE_ITER_ENTRY) {
                    play_tree_iter_free(mpctx->playtree_iter);
                    mpctx->playtree_iter = NULL;
                }
                filename = play_tree_iter_get_file(mpctx->playtree_iter,1);
            }
        }
    }

    print_version("MPlayer");

//+SkyMedi_Vincent03232010
    if(ipcdomain)
    {
        printf("== Using Domain socket IPC option ==\n");
        pthread_t thread1;

        pthread_create( &thread1, NULL, (void*)ipc_handler, NULL);
    }
//SkyMedi_Vincent03232010+

#if defined(__MINGW32__) || defined(__CYGWIN__)
#ifdef CONFIG_GUI
    void *runningmplayer = FindWindow("MPlayer GUI for Windows", "MPlayer for Windows");
    if(runningmplayer && filename && use_gui) {
        COPYDATASTRUCT csData;
        char file[MAX_PATH];
        char *filepart = filename;
        if(GetFullPathName(filename, MAX_PATH, file, &filepart)) {
            csData.dwData = 0;
            csData.cbData = strlen(file)*2;
            csData.lpData = file;
            SendMessage(runningmplayer, WM_COPYDATA, (WPARAM)runningmplayer, (LPARAM)&csData);
        }
    }
#endif

    {
        HMODULE kernel32 = GetModuleHandle("Kernel32.dll");
        BOOL WINAPI (*setDEP)(DWORD) = NULL;
        if (kernel32)
            setDEP = GetProcAddress(kernel32, "SetProcessDEPPolicy");
        if (setDEP) setDEP(3);
    }
    // stop Windows from showing all kinds of annoying error dialogs
    SetErrorMode(0x8003);
    // request 1ms timer resolution
    timeBeginPeriod(1);
#endif

#ifdef CONFIG_PRIORITY
    set_priority();
#endif

#ifndef CONFIG_GUI
    if(use_gui) {
        mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_NoGui);
        use_gui=0;
    }
#else
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
    if(use_gui && !vo_init()) {
        mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_GuiNeedsX);
        use_gui=0;
    }
#endif
    if (use_gui && mpctx->playtree_iter) {
        char cwd[PATH_MAX+2];
        // Free Playtree and Playtree-Iter as it's not used by the GUI.
        play_tree_iter_free(mpctx->playtree_iter);
        mpctx->playtree_iter=NULL;

        if (getcwd(cwd, PATH_MAX) != (char *)NULL)
        {
            strcat(cwd, "/");
            // Prefix relative paths with current working directory
            play_tree_add_bpf(mpctx->playtree, cwd);
        }
        // Import initital playtree into GUI.
        import_initial_playtree_into_gui(mpctx->playtree, mconfig, enqueue);
    }
#endif /* CONFIG_GUI */

    if(video_driver_list && strcmp(video_driver_list[0],"help")==0) {
        list_video_out();
        opt_exit = 1;
    }

    if(audio_driver_list && strcmp(audio_driver_list[0],"help")==0) {
        list_audio_out();
        opt_exit = 1;
    }

    /* Check codecs.conf. */
    if(!codecs_file || !parse_codec_cfg(codecs_file)) {
        if(!parse_codec_cfg(mem_ptr=get_path("codecs.conf"))) {
            if(!parse_codec_cfg(MPLAYER_CONFDIR "/codecs.conf")) {
                if(!parse_codec_cfg(NULL)) {
                    exit_player_with_rc(EXIT_NONE, 0);
                }
                mp_msg(MSGT_CPLAYER,MSGL_V,MSGTR_BuiltinCodecsConf);
            }
        }
        free( mem_ptr ); // release the buffer created by get_path()
    }

#if 0
    if(video_codec_list) {
        int i;
        video_codec=video_codec_list[0];
        for(i=0; video_codec_list[i]; i++)
            mp_msg(MSGT_FIXME,MSGL_FIXME,"vc#%d: '%s'\n",i,video_codec_list[i]);
    }
#endif
    if(audio_codec_list && strcmp(audio_codec_list[0],"help")==0) {
        mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AvailableAudioCodecs);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AUDIO_CODECS\n");
        list_codecs(1);
        mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
        opt_exit = 1;
    }
    if(video_codec_list && strcmp(video_codec_list[0],"help")==0) {
        mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AvailableVideoCodecs);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_CODECS\n");
        list_codecs(0);
        mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
        opt_exit = 1;
    }
    if(video_fm_list && strcmp(video_fm_list[0],"help")==0) {
        vfm_help();
        mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
        opt_exit = 1;
    }
    if(audio_fm_list && strcmp(audio_fm_list[0],"help")==0) {
        afm_help();
        mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
        opt_exit = 1;
    }
    if(af_cfg.list && strcmp(af_cfg.list[0],"help")==0) {
        af_help();
        printf("\n");
        opt_exit = 1;
    }
#ifdef CONFIG_X11
    if(vo_fstype_list && strcmp(vo_fstype_list[0],"help")==0) {
        fstype_help();
        mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
        opt_exit = 1;
    }
#endif
    if((demuxer_name && strcmp(demuxer_name,"help")==0) ||
            (audio_demuxer_name && strcmp(audio_demuxer_name,"help")==0) ||
            (sub_demuxer_name && strcmp(sub_demuxer_name,"help")==0)) {
        demuxer_help();
        mp_msg(MSGT_CPLAYER, MSGL_INFO, "\n");
        opt_exit = 1;
    }
    if(list_properties) {
        property_print_help();
        opt_exit = 1;
    }

    if(opt_exit)
        exit_player(EXIT_NONE);

    if (player_idle_mode && use_gui) {
        mp_msg(MSGT_CPLAYER, MSGL_FATAL, MSGTR_NoIdleAndGui);
        exit_player_with_rc(EXIT_NONE, 1);
    }

    if(!filename && !player_idle_mode) {
        if(!use_gui) {
            // no file/vcd/dvd -> show HELP:
            mp_msg(MSGT_CPLAYER, MSGL_INFO, help_text);
            exit_player_with_rc(EXIT_NONE, 0);
        } else gui_no_filename=1;
    }

    /* Display what configure line was used */
    mp_msg(MSGT_CPLAYER, MSGL_V, "Configuration: " CONFIGURATION "\n");

    // Many users forget to include command line in bugreports...
    if( mp_msg_test(MSGT_CPLAYER,MSGL_V) ) {
        mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_CommandLine);
        for(i=1; i<argc; i++)mp_msg(MSGT_CPLAYER, MSGL_INFO," '%s'",argv[i]);
        mp_msg(MSGT_CPLAYER, MSGL_INFO, "\n");
    }

//------ load global data first ------

// check font
#ifdef CONFIG_FREETYPE
    init_freetype();
#endif
#ifdef CONFIG_FONTCONFIG
    if(font_fontconfig <= 0)
    {
#endif
#ifdef CONFIG_BITMAP_FONT
        if(font_name) {
            vo_font=read_font_desc(font_name,font_factor,verbose>1);
            if(!vo_font) mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CantLoadFont,
                                    filename_recode(font_name));
        } else {
            // try default:
            vo_font=read_font_desc( mem_ptr=get_path("font/font.desc"),font_factor,verbose>1);
            free(mem_ptr); // release the buffer created by get_path()
            if(!vo_font)
                vo_font=read_font_desc(MPLAYER_DATADIR "/font/font.desc",font_factor,verbose>1);
        }
        if (sub_font_name)
            sub_font = read_font_desc(sub_font_name, font_factor, verbose>1);
        else
            sub_font = vo_font;
#endif
#ifdef CONFIG_FONTCONFIG
    }
#endif

    vo_init_osd();

#ifdef CONFIG_ASS
    ass_library = ass_init();
#endif

#ifdef HAVE_RTC
    if(!nortc)
    {
        // seteuid(0); /* Can't hurt to try to get root here */
        if ((rtc_fd = open(rtc_device ? rtc_device : "/dev/rtc", O_RDONLY)) < 0)
            mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_RTCDeviceNotOpenable,
                   rtc_device ? rtc_device : "/dev/rtc", strerror(errno));
        else {
            unsigned long irqp = 1024; /* 512 seemed OK. 128 is jerky. */

            if (ioctl(rtc_fd, RTC_IRQP_SET, irqp) < 0) {
                mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_LinuxRTCInitErrorIrqpSet, irqp, strerror(errno));
                mp_msg(MSGT_CPLAYER, MSGL_HINT, MSGTR_IncreaseRTCMaxUserFreq, irqp);
                close (rtc_fd);
                rtc_fd = -1;
            } else if (ioctl(rtc_fd, RTC_PIE_ON, 0) < 0) {
                /* variable only by the root */
                mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_LinuxRTCInitErrorPieOn, strerror(errno));
                close (rtc_fd);
                rtc_fd = -1;
            } else
                mp_msg(MSGT_CPLAYER, MSGL_V, MSGTR_UsingRTCTiming, irqp);
        }
    }
#ifdef CONFIG_GUI
// breaks DGA and SVGAlib and VESA drivers:  --A'rpi
// and now ? -- Pontscho
    if(use_gui) setuid( getuid() ); // strongly test, please check this.
#endif
    if(rtc_fd<0)
#endif /* HAVE_RTC */
        mp_msg(MSGT_CPLAYER, MSGL_V, "Using %s timing\n",
               softsleep?"software":timer_name);

#ifdef HAVE_TERMCAP
    if ( !use_gui ) load_termcap(NULL); // load key-codes
#endif

// ========== Init keyboard FIFO (connection to libvo) ============

// Init input system
    current_module = "init_input";
    mp_input_init(use_gui);
    mp_input_add_key_fd(-1,0,mplayer_get_key,NULL);
    if(slave_mode)
        mp_input_add_cmd_fd(0,USE_SELECT,MP_INPUT_SLAVE_CMD_FUNC,NULL);
    else if(!noconsolecontrols)
        mp_input_add_event_fd(0, getch2);
// Set the libstream interrupt callback
    stream_set_interrupt_callback(mp_input_check_interrupt);

#ifdef CONFIG_MENU
    if(use_menu) {
        if(menu_cfg && menu_init(mpctx, menu_cfg))
            mp_msg(MSGT_CPLAYER, MSGL_V, MSGTR_MenuInitialized, menu_cfg);
        else {
            menu_cfg = get_path("menu.conf");
            if(menu_init(mpctx, menu_cfg))
                mp_msg(MSGT_CPLAYER, MSGL_V, MSGTR_MenuInitialized, menu_cfg);
            else {
                if(menu_init(mpctx, MPLAYER_CONFDIR "/menu.conf"))
                    mp_msg(MSGT_CPLAYER, MSGL_V, MSGTR_MenuInitialized, MPLAYER_CONFDIR"/menu.conf");
                else {
                    mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_MenuInitFailed);
                    use_menu = 0;
                }
            }
        }
    }
#endif

    initialized_flags|=INITIALIZED_INPUT;
    current_module = "end of init_input";

    /// Catch signals
#ifndef __MINGW32__
    signal(SIGCHLD,child_sighandler);
#endif

#ifdef CONFIG_CRASH_DEBUG
    prog_path = argv[0];
#endif
    //========= Catch terminate signals: ================
    // terminate requests:
    signal(SIGTERM,exit_sighandler); // kill
    signal(SIGHUP,exit_sighandler);  // kill -HUP  /  xterm closed

    signal(SIGINT,exit_sighandler);  // Interrupt from keyboard

    signal(SIGQUIT,exit_sighandler); // Quit from keyboard
    signal(SIGPIPE,exit_sighandler); // Some window managers cause this
#ifndef COREDUMP_DEBUG
#ifdef CONFIG_SIGHANDLER
    // fatal errors:
    signal(SIGBUS,exit_sighandler);  // bus error
    signal(SIGSEGV,exit_sighandler); // segfault
    signal(SIGILL,exit_sighandler);  // illegal instruction
    signal(SIGFPE,exit_sighandler);  // floating point exc.
    signal(SIGABRT,exit_sighandler); // abort()
#ifdef CONFIG_CRASH_DEBUG
    if (crash_debug)
        signal(SIGTRAP,exit_sighandler);
#endif
#endif
#endif //COREDUMP_DEBUG

#ifdef CONFIG_GUI
    if(use_gui) {
        guiInit();
        guiGetEvent(guiSetContext, mpctx);
        initialized_flags|=INITIALIZED_GUI;
        guiGetEvent( guiCEvent,(char *)((gui_no_filename) ? 0 : 1) );
    }
#endif

// ******************* Now, let's see the per-file stuff ********************
    /*
    play_next_file________________()
    */
#ifdef ENABLE_HWTS_WHEN_USING_SKYDROID
    if (skydroid)
    {
        hwtsdemux = 1;
        printf("@@ We auto enable hwts in skydroid flag is enabled @@\n");
    }
#endif // end of ENABLE_HWTS_WHEN_USING_SKYDROID
play_next_file:
    current_module="play_next_file";	//Barry 2010-08-09
    reset_all_global_variable();	//Fuchun 2010.08.10
    entry_while = 0;

    if(reset_option)
        reset_option_variable();
    reset_option = 1;

    // init global sub numbers
    mpctx->global_sub_size = 0;
    {
        int i;
        for (i = 0; i < SUB_SOURCES; i++) mpctx->global_sub_indices[i] = -1;
    }

    if (filename) {
        load_per_protocol_config (mconfig, filename);
        load_per_extension_config (mconfig, filename);
        load_per_file_config (mconfig, filename);
    }

    if (video_driver_list)
        load_per_output_config (mconfig, PROFILE_CFG_VO, video_driver_list[0]);
    if (audio_driver_list)
        load_per_output_config (mconfig, PROFILE_CFG_AO, audio_driver_list[0]);

// We must enable getch2 here to be able to interrupt network connection
// or cache filling
    if(!noconsolecontrols && !slave_mode) {
        if(initialized_flags&INITIALIZED_GETCH2)
            mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_Getch2InitializedTwice);
        else
            getch2_enable();  // prepare stdin for hotkeys...
        initialized_flags|=INITIALIZED_GETCH2;
        mp_msg(MSGT_CPLAYER,MSGL_DBG2,"\n[[[init getch2]]]\n");
    }

// =================== GUI idle loop (STOP state) ===========================
#ifdef CONFIG_GUI
    if ( use_gui ) {
        mpctx->file_format=DEMUXER_TYPE_UNKNOWN;
        guiGetEvent( guiSetDefaults,0 );
        while ( guiIntfStruct.Playing != 1 )
        {
            mp_cmd_t* cmd;
            usec_sleep(20000);
            guiEventHandling();
            guiGetEvent( guiReDraw,NULL );
            if ( (cmd = mp_input_get_cmd(0,0,0)) != NULL) {
                guiGetEvent(guiIEvent, (char *)cmd->id);
                mp_cmd_free(cmd);
            }
        }
        guiGetEvent( guiSetParameters,NULL );
        if ( guiIntfStruct.StreamType == STREAMTYPE_STREAM )
        {
            play_tree_t * entry = play_tree_new();
            play_tree_add_file( entry,guiIntfStruct.Filename );
            if ( mpctx->playtree ) play_tree_free_list( mpctx->playtree->child,1 );
            else mpctx->playtree=play_tree_new();
            play_tree_set_child( mpctx->playtree,entry );
            if(mpctx->playtree)
            {
                mpctx->playtree_iter = play_tree_iter_new(mpctx->playtree,mconfig);
                if(mpctx->playtree_iter)
                {
                    if(play_tree_iter_step(mpctx->playtree_iter,0,0) != PLAY_TREE_ITER_ENTRY)
                    {
                        play_tree_iter_free(mpctx->playtree_iter);
                        mpctx->playtree_iter = NULL;
                    }
                    filename = play_tree_iter_get_file(mpctx->playtree_iter,1);
                }
            }
        }
    }
#endif /* CONFIG_GUI */

    while (player_idle_mode && !filename) {
        play_tree_t * entry = NULL;
        mp_cmd_t * cmd;
        int cmd_id;
        if (mpctx->video_out && vo_config_count)
            mpctx->video_out->control(VOCTRL_PAUSE, NULL);
        while (!(cmd = mp_input_get_cmd(0,1,0)) && !domainsocket_cmd) { // wait for command
            if(force_quit)
            {
                cmd_id = MP_CMD_QUIT;
                break;
            }

            if (mpctx->video_out && vo_config_count) mpctx->video_out->check_events();
            usec_sleep(20000);
        }

        if(domainsocket_cmd)
            cmd = domainsocket_cmd;

        if(!force_quit)
            cmd_id = cmd->id;

        switch (cmd_id) {
        case MP_CMD_LOADFILE:
            // prepare a tree entry with the new filename
            entry = play_tree_new();
            play_tree_add_file(entry, cmd->args[0].v.s);
            // The entry is added to the main playtree after the switch().
//            thumbnail_mode = 0;
            break;
//+SkyViia_Vincent03232010
        case MP_CMD_GETTHUMB:
            // prepare a tree entry with the new filename
            entry = play_tree_new();
            play_tree_add_file(entry, cmd->args[0].v.s);
            // The entry is added to the main playtree after the switch().
//            thumbnail_mode = 1;
            break;
//SkyViia_Vincent03232010+
        case MP_CMD_LOADLIST:
            entry = parse_playlist_file(cmd->args[0].v.s);
            break;
        case MP_CMD_QUIT:
            slave_mode_quit = 1; //SkyViia_Vincent03232010
            if(force_quit)
                exit_player_with_rc(EXIT_QUIT,0);
            else
                exit_player_with_rc(EXIT_QUIT, (cmd->nargs > 0)? cmd->args[0].v.i : 0);
            break;
#ifdef QT_SUPPORT_DVBT //for reload dvb_conf by carlos 20100804
        case MP_CMD_DVB_LOAD_CONFFILE:
            reload_dvb_conf(cmd->args[0].v.s);
            break;
#endif // carlos add for dvb change conf_file end of QT_SUPPORT_DVBT
        case MP_CMD_GET_PROPERTY:
        case MP_CMD_SET_PROPERTY:
        case MP_CMD_STEP_PROPERTY:
            run_command(mpctx, cmd);
            break;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
        case MP_CMD_DVB_LOAD_CHANNEL:
            if (cmd->args[0].v.i > 0)
                setdvb_by_pid=  1;
            else
                setdvb_by_pid = 0;
            printf("Setting dynamic dvb pid to [%d] @@@@\n", setdvb_by_pid);
            break;
        case MP_CMD_DVB_STOP_TUNER:
            if (cmd->args[0].v.i > 0)
                stop_tuner = 1;
            else
                stop_tuner = 0;
            printf("Setting stop tuner to [%d] @@@@\n", stop_tuner);
            break;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
        }

        if(!domainsocket_cmd)
            mp_cmd_free(cmd);
        else
            domainsocket_cmd = NULL;

        if (entry) { // user entered a command that gave a valid entry
            if (mpctx->playtree) // the playtree is always a node with one child. let's clear it
                play_tree_free_list(mpctx->playtree->child, 1);
            else mpctx->playtree=play_tree_new(); // .. or make a brand new playtree

            if (!mpctx->playtree) continue; // couldn't make playtree! wait for next command

            play_tree_set_child(mpctx->playtree, entry);

            /* Make iterator start at the top the of tree. */
            mpctx->playtree_iter = play_tree_iter_new(mpctx->playtree, mconfig);
            if (!mpctx->playtree_iter) continue;

            // find the first real item in the tree
            if (play_tree_iter_step(mpctx->playtree_iter,0,0) != PLAY_TREE_ITER_ENTRY) {
                // no items!
                play_tree_iter_free(mpctx->playtree_iter);
                mpctx->playtree_iter = NULL;
                continue; // wait for next command
            }
            filename = play_tree_iter_get_file(mpctx->playtree_iter, 1);
        }
    }
//---------------------------------------------------------------------------

    if (mpctx->video_out && vo_config_count)
        mpctx->video_out->control(VOCTRL_RESUME, NULL);

    if(filename) {
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_Playing,
               filename_recode(filename));
        if(use_filename_title && vo_wintitle == NULL)
            vo_wintitle = strdup ( mp_basename2 (filename));
    }

    if (edl_filename) {
        if (edl_records) free_edl(edl_records);
        next_edl_record = edl_records = edl_parse_file();
    }
    if (edl_output_filename) {
        if (edl_fd) fclose(edl_fd);
        if ((edl_fd = fopen(edl_output_filename, "w")) == NULL)
        {
            mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_EdlCantOpenForWrite,
                   filename_recode(edl_output_filename));
        }
    }
//==================== Open VOB-Sub ============================

    current_module="vobsub";
    if (vobsub_name) {
        vo_vobsub=vobsub_open(vobsub_name,spudec_ifo,1,&vo_spudec);
        if(vo_vobsub==NULL)
            mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CantLoadSub,
                   filename_recode(vobsub_name));
    } else if (sub_auto && filename) {
        /* try to autodetect vobsub from movie filename ::atmos */
        char *buf = strdup(filename), *psub;
        char *pdot = strrchr(buf, '.');
        char *pslash = strrchr(buf, '/');
#if defined(__MINGW32__) || defined(__CYGWIN__)
        if (!pslash) pslash = strrchr(buf, '\\');
#endif
        if (pdot && (!pslash || pdot > pslash))
            *pdot = '\0';
        vo_vobsub=vobsub_open(buf,spudec_ifo,0,&vo_spudec);
        /* try from ~/.mplayer/sub */
        if(!vo_vobsub && (psub = get_path( "sub/" ))) {
            char *bname;
            int l;
            bname = strrchr(buf,'/');
#if defined(__MINGW32__) || defined(__CYGWIN__)
            if(!bname) bname = strrchr(buf,'\\');
#endif
            if(bname) bname++;
            else bname = buf;
            l = strlen(psub) + strlen(bname) + 1;
            psub = realloc(psub,l);
            strcat(psub,bname);
            vo_vobsub=vobsub_open(psub,spudec_ifo,0,&vo_spudec);
            free(psub);
        }
        free(buf);
    }
    if(vo_vobsub) {
        initialized_flags|=INITIALIZED_VOBSUB;
        vobsub_set_from_lang(vo_vobsub, dvdsub_lang);
        mp_property_do("sub_forced_only", M_PROPERTY_SET, &forced_subs_only, mpctx);

        // setup global sub numbering
        mpctx->global_sub_indices[SUB_SOURCE_VOBSUB] = mpctx->global_sub_size; // the global # of the first vobsub.
        mpctx->global_sub_size += vobsub_get_indexes_count(vo_vobsub);
    }

    // Raymond 2009/07/24
    {
        int nret = 0;
        nret = init_sky();	// Raymond 2009/04/01
        if(nret == -1)
            exit_player(EXIT_ERROR);
        initialized_flags|=INITIALIZED_SKYVDEC;//SkyViia_Vincent06172010
    }


//Robert 20110328 do shm init in svsd_ipc_init_socket
    svsd_ipc_init_socket();

//============ Open & Sync STREAM --- fork cache2 ====================

    // Charles 20110120 don't use cache on upnp streaming
    // stream_cache_size must be zero, or cache will be enabled in fixup_network_stream_cache()
    if(upnp){
#if 0
	    if(video_id == -2){ // novideo => music
		    stream_cache_size = 4096;
		    stream_cache_min_percent = 5;
	    }else
#endif
	    {
		char *buf = strdup(filename);
		char *pdot = strrchr(buf, '.');
		if(stream_cache_size < 4096 &&
		   (!strcmp(pdot, ".mp4") || !strcmp(pdot, ".mov") || !strcmp(pdot, ".3gp") || !strcmp(pdot, ".m4a") || !strcmp(pdot, ".3g2") || !strcmp(pdot, ".flv")))
			stream_cache_size = 4096;
		else
			stream_cache_size = 0;
	    }
    }

    mpctx->stream=NULL;
    mpctx->demuxer=NULL;
    if (mpctx->d_audio) {
        //free_demuxer_stream(mpctx->d_audio);
        mpctx->d_audio=NULL;
    }
    if (mpctx->d_video) {
        //free_demuxer_stream(d_video);
        mpctx->d_video=NULL;
    }
    mpctx->sh_audio=NULL;
    mpctx->sh_video=NULL;

    current_module="open_stream";
    mpctx->stream=open_stream(filename,0,&mpctx->file_format);
    if(!mpctx->stream) { // error...
        mpctx->eof = libmpdemux_was_interrupted(PT_NEXT_ENTRY);

        if(ipcdomain)
            ipc_callback_error();

        goto goto_next_file;
    }
    initialized_flags|=INITIALIZED_STREAM;

	if(is_dvdnav && audio_id != -1)
	{
		int ii;
		for(ii = 0; ii < 8; ii++)
		{
			if(dvdnavstate.aid[ii] == audio_id)
			{
				dvdnavstate.cur_audio_num = ii;
				break;
			}
		}
		if(ii == 8)
			dvdnavstate.cur_audio_num = -1;
	}

#ifdef CONFIG_GUI
    if ( use_gui ) guiGetEvent( guiSetStream,(char *)mpctx->stream );
#endif

    if(mpctx->file_format == DEMUXER_TYPE_PLAYLIST) {
        play_tree_t* entry;
        // Handle playlist
        current_module="handle_playlist";
        mp_msg(MSGT_CPLAYER,MSGL_V,"Parsing playlist %s...\n",
               filename_recode(filename));
        entry = parse_playtree(mpctx->stream,0);
        mpctx->eof=playtree_add_playlist(entry);
        goto goto_next_file;
    }
    mpctx->stream->start_pos+=seek_to_byte;

    if(stream_dump_type==5) {
        unsigned char buf[4096];
        int len;
        FILE *f;
        current_module="dumpstream";
        stream_reset(mpctx->stream);
        stream_seek(mpctx->stream,mpctx->stream->start_pos);
        f=fopen(stream_dump_name,"wb");
        if(!f) {
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_CantOpenDumpfile);
            exit_player(EXIT_ERROR);
        }
        if (dvd_chapter > 1) {
            int chapter = dvd_chapter - 1;
            stream_control(mpctx->stream, STREAM_CTRL_SEEK_TO_CHAPTER, &chapter);
        }
        while(!mpctx->stream->eof && !async_quit_request) {
            len=stream_read(mpctx->stream,buf,4096);
            if(len>0) {
                if(fwrite(buf,len,1,f) != 1) {
                    mp_msg(MSGT_MENCODER,MSGL_FATAL,MSGTR_ErrorWritingFile,stream_dump_name);
                    exit_player(EXIT_ERROR);
                }
            }
            if(dvd_last_chapter > 0) {
                int chapter = -1;
                if (stream_control(mpctx->stream, STREAM_CTRL_GET_CURRENT_CHAPTER,
                                   &chapter) == STREAM_OK && chapter + 1 > dvd_last_chapter)
                    break;
            }
        }
        if(fclose(f)) {
            mp_msg(MSGT_MENCODER,MSGL_FATAL,MSGTR_ErrorWritingFile,stream_dump_name);
            exit_player(EXIT_ERROR);
        }
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_CoreDumped);
        exit_player_with_rc(EXIT_EOF, 0);
    }

#ifdef CONFIG_DVDREAD
    if(mpctx->stream->type==STREAMTYPE_DVD) {
        current_module="dvd lang->id";
        if(audio_id==-1) audio_id=dvd_aid_from_lang(mpctx->stream,audio_lang);
        if(dvdsub_lang && dvdsub_id==-1) dvdsub_id=dvd_sid_from_lang(mpctx->stream,dvdsub_lang);
        // setup global sub numbering
        mpctx->global_sub_indices[SUB_SOURCE_DEMUX] = mpctx->global_sub_size; // the global # of the first demux-specific sub.
        mpctx->global_sub_size += dvd_number_of_subs(mpctx->stream);
        current_module="end of dvd lang->id";
    }
#endif

    dvdnavstate.dvdnav_stream_state = 0;
    dvdnavstate.dvdnav_stream = 0;

#ifdef CONFIG_DVDNAV
    if(mpctx->stream->type==STREAMTYPE_DVDNAV) {
        current_module="dvdnav lang->id";
        if(audio_id==-1) audio_id=mp_dvdnav_aid_from_lang(mpctx->stream,audio_lang);
        dvdsub_lang_id = -3;
        if(dvdsub_lang && dvdsub_id==-1)
            dvdsub_lang_id=dvdsub_id=mp_dvdnav_sid_from_lang(mpctx->stream,dvdsub_lang);
        // setup global sub numbering
        mpctx->global_sub_indices[SUB_SOURCE_DEMUX] = mpctx->global_sub_size; // the global # of the first demux-specific sub.
        mpctx->global_sub_size += mp_dvdnav_number_of_subs(mpctx->stream);
        current_module="end of dvdnav lang->id";

        vo_need_dvdnav_menu = 1;

        //init DvdnavState data struct
        dvdnavstate.dvdnav_stream = 1;
        dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_IDLE;
        dvdnavstate.dvdnav_state_change = 1;
        dvdnavstate.dvdnav_cell_change = 0;
    }
#endif

// CACHE2: initial prefill: 20%  later: 5%  (should be set by -cacheopts)
goto_enable_cache:
    if(stream_cache_size>0) {
        int res;
        current_module="enable_cache";
        res = stream_enable_cache(mpctx->stream,stream_cache_size*1024,
                                  stream_cache_size*1024*(stream_cache_min_percent / 100.0),
                                  stream_cache_size*1024*(stream_cache_seek_min_percent / 100.0));
        if(res == 0)
            if((mpctx->eof = libmpdemux_was_interrupted(PT_NEXT_ENTRY))) goto goto_next_file;
    }

#ifdef READ_PERF_TEST
    if(spd) {
        char buf[2048];
        unsigned int cnt = 0;
        unsigned int len;
        mp_cmd_t *cmd;
        stream_t *s = mpctx->stream;
        s->buf_pos = s->buf_len; //make buffer empty
        dio_enable(mpctx->stream);
        while(len = stream_fill_buffer(s)) {
            cnt++;
            //printf("len %d\n", len);
            if(cpy) {
                int i;
                for(i=0; i<s->buf_len; i+=2048) {
                    memcpy(buf, s->buffer + i, 2048);
                    s->buf_pos += 2048;
                }
            } else {
                s->buf_pos = s->buf_len;
            }

            if(cnt % 100 == 0 &&
                    (cmd = mp_input_get_cmd(0, 0, 0)))
                run_command(mpctx, cmd);
        }
    }
#endif

//============ Open DEMUXERS --- DETECT file type =======================
    current_module="demux_open";
//Robert 20100604
        h264_frame_mbs_only = 1;

#ifdef HAS_DEC_CAPABILITY    //Barry 2011-03-24
    if (video_id == -1)
    {
    	if (svsd_shm_id == 0)
		svsd_shm_id = shmget((key_t) 2688, 1024+1024*8+128, 0666);
	if (svsd_shm_array == NULL)
		svsd_shm_array = (unsigned char *)shmat(svsd_shm_id, (void *)0, 0);
	svsd_state = (vdec_shm_t *)svsd_shm_array;
    }

    if (!svsd_state->dec_cap.dts && !sky_hwac3)
		ts_easy_audio =1;	//for DEMUXER_TYPE_MPEG_TS
#endif
#ifdef SUPPORT_DIVX_DRM
   DRM_Open = DRMInit(filename);       
   if(DRM_Open ==  NOT_INCLUDE_DIVX_DRM_DATA)
   {
       mplayer_debug("### In [%s][%d] this file is not include DRM. It's normal file ###\n", __func__, __LINE__);
   }
   else if (DRM_Open == DIVX_NEED_IPC_CALLBACK_ERROR)
   {
       if(ipcdomain && !thumbnail_mode)
       {
           char callback_str[42];
           strcpy(callback_str,"VIDEOFILE : DIVX FAIL\0");
           ipc_callback(callback_str);
       }
       mplayer_debug("### In [%s][%d] this file contain DRM data, but initial failed###\n", __func__, __LINE__);
       goto goto_next_file;
   }
   else
   {
       mplayer_debug("### [%s][%d] Drm file [%d], check###\n", __func__, __LINE__, DRM_Open);
	   skyfb_trick_play_workaround(1);
	   slow_down_speed = 1;;
   }
   /* For inconsistent motion B01 ~ B05 */
   /* TODO, need check the file is made from Divx Group */
   if (check_HD_output() && slow_down_speed == 0)
   {
       skyfb_trick_play_workaround(1);
       slow_down_speed = 1;;
   }
#endif /* end of SUPPORT_DIVX_DRM */   
    mpctx->demuxer=demux_open(mpctx->stream,mpctx->file_format,audio_id,video_id,dvdsub_id,filename);

    if(mpctx->demuxer)  //Polun 2011-12-2 fixed amr file smaller than 32kB size can't playback
        first_frame_pts = mpctx->demuxer->video->pts ;//Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv -ss can't AVSYNC move from video_updata to here.
    else{
        if(ipcdomain)
            ipc_callback_error();
        goto goto_next_file;
    }
    
   //Polun 2011-08-04 remove let AAC_MAIN_PROFILE is no sound only playback video
   /*
    if(!mpctx->demuxer || AAC_MAIN_PROFILE)
    {
        if(ipcdomain) //using domain socket option
            ipc_callback_error();

        goto goto_next_file;
    }
    */
#if 1	//Barry 2011-03-22
    if (mpctx->demuxer && mpctx->demuxer->audio && !mpctx->demuxer->video && ipcdomain && start_play == PLAYER_STATUS_STOP && !no_osd)
    {
	free_demuxer(mpctx->demuxer);
	mpctx->demuxer = NULL;
	goto goto_next_file;
    }
#endif
#if 1 //20120105 charleslin
    //printf("%s:%d divx:%d rmvb:%d\n", __func__, __LINE__, svsd_state->dec_cap.divx, svsd_state->dec_cap.rmvb);
    //printf("%s:%d demuxer:%p video:%p sh_v:%p audio:%p sh_a:%p\n", __func__, __LINE__, mpctx->demuxer, mpctx->demuxer->video, mpctx->demuxer->video->sh, mpctx->demuxer->audio, mpctx->demuxer->audio->sh);
    if(svsd_state->dec_cap.divx == 0 &&
       mpctx->demuxer && mpctx->demuxer->video && mpctx->demuxer->video->sh &&
       (((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('D','I','V','3') ||
	((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('d','i','v','3') ||
	((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('M','P','4','3') ||
	((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('m','p','4','3')))
    {
	    not_supported_profile_level = 1;
    }
    else if(svsd_state->dec_cap.rmvb == 0 &&
       mpctx->demuxer && mpctx->demuxer->video && mpctx->demuxer->video->sh &&
       (((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('R','V','3','0') ||
	((sh_video_t *)mpctx->demuxer->video->sh)->format == mmioFOURCC('R','V','4','0')))
    {
	    not_supported_profile_level = 1;
    }
    else if(svsd_state->dec_cap.rmvb == 0 &&
       mpctx->demuxer && mpctx->demuxer->audio && mpctx->demuxer->audio->sh &&
       ((sh_audio_t *)mpctx->demuxer->audio->sh)->format == mmioFOURCC('c','o','o','k'))
    {
	    not_supported_profile_level = 2;
    }
#endif

    if(mpctx->stream->type == STREAMTYPE_FILE)
    {
#ifdef STREAM_DIO_AUTO
        if (mpctx->demuxer->audio && mpctx->demuxer->audio->sh)
            if(dio == 0 &&
                    mpctx->demuxer->video && mpctx->demuxer->video->sh &&
                    ( !((mpctx->demuxer->type == DEMUXER_TYPE_LAVF || mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED) &&
                        ((sh_video_t *)mpctx->demuxer->video->sh)->bih->biCompression == mmioFOURCC('H', '2', '6', '4') &&
                        ((sh_audio_t *)mpctx->demuxer->audio->sh)->format == 0x4134504D) )
              ) {
                dio_enable(mpctx->stream);
            }

#ifdef STREAM_AIO_AUTO
        if(dio &&
                mpctx->demuxer->type != DEMUXER_TYPE_MOV &&
                mpctx->demuxer->type != DEMUXER_TYPE_AVI_NI &&
#ifdef HW_TS_DEMUX
                ( !(mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)) &&
#endif // end of HW_TS_DEMUX
                mpctx->demuxer->type != DEMUXER_TYPE_AVI_NINI) {
            if(aio == 0) {
                if(stream_cache_size > 0)
                    cache_aio_enable(mpctx->stream->cache_data);
                else
                    aio_enable(mpctx->stream);
            }
        }
#endif
#endif
    }
#ifdef SUPPORT_DIVX_DRM
   if (mpctx->demuxer && mpctx->demuxer->divx_drm_notsupport)
   {
        if(ipcdomain && !thumbnail_mode) //using domain socket option
       {
           char callback_str[42];
           strcpy(callback_str,"VIDEOFILE : DIVX DRM FAIL\0");
           ipc_callback(callback_str);
            ipc_callback_error();
       }
       free_demuxer(mpctx->demuxer);
       mpctx->demuxer = NULL;
        goto goto_next_file;
   }
#endif /* end of SUPPORT_DIVX_DRM */

    /* Hamer, 20100802  start */
    /* If DxClient is initialized but drmFileOpened is false, stop playback and then go to next */
    /* to avoid mplayer can't understand the stream data because encrypted data */
#ifdef CONFIG_DRM_ENABLE
    if ( mpctx->demuxer->clientInitialized == DX_TRUE && mpctx->demuxer->drmFileOpened == DX_FALSE
            && mpctx->demuxer->fileIsDrmProtected == DX_TRUE)
    {
        free_demuxer(mpctx->demuxer);
        mpctx->demuxer = NULL;
        if(ipcdomain) //using domain socket option
            ipc_callback_stop();
        goto goto_next_file;
    }
#endif
    /* Hamer, 20100802  end */

// HACK to get MOV Reference Files working

    if (mpctx->demuxer && mpctx->demuxer->type==DEMUXER_TYPE_PLAYLIST)
    {
        unsigned char* playlist_entry;
        play_tree_t *list = NULL, *entry = NULL;

        current_module="handle_demux_playlist";
        while (ds_get_packet(mpctx->demuxer->video,&playlist_entry)>0)
        {
            char *temp, *bname;

            mp_msg(MSGT_CPLAYER,MSGL_V,"Adding file %s to element entry.\n",
                   filename_recode(playlist_entry));

            bname=mp_basename(playlist_entry);
            if ((bname && strlen(bname)>10) && !strncmp(bname,"qt",2) && !strncmp(bname+3,"gateQT",6))
                continue;

            if (!strcmp(playlist_entry,filename)) // ignoring self-reference
                continue;

            entry = play_tree_new();

            if (filename && !strcmp(mp_basename(playlist_entry),playlist_entry)) // add reference path of current file
            {
                temp=malloc((strlen(filename)-strlen(mp_basename(filename))+strlen(playlist_entry)+1));
                if (temp)
                {
                    strncpy(temp, filename, strlen(filename)-strlen(mp_basename(filename)));
                    temp[strlen(filename)-strlen(mp_basename(filename))]='\0';
                    strcat(temp, playlist_entry);
                    if (!strcmp(temp, filename)) {
                        free(temp);
                        continue;
                    }
                    play_tree_add_file(entry,temp);
                    mp_msg(MSGT_CPLAYER,MSGL_V,"Resolving reference to %s.\n",temp);
                    free(temp);
                }
            }
            else
                play_tree_add_file(entry,playlist_entry);

            if(!list)
                list = entry;
            else
                play_tree_append_entry(list,entry);
        }
        free_demuxer(mpctx->demuxer);
        mpctx->demuxer = NULL;

        if (list)
        {
            entry = play_tree_new();
            play_tree_set_child(entry,list);
            mpctx->eof=playtree_add_playlist(entry);
            goto goto_next_file;
        }
    }

    if(dvd_chapter>1) {
        float pts;
        if (demuxer_seek_chapter(mpctx->demuxer, dvd_chapter-1, 1, &pts, NULL, NULL) >= 0 && pts > -1.0)
            seek(mpctx, pts, SEEK_ABSOLUTE);
    }

    if(bluray_chapter) {
        float pts;
        if (demuxer_seek_chapter(mpctx->demuxer, bluray_chapter, 1, &pts, NULL, NULL) >= 0 && pts > -1.0)
            seek(mpctx, pts, SEEK_ABSOLUTE);
    }

    initialized_flags|=INITIALIZED_DEMUXER;
//Robert 20110324 reset dp_last = NULL
    dp_last = NULL;

    if (mpctx->stream->type != STREAMTYPE_DVD && mpctx->stream->type != STREAMTYPE_DVDNAV) {
        int i;
        int maxid = -1;
        int pgs_num = 0;
        // setup global sub numbering
        mpctx->global_sub_indices[SUB_SOURCE_DEMUX] = mpctx->global_sub_size; // the global # of the first demux-specific sub.
        for (i = 0; i < MAX_S_STREAMS; i++)
        {
            if (mpctx->demuxer->s_streams[i] && ((((sh_sub_t *)mpctx->demuxer->s_streams[i])->type == 'p') ||
                                                 (((sh_sub_t *)mpctx->demuxer->s_streams[i])->type == 'd')) )
                pgs_num++;
            else if (mpctx->demuxer->s_streams[i])
                maxid = FFMAX(maxid, ((sh_sub_t *)mpctx->demuxer->s_streams[i])->sid);
        }
        if (maxid != -1)
            mpctx->global_sub_size += maxid + 1;
        if (pgs_num != 0)
            mpctx->global_sub_size += pgs_num;
    }
// Make dvdsub_id always selectable if set.
    if (mpctx->global_sub_size <= mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + dvdsub_id)
        mpctx->global_sub_size = mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + dvdsub_id + 1;

#ifdef CONFIG_ASS
    if (ass_enabled && ass_library) {
        for (i = 0; i < mpctx->demuxer->num_attachments; ++i) {
            demux_attachment_t* att = mpctx->demuxer->attachments + i;
            if (extract_embedded_fonts &&
                    att->name && att->type && att->data && att->data_size &&
                    (strcmp(att->type, "application/x-truetype-font") == 0 ||
                     strcmp(att->type, "application/x-font") == 0))
                ass_add_font(ass_library, att->name, att->data, att->data_size);
        }
    }
#endif

    current_module="demux_open2";

//file_format=demuxer->file_format;

    mpctx->d_audio=mpctx->demuxer->audio;
    mpctx->d_video=mpctx->demuxer->video;
    mpctx->d_sub=mpctx->demuxer->sub;
    if (ts_prog) {
        int tmp = ts_prog;
        mp_property_do("switch_program", M_PROPERTY_SET, &tmp, mpctx);
    }
// select audio stream
    select_audio(mpctx->demuxer, audio_id, audio_lang);

// DUMP STREAMS:
    if((stream_dump_type)&&(stream_dump_type<4)) {
        FILE *f;
        demux_stream_t *ds=NULL;
        current_module="dump";
        // select stream to dump
        switch(stream_dump_type) {
        case 1:
            ds=mpctx->d_audio;
            break;
        case 2:
            ds=mpctx->d_video;
            break;
        case 3:
            ds=mpctx->d_sub;
            break;
        }
        if(!ds) {
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_DumpSelectedStreamMissing);
            exit_player(EXIT_ERROR);
        }
        // disable other streams:
        if(mpctx->d_audio && mpctx->d_audio!=ds) {
            ds_free_packs(mpctx->d_audio);
            mpctx->d_audio->id=-2;
        }
        if(mpctx->d_video && mpctx->d_video!=ds) {
            ds_free_packs(mpctx->d_video);
            mpctx->d_video->id=-2;
        }
        if(mpctx->d_sub && mpctx->d_sub!=ds) {
            ds_free_packs(mpctx->d_sub);
            mpctx->d_sub->id=-2;
        }
        // let's dump it!
        f=fopen(stream_dump_name,"wb");
        if(!f) {
            mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_CantOpenDumpfile);
            exit_player(EXIT_ERROR);
        }
        while(!ds->eof) {
            unsigned char* start;
            int in_size=ds_get_packet(ds,&start);
            if( (mpctx->demuxer->file_format==DEMUXER_TYPE_AVI || mpctx->demuxer->file_format==DEMUXER_TYPE_ASF || mpctx->demuxer->file_format==DEMUXER_TYPE_MOV)
                    && stream_dump_type==2) fwrite(&in_size,1,4,f);
            if(in_size>0) fwrite(start,in_size,1,f);
            if(dvd_last_chapter>0) {
                int cur_chapter = demuxer_get_current_chapter(mpctx->demuxer);
                if(cur_chapter!=-1 && cur_chapter+1>dvd_last_chapter)
                    break;
            }
        }
        fclose(f);
        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_CoreDumped);
        exit_player_with_rc(EXIT_EOF, 0);
    }

#ifdef DVBT_USING_NORMAL_METHOD
dvb_change_channel:
#endif // end of DVBT_USING_NORMAL_METHOD
    mpctx->sh_audio=mpctx->d_audio->sh;
    mpctx->sh_video=mpctx->d_video->sh;

    if(mpctx->sh_video) {

        bidx = 0;		// Raymond 2010/03/17

        current_module="video_read_properties";

//Robert 20100712 reset video_aspect_ratio to default
        video_aspect_ratio = 0.0;
		SET_TIME(check_time);
#ifdef HW_TS_DEMUX
        if (hwtsdemux && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
        {
            int check_count = MAX_CHECK_VIDEO;
            while(check_count)
            {
                if((check_hwts_thread() & HWTS_WRITE_STREAM_WRITE_OK) ||(check_hwts_packets(mpctx->demuxer->priv, CHECK_VIDEO) > 2))
                    break;
                else
				{
					//printf("=== In [%s][%d] video[%d] audio[%d] sub[%d] check_hwts_thread() [%x]====\n", __func__, __LINE__, check_hwts_packets(mpctx->demuxer->priv, CHECK_VIDEO), check_hwts_packets(mpctx->demuxer->priv,CHECK_AUDIO), check_hwts_packets(mpctx->demuxer->priv, CHECK_SUB), check_hwts_thread() );
                    usleep(10000);
				}
                check_count--;
            }
            if (!check_count)
                printf("#### BUG!!!!! We in [%s][%d] check video packets [%d] vq[%d]####\n", __func__, __LINE__, MAX_CHECK_VIDEO, mpctx->d_video->packs);

        }
#endif // end of HW_TS_DEMUX
		PRINT_TIME(check_time, "wait check thread");
#ifdef QT_SUPPORT_DVBT
#ifdef DVBT_USING_NORMAL_METHOD
        if ((mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS) && (get_skydvb_wrokaround()== READY_TO_DISPLAY))
        {
            int check_count = MAX_CHECK_VIDEO ;
            while(check_count)
            {
                if(mpctx->d_video->packs)
                    break;
                else
                    usleep(10000);
                check_count--;
            }
            printf("####We in [%s][%d] check video packets [%d] vq[%d] count [%d] ####\n", __func__, __LINE__, MAX_CHECK_VIDEO, mpctx->d_video->packs, MAX_CHECK_VIDEO-check_count);
        }
#endif // end of DVBT_USING_NORMAL_METHOD
#endif /* end of QT_SUPPORT_DVBT */
		SET_TIME(check_time);
        if(!video_read_properties(mpctx->sh_video)) {
            mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CannotReadVideoProperties);
            mpctx->sh_video=mpctx->d_video->sh=NULL;

        } else {
            mp_msg(MSGT_CPLAYER,MSGL_V,MSGTR_FilefmtFourccSizeFpsFtime,
                   mpctx->demuxer->file_format,mpctx->sh_video->format, mpctx->sh_video->disp_w,mpctx->sh_video->disp_h,
                   mpctx->sh_video->fps,mpctx->sh_video->frametime
                  );

            /* need to set fps here for output encoders to pick it up in their init */
            if(force_fps) {
                mpctx->sh_video->fps=force_fps;
                mpctx->sh_video->frametime=1.0f/mpctx->sh_video->fps;
            }
            vo_fps = mpctx->sh_video->fps;

            if(!mpctx->sh_video->fps && !force_fps) {
                mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_FPSnotspecified);
//Robert 20100608 should fine tune later
//      mpctx->sh_video=mpctx->d_video->sh=NULL;
                mpctx->sh_video->fps = 1000.0;
                mp_msg(MSGT_CPLAYER,MSGL_ERR,"NO FPS Found, Force setting FPS = 1000.0\n");
            }
        }
		PRINT_TIME(check_time, "after video read properties");

        //Barry 2010-11-05
        if ( (mpctx->sh_video) && ((mpctx->sh_video->disp_w <= 640 && mpctx->sh_video->disp_w != 0) || (mpctx->sh_video->disp_h <= 480 && mpctx->sh_video->disp_h != 0)) )
        {
            cvq_threshold = 1;
            mp_msg(MSGT_CPLAYER,MSGL_V,"================> set cvq_threshold = %d\n", cvq_threshold);
        }
#if defined(HW_TS_DEMUX) && defined(SUPPORT_QT_BD_ISO_ENHANCE)
//Robert 20100119 disable quick_bd while format is VC1, fix this later
        else if ( (mpctx->sh_video) && quick_bd && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux)
        {
            if (mpctx->sh_video->format == mmioFOURCC('W','V','C','1'))
            {
                printf("MPlayer: found VC1 format, disable quick_bd\n");
                quick_bd = 0;
            }
        }
#if 1
        else if ( (mpctx->sh_video) && ((mpctx->sh_video->disp_w == 1920) || (mpctx->sh_video->disp_h >= 1080 && mpctx->sh_video->disp_h <= 1088)) )
        {
            if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux)
            {
                if ( (mpctx->sh_video->format == 0x10000002 && mpctx->sh_video->i_bps*8/1000 >= 60000) &&
                        (audio_id != -2) && (mpctx->sh_audio) && (mpctx->sh_audio->format == 0x2001)
                   )
                {
                    quick_bd = 1;
                    printf("MPEG2 %dx%d bit rate %d set quick_bd=1\n", mpctx->sh_video->disp_w, mpctx->sh_video->disp_h, mpctx->sh_video->i_bps*8/1000);
                }
            }
        }
#endif

//Robert 20110509 Force disable quick_bd, now. Until fix buffer overwrite issue.
        quick_bd = 0;

#endif
        sky_video_fmt_is_h264 = 0;
        sky_h264_need_check_dropB = 0;

        //Barry 2010-06-21, Carlos add check mpctx->sh_video avoid access NULL point, 2010-10-05
        if (mpctx->sh_video)
        {
            if (mpctx->sh_video->aspect > 0.0)
                video_aspect_ratio = mpctx->sh_video->aspect;
            else	//Barry 2010-10-11 set width/height as default aspect ratio, if mpctx->sh_video->aspect = 0.0
            {
                if (mpctx->sh_video->disp_h > 0)
                    video_aspect_ratio = (float)mpctx->sh_video->disp_w / (float)mpctx->sh_video->disp_h;
            }

            if (  
                 (
                  (mpctx->sh_video->format == mmioFOURCC('a','v','c','1')) ||
                  (mpctx->sh_video->format == mmioFOURCC('A','V','C','1')) ||
                  (mpctx->sh_video->format == mmioFOURCC('d','a','v','c')) ||
                  (mpctx->sh_video->format == mmioFOURCC('D','A','V','C')) ||
                  (mpctx->sh_video->format == mmioFOURCC('H','2','6','4')) ||
                  (mpctx->sh_video->format == mmioFOURCC('h','2','6','4')) ||
                  (mpctx->sh_video->format == mmioFOURCC('X','2','6','4')) ||
                  (mpctx->sh_video->format == mmioFOURCC('x','2','6','4')) ||
                  (mpctx->sh_video->format == 0x10000005) 
                 )
                )
            {
                sky_video_fmt_is_h264 = 1;
                printf("MPlayer: found H.264 format, FOURCC = 0x%.8x\n", mpctx->sh_video->format);
                if ((mpctx->sh_video->disp_w >= 1900) && (no_osd == 0))
                {
                    sky_h264_need_check_dropB = 1;
                    printf("MPlayer: Need check H.264 drop B frame\n");
                }
            }


        }

#if 1
//Robert 20100608 WMV3, WVC1, RV30, RV40, FLV1 use old A/V sync
        if (mpctx->sh_video)
        {
            switch(mpctx->sh_video->format)
            {
            case mmioFOURCC('W','M','V','3'):
                if (mpctx->sh_video->format == mmioFOURCC('W','M','V','3'))
                    printf("=== FOURCC WMV3 ===\n");
            case mmioFOURCC('W','V','C','1'):
                if (mpctx->sh_video->format == mmioFOURCC('W','V','C','1'))
                    printf("=== FOURCC WVC1 ===\n");
            case mmioFOURCC('R','V','3','0'):
                if (mpctx->sh_video->format == mmioFOURCC('R','V','3','0'))
                    printf("=== FOURCC RV30 ===\n");
            case mmioFOURCC('R','V','4','0'):
                if (mpctx->sh_video->format == mmioFOURCC('R','V','4','0'))
                    printf("=== FOURCC RV40 ===\n");
            case mmioFOURCC('F','L','V','1'):
                if (mpctx->sh_video->format == mmioFOURCC('F','L','V','1'))
                    printf("=== FOURCC FLV1 ===\n");
                sky_set_framerate((int)(0));
                break;
            default:
//Robert 20100601 change display framerate
                printf("=== FOURCC 0x%x ===\n", mpctx->sh_video->format);
                if (mpctx->sh_video->fps == 1000.0)
                {
                    sky_set_framerate((int)(0));
                }
                else
                {
                    sky_set_framerate((int)(mpctx->sh_video->fps*1000));
                }
                break;
            }
        }

        printf("======== debug , all set framerate 0\n");
        sky_set_framerate((int)(0));

#endif
    }

#if 1	//Barry 2011-03-22
    if (mpctx->demuxer && mpctx->demuxer->audio && !mpctx->demuxer->video && ipcdomain && start_play == PLAYER_STATUS_STOP && !no_osd)
    {
	free_demuxer(mpctx->demuxer);
	mpctx->demuxer = NULL;
	goto goto_next_file;
    }
#endif

    if(!mpctx->sh_video && !mpctx->sh_audio) {
        mp_msg(MSGT_CPLAYER,MSGL_FATAL, MSGTR_NoStreamFound);
#ifdef QT_SUPPORT_DVBT			
#ifdef CONFIG_DVBIN
        if(mpctx->stream->type == STREAMTYPE_DVB)
        {
            int dir;
            int v = mpctx->last_dvb_step;
            if(v > 0)
                dir = DVB_CHANNEL_HIGHER;
            else
                dir = DVB_CHANNEL_LOWER;

            if(dvb_step_channel(mpctx->stream, dir))
                mpctx->eof = mpctx->dvbin_reopen = 1;
        }
#endif
#endif /* end of QT_SUPPORT_DVBT */
        if(ipcdomain) //using domain socket option
            ipc_callback_error();
        goto goto_next_file; // exit_player(MSGTR_Exit_error);
    }

//20101213 Robert FOURCC RV30/RV40 need enable DTH + ATH for performance issue
#ifdef DEMUX_THREAD
    if (upnp)	//Barry 2011-01-18
    {
#if 0	//Fuchun 20110805 disable, avoid more audio packet be freed when FF2x
        if (mpctx->sh_video && mpctx->sh_audio && mpctx->demuxer && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS)
        {
            printf("===  UPNP (MPEG-PS): need enable DTH + ATH + VTH  ===\n");
            athreshold = 30;
            vthreshold = 2000;
            if (demuxthread == 0)
            {
                demuxthread = 1;
                demux_cache_enable(mpctx->demuxer);
            }
        }
        else 
#endif
        //Polun 2011-09-07 mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS for 元宵節影片
        //if (mpctx->sh_video && mpctx->sh_audio && mpctx->demuxer && mpctx->demuxer->type != DEMUXER_TYPE_MPEG_TS)
        if (mpctx->sh_video && mpctx->sh_audio && mpctx->demuxer && mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS)
        {
            printf("===  UPNP (other): need enable DTH + ATH + VTH  ===\n");
            sh_audio_t *sh = mpctx->sh_audio;
            if(sh->format == 0x160 || sh->format == 0x161 || sh->format == 0x162 || sh->format == 0x58)	//Fuchun 20110811 for wmapro, wma, amr
            {
                 athreshold = 5;
            }
            else
                athreshold = 30;
            vthreshold = 30;
            if (demuxthread == 0)
            {
                demuxthread = 1;
                demux_cache_enable(mpctx->demuxer);
            }
        }
    }
    else if (mpctx->sh_video && mpctx->sh_audio && mpctx->demuxer->type == DEMUXER_TYPE_REAL)
    {
        if (mpctx->sh_audio->format == mmioFOURCC('c','o','o','k'))
        {
            if ((mpctx->sh_video->format == mmioFOURCC('R','V','3','0') ||
                    mpctx->sh_video->format == mmioFOURCC('R','V','4','0'))
                    && (mpctx->sh_video->disp_w >= 1440 && mpctx->sh_video->disp_h >= 1080/2)
               )
            {
                printf("=== FOURCC RV30/RV40 === need enable DTH + ATH\n");
                athreshold = 25;
                if (demuxthread == 0)
                {
                    demuxthread = 1;
                    demux_cache_enable(mpctx->demuxer);
                }
            }
        }
    }
#endif

    /* display clip info */
    demux_info_print(mpctx->demuxer);

//================== Read SUBTITLES (DVD & TEXT) ==========================
    if(vo_spudec==NULL &&
            (mpctx->stream->type==STREAMTYPE_DVD || mpctx->stream->type == STREAMTYPE_DVDNAV)) {
        init_vo_spudec();
    }
    //Polun 2011-06-13 ++s 
    if((mpctx->global_sub_size == 0) && (mpctx->demuxer->file_format == DEMUXER_TYPE_MATROSKA) )
    {
        ass_enabled = 0;
        printf("!!!!!!!!!!!!no ass on MKV file  ass_enabled = 0\n");
    }
    //Polun 2011-06-13 ++e 
//if(1 || mpctx->sh_video) {
    if ( (1 || mpctx->sh_video) && (!no_osd) ) {	//Barry 2010-08-09
// after reading video params we should load subtitles because
// we know fps so now we can adjust subtitle time to ~6 seconds AST
// check .sub
        double fps = mpctx->sh_video ? mpctx->sh_video->fps : 25;
        current_module="read_subtitles_file";
        if(sub_name) {
            for (i = 0; sub_name[i] != NULL; ++i)
                add_subtitles (sub_name[i], fps, 0);
        }
        if(sub_auto) { // auto load sub file ...
            char *psub = get_path( "sub/" );
            char **tmp = sub_filenames((psub ? psub : ""), filename);
            int i = 0;
            free(psub); // release the buffer created by get_path() above
            if (tmp)
            {
                while (tmp[i]) {
                    add_subtitles (tmp[i], fps, 1);
                    free(tmp[i++]);
                }
                free(tmp);
            }
        }
        if (mpctx->set_of_sub_size > 0)  {
            // setup global sub numbering
            mpctx->global_sub_indices[SUB_SOURCE_SUBS] = mpctx->global_sub_size; // the global # of the first sub.
            mpctx->global_sub_size += mpctx->set_of_sub_size;
        }
    }

#if 1	//Barry 2011-03-22
    if (mpctx->demuxer && mpctx->demuxer->audio && !mpctx->demuxer->video && ipcdomain && start_play == PLAYER_STATUS_STOP && !no_osd)
    {
	free_demuxer(mpctx->demuxer);
	mpctx->demuxer = NULL;
	goto goto_next_file;
    }
#endif

    if (mpctx->global_sub_size) {
        select_subtitle(mpctx, 0);
        subcc_force_off = 0;	//Fuchun 20110823

        if(subdata)
            switch (stream_dump_type) {
            case 3:
                list_sub_file(subdata);
                break;
            case 4:
                dump_mpsub(subdata, mpctx->sh_video->fps);
                break;
            case 6:
                dump_srt(subdata, mpctx->sh_video->fps);
                break;
            case 7:
                dump_microdvd(subdata, mpctx->sh_video->fps);
                break;
            case 8:
                dump_jacosub(subdata, mpctx->sh_video->fps);
                break;
            case 9:
                dump_sami(subdata, mpctx->sh_video->fps);
                break;
            }
        if(no_osd == 0)
            vo_need_osd = 1;	//Fuchun 2009.12.15
        else
            vo_need_osd = 0;
    }
    else		//Fuchun 2010.03.24
    {
        sprintf(callback_str, "SUBTITLE: SKY_NOSUB\0");
        ipc_callback(callback_str);
        vo_need_osd = 0;
    }

    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_FILENAME=%s\n",
           filename_recode(filename));
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_DEMUXER=%s\n", mpctx->demuxer->desc->name);
    if (mpctx->sh_video) {
        /* Assume FOURCC if all bytes >= 0x20 (' ') */
        if (mpctx->sh_video->format >= 0x20202020)
            mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FORMAT=%.4s\n", (char *)&mpctx->sh_video->format);
        else
            mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FORMAT=0x%08X\n", mpctx->sh_video->format);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_BITRATE=%d\n", mpctx->sh_video->i_bps*8);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_WIDTH=%d\n", mpctx->sh_video->disp_w);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_HEIGHT=%d\n", mpctx->sh_video->disp_h);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FPS=%5.3f\n", mpctx->sh_video->fps);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_ASPECT=%1.4f\n", mpctx->sh_video->aspect);
    }
    if (mpctx->sh_audio) {
        /* Assume FOURCC if all bytes >= 0x20 (' ') */
        if (mpctx->sh_audio->format >= 0x20202020)
            mp_msg(MSGT_IDENTIFY,MSGL_INFO, "ID_AUDIO_FORMAT=%.4s\n", (char *)&mpctx->sh_audio->format);
        else
            mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_FORMAT=%d\n", mpctx->sh_audio->format);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_BITRATE=%d\n", mpctx->sh_audio->i_bps*8);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_RATE=%d\n", mpctx->sh_audio->samplerate);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_NCH=%d\n", mpctx->sh_audio->channels);
    }
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_LENGTH=%.2lf\n", demuxer_get_time_length(mpctx->demuxer));
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_SEEKABLE=%d\n",
           mpctx->stream->seek && (!mpctx->demuxer || mpctx->demuxer->seekable));
    if (mpctx->demuxer) {
        if (mpctx->demuxer->num_chapters == 0)
            stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS, &mpctx->demuxer->num_chapters);
        mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_CHAPTERS=%d\n", mpctx->demuxer->num_chapters);
    }

    if(!mpctx->sh_video) goto main; // audio-only

    if(!reinit_video_chain()) {
        if(!mpctx->sh_video) {
            if(!mpctx->sh_audio)
            {
                if(ipcdomain) //using domain socket option
                    ipc_callback_error();
                goto goto_next_file;
            }
            goto main; // exit_player(MSGTR_Exit_error);
        }
    }

    if(vo_flags & 0x08 && vo_spudec)
        spudec_set_hw_spu(vo_spudec,mpctx->video_out);

#ifdef CONFIG_FREETYPE
    force_load_font = 1;
#endif

#if 1	//Barry 2011-03-22
    if (mpctx->demuxer && mpctx->demuxer->audio && !mpctx->demuxer->video && ipcdomain && start_play == PLAYER_STATUS_STOP && !no_osd)
    {
	free_demuxer(mpctx->demuxer);
	mpctx->demuxer = NULL;
	goto goto_next_file;
    }
#endif

    /*
    main________________()
    */
//================== MAIN: ==========================
main:
    current_module="main";

    if(playing_msg) {
        char* msg = property_expand_string(mpctx, playing_msg);
        mp_msg(MSGT_CPLAYER,MSGL_INFO,"%s",msg);
        free(msg);
    }


// Disable the term OSD in verbose mode
    if(verbose) term_osd = 0;

    {
//int frame_corr_num=0;   //
//float v_frame=0;    // Video
//float num_frames=0;      // number of frames played

        int frame_time_remaining=0; // flag
        int blit_frame=0;
        mpctx->num_buffered_frames=0;

// Make sure old OSD does not stay around,
// e.g. with -fixed-vo and same-resolution files
        clear_osd_msgs();
        update_osd_msg();

//Robert 20110318
//+SkyViia_Vincent01272010
/*
        if(thumbnail_mode == 1)
        {
            mpctx->sh_audio=mpctx->d_audio->sh=NULL; // -> nosound
            mpctx->d_audio->id = -2;
        }
*/
//SkyViia_Vincent01272010+

//================ SETUP AUDIO ==========================
        //Barry 2011-05-04
        #if 0 //Polun 2012-01-20 fixed mantis 6860  Audio format(AAC-advanc) not support return value.
        if(!mpctx->sh_audio)
        #else
        if(!mpctx->sh_audio && not_supported_profile_level == 0 )
        #endif
        {
		not_supported_profile_level = 3;
        } 
        if(mpctx->sh_audio) {
#ifdef QT_SUPPORT_DVBT		
#ifdef DVBT_USING_NORMAL_METHOD
            if ((mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS) && (get_skydvb_wrokaround() == READY_TO_DISPLAY))
            {
                int check_count = MAX_CHECK_AUDIO;
                while(check_count)
                {
                    if(mpctx->d_audio->packs)
                        break;
                    else
                        usleep(100000);
                    check_count--;
                }
                printf("####We in [%s][%d] check audio packets [%d] aq[%d] count [%d] ####\n", __func__, __LINE__, MAX_CHECK_AUDIO, mpctx->d_audio->packs, MAX_CHECK_AUDIO-check_count);
            }
#endif // end of DVBT_USING_NORMAL_METHOD
#endif /* end of QT_SUPPORT_DVBT */
#ifdef CHECK_AAC_HEADER
			//if(mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS && mpctx->sh_audio->format = AUDIO_MP2)
			if(mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS && mpctx->sh_audio->format == 0x50)
			{
				int ii = 0;
				audio_check_debug("@@@@ In [%s][%d] we find the DEMUXER_TYPE_MPEG_TS audio format [%x]@@@@\n", __func__, __LINE__, mpctx->sh_audio->format);
				while(mpctx->d_audio->bytes < 8000 && ii < 20)
				{
					ii ++;
					audio_check_debug("@@@@ In [%s][%d] before call get audio count [%d]  lenght [%d] @@@@\n", __func__, __LINE__, mpctx->d_audio->packs, mpctx->d_audio->bytes);
					demux_fill_buffer(mpctx->demuxer,mpctx->d_audio);

				}
				audio_check_debug("@@@@ In [%s][%d] before call get audio count [%d]  lenght [%d] @@@@\n", __func__, __LINE__, mpctx->d_audio->packs, mpctx->d_audio->bytes);
				if (mpctx->d_audio->bytes > 8000)
				{
					int check_length = 0;
					char *new_buff = NULL;
					new_buff = calloc(mpctx->d_audio->bytes, sizeof(unsigned char));
					if (new_buff)
					{
						demux_packet_t *p = mpctx->demuxer->audio->first;
						for(ii = 0 ; ii < mpctx->d_audio->packs ; ii++)
						{
							if (p)
							{
								audio_check_debug("$$$$ In [%s][%d] new_buff [%p] + [%d] [%p] , p->buffer[%p], p->len[%d] $$$$\n", __func__, __LINE__, new_buff, check_length, new_buff +  check_length, p->buffer, p->len);
								memcpy(new_buff +  check_length , p->buffer, p->len);
								check_length += p->len;
								p = p->next;
							}
							else
								mplayer_debug("$$$$ In [%s][%d] p is NULL ii [%d] mpctx->d_audio->packs[%d]$$$$\n", __func__, __LINE__, ii, mpctx->d_audio->packs);

						}
						audio_check_debug("$$$$ In [%s][%d] will do check_aac_header $$$$\n", __func__, __LINE__);
						check_length = check_aac_header(new_buff, check_length, 3);
						audio_check_debug("$$$$ In [%s][%d] after check_aac_header check_length [%d] $$$$\n", __func__, __LINE__, check_length);
						if (check_length < (mpctx->d_audio->bytes - 4) )
						{
							//mpctx->sh_audio->format = AUDIO_AAC;
							mpctx->sh_audio->format = mmioFOURCC('M', 'P', '4', 'A');
							mplayer_debug("@@@@ [%s][%d] We switch format from [%x] to [%x] @@@@\n", __func__, __LINE__, 0x50, mpctx->sh_audio->format);
						}
						free(new_buff);
						new_buff = NULL;
					}
					else
						mplayer_debug("$$$$ In check audio header [%s][%d] new_buff is NUll$$$$\n", __func__, __LINE__);
				}
			}
#endif /* end of CHECK_AAC_HEADER */
            reinit_audio_chain();
            //Charles 2011.03.22 don't do this for STREAMTYPE_STREAM (iTV)
            //Polun 2011-10-24 ++s fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv -ss can't AVSYNC move from video_updata to here.
            if(recalcul_apts && mpctx->sh_audio && mpctx->stream->type != STREAMTYPE_STREAM && mpctx->d_audio->id != -2)
	     {
                sh_video_t * const sh_video = mpctx->sh_video;
		  if(mpctx->demuxer->type == DEMUXER_TYPE_ASF && (mpctx->sh_audio->format == 0x160 || mpctx->sh_audio->format == 0x161))
		  {
                     fill_audio_out_buffers();
			float a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
                     #if 0  //Polun 2011-11-4 fixed mantis6373 avsync problem. because code have change the value -0.033 not correct 
			audio_delay = a_pts - first_frame_pts -0.033;
                     #else
                     if(a_pts < first_frame_pts) //Polun 2012-01-19 fixed mantis6806 avsync problem.
			    audio_delay = a_pts - first_frame_pts + 0.132;
                     else
                         audio_delay = a_pts - first_frame_pts - 0.132;
                     #endif
                     printf("#### a_pts = %f  first_frame_pts = %f \n",a_pts,first_frame_pts);
			printf("#### ASF with wma(0x%x)  set audio_delay:%f ####\n", mpctx->sh_audio->format, audio_delay);
		   }
		   recalcul_apts = 0;
	     }
            //Polun 2011-10-24 ++e
            if (mpctx->sh_audio && mpctx->sh_audio->codec)
                mp_msg(MSGT_IDENTIFY,MSGL_INFO, "ID_AUDIO_CODEC=%s\n", mpctx->sh_audio->codec->name);
        }

        current_module="av_init";

        if(mpctx->sh_video) {
            mpctx->sh_video->timer=0;
            if (! ignore_start)
                audio_delay += mpctx->sh_video->stream_delay;
        }
        if(mpctx->sh_audio) {
            if (start_volume >= 0)
                mixer_setvolume(&mpctx->mixer, start_volume, start_volume);
            if (! ignore_start)
                audio_delay -= mpctx->sh_audio->stream_delay;
            mpctx->delay=-audio_delay;
        }

        if(!mpctx->sh_audio) {
            mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_NoSound);
            mp_msg(MSGT_CPLAYER,MSGL_V,"Freeing %d unused audio chunks.\n",mpctx->d_audio->packs);
            ds_free_packs(mpctx->d_audio); // free buffered chunks

#ifdef HW_TS_DEMUX
#ifdef QT_SUPPORT_DVBT
            if ((mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS) || ( mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS && (hwtsdemux)))
#else /* else of QT_SUPPORT_DVBT */
            if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS && (hwtsdemux))
#endif /* end of QT_SUPPORT_DVBT */
#else
            if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
#endif // end of HW_TS_DEMUX
                //stop audio demux
            {
                dvb_stop_audio(mpctx->stream, mpctx->demuxer, mpctx->demuxer);
            }

            //mpctx->d_audio->id=-2;         // do not read audio chunks
            //uninit_player(INITIALIZED_AO); // close device
        }

#ifdef  jfueng_2011_0311
        else
          {

           audio_format_ref = mpctx->sh_audio->format; 
           //printf("\n  JF ::  reset  0  \n");          
	     audio_long_period = 0;	   
           a_long_quiet_SP_yes =0;
           a_long_quiet_SP_cnt=0;
		             
          }		 
#endif
		
        if(!mpctx->sh_video) {
            mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_Video_NoVideo);
            mp_msg(MSGT_CPLAYER,MSGL_V,"Freeing %d unused video chunks.\n",mpctx->d_video->packs);
            ds_free_packs(mpctx->d_video);
            mpctx->d_video->id=-2;
            //if(!fixed_vo) uninit_player(INITIALIZED_VO);
        }

        if (!mpctx->sh_video && !mpctx->sh_audio)
        {
            if(ipcdomain) //using domain socket option
                ipc_callback_error();
#ifdef QT_SUPPORT_DVBT				
#ifdef DVBT_USING_NORMAL_METHOD
            if ((mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS) && (get_skydvb_wrokaround() & LOST_DVBT_SIGNAL))
            {
                dvb_priv_t *priv = (dvb_priv_t*) mpctx->stream->priv;
                if(priv->is_on)
                {
                    int dir;
                    dir = 0;
                    mpctx->eof = mpctx->dvbin_reopen = 1;
                    printf("#### We received skydvb_need_workaround[%d] is_on [%d]### in [%s][%d] \n", get_skydvb_wrokaround(), priv->is_on, __func__, __LINE__);
                }
                set_skydvb_wrokaround(INIT_STATUS);
                if (parse_and_run_command() == 2)
                    goto goto_enable_cache;
            }
#endif // end of DVBT_USING_NORMAL_METHOD
#endif /* end of QT_SUPPORT_DVBT */
            goto goto_next_file;
        }

//if(demuxer->file_format!=DEMUXER_TYPE_AVI) pts_from_bps=0; // it must be 0 for mpeg/asf!
        if(force_fps && mpctx->sh_video) {
            vo_fps = mpctx->sh_video->fps=force_fps;
            mpctx->sh_video->frametime=1.0f/mpctx->sh_video->fps;
            mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_FPSforced,mpctx->sh_video->fps,mpctx->sh_video->frametime);
        }

#ifdef CONFIG_GUI
        if ( use_gui ) {
            if ( mpctx->sh_audio ) guiIntfStruct.AudioType=mpctx->sh_audio->channels;
            else guiIntfStruct.AudioType=0;
            if ( !mpctx->sh_video && mpctx->sh_audio ) guiGetEvent( guiSetAudioOnly,(char *)1 );
            else guiGetEvent( guiSetAudioOnly,(char *)0 );
            guiGetEvent( guiSetFileFormat,(char *)mpctx->demuxer->file_format );
            if ( guiGetEvent( guiSetValues,(char *)mpctx->sh_video ) ) goto goto_next_file;
            guiGetEvent( guiSetDemuxer,(char *)mpctx->demuxer );
        }
#endif

        mp_input_set_section(NULL);
//TODO: add desired (stream-based) sections here
        if (mpctx->stream->type==STREAMTYPE_TV) mp_input_set_section("tv");
        if (mpctx->stream->type==STREAMTYPE_DVDNAV) mp_input_set_section("dvdnav");

#if 1	//Barry 2011-03-22
    if (mpctx->demuxer && mpctx->demuxer->audio && !mpctx->demuxer->video && ipcdomain && start_play == PLAYER_STATUS_STOP && !no_osd)
    {
	free_demuxer(mpctx->demuxer);
	mpctx->demuxer = NULL;
	goto goto_next_file;
    }
#endif
//==================== START PLAYING =======================
        if(mpctx->sh_video && video_id != -2) {
//H264 do sync here
#ifdef QT_SUPPORT_DVBT // carlos  marked for testing 2010-09-03
            if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
//    unsigned char *v_start, *a_start;
//    double apts, vpts;
//    int in_size, vin_size;
//    int need_packs;
//    float avdelay_check=0.0;
                /*
                    if (mpctx->sh_video->format == 0x10000005)
                    {
                        need_packs = 100;
                    }
                    else
                    {
                        need_packs = 15;
                    }
                */
//    demux_stream_t *d_video = mpctx->demuxer->video;
//    demux_stream_t *d_audio = mpctx->demuxer->audio;

                if (video_id != -2 && audio_id != -2)
                {
                    sample_resize_init();
//20100811 Robert some ts stream with large frame size, can not pre-queue too much
#if 1
                    demux_fill_buffer(mpctx->demuxer,mpctx->d_video);
                    demux_fill_buffer(mpctx->demuxer,mpctx->d_audio);
                    fill_audio_out_buffers_with_resamp(1);

#else
                    printf("starting fill %d video packs\n", need_packs);
                    demux_fill_buffer(mpctx->demuxer,mpctx->d_video);
                    demux_fill_buffer(mpctx->demuxer,mpctx->d_audio);

                    printf("video packs:%d  audio_packs:%d\n", mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);

                    avdelay_check = (float)mpctx->d_video->pts - (float)mpctx->d_audio->pts;
//        printf("avdelay_check=%f  avdelay_check*mpctx->sh_video->fps=%f\n", avdelay_check, avdelay_check*mpctx->sh_video->fps);


#if 0
                    while(mpctx->demuxer->video->packs < need_packs)
                    {
                        demux_fill_buffer(mpctx->demuxer,mpctx->d_video);
                    }
#else
                    while(avdelay_check >= 0.3)
                    {
//            demux_fill_buffer(mpctx->demuxer,mpctx->d_video);
                        while (mpctx->demuxer->video->packs<=20)
                        {
                            demux_fill_buffer(mpctx->demuxer,mpctx->d_video);
                        }
                        while (mpctx->demuxer->audio->packs<=2)
                        {
                            demux_fill_buffer(mpctx->demuxer,mpctx->d_audio);
                        }
                        fill_audio_out_buffers_with_resamp(1);
                        avdelay_check = (float)mpctx->d_video->pts - (float)mpctx->d_audio->pts;
//        printf("~~~avdelay_check=%f  vp:%d ap:%d\n", avdelay_check, mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);
                    }
#endif
//printf("  %d video packs ready\n", need_packs);
                    printf("pre queue video packs:%d  audio_packs:%d\n", mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);

//printf("======== v_pts:%f   a_pts:%f\n", (float)mpctx->d_video->pts, (float)mpctx->d_audio->pts);
                    avdelay_check = (float)mpctx->d_video->pts - (float)mpctx->d_audio->pts;
                    if ( avdelay_check >= 0)
                    {
                        while(avdelay_check > 0.4)
                        {
                            fill_audio_out_buffers_with_resamp(1);
                            avdelay_check = (float)mpctx->d_video->pts - (float)mpctx->d_audio->pts;

                        }
                    }

                    printf("======== v_pts:%f   a_pts:%f\n", (float)mpctx->d_video->pts, (float)mpctx->d_audio->pts);
                    printf("video packs:%d  audio_packs:%d\n", mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);
                    avdelay_check = (float)mpctx->d_video->pts - (float)mpctx->d_audio->pts;
                    printf("~~~avdelay_check=%f  vp:%d ap:%d\n", avdelay_check, mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);
#endif /* end of 1 */
                }
            }
#endif /* end of QT_SUPPORT_DVBT */
        }


        if(mpctx->loop_times>1) mpctx->loop_times--;
        else if(mpctx->loop_times==1) mpctx->loop_times = -1;

        mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_StartPlaying);

        total_time_usage_start=GetTimer();
        audio_time_usage=0;
        video_time_usage=0;
        vout_time_usage=0;
        total_frame_cnt=0;
        drop_frame_cnt=0; // fix for multifile fps benchmark
        play_n_frames=play_n_frames_mf;
        mpctx->startup_decode_retry = DEFAULT_STARTUP_DECODE_RETRY;

        if(play_n_frames==0) {
            mpctx->eof=PT_NEXT_ENTRY;

            if(ipcdomain) //using domain socket option
                ipc_callback_error();

            goto goto_next_file;
        }

#ifdef HAS_DEC_CAPABILITY        //Barry 2011-03-24
        if ( audio_id == -1 && !mpctx->sh_audio && (!svsd_state->dec_cap.dts || !svsd_state->dec_cap.dolby) && !sky_hwac3)
        {
        	int jj;

		if (mpctx->demuxer->type != DEMUXER_TYPE_MPEG_TS && mpctx->demuxer->type != DEMUXER_TYPE_MPEG_PS)
		{
			for (jj = 0; jj < MAX_A_STREAMS; jj++)
			{
				sh_audio_t *sh = mpctx->demuxer->a_streams[jj];
				if (sh)
				{
					if ( (sh->format != 0x2001 && !svsd_state->dec_cap.dts) || (sh->format != 0x2000 && !svsd_state->dec_cap.dolby) )
					{
						if ( !svsd_state->dec_cap.dts && !svsd_state->dec_cap.dolby && (sh->format == 0x2001 ||sh->format == 0x2000) )
							continue;
						mpctx->demuxer->audio->id = jj;
						sh->ds = mpctx->demuxer->audio;
						mpctx->sh_audio = mpctx->d_audio->sh = mpctx->demuxer->audio->sh = sh;
						reinit_audio_chain();
						if (!seek_to_sec)
							rel_seek_secs -= 5;
                                          //Charles 2011.03.22 don't do this for STREAMTYPE_STREAM (iTV)
                                          //Polun 2011-10-24 ++s fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv -ss can't AVSYNC move from video_updata to here.
                                         if(recalcul_apts && mpctx->sh_audio && mpctx->stream->type != STREAMTYPE_STREAM && mpctx->d_audio->id != -2)
	                                  {
                                             sh_video_t * const sh_video = mpctx->sh_video;
		                               if(mpctx->demuxer->type == DEMUXER_TYPE_ASF && (mpctx->sh_audio->format == 0x160 || mpctx->sh_audio->format == 0x161))
		                               {
                                                 fill_audio_out_buffers();
			                            float a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
                                                 #if 0  //Polun 2011-11-4 fixed mantis6373 avsync problem. because code have change the value -0.033 not correct 
                            			audio_delay = a_pts - first_frame_pts -0.033;
                                                 #else
                                                 if(a_pts < first_frame_pts)//Polun 2012-01-19 fixed mantis6806 avsync problem.
                            			    audio_delay = a_pts - first_frame_pts + 0.132;
                                                 else
                                                     audio_delay = a_pts - first_frame_pts - 0.132;
                                                 #endif
                                                 printf("#### a_pts = %f  first_frame_pts = %f \n",a_pts,first_frame_pts);
			                            printf("#### ASF with wma(0x%x)  set audio_delay:%f ####\n", mpctx->sh_audio->format, audio_delay);
		                               }
		                               recalcul_apts = 0;
	                                   }
                                          //Polun 2011-10-24 ++e 
                                          //Polun 2011-08-19 ++s for mantis 5888 if first audio not support, jump to next support audio and break, 
                                          if(mpctx->sh_audio!=NULL && mpctx->d_audio->id != -2)
                                               break;
                                          //Polun 2011-08-19 ++e

					}
				}
			}
		}
        }
#endif

//20110623 Robert for BD, set seek_to_sec = 0.1 while -ss 0 or no -ss
#ifdef CONFIG_BLURAY
        if (bluray_device != NULL && seek_to_sec == 0)
        {
                seek_to_sec = 0.1;
        }
#endif
        if (seek_to_sec) {
            //Barry 2011-03-15
            if (mpctx->demuxer->type == DEMUXER_TYPE_REAL && mpctx->sh_video)
            {
            	strcpy(callback_str,"status: demuxing\0");
		ipc_callback(callback_str);
            }

            seek(mpctx, seek_to_sec, SEEK_ABSOLUTE);
            end_at.pos += seek_to_sec;
        }

        if (end_at.type == END_AT_SIZE) {
            mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_MPEndposNoSizeBased);
            end_at.type = END_AT_NONE;
        }

        if(ipcdomain) //using domain socket option
        {
            start_play = PLAYER_STATUS_PLAYING;

            if (mpctx->stream->type == STREAMTYPE_DVDNAV)
            {
//                if(dvdnavstate.dvdnav_state_change == 1) // for call back status menu or movie
//                {
//                //call back of the first state for dvanav
//                if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
//                {
//                    strcpy(callback_str,"status: menu\0");
//                    ipc_callback(callback_str);
//                }
//                else if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
//                {
//						strcpy(callback_str,"status: playing\0");
//						ipc_callback(callback_str);
//					}
//					dvdnavstate.dvdnav_state_change = 0;
//				}

                if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                {
                    //Barry 2010-11-11
                    if (not_supported_profile_level == 1)
                    {
                        strcpy(callback_str,"status: playing 1\0");
                        ipc_callback(callback_str);
                        free_demuxer(mpctx->demuxer);
                        mpctx->demuxer = NULL;
                        goto goto_next_file;
                    }
		    else if ( (not_supported_profile_level == 2 || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
                    {
                        if (not_supported_profile_level == 3)
                        	strcpy(callback_str,"status: playing 3\0");
                        else
                        {
                        	if ( (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001)) || (!mpctx->sh_audio && no_audio_format == 0x2001) )
					strcpy(callback_str,"status: playing 2\0");
				else
					strcpy(callback_str,"status: playing 4\0");
				sprintf(audio_names, "AUDIO: No Audio can switch\0");
				ipc_callback(audio_names);
                        }
                    }
                    else
                        strcpy(callback_str,"status: playing 0\0");
                    ipc_callback(callback_str);
                }
            }
            else
            {
                //Barry 2010-11-11
                if (not_supported_profile_level == 1)
                {
                    strcpy(callback_str,"status: playing 1\0");
                    ipc_callback(callback_str);
                    free_demuxer(mpctx->demuxer);
                    mpctx->demuxer = NULL;
                    goto goto_next_file;
                }
		else if ( (not_supported_profile_level == 2 || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
                {
                    if (not_supported_profile_level == 3)
                    		strcpy(callback_str,"status: playing 3\0");
                    else
                    {
                    		if ( (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001)) || (!mpctx->sh_audio && no_audio_format == 0x2001) )
					strcpy(callback_str,"status: playing 2\0");
				else
					strcpy(callback_str,"status: playing 4\0");
				sprintf(audio_names, "AUDIO: No Audio can switch\0");
				ipc_callback(audio_names);
                    }
                }
                else
                    strcpy(callback_str,"status: playing 0\0");
                ipc_callback(callback_str);

		ipc_callback_audio_sub();
            }
			usleep(300000);
        }

#ifdef CONFIG_DVDNAV
        mp_dvdnav_context_free(mpctx);
        if (mpctx->stream->type == STREAMTYPE_DVDNAV) {
		//Barry 2011-05-07
		if (!no_osd)
		{
			strcpy(callback_str,"video: progressive\0");
			ipc_callback(callback_str);
		}
		
            // enable read from dvdnav stream
            mp_dvdnav_read_wait(mpctx->stream, 0, 1);

            // reset cell change flag
            mp_dvdnav_cell_has_changed(mpctx->stream,1);
        }
#endif

#if 0	//Barry 2010-07-08 enable frame_dropping for TS file
//Robert 20100617 disable frame_dropping for MPEG_TS filed picture format
//   please check this later
//if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && (h264_frame_mbs_only == 0))
        if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
        {
//    frame_dropping = 0;
        }
#endif
        entry_while = 1;    //SkyViia_Vincent06182010

        if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video && nosync==0 && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU) 	//Fuchun 2010.07.20
        {
#ifdef DVBT_USING_NORMAL_METHOD
// TODO, Need FuChun add new A-V sync method, for dvb-T
#ifdef QT_SUPPORT_DVBT
            if (mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
			{
#ifdef SUPPORT_DIVX_DRM
				if (strncmp(video_names, "Xvid_Compatible", strlen("Xvid_Compatible")) == 0)
					seek_sync_flag = 0;
				else
#endif /* end of SUPPORT_DIVX_DRM */				
					seek_sync_flag = 2;
				//printf("@@@@ In [%s][%d] set seek_sync_flag to [%d] video_names[%s]@@@@\n", __func__, __LINE__, seek_sync_flag, video_names);
			}
            else
#endif /* end of QT_SUPPORT_DVBT */			
			{
                seek_sync_flag = 3;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
				//if (stop_tuner && (mpctx->d_video->packs > (int)(mpctx->sh_video->fps) * 2))
				if (stop_tuner && (mpctx->d_video->packs > (int)(mpctx->sh_video->fps) ))
				{
					printf("### In [%s][%d], vq :[%d] aq[%d] ###\n", __func__, __LINE__, mpctx->d_video->packs, mpctx->d_audio->packs);
					if (mpctx->d_video)
						ds_free_packs(mpctx->d_video); 
					if (mpctx->d_audio)
						ds_free_packs(mpctx->d_audio); 
					if (mpctx->d_sub)
						ds_free_packs(mpctx->d_sub); 
				}
#endif /* end of SUPPORT_SKYDVB_DYNAMIC_PID */
			}
#else // else of DVBT_USING_NORMAL_METHOD
            seek_sync_flag = 2;
#endif
        }

        is_first_loop = 1;

#if defined(CONFIG_DVDNAV) && defined(CONFIG_DVDNAV_MAIN2)
    if (mpctx->stream->type == STREAMTYPE_DVDNAV) {
    	if(seek_to_sec)
    		dvdnav_resume = 1;

    	if(dvdnav_resume)
    	{
    		dvdnav_sub_menu_select = 2;
    		dvdnav_audio_menu_select = 2;

    		mpctx->demuxer->audio->id = audio_id;
    		dvdnav_movie_audio_id = mpctx->demuxer->audio->id;

		int i;
		for(i = 0; i < 8; i++)
		{
			if(dvdnavstate.aid[i] == mpctx->demuxer->audio->id)
			{
				//printf("***command.c****ii[%d]\n",i);
				dvdnav_demuxer_set_audio_reg(mpctx->demuxer,i);
				break;
			}
		}

		int x,y=0;
		extern int tmp_sid;
		extern int needs_change_dsub_id;
		for (x = 0; x < 32; x++)
		{
			if(dvdnavstate.sid[x] != -1)
			{
				if(dvdsub_id == y)
				{
					tmp_sid = dvdnavstate.sid[x];
					break;
				}
				y++;
			}
			else
				needs_change_dsub_id = 1;
		}
	
		dvdnav_movie_sub_id = mpctx->d_sub->id=tmp_sid;


		int dvdnav_sub_result;
		dvdnav_sub_result = dvdnav_demuxer_set_sub_reg(mpctx->demuxer, dvdsub_id);
mplayer_dvdnav_debug("#0# DVDNAV In [%s][%d],mpctx->d_sub->id[%d],dvdnav_movie_sub_id[%d],dvdsub_id[%d]##\n", __func__, __LINE__,mpctx->d_sub->id,dvdnav_movie_sub_id,dvdsub_id);
    	}
    	
        //Barry 2011-05-04
        if (!no_osd)
        {
	        strcpy(callback_str,"video: progressive\0");
	        ipc_callback(callback_str);
        }

        int ret = 0;
        ret = do_dvdloop(mpctx, &frame_cnt, &c_total, seek_to_sec, &end_at, &initialized_flags);
        if (ret == 2)
        {
            goto goto_enable_cache;
        }
        else if (ret == 3)
        {
            goto goto_next_file;
        }
    }
    else
#endif
    {
        /*
        MAIN_while________________()
        */
        while(!mpctx->eof) {
            float aq_sleep_time=0;
            //printf("### Carlos in [%s][%d], vq :[%d] aq[%d] ###\n", __func__, __LINE__, mpctx->d_video->packs, mpctx->d_audio->packs);
#if 0
//if (mpctx->delay < 0)
            {
                static float last_delay=1.0;
                vdec_svread_buf_t nrb;
//printf("bf sv_read...\n");
                svread(0, &nrb, 0);
                if (nrb.qlen[2] == 0)
                    printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d [2]:[%d]r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.qlen[2], nrb.ft_ridx, nrb.ft_widx);
                /*
                printf("apts2 = %f next_pts = %f ", playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out), sky_get_next_pts(nrb.cur_pts));
                if (last_delay < 0.0 && mpctx->delay > 0.0)
                {
                printf("!!! === a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
                }
                else
                {
                printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
                }
                */
                last_delay = mpctx->delay;
//printf("=== a_pts:%f  v_pts:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
//printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
            }
#endif

#if 1 //20120113 charleslin - set mpctx->eof if both audio and video are invalid or eof
	    if((mpctx->sh_audio == NULL || mpctx->d_audio->eof) &&
	       (mpctx->sh_video == NULL || mpctx->d_video->eof)){
		mpctx->eof = PT_NEXT_ENTRY;
		printf("sh_audio:%p aeof:%d apacks:%d sh_video:%p veof:%d vpacks:%d\n", mpctx->sh_audio, mpctx->d_audio->eof, mpctx->d_audio->packs, mpctx->sh_video, mpctx->d_video->eof, mpctx->d_video->packs);
		break;
	    }
#endif

            if(force_quit)
                break;

#ifdef DEMUX_THREAD //charleslin 20100603
            //demux_check(mpctx->demuxer);
#endif

if(seek_by_time_done && (wait_video_or_audio_sync & AVSYNC_NORMAL) && seek_sync_flag==0)
	ipc_callback_seek_by_time_done();

            if(dvd_last_chapter>0) {
                int cur_chapter = demuxer_get_current_chapter(mpctx->demuxer);
                if(cur_chapter!=-1 && cur_chapter+1>dvd_last_chapter)
                    goto goto_next_file;
            }

            //if(!mpctx->sh_audio && mpctx->d_audio->sh) {
            if(!mpctx->sh_audio && mpctx->d_audio->sh && !switch_audio_thread_status && !audio_not_support) {
                mpctx->sh_audio = mpctx->d_audio->sh;
                mpctx->sh_audio->ds = mpctx->d_audio;
                reinit_audio_chain();
                dvdnav_audio_need_uninit = 0;

				if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE){
		    if (not_supported_profile_level == 1)
		    {
			strcpy(callback_str,"status: playing 1\0");
			ipc_callback(callback_str);
			free_demuxer(mpctx->demuxer);
			mpctx->demuxer = NULL;
			goto goto_next_file;
		    }
		    else if ( (not_supported_profile_level == 2 || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
		    {
                        if (not_supported_profile_level == 3)
                        	strcpy(callback_str,"status: playing 3\0");
                        else
                        {
                        	if ( (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001)) || (!mpctx->sh_audio && no_audio_format == 0x2001) )
					strcpy(callback_str,"status: playing 2\0");
				else
					strcpy(callback_str,"status: playing 4\0");
                        }
		    }
		    else
			strcpy(callback_str,"status: playing 0\0");
		    ipc_callback(callback_str);

		    ipc_callback_audio_sub();
		}

                if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video && nosync==0 && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU && (is_first_loop == 1) ) 	//Fuchun 2010.07.20
                {
		    is_first_loop = 0;
#ifdef DVBT_USING_NORMAL_METHOD
// TODO, Need FuChun add new A-V sync method, for dvb-T
#ifdef QT_SUPPORT_DVBT
                    if (mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
					{
#ifdef SUPPORT_DIVX_DRM
						if (strncmp(video_names, "Xvid_Compatible", strlen("Xvid_Compatible") == 0))
							seek_sync_flag = 0;
						else
#endif /* end of SUPPORT_DIVX_DRM */				
							seek_sync_flag = 2;
						//printf("@@@@ In [%s][%d] set seek_sync_flag to [%d]@@@@\n", __func__, __LINE__, seek_sync_flag);
					}
                    else
#endif /* end of QT_SUPPORT_DVBT */					
                        seek_sync_flag = 3;
#else // else of DVBT_USING_NORMAL_METHOD
                    seek_sync_flag = 2;
#endif
                }
            }
	    if (is_first_loop == 1)
		is_first_loop = 0;

            /*========================== PLAY AUDIO ============================*/


//if (mpctx->sh_audio)
//if (mpctx->sh_audio && audio_id != -2)	//Fuchun 2009.12.02
//Robert 20101130 PAUSE command lock/unlock flag
            //if ((set_pause_lock==0) && mpctx->sh_audio && audio_id != -2)	//Fuchun 2009.12.02
            if ((set_pause_lock==0) && mpctx->sh_audio && audio_id != -2 && !switch_audio_thread_status)	//Fuchun 2009.12.02
            {
            	  last_audio_id = audio_id;
//printf("== dvbt_wait_sync=%d wait_video=%d  AV_delay=%f seek_sync_flag=%d\n", dvbt_wait_sync, wait_video_or_audio_sync, (float)AV_delay, seek_sync_flag);
#ifdef QT_SUPPORT_DVBT
                if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                {
//printf("wait_video_or_audio_sync=%d  dvbt_wait_sync=%d\n", wait_video_or_audio_sync, dvbt_wait_sync);
                    if(AV_delay > 1.0 && mpctx->stream->type == STREAMTYPE_STREAM)
                    {
                    }
                    else if((wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && AV_delay <= 0.1)	//Fuchun 2010.07.05
                    {
//                        printf("===== video catch up with audio!!\n");
                        printf("===== video catch up with audio!!  AV_delay[%f] apts:%f\n", AV_delay, (float)mpctx->d_audio->pts);
                        sky_set_display_lock(0);
                        mpctx->delay = mpctx->audio_out->get_delay();
                        c_total = mpctx->delay;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;
                        adjust_sync_and_print_status(0, mpctx->time_frame);
                    }
                    else if((wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO) && AV_delay >= -0.1)	//Fuchun 2010.07.20
                    {
                        printf("===== audio catch up with video!!\n");
                        sky_set_display_lock(0);
                        mpctx->delay = mpctx->audio_out->get_delay();
                        c_total = mpctx->delay;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;
                        adjust_sync_and_print_status(0, mpctx->time_frame);
                    }
                    else if(dvbt_wait_sync == 1 && AV_delay <= 0.1 /*&& AV_delay >= -0.1*/)
                    {
/*
			if (mpctx->sh_video && mpctx->d_video->packs < 10)
			{
//			    usleep(10000*20);
			        int cout = 0;
			        while(mpctx->d_audio->packs < 2 && (cout++ <6))
			        {
			            usleep(10000*10);
			            if (mpctx->d_audio->packs < 2)
			                demux_fill_buffer(mpctx->demuxer, mpctx->d_audio);
                                }
                        }
*/
                        printf("===== add audio playback!!\n");
                        mpctx->delay = mpctx->audio_out->get_delay();
//mpctx->delay = 0;
                        c_total = mpctx->delay;
                        dvbt_wait_sync = 0;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;
                        adjust_sync_and_print_status(0, mpctx->time_frame);
                    }
                    else if(dvbt_wait_sync == 2 && AV_delay >= -0.1 /*&& AV_delay <= 0.1*/)
                    {
                        printf("===== add video playback!!! \n");
                        mpctx->delay = mpctx->audio_out->get_delay();
                        c_total = mpctx->delay;
                        dvbt_wait_sync = 0;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;
                        adjust_sync_and_print_status(0, mpctx->time_frame);                        

                    }
                    else if(!(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && dvbt_wait_sync != 1)
                    {
//20100720 Robert let audio not so busy in SKYTS mode
//printf("== dvbt_wait_sync=%d wait_video_or_audio_sync=%d  AV_delay=%f\n", dvbt_wait_sync, wait_video_or_audio_sync, (float)AV_delay);
                        if (dvbt_wait_sync == 2 && AV_delay < -0.1)
                        {
//printf("== !!!\n");
                            fill_audio_out_buffers_with_resamp(0);
			adjust_sync_and_print_status(0, mpctx->time_frame);
//                            while (AV_delay < -0.05)
                            while (AV_delay < 0.05)
                            {
//printf("== !!! AV_delay=%f\n", (float)AV_delay);
                                fill_audio_out_buffers_with_resamp(0);
                                adjust_sync_and_print_status(0, mpctx->time_frame);
			    }
/*
			    if (mpctx->sh_video && mpctx->d_video->packs < 10)
			    {
			        int cout = 0;
			        while(mpctx->d_audio->packs < 2 && (cout++ <6))
			        {
			            usleep(10000*10);
			            if (mpctx->d_audio->packs < 2)
			                demux_fill_buffer(mpctx->demuxer, mpctx->d_audio);
                                }
                            }
*/
                            printf("===== add video playback!! \n");
//printf("=== set dvbt_wait_sync = 1\n");
                            mpctx->delay = mpctx->audio_out->get_delay();
                            dvbt_wait_sync = 0;
                            c_total = mpctx->delay;
//                            wait_video_or_audio_sync = AVSYNC_NORMAL;
                            adjust_sync_and_print_status(0, mpctx->time_frame);
                        }
                        else 
                        {
                            if (!fill_audio_out_buffers_with_resamp(0))
                            {
                            // at eof, all audio at least written to ao
                                if (!mpctx->sh_video)
                                    mpctx->eof = PT_NEXT_ENTRY;
                            }
                        }

                        /*
                        		if (!fill_audio_out_buffers_with_resamp(0))
                        		{
                        	// at eof, all audio at least written to ao
                        		  if (!mpctx->sh_video)
                        		      mpctx->eof = PT_NEXT_ENTRY;
                        		}
                        */
/*
#ifdef BITSTREAM_BUFFER_CONTROL
                        if (chk_bufspace(mpctx->demuxer) && last_audio_bytes_to_write < 16384)
#else
                        if (svsd_state->queue_length >= cvq_threshold && last_audio_bytes_to_write < 5000)
#endif
                        {
//			printf("Usleep:%s:%d\n", __FUNCTION__, __LINE__);
                            usleep(1000);
                        }
//#endif
*/
                    }
                }
                else
#endif /* end of QT_SUPPORT_DVBT */				
                {
//Robert 20100712 keep audio decoding in STREAMing mode, fix me later...
#if 1
                    if (wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO)
                    {
//printf("\n==> wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO AV_delay=%f\n", AV_delay);
                        fill_audio_out_buffers();
                    }

                    if(AV_delay > 1.0 && mpctx->stream->type == STREAMTYPE_STREAM)
                    {
                    }
                    else if((wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && AV_delay <= -0.05)//0.1)	//Fuchun 2010.07.05
                    {
#ifdef  jfueng_2011_0311

                        if (audio_long_period == 0)
                        {

		//printf("===== video catch up with audio!!  AV_delay[%f]\n", AV_delay);
                            printf("===== video catch up with audio!!  AV_delay[%f] apts:%f\n", AV_delay, (float)mpctx->d_audio->pts);
                            sky_set_display_lock(0);
		//Barry 2011-01-13
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
                            if ( (wait_video_or_audio_sync & AVSYNC_BEGINNING) && bluray_device && !seek_to_sec && quick_bd )
                                rel_seek_secs -= 10;
#endif
      		            mpctx->delay = 0;
        	            wait_video_or_audio_sync = AVSYNC_NORMAL;


        	        }
        	        else
        	        {


		            if (mpctx->d_audio->packs  > 0)
		            {   //JF 
		                printf("\nJF:===== video catch up with audio!!  AV_delay[%f]\n", AV_delay);
		
	        	        sky_set_display_lock(0);
		
	        	        mpctx->delay = mpctx->audio_out->get_delay();
	        	        wait_video_or_audio_sync = AVSYNC_NORMAL;
	                    }				

	                }


#else
                        printf("===== video catch up with audio!!  AV_delay[%f] apts:%f\n", AV_delay, (float)mpctx->d_audio->pts);
                        sky_set_display_lock(0);
                        //Barry 2011-01-13
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
                        if ( (wait_video_or_audio_sync & AVSYNC_BEGINNING) && bluray_device && !seek_to_sec && quick_bd )
                            rel_seek_secs -= 10;
#endif

                        mpctx->delay = 0;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;

#endif //JF

                    }
                    else if((wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO) && AV_delay >= -0.1)	//Fuchun 2010.07.20
                    {
                        printf("===== audio catch up with video!!  AV_delay[%f]\n", AV_delay);
                        if(AV_delay >= 0.1)
                            wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                        else
                        {
                            sky_set_display_lock(0);
                            //Barry 2011-01-13
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
                            if ( (wait_video_or_audio_sync & AVSYNC_BEGINNING) && bluray_device  && !seek_to_sec && quick_bd )
                                rel_seek_secs -= 10;
#endif

                            mpctx->delay = 0;
                            wait_video_or_audio_sync = AVSYNC_NORMAL;
                        }
                    }
                    #if 0 //Polun 2012-01-10 fixed mantis 6780 playerback youtobe eof handle 5s issue.
                    else if(!(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO))
                    #else
                    else if(!(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && mpctx->d_audio->eof == 0)
                    #endif    
#endif
                    {
                        if (!fill_audio_out_buffers())
                        {
                            // at eof, all audio at least written to ao
                            if (!mpctx->sh_video)
                                mpctx->eof = PT_NEXT_ENTRY;
                        }
                        else
                        {
                            if(!mpctx->sh_video && get_audio_pts_first_flag == 1 && written_audio_pts(mpctx->sh_audio, mpctx->d_audio) > 0.01)
                            {
                                get_audio_pts_first_flag = 0;
                                audio_start_pts = written_audio_pts(mpctx->sh_audio, mpctx->d_audio);
                                printf("@@@ audio_start_pts[%f] @@@\n", audio_start_pts);
                            }

                            if(audio_speed >= 3 || audio_speed < 0)
                                usleep(100000);

                            if(audio_speed < 0 && (written_audio_pts(mpctx->sh_audio, mpctx->d_audio) - audio_start_pts + audio_speed) < 0)
                            {
                                audio_speed = 0;
                                abs_seek_pos = 1;
                                rel_seek_secs = 0.0;
                                ipc_callback_audio_fast();
                            }
                        }
                    }
                }
            }
            if(!mpctx->sh_video) {
                // handle audio-only case:
                double a_pos=0;
                // sh_audio can be NULL due to video stream switching
                // TODO: handle this better
                //if(!quiet || end_at.type == END_AT_TIME ) //SkyMedi_Vincent09072009 mark: always evaluate current position of a_pos
                a_pos = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);

		if(!is_dvdnav)	//Fuchun 2011.04.25
                    my_current_pts = a_pos;

                if(!quiet)
                {
//Robert 20101217 reduce print_status cpu loading
                    static double last_nov_apts=0.0;
                    if (mpctx->d_audio && (last_nov_apts != a_pos))
                    {
                        print_status(a_pos, 0, 0);
                        last_nov_apts = a_pos;
                    }
                }

                if(end_at.type == END_AT_TIME && end_at.pos < a_pos)
                    mpctx->eof = PT_NEXT_ENTRY;
                update_subtitles(NULL, a_pos, mpctx->d_sub, 0);
                update_osd_msg();

//Robert 20100329 handle audio only case, prevent cpu busy
//  if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                {
//printf("Usleep:%s:%d\n", __FUNCTION__, __LINE__);
//    usec_sleep(1000);
                    usec_sleep(1001);
                }

            } else {
                /*========================== PLAY VIDEO ============================*/
                vo_pts=mpctx->sh_video->timer*90000.0;
                vo_fps=mpctx->sh_video->fps;

//Robert 20101130 PAUSE command lock/unlock flag
                if (set_pause_lock == 1)
                {
                    if ((mpctx->sh_audio && audio_id != -2 && (mpctx->audio_out && mpctx->audio_out->get_delay() <= 0.03)) || !mpctx->audio_out)
                        set_pause_lock = 2;
                }

		if(!is_dvdnav)	//Fuchun 2011.04.25
		{
			if(mpctx->sh_audio && mpctx->audio_out && !switch_audio_thread_status && audio_id != -2)
			{
				double a_pos = 0.0;
				a_pos = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
                            #if 0 //Polun 2011-11-22 fixed Alexander_Trailer_1080p.mpeg audio is end on 37 sec,can't playback video.
                            my_current_pts = a_pos;
                            #else
                            if(mpctx->demuxer->audio->eof != 1)
                                my_current_pts = a_pos;
                            else
                                my_current_pts = mpctx->sh_video->pts;
                            #endif
			}
			else
			{
				my_current_pts = mpctx->sh_video->pts;
			}
			if(mpctx->demuxer->type == DEMUXER_TYPE_ASF)
				my_current_pts -= correct_sub_pts;
		}
//printf("mpctx->sh_video->pts[%f]\n",mpctx->sh_video->pts);
#if 1
                int svsd_video_cnts = svsd_state->queue_length;
//Robert 20101004 need checking mplayer side counter
                svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);
                last_svsd_video_cnts = svsd_video_cnts;
//if (svsd_video_cnts < 3)
//printf("svsd_video_cnts=%d\n", svsd_video_cnts);
/*
{
vdec_svread_buf_t nrb;
svread(0, &nrb, 0);
if (nrb.qlen[2] == 0)
printf("=== [2]:[%d] r:%d w:%d svsd cnt:%d num_buffered_frames=%d buf=%d\n", nrb.qlen[2], nrb.ft_ridx, nrb.ft_widx, svsd_video_cnts, mpctx->num_buffered_frames, chk_bufspace(mpctx->demuxer));
}
*/
#ifdef QT_SUPPORT_DVBT
                if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS && mpctx->d_video->packs == 0 )
                {
                }
                else
#endif /* end of QT_SUPPORT_DVBT */				
#ifdef BITSTREAM_BUFFER_CONTROL
                    if (!mpctx->num_buffered_frames && /*!(wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO) &&*/ chk_bufspace(mpctx->demuxer))
#else
                    if (svsd_video_cnts <= cvq_threshold && !mpctx->num_buffered_frames /*&& !(wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO)*/)
#endif
#else
                if (!mpctx->num_buffered_frames && wait_video_or_audio_sync != 2)
#endif
                    {

                        double frame_time = update_video(&blit_frame);
//Robert 20100324 benchmark
                        if((speed_mult < 0 || speed_mult >= 2) && decode_num_cnt == 0)
                        {
#if 0
                            if (mpctx->demuxer->desc->type==DEMUXER_TYPE_AVI && is_mjpeg == 0)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->desc->type == DEMUXER_TYPE_AVI_NI && is_mjpeg == 0)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if (mpctx->demuxer->desc->type==DEMUXER_TYPE_MOV && is_mjpeg == 0)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_ASF)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_LAVF)
                                usleep(400000/(1 << (abs(speed_mult)-1)));
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_REAL)
                                usleep(600000/(1 << (abs(speed_mult)-1)));
                            else if (mpctx->demuxer->type == DEMUXER_TYPE_OGG)	//Barry 2010-11-25
                                usleep(200000/(1 << (abs(speed_mult)-1)));
#else
                            unsigned int sleep_time = 0;
                            unsigned int total_sleep_time = 0;
                            if((mpctx->demuxer->desc->type==DEMUXER_TYPE_AVI && is_mjpeg == 0)
                                    || (mpctx->demuxer->desc->type == DEMUXER_TYPE_AVI_NI && is_mjpeg == 0)
                                    || (mpctx->demuxer->desc->type==DEMUXER_TYPE_MOV && is_mjpeg == 0)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_ASF)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED)
                                    || (mpctx->demuxer->type == DEMUXER_TYPE_LAVF))
                            {
                                total_sleep_time = 400000/(1 << (abs(speed_mult)-1));
                            }
                            else if(mpctx->demuxer->type == DEMUXER_TYPE_REAL)
                            {
                                total_sleep_time = 600000/(1 << (abs(speed_mult)-1));
                            }
                            else if (mpctx->demuxer->type == DEMUXER_TYPE_OGG)
                            {
                                total_sleep_time = 200000/(1 << (abs(speed_mult)-1));
                            }

                            if(total_sleep_time > read_frame_time)
                            {
                                sleep_time = total_sleep_time - read_frame_time;
                                usleep(sleep_time);
                            }
#endif
                        }

                        if(decode_num_cnt) decode_num_cnt--;
/*
                        if(thumbnail_mode == 1)
                        {
                            int		ret;

                            sprintf(callback_str,"get_thumb ");
                            ret = get_thumbnail(callback_str);

                            if(ipcdomain)
                            {
                                printf("main():callback_str[%s]\n",callback_str);
                                ipc_callback(callback_str);
                            }
                            //printf("get_thumbnail = [%d]\n",ret);
                            break;
                        }
*/
//printf("     ##3  blit_frame=%d  startup=%d\n", blit_frame, mpctx->startup_decode_retry);
//      while (!blit_frame && mpctx->startup_decode_retry > 0)
                        if (mpctx->startup_decode_retry > 0)
                        {
                            //double need_to_catch_pts = mpctx->d_video->pts;
                            double need_to_catch_pts = mpctx->sh_video->pts;	//modify by Robert

//      int decoder_need_frame = -1, need_check_decoder_ref = 1, decoder_ref_cnts;
//Robert 20101230 don't need to check decoder ref
                            //int decoder_need_frame = -1, need_check_decoder_ref = 0, decoder_ref_cnts=0;	//Barry 2011-01-11 disable, use in A2
                            int decoder_ref_cnts=0;
                            int ft_idx_cnts;

#if 1
                            svread(0, &sky_nrb, 0);
//      printf("!!!!! decoder_need_frame = %d\n", decoder_need_frame);
                            ft_idx_cnts = (sky_nrb.ft_widx - sky_nrb.ft_ridx + 128)%128;

                            while(ft_idx_cnts > 1)
                            {
                                sky_set_sync_frame_ridx();
                                svread(0, &sky_nrb, 0);
                                ft_idx_cnts = (sky_nrb.ft_widx - sky_nrb.ft_ridx + 128)%128;
                            }
#endif
//printf("!!! #@@@@@# pts:%f r:%d w:%d qlen[2]=%d need_to_catch_pts=%f AV=%f \n", sky_nrb.cur_pts, sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_nrb.qlen[2], (float)need_to_catch_pts, (float)AV_delay);

                            while (mpctx->startup_decode_retry > 0) {
                                double delay = mpctx->delay;
                                if (need_to_catch_pts == MP_NOPTS_VALUE)
                                {
                                    need_to_catch_pts = 0.0;
                                }
                                else if (need_to_catch_pts == 0.0 && mpctx->sh_video->pts != MP_NOPTS_VALUE)
                                {
#ifdef QT_SUPPORT_DVBT
                                    if ((mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS && mpctx->sh_video->pts != 0.0))
#endif /* end of QT_SUPPORT_DVBT */									
                                        need_to_catch_pts = mpctx->sh_video->pts;
                                }

                                // these initial decode failures are probably due to codec delay,
                                // ignore them and also their probably nonsense durations
#if 1
                                svsd_video_cnts = svsd_state->queue_length;
                                int time_cnt=0;
#ifdef BITSTREAM_BUFFER_CONTROL
                                while (chk_bufspace(mpctx->demuxer) == 0)
#else
                                while (svsd_video_cnts > cvq_threshold)
#endif
                                {
                                    usleep(1000);
                                    svsd_video_cnts = svsd_state->queue_length;

                                    if(time_cnt++ > 100)
                                    {
                                        printf("-- svsd_video_cnts no change --\n");
                                        mpctx->startup_decode_retry = 0;
                                        break;
                                    }
                                }
#endif
                                {
                                    update_video(&blit_frame);

                                    mpctx->delay = delay;
                                    mpctx->startup_decode_retry--;

#if 1
#ifdef QT_SUPPORT_DVBT
                                    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                                    {
                                        need_to_catch_pts = AV_delay;	//keep last AV_delay
                                        if (sky_nrb.cur_pts > 0.0)
                                        {
                                            fill_audio_out_buffers_with_resamp(0);
                                        }
                                        adjust_sync_and_print_status(0, mpctx->time_frame);
                                    }

//printf(" #@@@@@# pts:%f r:%d w:%d qlen[2]=%d need_to_catch_pts=%f AV=%f \n", sky_nrb.cur_pts, sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_nrb.qlen[2], (float)need_to_catch_pts, (float)AV_delay);
//printf("seek_sync_flag=%d\n", seek_sync_flag);
//check again
                                    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                                    {
                                        if (sky_nrb.cur_pts > 0.0 && (DEFAULT_STARTUP_DECODE_RETRY - mpctx->startup_decode_retry < 20))
                                        {
                                            if (need_to_catch_pts <= 0 && (AV_delay > -0.5))
                                            {
                                                mpctx->startup_decode_retry = 0;
                                            }
                                            else if (need_to_catch_pts > 0 && AV_delay < 1 && AV_delay > -0.3)
                                            {
                                                mpctx->startup_decode_retry = 0;
                                            }
                                        }
                                    }
                                    else
#endif /* end of QT_SUPPORT_DVBT */									
                                    {
                                        if ((seek_sync_flag == 1 ? sky_nrb.cur_pts != 0.0 : 1) && sky_nrb.cur_pts >= need_to_catch_pts && ((sky_nrb.cur_pts - need_to_catch_pts <= 0.06)) )
                                        {
                                            mpctx->startup_decode_retry = 0;
                                        }
                                        else if ((seek_sync_flag == 1 ? sky_nrb.cur_pts != 0.0 : 1) && need_to_catch_pts >= sky_nrb.cur_pts && (need_to_catch_pts - sky_nrb.cur_pts <= 0.06))
                                        {
                                            mpctx->startup_decode_retry = 0;
                                        }
                                    }


//		  if (mpctx->startup_decode_retry == 0)
#ifdef QT_SUPPORT_DVBT
                                    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                                    {
                                        if(mpctx->startup_decode_retry == 0 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && dvbt_wait_sync == 0)	//Fuchun 2010.08.19
                                        {
                                            sky_set_display_lock(0);
                                        }
                                        
                                        while (mpctx->d_audio->packs < 5 && mpctx->d_video->packs < 5)
                                        {
                                            demux_fill_buffer(mpctx->demuxer, mpctx->d_video);
                                        }
                                    }
                                    else
#endif /* end of QT_SUPPORT_DVBT */									
                                    {
                                        if(mpctx->startup_decode_retry == 0 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && seek_sync_flag == 0)	//Fuchun 2010.08.19
                                        {
//			if (is_first_loop == 0)
                                            sky_set_display_lock(0);
                                        }
                                    }
#endif
                                }
				svread(0, &sky_nrb, 0);
                            }
                        }

                        if(dvdnav_return_menu)
                        {
                            int button = -1;
                            mp_dvdnav_handle_input(mpctx->stream,MP_CMD_DVDNAV_MENU,&button);
                            dvdnav_return_menu = 0;
                        }

                        mpctx->startup_decode_retry = 0;
                        mp_dbg(MSGT_AVSYNC,MSGL_DBG2,"*** ftime=%5.3f ***\n",frame_time);
                        if (mpctx->sh_video->vf_initialized < 0) {
                            mp_msg(MSGT_CPLAYER,MSGL_FATAL, MSGTR_NotInitializeVOPorVO);
                            mpctx->eof = 1;
                            if(ipcdomain) //using domain socket option
                                ipc_callback_error();
                            goto goto_next_file;
                        }
                        if (frame_time < 0)
                        {
                            mpctx->eof = 1;
                        }
                        else
                        {
                            // might return with !eof && !blit_frame if !correct_pts
                            if (mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
                            {
                                mpctx->num_buffered_frames += blit_frame;
                            }
                            mpctx->time_frame += frame_time / playback_speed;  // for nosound
                        }

                    }
                    else
                    {
//	if (speed_mult > 0 || (speed_mult == 0 && audio_id < 0))
                        if (speed_mult > 0 || (speed_mult == 0 && !mpctx->sh_audio))
                        {
//printf("Usleep:%s:%d\n", __FUNCTION__, __LINE__);
//  		usec_sleep(1000);	//1ms, avoid system too busy at FF 2x or 4x
                            usec_sleep(1002);	//1ms, avoid system too busy at FF 2x or 4x
                        }

//      printf("@@ no update_video svsd_cnts=%d num_buf_frames=%d wait_va_sync=%d\n",svsd_video_cnts, mpctx->num_buffered_frames, wait_video_or_audio_sync);
                    }
// ==========================================================================


                current_module="draw_osd";
                int draw_by_ass = 0;
                if (ass_enabled)
                {
                    sh_sub_t* sh = mpctx->d_sub->sh;
                    if (sh && (sh->type == 'a'))
                    {
                        draw_by_ass = 1;
                    } else {
                        draw_by_ass = 0;
                    }
                }

//    if(vo_config_count) mpctx->video_out->draw_osd();

#ifdef CONFIG_DVDNAV
                if(vo_need_dvdnav_menu == 0)
#endif /* CONFIG_DVDNAV */
                    if(speed_mult == 0 && FR_to_end == 0 && vo_need_osd == 1 && (draw_by_ass == 0))
                    {
                        //Fuchun 2010.03.25
                        if(mpctx->global_sub_size > 0 && mpctx->set_of_sub_pos >= 0)		//for external subtitle file
                        {
                            if(sub_visible)
                            {
                                mpctx->video_out->draw_osd();
                                sub_visible = 0;
                            }
                        }
#if 0
                        else if(pgs_subtitle_flag && vo_spudec)
                        {
                            if(pgs_subtitle_flag == 2)
                            {
                                pgs_subtitle_flag = 1;
                                mpctx->video_out->draw_osd();
                            }
                        }
#endif
                        else if(vo_sub != NULL && ((vo_sub->endpts[vo_sub->lines] != last_sub_pts) ||		//for the subtitle is a part of media file
                                                   (vo_sub->lines != last_sub_lines)) )
                        {
                            if (vo_sub->lines == 0)
                                clear_osd();
                            else
                                mpctx->video_out->draw_osd();
                            last_sub_pts = vo_sub->endpts[vo_sub->lines];
                            last_sub_lines = vo_sub->lines;
                        }
                        else if(mpctx->demuxer && mpctx->demuxer->teletext)
                        {
                            if (teletext_subtitle_flag == 1)
                            {
                                //clear_osd();  /* Marked this on 2011-01-17, fixed teletext is too slow to display */
                                mpctx->video_out->draw_osd();
                            }
                        }
			else if(vo_spudec != NULL)
                        {
                            if(spudec_visible(vo_spudec) && spu_get_pts(vo_spudec) != last_spu_pts)
                            {
                                mpctx->video_out->draw_osd();
                                last_spu_pts = spu_get_pts(vo_spudec);
                            }
                            else if(!spudec_visible(vo_spudec) && spu_get_pts(vo_spudec) == last_spu_pts)
                            {
//                              mpctx->video_out->draw_osd();
                                clear_osd();
                                last_spu_pts = -1;
                            }
                        }

                        //Fuchun 2010.03.24  need once draw osd
                        if(vo_need_osd == 1 && first_osd)
                        {
                            first_osd = 0;
                            mpctx->video_out->draw_osd();
                        }
                    }

#ifdef CONFIG_GUI
                if(use_gui) guiEventHandling();
#endif

                current_module="vo_check_events";
                if (vo_config_count) mpctx->video_out->check_events();

#ifdef CONFIG_X11
                if (stop_xscreensaver) {
                    current_module = "stop_xscreensaver";
                    xscreensaver_heartbeat();
                }
#endif
                if (heartbeat_cmd) {
                    static unsigned last_heartbeat;
                    unsigned now = GetTimerMS();
                    if (now - last_heartbeat > 30000) {
                        last_heartbeat = now;
                        system(heartbeat_cmd);
                    }
                }

//Robert 20100324 benchmark
#if 0
                if (mpctx->sh_video && video_id != -2 && mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS && mpctx->sh_video->format == 0x10000005)
                {
                    static unsigned int pre_time=0;
                    static float last_period=0.0;
                    float this_period=0.0;
                    unsigned int cur_time;
                    cur_time = GetTimer();
                    if (pre_time == 0)
                        pre_time = GetTimer();
                    else
                    {
                        struct timeval ttv;
                        gettimeofday(&ttv,NULL);
                        {
                            static int qqqq=0;
//  if (qqqq++ > 120)
                            if (qqqq++ > 480)
                            {
                                printf("--- %f    ", (float)(cur_time-pre_time)/10000);
                                printf("bf update GetTimer()=%d : %d   vq:%d  aq:%d  dvq:%d  daq:%d\n", ttv.tv_sec, ttv.tv_usec, dvb_get_vq_count(), dvb_get_aq_count(), mpctx->demuxer->video->packs, mpctx->demuxer->audio->packs);
                                qqqq=0;
                            }
                        }
                        pre_time = cur_time;
                    }
                }
#endif
                if(no_osd)
                {
                    //frame_time_remaining = sleep_until_update_original(&mpctx->time_frame, &aq_sleep_time);
                    //Polun 2011-08-08 ++s mantis 5678
                    if(upnp  && mpctx->d_video->eof == 1)
                    {
                        printf("@@@@@@@ upnp = %d  mpctx->eof = %d   do'nt need sleep_until_update_original \n",upnp, mpctx->d_video->eof );
                    }
                    else                       
                        frame_time_remaining = sleep_until_update_original(&mpctx->time_frame, &aq_sleep_time);
                    //Polun 2011-08-08 ++e mantis 5678  
                }
                //else if(((speed_mult == 0 && FR_to_end == 0) || is_mjpeg == 1) && (wait_video_or_audio_sync&AVSYNC_NORMAL) && check_framedrop_flag == 0)	//Fuchun 2010.05.03
                else if(((speed_mult == 0 && FR_to_end == 0) || is_mjpeg == 1) && (wait_video_or_audio_sync&AVSYNC_NORMAL) && check_framedrop_flag == 0 && !switch_audio_thread_status)	//Fuchun 2010.05.03
                {
                    frame_time_remaining = sleep_until_update(&mpctx->time_frame, &aq_sleep_time);
//                    frame_time_remaining = sleep_until_update_dvdnav(&mpctx->time_frame, &aq_sleep_time);

                }
                else
                    frame_time_remaining = 0;
//====================== FLIP PAGE (VIDEO BLT): =========================
                if (benchmark)
                {
                    current_module="flip_page";
#if 1
#ifdef BITSTREAM_BUFFER_CONTROL
//                    if (!frame_time_remaining && blit_frame && chk_bufspace(mpctx->demuxer))
                    if ((mpctx->num_buffered_frames > 0) && !frame_time_remaining && blit_frame)
#else
                    if (!frame_time_remaining && blit_frame && (svsd_video_cnts <= cvq_threshold))
#endif
#else
                    if (!frame_time_remaining && blit_frame)
#endif
                    {
                        unsigned int t2=GetTimer();

                        if(vo_config_count) mpctx->video_out->flip_page();
                        mpctx->num_buffered_frames--;

                        vout_time_usage += (GetTimer() - t2) * 0.000001;
                    }
                }
                else
                {
#ifdef BITSTREAM_BUFFER_CONTROL
//                    if (!frame_time_remaining && blit_frame && chk_bufspace(mpctx->demuxer))
                    if ((mpctx->num_buffered_frames > 0) && !frame_time_remaining && blit_frame)
#else
                    if (!frame_time_remaining && blit_frame && (svsd_video_cnts <= cvq_threshold))
#endif
                    {
                        mpctx->num_buffered_frames--;
                    }
                }

                if (mpctx->num_buffered_frames < 0)
                    mpctx->num_buffered_frames = 0;
//====================== A-V TIMESTAMP CORRECTION: =========================

		if (nosync == 0)
			adjust_sync_and_print_status(frame_time_remaining, mpctx->time_frame);
		else
		{
			if(!quiet)
			{
				if(mpctx->sh_audio && !switch_audio_thread_status)
				{
					//Robert 20101217 reduce print_status cpu loading
					static double last_vpts=0.0, last_apts=0.0;
					if (mpctx->d_video && (last_vpts != mpctx->d_video->pts || last_apts != mpctx->d_audio->pts))
					{
						float a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
						float v_pts;
						svread(0, &sky_nrb, 0);
						v_pts = sky_nrb.cur_pts;
						
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
						if ( (vdec_init2_info.pulldown32 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && !seek_sync_flag)
							|| (quick_bd && mpctx->sh_video->format != 0x10000005)	//Barry 2011-01-14
							|| (is_dvdnav))
#else
						if(vdec_init2_info.pulldown32 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && !seek_sync_flag)
#endif
                                                {
							AV_delay = a_pts - audio_delay - mpctx->sh_video->pts;
							sky_avsync_method = 0;
                                                }
						else if(sky_vdec_wait_sync_vpts != 0.0)
						{
							AV_delay = a_pts - audio_delay - sky_vdec_wait_sync_vpts;
							sky_avsync_method = 1;
                                                }
						else
						{
							AV_delay = a_pts - audio_delay - v_pts;
							sky_avsync_method = 2;
                                                }
						
						print_status(a_pts - audio_delay, AV_delay, c_total);
						last_vpts = mpctx->d_video->pts;
						last_apts = mpctx->d_audio->pts;
					}
				}
				else
				{
					//Robert 20101217 reduce print_status cpu loading
					static double last_noa_vpts=0.0;
					if (mpctx->d_video && last_noa_vpts != mpctx->d_video->pts )
					{
						print_status(0, 0, 0);
						last_noa_vpts = mpctx->d_video->pts;
					}
				}
			}
		}
		

		//Fuchun 2011.01.07 add timeout for av-sync
		if(seek_sync_flag == 1 || seek_sync_flag == 2)
		{
			if(avsync_timeout_cnt > avsync_timeout_num)
			{
				seek_sync_flag = 0;
				avsync_timeout_cnt = 0;
				if(AV_delay > 0)
					wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
			}
			if(!mpctx->num_buffered_frames && (AV_delay < -10 || AV_delay > 10))
				avsync_timeout_cnt++;
		}
		else if(avsync_timeout_cnt)
			avsync_timeout_cnt = 0;
//============================ Auto QUALITY ============================

                /*Output quality adjustments:*/
                if(auto_quality>0) {
                    current_module="autoq";
//  float total=0.000001f * (GetTimer()-aq_total_time);
//  if(output_quality<auto_quality && aq_sleep_time>0.05f*total)
                    if(output_quality<auto_quality && aq_sleep_time>0)
                        ++output_quality;
                    else
//  if(output_quality>0 && aq_sleep_time<-0.05f*total)
                        if(output_quality>1 && aq_sleep_time<0)
                            --output_quality;
                        else if(output_quality>0 && aq_sleep_time<-0.050f) // 50ms
                            output_quality=0;
//  printf("total: %8.6f  sleep: %8.6f  q: %d\n",(0.000001f*aq_total_time),aq_sleep_time,output_quality);
                    set_video_quality(mpctx->sh_video,output_quality);
                }

                if (play_n_frames >= 0 && !frame_time_remaining && blit_frame) {
                    --play_n_frames;
                    if (play_n_frames <= 0) mpctx->eof = PT_NEXT_ENTRY;
                }


// FIXME: add size based support for -endpos
                if (end_at.type == END_AT_TIME &&
                        !frame_time_remaining && end_at.pos <= mpctx->sh_video->pts)
                    mpctx->eof = PT_NEXT_ENTRY;

#if 1	//Fuchun 2010.05.19
                if(mpctx->eof == 1 && speed_mult == 0 && mpctx->sh_audio)
                {
                    if(mpctx->d_audio->eof == 0 && mpctx->d_video->eof == 1)
                    {
                        mpctx->eof = 0;

                        {
                            initialized_flags&=~INITIALIZED_VCODEC;
                            seek_sync_flag = 0;
                            wait_video_or_audio_sync = AVSYNC_NORMAL;
                            current_module="uninit_vcodec";
                            if(mpctx->sh_video) uninit_video(mpctx->sh_video);
                            mpctx->sh_video=NULL;
#ifdef CONFIG_MENU
                            vf_menu=NULL;
#endif
                        }
                    }
                }
#endif
            } // end if(mpctx->sh_video)

#if 1	//Barry 2011-04-20
            if (mpctx->sh_video && !no_osd)
            {
            	if (mpctx->stream->type != STREAMTYPE_DVDNAV)	//Barry 2011-05-07
            	{
	            	if (correct_sub_pts && (mpctx->sh_video->pts - correct_sub_pts > 0.8))
	            		ipc_callback_scan_type();
			else if (mpctx->sh_video->pts > 0.8)
				ipc_callback_scan_type();
            	}
            }
#endif

#ifdef CONFIG_DVDNAV
            if (mpctx->stream->type == STREAMTYPE_DVDNAV) {

                if(dvdnav_color_spu==0) {
                    nav_highlight_t hl;
                    mp_dvdnav_get_highlight (mpctx->stream, &hl);
                    osd_set_nav_box (hl.sx, hl.sy, hl.ex, hl.ey);
                    vo_osd_changed (OSDTYPE_DVDNAV);
                }
#if 0
                else
                {
                    if (mp_dvdnav_is_stream_change()) // is cell change (title, part, audio, subtitle)
                    {

                        double ar=-1.0;
                        if(stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
                            mpctx->sh_video->stream_aspect = ar;
                        mp_dvdnav_audio_handle();	// call audio process
                        mp_dvdnav_spu_handle();		// call subtitle process
#ifdef HAVE_NEW_GUI
                        if ( use_gui ) guiGetEvent( guiSetStream,(char *)mpctx->stream );	// update gui dvdnav stream info
#endif
                        mp_dvdnav_highlight_handle(1);	// update dvdnav menu //◎◎note◎◎:switch menu page. stream_change=1
                    } else
                        mp_dvdnav_highlight_handle(0);
                }
#endif //#if 0

                if (mp_dvdnav_stream_has_changed(mpctx->stream))
                {
                    double ar = -1.0;
                    if (stream_control (mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
                    {
                        mpctx->sh_video->stream_aspect = ar;
                    }

                    if(dvdnav_color_spu==1 && (dvdnavstate.new_aid_map >= 0))
                    {
                        //mp_dvdnav_audio_handle();	// call audio process
                        if(dvdnavstate.aid[dvdnavstate.new_aid_map] != mpctx->demuxer->audio->id)
                            mpctx->demuxer->audio->id = dvdnavstate.aid[dvdnavstate.new_aid_map];

                        mp_dvdnav_spu_handle();    // call subtitle process
                        mp_dvdnav_highlight_handle(1);
                    }
                }
                else if(dvdnav_color_spu==1)
                    mp_dvdnav_highlight_handle(0);
            }
#endif

//============================ Handle PAUSE ===============================

            current_module="pause";

//  if (mpctx->osd_function == OSD_PAUSE) {
//Robert 20101130 PAUSE command lock/unlock flag
            if (((set_pause_lock == 0) && mpctx->osd_function == OSD_PAUSE) || (set_pause_lock == 2)) {
                mpctx->was_paused = 1;
                set_pause_lock = 0;
// WT, 100714, DRM pause event
#ifdef CONFIG_DRM_ENABLE
                if (mpctx->demuxer->drmFileOpened)
                {
                    int er;
                    er = DxDrmStream_HandleConsumptionEvent(mpctx->demuxer->drmStream, DX_EVENT_PAUSE);
                    if (er != DX_SUCCESS)
                        printf("DX:ERROR - pause event failed\n");
                    else
                        printf("DX:DRM pause\n");
                }
#endif
                pause_loop();
//+SkyMedi_Vincent03232010
//	if(ipcdomain && !thumbnail_mode) //using domain socket option
                if(ipcdomain && !(domainsocket_cmd && domainsocket_cmd->id == MP_CMD_FRAME_STEP)) //using domain socket option
                {
                    char callback_str[42];
                    is_pause = 0;

#ifdef HAS_DEC_CAPABILITY
                    if (!mpctx->eof && (skydroid || (mpctx->sh_audio && !mpctx->sh_video) || (mpctx->sh_video && svsd_state->dec_cap.dolby && svsd_state->dec_cap.dts)))	//Barry 2011-09-05
#else
                    if (!mpctx->eof)
#endif
                    {
	                    //Barry 2010-11-11
	                    if (not_supported_profile_level == 1)
	                    {
	                        strcpy(callback_str,"status: playing 1\0");
	                        ipc_callback(callback_str);
	                        free_demuxer(mpctx->demuxer);
	                        mpctx->demuxer = NULL;
	                        goto goto_next_file;
	                    }
			    else if ( (not_supported_profile_level == 2 || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
	                    {
	                        if (not_supported_profile_level == 3)
	                        	strcpy(callback_str,"status: playing 3\0");
	                        else
	                        {
	                        	if ( (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001)) || (!mpctx->sh_audio && no_audio_format == 0x2001) )
						strcpy(callback_str,"status: playing 2\0");
					else
						strcpy(callback_str,"status: playing 4\0");
					sprintf(audio_names, "AUDIO: No Audio can switch\0");
					ipc_callback(audio_names);
	                        }
	                    }
	                    else
	                        strcpy(callback_str,"status: playing 0\0");
	                    ipc_callback(callback_str);

	                    ipc_callback_audio_sub();
                    }
                }
#if 0
#ifdef HW_TS_DEMUX
                if (hwtsdemux || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS))
                    is_pause = 0;
#endif // end of HW_TS_DEMUX
#endif
//SkyMedi_Vincent03232010+
            }

// handle -sstep
            if(step_sec>0) {
                mpctx->osd_function=OSD_FFW;
                rel_seek_secs+=step_sec;
            }

            edl_update(mpctx);

#ifdef QT_SUPPORT_DVBT
#ifdef HAS_DVBIN_SUPPORT
//================= DVB Channel Reset ====================
            if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
#ifdef DVBT_USING_NORMAL_METHOD
                if ( (get_skydvb_wrokaround()== NEED_SEEK_CHANNEL)  || ( (video_id != -2) && (mpctx->sh_video && ((mpctx->demuxer->video->packs+dvb_get_vq_count())>((int)(mpctx->sh_video->fps) * 4 )) || ((mpctx->demuxer->audio->packs+dvb_get_aq_count())>50))  ) )
#else // else of DVBT_USING_NORMAL_METHOD
                if(get_skydvb_wrokaround()== NEED_SEEK_CHANNEL || ((video_id != -2)&& ( ((mpctx->demuxer->video->packs+dvb_get_vq_count())>((int)(mpctx->sh_video->fps) * 4 )) || (mpctx->demuxer->audio->packs+dvb_get_aq_count())>300) ) )
#endif // end of DVBT_USING_NORMAL_METHOD
                {
                    dvb_priv_t *priv = (dvb_priv_t*) mpctx->stream->priv;
                    printf("#### We received skydvb_need_workaround[%d] is_on [%d]### in [%s][%d] \n", get_skydvb_wrokaround(), priv->is_on, __func__, __LINE__);
#if 1	//Barry 2010-08-19 use seek function instead of dvb reopen
                    if (!h264_frame_mbs_only)	//Barry 2010-08-23 A3 need to check again (disable ??)
                    {
                        if(priv->is_on)
                        {
                            int dir;
                            //int v = mpctx->last_dvb_step;

                            dir = 0;
                            if(dvb_step_channel(mpctx->stream, dir))
                                mpctx->eof = mpctx->dvbin_reopen = 1;
                        }
                    }
                    else
                    {

#if 0	//Robert 20101118 not yet support nrb.timeout
                        sky_nrb.timeout_current = 6;
#endif
                        rel_seek_secs += 1;
                    }
                    /* We move demux_flush from libmpdemux/demux_skyts.c to here */
                    demux_flush(mpctx->demuxer);
#else
                    dvb_priv_t *priv = (dvb_priv_t*) mpctx->stream->priv;
                    if(priv->is_on)
                    {
                        int dir;
                        int v = mpctx->last_dvb_step;


                        dir = 0;
                        if(dvb_step_channel(mpctx->stream, dir))
                            mpctx->eof = mpctx->dvbin_reopen = 1;
                    }
#endif
                    set_skydvb_wrokaround(INIT_STATUS);
                }
#ifdef DVBT_USING_NORMAL_METHOD
                else if (get_skydvb_wrokaround() & LOST_DVBT_SIGNAL)
                {
                    dvb_priv_t *priv = (dvb_priv_t*) mpctx->stream->priv;
                    if(priv->is_on)
                    {
                        int dir;
                        dir = 0;
                        if(dvb_step_channel(mpctx->stream, dir))
                            mpctx->eof = mpctx->dvbin_reopen = 1;
                        printf("#### We received skydvb_need_workaround[%d] is_on [%d]### in [%s][%d] \n", get_skydvb_wrokaround(), priv->is_on, __func__, __LINE__);
                    }
                    set_skydvb_wrokaround(INIT_STATUS);
                }
                else
                {
                }
#endif // end of DVBT_USING_NORMAL_METHOD	  
            }

#endif
#endif /* end of QT_SUPPORT_DVBT */

//================= Keyboard events, SEEKing ====================
#ifdef ENABLE_KEY_EVENT_FUNCTION_MODE
            if (parse_and_run_command() == 2)
                goto goto_enable_cache;
#else  // else of ENABLE_KEY_EVENT_FUNCTION_MODE
            current_module="key_events";

            {
                mp_cmd_t* cmd;
                int brk_cmd = 0;

//Robert 20101130 PAUSE command lock/unlock flag, bypass all other command while pre_pause status
                if (set_pause_lock == 0)
                {
                    while( !brk_cmd && (cmd = mp_input_get_cmd(0,0,0)) != NULL || domainsocket_cmd) {
                        if(domainsocket_cmd)
                            cmd = domainsocket_cmd;
                        brk_cmd = run_command(mpctx, cmd);

#if 1	//Fuchun 2009.12.02
                        if(cmd->pausing  && cmd->pausing != 4)
                        {
                            FFFR_to_normalspeed(cmd);
                        }
#endif

                        if(!domainsocket_cmd)
                            mp_cmd_free(cmd);
                        else
                            domainsocket_cmd = NULL;
                        if (brk_cmd == 2)
                            goto goto_enable_cache;
                    }
                }
            }
#endif // end of ENABLE_KEY_EVENT_FUNCTION_MODE

//Robert 20101130 PAUSE command lock/unlock flag
            if (mpctx->osd_function == OSD_PAUSE)
            {
                if (mpctx->sh_video && mpctx->sh_audio)
                {
                    unsigned char ao_buf_256[256];
                    memset(ao_buf_256, 0, 256);
                    set_pause_lock = 1;
                    if (mpctx->audio_out && mpctx->audio_out->play)
                    {
                        mpctx->audio_out->play(ao_buf_256, 256, 0);
                    }
                }
            }

            if(FR_to_end == 1)
            {
                speed_mult = 0;
                FR_to_end = 0;
                last_rewind_pts = 0.0;

                audio_id = rel_audio_id;
                rel_audio_id = -5;
                rel_seek_secs -= 0.1;

                if(mpctx->global_sub_size > 0)
                {
                    rel_global_sub_pos = -5;

                    if(vo_spudec != NULL)
                    {
                        last_spu_pts = 0;
                    }
                }

                ipc_callback_fast();
				
			if(mpctx->audio_out)
                		mpctx->audio_out->reset();
            }

            pthread_mutex_lock(&mut); //for waiting domain socket to switch audio language
            pthread_mutex_unlock(&mut);

            //Fuchun 2010.04.30
            if (speed_mult != 0 && mpeg_fast== 1)
            {
                if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS) {
                    if (speed_mult < 0)
                        rel_seek_secs += -2.5*rewind_mult;
                    else if (speed_mult >= 2)
                        rel_seek_secs += 1.0;
                }

                if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES) {
                    if (speed_mult < 0)
                        rel_seek_secs += -1.0*rewind_mult;
                    else if (speed_mult >= 2)
                        rel_seek_secs += 1.0;
                }

                if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) {
                    if (speed_mult < 0)
#if 1
                    {
                        float multiple = 1.0;
                        int k;
                        for(k=0; k<((-1)*speed_mult-1); k++)
                            multiple *= 2.0;

                        rel_seek_secs += -2.5*rewind_mult*multiple;
                    }
#else
                        rel_seek_secs += -2.5*rewind_mult;
#endif
                    else if (speed_mult >= 2)
#if 1
                    {
                        float multiple = 1.0;
                        int k;
                        for(k=0; k<(speed_mult-2); k++)
                            multiple *= 2.0;

                        rel_seek_secs += multiple;
                    }
#else
                        rel_seek_secs += 1.0;
#endif
                }

                if (mpctx->stream->type == STREAMTYPE_DVDNAV) {
                    if (speed_mult < 0)
                        rel_seek_secs = 1; //special case, don't let newpos become to negative
                    else if (speed_mult >= 2)
                        rel_seek_secs += 1.0;
                }

                mpeg_fast = 0;
            }
            else if(is_mjpeg == 0 && speed_mult >= 2 && read_nextframe == 1 		//for mkv, flv, real, raw use
                    && mpctx->demuxer->type != DEMUXER_TYPE_ASF
                    && mpctx->demuxer->type != DEMUXER_TYPE_MOV
                    && mpctx->demuxer->type != DEMUXER_TYPE_AVI
                    && mpctx->demuxer->type != DEMUXER_TYPE_AVI_NI
                    && mpctx->demuxer->type != DEMUXER_TYPE_MPEG_PS
                    && mpctx->demuxer->type != DEMUXER_TYPE_MPEG_ES
                    && mpctx->demuxer->type != DEMUXER_TYPE_MPEG_TS
                   )
            {
                rel_seek_secs += 1.0;
            }

            //Barry 2010-11-25
            if (mpctx->sh_video && mpctx->demuxer->type == DEMUXER_TYPE_OGG && speed_mult < 0)
            {
                if (mpctx->sh_video->pts <= 0)
                    FR_to_end = 1;
                else
                    rel_seek_secs -= 2.0;
            }

            if(audio_speed != 0)
            {
                float time_interval = 1.0;
                if(audio_speed >= 3)
                {
                    if(audio_speed == 3)		//4x
                        time_interval = 4.0;
                    else if(audio_speed == 4)		//8x
                        time_interval = 8.0;
                    else if(audio_speed == 5)		//16x
                        time_interval = 16.0;
                    else if(audio_speed == 6)		//32x
                        time_interval = 32.0;
                    rel_seek_secs += (float)(time_interval);
                }
                else if(audio_speed < 0)
                {
                    if(audio_speed == -1)	//-1.5x
                        time_interval = -2.0;
                    else if(audio_speed == -2)	//-2x
                        time_interval = -3.0;
                    else if(audio_speed == -3)	//-4x
                        time_interval = -4.0;
                    else if(audio_speed == -4)	//-8x
                        time_interval = -8.0;
                    else if(audio_speed == -5)	//-16x
                        time_interval = -16.0;
                    else if(audio_speed == -6)	//-32x
                        time_interval = -32.0;
                    rel_seek_secs += (float)(time_interval);
                }
            }

            mpctx->was_paused = 0;

            /* Looping. */
            if(mpctx->eof==1 && mpctx->loop_times>=0) {
                mp_msg(MSGT_CPLAYER,MSGL_V,"loop_times = %d, eof = %d\n", mpctx->loop_times,mpctx->eof);

                if(mpctx->loop_times>1) mpctx->loop_times--;
                else if(mpctx->loop_times==1) mpctx->loop_times=-1;
                play_n_frames=play_n_frames_mf;
                mpctx->eof=0;
                abs_seek_pos=SEEK_ABSOLUTE;
                rel_seek_secs=seek_to_sec;
                loop_seek = 1;
            }

            if(rel_seek_secs || abs_seek_pos) {
                if (seek(mpctx, rel_seek_secs, abs_seek_pos) >= 0) {
                    // Set OSD:
                    if(!loop_seek) {
                        if( !edl_decision )
                            set_osd_bar(0,"Position",0,100,demuxer_get_percent_pos(mpctx->demuxer));
                    }
                    if(is_dvdnav)
                    {
                    	if(dvdnav_seek_result == 1)
                    	{
                    		//Skyviia_Vincent02222011:usavich_season1.iso seek to end can't return to menu
                    		FFFR_to_normalspeed(NULL);
                    		dvdnav_seek_result = 0;
                    	}
                    }
                }

                rel_seek_secs=0;
                abs_seek_pos=0;
                loop_seek=0;
                edl_decision = 0;
            }

            if(speed_mult != 0) mpctx->num_buffered_frames = 0;	//Fuchun 2010.04.30
            if(check_framedrop_flag == 1)
            {
                mpctx->delay = mpctx->audio_out->get_delay();
                check_framedrop_flag = 0;
            }

#ifdef CONFIG_GUI
            if(use_gui) {
                guiEventHandling();
                if(mpctx->demuxer->file_format==DEMUXER_TYPE_AVI && mpctx->sh_video && mpctx->sh_video->video.dwLength>2) {
                    // get pos from frame number / total frames
                    guiIntfStruct.Position=(float)mpctx->d_video->pack_no*100.0f/mpctx->sh_video->video.dwLength;
                } else {
                    guiIntfStruct.Position=demuxer_get_percent_pos(mpctx->demuxer);
                }
                if ( mpctx->sh_video ) guiIntfStruct.TimeSec=mpctx->sh_video->pts;
                else if ( mpctx->sh_audio ) guiIntfStruct.TimeSec=playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
                guiIntfStruct.LengthInSec=demuxer_get_time_length(mpctx->demuxer);
                guiGetEvent( guiReDraw,NULL );
                guiGetEvent( guiSetVolume,NULL );
                if(guiIntfStruct.Playing==0) break; // STOP
                if(guiIntfStruct.Playing==2) mpctx->osd_function=OSD_PAUSE;
                if ( guiIntfStruct.DiskChanged || guiIntfStruct.NewPlay ) goto goto_next_file;
#ifdef CONFIG_DVDREAD
                if ( mpctx->stream->type == STREAMTYPE_DVD )
                {
                    dvd_priv_t * dvdp = mpctx->stream->priv;
                    guiIntfStruct.DVD.current_chapter=dvd_chapter_from_cell(dvdp,guiIntfStruct.DVD.current_title-1, dvdp->cur_cell)+1;
                }
#endif
            }
#endif /* CONFIG_GUI */

            if(ipcdomain)
            {
                duration_sec_cur = demuxer_get_time_length(mpctx->demuxer);
                if(duration_sec_old != duration_sec_cur)
                {
                    ipc_callback_duration();
                    duration_sec_old = duration_sec_cur;
                }
            }
        } // while(!mpctx->eof)
    }

        entry_while = 0;
        printf("mpctx->eof[%d]\n",mpctx->eof);

#ifdef  jfueng_2011_0311

            //printf("\n JF::  audio END.....\n");
	     audio_long_period = 0;	   
           a_long_quiet_SP_yes =0;
           a_long_quiet_SP_cnt=0;


#endif


        if(ipcdomain && (mpctx->eof==1 || mpctx->eof==4) && !force_quit)
        {
            printf("MPLAYER: mpctx->eof=%d  force_quit=%d\n", mpctx->eof, force_quit);
#ifdef QT_SUPPORT_DVBT			
#ifdef CONFIG_DVBIN
            if(mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
//don't stop
            }
            else
#endif /* end of CONFIG_DVBIN */
#endif /* end of QT_SUPPORT_DVBT */
            {
                ipc_callback_stop();
            }

        }

        mp_msg(MSGT_GLOBAL,MSGL_V,"EOF code: %d  \n",mpctx->eof);

goto_next_file:  // don't jump here after ao/vo/getch initialization!

#ifdef CONFIG_DVBIN
        if(mpctx->dvbin_reopen)
        {
            mpctx->eof = 0;
#ifdef DVBT_USING_NORMAL_METHOD
            if(mpctx->demuxer)
            {
                if (mpctx->demuxer->desc && mpctx->demuxer->desc->close)
                    mpctx->demuxer->desc->close(mpctx->demuxer);
            }
            uninit_player(INITIALIZED_ALL-(INITIALIZED_GUI|INITIALIZED_STREAM|INITIALIZED_INPUT|INITIALIZED_GETCH2|(fixed_vo?INITIALIZED_VO:0)|INITIALIZED_STREAM | INITIALIZED_DEMUXER));

            {
                mpctx->d_audio=mpctx->demuxer->audio;
                mpctx->d_video=mpctx->demuxer->video;
                mpctx->d_sub=mpctx->demuxer->sub;
                /* Carlos 2010-10-13 add, fixed next audio pts locked when this time init  audio decoder failed */
                if (mpctx->d_audio->sh == NULL)
                    mpctx->d_audio->sh = mpctx->demuxer->a_streams[0];
                if (mpctx->d_video->sh == NULL)
                    mpctx->d_video->sh = mpctx->demuxer->v_streams[0];
                mpctx->sh_audio = mpctx->d_audio->sh ;
                mpctx->sh_audio->pts = MP_NOPTS_VALUE;
                mpctx->sh_video = mpctx->d_video->sh ;
                mpctx->sh_video->pts = MP_NOPTS_VALUE;

                mpctx->demuxer->desc->open(mpctx->demuxer);
            }
#else // else of DVBT_USING_NORMAL_METHOD
            uninit_player(INITIALIZED_ALL-(INITIALIZED_GUI|INITIALIZED_STREAM|INITIALIZED_INPUT|INITIALIZED_GETCH2|(fixed_vo?INITIALIZED_VO:0)));
            cache_uninit(mpctx->stream);
#endif // end of DVBT_USING_NORMAL_METHOD
            mpctx->dvbin_reopen = 0;

//Robert 20110328 do shm init in svsd_ipc_init_socket
            svsd_ipc_init_socket();
            /*#endif*/
#ifdef DVBT_USING_NORMAL_METHOD
            if (parse_and_run_command() == 2)
                goto goto_enable_cache;
            else
                goto dvb_change_channel;
#else // else of DVBT_USING_NORMAL_METHOD
            goto goto_enable_cache;
#endif // end of DVBT_USING_NORMAL_METHOD
        }
#endif
    }

    /*
    goto_next_file________________()
    */
//goto_next_file:  // don't jump here after ao/vo/getch initialization!

    dts_packet = 0;
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"\n");

    if(benchmark)
    {
#if 1	// Raymond 2009/07/22
        double tot=video_time_usage+video_read_time_usage+audio_time_usage;
        double total_time_usage;
        total_time_usage_start = GetTimer()-total_time_usage_start;
        total_time_usage = (float)total_time_usage_start*0.000001;

        mp_msg(MSGT_CPLAYER,MSGL_INFO,"\nBENCHMARKs: VC:%8.3fs VR:%8.3fs A:%8.3fs Sys:%8.3fs = %8.3fs\n",
               video_time_usage,video_read_time_usage,audio_time_usage,
               total_time_usage-tot,total_time_usage);

        if(total_time_usage>0.0)
            mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARK%%: VC:%8.4f%% VR:%8.4f%% A:%8.4f%% Sys:%8.4f%% = %8.4f%%\n",
                   100.0*video_time_usage/total_time_usage,
                   100.0*video_read_time_usage/total_time_usage,
                   100.0*audio_time_usage/total_time_usage,
                   100.0*(total_time_usage-tot)/total_time_usage,
                   100.0);

        //  if(total_frame_cnt && frame_dropping)
        if(total_frame_cnt)	// Raymond 2009/04/27
            mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARKn: disp: %d (%3.2f fps)  drop: %d (%d%%)  total: %d (%3.2f fps)\n",
                   total_frame_cnt-drop_frame_cnt,
                   (total_time_usage>0.5)?((total_frame_cnt-drop_frame_cnt)/total_time_usage):0,
                   drop_frame_cnt,
                   100*drop_frame_cnt/total_frame_cnt,
                   total_frame_cnt,
                   (total_time_usage>0.5)?(total_frame_cnt/total_time_usage):0);
#else
        double tot=video_time_usage+vout_time_usage+audio_time_usage;
        double total_time_usage;
        total_time_usage_start=GetTimer()-total_time_usage_start;
        total_time_usage = (float)total_time_usage_start*0.000001;
        mp_msg(MSGT_CPLAYER,MSGL_INFO,"\nBENCHMARKs: VC:%8.3fs VO:%8.3fs A:%8.3fs Sys:%8.3fs = %8.3fs\n",
               video_time_usage,vout_time_usage,audio_time_usage,
               total_time_usage-tot,total_time_usage);
        if(total_time_usage>0.0)
            mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARK%%: VC:%8.4f%% VO:%8.4f%% A:%8.4f%% Sys:%8.4f%% = %8.4f%%\n",
                   100.0*video_time_usage/total_time_usage,
                   100.0*vout_time_usage/total_time_usage,
                   100.0*audio_time_usage/total_time_usage,
                   100.0*(total_time_usage-tot)/total_time_usage,
                   100.0);
        if(total_frame_cnt && frame_dropping)
            mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARKn: disp: %d (%3.2f fps)  drop: %d (%d%%)  total: %d (%3.2f fps)\n",
                   total_frame_cnt-drop_frame_cnt,
                   (total_time_usage>0.5)?((total_frame_cnt-drop_frame_cnt)/total_time_usage):0,
                   drop_frame_cnt,
                   100*drop_frame_cnt/total_frame_cnt,
                   total_frame_cnt,
                   (total_time_usage>0.5)?(total_frame_cnt/total_time_usage):0);
#endif
    }

// time to uninit all, except global stuff:
    uninit_player(INITIALIZED_ALL-(INITIALIZED_GUI+INITIALIZED_INPUT+(fixed_vo?INITIALIZED_VO:0)));

    if(mpctx->set_of_sub_size > 0) {
        current_module="sub_free";
        for(i = 0; i < mpctx->set_of_sub_size; ++i) {
            sub_free(mpctx->set_of_subtitles[i]);
#ifdef CONFIG_ASS
            if(mpctx->set_of_ass_tracks[i])
                ass_free_track( mpctx->set_of_ass_tracks[i] );
#endif
        }
        mpctx->set_of_sub_size = 0;
    }
    vo_sub_last = vo_sub=NULL;
    subdata=NULL;
#ifdef CONFIG_ASS
    ass_track = NULL;
    if(ass_library)
        ass_clear_fonts(ass_library);
#endif

//#ifdef _SKY_VDEC_V1 /*johnnyke 20100909*/
#if 1
    if (set_disp_off == 1)
    {
    	set_disp_off = 0;
    	skyfb_set_display_status(1); // Carlos add, move this function from uninit video codec
    }
#endif
    if(mpctx->eof == PT_NEXT_ENTRY || mpctx->eof == PT_PREV_ENTRY) {
        mpctx->eof = mpctx->eof == PT_NEXT_ENTRY ? 1 : -1;
        if(play_tree_iter_step(mpctx->playtree_iter,mpctx->play_tree_step,0) == PLAY_TREE_ITER_ENTRY) {
            mpctx->eof = 1;
        } else {
            play_tree_iter_free(mpctx->playtree_iter);
            mpctx->playtree_iter = NULL;
        }
        mpctx->play_tree_step = 1;
    } else if(mpctx->eof == PT_UP_NEXT || mpctx->eof == PT_UP_PREV) {
        mpctx->eof = mpctx->eof == PT_UP_NEXT ? 1 : -1;
        if(mpctx->playtree_iter) {
            if(play_tree_iter_up_step(mpctx->playtree_iter,mpctx->eof,0) == PLAY_TREE_ITER_ENTRY) {
                mpctx->eof = 1;
            } else {
                play_tree_iter_free(mpctx->playtree_iter);
                mpctx->playtree_iter = NULL;
            }
        }
    } else if (mpctx->eof == PT_STOP) {
        /* Keep the parameters of subtitle for next file use, Fuchun 2010.12.28 */
        char *temp_subcp = sub_cp;
        int temp_color_num = color_num;
        int temp_sub_pos = sub_pos;
        float temp_sub_scale = text_font_scale_factor;
        int temp_fuzziness = sub_match_fuzziness;

        /* We move the below from command.c, 2010-11-24 carlos */
        while (play_tree_iter_up_step(mpctx->playtree_iter, 0, 1) != PLAY_TREE_ITER_END)
            /* NOP */ ;

        /* Keep the parameters of subtitle for next file use, Fuchun 2010.12.28 */
        sub_cp = temp_subcp;
        color_num = temp_color_num;
        sub_pos = temp_sub_pos;
        text_font_scale_factor = temp_sub_scale;
        sub_match_fuzziness = temp_fuzziness;

        play_tree_iter_free(mpctx->playtree_iter);
        mpctx->playtree_iter = NULL;
    } else { // NEXT PREV SRC
        mpctx->eof = mpctx->eof == PT_PREV_SRC ? -1 : 1;
    }

    if(mpctx->eof == 0) mpctx->eof = 1;

    while(mpctx->playtree_iter != NULL) {
        filename = play_tree_iter_get_file(mpctx->playtree_iter,mpctx->eof);
        if(filename == NULL) {
            if(play_tree_iter_step(mpctx->playtree_iter,mpctx->eof,0) != PLAY_TREE_ITER_ENTRY) {
                play_tree_iter_free(mpctx->playtree_iter);
                mpctx->playtree_iter = NULL;
            };
        } else
            break;
    }

#ifdef CONFIG_GUI
    if(use_gui && !mpctx->playtree_iter) {
#ifdef CONFIG_DVDREAD
        if(!guiIntfStruct.DiskChanged)
#endif
            mplEnd();
    }
#endif

    if((use_gui || mpctx->playtree_iter != NULL || player_idle_mode) && !force_quit) {
        if(!mpctx->playtree_iter) filename = NULL;
        mpctx->eof = 0;
        vo_reinit_osd();	//Fuchun 2010.11.25
        goto play_next_file;
    }


    exit_player_with_rc(EXIT_EOF, 0);

    return 1;
}

#endif /* DISABLE_MAIN */

#ifdef ENABLE_KEY_EVENT_FUNCTION_MODE
int parse_and_run_command(void)
{
	if((speed_mult != 0) && !h264_frame_mbs_only && (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES
		|| mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS || mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && !top_field_frame)
		return 0;

    mp_cmd_t* cmd;
    int brk_cmd = 0;
    current_module="key_events";

    while( !brk_cmd && (cmd = mp_input_get_cmd(0,0,0)) != NULL || domainsocket_cmd) {
        if(domainsocket_cmd)
            cmd = domainsocket_cmd;
        brk_cmd = run_command(mpctx, cmd);

#if 1	//Fuchun 2009.12.02
        if(cmd->pausing && cmd->pausing != 4)
        {
            FFFR_to_normalspeed(cmd);
        }
#endif

        if(!domainsocket_cmd)
            mp_cmd_free(cmd);
        else
            domainsocket_cmd = NULL;
        if (brk_cmd == 2)
            break;
    }
    return brk_cmd;
}
#endif // end of ENABLE_KEY_EVENT_FUNCTION_MODE

void FFFR_to_normalspeed(void *this)
{
    mp_cmd_t* cmd = (this ? (mp_cmd_t*)this : NULL);
    if(speed_mult != 0 && FR_to_end == 0)
    {
        speed_mult = 0;
        FFFR_to_normal = 1;

        audio_id = rel_audio_id;
        rel_audio_id = -5;
        last_rewind_pts = 0.0;

#ifdef _SKY_VDEC_V1
        if(h264_frame_mbs_only == 1 && Can_FF_FB == 1)
#else
        /* Carlos add 2010-12-30, fixed HD-DVD_Pan's Labyrinth.ts seek caused mosaic */
        if(Can_FF_FB == 1 && mpctx->sh_video && mpctx->demuxer->seekable && !(is_dvdnav && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU))
#endif
		{
			if (!((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_ES) && mpctx->sh_video && !mpctx->sh_audio))	//Barry 2011-08-11
				rel_seek_secs += 0.1;
#ifdef HW_TS_DEMUX
			//if(hwtsdemux)
			if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux)
				change_axi_speed(mpctx->demuxer->video, mpctx->demuxer->priv, NORMAL_SPPED);
#endif /* end of HW_TS_DEMUX */
		}
        else
        {
#if 0
            svsd_ipc_drop_video_cnts();
            sky_set_reset_display_queue();
#endif
            if(mpctx->demuxer->type == DEMUXER_TYPE_AVI_NINI)
                demux_avi_set_index_pos(mpctx->demuxer);

            if(speed_mult == 0 && num_reorder_frames != 0)
                queue_frames = num_reorder_frames;

            if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video)
                seek_sync_flag = 1;

            if(speed_mult == 0)
                mpctx->startup_decode_retry = DEFAULT_STARTUP_DECODE_RETRY;
            if (mpctx->sh_video) {
                current_module = "seek_video_reset";
                if (vo_config_count)
                    mpctx->video_out->control(VOCTRL_RESET, NULL);
                mpctx->num_buffered_frames = 0;
                mpctx->delay = 0;
                mpctx->time_frame = 0;

                mpctx->sh_video->pts = mpctx->d_video->pts;
                update_subtitles(mpctx->sh_video, mpctx->sh_video->pts-correct_sub_pts, mpctx->d_sub, 1);
                update_teletext(mpctx->sh_video, mpctx->demuxer, 1);
            }

            if (mpctx->sh_audio) {
                current_module = "seek_audio_reset";
                mpctx->audio_out->reset(); // stop audio, throwing away buffered data
                if (!mpctx->sh_video)
                    update_subtitles(NULL, mpctx->sh_audio->pts, mpctx->d_sub, 1);
#ifdef HW_TS_DEMUX
                /* Carlos add 2010-07-23 for sync audio when file_format is TS and using hwts */
                if (hwtsdemux && mpctx->demuxer->file_format==DEMUXER_TYPE_MPEG_TS)
                {
                    restart_audio_buffer(mpctx->demuxer->audio, mpctx->demuxer->priv, NEED_RESET_FILTER);
#ifdef DVB_SUBTITLES
                    if (mpctx->demuxer->sub && mpctx->demuxer->sub->id > 0)
                        restart_sub_buffer(mpctx->demuxer->sub, mpctx->demuxer->priv, NEED_RESET_FILTER);
#endif
                }
#endif // end of HW_TS_DEMUX
            }
#ifdef HW_TS_DEMUX
			//if(hwtsdemux)
			if ((mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS) && hwtsdemux)
				change_axi_speed(mpctx->demuxer->video, mpctx->demuxer->priv, NORMAL_SPPED);
#endif /* end of HW_TS_DEMUX */

            if (vo_vobsub && mpctx->sh_video) {
                current_module = "seek_vobsub_reset";
                vobsub_seek(vo_vobsub, mpctx->sh_video->pts);
            }

            edl_seek_reset(mpctx);

            c_total = 0;
            max_pts_correction = 0.1;
            audio_time_usage = 0;
            video_time_usage = 0;
            vout_time_usage = 0;
            drop_frame_cnt = 0;

            current_module = "end of speed_mult == 1";
        }

        if(mpctx->global_sub_size > 0)
        {
            rel_global_sub_pos = -5;

            if(vo_spudec != NULL)
            {
                last_spu_pts = 0;

            }
        }

        if(cmd == NULL || (cmd && cmd->pausing != 1))
            ipc_callback_fast();
    } //if(speed_mult != 0)

    if(audio_speed != 0)
    {
        audio_speed = 0;
        playback_speed = 1.0;
        build_afilter_chain(mpctx->sh_audio, &ao_data);
        ipc_callback_audio_fast();
    }
}

#if 0 //Robert 20101130 Reserved function
void mp_set_aopcm_zero(void)
{
    if (mpctx->sh_video && mpctx->sh_audio)
    {
        unsigned char ao_buf_256[256];
        memset(ao_buf_256, 0, 256);
//		set_pause_lock = 1;
        mpctx->audio_out->play(ao_buf_256, 256, 0);
    }
}
#endif

#ifdef CONFIG_DVDNAV_MAIN2
int fill_audio_out_buffers_wrapper(void)
{
    return fill_audio_out_buffers();
}

int sleep_until_update_original_wrapper(float *time_frame, float *aq_sleep_time)
{
    return sleep_until_update_original(time_frame, aq_sleep_time);
}

int sleep_until_update_wrapper(float *time_frame, float *aq_sleep_time)
{
    return sleep_until_update(time_frame, aq_sleep_time);
}

int sleep_until_update_dvdnav_wrapper(float *time_frame, float *aq_sleep_time)
{
    return sleep_until_update_dvdnav(time_frame, aq_sleep_time);
}

void print_status_wrapper(float a_pos, float a_v, float corr)
{
    print_status(a_pos, a_v, corr);
}

void adjust_sync_and_print_status_wrapper(int between_frames, float timing_error)
{
    adjust_sync_and_print_status(between_frames, timing_error);
}

void edl_update_wrapper(MPContext *mpctx)
{
    edl_update(mpctx);
}

int seek_wrapper(MPContext *mpctx, double amount, int style)
{
    return seek(mpctx, amount, style);
}

void pause_loop_wrapper(void)
{
    pause_loop();
}

double update_video_wrapper(int *blit_frame)
{
    double ret;
    int tmp_blit_frame = *blit_frame;
    ret = update_video(&tmp_blit_frame);
    *blit_frame = tmp_blit_frame;
    return ret;
}

void mp_dvdnav_highlight_handle_wrapper(int stream_is_change) {
    mp_dvdnav_highlight_handle(stream_is_change);
}

void mp_dvdnav_spu_handle_wrapper(void)
{
    mp_dvdnav_spu_handle();
}

int mp_dvdnav_audio_handle_wrapper(void)
{
    mp_dvdnav_audio_handle();
}

void update_osd_msg_wrapper(void)
{
    update_osd_msg();
}

double written_audio_pts_wrapper(sh_audio_t *sh_audio, demux_stream_t *d_audio)
{
    return written_audio_pts(sh_audio, d_audio);
}
#endif

#ifdef CHECK_AAC_HEADER
/* Copy from libmpcodecs/ad_saaac.c aac_probe function */
static int check_aac_header(unsigned char *buffer, int len, int mode)
{
       int i = 0, pos = 0;
       while(i <= len-4) 
       {
               int t, t2;
               if ((buffer[i] == 0x56) && (mode==0 || mode==1)) {
                       if ((buffer[i+1]&0xE0)==0xE0) {
                               t = ((buffer[i+1]&0x1F)<<8)+buffer[i+2];
                               if ((i+4+t)<len) {
                                       t2 = (buffer[i+3+t]<<8)+(buffer[i+4+t]);
                                       if ((t2>>5)==0x2b7) {
                                               pos = i;
                                               break;
                                       }
                               }
                       }
               }
               if ((buffer[i] == 0xff) && (mode==0 || mode==3)) {
                       if ((buffer[i+1] & 0xf6) == 0xf0) {
                               t = ((buffer[i+3]&0x3)<<11)+(buffer[i+4]<<3)+(buffer[i+5]>>5);
                              if ((i+1+t)<len && t>7) {
                                       t2 = (buffer[i+t]<<8)+(buffer[i+1+t]);
                                       if ((t2&0xfff6)==0xfff0) {
                                               pos = i;
                                               break;
                                       }
                               }
                       }
               }
               if ((mode==0 || mode==2) && (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')) {
                       pos = i;
                       break;
               }

               i++;
       }
       return i;
}
#endif /* end of CHECK_AAC_HEADER */
