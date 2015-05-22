/*
//------------------------------------------------------------------------------------------------------
//   skyviia DTSHD2  decoder API    
//     
//------------------------------------------------------------------------------------------------------
*/
// LFE :FIR
// HD Parser ON 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define SAVE_ES
#ifdef SAVE_ES
static FILE *fp;
#endif

//SK DRC control on(100)/off 
extern unsigned int night_mode ;             

//#define SAVE_CORE
#ifdef SAVE_CORE
static FILE *fpcore;
#endif



#ifndef  EngineMode 
//{
#include <assert.h>

#include <assert.h>
#include "config.h" 
#include "ad_internal.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>   
#include "skipc.h" 
#include <assert.h>
#include "mp_msg.h"

#include "skdtshd2.h"


static ad_info_t info =
{
    "Skyviia DTS-HD2  decoder",
    "sadtshd2",
    "Larry",
    "Larry",
    "Skyviia copyright"
};

LIBAD_EXTERN(sadtshd2) 




//}
#else
//{

#include "skdtshd2.h"
#include "skdtshd2_mplayer.h"



//}
#endif

#include "audio_server2.h"


extern unsigned char* sm_com;
extern unsigned char* sm_in; 
extern unsigned char* sm_out;  
static int sm_com_id=-1, sm_data_id=-1, id=-1;



//#define MY_DEBUG 

//#define MY_DEBUG_D0_CHECKSUM 
#ifdef MY_DEBUG_D0_CHECKSUM 
long long check_sum=0;
#endif

extern int audio_output_channels;
static int output_ch ; 

#define _DEFINE_PRE_SYN_HD_2_CORE_  //pre-parsing hd extession to core 
int DTSHD_Parser = 0 ;   //1:HD parser ON , 0 : OFF(complete HD)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@/ user options /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int DRCPercent = 0 ; //0: off   33: light   66:medium     100:heavy
int SpkrOut = -1; 
int l1 = 0;           //LFE mix 
int nodialnorm = 0 ;  //off/on  dialnorm 

int EnableDownmix = 1;
int EnableAnalogCompensation = 0 ; 
int bass = 0; //bass management
//

int NEWSpkrOut = 0 ; 
int checksum=0;
int DRC_PARA[3]={0,50,100};
/*
0 =  off DRC
1 =  50% DRC 
2 = 100% DRC  

*/
int SpkrOut_PARA[15]={ 0,1,2,3,6,9,10,11,14,262144,262152,27,19,26,18 };
/*
0:-SpkrOut  0  
1:-SpkrOut  1     
2:-SpkrOut  2      
3:-SpkrOut  3      
4:-SpkrOut  6      
5:-SpkrOut  9      
6:-SpkrOut  10     
7:-SpkrOut  11     
8:-SpkrOut  14     
9:-SpkrOut  262144 
10:-SpkrOut  262152 
11:-SpkrOut   27
12:-SpkrOut   19
13:-SpkrOut   26
14:-SpkrOut   18
*/


int DTS_LFE_Filter_Type = 2 ;//2011.01.25 
/*
   --FilterType0       ( 32*32 FIR ) MP
   --FilterType1       ( 32*16 FIR )not used
   --FilterType2       ( IIR ) Cert.
*/


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//-------------------------------------------------
dtsBoolean dtsDecoderProcessSpkrOutCommand( int *pValue, skdtsPlayerConfig *pConfig);
dtsBoolean dtsDecoderProcessDRCCommand( int *pValue, skdtsPlayerConfig *pConfig);

#define DTSBUFFER_SIZE 32*1024//DTS_FILE_READ_BUFFER_SIZE_IN_BYTES; 
#define	DTS_NEED_MORE_DATA	 0
#define DTS_FINISH			-1

static dtsd_config_param_t	dts_param_info;
static int read_index = 0  ; 
static int bswp =  0;
static int file_end_flag = 0;

#define BITSTREAM_LOAD_BATCH		256
static unsigned char dts_data[DTSBUFFER_SIZE] ; 

static int ptr_read = 0;
static int sync_ready_len = 0;
static int header_flag = 0 ; 


//-------------------------------------------------------------------------------------------------------
#ifdef  _DEFINE_PRE_SYN_HD_2_CORE_

#define  DTSHEADER_SIZE 14
static int dts_syncword = 0;
static int (*p_syncinfo)(unsigned char*, int* , int*);

static int dts_header_len;
static unsigned char dts_header_buf[DTSHEADER_SIZE];

static const int dts_bit_rate[] =
{
    32000, 56000, 64000, 96000, 112000, 128000,
    192000, 224000, 256000, 320000, 384000,
    448000, 512000, 576000, 640000, 768000,
    896000, 1024000, 1152000, 1280000, 1344000,
    1408000, 1411200, 1472000, 1536000, 1920000,
    2048000, 3072000, 3840000, 1, 2, 3
};


static const int dts_sample_rates[] =
{
    0, 8000, 16000, 32000, 0, 0, 11025, 22050, 44100, 0, 0,
    12000, 24000, 48000, 96000, 192000
};





static int syncinfo_14BE(unsigned char *buf, int *sample_rate, int *bit_rate)
{
   int fsize, amode, sfreq, brate;
   if (buf[4] != 0x07 || (buf[5]&0xf0) != 0xf0)
      return 0;

   fsize = (buf[6] & 0x03)<<12 | buf[7]<<4 | (buf[8] & 0x3f)>>2;
   fsize++;
   fsize = fsize * 8 / 14 * 2;
   amode = (buf[8] & 0x03) << 4 | (buf[9] & 0xf0) >> 4;
   if (amode > 63)
      return 0;
   sfreq = buf[9] & 0x0f;
   brate = (buf[10] & 0x3f) >> 1;
   *sample_rate = dts_sample_rates[sfreq];
   if (*sample_rate == 0)
      return 0;
   *bit_rate = dts_bit_rate[brate];
   return fsize;
}



static int syncinfo_14LE(unsigned char *buf, int *sample_rate, int *bit_rate)
{
   int fsize, amode, sfreq, brate;
   if (buf[5] != 0x07 || (buf[4]&0xf0) != 0xf0)
      return 0;

   fsize = (buf[7] & 0x03)<<12 | buf[6]<<4 | (buf[9] & 0x3f)>>2;
   fsize++;
   fsize = fsize * 8 / 14 * 2;
   amode = (buf[9] & 0x03) << 4 | (buf[8] & 0xf0) >> 4;
   if (amode > 63)
      return 0;
   sfreq = buf[8] & 0x0f;
   brate = (buf[11] & 0x3f) >> 1;
   *sample_rate = dts_sample_rates[sfreq];
   if (*sample_rate == 0)
      return 0;
   *bit_rate = dts_bit_rate[brate];

   

   return fsize;
}

static int syncinfo_16LE(unsigned char *buf, int *sample_rate, int *bit_rate)
{
   int fsize, amode, sfreq, brate;
   fsize = (buf[4] & 0x03) << 12 | buf[7] << 4  | buf[6] >> 4;
   fsize++;
   amode = (buf[6] & 0x0f) << 2 | buf[9] >> 6;
   if (amode > 63)
      return 0;
   sfreq = buf[9] >> 2 & 0x0f;
   brate = (buf[9] & 0x03) << 3 | (buf[8] >> 5 & 0x07);
   *sample_rate = dts_sample_rates[sfreq];
   if (*sample_rate == 0)
      return 0;
   *bit_rate = dts_bit_rate[brate];
   return fsize;
}

static int syncinfo_16BE(unsigned char *buf, int *sample_rate, int *bit_rate)
{
   int fsize, amode, sfreq, brate;
   fsize = (buf[5] & 0x03) << 12 | buf[6] << 4  | buf[7] >> 4;
   fsize++;
   amode = (buf[7] & 0x0f) << 2 | buf[8] >> 6;
   if (amode > 63)
      return 0;
   sfreq = buf[8] >> 2 & 0x0f;
   brate = (buf[8] & 0x03) << 3 | (buf[9] >> 5 & 0x07);
   *sample_rate = dts_sample_rates[sfreq];
   if (*sample_rate == 0)
      return 0;
   *bit_rate = dts_bit_rate[brate];
   return fsize;
}

