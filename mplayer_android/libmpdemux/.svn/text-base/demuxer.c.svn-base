/*
 * DEMUXER v2.5
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
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "m_config.h"

#include "libvo/fastmemcpy.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "mf.h"
#include "dbg_tool.h"

#include "libaf/af_format.h"
#include "libmpcodecs/dec_teletext.h"

#ifdef CONFIG_ASS
#include "libass/ass.h"
#include "libass/ass_mp.h"
#endif

#ifdef CONFIG_LIBAVCODEC
#include "libavcodec/avcodec.h"
#if MP_INPUT_BUFFER_PADDING_SIZE < FF_INPUT_BUFFER_PADDING_SIZE
#error MP_INPUT_BUFFER_PADDING_SIZE is too small!
#endif
#endif

#ifdef CONFIG_DRM_ENABLE
// WT, 100615, drm debug api header
#include <fcntl.h>
#include "libmpwmdrm/DxDrmDebugApi.h"
#endif

#include "stream/stream_dvdnav.h"
extern DvdnavState   dvdnavstate;

// This is quite experimental, in particular it will mess up the pts values
// in the queue - on the other hand it might fix some issues like generating
// broken files with mencoder and stream copy.
// Better leave it disabled for now, if we find no use for it this code should
// just be removed again.

//////////////////////////////
                                            //
#define  jfueng_2011_0311     //
                                            //
/////////////////////////////

#define PARSE_ON_ADD 0
/* Carlos add this defined for max check count when using DVB-T check audio decoder 2010-10-13 */
#define MAX_DVBT_READDATE_TRY_COUNT	2000
#define MAX_DVBT_FILL_CHECK_COUNT 200

#ifdef YOUTUBE_RECONNECT
extern int youtube;
double ds_last_pts = 0;
#endif

#ifdef  jfueng_2011_0311
extern unsigned int  audio_format_ref;
extern int    a_long_quiet_SP_yes;
extern int    a_long_quiet_SP_cnt;
int  audio_long_period= 0;
int  is_vob = 0;
int  have_audio_long_period = 0;	 
#endif


//charleslin 20100603
#ifdef DEMUX_THREAD

#include "pthread-macro.h"

#ifdef ADAPTIVE_THRESHOLD
#define MIN_QUEUE_LEN	20
#define MAX_QUEUE_LEN	300
#define IDLE_PERCENT1	0.6
#define IDLE_PERCENT2	0.4
#define QUEUE_LOW_LVL	0.5
#endif

#ifdef HAS_DVBIN_SUPPORT
#include "libmpdemux/demux_ts.h"
#endif // end of #ifdef HAS_DVBIN_SUPPORT

extern int my_current_pts;
extern int set_pause_lock;

extern char video_names[];
extern int audio_not_support;

int demuxthread = 0;
int vthreshold = 30;
int athreshold = 10;
int seeklocked = 0;
static int demuxstop = 0;
static int demuxrun = 0;
static pthread_t demuxtid = 0;
static pthread_cond_t fillcond;
static pthread_mutex_t fillmtx;
#ifdef DS_WAKEUP
static pthread_cond_t vidcond;
static pthread_mutex_t vidmtx;
static pthread_cond_t audcond;
static pthread_mutex_t audmtx;
#endif
static pthread_mutex_t demuxmtx;
static pthread_mutex_t seekmtx;
static void demux_thread_quit(void);
static inline double tv2double(struct timeval *tv);
static void demux_check(demuxer_t *demuxer);
#endif

static void clear_parser(sh_common_t *sh);
void resync_video_stream(sh_video_t *sh_video);
void resync_audio_stream(sh_audio_t *sh_audio);

// Demuxer list
extern const demuxer_desc_t demuxer_desc_rawaudio;
extern const demuxer_desc_t demuxer_desc_rawvideo;
extern const demuxer_desc_t demuxer_desc_tv;
extern const demuxer_desc_t demuxer_desc_mf;
extern const demuxer_desc_t demuxer_desc_avi;
extern const demuxer_desc_t demuxer_desc_y4m;
extern const demuxer_desc_t demuxer_desc_asf;
extern const demuxer_desc_t demuxer_desc_real;
extern const demuxer_desc_t demuxer_desc_smjpeg;
extern const demuxer_desc_t demuxer_desc_matroska;
extern const demuxer_desc_t demuxer_desc_realaudio;
extern const demuxer_desc_t demuxer_desc_vqf;
extern const demuxer_desc_t demuxer_desc_mov;
extern const demuxer_desc_t demuxer_desc_vivo;
extern const demuxer_desc_t demuxer_desc_fli;
extern const demuxer_desc_t demuxer_desc_film;
extern const demuxer_desc_t demuxer_desc_roq;
extern const demuxer_desc_t demuxer_desc_gif;
extern const demuxer_desc_t demuxer_desc_ogg;
extern const demuxer_desc_t demuxer_desc_avs;
extern const demuxer_desc_t demuxer_desc_pva;
extern const demuxer_desc_t demuxer_desc_nsv;
extern const demuxer_desc_t demuxer_desc_mpeg_ts;
extern const demuxer_desc_t demuxer_desc_lmlm4;
extern const demuxer_desc_t demuxer_desc_mpeg_ps;
extern const demuxer_desc_t demuxer_desc_mpeg_pes;
extern const demuxer_desc_t demuxer_desc_mpeg_es;
extern const demuxer_desc_t demuxer_desc_mpeg_gxf;
extern const demuxer_desc_t demuxer_desc_mpeg4_es;
extern const demuxer_desc_t demuxer_desc_h264_es;
extern const demuxer_desc_t demuxer_desc_rawdv;
extern const demuxer_desc_t demuxer_desc_mpc;
extern const demuxer_desc_t demuxer_desc_audio;
extern const demuxer_desc_t demuxer_desc_xmms;
extern const demuxer_desc_t demuxer_desc_mpeg_ty;
extern const demuxer_desc_t demuxer_desc_rtp;
extern const demuxer_desc_t demuxer_desc_rtp_nemesi;
extern const demuxer_desc_t demuxer_desc_lavf;
extern const demuxer_desc_t demuxer_desc_lavf_preferred;
extern const demuxer_desc_t demuxer_desc_aac;
extern const demuxer_desc_t demuxer_desc_ac3;
extern const demuxer_desc_t demuxer_desc_nut;
extern const demuxer_desc_t demuxer_desc_mng;
extern const demuxer_desc_t demuxer_desc_aac_adif;    //HONDA_ADIF
#ifdef QT_SUPPORT_DVBT
extern const demuxer_desc_t demuxer_desc_skyts;
#endif /* end of QT_SUPPORT_DVBT */
extern const demuxer_desc_t demuxer_desc_ape; //larry for ape

#if 1	// Raymond 2009/06/05
extern unsigned long long playPreRoll;
extern unsigned int asfHeaderSize;
extern unsigned int GotASFHeader;	// Raymond 2010/05/26
extern int asf_found_index_flag;
extern int is_dvdnav;
#endif
extern int VC1_has_framePrefix;

#ifdef SKY_HTTP
extern double sky_http_pts;
#endif
extern int upnp;
/* Please do not add any new demuxers here. If you want to implement a new
 * demuxer, add it to libavformat, except for wrappers around external
 * libraries and demuxers requiring binary support. */

const demuxer_desc_t *const demuxer_list[] = {

#ifndef BUILD_MPLAYER_LITE	//Barry 2010-10-27
    &demuxer_desc_rawvideo,
    &demuxer_desc_mf,
    &demuxer_desc_lmlm4,
    &demuxer_desc_fli,
    &demuxer_desc_film,
    &demuxer_desc_pva,
    &demuxer_desc_nsv,
    &demuxer_desc_roq,
    &demuxer_desc_y4m,
    &demuxer_desc_smjpeg,
    &demuxer_desc_vqf,
    &demuxer_desc_mpeg_ty,
#endif
    &demuxer_desc_rawaudio,
#ifdef CONFIG_TV
    &demuxer_desc_tv,
#endif
#ifdef CONFIG_LIBAVFORMAT
    &demuxer_desc_lavf_preferred,
#endif
    &demuxer_desc_avi,
    &demuxer_desc_asf,
#ifndef BUILD_C000000_UNUSED_CODEC	//Barry 2011-10-06
    &demuxer_desc_real,
    &demuxer_desc_realaudio,
#endif
    &demuxer_desc_matroska,
    &demuxer_desc_mov,
    &demuxer_desc_vivo,
    &demuxer_desc_aac_adif,        //HONDA_ADIF
    &demuxer_desc_ape,        //larry 
#ifdef CONFIG_GIF
    &demuxer_desc_gif,
#endif
#ifdef CONFIG_OGGVORBIS
    &demuxer_desc_ogg,
#endif
#ifdef CONFIG_WIN32DLL
    &demuxer_desc_avs,
#endif
    &demuxer_desc_mpeg_ts,
    &demuxer_desc_mpeg_ps,
    &demuxer_desc_mpeg_pes,
    &demuxer_desc_mpeg_es,
    &demuxer_desc_mpeg_gxf,
    &demuxer_desc_mpeg4_es,
    &demuxer_desc_h264_es,
    &demuxer_desc_audio,
#ifdef CONFIG_LIVE555
    &demuxer_desc_rtp,
#endif
#ifdef CONFIG_LIBNEMESI
    &demuxer_desc_rtp_nemesi,
#endif
#ifdef CONFIG_LIBAVFORMAT
    &demuxer_desc_lavf,
#endif
#ifdef CONFIG_MUSEPACK
    &demuxer_desc_mpc,
#endif
#ifdef CONFIG_LIBDV095
    &demuxer_desc_rawdv,
#endif
    &demuxer_desc_aac,
    &demuxer_desc_ac3,
#ifdef CONFIG_LIBNUT
    &demuxer_desc_nut,
#endif
#ifdef CONFIG_XMMS
    &demuxer_desc_xmms,
#endif
#ifdef CONFIG_MNG
    &demuxer_desc_mng,
#endif
#ifdef QT_SUPPORT_DVBT
#ifdef CONFIG_DVB
    &demuxer_desc_skyts,
#endif
#endif /* end of QT_SUPPORT_DVBT */
    /* Please do not add any new demuxers here. If you want to implement a new
     * demuxer, add it to libavformat, except for wrappers around external
     * libraries and demuxers requiring binary support. */
    NULL
};

void free_demuxer_stream(demux_stream_t *ds)
{
    ds_free_packs(ds);
    free(ds);
}

demux_stream_t *new_demuxer_stream(struct demuxer *demuxer, int id)
{
    demux_stream_t *ds = malloc(sizeof(demux_stream_t));
    *ds = (demux_stream_t){
        .id = id,
        .demuxer = demuxer,
        .asf_seq = -1,
    };
#ifdef DEMUX_THREAD
    pthread_mutex_init(&ds->dsmtx, NULL);
#endif
    return ds;
}


/**
 * Get demuxer description structure for a given demuxer type
 *
 * @param file_format    type of the demuxer
 * @return               structure for the demuxer, NULL if not found
 */
static const demuxer_desc_t *get_demuxer_desc_from_type(int file_format)
{
    int i;

    for (i = 0; demuxer_list[i]; i++)
        if (file_format == demuxer_list[i]->type)
            return demuxer_list[i];

    return NULL;
}


demuxer_t *new_demuxer(stream_t *stream, int type, int a_id, int v_id,
                       int s_id, char *filename)
{
    demuxer_t *d = malloc(sizeof(demuxer_t));
    memset(d, 0, sizeof(demuxer_t));
    d->stream = stream;
    d->stream_pts = MP_NOPTS_VALUE;
    d->reference_clock = MP_NOPTS_VALUE;
    d->movi_start = stream->start_pos;
    d->movi_end = stream->end_pos;
    d->seekable = 1;
    d->synced = 0;
    d->filepos = -1;
    d->audio = new_demuxer_stream(d, a_id);
    d->video = new_demuxer_stream(d, v_id);
    d->sub = new_demuxer_stream(d, s_id);
    d->type = type;
//Robert 20100712 new VideoBuffer Idx
    d->need_double_copy = 1;

    if (type)
        if (!(d->desc = get_demuxer_desc_from_type(type)))
            mp_msg(MSGT_DEMUXER, MSGL_ERR,
                   "BUG! Invalid demuxer type in new_demuxer(), "
                   "big troubles ahead.");
    if (filename) // Filename hack for avs_check_file
        d->filename = strdup(filename);
    stream->eof = 0;
    stream_seek(stream, stream->start_pos);


#ifdef CONFIG_DRM_ENABLE
    // WT, 100617, add DRM init
 // Discretix integration Start 
  //GST_INFO_OBJECT (demux, "DX: initializiang Discretix DRM client ");
  printf("start to enter DRM Init\n");

    d->lastResult = DX_SUCCESS;
    d->drmFileOpened = DX_FALSE;
    d->clientInitialized = DX_FALSE;
    d->fileIsDrmProtected = DX_FALSE;

  //Loading the Drm config file
    d->lastResult = DxLoadConfigFile("/data/DxDrm/DxDrmConfig.txt");
    if(d->lastResult != DX_SUCCESS)
    {
       	printf("DX: ERROR(%d) - Discretix configuration file not found\n", d->lastResult);
    }

	//Initialize Drm Client
    d->lastResult = DxDrmClient_Init();
    if(d->lastResult == DX_SUCCESS)
    {
         d->clientInitialized = DX_TRUE;
      	printf("DX: Discretix client intialized successfully\n");
    }
    else 
    {
    	printf("DX: ERROR - Discretix client initialization failed with error: %d\n",d->lastResult);
    }
  // Discretix integration End 
	#endif

    return d;
}

extern int dvdsub_id;

sh_sub_t *new_sh_sub_sid(demuxer_t *demuxer, int id, int sid)
{
    if (id > MAX_S_STREAMS - 1 || id < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_WARN,
               "Requested sub stream id overflow (%d > %d)\n", id,
               MAX_S_STREAMS);
        return NULL;
    }
    if (demuxer->s_streams[id])
        mp_msg(MSGT_DEMUXER, MSGL_WARN, "Sub stream %i redefined\n", id);
    else {
        sh_sub_t *sh = calloc(1, sizeof(sh_sub_t));
        demuxer->s_streams[id] = sh;
        sh->sid = sid;
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SUBTITLE_ID=%d\n", sid);
    }

    extern int dvdnav_resume;
    extern int dvdnav_movie_sub_id;
    extern int tmp_sid;
//printf("dvdsub_id[%d],id[%d],sid[%d],dvdnav_resume[%d],dvdnav_movie_sub_id[%d],tmp_sid[%d]\n",dvdsub_id,id,sid,dvdnav_resume,dvdnav_movie_sub_id,tmp_sid);
    if(is_dvdnav)
    {
    	if(dvdnav_resume)
    	{
    		if (sid == dvdnav_movie_sub_id) {
    		    demuxer->sub->id = id;
	    	    demuxer->sub->sh = demuxer->s_streams[id];
	    	}
    	}
    	else
    	{
    		if (sid == dvdsub_id) {
	    	    demuxer->sub->id = id;
	    	    demuxer->sub->sh = demuxer->s_streams[id];
	    	}
    	}
    }
    else
    {
    	if (sid == dvdsub_id) {
    	    demuxer->sub->id = id;
    	    demuxer->sub->sh = demuxer->s_streams[id];
    	}
    }
    return demuxer->s_streams[id];
}

static void free_sh_sub(sh_sub_t *sh)
{
    mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_sub at %p\n", sh);
    free(sh->extradata);
#ifdef CONFIG_ASS
    if (sh->ass_track)
        ass_free_track(sh->ass_track);
#endif
    free(sh->lang);
#ifdef CONFIG_LIBAVCODEC
    clear_parser((sh_common_t *)sh);
#endif
    free(sh);
}

sh_audio_t *new_sh_audio_aid(demuxer_t *demuxer, int id, int aid)
{
    if (id > MAX_A_STREAMS - 1 || id < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_WARN,
               "Requested audio stream id overflow (%d > %d)\n", id,
               MAX_A_STREAMS);
        return NULL;
    }
    if (demuxer->a_streams[id])
        mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_AudioStreamRedefined, id);
    else {
        sh_audio_t *sh = calloc(1, sizeof(sh_audio_t));
        mp_msg(MSGT_DEMUXER, MSGL_V, MSGTR_FoundAudioStream, id);
        demuxer->a_streams[id] = sh;
        sh->aid = aid;
        sh->ds = demuxer->audio;
        // set some defaults
        sh->samplesize = 2;
        sh->sample_format = AF_FORMAT_S16_NE;
        sh->audio_out_minsize = 8192;   /* default size, maybe not enough for Win32/ACM */
        sh->pts = MP_NOPTS_VALUE;
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AUDIO_ID=%d\n", aid);
    }
    return demuxer->a_streams[id];
}

void free_sh_audio(demuxer_t *demuxer, int id)
{
    sh_audio_t *sh = demuxer->a_streams[id];
    demuxer->a_streams[id] = NULL;
    mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_audio at %p\n", sh);
    free(sh->wf);
//    if(demuxer->file_format != DEMUXER_TYPE_LAVF && demuxer->file_format != DEMUXER_TYPE_LAVF_PREFERRED) free(sh->codecdata);		//Fuchun 2010.04.20
    free(sh->codecdata);	//Barry 2010-05-20
    free(sh->lang);
#ifdef CONFIG_LIBAVCODEC
    clear_parser((sh_common_t *)sh);
#endif
    free(sh);
}

sh_video_t *new_sh_video_vid(demuxer_t *demuxer, int id, int vid)
{
    if (id > MAX_V_STREAMS - 1 || id < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_WARN,
               "Requested video stream id overflow (%d > %d)\n", id,
               MAX_V_STREAMS);
        return NULL;
    }
    if (demuxer->v_streams[id])
        mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_VideoStreamRedefined, id);
    else {
        sh_video_t *sh = calloc(1, sizeof(sh_video_t));
        mp_msg(MSGT_DEMUXER, MSGL_V, MSGTR_FoundVideoStream, id);
        demuxer->v_streams[id] = sh;
        sh->vid = vid;
        sh->ds = demuxer->video;
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_ID=%d\n", vid);
    }
    return demuxer->v_streams[id];
}

void free_sh_video(sh_video_t *sh)
{
    mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_video at %p\n", sh);
    free(sh->bih);
#ifdef CONFIG_LIBAVCODEC
    clear_parser((sh_common_t *)sh);
#endif
    //rywu20090601++
    //free memory 
    if(sh->rmvb_sequence_header) 
    {
        //printf("<mp> free type_specific data\n");
        free(sh->rmvb_sequence_header);
        sh->rmvb_sequence_header        = 0;
        sh->rmvb_sequence_header_len    = 0;
    }
    //rywu20090601--

	//Fuchun 2010.04.21
	if(sh->mpeg4_header)
	{
		sh->mpeg4_header = 0;
		sh->mpeg4_header_len = 0;
	}
    free(sh);
}

void free_demuxer(demuxer_t *demuxer)
{
    int i;

//charleslin 20100614
#ifdef DEMUX_THREAD
    demux_thread_quit();
#endif

    mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing %s demuxer at %p\n",
           demuxer->desc->shortdesc, demuxer);
    if (demuxer->desc->close)
        demuxer->desc->close(demuxer);
    // Very ugly hack to make it behave like old implementation
    if (demuxer->desc->type == DEMUXER_TYPE_DEMUXERS)
        goto skip_streamfree;
    // free streams:
    for (i = 0; i < MAX_A_STREAMS; i++)
        if (demuxer->a_streams[i])
            free_sh_audio(demuxer, i);
    for (i = 0; i < MAX_V_STREAMS; i++)
        if (demuxer->v_streams[i])
            free_sh_video(demuxer->v_streams[i]);
    for (i = 0; i < MAX_S_STREAMS; i++)
        if (demuxer->s_streams[i])
            free_sh_sub(demuxer->s_streams[i]);
    // free demuxers:
    free_demuxer_stream(demuxer->audio);
    free_demuxer_stream(demuxer->video);
    free_demuxer_stream(demuxer->sub);
 skip_streamfree:
    if (demuxer->info) {
        for (i = 0; demuxer->info[i] != NULL; i++)
            free(demuxer->info[i]);
        free(demuxer->info);
    }
    free(demuxer->filename);
    if (demuxer->chapters) {
        for (i = 0; i < demuxer->num_chapters; i++)
            free(demuxer->chapters[i].name);
        free(demuxer->chapters);
    }
    if (demuxer->attachments) {
        for (i = 0; i < demuxer->num_attachments; i++) {
            free(demuxer->attachments[i].name);
            free(demuxer->attachments[i].type);
            free(demuxer->attachments[i].data);
        }
        free(demuxer->attachments);
    }
    if (demuxer->teletext)
        teletext_control(demuxer->teletext, TV_VBI_CONTROL_STOP, NULL);
#ifdef CONFIG_DRM_ENABLE
// WT, 100630, close DRM client
if (demuxer->clientInitialized == DX_TRUE)
{
	if (demuxer->drmFileOpened == DX_TRUE)
	{
		//Barry 2011-12-02
		int er;
		er = DxDrmStream_HandleConsumptionEvent(demuxer->drmStream, DX_EVENT_STOP);
		if (er != DX_SUCCESS)
		    printf("DX:ERROR - stop event failed due to error %d\n", er);
		else
		    printf("DX:DRM stop\n");

		DxDrmStream_Close(&demuxer->drmStream);
		printf("DX:Close the last DRM stream\n");
	}
	
	DxDrmClient_Terminate();
	demuxer->clientInitialized == DX_FALSE;
	printf ("DX:Close last DRM client\n");
}
#endif

    free(demuxer);
}


static void ds_add_packet_internal(demux_stream_t *ds, demux_packet_t *dp)
{
#ifdef DEBUG_ADD_PACKET_PTS
	extern unsigned int showpts;
	extern unsigned int showvideo;
	extern unsigned int showaudio;
	extern unsigned int showsub;
#endif // end of DEBUG_ADD_PACKET_PTS

#ifdef STATISTICAL_THRESHOLD
	static struct timeval t1 = { 0, 0 };
	struct timeval t2;
	double aavg, vavg, ashort, vshort, along, vlong;
	double tdiff;
	static double ttotal = 0;
	static unsigned int apkts = 0;
	static unsigned int vpkts = 0;
	static unsigned int atotal = 0;
	static unsigned int vtotal = 0;

	if(t1.tv_sec == 0){
		gettimeofday(&t1, NULL);
	}

	if(ds == ds->demuxer->audio)
		apkts++;
	else
		vpkts++;

	gettimeofday(&t2, NULL);
	tdiff = tv2double(&t2) - tv2double(&t1);
	if(tdiff >= 5){
		atotal += apkts;
		vtotal += vpkts;
		ttotal += tdiff;
		
		ashort = (double)apkts / tdiff;
		vshort = (double)vpkts / tdiff;
		//printf("short-term average aavg %.2f vavg %.2f \n", aavg, vavg);
		
		along = (double)atotal / ttotal;
		vlong = (double)vtotal / ttotal;
		//printf("long-term average aavg %.2f vavg %.2f \n", aavg, vavg);

		printf("ashort %.2f vshort %.2f along %.2f vlong %.2f \n", ashort, vshort, along, vlong);

		gettimeofday(&t1, NULL);
		apkts = 0;
		vpkts = 0;

		if(demuxthread){
			aavg = along;
			vavg = vlong;

			if(aavg < 1)
				athreshold = 1;
			else if(aavg > 300)
				athreshold = 300;
			else
				athreshold = aavg + 0.5;

			if(vavg < 1)
				vthreshold = 1;
			else if(vavg > 300)
				vthreshold = 300;
			else
				vthreshold = vavg + 0.5;
			//printf("ath %d vth %d \n", athreshold, vthreshold);
		}
	}else if(t2.tv_sec < t1.tv_sec){
		gettimeofday(&t1, NULL);
		apkts = 0;
		vpkts = 0;
		printf("%s:%d timer reset\n", __func__, __LINE__);
	}
#endif
    // append packet to DS stream:
//	if (dp->buffer)	//Barry 2010-10-20 disable, if dp->len = 0, need to repeat previous frame
	{
#ifdef DEMUX_THREAD
		pthread_mutex_lock(&ds->dsmtx);
#endif
		++ds->packs;
		ds->bytes += dp->len;
		if (ds->last) {
			// next packet in stream
			ds->last->next = dp;
			ds->last = dp;
		} else {
			// first packet in stream
			ds->first = ds->last = dp;
		}
#ifdef DEMUX_THREAD
		pthread_mutex_unlock(&ds->dsmtx);
#endif
	}
//	else
//		printf("### In [%s][%d], buffer is NULL ds->packs[%d] ds->bytes[%d] dp->len[%d]###\n", __func__, __LINE__, ds->packs, ds->bytes, dp->len);
#ifdef DS_WAKEUP
    if(ds == ds->demuxer->video)
      pwake_up(&vidcond, &vidmtx);
    else if(ds == ds->demuxer->audio)
      pwake_up(&audcond, &audmtx);
#endif
#ifdef DEBUG_ADD_PACKET_PTS
	if((showpts | showvideo) && ds == ds->demuxer->video)
		printf("DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d S=%d]\n",(ds == ds->demuxer->audio) ? "d_audio" : (ds == ds->demuxer->video) ? "d_video" : "d_sub", dp->len, dp->pts, (unsigned int) dp->pos, ds->demuxer->audio->packs, ds->demuxer->video->packs, ds->demuxer->sub->packs);
	if((showpts | showsub) && ds == ds->demuxer->sub)
		printf("DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d S=%d]\n",(ds == ds->demuxer->audio) ? "d_audio" : (ds == ds->demuxer->video) ? "d_video" : "d_sub", dp->len, dp->pts, (unsigned int) dp->pos, ds->demuxer->audio->packs, ds->demuxer->video->packs, ds->demuxer->sub->packs);
	if((showpts | showaudio) && ds == ds->demuxer->audio)
		printf("DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d S=%d]\n",(ds == ds->demuxer->audio) ? "d_audio" : (ds == ds->demuxer->video) ? "d_video" : "d_sub", dp->len, dp->pts, (unsigned int) dp->pos, ds->demuxer->audio->packs, ds->demuxer->video->packs, ds->demuxer->sub->packs);
#endif // end of DEBUG_ADD_PACKET_PTS		
#if 0
if(ds == ds->demuxer->audio){
	static int dfd = 0;
	dump_to_file(&dfd, dp->buffer, dp->len);
}
#endif
#ifdef YOUTUBE_RECONNECT
    if(youtube && dp->pts != MP_NOPTS_VALUE)
      ds_last_pts = dp->pts;
#endif

    mp_dbg(MSGT_DEMUXER, MSGL_DBG2,
           "DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d]\n",
           (ds == ds->demuxer->audio) ? "d_audio" : "d_video", dp->len,
           dp->pts, (unsigned int) dp->pos, ds->demuxer->audio->packs,
           ds->demuxer->video->packs);
}

#ifdef CONFIG_LIBAVCODEC
static void allocate_parser(AVCodecContext **avctx, AVCodecParserContext **parser, unsigned format)
{
    enum CodecID codec_id = CODEC_ID_NONE;
    extern int avcodec_initialized;
    if (!avcodec_initialized) {
        avcodec_init();
        avcodec_register_all();
        avcodec_initialized = 1;
    }
    switch (format) {
    case 0x2000:
    case 0x332D6361:
    case 0x332D4341:
    case MKTAG('d', 'n', 'e', 't'):
    case MKTAG('s', 'a', 'c', '3'):
        codec_id = CODEC_ID_AC3;
        break;
    case MKTAG('E', 'A', 'C', '3'):
        codec_id = CODEC_ID_EAC3;
        break;
    case 0x2001:
    case 0x86:
        codec_id = CODEC_ID_DTS;
        break;
    case MKTAG('M', 'L', 'P', ' '):
        codec_id = CODEC_ID_MLP;
        break;
    case 0x55:
    case 0x5500736d:
    case MKTAG('.', 'm', 'p', '3'):
    case MKTAG('M', 'P', 'E', ' '):
    case MKTAG('L', 'A', 'M', 'E'):
        codec_id = CODEC_ID_MP3;
        break;
    case 0x50:
    case MKTAG('.', 'm', 'p', '2'):
    case MKTAG('.', 'm', 'p', '1'):
        codec_id = CODEC_ID_MP2;
        break;
    case MKTAG('T', 'R', 'H', 'D'):
        codec_id = CODEC_ID_TRUEHD;
        break;
    }
    if (codec_id != CODEC_ID_NONE) {
        *avctx = avcodec_alloc_context();
        if (!*avctx)
            return;
        *parser = av_parser_init(codec_id);
        if (!*parser)
            av_freep(avctx);
    }
}

static void get_parser(sh_common_t *sh, AVCodecContext **avctx, AVCodecParserContext **parser)
{
    *avctx  = NULL;
    *parser = NULL;

    if (!sh || !sh->needs_parsing)
        return;

    *avctx  = sh->avctx;
    *parser = sh->parser;
    if (*parser)
        return;

    allocate_parser(avctx, parser, sh->format);
    sh->avctx  = *avctx;
    sh->parser = *parser;
}

int ds_parse(demux_stream_t *ds, uint8_t **buffer, int *len, double pts, off_t pos)
{
    AVCodecContext *avctx;
    AVCodecParserContext *parser;
    get_parser(ds->sh, &avctx, &parser);
    if (!parser)
        return *len;
    return av_parser_parse2(parser, avctx, buffer, len, *buffer, *len, pts, pts, pos);
}

static void clear_parser(sh_common_t *sh)
{
    av_parser_close(sh->parser);
    sh->parser = NULL;
    av_freep(&sh->avctx);
}

void ds_clear_parser(demux_stream_t *ds)
{
    if (!ds->sh)
        return;
    clear_parser(ds->sh);
}
#endif

void ds_add_packet(demux_stream_t *ds, demux_packet_t *dp)
{
#if PARSE_ON_ADD && defined(CONFIG_LIBAVCODEC)
    int len = dp->len;
    int pos = 0;
    while (len > 0) {
        uint8_t *parsed_start = dp->buffer + pos;
        int parsed_len = len;
        int consumed = ds_parse(ds->sh, &parsed_start, &parsed_len, dp->pts, dp->pos);
        pos += consumed;
        len -= consumed;
        if (parsed_start == dp->buffer && parsed_len == dp->len) {
            ds_add_packet_internal(ds, dp);
        } else if (parsed_len) {
            demux_packet_t *dp2 = new_demux_packet(parsed_len);
            dp2->pos = dp->pos;
            dp2->pts = dp->pts; // should be parser->pts but that works badly
            memcpy(dp2->buffer, parsed_start, parsed_len);
            ds_add_packet_internal(ds, dp2);
        }
    }
#else
    ds_add_packet_internal(ds, dp);
#endif
}

void ds_read_packet(demux_stream_t *ds, stream_t *stream, int len,
                    double pts, off_t pos, int flags)
{
    demux_packet_t *dp = new_demux_packet(len);
    len = stream_read(stream, dp->buffer, len);
    resize_demux_packet(dp, len);
    dp->pts = pts;
    dp->pos = pos;
    dp->flags = flags;
    // append packet to DS stream:
    ds_add_packet(ds, dp);
}

// return value:
//     0 = EOF or no stream found or invalid type
//     1 = successfully read a packet

int demux_fill_buffer(demuxer_t *demux, demux_stream_t *ds)
{
//printf("in demux_fill_buffer()\n");
if (demux->desc->type == DEMUXER_TYPE_RTP)
{
    return demux->desc->fill_buffer(demux, ds);
}
else
{
#ifdef DEMUX_THREAD
    //charleslin 20100611
    /* Main thread and demux thread may call this function at the same time.
       Use mutex to protect critical section. */
    int ret;
    pthread_mutex_lock(&demuxmtx);
    ret = demux->desc->fill_buffer(demux, ds);
    pthread_mutex_unlock(&demuxmtx);
    return ret;
#else
    // Note: parameter 'ds' can be NULL!
    return demux->desc->fill_buffer(demux, ds);
#endif
}
}

// return value:
//     0 = EOF
//     1 = successful
#define MAX_ACUMULATED_PACKETS 64
int ds_fill_buffer(demux_stream_t *ds)
{
    int timeout = 0;
    demuxer_t *demux = ds->demuxer;
    if (ds->current)
        free_demux_packet(ds->current);
    ds->current = NULL;
    if (mp_msg_test(MSGT_DEMUXER, MSGL_DBG3)) {
        if (ds == demux->audio)
            mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
                   "ds_fill_buffer(d_audio) called\n");
        else if (ds == demux->video)
            mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
                   "ds_fill_buffer(d_video) called\n");
        else if (ds == demux->sub)
            mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(d_sub) called\n");
        else
            mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
                   "ds_fill_buffer(unknown 0x%X) called\n", (unsigned int) ds);
    }
    
#if 1	//Fuchun 2009.12.02
	if(speed_mult  !=  0 || audio_not_support == 1)
	{
		ds_free_packs(demux->audio);

		//Fuchun 2010.03.12
		if(speed_mult < 0 
			&& (demux->type == DEMUXER_TYPE_REAL || demux->type == DEMUXER_TYPE_MATROSKA))
			ds_free_packs(demux->video);
	}
#endif

    while (1) {
#ifdef DEMUX_THREAD
        pthread_mutex_lock(&ds->dsmtx);
#endif
        if (ds->packs) {

#ifdef  jfueng_2011_0311

if(ds==demux->audio)  
audio_long_period = 0; 

#endif

            demux_packet_t *p = ds->first;
#if 0
            if (demux->reference_clock != MP_NOPTS_VALUE) {
                if (   p->pts != MP_NOPTS_VALUE
                    && p->pts >  demux->reference_clock
                    && ds->packs < MAX_ACUMULATED_PACKETS) {
                    if (demux_fill_buffer(demux, ds))
                        continue;
                }
            }
#endif
            // copy useful data:
			if (p)
			{
            ds->buffer = p->buffer;
            ds->buffer_pos = 0;
            ds->buffer_size = p->len;
            ds->pos = p->pos;
            ds->dpos += p->len; // !!!
            ++ds->pack_no;
            if (p->pts != MP_NOPTS_VALUE) {
                ds->pts = p->pts;
                ds->pts_bytes = 0;

                if(speed_mult  !=  0 && !is_dvdnav)
                    my_current_pts=ds->pts;
            }
            ds->pts_bytes += p->len;    // !!!
            if (p->stream_pts != MP_NOPTS_VALUE)
                demux->stream_pts = p->stream_pts;
            ds->flags = p->flags;
            // unlink packet:
            ds->bytes -= p->len;
            ds->current = p;
            ds->first = p->next;
            if (!ds->first)
                ds->last = NULL;
            --ds->packs;
			}
			else
			{
				/* In dvb-t signal is too low, sometimes we meet abnormal case, p is NULL point */
				printf("@@@@@@ Video buf is illegal address [0x%.8x] @@@@\n", p);
#ifdef QT_SUPPORT_DVBT				
#ifdef HAS_DVBIN_SUPPORT
				set_skydvb_wrokaround(NEED_SEEK_CHANNEL);
#endif // end of #ifdef HAS_DVBIN_SUPPORT
#endif /* end of QT_SUPPORT_DVBT */
#ifdef DEMUX_THREAD
				pthread_mutex_unlock(&ds->dsmtx);
#endif
				return 2;
			}
#ifdef DEMUX_THREAD //charleslin 20100603
            demux_check(demux);
            pthread_mutex_unlock(&ds->dsmtx);
#endif
            return 1;
        }
#ifdef DEMUX_THREAD
        pthread_mutex_unlock(&ds->dsmtx);
#endif
        if (demux->audio->packs >= MAX_PACKS
            || demux->audio->bytes >= MAX_PACK_BYTES) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyAudioInBuffer,
                   demux->audio->packs, demux->audio->bytes);
            mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
              #if 0 //Polun 2012-01-04 for mantis 5990 皮克斯的短片精選_BD  mpctx->eof[1] 
		if(!(wait_video_or_audio_sync&AVSYNC_NORMAL) || (set_pause_lock != 0))	//Fuchun 2011.01.14
              #else
		if(!(wait_video_or_audio_sync&AVSYNC_NORMAL) || (set_pause_lock != 0) ||(demux->video && !demux->video->eof) )	
              #endif
              {
			printf("### Free audio packs, and continue playback ###\n");
			{
				demux_packet_t *dp = demux->audio->first;
				while (dp) 
				{
					demux_packet_t *dn = dp->next;
					free_demux_packet(dp);
					dp = dn;
				}
				if (demux->audio->asf_packet) 
				{
					// free unfinished .asf fragments:
					free(demux->audio->asf_packet->buffer);
					free(demux->audio->asf_packet);
					demux->audio->asf_packet = NULL;
				}
				demux->audio->first = demux->audio->last = NULL;
				demux->audio->packs = 0; // !!!!!
				demux->audio->bytes = 0;
				if (demux->audio->current)
					free_demux_packet(demux->audio->current);
				demux->audio->current = NULL;
				demux->audio->buffer = NULL;
				demux->audio->buffer_pos = demux->audio->buffer_size;
				demux->audio->pts_bytes = 0;
			}
		}
		else
		{
			//Barry 2010-05-18
			demux->audio->eof = 1;
			demux->video->eof = 1;
			break;
		}
        }

#ifndef  jfueng_2011_0311  //JF 2011/03/01  :: remove for  change  procedure flow  older///////////		
        if (demux->video->packs >= MAX_PACKS
            || demux->video->bytes >= MAX_PACK_BYTES) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,
                   demux->video->packs, demux->video->bytes);
            mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);

		//Barry 2010-05-18
		demux->audio->eof = 1;
		demux->video->eof = 1;
            break;
        }

#endif  //JF 2011/03/01    ////////////////////////// /////////////////////////////////

#ifdef DEMUX_THREAD
	//printf("!\n");
	//Polun 2011-08-22 mantis5681 Panasonic-LC5_1M.MOV & i85.avi
	//if(demuxrun && !seeklocked && !timeout && speed_mult >= 0 && speed_mult <= 1){
	if(demuxrun && !seeklocked && !timeout && speed_mult >= 0 && speed_mult <= 1 && (!upnp || demux->stream->eof == 0) ){
#ifdef DS_WAKEUP
		pwake_up(&fillcond, &fillmtx);
		 if(ds == demux->video && demux->video->packs == 0 && pwait_event_timeout(&vidcond, &vidmtx, 1) == ETIMEDOUT){
			timeout = 1;
			printf("wait video timeout\n");
		}else if(ds == demux->audio && demux->audio->packs == 0 && pwait_event_timeout(&audcond, &audmtx, 1) == ETIMEDOUT){
			timeout = 1;
			printf("wait audio timeout\n");
		}
#else
		int i;
		for(i=0; i<100 && !ds->packs; i++){
			usleep(1);
		}
		//printf("%s wait %d times\n", ds == demux->audio ? "audio" : "video", i);
		if(i >= 100){
			timeout = 1;
			printf("%s:%d wait %s timedout %d \n", __func__, __LINE__, ds == demux->audio ? "audio" : "video", i);
		}
#endif
	}else
#endif
        if (!demux_fill_buffer(demux, ds)) {
#if PARSE_ON_ADD && defined(CONFIG_LIBAVCODEC)
            uint8_t *parsed_start = NULL;
            int parsed_len = 0;
            ds_parse(ds->sh, &parsed_start, &parsed_len, MP_NOPTS_VALUE, 0);
            if (parsed_len) {
                demux_packet_t *dp2 = new_demux_packet(parsed_len);
                dp2->pts = MP_NOPTS_VALUE;
                memcpy(dp2->buffer, parsed_start, parsed_len);
                ds_add_packet_internal(ds, dp2);
                continue;
            }
#endif
            mp_dbg(MSGT_DEMUXER, MSGL_DBG2,
                   "ds_fill_buffer()->demux_fill_buffer() failed\n");


#ifdef  jfueng_2011_0311
        if(ds==demux->audio)  
                audio_long_period = 0;
#endif		

            break; // EOF
        }


#ifdef  jfueng_2011_0311   //JF 2011/03/01  :: change  procedure flow  older   ///////

        if (demux->video->packs >= MAX_PACKS   || demux->video->bytes >= MAX_PACK_BYTES) {

          if(   is_vob     &&   ( audio_format_ref == 0x2000)  )
          {
		printf("audio_long_period = %d\n",audio_long_period);
			
           	if (audio_long_period == 0)        // give one chance    for  first  meet "TooManyVideoInBuffer"  after   normal  ( beggining or repeat ! )  
	    	{  
	         audio_long_period = 1; 
		  have_audio_long_period = 1;	 

  		  printf("audio_long_period  →  1 \n");
		
           	  mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,demux->video->packs, demux->video->bytes);
            	  mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
          	}
          }		
          else
          {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,demux->video->packs, demux->video->bytes);
            mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);

            if(!is_dvdnav) //Mantis:5781 Dirty prety things.iso
            {
		//Barry 2010-05-18
		demux->audio->eof = 1;
              #if 0 //Polun 2011-11-22 fixed Alexander_Trailer_1080p.mpeg audio is end on 37 sec,can't playback video.
		demux->video->eof = 1;
              #else
              if( demux->type != DEMUXER_TYPE_MPEG_PS)
		   demux->video->eof = 1;
              #endif
            }
            
            if(is_dvdnav)
            {
		extern int dvdnav_toomanypkg;
		dvdnav_toomanypkg = 1;
            }
          }
		  
            break;

        }

#endif //JF 2011/03/01    ////////////////////////// ///////////////////////


#ifdef DVBT_USING_NORMAL_METHOD
		else
		{
			if (demux->type == DEMUXER_TYPE_SKYMPEG_TS)
				return 2;
		}
#endif // end of DVBT_USING_NORMAL_METHOD
    }
    ds->buffer_pos = ds->buffer_size = 0;
    ds->buffer = NULL;
    mp_msg(MSGT_DEMUXER, MSGL_V,
           "ds_fill_buffer: EOF reached (stream: %s)  \n",
           ds == demux->audio ? "audio" : "video");
    ds->eof = 1;
    return 0;
}

int demux_read_data(demux_stream_t *ds, unsigned char *mem, int len)
{
    int x;
    int bytes = 0;
	int ret = 0;
#ifdef DVBT_USING_NORMAL_METHOD
	int retry_count = 0;
	demuxer_t *demux = ds->demuxer;
#endif // end of DVBT_USING_NORMAL_METHOD
    while (len > 0) {
        x = ds->buffer_size - ds->buffer_pos;
        if (x == 0) {
			ret = ds_fill_buffer(ds);
            if (!ret)
                return bytes;
#ifdef DVBT_USING_NORMAL_METHOD
			else 
			{
				if (ret == 2)
				{
					/* fixed seek hang issue */
					if (retry_count > MAX_DVBT_READDATE_TRY_COUNT)
					{
//						printf("#############In [%s][%d] --- retry_count [%d]#######\n", __func__, __LINE__, retry_count);
						return bytes;
					}
					else
						retry_count++;
				}
			}
        } 
		else if (x < 0)
		{
			printf("### In [%s][%d] ds->buffer_size[%d] ds->buffer_pos[%d], x[%d]\n", __func__, __LINE__, ds->buffer_size, ds->buffer_pos, x);
			return bytes;
#endif // end of DVBT_USING_NORMAL_METHOD
		}
		else
		{
            if (x > len)
                x = len;
            if (mem)
                fast_memcpy(mem + bytes, &ds->buffer[ds->buffer_pos], x);
            bytes += x;
            len -= x;
            ds->buffer_pos += x;
        }
    }
    return bytes;
}

/**
 * \brief read data until the given 3-byte pattern is encountered, up to maxlen
 * \param mem memory to read data into, may be NULL to discard data
 * \param maxlen maximum number of bytes to read
 * \param read number of bytes actually read
 * \param pattern pattern to search for (lowest 8 bits are ignored)
 * \return whether pattern was found
 */
int demux_pattern_3(demux_stream_t *ds, unsigned char *mem, int maxlen,
                    int *read, uint32_t pattern)
{
    register uint32_t head = 0xffffff00;
    register uint32_t pat = pattern & 0xffffff00;
    int total_len = 0;
	
	if(change_flag == 1)	//Fuchun 2011.01.05 fix display mosasic when switch menu to movie
	{
//		if(!dvdnavstate.dvdnav_button_on) //dolby play movie cause no display frame
		{
			change_flag = 2;
			svsd_ipc_drop_video_cnts();
			sky_set_reset_display_queue(0);
			ds_free_packs(ds->demuxer->video);
		}
//		else
//			change_flag = 0;
	}
	
#ifdef DVBT_USING_NORMAL_METHOD
  int check_fill_number = 0;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	extern int setdvb_by_pid;
	extern int stop_tuner;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
#endif // end of DVBT_USING_NORMAL_METHOD
	
    do {
        register unsigned char *ds_buf = &ds->buffer[ds->buffer_size];
        int len = ds->buffer_size - ds->buffer_pos;
        register long pos = -len;
        if (unlikely(pos >= 0)) { // buffer is empty
#ifdef DVBT_USING_NORMAL_METHOD
			if(ds->demuxer->file_format == DEMUXER_TYPE_SKYMPEG_TS)
			{
				if (get_skydvb_wrokaround()& LOST_DVBT_SIGNAL)
				{
					printf("#### DVB-T in [%s][%d]  check_format failed  , can't recevied any data, don't wait ####\n", __func__, __LINE__);
					break;
				}
				else if (get_skydvb_wrokaround()& READY_TO_DISPLAY)
				{
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
					if ((stop_tuner == 1) && (setdvb_by_pid == 1))
					{
						check_fill_number++;
						usleep(100000);
						if(check_fill_number > 50)
						{
							printf("#### DVB-T in [%s][%d]  check_format success, can't recevied any data, skydvb_need_workaround[%d] ####\n", __func__, __LINE__, get_skydvb_wrokaround());
							break;
						}
					}
					
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID

#if 0			
					check_fill_number++;
					if(check_fill_number > MAX_DVBT_FILL_CHECK_COUNT * 10)
					{
						printf("#### DVB-T in [%s][%d]  check_format success, can't recevied any data, skydvb_need_workaround[%d] ####\n", __func__, __LINE__, get_skydvb_wrokaround());
						break;
					}
#endif					
				}
				else
				{
					check_fill_number++;
					if(check_fill_number > MAX_DVBT_FILL_CHECK_COUNT)
					{
						printf("#### DVB-T in [%s][%d]  check_format failed  , can't recevied any data, skydvb_need_workaround[%d] ####\n", __func__, __LINE__, get_skydvb_wrokaround());
						break;
					}
				}
			}
#endif // #endif of DVBT_USING_NORMAL_METHOD
            ds_fill_buffer(ds);
            continue;
        }
        do {
            head |= ds_buf[pos];
            head <<= 8;
        } while (++pos && head != pat);
        len += pos;
        if (total_len + len > maxlen)
            len = maxlen - total_len;
        len = demux_read_data(ds, mem ? &mem[total_len] : NULL, len);
        total_len += len;
    } while ((head != pat || total_len < 3) && total_len < maxlen && !ds->eof);
    if (read)
        *read = total_len;
    return total_len >= 3 && head == pat;
}

void ds_free_packs(demux_stream_t *ds)
{
    demux_packet_t *dp = ds->first;
    while (dp) {
        demux_packet_t *dn = dp->next;
        free_demux_packet(dp);
        dp = dn;
    }
    if (ds->asf_packet) {
        // free unfinished .asf fragments:
        free(ds->asf_packet->buffer);
        free(ds->asf_packet);
        ds->asf_packet = NULL;
    }
    ds->first = ds->last = NULL;
    ds->packs = 0; // !!!!!
    ds->bytes = 0;
    if (ds->current)
        free_demux_packet(ds->current);
    ds->current = NULL;
    ds->buffer = NULL;
    ds->buffer_pos = ds->buffer_size;
    ds->pts = 0;
    ds->pts_bytes = 0;
}

int ds_get_packet(demux_stream_t *ds, unsigned char **start)
{
	int len;
	if (ds->buffer_pos >= ds->buffer_size)
	{
		if (!ds_fill_buffer(ds))
		{
			// EOF
			*start = NULL;
			return -1;
		}
	}
	len = ds->buffer_size - ds->buffer_pos;
	*start = &ds->buffer[ds->buffer_pos];
	ds->buffer_pos += len;

	/*johnnyke 20100728*/
	if ( (ds->demuxer->type == DEMUXER_TYPE_ASF || ds->demuxer->type == DEMUXER_TYPE_MATROSKA) && (ds == ds->demuxer->video))
	{
		unsigned int framePrefix[2];
		unsigned int timeStamp = (unsigned int)((unsigned int)(ds->pts * 1000) - playPreRoll);
		sh_video_t* sh_video = (sh_video_t*)ds->sh;

		if( sh_video->bih->biCompression ==mmioFOURCC('W','M','V','3') )
		{
			framePrefix[1] = timeStamp;

			// Raymond 2010/06/03
			unsigned int flen = len - 8;
			ds->buffer[0] = flen & 0xFF;
			ds->buffer[1] = (flen >> 8) & 0xFF;
			ds->buffer[2] = (flen >> 16) & 0xFF;
			PUT32_L(framePrefix[1], (unsigned char*)(ds->buffer+4));
		}
		//if( ds->demuxer->type == DEMUXER_TYPE_ASF && sh_video->bih->biCompression == mmioFOURCC('W','V','C','1') )
		if( !VC1_has_framePrefix && ds->demuxer->type == DEMUXER_TYPE_ASF && sh_video->bih->biCompression == mmioFOURCC('W','V','C','1') )	//Barry 2011-12-29
		{
                     unsigned char* bufd = ds->buffer; //Polun 2011-08-12 
                     framePrefix[0] = 0x0D010000;
			
			if(GotASFHeader== 1)
			{
				PUT32_L(framePrefix[0], (unsigned char*)(ds->buffer+asfHeaderSize));

				GotASFHeader = 0;
			}//Polun 2011-08-15 ++s add asf_first_start_code for mantis 5680 WVC1
                     else if(bufd[0]==0 && bufd[1]==0 && bufd[2]==1 && bufd[3]==0x0F )
                     {
				PUT32_L(framePrefix[0], (unsigned char*)(ds->buffer+asfHeaderSize));
                     }//Polun 2011-08-15 ++e
			else
			{
				PUT32_L(framePrefix[0], (unsigned char*)(ds->buffer));
			}				
		}
	}
        //Polun 2011-08-11 ++s add asf_first_start_code for mantis 5680
       extern int asf_first_start_code;
       asf_first_start_code = 0 ;
        //Polun 2011-08-11 ++e 
	return len;
}

int ds_get_packet_pts(demux_stream_t *ds, unsigned char **start, double *pts)
{
    int len;
    *pts = MP_NOPTS_VALUE;
    len = ds_get_packet(ds, start);
    //if (len < 0) /* Avoid ds->current is NULL point cause SIGSEGV, add by carlos 2010-09-29 */
    if (len <= 0)
        return len;
    // Return pts unless this read starts from the middle of a packet
    //if (len == ds->buffer_pos) /* Avoid ds->current is NULL point cause SIGSEGV, add by carlos 2010-09-29 */
    if (len == ds->buffer_pos && ds->current)
        *pts = ds->current->pts;
    return len;
}

int ds_get_packet_sub(demux_stream_t *ds, unsigned char **start)
{
    int len;
    if (ds->buffer_pos >= ds->buffer_size) {
        *start = NULL;
        if (!ds->packs)
            return -1;  // no sub
        if (!ds_fill_buffer(ds))
            return -1;  // EOF
    }
    len = ds->buffer_size - ds->buffer_pos;
    *start = &ds->buffer[ds->buffer_pos];
    ds->buffer_pos += len;
    return len;
}

#ifdef DVB_SUBTITLES
int ds_get_packet_dvbsub(demux_stream_t *ds, unsigned char **start,
                      double *pts, double *endpts)
{
    int len;
    *start = NULL;
    // initialize pts
    if (pts)
        *pts    = MP_NOPTS_VALUE;
    if (endpts)
        *endpts = MP_NOPTS_VALUE;

    if (ds->buffer_pos >= ds->buffer_size) {
        if (!ds->packs)
            return -1;  // no sub
        if (!ds_fill_buffer(ds))
            return -1;  // EOF
    }
    // only start of buffer has valid pts
    if (ds->buffer_pos == 0) {
        if (endpts)
            *endpts = ds->current->endpts;
        if (pts) {
            *pts    = ds->current->pts;
            // check if we are too early
            if (*pts != MP_NOPTS_VALUE && ds->current->pts != MP_NOPTS_VALUE &&
                ds->current->pts > *pts)
                return -1;
        }
    }
    len = ds->buffer_size - ds->buffer_pos;
    *start = &ds->buffer[ds->buffer_pos];
    ds->buffer_pos += len;
    return len;
}
#endif //end of DVB_SUBTITLES

double ds_get_next_pts(demux_stream_t *ds)
{	
    demuxer_t *demux = ds->demuxer;

	if(asf_found_index_flag == 1 && (speed_mult < 0 || speed_mult >= 2) && demux->type == DEMUXER_TYPE_ASF)
	{
		sh_video_t *sh_video = demux->video->sh;
		if(sh_video->fps != 1000 && sh_video->fps > 1) return (ds->pts + 1/sh_video->fps);
		else return (ds->pts + 0.033);
	}
	
    while (!ds->first) {
        if (demux->audio->packs >= MAX_PACKS
            || demux->audio->bytes >= MAX_PACK_BYTES) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyAudioInBuffer,
                   demux->audio->packs, demux->audio->bytes);
            mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
            return MP_NOPTS_VALUE;
        }
        if (demux->video->packs >= MAX_PACKS
            || demux->video->bytes >= MAX_PACK_BYTES) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,
                   demux->video->packs, demux->video->bytes);
            mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
            return MP_NOPTS_VALUE;
        }
        if (!demux_fill_buffer(demux, ds))
            return MP_NOPTS_VALUE;
    }
    return ds->first->pts;
}

// ====================================================================

void demuxer_help(void)
{
    int i;

    mp_msg(MSGT_DEMUXER, MSGL_INFO, "Available demuxers:\n");
    mp_msg(MSGT_DEMUXER, MSGL_INFO, " demuxer:  type  info:  (comment)\n");
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_DEMUXERS\n");
    for (i = 0; demuxer_list[i]; i++) {
        if (demuxer_list[i]->type > DEMUXER_TYPE_MAX)   // Don't display special demuxers
            continue;
        if (demuxer_list[i]->comment && strlen(demuxer_list[i]->comment))
            mp_msg(MSGT_DEMUXER, MSGL_INFO, "%10s  %2d   %s (%s)\n",
                   demuxer_list[i]->name, demuxer_list[i]->type,
                   demuxer_list[i]->info, demuxer_list[i]->comment);
        else
            mp_msg(MSGT_DEMUXER, MSGL_INFO, "%10s  %2d   %s\n",
                   demuxer_list[i]->name, demuxer_list[i]->type,
                   demuxer_list[i]->info);
    }
}


/**
 * Get demuxer type for a given demuxer name
 *
 * @param demuxer_name    string with demuxer name of demuxer number
 * @param force           will be set if demuxer should be forced.
 *                        May be NULL.
 * @return                DEMUXER_TYPE_xxx, -1 if error or not found
 */
int get_demuxer_type_from_name(char *demuxer_name, int *force)
{
    int i;
    long type_int;
    char *endptr;

    if (!demuxer_name || !demuxer_name[0])
        return DEMUXER_TYPE_UNKNOWN;
    if (force)
        *force = demuxer_name[0] == '+';
    if (demuxer_name[0] == '+')
        demuxer_name = &demuxer_name[1];
    for (i = 0; demuxer_list[i]; i++) {
        if (demuxer_list[i]->type > DEMUXER_TYPE_MAX)   // Can't select special demuxers from commandline
            continue;
        if (strcmp(demuxer_name, demuxer_list[i]->name) == 0)
            return demuxer_list[i]->type;
    }

    // No match found, try to parse name as an integer (demuxer number)
    type_int = strtol(demuxer_name, &endptr, 0);
    if (*endptr)  // Conversion failed
        return -1;
    if ((type_int > 0) && (type_int <= DEMUXER_TYPE_MAX))
        return (int) type_int;

    return -1;
}

//charleslin 20100603
#ifdef DEMUX_THREAD
extern int audio_id;
extern int video_id;

static inline double tv2double(struct timeval *tv)
{
	//printf("%u %u %.2f\n", tv->tv_sec, tv->tv_usec, (double)tv->tv_sec + (double)tv->tv_usec / 1000000.0);
	return (double)tv->tv_sec + (double)tv->tv_usec / 1000000.0;
}

static inline float vid_queue_level(demuxer_t *demuxer)
{
	return (float)demuxer->video->packs / (float)vthreshold;
}

static inline float aud_queue_level(demuxer_t *demuxer)
{
	return (float)demuxer->audio->packs / (float)athreshold;
}

static inline int vid_que_full(demuxer_t *demuxer)
{
	return video_id == -2 || demuxer->video->packs >= vthreshold;
}

static inline int aud_que_full(demuxer_t *demuxer)
{
	return audio_id == -2 || demuxer->audio->packs >= athreshold;
}

static inline int threshold_reached(demuxer_t *demuxer)
{
	if(vid_que_full(demuxer) && aud_que_full(demuxer))
		return 1;
	return 0;
}

static void demux_sighandler(int x)
{
	// close stream
	printf("%s:%d\n", __FUNCTION__, __LINE__);
	exit(0);
}

static void demux_check(demuxer_t *demuxer)
{
#if 0
	static int cnt = 0;
	static struct timeval t1 = { 0, 0 };
	struct timeval t2;
	double interval;

	if(t1.tv_sec == 0)
		gettimeofday(&t1, NULL);
	cnt++;
	gettimeofday(&t2, NULL);
	interval = tv2double(&t2) - tv2double(&t1);
	if(interval >= 5){
		printf("%d / %.2f = %.2f\n", cnt, interval, cnt / interval);
		cnt = 0;
		gettimeofday(&t1, NULL);
	}
#endif
	if(!demuxthread)
		return;
	if(!threshold_reached(demuxer))
	//if(demuxer->video->packs * 4 < vthreshold * 3 || demuxer->audio->packs * 4 < athreshold * 3)
		pwake_up(&fillcond, &fillmtx);
}

static void demux_thread_quit(void)
{
    if(demuxthread && demuxtid){
	demuxstop = 1;

	//printf("wakeup demux thread\n");
	pwake_up(&fillcond, &fillmtx);

	//printf("waiting for demux thread joined\n");
	pthread_join(demuxtid, NULL);
	//printf("demux thread joined\n");

	demuxstop = 0;
	demuxtid = 0;
    }
}

static void *demux_thread(void *data)
{
	demuxer_t *demuxer = data;
#ifdef ADAPTIVE_THRESHOLD
	double ttotal = 0;
	double tsleep = 0;
	struct timeval tstart, tend, t1, t2;
	gettimeofday(&tstart, NULL);
#endif
	demuxrun = 1;
	//printf("%s renice to %d\n", __func__, nice(-19));
	printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
	//signal(SIGTERM, demux_sighandler); // already catch SIGTERM in main()
	while(1){
#if 0
		if(vid_que_full(demuxer) && aud_que_full(demuxer)){
			//printf("w\n");
			usleep(3000);
			continue;
		}
#else
		while(threshold_reached(demuxer) || speed_mult < 0 || speed_mult > 1){
			//printf("demux full\n");
#ifdef ADAPTIVE_THRESHOLD
			gettimeofday(&t1, NULL);
#endif
			if(pwait_event_timeout(&fillcond, &fillmtx, 1) == ETIMEDOUT){
				//printf("wait a/v queue not full timedout\n");
#ifdef ADAPTIVE_THRESHOLD
				if(vthreshold > MIN_QUEUE_LEN)
					vthreshold = vthreshold * 9 / 10;
				if(athreshold > MIN_QUEUE_LEN)
					athreshold = athreshold * 9 / 10;
				printf("ath %d vth %d \n", athreshold, vthreshold);
#endif
			}

			if(demuxstop || (demuxer->video->eof && demuxer->audio->eof))
				break;

#ifdef ADAPTIVE_THRESHOLD
			gettimeofday(&t2, NULL);
			tsleep += tv2double(&t2) - tv2double(&t1);
#endif
		}

		if(demuxstop || (demuxer->video->eof && demuxer->audio->eof))
			break;

#ifdef ADAPTIVE_THRESHOLD
		gettimeofday(&tend, NULL);
		ttotal = tv2double(&tend) - tv2double(&tstart);
		//printf("%.2f / %.2f = %.2f \n", tsleep, ttotal, tsleep/ttotal);
		if(ttotal >= 5){
			double tidle = tsleep / ttotal;
			//printf("%.2f / %.2f = %.2f \n", tsleep, ttotal, tidle);
			if(tidle >= IDLE_PERCENT1 && vid_queue_level(demuxer) < QUEUE_LOW_LVL){
				if(vthreshold < MAX_QUEUE_LEN)
					vthreshold = vthreshold * 11 / 10 + 1;
			}else if(tidle >= IDLE_PERCENT2){
				if(vthreshold > MIN_QUEUE_LEN)
					vthreshold = vthreshold * 9 / 10;
			}

			if(tidle >= IDLE_PERCENT1 && aud_queue_level(demuxer) < QUEUE_LOW_LVL){
				if(athreshold < MAX_QUEUE_LEN)
						athreshold = athreshold * 11 / 10 + 1;
			}else if(tidle >= IDLE_PERCENT2){
				if(athreshold > MIN_QUEUE_LEN)
						athreshold = athreshold * 9 / 10;
			}

			//printf("idle %.0f%% ath %d vth %d \n", tidle * 100.0, athreshold, vthreshold);
			gettimeofday(&tstart, NULL);
			tsleep = 0;
		}
#endif
#endif
		//printf("v%d a%d\n", demuxer->video->packs, demuxer->audio->packs);
		//printf("veof %d aeof %d demuxstop %d\n", demuxer->video->eof,  demuxer->audio->eof, demuxstop);
		//printf("vid %d aid %d\n", video_id, audio_id);
		//printf("vf %d af %d\n", vid_que_full(demuxer), aud_que_full(demuxer));

		pthread_mutex_lock(&seekmtx);
		if(!vid_que_full(demuxer) && !demuxer->video->eof)
			demux_fill_buffer(demuxer, demuxer->video);

		if(!aud_que_full(demuxer) && !demuxer->audio->eof)
			demux_fill_buffer(demuxer, demuxer->audio);
		pthread_mutex_unlock(&seekmtx);
	}
	printf("%s ended\n", __FUNCTION__);
	demuxrun = 0;
	return NULL;
}

static inline void demux_wait_queue_full(demuxer_t *demuxer)
{
	int i;

	printf("wait until audio/video threshold is reached....\n");
	for(i=0; i< 1500; i++){
		if(threshold_reached(demuxer))
		//if(vid_que_full(demuxer))
		//if(aud_que_full(demuxer))
			break;
		usleep(1000);
	}
	printf("done\n");
}

void demux_cache_enable(demuxer_t *demuxer)
{
	int ret;
	if(demuxthread){
		ret = pthread_mutex_init(&demuxmtx, NULL);
		if(ret) printf("%s:%d demuxmtx %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_mutex_init(&seekmtx, NULL);
		if(ret) printf("%s:%d seekmtx %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_mutex_init(&fillmtx, NULL);
		if(ret) printf("%s:%d fillmtx %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_cond_init(&fillcond, NULL);
		if(ret) printf("%s:%d fillcond %s\n", __func__, __LINE__, strerror(ret));
#ifdef DS_WAKEUP
		ret = pthread_mutex_init(&vidmtx, NULL);
		if(ret) printf("%s:%d vidmtx %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_cond_init(&vidcond, NULL);
		if(ret) printf("%s:%d vidcond %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_mutex_init(&audmtx, NULL);
		if(ret) printf("%s:%d audmtx %s\n", __func__, __LINE__, strerror(ret));
		ret = pthread_cond_init(&audcond, NULL);
		if(ret) printf("%s:%d audcond %s\n", __func__, __LINE__, strerror(ret));
#endif
		pthread_create(&demuxtid, NULL, demux_thread, demuxer);
#ifdef WAIT_FULL_FIRST
		demux_wait_queue_full(demuxer);
#endif
	}
}
#endif


int extension_parsing = 1; // 0=off 1=mixed (used only for unstable formats)

int correct_pts = 0;
int user_correct_pts = -1;

/*
  NOTE : Several demuxers may be opened at the same time so
  demuxers should NEVER rely on an external var to enable them
  self. If a demuxer can't do any auto-detection it should only use
  file_format. The user can explicitly set file_format with the -demuxer
  option so there is really no need for another extra var.
  For convenience an option can be added to set file_format directly
  to the right type (ex: rawaudio,rawvideo).
  Also the stream can override the file_format so a demuxer which rely
  on a special stream type can set file_format at the stream level
  (ex: tv,mf).
*/

static demuxer_t *demux_open_stream(stream_t *stream, int file_format,
                                    int force, int audio_id, int video_id,
                                    int dvdsub_id, char *filename)
{
    demuxer_t *demuxer = NULL;

    sh_video_t *sh_video = NULL;

    const demuxer_desc_t *demuxer_desc;
    int fformat = 0;
    int i;

    // If somebody requested a demuxer check it
    if (file_format) {
        if ((demuxer_desc = get_demuxer_desc_from_type(file_format))) {
            demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
                                  video_id, dvdsub_id, filename);
            if (demuxer_desc->check_file)
                fformat = demuxer_desc->check_file(demuxer);
            if (force || !demuxer_desc->check_file)
                fformat = demuxer_desc->type;
            if (fformat != 0) {
                if (fformat == demuxer_desc->type) {
                    demuxer_t *demux2 = demuxer;
                    // Move messages to demuxer detection code?
                    mp_msg(MSGT_DEMUXER, MSGL_INFO,
                           MSGTR_Detected_XXX_FileFormat,
                           demuxer_desc->shortdesc);
                    file_format = fformat;
                    if (!demuxer->desc->open
                        || (demux2 = demuxer->desc->open(demuxer))) {
                        demuxer = demux2;
                        goto dmx_open;
                    }
                } else {
                    // Format changed after check, recurse
                    free_demuxer(demuxer);
                    return demux_open_stream(stream, fformat, force, audio_id,
                                             video_id, dvdsub_id, filename);
                }
            }
            // Check failed for forced demuxer, quit
            free_demuxer(demuxer);
            return NULL;
        }
    }
    // Test demuxers with safe file checks
    for (i = 0; (demuxer_desc = demuxer_list[i]); i++) {
        if (demuxer_desc->safe_check) {
            demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
                                  video_id, dvdsub_id, filename);
            if ((fformat = demuxer_desc->check_file(demuxer)) != 0) {
                if (fformat == demuxer_desc->type) {
                    demuxer_t *demux2 = demuxer;
                    mp_msg(MSGT_DEMUXER, MSGL_INFO,
                           MSGTR_Detected_XXX_FileFormat,
                           demuxer_desc->shortdesc);
                    file_format = fformat;
                    if (!demuxer->desc->open
                        || (demux2 = demuxer->desc->open(demuxer))) {
                        demuxer = demux2;
                        goto dmx_open;
                    }
                } else {
                    if (fformat == DEMUXER_TYPE_PLAYLIST)
                        return demuxer; // handled in mplayer.c
                    // Format changed after check, recurse
                    free_demuxer(demuxer);
                    demuxer = demux_open_stream(stream, fformat, force,
                                                audio_id, video_id,
                                                dvdsub_id, filename);
                    if (demuxer)
                        return demuxer; // done!
                    file_format = DEMUXER_TYPE_UNKNOWN;
                }
            }
            free_demuxer(demuxer);
            demuxer = NULL;
        }
    }

    // If no forced demuxer perform file extension based detection
    // Ok. We're over the stable detectable fileformats, the next ones are
    // a bit fuzzy. So by default (extension_parsing==1) try extension-based
    // detection first:
    if (file_format == DEMUXER_TYPE_UNKNOWN && filename
        && extension_parsing == 1) {
        file_format = demuxer_type_by_filename(filename);
        if (file_format != DEMUXER_TYPE_UNKNOWN) {
            // we like recursion :)
            demuxer = demux_open_stream(stream, file_format, force, audio_id,
                                        video_id, dvdsub_id, filename);
            if (demuxer)
                return demuxer; // done!
            file_format = DEMUXER_TYPE_UNKNOWN; // continue fuzzy guessing...
            mp_msg(MSGT_DEMUXER, MSGL_V,
                   "demuxer: continue fuzzy content-based format guessing...\n");
        }
    }
    // Try detection for all other demuxers
    for (i = 0; (demuxer_desc = demuxer_list[i]); i++) {

	//Barry 2010-06-01
	if ( demuxer_desc->type == DEMUXER_TYPE_SKYMPEG_TS )	//skyts
		return NULL;

        if (!demuxer_desc->safe_check && demuxer_desc->check_file) {
            demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
                                  video_id, dvdsub_id, filename);
            if ((fformat = demuxer_desc->check_file(demuxer)) != 0) {
                if (fformat == demuxer_desc->type) {
                    demuxer_t *demux2 = demuxer;
                    mp_msg(MSGT_DEMUXER, MSGL_INFO,
                           MSGTR_Detected_XXX_FileFormat,
                           demuxer_desc->shortdesc);
                    file_format = fformat;
                    if (!demuxer->desc->open
                        || (demux2 = demuxer->desc->open(demuxer))) {
                        demuxer = demux2;
                        goto dmx_open;
                    }
                } else {
                    if (fformat == DEMUXER_TYPE_PLAYLIST)
                        return demuxer; // handled in mplayer.c
                    // Format changed after check, recurse
                    free_demuxer(demuxer);
                    demuxer = demux_open_stream(stream, fformat, force,
                                                audio_id, video_id,
                                                dvdsub_id, filename);
                    if (demuxer)
                        return demuxer; // done!
                    file_format = DEMUXER_TYPE_UNKNOWN;
                }
            }
            free_demuxer(demuxer);
            demuxer = NULL;
        }
    }

    return NULL;
    //====== File format recognized, set up these for compatibility: =========
 dmx_open:

    demuxer->file_format = file_format;

    if ((sh_video = demuxer->video->sh) && sh_video->bih) {
        int biComp = le2me_32(sh_video->bih->biCompression);
		int len = 0;
        memset(video_names,0,12);

        if(biComp == mmioFOURCC('D','X','5','0') ||
	   biComp == mmioFOURCC('d','x','5','0') ||
	   biComp == mmioFOURCC('D','I','V','3') ||
	   biComp == mmioFOURCC('d','i','v','3') ||
	   biComp == mmioFOURCC('D','I','V','X') ||
	   biComp == mmioFOURCC('d','i','v','x'))
		{
			mp_msg(MSGT_DEMUX, MSGL_INFO,
					"VIDEO:  [%s]  %dx%d  %dbpp  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n",
					"Xvid_Compatible", sh_video->bih->biWidth,
					sh_video->bih->biHeight, sh_video->bih->biBitCount,
					sh_video->fps, sh_video->i_bps * 0.008f,
					sh_video->i_bps / 1024.0f);
			len = sprintf(video_names, "%s,%dx%d,%5.3f\0", "Xvid_Compatible",sh_video->bih->biWidth,sh_video->bih->biHeight,sh_video->fps);
		}
        else
		{
	        mp_msg(MSGT_DEMUX, MSGL_INFO,
	               "VIDEO:  [%.4s]  %dx%d  %dbpp  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n",
	               (char *) &biComp, sh_video->bih->biWidth,
	               sh_video->bih->biHeight, sh_video->bih->biBitCount,
	               sh_video->fps, sh_video->i_bps * 0.008f,
	               sh_video->i_bps / 1024.0f);
			len=sprintf(video_names, "%.4s,%dx%d,%5.3f\0", (char *)&biComp,sh_video->bih->biWidth,sh_video->bih->biHeight,sh_video->fps);
		}
#if 0
    	if(biComp == mmioFOURCC('D','X','5','0') ||
	   biComp == mmioFOURCC('d','x','5','0') ||
	   biComp == mmioFOURCC('D','I','V','3') ||
	   biComp == mmioFOURCC('d','i','v','3') ||
	   biComp == mmioFOURCC('D','I','V','X') ||
	   biComp == mmioFOURCC('d','i','v','x'))
	{
        	sprintf(video_names, "%s\0", "Xvid_Compatible");
        	video_names[strlen("Xvid_Compatible")] = '\0';
        }
        else
        {
        	sprintf(video_names, "%.4s\0", (char *) &biComp);
        	video_names[strlen((char *)&biComp)] = '\0';
        }

        int len;
    	if(biComp == mmioFOURCC('D','X','5','0') ||
	   biComp == mmioFOURCC('d','x','5','0') ||
	   biComp == mmioFOURCC('D','I','V','3') ||
	   biComp == mmioFOURCC('d','i','v','3') ||
	   biComp == mmioFOURCC('D','I','V','X') ||
	   biComp == mmioFOURCC('d','i','v','x'))
        	len=sprintf(video_names, "%s,%dx%d,%5.3f\0", "Xvid_Compatible",sh_video->bih->biWidth,sh_video->bih->biHeight,sh_video->fps);
        else
        	len=sprintf(video_names, "%.4s,%dx%d,%5.3f\0", (char *)&biComp,sh_video->bih->biWidth,sh_video->bih->biHeight,sh_video->fps);
#endif			
	video_names[len] = '\0';
	//printf("$$$ In [%s][%d] biComp [%x] video_names[%s]$$$$\n", __func__, __LINE__, biComp, video_names);
    }

#ifdef CONFIG_ASS
    if (ass_enabled && ass_library) {
        for (i = 0; i < MAX_S_STREAMS; ++i) {
            sh_sub_t *sh = demuxer->s_streams[i];
            if (sh && sh->type == 'a') {
                sh->ass_track = ass_new_track(ass_library);
                if (sh->ass_track && sh->extradata)
                    ass_process_codec_private(sh->ass_track, sh->extradata,
                                              sh->extradata_len);
            } else if (sh && sh->type != 'v')
                sh->ass_track = ass_default_track(ass_library);
        }
    }
#endif
    return demuxer;
}

char *audio_stream = NULL;
char *sub_stream = NULL;
int audio_stream_cache = 0;

char *demuxer_name = NULL;       // parameter from -demuxer
char *audio_demuxer_name = NULL; // parameter from -audio-demuxer
char *sub_demuxer_name = NULL;   // parameter from -sub-demuxer

extern int hr_mp3_seek;

extern float stream_cache_min_percent;
extern float stream_cache_seek_min_percent;
#ifdef SKY_HTTP
extern int sky_http_enable;
#endif

demuxer_t *demux_open(stream_t *vs, int file_format, int audio_id,
                      int video_id, int dvdsub_id, char *filename)
{
    stream_t *as = NULL, *ss = NULL;
    demuxer_t *vd, *ad = NULL, *sd = NULL;
    demuxer_t *res;
    int afmt = DEMUXER_TYPE_UNKNOWN, sfmt = DEMUXER_TYPE_UNKNOWN;
    int demuxer_type;
    int audio_demuxer_type = 0, sub_demuxer_type = 0;
    int demuxer_force = 0, audio_demuxer_force = 0, sub_demuxer_force = 0;

#ifdef  jfueng_2011_0311
    
    is_vob = 0;
    char *ptr = &filename[0];
     //JF       printf("\n  JF ::  reset  1  \n");          
    audio_long_period = 0;	   
    a_long_quiet_SP_yes =0;
    a_long_quiet_SP_cnt=0;
    have_audio_long_period = 0;	 	   
//Robert 20110324 check ptr is NULL
    if (ptr != NULL)
    {	 
        while (   *ptr  !=  '.'     &&  *ptr  !=  '\0' )	 //JF : '\0' for prevent ...
    	{
         //printf("::...%c\n",*ptr); 
	  ptr++;
    	}

        if (    ( *(ptr+1) == 'V' )    && ( *(ptr+2) == 'O' ) &&(  *(ptr+3) == 'B' ) )	
        {
          is_vob = 1;
        }

        if ( (    *(ptr+1) == 'v'  )  &&  (*(ptr+2) == 'o' ) && ( *(ptr+3) == 'b' )	)
        {
          is_vob = 1;
        }	
    }
	
#endif  //JF


    if ((demuxer_type =
         get_demuxer_type_from_name(demuxer_name, &demuxer_force)) < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_ERR, "-demuxer %s does not exist.\n",
               demuxer_name);
    }
    if ((audio_demuxer_type =
         get_demuxer_type_from_name(audio_demuxer_name,
                                    &audio_demuxer_force)) < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_ERR, "-audio-demuxer %s does not exist.\n",
               audio_demuxer_name);
    }
    if ((sub_demuxer_type =
         get_demuxer_type_from_name(sub_demuxer_name,
                                    &sub_demuxer_force)) < 0) {
        mp_msg(MSGT_DEMUXER, MSGL_ERR, "-sub-demuxer %s does not exist.\n",
               sub_demuxer_name);
    }

    if (audio_stream) {
        as = open_stream(audio_stream, 0, &afmt);
        if (!as) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenAudioStream,
                   audio_stream);
            return NULL;
        }
        if (audio_stream_cache) {
            if (!stream_enable_cache
                (as, audio_stream_cache * 1024,
                 audio_stream_cache * 1024 * (stream_cache_min_percent /
                                              100.0),
                 audio_stream_cache * 1024 * (stream_cache_seek_min_percent /
                                              100.0))) {
                free_stream(as);
                mp_msg(MSGT_DEMUXER, MSGL_ERR,
                       "Can't enable audio stream cache\n");
                return NULL;
            }
        }
    }
    if (sub_stream) {
        ss = open_stream(sub_stream, 0, &sfmt);
        if (!ss) {
            mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenSubtitlesStream,
                   sub_stream);
            return NULL;
        }
    }

    vd = demux_open_stream(vs, demuxer_type ? demuxer_type : file_format,
                           demuxer_force, audio_stream ? -2 : audio_id,
                           video_id, sub_stream ? -2 : dvdsub_id, filename);
    if (!vd) {
        if (as)
            free_stream(as);
        if (ss)
            free_stream(ss);
        return NULL;
    }
    if (as) {
        ad = demux_open_stream(as,
                               audio_demuxer_type ? audio_demuxer_type : afmt,
                               audio_demuxer_force, audio_id, -2, -2,
                               audio_stream);
        if (!ad) {
            mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_OpeningAudioDemuxerFailed,
                   audio_stream);
            free_stream(as);
        } else if (ad->audio->sh
                   && ((sh_audio_t *) ad->audio->sh)->format == 0x55) // MP3
            hr_mp3_seek = 1;    // Enable high res seeking
    }
    if (ss) {
        sd = demux_open_stream(ss, sub_demuxer_type ? sub_demuxer_type : sfmt,
                               sub_demuxer_force, -2, -2, dvdsub_id,
                               sub_stream);
        if (!sd) {
            mp_msg(MSGT_DEMUXER, MSGL_WARN,
                   MSGTR_OpeningSubtitlesDemuxerFailed, sub_stream);
            free_stream(ss);
        }
    }

    if (ad && sd)
        res = new_demuxers_demuxer(vd, ad, sd);
    else if (ad)
        res = new_demuxers_demuxer(vd, ad, vd);
    else if (sd)
        res = new_demuxers_demuxer(vd, vd, sd);
    else
        res = vd;

    correct_pts = user_correct_pts;
    if (correct_pts < 0)
        correct_pts = demux_control(res, DEMUXER_CTRL_CORRECT_PTS, NULL)
                      == DEMUXER_CTRL_OK;
#ifdef DEMUX_THREAD
    if(res){
#ifdef DEMUX_AUTO
	if(demuxthread == 0 &&
	   (res->type == DEMUXER_TYPE_MOV ||
	    res->type == DEMUXER_TYPE_AVI_NI ||
	    res->type == DEMUXER_TYPE_AVI_NINI)){
		demuxthread = 1;
		athreshold = 30;
		vthreshold = 30;
	}
#endif
	demux_cache_enable(res);
    }
#endif
    return res;
}

/**
 * Do necessary reinitialization after e.g. a seek.
 * Do _not_ call ds_fill_buffer between the seek and this, it breaks at least
 * seeking with ASF demuxer.
 */
static void demux_resync(demuxer_t *demuxer)
{
    sh_video_t *sh_video = demuxer->video->sh;
    sh_audio_t *sh_audio = demuxer->audio->sh;
    demux_control(demuxer, DEMUXER_CTRL_RESYNC, NULL);
    if (sh_video) {
        resync_video_stream(sh_video);
    }
    if (sh_audio) {
        resync_audio_stream(sh_audio);
    }
}

void demux_flush(demuxer_t *demuxer)
{
#if PARSE_ON_ADD
    ds_clear_parser(demuxer->video);
    ds_clear_parser(demuxer->audio);
    ds_clear_parser(demuxer->sub);
#endif
    ds_free_packs(demuxer->video);
    ds_free_packs(demuxer->audio);
    ds_free_packs(demuxer->sub);
}

int demux_seek(demuxer_t *demuxer, float rel_seek_secs, float audio_delay,
               int flags)
{
    double tmp = 0;
    double pts;
    extern int FFFR_to_normal;

    if (!demuxer->seekable) {
        if (demuxer->file_format == DEMUXER_TYPE_AVI)
            mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekRawAVI);
#ifdef CONFIG_TV
        else if (demuxer->file_format == DEMUXER_TYPE_TV)
            mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_TVInputNotSeekable);
#endif
        else
            mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekFile);
        return 0;
    }

#ifdef DEMUX_THREAD
    pthread_mutex_lock(&seekmtx);
    seeklocked = 1;
#endif
    demux_flush(demuxer);

    demuxer->stream->eof = 0;
    demuxer->video->eof = 0;
    demuxer->audio->eof = 0;

    if (flags & SEEK_ABSOLUTE)
        pts = 0.0f;
    else {
        if (demuxer->stream_pts == MP_NOPTS_VALUE)
            goto dmx_seek;
        pts = demuxer->stream_pts;
    }

    if (flags & SEEK_FACTOR) {
        if (stream_control(demuxer->stream, STREAM_CTRL_GET_TIME_LENGTH, &tmp)
            == STREAM_UNSUPPORTED)
            goto dmx_seek;
        pts += tmp * rel_seek_secs;
    } else
        pts += rel_seek_secs;

    if ((demuxer->stream->type != STREAMTYPE_DVDNAV || 
    	(FFFR_to_normal == 0 && speed_mult == 0 && demuxer->stream->type == STREAMTYPE_DVDNAV)) && 
    	stream_control(demuxer->stream, STREAM_CTRL_SEEK_TO_TIME, &pts) != STREAM_UNSUPPORTED)
    {
        demux_resync(demuxer);
#ifdef DEMUX_THREAD
    seeklocked = 0;
    pthread_mutex_unlock(&seekmtx);
#endif
        return 1;
    }

  dmx_seek:

#ifdef SKY_HTTP
		if(sky_http_enable)
		{
			if (flags & SEEK_ABSOLUTE)
			{
				sky_http_pts = pts;	
			}
			else
			{
				sh_video_t *sh_video = demuxer->video->sh;
				sky_http_pts = sh_video->pts+rel_seek_secs;  
			}
		}
#endif
	
    if (demuxer->desc->seek)
        demuxer->desc->seek(demuxer, rel_seek_secs, audio_delay, flags);

	if(speed_mult == 0 || FRtoFF == 1)
	{
		demux_resync(demuxer);
#ifdef HW_TS_DEMUX
		extern int hwtsdemux;
#endif /* end of HW_TS_DEMUX */
		//Barry 2010-12-24
		if (FRtoFF == 1)
		{
			FRtoFF = 0;
			printf("==========>    FRtoFF == 1\n");
#ifdef HW_TS_DEMUX
			/* FR to FF x 2 */
			if (hwtsdemux && demuxer->type == DEMUXER_TYPE_MPEG_TS)
			{
				change_axi_speed(demuxer->video, demuxer->priv, SLOW_DOWN_SPEED);
			}
#endif /* end of HW_TS_DEMUX */
		}
#ifdef HW_TS_DEMUX
		else
		{	
			/* FF or FR to Normal playback */
			if (hwtsdemux && demuxer->type == DEMUXER_TYPE_MPEG_TS && FFFR_to_normal)
			{
				change_axi_speed(demuxer->video, demuxer->priv, NORMAL_SPPED);
			}
		}
#endif /* end of HW_TS_DEMUX */

	}
#ifdef DEMUX_THREAD
    seeklocked = 0;
    pthread_mutex_unlock(&seekmtx);

#ifdef WAIT_FULL_FIRST
    //demux_wait_queue_full(demuxer);
#endif
#endif

    return 1;
}

int demux_info_add(demuxer_t *demuxer, const char *opt, const char *param)
{
    char **info = demuxer->info;
    int n = 0;


    for (n = 0; info && info[2 * n] != NULL; n++) {
        if (!strcasecmp(opt, info[2 * n])) {
            if (!strcmp(param, info[2 * n + 1])) {
                mp_msg(MSGT_DEMUX, MSGL_V, "Demuxer info %s set to unchanged value %s\n", opt, param);
                return 0;
            }
            mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_DemuxerInfoChanged, opt,
                   param);
            free(info[2 * n + 1]);
            info[2 * n + 1] = strdup(param);
            return 0;
        }
    }

    info = demuxer->info = realloc(info, (2 * (n + 2)) * sizeof(char *));
    info[2 * n] = strdup(opt);
    info[2 * n + 1] = strdup(param);
    memset(&info[2 * (n + 1)], 0, 2 * sizeof(char *));

    return 1;
}

int demux_info_print(demuxer_t *demuxer)
{
    char **info = demuxer->info;
    int n;

#if 1	//Barry 2010-12-30
    sh_video_t *sh_video = NULL;
    sh_video = demuxer->video->sh;
#endif

    if (!info)
        return 0;

    mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_ClipInfo);
    for (n = 0; info[2 * n] != NULL; n++) {

#if 1	//Barry 2010-12-30
    	if (sh_video && sh_video->bih)
	{
		if ( (sh_video->bih->biCompression == mmioFOURCC('V', 'P', '6', 'F')) ||
			(sh_video->bih->biCompression == mmioFOURCC('V', 'P', '6', 'A'))
		)
		{
			if (!strcasecmp("width", info[2 * n]) && !sh_video->bih->biWidth)
				sh_video->bih->biWidth = atoi(info[2 * n + 1]);
			if (!strcasecmp("height", info[2 * n]) && !sh_video->bih->biHeight)
				sh_video->bih->biHeight = atoi(info[2 * n + 1]);
			if (!strcasecmp("framerate", info[2 * n]))
				sh_video->fps = atof(info[2 * n + 1]);
		}
    	}
#endif

        mp_msg(MSGT_DEMUX, MSGL_INFO, " %s: %s\n", info[2 * n],
               info[2 * n + 1]);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_NAME%d=%s\n", n,
               info[2 * n]);
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_VALUE%d=%s\n", n,
               info[2 * n + 1]);
    }
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_N=%d\n", n);

    return 0;
}

char *demux_info_get(demuxer_t *demuxer, const char *opt)
{
    int i;
    char **info = demuxer->info;

    for (i = 0; info && info[2 * i] != NULL; i++) {
        if (!strcasecmp(opt, info[2 * i]))
            return info[2 * i + 1];
    }

    return NULL;
}

int demux_control(demuxer_t *demuxer, int cmd, void *arg)
{

    if (demuxer->desc->control)
        return demuxer->desc->control(demuxer, cmd, arg);

    return DEMUXER_CTRL_NOTIMPL;
}



double demuxer_get_time_length(demuxer_t *demuxer)
{
    double get_time_ans;
    sh_video_t *sh_video = demuxer->video->sh;
    sh_audio_t *sh_audio = demuxer->audio->sh;
    // <= 0 means DEMUXER_CTRL_NOTIMPL or DEMUXER_CTRL_DONTKNOW
    if (demux_control
        (demuxer, DEMUXER_CTRL_GET_TIME_LENGTH, (void *) &get_time_ans) <= 0) {
        if (sh_video && sh_video->i_bps && sh_audio && sh_audio->i_bps)
            get_time_ans = (double) (demuxer->movi_end -
                                     demuxer->movi_start) / (sh_video->i_bps +
                                                             sh_audio->i_bps);
        else if (sh_video && sh_video->i_bps)
            get_time_ans = (double) (demuxer->movi_end -
                                     demuxer->movi_start) / sh_video->i_bps;
        else if (sh_audio && sh_audio->i_bps)
            get_time_ans = (double) (demuxer->movi_end -
                                     demuxer->movi_start) / sh_audio->i_bps;
        else
            get_time_ans = 0;
    }
    return get_time_ans;
}

/**
 * \brief demuxer_get_current_time() returns the time of the current play in three possible ways:
 *        either when the stream reader satisfies STREAM_CTRL_GET_CURRENT_TIME (e.g. dvd)
 *        or using sh_video->pts when the former method fails
 *        0 otherwise
 * \return the current play time
 */
int demuxer_get_current_time(demuxer_t *demuxer)
{
    double get_time_ans = 0;
    sh_video_t *sh_video = demuxer->video->sh;
    if (demuxer->stream_pts != MP_NOPTS_VALUE)
        get_time_ans = demuxer->stream_pts;
    else if (sh_video)
        get_time_ans = sh_video->pts;
    return (int) get_time_ans;
}

int demuxer_get_sub_reg(demuxer_t *demuxer)
{
    int ris, curr_sub_reg = -1;
    ris = stream_control(demuxer->stream, STREAM_CTRL_GET_SUB, &curr_sub_reg);
    if (ris == STREAM_UNSUPPORTED)
        return -1;
    return curr_sub_reg;
}

int demuxer_get_percent_pos(demuxer_t *demuxer)
{
    int ans = 0;
    int res = demux_control(demuxer, DEMUXER_CTRL_GET_PERCENT_POS, &ans);
    int len = (demuxer->movi_end - demuxer->movi_start) / 100;
    if (res <= 0) {
        off_t pos = demuxer->filepos > 0 ? demuxer->filepos : stream_tell(demuxer->stream);
        if (len > 0)
            ans = (pos - demuxer->movi_start) / len;
        else
            ans = 0;
    }
    if (ans < 0)
        ans = 0;
    if (ans > 100)
        ans = 100;
    return ans;
}

int demuxer_switch_audio(demuxer_t *demuxer, int index)
{
    int res = demux_control(demuxer, DEMUXER_CTRL_SWITCH_AUDIO, &index);
    if (res == DEMUXER_CTRL_NOTIMPL)
        index = demuxer->audio->id;
    if (demuxer->audio->id >= 0)
        demuxer->audio->sh = demuxer->a_streams[demuxer->audio->id];
    else
        demuxer->audio->sh = NULL;
    return index;
}

int demuxer_switch_video(demuxer_t *demuxer, int index)
{
    int res = demux_control(demuxer, DEMUXER_CTRL_SWITCH_VIDEO, &index);
    if (res == DEMUXER_CTRL_NOTIMPL)
        index = demuxer->video->id;
    if (demuxer->video->id >= 0)
        demuxer->video->sh = demuxer->v_streams[demuxer->video->id];
    else
        demuxer->video->sh = NULL;
    return index;
}

#ifdef AVI_NOT_SORT
int demuxer_switch_sub(demuxer_t *demuxer)
{
    int index = 0;
    int res = demux_control(demuxer, DEMUXER_CTRL_SWITCH_SUB, &index);
    return res;
}
#endif

int demuxer_add_attachment(demuxer_t *demuxer, const char *name,
                           const char *type, const void *data, size_t size)
{
    if (!(demuxer->num_attachments & 31))
        demuxer->attachments = realloc(demuxer->attachments,
                (demuxer->num_attachments + 32) * sizeof(demux_attachment_t));

    demuxer->attachments[demuxer->num_attachments].name = strdup(name);
    demuxer->attachments[demuxer->num_attachments].type = strdup(type);
    demuxer->attachments[demuxer->num_attachments].data = malloc(size);
    memcpy(demuxer->attachments[demuxer->num_attachments].data, data, size);
    demuxer->attachments[demuxer->num_attachments].data_size = size;

    return demuxer->num_attachments++;
}

int demuxer_add_chapter(demuxer_t *demuxer, const char *name, uint64_t start,
                        uint64_t end)
{
    if (demuxer->chapters == NULL)
        demuxer->chapters = malloc(32 * sizeof(*demuxer->chapters));
    else if (!(demuxer->num_chapters % 32))
        demuxer->chapters = realloc(demuxer->chapters,
                                    (demuxer->num_chapters + 32) *
                                        sizeof(*demuxer->chapters));

    demuxer->chapters[demuxer->num_chapters].start = start;
    demuxer->chapters[demuxer->num_chapters].end = end;
    demuxer->chapters[demuxer->num_chapters].name = strdup(name ? name : MSGTR_Unknown);

    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_ID=%d\n", demuxer->num_chapters);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_START=%"PRIu64"\n", demuxer->num_chapters, start);
    if (end)
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_END=%"PRIu64"\n", demuxer->num_chapters, end);
    if (name)
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_NAME=%s\n", demuxer->num_chapters, name);

    return demuxer->num_chapters++;
}

/**
 * \brief demuxer_seek_chapter() seeks to a chapter in two possible ways:
 *        either using the demuxer->chapters structure set by the demuxer
 *        or asking help to the stream layer (e.g. dvd)
 * \param chapter - chapter number wished - 0-based
 * \param mode 0: relative to current main pts, 1: absolute
 * \param seek_pts set by the function to the pts to seek to (if demuxer->chapters is set)
 * \param num_chapters number of chapters present (set by this function is param is not null)
 * \param chapter_name name of chapter found (set by this function is param is not null)
 * \return -1 on error, current chapter if successful
 */

int demuxer_seek_chapter(demuxer_t *demuxer, int chapter, int mode,
                         float *seek_pts, int *num_chapters,
                         char **chapter_name)
{
    int ris;
    int current, total;

    dvd_fast = 1;	//Fuchun 2010.04.19
	
    if (!demuxer->num_chapters || !demuxer->chapters) {
        if (!mode) {
            ris = stream_control(demuxer->stream,
                                 STREAM_CTRL_GET_CURRENT_CHAPTER, &current);
            if (ris == STREAM_UNSUPPORTED)
                return -1;
            chapter += current;
        }

#ifdef DEMUX_THREAD
        pthread_mutex_lock(&seekmtx);
        seeklocked = 1;
#endif
        demux_flush(demuxer);

        ris = stream_control(demuxer->stream, STREAM_CTRL_SEEK_TO_CHAPTER,
                             &chapter);
        demuxer->filepos = demuxer->stream->pos;
#if 0
        // 20101101 charleslin fix gray screen after seek chapter
        extern float audio_delay;
        if (demuxer->desc->seek)
            demuxer->desc->seek(demuxer, 0.0, audio_delay, 0);
#endif
//        if(demuxer->stream->type != STREAMTYPE_DVDNAV) //after switch_chapter cause flicking //mantis:3344
            demux_resync(demuxer);
#ifdef DEMUX_THREAD
        seeklocked = 0;
        pthread_mutex_unlock(&seekmtx);
#endif

        // exit status may be ok, but main() doesn't have to seek itself
        // (because e.g. dvds depend on sectors, not on pts)
        *seek_pts = -1.0;

        if (num_chapters) {
            if (stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS,
                               num_chapters) == STREAM_UNSUPPORTED)
                *num_chapters = 0;
        }

        if (chapter_name) {
            *chapter_name = NULL;
            if (num_chapters && *num_chapters) {
                char *tmp = malloc(16);
                if (tmp) {
                    sprintf(tmp, " of %3d", *num_chapters);
                    *chapter_name = tmp;
                }
            }
        }

        return ris != STREAM_UNSUPPORTED ? chapter : -1;
    } else {  // chapters structure is set in the demuxer
        sh_video_t *sh_video = demuxer->video->sh;
        sh_audio_t *sh_audio = demuxer->audio->sh;

        total = demuxer->num_chapters;

        if (mode == 1)  //absolute seeking
            current = chapter;
        else {          //relative seeking
            uint64_t now;
            now = (sh_video ? sh_video->pts : (sh_audio ? sh_audio->pts : 0.))
                  * 1000 + .5;

            for (current = total - 1; current >= 0; --current) {
                demux_chapter_t *chapter = demuxer->chapters + current;
                if (chapter->start <= now)
                    break;
            }
            current += chapter;
        }

        if (current >= total)
            return -1;
        if (current < 0)
            current = 0;

        *seek_pts = demuxer->chapters[current].start / 1000.0;

        if (num_chapters)
            *num_chapters = demuxer->num_chapters;

        if (chapter_name) {
            if (demuxer->chapters[current].name)
                *chapter_name = strdup(demuxer->chapters[current].name);
            else
                *chapter_name = NULL;
        }

        return current;
    }
}

int demuxer_get_current_chapter(demuxer_t *demuxer)
{
    int chapter = -1;
    if (!demuxer->num_chapters || !demuxer->chapters) {
        if (stream_control(demuxer->stream, STREAM_CTRL_GET_CURRENT_CHAPTER,
                           &chapter) == STREAM_UNSUPPORTED)
            chapter = -1;
    } else {
        sh_video_t *sh_video = demuxer->video->sh;
        sh_audio_t *sh_audio = demuxer->audio->sh;
        uint64_t now;
        now = (sh_video ? sh_video->pts : (sh_audio ? sh_audio->pts : 0))
              * 1000 + 0.5;
        for (chapter = demuxer->num_chapters - 1; chapter >= 0; --chapter) {
            if (demuxer->chapters[chapter].start <= now)
                break;
        }
    }
    return chapter;
}

char *demuxer_chapter_name(demuxer_t *demuxer, int chapter)
{
    if (demuxer->num_chapters && demuxer->chapters) {
        if (chapter >= 0 && chapter < demuxer->num_chapters
            && demuxer->chapters[chapter].name)
            return strdup(demuxer->chapters[chapter].name);
    }
    return NULL;
}

char *demuxer_chapter_display_name(demuxer_t *demuxer, int chapter)
{
    char *chapter_name = demuxer_chapter_name(demuxer, chapter);
    if (chapter_name) {
        char *tmp = malloc(strlen(chapter_name) + 14);
        snprintf(tmp, 63, "(%d) %s", chapter + 1, chapter_name);
        free(chapter_name);
        return tmp;
    } else {
        int chapter_num = demuxer_chapter_count(demuxer);
        char tmp[30];
        if (chapter_num <= 0)
            sprintf(tmp, "(%d)", chapter + 1);
        else
            sprintf(tmp, "(%d) of %d", chapter + 1, chapter_num);
        return strdup(tmp);
    }
}

float demuxer_chapter_time(demuxer_t *demuxer, int chapter, float *end)
{
    if (demuxer->num_chapters && demuxer->chapters && chapter >= 0
        && chapter < demuxer->num_chapters) {
        if (end)
            *end = demuxer->chapters[chapter].end / 1000.0;
        return demuxer->chapters[chapter].start / 1000.0;
    }
    return -1.0;
}

int demuxer_chapter_count(demuxer_t *demuxer)
{
    if (!demuxer->num_chapters || !demuxer->chapters) {
        int num_chapters = 0;
        if (stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS,
                           &num_chapters) == STREAM_UNSUPPORTED)
            num_chapters = 0;
        return num_chapters;
    } else
        return demuxer->num_chapters;
}

