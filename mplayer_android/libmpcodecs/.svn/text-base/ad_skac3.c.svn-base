#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "a52.h"

#ifndef  EngineMode  

//#define SAVE_ES
#ifdef SAVE_ES
static FILE *fp;
#endif

#include "config.h"
#include "ad_internal.h"
#include "mpbswap.h"

static ad_info_t info = 
{
	"Skymedi Dolby AC-3 Decoder",   
	"skac3",                   // short name <liba52>
	"Jack Yang",               // maintainer
	"Duncan Lee",              // author
	"Skymedi copyright"        // comment 
};

LIBAD_EXTERN(skac3)           // register mp_skac3 codec


#else

#include "ac3_mplayer.h"
#include "a52_internal.h"

static void record_pcm32(int *inbuf, sh_audio_t *sh, a52_state_t *state_t)
{
   static unsigned char ACMode_Channel[]= { 2, 1, 2, 3, 3, 4, 4, 5};
   // int ch = ACMode_Channel[state_t->acmod - 1] + state_t->lfeon;
   int ch = ACMode_Channel[state_t->acmod] + state_t->lfeon;
   
   //if (flags & DTS_LFE)
   //   ch++;
   memcpy(&sh->pcm32[sh->pcm32_len], inbuf, 256*sizeof(int)*ch);
   sh->pcm32_len += 256*ch;   
}


extern int audio_output_channels;
#endif

int SW_DIALNORM = 1;

static a52_state_t *a52_state;
static unsigned int a52_flags=0;
/** Used by a52_resample_float, it defines the mapping between liba52
 * channels and output channels.  The ith nibble from the right in the
 * hex representation of channel_map is the index of the source
 * channel corresponding to the ith output channel.  Source channels are
 * indexed 1-6.  Silent output channels are marked by 0xf. */
static unsigned int channel_map;

#define DRC_NO_ACTION      0
#define DRC_NO_COMPRESSION 1
#define DRC_CALLBACK       2


/** The output is multiplied by this var.  Used for volume control */
static sample_t a52_level = 1;
/** The value of the -a52drc switch. */
static float a52_drc_level = 1.0;
static int a52_drc_action = DRC_NO_ACTION;
//extern int audio_output_channels;
static   int syn = 0 ; 

#define Readbufferlen 128 // biterate*4
static int a52_fillbuff(sh_audio_t *sh_audio)
{
   int length=0;
   int flags=0;
   int sample_rate=0;
   int bit_rate=0;
	int mvlen = 8;
 
   int fillin_ptr=0;     //CH add
   int consumed_ptr = 0 ; //CH add
   int residue_num = 0 ; //CH add 

   int c = 0 ; 

   /* sync frame:*/
   while(1)
   {
     
      if (residue_num<8)
      {
      int c = demux_read_data(sh_audio->ds, &sh_audio->a_in_buffer[fillin_ptr], mvlen);
      if(c <= 0) 
         return -1; // EOF
         fillin_ptr += c;//mvlen 
      }
      residue_num = fillin_ptr - consumed_ptr ;
#ifdef SAVE_ES
      fwrite(&sh_audio->a_in_buffer[consumed_ptr], 1, c, fp);
#endif

#if 0 //for simulation only 
      if (syn==0) {
          *sh_audio->a_in_buffer = 0xff;
          syn=1;
      }
#endif 
      //if(sh_audio->format!=0x2000) 
		//	swab_16(sh_audio->a_in_buffer+ consumed_ptr,sh_audio->a_in_buffer+ consumed_ptr,8);

      if(sh_audio->format==0x2000) 
        length = a52_syncinfo (sh_audio->a_in_buffer + consumed_ptr, &flags, &sample_rate, &bit_rate);
      else 
        length = a52_syncinfo_NOT2000 (sh_audio->a_in_buffer + consumed_ptr, &flags, &sample_rate, &bit_rate);

      if (length < 0)
         return -1;        //E-AC-3 format   
      if(length>=7 && length<=3840) 
         break; /* we're done.*/

         //do resync here
      /* bad file => resync*/
      //if(sh_audio->format!=0x2000) 
		//	swab_16(sh_audio->a_in_buffer+ consumed_ptr,sh_audio->a_in_buffer+ consumed_ptr,8);

      consumed_ptr++;//move to next byte
      residue_num = fillin_ptr - consumed_ptr ;

   if (residue_num<8) //¤£¨¬8 bytes  
     {
      memmove(sh_audio->a_in_buffer, sh_audio->a_in_buffer + consumed_ptr, 7);
      mvlen = Readbufferlen -7; //mvlen¥²»Ý<=min {length}
      consumed_ptr = 0 ; 
      fillin_ptr = 7; 
     }

   }

  if(sh_audio->format!=0x2000) 
	swab_16(sh_audio->a_in_buffer+ consumed_ptr,sh_audio->a_in_buffer+ consumed_ptr,8);

   memmove(sh_audio->a_in_buffer,sh_audio->a_in_buffer+consumed_ptr, residue_num );
   sh_audio->a_in_buffer_len = residue_num ; 


   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   
   sh_audio->samplesize = sh_audio->sample_format==AF_FORMAT_FLOAT_NE ? 4 : 2;
   //sh_audio->samplesize = sh_audio->sample_format==AF_FORMAT_S32_LE ? 4 : 2;

   demux_read_data(sh_audio->ds, sh_audio->a_in_buffer + residue_num, length - residue_num);//¸É»ô
#ifdef SAVE_ES
      fwrite(sh_audio->a_in_buffer + residue_num, 1, length -residue_num, fp);
#endif
   if(sh_audio->format!=0x2000)
	   swab_16(sh_audio->a_in_buffer+8,sh_audio->a_in_buffer+8,length-8);  
  
   //if(crc16_block(sh_audio->a_in_buffer+2, length-2) != 0)
      //fprintf(stderr,"a52: CRC check failed!  \n");
   //2010.08.05 crc16_block(sh_audio->a_in_buffer+2, length-2);
    
   return length;
}

/* returns: number of available channels*/

//static int a52_printinfo(sh_audio_t *sh_audio)
//{
//   int flags, sample_rate, bit_rate;
//   char* mode="unknown";
//   int channels=0;
//   a52_syncinfo (sh_audio->a_in_buffer, &flags, &sample_rate, &bit_rate);
//   switch(flags&A52_CHANNEL_MASK)
//   {
//      case A52_CHANNEL: mode="channel"; channels=2; break;
//      case A52_MONO: mode="mono"; channels=1; break;
//      case A52_STEREO: mode="stereo"; channels=2; break;
//      case A52_3F: mode="3f";channels=3;break;
//      case A52_2F1R: mode="2f+1r";channels=3;break;
//      case A52_3F1R: mode="3f+1r";channels=4;break;
//      case A52_2F2R: mode="2f+2r";channels=4;break;
//      case A52_3F2R: mode="3f+2r";channels=5;break;
//      case A52_CHANNEL1: mode="channel1"; channels=2; break;
//      case A52_CHANNEL2: mode="channel2"; channels=2; break;
//      case A52_DOLBY: mode="dolby"; channels=2; break;
//   }
   //fprintf(stderr,"AC3: %d.%d (%s%s)  %d Hz  %3.1f kbit/s\n",
	//         channels, (flags&A52_LFE)?1:0,
	//         mode, (flags&A52_LFE)?"+lfe":"",
	//S         sample_rate, bit_rate*0.001f);
//   return (flags&A52_LFE) ? (channels+1) : channels;
//}

static sample_t dynrng_call (sample_t c, void *data) 
{
	return (sample_t) pow((double)c, a52_drc_level);
}

static int preinit(sh_audio_t *sh)
{
	/* Dolby AC3 audio: */
	/* however many channels, 2 bytes in a word, 256 samples in a block, 6 blocks in a frame */
	if (sh->samplesize < 2) 
      sh->samplesize = 2;	// at least 2 bytes in a word
	sh->audio_out_minsize=audio_output_channels*sh->samplesize*256*6;
   sh->audio_in_minsize = 3840; 
   //a52_level = 1 << A52_F_FRACBITS;          // Q24
   a52_level = 1 << 26;          // Q24
	return 1;
}