static int sadts_syncinfo(unsigned char * buf, int * sample_rate, int * bit_rate)
{
   int frame_size=0;
   int sync = (buf[0]<<24)+(buf[1]<<16)+(buf[2]<<8)+buf[3];
   if (dts_syncword){
      if (dts_syncword == sync)
         frame_size = (*p_syncinfo)(buf, sample_rate, bit_rate);  
         if (frame_size)
            return frame_size;
   }
   else{
      p_syncinfo = NULL;
      if (sync == 0x7ffe8001)
         p_syncinfo = syncinfo_16BE;
      else if (sync == 0xfe7f0180)
         p_syncinfo = syncinfo_16LE;
      else if (sync == 0x1fffe800)
         p_syncinfo = syncinfo_14BE;
      else if (sync == 0xff1f00e8)
         p_syncinfo = syncinfo_14LE;
      
      if (p_syncinfo){
         frame_size = (*p_syncinfo)(buf, sample_rate, bit_rate);
         if (frame_size){
            dts_syncword = sync;
            return frame_size;
         }
      }
   }
   return 0;
}


#define MAX_CHECK_EXTSUBS 4
#define DTS_SYNCWORD_EXTSUBS	    0x64582025
#define SKIP_LEN 4096
//P.21
static int skip_ExtSubstream(stream_t *s, unsigned char *buf)
{
   int cnt = MAX_CHECK_EXTSUBS;
   int skip = 0;
   do {
      int sync = (buf[0]<<24)+(buf[1]<<16)+(buf[2]<<8)+buf[3];
      if (sync == DTS_SYNCWORD_EXTSUBS){
         int type = buf[5]>>5 & 1;

         int nExtSSindex = buf[5]>>6 & 3;
         
         int ess_len;
         unsigned char skip_buf[SKIP_LEN];
         skip = 1;  //printf("estension substream found!!...\n");
         if (type)
            ess_len = (buf[6]&1)<<19 | (buf[7]<<11) | (buf[8]<<3) | (buf[9]>>5);    //HONDA : Not verify yet //long 
         else
            ess_len = (buf[6]&0x1f)<<11 | (buf[7]<<3) | (buf[8]>>5);   //short      
      
         ess_len += (1-DTSHEADER_SIZE-MAX_CHECK_EXTSUBS+cnt);  
         do{
            int skip_len = SKIP_LEN;//避免讀太大塊 
            if (ess_len < skip_len)
               skip_len = ess_len;
            ess_len -= skip_len;
            demux_read_data(s, skip_buf, skip_len);
         
         }while(ess_len > 0);
         break;
      }
      buf++;
      cnt--;
   } while(cnt > 0);
   return skip;
}









#define Readbufferlen 95-1   // spec
 static int syn_data_main(stream_t *s, unsigned char* sm_in, int get_len){
       
   int length;
   int sample_rate;
   int bit_rate;
   //CH add
   int fillin_ptr=0;     
   int consumed_ptr = 0 ; 
   int residue_num = 0 ; 
   int mvlen = 0;
   int c ; 
   int skip = 0;
   int sh_a_in_buffer_len = 0 ;
   int allign = 0 ;
 
   {
      sh_a_in_buffer_len= dts_header_len;
      memcpy(sm_in, dts_header_buf, dts_header_len);
      residue_num = fillin_ptr = dts_header_len; 

 
      if (sh_a_in_buffer_len < DTSHEADER_SIZE)
         return -1;

      while(1) {
         int c;
         length = sadts_syncinfo(sm_in + consumed_ptr, &sample_rate, &bit_rate);

       
         //printf("length=%d\n",length);

         if(length >= DTSHEADER_SIZE)
            break;


    
          consumed_ptr++; //move to next byte
          residue_num = fillin_ptr - consumed_ptr ;
          if (residue_num<DTSHEADER_SIZE)//不足14 bytes
          {
            memmove(sm_in, sm_in + consumed_ptr, residue_num); //13
            mvlen = Readbufferlen -residue_num; //mvlen必需<=min {length}
            consumed_ptr = 0 ; 
            fillin_ptr = residue_num; 

            c = demux_read_data(s,(sm_in + fillin_ptr), mvlen);
           if(c <= 0) 
              return -1; //EOF  sync fail!
#ifdef SAVE_ES 
   fwrite((sm_in + fillin_ptr), 1, c, fp);
#endif  
              fillin_ptr += c;

            residue_num = fillin_ptr ;
          }
      }

      //sync ok ! 
          memmove(sm_in , (sm_in + consumed_ptr), residue_num );
          sh_a_in_buffer_len = residue_num ; 
           c = demux_read_data(s, sm_in + residue_num, length - residue_num) ; //補齊
          if( c!=(length - residue_num))
              return -1; 

      sh_a_in_buffer_len = length ;//2010.08.16 
      
     
#ifdef SAVE_ES 
      fwrite( sm_in + residue_num, 1, length - residue_num, fp);
#endif
   }



//----------find extenstion substream and skip it ------------------------ 
  // if(length>0){
   do {
        dts_header_len = demux_read_data(s, dts_header_buf, DTSHEADER_SIZE);

      if (dts_header_len == DTSHEADER_SIZE)
         skip = skip_ExtSubstream(s, dts_header_buf); 
	  else
		  break ; 

	  header_flag = 1 ; 
   } while(skip==1);

   

//----------- auto allignment ---------------------- 
   allign = length & 0x3 ;
   if(allign>0)
   {  allign = 4-allign;
      memset(sm_in+length,0,allign);
      length += allign ; 
   }




   return length ;

 }



 static int read_core_data(stream_t *s, unsigned char* get_dts_data, int wanted_len , int syn_head_flag){
    
	 int n=0; 
     int end_res=0;
	//if(syn_head_flag==0)
	if(0)//note1
	{
		if(header_flag==1){
		   memcpy(get_dts_data ,dts_header_buf, DTSHEADER_SIZE);
		   wanted_len-= DTSHEADER_SIZE ;
		   header_flag = 0 ;
           n = demux_read_data(s, get_dts_data+DTSHEADER_SIZE , wanted_len); 
		   n += DTSHEADER_SIZE;
		}else
	
    	n = demux_read_data(s, get_dts_data , wanted_len); 
		
	}
	else 
	{ //need to do sync
		if( sync_ready_len==0)
		{
			 sync_ready_len = syn_data_main(s, dts_data , wanted_len); 
			
			 if(sync_ready_len<0)
                 return 0 ;
             
				ptr_read=0 ; 

		}
	



		
		if (sync_ready_len>=wanted_len)
		{
			memcpy(get_dts_data , dts_data+ptr_read, wanted_len);
	    	sync_ready_len -= wanted_len ; 
			ptr_read += wanted_len;
			n = wanted_len ; 
		}else
		  {
         
		   memcpy(get_dts_data , dts_data+ptr_read, sync_ready_len);
		   end_res = sync_ready_len ; 
     
		   ptr_read += sync_ready_len;
		   n = wanted_len - sync_ready_len ; 
		   sync_ready_len = 0 ;

		   sync_ready_len = syn_data_main(s, dts_data , n); 
          
		 
             if (sync_ready_len>=wanted_len)
		     { 
			   memcpy(get_dts_data+end_res , dts_data, n);
	    	   sync_ready_len -= n ; 
			   ptr_read = n;
			   n +=end_res ; 
		     } else{
			   
			   return end_res; 

		     }



		
		}



	}
 


   
     return n ; 
 
 }
#endif 
//-------------------------------------------------------------------------------------------------------


void fill_bs(sh_audio_t *sh,int need_data_flag) 
{
	int nread;
	//int read_index = dtsd_get_read_index(pDecoder_indtance);
	int free = read_index - bswp - 1;
	int nreads = 0 ; 
 //if (dts_packet==0){
 if (1){ //for m2ts format ,20110119 
	if (free < 0)
		free += DTS_BITSTREAM_BUFFER_SIZE;

	while (free > BITSTREAM_LOAD_BATCH) {
	  
		      if(DTSHD_Parser)
		      nread = read_core_data(sh->ds, (unsigned char *)sm_in + bswp , BITSTREAM_LOAD_BATCH ,!need_data_flag);
              else 
              nread = demux_read_data(sh->ds, (unsigned char *)sm_in + bswp , BITSTREAM_LOAD_BATCH);
           
			  #ifdef SAVE_ES
              fwrite(sm_in + bswp, 1, nread, fpcore);
              #endif  
			    
		   	  #ifdef SAVE_CORE
              fwrite(sm_in + bswp, 1, nread, fpcore);
              #endif  





			  nreads +=nread ; 
			  
			if (nread != BITSTREAM_LOAD_BATCH) {
			bswp += nread;
			 file_end_flag = 1;
			//dtsd_set_int(pDecoder_indtance, ID_TRACK_END_FLAG, 1);
			return;
		}

		bswp += BITSTREAM_LOAD_BATCH;
		if (bswp >= DTS_BITSTREAM_BUFFER_SIZE)
			bswp -= DTS_BITSTREAM_BUFFER_SIZE;

		free -= BITSTREAM_LOAD_BATCH;
	}
	
}else{	//dts_packet=1 case	
	/*	
		unsigned char* bufptr=NULL;
    
    nread=ds_get_packet(sh->ds, &bufptr);
    if(nread<=0) 
       return;  
    memcpy(bs_buf + bswp,bufptr, nread);
	*/
				
				}	


	
	 sh->a_in_buffer_len = nreads ; 

	
}

