#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "wmapro_api.h"
#include "ad_internal.h"
//#include "macros.h"

static ad_info_t info = 
{
	"SKYVIIA WMA Professional Audio Decoder",
	"skwmapro",
	"Honda",
	"Honda",
	""
};

LIBAD_EXTERN(skwmapro)

extern int audio_output_channels;

static int BitPackedFrameSize;
static int fs_status;
static unsigned char *g_stream_buf = NULL;
static int g_stream_len;
static int seq_num;


static int fill_packet(sh_audio_t *sh_audio, unsigned char **buf)
{
   int len=0;
   int in_packet=0;
   double pts;
   int x;
   unsigned char *start;   
   int blocklen = sh_audio->wf->nBlockAlign;

   {
      do{
         int pktbit;
         int pktlen;
         int num;
         x = ds_get_packet_pts(sh_audio->ds, &start, &pts);
         
         if(x <= 0){ 
            if (len)
               break;
            else
               return -1; // error
         }

         if(blocklen < x){ 
            sh_audio->ds->buffer_pos += blocklen-x; 
            x = blocklen;
         }
         pktbit = 6;
         pktlen = 0;        
         
         while(pktbit < BitPackedFrameSize){
            int byte = pktbit>>3;
            int bit = 7-(pktbit & 7);
            byte = start[byte];
            pktlen <<= 1;
            if (byte & (1<<bit))
               pktlen++;
            pktbit++;
         }
         num = start[0]>>4;
         if (g_stream_len<(len+x)){
            g_stream_buf = realloc(g_stream_buf, len+x);
            if (g_stream_buf == NULL){
               g_stream_len = 0;
               return -1;
            }
            else{
               g_stream_len = len+x;
            }
         }  

         seq_num++; 
         if ((seq_num >=0) && (seq_num&0xf)!= num)
            in_packet = 1;

         seq_num = num;
         memcpy(g_stream_buf+len, start, x);
         len += x;

         if ((pktlen+BitPackedFrameSize) < blocklen*8)
            in_packet = 1;
      }
      while(in_packet==0);
   }
   if (pts != MP_NOPTS_VALUE) {
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }
   *buf = g_stream_buf;
   return len;
}

//======================= for decoder interface =====================//

static int preinit(sh_audio_t *sh)
{
   sh->audio_out_minsize = MAX_BUFFER_OUT*2;

   return 1;
}


static int init(sh_audio_t *sh_audio)
{
   int wValidBitsPerSample, wBitsPerSample;
   int ch, sample_rate, bit_rate, block_align, format;
   unsigned char *x;
   short nEncodeOpt, dwChannelMask, wAdvancedEncodeOptint;
   int  dwAdvancedEncodeOpt2;
   int ret=0,flag,nDstChannelMask=0;

   sh_audio->codecdata_len = sh_audio->wf->cbSize;

   sample_rate = sh_audio->wf->nSamplesPerSec;
   bit_rate = sh_audio->wf->nAvgBytesPerSec * 8;
   block_align = sh_audio->wf->nBlockAlign;
   format = sh_audio->wf->wFormatTag;
   ch = sh_audio->wf->nChannels;
   wValidBitsPerSample = sh_audio->wf->wBitsPerSample;
   wBitsPerSample = 8 * ((wValidBitsPerSample + 7)/8);

	if (sample_rate<=0 
		    || ch<=0 || ch>8
		    || bit_rate<=0)
      return -1;

   x = (unsigned char *)sh_audio->wf + 20;
   dwChannelMask = (x[1] << 8) | x[0];
   x += 8;
   dwAdvancedEncodeOpt2 = (x[3] << 24) | (x[2] << 16) | (x[1] << 8) | x[0];
   x += 4;
   nEncodeOpt = (x[1] << 8) | x[0];
   x += 2;
   wAdvancedEncodeOptint = (x[1] << 8) | x[0];
   
   flag = 0;
   if(audio_output_channels != 0)
   {
      if(dwChannelMask != 0x3 && 2 == audio_output_channels)
      {
         flag = 1;
         nDstChannelMask = 3;
      }
   }

   flag = flag | 0x40; 
   if (sample_rate > 48000)
      flag = flag | 0x8;

   ret = WMAPRO_Init(flag, nDstChannelMask, sample_rate, &ch, bit_rate>>3,
                                    wValidBitsPerSample, format, dwChannelMask,
                                    nEncodeOpt, block_align, wBitsPerSample,
                                    wAdvancedEncodeOptint, dwAdvancedEncodeOpt2, &sh_audio->samplesize, &sample_rate, &BitPackedFrameSize);

   sh_audio->samplesize >>= 3;	//Barry 2010-09-10
   
   if(ret != 0)
      return -1;
   BitPackedFrameSize += 6;

   sh_audio->channels = ch;
   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;

   if (g_stream_buf)
      free(g_stream_buf);
   g_stream_buf = NULL;
   g_stream_len = 0;
   seq_num = -2;


   fs_status = 1;
   return 1;
}

static void uninit(sh_audio_t *sh)
{
   WMAPRO_Uninit();
   if (g_stream_buf)
      free(g_stream_buf);
   g_stream_buf = NULL;
   g_stream_len = 0;
}


static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
      seq_num = -2;
      fs_status = 1;
      WMAPro_Resync();
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}


static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf, int minlen,int maxlen

   )
{
   unsigned char *start=NULL;
   int len2=0;
   int x;
   int len = 0;	

   while(len < minlen ) {
      if (fs_status>0){
         start = g_stream_buf;
         x = fill_packet(sh_audio, &start);
         if (x<0){
            x = 0;
            start=NULL;
         }
      }
      else{
         x = 0;
         start=NULL;
      }

      len2 = WMAPRO_Decode(buf, start, x, &fs_status);

      if (len2<0)
         return -1;

   //if(sh_audio->samplesize == 24)
   //   len2 = len2*3*sh_audio->channels;       
   //else
      len2 = len2*2*sh_audio->channels;

      len += len2;
      buf += len2;       
   }

   return len;
}