int demuxer_angles_count(demuxer_t *demuxer)
{
    int ris, angles = -1;

    ris = stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_ANGLES, &angles);
    if (ris == STREAM_UNSUPPORTED)
        return -1;
    return angles;
}

int demuxer_get_current_angle(demuxer_t *demuxer)
{
    int ris, curr_angle = -1;
    ris = stream_control(demuxer->stream, STREAM_CTRL_GET_ANGLE, &curr_angle);
    if (ris == STREAM_UNSUPPORTED)
        return -1;
    return curr_angle;
}


int demuxer_set_angle(demuxer_t *demuxer, int angle)
{
    int ris, angles = -1;

    angles = demuxer_angles_count(demuxer);
    if ((angles < 1) || (angle > angles))
        return -1;

    demux_flush(demuxer);

    ris = stream_control(demuxer->stream, STREAM_CTRL_SET_ANGLE, &angle);
    if (ris == STREAM_UNSUPPORTED)
        return -1;

    demux_resync(demuxer);

    return angle;
}

int dvdnav_demuxer_set_sub_reg(demuxer_t *demuxer, int sub)
{
	int result;

    result = stream_control(demuxer->stream, STREAM_CTRL_SET_SUB, &sub);
    if (result == STREAM_UNSUPPORTED)
        return -1;

    return result;
}

int dvdnav_demuxer_set_audio_reg(demuxer_t *demuxer, int audio)
{
	int result;

    result = stream_control(demuxer->stream, STREAM_CTRL_SET_AUDIO, &audio);
    if (result == STREAM_UNSUPPORTED)
        return -1;

    return result;
}

int demuxer_audio_track_by_lang(demuxer_t *d, char *lang)
{
    int i, len;
    lang += strspn(lang, ",");
    while ((len = strcspn(lang, ",")) > 0) {
        for (i = 0; i < MAX_A_STREAMS; ++i) {
            sh_audio_t *sh = d->a_streams[i];
            if (sh && sh->lang && strncmp(sh->lang, lang, len) == 0)
                return sh->aid;
        }
        lang += len;
        lang += strspn(lang, ",");
    }
    return -1;
}

int demuxer_sub_track_by_lang(demuxer_t *d, char *lang)
{
    int i, len;
    lang += strspn(lang, ",");
    while ((len = strcspn(lang, ",")) > 0) {
        for (i = 0; i < MAX_S_STREAMS; ++i) {
            sh_sub_t *sh = d->s_streams[i];
            if (sh && sh->lang && strncmp(sh->lang, lang, len) == 0)
                return sh->sid;
        }
        lang += len;
        lang += strspn(lang, ",");
    }
    return -1;
}

int demuxer_default_audio_track(demuxer_t *d)
#if 1  ////JF  2011/01/25
{
    int i;



    for (i = 0; i < MAX_A_STREAMS; ++i) {
        sh_audio_t *sh = d->a_streams[i];

//JF  2011/01/25
//        if (sh)
//        printf("\n::111___aid=%d ; default_track =%d \n",sh->aid,sh->default_track);
///////		

        if (sh && sh->default_track)
//JF  2011/01/25
//{
//        printf("\n::222......aid=%d ; default_track =%d \n",sh->aid,sh->default_track);  
            return sh->aid;
//JF  2011/01/25			
//}			
    }

//JF  2011/01/25		
    for (i = MAX_A_STREAMS - 1 ;  i  >= 0  ; --i) {
        sh_audio_t *sh = d->a_streams[i];
        if ( sh )                                 
           if (sh->aid == 128)                       //JF :: "first  meet" = 128   reversely
           	{
//              printf("\n::333...return....aid=%d ; i = %d \n", sh->aid, i);           	
              return sh->aid;  //128 
           	}
	    else                                           //JF NOTE :: if  + //  ::  find  out " 128"   reversely  ... can prevent  " 0  | 128 | 129 " error !!!
	      break;		                           //JF NOTE :: if  + //  ::  find  out " 128"   reversely 
    }

//JF  2011/01/25	
//    printf("\n\====: audio_track by first ....\n"); 

    for (i = 0; i < MAX_A_STREAMS; ++i) {
        sh_audio_t *sh = d->a_streams[i];

//JF  2011/01/25	
//        if (sh)
//        printf("\n::555 ...return....aid=%d  \n",sh->aid);	
		
        if (sh)
            return sh->aid;
    }

	
    return -1;
}
#else	
{
    int i;
    for (i = 0; i < MAX_A_STREAMS; ++i) {
        sh_audio_t *sh = d->a_streams[i];
        if (sh && sh->default_track)
            return sh->aid;
    }
    for (i = 0; i < MAX_A_STREAMS; ++i) {
        sh_audio_t *sh = d->a_streams[i];
        if (sh)
            return sh->aid;
    }
    return -1;
}

#endif

int demuxer_default_sub_track(demuxer_t *d)
{
#if 1	//Barry 2011-06-01 set disable as default //Fuchun 20110810 enable here
    int i;
    for (i = 0; i < MAX_S_STREAMS; ++i) {
        sh_sub_t *sh = d->s_streams[i];
        if (sh && sh->default_track)
            return sh->sid;
	else if(sh && sh->type == 'p')	//Fuchun 2010.10.12 for PGS select
		return 0;
	else if(sh && sh->type == 'b')
		return 0;
	else if(sh && sh->type == 'd')
		return 0;
    }
#endif
    return -1;
}

int demux_avi_set_index_pos(demuxer_t* d)
{
	if(!d->audio->sh) return 0;
	
	avi_priv_t *priv=d->priv;
	demux_stream_t *d_video=d->video;
	sh_video_t *sh_video=d_video->sh;
	demux_stream_t *d_audio=d->audio;
	sh_audio_t *sh_audio=d_audio->sh;
	off_t video_chunk_pos=priv->idx_pos_v;
	int id, len, audio_len;
	int video_ok = 0, audio_ok = 0;
	float current_video_pts = priv->avi_video_pts;
	float last_audio_pts = priv->avi_audio_pts;

	if(video_chunk_pos < 0) return 0;
//	priv->idx_pos_a = priv->idx_pos_v = video_chunk_pos;
//	printf("@@@ current_video_pts[%f]   last_audio_pts[%f] @@@\n", current_video_pts, last_audio_pts);

	while(1)
	{
		d->filepos = stream_tell(d->stream);
		id = stream_read_dword_le(d->stream);
		len = stream_read_dword_le(d->stream);

		if(id == mmioFOURCC('L','I','S','T'))
		{
			id = stream_read_dword_le(d->stream);
	//		usleep(1);
			continue;
		}

		if(id == mmioFOURCC('R','I','F','F'))
		{
			id = stream_read_dword_le(d->stream);
	//		usleep(1);
			continue;
		}

		int stream_id = avi_stream_id(id);
	//	printf("==== stream_id %d	filepos %"PRIX64"	len %X\n", stream_id, d->filepos, len);

		if(stream_id == d_audio->id && !audio_ok)
		{
			priv->idx_pos_a = stream_tell(d->stream) - 8;
			audio_len = len;
			audio_ok = 1;
		}

		if(stream_id == d_video->id && !video_ok)
		{
			priv->idx_pos_v = stream_tell(d->stream) - 8;
			video_ok = 1;
		}

		if(video_ok == 1 && audio_ok == 1)
		{
			stream_skip(d->stream, -8);
			break;
		}
		
		{
			int skip = (len+1)&(~1);
			stream_skip(d->stream, skip);
		}

	//	usleep(1);
	}

	priv->audio_block_no = (current_video_pts - last_audio_pts)*sh_audio->audio.dwRate/sh_audio->audio.dwScale;
//	printf("### audio_block_size[%d]   audio_len[%d] ###\n", priv->audio_block_size, audio_len);
	priv->audio_block_no += (sky_vdec_vdref_num*5)*(priv->audio_block_size ? ((audio_len+priv->audio_block_size-1)/priv->audio_block_size) : 1);

	return 1;
}

#ifdef AVI_NOT_SORT
void demux_avi_read_audio_index(demuxer_t *demux)
{
	avi_priv_t *priv = demux->priv;
	if(!priv->not_sort)
		return;

	demux_stream_t* ds = demux->audio;
	int id, i, j, ckid, idx_id;
	avisuperindex_chunk *cx;
	AVIINDEXENTRY *idx = NULL;
	uint32_t last_length, cur_length;
	int last_block_num, cur_block_num;
	off_t consume_size;
	off_t start_pos, end_pos, cur_pos, last_pos;
	off_t last_offset, current_offset;

	//last audio
	idx = &((AVIINDEXENTRY *)priv->idx_a)[priv->idx_pos_a];
	last_length = idx->dwChunkLength;
	last_block_num = priv->idx_pos_a;
	consume_size = (off_t)(last_block_num*last_length);
	last_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_a)[priv->idx_pos_a]);

	//read current audio index
	for (cx = priv->suidx; cx != &priv->suidx[priv->suidx_size]; cx++)
	{
		ckid = ((cx->dwChunkId[3] << 24) | (cx->dwChunkId[2] << 16) | (cx->dwChunkId[1] << 8) | cx->dwChunkId[0]);
		idx_id = ckid >> 16;
		id = avi_stream_id(ckid);
		if(id == ds->id && idx_id == 0x6277)
		{
			if(priv->suidx_size_a > 0)
			{
				free(priv->idx_a);
				priv->idx_a = NULL;
				priv->suidx_size_a = 0;
			}
			avistdindex_chunk *sic;
			for (sic = cx->stdidx; sic != &cx->stdidx[cx->nEntriesInUse]; sic++) 
			{
				avistdindex_entry *sie;

				priv->idx_a = realloc(priv->idx_a, (priv->suidx_size_a+sic->nEntriesInUse)*sizeof (AVIINDEXENTRY));
				idx = priv->idx_a + (priv->suidx_size_a*sizeof (AVIINDEXENTRY));
				priv->suidx_size_a += sic->nEntriesInUse;
				
				for (sie = sic->aIndex, i=0; sie != &sic->aIndex[sic->nEntriesInUse]; sie++, i++) 
				{
					uint64_t off = sic->qwBaseOffset + sie->dwOffset - 8;
					memcpy(&idx->ckid, sic->dwChunkId, 4);
					idx->dwChunkOffset = off;
					idx->dwFlags = (off >> 32) << 16;
					idx->dwChunkLength = sie->dwSize & 0x7fffffff;
					idx->dwFlags |= (sie->dwSize&0x80000000)?0x0:AVIIF_KEYFRAME; // bit 31 denotes !keyframe
					idx++;
				}
			}
		}
	}
	
	//calculate current audio index pos
	start_pos = 0;
	end_pos = priv->suidx_size_a-1;
	idx = &((AVIINDEXENTRY *)priv->idx_a)[priv->suidx_size_a/2];
	cur_length = idx->dwChunkLength;
	cur_block_num = (int)(consume_size/cur_length);
	cur_pos = start_pos+cur_block_num;
	if(cur_pos > end_pos)
		cur_pos = end_pos-1;
	current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_a)[cur_pos]);
	if(current_offset > last_offset)
	{
		for(j = cur_pos; j >= start_pos; j--)
		{
			current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_a)[j]);
			if(current_offset < last_offset)
			{
				j++;
				break;
			}
		}
	}
	else
	{
		for(j = cur_pos; j < end_pos; j++)
		{
			current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_a)[j]);
			if(current_offset > last_offset)
			{
				break;
			}
		}
	}
	if(j < end_pos && j >= start_pos)
		priv->idx_pos_a = j;
}
#endif

#ifdef SUPPORT_DIVX_DRM
void demux_avi_save_idxpos(demuxer_t* d)
{
   if(speed_mult == 0 || speed_mult == 1) return;

   avi_priv_t *priv=d->priv;
   int i;

   d->video->pack_no = 0;
   for(i = 0; i < d->video->pos; i++)
   {
#ifdef AVI_NOT_SORT
	int id;
	if(priv->not_sort)
		id = (&((AVIINDEXENTRY *)priv->idx_v)[i])->ckid;
	else
		id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#else
       int id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#endif
       if(avi_stream_id(id) == d->video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
	   {
		   //printf("$$$$ In [%s][%d] pack_no [%d] suidx_size[%d]  d->video->id[%d] id [%x]$$$$\n", __func__, __LINE__, d->video->pack_no, priv->suidx_size, d->video->id, id);
		   ++d->video->pack_no;
	   }
   }
   //printf("$$$$ In [%s][%d] pack_no [%d] suidx_size[%d]  $$$$\n", __func__, __LINE__, d->video->pack_no, priv->suidx_size);
   if(speed_mult < 0)
   {
       ++d->video->pack_no;
   }

   priv->last_idx_pos = d->video->pos;
}
#endif /* end of SUPPORT_DIVX_DRM */


#ifdef STREAM_NI
void demux_switch_stream(demuxer_t *demuxer, demux_stream_t* ds)
{
    if(demuxer->stream->type == STREAMTYPE_FILE){
      stream_t *s = demuxer->stream;

      if(s->fd == s->afd && ds == demuxer->video){
	//printf("switch to video buffer\n");
	//printf("old pos 0x%llx buffer %p len 0x%x pos 0x%x\n", s->pos, s->buffer, s->buf_len, s->buf_pos);
	s->apos = s->pos;
	s->abuf_len = s->buf_len;
	s->abuf_pos = s->buf_pos;

	s->fd = s->vfd;
	s->pos = s->vpos;
	s->buf_len = s->vbuf_len;
	s->buf_pos = s->vbuf_pos;
	s->buffer = s->vbuf;
	//printf("new pos 0x%llx buffer %p len 0x%x pos 0x%x\n", s->pos, s->buffer, s->buf_len, s->buf_pos);
      }else if(s->fd == s->vfd && ds == demuxer->audio){
	//printf("switch to audio buffer\n");
	//printf("old pos 0x%llx buffer %p len 0x%x pos 0x%x\n", s->pos, s->buffer, s->buf_len, s->buf_pos);
	s->vpos = s->pos;
	s->vbuf_len = s->buf_len;
	s->vbuf_pos = s->buf_pos;

	s->fd = s->afd;
	s->pos = s->apos;
	s->buf_len = s->abuf_len;
	s->buf_pos = s->abuf_pos;
	s->buffer = s->abuf;
	//printf("new pos 0x%llx buffer %p len 0x%x pos 0x%x\n", s->pos, s->buffer, s->buf_len, s->buf_pos);
      }
    }
}
#endif

