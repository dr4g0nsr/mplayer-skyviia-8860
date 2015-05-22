/*
//------------------------------------------------------------------------------------------------------
//   skyviia wma audio decoder   
//     
//------------------------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ad_internal.h"






static ad_info_t info = 
{
	"wma audio decoders",
	"sawma",
	"Jack Yang",
	"Jack Yang",
	""
};





#ifndef  EngineMode  
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>   
#include "skipc.h" 
#include <assert.h>
#include "mp_msg.h"


   LIBAD_EXTERN(sawma)

#else

   LIBAD_EXTERN(skwma)

   int *pcm32out;
#endif 

#include "audio_server.h"



//extern int audio_output_channels;略

extern unsigned char* sm_com;extern unsigned char* sm_in; extern unsigned char* sm_out;  
static int sm_com_id=-1, sm_data_id=-1, id=-1;static int in_buf_len;static int in_buf_size;








/*//------------------------------------------------------------------------------------------------------//     Allocate shraed memory ,set semaphore,wakeup audio server//    //------------------------------------------------------------------------------------------------------*/
static int preinit(sh_audio_t *sh)
{
      aserver_pid = 0;
         #ifndef  EngineMode 
	      union smc *sc;
	      long curr_codec;
	      #endif

         {
         sh->audio_out_minsize = 192000;
        // sh->audio_in_minsize = 14861+2048 ; //spec
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
	      curr_codec = WMA_ID;                                     //inform server what kind of coded to be used.

	      sc->magic.codec_id = curr_codec;                         
   	      if (!semaphore_v(id, 1))                                   //wakeup server!
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
	
   
         #endif

		 aserver_pid = sc->pinit.pid;

      return 1;
}



int WMA_frame_init(sh_audio_t *sh, aduio_info *ai,int buflen,int ch, int sample_rate, int bit_rate, int block_align, int format, int flags2){
    

  
         int init = 1 , ret;
         union smc *sc = (union smc *)sm_com;
         play_init *pi = &sc->pinit;
         serv_init *si = &sc->sinit;
      


         //send to server 
         // sh->channels = audio_output_channels;
         

         //some parameters to server for ini   
          pi->WMAinfo.ch = ch;
          pi->WMAinfo.sample_rate = sample_rate ;
          pi->WMAinfo.bit_rate = bit_rate ;
          pi->WMAinfo.block_align = block_align ; 
          pi->WMAinfo.format = format ;
          pi->WMAinfo.flags2 = flags2; 

         //pi->paremeter3 = audio_output_channels ;           
         
       
         if(!ipc_init(sc, WMA_ID, buflen, id, sm_data_id, 0)) //init server
			    return -1;//break;		//IPC Error 
         if (si->codec_id == WMA_ID)
            ret = 0;
         else
            ret = si->codec_id;   //error code

         init = 0;
     
			*ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

         return ret; // OK!!!
     

}





/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory,initialize audio server 
//    
//------------------------------------------------------------------------------------------------------
*/
static int init(sh_audio_t *sh_audio)
{
 
   int flags2;
   int ch, sample_rate, bit_rate, block_align, format;
   int  buf_len = 0 ;
   int  ret ; 





    //for audio server
   aduio_info gai;
   aduio_info *ai = &gai;
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, 14861+2048);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif
#endif

    //output_channels = audio_output_channels;






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




   ret=WMA_frame_init(sh_audio,ai,buf_len,ch, sample_rate, bit_rate, block_align, format, flags2); 
   if (ret==-1)
      return ret ; 
    




   // Decode at least 1 byte:  (to get header filled)
   //x=decode_audio(sh_audio,sh_audio->a_buffer,1,sh_audio->a_buffer_size);
   //if(x>0) sh_audio->a_buffer_len=x;
   sh_audio->channels = ch;
   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   sh_audio->samplesize = 2;

   return 1;
}



/*
//-----------------------------------------------------------------------------------------
//    free WMA decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static void uninit(sh_audio_t *sh)
{
   

      union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, WMA_ID, id);

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


#ifndef EngineMode
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
#else
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int *out,int minlen,int maxlen)
#endif
{
    
  

   int nsamples = 0;  
   int len2=0;
   unsigned char *bufptr=NULL;
   int x,y;

   union smc *sc = (union smc *)sm_com;     //  shared memory
 
   double pts;

    x = ds_get_packet_pts( sh_audio->ds, &bufptr, &pts);

   if(x<=0) 
      return -1; // error
   if (pts != MP_NOPTS_VALUE) {
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }

     memcpy(sm_in,bufptr, x); 


#ifdef  EngineMode   
       pcm32out = out;  //32 bit audio out 
#endif 


#if 1
		int ret;
		ret = ipc_decode(sc, WMA_ID, x, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;
#else
   if (!ipc_decode(sc, WMA_ID, x, id))
	      return 0 ;		      //IPC Error 
#endif
    if (sc->sctrl.codec_id == WMA_ID) //decode success!
           nsamples = sc->sctrl.data_len;   
            
          y = sc->sctrl.paremeter1 ;


           
   
    memcpy( buf, sm_out , nsamples);  // sm_out -->16 bit wav
             


     //get y from audio server 

   if(y < x) 
      sh_audio->ds->buffer_pos+=y-x;  // put back data (HACK!)
   return nsamples;
}









static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   union smc *sc = (union smc *)sm_com;     //  shared memory 
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
      ipc_ctrl(sc, WMA_ID, 1, id);
      //wma_resync();
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}
