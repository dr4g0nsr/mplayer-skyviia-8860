/*
//------------------------------------------------------------------------------------------------------
//   skyviia AMRNB audio decoder   
//     
//------------------------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#ifndef  EngineMode 
#include "config.h"
#include "ad_internal.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>   
#include "skipc.h" 
#include <assert.h>
#include "mp_msg.h"



static ad_info_t info = 
{
   "Skyviia  AMR-NB decoder",
   "saamrnb",       //compare to sh_audio->codec->drv
   "Archin",
   "Archin",
   "Skyviia copyright"
};

LIBAD_EXTERN(saamrnb)
#else

#include "mp_msg.h"
#include "help_mp.h"
#include "amrnb_mplayer.h" 

typedef unsigned char uint8_t;
#define MP_NOPTS_VALUE (-1LL<<63)

#endif




#include "audio_server.h"


typedef unsigned char UWord8;
typedef signed char Word8;
typedef signed short Word16;







extern unsigned char* sm_com;
extern unsigned char* sm_in; 
extern unsigned char* sm_out;  
static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;








/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
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
         sh->audio_out_minsize = 320; 

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
	      curr_codec = AMRNB_ID;                                     //inform server what kind of coded to be used.

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



int AMRNB_frame_init( aduio_info *ai, unsigned char *codecdata ,int codecdata_len ){
    
         int init = 1 , ret;
         union smc *sc = (union smc *)sm_com;
         play_init *pi = &sc->pinit;
         serv_init *si = &sc->sinit;
      


         //send to server 
         //some parameters to server for ini     
         //pi->paremeter3 = audio_output_channels ; 


          //memcpy(sm_in,codecdata, codecdata_len); 
       
         if(!ipc_init(sc, AMRNB_ID, codecdata_len, id, sm_data_id, 0)) //init server
	    return -1;//break;		//IPC Error 
         if (si->codec_id == AMRNB_ID)
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
//-----------------------------------------------------------------------------------------
//    free AMRNB decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static void uninit(sh_audio_t *sh)
{
   
union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, AMRNB_ID, id);

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





/*
//-----------------------------------------------------------------------------------------
//     AMRNB decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{

      int j = 0, len = -1;	
      union smc *sc = (union smc *)sm_com;     //  shared memory


   

   while(len < minlen ) {
      int y;
      int len2=0;
      unsigned char* bufptr=NULL;
      double pts;
      int buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);

      if(buflen<=0) 
         break;     //EOF
      if (pts != MP_NOPTS_VALUE) 
      {
	      sh->pts = pts;
	      sh->pts_bytes = 0;
      }




      memcpy(sm_in,bufptr, buflen); 
#if 1
		int ret;
		ret = ipc_decode(sc, AMRNB_ID, buflen, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;
#else
      if (!ipc_decode(sc, AMRNB_ID, buflen, id))
	      return 0 ;		      //IPC Error 
#endif
      if (sc->sctrl.codec_id == AMRNB_ID) //decode success!
           len2  = sc->sctrl.data_len;   
            
          y = sc->sctrl.paremeter1 ;
         // sh->channels = sc->sctrl.paremeter2;
         // sh->samplerate = sc->sctrl.paremeter3;   
       memcpy( buf, sm_out , len2);  // sm_out -->16 bit wav





      if(y<0)
	   { 
	      mp_msg(MSGT_DECAUDIO,MSGL_V,"AMR_audio: error\n");
	      break; 
	   }
      if(y<buflen) 
	      sh->ds->buffer_pos+=y-buflen;  // put back data (HACK!)
	   if(len2>0){
	      // first decoding
         if(len<0) 
            len=len2; 
         else 
	         len+=len2;
	      buf+=len2;
	      sh->pts_bytes += len2;
	   }
      mp_dbg(MSGT_DECAUDIO,MSGL_DBG2,"Decoded %d -> %d  \n",y,len2);
   }



 

    return len;
}





/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory,initialize audio server 
//    
//------------------------------------------------------------------------------------------------------
*/
static int init(sh_audio_t *sh)
{

   int ret; 
   //for audio server
   union smc *sc = (union smc *)sm_com;
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN,320 );
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif


 
   unsigned char *codecdata = NULL;
   int codecdata_len = 0;




   ret = AMRNB_frame_init(ai,codecdata,codecdata_len);
   if (ret==-1)
      return ret ; 
    



    //sh->channels =  sc->sctrl.paremeter2;
    //sh->samplerate = sc->sctrl.paremeter3;
    //sh->a_buffer_len = decoded_bytes;

 
   return 1;
}






#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    union smc *sc = (union smc *)sm_com;     //  shared memory 
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         ipc_ctrl(sc, AMRNB_ID, 1, id);
         //avcodec_flush_buffers(lavc_context);
     
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skamrnb = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif

