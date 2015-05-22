

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>





extern unsigned int dts_packet ;
//#define MY_DEBUG 
//#define MY_DEBUG_1 
//#define MY_DEBUG_D0_CHECKSUM 
#define UBUNTU_OR_PC_PLATFORM   //play on ubuntu/windows  ,differ with sv8860
#define SV8860DRIVEROUTPUT  //L,LFE,LS,R,C ,RS
#define SWAP_LFE_C //swap LFE,C ...L,C,LS,R,LFE ,RS


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@/ user options /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int DRCPercent = 0 ; //0: off   33: light   66:medium     100:heavy
int SpkrOut = 0XFFFF; 
int l1 = 0;           //LFE mix 
int nodialnorm = 0 ;  //off/on  dialnorm 

int EnableDownmix = 1;
int EnableAnalogCompensation = 0 ; 
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


int DTS_LFE_Filter_Type = 0 ;//2011.01.25 
/*
   --FilterType0       ( 32*32 FIR )
   --FilterType1       ( 32*16 FIR )
   --FilterType2       ( IIR )
*/


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#ifndef  EngineMode 
//{
#include <assert.h>

#include <assert.h>
#include "config.h" 
#include "ad_internal.h"
#include "skdtshd2.h"


static ad_info_t info =
{
    "Skyviia DTS-HD2  decoder",
    "skdtshd2",
    "Larry",
    "Larry",
    "Skyviia copyright"
};

LIBAD_EXTERN(skdtshd2) 




//}
#else
//{

#include "skdtshd2.h"
#include "skdtshd2_mplayer.h"



//}
#endif





dtsBoolean dtsDecoderProcessSpkrOutCommand( int *pValue, skdtsPlayerConfig *pConfig);
dtsBoolean dtsDecoderProcessDRCCommand( int *pValue, skdtsPlayerConfig *pConfig);
//------------------------------------------------------------------------
#ifdef MY_DEBUG_D0_CHECKSUM 
long long check_sum=0;
#endif


extern unsigned int dts_packet;

#define DTSBUFFER_SIZE 32*1024//DTS_FILE_READ_BUFFER_SIZE_IN_BYTES; 
extern int audio_output_channels;

int output_ch ; 






int out_pcmbuf[DTS_MAX_FRAME_SAMPLES * 6];		// 6 ch
char decoder_instance[DTS_WORK_SPACE];
static unsigned char bs_buf[DTS_BITSTREAM_BUFFER_SIZE];
void fill_bs(sh_audio_t *sh,void *pDecoder_indtance);

int file_end_flag = 0;
int  rc;
int  sync_16_count, sync_14_count;
int  frame_size;
int  nFrameCount;
int bswp = 0;
#define	DTS_NEED_MORE_DATA	 0
#define DTS_FINISH			-1

#define BITSTREAM_LOAD_BATCH		256



extern void dtsPlayerInitConfig( skdtsPlayerConfig *pConfig );



const short dts_br_tab[32] = { 
      32,  56,  64,  96, 112, 128, 192, 224, 
     256, 320, 384, 448, 512, 576, 640, 768,
     960, 1024, 1152, 1280, 134, 1408, 1411, 1472,
     1536, 1920, 2048, 3072, 3840, -1 /*open*/, -2/*variable*/, -3/*lossless*/,  
};



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


void fill_bs(sh_audio_t *sh,void *pDecoder_indtance)
{
	int nread;
	int read_index = dtsd_get_read_index(pDecoder_indtance);
	int free = read_index - bswp - 1;
	int nreads = 0 ; 
 //if (dts_packet==0){
 if (1){ //for m2ts format ,20110119 
	if (free < 0)
		free += DTS_BITSTREAM_BUFFER_SIZE;

	while (free > BITSTREAM_LOAD_BATCH) {
	  
		    nread = demux_read_data(sh->ds, (unsigned char *)bs_buf + bswp , BITSTREAM_LOAD_BATCH);
			  nreads +=nread ; 
			  
			if (nread != BITSTREAM_LOAD_BATCH) {
			bswp += nread;
			file_end_flag = 1;
			dtsd_set_int(pDecoder_indtance, ID_TRACK_END_FLAG, 1);
			return;
		}

		bswp += BITSTREAM_LOAD_BATCH;
		if (bswp >= DTS_BITSTREAM_BUFFER_SIZE)
			bswp -= DTS_BITSTREAM_BUFFER_SIZE;

		free -= BITSTREAM_LOAD_BATCH;
	}
	
}else{	//dts_packet=1 case	
		
		unsigned char* bufptr=NULL;
    
    nread=ds_get_packet(sh->ds, &bufptr);
    if(nread<=0) 
       return;  
    memcpy(bs_buf + bswp,bufptr, nread);
				
				}	
	
	sh->a_in_buffer_len = nreads ; 
}




//#define SPERKMAP

//int sv8860chmaping[6] ={1,5,3,2,0,4};

void  rearrange_rawdata_to_sv8860(int  nsamples, int *pcmbuf , unsigned char *buf, unsigned int SpkMap)
{

	//|      ch0		 |		ch1			|		ch2        |.....  ch
	//|---- 4096 pt  ----|----- 4096 pt ----|------ 4096 ------|.....  pcm
	//| 0  | .....  | 15 | 0  | .....  | 15 | 0  | .....  | 15 |.....  sub-subframe


  // speaker map
	// < X, X, LFE, Rs, Ls, R, L, C >	
	//以4bit欄位表示,第n個block,0xf:表示沒有output		
	int sv8860chmaping[6]; 		    
	int ch,i;
	int sample ; 
	unsigned char *pbuf =buf ;
//#ifdef SPERKMAP	
	unsigned int spmap[6];

   spmap[0]=SpkMap &0xf;
   spmap[1]=(SpkMap>>4) &0xf;
   spmap[2]=(SpkMap>>8) &0xf;
   spmap[3]=(SpkMap>>12) &0xf;
   spmap[4]=(SpkMap>>16) &0xf;
   spmap[5]=(SpkMap>>20) &0xf;
     
  #ifdef SWAP_LFE_C
   sv8860chmaping[0]=spmap[1]; //L 在 第spmap[1] block
   sv8860chmaping[1]=spmap[0]; //C
   sv8860chmaping[2]=spmap[3]; //Ls
   sv8860chmaping[3]=spmap[2]; //R
   sv8860chmaping[4]=spmap[5]; //LFE  
   sv8860chmaping[5]=spmap[4]; //Rs
  #else
   sv8860chmaping[0]=spmap[1]; //L 在 第spmap[1] block
   sv8860chmaping[1]=spmap[5]; 
   sv8860chmaping[2]=spmap[3]; 
   sv8860chmaping[3]=spmap[2]; 
   sv8860chmaping[4]=spmap[0];   
   sv8860chmaping[5]=spmap[4]; 
   #endif
     
#ifdef MY_DEBUG_1        
   printf("[output_ch]%d\n ",output_ch);
   printf("[SpkMap]:ch0=%d,ch1=%d,ch2=%d,ch3=%d,ch4=%d,ch5=%d\n ",spmap[0],spmap[1],spmap[2],spmap[3],spmap[4],spmap[5]);
   //printf("[sv8860chmaping]:ch0=%d,ch1=%d,ch2=%d,ch3=%d,ch4=%d,ch5=%d\n ",sv8860chmaping[0],sv8860chmaping[1],sv8860chmaping[2],sv8860chmaping[3],sv8860chmaping[4],sv8860chmaping[5]);
#endif   




//#endif  //SPERKMAP

	for (i=0; i<nsamples; i++)			
	{
		
		for(ch=0 ; ch<output_ch; ch++)  // fixed as 5.1 ch
		{
			
			 if(output_ch!=2){
       if(sv8860chmaping[ch]==0xf) { //channels no ouput 
	          sample = 0;  
	          goto fill_s;
	        }
	      }
	        
	        
#ifdef SV8860DRIVEROUTPUT
			if(output_ch==2)
			{
				//if(SpkrOut==6 || SpkrOut==0x30000000) 
				sample = pcmbuf[ DTS_MAX_FRAME_SAMPLES*(ch) +i ]; //get L/R 
				//else
				//sample = pcmbuf[ DTS_MAX_FRAME_SAMPLES*(spmap[ch]) +i ]; //get L/R (ch1/ch2)

			}
			else
		 sample = pcmbuf[ DTS_MAX_FRAME_SAMPLES*sv8860chmaping[ch] +i ]; 
		 
		 
#else
			sample = pcmbuf[ nsamples*sv8860chmaping[ch] +i ];
#endif 





#ifdef UBUNTU_OR_PC_PLATFORM
            sample = sample<<8 ; //24 bit to 32bit for driver 
#endif 



fill_s:	 
			memcpy(pbuf,&sample,4);
			pbuf+=4;
		}
  
         
	}





}
int oss_get_frag(int *fragments, int *fragstotal, int *fragsize);
//------------------------------------------------------------------------
//                     mplayer API 
//------------------------------------------------------------------------ 
static int decode_audio(sh_audio_t *sh, unsigned char *buf, int minlen, int maxlen)
{


   int data_size =0; 
   int ret,fs,br;
   unsigned int SpkerMap;
   #ifdef MY_DEBUG
   int frs,fratotal,frsize;
   #endif 

   //fprintf(stderr, "\nDTS decoding ...\n");
	//while (1)	// decode loop
	//{

decode_loop_start:
		// load bs
		if (!file_end_flag)
			fill_bs(sh,&decoder_instance);	
		


		// decode
		ret = dtsd_run(&decoder_instance, bswp, out_pcmbuf);				
		if (ret > 0) {

         // sample rate 
			fs = dtsd_get_int(&decoder_instance, ID_SAMPLING_RATE);

			// bit rate
			br = dtsd_get_int(&decoder_instance, ID_BITRATE);
			br = dts_br_tab[br]*1000/8;
    	if(br<0)
			  br=0; //20101207
			  
			// channel number
			/// numch = dtsd_get_int(&decoder_instance, ID_CHANNEL_NUMBER);
	

			// speaker map
			// < X, X, LFE, Rs, Ls, R, L, C >			
			SpkerMap = dtsd_get_int(&decoder_instance, ID_SPEAKER_MAP);
          
      rearrange_rawdata_to_sv8860( ret, out_pcmbuf , buf,SpkerMap);


		  //inform mplayer	
           sh->samplerate =fs;
	       sh->i_bps = br ; 
           data_size +=   ret*4*output_ch ;  //4byte,6ch ;

			nFrameCount++;
		}
		else if (ret == DTS_NEED_MORE_DATA) {
			goto decode_loop_start;//continue;
		} else if (ret == DTS_FINISH) {
			return -1 ;//break;
		} else {
			printf("decoding failed!\n");
			return -1 ;//goto exit_decode;
		}
		




		// print frame counter for every 256 frames 
		//if ((nFrameCount % 256) == 0)
		//{
		//	fprintf(stderr, "frame# = 0x%08x\n", nFrameCount);
		//}
	//}

   

  

 



#ifdef MY_DEBUG
//printf("data_size=%d",data_size);
//for(i=0;i<512;i++)
// checksum+=*pchsum++;

if (oss_get_frag(&frs, &fratotal, &frsize))
	printf("E: fragments=%d   ,fratotal=%d,   frsize=%d \n",frs,fratotal,frsize);
#endif 	



 #ifdef MY_DEBUG_D0_CHECKSUM
			 {
			 	   
			 	      int i=0;
			 	      char *Pbuf=buf;
			 	      //printf("\data_size @AD_SK=%d\n",data_size);  
			 	     for(i=0;i<data_size;i++)
			        {//printf("\*Pbuf %d\n",*Pbuf); 
			        	check_sum+=*Pbuf++;	        
			         }       
			      
			}
			 
			 #endif
			 

   return data_size;
}



//-----------------------pre-init---------------------------------------------
static int preinit(sh_audio_t *sh)
{
  
  	int config_istance_size;
	buffer_t bs_buffer;
	skdtsPlayerConfig PlayerConfigInsatnce;
	dtsd_config_param_t	dts_param_info;

  bswp = file_end_flag = 0; //2011.02.08  fix initialization,larry

	// size of player config
	config_istance_size = sizeof(skdtsPlayerConfig);

	// init bs_buffer
	bs_buffer.addr = bs_buf;
	bs_buffer.size = sizeof(bs_buf);
	nFrameCount=0;

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
            
    }
     else if(audio_output_channels==2  ) //2ch output and downmix
    {

     NEWSpkrOut = 6 ;
	   //l1 = 1; //defulat off when 2 channels play ,20110119
    }


	if(0<=DRCPercent && DRCPercent<=2)
       DRCPercent = DRC_PARA[DRCPercent];
	else 
		DRCPercent = 0 ;//default



	if(DRCPercent){
       //dtsDecoderProcessDRCCommand(&DRCPercent,&PlayerConfigInsatnce);
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
 

	// get input params, decoder api begins ...

	// init decoder
	dtsd_init(&decoder_instance, &bs_buffer);
	
	// set config params
	dtsd_set_config(&decoder_instance, &dts_param_info);

 












	

    output_ch  = audio_output_channels ;
   
    sh->audio_out_minsize = output_ch *4 * 256 * 16;
    sh->audio_in_minsize = DTSBUFFER_SIZE;
    sh->sample_format=AF_FORMAT_S32_LE; //packing as 4 bytes
    sh->samplesize=4; //32 bit out  

   

    return 1;
}



//---------------------------------------------------------------------------
static int init(sh_audio_t *sh)
{ 



	sh->a_buffer_len = decode_audio(sh, (unsigned char*)sh->a_buffer,1,sh->a_buffer_size);

      if (sh->a_buffer_len< 0){
      return 0;
        }


    sh->channels = output_ch;

     return 1;
}




//---------------------------------------------------------------------------
static void uninit(sh_audio_t *sh)
{



     #ifdef MY_DEBUG_D0_CHECKSUM
		 printf("\nchecksum @AD_SK=%d\n",check_sum);
		 #endif
		 

}



//---------------------------------------------------------------------------
#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    int flags;

    switch(cmd){
        case ADCTRL_RESYNC_STREAM:
            if (dts_packet==0)

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


