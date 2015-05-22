/* ad_faad.c - MPlayer AAC decoder using libfaad2
 * This file is part of MPlayer, see http://mplayerhq.hu/ for info.  
 * (c)2002 by Felix Buenemann <atmosfear at users.sourceforge.net>
 * File licensed under the GPL, see http://www.fsf.org/ for more info.
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef  EngineMode 

#include "config.h"
#include "ad_internal.h" 


static ad_info_t info = 
{
   "Skyviia AMR-NB decoder",
   "skamrnb",       //compare to sh_audio->codec->drv
   "Archin",
   "Archin",
   "Skyviia copyright"
};

LIBAD_EXTERN(skamrnb)
#else

#include "mp_msg.h"
#include "help_mp.h"
#include "amrnb_mplayer.h"

typedef unsigned char uint8_t;
#define MP_NOPTS_VALUE (-1LL<<63)

#endif

typedef unsigned char UWord8;
typedef signed char Word8;
typedef signed short Word16;


int AMRNB_get_prev_mode(void *state);

void AMRNB_set_prev_mode(void *state, int mode);

int UnpackBits (
    Word8  q,              /* i : Q-bit (i.e. BFI)        */
	Word16 ft,             /* i : frame type (i.e. mode)  */
    UWord8 packed_bits[],  /* i : sorted & packed bits    */
	int *mode,       /* o : mode information        */
    Word16 bits[]          /* o : serial bits             */
);

int Speech_Decode_Frame (
    void *st, /* io: post filter states                */
    int mode,               /* i : AMR mode                          */
    Word16 *serial,               /* i : serial bit stream                 */
    int frame_type,  /* i : Frame type                        */
    Word16 *synth                 /* o : synthesis speech (postfiltered    */
                                  /*     output)                           */
);

int Speech_Decode_Frame_init (void **st, char *id);
void Speech_Decode_Frame_exit (void **st);
Word16 decoder_homing_frame_test_first (Word16 input_frame[], int mode);
Word16 decoder_homing_frame_test (Word16 input_frame[], int mode);
int Speech_Decode_Frame_reset (void *st);

#define MAX_SERIAL_SIZE 244    /* max. num. of serial bits                 */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)
#define L_FRAME      160       /* Frame size                               */
#define EHF_MASK 0x0008        /* encoder homing frame pattern             */

typedef struct AMRContext {
    int frameCount;
    //Speech_Decode_FrameState *speech_decoder_state;
    void *speech_decoder_state;
    int rx_type;
    //enum Mode mode;
    int mode;
    Word16 reset_flag;
    Word16 reset_flag_old;
 
} AMRContext;


static int preinit(sh_audio_t *sh)
{
   sh->audio_out_minsize=320;
   sh->context = malloc(sizeof(AMRContext));
   return 1;
}

	
static int init(sh_audio_t *sh)
{
    AMRContext *s = sh->context;

	//printf("amr_nb_fixed_decode_init <libamr.c>\n");

    s->frameCount=0;
    s->speech_decoder_state=NULL;
    s->rx_type = 0;
    s->mode= 0;
    s->reset_flag=0;
    s->reset_flag_old=1;

    if(Speech_Decode_Frame_init(&s->speech_decoder_state, "Decoder"))
    {
        mp_msg(MSGT_DECAUDIO,MSGL_WARN, "Speech_Decode_Frame_init error\n");
        free(sh->context);
        sh->context = NULL;    
        return 0;
    }

#if 0
    amr_decode_fix_avctx(avctx);

    if(avctx->channels > 1)
    {
        mp_msg(MSGT_DECAUDIO,MSGL_WARN, "amr_nb: multichannel decoding not supported\n");
        return 0;
    }
#endif

    return 1;

}

static void uninit(sh_audio_t *sh)
{
 
   AMRContext *s = sh->context;
   Speech_Decode_Frame_exit(&s->speech_decoder_state);
   free(sh->context);
   sh->context = NULL;   
}

static Word16 packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};

static int amr_nb_decode_one_frame(sh_audio_t *sh, void *data, int *data_size, uint8_t * buf, int buf_size)
{
    AMRContext *s = sh->context;
    uint8_t*amrData=buf;
    int offset=0;
    UWord8 toc, q, ft;
    Word16 serial[SERIAL_FRAMESIZE] ;   /* coded bits */
    Word16 *synth;
    UWord8 *packed_bits;
    
    int i;

    //printf("amr_decode_frame data_size=%i buf=0x%X buf_size=%d frameCount=%d!!\n",*data_size,buf,buf_size,s->frameCount);
    //printf("amr_nb_decode_frame <libamr.c>\n");
    synth=data;

    toc=amrData[offset];
    /* read rest of the frame based on ToC byte */
    q  = (toc >> 2) & 0x01;
    ft = (toc >> 3) & 0x0F;

    //printf("offset=%d, packet_size=%d amrData= 0x%X %X %X %X\n",offset,packed_size[ft],amrData[offset],amrData[offset+1],amrData[offset+2],amrData[offset+3]);

    offset++;

    packed_bits=amrData+offset;

    offset+=packed_size[ft];

    //Unsort and unpack bits
    s->rx_type = UnpackBits(q, ft, packed_bits, &s->mode, &serial[1]);

    //We have a new frame
    s->frameCount++;

    if (s->rx_type == 7)
    {
        s->mode = AMRNB_get_prev_mode(s->speech_decoder_state);
    }
    else {
        //s->speech_decoder_state->prev_mode = s->mode;
        AMRNB_set_prev_mode(s->speech_decoder_state, s->mode);
    }

    /* if homed: check if this frame is another homing frame */

    if (s->reset_flag_old == 1)
    {
        /* only check until end of first subframe */
        s->reset_flag = decoder_homing_frame_test_first(&serial[1], s->mode);
    }
    /* produce encoder homing frame if homed & input=decoder homing frame */
    if ((s->reset_flag != 0) && (s->reset_flag_old != 0))
    {
        for (i = 0; i < L_FRAME; i++)
        {
            synth[i] = EHF_MASK;
        }
    }
    else
    {
        /* decode frame */
        Speech_Decode_Frame(s->speech_decoder_state, s->mode, &serial[1], s->rx_type, synth);
       

    }

    //Each AMR-frame results in 160 16-bit samples
    *data_size=160*2;

    /* if not homed: check whether current frame is a homing frame */
    if (s->reset_flag_old == 0)
    {
        /* check whole frame */
        s->reset_flag = decoder_homing_frame_test(&serial[1], s->mode);
    }
    /* reset decoder if current frame is a homing frame */
    if (s->reset_flag != 0)
    {
        Speech_Decode_Frame_reset(s->speech_decoder_state);
    }
    s->reset_flag_old = s->reset_flag;

    return offset;
}


static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
   int j = 0, len = -1;	

   while(len < minlen ) {
      int y;
      int len2=0;
      unsigned char* bufptr=NULL;
      double pts;
      int buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
      if(buflen<=0) 
         break;     //EOF
      if (pts != MP_NOPTS_VALUE) 
      {
	      sh->pts = pts;
	      sh->pts_bytes = 0;
      }
         //if (len==0xe000)
         //   len = len;
         //faac_sample_buffer = faacDecDecode(faac_hdec, &faac_finfo, bufptr, buflen);
      y=amr_nb_decode_one_frame(sh, buf, &len2, bufptr, buflen);
      if(y<0)
	   { 
	      mp_msg(MSGT_DECAUDIO,MSGL_V,"AMR_audio: error\n");
	      break; 
	   }
      if(y<buflen) 
	      sh->ds->buffer_pos+=y-buflen;  // put back data (HACK!)
	   if(len2>0){
	      // first decoding
         if(len<0) 
            len=len2; 
         else 
	         len+=len2;
	      buf+=len2;
	      sh->pts_bytes += len2;
	   }
      mp_dbg(MSGT_DECAUDIO,MSGL_DBG2,"Decoded %d -> %d  \n",y,len2);
   }
   return len;
}

#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
   switch(cmd){
      case ADCTRL_RESYNC_STREAM:
         //avcodec_flush_buffers(lavc_context);
      return CONTROL_TRUE;
   }
   return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skamrnb = {
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif




