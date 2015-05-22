/*
 * robertluo@skyviia.com.tw, February 2010
 *
 * Copyright (C) 2010 Skyviia
 *
 * The license covers the portions of this file regarding Skyviia additions.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <sys/syscall.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "parse_es.h"
#include "stheader.h"
#include "sub_cc.h"
#include "libavutil/avstring.h"
#include "libavutil/intreadwrite.h"
#include "stream/skydvbin.h"
#include "libmpcodecs/sky_vdec_2.h"
#include "demux_ts.h"
/* TODO, need move the below define to demux_ts.h */
#define DMX_PAUSE_FILTER        _IO('o', 59)
#define DMX_RESUME_FILTER       _IO('o', 60)

#define STEPACCURACY 12
unsigned	long phad = (1<<STEPACCURACY);
short presample[2] = {0, 0};
const static unsigned long mode_step[] = {4100, 4096, 4092};
#ifdef QT_SUPPORT_DVBT
#include <pthread.h>
static pthread_mutex_t dvb_st_mtx = PTHREAD_MUTEX_INITIALIZER; 
//#define USE_APES_BACKUP 1
//#define USE_READ_TIME_BENCH 1
static int ts_parse_pat(unsigned char *ts_buf, int len, int p_pid);
static int ts_parse_pmt(unsigned char *ts_buf, int len, int a_pid, int v_pid, int *a_type, int *v_type);
/* p_pid is program_pid, try_pid is which one we need found out the type pid */
static int get_video_audio_id(int v_pid, int a_pid, int p_pid, int *v_type, int *a_type, int fd);
unsigned int check_format(int type, int stream_type);

#ifdef AUTO_CHECK // add by carlos 2010-07-22
#include <stropts.h>
#if 0 // move below define to demux_ts.h
#define VIDEO_MPEG1	0x10000001
#define VIDEO_MPEG2	0x10000002
#define VIDEO_MPEG4	0x10000004
#define VIDEO_H264	0x10000005
#define VIDEO_VC1	mmioFOURCC('W', 'V', 'C', '1')
#define AUDIO_DVB_MP2	0x00000050
#define AUDIO_DVB_AAC	0x0000706d
#define	AUDIO_DVB_A52   0x00002000
#define AUDIO_DVB_DTS	0x00002001
#endif  // end if 0
#define BUF_LENGTH	4096

#endif // end of AUTO_CHECK

#ifdef DVBT_USING_NORMAL_METHOD
int skydvb_thread_running = 0;
int exit_skydvb_thread = 0; // extern to command 
static pthread_t skydvb_read_thread = 0;
static int skyviia_read_stream(demuxer_t *demux);
#else // else of DVBT_USING_NORMAL_METHOD
extern int dvb_is_read_video;
#endif // end of DVBT_USING_NORMAL_METHOD
int skydvb_need_workaround = INIT_STATUS;

extern int video_id, audio_id;

extern void skip_audio_frame( sh_audio_t *sh_audio );
extern int sub_justify;
extern int dvb_pfm_is_start;
#ifdef USE_PFM_ADDR_MODE
int dmx_skyts_fill_data_type = 0;
#else
int dmx_skyts_fill_data_type = 1;
#endif
int do_skyts_fill_first = 1;
static unsigned char    *chunk=0;

demux_stream_t *g_skyts_video=0, *g_skyts_audio=0;
demuxer_t *g_skyts_demux=0;


#if USE_APES_BACKUP
#define MAX_APES_LEN 320	//1280
int apes_cur_idx=0;
int apes_backup_idx_size[MAX_APES_LEN];
unsigned char *apes_backup_buffer[MAX_APES_LEN];

void skyts_dump_to_file(void)
{
    FILE *fn;
    char buf[60], buf2[60];
    int pp, last_apes_cur_idx;

    printf("\n===== try to skyts_dump_to_file() =====\n");
    last_apes_cur_idx = (apes_cur_idx+1)%MAX_APES_LEN;
    printf("   apes_cur_idx=%d   last_apes_cur_idx=%d\n", apes_cur_idx, last_apes_cur_idx);

    sprintf(buf, "/tmp/apes.%d", getpid());
    printf("==== dump file :%s\n", buf);
    fn = fopen(buf, "w");
//==== do apes backup for debugging
    for (pp=0; pp<MAX_APES_LEN; pp++)
    {
        if (apes_backup_buffer[last_apes_cur_idx])
        {
            fwrite(apes_backup_buffer[last_apes_cur_idx], apes_backup_idx_size[last_apes_cur_idx], 1, fn);
            printf(" == write idx :%d  size:%d\n", last_apes_cur_idx, apes_backup_idx_size[last_apes_cur_idx]);
            {
                FILE *fn2;
                sprintf(buf2, "%s.%d", buf, pp);
                fn2 = fopen(buf2, "w");
                fwrite(apes_backup_buffer[last_apes_cur_idx], apes_backup_idx_size[last_apes_cur_idx], 1, fn2);
                fclose(fn2);
            }
            free(apes_backup_buffer[last_apes_cur_idx]);
            apes_backup_buffer[last_apes_cur_idx] = 0;
        }
        last_apes_cur_idx = (last_apes_cur_idx + 1)% MAX_APES_LEN;
    }
    printf("\n ---- dump ok! ----\n");
}
#else
void skyts_dump_to_file(void)
{
}
#endif

#define PACKSIZE (USE_MAX_BUFFER_SIZE)
#define MAX_AUDIO_BUFFER ( 300 * 1024 )

typedef struct
{
    int             whichChunk;

    unsigned char   lastAudio[ MAX_AUDIO_BUFFER ];
    int             lastAudioEnd;

    int64_t        lastAudioPTS;
    int64_t        lastVideoPTS;

    off_t           size;
    int             readHeader;

} SkytsInfo;


void sample_resize_init(void)
{
   phad = (1<<STEPACCURACY);
   presample[0] = 0;
   presample[1] = 0;
}


int sample_resize_1ch(short *inbuf, short *outbuf, int in_len, int mode)
{
   unsigned	long step = mode_step[mode+1];
   int len = 0;
   unsigned	long mask = (1<<STEPACCURACY)-1;
   unsigned	long frac, tmp;
   unsigned	long end = (in_len)<<STEPACCURACY;

   while(phad < (1<<STEPACCURACY)){
      frac = phad & mask;
      *outbuf++ = (short)((((inbuf[0] - presample[0])*(int)(frac))>>STEPACCURACY) + presample[0]); 
      len++;
      phad += step;
   }
   while(phad < end){
      tmp = phad>>STEPACCURACY;
      frac = phad & mask;
      *outbuf++  = (short)((((inbuf[tmp] - inbuf[(tmp-1)])*(int)(frac))>>STEPACCURACY) + inbuf[(tmp-1)]); 
      len++;
      phad+=step;
   }
   presample[0] = inbuf[in_len-1];
   phad &= mask;
   return len;
}


// ===========================================================================

static int64_t get_skyts_pts( unsigned char *buf )
{
    int64_t pts;

    if (buf[7] & 0x80)
    { 	/* pts available */
        pts  = (int64_t)(buf[9] & 0x0E) << 29;
        pts |= buf[10] << 22 ;
        pts |= (buf[11] & 0xFE) << 14;
        pts |=  buf[12] <<  7 ;
        pts |= (buf[13] & 0xFE) >> 1;
    }
    else
        pts = MP_NOPTS_VALUE;

    return pts;
}

void demux_skyts_CopyToDemuxPacket( demux_stream_t *ds,
                                    unsigned char *buffer, int size, off_t pos, int64_t pts )
{
    demux_packet_t *dp = new_demux_packet( size );
    memcpy( dp->buffer, buffer, size );
    if (pts != MP_NOPTS_VALUE)
        dp->pts = pts / 90000.0;
    dp->pos = pos;
    dp->flags = 0;
    ds_add_packet( ds, dp );
}

static int demux_skyts_FindESHeader( uint8_t nal,
                                     unsigned char *buffer, int bufferSize )
{
    uint32_t search = 0x00000100 | (nal&0xf0);
    uint32_t found = -1;
    uint8_t *p = buffer;
    uint8_t *end = p + bufferSize;
    while (p < end) {
        found <<= 8;
        found |= *p++;
//Robert 20100325 Fix PES Search Pattern
        if ((found&0xfffffff0) == search)
            return p - buffer - 4;
    }
    return -1;
}

void demux_skyts_update_video_packet(unsigned char *buffer, int size)
{
    SkytsInfo *skyts = g_skyts_demux->priv;

    skyts->lastVideoPTS = get_skyts_pts( buffer );

    demux_skyts_CopyToDemuxPacket( g_skyts_video,
                                   buffer, size, g_skyts_demux->filepos + size,
                                   skyts->lastVideoPTS );
}

void demux_skyts_update_audio_packet(unsigned char *buffer, int size)
{
    SkytsInfo *skyts = g_skyts_demux->priv;

    skyts->lastAudioPTS = get_skyts_pts( buffer );

    demux_skyts_CopyToDemuxPacket( g_skyts_audio,
                                   buffer, size, g_skyts_demux->filepos + size,
                                   skyts->lastAudioPTS );
}


