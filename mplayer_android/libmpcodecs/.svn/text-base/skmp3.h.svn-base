# ifndef SKMP3_H
# define SKMP3_H

#ifndef mad_fixed_t
#define mad_fixed_t signed int 
#endif

# define MAD_F_FRACBITS		28


#  define MAD_F(x)		((mad_fixed_t) (x##L))


# define MAD_F_MIN		((mad_fixed_t) -0x80000000L)
# define MAD_F_MAX		((mad_fixed_t) +0x7fffffffL)

# define MAD_F_ONE		MAD_F(0x10000000)

/*
enum mad_layer {
  MAD_LAYER_I   = 1,			// Layer I 
  MAD_LAYER_II  = 2,			// Layer II 
  MAD_LAYER_III = 3			// Layer III 
};

enum mad_mode {
  MAD_MODE_SINGLE_CHANNEL = 0,		// single channel 
  MAD_MODE_DUAL_CHANNEL	  = 1,		// dual channel 
  MAD_MODE_JOINT_STEREO	  = 2,		//joint (MS/intensity) stereo 
  MAD_MODE_STEREO	  = 3		         //normal LR stereo 
};

enum mad_emphasis {
  MAD_EMPHASIS_NONE	  = 0,		// no emphasis 
  MAD_EMPHASIS_50_15_US	  = 1,		// 50/15 microseconds emphasis 
  MAD_EMPHASIS_CCITT_J_17 = 3,		// CCITT J.17 emphasis 
  MAD_EMPHASIS_RESERVED   = 2		// unknown emphasis 
};


*/




enum mad_error {
  MAD_ERROR_NONE	   = 0x0000,	/* no error */

  MAD_ERROR_BUFFRAME	   = 0x0001,	/* input buffer not enough for a frame decode */
  MAD_ERROR_BUFLEN	   = 0x0002,	/* input buffer too small (or EOF) */
  MAD_ERROR_BUFPTR	   = 0x0003,	/* invalid (null) buffer pointer */

  MAD_ERROR_NOMEM	   = 0x0031,	/* not enough memory */

  MAD_ERROR_LOSTSYNC	   = 0x0101,	/* lost synchronization */
  MAD_ERROR_BADLAYER	   = 0x0102,	/* reserved header layer value */
  MAD_ERROR_BADBITRATE	   = 0x0103,	/* forbidden bitrate value */
  MAD_ERROR_BADSAMPLERATE  = 0x0104,	/* reserved sample frequency value */
  MAD_ERROR_BADEMPHASIS	   = 0x0105,	/* reserved emphasis value */

  MAD_ERROR_BADCRC	   = 0x0201,	/* CRC check failed */
  MAD_ERROR_BADBITALLOC	   = 0x0211,	/* forbidden bit allocation value */
  MAD_ERROR_BADSCALEFACTOR = 0x0221,	/* bad scalefactor index */
  MAD_ERROR_BADMODE        = 0x0222,	/* bad bitrate/mode combination */
  MAD_ERROR_BADFRAMELEN	   = 0x0231,	/* bad frame length */
  MAD_ERROR_BADBIGVALUES   = 0x0232,	/* bad big_values count */
  MAD_ERROR_BADBLOCKTYPE   = 0x0233,	/* reserved block_type */
  MAD_ERROR_BADSCFSI	   = 0x0234,	/* bad scalefactor selection info */
  MAD_ERROR_BADDATAPTR	   = 0x0235,	/* bad main_data_begin pointer */
  MAD_ERROR_BADPART3LEN	   = 0x0236,	/* bad audio data length */
  MAD_ERROR_BADHUFFTABLE   = 0x0237,	/* bad Huffman table select */
  MAD_ERROR_BADHUFFDATA	   = 0x0238,	/* Huffman data overrun */
  MAD_ERROR_BADSTEREO	   = 0x0239	/* incompatible block_type for JS */
};



//LAYER3 320kbits 32kHz 每個frame大小為1440
# define MAD_BUFFER_GUARD	8
# define MAD_BUFFER_MDLEN	(512 + 1440 + MAD_BUFFER_GUARD)  

struct mad_bitptr {
   unsigned char const *byte;
   unsigned int re_cache;  
   int re_left;  //the cache left bit numbers
};


struct mad_header {
  unsigned char layer;			/* audio layer 1:layer1 , 2: layer2, 3: layer3) */   
  unsigned char mode;			/* channel mode 0:SINGLE_CHANNEL, 1:DUAL_CHANNEL, 2:JOINT_STEREO, 3:MODE_STEREO  */
  unsigned char mode_extension;			/* additional mode info */
  unsigned char emphasis;		/* de-emphasis to use 0:NONE, 1:50_15_US, 2:RESERVED, 3:CCITT_J_17 */

  unsigned long bitrate;		/* stream bitrate (bps) */
  unsigned int samplerate;		/* sampling frequency (Hz) */

  unsigned short crc_check;		/* frame CRC accumulator */
  unsigned short crc_target;		/* final target CRC checksum */
  
  int flags;				/* flags (see below) */
  int private_bits;			/* private bits (see below) */

};

struct mad_frame {
   struct mad_header header;		/* MPEG audio header */

  //int options;				/* decoding options (from stream) */
   mad_fixed_t sbsample[2][36][32];	/* synthesis subband filter samples */
   mad_fixed_t overlap[2][32][18];	/* Layer III block overlap data */
};

struct mad_pcm {
  unsigned int samplerate;		/* sampling frequency (Hz) */
  unsigned short channels;		/* number of channels */
  unsigned short length;		/* number of samples per channel */
  mad_fixed_t samples[2][1152];		/* PCM output samples [ch][sample] */
};

struct mad_synth {
   unsigned int phase;			/* current processing phase */
   mad_fixed_t filter[2][2][2][16][8];	/* polyphase filterbank outputs */
  					/* [ch][eo][peo][s][v] */
   struct mad_pcm pcm;			/* PCM output */
};

struct mad_stream {
  unsigned char const *buffer;		/* input bitstream buffer */
  unsigned char const *bufend;		/* end of buffer */
  //unsigned long skiplen;		/* bytes to skip before next frame */
  //long skiplen;		/* bytes to skip before next frame */

  //int sync;				/* stream sync found */
  unsigned long freerate;		/* free bitrate (fixed) */

  unsigned char const *this_frame;	/* start of current frame */
  unsigned char const *next_frame;	/* start of next frame */
  struct mad_bitptr ptr;		/* current processing bit pointer */

  struct mad_bitptr anc_ptr;		/* ancillary bits pointer */
  unsigned int anc_bitlen;		/* number of ancillary bits */

  unsigned char main_data[MAD_BUFFER_MDLEN];
					/* Layer III main_data() */
  unsigned int md_len;			/* bytes in main_data */

  //int options;				/* decoding options (see below) */
  enum mad_error error;			/* error code (see above) */
};


void skmp3_32to16(int *left, int *right, short *out, int len, int ch);
void skmchmp3_32to16(int *data_buf, short *out, int len, int ch);//mchmp3
void skmchmp3_32to16_6dm2(int *data_buf, short *out, int len, int ch);//mchmp3
void mad_synth_init(struct mad_synth *);
void mad_stream_init(struct mad_stream *);
void mad_stream_finish(struct mad_stream *);
void mad_frame_init(struct mad_frame *);
void mad_frame_finish(struct mad_frame *);
void mad_stream_buffer(struct mad_stream *, unsigned char const *, unsigned long);
int mad_frame_decode(struct mad_frame *, struct mad_stream *);
int mp3on4_frame_decode(struct mad_frame *, struct mad_stream *);//mp3on4 
void mad_synth_frame(struct mad_synth *, struct mad_frame const *);

#endif

