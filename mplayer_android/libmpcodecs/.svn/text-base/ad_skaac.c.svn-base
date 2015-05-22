/* ad_faad.c - MPlayer AAC decoder using libfaad2
 * This file is part of MPlayer, see http://mplayerhq.hu/ for info.  
 * (c)2002 by Felix Buenemann <atmosfear at users.sourceforge.net>
 * File licensed under the GPL, see http://www.fsf.org/ for more info.
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef  EngineMode 
#include "config.h"
#include "ad_internal.h"

static ad_info_t info = 
{
	"Skyviia HEAAC V2 decoder",
	"skaac",
	"Honda",
	"Honda",
	"Skyviia copyright"
};

LIBAD_EXTERN(skaac)
#else
#include "mp_msg.h"
#include "help_mp.h"
#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "aac_mplayer.h"

#endif


#include "aacdec.h"


/* configure maximum supported channels, *
 * this is theoretically max. 64 chans   */
#define AAC_2CH_ONLY

#ifdef   AAC_2CH_ONLY
#define AAC_MAX_CHANNELS 2
#else
#define AAC_MAX_CHANNELS 6
#endif
//#define AAC_BUFFLEN (FAAD_MIN_STREAMSIZE*AAC_MAX_CHANNELS)		   
#define AAC_BUFFLEN (FAAD_MIN_STREAMSIZE*6)    

//#define AAC_DUMP_COMPRESSED  

static AACDecHandle faac_hdec;
static AACDecFrameInfo faac_finfo;

static int ADIF_ON;     //Honda : for ADIF
static int AAC_MODE;    //0: mp4, 1: LATM, 2: ADIF, 3: ADTS

static int preinit(sh_audio_t *sh)
{
   sh->audio_out_minsize=8192*AAC_MAX_CHANNELS;
   sh->audio_in_minsize=AAC_BUFFLEN;
   return 1;
}

static int aac_probe(unsigned char *buffer, int len, int mode)
{
   int i = 0, pos = 0;
   mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: %d bytes\n", len);
   while(i <= len-4) {
#if 1
      int t, t2;
      if ((buffer[i] == 0x56) && (mode==0 || mode==1)){  
         if ((buffer[i+1]&0xE0)==0xE0){
            t = ((buffer[i+1]&0x1F)<<8)+buffer[i+2];
            if ((i+4+t)<len){
               t2 = (buffer[i+3+t]<<8)+(buffer[i+4+t]);
               if ((t2>>5)==0x2b7){
                  pos = i;
                  break;
               }
            } 
         }
      }
      if ((buffer[i] == 0xff) && (mode==0 || mode==3)){
         if ((buffer[i+1] & 0xf6) == 0xf0){
            t = ((buffer[i+3]&0x3)<<11)+(buffer[i+4]<<3)+(buffer[i+5]>>5);
            if ((i+1+t)<len){
               t2 = (buffer[i+t]<<8)+(buffer[i+1+t]);
               if ((t2&0xfff6)==0xfff0){
                  pos = i;
                  break;
               }
            }
         }
      }
      if ((mode==0 || mode==2) && (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')){
         pos = i;
         break;
      }
#else
    if(
       ((buffer[i] == 0xff) && ((buffer[i+1] & 0xf6) == 0xf0)) ||
       (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')
    ) {
      pos = i;
      break;
    }
#endif
    //mp_msg(MSGT_DECAUDIO,MSGL_V, "AUDIO PAYLOAD: %x %x %x %x\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]);
    i++;
  }
  mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: ret %d\n", pos);
  return pos;
}
	
extern int audio_output_channels;
static int init(sh_audio_t *sh)
{
  unsigned long faac_samplerate;
  unsigned char faac_channels;
  int faac_init, pos = 0;
  int output_channels;
  AAC_MODE = 0; 
#ifdef   AAC_2CH_ONLY
   output_channels = 2;
#else
   output_channels = audio_output_channels;
#endif

   faac_hdec = AAC_DecOpen(output_channels);
   ADIF_ON = 0;       //Honda : for ADIF

  // If we don't get the ES descriptor, try manual config
   if(!sh->codecdata_len && sh->wf) {
      sh->codecdata_len = sh->wf->cbSize;
//      sh->codecdata = (unsigned char*)(sh->wf+1);		//Barry 2010-05-20 disable	Fuchun 2010.04.20 enable
      sh->codecdata = malloc(sh->codecdata_len);		//Barry 2010-05-20 enable	Fuchun 2010.04.20 disable
      memcpy(sh->codecdata, sh->wf+1, sh->codecdata_len);
      mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SKAAC: codecdata extracted from WAVEFORMATEX\n");
   }
   if(!sh->codecdata_len) {
      AACDecConfigurationPtr faac_conf;
    /* Set the default object type and samplerate */
    /* This is useful for RAW AAC files */
      faac_conf = AACDecGetCurrentConfiguration(faac_hdec);
      if(sh->samplerate)
         faac_conf->defSampleRate = sh->samplerate;
    /* XXX: SKAAC support FLOAT output, how do we handle
      * that (FAAD_FMT_FLOAT)? ::atmos
      */
      if (output_channels <= 2) 
         faac_conf->downMatrix = 1;
      switch(sh->samplesize){
	   case 1: // 8Bit
	      mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SKAAC: 8Bit samplesize not supported by SKAAC, assuming 16Bit!\n");
	   default:
	      sh->samplesize=2;
	   case 2: // 16Bit
	      faac_conf->outputFormat = FAAD_FMT_16BIT;
	      break;
	   case 3: // 24Bit
	      faac_conf->outputFormat = FAAD_FMT_24BIT;
	      break;
	   case 4: // 32Bit
	      faac_conf->outputFormat = FAAD_FMT_32BIT;
	      break;
      }

      AACDecSetConfiguration(faac_hdec, faac_conf);

      sh->a_in_buffer_len = demux_read_data(sh->ds, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_size);
      pos = aac_probe((unsigned char*)sh->a_in_buffer, sh->a_in_buffer_len, AAC_MODE);
      if(pos) {
         sh->a_in_buffer_len -= pos;
         memmove(sh->a_in_buffer, &(sh->a_in_buffer[pos]), sh->a_in_buffer_len);
         sh->a_in_buffer_len +=
	         demux_read_data(sh->ds,(unsigned char*)&(sh->a_in_buffer[sh->a_in_buffer_len]), sh->a_in_buffer_size - sh->a_in_buffer_len);
         pos = 0;
      }

    /* init the codec */
      faac_init = AACDecInit(faac_hdec, (unsigned char*)sh->a_in_buffer,
         sh->a_in_buffer_len, &faac_samplerate, &faac_channels, &sh->i_bps, &AAC_MODE);
 
        //update by archin 20101026
         sh->i_bps = sh->i_bps/8;
      sh->a_in_buffer_len -= (faac_init > 0)?faac_init:0; // how many bytes init consumed
      if (faac_init>0){                  //Honda : for ADIF
         memmove(sh->a_in_buffer,&sh->a_in_buffer[faac_init],sh->a_in_buffer_len);
         ADIF_ON = 1;
      }  

   } 
   else { // We have ES DS in codecdata
      AACDecConfigurationPtr faac_conf = AACDecGetCurrentConfiguration(faac_hdec);
      if (output_channels <= 2) {
         faac_conf->downMatrix = 1;
         AACDecSetConfiguration(faac_hdec, faac_conf);
      }
    
      faac_init = AACDecInit2(faac_hdec, sh->codecdata,
      sh->codecdata_len,	&faac_samplerate, &faac_channels);
   }
   if(faac_init < 0) {
      mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SKAAC: Failed to initialize the decoder!\n"); // XXX: deal with cleanup!
      AACDecClose(faac_hdec);
    // XXX: free a_in_buffer here or in uninit?
      return 0;
   } 
   else {
      mp_msg(MSGT_DECAUDIO,MSGL_V,"SKAAC: Decoder init done (%dBytes)!\n", sh->a_in_buffer_len); // XXX: remove or move to debug!
      mp_msg(MSGT_DECAUDIO,MSGL_V,"SKAAC: Negotiated samplerate: %ldHz  channels: %d\n", faac_samplerate, faac_channels);
      //sh->channels = faac_channels;
      //if (output_channels <= 2) 
      //   sh->channels = faac_channels > 1 ? 2 : 1;
      sh->channels = output_channels;
      sh->samplerate = faac_samplerate;
      sh->samplesize=2;
    //sh->o_bps = sh->samplesize*faac_channels*faac_samplerate;
      if(!sh->i_bps) {
         mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SKAAC: compressed input bitrate missing, assuming 128kbit/s!\n");
         sh->i_bps = 128*1000/8; // XXX: HACK!!! ::atmos
      } else 
         mp_msg(MSGT_DECAUDIO,MSGL_V,"SKAAC: got %dkbit/s bitrate from MP4 header!\n",sh->i_bps*8/1000);
      mp_msg(MSGT_DECAUDIO,MSGL_V,"USING %s\n", GET_SKAAC_PROFILE());
   }  
   return 1;
}

static void uninit(sh_audio_t *sh)
{
   mp_msg(MSGT_DECAUDIO,MSGL_V,"SKAAC: Closing decoder!\n");
   AACDecClose(faac_hdec);
}

static int aac_sync(sh_audio_t *sh)
{
  int pos = 0;
  if(!sh->codecdata_len) {
    if(sh->a_in_buffer_len < sh->a_in_buffer_size){
      sh->a_in_buffer_len +=
	demux_read_data(sh->ds,(unsigned char*)&sh->a_in_buffer[sh->a_in_buffer_len],
	sh->a_in_buffer_size - sh->a_in_buffer_len);
    }
    pos = aac_probe((unsigned char*)sh->a_in_buffer, sh->a_in_buffer_len, AAC_MODE);
    if(pos) {
      sh->a_in_buffer_len -= pos;
      memmove(sh->a_in_buffer, &(sh->a_in_buffer[pos]), sh->a_in_buffer_len);
      mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC SYNC AFTER %d bytes\n", pos);
    }
  }
  return pos;
}

//static int aac_cnt=0;
 
#define MAX_FAAD_ERRORS 10
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
   int len = 0, last_dec_len = 1, errors = 0;	      
   void *faac_sample_buffer;

   while(len < minlen && last_dec_len > 0 && errors < MAX_FAAD_ERRORS) {
    /* update buffer for raw aac streams: */
      if(!sh->codecdata_len)
         if(sh->a_in_buffer_len < sh->a_in_buffer_size){
            sh->a_in_buffer_len +=
	         demux_read_data(sh->ds,(unsigned char*)&sh->a_in_buffer[sh->a_in_buffer_len],
	         sh->a_in_buffer_size - sh->a_in_buffer_len);
      }
      if(!sh->codecdata_len){
         //aac_cnt++;            
         //mp_msg(MSGT_DECAUDIO,0,"%d : len = %d                      \n",aac_cnt, sh->a_in_buffer_len); 
         //if (sh->a_in_buffer_len == 378)
         //   mp_msg(MSGT_DECAUDIO,MSGL_WARN,"last frame\n");     
   // raw aac stream:
         do {
            if (sh->a_in_buffer_len == 0)
               return len;
            faac_sample_buffer = AACDecDecode(faac_hdec, &faac_finfo, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_len);
            /* update buffer index after AACDecDecode */
            if(faac_finfo.bytesconsumed >= (unsigned int)sh->a_in_buffer_len) {
               sh->a_in_buffer_len=0;
            } 
            else {
               sh->a_in_buffer_len-=faac_finfo.bytesconsumed;
               memmove(sh->a_in_buffer,&sh->a_in_buffer[faac_finfo.bytesconsumed],sh->a_in_buffer_len);
            }

            if(faac_finfo.error > 0) {
               mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SKAAC: error: %s, trying to resync!\n", AACDecGetErrorMessage(faac_finfo.error));
               if ((sh->a_in_buffer_len <= 0)||ADIF_ON) {      ////Honda : for ADIF
                  errors = MAX_FAAD_ERRORS;
                  if (ADIF_ON){
                     sh->a_in_buffer_len = 0;
                     demux_read_data(sh->ds,sh->a_in_buffer, sh->a_in_buffer_size);
                     sh->ds->eof = 1;
                  } 
                  break;
               }
               sh->a_in_buffer_len--;
               memmove(sh->a_in_buffer,&sh->a_in_buffer[1],sh->a_in_buffer_len);
               aac_sync(sh);
               errors++;
            } else
               break;
         } while(errors < MAX_FAAD_ERRORS);	  
      } 
      else {
   // packetized (.mp4) aac stream:
         unsigned char* bufptr=NULL;
         double pts;
         int buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
         if(buflen<=0) 
            break;
         if (pts != MP_NOPTS_VALUE) {
	         sh->pts = pts;
	         sh->pts_bytes = 0;
         }
         faac_sample_buffer = AACDecDecode(faac_hdec, &faac_finfo, bufptr, buflen);
      }
#ifdef  EngineMode 
      AAC_PCM32_INFO(faac_hdec, &sh->pcm32.data, &sh->pcm32.ch, &sh->pcm32.len);
#endif
   
      if(faac_finfo.error > 0) {
         mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SKAAC: Failed to decode frame: %s \n",
         AACDecGetErrorMessage(faac_finfo.error));
      } 
      else if (faac_finfo.samples == 0) {
         mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SKAAC: Decoded zero samples!\n");
      } 
      else {
      /* XXX: samples already multiplied by channels! */
         mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SKAAC: Successfully decoded frame (%ld Bytes)!\n",
         sh->samplesize*faac_finfo.samples);
         memcpy(buf+len,faac_sample_buffer, sh->samplesize*faac_finfo.samples);
         last_dec_len = sh->samplesize*faac_finfo.samples;
         len += last_dec_len;
         sh->pts_bytes += last_dec_len;
      }
   }
   return len;
}

#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    switch(cmd)
    {
      case ADCTRL_RESYNC_STREAM:
         aac_sync(sh);
	 return CONTROL_TRUE;
#if 0      
      case ADCTRL_SKIP_FRAME:
	  return CONTROL_TRUE;
#endif
    }
  return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skaac = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};

#endif

