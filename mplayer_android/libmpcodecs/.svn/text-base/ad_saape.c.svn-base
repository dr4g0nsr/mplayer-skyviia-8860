/*
//------------------------------------------------------------------------------------------------------
//   skyviia APE  decoder API    (support 8/16/24 bits) 
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
   "Skyviia APE decoder",
   "saape",       //compare to sh_audio->codec->drv
   "larry",
   "larry",
   "Skyviia copyright"
};

LIBAD_EXTERN(saape)
#else

#include "mp_msg.h"
#include "help_mp.h"
#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "ape_mplayer.h" 


#endif




#include "audio_server.h"




extern unsigned char* sm_com;
extern unsigned char* sm_in; 
extern unsigned char* sm_out;  
static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;



#define BLOCKS_PER_LOOP     4608  //max sample size per block 

#define csd_buf_size  BLOCKS_PER_LOOP*3*2  //max subframe csd size  
static unsigned char *phead,*ptail,*pfillin_comin;
static int csd_avail_num;
static int res_num ;

/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
//    
//------------------------------------------------------------------------------------------------------
*/
static int preinit(sh_audio_t *sh)
{
        #ifndef  EngineMode 
	      union smc *sc;
	      long curr_codec;
	      #endif

         {

        #ifndef  EngineMode 
        if(sh->samplesize==2)
      	{
   	      sh->sample_format=AF_FORMAT_S16_LE; //packing as 2 bytes
         
   	  }else  if(sh->samplesize==1)
   	  {
   	      sh->sample_format=AF_FORMAT_U8; //packing as 1 bytes
         
   	  }else if(sh->samplesize==4)
        {
    	 
    	     sh->sample_format=AF_FORMAT_S32_LE; //packing as 4 bytes
      
        }
   
        #endif 

         sh->audio_out_minsize = BLOCKS_PER_LOOP*2*sh->samplesize; 

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
	      curr_codec = APE_ID;                                     //inform server what kind of coded to be used.

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

         csd_avail_num = res_num =  0;
        

      return 1;
}










/*
//-----------------------------------------------------------------------------------------
//    free APE decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static void uninit(sh_audio_t *sh)
{
   
union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, APE_ID, id);

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
//     APE decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{

     int len = 0;	
     union smc *sc = (union smc *)sm_com;     //  shared memory
     int temp ;
   

int buflen = sh->a_in_buffer_len;
   int force = 0;


   while(len < minlen ) {
      int y;
      int len2=0;
      unsigned char* bufptr=NULL;
      double pts;
      int ret;

      if (buflen<=0){
         buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
         //  printf("\nbuflen=%d\n",buflen);
      if(buflen<=0){ 
        
         sh->a_in_buffer_len = 0;
         return -1;     //EOF
      }
    
        // if (buflen>0){            
		    //fill buffer
          //   |............csd...................|
          //  phead           buflen          ptail
          csd_avail_num = 0; //total available csd num
    		 phead = bufptr;
		    ptail = phead + buflen ;
	
            force = 1;
            if (pts != MP_NOPTS_VALUE) 
            {
	            sh->pts = pts;
	            sh->pts_bytes = 0;
            }
        // }
      }

      

         
     if (phead<=ptail  ){            
		    //fill buffer [csd_buf_size]
          
   		pfillin_comin = sm_in  ; 
			csd_avail_num = 0 ;
        
			while ( csd_avail_num < csd_buf_size && phead<ptail){
			   
			  	*pfillin_comin++ = *phead++; 
				csd_avail_num++;           //get sm_in[csd_avail_num]
               
			}
	 }


      
  
  
       //force flag :inform decoder to start subframe 
        sc->pdecod.paremeter1 = force; //send to server 

      ret = ipc_decode(sc, APE_ID, csd_avail_num, id);
      if (!ret)
	      return 0 ;		      //IPC Error 
      else if(ret < 0)
	  	return -1;
      if (sc->sctrl.codec_id == APE_ID) //decode success!
           len2  = sc->sctrl.data_len;   
          
	    //copy res data   
         y = sc->sctrl.paremeter1 ;//y: consume byetes
         res_num = csd_avail_num - (sc->sctrl.paremeter2 &0xfffffffc) ;
		   phead -= res_num ; //shift phead abide by residue
		 

  

         if(len2>0)      
         memcpy( buf, sm_out , len2);  // sm_out -->wav out 



      
      force = 0;
      if(y<0)
	   { 
	      mp_msg(MSGT_DECAUDIO,MSGL_V,"APE: error code %d\n", y);
         buflen = 0;
	      break; 
	   }

      //buflen -= y; //total-consumed
      
      temp = buflen ;
      temp-=y;
      if(0<temp<=-3)//because of res allignment,it maybe minus   
      {
         temp=0;
         // if(temp<0) printf("\n%d\n",temp);
      }
      else if (temp <=-4)  //X 
      {
         printf("error decode");
         break ;
      }
      buflen = temp ; 
      
	 
  	  
         len+=len2;
	      buf+=len2;
	      sh->pts_bytes += len2;

	  
   }


  

   sh->a_in_buffer_len = buflen;


 

    return len;
}



//--------------------------------------------------------------------------------------

int APE_frame_init( aduio_info *ai, unsigned char *codecdata ,int codecdata_len,int channels ,int bitpersample  ){
    
         int init = 1 , ret;
         union smc *sc = (union smc *)sm_com;
         play_init *pi = &sc->pinit;
         serv_init *si = &sc->sinit;
      


         //send to server 
         //some parameters to server for ini   
         pi->paremeter2 = channels ; 
         pi->paremeter3 = bitpersample ;  



          memcpy(sm_in,codecdata, codecdata_len); 
       
         if(!ipc_init(sc, APE_ID, codecdata_len, id, sm_data_id, 0)) //init server
	    return -1;//break;		//IPC Error 
         if (si->codec_id == APE_ID)
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN,128*1024*3 );
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}	
#endif
#endif


 
   unsigned char *codecdata = NULL;
   int codecdata_len = 0;




   ret = APE_frame_init(ai,sh->codecdata, sh->codecdata_len,sh->channels,sh->samplesize<<3);
   if (ret==-1)
      return ret ; 
    




   
    sh->a_in_buffer_len = 0;
 
   return 1;
}






#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    union smc *sc = (union smc *)sm_com;     //  shared memory 
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         ipc_ctrl(sc, APE_ID, 1, id);
     
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}


ad_functions_t mp_skape = { 
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif

