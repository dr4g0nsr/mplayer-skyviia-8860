// SAMPLE audio decoder - you can use this file as template when creating new codec!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
 
#ifndef  EngineMode            

#include "config.h"
#include "ad_internal.h"
static ad_info_t info =  {
	"Skymedi mpeg audio decoder",
	"skmp3",
	"Honda",
	"Honda",
	"Skymedi copyright"
};

LIBAD_EXTERN(skmp3)
#else

#include "mp3_mplayer.h"


#endif


#include "skmp3.h"
#define new_read_packet //若非way2, AVI可能會錯, 若01wb超過一個frame
#ifdef new_read_packet 
#define way2

# ifndef way2
typedef struct {
unsigned short cnt;
unsigned short pack_offset;
} PACKET_INFO;
static PACKET_INFO pack;

static int fill_mp3_packet(sh_audio_t *sh)
{
   int len;
   unsigned char *buf;
   if (pack.cnt == 0){
      len = ds_get_packet(sh->ds, &buf);
      if (len<0)
         return len;
      memcpy(sh->a_in_buffer, buf, len);
      pack.cnt = 1;
      pack.pack_offset = len;
      sh->a_in_buffer_len = len;
   }
   if (pack.cnt == 1){
      len = ds_get_packet(sh->ds, &buf);
      if (len<0)
         return len;
      memcpy(&sh->a_in_buffer[sh->a_in_buffer_len], buf, len);
      pack.cnt = 2;
      sh->a_in_buffer_len += len;
   }
   return 0;
}
# else
static int mp3_eof;
static int pool;

static int mp3_read_data(sh_audio_t *sh, int pool)
{
   int len=0;
   if (mp3_eof==0){
      len = demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len],
          pool-sh->a_in_buffer_len);
      if (len<=0){
         mp3_eof = 1;
         len = 0;
      }
   }
   return len;
}
static unsigned char const *stream_sync(unsigned char const *ptr, unsigned char const *end)
{
   unsigned char const *endp = end - 1;
   while (ptr < endp && !(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0))
      ++ptr;
   if (ptr >= endp)
      return 0;
   else
      return ptr;
}

# endif

#endif

typedef struct mad_decoder_s {

  struct mad_synth  synth; 
  struct mad_stream stream;
  struct mad_frame  frame;
  
  int have_frame;

  int               output_sampling_rate;
  int               output_open;
  int               output_mode;

} mad_decoder_t;

static void mad_init(mad_decoder_t *this)
{
   mad_synth_init  (&this->synth);
   mad_stream_init (&this->stream);
   mad_frame_init  (&this->frame);
}

static int preinit(sh_audio_t *sh)
{
   mad_decoder_t *this = (mad_decoder_t *) malloc(sizeof(mad_decoder_t));
   memset(this,0,sizeof(mad_decoder_t));
   sh->context = this;
  
  //mad_synth_init  (&this->synth);
  //mad_stream_init (&this->stream);
  //mad_frame_init  (&this->frame);
   mad_init(this);
#ifdef new_read_packet
# ifndef way2
   pack.cnt = 0;
# else
   mp3_eof = 0;
   pool = 128;
# endif
#endif

#ifndef  EngineMode  
   sh->audio_out_minsize=2*4608;
   sh->audio_in_minsize=4096;
#endif
   return 1;
}

static int read_frame(sh_audio_t *sh)
{
   mad_decoder_t *this = (mad_decoder_t *) sh->context;
   int len;
#ifdef new_read_packet
# ifndef way2
   int ret;
   do {
      if (pack.cnt>=2){
         this->have_frame=0;        //illegal, do resync
         mad_init(this);
         pack.cnt = 0;
      }         
      if (fill_mp3_packet(sh)<0)
         return 0;         //end of file
      
      mad_stream_buffer (&this->stream, sh->a_in_buffer, sh->a_in_buffer_len);
      ret=mad_frame_decode (&this->frame, &this->stream);
      len = sh->a_in_buffer_len-pack.pack_offset;
      memmove(sh->a_in_buffer, &sh->a_in_buffer[pack.pack_offset], len);
      sh->a_in_buffer_len = len;
      pack.pack_offset = len;
      pack.cnt = 1;
   }while (ret != 0);      
   return 1;      //OK
# else
   while(1){
      int ret;
      len=mp3_read_data(sh, pool);
      sh->a_in_buffer_len+=len;
             
      mad_stream_buffer (&this->stream, sh->a_in_buffer, sh->a_in_buffer_len);
      ret=mad_frame_decode (&this->frame, &this->stream);
      if (ret){
         if (this->stream.error<=MAD_ERROR_BUFLEN){
            if (mp3_eof == 1)
               break;
            //pool is not enough, enlarge
            if (this->stream.error == MAD_ERROR_BUFFRAME){
               pool = (int)((char*)this->stream.next_frame-(char*)sh->a_in_buffer);
               if (pool > sh->a_in_buffer_size){
                  pool = sh->a_in_buffer_size;     //放棄目前的buffer, 向下一buffer尋找
                  sh->a_in_buffer_len = 0;
               }
               continue;
            }
         }
         //data errer, do resync
         {
            unsigned const char *ptr = stream_sync(this->stream.next_frame, this->stream.bufend);
            if (ptr){
               int num_bytes = (int)((unsigned char*)this->stream.bufend - ptr);
	            memmove(sh->a_in_buffer, ptr, num_bytes);
	            sh->a_in_buffer_len = num_bytes;
            }
            else
               sh->a_in_buffer_len = 0;
         }
      }
      else{
	      int num_bytes = (int)((char*)sh->a_in_buffer+sh->a_in_buffer_len - (char*)this->stream.next_frame);
	      memmove(sh->a_in_buffer, this->stream.next_frame, num_bytes);
	      sh->a_in_buffer_len = num_bytes;
         return 1; // OK!!!
      }
   }
   //mp_msg(MSGT_DECAUDIO,MSGL_INFO,"Cannot sync MAD frame\n");
   return 0;   //end of mp3
# endif
#else
   while((len=demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len],
          sh->a_in_buffer_size-sh->a_in_buffer_len))>0){
      sh->a_in_buffer_len+=len;
      while(1){
         int ret;
         mad_stream_buffer (&this->stream, sh->a_in_buffer, sh->a_in_buffer_len);
         ret=mad_frame_decode (&this->frame, &this->stream);
         if (this->stream.next_frame) {
	         int num_bytes = (int)((char*)sh->a_in_buffer+sh->a_in_buffer_len - (char*)this->stream.next_frame);
	         memmove(sh->a_in_buffer, this->stream.next_frame, num_bytes);
	         sh->a_in_buffer_len = num_bytes;
         }
         if (ret == 0) 
            return 1; // OK!!!
    // error! try to resync!
         if((this->stream.error==MAD_ERROR_BUFLEN) || (this->stream.error==MAD_ERROR_BUFFRAME))
            break;
      }
   }
   //mp_msg(MSGT_DECAUDIO,MSGL_INFO,"Cannot sync MAD frame\n");
   return 0;
#endif
}

static int init(sh_audio_t *sh){
  mad_decoder_t *this = (mad_decoder_t *) sh->context;

  this->have_frame=read_frame(sh);
  if(!this->have_frame) 
     return 0; // failed to sync...
  
  sh->channels=(this->frame.header.mode == 0) ? 1 : 2;
  sh->samplerate=this->frame.header.samplerate;
  sh->i_bps=this->frame.header.bitrate/8;
  sh->samplesize=2;
  
  return 1;
}

static void uninit(sh_audio_t *sh){
  mad_decoder_t *this = (mad_decoder_t *) sh->context;
  //mad_synth_finish (&this->synth);
  mad_frame_finish (&this->frame);
  mad_stream_finish(&this->stream);
  free(sh->context);
}

#ifndef  EngineMode 

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen){
   mad_decoder_t *this = (mad_decoder_t *) sh->context;
   int len=0;

   while(len<minlen && len+4608<=maxlen){
      if(!this->have_frame) 
         this->have_frame=read_frame(sh);
      if(!this->have_frame) 
         break; // failed to sync... or EOF
      this->have_frame=0;

      mad_synth_frame (&this->synth, &this->frame);

	   { 
         unsigned int         nchannels, nsamples;
	      mad_fixed_t    *left_ch, *right_ch;
	      struct mad_pcm      *pcm = &this->synth.pcm;
	      unsigned short      *output = (unsigned short*) buf;

	      nchannels = pcm->channels;
	      nsamples  = pcm->length;
	      left_ch   = pcm->samples[0];
	      right_ch  = pcm->samples[1];

	      len+=2*nchannels*nsamples;
	      buf+=2*nchannels*nsamples;

         skmp3_32to16(left_ch, right_ch, output, nsamples, nchannels); 
	   }
   }
  
   return len?len:-1;
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...){
   mad_decoder_t *this = (mad_decoder_t *) sh->context;
    // various optional functions you MAY implement:
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
	      this->have_frame=0;
         mad_init(this);
#ifdef new_read_packet
# ifndef way2
         pack.cnt = 0;
# else
         mp3_eof = 0;
# endif
#endif
	      return CONTROL_TRUE;
      case ADCTRL_SKIP_FRAME:
         this->have_frame=read_frame(sh);
	   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static void* decode_audio(sh_audio_t *sh)
{
   mad_decoder_t *this = (mad_decoder_t *) sh->context;
   if(!this->have_frame) 
       this->have_frame=read_frame(sh);
   if (!this->have_frame)
      return 0;
   this->have_frame=0;
   mad_synth_frame (&this->synth, &this->frame);
   return &this->synth.pcm;
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skmp3 = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};

#endif

