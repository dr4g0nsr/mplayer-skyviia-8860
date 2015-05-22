/*
//------------------------------------------------------------------------------------------------------
//    multi-channel mp3 decoder audio server Interface
//    
//------------------------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef  EngineMode 

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>    
#include "skipc.h" 

#include "config.h"
#include "ad_internal.h"

static ad_info_t info = 
{
	"Skyviia MchMP3 decoder",
	"samchmp3",
	"Larry",
	"Larry",
	"Skyviia copyright"
};

LIBAD_EXTERN(samchmp3)
#else


#include "stream1.h"
#include "demuxer.h"
#include "stheader.h"
#include "mchmp3_mplayer.h"

#endif

#include "skmp3.h"
#include "skmchmp3.h"
#include "mchmp3dec.h"


#include "audio_server.h"


extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;
//--------------------------------------------------------------------
extern int audio_output_channels;
static unsigned char* bufptr=NULL;  
static int buflen ; 

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio




static int frame_init(sh_audio_t *sh, aduio_info *ai,int chan_config)
{
 
   int init = 1 , ret;
   union smc *sc = (union smc *)sm_com;


     

      play_init *pi = &sc->pinit;
      serv_init *si = &sc->sinit;
     //en=mp3_read_data(sh, pool);
     //n_buf_len+=len;

      pi->paremeter1 = init;
      pi->paremeter2 = chan_config ;          //send channel configuration to server 
      pi->paremeter3 = audio_output_channels ;          //send channel configuration to server 
      if(!ipc_init(sc, MCHMP3_ID, buflen, id, sm_data_id, 0))
			 return 0;//break;		//IPC Error 
      if (si->codec_id == MCHMP3_ID)
         ret = 0;
      else
         ret = si->codec_id;   //error code

      init = 0;
     
			*ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

         return 1; // OK!!!

}
/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory 
//    
//------------------------------------------------------------------------------------------------------
*/
static int preinit(sh_audio_t *sh)
{
	      aserver_pid = 0;

        #ifndef  EngineMode 
	      union smc *sc;
	      long curr_codec;
	      #endif
	     
	      
	      { 
	      
	       sh->audio_out_minsize= AVCODEC_MAX_AUDIO_FRAME_SIZE;
	      
	      }
	     
#ifndef  EngineMode 
#ifdef AD_SHM_REWRITE
	sm_com_id = get_first_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);
	if(sm_com_id < 0)
		sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if(sm_com == NULL)
	{
		if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command )
			return 0;
	}
#else
	sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command ) 
		return 0;
#endif

	      id = get_semaphore_id();                                 //get semaphore ID 
	
	      if (!reset_semvalue(id, 0)){                             //reset semaphore 
		      return 0;
 	      }

	      sc = (union smc *)sm_com;                     

	      sc->magic.magic_num = PLAYER_MAGIC;           
	      curr_codec = MCHMP3_ID;                                  //inform server what kind of coded to be used.

	      sc->magic.codec_id = curr_codec;                         
   	   if (!semaphore_v(id, 1))                                 //wakeup server!
		     return 0;	

	      if (!wait_server(sc)){                                    //wait for server's response
		      deallocate_share_mem(sm_com);	
#ifndef AD_SHM_REWRITE
		      shmctl(sm_com_id, IPC_RMID, 0);
#endif
		      del_semvalue(id, 0);
		      printf("Audio server crash \n");
		      return 0;
	      }	

	      if (sc->magic.codec_id != curr_codec){                   //check codec 
		       printf("Unsuppoted codec\n");
		       return 0;		
	      }
	
         sh->audio_out_minsize=2*4608;
         sh->audio_in_minsize=64;  //useless
         #endif

		 aserver_pid = sc->pinit.pid;
     
   return 1;
}




/*
//------------------------------------------------------------------------------------------------------
//     decoder initialization  
//    
//------------------------------------------------------------------------------------------------------
*/
static int output_channels;
static decoded_f = 0 ; 



static int init(sh_audio_t *sh)
{       
       
         aduio_info gai;
         aduio_info *ai = &gai;
         int chan_config ; 
                           
         double pts;
         chan_config = (sh->codecdata[1] >> 3) & 0x0f; //ch config   
         
#ifndef  EngineMode
#ifdef AD_SHM_REWRITE
	sm_data_id = get_first_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_data_id < 0)
		sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_in == NULL)
	{
		if ((sm_in = allocate_share_mem(sm_data_id))==NULL) 
		{
			printf("data share memory allocate fail\n");
			return 0;
		}
	}
#else
	sm_data_id = get_shared_mem_id(KEY_SM_IN, 4096);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif


          buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);                                //buflen = packet size ,114,1180,,check ok !
          if(buflen<=0) 
          return 0;//
         
         memcpy(sm_in,bufptr, buflen); //  bistream  to sm_in 

         
         if(!frame_init(sh, ai,chan_config))
         return 0; // failed to sync...

	as_channels = ai->ch;	//Fuchun 2011.05.18
          sh->channels = audio_output_channels;
          sh->samplerate=ai->samplerate;
          sh->i_bps=ai->bitrate/8;
          sh->samplesize=2;
          
          decoded_f = 1 ; 
           
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
          int errframe = 0  ; 
          int      nsamples = 0;
      
          union smc *sc = (union smc *)sm_com;     //  shared memory 

		   while(len<3){
          //unsigned char decoderdonfig= *(sh->codecdata);                                    //esds.decoderdonfig
        

          //------- [get mp3on4 packet from demuxer]----------
     
                                       
          double pts;
          if (!decoded_f){
          buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);                                //buflen = packet size ,114,1180,,check ok !
          if(buflen<=0) 
          return 0;//
         
          if (pts != MP_NOPTS_VALUE) {   
	         sh->pts = pts;
	         sh->pts_bytes = 0;
          }

                
  
           memcpy(sm_in,bufptr, buflen); //  bistream  to sm_in 
          }
       
          errframe = 0  ;     // error flag ; 
      
          //-----------[mp3on4 decode procedure] ------------
          //gMch           : mp3on4 
          //output sample  : pcm out
          //nsamples       : total output sample #
          //bufptu         : one packet (all channels)
          //buflen         : packet length 
          //output_channels:audio out ch # 
          //buf_size = mchmp3_synth_frame( gMch,  output,  &nsamples,  bufptr,  buflen,output_channels);  
#if 1
		int ret;
		ret = ipc_decode(sc, MCHMP3_ID, buflen, id);
		if(!ret)
			break;
		else if(ret < 0)
			return -1;
#else
          if (!ipc_decode(sc, MCHMP3_ID, buflen, id))
			  break;		//IPC Error 
#endif
          if (sc->sctrl.codec_id == MCHMP3_ID){ //decode success!
      
              nsamples = sc->sctrl.data_len;   
              memcpy( buf, sm_out,nsamples );  //copy sysnthesis data from sm_out(server)
         
           }else
               {
                  errframe =  sc->sctrl.codec_id  ; //decode error!!
        
               }
                    
            
           
  
		    if ( errframe ) 
             len++;
          else {
             decoded_f = 0 ;
             break ; 
          }
           
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
  

      
      union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, MCHMP3_ID, id);
      //server
      #ifndef  EngineMode
	    deallocate_share_mem(sm_in);
	    deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE
	    shmctl(sm_data_id, IPC_RMID, 0);
#endif
	    deallocate_share_mem(sm_com);
      #endif
   
     
}



//------------------------------------------------------------------------------------
#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...){
    union smc *sc = (union smc *)sm_com;
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