static void convertSpkrOut( dtsUint32 *spkrOut )
{
	dtsUint32 newSpkrOut = 0;

	

	if (*spkrOut & SKDTSSPKROUT_MASK_C)
	{
		newSpkrOut |= CONV_MASK_C;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LR)
	{
		newSpkrOut |= CONV_MASK_LR;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LsRs)
	{
		newSpkrOut |= CONV_MASK_LsRs;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LFE1)
	{
		newSpkrOut |= CONV_MASK_LFE1;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_Cs)
	{
		newSpkrOut |= CONV_MASK_Cs;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LhRh)
	{
		newSpkrOut |= CONV_MASK_LhRh;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LsrRsr)
	{
		newSpkrOut |= CONV_MASK_LsrRsr;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_Ch)
	{
		newSpkrOut |= CONV_MASK_Ch;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_Oh)
	{
		newSpkrOut |= CONV_MASK_Oh;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LcRc)
	{
		newSpkrOut |= CONV_MASK_LcRc;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LwRw)
	{
		newSpkrOut |= CONV_MASK_LwRw;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LssRss)
	{
		newSpkrOut |= CONV_MASK_LssRss;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LFE_2)
	{
		newSpkrOut |= CONV_MASK_LFE_2;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LhsRhs)
	{
		newSpkrOut |= CONV_MASK_LhsRhs;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_Chr)
	{
		newSpkrOut |= CONV_MASK_Chr;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LhrRhr)
	{
		newSpkrOut |= CONV_MASK_LhrRhr;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_Clf)
	{
		newSpkrOut |= CONV_MASK_Clf;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LlfRlf)
	{
		newSpkrOut |= CONV_MASK_LlfRlf;
	}
	if (*spkrOut & SKDTSSPKROUT_MASK_LtRt)
	{
		newSpkrOut |= CONV_MASK_LtRt;
	}

	*spkrOut = newSpkrOut;
}






static void skdtsPlayerConfig2ParamInfo(skdtsPlayerConfig *pConfig, dtsd_config_param_t *pParam)
{
	pParam->enableDownmix = dtsTrue;//pConfig->primaryDecoderConfig.enableDownmix;
	pParam->DwnMixLFEMIXSet = dtsFalse;//pConfig->primaryDecoderConfig.lfeMixedToFrontWhenNoDedicatedLFEOutput;
	pParam->destinationSpeakerMask = 0;// pConfig->primaryDecoderConfig.spkrOut;
	pParam->enableAnalogCompensation = dtsFalse;// pConfig->primaryDecoderConfig.enableAnalogCompensation;
	pParam->enableDialNorm = dtsTrue;//pConfig->dialNormConfig.enableDialNorm;
	pParam->DRCPercent = 0;// pConfig->DRCConfig.DRCPercent;
	pParam->lfeFilterType = 0 ;//pConfig->primaryDecoderConfig.lfeFilterType;

}








 void skdtsPlayerInitConfig(skdtsPlayerConfig * pConfig)
{
	memset(pConfig, 0, sizeof(skdtsPlayerConfig));


	pConfig->playerControlConfig.peakLimiter		= DTSPLAYERPEAKLIMIT_SOFT_LINEAR;
	
	pConfig->playerControlConfig.outputBitWidth		= 24; 
	pConfig->playerControlConfig.forceMonoOutput	= dtsFalse;
	

	/* call init functions for all other components */

	//dtsDecoderInitConfig( &(pConfig->primaryDecoderConfig) ) 
	memset(&pConfig->primaryDecoderConfig, 0, sizeof(pConfig->primaryDecoderConfig));
	pConfig->primaryDecoderConfig.mode = dtsDecoderOperationMode_LossLess ;
	pConfig->primaryDecoderConfig.sndFldIdx = -1;
	pConfig->primaryDecoderConfig.enableDownmix = dtsTrue;;

	pConfig->primaryDecoderConfig.lfeFilterType = 0;		//default as FIR (32 * 32)


	//dtsDialNormInitConfig( &(pConfig->dialNormConfig) );	
	pConfig->dialNormConfig.enableDialNorm = dtsTrue;



	//dtsDRCInitConfig( &(pConfig->DRCConfig) );	
	pConfig->DRCConfig.DRCPercent = 0 ;  

     
	
}















/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
//    
//------------------------------------------------------------------------------------------------------
*/
static int preinit(sh_audio_t *sh)
{



       
	   int config_istance_size; 
	
       skdtsPlayerConfig PlayerConfigInsatnce;
       #ifdef SAVE_ES  
       fp = fopen("dtshd.file","wb");
       #endif
       
       #ifdef SAVE_CORE  
       fpcore = fopen("core.bin", "wb");
       #endif   



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

	      output_ch  = audio_output_channels ;
         sh->audio_out_minsize = output_ch *4 * 256 * 16; 

         }
         #ifndef  EngineMode 
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
	if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	
		return 0;
#endif

	id = get_semaphore_id2();	
	if (!reset_semvalue(id, 0)){
		return 0;
 	}

	sc = (union smc *)sm_com;
	sc->magic.magic_num = PLAYER_MAGIC;
	curr_codec = DTSHD2_ID;

	sc->magic.codec_id = curr_codec;
	if (!semaphore_v(id, 1))
		return 0;	

	if (!wait_server(sc)){
		deallocate_share_mem(sm_com);	
		shmctl(sm_com_id, IPC_RMID, 0);
		del_semvalue(id, 0);
		printf("Audio server crash \n");
		return 0;
	}	

	if (sc->magic.codec_id != curr_codec){
		printf("Unsuppoted codec\n");
		return 0;		
	}
	aserver_pid = sc->pinit.pid;

#endif   
         
         	   
   //------------------------------------------------------------    
         



	// size of player config
	config_istance_size = sizeof(skdtsPlayerConfig);


	

	// init player param config
	skdtsPlayerInitConfig( &PlayerConfigInsatnce );

	// parse command line
	//dtsDecoderParseCommandLine(argc, argv, &PlayerConfigInsatnce);


	// pass player config to struct dts_param_info
	skdtsPlayerConfig2ParamInfo( &PlayerConfigInsatnce, &dts_param_info );

     
	 //=======get the user options =======================
 
   //playerConfig.DRCConfig.DRCPercent = DRCPercent  ; 
   //playerConfig.primaryDecoderConfig.spkrOut = SpkrOut ; 
   //playerConfig.primaryDecoderConfig.lfeMixedToFrontWhenNoDedicatedLFEOutput = l1 ;
   //playerConfig.dialNormConfig.enableDialNorm = !nodialnorm ; 
   //playerConfig.primaryDecoderConfig.enableDownmix = EnableDownmix ;
   //playerConfig.primaryDecoderConfig.enableAnalogCompensation = EnableAnalogCompensation ; 
   
 
	if(0<=SpkrOut && SpkrOut<=14)
    {
       	NEWSpkrOut = SpkrOut_PARA[SpkrOut];
       //dtsDecoderProcessSpkrOutCommand(&NEWSpkrOut,&PlayerConfigInsatnce);
		PlayerConfigInsatnce.primaryDecoderConfig.remapRequired = dtsTrue;
        convertSpkrOut(&NEWSpkrOut);
        
        if (NEWSpkrOut==6 || NEWSpkrOut==0x30000000) //L,R or Lt,Rt            
            audio_output_channels = 2;  // L,R only
         else            
            audio_output_channels = 6; 
            
    }else if(audio_output_channels==2  ) //2ch output and downmix
    {
            NEWSpkrOut = 6 ;
	        //l1 = 1; //defulat off when 2 channels play ,20110119
    }else if(audio_output_channels==6  ) //5.1
    {
            NEWSpkrOut = 0 ;
	  }

	if(0<=DRCPercent && DRCPercent<=2)
       DRCPercent = DRC_PARA[DRCPercent];
	else 
		DRCPercent = 0 ;//default


   //set sk DRC 
   if(DTS_LFE_Filter_Type==0 && night_mode!=0)
   	DRCPercent = 100 ; 

	if(DRCPercent){

	   PlayerConfigInsatnce.DRCConfig.DRCPercent = DRCPercent ; 
    }



	if(l1< 0 || l1>1)
		l1 = 0 ;



	

    


   dts_param_info.enableDownmix =  EnableDownmix ;
   dts_param_info.DwnMixLFEMIXSet = l1 ;
   dts_param_info.destinationSpeakerMask = NEWSpkrOut ; 
   dts_param_info.enableAnalogCompensation =  EnableAnalogCompensation ;
   dts_param_info.enableDialNorm = !nodialnorm ; 
   dts_param_info.DRCPercent = DRCPercent  ; 
   dts_param_info.lfeFilterType = DTS_LFE_Filter_Type; 


#if 1
	
	fprintf(stderr, "enableDownmix	= %d\n", dts_param_info.enableDownmix);
	fprintf(stderr, "DwnMixLFEMIXSet= %d\n", dts_param_info.DwnMixLFEMIXSet);
	fprintf(stderr, "destinationSpeakerMask	= %d\n", dts_param_info.destinationSpeakerMask);
	fprintf(stderr, "enableAnalogCompensation = %d\n", dts_param_info.enableAnalogCompensation);
	fprintf(stderr, "enableDialNorm	= %d\n", dts_param_info.enableDialNorm);
	fprintf(stderr, "DRCPercent	= %d\n", dts_param_info.DRCPercent);
    fprintf(stderr, "lfeFilterType	= %d\n", dts_param_info.lfeFilterType);
#endif
  //===================================================





    output_ch  = audio_output_channels ;
   
    sh->audio_out_minsize = output_ch *4 * 256 * 16;
    sh->audio_in_minsize = DTSBUFFER_SIZE;
    sh->sample_format=AF_FORMAT_S32_LE; //packing as 4 bytes
    sh->samplesize=4; //32 bit out  


    sh->channels = output_ch;

	bswp = file_end_flag = 0; //2011.02.08  fix initialization,larry
	read_index = 0 ; 
  
	sync_ready_len = ptr_read =header_flag =dts_syncword = 0;
    
	
	return 1;
}










/*
//-----------------------------------------------------------------------------------------
//    free DTSHD2 decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static void uninit(sh_audio_t *sh)
{
   
union smc *sc = (union smc *)sm_com;
      ipc2_uninit(sc, DTSHD2_ID, id);

      //server
      #ifndef  EngineMode
	   deallocate_share_mem(sm_in);
	   deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE
	   shmctl(sm_data_id, IPC_RMID, 0);
#endif
	   deallocate_share_mem(sm_com);
      #endif

 

	  #ifdef MY_DEBUG_D0_CHECKSUM
	  printf("\nchecksum @AD_SK=%d\n",check_sum);
	  #endif



	  #ifdef SAVE_ES  
      fclose(fp);
      #endif  

	  #ifdef SAVE_CORE
      fclose(fpcore);
      #endif   


}


/*
//-----------------------------------------------------------------------------------------
//    DTSHD2 decoder  
//    
//-----------------------------------------------------------------------------------------
*/

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{

      int j = 0, len = -1;	
      union smc *sc = (union smc *)sm_com;     //  shared memory

	  int ret ; 
	


      int data_flag = 0 ; 

   //while(len < minlen ) {


      int len2=0;

decode_loop_start:

		// load bs
		if (!file_end_flag)
			fill_bs(sh,data_flag);	
		
		data_flag = 0 ; 


     // memcpy(sm_in,bufptr, buflen); 
	  sc->pdecod.paremeter1 = output_ch; //send to server 
      sc->pdecod.paremeter2 = file_end_flag; 

      if (!ipc2_decode(sc, DTSHD2_ID, bswp, id))
	      return 0 ;		      //IPC Error 
      if (sc->sctrl.codec_id == DTSHD2_ID) //decode success!            
		  len2= sc->sctrl.pg.dtshd2ret.decode_len;           
            

	 
	  	  sh->i_bps =  sc->sctrl.pg.dtshd2ret.bps;
		  sh->samplerate =  sc->sctrl.pg.dtshd2ret.samplerate;   
		  ret = sc->sctrl.pg.dtshd2ret.ret;  
		  read_index =  sc->sctrl.pg.dtshd2ret.readindex; 
      
	

		  if(ret<=0){
              if (ret == DTS_NEED_MORE_DATA) {
				 data_flag = 1; //larry add 
			     goto decode_loop_start;//continue;
		      } else if (ret == DTS_FINISH) {
			      return -1 ;//break;
		       } else {
			      printf("decoding failed!\n");
			       return -1 ;//goto exit_decode;
		            }

		  }


	  memcpy( buf, sm_out , len2);  // sm_out --> bit wav
	
     
  // }



  #ifdef MY_DEBUG_D0_CHECKSUM
			 {
			 	   
			 	      int i=0;
			 	      char *Pbuf=buf;
			 	      //printf("\data_size @AD_SK=%d\n",data_size);  
			 	     for(i=0;i<len2;i++)
			        {//printf("\*Pbuf %d\n",*Pbuf); 
			        	check_sum+=*Pbuf++;	        
			         }       
			      
			}
			 
 #endif




    return len2;
}



//--------------------------------------------------------------------------------------

int dtshd2_frame_init( aduio_info *ai, sh_audio_t *sh ){
    
         int init = 1 , ret;
         union smc *sc = (union smc *)sm_com;
         play_init *pi = &sc->pinit;
         serv_init *si = &sc->sinit;

         

         //send to server 
         //some parameters to server for ini   
		 pi->pg.dtshd2info.enableDownmix =  dts_param_info.enableDownmix; 
		 pi->pg.dtshd2info.DwnMixLFEMIXSet = dts_param_info.DwnMixLFEMIXSet;
         pi->pg.dtshd2info.destinationSpeakerMask = dts_param_info.destinationSpeakerMask ;
         pi->pg.dtshd2info.enableAnalogCompensation =dts_param_info.enableAnalogCompensation ; 
         pi->pg.dtshd2info.enableDialNorm = dts_param_info.enableDialNorm ;
         pi->pg.dtshd2info.DRCPercent =  dts_param_info.DRCPercent ;
         pi->pg.dtshd2info.lfeFilterType = dts_param_info.lfeFilterType ;
          

		 pi->paremeter3 = output_ch ; 



		 //def _DEFINE_PRE_SYN_HD_2_CORE_
         ////if(DTSHD_Parser) {
         ////dts_header_len = demux_read_data(sh->ds, dts_header_buf, DTSHEADER_SIZE);
         ////sh->a_in_buffer_len= dts_header_len;
        ////}

         
		 //preload bs 
         ////if (!file_end_flag)
		 //// fill_bs(sh,0);	

       
         if(!ipc2_init(sc, DTSHD2_ID, bswp, id, sm_data_id, SM_IN_SIZE_DTSHD2)) //init server
	    return -1;//break;		//IPC Error 
         if (si->codec_id == DTSHD2_ID)
            ret = 0;
         else
            ret = si->codec_id;   //error code

         init = 0;
     
			*ai = si->ai;
#ifndef  EngineMode
		 	if(sm_out == NULL || sm_out == (void *)-1)
			sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

 
 
            //


          






			//def _DEFINE_PRE_SYN_HD_2_CORE_
            if(DTSHD_Parser) {
              dts_header_len = demux_read_data(sh->ds, dts_header_buf, DTSHEADER_SIZE);
              sh->a_in_buffer_len= dts_header_len;
             }

	         sh->a_buffer_len = decode_audio(sh, (unsigned char*)sh->a_buffer,1,sh->a_buffer_size);

             if (sh->a_buffer_len< 0){
               return 0;
              }



         //from server 
			 sh->i_bps = sh->i_bps;
			 sh->samplerate = sh->samplerate;   
		     //read_index =  ai->dtshd2ret.readindex; 
			

			// ai->ch = sc->sctrl.dtshd2ret.ch_content ; 
			  ai->ch = sc->sctrl.pg.dtshd2ret.ch_content ; 

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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_IN_SIZE_DTSHD2);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif
 

   ret = dtshd2_frame_init(ai,sh); 
   if (ret==-1)
     return ret ; 
    




   
  

	 
    return 1;
 
}






#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    union smc *sc = (union smc *)sm_com;     //  shared memory 
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         ipc_ctrl(sc, DTSHD2_ID, 1, id);
     
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}


ad_functions_t mp_skdtshd2 = { 
	preinit,
	init,
    uninit,
	control,
	decode_audio
};
#endif
