// SAMPLE audio decoder - you can use this file as template when creating new codec!

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

static ad_info_t info =  {
	"Skyviia mpeg audio decoder",
	"samp3",
	"Honda",
	"Honda",
	"Skyviia copyright"
};

LIBAD_EXTERN(samp3)


#else

#include "mp3_mplayer.h"

#endif

#include "audio_server.h"

#define MP3_IN_SIZE 4096

extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;
static int mp3_eof;
static int pool;

static void uninit(sh_audio_t *sh);

static int mp3_read_data(sh_audio_t *sh, int pool)
{
   int len=0;
   if ((mp3_eof==0)&&(pool!=in_buf_len)) {
      len = demux_read_data(sh->ds, sm_in+in_buf_len, pool-in_buf_len);

//      if (sh->ds->pts != MP_NOPTS_VALUE)	//Fuchun 20110714 disable, because audio pts need to consider which the length of audio packet not be decoded yet.
//          sh->pts = sh->ds->pts;

      if (len<=0){
         if (sh->ds->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
         {
             mp3_eof = 1;
         }
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


static int preinit(sh_audio_t *sh)
{
	aserver_pid = 0;
#ifndef  EngineMode 
	union smc *sc;
	long curr_codec;
#endif
   mp3_eof = 0;
   pool = 128;
   in_buf_len = 0;
   in_buf_size = MP3_IN_SIZE;

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
	if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	
		return 0;
#endif
	      
	

	id = get_semaphore_id();
	
	if (!reset_semvalue(id, 0)){
		return 0;
 	}

	sc = (union smc *)sm_com;

	sc->magic.magic_num = PLAYER_MAGIC;
	curr_codec = MP3_ID;

	sc->magic.codec_id = curr_codec;
	if (!semaphore_v(id, 1))
		return 0;	

	if (!wait_server(sc)){
		deallocate_share_mem(sm_com);	
#ifndef AD_SHM_REWRITE
		shmctl(sm_com_id, IPC_RMID, 0);
#endif
		del_semvalue(id, 0);
		printf("Audio server crash \n");
		return 0;
	}	

	if (sc->magic.codec_id != curr_codec){
		printf("Unsuppoted codec\n");
		return 0;		
	}
	
   sh->audio_out_minsize=2*4608;
   sh->audio_in_minsize=64;  //useless
#endif

	aserver_pid = sc->pinit.pid;

   return 1;
}


static int error_resync(int ret, int nfo)
{
   if (ret<=2){
      if (mp3_eof == 1)
         return 1;   
            //pool is not enough, enlarge
      if (ret == 1){
         pool = nfo;
         if (pool > in_buf_size){
            pool = in_buf_size;     //放棄目前的buffer, 向下一buffer尋找
            in_buf_len = 0;
         }
         return 2;   
      }
   }
         //data errer, do resync
   {
      unsigned const char *ptr = stream_sync(sm_in+nfo, sm_in+in_buf_len);
      if (ptr){
         int num_bytes = (int)((unsigned char*)sm_in+in_buf_len - ptr);
	      memmove(sm_in, ptr, num_bytes);
	      in_buf_len = num_bytes;
      }
      else
         in_buf_len = 0;
   }
   return 0;
}

static int frame_init(sh_audio_t *sh, aduio_info *ai)
{
   int len;
   int init = 1;
   union smc *sc = (union smc *)sm_com;

   while(1){
      int ret;
      int nfo;
      play_init *pi = &sc->pinit;
      serv_init *si = &sc->sinit;
      len=mp3_read_data(sh, pool);
      in_buf_len+=len;

      pi->paremeter1 = init;

      if(!ipc_init(sc, MP3_ID, in_buf_len, id, sm_data_id, MP3_IN_SIZE))
			break;		//IPC Error 
      if (si->codec_id == MP3_ID)
         ret = 0;
      else
         ret = si->codec_id;   //error code

      init = 0;
      nfo = si->paremeter1;     //next_frame_offset
      if (ret){
         int tmp = error_resync(ret, nfo);
         if (tmp == 1)
            break;
         else if (tmp == 2)
            continue;
      }
      else{
	      int num_bytes = (int)(in_buf_len - nfo);
			*ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif
	      memmove(sm_in, sm_in+nfo, num_bytes);
	      in_buf_len = num_bytes;
         return 1; // OK!!!
      }
   }
   uninit(sh);       //clear allocated mp3 memory in audio server
   return 0;   //end of mp3
}

//synth = 1 : do synthese
static int decode_mp3_frame(sh_audio_t *sh, int *olen, int synth)
{
   int len;
   union smc *sc = (union smc *)sm_com;
   while(1){
      int ret;
      int nfo;
      len=mp3_read_data(sh, pool);
      in_buf_len+=len;
      sc->pdecod.paremeter1 = synth;
#if 1
		ret = ipc_decode(sc, MP3_ID, in_buf_len, id);
		if(!ret)
			break;
		else if(ret < 0)
			return -1;
#else
      if (!ipc_decode(sc, MP3_ID, in_buf_len, id))
			break;		//IPC Error 
#endif
      if (sc->sctrl.codec_id == MP3_ID){
         ret = 0;
         *olen = sc->sctrl.data_len;   
      }
      else
         ret = sc->sctrl.codec_id;     //error code
      nfo = sc->sctrl.paremeter1;      //next_frame_offset
      
      if (ret){
         int tmp = error_resync(ret, nfo);
         if (tmp == 1)
            break;
         else if (tmp == 2)
            continue;
      }
      else{
	      int num_bytes = (int)(in_buf_len - nfo);
	      memmove(sm_in, sm_in+nfo, num_bytes);
	      in_buf_len = num_bytes;
         return 1; // OK!!!
      }
   }
   return 0;   //end of mp3
}

static int init(sh_audio_t *sh){
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, MP3_IN_SIZE);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif

  if(!frame_init(sh, ai))
     return 0; // failed to sync...
  //this->have_frame = 1;
  if (!sh->channels)
  	sh->channels = ai->ch;
  if (!sh->samplerate)
  	sh->samplerate=ai->samplerate;
  if (!sh->i_bps)	//Barry 2010-12-02
  	sh->i_bps=ai->bitrate/8;
  sh->samplesize=2;
  
  return 1;
}

static void uninit(sh_audio_t *sh){
   union smc *sc = (union smc *)sm_com;
   ipc_uninit(sc, MP3_ID, id);
#ifndef  EngineMode
	deallocate_share_mem(sm_in);
	deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE
	shmctl(sm_data_id, IPC_RMID, 0);
#endif
	deallocate_share_mem(sm_com);
#endif
}

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen){
   int len=0;

   while(len<minlen && len+4608<=maxlen){
      int det;
      int ret = decode_mp3_frame(sh, &det, 1);
      if(!ret)  //do synthese
         break; 
      else if(ret < 0)
	  return -1;
      memcpy(buf, sm_out, det);
      len += det;
      buf += det;
   }
   return len?len:-1;
}


#ifndef  EngineMode 

static int control(sh_audio_t *sh,int cmd,void* arg, ...){
   union smc *sc = (union smc *)sm_com;
	int tmp;
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         ipc_ctrl(sc, MP3_ID, 1, id);
         mp3_eof = 0;
	      return CONTROL_TRUE;
      case ADCTRL_SKIP_FRAME:
         decode_mp3_frame(sh, &tmp, 0);   //do not synthese
	   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else


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

