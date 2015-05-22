/* ad_faad.c - MPlayer AAC decoder using libfaad2
 * This file is part of MPlayer, see http://mplayerhq.hu/ for info.  
 * (c)2002 by Felix Buenemann <atmosfear at users.sourceforge.net>
 * File licensed under the GPL, see http://www.fsf.org/ for more info.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef  EngineMode 

#include "config.h"
#include "ad_internal.h" 

static ad_info_t info = 
{
   "Skyviia AMR-WB decoder",
   "saamrwb",       //compare to sh_audio->codec->drv
   "Honda",
   "Honda",
   "Skyviia copyright"
};

LIBAD_EXTERN(saamrwb)
#include "skipc.h"
#else

#include "mp_msg.h"
#include "help_mp.h"
#include "amrwb_mplayer.h"

#endif


#include "audio_server.h"
#define MAX_AMRWB_IN_LEN 61
extern unsigned char* sm_com;
extern unsigned char* sm_in; 
extern unsigned char* sm_out;  
static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buffer_len;
static int preinit(sh_audio_t *sh)
{      
#ifndef  EngineMode    
   union smc *sc;
	long curr_codec;
   aserver_pid = 0;

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
	curr_codec = AMRWB_ID;                                     //inform server what kind of coded to be used.

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
	aserver_pid = sc->pinit.pid;
#endif
	sh->audio_in_minsize = 8;     //useless	 
   sh->audio_out_minsize = 640;
   return 1;
}
	
static int init(sh_audio_t *sh)
{
   union smc *sc = (union smc *)sm_com;
   serv_init *si = &sc->sinit;   //pi, si為重疊區域, 須先操作pi, 後si
   int ret; 

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
	sm_data_id = get_shared_mem_id(KEY_SM_IN,MAX_IN_LEN+4);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif

   if(!ipc_init(sc, AMRWB_ID, 0, id, sm_data_id, 0)) //init server
	   return -1;//break;		//IPC Error 
   if (si->codec_id == AMRWB_ID)
      ret = 0;
   else
      ret = si->codec_id;   //error code
   
   //if (AMRWB_INIT()<0)
   if (ret<0)
      return -1;     //get error code
      
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif      
      
   sh->channels = 1;
   sh->samplerate = 16000;
   sh->samplesize = 2;
   in_buffer_len = 0;
   return 1;

}

static void uninit(sh_audio_t *sh)
{ 
   //AMRWB_UNINIT();
   union smc *sc = (union smc *)sm_com;
   ipc_uninit(sc, AMRWB_ID, id);

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

static int decode_audio(sh_audio_t *sh, unsigned char *buf,int minlen,int maxlen)
{
   int len = -1;	
   union smc *sc = (union smc *)sm_com;     //  shared memory

   while(len < minlen ) {
      int len2=0;
      int inlen, clen, ret;

      inlen = MAX_AMRWB_IN_LEN - in_buffer_len;
      inlen = demux_read_data(sh->ds, sm_in+in_buffer_len, inlen);  
      in_buffer_len += inlen;
      if(in_buffer_len<=0) 
         break;     //EOF 

      //len2 = AMRWB_DECODER(sm_in, &clen, (short*)buf);
		ret = ipc_decode(sc, AMRWB_ID, in_buffer_len, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;

      clen = sc->sctrl.paremeter1 ;    //consumed data length
      if ((clen>in_buffer_len) || (sc->sctrl.codec_id != AMRWB_ID)){
         mp_msg(MSGT_DECAUDIO,MSGL_V,"AMRWB: Decode Error.\n");
         in_buffer_len = 0;
         break;      //Error
      }
      len2  = sc->sctrl.data_len;   //output data length
      memcpy(buf, sm_out, len2);

      in_buffer_len -= clen;
      memmove(sm_in, sm_in+clen, in_buffer_len);

	   if(len2>0){
	      // first decoding
         if(len<0) 
            len=len2; 
         else 
	         len+=len2;
	      buf+=len2;
	   }
   }
   return len;
}

#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
      in_buffer_len = 0;
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skamrwb = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif




