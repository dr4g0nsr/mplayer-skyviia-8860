
/*
//------------------------------------------------------------------------------------------------------
//   skyviia AC3  decoder    
//     
//------------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifndef  EngineMode  

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>    
#include "skipc.h" 

#include "config.h"
#include "ad_internal.h"
#include "mpbswap.h"

static ad_info_t info = 
{
	"Skyviia Dolby AC-3 Decoder",   
	"saac3",                 
	"Honda",                
	"Honda",              
	"Skyviia copyright"       
};

LIBAD_EXTERN(saac3)     // register mp_saac3 codec

static void swab_16(void* dst, void* src, int len)
{
   unsigned short *d16, *s16;
   unsigned short t;
   int i;
   d16 = (unsigned short *)dst;
   s16 = (unsigned short *)src;
   len >>= 1;
   for (i=0; i<len; i++){
      t = *s16++;
      t = (t>>8) + ((t&0xff)<<8);
      *d16++ = t;
   }
}
#else


#include "ac3_mplayer.h"

sh_audio_t *sh_v ; //驗證用only

#endif






#include "audio_server.h"


extern int audio_output_channels;


extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;  

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;

//--------------------------------------------------------------------





int ac3_syncinfo (unsigned char * buf, int * flags,
		  int * sample_rate, int * bit_rate)
{
   static int rate[] = { 32,  40,  48,  56,  64,  80,  96, 112,
			               128, 160, 192, 224, 256, 320, 384, 448,
	      		         512, 576, 640};
   static unsigned char lfeon[8] = {0x10, 0x10, 0x04, 0x04, 0x04, 0x01, 0x04, 0x01};
   static unsigned char halfrate[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3};
   int frmsizecod;
   int bitrate;
   int half;
   int acmod;
   int bsid; 

   if ((buf[0] != 0x0b) || (buf[1] != 0x77))	/* syncword */
	   return 0;

   bsid = buf[5] >> 3;
   if (bsid > 16)		
	   return 0;      //illegal bsid   
   if (bsid > 10){
      if (buf[2] >= 0xc0)
         return 0;   // Not E-AC-3 format
      else
         return -1;        //E-AC-3 format           
   }      	   
   half = halfrate[bsid];

   /* acmod, dsurmod and lfeon */
   acmod = buf[6] >> 5;
   *flags = ((((buf[6] & 0xf8) == 0x50) ? 10 : acmod) |
	         ((buf[6] & lfeon[acmod]) ? 16 : 0));

   frmsizecod = buf[4] & 63;
   if (frmsizecod >= 38)
	   return 0;
   bitrate = rate [frmsizecod >> 1];
   *bit_rate = (bitrate * 1000) >> half;

   switch (buf[4] & 0xc0) 
   {
      case 0:
	      *sample_rate = 48000 >> half;
	      return 4 * bitrate;
      case 0x40:
	      *sample_rate = 44100 >> half;
	      return 2 * (320 * bitrate / 147 + (frmsizecod & 1));
      case 0x80:
	      *sample_rate = 32000 >> half;
	      return 6 * bitrate;
      default:
	      return 0;
   }
}




#define DRC_NO_ACTION      0
#define DRC_NO_COMPRESSION 1
#define DRC_CALLBACK       2

static int ac3_level = 1;
//static int ac3_drc_action = DRC_NO_ACTION;

static   int syn = 0 ; 

#define Readbufferlen 128 // biterate*4
//------------------------------------------------------------------------------------------------------
static int ac3_fillbuff(sh_audio_t *sh_audio)
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
         c = demux_read_data(sh_audio->ds,(sm_in + fillin_ptr), mvlen);//read audio data to sm_in     
         if(c <= 0) 
         return -1; // EOF
         fillin_ptr += c;//mvlen 
       
      }
      residue_num = fillin_ptr - consumed_ptr ;
     


  
#if 0 //for simulation only 
      if (syn==0) {
          *sm_in = 0xff;
          syn=1;
      }
#endif 

      //if(sh_audio->format!=0x2000) 
		//	swab_16(sm_in,sm_in,8);

     


      if(sh_audio->format==0x2000) 
        length = a52_syncinfo (sm_in+ consumed_ptr, &flags, &sample_rate, &bit_rate);
      else 
        length = a52_syncinfo_NOT2000 (sm_in + consumed_ptr, &flags, &sample_rate, &bit_rate);

	  if (length < 0)
         return -1;        //E-AC-3 format
      if(length>=7 && length<=3840) 
         break; /* we're done.*/

      //do resync here
      /* bad file => resync*/
      //if(sh_audio->format!=0x2000) 
		//	swab_16(sm_in,sm_in,8);

      consumed_ptr++;//move to next byte
      residue_num = fillin_ptr - consumed_ptr ;


     if (residue_num<8) //不足8 bytes  
     {
      memmove(sm_in, sm_in + consumed_ptr, 7);
      mvlen = Readbufferlen -7; //mvlen必需<=min {length}
      consumed_ptr = 0 ; 
      fillin_ptr = 7; 
     }


   }

  if(sh_audio->format!=0x2000) 
	swab_16(sm_in+ consumed_ptr,sm_in+ consumed_ptr,8);

   memmove(sm_in, sm_in+consumed_ptr, residue_num );
   sh_audio->a_in_buffer_len = residue_num ; 


   sh_audio->samplerate = sample_rate;
   sh_audio->i_bps = bit_rate/8;
   
   sh_audio->samplesize = sh_audio->sample_format==AF_FORMAT_FLOAT_NE ? 4 : 2;
  

   demux_read_data(sh_audio->ds,sm_in +residue_num, length -residue_num);//補齊

   if(sh_audio->format!=0x2000)
	   swab_16(sm_in+8,sm_in+8,length-8);  
  

   //2010.08.05 crc16_block(sm_in+2, length-2);
    
   return length;
}





//-----------------------------------------------------------------------------------------------------

















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
	      sh->audio_out_minsize=audio_output_channels*sh->samplesize*256*6; //max 18432
          sh->audio_in_minsize = 3840; 
         }


         if (sh->samplesize < 2) 
         sh->samplesize = 2;	// at least 2 bytes in a word

         ac3_level = 1 << 26;          // Q24



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
	      curr_codec = AC3_ID;                                     //inform server what kind of coded to be used.

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




int AC3_frame_init(sh_audio_t *sh, aduio_info *ai,int buflen){
    

  
         int init = 1 , ret;
         union smc *sc = (union smc *)sm_com;
         play_init *pi = &sc->pinit;
         serv_init *si = &sc->sinit;
      
 

         //send to server 
         //memcpy(sm_in,sh->a_in_buffer, buflen);   //bistream  to sm_in

         sh->channels = audio_output_channels;
         
         //some parameters to server for ini
         pi->AC3info.sh_data_len = buflen;      
         pi->AC3info.sh_samplerate = sh->samplerate ; 
         pi->AC3info.sh_samplesize = sh->samplesize ; 
         pi->AC3info.sample_format = sh->sample_format  ;  
         pi->paremeter1 = ac3_level;
         pi->paremeter3 = audio_output_channels ;           //send channel number to server 
       
         if(!ipc_init(sc, AC3_ID, buflen, id, sm_data_id, 0)) //init server
			    return 0;//break;		//IPC Error 
         if (si->codec_id == AC3_ID)
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
       int output_channels;
       int ret_init=0;
       int buflen ; 


      
       
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, 3840);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif
#endif

       output_channels = audio_output_channels;
       buflen = ac3_fillbuff(sh) ;  // data length 
	    if(buflen<0)
     		return 0;

       ret_init = AC3_frame_init(sh,ai,buflen) ;  //ai:audio info
   

       //i_bps,channels,smplerate get from ac3_fillbuff()
      
       //sh->i_bps = ai->sh_i_bps ; 
       //sh->channels = ai->ch;;
       //sh->samplerate = ai->samplerate;
       //sh->samplesize=2;
     


      return 1;
}



/*
//-----------------------------------------------------------------------------------------
//    free AC3 decoder  
//    
//-----------------------------------------------------------------------------------------
*/
static void uninit(sh_audio_t *sh)
{
      union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, AC3_ID, id);

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
//    AC3 decoder  
//    
//-----------------------------------------------------------------------------------------
*/
#ifdef EngineMode
static int decode_audio(sh_audio_t *sh, short *pcm)
#else
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
#endif
{
 
   
    int len=-1 ;
    int   nsamples = 0,buflen = 0,data_len = 0;
    union smc *sc = (union smc *)sm_com;     //  shared memory 

    buflen = sh->a_in_buffer_len ;

    if(!sh->a_in_buffer_len) {

       buflen = ac3_fillbuff(sh) ; 
	    if(buflen < 0 ) 
          return len; /* EOF */
    }
	 sh->a_in_buffer_len=0;

       
    #ifdef  EngineMode  
    sh_v = sh ; 
    #endif 
           
          sc->pdecod.paremeter1 = 0; //send to server (reserved)
          //memcpy(sm_in,sh->a_in_buffer, buflen); //  bistream  to sm_in 
#if 1
		int ret;
		ret = ipc_decode(sc, AC3_ID, buflen, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;
#else
          if (!ipc_decode(sc, AC3_ID, buflen, id))
			 return 0 ;//break;		//IPC Error 
#endif
          if (sc->sctrl.codec_id == AC3_ID){ //decode success!
            nsamples = sc->sctrl.data_len;     //get from server 
          }






#ifdef  EngineMode      
                   
		    // sm_out -->16 bit wav
            memcpy(&pcm[0], sm_out, nsamples*2);   
            data_len = nsamples ; 
          
#else
           // sm_out -->  buf(synthesis output buffer) 
           memcpy(buf,sm_out, sh->samplesize*nsamples); 
           data_len = nsamples <<1;
         
#endif

 
    	return data_len;
 
   
}
//--------------------------------------------------------------------------

#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{   
   // union smc *sc = (union smc *)sm_com;
    switch(cmd)
    {
      case ADCTRL_RESYNC_STREAM:
         // ipc_ctrl(sc, AC3_ID, 1, id);
           
	 return CONTROL_TRUE;
    }
  return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skac3 = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};

#endif



