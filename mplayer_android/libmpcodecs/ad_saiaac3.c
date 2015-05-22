#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
//#include <assert.h>

#ifndef  EngineMode
//#define DOLBY_AT2
//#define DOLBY_OPTION 
#endif

#define SWAP_Cs


#ifdef DOLBY_AT2
#define MUTE_MS 5                 //delay for I2S to SPDIF converter
static int mute_sample = 0;
static unsigned int DD_CH = 0x80000000;          //for verify only
#endif

extern unsigned int night_mode;            // Honda 2010/12/29    

static int cur_sr = 0;    



unsigned int DD_C_DLY = 0;                //0-5 : 0-5 ms
unsigned int DD_SLR_DLY = 0;              //0-15: 0-15 ms
unsigned int DD_WORDSIZE24 = 0;         //0: 16-bit output, 1: 24-bit output




unsigned int DD_K = 10;                    //0:analog dialnorm 1:digital dialnorm 2:line out 3:RF remod
unsigned int DD_S = 10;                    //0:auto detect(Lt/Rt) 1:Lt/Rt 2:Lo/Ro
unsigned int DD_COMP = 10;               //0: compress off, 1:1/5 2:2/5 .... 5: compress full
unsigned int DD_MONO = 10;               //0: stereo 1: left mono 2: right mono 3: mixed mono
unsigned int DD_M = 0;                   //0: depend on audio_output_channels
                                         //1 = 1/0 (C), 2 = 2/0 (L, R), 3 = 3/0 (L, C, R), 4 = 2/1 (L, R, l), 5 = 3/1 (L, C, R, l)
                                          //6 = 2/2 (L, R, l, r), 7 = 3/2 (L, C, R, l, r)  

unsigned int DD_SP_LR = 0;              //0: LARGE  1: SMALL 
unsigned int DD_SP_C = 0;               //0: LARGE  1: SMALL 2:NONE       
unsigned int DD_SP_SLR = 0;             //0: LARGE  1: SMALL 2:NONE  
unsigned int DD_SP_LFE = 0;             //0: ON     1: OFF      



int sk_kcapablemode = -1;
int sk_compmode = -1;	
int sk_outlfeon = -1;	
int sk_outputmode = -1;

int sk_stereomode = -1;
int sk_dualmonomode = -1;
int sk_chanptr = -1;  //0x543210;
float sk_dynrngscalehi = -1.0;
float sk_dynrngscalelow = -1.0;
float sk_pcmscalefac = -1.0;		/* PCM scale factor */




#ifndef  EngineMode 

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>    
#include "skipc.h" 

#include "config.h"
#include "ad_internal.h"
#include "mpbswap.h"

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#ifdef HAVE_SOUNDCARD_H
#include <soundcard.h>
#endif
#endif

#include "libao2/audio_out.h"

int ao_fd = -1;
extern ao_data_t ao_data;

static ad_info_t info = 
{
	"Skyviia IA AC-3 Decoder",   
	"saiaac3",                   // short name 
	"Honda",               // maintainer
	"Honda",              // author
	"Skyviia copyright"        // comment 
};

LIBAD_EXTERN(saiaac3)          

#else

#include "iaac3_mplayer.h"

#endif


extern int audio_output_channels;
static unsigned char *sm_in_align; 
static int ui_inp_size, i_bytes_consumed, i_buff_size, prev_frm_len;
#define RETRYNUM  10

#include "audio_server2.h"
static int sm_com_id=-1, sm_data_id=-1, id=-1;

extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

//#define IAAC3_DECODE_PTS
#ifdef IAAC3_DECODE_PTS
static unsigned long long decsum = 0;
static double startpts = MP_NOPTS_VALUE;
#endif

#ifdef DOLBY_OPTION
//delay channel by archin 2010/09/02
typedef struct _ac3d_delay{
  
		
	short DBufferC[720];
	short DBufferS[720*2];
	
	int DNumC;
	int DNumS;	

    int delaymsC;
	int delaymsS;
}ac3d_delay;
ac3d_delay g_ac3d_delay;


static void ac3d_delay_init(int *srate)
{

	int i;
		
	for (i = 0; i< 720; i++)
	{		
		g_ac3d_delay.DBufferC[i] = 0;
		g_ac3d_delay.DBufferS[i]= 0;
	}
	g_ac3d_delay.delaymsC = DD_C_DLY; //Channel C delay 4ms
	g_ac3d_delay.delaymsS = DD_SLR_DLY; //Channel SR SL delay 10ms
    
	g_ac3d_delay.DNumC = (*srate/1000)*g_ac3d_delay.delaymsC; //Channel C delay Buffer length
	g_ac3d_delay.DNumS = (*srate/1000)*g_ac3d_delay.delaymsS; //Channel SR SL delay Buffer length
	
}

static void ac3d_delay_func(short *buf,int len)
{
	short buffertemp[3840];
   short *fpbuf;
    int len6,i;

	len6 = len/12;
	for(i = 0; i<g_ac3d_delay.DNumS;i++)
	{
		buffertemp[2*i] = *(buf+6*i+2);			
		buffertemp[2*i+1] = *(buf+6*i+5);
		*(buf+6*i+2) = g_ac3d_delay.DBufferS[2*i];
		*(buf+6*i+5) = g_ac3d_delay.DBufferS[2*i+1];
	}
   
	for(i = g_ac3d_delay.DNumS; i<(len6-g_ac3d_delay.DNumS);i++)
	{
		buffertemp[2*i] = *(buf+6*i+2);			
		buffertemp[2*i+1] = *(buf+6*i+5);
	
	}
   
	fpbuf = g_ac3d_delay.DBufferS;
	for(i = (len6-g_ac3d_delay.DNumS); i<len6;i++)
	{
		*fpbuf++ = *(buf+6*i+2);			
		*fpbuf++ = *(buf+6*i+5);	
	}
	
	
	for(i = 0; i<(len6-g_ac3d_delay.DNumS);i++)
	{
		buf[6*g_ac3d_delay.DNumS+6*i+2] =buffertemp[2*i];			
		buf[6*g_ac3d_delay.DNumS+6*i+5] =buffertemp[2*i+1];	
	}

	for(i = 0; i<g_ac3d_delay.DNumC;i++)
	{
		buffertemp[i] = *(buf+6*i+4);			
		*(buf+6*i+4) = g_ac3d_delay.DBufferC[i];
	}
   
	for(i = g_ac3d_delay.DNumC; i<(len6-g_ac3d_delay.DNumC);i++)
	{
		buffertemp[i] = *(buf+6*i+4);			
	
	}
   
	fpbuf = g_ac3d_delay.DBufferC;
	for(i = (len6-g_ac3d_delay.DNumC); i<len6;i++)
	{
		*fpbuf++ = *(buf+6*i+4);			
	}
	

	for(i = 0; i<(len6-g_ac3d_delay.DNumC);i++)
	{
		buf[6*g_ac3d_delay.DNumC+6*i+4] =buffertemp[i];		
	}

}

#endif


#define IA_FATAL_ERROR				0x80000000	

static float dyntbl[ ] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0}; 

static int preinit(sh_audio_t *sh)
{
	aserver_pid = 0;
#ifndef  EngineMode 
	union smc *sc;
	long curr_codec;
#endif   
#ifdef IAAC3_DECODE_PTS
	printf("%%%%%%%%%%%%%%%%%%%%%%%%%% reset startpts to MP_NOPTS_VALUE %%%%%%%%%%%%%%%%%%\n");
	startpts = MP_NOPTS_VALUE;
	decsum = 0;
#endif

	/* Dolby AC3 audio: */
	/* however many channels, 2 bytes in a word, 256 samples in a block, 6 blocks in a frame */
   if (DD_WORDSIZE24==0){   
      sh->samplesize = 2;
      sh->sample_format=AF_FORMAT_S16_LE;
   }
   else{   
      sh->samplesize = 4;
      sh->sample_format=AF_FORMAT_S32_LE;
   }   
   
//DD parameter setup  
   if (DD_M){
      if (DD_M<8){
         sk_outputmode = DD_M;   
         if (DD_M==2)            
            audio_output_channels = 2; 
         else            
            audio_output_channels = 6; 
#ifdef DOLBY_AT2    
         if (DD_M == 7){    
            if ((int)DD_CH >= 0){
               printf("Switch channel to 0X%x\n", DD_CH);
               sk_chanptr = (int)DD_CH;  
            }        
         }               
#endif         
      } 
   }
       
   if (sk_outputmode<0)
      sk_outputmode = (audio_output_channels ==2 ? 2:7); 
   if (sk_chanptr<0)
#ifndef SWAP_Cs      
      sk_chanptr = (audio_output_channels ==2 ? 0xFFF1F0:0x152340);   
#else         
      sk_chanptr = (audio_output_channels ==2 ? 0xFFF1F0:0x452310);   //change C and s for testing  
#endif         
             
   if (DD_COMP < 6){
      float f = dyntbl[DD_COMP];
      sk_dynrngscalehi = f;
      sk_dynrngscalelow = f;   
   }    
   else{
      if (night_mode){
         //printf("night_mode ON\n");
         sk_dynrngscalehi = 1.0;
         sk_dynrngscalelow = 1.0;        
      }
      else{
         //printf("night_mode OFF\n");
         sk_dynrngscalehi = 0.0;
         sk_dynrngscalelow = 0.0;      
      }                  
   }      
   if (DD_S < 3)
      sk_stereomode = DD_S;         
   if (DD_K < 4)
      sk_compmode = DD_K;          
   if (DD_MONO < 4)   
      sk_dualmonomode = DD_MONO;
   if (DD_SP_LFE == 1)
      sk_outlfeon = 0;      
   
   
	sh->audio_out_minsize=audio_output_channels*sh->samplesize*256*6;//sh->audio_out_minsize=audio_output_channels*sh->samplesize*256*6
   sh->audio_in_minsize = 4096; 
   
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
	curr_codec = IAAC3_ID;

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
#endif   

	aserver_pid = sc->pinit.pid;
   
	return 1;
}

extern int ac3_bitrate;
static int g_EBC;

static int init(sh_audio_t *sh_audio)
{
   int sm_in_shift = 0;
   int i_samp_freq, i_bitrate=0;
   int i_bytes_read, ui_init_done = 0;
   int c;
   int retry = RETRYNUM;
   
   union smc *sc = (union smc *)sm_com;   
   play_init *pi = &sc->pinit;
   serv_init *si = &sc->sinit;
   iac3_initpara *i3init = &pi->pg.i3init;
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_IN_SIZE_IAC3);
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif
#endif  

   i3init->sk_kcapablemode       = sk_kcapablemode;      
   i3init->sk_compmode  	      = sk_compmode; 	      
   i3init->sk_outlfeon  	      = sk_outlfeon; 	        
   i3init->sk_outputmode         = sk_outputmode;        
   i3init->sk_stereomode         = sk_stereomode;        
   i3init->sk_dualmonomode       = sk_dualmonomode;      
   i3init->sk_chanptr            = sk_chanptr;           
   i3init->sk_dynrngscalehi      = sk_dynrngscalehi;     
   i3init->sk_dynrngscalelow     = sk_dynrngscalelow;    
   i3init->sk_pcmscalefac  		= sk_pcmscalefac; 		
   i3init->audio_output_channels = audio_output_channels;
   i3init->DD_WORDSIZE24         = DD_WORDSIZE24;  
     
   pi->paremeter1 = 1;     //INIT1
   if(!ipc2_init(sc, IAAC3_ID, 0, id, sm_data_id, SM_IN_SIZE_IAC3))     //SM_IN_SIZE_IAC3 可重複送, server2.c會擋
      return -1;		//IPC Error 
   if (si->codec_id == IAAC3_ID)
      c = 0;
   else
      c = si->codec_id;   //error code   
   if((c) & IA_FATAL_ERROR)
      return 0;   //function fail  
      
   sm_in_shift = si->paremeter1;
   ui_inp_size = si->paremeter2;  
      
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif      
      
   sm_in_align = sm_in + sm_in_shift; 
   i_bytes_consumed = ui_inp_size;  
   i_buff_size = ui_inp_size;
   do {
      unsigned char *new_in;
      if (ui_inp_size > i_bytes_consumed)
         memmove(sm_in_align, sm_in_align+i_bytes_consumed, ui_inp_size - i_bytes_consumed); 
      new_in = sm_in_align + (ui_inp_size - i_bytes_consumed);         
      i_bytes_read = demux_read_data(sh_audio->ds, new_in, i_bytes_consumed);	

#ifdef IAAC3_DECODE_PTS
      if(startpts == MP_NOPTS_VALUE && sh_audio->ds->pts != MP_NOPTS_VALUE){
	startpts = sh_audio->ds->pts;
	printf("%%%%%%%%%%%%%%%%%%%%%%%%%% set startpts to %f %%%%%%%%%%%%%%%%%%\n", startpts);
      }
#else
#if 0 //Polun 2012-01-06 for DLNA CCT test file O-MP2TS_SN-1.mpg.
if (sh_audio->ds->pts != MP_NOPTS_VALUE)
 sh_audio->pts = sh_audio->ds->pts;
#endif
#endif

      //if(sh_audio->format==0x2000) 
      //   swab(new_in, new_in, i_bytes_read);         
      if(i_bytes_read <= 0) 
         return 0; // EOF
		/* New buffer size */
		i_buff_size = i_buff_size - (i_bytes_consumed - i_bytes_read);  
		
      pi->paremeter1 = 2;     //INIT2
      pi->paremeter2 = i_bytes_consumed;
      pi->paremeter3 = ui_init_done;      
      if(!ipc2_init(sc, IAAC3_ID, i_buff_size, id, sm_data_id, SM_IN_SIZE_IAC3))     //SM_IN_SIZE_IAC3 可重複送, server2.c會擋
         return -1;		//IPC Error 
      if (si->codec_id == IAAC3_ID)
         c = 0;
      else
         c = si->codec_id;   //error code   
      if((c) & IA_FATAL_ERROR)
         return 0;   //function fail  		
         
      i_buff_size = si->paremeter1;      
      i_bytes_consumed = si->paremeter2; 
      ui_init_done = si->paremeter3;   
      i_samp_freq = ai->samplerate;   
      i_bitrate = ai->bitrate; 

      retry--;            
   } while(!ui_init_done && retry>=0);         
		
   if (retry < 0)       
      return 0;   // no AC3 bitstream
    
   sh_audio->channels = audio_output_channels;
   sh_audio->samplerate = i_samp_freq;
   cur_sr = i_samp_freq; 

   if (i_bitrate != 0)
	   sh_audio->i_bps = i_bitrate/8;
#if 0    
   else
   {  	   
      if (ac3_bitrate != 0)
         sh_audio->i_bps = ac3_bitrate/8;      
   }
#endif    
  
#ifdef DOLBY_OPTION   
   ac3d_delay_init(&sh_audio->samplerate);
#endif   

#ifdef DOLBY_AT2
   mute_sample = MUTE_MS*48*2*audio_output_channels;
#endif   
   prev_frm_len = 0;      
   g_EBC = -1;     
  	return 1;
}

static int cnt = 0;
static int flen = 0;

static void uninit(sh_audio_t *sh)
{
   union smc *sc = (union smc *)sm_com;
   ipc2_uninit(sc, IAAC3_ID, id);
#ifndef  EngineMode
	deallocate_share_mem(sm_in);
	deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE
	   shmctl(sm_data_id, IPC_RMID, 0);
#endif		
	deallocate_share_mem(sm_com);
#endif 

   sk_outputmode = -1;
   sk_chanptr = -1;
   sk_stereomode = -1;
   sk_dynrngscalehi = -1.0;
   sk_dynrngscalelow = -1.0;   
   sk_compmode = -1;
   sk_dualmonomode = -1;
}

static char *prefer_downmix[] = {
   "not indicated",
   "Lt/Rt downmix preferred",
   "Lo/Ro downmix preferred",
   "reserved",
};   

static char *surround_EX[] = {
   "not indicated",
   "not Dolby Surround EX encoded",
   "Dolby Surround EX encoded",
   "reserved",
};   

static char *Headphone[] = {
   "not indicated",
   "not Dolby Headphone encoded",
   "Dolby Headphone encoded",
   "reserved",
};   

static char *AD_Converter[] = {
   "Standard",
   "HDCD",
};   

static char **ECB_item[] = {
   prefer_downmix,
   surround_EX,
   Headphone,
   AD_Converter
};     
   
static char *ECB_name[] = {
   "Preferred 2/0 Downmix",   
   "Dolby Surround EX Mode",
   "Dolby Headphone Mode",
   "A/D Converter Type",
};   

static void handle_ECB_Flag(int EBC_f)
{
   int i;
   int mask = 0xF;
   char *name;
   char **item;
   if (EBC_f != g_EBC){
      int t = EBC_f^g_EBC;
      for (i=0; i<4; i++){
         if (t&mask){
            int t2;
            name = ECB_name[i];
            item = ECB_item[i];   
            t2 = EBC_f & mask;
            //fprintf(stdout, "%x: %s - %s\n",EBC_f, name, item[t2>>(i*4)]); 
            fprintf(stdout, "%s - %s\n", name, item[t2>>(i*4)]); 
            g_EBC = (g_EBC & ~mask) | t2;
         }
         mask <<= 4;  
      }            
   }
}   


//#define SMPL_DEBUG
#define FADEIN_COEF 9
int oss_get_frag(int *fragments, int *fragstotal, int *fragsize);

int ui_exec_done;
#define MAX_NODATA_COUNT 20			//Honda 2011-03-17
static int decode_audio(sh_audio_t *sh_audio,unsigned char *buf,int minlen,int maxlen)
{
   int  len=-1, len2 = 0;
   int c, i_bytes_read;
   int EBC_f;
	int max_nodata_cnt = 0;			//Honda 2011-03-17
   union smc *sc = (union smc *)sm_com;
   play_decode *pi = &sc->pdecod;
   serv_ctrl *si = &sc->sctrl;
   
   
   ui_exec_done=0;
   while(len<minlen){   
      unsigned char *new_in;
#ifdef DOLBY_AT2      
      if (mute_sample>0){
         if (mute_sample > minlen)
            len2 = minlen;
         else
            len2 = mute_sample;  
         mute_sample -= len2;
         memset(buf, 0, len2);                         
      }      
      else
#endif         
      {  
         if (prev_frm_len){
            int i = (1<<FADEIN_COEF);
            short *sbuf = (short *)sm_out;
            len2 = prev_frm_len;
            prev_frm_len = 0;       
            if ((i*sh_audio->samplesize*audio_output_channels < len2) && DD_WORDSIZE24==0){
               int ph=0, j;
               do{
                  for (j=0; j<audio_output_channels;j++){
                     int t = *sbuf;
                     t *= ph;
                     t >>= FADEIN_COEF;
                     *sbuf++ = t;
                  }  
                  ph++;                   
                  i--;    
               }                  
               while (i>=0);                    
            }
         }
         else {            
	    int i_bytes_need;
            if ((ui_inp_size > i_bytes_consumed) && (i_bytes_consumed > 0))
               memmove(sm_in_align, sm_in_align+i_bytes_consumed, ui_inp_size - i_bytes_consumed); 
            new_in = sm_in_align + (ui_inp_size - i_bytes_consumed);         
	    if (i_bytes_consumed == 0)
	    {
		i_bytes_need = ui_inp_size - i_buff_size;
	    } else {
		i_bytes_need = i_bytes_consumed;
	    }
            i_bytes_read = demux_read_data(sh_audio->ds, new_in, i_bytes_need);	
#ifdef IAAC3_DECODE_PTS
	    if(startpts == MP_NOPTS_VALUE && sh_audio->ds->pts != MP_NOPTS_VALUE){
		    startpts = sh_audio->ds->pts;
		    printf("%%%%%%%%%%%%%%%%%%%%%%%%%% set startpts to %f %%%%%%%%%%%%%%%%%%\n", startpts);
	    }
#else
#if 0 //Polun 2012-01-06 for DLNA CCT test file O-MP2TS_SN-1.mpg.
if (sh_audio->ds->pts != MP_NOPTS_VALUE)
 sh_audio->pts = sh_audio->ds->pts;
//printf("sh_audio->ds->pts=%f\n", sh_audio->ds->pts);
#endif
#endif
	    if ((i_buff_size - (i_bytes_consumed - i_bytes_read) <= 0) && (i_bytes_read == 0))
	    {
		// no data to decode skip this time
		//puts("skip this audio decode");
		break;
	    }
	    if ((i_bytes_consumed > 0) && (i_buff_size - (i_bytes_consumed - i_bytes_read) < i_bytes_consumed))
	    {
		// (The day after tomorrow) - dvd, when buffer not enough will cause decoder done
		//printf("skip this audio decode, buffer not enough: %d\n", (i_buff_size - (i_bytes_consumed - i_bytes_read)));
		break;
	    }
            //if(sh_audio->format==0x2000) 
            //   swab(new_in, new_in, i_bytes_read);         
				/* New buffer size */
				i_buff_size = i_buff_size - (i_bytes_consumed - i_bytes_read);  
				if (i_bytes_read != i_bytes_need){		//Honda 2011-03-17
            	max_nodata_cnt++;
            	if (max_nodata_cnt > MAX_NODATA_COUNT)
      				break;
            }

				pi->paremeter1 = i_bytes_consumed;
#if 1
		int ret;
		ret = ipc2_decode(sc, IAAC3_ID, i_buff_size, id);
		if(!ret)
			break;
		else if(ret < 0)
			return -1;
#else
				if (!ipc2_decode(sc, IAAC3_ID, i_buff_size, id))
			      break;		//IPC Error 
#endif

            c = si->paremeter1;
            i_buff_size = si->paremeter2;
            //i_bytes_consumed = si->paremeter3;
            ui_exec_done = si->paremeter4;
            sh_audio->samplerate = si->paremeter5;
            EBC_f = si->paremeter6;
            len2 = si->data_len;	
           if(ui_exec_done == 1)
           {
		printf("@@@ In[%s][%d]  ui_exec_done:%d  need reinit audio @@@\n", __func__, __LINE__, ui_exec_done);
		extern unsigned int aserver_crash;
		aserver_crash = 1;
		return -1;
           }
	    if ((len2 == 0) && (si->paremeter3 == 0) && (i_buff_size > 0))
	    {
		// audio data can't decode. drop these audio data.
		//puts("audio decode loop!!!");
		i_bytes_consumed = 0;
		i_buff_size = 0;
		sh_audio->a_in_buffer_len = 0;
		break;
	    }
            i_bytes_consumed = si->paremeter3;
            sh_audio->a_in_buffer_len = i_buff_size - i_bytes_consumed;
            if (sh_audio->a_in_buffer_len < 0)
            {
              sh_audio->a_in_buffer_len = 0;
            }
	     if(i_bytes_consumed > ui_inp_size)		//Fuchun 2011.01.19
	     {
		 	i_bytes_consumed = ui_inp_size;
			break;
	     }
            if((c) & IA_FATAL_ERROR)
               break;   //function fail  
#if 0
            if (EBC_f != -1)
               handle_ECB_Flag(EBC_f);
#endif            
#ifndef  EngineMode           
//checking sample rate sweep
            if (sh_audio->a_out_buffer && cur_sr != sh_audio->samplerate && ao_fd >= 0){
               int t, sr;               
	            cur_sr = sh_audio->samplerate;
#ifdef SMPL_DEBUG	
	            if (cur_sr == 44100)
	               fprintf(stdout,"\nB:Current sampling rate is %d\n",cur_sr); 
               else if (cur_sr == 32000)	  
         	      fprintf(stdout,"\nC:Current sampling rate is %d\n",cur_sr); 
         	   else             
	      	      fprintf(stdout,"\nA:Current sampling rate is %d\n",cur_sr);  
#endif
#ifdef NEW_AUDIO_FILL_BUFFER
		int rlen, wlen, total_decode_len;
		int total_len = sh_audio->a_out_buffer_size*AUDIO_OUT_QUEUE_NUM;
#endif
#if 1	         
               if (DD_WORDSIZE24==0)   
               {
                  int fragments, fragstotal, fragsize;    
                  int base = sh_audio->samplesize*audio_output_channels;  
                  int fade_len = base<<FADEIN_COEF;
                  int half_tf, fg_mute, fg_release;
                  int j;
                  if(oss_get_frag(&fg_release, &fragstotal, &fragsize)==0)
                     goto error_frag;
#ifdef NEW_AUDIO_FILL_BUFFER
			rlen = sh_audio->r_a_out_buffer_len;
			wlen = sh_audio->w_a_out_buffer_len;
			total_decode_len = (wlen < rlen ? total_len-rlen+wlen : wlen-rlen);
			if(total_decode_len < fade_len)
			{
				memset(sh_audio->a_out_buffer+wlen, 0, fade_len-total_decode_len);
				sh_audio->w_a_out_buffer_len += (fade_len-total_decode_len);
				if(sh_audio->w_a_out_buffer_len >= total_len)
				{
					int memmove_len =  sh_audio->w_a_out_buffer_len - total_len;
					memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[total_len], memmove_len);
					sh_audio->w_a_out_buffer_len = memmove_len;
				}
			}
#else
                  if (sh_audio->a_out_buffer_len < fade_len)                     
                     memset(sh_audio->a_out_buffer+sh_audio->a_out_buffer_len, 0, fade_len-sh_audio->a_out_buffer_len);
#endif
                  {
#ifdef NEW_AUDIO_FILL_BUFFER
			wlen = sh_audio->w_a_out_buffer_len;
			short *sbuf = (short *)(&sh_audio->a_out_buffer[rlen]);
			int i = 1<<FADEIN_COEF;
			int ph=i-1;
			int memmove_flag = 0;
			int memmove_len;
			if((total_len-rlen) < i*audio_output_channels)
			{
				memmove_flag = 1;
				memmove_len = i*audio_output_channels - (total_len-rlen);
				memmove(&sh_audio->a_out_buffer[total_len], sh_audio->a_out_buffer, memmove_len);
			}
			do
			{
				for (j=0; j<audio_output_channels;j++)
				{
					int t = *sbuf;
					t *= ph;
					t >>= FADEIN_COEF;
					*sbuf++ = t;
				}
				ph--;
				i--;
			}while(i >= 0);
			if(memmove_flag)
			{
				memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[total_len], memmove_len);
			}
#else
                     short *sbuf = (short *)sh_audio->a_out_buffer;
                     int i = 1<<FADEIN_COEF;
                     int ph=i-1;
                     do{
                        for (j=0; j<audio_output_channels;j++){
                           int t = *sbuf;
                           t *= ph;
                           t >>= FADEIN_COEF;
                           *sbuf++ = t;
                        }  
                        ph--;                   
                        i--; 
                     }                  
                     while (i>=0);      
#endif
                  }                                           
                  half_tf = fragstotal>>1;
                  fg_mute = half_tf*fragsize;
                  j = fg_mute%base;
                  fg_mute -= j;
#ifdef NEW_AUDIO_FILL_BUFFER
			wlen = sh_audio->w_a_out_buffer_len;
			total_decode_len = (wlen < rlen ? total_len-rlen+wlen : wlen-rlen);
			if(total_decode_len < fg_mute)
			{
				memset(sh_audio->a_out_buffer+wlen, 0, fg_mute-total_decode_len);
				sh_audio->w_a_out_buffer_len += (fg_mute-total_decode_len);
				if(sh_audio->w_a_out_buffer_len >= total_len)
				{
					int memmove_len =  sh_audio->w_a_out_buffer_len - total_len;
					memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[total_len], memmove_len);
					sh_audio->w_a_out_buffer_len = memmove_len;
				}
			}
			
#else
                  if (fg_mute > fade_len)
                     memset(sh_audio->a_out_buffer+fade_len, 0, fg_mute-fade_len);
#endif
                  j=0;                     
                  do{       
                     usec_sleep(5000);              
                     if(oss_get_frag(&fragments, &fragstotal, &fragsize)==0)
                        goto error_frag;    
                     j++;    
#ifdef SMPL_DEBUG                         
                     printf("s1: %d, %d, %d\n", fragments, half_tf, j);      
#endif                                   
                  }
                  while((fragments < half_tf+1) && j<1000);  
#ifdef NEW_AUDIO_FILL_BUFFER
			j = write(ao_fd,&sh_audio->a_out_buffer[rlen],fg_mute);
#else
                  j = write(ao_fd,sh_audio->a_out_buffer,fg_mute);
#endif
                  j=0;                     
                  do{       
                     usec_sleep(5000);              
                     if(oss_get_frag(&fragments, &fragstotal, &fragsize)==0)
                        goto error_frag;    
                     j++;       
#ifdef SMPL_DEBUG                       
                     printf("s2: %d, %d, %d\n", fragments, fg_release, j); 
#endif                             
                  }
                  while((fragments<(half_tf+1)) && j<1000);  
               }    
error_frag:                             
#endif                        
	            sr = cur_sr;   
               ioctl(ao_fd, SNDCTL_DSP_SPEED, &sr);	      
#ifdef NEW_AUDIO_FILL_BUFFER
		wlen = sh_audio->w_a_out_buffer_len;
		total_decode_len = (wlen < rlen ? total_len-rlen+wlen : wlen-rlen);
		printf("flush %d bytes, sampling rate %d\n", sh_audio->a_buffer_len+total_decode_len+len, sr);	
#else
               printf("flush %d bytes, sampling rate %d\n", sh_audio->a_buffer_len+sh_audio->a_out_buffer_len+len, sr);	
#endif
               ao_data.samplerate = sr;
               t = audio_output_channels*sh_audio->samplesize*1024;
               memset(sh_audio->a_buffer, 0, t);
               sh_audio->a_buffer_len = 0;
#ifdef NEW_AUDIO_FILL_BUFFER
		sh_audio->r_a_out_buffer_len = sh_audio->w_a_out_buffer_len;
#else
               sh_audio->a_out_buffer_len = 0;
#endif
               prev_frm_len = len2;    //current frame length, waiting for next decode_audio
               return t;
#endif    //EngineMode      
	      	}  
         }
                              
#ifdef IAAC3_DECODE_PTS
	 if(startpts == MP_NOPTS_VALUE){
		 printf("startpts is MP_NOPTS_VALUE\n");
	 }else if(len2 > 0){
		 double tpast;
		 decsum += len2;
		 tpast = (double)decsum / (double)sh_audio->o_bps;
		 sh_audio->pts = startpts + tpast;
		 printf("%llu / %d = %f + %f = %f\n", decsum, sh_audio->o_bps, tpast, startpts, sh_audio->pts);
	 }
#endif

#if (defined(RVDS))      
         memcpy(buf, sm_out, len2);  //copy data to output 
#else
         if (DD_WORDSIZE24==0){ 
            memcpy(buf, sm_out, len2);  //copy data to output 
        
#ifdef DOLBY_OPTION         
            if(audio_output_channels==6)
        	      ac3d_delay_func((short*)buf,len2);
#endif        	   
         }         
         else            
         {
        	  int i, nlen = 0;
       	  unsigned char d[3] , *s;
       	  int *d4, i4;
       	  s = sm_out;
       	  d4 = (int*)buf;
       	  for (i=0; i<len2; i+=3){
       		  d[0] = *s++;
       		  d[1] = *s++;
       		  d[2] = *s++;
              i4 = (d[2]<<24)+(d[1]<<16)+(d[0]<<8);
              i4 >>= 8;    		  
       		  *d4 = i4;
       		  d4++;
       		  nlen += 4;		
       	  }
       	  len2 = nlen;
         }
      }         
#endif      
      if(len<0) 
         len=len2; 
      else 
         len+=len2;
      buf += len2;
      if (ui_exec_done)
         break;
   }

   return len;//len;
}


static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
   case ADCTRL_RESYNC_STREAM:
      i_bytes_consumed = ui_inp_size;
      i_buff_size = ui_inp_size;
   return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}



#ifdef EngineMode
ad_functions_t mp_skdac3 = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif


