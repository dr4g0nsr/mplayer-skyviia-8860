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

#include <stdlib.h>
#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
#include "mplayer.h"
#include "libvo/sub.h"
#include "libvo/video_out.h"
#include "cpudetect.h"
#include "help_mp.h"
#include "mp_msg.h"
#include "spudec.h"
#include "version.h"
#include "vobsub.h"
#include "libmpcodecs/dec_teletext.h"
#include "libavutil/intreadwrite.h"
#include "m_option.h"
#include "mpcommon.h"
#include "libmpcodecs/dec_video.h"
#ifdef DVB_SUBTITLES
#include "av_sub.h"
#endif

double sub_last_pts = -303;

#ifdef CONFIG_ASS
#include "libass/ass_mp.h"
ass_track_t* ass_track = 0; // current track to render
#endif

sub_data* subdata = NULL;
subtitle* vo_sub_last = NULL;

#ifdef DVB_SUBTITLES
extern int pgs_subtitle_flag;
#endif
#include "stream/stream_dvdnav.h"
extern DvdnavState dvdnavstate;

#ifdef DOLBY_DVD
#include "stream/stream_dvdnav.h"
extern DvdnavState dvdnavstate;
#endif

//Fuchun 2010.10.18
static void fix_ass_packet(unsigned char *buffer, unsigned char **ass_buf,  int buf_len, int *ass_len, double cur_pts, double end_pts)
{
    char *layer, *ptr = buffer, *end = buffer+buf_len;
    unsigned char *line;
    for (; *ptr!=',' && ptr<end-1; ptr++);
    if (*ptr == ',')
        layer = ++ptr;
    for (; *ptr!=',' && ptr<end-1; ptr++);
    if (*ptr == ',') {
        int sc = cur_pts*100;
        int ec = end_pts*100;
        int sh, sm, ss, eh, em, es, len;
        sh = sc/360000;  sc -= 360000*sh;
        sm = sc/  6000;  sc -=   6000*sm;
        ss = sc/   100;  sc -=    100*ss;
        eh = ec/360000;  ec -= 360000*eh;
        em = ec/  6000;  ec -=   6000*em;
        es = ec/   100;  ec -=    100*es;
        *ptr++ = '\0';
        len = 50 + end-ptr + 8;
        if (!(line = av_malloc(len)))
            return;
        snprintf(line,len,"Dialogue: %s,%d:%02d:%02d.%02d,%d:%02d:%02d.%02d,%s\r\n",
                 layer, sh, sm, ss, sc, eh, em, es, ec, ptr);
//        av_free(*buffer);
        *ass_buf = line;
        *ass_len = strlen(line);
    }
}

extern int teletext_subtitle_flag;


void print_version(const char* name)
{
    mp_msg(MSGT_CPLAYER, MSGL_INFO, MP_TITLE, name);

    /* Test for CPU capabilities (and corresponding OS support) for optimizing */
    GetCpuCaps(&gCpuCaps);
#if ARCH_X86
    mp_msg(MSGT_CPLAYER, MSGL_V,
	   "CPUflags:  MMX: %d MMX2: %d 3DNow: %d 3DNowExt: %d SSE: %d SSE2: %d SSSE3: %d\n",
	   gCpuCaps.hasMMX, gCpuCaps.hasMMX2,
	   gCpuCaps.has3DNow, gCpuCaps.has3DNowExt,
	   gCpuCaps.hasSSE, gCpuCaps.hasSSE2, gCpuCaps.hasSSSE3);
#if CONFIG_RUNTIME_CPUDETECT
    mp_msg(MSGT_CPLAYER,MSGL_V, MSGTR_CompiledWithRuntimeDetection);
#else
    mp_msg(MSGT_CPLAYER,MSGL_V, MSGTR_CompiledWithCPUExtensions);
if (HAVE_MMX)
    mp_msg(MSGT_CPLAYER,MSGL_V," MMX");
if (HAVE_MMX2)
    mp_msg(MSGT_CPLAYER,MSGL_V," MMX2");
if (HAVE_AMD3DNOW)
    mp_msg(MSGT_CPLAYER,MSGL_V," 3DNow");
if (HAVE_AMD3DNOWEXT)
    mp_msg(MSGT_CPLAYER,MSGL_V," 3DNowExt");
if (HAVE_SSE)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE");
if (HAVE_SSE2)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE2");
if (HAVE_SSSE3)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSSE3");
if (HAVE_CMOV)
    mp_msg(MSGT_CPLAYER,MSGL_V," CMOV");
    mp_msg(MSGT_CPLAYER,MSGL_V,"\n");
#endif /* CONFIG_RUNTIME_CPUDETECT */
#endif /* ARCH_X86 */
}


void update_subtitles(sh_video_t *sh_video, double refpts, demux_stream_t *d_dvdsub, int reset)
{
    unsigned char *packet=NULL;
    unsigned char *ass_packet=NULL;
    int len;
    int type = d_dvdsub->sh ? ((sh_sub_t *)d_dvdsub->sh)->type : 'v';
    static subtitle subs;
#ifdef VO_SUB_THREAD
	extern volatile int draw_osd_cmdqueue;
	if(draw_osd_cmdqueue){
		//printf("%s:%d draw_osd_cmdqueue not empty, skip updating subtitle once\n", __func__, __LINE__);
		return;
	}
#endif
    if (reset) {
        sub_clear_text(&subs, MP_NOPTS_VALUE);
        if (vo_sub) {
            set_osd_subtitle(NULL);
        }
        if (vo_spudec) {
            spudec_reset(vo_spudec);
            vo_osd_changed(OSDTYPE_SPU);
        }
        if ((d_dvdsub->demuxer->teletext != NULL) && (vo_osd_teletext_mode == 1)) {
		vo_osd_teletext_mode = 0;
		teletext_control(d_dvdsub->demuxer->teletext, TV_VBI_CONTROL_SET_MODE, &vo_osd_teletext_mode);
	}
#ifdef DVB_SUBTITLES
	if(type == 'p' || type == 'x' || type == 'b')
		reset_avsub(d_dvdsub->sh);
#endif
    }

    // find sub
    if (subdata) {
        if (sub_fps==0) sub_fps = sh_video ? sh_video->fps : 25;
        current_module = "find_sub";
        if (refpts > sub_last_pts || refpts < sub_last_pts-1.0) {
            find_sub(subdata, (refpts+sub_delay) *
                     (subdata->sub_uses_time ? 100. : sub_fps));
            if (vo_sub) vo_sub_last = vo_sub;
            // FIXME! frame counter...
            sub_last_pts = refpts;
        }
    }
//extern int dvdnav_button_on;
    // DVD sub:
    if (vo_config_count && vo_spudec &&
//        (vobsub_id >= 0 || (dvdsub_id >= 0 && type == 'v'))) {
        (vobsub_id >= 0 || (dvdsub_id >= 0 && type == 'v') || (dvdnavstate.dvdnav_button_on == 1))) {
        int timestamp;
        current_module = "spudec";
#include "libmpcodecs/sky_vdec_2.h"
extern vdec_svread_buf_t sky_nrb;

        if (vo_vobsub)
            spudec_heartbeat(vo_spudec, 90000*sh_video->timer);
        else
        {
//            if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
//                spudec_heartbeat(vo_spudec, 90000*sky_nrb.cur_pts);
//20110705 Robert use audio pts as spudec_heartbeat instead of video pts
extern double last_audio_pts;
                spudec_heartbeat(vo_spudec, 90000*last_audio_pts);
//            else
//                spudec_heartbeat(vo_spudec, 90000*sh_video->timer);
        }

        /* Get a sub packet from the DVD or a vobsub and make a timestamp
         * relative to sh_video->timer */
        while(1) {
            // Vobsub
            len = 0;
            if (vo_vobsub) {
                if (refpts+sub_delay >= 0) {
                    len = vobsub_get_packet(vo_vobsub, refpts+sub_delay,
                                            (void**)&packet, &timestamp);
                    if (len > 0) {
                        timestamp -= (refpts + sub_delay - sh_video->timer)*90000;
                        mp_dbg(MSGT_CPLAYER,MSGL_V,"\rVOB sub: len=%d v_pts=%5.3f v_timer=%5.3f sub=%5.3f ts=%d \n",len,refpts,sh_video->timer,timestamp / 90000.0,timestamp);
                    }
                }
            } else {
                // DVD sub
                len = ds_get_packet_sub(d_dvdsub, (unsigned char**)&packet);
                if (len > 0) {
                    // XXX This is wrong, sh_video->pts can be arbitrarily
                    // much behind demuxing position. Unfortunately using
                    // d_video->pts which would have been the simplest
                    // improvement doesn't work because mpeg specific hacks
                    // in video.c set d_video->pts to 0.
                    float x = d_dvdsub->pts - refpts;
//printf("\n==> got SUB packet, d_dvdsub->pts=%f\n", d_dvdsub->pts);
                    if (x > -20 && x < 20) // prevent missing subs on pts reset
                        timestamp = 90000*(sh_video->timer + d_dvdsub->pts
                                           + sub_delay - refpts);
                    else timestamp = 90000*(sh_video->timer + sub_delay);
                    mp_dbg(MSGT_CPLAYER, MSGL_V, "\rDVD sub: len=%d  "
                           "v_pts=%5.3f  s_pts=%5.3f  ts=%d \n", len,
                           refpts, d_dvdsub->pts, timestamp);

//                    if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                    {
//printf("==> orig timestamp=%d,   new times:%d  sh timer:%f\n", timestamp, (int)(d_dvdsub->pts+sub_delay)*90000, sh_video->timer);
timestamp = 90000*(d_dvdsub->pts + sub_delay);
                    }
                }
            }
            if (len<=0 || !packet) break;
            if (vo_vobsub || timestamp >= 0)
                spudec_assemble(vo_spudec, packet, len, timestamp);
        }

        if (spudec_changed(vo_spudec))
            vo_osd_changed(OSDTYPE_SPU);
    } else if (dvdsub_id >= 0 && (type == 't' || type == 'm' || type == 'a' || type == 'd')) {
        double curpts = refpts + sub_delay;
        double endpts;
	if (type == 'd') {
		if (!d_dvdsub->demuxer->teletext) {
			tt_stream_props tsp = {0};
			void *ptr = &tsp;
			if (teletext_control(NULL, TV_VBI_CONTROL_START, &ptr) == VBI_CONTROL_TRUE)
				d_dvdsub->demuxer->teletext = ptr;
			if (teletext_control(ptr, TV_VBI_CONTROL_GET_MODE, &vo_osd_teletext_mode) == VBI_CONTROL_TRUE)
			{
				// default set on teletext 
				if (vo_osd_teletext_mode == 0)
				{
					vo_osd_teletext_mode = 1;
					if (teletext_control(ptr, TV_VBI_CONTROL_SET_MODE, &vo_osd_teletext_mode) != VBI_CONTROL_TRUE)
					{
						mp_dbg(MSGT_CPLAYER, MSGL_V, "Can't enable teletext!!!");
					}
				}
			}
		} else {
			if (reset != 1)
			{
				if (vo_osd_teletext_mode == 0)
				{
					vo_osd_teletext_mode = 1;
					if (teletext_control(d_dvdsub->demuxer->teletext, TV_VBI_CONTROL_SET_MODE, &vo_osd_teletext_mode) != VBI_CONTROL_TRUE)
					{
						mp_dbg(MSGT_CPLAYER, MSGL_V, "Can't enable teletext!!!");
					}
				}
			}
		}
	}
        if (d_dvdsub->non_interleaved)
            ds_get_next_pts(d_dvdsub);
        while (d_dvdsub->first) {
            double subpts = ds_get_next_pts(d_dvdsub);
            if ((subpts > curpts) && type != 'd')
                break;
			/* Fixed teletext do not display follow video pts, 2011-01-17 */
			if ((type == 'd') && d_dvdsub && sh_video && d_dvdsub->pts > sh_video->pts)
				break;
            endpts = d_dvdsub->first->endpts;
            len = ds_get_packet_sub(d_dvdsub, &packet);
            if (type == 'm') {
                if (len < 2) continue;
                len = FFMIN(len - 2, AV_RB16(packet));
                packet += 2;
            }
            if (type == 'd') {
                if (d_dvdsub->demuxer->teletext) {
                    uint8_t *p = packet;
                    p++;
                    len--;
                    while (len >= 46) {
                        int sublen = p[1];
                        //if (p[0] == 2 || p[0] == 3)
			// only decode subtitle data
                        if (p[0] == 3)
                            teletext_control(d_dvdsub->demuxer->teletext,
                                TV_VBI_CONTROL_DECODE_DVB, p + 2);
                        p   += sublen + 2;
                        len -= sublen + 2;
                    }
                }
                continue;
            }
#ifdef CONFIG_ASS
            if (ass_enabled) {
                sh_sub_t* sh = d_dvdsub->sh;
                ass_track = sh ? sh->ass_track : NULL;
                if (!ass_track) continue;
                if (type == 'a') { // ssa/ass subs with libass
                	int ass_len = 0;
			fix_ass_packet(packet, &ass_packet, len, &ass_len, subpts, endpts);
//			printf("### In [%s] [%d]   ass[%s] ###\n", __func__, __LINE__, ass_packet);
			if(ass_packet && ass_len > 10 && memcmp(ass_packet, "Dialogue: ", 10) == 0)
				ass_process_data(ass_track, ass_packet, ass_len);
			else
				ass_process_chunk(ass_track, packet, len,
							(long long)(subpts*1000 + 0.5),
							(long long)((endpts-subpts)*1000 + 0.5));
			if (ass_packet)
			    av_free(ass_packet);
                } else { // plaintext subs with libass
                    if (subpts != MP_NOPTS_VALUE) {
                        subtitle tmp_subs = {0};
                        if (endpts == MP_NOPTS_VALUE) endpts = subpts + 3;
                        sub_add_text(&tmp_subs, packet, len, endpts);
                        tmp_subs.start = subpts * 100;
                        tmp_subs.end = endpts * 100;
                        ass_process_subtitle(ass_track, &tmp_subs);
                        sub_clear_text(&tmp_subs, MP_NOPTS_VALUE);
                    }
                }
                continue;
            }
#endif
            if (subpts != MP_NOPTS_VALUE) {
                if (endpts == MP_NOPTS_VALUE)
                    sub_clear_text(&subs, MP_NOPTS_VALUE);
                if (type == 'a') { // ssa/ass subs without libass => convert to plaintext
                    int i;
                    unsigned char* p = packet;
                    for (i=0; i < 8 && *p != '\0'; p++)
                        if (*p == ',')
                            i++;
                    if (*p == '\0')  /* Broken line? */
                        continue;
                    len -= p - packet;
                    packet = p;
                }
                sub_add_text(&subs, packet, len, endpts);
                set_osd_subtitle(&subs);
            }
            if (d_dvdsub->non_interleaved)
                ds_get_next_pts(d_dvdsub);
        }
#if (defined(CONFIG_ASS) && defined(MULTI_LAYER))
	if (sh_video && ass_enabled && (speed_mult == 0)) {
		sh_sub_t* sh = d_dvdsub->sh;
		ass_track = sh ? sh->ass_track : NULL;
		if (ass_track && sh_video->vfilter)
		{
			filter_video(sh_video, NULL, sh_video->pts);
		}
	}
#endif
        if (sub_clear_text(&subs, curpts))
            set_osd_subtitle(&subs);
    }
	
#ifdef DVB_SUBTITLES
	else if(type == 'p' || type == 'x' || type == 'b')
	{
		if(dvdsub_id >= 0)
		{
			pgs_subtitle_flag = 1;
			int orig_type = type;
			double endpts;
			//printf("@@@@ Carlos in [%s][%d] non_interleaved[%d]####\n", __func__, __LINE__, d_dvdsub->non_interleaved);
			if (d_dvdsub->non_interleaved)
			{
				static int check_cnt = 0;
				if(check_cnt > 15)
				{
					ds_get_next_pts(d_dvdsub);
					check_cnt = 0;
				}
				check_cnt++;
			}

			while (1)
			{
				double subpts = refpts + sub_delay;
				type = orig_type;
				len = ds_get_packet_dvbsub(d_dvdsub, &packet, &subpts, &endpts);
				if (len < 0)
					break;

				type = decode_avsub(d_dvdsub->sh, &packet, &len, &subpts, &endpts);
				if (type <= 0)
					continue;

				if (subpts != MP_NOPTS_VALUE)
				{
					if (endpts == MP_NOPTS_VALUE)
						sub_clear_text(&subs, MP_NOPTS_VALUE);
					sub_add_text(&subs, packet, len, endpts);
					set_osd_subtitle(&subs);
				}

//				if (d_dvdsub->non_interleaved)
//					ds_get_next_pts(d_dvdsub);
			}
			if (sub_clear_text(&subs, refpts + sub_delay))
				set_osd_subtitle(&subs);

			if (vo_spudec)
			{
//				printf("@@@ In[%s][%d]  [%f] @@@\n", __func__, __LINE__, (refpts + sub_delay));
				spudec_heartbeat(vo_spudec, 90000*(refpts + sub_delay));
				if (spudec_changed(vo_spudec))
					vo_osd_changed(OSDTYPE_SPU);
			}
		}
		else
		{
			pgs_subtitle_flag = 0;
			if(d_dvdsub->packs)
				ds_free_packs(d_dvdsub);

			if(vo_spudec)
			{
				spudec_reset(vo_spudec);
				vo_spudec = NULL;
			}			
		}
	}
	else if(type == 'd')
	{
		if(dvdsub_id < 0)
		{
			if(d_dvdsub->packs)
			{
				ds_free_packs(d_dvdsub);
			}
		}
	}
	else
	{
#if (defined(CONFIG_ASS) && defined(MULTI_LAYER))
		if (sh_video && ass_enabled && speed_mult == 0) {
			sh_sub_t* sh = d_dvdsub->sh;
			if (ass_track && sh_video->vfilter)
			{
				filter_video(sh_video, NULL, sh_video->pts);
			}
		}
#endif
	}
#endif //end of DVB_SUBTITLES

    current_module="end of update_subtitles";
}

void update_teletext(sh_video_t *sh_video, demuxer_t *demuxer, int reset)
{
    int page_changed;

    teletext_subtitle_flag = 0;
    if (!demuxer->teletext)
        return;

    if (reset)
    {
        struct tt_param tt_param;
	memset((void *)&tt_param, 0, sizeof(struct tt_param));
	teletext_control(demuxer->teletext, TV_VBI_CONTROL_RESET, &tt_param);
    }

    //Also forcing page update when such ioctl is not supported or call error occured
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_IS_CHANGED,&page_changed)!=VBI_CONTROL_TRUE)
        page_changed=1;

    if(!page_changed)
        return;

    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_VBIPAGE,&vo_osd_teletext_page)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_page=NULL;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_HALF_PAGE,&vo_osd_teletext_half)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_half=0;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_MODE,&vo_osd_teletext_mode)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_mode=0;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_FORMAT,&vo_osd_teletext_format)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_format=0;
	
    vo_osd_changed(OSDTYPE_TELETEXT);

    teletext_control(demuxer->teletext,TV_VBI_CONTROL_MARK_UNCHANGED,NULL);
    teletext_subtitle_flag = 1;
}

int select_audio(demuxer_t* demuxer, int audio_id, char* audio_lang)
{
    if (audio_id == -1 && audio_lang)
        audio_id = demuxer_audio_track_by_lang(demuxer, audio_lang);
    if (audio_id == -1)
        audio_id = demuxer_default_audio_track(demuxer);
    if (audio_id != -1) // -1 (automatic) is the default behaviour of demuxers
        demuxer_switch_audio(demuxer, audio_id);
    if (audio_id == -2) { // some demuxers don't yet know how to switch to no sound
        demuxer->audio->id = -2;
        demuxer->audio->sh = NULL;
    }
    return demuxer->audio->id;
}

/* Parse -noconfig common to both programs */
int disable_system_conf=0;
int disable_user_conf=0;
#ifdef CONFIG_GUI
int disable_gui_conf=0;
#endif /* CONFIG_GUI */

/* Disable all configuration files */
static void noconfig_all(void)
{
    disable_system_conf = 1;
    disable_user_conf = 1;
#ifdef CONFIG_GUI
    disable_gui_conf = 1;
#endif /* CONFIG_GUI */
}

const m_option_t noconfig_opts[] = {
    {"all", noconfig_all, CONF_TYPE_FUNC, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 0, NULL},
    {"system", &disable_system_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
    {"user", &disable_user_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
#ifdef CONFIG_GUI
    {"gui", &disable_gui_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
#endif /* CONFIG_GUI */
    {NULL, NULL, 0, 0, 0, 0, NULL}
};
