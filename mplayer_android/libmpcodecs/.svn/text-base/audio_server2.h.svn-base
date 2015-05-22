
//command id
#define PLAYER_MAGIC 0x1234
#define SERVER_MAGIC 0x5678
#define PLAYER_INIT 0x9abc
#define PLAYER_DECODE 0xdef0
#define PLAYER_CTRL 0x0168
#define PLAYER_UNINIT 0x6543
#define SERVER_QUIT 0xEDCB


//codec id
#define WMAPRO_ID 0x57500162
#define COOK_ID 0x636f6f6b
#define IAAC3_ID 0x69616333
#define DTSHD2_ID 0x44544844

#define SM_COM_SIZE 1024
#define AD_SHM_REWRITE
#ifdef AD_SHM_REWRITE
#define SM_DATA_IN 128*1024*3
#define SM_DATA_OUT 128*1024*3
#endif



//For decoder information
#define SM_IN_SIZE_WMAPRO 4096*16
#ifdef MAX_BUFFER_OUT
#define SM_OUT_SIZE_WMAPRO MAX_BUFFER_OUT*2
#endif

#define SM_IN_SIZE_RA 4096*16
#define SM_OUT_SIZE_RA 4096*48

#define SM_IN_SIZE_IAC3 4096
#define SM_OUT_SIZE_IAC3 256*6*4*6

#define SM_IN_SIZE_DTSHD2 4096*32
#define SM_OUT_SIZED_DTSHD2 6*4 * 256 * 16

typedef struct 
{
  int  magic_num;	/* magic number. */
  int  codec_id;	/* codec id */
} smc_magic;

typedef struct 
{
 int enableDownmix  ;
 int DwnMixLFEMIXSet  ;
 int destinationSpeakerMask  ; 
 int enableAnalogCompensation  ;
 int enableDialNorm  ; 
 int DRCPercent  ; 
 int lfeFilterType ;
} dtshd2_info;


typedef struct 
{
 int  decode_len ; 
 int  bps  ;
 int samplerate ;
 int ret ;
 int readindex ; 
 int ch_content  ;
} dtshd2_ret;

typedef struct 
{
	int bitrate;
	int samplerate;
	int ch;

} aduio_info;

typedef struct 
{
   int flag; 
   int nDstChannelMask; 
   int sample_rate;
   int ch; 
   int bit_rate;
   int wValidBitsPerSample; 
   int format; 
   int dwChannelMask;
   int nEncodeOpt; 
   int block_align; 
   int wBitsPerSample;
   int wAdvancedEncodeOptint; 
   int dwAdvancedEncodeOpt2; 
}wmapro_initpara;

typedef struct 
{
   int sk_kcapablemode;
   int sk_compmode;	
   int sk_outlfeon;	
   int sk_outputmode;
   int sk_stereomode;
   int sk_dualmonomode;
   int sk_chanptr;  //0x543210;
   float sk_dynrngscalehi;
   float sk_dynrngscalelow;
   float sk_pcmscalefac;		/* PCM scale factor */      
   int audio_output_channels;
   unsigned int DD_WORDSIZE24;
}iac3_initpara;  

union para_group{
   wmapro_initpara wpinit;
   iac3_initpara i3init;
      //dtshd2   
   dtshd2_info dtshd2info;
   dtshd2_ret  dtshd2ret ;
};

typedef struct 
{
	int com_id;
	int codec_id;	/* codec id */
	int shmid;  //shmid input stream 
	int shmid_len;  //shmid input stream 	
   int data_len;  //input stream length for this frame
   int paremeter1;
   int paremeter2;
   int paremeter3; 
   union para_group pg;
   unsigned int pid;
   
} play_init;


typedef struct 
{
	int codec_id;	//codec id or error code
	int shmid;  	//shmid output stream 
	int shmid_len;  //shmid output stream 	
   int data_len;  //output stream length for this frame
	aduio_info ai;
   int paremeter1;
   int paremeter2;
   int paremeter3;
} serv_init;	

typedef struct 
{
	int com_id;
	int codec_id;	/* codec id */
	int data_len;  //input stream length for this frame	
   int paremeter1;
   int paremeter2;
} play_decode;

typedef struct 
{
	int codec_id;	//codec id or error code
	int data_len;  //output pcm length for this frame	
   int paremeter1;
   int paremeter2;
   int paremeter3;
   int paremeter4;
   int paremeter5;
   int paremeter6;
   int paremeter7;
   int paremeter8;
   union para_group pg;
} serv_ctrl;

typedef struct 
{
	int com_id; 	//sync , uninit, audio_server quit 
	int codec_id;	/* codec id */
	int command;  //input stream length for this frame	
   int paremeter1;
   int paremeter2;
} play_ctrl;

union smc{
	int command;
   smc_magic magic;
	play_init pinit;
	serv_init sinit;
	play_decode pdecod;
	play_ctrl pctrl; 
	serv_ctrl sctrl;
};

typedef struct 
{
   int (*init)(union smc *sc);
   void (*uninit)(union smc *sc);
   void (*control)(union smc *sc);
   void (*decode_audio)(union smc *sc);
} ad_functions;

//function call for ipc
int ipc2_init(union smc *sc, int codec_id, int data_len, int id, int shmid, int shmid_len);
int ipc2_decode(union smc *sc, int codec_id, int data_len, int id);
int ipc2_uninit(union smc *sc, int codec_id, int id);
int ipc2_ctrl(union smc *sc, int codec_id, int command, int id);


//function call for wmapro
int sawmapro_init(int length, wmapro_initpara *wpinit, aduio_info *ai, int *out_para1, int *out_para2);
int sawmapro_decode(int length, int in_para, int *out_para);
int sawmapro_uninit(void);
int sawmapro_sync(void);

//function call for ra
int sara_init(unsigned char* pBuf, int len, int nFrameBits, int nChannels, int sampRate);
int sara_decode(int x, int *len2, int channels);
int sara_uninit(void);
int sara_sync(void);

//function call for iaac3
int saiac3_init1(iac3_initpara *i3init, int *para1, int *para2);
int saiac3_init2(int *i_buff_size, int *i_bytes_consumed, int *ui_init_done, int *i_samp_freq, int *i_bitrate, int *i_ch);
int saiac3_decode(int *len2, int *i_buff_size, int *i_bytes_consumed, int *ui_exec_done, int *i_samp_freq, int *EBC_flag, int *i_bitrate);
int saiac3_uninit(void);
int saiac3_sync(void);

//function call for DTSHD2 
int sadtshd2_init(unsigned long bswp_ad,  dtshd2_info *info,int chan_confi,int chan_num);
int sadtshd2_decode(unsigned long bswp_ad, int ch,int file_end_flag,dtshd2_ret*  dtshd2ret); 
int sadtshd2_uninit(void);
int sadtshd2_control(void);




