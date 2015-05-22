#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "wma_api.h"
#include "ad_internal.h"

static ad_info_t info = 
{
	"wma audio decoders",
	"skwma",
	"Jack Yang",
	"Jack Yang",
	""
};

LIBAD_EXTERN(skwma)


//======================= for decoder interface =====================//

static int preinit(sh_audio_t *sh)
{
   sh->audio_out_minsize = 192000;
   return 1;
}


static int init(sh_audio_t *sh_audio)
{
   int flags2;
   int ch, sample_rate, bit_rate, block_align, format;

   flags2 = 0;

   sh_audio->codecdata_len = sh_audio->wf->cbSize;
   ch = sh_audio->wf->nChannels;
   sample_rate = sh_audio->wf->nSamplesPerSec;
   bit_rate = sh_audio->wf->nAvgBytesPerSec * 8;
   block_align = sh_audio->wf->nBlockAlign;
   format = sh_audio->wf->wFormatTag;

	if (sample_rate<=0 || sample_rate>50000
		    || ch<=0 || ch>8
		    || bit_rate<=0)
      return -1;
   if (sh_audio->codecdata_len >= 6 && format == 0x161) {
      char *x;
      x = (char *)sh_audio->wf + sizeof(WAVEFORMATEX) + 4;
      flags2 = (x[1] << 8) | x[0];
   }
   else if(sh_audio->codecdata_len >= 4 && format == 0x160)
   {
      char *x;
      x = (char *)sh_audio->wf + sizeof(WAVEFORMATEX) + 2;
      flags2 = (x[1] << 8) | x[0];
   }
   if(wma_init(ch, sample_rate, bit_rate, block_align, format, flags2)<0)
      return -1;

   // Decode at least 1 byte:  (to get header filled)
   //x=decode_audio(sh_audio,sh_audio->a_buffer,1,sh_audio->a_buffer_size);
   //if(x>0) sh_audio->a_buffer_len=x;
   sh_audio->channels = ch;
   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   sh_audio->samplesize = 2;

   return 1;
}

static void uninit(sh_audio_t *sh)
{
   wma_end();
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
      wma_resync();
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}
#ifndef WMA_DEBUG
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
#else
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int *out,int minlen,int maxlen)
#endif
{

   unsigned char *start=NULL;
   int y;

   int len2=0;
   double pts;
   int x=ds_get_packet_pts(sh_audio->ds,&start, &pts);
   if(x<=0) 
      return -1; // error
   if (pts != MP_NOPTS_VALUE) {
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }
#ifndef WMA_DEBUG
   y = wma_decode_superframe((int16_t*)buf,&len2,start,x);
#else
   y = wma_decode_superframe((int16_t*)buf,out,&len2,start,x);
#endif
   if(y < x) 
      sh_audio->ds->buffer_pos+=y-x;  // put back data (HACK!)
   return len2;
}
