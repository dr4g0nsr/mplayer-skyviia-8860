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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"

#include "codec-cfg.h"
#include "libmpdemux/stheader.h"

#include "dec_audio.h"
#include "ad.h"
#include "libaf/af_format.h"

#include "libaf/af.h"

#ifdef CONFIG_DYNAMIC_PLUGINS
#include <dlfcn.h>
#endif

/* We need check audio decoder when playing DVB-T, 2010-10-13 */
/* Carlos change MAX_TRY_AUDIO_DECODE_COUNT from 100 to 50 */
#define MAX_TRY_AUDIO_DECODE_COUNT	50
//#define MAX_TRY_AUDIO_DECODE_COUNT	100
#ifdef DVBT_USING_NORMAL_METHOD
#include "libmpdemux/demux_ts.h"
#endif // end of DVBT_USING_NORMAL_METHOD	

#ifdef CONFIG_FAKE_MONO
int fakemono = 0;
#endif


//////////////////////////////
                                            //
#define  jfueng_2011_0311     //
                                            //
/////////////////////////////

/* used for ac3surround decoder - set using -channels option */
int audio_output_channels = 2;
af_cfg_t af_cfg = { 1, NULL };	// Configuration for audio filters
unsigned int sky_hwac3 = 0;	//Barry 2010-10-09
extern int ad_real_channels;	//Fuchun 2011.05.17
int as_channels = 0;

void afm_help(void)
{
    int i;
    mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_AvailableAudioFm);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AUDIO_DRIVERS\n");
    mp_msg(MSGT_DECAUDIO, MSGL_INFO, "    afm:    info:  (comment)\n");
    for (i = 0; mpcodecs_ad_drivers[i] != NULL; i++)
	if (mpcodecs_ad_drivers[i]->info->comment
	    && mpcodecs_ad_drivers[i]->info->comment[0])
	    mp_msg(MSGT_DECAUDIO, MSGL_INFO, "%9s  %s (%s)\n",
		   mpcodecs_ad_drivers[i]->info->short_name,
		   mpcodecs_ad_drivers[i]->info->name,
		   mpcodecs_ad_drivers[i]->info->comment);
	else
	    mp_msg(MSGT_DECAUDIO, MSGL_INFO, "%9s  %s\n",
		   mpcodecs_ad_drivers[i]->info->short_name,
		   mpcodecs_ad_drivers[i]->info->name);
}

static int init_audio_codec(sh_audio_t *sh_audio)
{
    if ((af_cfg.force & AF_INIT_FORMAT_MASK) == AF_INIT_FLOAT) {
	int fmt = AF_FORMAT_FLOAT_NE;
	if (sh_audio->ad_driver->control(sh_audio, ADCTRL_QUERY_FORMAT,
					 &fmt) == CONTROL_TRUE) {
	    sh_audio->sample_format = fmt;
	    sh_audio->samplesize = 4;
	}
    }
    if (!sh_audio->ad_driver->preinit(sh_audio)) {
	mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_ADecoderPreinitFailed);
	return 0;
    }

    /* allocate audio in buffer: */
    if (sh_audio->audio_in_minsize > 0) {
	sh_audio->a_in_buffer_size = sh_audio->audio_in_minsize;
	mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_AllocatingBytesForInputBuffer,
	       sh_audio->a_in_buffer_size);
	sh_audio->a_in_buffer = av_mallocz(sh_audio->a_in_buffer_size);
	sh_audio->a_in_buffer_len = 0;
    }

    sh_audio->a_buffer_size = sh_audio->audio_out_minsize + MAX_OUTBURST;

    mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_AllocatingBytesForOutputBuffer,
	   sh_audio->audio_out_minsize, MAX_OUTBURST, sh_audio->a_buffer_size);

    sh_audio->a_buffer = av_mallocz(sh_audio->a_buffer_size);
    if (!sh_audio->a_buffer) {
	mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_CantAllocAudioBuf);
	return 0;
    }

    sh_audio->a_buffer_len = 0;

    //if (!sh_audio->ad_driver->init(sh_audio)) {
    if (sh_audio->ad_driver->init(sh_audio) <= 0) {	//Barry 2010-12-29 audio init fail
       //Polun 2011-08-12 + modify fist audio decode init fail retry again only for ac3 .
       if(sh_audio->format  == 0x2000 )
       {
            //Polun 2011-06-28 ++s if fist audio decode init fail retry one time.
            if(sh_audio->a_buffer_len != 0)
            {
                sh_audio->a_buffer_len = 0;
            }
            printf("fist audio decode init fail retry again\n");
            if (sh_audio->ad_driver->init(sh_audio) <= 0) {
	            mp_msg(MSGT_DECAUDIO, MSGL_WARN, MSGTR_ADecoderInitFailed);
	            uninit_audio(sh_audio);	// free buffers
	            return 0;
            }
            //Polun 2011-06-28 ++e
        }
       else
        {
	    mp_msg(MSGT_DECAUDIO, MSGL_WARN, MSGTR_ADecoderInitFailed);
	    uninit_audio(sh_audio);	// free buffers
	    return 0;
       }
    }

    sh_audio->initialized = 1;

    if (!sh_audio->channels || !sh_audio->samplerate) {
	mp_msg(MSGT_DECAUDIO, MSGL_WARN, MSGTR_UnknownAudio);
	uninit_audio(sh_audio);	// free buffers
	return 0;
    }

    if (!sh_audio->o_bps)
	sh_audio->o_bps = sh_audio->channels * sh_audio->samplerate
	                  * sh_audio->samplesize;

    mp_msg(MSGT_DECAUDIO, MSGL_INFO,
	   "AUDIO: %d Hz, %d ch, %s, %3.1f kbit/%3.2f%% (ratio: %d->%d)\n",
	   sh_audio->samplerate, sh_audio->channels,
	   af_fmt2str_short(sh_audio->sample_format),
	   sh_audio->i_bps * 8 * 0.001,
	   ((float) sh_audio->i_bps / sh_audio->o_bps) * 100.0,
	   sh_audio->i_bps, sh_audio->o_bps);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO,
	   "ID_AUDIO_BITRATE=%d\nID_AUDIO_RATE=%d\n" "ID_AUDIO_NCH=%d\n",
	   sh_audio->i_bps * 8, sh_audio->samplerate, sh_audio->channels);

    sh_audio->a_out_buffer_size = 0;
    sh_audio->a_out_buffer = NULL;
    sh_audio->a_out_buffer_len = 0;

#ifdef NEW_AUDIO_FILL_BUFFER
	sh_audio->w_a_out_buffer_len = 0;
	sh_audio->r_a_out_buffer_len = 0;
#endif

    return 1;
}

