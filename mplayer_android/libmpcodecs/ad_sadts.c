
/*
//------------------------------------------------------------------------------------------------------
//                                   skyviia DTS decoder
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

#include <assert.h>
#include "config.h"
#include "mp_msg.h"
#include "ad_internal.h"



static ad_info_t info =
{
    "Skyviia DTS core decoder",
    "sadts",
    "Honda",
    "Honda",
    "Skyviia copyright"
};

LIBAD_EXTERN(sadts)     // register mp_sadts codec

#define mp_msg_p2 mp_msg

#else

#include "dts_mplayer.h"

sh_audio_t *sh_v ; //驗證用only

#endif






//DRC control on(100)/off 
extern unsigned int night_mode ;


static unsigned char *buf_in;

//#define SAVE_ES
#ifdef SAVE_ES
static FILE *fp;
#endif


extern unsigned int dts_packet ;
static int sync;
#define  DTSBUFFER_SIZE 18726
#define  INIT_RETRY 3
#define  DTSHEADER_SIZE 14




#include "audio_server.h"


extern int audio_output_channels;


extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;


//--------------------------bistream function-----------------------------
struct dts_state {

    unsigned short* word;
    unsigned int re_cache;
    int re_left;
    unsigned char word_mode;
    unsigned char bigendian_mode;
    unsigned short frame_size;
    unsigned char amode;
    unsigned char sample_rate;
    unsigned char bit_rate;

};

typedef struct dts_state  sadts_state_t;

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


void sadts_cache_14(void* st)
{
    sadts_state_t* state = st;
    unsigned int tmp = (*state->word)&0x3fff;
    state->re_cache = (state->re_cache<<14) | tmp;
    state->word++;
    state->re_left += 14;
}

void sadts_cache_16(void* st)
{
    sadts_state_t* state = st;
    unsigned int tmp = (*state->word);
    state->re_cache = (state->re_cache<<16) | tmp;
    state->word++;
    state->re_left += 16;
}


static  int alignment[14] ;
void sadts_bitstream_init (sadts_state_t * state, unsigned char * buf, int word_mode,
                         int bigendian_mode)
{
 //state->word = (unsigned short *)buf;  

   memcpy((unsigned char *)alignment,buf,14);
   state->word = alignment;


    state->re_cache = 0;
    state->re_left = 0;
    state->word_mode = word_mode;
    state->bigendian_mode = bigendian_mode;

}



sadts_state_t * sadts_bits_ini(void)
{
    sadts_state_t * state;
    state = (sadts_state_t *) malloc (sizeof (sadts_state_t));
    memset (state, 0, sizeof(sadts_state_t));


    return state;
}

static  unsigned long sadts_bit_read(sadts_state_t* state, int len)
{
    unsigned int val;
    if(state->re_left < len)
    {
        if (state->word_mode ==1)
            sadts_cache_16(state);
        else
            sadts_cache_14(state);
    }

    state->re_left -= len;
    val = ((state->re_cache >> state->re_left) & ((1<<len)-1));
    return val;
}

static  unsigned int sadts_bitstream_read(sadts_state_t * state, int num_bits)
{
    unsigned int val = 0;
    int len = 0;
    while (len < num_bits) {
        unsigned int tmp;
        int lin = num_bits-len;
        if (lin>14)
            lin = 14;
        tmp = sadts_bit_read(state, lin);
        val = (val<<lin)+tmp;
        len += lin;
    }
    return val;
}





static int sa_syncinfos (sadts_state_t * state, int * flags,int * sample_rate, int * bit_rate)
{
    int frame_size;
    unsigned int ut32;


    sadts_bitstream_read (state, 32);
    ut32 = sadts_bitstream_read(state, 28);

    //find frame size
    frame_size = (ut32&0x3FFF) + 1;
    if (!state->word_mode)
        frame_size = frame_size * 8 / 14 * 2;
    ut32 = sadts_bitstream_read(state, 27);

    state->amode = *flags = ut32>>21;
    if (*flags > 63)
        return 0;

    //find sample rate
    state->sample_rate = *sample_rate = (ut32>>17)&0xF;
    if ((size_t)*sample_rate >= 16)
        return 0;
    *sample_rate = dts_sample_rates[ *sample_rate ];
    if (!*sample_rate)
        return 0;


    //find bit rate
    state->bit_rate = *bit_rate = (ut32>>12)&0x1F;
    if ((size_t)*bit_rate >= 32)
        return 0;
    *bit_rate = dts_bit_rate[ *bit_rate ];
    if (!*bit_rate)
        return 0;

    return frame_size;
}


void WordSwap(unsigned char* x, int len)
{
    int i;
    unsigned short* buf = (unsigned short*)x;
    len = (len+1)>>1;
    for (i=0; i<len; i++) {
        x = (unsigned char *)buf;
        *buf = (x[0]<<8)+x[1];
        buf++;
    }
}




int sadts_syncinfo (sadts_state_t * state, unsigned char * buf, int * flags,
                  int * sample_rate, int * bit_rate, int * frame_length, int buflen,int cons)
{
   int frame_size = 0 ;
      //int *buf32 = (int *)buf;  2011.05.04 fix bug
     int sync1 = (buf[3]<<24)+(buf[2]<<16)+(buf[1]<<8)+buf[0];
     int sync2 ;


     /* 14 bits and little endian bitstream */
   if (sync1 == 0xe8001fff )
   {
        sync2 = (buf[7]<<24)+(buf[6]<<16)+(buf[5]<<8)+buf[4];
       if((sync2 & 0xfff0) == 0x07f0)
       {
         sadts_bitstream_init (state, buf, 0, 0);
          frame_size = sa_syncinfos (state, flags, sample_rate,
                               bit_rate);
	    //printf("14 bits and big little bitstream \n");
        return frame_size;
       }
   }

    /* 14 bits and big endian bitstream */
   if (sync1== 0xe8ff1f )
   {
        sync2 = (buf[7]<<24)+(buf[6]<<16)+(buf[5]<<8)+buf[4];
        if((sync2 & 0xf0ff) == 0xf007){
          WordSwap(buf, buflen);
          sadts_bitstream_init (state, buf, 0, 1);
          frame_size = sa_syncinfos (state, flags, sample_rate,
                               bit_rate);
        if(frame_size < buflen)
           WordSwap(buf, buflen);

          WordSwap(buf, buflen); // 2010.08.17 add 
	      //printf("14 bits and big endian bitstream \n");

         return frame_size;
        }
   }

    /* 16 bits and little endian bitstream */
   if (sync1 == 0x80017ffe)
   {
      //int frame_size;
      sadts_bitstream_init (state, buf, 1, 0);
      frame_size = sa_syncinfos (state, flags, sample_rate, bit_rate);
	   //  printf("16 bits and little endian bitstream \n");

       
      
      return frame_size;
   }



    /* 16 bits and big endian bitstream */
   if (sync1 == 0x0180fe7f)
   {
     
      WordSwap(buf, buflen);
      sadts_bitstream_init (state, buf, 1, 1);
      frame_size = sa_syncinfos (state, flags, sample_rate, bit_rate);
      if(frame_size < buflen)
         WordSwap(buf, buflen);

        WordSwap(buf, buflen); // 2010.08.17 add 
	   // printf("16 bits and big endian bitstream\n");

      
      return frame_size;
   }

   return 0;
}




