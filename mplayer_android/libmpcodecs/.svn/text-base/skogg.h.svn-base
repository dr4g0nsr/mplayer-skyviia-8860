#ifndef _vorbis_codec_h_
#define _vorbis_codec_h_
#endif /* __cplusplus */
#define VI_TRANSFORMB 1
#define VI_WINDOWB 1
#define VI_TIMEB 1
#define VI_FLOORB 2
#define VI_RESB 3
#define VI_MAPB 1
#define OV_FALSE      -1  
#define OV_EOF        -2
#define OV_HOLE       -3
#define VIF_POSIT 63
#define VIF_CLASS 16
#define VIF_PARTS 31

#define OV_EREAD      -128
#define OV_EFAULT     -129
#define OV_EIMPL      -130
#define OV_EINVAL     -131
#define OV_ENOTVORBIS -132
#define OV_EBADHEADER -133
#define OV_EVERSION   -134
#define OV_ENOTAUDIO  -135
#define OV_EBADPACKET -136
#define OV_EBADLINK   -137
#define OV_ENOSEEK    -138

typedef signed long long ogg_int64_t;
typedef signed int ogg_int32_t;
typedef unsigned int  ogg_uint32_t;
typedef unsigned short ogg_uint16_t;
typedef signed short ogg_int16_t;

typedef struct {
  unsigned char *header;
  long header_len;
  unsigned char *body;
  long body_len;
} ogg_page;

typedef struct{
  int   order;
  long  rate;
  long  barkmap;

  int   ampbits;
  int   ampdB;

  int   numbooks; /* <= 16 */
  int   books[16];

} vorbis_info_floor0;
typedef struct{
  int   partitions;                /* 0 to 31 */
  int   partitionclass[VIF_PARTS]; /* 0 to 15 */

  int   class_dim[VIF_CLASS];        /* 1 to 8 */
  int   class_subs[VIF_CLASS];       /* 0,1,2,3 (bits: 1<<n poss) */
  int   class_book[VIF_CLASS];       /* subs ^ dim entries */
  int   class_subbook[VIF_CLASS][8]; /* [VIF_CLASS][subs] */


  int   mult;                      /* 1 2 3 or 4 */ 
  int   postlist[VIF_POSIT+2];    /* first two implicit */ 

} vorbis_info_floor1;
typedef struct vorbis_info_mapping0{
  int   submaps;  /* <= 16 */
  int   chmuxlist[256];   /* up to 256 channels in a Vorbis stream */
  
  int   floorsubmap[16];   /* [mux] submap to floors */
  int   residuesubmap[16]; /* [mux] submap to residue */

  int   psy[2]; /* by blocktype; impulse/padding for short,
                   transition/normal for long */

  int   coupling_steps;
  int   coupling_mag[256];
  int   coupling_ang[256];
} vorbis_info_mapping0;
typedef struct vorbis_info_residue0{
/* block-partitioned VQ coded straight residue */
  long  begin;
  long  end;

  /* first stage (lossless partitioning) */
  int    grouping;         /* group n vectors per partition */
  int    partitions;       /* possible codebooks for a partition */
  int    groupbook;        /* huffbook for partitioning */
  int    secondstages[64]; /* expanded out to pointers in lookup */
  int    booklist[256];    /* list of second stage books */
} vorbis_info_residue0;

typedef struct {
  unsigned char *data;
  int storage;
  int fill;
  int returned;

  int unsynced;
  int headerbytes;
  int bodybytes;
} ogg_sync_state;
typedef struct {
  unsigned char   *body_data;    /* bytes from packet bodies */
  long    body_storage;          /* storage elements allocated */
  long    body_fill;             /* elements stored; fill mark */
  long    body_returned;         /* elements of fill returned */


  int     *lacing_vals;      /* The values that will go to the segment table */
  ogg_int64_t *granule_vals; /* granulepos values for headers. Not compact
				this way, but it is simple coupled to the
				lacing fifo */
  long    lacing_storage;
  long    lacing_fill;
  long    lacing_packet;
  long    lacing_returned;

  unsigned char    header[282];      /* working space for header encode */
  int              header_fill;

  int     e_o_s;          /* set when we have buffered the last packet in the
                             logical bitstream */
  int     b_o_s;          /* set after we've written the initial page
                             of a logical bitstream */
  long    serialno;
  long    pageno;
  ogg_int64_t  packetno;      /* sequence number for decode; the framing
                             knows where there's a hole in the data,
                             but we need coupling so that the codec
                             (which is in a seperate abstraction
                             layer) also knows about the gap */
  ogg_int64_t   granulepos;

} ogg_stream_state;

typedef struct {
  int blockflag;
  int windowtype;
  int transformtype;
  int mapping;
} vorbis_info_mode;
typedef void vorbis_info_floor;
typedef void vorbis_info_residue;
typedef void vorbis_info_mapping;
typedef struct static_codebook{
  long   dim;            /* codebook dimensions (elements per vector) */
  int   entries;        /* codebook entries */
  int  *lengthlist;     /* codeword lengths in bits */

  /* mapping ***************************************************************/
  int    maptype;        /* 0=none
			    1=implicitly populated values from map column 
			    2=listed arbitrary values */

  /* The below does a linear, single monotonic sequence mapping. */
  long     q_min;       /* packed 32 bit float; quant value 0 maps to minval */
  long     q_delta;     /* packed 32 bit float; val 1 - val 0 == delta */
  int      q_quant;     /* bits: 0 < quant <= 16 */
  int      q_sequencep; /* bitflag */

  long     *quantlist;  /* map == 1: (int)(entries^(1/dim)) element column map
			   map == 2: list of dim*entries quantized entry vals
			*/
} static_codebook;

typedef struct codebook{
  long dim;           /* codebook dimensions (elements per vector) */
  int entries;       /* codebook entries */
  long used_entries;  /* populated codebook entries */

  /* the below are ordered by bitreversed codeword and only used
     entries are populated */
  int           binarypoint;
  ogg_int32_t  *valuelist;  /* list of dim*entries actual entry values */  
  ogg_uint32_t *codelist;   /* list of bitstream codewords for each entry */

  int          *dec_index;  
  char         *dec_codelengths;
  ogg_uint32_t *dec_firsttable;
  int           dec_firsttablen;
  int           dec_maxlength;

  long     q_min;       /* packed 32 bit float; quant value 0 maps to minval */
  long     q_delta;     /* packed 32 bit float; val 1 - val 0 == delta */

} codebook;

typedef struct codec_setup_info {

  /* Vorbis supports only short and long blocks, but allows the
     encoder to choose the sizes */

  long blocksizes[2];

  /* modes are the primary means of supporting on-the-fly different
     blocksizes, different channel mappings (LR or M/A),
     different residue backends, etc.  Each mode consists of a
     blocksize flag and a mapping (along with the mapping setup */

  int        modes;
  int        maps;
  int        times;
  int        floors;
  int        residues;
  int        books;

  vorbis_info_mode       *mode_param[64];
  int                     map_type[64];
  vorbis_info_mapping    *map_param[64];
  int                     time_type[64];
  int                     floor_type[64];
  vorbis_info_floor      *floor_param[64];
  int                     residue_type[64];
  vorbis_info_residue    *residue_param[64];
  static_codebook        *book_param[256];
  codebook               *fullbooks;

  int    passlimit[32];     /* iteration limit per couple/quant pass */
  int    coupling_passes;
} codec_setup_info;
/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: libvorbis codec headers

 ********************************************************************/
typedef struct vorbis_info{
  int version;
  int channels;
  long rate;

  /* The below bitrate declarations are *hints*.
     Combinations of the three values carry the following implications:
     
     all three set to the same value: 
       implies a fixed rate bitstream
     only nominal set: 
       implies a VBR stream that averages the nominal bitrate.  No hard 
       upper/lower limit
     upper and or lower set: 
       implies a VBR bitstream that obeys the bitrate limits. nominal 
       may also be set to give a nominal rate.
     none set:
       the coder does not care to speculate.
  */

  long bitrate_upper;
  long bitrate_nominal;
  long bitrate_lower;
  long bitrate_window;

  void *codec_setup;
} vorbis_info;
/* vorbis_dsp_state buffers the current vorbis audio
   analysis/synthesis state.  The DSP state belongs to a specific
   logical bitstream ****************************************************/
typedef struct vorbis_dsp_state{
  int analysisp;
  vorbis_info *vi;

  long **pcm;
  long **pcmret;
  int      pcm_storage;
  int      pcm_current;
  int      pcm_returned;

  int  preextrapolate;
  int  eofflag;

  long lW;
  long W;
  long nW;
  long centerW;

  ogg_int64_t granulepos;
  double sequence;

  void       *backend_state;
} vorbis_dsp_state;

typedef struct {

  int  endbit;
  long endbyte;
  unsigned char *buffer;
  unsigned char *ptr;
  long storage;
} oggpack_buffer;

typedef struct {
  unsigned char *packet;
  long  bytes;
  long  b_o_s;
  long  e_o_s;

  ogg_int64_t  granulepos;
  
  long  packetno;     /* sequence number for decode; the framing
				knows where there's a hole in the data,
				but we need coupling so that the codec
				(which is in a seperate abstraction
				layer) also knows about the gap */
} ogg_packet;

typedef struct vorbis_block{
  /* necessary stream state for linking to the framing abstraction */
  long  **pcm;       /* this is a pointer into local storage */ 
  oggpack_buffer opb;
  
  long  lW;
  long  W;
  long  nW;
  int   pcmend;
  int   mode;

  int         eofflag;
  ogg_int64_t granulepos;
  double sequence;
  vorbis_dsp_state *vd; /* For read-only access of configuration */

  /* local storage to avoid remallocing; it's up to the mapping to
     structure it */
  void               *localstore;
  long                localtop;
  long                localalloc;
  long                totaluse;
  struct alloc_chain *reap;

} vorbis_block;

typedef struct vorbis_comment{
  /* unlimited user comment fields.  libvorbis writes 'libvorbis'
     whatever vendor is set to in encode */
  char **user_comments;
  int   *comment_lengths;
  int    comments;
  char  *vendor;

} vorbis_comment;

typedef struct ov_struct_st {
  vorbis_info      vi; /* struct that stores all the static vorbis bitstream
	//		  settings */
   vorbis_comment   vc; /* struct that stores all the bitstream user comments */
   vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */
  int              rg_scale_int;
  float            rg_scale; /* replaygain scale */

} ov_struct_t;


void     vorbis_info_init(vorbis_info *vi);
void     vorbis_info_clear(vorbis_info *vi);
int      vorbis_info_blocksize(vorbis_info *vi,int zo);
void     vorbis_comment_init(vorbis_comment *vc);
void     vorbis_comment_add(vorbis_comment *vc, char *comment); 
void     vorbis_comment_add_tag(vorbis_comment *vc, 
				       char *tag, char *contents);
char    *vorbis_comment_query(vorbis_comment *vc, char *tag, int count);
int      vorbis_comment_query_count(vorbis_comment *vc, char *tag);
void     vorbis_comment_clear(vorbis_comment *vc);

int      vorbis_block_init(vorbis_dsp_state *v, vorbis_block *vb);
int      vorbis_block_clear(vorbis_block *vb);
void     vorbis_dsp_clear(vorbis_dsp_state *v);

/* Vorbis PRIMITIVES: synthesis layer *******************************/
int      vorbis_synthesis_headerin(vorbis_info *vi,vorbis_comment *vc,
					  ogg_packet *op);

int      vorbis_synthesis_init(vorbis_dsp_state *v,vorbis_info *vi);
int      vorbis_synthesis(vorbis_block *vb,ogg_packet *op);
int      vorbis_synthesis_blockin(vorbis_dsp_state *v,vorbis_block *vb);
int      vorbis_synthesis_pcmout(vorbis_dsp_state *v,long ***pcm);
int      vorbis_synthesis_read(vorbis_dsp_state *v,int samples);
long     vorbis_packet_blocksize(vorbis_info *vi,ogg_packet *op);
static const unsigned long mask[]=
{0x00000000,0x00000001,0x00000003,0x00000007,0x0000000f,
 0x0000001f,0x0000003f,0x0000007f,0x000000ff,0x000001ff,
 0x000003ff,0x000007ff,0x00000fff,0x00001fff,0x00003fff,
 0x00007fff,0x0000ffff,0x0001ffff,0x0003ffff,0x0007ffff,
 0x000fffff,0x001fffff,0x003fffff,0x007fffff,0x00ffffff,
 0x01ffffff,0x03ffffff,0x07ffffff,0x0fffffff,0x1fffffff,
 0x3fffffff,0x7fffffff,0xffffffff };
static __inline long oggpack_read1(oggpack_buffer *b,int bits){
  int ret;
  unsigned long m=mask[bits];

  bits+=b->endbit;

  if(b->endbyte+4>=b->storage){
    /* not the main path */
    ret=-1L;
    if((b->endbyte<<3)+bits>(b->storage<<3))goto overflow;
  }
  
  ret=b->ptr[0]>>b->endbit;
  if(bits>8){
    ret|=b->ptr[1]<<(8-b->endbit);  
    if(bits>16){
      ret|=b->ptr[2]<<(16-b->endbit);  
      if(bits>24){
	ret|=b->ptr[3]<<(24-b->endbit);  
	if(bits>32 && b->endbit){
	  ret|=b->ptr[4]<<(32-b->endbit);
	}
      }
    }
  }
  ret&=m;
  
 overflow:

  b->ptr+=(bits>>3);
  b->endbyte+=(bits>>3);
  b->endbit=bits&7;
  return(ret);
}
