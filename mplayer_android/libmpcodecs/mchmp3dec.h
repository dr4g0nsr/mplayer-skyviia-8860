
#ifndef __NEMCHMP3DEC_H__
#define __NEMCHMP3DEC_H__




typedef struct  {  struct mad_synth  synth;   struct mad_stream stream;  struct mad_frame  frame;
} mchmp3_decoder_t;


//mp3on4
typedef struct MchMP3DecodeContext{
    int frames;                      // number of mp3 decoder instances
    int chan_cfg;                    // channel config 
    int channels ;                   // channel number
    mchmp3_decoder_t *mp3decctx[5];  //  decoder instance
} MchMP3DecodeContext;



#endif