static int init_audio(sh_audio_t *sh_audio, char *codecname, char *afm,
		      int status, stringset_t *selected)
{
    unsigned int orig_fourcc = sh_audio->wf ? sh_audio->wf->wFormatTag : 0;
    int force = 0;
    unsigned int try_again = 1;	//Fuchun 2010.10.13 if init fail, try init once again for ts.
#if 0
/* Carlos add 2010-12-29, We need make sure have audio packet before initial audio_decoder, move this condition in mplayer.c */
#ifdef DVBT_USING_NORMAL_METHOD
	unsigned char do_retry = 0; /* Carlos add for check do_retry, 2010-12-27 */
#endif /* end of DVBT_USING_NORMAL_METHOD */
#endif /* end of 0 */
    if (codecname && codecname[0] == '+') {
	codecname = &codecname[1];
	force = 1;
    }
    sh_audio->codec = NULL;
    while (1) {
	ad_functions_t *mpadec;
	int i;
	sh_audio->ad_driver = 0;
	// restore original fourcc:
	if (sh_audio->wf)
	    sh_audio->wf->wFormatTag = i = orig_fourcc;
	if (!(sh_audio->codec = find_audio_codec(sh_audio->format,
						 sh_audio->wf ? (&i) : NULL,
						 sh_audio->codec, force)))
	    break;

	//Barry 2010-10-09
	if (sh_audio->format == 0x2000 || sh_audio->format == 0x2001)
	{
		if (sky_hwac3)
		{
			if (strcmp("hwac3", sh_audio->codec->drv))
				continue;
		}
		else
		{
			if (!strcmp("hwac3", sh_audio->codec->drv))
				continue;
		}
	}
	
	if (sh_audio->wf)
	    sh_audio->wf->wFormatTag = i;
	// ok we found one codec
	if (stringset_test(selected, sh_audio->codec->name))
	    continue;	// already tried & failed
	if (codecname && strcmp(sh_audio->codec->name, codecname))
	    continue;	// -ac
	if (afm && strcmp(sh_audio->codec->drv, afm))
	    continue;	// afm doesn't match
	if (!force && sh_audio->codec->status < status)
	    continue;	// too unstable
	stringset_add(selected, sh_audio->codec->name);	// tagging it
#if 0	
#ifdef DVBT_USING_NORMAL_METHOD
retry_dvbt_audio:
#endif // end of DVBT_USING_NORMAL_METHOD	
#endif /* end of 0 */
	// ok, it matches all rules, let's find the driver!
	for (i = 0; mpcodecs_ad_drivers[i] != NULL; i++)
	    if (!strcmp(mpcodecs_ad_drivers[i]->info->short_name,
		 sh_audio->codec->drv))
		break;
	mpadec = mpcodecs_ad_drivers[i];
#ifdef CONFIG_DYNAMIC_PLUGINS
	if (!mpadec) {
	    /* try to open shared decoder plugin */
	    int buf_len;
	    char *buf;
	    ad_functions_t *funcs_sym;
	    ad_info_t *info_sym;

	    buf_len =
		strlen(MPLAYER_LIBDIR) + strlen(sh_audio->codec->drv) + 16;
	    buf = malloc(buf_len);
	    if (!buf)
		break;
	    snprintf(buf, buf_len, "%s/mplayer/ad_%s.so", MPLAYER_LIBDIR,
		     sh_audio->codec->drv);
	    mp_msg(MSGT_DECAUDIO, MSGL_DBG2,
		   "Trying to open external plugin: %s\n", buf);
	    sh_audio->dec_handle = dlopen(buf, RTLD_LAZY);
	    if (!sh_audio->dec_handle)
		break;
	    snprintf(buf, buf_len, "mpcodecs_ad_%s", sh_audio->codec->drv);
	    funcs_sym = dlsym(sh_audio->dec_handle, buf);
	    if (!funcs_sym || !funcs_sym->info || !funcs_sym->preinit
		|| !funcs_sym->init || !funcs_sym->uninit
		|| !funcs_sym->control || !funcs_sym->decode_audio)
		break;
	    info_sym = funcs_sym->info;
	    if (strcmp(info_sym->short_name, sh_audio->codec->drv))
		break;
	    free(buf);
	    mpadec = funcs_sym;
	    mp_msg(MSGT_DECAUDIO, MSGL_V,
		   "Using external decoder plugin (%s/mplayer/ad_%s.so)!\n",
		   MPLAYER_LIBDIR, sh_audio->codec->drv);
	}
#endif
	if (!mpadec) {		// driver not available (==compiled in)
	    mp_msg(MSGT_DECAUDIO, MSGL_ERR,
		   MSGTR_AudioCodecFamilyNotAvailableStr,
		   sh_audio->codec->name, sh_audio->codec->drv);
	    continue;
	}
	// it's available, let's try to init!
	// init()
	mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_OpeningAudioDecoder,
	       mpadec->info->short_name, mpadec->info->name);
	sh_audio->ad_driver = mpadec;
	if (!init_audio_codec(sh_audio)) {
	    mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_ADecoderInitFailed);

#if 0	//Fuchun 2010.12.06 disable - by carlos
		if(try_again && sh_audio->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS)		//Fuchun 2010.10.13 try change format and initial again
		{
			if(sh_audio->format == 0x2000)
			{
				try_again = 0;
				sh_audio->format = 0x2001;
			}
			else if(sh_audio->format == 0x2001)
			{
				try_again = 0;
				sh_audio->format = 0x2000;
			}
		}
#endif

#if 0
/* Carlos add 2010-12-29, We need make sure have audio packet before initial audio_decoder, move this condition in mplayer.c */
#ifdef DVBT_USING_NORMAL_METHOD
		if (!(get_skydvb_wrokaround()& LOST_DVBT_SIGNAL))
		{
			if((try_again < MAX_TRY_AUDIO_DECODE_COUNT)&& sh_audio->ds->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)		//Fuchun 2010.10.13 try change format and initial again
			{
					if (try_again && (try_again % 10) == 0 )
						printf("### We in [%s][%d], do retry audio decoder again -- try_again[%d]###\n", __func__, __LINE__, try_again);
					usleep(1000);
					try_again++;
					goto retry_dvbt_audio;
			}
		}
#endif // end of DVBT_USING_NORMAL_METHOD		
#endif // end of 0
	    continue;		// try next...
	}
	// Yeah! We got it!
	return 1;
    }
    return 0;
}

int init_best_audio_codec(sh_audio_t *sh_audio, char **audio_codec_list,
			  char **audio_fm_list)
{
    stringset_t selected;
    char *ac_l_default[2] = { "", (char *) NULL };
    ad_real_channels = 0;	//Fuchun 2011.05.17
    as_channels = 0;	//Fuchun 2011.05.18
    // hack:
    if (!audio_codec_list)
	audio_codec_list = ac_l_default;
    // Go through the codec.conf and find the best codec...
    sh_audio->initialized = 0;
    stringset_init(&selected);
    while (!sh_audio->initialized && *audio_codec_list) {
	char *audio_codec = *(audio_codec_list++);
	if (audio_codec[0]) {
	    if (audio_codec[0] == '-') {
		// disable this codec:
		stringset_add(&selected, audio_codec + 1);
	    } else {
		// forced codec by name:
		mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_ForcedAudioCodec,
		       audio_codec);
		init_audio(sh_audio, audio_codec, NULL, -1, &selected);
	    }
	} else {
	    int status;
	    // try in stability order: UNTESTED, WORKING, BUGGY.
	    // never try CRASHING.
	    if (audio_fm_list) {
		char **fmlist = audio_fm_list;
		// try first the preferred codec families:
		while (!sh_audio->initialized && *fmlist) {
		    char *audio_fm = *(fmlist++);
		    mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_TryForceAudioFmtStr,
			   audio_fm);
		    for (status = CODECS_STATUS__MAX;
			 status >= CODECS_STATUS__MIN; --status)
			if (init_audio(sh_audio, NULL, audio_fm, status, &selected))
			    break;
		}
	    }
	    if (!sh_audio->initialized)
		for (status = CODECS_STATUS__MAX; status >= CODECS_STATUS__MIN;
		     --status)
		    if (init_audio(sh_audio, NULL, NULL, status, &selected))
			break;
	}
    }
    stringset_free(&selected);

    //Barry 2011-07-04
    if (sh_audio->format == 0x4D435042 && sh_audio->channels > 2)
	sh_audio->initialized = 0;

    if (!sh_audio->initialized) {
	mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_CantFindAudioCodec,
	       sh_audio->format);
	return 0;   // failed
    }

    mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_SelectedAudioCodec,
	   sh_audio->codec->name, sh_audio->codec->drv, sh_audio->codec->info);
    return 1;   // success
}

void uninit_audio(sh_audio_t *sh_audio)
{
    if (sh_audio->afilter) {
	mp_msg(MSGT_DECAUDIO, MSGL_V, "Uninit audio filters...\n");
	af_uninit(sh_audio->afilter);
	free(sh_audio->afilter);
	sh_audio->afilter = NULL;
    }
    if (sh_audio->initialized) {
	mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_UninitAudioStr,
	       sh_audio->codec->drv);
	sh_audio->ad_driver->uninit(sh_audio);
#ifdef CONFIG_DYNAMIC_PLUGINS
	if (sh_audio->dec_handle)
	    dlclose(sh_audio->dec_handle);
#endif
	sh_audio->initialized = 0;
    }
    free(sh_audio->a_out_buffer);
    sh_audio->a_out_buffer = NULL;
    sh_audio->a_out_buffer_size = 0;
    av_freep(&sh_audio->a_buffer);
    av_freep(&sh_audio->a_in_buffer);
}


int init_audio_filters(sh_audio_t *sh_audio, int in_samplerate,
		       int *out_samplerate, int *out_channels, int *out_format)
{
    af_stream_t *afs = sh_audio->afilter;
    if (!afs) {
	afs = malloc(sizeof(af_stream_t));
	memset(afs, 0, sizeof(af_stream_t));
    }
    // input format: same as codec's output format:
    afs->input.rate   = in_samplerate;
    afs->input.nch    = sh_audio->channels;
    afs->input.format = sh_audio->sample_format;
    af_fix_parameters(&(afs->input));

    // output format: same as ao driver's input format (if missing, fallback to input)
    afs->output.rate   = *out_samplerate;
    afs->output.nch    = *out_channels;
    afs->output.format = *out_format;
    af_fix_parameters(&(afs->output));

    // filter config:
    memcpy(&afs->cfg, &af_cfg, sizeof(af_cfg_t));

    mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_BuildingAudioFilterChain,
	   afs->input.rate, afs->input.nch,
	   af_fmt2str_short(afs->input.format), afs->output.rate,
	   afs->output.nch, af_fmt2str_short(afs->output.format));

    // let's autoprobe it!
    if (0 != af_init(afs)) {
	sh_audio->afilter = NULL;
	free(afs);
	return 0;   // failed :(
    }

    *out_samplerate = afs->output.rate;
    *out_channels = afs->output.nch;
    *out_format = afs->output.format;

    sh_audio->a_out_buffer_len = 0;

#ifdef NEW_AUDIO_FILL_BUFFER
	sh_audio->w_a_out_buffer_len = 0;
	sh_audio->r_a_out_buffer_len = 0;
#endif

    // ok!
    sh_audio->afilter = (void *) afs;
    return 1;
}

#ifdef  jfueng_2011_0311
extern int  audio_long_period;
#endif

static int filter_n_bytes(sh_audio_t *sh, int len)
{
#ifdef  jfueng_2011_0311
    int error = 1; 
#else
    int error = 0;
#endif

    // Filter
    af_data_t filter_input = {
	.audio = sh->a_buffer,
	.rate = sh->samplerate,
	.nch = sh->channels,
	.format = sh->sample_format
    };
    af_data_t *filter_output;

    assert(len-1 + sh->audio_out_minsize <= sh->a_buffer_size);

    // Decode more bytes if needed
    while (sh->a_buffer_len < len) {
	unsigned char *buf = sh->a_buffer + sh->a_buffer_len;
	int minlen = len - sh->a_buffer_len;
	int maxlen = sh->a_buffer_size - sh->a_buffer_len;
	int ret = sh->ad_driver->decode_audio(sh, buf, minlen, maxlen);
	if (ret <= 0) {
#ifdef  jfueng_2011_0311
       if( audio_long_period ==1) 
         {
           len = sh->a_buffer_len;
	    return 0 ;  
	   }
	   else
	   {    
	      error = -1;
	      len = sh->a_buffer_len;
	      break;
	   }
#else		
	    error = -1;
	    len = sh->a_buffer_len;
	    break;
#endif		

	}
	sh->a_buffer_len += ret;
    }

    filter_input.len = len;
    af_fix_parameters(&filter_input);
    filter_output = af_play(sh->afilter, &filter_input);
    if (!filter_output)
	return -1;

#ifdef NEW_AUDIO_FILL_BUFFER
	if (sh->a_out_buffer_size*(AUDIO_OUT_QUEUE_NUM+1) < filter_output->len) {	//after first time, never let a_out_buffer realloc
		static int out_buffer_size = 0;
		//int newlen = filter_output->len;
		int newlen = 0;
		if (out_buffer_size)
		{
			if (out_buffer_size < filter_output->len)
			{
				printf("#### In [%s][%d] we will change sh->a_out_buffer size from [%d] to [%d] ####\n", __func__, __LINE__, out_buffer_size*(AUDIO_OUT_QUEUE_NUM+1), filter_output->len*(AUDIO_OUT_QUEUE_NUM+1));
				out_buffer_size = filter_output->len;
			}
		}
		else
		{
			out_buffer_size = filter_output->len;
			//printf("$$$$ In [%s][%d] first time , we set out_buffer_size to [%d] real size is [%d] $$$$\n", __func__, __LINE__, out_buffer_size*(AUDIO_OUT_QUEUE_NUM+1), out_buffer_size);
		}
		newlen = out_buffer_size;
		mp_msg(MSGT_DECAUDIO, MSGL_V, "Increasing filtered audio buffer size "
			"from %d to %d\n", sh->a_out_buffer_size, newlen*(AUDIO_OUT_QUEUE_NUM+1));
		sh->a_out_buffer = realloc(sh->a_out_buffer, newlen*(AUDIO_OUT_QUEUE_NUM+1));	//avoid overflow, so add 1
		sh->a_out_buffer_size = newlen;
	}
	memcpy(sh->a_out_buffer + sh->w_a_out_buffer_len, filter_output->audio, filter_output->len);
	sh->w_a_out_buffer_len += filter_output->len;
	if(sh->w_a_out_buffer_len >= sh->a_out_buffer_size*AUDIO_OUT_QUEUE_NUM)
	{
		int memmove_len =  sh->w_a_out_buffer_len - sh->a_out_buffer_size*AUDIO_OUT_QUEUE_NUM;
		memmove(sh->a_out_buffer, &sh->a_out_buffer[sh->a_out_buffer_size*AUDIO_OUT_QUEUE_NUM], memmove_len);
		sh->w_a_out_buffer_len = memmove_len;
	}
#else	//else of NEW_AUDIO_FILL_BUFFER
    if (sh->a_out_buffer_size < sh->a_out_buffer_len + filter_output->len) {
	int newlen = sh->a_out_buffer_len + filter_output->len;
	mp_msg(MSGT_DECAUDIO, MSGL_V, "Increasing filtered audio buffer size "
	       "from %d to %d\n", sh->a_out_buffer_size, newlen);
	sh->a_out_buffer = realloc(sh->a_out_buffer, newlen);
	sh->a_out_buffer_size = newlen;
    }
    memcpy(sh->a_out_buffer + sh->a_out_buffer_len, filter_output->audio,
	   filter_output->len);
    sh->a_out_buffer_len += filter_output->len;
#endif	//end of NEW_AUDIO_FILL_BUFFER

    // remove processed data from decoder buffer:
    sh->a_buffer_len -= len;
    memmove(sh->a_buffer, sh->a_buffer + len, sh->a_buffer_len);

    return error;
}

/* Try to get at least minlen decoded+filtered bytes in sh_audio->a_out_buffer
 * (total length including possible existing data).
 * Return 0 on success, -1 on error/EOF (not distinguished).
 * In the former case sh_audio->a_out_buffer_len is always >= minlen
 * on return. In case of EOF/error it might or might not be.
 * Can reallocate sh_audio->a_out_buffer if needed to fit all filter output. */
int decode_audio(sh_audio_t *sh_audio, int minlen)
{
    // Indicates that a filter seems to be buffering large amounts of data
    int huge_filter_buffer = 0;
    // Decoded audio must be cut at boundaries of this many bytes
    int unitsize = sh_audio->channels * sh_audio->samplesize * 16;

    /* Filter output size will be about filter_multiplier times input size.
     * If some filter buffers audio in big blocks this might only hold
     * as average over time. */
    double filter_multiplier = af_calc_filter_multiplier(sh_audio->afilter);

    /* If the decoder set audio_out_minsize then it can do the equivalent of
     * "while (output_len < target_len) output_len += audio_out_minsize;",
     * so we must guarantee there is at least audio_out_minsize-1 bytes
     * more space in the output buffer than the minimum length we try to
     * decode. */
    int max_decode_len = sh_audio->a_buffer_size - sh_audio->audio_out_minsize;
    max_decode_len -= max_decode_len % unitsize;

#ifdef NEW_AUDIO_FILL_BUFFER
	int r_len = sh_audio->r_a_out_buffer_len, total_size = sh_audio->a_out_buffer_size;
	int total_decode_len = sh_audio->w_a_out_buffer_len < r_len ? AUDIO_OUT_QUEUE_NUM*total_size - r_len + sh_audio->w_a_out_buffer_len 
		: sh_audio->w_a_out_buffer_len - r_len;
	while (total_decode_len < minlen) {
		int declen = (minlen - total_decode_len) / filter_multiplier + (unitsize << 5); // some extra for possible filter buffering
		if (huge_filter_buffer)
		/* Some filter must be doing significant buffering if the estimated
		* input length didn't produce enough output from filters.
		* Feed the filters 2k bytes at a time until we have enough output.
		* Very small amounts could make filtering inefficient while large
		* amounts can make MPlayer demux the file unnecessarily far ahead
		* to get audio data and buffer video frames in memory while doing
		* so. However the performance impact of either is probably not too
		* significant as long as the value is not completely insane. */
			declen = 2000;
		declen -= declen % unitsize;
		if (declen > max_decode_len)
			declen = max_decode_len;
		else
		/* if this iteration does not fill buffer, we must have lots
		* of buffering in filters */
			huge_filter_buffer = 1;
#ifdef  jfueng_2011_0311

            int re_val = filter_n_bytes(sh_audio, declen);
            if (re_val == 0)
		   	return 0;
            if (re_val < 0)
		   	return -1;	     

#else
		if (filter_n_bytes(sh_audio, declen) < 0)
			return -1;
#endif		

		total_decode_len = sh_audio->w_a_out_buffer_len < r_len ? AUDIO_OUT_QUEUE_NUM*total_size - r_len + sh_audio->w_a_out_buffer_len
			: sh_audio->w_a_out_buffer_len - r_len;
	}
#else	//else of NEW_AUDIO_FILL_BUFFER
    while (sh_audio->a_out_buffer_len < minlen) {
	int declen = (minlen - sh_audio->a_out_buffer_len) / filter_multiplier
	    + (unitsize << 5); // some extra for possible filter buffering
	if (huge_filter_buffer)
	/* Some filter must be doing significant buffering if the estimated
	 * input length didn't produce enough output from filters.
	 * Feed the filters 2k bytes at a time until we have enough output.
	 * Very small amounts could make filtering inefficient while large
	 * amounts can make MPlayer demux the file unnecessarily far ahead
	 * to get audio data and buffer video frames in memory while doing
	 * so. However the performance impact of either is probably not too
	 * significant as long as the value is not completely insane. */
	    declen = 2000;
	declen -= declen % unitsize;
	if (declen > max_decode_len)
	    declen = max_decode_len;
	else
	    /* if this iteration does not fill buffer, we must have lots
	     * of buffering in filters */
	    huge_filter_buffer = 1;
#ifdef  jfueng_2011_0311
            int re_val = filter_n_bytes(sh_audio, declen);
            if (re_val == 0)
		   	return 0;
            if (re_val < 0)
		   	return -1;	     		
#else
	if (filter_n_bytes(sh_audio, declen) < 0)
	    return -1;
#endif

    }
#endif	//end of NEW_AUDIO_FILL_BUFFER
#ifdef  jfueng_2011_0311
    return 1;
#else
    return 0;
#endif

}

extern float total_audio_out_bytes, total_new_out;
static int filter_n_bytes_with_resamp(sh_audio_t *sh, int len, demuxer_t *p_demux, int mute)
{
    int error = 0;
    int newsize;
    static unsigned char *aud_local_buffer=0;

    // Filter
    af_data_t filter_input = {
	.audio = sh->a_buffer,
	.rate = sh->samplerate,
	.nch = sh->channels,
	.format = sh->sample_format
    };
    af_data_t *filter_output;

    assert(len-1 + sh->audio_out_minsize <= sh->a_buffer_size);

    if (aud_local_buffer == 0)
    {
      aud_local_buffer = malloc(1024*16*2);
    }

    // Decode more bytes if needed
    while (sh->a_buffer_len < len) {
	unsigned char *buf = sh->a_buffer + sh->a_buffer_len;
	int minlen = len - sh->a_buffer_len;
	int maxlen = sh->a_buffer_size - sh->a_buffer_len;
	int buflen = sh->a_buffer_len;
	int ret = sh->ad_driver->decode_audio(sh, buf, minlen, maxlen);
	if (ret <= 0) {
	    error = -1;
	    len = sh->a_buffer_len;
	    break;
	}
#if 0
	sh->a_buffer_len += ret;
#else
            total_audio_out_bytes += ret;
//printf("p_demux->audio->packs=%d  dvb_get_aq_count()=%d\n", p_demux->audio->packs, dvb_get_aq_count());
#ifdef QT_SUPPORT_DVBT
            if ((p_demux->audio->packs+dvb_get_aq_count()) >=2)
#else /* else of QT_SUPPORT_DVBT */
            if (p_demux->audio->packs >=2)
#endif /* end of QT_SUPPORT_DVBT */
            {
                newsize = sample_resize_2ch(&sh->a_buffer[buflen], aud_local_buffer, ret/2, -1);
            }
            else
            {
                newsize = sample_resize_2ch(&sh->a_buffer[buflen], aud_local_buffer, ret/2, 0);
            }
//printf("ret=%d  newsize=%d mute=%d\n", ret, newsize, mute);
            total_new_out += newsize*2;

            memcpy(&sh->a_buffer[buflen], aud_local_buffer, newsize*2);
            sh->a_buffer_len += newsize*2;
#endif
    }

    filter_input.len = len;
    af_fix_parameters(&filter_input);
//    if (mute == 0)
    {
        filter_output = af_play(sh->afilter, &filter_input);

        if (!filter_output)
            return -1;
    }
    if (sh->a_out_buffer_size < sh->a_out_buffer_len + filter_output->len) {
	int newlen = sh->a_out_buffer_len + filter_output->len;
	mp_msg(MSGT_DECAUDIO, MSGL_V, "Increasing filtered audio buffer size "
	       "from %d to %d\n", sh->a_out_buffer_size, newlen);
	sh->a_out_buffer = realloc(sh->a_out_buffer, newlen);
	sh->a_out_buffer_size = newlen;
    }
    memcpy(sh->a_out_buffer + sh->a_out_buffer_len, filter_output->audio,
	   filter_output->len);
    sh->a_out_buffer_len += filter_output->len;

    // remove processed data from decoder buffer:
    sh->a_buffer_len -= len;
    memmove(sh->a_buffer, sh->a_buffer + len, sh->a_buffer_len);

    return error;
}

int decode_audio_with_resamp(sh_audio_t *sh_audio, int minlen, demuxer_t *p_demux, int mute)
{
    // Indicates that a filter seems to be buffering large amounts of data
    int huge_filter_buffer = 0;
    // Decoded audio must be cut at boundaries of this many bytes
    int unitsize = sh_audio->channels * sh_audio->samplesize * 16;

    /* Filter output size will be about filter_multiplier times input size.
     * If some filter buffers audio in big blocks this might only hold
     * as average over time. */
    double filter_multiplier = af_calc_filter_multiplier(sh_audio->afilter);

    /* If the decoder set audio_out_minsize then it can do the equivalent of
     * "while (output_len < target_len) output_len += audio_out_minsize;",
     * so we must guarantee there is at least audio_out_minsize-1 bytes
     * more space in the output buffer than the minimum length we try to
     * decode. */
    int max_decode_len = sh_audio->a_buffer_size - sh_audio->audio_out_minsize;
    max_decode_len -= max_decode_len % unitsize;

    while (sh_audio->a_out_buffer_len < minlen) {
	int declen = (minlen - sh_audio->a_out_buffer_len) / filter_multiplier
	    + (unitsize << 5); // some extra for possible filter buffering
	if (huge_filter_buffer)
	/* Some filter must be doing significant buffering if the estimated
	 * input length didn't produce enough output from filters.
	 * Feed the filters 2k bytes at a time until we have enough output.
	 * Very small amounts could make filtering inefficient while large
	 * amounts can make MPlayer demux the file unnecessarily far ahead
	 * to get audio data and buffer video frames in memory while doing
	 * so. However the performance impact of either is probably not too
	 * significant as long as the value is not completely insane. */
	    declen = 2000;
	declen -= declen % unitsize;
	if (declen > max_decode_len)
	    declen = max_decode_len;
	else
	    /* if this iteration does not fill buffer, we must have lots
	     * of buffering in filters */
	    huge_filter_buffer = 1;
	if (filter_n_bytes_with_resamp(sh_audio, declen, p_demux, mute) < 0)
	    return -1;
    }
    return 0;
}

void resync_audio_stream(sh_audio_t *sh_audio)
{
    sh_audio->a_buffer_len = 0;
    sh_audio->a_out_buffer_len = 0;

#ifdef NEW_AUDIO_FILL_BUFFER
	sh_audio->w_a_out_buffer_len = 0;
	sh_audio->r_a_out_buffer_len = 0;
#endif

    sh_audio->a_in_buffer_len = 0;	// clear audio input buffer
    if (!sh_audio->initialized)
	return;
    sh_audio->ad_driver->control(sh_audio, ADCTRL_RESYNC_STREAM, NULL);
}

void skip_audio_frame(sh_audio_t *sh_audio)
{
    if (!sh_audio->initialized)
	return;
    if (sh_audio->ad_driver->control(sh_audio, ADCTRL_SKIP_FRAME, NULL) ==
	CONTROL_TRUE)
	return;
    // default skip code:
    ds_fill_buffer(sh_audio->ds);	// skip block
}

#ifdef PARSE_REAL_CHANNELS
// AAC sampling rate
static int SampleRate[16] = 
{
	96000, 88200, 64000, 48000, 44100, 32000, 24000, 
	22050, 16000, 12000, 11025, 8000, 0, 0, 0, 0
};
const static int dts_channe_table[] = {1, 2, 2, 2, 2,  3, 3, 4, 4, 5,  6, 6, 6, 7, 8,  8};
static int mp3_mult[3] = { 12, 144, 144 };
static int tabsel_123[2][3][16] = 
{
   { 
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,0} 
   },

   {
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0} 
   }
};

unsigned char getbits(unsigned char *buffer, unsigned int from, unsigned char len)
{
    unsigned int n;
    unsigned char m, u, l, y;

    n = from / 8;
    m = from % 8;
    u = 8 - m;
    l = (len > u ? len - u : 0);

    y = (buffer[n] << m);
    if(8 > len)
        y  >>= (8-len);
    if(l)
        y |= (buffer[n+1] >> (8-l));

    return  y;
}

void InitGetBits(BitData *bf, unsigned char *pSource, int N)
{
	bf->pSource=(unsigned char*)pSource;	
//	bf->nMaxNumSourceBytes=N;
	bf->nSourceCount=0;
	bf->cword=0;
	bf->nbits=0;
}

static int GetByte(BitData *bf)
{
	return (int)(bf->pSource[bf->nSourceCount++]);
}

long GetBits(BitData* bf, int n)
{
	while ( bf->nbits < n )
	{
		bf->cword = (bf->cword << 8) | GetByte(bf);
		bf->nbits += 8;
	}

	bf->nbits -= n;

	return (long)((bf->cword >> bf->nbits) & (((int64_t)1 << n) - 1));
}

static inline int Get24bBE( unsigned char *p )
{
	    return (p[0] << 16)|(p[1] << 8)|(p[2]);
}

static int get_ac3_channel(int acmod) {
	unsigned int channel_table[8] = {
		2, 1, 2, 3, 3,  4, 4, 5
	};
	if (acmod < 0 || acmod > 7)
		return 0;
	return channel_table[acmod];
}

int mp_a52_header_parse_ac3(unsigned char *p, int packet_len)
{
	int nRet = 0;
	int fscod = p[4] >> 6;
	int bsid = p[5] >> 3;
	int frmsizcod = p[4] & 0x3f;
	int acmod;
	int lfeon;
	int bitcnt = 8 * 6;
	static const uint8_t pi_halfrate[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3 };
	const unsigned i_rate_shift = pi_halfrate[bsid]; // from vlc player

	acmod = getbits(p, bitcnt, 3);
	bitcnt += 3;
	if ((acmod & 0x1) && (acmod != 0x1))
		bitcnt += 2;
	if (acmod & 0x4)
		bitcnt += 2;
	if (acmod == 0x2)
		bitcnt += 2;
	lfeon = getbits(p, bitcnt++, 1);

	if ((p[6] & 0xf8) == 0x50) {
		// Dolby surround = stereo + Dolby
		return 2;
	} else {
		return get_ac3_channel(acmod) + lfeon;
	}
	
	return nRet;
}

static int mp_a52_header_parse_eac3(unsigned char *p, int packet_len)
{
	int nRet = 0;
	int bitcnt;
	int lfeon;
	int frmsiz, fscod, acmod;
	int size;
	int numblkscod, bsid;

	bitcnt = 16 +	// syncword
		2 +			// bsi - strmtyp
		3;			// bsi - substreamid
	frmsiz = getbits(p, bitcnt, 3) << 8;
	bitcnt += 3;
	frmsiz |= getbits(p, bitcnt, 8);
	bitcnt += 8;
	if (frmsiz < 2)
		return nRet;
	size = 2 * (frmsiz + 1);
	fscod = getbits(p, bitcnt, 2);
	bitcnt += 2;
	if (fscod == 0x03)
	{
		const unsigned fscod2 = getbits(p, bitcnt, 2 );
		bitcnt += 2;
		if( fscod2 == 0X03 )
			return nRet;
	}
	else
	{
		numblkscod = getbits(p, bitcnt, 2 );
		bitcnt += 2;
	}
	acmod = getbits(p, bitcnt, 3);
	bitcnt += 3;
	lfeon = getbits(p, bitcnt++, 1);
	bsid = getbits(p, bitcnt, 5);

	return get_ac3_channel(acmod) + lfeon;
}

int mp_a52_header(unsigned char *p, int packet_len)
{
	int cnt = 0;
	int bsid;
	for (cnt = 0; cnt < packet_len - 7; cnt++)
	{
		if(p[cnt] == 0x0B && p[cnt+1] == 0x77)
		{
			bsid = p[cnt+5] >> 3;
			if (bsid > 16)
				continue;

			if (bsid <= 10)
				return mp_a52_header_parse_ac3(p + cnt, packet_len - cnt);
			else
				return mp_a52_header_parse_eac3(p + cnt, packet_len - cnt);
		}
	}

	return 0;
}

int mp_mp3_header(unsigned char* hbuf, int length)
{
	int lsf, framesize, padding, bitrate_index, sampling_frequency = 3, layer;
	int ii = 0, divisor;
	unsigned long newhead = 0;
	int bitrate = 0;
	int i = 0;
		
	if (hbuf == NULL)
		return 0;

	while(sampling_frequency == 3 && ii < length-3)
	{
		// head_check:
		for (; ii < length-3; ii++)
		{
			newhead = hbuf[ii] << 8 | hbuf[ii+1];
			if( ( newhead & 0xffe0 ) == 0xffe0 )
				break;
		}
		
		if( ( newhead & 0xffe0 ) != 0xffe0 )
		{
			return 0;
		}
		
		layer = 4 - ( ( hbuf[ii+1] >> 1 ) & 3 );
		if( layer == 4 )
		{ 
//			printf("not layer-1/2/3\n"); 
			return 0;
		}

		bitrate_index = hbuf[ii+2] >> 4 ;  // valid: 1..14
		sampling_frequency = ( ( hbuf[ii+2] >> 2 ) & 0x3 );  // valid: 0..2

		if(sampling_frequency == 3) ii+=3;
	}
	
	return ((( hbuf[ii+3] >> 6 )) == 3) ? 1 : 2;
}

// program configuration element 
static int get_ele_list(BitData *bf, EleList *p, int enable_cpe)
{  
    int i, j, nChannels = 0;
    for (i = 0, j = p->num_ele; i<j; i++) 
	{
		if (enable_cpe)		
		{
			if ((p->ele_is_cpe[i] = GetBits(bf, 1)))
				nChannels++;
		}		
		else
			p->ele_is_cpe[i] = 0; // sdb 

		p->ele_tag[i] = GetBits(bf, 4);
		nChannels++;
    }

	return nChannels;
}

static int get_prog_config(BitData *bf, ProgConfig *p)
{
    int tag;
		
	p->nChannels = 0;

    tag = GetBits(bf, 4);

    p->profile				= GetBits(bf, 2);
    p->sampling_rate_idx	= GetBits(bf, 4);
    p->front.num_ele		= GetBits(bf, 4);
    p->side.num_ele			= GetBits(bf, 4);
    p->back.num_ele			= GetBits(bf, 4);
    p->lfe.num_ele			= GetBits(bf, 2);
    p->data.num_ele			= GetBits(bf, 3);
    p->coupling.num_ele		= GetBits(bf, 4);

    if ( ( p->mono_mix.present = GetBits(bf, 1) ) == 1 )
		p->mono_mix.ele_tag = GetBits(bf, 4);
    if ( ( p->stereo_mix.present = GetBits(bf, 1) ) == 1 )
		p->stereo_mix.ele_tag = GetBits(bf, 4);
	
    if ( ( p->matrix_mix.present = GetBits(bf, 1) ) == 1 ) 
	{
		p->matrix_mix.ele_tag		= GetBits(bf, 2);
		p->matrix_mix.pseudo_enab	= GetBits(bf, 1);
    }

    p->nChannels += get_ele_list(bf, &p->front, 1);
    p->nChannels += get_ele_list(bf, &p->side, 1);
    p->nChannels += get_ele_list(bf, &p->back, 1);
    p->nChannels += get_ele_list(bf, &p->lfe, 0);
    
    return 1;
}

static int get_adif_header(BitData* bf, ADIFHeader *p)
{
    int i, n;
    // adif header 
    for (i = 0; i < 4; i++)
		p->adif_id[i] = (char)GetBits(bf, 8); 
    p->adif_id[i] = 0;	    // null terminated string 
    
	// test for id 
    if (strncmp(p->adif_id, "ADIF", 4) != 0)
		return 0;	    // bad id 
    
	// copyright string 
    if ((p->copy_id_present = GetBits(bf, 1)) == 1) 
	{
		for (i = 0; i < 9; i++)
			p->copy_id[i] = (char)GetBits(bf, 8); 
		p->copy_id[i] = 0;  // null terminated string 
    }

    p->original_copy	= GetBits(bf, 1);
    p->home				= GetBits(bf, 1);
    p->bitstream_type	= GetBits(bf, 1);
    p->bitrate			= GetBits(bf, 23);

    // program config elements 
    n = GetBits(bf, 4) + 1;
    for ( i = 0 ; i < n ; i++ ) 
	{
		if( p->bitstream_type == 0 )
			p->prog_config.buffer_fullness = GetBits(bf, 20);

		if( !get_prog_config(bf, &(p->prog_config)) )
		{
			return 0;
		}
    }
 
    return 1;
}

static int get_adts_header(BitData* bf, ADTSHeader *p)
{
	p->syncword = GetBits(bf, 12);
	
	if ( p->syncword != 0xfff) 
		return 0;

	p->id					= GetBits(bf, 1);
	p->layer				= GetBits(bf, 2);
	p->protection_abs		= GetBits(bf, 1);
	p->profile				= GetBits(bf, 2);
	p->sampling_freq_idx	= GetBits(bf, 4);
	p->private_bit			= GetBits(bf, 1);
	p->channel_config		= GetBits(bf, 3);
	p->original_copy		= GetBits(bf, 1);
	p->home					= GetBits(bf, 1);
	p->copyright_id_bit		= GetBits(bf, 1);
	p->copyright_id_start	= GetBits(bf, 1);
	p->frame_length			= GetBits(bf, 13);
	p->adts_buffer_fullness = GetBits(bf, 11);

	if( p->layer != 0 )		
		return 0;	// ADTS layer data error 

	if (p->sampling_freq_idx > 11)
		return 0;

	return 1;
}

int mp_aac_header(unsigned char *hdr, int bufsize)
{
	int nRet = -1;
	BitData bf;
	int ii;
	int channels = 0;
	AAC_INFO *AacInfo = NULL;;

	AacInfo = malloc(sizeof(AAC_INFO));
	memset((void *)AacInfo, 0, sizeof(AAC_INFO));
	for (ii=0; ii<bufsize - 12; ii++)
	{
		// AAC Header
		if(!strncmp((char *)(hdr + ii), "ADIF", 4)) 
		{
			// ADIF 
			InitGetBits(&bf, &hdr[ii], bufsize - ii);
			nRet = get_adif_header(&bf, &(AacInfo->adifHeader));
			if ( nRet == 0 ) 
			{
				free(AacInfo);
				return 0;
			}

			channels = AacInfo->adifHeader.prog_config.nChannels;
			free(AacInfo);
			return channels;
		}
		else if( hdr[ii] == 0xFF && ( (unsigned char)(hdr[ii+1] & 0xF6) == (unsigned char)0xF0 ) )
		{
			// ADTS 		
			// need 4 bytes
			InitGetBits(&bf, &hdr[ii], bufsize - ii);
			nRet = get_adts_header(&bf, &(AacInfo->adtsHeader));
			if ( nRet == 0 ) 
			{
				free(AacInfo);
				return 0;
			}

			channels = AacInfo->adtsHeader.channel_config;
			free(AacInfo);
			return channels;

		}
		if (nRet == 1)
		{
			nRet = ii;
			break;
		}
	}

	free(AacInfo);
	return nRet;
}

static void flac_parse_block_header(unsigned char *data, int *last, int *type, int *size)
{
	if (data == NULL)
		return;
	if(last)
		*last = data[0] >> 7;
	if(type)
		*type = data[0] & 0x7;
	if(size)
		*size = Get24bBE(&data[1]);
}

int mp_flac_header(unsigned char *buf, int bufsize)
{
	int nRet = 0;
	int last = 0, type, size;
	unsigned char hdr[/*FLAC_STREAMINFO_SIZE*/34+4];
	int min_block, max_block, min_frame, max_frame, sample_rate, channel, bit_per_sample;
	float duration = 0;
	uint64_t sample;
	BitData bf;

	buf+=4;
	while (last == 0)
	{
		memcpy(hdr, buf, 4);
		buf+=4;
		flac_parse_block_header(hdr, &last, &type, &size);
		//printf("last: %d, type: %d, size: %d\n", last, type, size);

		if (size > bufsize)
			return 0;
		if (type != /*FLAC_METADATA_TYPE_STREAMINFO*/0)
		{
			buf+=size;
		} else {
			if (size != /*FLAC_STREAMINFO_SIZE*/34)
			{
				//printf("info size error!!!");
				break;
			}
			memcpy(hdr, buf, /*FLAC_STREAMINFO_SIZE*/34);
			InitGetBits(&bf, hdr, /*FLAC_STREAMINFO_SIZE*/34*8);
			min_block = GetBits(&bf, 16);
			max_block = GetBits(&bf, 16);
			min_frame = GetBits(&bf, 24);
			max_frame = GetBits(&bf, 24);
			sample_rate = GetBits(&bf, 20);
			channel = GetBits(&bf, 3) + 1;
			bit_per_sample = GetBits(&bf, 5) + 1;
			sample = GetBits(&bf, 32) << 4;
			sample |= GetBits(&bf, 4);

			if ((min_block < 16) || (min_block > /*FLAC_MAX_BLOCKSIZE*/65535) ||
					(max_block < 16) || (max_block > /*FLAC_MAX_BLOCKSIZE*/65535) ||
					(channel > 8)
					)
				break;

			if ((sample > 0) && (sample_rate > 0))
				duration = sample / sample_rate;

			return channel;
		}
	}

	return nRet;
}

static int get_dts_channel(int amode)
{
	int nRet = 0;

	if ((amode >= 0) && (amode <= 15))
	{
		nRet = dts_channe_table[amode];
	}
	
	return nRet;
}

int mp_dts_header(unsigned char* buf, int buflen)
{
	int nRet = 0;
	int ii;
	int chans = 0;
	//const static unsigned char dts_hdr[] = {0x7f, 0xfe, 0x80, 0x01};
	int amode, sfreq, rate, ext_audio_id, ext_audio, lff;

	if ( (buf == NULL) ||(buflen < 11) )
		return nRet;

	for (ii = 0; ii < buflen - 11; ii++)
	{
		if ( (buf[ii] == 0x7f) && (buf[ii+1] == 0xfe) && (buf[ii+2] == 0x80) && (buf[ii+3] == 0x1) )
			break;
	}
	if (ii == buflen - 11)
		return nRet;
	
	buf+=ii;
	amode = ((buf[7] & 0x0f) << 2) | (buf[8] >> 6);
	sfreq = (buf[8] & 0x3c) >> 2;
	rate = (buf[8] & 0x03 << 3) | (buf[9] >> 5);
	ext_audio_id = buf[10] >> 5;
	ext_audio = (buf[10] >> 4) & 0x1;
	lff = (buf[10] >> 1) & 0x3;
//	printf("amode: %d, sfreq: %d, srate: %d, ext_audio_id %d, ext_audio %d, lff %d\n",
//			amode, sfreq, rate, ext_audio_id, ext_audio, lff);

	chans = get_dts_channel(amode);	
	if ( (ext_audio == 1) && ((ext_audio_id == 0) || (ext_audio_id == 3)) )
			(chans)++;
	if ((lff == 1) || (lff == 2))
			(chans)++;

	return chans;
}

static int ogg_check_audio(unsigned char *ptr, int bodybytes)
{
	if( bodybytes >= 7 && !strncmp((char *)(ptr+1),"vorbis", 6) ) 
	{
		int version;
		long bitrate_upper;
		long bitrate_nominal;
		long bitrate_lower;

		ptr += 7;

		version		= oggpack_read_dword(ptr);		
		ptr += 4;
		if( version != 0 )
			return 0;

		return (*ptr);
	}
	else
		return 0;
}

int mp_lpcm_header(unsigned char *p, int packet_len)
{
	int nRet = 0;
	int tmp, frm_cnt, first_aac_unit, frm_num, quantization, sample_idx, chan_num;
	BitData bf;

	if ((packet_len < 6))
		return nRet;

	InitGetBits(&bf, p, packet_len);
	frm_cnt = GetBits(&bf, 8);
	first_aac_unit = GetBits(&bf, 16);
	tmp = GetBits(&bf, 3);
	frm_num = GetBits(&bf, 5);
	quantization = GetBits(&bf, 2);
	sample_idx = GetBits(&bf, 2);
	tmp = GetBits(&bf, 1);
	chan_num = GetBits(&bf, 3);

	// check value range
	if (quantization == 3)
	{
		printf("LPCM: illegal quantization value\n");
		return nRet;
	}

	return chan_num;
}

int get_audio_channels(sh_audio_t *sh)
{
	unsigned int read_len = 0;
	unsigned int read_cnt = 20;
	unsigned char *buf = NULL;
	demuxer_t *demux = sh->ds->demuxer;
	int real_channels = 0;
	int ii = 0;
	
	if(sh && sh->codec)
	{
		while(sh->ds->bytes < 1024 && read_cnt < 1000)
		{
			read_cnt++;
			demux_fill_buffer(demux, sh->ds);
		}

		if(sh->ds->bytes >= 1024)
		{
			int check_length = 0;
			read_len = sh->ds->bytes;
			buf = calloc(sh->ds->bytes, sizeof(unsigned char));
			if(!buf)
				return 0;

			demux_packet_t *p = sh->ds->first;
			for(ii = 0 ; ii < sh->ds->packs ; ii++)
			{
				memcpy(buf +  check_length , p->buffer, p->len);
				check_length += p->len;
				p = p->next;
			}
			
			if(!strcmp(sh->codec->drv, "saiaac3") || !strcmp(sh->codec->drv, "saac3"))
			{
				real_channels = mp_a52_header(buf, read_len);
			}
			else if(!strcmp(sh->codec->drv, "sara"))		//mkv
			{
				printf("@@@@@ sara need be parsed @@@@@\n");
			}
			else if(!strcmp(sh->codec->drv, "sawmapro"))	//asf
			{
				printf("@@@@@ sawmapro need be parsed @@@@@\n");
			}
			else if(!strcmp(sh->codec->drv, "samp3"))
			{
				real_channels = sh->channels;
			}
			else if(!strcmp(sh->codec->drv, "samchmp3"))	//mov
			{
//				printf("@@@@@ samchmp3 need be parsed @@@@@\n");
				real_channels = as_channels;
			}
			else if(!strcmp(sh->codec->drv, "saaac"))
			{
				real_channels = mp_aac_header(buf, read_len);
			}
			else if(!strcmp(sh->codec->drv, "saflac"))
			{
				real_channels = mp_flac_header(buf, read_len);
			}
			else if(!strcmp(sh->codec->drv, "skdts") || !strcmp(sh->codec->drv, "sadts") || !strcmp(sh->codec->drv, "sadtshd2"))
			{
//				real_channels = mp_dts_header(buf, read_len);
				real_channels = as_channels;
			}
			else if(!strcmp(sh->codec->drv, "sawma"))	//asf
			{
				printf("@@@@@ sawma need be parsed @@@@@\n");
			}
			else if(!strcmp(sh->codec->drv, "saogg"))
			{
				real_channels = ogg_check_audio(buf, read_len);
			}
			else if(!strcmp(sh->codec->drv, "saamrnb"))	//mov
			{
				printf("@@@@@ saamrnb need be parsed @@@@@\n");
			}
			else if(!strcmp(sh->codec->drv, "hwac3"))
			{
				if (sh->format == 0x2001)
				{
					real_channels = mp_dts_header(buf, read_len);
#if 1	//Barry 2011-07-26
					read_cnt = 0;
					while (!real_channels && read_cnt < 100)
					{
						read_cnt++;
						demux_fill_buffer(demux, sh->ds);

						if (buf)
							free(buf);
						check_length = 0;
						read_len = sh->ds->bytes;
						buf = calloc(sh->ds->bytes, sizeof(unsigned char));
						if(!buf)
							return 0;

						demux_packet_t *p = sh->ds->first;
						for(ii = 0 ; ii < sh->ds->packs ; ii++)
						{
							memcpy(buf +  check_length , p->buffer, p->len);
							check_length += p->len;
							p = p->next;
						}
						real_channels = mp_dts_header(buf, read_len);
					}
#endif

				}
				else
				{
					real_channels = mp_a52_header(buf, read_len);
				}
			}
			else if(sh->format == mmioFOURCC('B', 'P', 'C', 'M'))
			{
				static const uint32_t channels[16] = {
					0, 1, 0, 2, 3, 3, 4, 4, 5, 6, 7, 8, 0, 0, 0, 0
				};
				real_channels = channels[buf[2] >> 4];
			}
			else if(sh->format == 0x10001)
			{
				real_channels = mp_lpcm_header(buf, read_len);
			}
		}
	}
	if(buf) free(buf);
	return real_channels;
}
#endif
