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
//#include "macros.h"

#define MAX_BUFFER_OUT  8192*8
#include "audio_server2.h"

static ad_info_t info = 
{
	"SKYVIIA WMA Professional Audio Decoder",
	"sawmapro",
	"Honda",
	"Honda",
	""
};

LIBAD_EXTERN(sawmapro)

extern int audio_output_channels;

static int BitPackedFrameSize;
static int fs_status;

static int seq_num;

static int sm_com_id=-1, sm_data_id=-1, id=-1;

extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;


static int fill_packet(sh_audio_t *sh_audio, unsigned char *sm_buf)
{
   int len=0;
   int in_packet=0;
   double pts;
   int x;
   unsigned char *start;   
   int blocklen = sh_audio->wf->nBlockAlign;

   {
      do{
         int pktbit;
         int pktlen;
         int num;
         x = ds_get_packet_pts(sh_audio->ds, &start, &pts);
         
         if(x <= 0){ 
            if (len)
               break;
            else
               return -1; // error
         }

         if(blocklen < x){ 
            sh_audio->ds->buffer_pos += blocklen-x; 
            x = blocklen;
         }
         pktbit = 6;
         pktlen = 0;        
         
         while(pktbit < BitPackedFrameSize){
            int byte = pktbit>>3;
            int bit = 7-(pktbit & 7);
            byte = start[byte];
            pktlen <<= 1;
            if (byte & (1<<bit))
               pktlen++;
            pktbit++;
         }
         num = start[0]>>4;
         if (SM_IN_SIZE_WMAPRO<(len+x)){
            printf("SA_WAMPRO: Input Buffer Size Not Enough.\n");
            return -1;
         }  

         seq_num++; 
         if ((seq_num >=0) && (seq_num&0xf)!= num)
            in_packet = 1;

         seq_num = num;
         memcpy(sm_buf+len, start, x);
         len += x;

         if ((pktlen+BitPackedFrameSize) < blocklen*8)
            in_packet = 1;
      }
      while(in_packet==0);
   }
#if 0 //20120120 charleslin - mantis 5685 audio pts updates every 2sec and av sync causes lag
   if (pts != MP_NOPTS_VALUE) {
      sh_audio->pts = pts;
      sh_audio->pts_bytes = 0;
   }
#endif

   return len;
}

//======================= for decoder interface =====================//

static int preinit(sh_audio_t *sh)
{
	aserver_pid = 0;
#ifndef  EngineMode 
	union smc *sc;
	long curr_codec;
#endif
   sh->audio_out_minsize = MAX_BUFFER_OUT*2;

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
	curr_codec = WMAPRO_ID;

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


static int init(sh_audio_t *sh_audio)
{
   int wValidBitsPerSample, wBitsPerSample;
   int ch, sample_rate, bit_rate, block_align, format;
   unsigned char *x;
   short nEncodeOpt, dwChannelMask, wAdvancedEncodeOptint;
   int  dwAdvancedEncodeOpt2;
   int ret=0,flag,nDstChannelMask=0;

   union smc *sc = (union smc *)sm_com;   
   play_init *pi = &sc->pinit;
   serv_init *si = &sc->sinit;
   wmapro_initpara *wpinit = &pi->pg.wpinit;
   aduio_info *ai = &si->ai;

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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_IN_SIZE_WMAPRO);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif

   sh_audio->codecdata_len = sh_audio->wf->cbSize;
   sample_rate = sh_audio->wf->nSamplesPerSec;
   bit_rate = sh_audio->wf->nAvgBytesPerSec * 8;
   block_align = sh_audio->wf->nBlockAlign;
   format = sh_audio->wf->wFormatTag;
   ch = sh_audio->wf->nChannels;
   wValidBitsPerSample = sh_audio->wf->wBitsPerSample;
   wBitsPerSample = 8 * ((wValidBitsPerSample + 7)/8);

	if (sample_rate<=0 
		    || ch<=0 || ch>8
		    || bit_rate<=0)
      return -1;

   x = (unsigned char *)sh_audio->wf + 20;
   dwChannelMask = (x[1] << 8) | x[0];
   x += 8;
   dwAdvancedEncodeOpt2 = (x[3] << 24) | (x[2] << 16) | (x[1] << 8) | x[0];
   x += 4;
   nEncodeOpt = (x[1] << 8) | x[0];
   x += 2;
   wAdvancedEncodeOptint = (x[1] << 8) | x[0];
   
   flag = 0;
   if(audio_output_channels != 0)
   {
      if(dwChannelMask != 0x3 && 2 == audio_output_channels)
      {
         flag = 1;
         nDstChannelMask = 3;
      }
   }

   flag = flag | 0x40; 
   if (sample_rate > 48000)
      flag = flag | 0x8;

   wpinit->flag = flag;
   wpinit->nDstChannelMask = nDstChannelMask;
   wpinit->sample_rate = sample_rate;
   wpinit->ch = ch;
   wpinit->bit_rate = bit_rate;
   wpinit->wValidBitsPerSample = wValidBitsPerSample;
   wpinit->format = format;
   wpinit->dwChannelMask = dwChannelMask;
   wpinit->nEncodeOpt = nEncodeOpt;
   wpinit->block_align = block_align;
   wpinit->wBitsPerSample = wBitsPerSample;
   wpinit->wAdvancedEncodeOptint = wAdvancedEncodeOptint;
   wpinit->dwAdvancedEncodeOpt2 = dwAdvancedEncodeOpt2;
 
   if(!ipc2_init(sc, WMAPRO_ID, 0, id, sm_data_id, SM_IN_SIZE_WMAPRO))
      return -1;		//IPC Error 
   if (si->codec_id == WMAPRO_ID)
      ret = 0;
   else
      ret = si->codec_id;   //error code   
   
   if(ret != 0)
      return -1;

#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

   sh_audio->samplesize = si->paremeter1; 
   BitPackedFrameSize = si->paremeter2;
   BitPackedFrameSize += 6;

   sh_audio->channels = ai->ch;
   sh_audio->samplerate = ai->samplerate;
   sh_audio->i_bps = bit_rate/8;

   seq_num = -2;


   fs_status = 1;
   return 1;
}

static void uninit(sh_audio_t *sh)
{
   union smc *sc = (union smc *)sm_com;
   ipc2_uninit(sc, WMAPRO_ID, id);
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
   union smc *sc = (union smc *)sm_com;
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
      seq_num = -2;
      fs_status = 1;
      ipc2_ctrl(sc, WMAPRO_ID, ADCTRL_RESYNC_STREAM, id);
      
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}


static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf, int minlen,int maxlen)
{
   unsigned char *start=NULL;
   int len2=0;
   int x;
   int len = 0;	
   union smc *sc = (union smc *)sm_com;

   while(len < minlen ) {
      if (fs_status>0){
         x = fill_packet(sh_audio, sm_in);
      }
      else{
         x = 0;
      }

      sc->pdecod.paremeter1 = fs_status;
#if 1
		int ret;
		ret = ipc2_decode(sc, WMAPRO_ID, x, id);
		if(!ret)
			break;
		else if(ret < 0)
			return -1;
#else
      if (!ipc2_decode(sc, WMAPRO_ID, x, id))
			break;		//IPC Error 
#endif
      if (sc->sctrl.codec_id != WMAPRO_ID){
         fs_status = 1;
         printf("Failed to decode packet\n");
         break;         
      } 
      fs_status = sc->sctrl.paremeter1;     
      len2 = sc->sctrl.data_len;   

      if (len2<0)
         return -1;
      
   //if(sh_audio->samplesize == 24)
   //   len2 = len2*3*sh_audio->channels;      
   //else
      len2 = len2*2*sh_audio->channels;
      memcpy(buf, sm_out, len2);

      len += len2;
      buf += len2;       
   }
   return len;
}