static int demux_skyts_fill_buffer( demuxer_t *demux, demux_stream_t *ds )
{
    int              invalidType = 0;
    int              errorHeader = 0;
    int              recordsDecoded = 0;

    int              readSize = 0;

    int              numberRecs;
    unsigned char    *recPtr;
    int              offset;

    int              counter;

    int              aid;
    static int video_is_h264=0, video_is_mpeg2=0;
    static int got_h264_start_code = 0;
#if USE_READ_TIME_BENCH
    clock_t skdvb_read_time=0;
#endif
    SkytsInfo         *skyts = demux->priv;
    dvb_priv_t *dvbpriv  = (dvb_priv_t *) demux->stream->priv;
	extern unsigned int no_osd;
	extern int dvdsub_id;

    g_skyts_demux = demux;


#ifdef DVBT_USING_NORMAL_METHOD
    if (do_skyts_fill_first == 1)
	{
		if (demux->stream)
			dvb_update_fd(demux->stream);
        if (skydvb_thread_running == 0)
        {
            if(pthread_create(&skydvb_read_thread, NULL, skyviia_read_stream, demux)!= 0)
				printf("@@@ We create skyviia_read_stream failed and skydvb_read_thread is [%d] errno[%d][%s]@@@@\n", skydvb_read_thread, errno, strerror(errno));
			else
				printf("@@@ We create skyviia_read_stream success and skydvb_read_thread is [%d] errno[%d][%s]@@@@\n", skydvb_read_thread, errno, strerror(errno));
            while (skydvb_thread_running == 0)
                usleep(100);
        }
		else
		{
			/* TODO, we need re-create thread */
			printf("######## skydvb_thread_running != 0 [%d], not call skyviia_read_stream skydvb_read_thread[%d]#####\n", skydvb_thread_running, skydvb_read_thread);
		}
	}
#else // else of DVBT_USING_NORMAL_METHOD
    if (ds==demux->video)
    {
        dvb_is_read_video = 1;
    }
    else
    {
        dvb_is_read_video = 0;
    }
#endif // end of DVBT_USING_NORMAL_METHOD

#ifdef AUTO_CHECK
    if (do_skyts_fill_first == 1)
    {
		int a_type = 0, v_type = 0;
		int s_type = 0;
		int check_status = 0;
		extern es_stream_type_t;
        extern int skyfd;
        demux_stream_t *ds = demux->video;
		dvb_channel_t *channels = &(dvbpriv->list->channels[dvbpriv->list->current]);
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
		extern int setdvb_by_pid;
		if(!setdvb_by_pid)
		{
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID

			skydvb_printf("@@ call get_video_audio_id parameter is v_pid[%d] a_pid[%d] p_pid [%d] v_type[%d] a_type[%d], fd[%d]@@@\n", channels->pids[0], channels->pids[1], channels->pids[2], v_type, a_type, dvbpriv->demux_fds[2]);
			check_status = get_video_audio_id(channels->pids[0], channels->pids[1], channels->pids[2], &v_type, &a_type, dvbpriv->demux_fds[2]);
#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
		}
		else
		{
			v_type = channels->pids_format[0]; 
			a_type = channels->pids_format[1]; 
			s_type = channels->pids_format[2]; 
			skydvb_printf("@@ call get_video_audio_id parameter is v_pid[%d] a_pid[%d] p_pid [%d] v_type[%d] a_type[%d], fd[%d]@@@\n", channels->pids[0], channels->pids[1], channels->pids[2], v_type, a_type, dvbpriv->demux_fds[2]);
			check_status = 1;

		}
#endif // end of SUPPORT_SKYDVB_DYNAMIC_PID
		printf("@@@@ video_type is [%.8x], audio_type is [%.8x] check_status [%d]@@@@@\n", v_type, a_type, check_status);

		/* If we can't received any pmt information, then we need send skydvb_need_workaround */
		if (check_status > 0)
			set_skydvb_wrokaround(READY_TO_DISPLAY);
		else 
		{
			set_skydvb_wrokaround(LOST_DVBT_SIGNAL);
			if (check_status < 0)
			{
				printf("@@@@ In [%s][%d] DVBT: Can't match program id[%d] @@@@\n", __func__, __LINE__, channels->pids[2]);
				ipc_callback("DVBT: Can't match program id");
				return 0;
			}
		}
			


        g_skyts_video = 0;
        g_skyts_audio = 0;
        aid = 0x0;
        if ( demux->v_streams && !demux->v_streams[ aid ] ) new_sh_video( demux, aid );

        if (demux->video->id != -2)
        {
            demux->video->id = aid;

            g_skyts_video = demux->video;

            if ( !ds->sh )
            {
                sh_video_t *sh;

                ds->sh = demux->v_streams[ aid ];
                sh = ds->sh;
				if (v_type)
				{
					sh->format = check_format(v_type, TYPE_VIDEO);
					if (!sh->format)
						sh->format = VIDEO_MPEG2; // set default mpeg2
					printf("==== set video format to [%.4x]===\n", sh->format);
				}
				else
				{
					if (channels->pids[0] == 2001)
						sh->format = 0x10000005;
					else
						sh->format = 0x10000002;
				}
				if (sh->format == 0x10000005)
				{
					video_is_h264 = 1;
					video_is_mpeg2 = 0;
				}
				else
				{
					video_is_h264 = 0;
					video_is_mpeg2 = 1;
				}
            }
#ifdef DVBT_USING_NORMAL_METHOD
			/* For DVB-T change channel */
			else
			{
				sh_video_t *sh;

				ds->sh = demux->v_streams[ aid ];
				sh = ds->sh;
				if (v_type)
					sh->format = check_format(v_type, TYPE_VIDEO);
				/* Set defalut audio decoder if format is unknown */
				if (!sh->format)
					sh->format = VIDEO_MPEG2;
				printf("==== set video format to [%.4x] [%s][%d]===\n", sh->format, __func__, __LINE__);
			}
#endif // end of DVBT_USING_NORMAL_METHOD
        }

		got_h264_start_code = 0;

#ifdef SUPPORT_SKYDVB_DYNAMIC_PID
		if((!no_osd) && (dvdsub_id != DO_NOT_PLAY) && channels->pids[2] > 0)
		{
			sh_sub_t *sh_s = NULL;
			aid = 0x0;	// assume 
			if (demux->s_streams && !demux->s_streams[aid])
				sh_s = new_sh_sub_sid(demux, aid, channels->pids[2]);
			else
			{
				sh_s = (sh_sub_t *)demux->s_streams[aid];
			}

			if(sh_s)
			{
				sh_s->format = check_format(s_type, TYPE_SUB);
				if(!sh_s->format)
					sh_s->format = SPU_TELETEXT;
				switch(sh_s->format)
				{
					case SPU_DVB:
						sh_s->type = 'b';
						break;
					case SPU_PGS:
						sh_s->type = 'p';
						break;
					default:
					case SPU_TELETEXT:
						sh_s->type = 'd';
				}
				skydvb_printf("$$$ In [%s][%d] DVB-T set subtitle to [%c][%d] $$$\n", __func__, __LINE__, sh_s->type, sh_s->format);
				skydvb_printf("### In [%s][%d] set format to [%x] ###\n", __func__, __LINE__, sh_s->format);
			}
			else
			{
				mplayer_debug("### In [%s][%d] create sub failed pid [%d] ###\n", __func__, __LINE__, channels->pids[2]);
			}
		}
#endif /* end of SUPPORT_SKYDVB_DYNAMIC_PID */		
        //if ( demux->audio->id != -2 ) /* Sometimes the strength of DVB-T singal is weak then causes video to no-sound  */
        if ( demux->audio->id != -2  || channels->pids[1])
        {
            aid = 0x0;   // assume MPEG Audio

            g_skyts_audio = demux->audio;

            demux->audio->id = aid;
            if ( demux->a_streams && !demux->a_streams[ aid ] ) new_sh_audio( demux, aid );
            if ( demux->audio->id == aid )
            {
                ds = demux->audio;
                if ( !ds->sh ) {
                    sh_audio_t* sh_a;
                    ds->sh = demux->a_streams[ aid ];
                    sh_a = (sh_audio_t*)ds->sh;
#if 1 // carlos add 2010-07-22, check audio_format by self
					sh_a->format = check_format(a_type, TYPE_AUDIO);
					sh_a->aid = channels->pids[1];
					/* Set defalut audio decoder if format is unknown */
					if (!sh_a->format)
						sh_a->format = AUDIO_MP2;
					printf("==== set audio format to [%.4x]===\n", sh_a->format);
#else // carlos add 2010-07-22
                    switch (aid & 0xE0) {  // 1110 0000 b  (high 3 bit: type  low 5: id)
                    case 0x00:
                        sh_a->format=0x50;
                        break; // mpeg
                    case 0xA0:
                        sh_a->format=0x10001;
                        break;  // dvd pcm
                    case 0x80:
                        if ((aid & 0xF8) == 0x88) sh_a->format=0x2001;//dts
                        else sh_a->format=0x2000;
                        break; // ac3
                    }

                    if (video_is_h264)
                    {
                        sh_a->format=0x706d;
                        printf("=== set sh_a->format = 0x706d --> fix me later, need to do parse..\n");
                    }
#endif // end of carlos add 2010-07-22
                }
#ifdef DVBT_USING_NORMAL_METHOD
				/* For DVB-T change channel */
				else
				{
					sh_audio_t* sh_a;
					ds->sh = demux->a_streams[0];
					sh_a = (sh_audio_t*)ds->sh;
					sh_a->format = check_format(a_type, TYPE_AUDIO);
					sh_a->aid = channels->pids[1];
					if (!sh_a->format)
						sh_a->format = AUDIO_MP2;
					printf("==== set audio format to [%.4x]-[%s][%d]===\n", sh_a->format, __func__, __LINE__);
				}
#endif // end of DVBT_USING_NORMAL_METHOD
            }
        }
#ifdef DVBT_USING_NORMAL_METHOD
#else // else of DVBT_USING_NORMAL_METHOD
        if (chunk == 0)
        {
            chunk = malloc(PACKSIZE);
            printf(" @@@@@@ chunk=0x%p\n", chunk);
            memset(chunk, 0x00, PACKSIZE);
        }

#if USE_APES_BACKUP
//==== do apes backup init
        {
            int pp;
            for (pp=0; pp<MAX_APES_LEN; pp++)
            {
                if (apes_backup_buffer[pp])
                {
                    free(apes_backup_buffer[pp]);
                    apes_backup_buffer[pp]=0;
                    apes_backup_idx_size[pp]=0;
                }
                else
                {
                    apes_backup_buffer[pp]=0;
                    apes_backup_idx_size[pp]=0;
                }
            }
        }
#endif
#endif // end of DVBT_USING_NORMAL_METHOD

        if(skyts)
		{
			skyts->readHeader = 0;
		}
		else
		{
			dvb_debug_printf("@@@change channel, we don't have skyts @@@@\n");
			skyts = calloc(1, sizeof(SkytsInfo));
			demux->priv = skyts;
		}
        dmx_skyts_fill_data_type = 0;
        do_skyts_fill_first = 0;
        if (skyfd == -1)
            init_sky();
    }
#else // else of AUTO_CHECK
    if (do_skyts_fill_first == 1)
    {
        extern int skyfd;
        int get_channel_id = dvb_get_channel_id(demux->stream);
        demux_stream_t *ds = demux->video;


        g_skyts_video = 0;
        g_skyts_audio = 0;
        aid = 0x0;
        if ( !demux->v_streams[ aid ] ) new_sh_video( demux, aid );

        if (demux->video->id != -2)
        {
            demux->video->id = aid;

            g_skyts_video = demux->video;

            if ( !ds->sh )
            {
                sh_video_t *sh;

                ds->sh = demux->v_streams[ aid ];
                sh = ds->sh;
                if (get_channel_id == 2001)
                {
                    sh->format = 0x10000005;
                }
                else
                {
                    sh->format = 0x10000002;
                }
            }
        }

        if (get_channel_id == 2001)
        {
            video_is_h264 = 1;
            video_is_mpeg2 = 0;
        }
        else
        {
            video_is_h264 = 0;
            video_is_mpeg2 = 1;
        }
		got_h264_start_code = 0;

        if ( demux->audio->id != -2 )
        {
            aid = 0x0;   // assume MPEG Audio

            g_skyts_audio = demux->audio;

            demux->audio->id = aid;
            if ( !demux->a_streams[ aid ] ) new_sh_audio( demux, aid );
            if ( demux->audio->id == aid )
            {
                ds = demux->audio;
                if ( !ds->sh ) {
                    sh_audio_t* sh_a;
                    ds->sh = demux->a_streams[ aid ];
                    sh_a = (sh_audio_t*)ds->sh;
                    switch (aid & 0xE0) {  // 1110 0000 b  (high 3 bit: type  low 5: id)
                    case 0x00:
                        sh_a->format=0x50;
                        break; // mpeg
                    case 0xA0:
                        sh_a->format=0x10001;
                        break;  // dvd pcm
                    case 0x80:
                        if ((aid & 0xF8) == 0x88) sh_a->format=0x2001;//dts
                        else sh_a->format=0x2000;
                        break; // ac3
                    }

                    if (video_is_h264)
                    {
                        sh_a->format=0x706d;
                        printf("=== set sh_a->format = 0x706d --> fix me later, need to do parse..\n");
                    }
                }
            }
        }

        if (chunk == 0)
        {
            chunk = malloc(PACKSIZE);
            printf(" @@@@@@ chunk=0x%p\n", chunk);
            memset(chunk, 0x00, PACKSIZE);
        }

#if USE_APES_BACKUP
//==== do apes backup init
        {
            int pp;
            for (pp=0; pp<MAX_APES_LEN; pp++)
            {
                if (apes_backup_buffer[pp])
                {
                    free(apes_backup_buffer[pp]);
                    apes_backup_buffer[pp]=0;
                    apes_backup_idx_size[pp]=0;
                }
                else
                {
                    apes_backup_buffer[pp]=0;
                    apes_backup_idx_size[pp]=0;
                }
            }
        }
#endif

		printf("demux_skyts_fill_buffer: do_skyts_fill_first = 1\n");
		skyts->readHeader = 0;
        dmx_skyts_fill_data_type = 0;
        do_skyts_fill_first = 0;
        if (skyfd == -1)
        {
            init_sky();
        }
    }
#endif // end of AUTO_CHECK

    if ( demux->stream->eof ) return 0;

    {
        if ( skyts && skyts->readHeader == 0 )
        {
            off_t filePos;
            dvb_debug_printf("@@@@@@ skyts->readHeader=%d\n", skyts->readHeader);

            skyts->readHeader = 1;

            filePos = demux->filepos;
            stream_seek( demux->stream, 0 );

#if USE_READ_TIME_BENCH
            skdvb_read_time = GetTimer();
#endif

#if USE_DIRECT_STREAM_READ
            readSize = dvb_streaming_read( demux, demux->stream, chunk, PACKSIZE );
#else
            readSize = stream_read( demux->stream, chunk, PACKSIZE );
#endif

#if USE_READ_TIME_BENCH
            skdvb_read_time = GetTimer() - skdvb_read_time;
            printf("===== need to add special Header here..., read time:%d ms\n", skdvb_read_time/1000);
#endif
            if ( demux->stream->start_pos > 0 )
                filePos = demux->stream->start_pos;
//            stream_seek( demux->stream, filePos );
            demux->filepos = stream_tell( demux->stream );
            skyts->whichChunk = filePos / PACKSIZE;

//dvb_pfm_buffer_update(0xffffffff);

        }
        demux->movi_start = 0;
        demux->movi_end = skyts->size;
    }

#ifdef DVBT_USING_NORMAL_METHOD
#else // else of DVBT_USING_NORMAL_METHOD

#if 1
    do {
        demux->filepos = stream_tell( demux->stream );
        skyts->whichChunk = demux->filepos / PACKSIZE;
        if (readSize == 0)
        {
	    int dvb_checking_timeout=0;

#if USE_READ_TIME_BENCH
            skdvb_read_time = clock();
#endif
            while ((dvb_is_read_video && dvb_get_vq_count()<=0) || (!dvb_is_read_video && dvb_get_aq_count()<=0) )
            {
                usec_sleep(1);
		if (dvb_checking_timeout++ > 5)
			break;
            }

            if ((dvb_is_read_video && dvb_get_vq_count()>0) || (!dvb_is_read_video && dvb_get_aq_count()>0) )
            {
#if USE_DIRECT_STREAM_READ
                readSize = dvb_streaming_read( demux, demux->stream, chunk, PACKSIZE );
#else
                readSize = stream_read( demux->stream, chunk, PACKSIZE );
#endif
            }
            else
            {
                return 1;
            }

#if USE_READ_TIME_BENCH
            skdvb_read_time = clock() - skdvb_read_time;
#endif
        }
    } while (0);
#endif

    numberRecs = chunk[ 0 ];
    recPtr = &chunk[ 4 ];

#if USE_READ_TIME_BENCH

    if (skdvb_read_time>0)
    {
        if (numberRecs == 0)
        {
            if (dvb_is_read_video)
                printf("        ******  V  numberRecs=%d  readtime:%d ms  ds queue v:%d a:%d\n", numberRecs, skdvb_read_time/1000, demux->video->packs, demux->audio->packs);
            else
                printf("        ******  A  numberRecs=%d  readtime:%d ms  ds queue v:%d a:%d\n", numberRecs, skdvb_read_time/1000, demux->video->packs, demux->audio->packs);
        }
        else
        {
            if (dvb_is_read_video)
                printf("           #####  V  numberRecs=%d  readtime:%d ms  ds queue v:%d a:%d\n", numberRecs, skdvb_read_time/1000, demux->video->packs, demux->audio->packs);
            else
                printf("           #####  A  numberRecs=%d  readtime:%d ms  ds queue v:%d a:%d\n", numberRecs, skdvb_read_time/1000, demux->video->packs, demux->audio->packs);
        }
    }
#endif

    if (numberRecs>0)
    {
//
    }
    else
    {
        return 1;
    }

    for ( counter = 0 ; counter < numberRecs ; counter++ )
    {
        int size = *((unsigned int *) &recPtr[0]) ;
        int type = *((unsigned int *) &recPtr[8]) ;
        int nybbleType = *((unsigned int *) &recPtr[12]) ;

        if (nybbleType== 0xe0)
        {
            type = 0xe0;
        }
        offset = *((unsigned int *) &recPtr[4]) ;

        recordsDecoded++;

        // ================================================================
        // Video
        // ================================================================
#ifndef USE_PFM_ADDR_MODE	//PFM DATA Mode
        //if ( type == 0xe0 )
		/* 
		 * carlos 2010-08-18 add
		 * Support all video stream, according to the pes spec(13818-1, 2.4.3.7 Semantic definition of fields in PES packet), video type is from 0xe0~0xef
		 */
        if ( (type >= 0xe0) && (type <= 0xef))
        {
            if ( size > 0 && size + offset <= PACKSIZE )
            {
                int esOffset1 = demux_skyts_FindESHeader( 0xe0, &chunk[ offset ],
                                size);
                if ( esOffset1 != -1 )
                    skyts->lastVideoPTS = get_skyts_pts( &chunk[ offset + esOffset1 ] );	//Barry 2010-01-28
//printf("vPTS=%f   vPTS-aPTS=%f\n", (float)skyts->lastVideoPTS, (float)skyts->lastVideoPTS-(float)skyts->lastAudioPTS);

                {
                    int headerSize = 0;
                    int packetSize = size;
//int v_payload_size=chunk[offset + headerSize+4]<<8|chunk[offset + headerSize+5];
                    int v_payload_size = size;
//                    int v_pes_is_aligned = chunk[offset + headerSize+6] & 4;
                    int v_hdr_len=chunk[offset + headerSize+8];
					//printf("=== carlos say offset [%d]headerSize [%d] v_hdr_len [%d] v_payload_size [%d] [%d] pts[%f]===\n", offset, headerSize, v_hdr_len, v_payload_size, v_payload_size - v_hdr_len -8, skyts->lastVideoPTS);
                    //v_payload_size -= (v_hdr_len + 8);
                    v_payload_size -= (v_hdr_len + 8 + 1); // carlos change 2010-08-18, we need decrease 1 when calculate v_payload_size

                    demux_skyts_CopyToDemuxPacket( demux->video,
                                                   &chunk[ offset + headerSize + 8 + v_hdr_len +1],
                                                   v_payload_size,
                                                   demux->filepos + offset,
                                                   skyts->lastVideoPTS );
                }
                offset += size;
            }
            else
                errorHeader++;
        }
#else	//PFM Addr Mode
        //if ( type == 0xe0 )
		/* 
		 * carlos 2010-08-18 add
		 * Support all video stream, according to the pes spec(13818-1, 2.4.3.7 Semantic definition of fields in PES packet), video type is from 0xe0~0xef
		 */
        if ( (type >= 0xe0) && (type <= 0xef))
        {
//            printf("== type == 0xe0 (video) nybbleType=%d size=%d size+offset=%d\n", nybbleType, size, size+offset);
//printf("cnt:%d  type=0xe0  size=%d\n", counter, size);
            if ( size > 0 && size + offset <= PACKSIZE )
            {
#if 0
                int esOffset1 = demux_skyts_FindESHeader( 0xe0, &chunk[ offset ],
                                size);
#else
                int esOffset1 = 0;
#endif
//printf("~~~~~ esOffset1=%d nybbleType=%d\n", esOffset1, nybbleType);

                if (dmx_skyts_fill_data_type == 0)
                {
                    int pp;
                    unsigned int *lptr = &chunk[offset];

                    unsigned char *pVal;
                    unsigned int addr1, addr2, addr3, h264_soffset=0;
//int got_h264_start_code = 0;
                    int64_t *vPTS = &lptr[4];
//printf("vPTS=%f\n", (float)*vPTS);
                    /*
                    printf("\n===========================\n");
                    printf("lptr[0]=0x%x(%d)\n", lptr[0], lptr[0]);
                    printf("lptr[1]=0x%x(%d)\n", lptr[1], lptr[1]); //size
                    printf("lptr[2]=0x%x(%d)\n", lptr[2], lptr[2]);	//addr
                    printf("lptr[3]=0x%x(%d)\n", lptr[3], lptr[3]);
                    */
                    addr1 = lptr[2]&0x0fffffff;
                    addr2 = (unsigned int)PFMPhysicalBase;
                    addr3 = (unsigned int)PFMVirtualBase;
                    pVal = (unsigned int *) (addr1 - addr2 + addr3+lptr[1]-64);

                    if ( *vPTS != MP_NOPTS_VALUE )
                    {
                        skyts->lastVideoPTS = *vPTS;
                    }
                    else
                    {
                        skyts->lastVideoPTS = MP_NOPTS_VALUE;
                    }

                    demux_skyts_CopyToDemuxPacket( demux->video,
                                                   pVal, lptr[1], demux->filepos + offset,
                                                   skyts->lastVideoPTS );
//printf("#### copy raw es done, do pfm buf update\n");
                    dvb_pfm_buffer_update(lptr[1]);
//printf("  **** copy done\n");
                }
                else
                {
//printf("############ copy addr data\n");
//int pp;
                    unsigned int *lptr = &chunk[offset];
                    int64_t *vPTS = &lptr[4];
//printf("vPTS=%f\n", (float)*vPTS);

                    unsigned char *pVal;
                    unsigned int addr1, addr2, addr3, h264_soffset=0, pp;
                    addr1 = lptr[2]&0x0fffffff;
                    addr2 = (unsigned int)PFMPhysicalBase;
                    addr3 = (unsigned int)PFMVirtualBase;
                    pVal = (unsigned int *) (addr1 - addr2 + addr3 +lptr[1]-64);
                    if (video_is_h264 == 1 && got_h264_start_code == 0)
                    {
                        for (pp=0; pp<64; pp++)
                        {
                            /*
                                  if (pVal[pp] == 0x28 || pVal[pp] == 0x27)
                                    printf("\"%.2x\" ", pVal[pp]);
                                  else
                                    printf("%.2x ", pVal[pp]);
                            */
                            //find h264 start code
                            if (pVal[pp+0]==0 && pVal[pp+1]==0 && pVal[pp+2]==1 && ((pVal[pp+3]&0x0f)==0x07))
                            {
                                h264_soffset = pp;
                                got_h264_start_code = 1;
                                printf("\n@@@@@ got h264_start_code, offset=%d\n", pp);
                                break;
                            }
                        }
                    }

//printf("############ copy raw es data\n");
                    if ( *vPTS != MP_NOPTS_VALUE )
                    {
                        skyts->lastVideoPTS = *vPTS;
                    }
                    else
                    {
                        skyts->lastVideoPTS = MP_NOPTS_VALUE;
                    }

                    if ((video_is_mpeg2 == 1) || (video_is_h264 == 1 && got_h264_start_code == 1))
                    {
                        demux_skyts_CopyToDemuxPacket( demux->video,
                                                       &chunk[ offset ], size, demux->filepos + offset,
                                                       skyts->lastVideoPTS );
                    }
                    dvb_pfm_buffer_update(lptr[1]);
                }
                offset += size;
//printf("vPTS=%f\n", (float)skyts->lastAudioPTS);

//printf("skyts->lastVideoPTS = %f\n", (float)skyts->lastVideoPTS);

            }
            else
                errorHeader++;
        }
#endif
        // ================================================================
        // Audio
        // ================================================================
//		else if ( type == 0xc0 || type == 0xc1  || type == 0xbd )
		/* 
		 * carlos 2010-08-18 add
		 * 0xfd for AC3 audio , follow MPEG4 video, 0xfd is reserved data stream
		 * Support spain audio stream_id 0xc4, 0xc6, according to the pes spec, audio type is from 0xc0~0xdf
		 */
        else if ( ((type >= 0xc0) && (type <= 0xdf)) || type == 0xbd || type == 0xfd ) 
        {
            if ( size > 0 && size + offset <= PACKSIZE )
            {
                aid = demux->audio->id;

                // ================================================
                if ( nybbleType == 0x03 || nybbleType == 0x09 )
                {
                    int esOffset1, esOffset2;
                    if (size==16)
                        printf("======= audio size=16\n");
                    // ================================================
                    // Audio with PES Header
                    // ================================================
                    {
                        esOffset1 = demux_skyts_FindESHeader( type, &chunk[ offset ], size);

                        if (esOffset1 != -1)
                        {
                            int packetSize = size;
                            int headerSize=0;
                            int ptsOffset=0;

                            {

                                skyts->lastAudioPTS = get_skyts_pts(
                                                          &chunk[ offset + esOffset1 + ptsOffset ] );
//printf("@@@@@ aPTS=%f\n", (float)skyts->lastAudioPTS);

                                if (nybbleType == 9) headerSize = 0;
                                if ((dmx_skyts_fill_data_type == 0) || (video_is_mpeg2 == 1) || (video_is_h264 == 1 && got_h264_start_code == 1))
                                {
                                    int a_payload_size=chunk[offset + esOffset1 + headerSize+4]<<8|chunk[offset + esOffset1 + headerSize+5];
                                    int a_pes_is_aligned = chunk[offset + esOffset1 + headerSize+6] & 4;
                                    int a_hdr_len=chunk[offset + esOffset1 + headerSize+8];
                                    a_payload_size -= a_hdr_len + 3;
//printf("len:%d a payload:%d  is_aligned:%d  hdr_len:%d\n",packetSize - headerSize, a_payload_size, a_pes_is_aligned, a_hdr_len);
                                    demux_skyts_CopyToDemuxPacket
                                    (
                                        demux->audio,
                                        &chunk[ offset + esOffset1 + headerSize + (packetSize - headerSize-a_payload_size)  ],
                                        a_payload_size,
                                        demux->filepos + offset,
                                        skyts->lastAudioPTS
                                    );


#if USE_APES_BACKUP
//==== do apes backup for debugging
                                    if (apes_backup_buffer[apes_cur_idx])
                                    {
                                        free(apes_backup_buffer[apes_cur_idx]);
                                    }
//printf("!! packetSize=%d headerSize=%d\n", packetSize, headerSize);
                                    apes_backup_buffer[apes_cur_idx] = malloc(packetSize - headerSize);
                                    apes_backup_idx_size[apes_cur_idx] = packetSize - headerSize;
                                    memcpy(apes_backup_buffer[apes_cur_idx], &chunk[ offset + esOffset1 + headerSize ], packetSize - headerSize);
                                    apes_cur_idx = (apes_cur_idx + 1)% MAX_APES_LEN;
#endif
                                }
                            }
                        }

                    }
                }

                offset += size;
            }
            else
                errorHeader++;
        }
        // ================================================================
        // Unknown
        // ================================================================
        else
        {
            if ( size > 0 && size + offset <= PACKSIZE )
                offset += size;
            if (type != 3 && type != 5 && (type != 0 || size > 0)) {
                mp_msg( MSGT_DEMUX, MSGL_DBG3, "skyts:Invalid Type %x\n", type );
                printf("skyts:Invalid Type %x\n", type );
                invalidType++;
            }
        }
        recPtr += 16;
    }

    if ( errorHeader > 0 || invalidType > 0 )
    {
        mp_msg( MSGT_DEMUX, MSGL_DBG3,
                "skyts:Error Check - Records %d, Parsed %d, Errors %d + %d\n",
                numberRecs, recordsDecoded, errorHeader, invalidType );
        printf("skyts:Error Check - Records %d, Parsed %d, Errors %d + %d\n", numberRecs, recordsDecoded, errorHeader, invalidType );

        // Invalid MPEG ES Size Check
        if ( errorHeader > numberRecs / 2 )
            return 0;

        // Invalid MPEG Stream Type Check
        if ( invalidType > numberRecs / 2 )
            return 0;
    }
#endif // end of ifndef DVBT_USING_NORMAL_METHOD
    demux->filepos = stream_tell( demux->stream );

    return 1;
}

static void demux_seek_skyts( demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags )
{
//    demux_stream_t *d_audio = demuxer->audio;
    demux_stream_t *d_video = demuxer->video;
//    sh_audio_t     *sh_audio = d_audio->sh;
    sh_video_t     *sh_video = d_video->sh;
    off_t          newpos;
    off_t          res;
    SkytsInfo       *skyts = demuxer->priv;

    printf("!!!! skyts:Seeking to %7.1f\n", rel_seek_secs );

    mp_msg( MSGT_DEMUX, MSGL_DBG3, "skyts:Seeking to %7.1f\n", rel_seek_secs );

    //skyts->lastAudioEnd = 0;
    skyts->lastAudioPTS = MP_NOPTS_VALUE;
    skyts->lastVideoPTS = MP_NOPTS_VALUE;
    //
    //================= seek in MPEG ==========================
    demuxer->filepos = stream_tell( demuxer->stream );

    newpos = ( flags & 1 ) ? demuxer->movi_start : demuxer->filepos;
#ifdef DVBT_USING_NORMAL_METHOD
#else // else of DVBT_USING_NORMAL_METHOD

    if ( flags & 2 )
        // float seek 0..1
        newpos += ( demuxer->movi_end - demuxer->movi_start ) * rel_seek_secs;
    else
    {
        // time seek (secs)
        if ( ! sh_video->i_bps ) // unspecified or VBR
            newpos += 2324 * 75 * rel_seek_secs; // 174.3 kbyte/sec
        else
            newpos += sh_video->i_bps * rel_seek_secs;
    }

    if ( newpos < demuxer->movi_start )
    {
        if ( demuxer->stream->type != STREAMTYPE_VCD ) demuxer->movi_start = 0;
        if ( newpos < demuxer->movi_start ) newpos = demuxer->movi_start;
    }

    res = newpos / PACKSIZE;
    if ( rel_seek_secs >= 0 )
        newpos = ( res + 1 ) * PACKSIZE;
    else
        newpos = res * PACKSIZE;

    if ( newpos < 0 )
        newpos = 0;

    skyts->whichChunk = newpos / PACKSIZE;

    stream_seek( demuxer->stream, newpos );
#endif // end of DVBT_USING_NORMAL_METHOD
    // re-sync video:
    videobuf_code_len = 0; // reset ES stream buffer
}

static int demux_skyts_control( demuxer_t *demuxer,int cmd, void *arg )
{
    demux_stream_t *d_video;
    sh_video_t     *sh_video;
//printf("in demux_skyts_control....\n");

    d_video = demuxer->video;
    sh_video = d_video->sh;
	SkytsInfo *skyts = demuxer->priv;

    switch (cmd)
    {
    case DEMUXER_CTRL_GET_TIME_LENGTH:
        if (sh_video == NULL)	//audio only
            return DEMUXER_CTRL_DONTKNOW;
        if (!sh_video->i_bps) // unspecified or VBR
            return DEMUXER_CTRL_DONTKNOW;
        *(double *)arg=
            (double)(demuxer->movi_end-demuxer->movi_start)/sh_video->i_bps;
        return DEMUXER_CTRL_GUESS;

    case DEMUXER_CTRL_GET_PERCENT_POS:
        return DEMUXER_CTRL_DONTKNOW;
		/*  Switch subtitle */
	case DEMUXER_CTRL_ADD_SUBTITLE_STREAM:
		{
			int ii = 0;
			char need_add_new = 1;
			demux_program_t *prog = arg;
			demux_stream_t *ds = demuxer->sub;
			//mplayer_debug(" #### In [%s][%d] got add subtitle command sid[%d] sformat[%d]####\n", __func__, __LINE__, prog->sid, prog->sformat);

			sh_sub_t *sh_s = NULL;

			ds = demuxer->sub;
			if (ds)
			{
				ds->sh = demuxer->s_streams[0];

				if (ds->sh)
				{
					sh_s = (sh_sub_t *)demuxer->s_streams[0];
					if (sh_s == NULL)
					{
						printf("$$$$$$$$$$$$$$$$sh_s is NULL in [%s][%d] $$$$$$$$$\n", __func__, __LINE__);
						break;
					}
					/* exist subtitle id, need check duplicated */
					//printf("#### In [%s][%d] sh_s->sid [%d]prog->sid [%d] ####\n", __func__, __LINE__, sh_s->sid, prog->sid);
					if (sh_s->sid != prog->sid)
					{
						/* Stop old filter, free packets, and restart new fitler */
						dvb_priv_t *priv  = (dvb_priv_t *) demuxer->stream->priv;
						printf("#### In [%s][%d] Got the switch audio pid from [%d] to [%d]####\n", __func__, __LINE__, sh_s->sid, prog->sid);
						sh_s->sid = prog->sid;
						dvb_demux_stop(priv->demux_fds[2]);
						ds_free_packs(ds);
						dvb_set_tsout_filt(priv->demux_fds[2], sh_s->sid, DMX_PES_SUBTITLE, 0);

					}
					else
					{
						/* the duplicated case pid  */
						//mplayer_debug("#### In [%s][%d] duplicated subtitle pid, check format , sh_s->type [%x] prog->sformat [%x] type[%x] ####\n", __func__, __LINE__, sh_s->type, prog->sformat, check_format(prog->sformat, TYPE_SUB));
						if (sh_s->format != check_format(prog->sformat, TYPE_SUB))
						{
							sh_s->format = check_format(prog->sformat, TYPE_SUB);
							switch(sh_s->format)
							{
								case SPU_DVB:
									sh_s->type = 'b';
									break;
								case SPU_PGS:
									sh_s->type = 'p';
									break;
								default:
								case SPU_TELETEXT:
									sh_s->type = 'd';
							}
							mplayer_debug("$$$ In [%s][%d] DVB-T set subtitle to [%c][%x] $$$\n", __func__, __LINE__, sh_s->type, sh_s->format);
						}
					}
				}
				else
				{
					dvb_priv_t *priv  = (dvb_priv_t *) demuxer->stream->priv;
					sh_s = new_sh_sub_sid(demuxer, 0, prog->sid);
					sh_s->format = check_format(prog->sformat, TYPE_SUB);
					sh_s->sid = prog->sid;
					if (!sh_s->format)
						sh_s->format = SPU_TELETEXT;
					switch(sh_s->format)
					{
						case SPU_DVB:
							sh_s->type = 'b';
							break;
						case SPU_PGS:
							sh_s->type = 'p';
							break;
						default:
						case SPU_TELETEXT:
							sh_s->type = 'd';
					}
					mplayer_debug("$$$ In [%s][%d] DVB-T set subtitle to [%c][%d] $$$\n", __func__, __LINE__, sh_s->type, sh_s->format);
					dvb_set_tsout_filt(priv->demux_fds[2], sh_s->sid, DMX_PES_SUBTITLE, 0);
				}
			}
			else
			{
				mplayer_debug("$$$$$$$$$$$$$In [%s][%d] ds is NULL $$$$$$$$$$$$$$$$$$$$\n", __func__, __LINE__);
			}


			
			break;
		}
	case DEMUXER_CTRL_SWITCH_AUDIO:
		{
			int tmp = *((int *) arg);
			int ii = 0;
			demux_stream_t *ds = demuxer->audio;
			//mplayer_debug(" #### In [%s][%d] got switch audio command ####\n", __func__, __LINE__);
			mplayer_debug(" #### In [%s][%d] got switch audio command aid[%d] tmp[%d] priv->last_aid is [%d]###\n", __func__, __LINE__, skyts->whichChunk, tmp, skyts? skyts->lastAudioEnd:-1);
			for(ii = 0; ii <= skyts->lastAudioEnd; ii++)
			{
				sh_audio_t* sh_a = NULL;
				ds->sh = demuxer->a_streams[ii];
				sh_a = (sh_audio_t*)ds->sh;
				//printf("== in [%s][%d] DEMUXER_CTRL_SWITCH_AUDIO audio stream[%d] = [%d], new aid is [%d] ==\n", __func__, __LINE__, ii, sh_a->aid);
				if (tmp == sh_a->aid)
				{
					if (ds->id != ii)
					{
						dvb_priv_t *priv  = (dvb_priv_t *) demuxer->stream->priv;
						ds->id = ii;
						printf("#### In [%s][%d] Got the switch audio pid ####\n", __func__, __LINE__);
						dvb_demux_stop(priv->demux_fds[1]);
						ds_free_packs(ds);
						dvb_set_tsout_filt(priv->demux_fds[1], sh_a->aid, DMX_PES_AUDIO, 0);
						*((int*)arg) = ds->id;
					}
					else
					{
						printf("### In [%s][%d] the same audio pid, don't switch ds->id [%d], ii[%d]-[%d] ###\n", __func__, __LINE__, ds->id, ii, sh_a->aid);
					}
					break;
				}

			}
			break;
		}
	case DEMUXER_CTRL_ADD_AUDIO_STREAM:
		{
			int ii = 0;
			char need_add_new = 1;
			demux_program_t *prog = arg;
			demux_stream_t *ds = demuxer->audio;
			mplayer_debug(" $$$$ In [%s][%d] priv->last_aid is [%d] $$$$\n", __func__, __LINE__, skyts? skyts->lastAudioEnd:-1);
			//mplayer_debug(" #### In [%s][%d] got add audio command aid[%d] aformat[%d]####\n", __func__, __LINE__, prog->aid, prog->aformat);
			if(skyts)
			{
				for(ii = 0; ii <= skyts->lastAudioEnd; ii++)
				{
					sh_audio_t* sh_a = NULL;
					ds->sh = demuxer->a_streams[ii];
					sh_a = (sh_audio_t*)ds->sh;
					if (sh_a->aid == prog->aid)
					{
						printf("== in [%s][%d] find duplicate audio stream [%d] ==\n", __func__, __LINE__, sh_a->aid);
						need_add_new = 0;
						break;
					}
					else
						printf("== in [%s][%d] original audio stream[%d] = [%d], new aid is [%d] ==\n", __func__, __LINE__, ii, sh_a->aid, prog->aid);
				}
				if(need_add_new)
				{
					sh_audio_t *sh = NULL;
					sh =  new_sh_audio_aid(demuxer, skyts->lastAudioEnd+1, prog->aid);
					skyts->lastAudioEnd++;
					if (sh)
					{
						ds = demuxer->audio;
						if (!ds->sh)
						{
							sh_audio_t* sh_a;
							ds->sh = demuxer->a_streams[skyts->lastAudioEnd];
							sh_a = (sh_audio_t*)ds->sh;
							sh_a->format = check_format(prog->aformat, TYPE_AUDIO);
							if (!sh_a->format)
								sh_a->format = AUDIO_MP2;
							printf("==== set audio format to [%.4x]-[%s][%d] skyts->lastAudioEnd[%d]===\n", sh_a->format, __func__, __LINE__, skyts->lastAudioEnd);
						}
						else
						{
							sh_audio_t* sh_a;
							ds->sh = demuxer->a_streams[skyts->lastAudioEnd];
							sh_a = (sh_audio_t*)ds->sh;
							sh_a->format = check_format(prog->aformat, TYPE_AUDIO);
							if (!sh_a->format)
								sh_a->format = AUDIO_MP2;
							printf("==== set audio format to [%.4x]-[%s][%d] skyts->lastAudioEnd[%d]===\n", sh_a->format, __func__, __LINE__, skyts->lastAudioEnd);
						}

					}
					else
					{
						printf("@@@@ In [%s][%d] alloc new_sh_audio_aid failed @@@@\n", __func__, __LINE__);
					}
				}
			}
			else
				printf("==== Don't have skyts in [%s][%d] ====\n", __func__, __LINE__);
			
			break;
		}
    default:
        return DEMUXER_CTRL_NOTIMPL;
    }
	return DEMUXER_CTRL_OK;
}


static void demux_close_skyts( demuxer_t *demux )
{
    SkytsInfo         *skyts = demux->priv;
#ifdef DVBT_USING_NORMAL_METHOD
	printf("@@@@@ We in [%s]-[%s]-[%d] call stop_skydvb_thread@@@@@@\n", __FILE__, __func__, __LINE__);
	stop_skydvb_thread();
	dvb_debug_printf("@@@@@ after stop_skydvb_thread @@@@\n");

	dvb_debug_printf("@@@@ lock [%s][%d]@@@@\n", __func__, __LINE__);
	sky_set_display_lock(1);
	dvb_debug_printf("@@@@ In [%s][%d] we flush not play video queue @@@\n", __func__, __LINE__);
	demux_flush(demux);
	dvb_debug_printf("@@@@ dvb_debug_printf In [%s][%d] we svsd_ipc_drop_video_cnts @@@\n", __func__, __LINE__);
	svsd_ipc_drop_video_cnts();
	dvb_debug_printf("@@@@ In [%s][%d], we drop svsd queue @@@@\n", __func__, __LINE__);
	sky_set_reset_display_queue(0);
#endif // end of DVBT_USING_NORMAL_METHOD
	do_skyts_fill_first = 1; // reinitial do_skyts_fill_first for slave mode to reload conf_file
    free( skyts );
	demux->priv = NULL;
    sub_justify = 0;
}


static int skyts_check_file(demuxer_t* demuxer)
{
    SkytsInfo *skyts = calloc(1, sizeof(SkytsInfo));
    static int orig_video_id = -3;
    static int orig_audio_id = -3;
    int ret = 0;
    demuxer->priv = skyts;

    if (orig_video_id == -3)
    {
        orig_video_id = video_id;
    }
    if (orig_audio_id == -3)
    {
        orig_audio_id = audio_id;
    }

    if (dvb_get_pids(demuxer->stream, 0) == 0)
    {
        video_id = -2;
        demuxer->video->id = -2;
    }
    else
    {
        video_id = orig_video_id;
        demuxer->video->id = orig_video_id;
    }

    if (dvb_get_pids(demuxer->stream, 1) == 0)
    {
        audio_id = -2;
        demuxer->audio->id = -2;
    }
    else
    {
        audio_id = orig_audio_id;
        demuxer->audio->id = orig_audio_id;
    }

    if (video_id != -2)
    {
//        return ds_fill_buffer(demuxer->video) ? DEMUXER_TYPE_SKYMPEG_TS : 0;
        ret = ds_fill_buffer(demuxer->video);
        if (ret)
        {
            return DEMUXER_TYPE_SKYMPEG_TS;
        }
        else if (audio_id != -2 && (get_skydvb_wrokaround() == READY_TO_DISPLAY)) 
		/* carlos fixed double set filter when can't get video or audio frame. Example : wrong conf_file , 2010-12-29*/
        {
            return ds_fill_buffer(demuxer->audio) ? DEMUXER_TYPE_SKYMPEG_TS : 0;
        }
        else
            return 0;
    }
    else if (video_id == -2 && audio_id != -2)
    {
        return DEMUXER_TYPE_SKYMPEG_TS;
    }
    else
        return 0;
}


static demuxer_t* demux_open_skyts(demuxer_t* demuxer)
{
    sh_audio_t *sh_audio=NULL;
    sh_video_t *sh_video=NULL;
    printf("$$$$=== %s: \n", __FUNCTION__);
    if (video_id!=-2) {
		if (demuxer->video)
		{
			if (demuxer->video->sh == NULL)
				new_sh_video(demuxer, 0);
			sh_video=demuxer->video->sh;
			if (sh_video)
				sh_video->ds=demuxer->video;
			else
				printf("#### %s-%d demuxer->video->sh is NULL ####\n", __func__, __LINE__);
		}
    }
//    if (demuxer->audio->id!=-2) {
    if (audio_id!=-2) {
        if (!ds_fill_buffer(demuxer->audio)) {
            mp_msg(MSGT_DEMUXER,MSGL_INFO,"MPEG: " MSGTR_MissingAudioStream);
            demuxer->audio->sh=NULL;
            printf("=== %s: af ds_fill_buffer, MissingAudioStream\n", __FUNCTION__);
        } else {
			if (demuxer->audio && demuxer->audio->sh == NULL)
				new_sh_audio(demuxer, 0);
			sh_audio=demuxer->audio->sh;
			if (sh_audio)
			{
				sh_audio->ds=demuxer->audio;
				printf("=== %s: af ds_fill_buffer, Got Audio\n", __FUNCTION__);
			}
			else
				printf("#### %s-%d demuxer->audio->sh is NULL ####\n", __func__, __LINE__);
        }
    }
	else /* Fixed DVB-T using nosound caused change channel hang issue, by carlos 2010-09-27 */
	{
		if (!ds_fill_buffer(demuxer->video))
			printf("### failed to got video in [%s][%d] ###\n", __func__, __LINE__);
		else
			printf("### Got Video### in [%s][%d] ###\n", __func__, __LINE__);
	}
    printf("$$$$=== %s: return!\n", __FUNCTION__);
    return demuxer;
}


demuxer_desc_t demuxer_desc_skyts = {
    "SkyTS demuxer",
    "skyts",
    "STS",
    "Robert",
    "Demux streams from SkyviiaTS",
    DEMUXER_TYPE_SKYMPEG_TS,
    0, // unsafe autodetect
    skyts_check_file,
    demux_skyts_fill_buffer,
    demux_open_skyts,
    demux_close_skyts,
    demux_seek_skyts,
    demux_skyts_control
};

#ifdef AUTO_CHECK // carlos add 2010-07-22
static int get_video_audio_id(int v_pid, int a_pid, int p_pid, int *v_type, int *a_type, int fd)
{
	struct dmx_sct_filter_params sFP;
	int ret = 0, retval;
	unsigned char *ts_buf = NULL;
	struct timeval tv;
	int m_pid = 0;
	int time_count= 0; // for break this function, we will 
	fd_set rfds;
	memset(&sFP, 0, sizeof(struct dmx_sct_filter_params));
	sFP.pid = 0;
	sFP.filter.filter[0] = 0; /* check PAT tables */
	sFP.filter.mask[0] = 0xff;
	sFP.timeout = 0;
	sFP.flags = DMX_CHECK_CRC | DMX_IMMEDIATE_START;
	ts_buf = calloc(1, BUF_LENGTH);
	if (!ts_buf)
		return ret;
	if (ioctl(fd, DMX_SET_FILTER, &sFP) <0)
		printf("=== ioctl failed : [%s] when pid[%d], filter[%d]==\n", strerror(errno), sFP.pid, sFP.filter.filter[0]);
	else
		debug_printf("== do ioctl DMX_SET_FILTER pid[%d] table_id[%d] success ==\n", sFP.pid, sFP.filter.filter[0]);

	/* First time, we need get the program id */
	do
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		tv.tv_usec = 1000;
		tv.tv_sec = 0;
		retval = select((fd+1), &rfds, NULL, NULL, &tv);
		if (retval < 0)
			printf("Error Happens retval = 0x%08x!!\n", retval);
		else if (retval == 0) 				//Time out
		{
			//printf("Time Out!!\n");
			time_count++;
			if (time_count > 200)
				break;
		}
		else 
		{
			ret = read(fd, ts_buf, BUF_LENGTH);
			if (ret > 0)
			{
#ifdef SEE_FILTER_PACKET
				int ii = 0;
				printf("### read length is [%d]###\n", ret);
				for (ii = 0; ii < ret; ii++)
				{
					if (ii > 0 && (ii%16) == 0)
						printf("\n");
					printf("%.2x ", ts_buf[ii]);
				}
				printf("### read finished ###\n");
#endif 
				if (m_pid == 0)
				{
					m_pid = ts_parse_pat(ts_buf, ret, p_pid);
					if (m_pid > 0)
					{
						if (ioctl(fd, DMX_STOP, NULL) <0)
							printf("=== ioctl failed : [%s] when pid[%d], filter[%d]==\n", strerror(errno), sFP.pid, sFP.filter.filter[0]);
						else
							debug_printf("== do ioctl STOP success ==\n");
						sFP.pid = m_pid;
						sFP.filter.filter[0] = 0x02; ///> We need get the audio and video id , 0x02 mean PMT table_id 
						debug_printf("== will do DMX_SET_FILTER ==\n");
						if (ioctl(fd, DMX_SET_FILTER, &sFP) <0)
							printf("=== ioctl failed : [%s] when pid[%d], filter[%d]==\n", strerror(errno), sFP.pid, sFP.filter.filter[0]);
						else
							debug_printf("== do ioctl DMX_SET_FILTER pid[%d] table_id[%d] success ==\n", sFP.pid, sFP.filter.filter[0]);
					}
					else
					{
						if (m_pid == -1)
						{
							ret = m_pid ; 
							break;
						}
					}
				}
				else
				{
					ret = ts_parse_pmt(ts_buf, ret, a_pid, v_pid, a_type, v_type);
					/* 
					 * if ret > 0 mean we got the audio id or video id 
					 * if ret == 0 mean we don't get audio and video id 
					 */
					break;
				}
			}
			else if (ret < 0)
			{
				printf("@@@@ read failed : [%s]@@@@n", strerror(errno));
				usleep(1000);
			}
			else
				printf("@@@@ no data @@@@\n");
		}
	}while(1);
	if (ioctl(fd, DMX_STOP, NULL) <0)
		printf("=== ioctl failed : [%s] when pid[%d], filter[%d]==\n", strerror(errno), sFP.pid, sFP.filter.filter[0]);
	else
		debug_printf("== do ioctl STOP success ==\n");
	if (ts_buf)
		free(ts_buf);
	return ret;
}

/* return 1 mean ok, 0 mean failed */
static int ts_parse_pmt(unsigned char *ts_buf, int len, int a_pid, int v_pid, int *a_type, int *v_type)
{
	int sub_len = 0;
	int skip_len = 0;
	int ret = 0;
	int found_pid = 0;
	int stream_type = 0;
	unsigned char *ptr = NULL;

	skip_len = (unsigned short)(ts_buf[10]& 0x0f) << 8 | ts_buf[11];
	debug_printf("==== skip_len is [%d][%x]===\n", skip_len, skip_len);
	ptr = ts_buf + 12 + skip_len;
	
	sub_len = len - 12 - skip_len - 4;

	if (sub_len > 0)
	{
		while(sub_len)
		{
			stream_type = ptr[0];
			found_pid = (unsigned short)(ptr[1]&0x1f) << 8 | ptr[2];
			skip_len = (unsigned short)(ptr[3]&0xf) << 8 | ptr[4];
			if (sub_len <= 0)
			{
				printf("==== somthing error , sub_len < 0 [%d], skip_len is [%d]==\n", sub_len, skip_len);
				sub_len = 0;
			}
			else
			{
				debug_printf("##stream_type[%x][%d], found_pid[%x][%d], skip_len[%x][%d]sub_len[%d]\n", stream_type, stream_type, found_pid, found_pid, skip_len, skip_len, sub_len);
				if (found_pid == a_pid)
				{
					*a_type = stream_type;
					debug_printf("=== found it found_pid[%d] , a_pid[%d], type is [%.4x]==\n", found_pid, a_pid, *a_type);
					ret++;
				}
				if (found_pid == v_pid)
				{
					*v_type = stream_type;
					debug_printf("=== found it found_pid[%d] , v_pid[%d], type is [%.4x]==\n", found_pid, v_pid, *v_type);
					ret++;
				}
			}
			sub_len = sub_len - 5 - skip_len ;
			ptr = ptr + 5 + skip_len;
		}
	}
	else
		printf("== We got nothing, sub_len is [%d]==\n", sub_len);
	return ret;
}

static int ts_parse_pat(unsigned char *ts_buf, int len, int p_pid)
{
	int sub_len = 0;
	int count = 0;
	int pid = 0;
	unsigned char *ptr = ts_buf+8;
	int program_num = 0;
	int not_match = 1;

	sub_len = (unsigned short)(ts_buf[1]& 0x0f) << 8 | ts_buf[2];
	count = (len - 8 - 4 ) / 4; // 8 is other descriptor , -4 mean CRC 

	if (count)
	{
		while(count)
		{
			program_num = (unsigned short)(ptr[0]) << 8 | ptr[1];
			pid = (unsigned short)(ptr[2]&0x1f) << 8 | ptr[3];
			debug_printf("=== program_num is [%d][%x], pid [%d][%x] , we want found [%d]== \n", program_num, program_num, pid, pid, p_pid);
			if (program_num == p_pid)
			{
				not_match = 0;
				break;
			}
			count--;
			ptr+=4;
		}
	}
	else
		printf("== We got nothing, count is 0==\n");

	if (not_match)
		pid = -1;

	return pid;
}

/* define in T-REC-H.222.0-200605-IPDF-E.pdf */
unsigned int check_format(int type, int stream_type)
{
	unsigned int format_type = 0;
	if (type == 0x02) // Video, MPEG2
		format_type = VIDEO_MPEG2;
	else if (type == 0x03 || type == 0x04) // Audio, MP3
		format_type = AUDIO_MP2; 
	else if (type == 0x0f || type == 0x11) // Audio, AAC
		format_type = AUDIO_AAC;
	else if (type == 0x1b) // Video, H264
		format_type = VIDEO_H264;
	else if (type == 0x01) // Video, MPEG-1
		format_type = VIDEO_MPEG1;
	else if (type == 0x10) // Video, MPEG-4 
		format_type = VIDEO_MPEG4;
	else if (type == 0x81 || (type == 0x06 && stream_type == TYPE_AUDIO)) // Audio, A52
		format_type = AUDIO_A52;
	else if (type == 0x01) // Video MPEG1
		format_type = VIDEO_MPEG1;
	else if (type == 0x8A || type == 0x82 || type == 0x85 || type == 0x86) // Audio, DTS
		format_type = AUDIO_DTS;
#ifdef DVB_SUBTITLES		
	else if ((type == 0x06) && (stream_type == TYPE_SUB))
		format_type = SPU_TELETEXT;
	else if (type == 0x90)
		format_type = SPU_PGS;
	else if (type == 0x07)
		format_type = SPU_DVB;
#endif // end of DVB_SUBTITLES		
	else if (type == 0xEA)
		format_type = VIDEO_VC1;
	else
		format_type = 0;
	debug_printf("@@@ Carlos in [%s][%d], check_type is [%d] , format  setup to [%.8x]@@@\n", __func__, __LINE__, type, format_type);
	return format_type;
}

#endif // end of AUTO_CHECK
#ifdef DVBT_USING_NORMAL_METHOD
extern void sky_set_display_lock(int lock_stat);
//extern int dvb_fill_ts_buffer(demuxer_t *demux, ts_priv_t *ts_priv,  demux_stream_t *ds);
//#define SUPPORT_DVB_T_PAUSE_FLUSH
extern void sky_set_display_drop_cnt(int drop_cnt);
static int skyviia_read_stream(demuxer_t *demux)
{
	int ret = 0;
    dvb_priv_t *priv  = (dvb_priv_t *) demux->stream->priv;
	ts_priv_t ts_priv;
	extern int h264_frame_mbs_only;
	int i = 0;
	int check_ret = 0;
	int lost_conn = 0;
	extern int sky_vdec_vdref_num;
#ifdef SUPPORT_DVB_T_PAUSE_FLUSH		
	extern int is_pause;
	unsigned char flush_pause = 0;
#endif // end of SUPPORT_DVB_T_PAUSE_FLUSH	

	printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
	memset(&ts_priv, 0, sizeof(ts_priv_t));

	ts_priv.fifo[0].pfd = priv->demux_fds[1]; // Audio
	ts_priv.fifo[1].pfd = priv->demux_fds[0]; // Video
	ts_priv.fifo[2].pfd = priv->demux_fds[2]; // Subtitle
	ts_priv.last_pid = priv->fe_fd; // using this parameter for recording front dev_node;
	dvb_debug_printf("@@@@ We set ts_priv.last_pid to [%d] priv->fe_fd [%d]\n", ts_priv.last_pid, priv->fe_fd);
	ts_priv.prog = 0;
	skydvb_thread_running = 1;

	while(skydvb_thread_running != -1 )
	{
#ifdef SUPPORT_DVB_T_PAUSE_FLUSH		
		while(is_pause)
		{
			if (!flush_pause)
			{
				flush_pause = 1;
				/* Pause Audio and Video , TODO :  need handle stop and die status */
				#if 0
				if (ioctl(ts_priv.fifo[0].pfd, DMX_PAUSE_FILTER) < 0)
					printf("Audio: ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[0].pfd, errno);
				if (ioctl(ts_priv.fifo[1].pfd, DMX_PAUSE_FILTER) < 0)
					printf("Video: ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[1].pfd, errno);
				printf("@@@@ Carlos in [%s][%d] pause filter ---audio and video ----@@@@\n", __func__, __LINE__);
				#else
				if (ioctl(ts_priv.fifo[0].pfd, DMX_STOP) < 0)
					printf("Audio: ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[0].pfd, errno);
				if (ioctl(ts_priv.fifo[1].pfd, DMX_STOP) < 0)
					printf("Video: ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[1].pfd, errno);
				printf("@@@@ Carlos in [%s][%d] stop filter ---audio and video ----@@@@\n", __func__, __LINE__);
				#endif
				#if 0
				if (ioctl(ts_priv.fifo[2].pfd, DMX_PAUSE_FILTER) < 0)
					printf("Subtitle: ERROR IN SETTING DMX_PAUSE_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[2].pfd, errno);
				#endif
				svsd_ipc_drop_video_cnts();
				//set_skydvb_wrokaround(NEED_SEEK_CHANNEL);
			}
			usleep(100000);
		}
		if (flush_pause)
		{
			dvb_channel_t *channels = &(priv->list->channels[priv->list->current]);
			flush_pause = 0;
			#if 0
			/* TODO, Resume audio and Video */
			if (ioctl(ts_priv.fifo[0].pfd, DMX_RESUME_FILTER) < 0)
				printf("Audio: ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[0].pfd, errno);
			if (ioctl(ts_priv.fifo[1].pfd, DMX_RESUME_FILTER) < 0)
				printf("Video: ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[1].pfd, errno);
			printf("@@@@ Carlos in [%s][%d] Resume filter ---audio and video ----@@@@\n", __func__, __LINE__);
			#else 
			dvb_set_tsout_filt(ts_priv.fifo[0].pfd, channels->pids[1], DMX_PES_AUDIO, 0);
			dvb_set_tsout_filt(ts_priv.fifo[1].pfd, channels->pids[0], DMX_PES_VIDEO, channels->pids_format[0]);
			printf("@@@@ Carlos in [%s][%d] tsout_filt filter ---audio and video ----@@@@\n", __func__, __LINE__);

			#endif
			#if 0
			if (ioctl(ts_priv.fifo[2].pfd, DMX_RESUME_FILTER) < 0)
				printf("Subtitle: ERROR IN SETTING DMX_RESUME_FILTER for fd %d: ERRNO: %d", ts_priv.fifo[2].pfd, errno);
			#endif
		}
#endif // end of SUPPORT_DVB_T_PAUSE_FLUSH		
		check_ret = dvb_fill_ts_buffer(demux, &ts_priv, demux->video);
		if (skydvb_thread_running == -1 || exit_skydvb_thread)
		{
			printf("\n !!! skydvb_thread_running = %d, need to  quit !!!\n", skydvb_thread_running);
			ts_priv.prog = -2;
			break;
		}
		//check_ret = dvb_fill_ts_buffer(demux, &ts_priv, demux->audio); // alway fill audio and video
		check_ret = dvb_fill_ts_buffer(demux, &ts_priv, demux->video);
		if (check_ret)
		{
			if (lost_conn)
			{
				/* Now, we get the singal again */
				lost_conn = 0;
				dvb_debug_printf("@@@@ unlock @@@@\n");
				if (!h264_frame_mbs_only)
				{
					set_skydvb_wrokaround(NEED_SEEK_CHANNEL);
					dvb_debug_printf("@@@@ In [%s][%d], we restart video driver set skydvb_need_workaround to 1@@@\n", __func__, __LINE__);
				}
				sky_set_display_lock(0);
			}
		}
		else
		{
			/* Lost connection */
			if (!lost_conn)
			{
				lost_conn = 1; 
				/* first time */
				dvb_debug_printf("@@@@ lock [%s][%d]@@@@\n", __func__, __LINE__);
				sky_set_display_lock(1);
				dvb_debug_printf("@@@@ In [%s][%d] we flush not play video queue @@@\n", __func__, __LINE__);
				/* We move this function call to main function, avoid free data and play data at the same time */
				//demux_flush(demux); 
				dvb_debug_printf("@@@@ dvb_debug_printf In [%s][%d] we svsd_ipc_drop_video_cnts @@@\n", __func__, __LINE__);
				svsd_ipc_drop_video_cnts();
				dvb_debug_printf("@@@@ In [%s][%d], we drop svsd queue @@@@\n", __func__, __LINE__);
				#if 1 // carlos change handle lost_connection method for On2
					sky_set_reset_display_queue(0);
				#else // if 1
				if (!h264_frame_mbs_only)
				{
					printf("@@@@ In [%s][%d], we restart video driver@@@@\n", __func__, __LINE__);
					Vd_Reinit(NULL);
				}
				else
				{
					sky_set_reset_display_queue();
					printf("@@@@ In [%s][%d], after sky_set_reset_display_queue@@@@\n", __func__, __LINE__);
				}
				#endif // end of 1
			}
		}
	}
    skydvb_thread_running = -2;
	exit_skydvb_thread = 0;
	printf("########## We exit [%s]########\n", __func__);

    return 0;
}

void stop_skydvb_thread(void)
{
	if (skydvb_thread_running == 1)
	{
		skydvb_thread_running = -1;
		dvb_debug_printf("\n=========== try to join prev thread and then kill it ====[%s][%d]\n", __func__, __LINE__);
		dvb_debug_printf("@@ Carlos in [%s][%d] stop carlos_read_stream @@\n", __func__, __LINE__);
		while(skydvb_thread_running != -2)
			usleep(1);
		dvb_debug_printf("#### before pthread_join , the skydvb_read_thread is [%d]  in [%s][%d]####\n", skydvb_read_thread, __func__, __LINE__);
		if	(pthread_join(skydvb_read_thread, 0) != 0)
			printf("=== pthread_join failed, reason is [%s] ==\n", strerror(errno));

		printf("\n=========== thread clear ======\n");
		/* Next time, we can recreate pthreawd */
		//do_skyts_fill_first = 1;
	}
	else
	{
		dvb_debug_printf("#### Carlos in [%s][%d], skydvb_thread_running is [%d], don't execute stop skydvb_thread command ####\n", __func__, __LINE__, skydvb_read_thread);
		if (skydvb_read_thread)
		{
			if	(pthread_join(skydvb_read_thread, 0) != 0)
				printf("=== pthread_join failed, reason is [%s] in [%s][%d] ==\n", strerror(errno), __func__, __LINE__);
		}
	}
	skydvb_read_thread = 0;
	skydvb_thread_running = 0;
}
#endif // end of DVBT_USING_NORMAL_METHOD


int get_skydvb_wrokaround(void)
{
	int ret = 0;
	pthread_mutex_lock(&dvb_st_mtx);
	ret = skydvb_need_workaround;
	pthread_mutex_unlock(&dvb_st_mtx);
	return ret;
}

void set_skydvb_wrokaround(int value)
{
	pthread_mutex_lock(&dvb_st_mtx);
	skydvb_need_workaround = value ;
	pthread_mutex_unlock(&dvb_st_mtx);
}

#endif /* end of QT_SUPPORT_DVBT */

//inbuf: input data address
//outbuf: output data address
//in_len: input samples length in words
//mode: 0 => not change, 1 => increase samples, -1 => decrease samples
//return value : output samples length in words
int sample_resize_2ch(short *inbuf, short *outbuf, int in_len, int mode)
{
   unsigned	long step = mode_step[mode+1];
   int len = 0;
   unsigned	long mask = (1<<STEPACCURACY)-1;
   unsigned	long frac, tmp;
   unsigned	long end = (in_len>>1)<<STEPACCURACY;

   while(phad < (1<<STEPACCURACY)){
      frac = phad & mask;
      *outbuf++ = (short)((((inbuf[0] - presample[0])*(int)(frac))>>STEPACCURACY) + presample[0]);
      *outbuf++ = (short)((((inbuf[1] - presample[1])*(int)(frac))>>STEPACCURACY) + presample[1]);
      len += 2;
      phad += step;
   }
   while(phad < end){
      tmp = phad>>STEPACCURACY;
      frac = phad & mask;
      *outbuf++  = (short)((((inbuf[tmp*2] - inbuf[(tmp-1)*2])*(int)(frac))>>STEPACCURACY) + inbuf[(tmp-1)*2]);  
      *outbuf++  = (short)((((inbuf[tmp*2+1] - inbuf[(tmp-1)*2+1])*(int)(frac))>>STEPACCURACY) + inbuf[(tmp-1)*2+1]);
      len += 2;
      phad+=step;
   }
   presample[0] = inbuf[in_len-2];
   presample[1] = inbuf[in_len-1];
   phad &= mask;
   return len;
}

