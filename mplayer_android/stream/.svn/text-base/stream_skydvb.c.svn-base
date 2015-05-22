/*

dvbstream
(C) Dave Chapman <dave@dchapman.com> 2001, 2002.

The latest version can be found at http://www.linuxstb.org/dvbstream

Modified for use with MPlayer, for details see the changelog at
http://svn.mplayerhq.hu/mplayer/trunk/
$Id: stream_dvb.c 24277 2007-08-28 22:38:45Z diego $

Copyright notice:

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/demux_ts.h" // carlos add for debug message
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"
#include "get_path.h"
#include "libavutil/avstring.h"

#include "skydvbin.h"
#include "dvb/dmx.h"

#define MAX_CHANNELS 8
#define CHANNEL_LINE_LEN 256
#define min(a, b) ((a) <= (b) ? (a) : (b))

#define VIDEO_READ_LENGTH	1024*512
#define AUDIO_READ_LENGTH	1024*512

int dvb_pfm_is_start=0;
int fd_pfm=-1;

extern unsigned int skydroid;   //SkyViia_Vincent02102010
extern int dvb_demux_stop(int fd);
static void dvbin_close(stream_t *stream);
#ifdef QT_SUPPORT_DVBT
int decode_video_cnts_5min=0;
int decode_audio_cnts_5min=0;
int dvb_open_devices(dvb_priv_t *priv, int n, int demux_cnt, int *pids);
extern int quiet;
extern int video_id;
extern int audio_id;
//TODO: CAMBIARE list_ptr e da globale a per_priv
static unsigned char *dvb_local_buffer=0, *dvb_local_buffer2=0;
// carlos add for change default conf, 20100804
static char *default_conf_file = NULL;
static char change_conf = 0;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
extern int unsigned stop_tuner;
extern int setdvb_by_pid;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID	
#define DVB_LOCAL_VIDEO_IDX_MAX (512*2)//(256)
#define DVB_LOCAL_AUDIO_IDX_MAX (512)//(256)
#define DVB_LOCAL_VIDEO_BUF_MAX (1024*1024*8) //(1024*1024*8)//(1024*1024*6)//(1024*1024*8) //(1024*1024*4)
#define DVB_LOCAL_AUDIO_BUF_MAX (1024*(12*12+16)*5)	//(1024*1024) //1024*512)

#ifdef DVBT_USING_NORMAL_METHOD
//extern int skydvb_thread_running;
//extern void stop_skydvb_thread(void);
//int do_it_first=1;
#else // else of DVBT_USING_NORMAL_METHOD
static unsigned char *dvb_local_video_buffer=0, *dvb_local_audio_buffer=0;
static int consume_cnt=0, dvb_read_method=0;

//static struct dmx_stc dvb_stc;
int64_t g_STC_frame=0;

int dvb_is_read_video=0;

extern demux_stream_t *g_skyts_video, *g_skyts_audio;
extern void demux_skyts_CopyToDemuxPacket( demux_stream_t *ds, unsigned char *buffer, int size, off_t pos, int64_t pts );


static pthread_mutex_t skydvb_mutex, skydvb_mutex_vq, skydvb_mutex_aq;
static int skydvb_video_ptr_idx=0, skydvb_audio_ptr_idx=0;	//index to store buffer
static int skydvb_video_cur_idx=0, skydvb_audio_cur_idx=0; //index to index table for read from demux
static int skydvb_video_get_idx=0, skydvb_audio_get_idx=0; //index to index table for get buffer
static int skydvb_video_queue_cnts=0, skydvb_audio_queue_cnts=0; //total index counter
static int v_hdr[DVB_LOCAL_VIDEO_IDX_MAX][4];
static int a_hdr[DVB_LOCAL_AUDIO_IDX_MAX][4];

static int skydvb_read_stream(stream_t *stream);
static int skydvb_thread_running = 0;
static int do_it_first=1;
static pthread_t skydvb_read_thread;
#endif // end of DVBT_USING_NORMAL_METHOD

static struct stream_priv_s
{
    char *prog;
    int card;
    char *type;
    int vid, aid;
    int timeout;
    char *file;
}
stream_defaults =
{
    "", 1, "", 0, 0, 5 /*30*/, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s, f)

/// URL definition
static m_option_t stream_params[] = {
    {"prog", ST_OFF(prog), CONF_TYPE_STRING, 0, 0 ,0, NULL},
    {"card", ST_OFF(card), CONF_TYPE_INT, M_OPT_RANGE, 1, 4, NULL},
    {"type", ST_OFF(type), CONF_TYPE_STRING, 0, 0 ,0, NULL},
    {"vid",  ST_OFF(vid),  CONF_TYPE_INT, 0, 0 ,0, NULL},
    {"aid",  ST_OFF(aid),  CONF_TYPE_INT, 0, 0 ,0, NULL},
    {"timeout",ST_OFF(timeout),  CONF_TYPE_INT, M_OPT_RANGE, 1, 5 /*30*/, NULL},
    {"file", ST_OFF(file), CONF_TYPE_STRING, 0, 0 ,0, NULL},

    {"hostname", 	ST_OFF(prog), CONF_TYPE_STRING, 0, 0, 0, NULL },
    {"username", 	ST_OFF(card), CONF_TYPE_INT, M_OPT_RANGE, 1, 4, NULL},
    {NULL, NULL, 0, 0, 0, 0, NULL}
};

static struct m_struct_st stream_opts = {
    "dvbin",
    sizeof(struct stream_priv_s),
    &stream_defaults,
    stream_params
};

m_option_t dvbin_opts_conf[] = {
    {"prog", &stream_defaults.prog, CONF_TYPE_STRING, 0, 0 ,0, NULL},
    {"card", &stream_defaults.card, CONF_TYPE_INT, M_OPT_RANGE, 1, 4, NULL},
    {"type", "DVB card type is autodetected and can't be overridden\n", CONF_TYPE_PRINT, CONF_NOCFG, 0 ,0, NULL},
    {"vid",  &stream_defaults.vid,  CONF_TYPE_INT, 0, 0 ,0, NULL},
    {"aid",  &stream_defaults.aid,  CONF_TYPE_INT, 0, 0 ,0, NULL},
    {"timeout",  &stream_defaults.timeout,  CONF_TYPE_INT, M_OPT_RANGE, 1, 5 /*30*/, NULL},
    {"file", &stream_defaults.file, CONF_TYPE_STRING, 0, 0 ,0, NULL},

    {NULL, NULL, 0, 0, 0, 0, NULL}
};
extern int dvb_set_ts_filt(int fd, uint16_t pid, dmx_pes_type_t pestype);
extern int dvb_get_tuner_type(int fd);
int dvb_fix_demuxes(dvb_priv_t *priv, int cnt, int *pids);
extern int dvb_tune(dvb_priv_t *priv, int freq, char pol, int srate, int diseqc, int tone,
                        fe_spectral_inversion_t specInv, fe_modulation_t modulation, fe_guard_interval_t guardInterval,
                        fe_transmit_mode_t TransmissionMode, fe_bandwidth_t bandWidth, fe_code_rate_t HP_CodeRate,
                        fe_code_rate_t LP_CodeRate, fe_hierarchy_t hier, int timeout);
extern char *dvb_dvrdev[4], *dvb_demuxdev[4], *dvb_frontenddev[4];

static dvb_config_t *dvb_config = NULL;

#ifdef DVBT_USING_NORMAL_METHOD
int dvb_get_vq_count(void)
{
	return 0;
}
int dvb_get_aq_count(void)
{
	return 0;
}
#else // else of DVBT_USING_NORMAL_METHOD
int dvb_get_vq_count(void)
{
    return skydvb_video_queue_cnts;
}
int dvb_get_aq_count(void)
{
    return skydvb_audio_queue_cnts;
}

int64_t dvb_get_stc(stream_t *stream)
{
    struct dmx_stc stc;
    dvb_priv_t *priv  = (dvb_priv_t *) stream->priv;
    stc.num = 0;

    ioctl(priv->demux_fds[0], DMX_GET_STC, &stc);

    if (stc.stc < 0)
    {
        g_STC_frame = -stc.stc;
        g_STC_frame = ((~(g_STC_frame))&0x00000000ffffffff);
        g_STC_frame |= 0x0000000100000000;
    }
    else
    {
        g_STC_frame = stc.stc;
    }
    return (int64_t)g_STC_frame;
}
#endif // end of DVBT_USING_NORMAL_METHOD

void dvb_pfm_buffer_update(unsigned int size)
{
    extern int use_PP_buf;
    ioctl(fd_pfm, DMX_PFM_BUFFER_UPDATE, &size);
}

static dvb_channels_list *dvb_get_channels(char *filename, int type)
{
    dvb_channels_list  *list;
    FILE *f;
    char line[CHANNEL_LINE_LEN], *colon;

    int fields, cnt, pcnt, k;
    int has8192, has0;
    dvb_channel_t *ptr, *tmp, chn;
    char tmp_lcr[256], tmp_hier[256], inv[256], bw[256], cr[256], mod[256], transm[256], gi[256], vpid_str[256], apid_str[256];
    const char *cbl_conf = "%d:%255[^:]:%d:%255[^:]:%255[^:]:%255[^:]:%255[^:]\n";
    const char *sat_conf = "%d:%c:%d:%d:%255[^:]:%255[^:]\n";
    //const char *ter_conf = "%d:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]\n";
	// carlos add program id, 2010-07-22
    const char *ter_conf = "%d:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]:%255[^:]\n";
	char ppid_str[256];
    const char *atsc_conf = "%d:%255[^:]:%255[^:]:%255[^:]\n";
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	const char *new_dvb_name = "dvb://%d:%d:%d:%d:%d:%d";
	extern int setdvb_by_pid;
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID

    mp_msg(MSGT_DEMUX, MSGL_V, "CONFIG_READ FILE: %s, type: %d\n", filename, type);
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
    skydvb_printf("@@@@ In [%s][%d] CONFIG_READ FILE: %s, type: %d@@@@@ setdvb_by_pid[%d]\n", __func__, __LINE__, filename, type, setdvb_by_pid);
	if (setdvb_by_pid)
	{
		unsigned int vpid = 0;
		unsigned int apid = 0;
		unsigned int spid = 0;
		unsigned int v_type= 0;
		unsigned int a_type= 0;
		unsigned int s_type= 0;

		list = malloc(sizeof(dvb_channels_list));
		if (list == NULL)
		{
			printf("@@@In [%s][%d] : DVB_GET_CHANNELS: couldn't malloc enough memory\n", __func__, __LINE__);
			return NULL;
		}
		list->NUM_CHANNELS = 1;
		list->channels = NULL;
		list->channels = malloc (sizeof(dvb_channel_t));
		if (list->channels)
		{
			skydvb_printf("@@@ Before call sscan file name is [%s] @@@\n", filename);
			list->channels->pids_cnt = 4;
			list->channels->name = strdup("skytv");
			fields = sscanf(filename, new_dvb_name,	&(list->channels->pids[0]), &(list->channels->pids_format[0]), &(list->channels->pids[1]), &(list->channels->pids_format[1]), &(list->channels->pids[2]), &(list->channels->pids_format[2]));
			skydvb_printf("@@@ In [%s][%d] filename [%s] vpid[%d] vtype[%d] apid[%d] atype[%d] spid[%d] stype[%d] @@@\n", __func__, __LINE__, filename, list->channels->pids[0], list->channels->pids_format[0], list->channels->pids[1], list->channels->pids_format[1], list->channels->pids[2], list->channels->pids_format[2]);
		}
	}
	else
	{
#endif // end of  SUPPORT_SKYDVB_DYNAMIC_PID	
		if ((f=fopen(filename, "r"))==NULL)
		{
			mp_msg(MSGT_DEMUX, MSGL_FATAL, "CAN'T READ CONFIG FILE %s\n", filename);
			return NULL;
		}

		list = malloc(sizeof(dvb_channels_list));
		if (list == NULL)
		{
			fclose(f);
			mp_msg(MSGT_DEMUX, MSGL_V, "DVB_GET_CHANNELS: couldn't malloc enough memory\n");
			return NULL;
		}

		ptr = &chn;
		list->NUM_CHANNELS = 0;
		list->channels = NULL;
		while (! feof(f))
		{
			if ( fgets(line, CHANNEL_LINE_LEN, f) == NULL )
				continue;

			if ((line[0] == '#') || (strlen(line) == 0))
				continue;

			colon = strchr(line, ':');
			if (colon)
			{
				k = colon - line;
				if (!k)
					continue;
				ptr->name = malloc(k+1);
				if (! ptr->name)
					continue;
				av_strlcpy(ptr->name, line, k+1);
			}
			else
				continue;
			k++;
			apid_str[0] = vpid_str[0] = 0;
			ppid_str[0] = 0; // carlos add 2010-07-22
			ptr->pids_cnt = 0;
			ptr->freq = 0;
			if (type == TUNER_TER)
			{
				fields = sscanf(&line[k], ter_conf,
						&ptr->freq, inv, bw, cr, tmp_lcr, mod,
						transm, gi, tmp_hier, vpid_str, apid_str, ppid_str); // carlos 2010-07-22
				//                            transm, gi, tmp_hier, vpid_str, apid_str);
				mp_msg(MSGT_DEMUX, MSGL_V,
						"TER, NUM: %d, NUM_FIELDS: %d, NAME: %s, FREQ: %d",
						list->NUM_CHANNELS, fields, ptr->name, ptr->freq);
			}
			else if (type == TUNER_CBL)
			{
				fields = sscanf(&line[k], cbl_conf,
						&ptr->freq, inv, &ptr->srate,
						cr, mod, vpid_str, apid_str);
				mp_msg(MSGT_DEMUX, MSGL_V,
						"CBL, NUM: %d, NUM_FIELDS: %d, NAME: %s, FREQ: %d, SRATE: %d",
						list->NUM_CHANNELS, fields, ptr->name, ptr->freq, ptr->srate);
			}
#ifdef DVB_ATSC
			else if (type == TUNER_ATSC)
			{
				fields = sscanf(&line[k], atsc_conf,
						&ptr->freq, mod, vpid_str, apid_str);
				mp_msg(MSGT_DEMUX, MSGL_V,
						"ATSC, NUM: %d, NUM_FIELDS: %d, NAME: %s, FREQ: %d\n",
						list->NUM_CHANNELS, fields, ptr->name, ptr->freq);
			}
#endif
			else //SATELLITE
			{
				fields = sscanf(&line[k], sat_conf,
						&ptr->freq, &ptr->pol, &ptr->diseqc, &ptr->srate, vpid_str, apid_str);
				ptr->pol = toupper(ptr->pol);
				ptr->freq *=  1000UL;
				ptr->srate *=  1000UL;
				ptr->tone = -1;
				ptr->inv = INVERSION_AUTO;
				ptr->cr = FEC_AUTO;
				if ((ptr->diseqc > 4) || (ptr->diseqc < 0))
					continue;
				if (ptr->diseqc > 0)
					ptr->diseqc--;
				mp_msg(MSGT_DEMUX, MSGL_V,
						"SAT, NUM: %d, NUM_FIELDS: %d, NAME: %s, FREQ: %d, SRATE: %d, POL: %c, DISEQC: %d",
						list->NUM_CHANNELS, fields, ptr->name, ptr->freq, ptr->srate, ptr->pol, ptr->diseqc);
			}

			if (vpid_str[0])
			{
				pcnt = sscanf(vpid_str, "%d+%d+%d+%d+%d+%d+%d", &ptr->pids[0], &ptr->pids[1], &ptr->pids[2], &ptr->pids[3],
						&ptr->pids[4], &ptr->pids[5], &ptr->pids[6]);
				if (pcnt > 0)
				{
					ptr->pids_cnt = pcnt;
					fields++;
				}
			}

			if (apid_str[0])
			{
				cnt = ptr->pids_cnt;
				pcnt = sscanf(apid_str, "%d+%d+%d+%d+%d+%d+%d+%d", &ptr->pids[cnt], &ptr->pids[cnt+1], &ptr->pids[cnt+2],
						&ptr->pids[cnt+3], &ptr->pids[cnt+4], &ptr->pids[cnt+5], &ptr->pids[cnt+6], &ptr->pids[cnt+7]);
				if (pcnt > 0)
				{
					ptr->pids_cnt += pcnt;
					fields++;
				}
			}
			// carlos 2010-07-22, parse program num
			if (ppid_str[0])
			{
				cnt = ptr->pids_cnt;
				pcnt = sscanf(ppid_str, "%d+%d+%d+%d+%d+%d+%d+%d", &ptr->pids[cnt], &ptr->pids[cnt+1], &ptr->pids[cnt+2],
						&ptr->pids[cnt+3], &ptr->pids[cnt+4], &ptr->pids[cnt+5], &ptr->pids[cnt+6], &ptr->pids[cnt+7]);
				if (pcnt > 0)
				{
					ptr->pids_cnt += pcnt;
					fields++;
				}
			}

			if ((fields < 2) || (ptr->pids_cnt <= 0) || (ptr->freq == 0) || (strlen(ptr->name) == 0))
				continue;

			has8192 = has0 = 0;
			for (cnt = 0; cnt < ptr->pids_cnt; cnt++)
			{
				if (ptr->pids[cnt] == 8192)
					has8192 = 1;
				if (ptr->pids[cnt] == 0)
					has0 = 1;
			}
			if (has8192)
			{
				ptr->pids[0] = 8192;
				ptr->pids_cnt = 1;
			}
			else if (! has0)
			{
				ptr->pids[ptr->pids_cnt] = 0;	//PID 0 is the PAT
				ptr->pids_cnt++;
			}
			mp_msg(MSGT_DEMUX, MSGL_V, " PIDS: ");
			for (cnt = 0; cnt < ptr->pids_cnt; cnt++)
				mp_msg(MSGT_DEMUX, MSGL_V, " %d ", ptr->pids[cnt]);
			mp_msg(MSGT_DEMUX, MSGL_V, "\n");

			if ((type == TUNER_TER) || (type == TUNER_CBL))
			{
				if (! strcmp(inv, "INVERSION_ON"))
					ptr->inv = INVERSION_ON;
				else if (! strcmp(inv, "INVERSION_OFF"))
					ptr->inv = INVERSION_OFF;
				else
					ptr->inv = INVERSION_AUTO;


				if (! strcmp(cr, "FEC_1_2"))
					ptr->cr =FEC_1_2;
				else if (! strcmp(cr, "FEC_2_3"))
					ptr->cr =FEC_2_3;
				else if (! strcmp(cr, "FEC_3_4"))
					ptr->cr =FEC_3_4;
#ifdef HAVE_DVB_HEAD
				else if (! strcmp(cr, "FEC_4_5"))
					ptr->cr =FEC_4_5;
				else if (! strcmp(cr, "FEC_6_7"))
					ptr->cr =FEC_6_7;
				else if (! strcmp(cr, "FEC_8_9"))
					ptr->cr =FEC_8_9;
#endif
				else if (! strcmp(cr, "FEC_5_6"))
					ptr->cr =FEC_5_6;
				else if (! strcmp(cr, "FEC_7_8"))
					ptr->cr =FEC_7_8;
				else if (! strcmp(cr, "FEC_NONE"))
					ptr->cr =FEC_NONE;
				else ptr->cr =FEC_AUTO;
			}


			if ((type == TUNER_TER) || (type == TUNER_CBL) || (type == TUNER_ATSC))
			{
				if (! strcmp(mod, "QAM_128"))
					ptr->mod = QAM_128;
				else if (! strcmp(mod, "QAM_256"))
					ptr->mod = QAM_256;
				else if (! strcmp(mod, "QAM_64"))
					ptr->mod = QAM_64;
				else if (! strcmp(mod, "QAM_32"))
					ptr->mod = QAM_32;
				else if (! strcmp(mod, "QAM_16"))
					ptr->mod = QAM_16;
#ifdef DVB_ATSC
				else if (! strcmp(mod, "VSB_8") || ! strcmp(mod, "8VSB"))
					ptr->mod = VSB_8;
				else if (! strcmp(mod, "VSB_16") || !strcmp(mod, "16VSB"))
					ptr->mod = VSB_16;

				ptr->inv = INVERSION_AUTO;
#endif
			}

			if (type == TUNER_TER)
			{
				if (! strcmp(bw, "BANDWIDTH_6_MHZ"))
					ptr->bw = BANDWIDTH_6_MHZ;
				else if (! strcmp(bw, "BANDWIDTH_7_MHZ"))
					ptr->bw = BANDWIDTH_7_MHZ;
				else if (! strcmp(bw, "BANDWIDTH_8_MHZ"))
					ptr->bw = BANDWIDTH_8_MHZ;


				if (! strcmp(transm, "TRANSMISSION_MODE_2K"))
					ptr->trans = TRANSMISSION_MODE_2K;
				else if (! strcmp(transm, "TRANSMISSION_MODE_8K"))
					ptr->trans = TRANSMISSION_MODE_8K;


				if (! strcmp(gi, "GUARD_INTERVAL_1_32"))
					ptr->gi = GUARD_INTERVAL_1_32;
				else if (! strcmp(gi, "GUARD_INTERVAL_1_16"))
					ptr->gi = GUARD_INTERVAL_1_16;
				else if (! strcmp(gi, "GUARD_INTERVAL_1_8"))
					ptr->gi = GUARD_INTERVAL_1_8;
				else ptr->gi = GUARD_INTERVAL_1_4;

				if (! strcmp(tmp_lcr, "FEC_1_2"))
					ptr->cr_lp =FEC_1_2;
				else if (! strcmp(tmp_lcr, "FEC_2_3"))
					ptr->cr_lp =FEC_2_3;
				else if (! strcmp(tmp_lcr, "FEC_3_4"))
					ptr->cr_lp =FEC_3_4;
#ifdef HAVE_DVB_HEAD
				else if (! strcmp(tmp_lcr, "FEC_4_5"))
					ptr->cr_lp =FEC_4_5;
				else if (! strcmp(tmp_lcr, "FEC_6_7"))
					ptr->cr_lp =FEC_6_7;
				else if (! strcmp(tmp_lcr, "FEC_8_9"))
					ptr->cr_lp =FEC_8_9;
#endif
				else if (! strcmp(tmp_lcr, "FEC_5_6"))
					ptr->cr_lp =FEC_5_6;
				else if (! strcmp(tmp_lcr, "FEC_7_8"))
					ptr->cr_lp =FEC_7_8;
				else if (! strcmp(tmp_lcr, "FEC_NONE"))
					ptr->cr_lp =FEC_NONE;
				else ptr->cr_lp =FEC_AUTO;


				if (! strcmp(tmp_hier, "HIERARCHY_1"))
					ptr->hier = HIERARCHY_1;
				else if (! strcmp(tmp_hier, "HIERARCHY_2"))
					ptr->hier = HIERARCHY_2;
				else if (! strcmp(tmp_hier, "HIERARCHY_4"))
					ptr->hier = HIERARCHY_4;
#ifdef HAVE_DVB_HEAD
				else if (! strcmp(tmp_hier, "HIERARCHY_AUTO"))
					ptr->hier = HIERARCHY_AUTO;
#endif
				else	ptr->hier = HIERARCHY_NONE;
			}

			tmp = realloc(list->channels, sizeof(dvb_channel_t) * (list->NUM_CHANNELS + 1));
			if (tmp == NULL)
				break;

			list->channels = tmp;
			memcpy(&(list->channels[list->NUM_CHANNELS]), ptr, sizeof(dvb_channel_t));
			list->NUM_CHANNELS++;
			if (sizeof(dvb_channel_t) * list->NUM_CHANNELS >= 1024*1024)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "dvbin.c, > 1MB allocated for channels struct, dropping the rest of the file\r\n");
				break;
			}
		}

		fclose(f);
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
    if (list->NUM_CHANNELS == 0)
    {
        if (list->channels != NULL)
            free(list->channels);
        free(list);
        return NULL;
    }

    list->current = 0;
    return list;
}


#ifdef DVBT_USING_NORMAL_METHOD
int dvb_streaming_update_idx(int idx)
{
	return 0;
}
#else // else of DVBT_USING_NORMAL_METHOD
int dvb_streaming_update_idx(int idx)
{
    consume_cnt = idx;
    if (idx == -1)
    {
        dvb_read_method=1;
////		do_it_first =0;
    }
}
#endif // end of DVBT_USING_NORMAL_METHOD

#if USE_DIRECT_STREAM_READ
int dvb_streaming_read(demuxer_t *demux, stream_t *stream, char *buffer, int size)
#else
static int dvb_streaming_read(stream_t *stream, char *buffer, int size)
#endif
{
    
#ifdef DVBT_USING_NORMAL_METHOD 
	int pos = 0;
#else // else of DVBT_USING_NORMAL_METHOD
    struct pollfd pfds[1];
    int pos=0, rk, fd;
    int check_got_av=0;
    int need_read_cnt=127, need_min_cnt = 4;
    struct timeval tv;
    int got_video_packet=0;
    clock_t check_read_time=0;

    dvb_priv_t *priv  = (dvb_priv_t *) stream->priv;

    dvb_local_buffer = buffer;
    if (do_it_first == 1)
    {
        dvb_channel_t *channel = &(priv->list->channels[priv->list->current]);
//        printf("==== got channel->pids[0]=%d  fd=%d  stream->fd=%d\n", channel->pids[0], priv->demux_fds[0], stream->fd);
        dvb_update_fd(stream);
        dvb_streaming_update_idx(-1);



#ifndef USE_PFM_ADDR_MODE
        need_read_cnt = 127;
#else
        need_read_cnt = 16;
#endif

        if (skydvb_thread_running == 0)
        {
            if (dvb_local_video_buffer == 0)
            {
                dvb_local_video_buffer = malloc(DVB_LOCAL_VIDEO_BUF_MAX);
                dvb_local_audio_buffer = malloc(DVB_LOCAL_AUDIO_BUF_MAX);
            }
            else
            {
                memset(dvb_local_video_buffer, 0x00, DVB_LOCAL_VIDEO_BUF_MAX);
                memset(dvb_local_audio_buffer, 0x00, DVB_LOCAL_AUDIO_BUF_MAX);
            }

            pthread_create(&skydvb_read_thread, NULL, skydvb_read_stream, stream);
            pthread_mutex_init(&skydvb_mutex,NULL);
            pthread_mutex_init(&skydvb_mutex_vq,NULL);
            pthread_mutex_init(&skydvb_mutex_aq,NULL);

            while (skydvb_thread_running == 0)
            {
                usleep(10);
            }
        }
    }
#ifdef USE_PFM_ADDR_MODE
    else
    {
        if (dvb_is_read_video == 1)
        {
            need_read_cnt = 90;
            need_min_cnt = 30;
            tv.tv_usec = 1000;
        }
        else
        {
            need_read_cnt = 16;
            need_min_cnt = 4;
            tv.tv_usec = 1000;
        }
    }
#else
    else
    {
        if (dvb_is_read_video == 1)
        {
            need_read_cnt = 60;//20;//2;//20;
            tv.tv_usec = 1000;
        }
        else
        {
            need_read_cnt = 60;//20;
            tv.tv_usec = 1000;
        }
    }

#endif

    fd = stream->fd;

    if (consume_cnt <= 0)
    {
        unsigned int *hdr_ptr, sky_dvb_hdr[128][4];
        int sky_dvb_hdr_cnt=0;
        int check_video=0, check_audio=0;
        int ret;

        memset(dvb_local_buffer, 0x00, 16);
        consume_cnt = 2048;

        if (video_id != -2)
            check_video = 1;
        if (audio_id != -2)
            check_audio = 1;

        if (do_it_first == 0)
        {
            if (dvb_is_read_video == 1)
            {
                check_audio = 1;
            }
            else
            {
//		if (demux->video && demux->video < 60)
		if (demux->video)
	        {
			check_video = 1;
			need_read_cnt = 30;
		}
		else
		{
	                check_video = 0;
		}
            }
        }


//        while (consume_cnt < (USE_MAX_BUFFER_SIZE - (384)*1024) && sky_dvb_hdr_cnt < need_read_cnt)
        while (consume_cnt < (USE_MAX_BUFFER_SIZE - (384)*1024) && sky_dvb_hdr_cnt < need_read_cnt)
        {
            {
#ifndef USE_PFM_ADDR_MODE	//PFM DATA mode
                if (check_video == 1)
                {		//vpes
                    unsigned int *lptr = &dvb_local_buffer[consume_cnt];
                    int offset=0;

//                    pthread_mutex_lock(&skydvb_mutex);
                    if (skydvb_video_queue_cnts > 0)
                    {
                        ret = v_hdr[skydvb_video_get_idx][0];
			if (consume_cnt + ret > USE_MAX_BUFFER_SIZE)
			{
				break;
			}
                        memcpy(&dvb_local_buffer[consume_cnt], &dvb_local_video_buffer[v_hdr[skydvb_video_get_idx][1]], v_hdr[skydvb_video_get_idx][0]);
                        /*
                        	printf("==chunk: ");
                                {
                                    int pp;
                                    unsigned char *nptr = &dvb_local_buffer[consume_cnt];
                                    for (pp=0; pp<32; pp++)
                                    {
                                        printf("%.2x ", nptr[pp]);
                                    }
                                }
                                printf("\n");
                        */
                        //size
                        sky_dvb_hdr[sky_dvb_hdr_cnt][0] = ret;
                        //offset
                        sky_dvb_hdr[sky_dvb_hdr_cnt][1] = consume_cnt;
                        sky_dvb_hdr[sky_dvb_hdr_cnt][2] = dvb_local_buffer[consume_cnt+3];
                        sky_dvb_hdr[sky_dvb_hdr_cnt][3] = 0xe0;
                        sky_dvb_hdr_cnt++;
                        consume_cnt += ret;
                        consume_cnt = ((consume_cnt +3)/4)*4;

                        check_got_av |= 0x10;
                        got_video_packet = 1;
                        pthread_mutex_lock(&skydvb_mutex_vq);

                        skydvb_video_queue_cnts--;
                        skydvb_video_get_idx = (skydvb_video_get_idx+1)%DVB_LOCAL_VIDEO_IDX_MAX;
                        pthread_mutex_unlock(&skydvb_mutex_vq);

                        if (audio_id == -2)
                            check_got_av |= 0x11;

                    }
//                    pthread_mutex_unlock(&skydvb_mutex);
                }
#else	//PFM Addr mode
                if (FD_ISSET(priv->demux_fds[0], &rfds))
                {		//vpes
                    unsigned int *lptr = &dvb_local_buffer[consume_cnt];
                    int offset=0;
                    ret= read(priv->demux_fds[0], &dvb_local_buffer[consume_cnt+4], 0x1000);
                    *lptr = 0xff008860;	//special tag
                    ret += 4;
                    //size
                    sky_dvb_hdr[sky_dvb_hdr_cnt][0] = ret - offset;
                    //offset
                    sky_dvb_hdr[sky_dvb_hdr_cnt][1] = consume_cnt+offset;
                    sky_dvb_hdr[sky_dvb_hdr_cnt][2] = dvb_local_buffer[consume_cnt+3+offset];
                    sky_dvb_hdr[sky_dvb_hdr_cnt][3] = 0xe0;
                    sky_dvb_hdr_cnt++;
                    consume_cnt += ret;
                    consume_cnt = ((consume_cnt +3)/4)*4;
//                        got_video_packet = 1;

                    check_got_av |= 0x10;
                }

#endif
                if (check_audio == 1)
                {
                    unsigned int *lptr = &dvb_local_buffer[consume_cnt];
                    int offset=0;


                    /*
                        if (video_id != -2 && do_it_first == 1 && sky_dvb_hdr_cnt == 0)
                        {
                            printf("@@@ skip audio frame... wait first video frame video_id=%d\n", video_id);
                            continue;
                        }
                    */
//                    pthread_mutex_lock(&skydvb_mutex);
                    if (skydvb_audio_queue_cnts > 0)
                    {
                        ret = a_hdr[skydvb_audio_get_idx][0];
			if (consume_cnt + ret > USE_MAX_BUFFER_SIZE)
			{
				break;
			}
                        memcpy(&dvb_local_buffer[consume_cnt], &dvb_local_audio_buffer[a_hdr[skydvb_audio_get_idx][1]], a_hdr[skydvb_audio_get_idx][0]);
                        //size
                        sky_dvb_hdr[sky_dvb_hdr_cnt][0] = ret;
                        //offset
                        sky_dvb_hdr[sky_dvb_hdr_cnt][1] = consume_cnt;
                        sky_dvb_hdr[sky_dvb_hdr_cnt][2] = dvb_local_buffer[consume_cnt+3];
                        sky_dvb_hdr[sky_dvb_hdr_cnt][3] = 0x03;
                        sky_dvb_hdr_cnt++;
                        consume_cnt += ret;
                        consume_cnt = ((consume_cnt +3)/4)*4;

                        check_got_av |= 0x01;
//                        got_audio_packet = 1;
                        pthread_mutex_lock(&skydvb_mutex_aq);
                        skydvb_audio_queue_cnts--;
                        skydvb_audio_get_idx = (skydvb_audio_get_idx+1)%DVB_LOCAL_AUDIO_IDX_MAX;
                        pthread_mutex_unlock(&skydvb_mutex_aq);
                        decode_audio_cnts_5min++;
                        if (video_id == -2)
                            check_got_av |= 0x11;

                    }
//                    pthread_mutex_unlock(&skydvb_mutex);
                }
            }

            if (do_it_first == 1)
            {
                if (check_got_av == 0x11)
                {
                    do_it_first = 0;
                    break;
                }
            }
//            pthread_mutex_lock(&skydvb_mutex);
            if (check_video == 1 && check_audio == 0)
            {
                if (skydvb_video_queue_cnts == 0)
                {
//                    pthread_mutex_unlock(&skydvb_mutex);
                    break;
                }
            }
            else if (check_video == 0 && check_audio == 1)
            {
                if (skydvb_audio_queue_cnts == 0)
                {
//                    pthread_mutex_unlock(&skydvb_mutex);
                    break;
                }
            }
            else if (check_video == 1 && check_audio == 1)
            {
                if (skydvb_video_queue_cnts == 0 && skydvb_audio_queue_cnts == 0)
                {
//                    pthread_mutex_unlock(&skydvb_mutex);
                    break;
                }
            }
//            pthread_mutex_unlock(&skydvb_mutex);
//printf("GetTimer()=%f\n", (float)GetTimer());
            if (GetTimer() - check_read_time > 0)
            {
                //more then 10ms
                break;
            }


        }
        hdr_ptr = (unsigned int *)&dvb_local_buffer[0];
        *hdr_ptr = sky_dvb_hdr_cnt;
        memcpy(&dvb_local_buffer[4], sky_dvb_hdr, 4*4*sky_dvb_hdr_cnt);

        do_it_first = 0;

        if (USE_MAX_BUFFER_SIZE>=consume_cnt)
        {
//            memset(&dvb_local_buffer[consume_cnt], 0x00, USE_MAX_BUFFER_SIZE-consume_cnt);
            memset(&dvb_local_buffer[consume_cnt], 0x00, 32);
        }
        else
        {
            printf("@@@@@@ %s: bug!!!!!! consume_cnt=%d > %d\n", __FUNCTION__, consume_cnt, USE_MAX_BUFFER_SIZE);
        }
        consume_cnt = USE_MAX_BUFFER_SIZE;//128*1024;
    }
////printf("=== get consume_cnt=%d\n", consume_cnt);
    rk = size - pos;
    consume_cnt -= rk;
    pos += rk;
#endif // end of DVBT_USING_NORMAL_METHOD
    return pos;
}


#ifdef DVBT_USING_NORMAL_METHOD
#else // end of DVBT_USING_NORMAL_METHOD
static int skydvb_read_stream(stream_t *stream)
{
#if 1
    int i;
    int pos=0, rk, fd;
    int check_got_av=0;
    int need_read_cnt=127, need_min_cnt = 4;
    struct timeval tv, tv1, tv2;
    float check_diff_time=0.0;
    int got_video_packet=0;

    int video_total_size=0, audio_total_size=0;
    int video_total_cnts=0, audio_total_cnts=0;
    int video_max_get_size=0;

    int do_workaround_cnts=0;


    struct timeval tv_5min_before, tv_10min_before;
    int read_video_in_cnts_5min, read_video_in_cnts_10min;
    int read_audio_in_cnts_5min, read_audio_in_cnts_10min;
    float check_diff_time_5min=0.0;
    float check_diff_time_10min=0.0;
    int reset_5min_one=0;
    int check_read_status=0;

#ifdef HWTS_SUPPORT_SPLIT_PACKET
	int ret_len = 0;
#endif // end of HWTS_SUPPORT_SPLIT_PACKET

    dvb_priv_t *priv  = (dvb_priv_t *) stream->priv;


//	static int consume_cnt=0;
//	static int do_it_first=1;
    printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));

    if (do_it_first == 1)
    {
        dvb_channel_t *channel = &(priv->list->channels[priv->list->current]);

//    pthread_mutex_init(&skydvb_mutex,NULL);
//    pthread_mutex_lock(&skydvb_mutex);
//    pthread_mutex_unlock(&skydvb_mutex);


        printf("==== got channel->pids[0]=%d  fd=%d  stream->fd=%d\n", channel->pids[0], priv->demux_fds[0], stream->fd);

        skydvb_video_ptr_idx=0;
        skydvb_audio_ptr_idx=0;	//index to store buffer
        skydvb_video_cur_idx=0;
        skydvb_audio_cur_idx=0; //index to index table for read from demux
        skydvb_video_get_idx=0;
        skydvb_audio_get_idx=0; //index to index table for get buffer
        skydvb_video_queue_cnts=0;
        skydvb_audio_queue_cnts=0; //total index counter
        set_skydvb_wrokaround(INIT_STATUS);


        skydvb_thread_running = 1;
        check_read_status = 0;

        gettimeofday(&tv_5min_before, 0);
        gettimeofday(&tv_10min_before, 0);
        read_video_in_cnts_5min=0;
        read_video_in_cnts_10min=0;
        read_audio_in_cnts_5min=0;
        read_audio_in_cnts_10min=0;
        check_diff_time_5min=0.0;
        check_diff_time_10min=0.0;
        reset_5min_one = 0;
    }

    fd = stream->fd;
    {
        {
            dvb_channel_t *channel = &(priv->list->channels[priv->list->current]);
            int ret, retval, fdmax;
            fd_set rfds;
            int i;

            fdmax = (priv->demux_fds[0] > priv->demux_fds[1]) ? priv->demux_fds[0] : priv->demux_fds[1];

//            while (consume_cnt < USE_MAX_BUFFER_SIZE && sky_dvb_hdr_cnt < need_read_cnt)
            gettimeofday(&tv2, 0);
            while (1)
            {
                if (skydvb_thread_running == -1)
                {
                    printf("\n !!! skydvb_thread_running = %d, need to  quit !!!\n", skydvb_thread_running);
                    break;
                }
                FD_ZERO(&rfds);

                if (video_id != -2 && priv->demux_fds[0] > 0)
                    FD_SET(priv->demux_fds[0], &rfds);
                if (audio_id != -2 && priv->demux_fds[1] > 0)
                    FD_SET(priv->demux_fds[1], &rfds);
                tv.tv_usec = 10000;
                tv.tv_sec = 0;

//                usec_sleep(10);

//                retval = select((fdmax+1), &rfds, NULL, NULL, NULL);
                retval = select((fdmax+1), &rfds, NULL, NULL, &tv);

                if (retval < 0) {
                    if (check_read_status == 0)
                    {
                        printf("Error Happens retval = 0x%08x!!\n", retval);
                        check_read_status = 1;
                    }
                    usleep(1);
                }
                else if (retval == 0) {				//Time out
//                    printf("Time Out!!\n");
//                    break;
//                        usleep(1);
                }
                else 
                {
#ifndef USE_PFM_ADDR_MODE	//PFM DATA mode

//    pthread_mutex_lock(&skydvb_mutex);
//    pthread_mutex_unlock(&skydvb_mutex);
//                    pthread_mutex_lock(&skydvb_mutex);
                  while(1)
                  {
                    if (FD_ISSET(priv->demux_fds[0], &rfds))
                    {		//vpes
                        unsigned int *lptr = &dvb_local_video_buffer[skydvb_video_ptr_idx];
                        int offset=0;
//                        ret= read(priv->demux_fds[0], &dvb_local_video_buffer[skydvb_video_ptr_idx], 1024*512);

                        //ret= read(priv->demux_fds[0], &dvb_local_video_buffer[skydvb_video_ptr_idx], 1024*1024*2);
                        ret= read(priv->demux_fds[0], &dvb_local_video_buffer[skydvb_video_ptr_idx], VIDEO_READ_LENGTH);

                        if (ret <=0)
                        {
//                            continue;
                            break;
                        }
                        if ((lptr[0]&0xffffff00) != 0x10000)
                        {
//			    printf("lptr=%d\n", *lptr);
//			    printf("==chunk: ");
                            {
                                int pp;
                                unsigned char *nptr = &dvb_local_video_buffer[skydvb_video_ptr_idx];
                                for (pp=0; pp<32; pp++)
                                {
//                                    if ((nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1)&&(nptr[pp+3]==0||nptr[pp+3]==0xe0||nptr[pp+3]==0x01||nptr[pp+3]==0x21))
									/*
									 * carlos 2010-08-18 add
									 * Support all video stream, according to the pes spec(13818-1, 2.4.3.7 Semantic definition of fields in PES packet),
									 * video type is from 0xe0~0xef
									 */
                                    //if ((nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1)&&((nptr[pp+3]&0xf0)==0xe0))
                                    if ((nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1))
                                    {
										/* 
										 * carlos add 2010-08-18
										 * Support video spec stream_id, according to the pes spec (13818-1 table 2-19 Stream_id assignments),
										 * video type is from 0xe0~0xef
										 */
										 if ((nptr[pp+3] >= 0xe0) && (nptr[pp+3] <= 0xef) )
										 {
											 //					printf("got offset=%d\n", pp);
											 offset = pp;
											 break;
										 }
                                    }
//                                    printf("%.2x ", nptr[pp]);
                                }
                                if (pp==32)
                                {
                                    printf("BUG Video offset=%d  pes size:%d  try to workaround Demuxer\n", pp, ret);
                                    set_skydvb_wrokaround(NEED_SEEK_CHANNEL);
                                    do_workaround_cnts++;
//                                    continue;
                                    break;
                                }
                            }

                        }
#ifdef HWTS_SUPPORT_SPLIT_PACKET
						/* if ret > VIDEO_READ_LENGTH, we need read remaining video data */
						if (ret > VIDEO_READ_LENGTH)
						{
							//printf("### Carlos got more size in video packet size[%d] remain size is [%d] default [%d] in [%s][%d]###", ret, ret - VIDEO_READ_LENGTH, VIDEO_READ_LENGTH, __func__, __LINE__);
							ret_len = read(priv->demux_fds[0], &dvb_local_video_buffer[skydvb_video_ptr_idx+VIDEO_READ_LENGTH], ret - VIDEO_READ_LENGTH);
							if (ret_len < 0)
								break;
						}
						if (ret <= 0)
							break;

#endif // end of HWTS_SUPPORT_SPLIT_PACKET

                        //size
                        v_hdr[skydvb_video_cur_idx][0] = ret - offset;
                        //offset
                        v_hdr[skydvb_video_cur_idx][1] = skydvb_video_ptr_idx+offset;
                        v_hdr[skydvb_video_cur_idx][2] = dvb_local_video_buffer[skydvb_video_ptr_idx+3+offset];
                        v_hdr[skydvb_video_cur_idx][3] = 0xe0;
                        video_total_size += ret;
                        video_total_cnts ++;
                        read_video_in_cnts_5min++;
                        read_video_in_cnts_10min++;
                        if (ret > video_max_get_size)
                            video_max_get_size = ret;
                        pthread_mutex_lock(&skydvb_mutex_vq);

                        if ( (skydvb_video_cur_idx + 1)%DVB_LOCAL_VIDEO_IDX_MAX == skydvb_video_get_idx)
                        {
                            printf("==== warning... skydvb_video_queue_cnts=%d consume too slow...\n", skydvb_video_queue_cnts);
                            skydvb_video_get_idx = (skydvb_video_get_idx + 1)%DVB_LOCAL_VIDEO_IDX_MAX;
                        }
                        else
                        {
                            skydvb_video_queue_cnts++;
                        }
                        skydvb_video_cur_idx = (skydvb_video_cur_idx + 1)%DVB_LOCAL_VIDEO_IDX_MAX;
                        skydvb_video_ptr_idx += ret;
                        skydvb_video_ptr_idx = ((skydvb_video_ptr_idx +3)/4)*4;

                        if ( (skydvb_video_ptr_idx + (((ret+3)/4)*4) + (384+1500)*1024) > DVB_LOCAL_VIDEO_BUF_MAX)
                        {
                            skydvb_video_ptr_idx = 0;
                        }

#if 0
                        if (g_skyts_video != 0 && skydvb_video_queue_cnts > 1)
                        {
                            demux_skyts_update_video_packet(&dvb_local_video_buffer[v_hdr[skydvb_video_get_idx][1]], ((v_hdr[skydvb_video_get_idx][0] +31)/32)*32);
                            skydvb_video_queue_cnts--;
                            skydvb_video_get_idx = (skydvb_video_get_idx+1)%DVB_LOCAL_VIDEO_IDX_MAX;
                        }
#endif
                        pthread_mutex_unlock(&skydvb_mutex_vq);
                    }
                    break;
                  }

#else	//PFM Addr mode
                    /*
                                        if (FD_ISSET(priv->demux_fds[0], &rfds))
                                        {		//vpes
                    //			unsigned int *size = (unsigned int )&dvb_local_buffer[consume_cnt];
                                            unsigned int *lptr = &dvb_local_video_buffer[skydvb_video_cnt_idx];
                                            int offset=0;
                                            ret= read(priv->demux_fds[0], &dvb_local_video_buffer[skydvb_video_cnt_idx+4], 0x1000);
                                            *lptr = 0xff008860;	//special tag
                                            ret += 4;
                                            //size
                                            sky_dvb_hdr[sky_dvb_hdr_cnt][0] = ret - offset;
                                            //offset
                                            sky_dvb_hdr[sky_dvb_hdr_cnt][1] = consume_cnt+offset;
                                            sky_dvb_hdr[sky_dvb_hdr_cnt][2] = dvb_local_buffer[consume_cnt+3+offset];
                                            sky_dvb_hdr[sky_dvb_hdr_cnt][3] = 0xe0;
                                            sky_dvb_hdr_cnt++;
                                            consume_cnt += ret;
                                            consume_cnt = ((consume_cnt +3)/4)*4;
                    //                        check_got_av |= 0x10;
                                        }
                    */
#endif
//                    pthread_mutex_lock(&skydvb_mutex);
                  while(1)
                  {
                    if (FD_ISSET(priv->demux_fds[1], &rfds))
                    {
                        unsigned int *lptr = &dvb_local_audio_buffer[skydvb_audio_ptr_idx];
                        int offset=0;
                        //ret= read(priv->demux_fds[1], &dvb_local_audio_buffer[skydvb_audio_ptr_idx], 1024*512);
                        ret= read(priv->demux_fds[1], &dvb_local_audio_buffer[skydvb_audio_ptr_idx], AUDIO_READ_LENGTH);

                        if (ret <=0)
                        {
//                            continue;
                            break;
                        }
                        if ((lptr[0]&0xffffff00) != 0x10000)
                        {
//                        			    printf("lptr=%d\n", *lptr);
                            //			    printf("==chunk: ");
                            int pp;
                            unsigned char *nptr = &dvb_local_audio_buffer[skydvb_audio_ptr_idx];

                            for (pp=0; pp<32; pp++)
                            {
                                //if ((nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1)&&(((nptr[pp+3]&0xf0)==0xc0) ||((nptr[pp+3]==0xbd) || nptr[pp+3] ==0xfd)))
                                if (nptr[pp]==0&&nptr[pp+1]==0&&nptr[pp+2]==1)
                                {
									/* 
									 * carlos add 2010-08-18
									 * Support spain audio stream_id 0xc4, 0xc6, according to the pes spec, audio type is from 0xc0~0xdf
									 * 0xfd is for reserverd data stream, some AC-3 type 
									 */
									if ((nptr[pp+3]==0xbd) ||(nptr[pp+3] == 0xfd) || ((nptr[pp+3] >= 0xc0) &&(nptr[pp+3] <= 0xdf)))
									{
										//										printf("got offset=%d\n", pp);
										offset = pp;
										break;
									}
                                }
                                printf("%.2x ", nptr[pp]);
                            }

                            if (pp!=0)
                            {
                                printf("Audio BUG offset=%d\n", pp);
                                do_workaround_cnts++;
                                set_skydvb_wrokaround(NEED_SEEK_CHANNEL);

//                                continue;
                                break;
                            }
                        }
#ifdef HWTS_SUPPORT_SPLIT_PACKET
						if (ret > AUDIO_READ_LENGTH)
						{
							//printf("### Carlos got more size in audio packet size[%d] default [%d] in [%s][%d]###\n", ret, AUDIO_READ_LENGTH, __func__, __LINE__);
							ret_len = read(priv->demux_fds[1], &dvb_local_audio_buffer[skydvb_audio_ptr_idx+AUDIO_READ_LENGTH], ret - AUDIO_READ_LENGTH);
							if (ret_len < 0)
								break;
						}
                        if (ret <=0)
                            break;
#endif // else of HWTS_SUPPORT_SPLIT_PACKET
                        //size
                        a_hdr[skydvb_audio_cur_idx][0] = ret - offset;
                        //offset
                        a_hdr[skydvb_audio_cur_idx][1] = skydvb_audio_ptr_idx+offset;
                        a_hdr[skydvb_audio_cur_idx][2] = dvb_local_audio_buffer[skydvb_audio_ptr_idx+3+offset];
                        a_hdr[skydvb_audio_cur_idx][3] = 0x03;
                        audio_total_size += ret;
                        audio_total_cnts ++;
                        read_audio_in_cnts_5min++;
                        read_audio_in_cnts_10min++;

                        pthread_mutex_lock(&skydvb_mutex_aq);

                        if ( (skydvb_audio_cur_idx + 1)%DVB_LOCAL_AUDIO_IDX_MAX == skydvb_audio_get_idx)
                        {
                            printf("==== warning... skydvb_audio_queue_cnts=%d consume too slow...\n", skydvb_audio_queue_cnts);
                            skydvb_audio_get_idx = (skydvb_audio_get_idx + 1)%DVB_LOCAL_AUDIO_IDX_MAX;
                        }
                        else
                        {
                            skydvb_audio_queue_cnts++;
                        }

                        skydvb_audio_cur_idx = (skydvb_audio_cur_idx + 1)%DVB_LOCAL_AUDIO_IDX_MAX;
                        skydvb_audio_ptr_idx += ret;
                        skydvb_audio_ptr_idx = ((skydvb_audio_ptr_idx +3)/4)*4;

                        if ( (skydvb_audio_ptr_idx + (((ret+3)/4)*4) + 16*1024) > DVB_LOCAL_AUDIO_BUF_MAX)
                        {
                            skydvb_audio_ptr_idx = 0;
                        }

#if 0
                        if (g_skyts_audio != 0 && skydvb_audio_queue_cnts > 1)
                        {
                            demux_skyts_update_audio_packet(&dvb_local_audio_buffer[a_hdr[skydvb_audio_get_idx][1]], ((a_hdr[skydvb_audio_get_idx][0] +31)/32)*32 );
                            skydvb_audio_queue_cnts--;
                            skydvb_audio_get_idx = (skydvb_audio_get_idx+1)%DVB_LOCAL_AUDIO_IDX_MAX;
                        }
#endif
                        pthread_mutex_unlock(&skydvb_mutex_aq);

                    }
//                    pthread_mutex_unlock(&skydvb_mutex);
                    break;
                  }

                }

#if 1
	    if (!quiet)
	    {
                gettimeofday(&tv1, 0);
                check_diff_time = (float)(tv1.tv_sec - tv2.tv_sec);
                if (tv1.tv_usec > tv2.tv_usec)
                {
                    check_diff_time += (float)(tv1.tv_usec - tv2.tv_usec)/1000000;
                }
                else
                {
                    check_diff_time -= 1.0;
                    check_diff_time += (float)(1000000 - tv2.tv_usec + tv1.tv_usec)/1000000;
                }
//5min
                check_diff_time_5min = (float)(tv1.tv_sec - tv_5min_before.tv_sec);
                if (tv1.tv_usec > tv_5min_before.tv_usec)
                {
                    check_diff_time_5min += (float)(tv1.tv_usec - tv_5min_before.tv_usec)/1000000;
                }
                else
                {
                    check_diff_time_5min -= 1.0;
                    check_diff_time_5min += (float)(1000000 - tv_5min_before.tv_usec + tv1.tv_usec)/1000000;
                }


                if (check_diff_time > 10)
                {
                    float av_total_size = (float)(video_total_size + audio_total_size);
                    printf("== in thread  v in  %f pkt/s  a in  %f pkts/s\n", (float)read_video_in_cnts_5min/check_diff_time_5min, (float)read_audio_in_cnts_5min/check_diff_time_5min);
                    printf("== in thread  v out %f pkt/s  a out %f pkts/s\n", (float)decode_video_cnts_5min/check_diff_time_5min, (float)decode_audio_cnts_5min/check_diff_time_5min);
                    if (video_total_cnts < 1800)
                    {
                        check_diff_time_5min=0.0;
                        gettimeofday(&tv_5min_before, 0);
                        read_video_in_cnts_5min=0;
                        read_audio_in_cnts_5min=0;
                        decode_video_cnts_5min=0;
                        decode_audio_cnts_5min=0;
                        printf("reset 5min counter...\n");
                    }
                    /*
                    printf("=in thread vq:%d aq:%d size v:%dKB(%d) a:%dKB(%d) av:%dKB ave:%f (KB/sec) vmax:%dKB workaround:%d\n",
                            skydvb_video_queue_cnts, skydvb_audio_queue_cnts,
                            (video_total_size/1024), video_total_cnts,
                            (audio_total_size/1024), audio_total_cnts,
                            (int)(av_total_size/1024), (av_total_size/1024)/check_diff_time,
                            video_max_get_size/1024, do_workaround_cnts);
                    */
                    tv2.tv_sec = tv1.tv_sec;
                    tv2.tv_usec = tv1.tv_usec;
                    video_total_size = 0;
                    audio_total_size = 0;
//video_total_cnts = 0;
//audio_total_cnts = 0;

                }
	    }//if !quiet
#endif

            }

        }
    }
    for (i = priv->demux_fds_cnt-1; i >= 0; i--)
    {
        priv->demux_fds_cnt--;
        mp_msg(MSGT_DEMUX, MSGL_V, "DVBIN_CLOSE, close(%d), fd=%d, COUNT=%d\n", i, priv->demux_fds[i], priv->demux_fds_cnt);
        printf("================== in thread DVBIN_CLOSE i=%d fd=%d\n", i, priv->demux_fds[i]);
        ioctl(priv->demux_fds[i], DMX_STOP, NULL);
        close(priv->demux_fds[i]);
    }
    if (dvb_local_video_buffer)
    {
        free(dvb_local_video_buffer);
        free(dvb_local_audio_buffer);
        dvb_local_video_buffer = 0;
        dvb_local_audio_buffer = 0;
    }
    skydvb_thread_running = -2;

    return 0;
#endif
}
#endif // end of DVBT_USING_NORMAL_METHOD

#ifdef DVBT_USING_NORMAL_METHOD
int dvb_set_channel(stream_t *stream, int card, int n)
{
    dvb_channels_list *new_list;
    dvb_channel_t *channel;
    dvb_priv_t *priv = stream->priv;
    char buf[4096];
    dvb_config_t *conf = (dvb_config_t *) priv->config;
    int devno;
    int i;
    extern int skyfd;
    printf("===== in dvb_set_channel() skyfd=%d\n", skyfd);

    if ((card < 0) || (card > conf->count))
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_set_channel: INVALID CARD NUMBER: %d vs %d, abort\n", card, conf->count);
        return 0;
    }

    devno = conf->cards[card].devno;
    new_list = conf->cards[card].list;
    if ((n > new_list->NUM_CHANNELS) || (n < 0))
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_set_channel: INVALID CHANNEL NUMBER: %d, for card %d, abort\n", n, card);
        return 0;
    }
//	printf("============= will change to %d channel ==========\n", n);
    channel = &(new_list->channels[n]);

    if (priv->is_on)	//the fds are already open and we have to stop the demuxers
    {
#if 0 // now, we don't used pfm mode , mark by carlos 20100903
        if (fd_pfm > 0)
        {
            if (ioctl(fd_pfm, DMX_PFM_STOP) < 0) {
                printf("    0:DMX_PFM_STOP ioctl failed\n");
            }
            close(fd_pfm);
            dvb_pfm_is_start = 0;
            fd_pfm = -1;
        }
#endif 
        priv->retry = 0;
        if (priv->dvr_fd > 0)
        {
            close(priv->dvr_fd);
			priv->dvr_fd = -1;
        }

        dvbin_close(stream); // we will stop thread in dvbin_close 

        if (! dvb_open_devices(priv, devno, channel->pids_cnt, channel->pids))
        {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB_SET_CHANNEL, COULDN'T OPEN DEVICES OF CARD: %d, EXIT[%s][%d]\n", card, __func__, __LINE__);
            return 0;
        }
    }
    else
    {
        if (! dvb_open_devices(priv, devno, channel->pids_cnt, channel->pids))
        {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB_SET_CHANNEL, COULDN'T OPEN DEVICES OF CARD: %d, EXIT[%s][%d]\n", card, __func__, __LINE__);
            return 0;
        }
    }

    dvb_config->priv = priv;
    priv->card = card;
    priv->list = new_list;
    priv->retry = 5;
    new_list->current = n;
    stream->fd = priv->dvr_fd;
    mp_msg(MSGT_DEMUX, MSGL_V, "DVB_SET_CHANNEL: new channel name=%s, card: %d, channel %d\n", channel->name, card, n);
    printf("DVB_SET_CHANNEL: new channel name=%s, card: %d, channel %d\n", channel->name, card, n);
    printf("DVB_SET_CHANNEL: new channel name=%s, card: %d, channel %d vid %d aid %d sid %d \n", channel->name, card, n, channel->pids[0], channel->pids[1], channel->pids[2]);
    stream->eof=1;
    //printf("===== bf stream_reset...\n");
    stream_reset(stream);

#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	if (!stop_tuner)
	{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
		if (priv->last_freq != channel->freq)
		{
			//printf("===== skip dvb_tune (tune_it) now...\n");
			printf("priv->last_freq is [%d] channel->freq is [%d] #####\n", priv->last_freq, channel->freq);

			/* TODO, need check freq, if the tuner option are the same, don't need tune it again */        
			if (! dvb_tune(priv, channel->freq, channel->pol, channel->srate, channel->diseqc, channel->tone,
						channel->inv, channel->mod, channel->gi, channel->trans, channel->bw, channel->cr, channel->cr_lp, channel->hier, priv->timeout))
				return 0;
		}   
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	}
	else
		printf("@@@@@ In [%s]received stop dvb_tune @@@@\n", __func__);
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
	priv->last_freq = channel->freq;
    priv->is_on = 1;
#if 0 // now, we don't used pfm mode , mark by carlos 20100903
    {
        struct dmx_pfm_params st_params;
        if (fd_pfm > 0)
        {
            ioctl(fd_pfm, DMX_PFM_STOP);
            close(fd_pfm);
            dvb_pfm_is_start = 0;
            fd_pfm = -1;
        }
    }
#endif 
    return 1;
}
#else // else of DVBT_USING_NORMAL_METHOD
int dvb_set_channel(stream_t *stream, int card, int n)
{
    dvb_channels_list *new_list;
    dvb_channel_t *channel;
    dvb_priv_t *priv = stream->priv;
    char buf[4096];
    dvb_config_t *conf = (dvb_config_t *) priv->config;
    int devno;
    int i;
    extern int skyfd;
    printf("===== in dvb_set_channel() skyfd=%d\n", skyfd);
    /*
    	if (skyfd == -1)
    	{
    		init_sky();
    	}
    */
    if ((card < 0) || (card > conf->count))
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_set_channel: INVALID CARD NUMBER: %d vs %d, abort\n", card, conf->count);
        return 0;
    }

    devno = conf->cards[card].devno;
    new_list = conf->cards[card].list;
    if ((n > new_list->NUM_CHANNELS) || (n < 0))
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_set_channel: INVALID CHANNEL NUMBER: %d, for card %d, abort\n", n, card);
        return 0;
    }
    channel = &(new_list->channels[n]);

    if (priv->is_on)	//the fds are already open and we have to stop the demuxers
    {
        extern int do_skyts_fill_first;
        do_skyts_fill_first = 1;
        if (fd_pfm > 0)
        {
            if (ioctl(fd_pfm, DMX_PFM_STOP) < 0) {
                printf("    0:DMX_PFM_STOP ioctl failed\n");
            }
            close(fd_pfm);
            dvb_pfm_is_start = 0;
            fd_pfm = -1;
        }


        //stop thread
        if (skydvb_thread_running == 1)
        {
            skydvb_thread_running = -1;
            printf("\n=========== try to join prev thread and then kill it ====[%s][%d]\n", __func__, __LINE__);
//    pthread_join(&skydvb_read_thread, NULL);
            pthread_cancel(&skydvb_read_thread);
//            pthread_cancel(skydvb_read_thread);
//    pthread_join(&skydvb_read_thread, NULL);
//    while (pthread_kill(&skydvb_read_thread, 0)!= ESRCH)
            while(skydvb_thread_running != -2)
            {
                usleep(1);
            }
            pthread_join(skydvb_read_thread, 0);
            printf("\n=========== thread clear ======\n");
            skydvb_thread_running = 0;
        }


        for (i = 0; i < priv->demux_fds_cnt; i++)
        {
            dvb_demux_stop(priv->demux_fds[i]);
            close(priv->demux_fds[i]);
        }

        priv->retry = 0;
        consume_cnt = 0;
        do_it_first = 1;
        if (priv->dvr_fd > 0)
        {
            close(priv->dvr_fd);
        }

        
#if 0
        //+SkyViia_Vincent02102010
        if (skydroid)
            priv->dvr_fd = open("/dev/dvb0.dvr0", O_RDONLY);
        else
            //SkyViia_Vincent02102010+
            priv->dvr_fd = open("/dev/dvb/adapter0/dvr0", O_RDONLY);
#else
        priv->dvr_fd = -1;
#endif

#if 0
        if (priv->card != card)
        {
            dvbin_close(stream);
            if (! dvb_open_devices(priv, devno, channel->pids_cnt, channel->pids))
            {
                mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB_SET_CHANNEL, COULDN'T OPEN DEVICES OF CARD: %d, EXIT\n", card);
                return 0;
            }
        }
        else	//close all demux_fds with pos > pids required for the new channel or open other demux_fds if we have too few
        {
            if (! dvb_fix_demuxes(priv, channel->pids_cnt, channel->pids))
                return 0;
        }
#else
        dvbin_close(stream);
        if (! dvb_open_devices(priv, devno, channel->pids_cnt, channel->pids))
        {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB_SET_CHANNEL, COULDN'T OPEN DEVICES OF CARD: %d, EXIT\n", card);
            return 0;
        }

#endif


    }
    else
    {
        if (! dvb_open_devices(priv, devno, channel->pids_cnt, channel->pids))
        {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB_SET_CHANNEL2, COULDN'T OPEN DEVICES OF CARD: %d, EXIT\n", card);
            return 0;
        }
    }

    dvb_config->priv = priv;
    priv->card = card;
    priv->list = new_list;
    priv->retry = 5;
    new_list->current = n;
    stream->fd = priv->dvr_fd;
//	stream->fd = priv->demux_fds[0];
    mp_msg(MSGT_DEMUX, MSGL_V, "DVB_SET_CHANNEL: new channel name=%s, card: %d, channel %d\n", channel->name, card, n);
    printf("\n=====\n");
    stream->eof=1;
    printf("===== bf stream_reset...\n");
    stream_reset(stream);

    do_it_first = 1;
    dvb_read_method = 0;
    printf("===== skip dvb_tune (tune_it) now...\n");

#if 0
    if (channel->freq != priv->last_freq)
#endif
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	if (!stop_tuner)
	{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
        
                if (! dvb_tune(priv, channel->freq, channel->pol, channel->srate, channel->diseqc, channel->tone,
                               channel->inv, channel->mod, channel->gi, channel->trans, channel->bw, channel->cr, channel->cr_lp, channel->hier, priv->timeout))
                    return 0;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
        
        priv->last_freq = channel->freq;
    priv->is_on = 1;
//====
// setup PFM

    {
//		int fd_pfm;
        struct dmx_pfm_params st_params;

//		fd_pfm = open("/dev/dvb/adapter0/pfm0", (O_RDONLY));
        if (fd_pfm > 0)
        {
            ioctl(fd_pfm, DMX_PFM_STOP);
            close(fd_pfm);
            dvb_pfm_is_start = 0;
            fd_pfm = -1;
        }
#if 0
        printf("---- fd_pfm=%d\n", fd_pfm);
        memset(&st_params, 0, sizeof(struct dmx_pfm_params));
        st_params.input = PFM_IN_TSDEMUX;

//#ifdef PFM_ADDR_MODE
//	        st_params.out_type = PFM_FRAME_PTR_SIZE;
//#else
        st_params.out_type = PFM_FRAME_DATA;
//#endif
        st_params.pes_stream_id = 0xe0;
        st_params.video_type = DMX_PFM_MPEG2_MPEG1;
        //st_params.video_type = DMX_PFM_H264;
        /*
        	        if (ioctl(fd_pfm, DMX_PFM_START, &st_params) < 0) {
                	    printf("	0:DMX_PFM_START ioctl failed\n");
        	        }
        */
#endif
    }
//==== end of PFM setting
//    printf("===== PFM setting ok\n");
//    printf("==== channel->pids_cnt=%d\n", channel->pids_cnt);
    return 1;
}
#endif // end of DVBT_USING_NORMAL_METHOD


int dvb_step_channel(stream_t *stream, int dir)
{
    int new_current;
    dvb_priv_t *priv = stream->priv;
    dvb_channels_list *list;

    mp_msg(MSGT_DEMUX, MSGL_V, "DVB_STEP_CHANNEL dir %d\n", dir);
printf("priv=0x%x\n", (unsigned int)priv);
    if ((unsigned int)priv == NULL)
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_step_channel: NULL priv_ptr, quit\n");
        return 0;
    }

    list = priv->list;
    if (list == NULL)
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "dvb_step_channel: NULL list_ptr, quit\n");
        return 0;
    }

//20100503 Robert
    if (dir == 0)
    {
        new_current = (list->NUM_CHANNELS + list->current ) % list->NUM_CHANNELS;
    }
    else
    {
        new_current = (list->NUM_CHANNELS + list->current + (dir == DVB_CHANNEL_HIGHER ? 1 : -1)) % list->NUM_CHANNELS;
    }

    return dvb_set_channel(stream, priv->card, new_current);
}


#ifdef DVBT_USING_NORMAL_METHOD
static void dvbin_close(stream_t *stream)
{
    int i;
    dvb_priv_t *priv  = (dvb_priv_t *) stream->priv;

    if (fd_pfm != -1) {
        if (ioctl(fd_pfm, DMX_PFM_STOP) < 0) {
            printf("	0:DMX_PFM_STOP ioctl failed\n");
        }
        close(fd_pfm);
        dvb_pfm_is_start = 0;
        fd_pfm = -1;
    }

    for (i = priv->demux_fds_cnt-1; i >= 0; i--)
    {
        priv->demux_fds_cnt--;
        mp_msg(MSGT_DEMUX, MSGL_V, "DVBIN_CLOSE, close(%d), fd=%d, COUNT=%d\n", i, priv->demux_fds[i], priv->demux_fds_cnt);
        dvb_debug_printf("================== DVBIN_CLOSE i=%d fd=%d\n", i, priv->demux_fds[i]);
        printf("================== DVBIN_CLOSE i=%d fd=%d\n", i, priv->demux_fds[i]);
        ioctl(priv->demux_fds[i], DMX_STOP, NULL);
        close(priv->demux_fds[i]);
    }

    if (priv->dvr_fd > 0)
    {
        close(priv->dvr_fd);
    }

    if (priv->fe_fd > 0)
    {
        close(priv->fe_fd);
    }
#ifdef HAVE_DVB
    if (priv->sec_fd > 0)
    {
        close(priv->sec_fd);
    }
#endif

    priv->is_on = 0;
    dvb_config->priv = NULL;
#if 1 //carlos add for change default conf, 20100804
	if (default_conf_file)
	{
		free(default_conf_file);
		default_conf_file = NULL;
	}
#endif 
}

#else // else of DVBT_USING_NORMAL_METHOD

static void dvbin_close(stream_t *stream)
{
    int i;
    dvb_priv_t *priv  = (dvb_priv_t *) stream->priv;

    //stop thread
    if (skydvb_thread_running == 1)
    {
        skydvb_thread_running = -1;
        printf("\n=========== try to join prev thread and then kill it [%s][%d]====\n", __func__, __LINE__);
        pthread_cancel(&skydvb_read_thread);
//        pthread_cancel(skydvb_read_thread);
        while(skydvb_thread_running != -2)
        {
            usleep(1);
        }
        printf("\n=========== thread clear ======\n");
		pthread_join(skydvb_read_thread, 0);
        skydvb_thread_running = 0;
    }

    if (fd_pfm != -1) {
        if (ioctl(fd_pfm, DMX_PFM_STOP) < 0) {
            printf("	0:DMX_PFM_STOP ioctl failed\n");
        }
        close(fd_pfm);
        dvb_pfm_is_start = 0;
        fd_pfm = -1;
    }

    for (i = priv->demux_fds_cnt-1; i >= 0; i--)
    {
        priv->demux_fds_cnt--;
        mp_msg(MSGT_DEMUX, MSGL_V, "DVBIN_CLOSE, close(%d), fd=%d, COUNT=%d\n", i, priv->demux_fds[i], priv->demux_fds_cnt);
        printf("================== DVBIN_CLOSE i=%d fd=%d\n", i, priv->demux_fds[i]);
        ioctl(priv->demux_fds[i], DMX_STOP, NULL);
        close(priv->demux_fds[i]);
    }
    if (priv->dvr_fd > 0)
    {
        close(priv->dvr_fd);
    }

    if (priv->fe_fd > 0)
    {
        close(priv->fe_fd);
    }
#ifdef HAVE_DVB
    if (priv->sec_fd > 0)
    {
        close(priv->sec_fd);
    }
#endif

    priv->is_on = 0;
    dvb_config->priv = NULL;
#if 1 //carlos add for change default conf, 20100804
	if (default_conf_file)
	{
		free(default_conf_file);
		default_conf_file = NULL;
	}
#endif 
}
#endif // end of DVBT_USING_NORMAL_METHOD

static int dvb_streaming_start(stream_t *stream, struct stream_priv_s *opts, int tuner_type, char *progname)
{
    int i;
    dvb_channel_t *channel = NULL;
    dvb_priv_t *priv = stream->priv;

	skydvb_printf("@@@ In [%s][%d]  progname[%s] setdvb_by_pid [%d]@@@\n", __func__, __LINE__, progname, setdvb_by_pid);
    mp_msg(MSGT_DEMUX, MSGL_V, "\r\ndvb_streaming_start(PROG: %s, CARD: %d, VID: %d, AID: %d, TYPE: %s, FILE: %s)\r\n",
           opts->prog, opts->card, opts->vid, opts->aid,  opts->type, opts->file);

    priv->is_on = 0;

#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	if (setdvb_by_pid)
	{
		channel = &(priv->list->channels[0]);
		priv->list->current = 0;
	}
	else
	{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID	
		i = 0;
		while ((channel == NULL) && i < priv->list->NUM_CHANNELS)
		{
			if (! strcmp(priv->list->channels[i].name, progname))
				channel = &(priv->list->channels[i]);

			i++;
		}

		if (channel != NULL)
		{
			priv->list->current = i-1;
			mp_msg(MSGT_DEMUX, MSGL_V, "PROGRAM NUMBER %d: name=%s, freq=%u\n", i-1, channel->name, channel->freq);
		}
		else
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "\n\nDVBIN: no such channel \"%s\"\n\n", progname);
			return 0;
		}
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID

    mp_msg(MSGT_DEMUX, MSGL_V,"----------   bf dvb_set_channel\n");
    if (!dvb_set_channel(stream, priv->card, priv->list->current))
    {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "ERROR, COULDN'T SET CHANNEL  %i: ", priv->list->current);
        dvbin_close(stream);
        return 0;
    }

    mp_msg(MSGT_DEMUX, MSGL_V,  "SUCCESSFUL EXIT from dvb_streaming_start\n");

    return 1;
}

static int dvb_open(stream_t *stream, int mode, void *opts, int *file_format)
{
    // I don't force  the file format bacause, although it's almost always TS,
    // there are some providers that stream an IP multicast with M$ Mpeg4 inside
    struct stream_priv_s* p = (struct stream_priv_s*)opts;
    dvb_priv_t *priv;
    char *progname;
    int tuner_type = 0, i;


    if (mode != STREAM_READ)
        return STREAM_UNSUPPORTED;

    stream->priv = calloc(1, sizeof(dvb_priv_t));
    if (stream->priv ==  NULL)
        return STREAM_ERROR;

    priv = (dvb_priv_t *)stream->priv;
    priv->stream = stream;
    dvb_config = dvb_get_config(stream->url);
    if (dvb_config == NULL)
    {
        free(priv);
        mp_msg(MSGT_DEMUX, MSGL_ERR, "DVB CONFIGURATION IS EMPTY, exit\n");
        return STREAM_ERROR;
    }
    dvb_config->priv = priv;
    priv->config = dvb_config;

    priv->card = -1;
    for (i=0; i<priv->config->count; i++)
    {
        if (priv->config->cards[i].devno+1 == p->card)
        {
            priv->card = i;
            break;
        }
    }

    if (priv->card == -1)
    {
        free(priv);
        mp_msg(MSGT_DEMUX, MSGL_ERR, "NO CONFIGURATION FOUND FOR CARD N. %d, exit\n", p->card);
        return STREAM_ERROR;
    }
    priv->timeout = p->timeout;

    tuner_type = priv->config->cards[priv->card].type;

    if (tuner_type == 0)
    {
        free(priv);
        mp_msg(MSGT_DEMUX, MSGL_V, "OPEN_DVB: UNKNOWN OR UNDETECTABLE TUNER TYPE, EXIT\n");
		printf("OPEN_DVB: UNKNOWN OR UNDETECTABLE TUNER TYPE, EXIT\n");
        return STREAM_ERROR;
    }


    priv->tuner_type = tuner_type;

    mp_msg(MSGT_DEMUX, MSGL_V, "OPEN_DVB: prog=%s, card=%d, type=%d, vid=%d, aid=%d\n",
           p->prog, priv->card+1, priv->tuner_type, p->vid, p->aid);

    priv->list = priv->config->cards[priv->card].list;


    if ((! strcmp(p->prog, "")) && (priv->list != NULL))
        progname = priv->list->channels[0].name;
    else
        progname = p->prog;


    if (! dvb_streaming_start(stream, p, tuner_type, progname))
    {
        free(stream->priv);
        stream->priv = NULL;
        return STREAM_ERROR;
    }

    stream->type = STREAMTYPE_DVB;
    stream->fill_buffer = dvb_streaming_read;
    stream->close = dvbin_close;
    m_struct_free(&stream_opts, opts);

    *file_format = DEMUXER_TYPE_SKYMPEG_TS;

    return STREAM_OK;
}


#define MAX_CARDS 4
//dvb_config_t *dvb_get_config(void)
dvb_config_t *dvb_get_config(char *file_name)
{
    int i, fd, type, size;
    char filename[30], *conf_file, *name;
    dvb_channels_list *list;
    dvb_card_config_t *cards = NULL, *tmp;
    dvb_config_t *conf = NULL;

#if 1 //carlos add for change default conf, 20100804
    if (dvb_config != NULL)
	{
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
		skydvb_printf("#### In [%s][%d] setdvb_by_pid [%d] conf->cards [%p] ####\n", __func__, __LINE__, setdvb_by_pid, dvb_config->cards);
		if (setdvb_by_pid && dvb_config->cards && dvb_config->cards[0].list)
		{
			free(dvb_config->cards[0].list);
			dvb_config->cards[0].list = NULL;
			dvb_config->cards[0].list = dvb_get_channels(file_name, type);
		}
		else
		{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
			if (!change_conf)
				return dvb_config;
			else
			{
				change_conf = 0;
				free(dvb_config);
				dvb_config = NULL;
			}
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
		}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
	}
#else
    if (dvb_config != NULL)
        return dvb_config;
#endif

    conf = malloc(sizeof(dvb_config_t));
    if (conf == NULL)
        return NULL;

    conf->priv = NULL;
    conf->count = 0;
    conf->cards = NULL;
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	if (setdvb_by_pid)
	{
		conf->cards = malloc(sizeof(dvb_card_config_t));
		conf->cards[conf->count].list = dvb_get_channels(file_name, type);
		conf->cards[conf->count].devno = 0;
		conf->cards[conf->count].type = TUNER_TER;
		name = malloc(20);
		if (name==NULL)
			fprintf(stderr, "DVB_CONFIG, can't realloc 20 bytes, skipping [%s][%d]\n", __func__, __LINE__);
		else
		{
			snprintf(name, 20, "Special DVD PID ");
			conf->cards[conf->count].name = name;
		}
		conf->count++;
	}
	else
	{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
		for (i=0; i<MAX_CARDS; i++)
		{
			//+SkyViia_Vincent02102010
			if (skydroid)
				snprintf(filename, sizeof(filename), "/dev/dvb0.frontend0");
			else
				//SkyViia_Vincent02102010+
				snprintf(filename, sizeof(filename), "/dev/dvb/adapter%d/frontend0", i);

			fd = open(filename, O_RDONLY|O_NONBLOCK);
			if (fd < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "DVB_CONFIG, can't open device %s, skipping\n", filename);
				continue;
			}

			type = dvb_get_tuner_type(fd);
			close(fd);
			if (type != TUNER_SAT && type != TUNER_TER && type != TUNER_CBL && type != TUNER_ATSC)
			{
				mp_msg(MSGT_DEMUX, MSGL_V, "DVB_CONFIG, can't detect tuner type of card %d, skipping\n", i);
				continue;
			}

			switch (type)
			{
				case TUNER_TER:
					conf_file = get_path("channels.conf.ter");
					break;
				case TUNER_CBL:
					conf_file = get_path("channels.conf.cbl");
					break;
				case TUNER_SAT:
					conf_file = get_path("channels.conf.sat");
					break;
				case TUNER_ATSC:
					conf_file = get_path("channels.conf.atsc");
					break;
			}
#if 1 // carlos add for change default conf, 20100804
			if (default_conf_file)
			{
				if ((access(default_conf_file, F_OK | R_OK) != 0))
					conf_file = get_path("channels.conf");
				else
				{
					conf_file = strdup(default_conf_file);
					change_conf = 0;
				}
			}
			else
			{
				if ((access(conf_file, F_OK | R_OK) != 0))
					conf_file = get_path("channels.conf");
			}
			printf("==== DVB conf is [%s] ====\n", conf_file);
#else
			if ((access(conf_file, F_OK | R_OK) != 0))
				conf_file = get_path("channels.conf");
#endif 
			list = dvb_get_channels(conf_file, type);
			if (list == NULL)
				continue;

			size = sizeof(dvb_card_config_t) * (conf->count + 1);
			tmp = realloc(conf->cards, size);

			if (tmp == NULL)
			{
				fprintf(stderr, "DVB_CONFIG, can't realloc %d bytes, skipping\n", size);
				continue;
			}
			cards = tmp;

			name = malloc(20);
			if (name==NULL)
			{
				fprintf(stderr, "DVB_CONFIG, can't realloc 20 bytes, skipping\n");
				continue;
			}

			conf->cards = cards;
			conf->cards[conf->count].devno = i;
			conf->cards[conf->count].list = list;
			conf->cards[conf->count].type = type;
			snprintf(name, 20, "DVB-%c card n. %d", type==TUNER_TER ? 'T' : (type==TUNER_CBL ? 'C' : 'S'), conf->count+1);
			conf->cards[conf->count].name = name;
			conf->count++;
		}

		if (conf->count == 0)
		{
			free(conf);
			conf = NULL;
		}
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
	}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID

    dvb_config = conf;
    return conf;
}

stream_info_t stream_info_dvb = {
    "Dvb Input",
    "dvbin",
    "Nico",
    "based on the code from ??? (probably Arpi)",
    dvb_open,
    { "dvb", NULL },
    &stream_opts,
    1 				// Urls are an option string
};

#if 1 // carlos add for change default conf, 20100804
void reload_dvb_conf(char *dvb_conf_filename)
{
	int len = 0;
	if (dvb_conf_filename)
	{
		if (default_conf_file == NULL)
			default_conf_file = calloc(1, 512);
		else
			memset(default_conf_file, 0, 512);
		len = strlen(dvb_conf_filename) > 512 ? 512 : strlen(dvb_conf_filename) ;
		strncpy(default_conf_file, dvb_conf_filename, len);
		change_conf = 1;
	}
}
#endif 

#endif /* end of QT_SUPPORT_DVBT */