//--------------------------------------------------------------------------------


/*
//-------------------------------------------------------------------------------
//     DTS core sync bistream
//
//--------------------------------------------------------------------------------
*/

#define Readbufferlen 95-1   // spec
static int sadts_sync(sh_audio_t *sh, int *flags )
{

    void *s = sh->context;
    int length;
    int sample_rate;
    int frame_length;
    int bit_rate;
    //CH add
    int fillin_ptr=0;
    int consumed_ptr = 0 ;
    int residue_num = 0 ;
    int mvlen = 0;
    int c ;


    if (dts_packet==0) {
        sh->a_in_buffer_len=0;
        sh->a_in_buffer_len = demux_read_data(sh->ds, sm_in, DTSHEADER_SIZE);
        residue_num = fillin_ptr = sh->a_in_buffer_len  ;

#ifdef SAVE_ES
        fwrite(sm_in, 1, DTSHEADER_SIZE, fp);
#endif
        if (sh->a_in_buffer_len < DTSHEADER_SIZE)
            return -1;

        while(1) {
            int c;
            length = sadts_syncinfo(s, sm_in + consumed_ptr, flags, &sample_rate,
                                    &bit_rate, &frame_length, DTSHEADER_SIZE,consumed_ptr);

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

                c = demux_read_data(sh->ds,(sm_in + fillin_ptr), mvlen);
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
        sh->a_in_buffer_len = residue_num ;
        c = demux_read_data(sh->ds, sm_in + residue_num, length - residue_num) ; //補齊
        if( c!=(length - residue_num))
            return -1;

        sh->a_in_buffer_len = length ;//2010.08.16
        buf_in =  sm_in;



#ifdef SAVE_ES
        fwrite( sm_in + residue_num, 1, length - residue_num, fp);
#endif
    }
    else {
        unsigned char* bufptr=NULL;
        int *psync;
        int buflen=ds_get_packet(sh->ds, &bufptr);
        if(buflen<=0)
            return -1;
        psync = (int*)bufptr;
        if (sync==0) {
            int retry = INIT_RETRY;
            while (1) {
                sync = *psync;
                length = sadts_syncinfo(s, bufptr, flags, &sample_rate,
                                        &bit_rate, &frame_length, DTSHEADER_SIZE , 0);
                if (length>buflen || length < DTSHEADER_SIZE) {
                    retry--;
                    if (retry < 0)
                        return -1;
                    buflen=ds_get_packet(sh->ds, &bufptr);
                    if(buflen<=0)
                        return -1;
                    psync = (int*)bufptr;
                }
                else
                    break;
            }
        }
        else {
            do {
                while (sync != *psync) {
                    buflen=ds_get_packet(sh->ds, &bufptr);
                    if(buflen<=0)
                        return -1;
                    psync = (int*)bufptr;
                }
                length = sadts_syncinfo(s, bufptr, flags, &sample_rate,
                                        &bit_rate, &frame_length, DTSHEADER_SIZE,0);
                if (length>buflen || length < DTSHEADER_SIZE) {
                    buflen=ds_get_packet(sh->ds, &bufptr);
                    psync = (int*)bufptr;
                    if (buflen<0)
                        return -1;
                }
                else
                    psync = NULL;
            }
            while(psync);
        }
        buf_in = bufptr;
        memcpy(sm_in,buf_in, length); //  bistream  to sm_in
    }







    sh->samplerate = sample_rate;
    sh->i_bps = bit_rate/8;

    return length;
}




/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
//
//------------------------------------------------------------------------------------------------------
*/
#define DTSBUFFER_SIZE 18726
static int preinit(sh_audio_t *sh)
{
    aserver_pid = 0;
#ifndef  EngineMode
    union smc *sc;
    long curr_codec;
#endif

    {

        sh->audio_out_minsize = audio_output_channels * sizeof(short) * 256 * 16;
        sh->audio_in_minsize = DTSBUFFER_SIZE;
        sh->samplesize=2;
    }





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

    if (!reset_semvalue(id, 0)) {                            //reset semaphore
        return 0;
    }

    sc = (union smc *)sm_com;

    sc->magic.magic_num = PLAYER_MAGIC;
    curr_codec = DTS_ID;                                     //inform server what kind of coded to be used.

    sc->magic.codec_id = curr_codec;
    if (!semaphore_v(id, 1))                                   //wakeup server!
        return 0;

    if (!wait_server(sc)) {                                   //wait for server's response
        deallocate_share_mem(sm_com);
#ifndef AD_SHM_REWRITE
        shmctl(sm_com_id, IPC_RMID, 0);
#endif
        del_semvalue(id, 0);
        printf("Audio server crash \n");
        return 0;
    }

    if (sc->magic.codec_id != curr_codec) {                  //check codec
        printf("Unsuppoted codec\n");
        return 0;
    }


#endif

    aserver_pid = sc->pinit.pid;

    return 1;
}





int DTS_frame_init(sh_audio_t *sh, aduio_info *ai,int buflen) {



    int init = 1 , ret;
    union smc *sc = (union smc *)sm_com;
    play_init *pi = &sc->pinit;
    serv_init *si = &sc->sinit;



    //send to server
    //memcpy(sm_in,sh->a_in_buffer, buflen);   //bistream  to sm_in

    sh->channels = audio_output_channels;

    //some parameters to server for ini
    //pi->paremeter1 = bit_format;
    pi->paremeter3 = audio_output_channels ;           //send channel number to server


    if(!ipc_init(sc, DTS_ID, buflen, id, sm_data_id, 0)) //init server
        return 0;//break;		//IPC Error
    if (si->codec_id == DTS_ID)
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
static buf_len = 0 ;
static int init(sh_audio_t *sh)
{
    int output_channels;
    int ret_init=0;

    int flags;
    void *s;



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
    sm_data_id = get_shared_mem_id(KEY_SM_IN, DTSBUFFER_SIZE);
    if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
    {
        printf("data share memory allocate fail\n");
        return 0;
    }
#endif
#endif

    output_channels = audio_output_channels;



    s = sadts_bits_ini();
    if(s == NULL) {
        printf( "dts_init() failed\n");
        return 0;
    }
    sh->context = s;


    buf_len = sadts_sync(sh, &flags);
    if(buf_len < 0) {
        printf("dts sync fail\n");
        free(s);

        return 0;
    }







    ret_init = DTS_frame_init(sh,ai,buf_len) ;  //ai:audio info

    int decoded_bytes = 0;
    decoded_bytes = decode_audio(sh, (unsigned char *)sh->a_buffer, 1, sh->a_buffer_size);
    if(decoded_bytes > 0)
        sh->a_buffer_len = decoded_bytes;
    else {
        mp_msg(MSGT_DECAUDIO, MSGL_ERR, "dts decode failed on first frame (up/downmix problem?)\n");
//        skdts_free(s);
//        return 0;
        free(s);

        return 0;

    }

    //i_bps,channels,smplerate get from
    as_channels = ai->ch;	//Fuchun 2011.05.18

    //sh->i_bps = ai->sh_i_bps ;
    //sh->channels = ai->ch;;
    //sh->samplerate = ai->samplerate;
    //sh->samplesize=2;



    return 1;
}

/*
//-----------------------------------------------------------------------------------------
//    DTS decoder
//
//-----------------------------------------------------------------------------------------
*/

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{


    int   len=-1 ;
    int   nsamples = 0;
    union smc *sc = (union smc *)sm_com;     //  shared memory

    int   flags  ;


#ifdef  EngineMode
    sh_v = sh ;
#endif
//printf("dts_packet=%d buf_in=%p\n", dts_packet, buf_in);
    //sync and fetch

    if(buf_in == NULL)
    {
        buf_len = sadts_sync(sh, &flags);
        if( buf_len<0)
            return -1; /* EOF */

    }



   
      sc->pdecod.paremeter1 = night_mode; //send to server 

#if 1
    int ret;
    ret = ipc_decode(sc, DTS_ID, buf_len, id);

    if(!ret)
        return 0;
    else if(ret < 0)
        return -1;
#else
    if (!ipc_decode(sc, DTS_ID, buf_len, id))
        return 0 ;//break;		//IPC Error
#endif
    if (sc->sctrl.codec_id == DTS_ID) { //decode success!
        nsamples = sc->sctrl.data_len;     //get from server
        memcpy( buf, sm_out , nsamples*2 );  // sm_out -->16 bit wav
    }




    len = nsamples ;

    buf_in = NULL;

    return len;




}

/*
//-----------------------------------------------------------------------------------------
//    free DTS decoder
//
//-----------------------------------------------------------------------------------------
*/
static void uninit(sh_audio_t *sh)
{
    union smc *sc = (union smc *)sm_com;
    ipc_uninit(sc, DTS_ID, id);

    //server
#ifndef  EngineMode
    deallocate_share_mem(sm_in);
    deallocate_share_mem(sm_out);
#ifndef AD_SHM_REWRITE
    shmctl(sm_data_id, IPC_RMID, 0);
#endif
    deallocate_share_mem(sm_com);
#endif


#ifdef SAVE_ES
    fclose(fp);
#endif
}




//--------------------------------------------------------------------------

#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    int flags;
    switch(cmd)
    {
    case ADCTRL_RESYNC_STREAM:
        if (dts_packet==0)
            sadts_sync(sh, &flags);
        //ipc_ctrl(sc, DTS_ID, 1, id);


        return CONTROL_TRUE;
    }
    return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...) {

    return 0;
}

ad_functions_t mp_skdts = {
    preinit,
    init,
    uninit,
    control,
    decode_audio
};

#endif



