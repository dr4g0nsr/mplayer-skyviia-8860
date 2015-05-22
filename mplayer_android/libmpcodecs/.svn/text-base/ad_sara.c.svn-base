#include <stdio.h>
#include <stdlib.h>
#ifndef  EngineMode 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>    
#include "skipc.h" 
#endif

#include "config.h"
#include "ad_internal.h"

#include "audio_server2.h"

static int sm_com_id=-1, sm_data_id=-1, id=-1;
extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static ad_info_t info = 
{
	"Skyviia Real Audio Decoder",
	"sara",
	"Honda Hsu",
	"Honda Hsu",
	"Skyviia Copyright"
};

LIBAD_EXTERN(sara)

//======================= for decoder interface =====================//

static int preinit(sh_audio_t *sh)
{
	aserver_pid = 0;
#ifndef  EngineMode 
	union smc *sc;
	long curr_codec;
#endif
   sh->audio_out_minsize = 192000;
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

	id = get_semaphore_id2();	
	if (!reset_semvalue(id, 0)){
		return 0;
 	}

	sc = (union smc *)sm_com;
	sc->magic.magic_num = PLAYER_MAGIC;
	curr_codec = COOK_ID;

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
#endif

	aserver_pid = sc->pinit.pid;

   return 1;
}
extern int audio_output_channels;
#define GECKO_VERSION               ((1L<<24)|(0L<<16)|(0L<<8)|(3L))
#define GECKO_MC1_VERSION           ((2L<<24)|(0L<<16)|(0L<<8)|(0L))
static int init(sh_audio_t *sh_audio)
{
   int flags2;
   int ch, sample_rate, bit_rate, block_align;
   union smc *sc = (union smc *)sm_com;
   play_init *pi = &sc->pinit;
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_IN_SIZE_RA);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif

   flags2 = 0;
   sh_audio->codecdata_len = sh_audio->wf->cbSize;
   ch = sh_audio->wf->nChannels;
   sample_rate = sh_audio->wf->nSamplesPerSec;
   bit_rate = sh_audio->wf->nAvgBytesPerSec * 8;
   block_align = sh_audio->wf->nBlockAlign;

   //sm_in(sh_audio->wf + sizeof(WAVEFORMATEX), pi->data_len(sh_audio->codecdata_len)
   //pi->paremeter1(block_align<<3), pi->paremeter2(ch), pi->paremeter3(sample_rate)
   memcpy(sm_in, (char *)sh_audio->wf + sizeof(WAVEFORMATEX), sh_audio->codecdata_len);
   pi->paremeter1 = block_align<<3;
   pi->paremeter2 = ch;
   pi->paremeter3 = sample_rate;
   if(!ipc2_init(sc, COOK_ID, sh_audio->codecdata_len, id, sm_data_id, SM_IN_SIZE_RA))
      return -1;		//IPC Error 
   if (si->codec_id == COOK_ID)
      ret = 0;
   else
      ret = si->codec_id;   //error code   

   if(ret < 0)
      return -1;

   if(audio_output_channels != 0)
   {
      if(audio_output_channels <= ch)
      {
         ch = audio_output_channels;
      }
   }

#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

   // Decode at least 1 byte:  (to get header filled)
   //if(x>0) sh_audio->a_buffer_len=x;
   sh_audio->channels = ch;
   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   sh_audio->samplesize = 2;

   return 1;
}

static void uninit(sh_audio_t *sh)
{
   union smc *sc = (union smc *)sm_com;
   ipc2_uninit(sc, COOK_ID, id);
#ifndef  EngineMode
	deallocate_share_mem(sm_in);
	deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE
	   shmctl(sm_data_id, IPC_RMID, 0);
#endif	
	deallocate_share_mem(sm_com);
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

static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
{
   unsigned char *start=NULL;
   int y;
   union smc *sc = (union smc *)sm_com;
   int len2 = 0;
   double pts;
   int x=ds_get_packet_pts(sh_audio->ds,&start, &pts);
   
   if(x<=0) 
      return -1; // error
   //if (pts != MP_NOPTS_VALUE) {
   if  (pts != MP_NOPTS_VALUE && pts != 0) {	//Barry 2010-12-29
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }

   //pi->data_len(x), pi->paremeter1(sh_audio->channels)
   sc->pdecod.paremeter1 = sh_audio->channels;
   memcpy(sm_in, start, x);
#if 1
		int ret;
		ret = ipc2_decode(sc, COOK_ID, x, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;
#else
   if (!ipc2_decode(sc, COOK_ID, x, id))
	   return -1;		//IPC Error 
#endif
   //si->paremeter1(y), si->data_len(len2)
   len2 = sc->sctrl.data_len;   
   y = sc->sctrl.paremeter1;
   memcpy(buf, sm_out, len2);

   if(y < x) 
      sh_audio->ds->buffer_pos+=y-x;  // put back data (HACK!)
   sh_audio->pts_bytes += len2;
   return len2;
}
