/*
//------------------------------------------------------------------------------------------------------
//    multi-channel mp3 on mp4 decoder Application Interface
//    
//------------------------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h>

#ifndef  EngineMode 
#include "config.h"
#include "ad_internal.h"

static ad_info_t info = 
{
	"Skyviia MchMP3 decoder",
	"skmchmp3",
	"Larry",
	"Larry",
	"Skyviia copyright"
};

LIBAD_EXTERN(skmchmp3)
#else


#include "stream1.h"
#include "demuxer.h"
#include "stheader.h"
#include "mchmp3_mplayer.h"

#endif

#include "skmp3.h"
#include "skmchmp3.h"
#include "mchmp3dec.h" 





extern int audio_output_channels;
MchMP3DecodeContext  *gMch=NULL; //global 

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

static int preinit(sh_audio_t *sh)
{
          sh->audio_out_minsize= AVCODEC_MAX_AUDIO_FRAME_SIZE;
       
   return 1;
}




/*
//------------------------------------------------------------------------------------------------------
//     decoder initialization  
//    
//------------------------------------------------------------------------------------------------------
*/
static int output_channels;
static int mp3Frames[16] = {0,1,1,2,3,3,4,5,2};   // number of mp3 decoder instances 
static int mp3Channels[16] = {0,1,2,3,4,5,6,8,4}; // total output channels 



static int init(sh_audio_t *sh)
{       
        int ret ; 
     
         gMch =(MchMP3DecodeContext *) malloc(sizeof(MchMP3DecodeContext));


           //get channel configure
         gMch->chan_cfg = (sh->codecdata[1] >> 3) & 0x0f; //ch config   
         gMch->frames = mp3Frames[gMch->chan_cfg];  
         gMch->channels = mp3Channels[gMch->chan_cfg];    //channel number 


         ret =decode_init_mp3on4(gMch->frames,gMch); 
         

         output_channels = audio_output_channels;
  
         sh->channels = output_channels;
         //sh->samplerate = faac_samplerate;
         sh->samplesize=2;

   return 1;
}




/*
//------------------------------------------------------------------------------------------------------
//    decode multi-ch mp3  
//    
//------------------------------------------------------------------------------------------------------
*/
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
          int len = 0;	     
          unsigned short   *output = (unsigned short*) buf;
          unsigned int      nsamples;
          int buf_size,buflen;
          

		   while(len<3){
          //unsigned char decoderdonfig= *(sh->codecdata);                                    //esds.decoderdonfig
        

          //------- [get mp3on4 packet from demuxer]----------
          unsigned char* bufptr=NULL;                                  
          double pts;
          buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);                                //buflen = packet size ,114,1180,,check ok !
          if(buflen<=0) 
          return 0;//
         
          if (pts != MP_NOPTS_VALUE) {   
	         sh->pts = pts;
	         sh->pts_bytes = 0;
          }




      
          //-----------[mp3on4 decode procedure] ------------
          //gMch           : mp3on4 
          //output sample  : pcm out
          //nsamples       : total output sample #
          //bufptu         : one packet (all channels)
          //buflen         : packet length 
          //output_channels:audio out ch # 
          buf_size = mchmp3_synth_frame( gMch,  output,  &nsamples,  bufptr,  buflen,output_channels);     
		    if (!buf_size) len++;
		    else break ; 
           
		  }
   
   return  nsamples;
}




/*
//------------------------------------------------------------------------------------------------------
//    free multi-ch mp3  
//    
//------------------------------------------------------------------------------------------------------
*/
static void uninit(sh_audio_t *sh)
{
  
   int i,st;
   
      st = gMch->frames;
      if (gMch != NULL){
      for (i = 0; i < st; i++) {
       mad_frame_finish (&gMch->mp3decctx[i]->frame);
       mad_stream_finish(&gMch->mp3decctx[i]->stream);
      }
            
       free(gMch);
      }
      gMch = NULL;
   
     
}



//------------------------------------------------------------------------------------
#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...){
   mchmp3_decoder_t *this = (mchmp3_decoder_t *) sh->context;
    // various optional functions you MAY implement:
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
	     // this->have_frame=0;
        // mad_init(this);

	      return CONTROL_TRUE;
      case ADCTRL_SKIP_FRAME:
         //this->have_frame=read_frame(sh);
	   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_mchmp3 = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};

#endif

