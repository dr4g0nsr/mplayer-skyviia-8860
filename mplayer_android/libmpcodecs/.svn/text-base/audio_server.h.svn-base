
//command id
#define PLAYER_MAGIC 0x1234
#define SERVER_MAGIC 0x5678
#define PLAYER_INIT 0x9abc
#define PLAYER_DECODE 0xdef0
#define PLAYER_CTRL 0x0168
#define PLAYER_UNINIT 0x6543
#define SERVER_QUIT 0xEDCB


//codec id
#define MP3_ID 0x2e6d7033
#define AAC_ID 0x41414320
#define WMAPRO_ID 0x57500162
#define COOK_ID 0x636f6f6b
#define AC3_ID 0x73616333
#define WMA_ID 0x4d530161
#define FLAC_ID 0x43614c66
#define AMRNB_ID 0x73616d72
#define DTS_ID 0x61445453
#define OGG_ID 0x76726273
#define MCHMP3_ID 0x4d4d5033 //2010.06.25 add
#define APE_ID 0x41504544 //2011.02.09 add
#define AMRWB_ID 0x7361620d //2011.03.23 add


#define SM_COM_SIZE 1024
#define AD_SHM_REWRITE
#ifdef AD_SHM_REWRITE
#define SM_DATA_IN 128*1024*3
#define SM_DATA_OUT 128*1024*3
#endif

typedef struct 
{
  int  magic_num;	/* magic number. */
  int  codec_id;	/* codec id */
} smc_magic;


typedef struct 
{
	int sh_codecdata_len ; 
   int sh_samplerate ; 
   int sh_samplesize ; 

  

} AAC_info;

typedef struct 
{
	int sh_data_len ; 
   int sh_samplerate ; 
   int sh_samplesize ; 

   int sample_format ; 

} AC3_info;


typedef struct 
{
	int bitrate;
	int samplerate;
	int ch;
   int sh_i_bps;//AAC
   int AAC_MODE ;//AAC
   int skaac_ini;//AAC 


} aduio_info;

typedef struct 
{
   int ch;
   int sample_rate;
   int bit_rate;
   int block_align;
   int format;
   int flags2 ; 

} WMA_info;

typedef struct 
{
  //  unsigned char* extradata;
   int Ogghsizes[3];
    void *Oggheaders[3];
} OGG_info;

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
   AAC_info AACinfo; 
   AC3_info AC3info ; 
   WMA_info WMAinfo ;
   OGG_info OGGinfo ; 
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
    unsigned long bytesconsumed;
    unsigned char error;

} AAC_ret;

typedef struct 
{
	int codec_id;	//codec id or error code
	int data_len;  //output pcm length for this frame	
   int paremeter1;
   int paremeter2;
   int paremeter3;
   AAC_ret finfo ;
   int paremeter4;
   int paremeter5;
   int paremeter6;
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
int ipc_init(union smc *sc, int codec_id, int data_len, int id, int shmid, int shmid_len);
int ipc_decode(union smc *sc, int codec_id, int data_len, int id);
int ipc_uninit(union smc *sc, int codec_id, int id);
int ipc_ctrl(union smc *sc, int codec_id, int command, int id);


//function call for mp3
int samp3_init(unsigned long length, aduio_info *ai, int init, int *next_frame_offset);
int samp3_decode(unsigned long length, int synth, int *len, int *next_frame_offset);
int samp3_uninit(void);
int samp3_sync(void);


//function call for MCHMP3
int samchmp3_init(unsigned long length, aduio_info *ai, int init,int chan_confi,int chan_num);
int samchmp3_decode(unsigned long length, int synth, int *len);
int samchmp3_uninit(void);


//for AAC
#ifdef  EngineMode  //≈Á√“•Œonly
typedef struct {
     int** data;
     int len;
     int ch;
    }PCM32S;


#endif

		

//function call for AAC
int saaac_init(unsigned long length, aduio_info *ai, int init,int chan_confi,int chan_num,AAC_info s_info);
int saaac_decode(unsigned long length, int consumed_ptr, int *len,AAC_ret *finfo);
int saaac_uninit(void);


//function call for AC3
int saac3_init(unsigned long length, aduio_info *ai, int ac3level,int chan_confi,int chan_num,AC3_info s_info);
int saac3_decode(unsigned long length, int parem1, int *len,AAC_ret *finfo);
int saac3_uninit(void);




//function call for DTS
int sadts_init(unsigned long length, aduio_info *ai, int format,int chan_confi,int chan_num);
int sadts_decode(unsigned long length, int parem1, int *len); 
int sadts_uninit(void);




//function call for WMA
int sawma_init(unsigned long length, aduio_info *ai, int pare1,int pare2,int chan_num,WMA_info s_info);
int sawma_decode(unsigned long length, int *parem1, int *len); 
int sawma_uninit(void);
int sawma_control(void);



//function call for AMRNB
int saamrnb_init(unsigned long length, aduio_info *ai, int format,int chan_confi,int chan_num);
int saamrnb_decode(unsigned long length, int *parem1, int *len); 
int saamrnb_uninit(void);
int saamrnb_control(void);

//function call for ogg
int saogg_init(unsigned long length, aduio_info *ai,  OGG_info ogginfo, int *next_frame_offset);
int saogg_decode(unsigned long length, int do_synth, int *len, aduio_info *ai);
int saogg_uninit(aduio_info *ai);


//function call for APE 
int saape_init(unsigned long length, aduio_info *ai, int format,int chan_confi,int chan_num);
int saape_decode(unsigned long length, int force,int *parem1,int *parem2, int *len); 
int saape_uninit(void);
int saape_control(void);

//function call for AMRWB
int saamrwb_init(void);
int saamrwb_decode(long *clen);
void saamrwb_uninit(void);


