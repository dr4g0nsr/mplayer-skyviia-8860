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

#ifndef MPLAYER_CFG_MPLAYER_H
#define MPLAYER_CFG_MPLAYER_H

/*
 * config for cfgparser
 */

#include "cfg-common.h"
#include "libmpcodecs/vd.h"
#include "libvo/vo_zr.h"

#if 1	// Added by Skymedi  

extern unsigned int DD_K;                 //Honda for DD 2010/09/03
extern unsigned int DD_S;                  //Honda for DD 2010/09/03
extern unsigned int DD_COMP;               //Honda for DD 2010/09/03  
extern unsigned int DD_MONO;               //Honda for DD 2010/09/28    
extern unsigned int DD_M;                  //Honda for DD 2010/09/28  

unsigned int night_mode;            // Honda 2010/12/29          
extern unsigned int audio_s32;		// Honda 2010/04/30
extern unsigned int dts_packet;     // Honda 2010/07/07
extern unsigned int pingpong_op_mode;						/*johnnyke 20100412*/
extern unsigned int multibuf_op_mode;						/*johnnyke 20100412*/
extern int		    rotation_degree;							/*johnnyke 20100412*/
extern float		    zoom_ratio;							/*johnnyke 20100412*/
extern unsigned int scaling;									/*johnnyke 20100412*/
extern unsigned int deinterlacing;							/*johnnyke 20100412*/
extern unsigned int rgb_converstion;						/*johnnyke 20100412*/
extern unsigned int dithering_en;							/*johnnyke 20100412*/
extern unsigned int pulldown32;							/*johnnyke 20100428*/
extern unsigned int buf_num;						/*johnnyke 20100503*/
extern unsigned int alpha_blend;					/*johnnyke 20101020*/
extern unsigned int yuv422_outFmt;				/*johnnyke 20101021*/
extern int check_dma;				// Raymond 2009/06/24
extern char *check_file_name;		// Raymond 2009/06/24
extern unsigned int disable_disp_deint;		//Fuchun 2010.11.17
extern int select_sub_id;

extern int scanf_per_frame;		// Raymond 2009/07/06
extern int usleep_cnt;				// Raymond 2009/07/06

// Raymond 2009/08/03
extern int print_video_read_time;
extern int print_video_decode_time;
extern int print_audio_decode_time;

extern unsigned int skydroid;		// Raymond 2009/08/11
extern unsigned int skyqt;		// Robert 2011/02/18

extern unsigned int forcelock;		// Mark 2010/04/21

#ifdef HW_TS_DEMUX
extern int hwtsdemux;		// Carlos 2010/05/19
#ifdef SUPPORT_NETWORKING_HWTS
extern unsigned int hwtsnet;		/* Carlos add 2011-01-18 */
#endif /* end of SUPPORT_NETWORKING_HWTS */
#endif /* end of HW_TS_DEMUX */
#ifdef SUPPORT_TS_SELECT_LOW_AUDIO	/* Carlos add 2011-01-18 */
extern unsigned int ts_easy_audio;
#endif /* end of SUPPORT_TS_SELECT_LOW_AUDIO */
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID	// Carlos add 2010-10-14
extern int stop_tuner;
extern int setdvb_by_pid;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
#ifdef TS_STREAM_CHECK_RANGE
extern unsigned int ts_check_cache;
#endif /* end of TS_STREAM_CHECK_RANGE */ // add by carlos, 2011-03-15
#ifdef DEBUG_ADD_PACKET_PTS
extern unsigned int showpts;
extern unsigned int showvideo;
extern unsigned int showaudio;
extern unsigned int showsub;
#endif /* end of DEBUG_ADD_PACKET_PTS */
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
extern int quick_bd;
#endif /* end of SUPPORT_QT_BD_ISO_ENHANCE */

#ifndef BUILD_C000000_UNUSED_CODEC	//Barry 2011-10-06
extern int DRCPercent ; //larry DTS-HD2
extern int SpkrOut ;    //larry DTS-HD2
extern int l1;          //larry DTS-HD2
extern int nodialnorm ; //larry DTS-HD2
extern int bass;          //larry DTS-HD2
#endif

extern int vol_blksz;
#ifdef DEMUX_THREAD			// Charleslin 2010/06/04
extern int demuxthread;
extern int athreshold;
extern int vthreshold;
extern int upnp;
extern int samba;
extern int http_retry;
#endif
#ifdef STREAM_BUF_PTR
extern int bsz;
#endif
#ifdef STREAM_DIO
extern int dio;
#endif
#ifdef STREAM_AIO
extern int aio;
extern int nbuf;
#endif
#ifdef SKY_HTTP
extern int sky_http_enable;
#endif


#ifdef STREAM_DIO_AUTO
void dio_enable(stream_t *s);
#endif
#ifdef STREAM_AIO_AUTO
void aio_enable(stream_t *s);
void cache_aio_enable(void *data);
#endif
#ifdef STREAM_USE_DEVMEM
extern int nodevmem;
#endif
#ifdef READ_PERF_TEST
extern int cpy;
extern int spd;
#endif
extern int cache_type;
#ifdef CACHE_IN_FILE
extern int fcache;
extern int fcache_time;
extern char *fcache_fn;
#endif
#ifdef FILE_CACHE
extern int ncache_size;
extern char *ncache_fn;
#endif
#ifdef YOUTUBE_RECONNECT
extern int youtube;
#endif

extern int cvq_threshold, btw_threshold;

extern unsigned int stthumb_mode;
// Raymond 2009/09/25
extern unsigned int thumbnail_mode;	
extern unsigned int th_width;
extern unsigned int th_height;

extern unsigned int ipcdomain;		//SkyMedi_Vincent08122009

extern char *dump_yuv_file_name;	// Raymond 2009/09/28

extern char *parser_table_path;		// Raymond 2009/11/27

extern unsigned int no_osd;		//Fuchun 2010.07.21
extern unsigned int get_output_channels;	//Fuchun 2011.05.17

extern int sky_aspect;	//Barry 2010-10-06
extern unsigned int sky_hwac3;	//Barry 2010-10-09

extern unsigned int start_x;
extern unsigned int start_y;
extern unsigned int end_x;
extern unsigned int end_y;

#endif

extern int key_fifo_size;
extern unsigned doubleclick_time;

extern char *fb_mode_cfgfile;
extern char *fb_mode_name;
extern char *dfb_params;

extern char *lirc_configfile;

extern float vo_panscanrange;
/* only used at startup (setting these values from configfile) */
extern char *vo_geometry;

extern char *ao_outputfilename;
extern int ao_pcm_waveheader;

extern int fs_layer;
extern int stop_xscreensaver;

extern int menu_startup;
extern int menu_keepdir;
extern char *menu_chroot;
extern char *menu_fribidi_charset;
extern int menu_flip_hebrew;
extern int menu_fribidi_flip_commas;

extern char *unrar_executable;

extern m_option_t dxr2_opts[];

extern char * skinName;
extern int guiWinID;


/* from libvo/aspect.c */
extern float force_monitor_aspect;
extern float monitor_pixel_aspect;

extern int sws_flags;
extern char* pp_help;

const m_option_t vd_conf[]={
	{"help", "Use MPlayer with an appropriate video file instead of live partners to avoid vd.\n", CONF_TYPE_PRINT, CONF_NOCFG|CONF_GLOBAL, 0, 0, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};

#ifdef CONFIG_TV
const m_option_t tvscan_conf[]={
	{"autostart", &stream_tv_defaults.scan, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"threshold", &stream_tv_defaults.scan_threshold, CONF_TYPE_INT, CONF_RANGE, 1, 100, NULL},
	{"period", &stream_tv_defaults.scan_period, CONF_TYPE_FLOAT, CONF_RANGE, 0.1, 2.0, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};
#endif
/*
 * CONF_TYPE_FUNC_FULL :
 * allows own implementations for passing the params
 *
 * the function receives parameter name and argument (if it does not start with - )
 * useful with a conf.name like 'aa*' to parse several parameters to a function
 * return 0 =ok, but we didn't need the param (could be the filename)
 * return 1 =ok, we accepted the param
 * negative values: see cfgparser.h, ERR_XXX
 *
 * by Folke
 */

const m_option_t mplayer_opts[]={
	/* name, pointer, type, flags, min, max */

#if 1	// Added by Skyviia  
   {"dd_k", &DD_K, CONF_TYPE_INT, CONF_RANGE, 0, 3, NULL},   //Honda for DD 2010/09/03
   {"dd_s", &DD_S, CONF_TYPE_INT, CONF_RANGE, 0, 2, NULL},   //Honda for DD 2010/09/03 
   {"dd_comp", &DD_COMP, CONF_TYPE_INT, CONF_RANGE, 0, 5, NULL},   //Honda for DD 2010/09/03 
   {"dd_mono", &DD_MONO, CONF_TYPE_INT, CONF_RANGE, 0, 3, NULL},  //Honda for DD 2010/09/28 
   {"dd_m", &DD_M, CONF_TYPE_INT, CONF_RANGE, 0, 7, NULL},  //Honda for DD 2010/09/28

   {"nightmode", &night_mode, CONF_TYPE_FLAG, 0, 0, 1, NULL},      // Honda 2010/12/29  
   {"dts_packet", &dts_packet, CONF_TYPE_FLAG, 0, 0, 1, NULL},      // Honda 2010/07/07
   {"b24_32", &audio_s32, CONF_TYPE_FLAG, 0, 0, 1, NULL},      // Honda 2010/06/02  
   {"ao_s32", &audio_s32, CONF_TYPE_FLAG, 0, 0, 1, NULL},      // Honda 2010/04/30

#ifndef BUILD_C000000_UNUSED_CODEC	//Barry 2011-10-06
   {"DRC", &DRCPercent, CONF_TYPE_INT, CONF_RANGE, 0, 2, NULL},      //larry DTS-HD2
	 {"SpkrOut", &SpkrOut, CONF_TYPE_INT, CONF_RANGE, 0, 14, NULL},   //larry DTS-HD2
	 {"l", &l1, CONF_TYPE_INT, 0, 0, 1, NULL},                         //larry DTS-HD2
	 {"nodialnorm", &nodialnorm, CONF_TYPE_INT, 0, 0, 1, NULL},         //larry DTS-HD2
	 {"bass", &bass, CONF_TYPE_INT, 0, 0, 1, NULL},                         //larry DTS-HD2
#endif
	 
	// Raymond 2009/06/24
	{"checkdma", &check_dma, CONF_TYPE_FLAG, 0, 0, 1, NULL},		
	{"checkfile", &check_file_name, CONF_TYPE_STRING, 0, 0, 0, NULL},

	// Raymond 2009/07/06
	{"scanf", &scanf_per_frame, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"sleep", &usleep_cnt, CONF_TYPE_INT, CONF_RANGE, 0, 1000000, NULL},

	// Raymond 2009/08/03
	{"vrt", &print_video_read_time, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"vdt", &print_video_decode_time, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"adt", &print_audio_decode_time, CONF_TYPE_FLAG, 0, 0, 1, NULL},

	// Raymond 2009/09/25
//Robert 20110324
	{"stthumb", &stthumb_mode, CONF_TYPE_INT, CONF_RANGE, 0, 3, NULL},
	{"thumbnail", &thumbnail_mode, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"tx", &th_width, CONF_TYPE_INT, CONF_RANGE, 0, 1920, NULL},
	{"ty", &th_height, CONF_TYPE_INT, CONF_RANGE, 0, 1080, NULL},

	// Raymond 2009/09/25
	{"skydroid", &skydroid, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"skyqt", &skyqt, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	
	// Mark 2010/04/21
	{"forcelock", &forcelock, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	
	//SkyMedi_Vincent08122009
	{"ipcdomain", &ipcdomain, CONF_TYPE_FLAG, 0, 0, 1, NULL},

	// Raymond 2009/09/28
	{"dumpyuv", &dump_yuv_file_name, CONF_TYPE_STRING, 0, 0, 0, NULL},

	// Raymond 2009/11/27
	{"pt", &parser_table_path, CONF_TYPE_STRING, 0, 0, 0, NULL},

	//Fuchun 2009.12.18
	{"colornum", &color_num, CONF_TYPE_INT, CONF_RANGE, 0, 7, NULL},
	{"noosd", &no_osd, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"subopt", subopt, CONF_TYPE_SUBCONFIG, 0, 0, 0, NULL},
	{"output_channels", &get_output_channels, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"subid", &select_sub_id, CONF_TYPE_INT, CONF_RANGE, -2, 5120, NULL},

	#ifdef _SKY_VDEC_V2

	//johnnyke 20100412
	{"pp_pingpong", &pingpong_op_mode, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"pp_multibuf", &multibuf_op_mode, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"rotation", &rotation_degree, CONF_TYPE_INT, 0, 0, 0, NULL},
	{"zoomin", &zoom_ratio, CONF_TYPE_FLOAT, CONF_RANGE, 1.0, 3.0, NULL},
	{"scaling", &scaling, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"deint", &deinterlacing, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"RGBconv", &rgb_converstion, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"dither", &dithering_en, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"pd32", &pulldown32, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"buf_num", &buf_num, CONF_TYPE_INT, CONF_RANGE, 3, 17, NULL},
	{"alpha_blend", &alpha_blend, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"yuv422", &yuv422_outFmt, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"no_dispdeint", &disable_disp_deint, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	#endif

#ifdef HW_TS_DEMUX
	// Carlos 20100519
	{"hwts", &hwtsdemux, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#ifdef SUPPORT_NETWORKING_HWTS	
	{"tsnet", &hwtsnet, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif /* end of SUPPORT_NETWORKING_HWTS */	
#endif /* end of HW_TS_DEMUX */
#ifdef TS_STREAM_CHECK_RANGE
	{"tscache", &ts_check_cache, CONF_TYPE_INT, CONF_RANGE, 0x10000, 0x800000, NULL},
#endif /* end of TS_STREAM_CHECK_RANGE */
#ifdef SUPPORT_TS_SELECT_LOW_AUDIO
	{"fa", &ts_easy_audio, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif /* end of SUPPORT_TS_SELECT_LOW_AUDIO */
#ifdef DEBUG_ADD_PACKET_PTS	
	{"showp", &showpts, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"showv", &showvideo, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"showa", &showaudio, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"shows", &showsub, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif // end of DEBUG_ADD_PACKET_PTS	
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	{"stop_tuner", &stop_tuner, CONF_TYPE_INT, CONF_RANGE, 0, 1, NULL},
	{"skytv", &setdvb_by_pid, CONF_TYPE_INT, CONF_RANGE, 0, 1, NULL},
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
	{"quick_bd", &quick_bd, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif // end of SUPPORT_QT_BD_ISO_ENHANCE

#ifdef DEMUX_THREAD
	{ "dth", &demuxthread, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{ "ath", &athreshold,  CONF_TYPE_INT, CONF_RANGE, 0, MAX_PACKS, NULL},
	{ "vth", &vthreshold,  CONF_TYPE_INT, CONF_RANGE, 0, MAX_PACKS, NULL},
#endif
	{ "upnp", &upnp, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{ "upnp_duration", &upnp_duration, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	/* 2011-01-20 add for samba parameter */
	{ "samba", &samba, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#ifdef STREAM_BUF_PTR
	{ "bsz", &bsz, CONF_TYPE_INT, CONF_RANGE, 0x800, 0x100000, NULL},
#endif
#ifdef STREAM_DIO
	{ "dio", &dio, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif
#ifdef STREAM_AIO
	{ "aio", &aio, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif
#ifdef SKY_HTTP
	{ "skyhttp", &sky_http_enable, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif

#ifdef STREAM_USE_DEVMEM
	{ "nodevmem", &nodevmem, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif
#ifdef READ_PERF_TEST
	{ "cpy", &cpy, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{ "spd", &spd, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif
	{ "cache-type", &cache_type, CONF_TYPE_INT, CONF_RANGE, 0, 2, NULL},
#ifdef CACHE_IN_FILE
	{ "fcache", &fcache, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{ "fcache-time", &fcache_time, CONF_TYPE_INT, CONF_RANGE, 0, 9999, NULL},
	{ "fcache-file", &fcache_fn, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif
#ifdef FILE_CACHE
	{ "ncache-size", &ncache_size, CONF_TYPE_INT, CONF_RANGE, 0, 0x10000000, NULL},
	{ "ncache-file", &ncache_fn, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif
#ifdef YOUTUBE_RECONNECT
	{ "youtube", &youtube, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif
	{ "blksz", &vol_blksz, CONF_TYPE_INT, CONF_RANGE, 0x200, 0x100000, NULL },
	{ "http-retry", &http_retry, CONF_TYPE_FLAG, 0, 0, 1, NULL },

//Robert 20100617 check video queue & audio buffers, prevent CPU Busy...
        { "cvq", &cvq_threshold,  CONF_TYPE_INT, CONF_RANGE, 0, 10, NULL},
        { "btw", &btw_threshold,  CONF_TYPE_INT, CONF_RANGE, 0, 16384, NULL},

        { "nosync", &nosync,  CONF_TYPE_INT, CONF_RANGE, 0, 1, NULL},

	{ "sky_aspect", &sky_aspect,  CONF_TYPE_INT, CONF_RANGE, 0, 10, NULL},	//Barry 2010-10-06
	{ "sky_hwac3", &sky_hwac3,  CONF_TYPE_FLAG, 0, 0, 1, NULL},	//Barry 2010-10-09

        //Polun 2011-05-16
	{ "stx", &start_x,    CONF_TYPE_INT, CONF_RANGE, 0, 1920, NULL},	
	{ "sty", &start_y,    CONF_TYPE_INT, CONF_RANGE, 0, 1080, NULL},	
	{ "edx", &end_x,    CONF_TYPE_INT, CONF_RANGE, 0, 1920, NULL},	
	{ "edy", &end_y,    CONF_TYPE_INT, CONF_RANGE, 0, 1080, NULL},	
#endif


//---------------------- libao/libvo options ------------------------
	{"o", "Option -o has been renamed to -vo (video-out), use -vo.\n",
            CONF_TYPE_PRINT, CONF_NOCFG, 0, 0, NULL},
	{"vo", &video_driver_list, CONF_TYPE_STRING_LIST, 0, 0, 0, NULL},
	{"ao", &audio_driver_list, CONF_TYPE_STRING_LIST, 0, 0, 0, NULL},
	{"fixed-vo", &fixed_vo, CONF_TYPE_FLAG,CONF_GLOBAL , 0, 1, NULL},
	{"nofixed-vo", &fixed_vo, CONF_TYPE_FLAG,CONF_GLOBAL, 1, 0, NULL},
	{"ontop", &vo_ontop, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"noontop", &vo_ontop, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"rootwin", &vo_rootwin, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"border", &vo_border, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"noborder", &vo_border, CONF_TYPE_FLAG, 0, 1, 0, NULL},

	{"aop", "-aop has been removed, use -af instead.\n", CONF_TYPE_PRINT, CONF_NOCFG, 0, 0, NULL},
	{"dsp", "-dsp has been removed. Use -ao oss:dsp_path instead.\n", CONF_TYPE_PRINT, CONF_NOCFG, 0, 0, NULL},
        {"mixer", &mixer_device, CONF_TYPE_STRING, 0, 0, 0, NULL},
        {"mixer-channel", &mixer_channel, CONF_TYPE_STRING, 0, 0, 0, NULL},
        {"softvol", &soft_vol, CONF_TYPE_FLAG, 0, 0, 1, NULL},
        {"nosoftvol", &soft_vol, CONF_TYPE_FLAG, 0, 1, 0, NULL},
        {"softvol-max", &soft_vol_max, CONF_TYPE_FLOAT, CONF_RANGE, 10, 10000, NULL},
	{"volstep", &volstep, CONF_TYPE_INT, CONF_RANGE, 0, 100, NULL},
	{"volume", &start_volume, CONF_TYPE_FLOAT, CONF_RANGE, -1, 10000, NULL},
	{"master", "Option -master has been removed, use -af volume instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	// override audio buffer size (used only by -ao oss, anyway obsolete...)
	{"abs", &ao_data.buffersize, CONF_TYPE_INT, CONF_MIN, 0, 0, NULL},

	// -ao pcm options:
	{"aofile", "-aofile has been removed. Use -ao pcm:file=<filename> instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"waveheader", "-waveheader has been removed. Use -ao pcm:waveheader instead.\n", CONF_TYPE_PRINT, 0, 0, 1, NULL},
	{"nowaveheader", "-nowaveheader has been removed. Use -ao pcm:nowaveheader instead.\n", CONF_TYPE_PRINT, 0, 1, 0, NULL},

	{"alsa", "-alsa has been removed. Remove it from your config file.\n",
            CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"noalsa", "-noalsa has been removed. Remove it from your config file.\n",
            CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"edlout", &edl_output_filename,  CONF_TYPE_STRING, 0, 0, 0, NULL},

#ifdef CONFIG_X11
	{"display", &mDisplayName, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif

	// -vo png only:
#ifdef CONFIG_PNG
	{"z", "-z has been removed. Use -vo png:z=<0-9> instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
#endif
	// -vo jpeg only:
#ifdef CONFIG_JPEG
	{"jpeg", "-jpeg has been removed. Use -vo jpeg:<options> instead.\n",
	    CONF_TYPE_PRINT, 0, 0, 0, NULL},
#endif
	// -vo sdl only:
	{"sdl", "Use -vo sdl:driver=<driver> instead of -vo sdl -sdl driver.\n",
	    CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"noxv", "-noxv has been removed. Use -vo sdl:nohwaccel instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"forcexv", "-forcexv has been removed. Use -vo sdl:forcexv instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	// -ao sdl only:
	{"sdla", "Use -ao sdl:driver instead of -ao sdl -sdla driver.\n",
	    CONF_TYPE_PRINT, 0, 0, 0, NULL},

#if defined(CONFIG_FBDEV) || defined(CONFIG_VESA)
       {"monitor-hfreq", &monitor_hfreq_str, CONF_TYPE_STRING, 0, 0, 0, NULL},
       {"monitor-vfreq", &monitor_vfreq_str, CONF_TYPE_STRING, 0, 0, 0, NULL},
       {"monitor-dotclock", &monitor_dotclock_str, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif

#ifdef CONFIG_FBDEV
	{"fbmode", &fb_mode_name, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"fbmodeconfig", &fb_mode_cfgfile, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif
#ifdef CONFIG_DIRECTFB
#if DIRECTFBVERSION > 912
	{"dfbopts", "-dfbopts has been removed. Use -vf directfb:dfbopts=... instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
#endif
#endif

	// force window width/height or resolution (with -vm)
	{"x", &opt_screen_size_x, CONF_TYPE_INT, CONF_RANGE, 0, 4096, NULL},
	{"y", &opt_screen_size_y, CONF_TYPE_INT, CONF_RANGE, 0, 4096, NULL},
	// set screen dimensions (when not detectable or virtual!=visible)
	{"screenw", &vo_screenwidth, CONF_TYPE_INT, CONF_RANGE|CONF_OLD, 0, 4096, NULL},
	{"screenh", &vo_screenheight, CONF_TYPE_INT, CONF_RANGE|CONF_OLD, 0, 4096, NULL},
	// Geometry string
	{"geometry", &vo_geometry, CONF_TYPE_STRING, 0, 0, 0, NULL},
	// vo name (X classname) and window title strings
	{"name", &vo_winname, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"title", &vo_wintitle, CONF_TYPE_STRING, 0, 0, 0, NULL},
	// set aspect ratio of monitor - useful for 16:9 TV-out
	{"monitoraspect", &force_monitor_aspect, CONF_TYPE_FLOAT, CONF_RANGE, 0.0, 9.0, NULL},
	{"monitorpixelaspect", &monitor_pixel_aspect, CONF_TYPE_FLOAT, CONF_RANGE, 0.2, 9.0, NULL},
	// video mode switching: (x11,xv,dga)
        {"vm", &vidmode, CONF_TYPE_FLAG, 0, 0, 1, NULL},
        {"novm", &vidmode, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	// start in fullscreen mode:
	{"fs", &fullscreen, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nofs", &fullscreen, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	// set fullscreen switch method (workaround for buggy WMs)
	{"fsmode", "-fsmode is obsolete, avoid it and use -fstype instead.\nIf you really want it, try -fsmode-dontuse, but don't report bugs!\n", CONF_TYPE_PRINT, CONF_RANGE, 0, 31, NULL},
	{"fsmode-dontuse", &vo_fsmode, CONF_TYPE_INT, CONF_RANGE, 0, 31, NULL},
	// set bpp (x11+vm, dga, fbdev, vesa, svga?)
        {"bpp", &vo_dbpp, CONF_TYPE_INT, CONF_RANGE, 0, 32, NULL},
	{"colorkey", &vo_colorkey, CONF_TYPE_INT, 0, 0, 0, NULL},
	{"nocolorkey", &vo_colorkey, CONF_TYPE_FLAG, 0, 0, 0x1000000, NULL},
	{"double", &vo_doublebuffering, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nodouble", &vo_doublebuffering, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	// wait for v-sync (vesa)
	{"vsync", &vo_vsync, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"novsync", &vo_vsync, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"panscan", &vo_panscan, CONF_TYPE_FLOAT, CONF_RANGE, -1.0, 1.0, NULL},
	{"panscanrange", &vo_panscanrange, CONF_TYPE_FLOAT, CONF_RANGE, -19.0, 99.0, NULL},

	{"grabpointer", &vo_grabpointer, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nograbpointer", &vo_grabpointer, CONF_TYPE_FLAG, 0, 1, 0, NULL},

    {"adapter", &vo_adapter_num, CONF_TYPE_INT, CONF_RANGE, 0, 5, NULL},
    {"refreshrate",&vo_refresh_rate,CONF_TYPE_INT,CONF_RANGE, 0,100, NULL},
	{"wid", &WinID, CONF_TYPE_INT64, 0, 0, 0, NULL},
#ifdef CONFIG_X11
	// x11,xv,xmga,xvidix
	{"icelayer", "-icelayer has been removed. Use -fstype layer:<number> instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"stop-xscreensaver", &stop_xscreensaver, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nostop-xscreensaver", &stop_xscreensaver, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"stop_xscreensaver", "Use -stop-xscreensaver instead, options with _ have been obsoleted.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"fstype", &vo_fstype_list, CONF_TYPE_STRING_LIST, 0, 0, 0, NULL},
#endif
	{"heartbeat-cmd", &heartbeat_cmd, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"mouseinput", &vo_nomouse_input, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"nomouseinput", &vo_nomouse_input, CONF_TYPE_FLAG,0, 0, 1, NULL},

	{"xineramascreen", &xinerama_screen, CONF_TYPE_INT, CONF_RANGE, -2, 32, NULL},

	{"brightness",&vo_gamma_brightness, CONF_TYPE_INT, CONF_RANGE, -100, 100, NULL},
	{"saturation",&vo_gamma_saturation, CONF_TYPE_INT, CONF_RANGE, -100, 100, NULL},
	{"contrast",&vo_gamma_contrast, CONF_TYPE_INT, CONF_RANGE, -100, 100, NULL},
	{"hue",&vo_gamma_hue, CONF_TYPE_INT, CONF_RANGE, -100, 100, NULL},
	{"keepaspect", &vo_keepaspect, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nokeepaspect", &vo_keepaspect, CONF_TYPE_FLAG, 0, 1, 0, NULL},

	// direct rendering (decoding to video out buffer)
	{"dr", &vo_directrendering, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"nodr", &vo_directrendering, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"vaa_dr", "-vaa_dr has been removed, use -dr.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"vaa_nodr", "-vaa_nodr has been removed, use -nodr.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},

#ifdef CONFIG_AA
	// -vo aa
	{"aa*", "-aa* has been removed. Use -vo aa:suboption instead.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
#endif

#ifdef CONFIG_ZR
	// -vo zr
	{"zr*", vo_zr_parseoption, CONF_TYPE_FUNC_FULL, 0, 0, 0, &vo_zr_revertoption },
#endif

#ifdef CONFIG_DXR2
	{"dxr2", &dxr2_opts, CONF_TYPE_SUBCONFIG, 0, 0, 0, NULL},
#endif


//---------------------- mplayer-only options ------------------------

	{"use-filedir-conf", &use_filedir_conf, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"nouse-filedir-conf", &use_filedir_conf, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
	{"use-filename-title", &use_filename_title, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"nouse-filename-title", &use_filename_title, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
#ifdef CONFIG_CRASH_DEBUG
	{"crash-debug", &crash_debug, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"nocrash-debug", &crash_debug, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
#endif
	{"osdlevel", &osd_level, CONF_TYPE_INT, CONF_RANGE, 0, 3, NULL},
	{"osd-duration", &osd_duration, CONF_TYPE_INT, CONF_MIN, 0, 0, NULL},
#ifdef CONFIG_MENU
	{"menu", &use_menu, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"nomenu", &use_menu, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
	{"menu-root", &menu_root, CONF_TYPE_STRING, CONF_GLOBAL, 0, 0, NULL},
	{"menu-cfg", &menu_cfg, CONF_TYPE_STRING, CONF_GLOBAL, 0, 0, NULL},
	{"menu-startup", &menu_startup, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"menu-keepdir", &menu_keepdir, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"menu-chroot", &menu_chroot, CONF_TYPE_STRING, 0, 0, 0, NULL},
#ifdef CONFIG_FRIBIDI
	{"menu-fribidi-charset", &menu_fribidi_charset, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"menu-flip-hebrew", &menu_flip_hebrew, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"menu-noflip-hebrew", &menu_flip_hebrew, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"menu-flip-hebrew-commas", &menu_fribidi_flip_commas, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"menu-noflip-hebrew-commas", &menu_fribidi_flip_commas, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#endif /* CONFIG_FRIBIDI */
#else
	{"menu", "OSD menu support was not compiled in.\n", CONF_TYPE_PRINT,0, 0, 0, NULL},
#endif /* CONFIG_MENU */

	// these should be moved to -common, and supported in MEncoder
	{"vobsub", &vobsub_name, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"vobsubid", &vobsub_id, CONF_TYPE_INT, CONF_RANGE, 0, 31, NULL},
#ifdef CONFIG_UNRAR_EXEC
	{"unrarexec", &unrar_executable, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif

	{"sstep", &step_sec, CONF_TYPE_INT, CONF_MIN, 0, 0, NULL},

	{"framedrop", &frame_dropping, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"hardframedrop", &frame_dropping, CONF_TYPE_FLAG, 0, 0, 2, NULL},
	{"noframedrop", &frame_dropping, CONF_TYPE_FLAG, 0, 1, 0, NULL},

	{"autoq", &auto_quality, CONF_TYPE_INT, CONF_RANGE, 0, 100, NULL},

	{"benchmark", &benchmark, CONF_TYPE_FLAG, 0, 0, 1, NULL},

	// dump some stream out instead of playing the file
	// this really should be in MEncoder instead of MPlayer... -> TODO
	{"dumpfile", &stream_dump_name, CONF_TYPE_STRING, 0, 0, 0, NULL},
	{"dumpaudio", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"dumpvideo", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 2, NULL},
	{"dumpsub", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 3, NULL},
	{"dumpmpsub", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 4, NULL},
	{"dumpstream", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 5, NULL},
	{"dumpsrtsub", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 6, NULL},
	{"dumpmicrodvdsub", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 7, NULL},
	{"dumpjacosub", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 8, NULL},
	{"dumpsami", &stream_dump_type, CONF_TYPE_FLAG, 0, 0, 9, NULL},

#ifdef CONFIG_LIRC
	{"lircconf", &lirc_configfile, CONF_TYPE_STRING, CONF_GLOBAL, 0, 0, NULL},
#endif

	{"gui", "The -gui option will only work as the first command line argument.\n", CONF_TYPE_PRINT, 0, 0, 0, (void *)1},
	{"nogui", "The -nogui option will only work as the first command line argument.\n", CONF_TYPE_PRINT, 0, 0, 0, (void *)1},

#ifdef CONFIG_GUI
	{"skin", &skinName, CONF_TYPE_STRING, CONF_GLOBAL, 0, 0, NULL},
	{"enqueue", &enqueue, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"noenqueue", &enqueue, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"guiwid", &guiWinID, CONF_TYPE_INT, 0, 0, 0, NULL},
#endif

	{"noloop", &mpctx_s.loop_times, CONF_TYPE_FLAG, 0, 0, -1, NULL},
	{"loop", &mpctx_s.loop_times, CONF_TYPE_INT, CONF_RANGE, -1, 10000, NULL},
	{"playlist", NULL, CONF_TYPE_STRING, 0, 0, 0, NULL},

	// a-v sync stuff:
        {"correct-pts", &user_correct_pts, CONF_TYPE_FLAG, 0, 0, 1, NULL},
        {"nocorrect-pts", &user_correct_pts, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"noautosync", &autosync, CONF_TYPE_FLAG, 0, 0, -1, NULL},
	{"autosync", &autosync, CONF_TYPE_INT, CONF_RANGE, 0, 10000, NULL},
//	{"dapsync", &dapsync, CONF_TYPE_FLAG, 0, 0, 1, NULL},
//	{"nodapsync", &dapsync, CONF_TYPE_FLAG, 0, 1, 0, NULL},

	{"softsleep", &softsleep, CONF_TYPE_FLAG, 0, 0, 1, NULL},
#ifdef HAVE_RTC
	{"nortc", &nortc, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"rtc", &nortc, CONF_TYPE_FLAG, 0, 1, 0, NULL},
	{"rtc-device", &rtc_device, CONF_TYPE_STRING, 0, 0, 0, NULL},
#endif

	{"term-osd", &term_osd, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{"noterm-osd", &term_osd, CONF_TYPE_FLAG, 0, 1, 0, NULL},
    	{"term-osd-esc", &term_osd_esc, CONF_TYPE_STRING, 0, 0, 1, NULL},
	{"playing-msg", &playing_msg, CONF_TYPE_STRING, 0, 0, 0, NULL},

	{"slave", &slave_mode, CONF_TYPE_FLAG,CONF_GLOBAL , 0, 1, NULL},
	{"idle", &player_idle_mode, CONF_TYPE_FLAG,CONF_GLOBAL , 0, 1, NULL},
	{"noidle", &player_idle_mode, CONF_TYPE_FLAG,CONF_GLOBAL , 1, 0, NULL},
	{"use-stdin", "-use-stdin has been renamed to -noconsolecontrols, use that instead.", CONF_TYPE_PRINT, 0, 0, 0, NULL},
	{"key-fifo-size", &key_fifo_size, CONF_TYPE_INT, CONF_RANGE, 2, 65000, NULL},
	{"noconsolecontrols", &noconsolecontrols, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"consolecontrols", &noconsolecontrols, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
	{"mouse-movements", &enable_mouse_movements, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"nomouse-movements", &enable_mouse_movements, CONF_TYPE_FLAG, CONF_GLOBAL, 1, 0, NULL},
	{"doubleclick-time", &doubleclick_time, CONF_TYPE_INT, CONF_RANGE, 0, 1000, NULL},
#ifdef CONFIG_TV
	{"tvscan", tvscan_conf, CONF_TYPE_SUBCONFIG, 0, 0, 0, NULL},
#else
	{"tvscan", "MPlayer was compiled without TV interface support.\n", CONF_TYPE_PRINT, 0, 0, 0, NULL},
#endif /* CONFIG_TV */

#include "cfg-common-opts.h"

	{"list-properties", &list_properties, CONF_TYPE_FLAG, CONF_GLOBAL, 0, 1, NULL},
	{"identify", &mp_msg_levels[MSGT_IDENTIFY], CONF_TYPE_FLAG, CONF_GLOBAL, 0, MSGL_V, NULL},
	{"-help", help_text, CONF_TYPE_PRINT, CONF_NOCFG|CONF_GLOBAL, 0, 0, NULL},
	{"help", help_text, CONF_TYPE_PRINT, CONF_NOCFG|CONF_GLOBAL, 0, 0, NULL},
	{"h", help_text, CONF_TYPE_PRINT, CONF_NOCFG|CONF_GLOBAL, 0, 0, NULL},

	{"vd", vd_conf, CONF_TYPE_SUBCONFIG, 0, 0, 0, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};

#endif /* MPLAYER_CFG_MPLAYER_H */
