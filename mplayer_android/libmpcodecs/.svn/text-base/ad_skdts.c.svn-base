

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>



#ifndef  EngineMode 

#include <assert.h>
#include "config.h" 
#include "mp_msg.h"
#include "ad_internal.h"
#include "skdts.h"

static ad_info_t info =
{
    "Skyviia DTS core decoder",
    "skdts",
    "Honda",
    "Honda",
    "Skyviia copyright"
};

LIBAD_EXTERN(skdts) 

#define mp_msg_p2 mp_msg

#else
extern int g_frame;
#include "dts_mplayer.h"
#include <skdts.h>

#define mp_msg(x, y, z) printf(z)
#define mp_msg_p2(x, y, z, a, b) printf(z,a,b)

static unsigned char AMODE_ch[] = {
   1, 2, 2, 2, 2, 3, 3, 4, 4, 5
};

static void record_pcm32(int *inbuf, sh_audio_t *sh, int flags)
{
   int ch = AMODE_ch[flags&DTS_CHANNEL_MASK];
   
   if (flags & DTS_LFE)
      ch++;
   memcpy(&sh->pcm32[sh->pcm32_len], inbuf, 256*sizeof(int)*ch);
   sh->pcm32_len += 256*ch; 
}


#endif
static unsigned char *buf_in;
//#define SAVE_ES
#ifdef SAVE_ES
static FILE *fp;
#endif
extern unsigned int dts_packet ;
extern int audio_output_channels;
static int sync;
#define DTSBUFFER_SIZE 18726
#define HEADER_SIZE 14
#define CONVERT_LEVEL 1
#define CONVERT_BIAS 0
#define INIT_RETRY 3


static const signed char ch2flags[6] = {
    DTS_MONO,
    DTS_STEREO,
    DTS_3F,
    DTS_2F2R,
    DTS_3F2R,
    DTS_3F2R | DTS_LFE
};



static void channels_info(int flags)
{
    int lfe = 0;
    signed char lfestr[5] = "";

    if (flags & DTS_LFE) {
        lfe = 1;
        strcpy(lfestr, "+lfe");
    }
    mp_msg(MSGT_DECAUDIO, MSGL_V, "DTS: ");
    switch(flags & DTS_CHANNEL_MASK){
    case DTS_MONO:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "1.%d (mono%s)", lfe, lfestr);
        break;
    case DTS_CHANNEL:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "2.%d (channel%s)", lfe, lfestr);
        break;
    case DTS_STEREO:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "2.%d (stereo%s)", lfe, lfestr);
        break;
    case DTS_3F:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "3.%d (3f%s)", lfe, lfestr);
        break;
    case DTS_2F2R:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "4.%d (2f+2r%s)", lfe, lfestr);
        break;
    case DTS_3F2R:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "5.%d (3f+2r%s)", lfe, lfestr);
        break;
    case DTS_3F1R:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "4.%d (3f+1r%s)", lfe, lfestr);
        break;    
    default:
        mp_msg_p2(MSGT_DECAUDIO, MSGL_V, "x.%d (unknown%s)", lfe, lfestr);
    }
    mp_msg(MSGT_DECAUDIO, MSGL_V, "\n");
}

#define Readbufferlen 95-1   // spec


static int dts_sync(sh_audio_t *sh, int *flags)
{
   
   void *s = sh->context;
   int length;
   int sample_rate;
   int frame_length;
   int bit_rate;
   //CH add
   int fillin_ptr=0;     
   int consumed_ptr = 0 ; 
   int residue_num = 0 ; 
   int mvlen = 0;
   int c ; 

   if (dts_packet==0){
      sh->a_in_buffer_len=0;
      sh->a_in_buffer_len = demux_read_data(sh->ds, (unsigned char *)sh->a_in_buffer, HEADER_SIZE);
      residue_num = fillin_ptr = sh->a_in_buffer_len  ; 

#ifdef SAVE_ES 
   fwrite(sh->a_in_buffer, 1, HEADER_SIZE, fp);
#endif   
      if (sh->a_in_buffer_len < HEADER_SIZE)
         return -1;

      while(1) {
         int c;
         length = skdts_syncinfo(s, (unsigned char *)sh->a_in_buffer + consumed_ptr, flags, &sample_rate,
                              &bit_rate, &frame_length, HEADER_SIZE,consumed_ptr);

         if(length >= HEADER_SIZE)
            break;


    
          consumed_ptr++; //move to next byte
          residue_num = fillin_ptr - consumed_ptr ;
          if (residue_num<HEADER_SIZE)//¤£¨¬14 bytes
          {
            memmove(sh->a_in_buffer, sh->a_in_buffer + consumed_ptr, residue_num); //13
            mvlen = Readbufferlen -residue_num; //mvlen¥²»Ý<=min {length}
            consumed_ptr = 0 ; 
            fillin_ptr = residue_num; 

            c = demux_read_data(sh->ds, &sh->a_in_buffer[fillin_ptr], mvlen);
           if(c <= 0) 
              return -1; //EOF  sync fail!
#ifdef SAVE_ES 
   fwrite(&sh->a_in_buffer[fillin_ptr], 1, c, fp);
#endif  
              fillin_ptr += c;

            residue_num = fillin_ptr ;
          }

            



      }

      //sync ok ! 
      memmove(sh->a_in_buffer,sh->a_in_buffer+consumed_ptr, residue_num );
      sh->a_in_buffer_len = residue_num ; 
      c = demux_read_data(sh->ds, sh->a_in_buffer + residue_num, length - residue_num) ; //¸É»ô
         if( c!=(length - residue_num))
         return -1; 

      sh->a_in_buffer_len = length ;//2010.08.16 DTSHEADER_SIZE;
      buf_in = sh->a_in_buffer;  


     
#ifdef SAVE_ES 
      fwrite( sh->a_in_buffer + residue_num, 1, length - residue_num, fp);
#endif
   }
   else{
      unsigned char* bufptr=NULL;
      int *psync;
      int buflen=ds_get_packet(sh->ds, &bufptr);
      if(buflen<=0) 
         return -1;
      psync = (int*)bufptr;         
      if (sync==0){
         int retry = INIT_RETRY;
         while (1){
            sync = *psync; 
            length = skdts_syncinfo(s, bufptr, flags, &sample_rate,
                              &bit_rate, &frame_length, HEADER_SIZE,0);   
            if (length>buflen || length < HEADER_SIZE){
               retry--;
               if (retry < 0)
                  return -1;
               buflen=ds_get_packet(sh->ds, &bufptr);                  
               if(buflen<=0) 
                  return -1;     
               psync = (int*)bufptr;                                   
            }
            else
               break;                   
         }                                      
      }
      else{
         do{
            while (sync != *psync){
               buflen=ds_get_packet(sh->ds, &bufptr);   
               if(buflen<=0) 
                  return -1; 
               psync = (int*)bufptr;                   
            }     
            length = skdts_syncinfo(s, bufptr, flags, &sample_rate,
                              &bit_rate, &frame_length, HEADER_SIZE,0);      
            if (length>buflen || length < HEADER_SIZE){
               buflen=ds_get_packet(sh->ds, &bufptr);   
               psync = (int*)bufptr;  
               if (buflen<0)
                  return -1;                            
            }    
            else
               psync = NULL;                 
         }                      
         while(psync);  
      }            
      buf_in = bufptr;
   }                         
   sh->samplerate = sample_rate;
   sh->i_bps = bit_rate/8;

   return length;
}

static int decode_audio(sh_audio_t *sh, unsigned char *buf, int minlen, int maxlen)
{
   void *s = sh->context;
   short *out_samples = (short*)buf;
   int flags;
   int nblocks;
   int i;
   int data_size = 0;
   while(data_size < minlen ){
      if(buf_in == NULL)
         if(dts_sync(sh, &flags) < 0) 
            return -1; /* EOF */
            
      flags &= ~(DTS_CHANNEL_MASK | DTS_LFE);
      flags |= ch2flags[sh->channels - 1];

      flags |= DTS_ADJUST_LEVEL;
      if(skdts_frame(s, (unsigned char *)buf_in, &flags, HEADER_SIZE)) {
         mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts_frame() failed\n");
         buf_in = NULL;
         break;
      }

      nblocks = skdts_blocks_num(s);
      for(i = 0; i < nblocks; i++) {
         if(skdts_block(s)) {
            mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts_block() failed\n");
            buf_in = NULL;
            break;
         }
#ifdef  EngineMode
         record_pcm32((int *)skdts_samples(s), sh, flags);               
#endif
         skdts_convert2s16(skdts_samples(s), out_samples, flags, sh->channels);

         out_samples += 256 * sh->channels;
         data_size += 256 * sizeof(short) * sh->channels;         
      }
      buf_in = NULL;
   }      

   return data_size;
}

static int preinit(sh_audio_t *sh)
{
    /* 256 = samples per block, 16 = max number of blocks */
    sh->audio_out_minsize = audio_output_channels * sizeof(short) * 256 * 16;
    sh->audio_in_minsize = DTSBUFFER_SIZE;
    sh->samplesize=2;

    return 1;
}

static int init(sh_audio_t *sh)
{
    void *s;
    int flags;
    int decoded_bytes;
#ifdef SAVE_ES  
   fp = fopen("es.dts","wb");
#endif    
    buf_in = NULL;
    sync = 0;
    //printf("packet mode : %d \n", dts_packet);
    s = skdts_init();
    if(s == NULL) {
        mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts_init() failed\n");
        return 0;
    }
    sh->context = s;
 
    if(dts_sync(sh, &flags) < 0) {
         mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts sync failed\n");
         skdts_free(s);
         return 0;
    } 
             
    channels_info(flags);

    //assert(audio_output_channels >= 1 && audio_output_channels <= 6);
    sh->channels = audio_output_channels;

    decoded_bytes = decode_audio(sh, (unsigned char *)sh->a_buffer, 1, sh->a_buffer_size);
    if(decoded_bytes > 0)
        sh->a_buffer_len = decoded_bytes;
    else {
        mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts decode failed on first frame (up/downmix problem?)\n");
        skdts_free(s);
        return 0;
    }

    return 1;
}

static void uninit(sh_audio_t *sh)
{
    void *s = sh->context;

    skdts_free(s);
#ifdef SAVE_ES  
   fclose(fp);
#endif    
}

#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    int flags;

    switch(cmd){
        case ADCTRL_RESYNC_STREAM:
            if (dts_packet==0)
               dts_sync(sh, &flags);
            return CONTROL_TRUE;
    }
    return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skdts = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};

#endif


