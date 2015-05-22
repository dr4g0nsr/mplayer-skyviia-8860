#include <stdio.h>
#include <stdlib.h>
#include "ra_api.h"
#include "config.h"
#include "ad_internal.h"



static ad_info_t info = 
{
	"Skymedia Real Audio Decoder",
	"skra",
	"Jack Yang",
	"Jack Yang",
	"Skyviia Copyright"
};

LIBAD_EXTERN(skra)

//======================= for decoder interface =====================//

static int preinit(sh_audio_t *sh)
{
   sh->audio_out_minsize = 192000;
   return 1;
}
extern int audio_output_channels;
#define GECKO_VERSION               ((1L<<24)|(0L<<16)|(0L<<8)|(3L))
#define GECKO_MC1_VERSION           ((2L<<24)|(0L<<16)|(0L<<8)|(0L))
static int init(sh_audio_t *sh_audio)
{
   int flags2;
   int ch, sample_rate, bit_rate, block_align;

   flags2 = 0;

   sh_audio->codecdata_len = sh_audio->wf->cbSize;
   ch = sh_audio->wf->nChannels;
   sample_rate = sh_audio->wf->nSamplesPerSec;
   bit_rate = sh_audio->wf->nAvgBytesPerSec * 8;
   block_align = sh_audio->wf->nBlockAlign;

   if(Gecko2InitDecode((char *)sh_audio->wf + sizeof(WAVEFORMATEX), sh_audio->codecdata_len, block_align<<3, ch, sample_rate) < 0)
         return -1;

   if(audio_output_channels != 0)
   {
      if(audio_output_channels <= ch)
      {
         ch = audio_output_channels;
      }
   }
   // Decode at least 1 byte:  (to get header filled)
   //if(x>0) sh_audio->a_buffer_len=x;
   sh_audio->channels = ch;
   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   sh_audio->samplesize = 2;

   return 1;
}

static void uninit(sh_audio_t *sh)
{
   Gecko2Uninit();
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#ifndef RA_DEBUG
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
#else
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf, int *buf1, int minlen,int maxlen)
#endif
{

   unsigned char *start=NULL;
   int y;

   int len2;
   double pts;
   int x=ds_get_packet_pts(sh_audio->ds,&start, &pts);
   
   if(x<=0) 
      return -1; // error
   if (pts != MP_NOPTS_VALUE) {
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }

#ifndef RA_DEBUG
   y = Gecko2Dec(x, start, 0, &len2, (short*)buf, sh_audio->channels);
#else
   y = Gecko2Dec(x, start, 0, &len2, (short*)buf, buf1, sh_audio->channels);
#endif

   if(y < x) 
      sh_audio->ds->buffer_pos+=y-x;  // put back data (HACK!)
   sh_audio->pts_bytes += len2;
   return len2;
}