static int init(sh_audio_t *sh_audio)
{
	unsigned int a52_accel = 0;
	sample_t level = a52_level, bias = 384;
	int flags = 0;

#ifdef SAVE_ES  
   fp = fopen("es.ac3","wb");
#endif

#ifndef  EngineMode
   SW_DIALNORM = 1;
#endif

	a52_state = a52_init (SW_DIALNORM);      // in <ac3_parse.c>
	if (a52_state == NULL) 
   {
      //fprintf (stderr, "A52 init failed\n");
		return 0;
	}

	if(a52_fillbuff(sh_audio)<0)
   {
      //fprintf(stderr, "A52 sync failed\n");
		return 0;
	}

	/* Init a52 dynrng */
	if (a52_drc_level < 0.001) 
   {
		/* level == 0 --> no compression, init library without callback */
	   a52_drc_action = DRC_NO_COMPRESSION;
	} 
   else if (a52_drc_level > 0.999) 
   {
		   /* level == 1 --> full compression, do nothing at all (library default = full compression) */
		   a52_drc_action = DRC_NO_ACTION;
	} 
   else 
   {
		   a52_drc_action = DRC_CALLBACK;
	}
	/* Library init for dynrng has to be done for each frame, see decode_audio() */


	/* 'a52 cannot upmix' hotfix:*/
	// a52_printinfo(sh_audio);		// removed by Duncan, 02.12.09'
	sh_audio->channels = audio_output_channels;
	while(sh_audio->channels>0)
	{
	  	switch(sh_audio->channels)
		{
		   case 1: a52_flags=A52_MONO; break;
		/*	case 2: a52_flags=A52_STEREO; break;*/
			case 2: a52_flags=A52_DOLBY; break;
		/*	case 3: a52_flags=A52_3F; break;*/
			case 3: a52_flags=A52_2F1R; break;
			case 4: a52_flags=A52_2F2R; break; /* 2+2*/
			case 5: a52_flags=A52_3F2R; break;
			case 6: a52_flags=A52_3F2R|A52_LFE; break; /* 5.1*/
		}
	  	/* test:*/
	  	flags=a52_flags|A52_ADJUST_LEVEL;
	  	
	  	if (a52_frame (a52_state, sh_audio->a_in_buffer, &flags, &level, bias))
		{
            //fprintf(stderr, "a52: error decoding frame -> nosound\n"); 
	    		return 0;
	  	}
	  	
	  	/* frame decoded, let's init resampler:*/
	 	channel_map = 0;

      if (sh_audio->sample_format == AF_FORMAT_FLOAT_NE) 
      //if (sh_audio->sample_format == AF_FORMAT_S32_LE) 
		{
	     	if (!(flags & A52_LFE)) 
			{
				switch ((flags<<3) | sh_audio->channels) 
				{
					 case (A52_MONO    << 3) | 1: channel_map = 0x1; break;
					 case (A52_CHANNEL << 3) | 2:
					 case (A52_STEREO  << 3) | 2:
					 case (A52_DOLBY   << 3) | 2: channel_map =    0x21; break;
					 case (A52_2F1R    << 3) | 3: channel_map =   0x321; break;
					 case (A52_2F2R    << 3) | 4: channel_map =  0x4321; break;
					 case (A52_3F      << 3) | 5: channel_map = 0x2ff31; break;
					 case (A52_3F2R    << 3) | 5: channel_map = 0x25431; break;
				}
	      } else if (sh_audio->channels == 6) 
	      {
			  	switch (flags & ~A52_LFE) 
				{
					case A52_MONO   : channel_map = 0x12ffff; break;
					case A52_CHANNEL:
					case A52_STEREO :
					case A52_DOLBY  : channel_map = 0x1fff32; break;
					case A52_3F     : channel_map = 0x13ff42; break;
					case A52_2F1R   : channel_map = 0x1f4432; break;
					case A52_2F2R   : channel_map = 0x1f5432; break;
					case A52_3F2R   : channel_map = 0x136542; break;
			  	}
	      }
	      if (channel_map) 
			{
		  		// a52_resample = a52_resample_float;     // removed by duncan 
            //printf("This a52_resample_float function already stopping to execute !!!\n");
            system("PAUSE");                          // this path for floating point
		  		break;
	      }
	  	} 
		else
	  		if(a52_resample_init(a52_accel,flags,sh_audio->channels)) 
            break;
  		--sh_audio->channels; /* try to decrease no. of channels*/
	}
  	if(sh_audio->channels<=0)
	{
      //fprintf(stderr,"a52: no resampler. try different channel setup!\n");
    	return 0;
  	}
  	return 1;
}

static void uninit(sh_audio_t *sh)
{
    free(a52_state);           // added by duncan   
#ifdef SAVE_ES  
   fclose(fp);
#endif
}


// static int decode_audio(sh_audio_t *sh_audio, signed int *pcm)
#ifdef EngineMode
static int decode_audio(sh_audio_t *sh_audio, short *pcm)
#else
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
#endif
{
   sample_t level=a52_level, bias=384;
   int flags=a52_flags|A52_ADJUST_LEVEL;
   int i, len=-1;
   		
	if (sh_audio->sample_format == AF_FORMAT_FLOAT_NE)
	    	bias = 0;
	if(!sh_audio->a_in_buffer_len) 
	   if(a52_fillbuff(sh_audio)<0) 
         return len; /* EOF */
	sh_audio->a_in_buffer_len=0;

	if (a52_frame (a52_state, sh_audio->a_in_buffer, &flags, &level, bias))
   {
      //fprintf(stderr,"a52: error decoding frame\n");
	   return len;
	}
      
	/* handle dynrng */
	if (a52_drc_action != DRC_NO_ACTION) 
   {
	   if (a52_drc_action == DRC_NO_COMPRESSION)
		   a52_dynrng(a52_state, NULL, NULL);
	   else
		   a52_dynrng(a52_state, dynrng_call, NULL);
	}
         
	len=0;
	for (i = 0; i < 6; i++) 	// 6 - Audio Blocks
	{
      //if(a52_state->acmod == 7 /*&& a52_state->lfeon*/)
      //   a52_state->acmod = a52_state->acmod;
	   if (a52_block (a52_state))
		{
         //fprintf(stderr,"a52: error at resampling\n");
			break;
	   }
#ifdef  EngineMode      
      record_pcm32((int *)a52_samples(a52_state), sh_audio, a52_state);               
      // record_pcm32((int *)skdts_samples(s), sh, flags);               

	  // len+=2*a52_resample(a52_samples(a52_state),(short *)&buf[len]);
      len+=a52_resample(a52_samples(a52_state), &pcm[len]);
#else
      len+=a52_resample(a52_samples(a52_state), &buf[len<<1]);
#endif
      
	}
	//assert(len <= maxlen);
   //if (len <=0)
   //   len = len;
#ifdef  EngineMode 
  	return len;
#else
   return len<<1;
#endif
}
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}



#ifdef EngineMode
ad_functions_t mp_skac3 = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif


