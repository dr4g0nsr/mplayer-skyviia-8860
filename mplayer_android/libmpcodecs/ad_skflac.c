/* ad_faad.c - MPlayer AAC decoder using libfaad2
 * This file is part of MPlayer, see http://mplayerhq.hu/ for info.  
 * (c)2002 by Felix Buenemann <atmosfear at users.sourceforge.net>
 * File licensed under the GPL, see http://www.fsf.org/ for more info.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef  EngineMode 

#include "config.h"
#include "ad_internal.h" 


static ad_info_t info = 
{
   "Skymedi FLAC decoder",
   "skflac",       //compare to sh_audio->codec->drv
   "Honda",
   "Honda",
   "Skymedi copyright"
};

LIBAD_EXTERN(skflac)
#else

#include "mp_msg.h"
#include "help_mp.h"
#include "flac_mplayer.h"

typedef unsigned char uint8_t;
#define MP_NOPTS_VALUE (-1LL<<63)

#endif

typedef struct {
   int samplerate;
   int channels;     
} FLACContextLite;


int flac_get_memory_size(void);
void flac_flush(void *avctx);
int flac_decode_init(void* avctx, unsigned char* extradata, int extradata_size);
int flac_decode_close(void *avctx);
int flac_decode_frame(void *avctx, void *data, int *data_size, uint8_t *buf, int buf_size);


//return -1: EOF
//return 0: possible error frame
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
   int len = 0;	
   FLACContextLite *s = sh->context;

   while(len < minlen ) {
      int y;
      int len2=0;
      unsigned char* bufptr=NULL;
      double pts;
      int buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
      if(buflen<=0) 
         return -1;     //EOF
      if (pts != MP_NOPTS_VALUE) 
      {
	      sh->pts = pts;
	      sh->pts_bytes = 0;
      }
      y = flac_decode_frame((void *)s, buf, &len2, bufptr, buflen);
      if(y<0)
	   { 
	      mp_msg(MSGT_DECAUDIO,MSGL_V,"FLAC_audio: error code %d\n", y);
	      break; 
	   }
      if(y<buflen) 
	      sh->ds->buffer_pos+=y-buflen;  
	   {
	      len+=len2;
	      buf+=len2;
	      sh->pts_bytes += len2;
	   }
   }
   return len;
}



static int preinit(sh_audio_t *sh)
{
   int size = flac_get_memory_size();
   sh->audio_out_minsize=65536*2*6;
   sh->context = malloc(size);
   //memset(sh->context, 0 , size);
   return 1;
}

	
static int init(sh_audio_t *sh)
{
   FLACContextLite *s = sh->context;
   int decoded_bytes;
   unsigned char *codecdata = NULL;
   int codecdata_len = 0;

   if (sh->codecdata && sh->codecdata_len>0){
      codecdata = sh->codecdata;
      codecdata_len = sh->codecdata_len;
   }
   else if (sh->wf && sh->wf->cbSize){
      codecdata = (unsigned char*)sh->wf + sizeof(WAVEFORMATEX);
      codecdata_len = sh->wf->cbSize;
   }


   if(!flac_decode_init((void*)s, codecdata, codecdata_len))
      goto init_fail;
 
   decoded_bytes = decode_audio(sh, (unsigned char*)sh->a_buffer,1,sh->a_buffer_size);
   if (decoded_bytes <= 0){
init_fail:
      mp_msg(MSGT_DECAUDIO,MSGL_WARN, "FLAC initial error.\n");
      free(sh->context);
      sh->context = NULL;   
      return 0;
   }
 
   sh->channels = s->channels;
   sh->samplerate = s->samplerate;
   sh->a_buffer_len = decoded_bytes;

   return 1;

}

static void uninit(sh_audio_t *sh)
{
   FLACContextLite *s = sh->context;
   flac_decode_close(s);
   free(sh->context);
   sh->context = NULL;   
}


#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         flac_flush(sh->context);
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skflac = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif




