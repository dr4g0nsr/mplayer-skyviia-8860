
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include "config.h"
#include "ad_internal.h"

#ifndef  EngineMode  
#include "skogg.h"

static int ilog(unsigned int v){
  int ret=0;
  if(v)--v;
  while(v){
    ret++;
    v>>=1;
  }
  return(ret);
}

int vorbisicount(unsigned int v){
  int ret=0;
  while(v){
    ret+=v&1;
    v>>=1;
  }
  return(ret);
}
int vorbisilog(unsigned int v){
  int ret=0;
  while(v){
    ret++;
    v>>=1;
  }
  return(ret);
}

int ogg_page_serialno(ogg_page *og){
  return(og->header[14] |
	 (og->header[15]<<8) |
	 (og->header[16]<<16) |
	 (og->header[17]<<24));
}
/* clear non-flat storage within */
int ogg_sync_clear(ogg_sync_state *oy){
  if(oy){
    if(oy->data)free(oy->data);
    ogg_sync_init(oy);
  }
  return(0);
}
int ogg_stream_clear(ogg_stream_state *os){
  if(os){
    if(os->body_data)free(os->body_data);
    if(os->lacing_vals)free(os->lacing_vals);
    if(os->granule_vals)free(os->granule_vals);

    memset(os,0,sizeof(*os));    
  }
  return(0);
} 
long ogg_sync_pageseek(ogg_sync_state *oy,ogg_page *og){
  unsigned char *page=oy->data+oy->returned;
  unsigned char *next;
  long bytes=oy->fill-oy->returned;
  
  if(oy->headerbytes==0){
    int headerbytes,i;
    if(bytes<27)return(0); /* not enough for a header */
    
    /* verify capture pattern */
    if(memcmp(page,"OggS",4))goto sync_fail;
    
    headerbytes=page[26]+27;
    if(bytes<headerbytes)return(0); /* not enough for header + seg table */
    
    /* count up body length in the segment table */
    
    for(i=0;i<page[26];i++)
      oy->bodybytes+=page[27+i];
    oy->headerbytes=headerbytes;
  }
  
  if(oy->bodybytes+oy->headerbytes>bytes)return(0);
  
  /* The whole test page is buffered.  Verify the checksum */
  {
    /* Grab the checksum bytes, set the header field to zero */
    char chksum[4];
    ogg_page log;
    
    memcpy(chksum,page+22,4);
    memset(page+22,0,4);
    
    /* set up a temp page struct and recompute the checksum */
    log.header=page;
    log.header_len=oy->headerbytes;
    log.body=page+oy->headerbytes;
    log.body_len=oy->bodybytes;
    ogg_page_checksum_set(&log);
    
    /* Compare */
    if(memcmp(chksum,page+22,4)){
      /* D'oh.  Mismatch! Corrupt page (or miscapture and not a page
	 at all) */
      /* replace the computed checksum with the one actually read in */
      memcpy(page+22,chksum,4);
      
      /* Bad checksum. Lose sync */
      goto sync_fail;
    }
  }
  
  /* yes, have a whole page all ready to go */
  {
    unsigned char *page=oy->data+oy->returned;
    long bytes;

    if(og){
      og->header=page;
      og->header_len=oy->headerbytes;
      og->body=page+oy->headerbytes;
      og->body_len=oy->bodybytes;
    }

    oy->unsynced=0;
    oy->returned+=(bytes=oy->headerbytes+oy->bodybytes);
    oy->headerbytes=0;
    oy->bodybytes=0;
    return(bytes);
  }
  
 sync_fail:
  
  oy->headerbytes=0;
  oy->bodybytes=0;
  
  /* search for possible capture */
  next=memchr(page+1,'O',bytes-1);
  if(!next)
    next=oy->data+oy->fill;

  oy->returned=next-oy->data;
  return(-(next-page));
}
int ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og){

  /* all we need to do is verify a page at the head of the stream
     buffer.  If it doesn't verify, we look for the next potential
     frame */

  for(;;){
    long ret=ogg_sync_pageseek(oy,og);
    if(ret>0){
      /* have a page */
      return(1);
    }
    if(ret==0){
      /* need more data */
      return(0);
    }
    
    /* head did not start a synced page... skipped some bytes */
    if(!oy->unsynced){
      oy->unsynced=1;
      return(-1);
    }

    /* loop. keep looking */

  }
}

long _book_maptype1_quantvals(const static_codebook *b){
  /* get us a starting hint, we'll polish it below */
  int bits=vorbisilog(b->entries);
  int vals=b->entries>>((bits-1)*(b->dim-1)/b->dim);

  while(1){
    long acc=1;
    long acc1=1;
    int i;
    for(i=0;i<b->dim;i++){
      acc*=vals;
      acc1*=vals+1;
    }
    if(acc<=b->entries && acc1>b->entries){
      return(vals);
    }else{
      if(acc>b->entries){
	vals--;
      }else{
	vals++;
      }
    }
  }
}




static __inline void oggpack_readinit1(oggpack_buffer *b,unsigned char *buf,int bytes){
  memset(b,0,sizeof(*b));
  b->buffer=b->ptr=buf;
  b->storage=bytes;
}
static void _v_readstring(oggpack_buffer *o,char *buf,int bytes){
  while(bytes--){
    *buf++=(char)oggpack_read1(o,8);         
   //  *buf++=(char)oggpack_read(o,8);

  }
}
static int _vorbis_unpack_info(vorbis_info *vi,oggpack_buffer *opb){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  if(!ci)return(OV_EFAULT);

  vi->version=oggpack_read1(opb,32);
    
  //vi->version=oggpack_read(opb,32);

  if(vi->version!=0)return(OV_EVERSION);

  vi->channels=oggpack_read1(opb,8);
  vi->rate=oggpack_read1(opb,32);

  vi->bitrate_upper=oggpack_read1(opb,32);
  vi->bitrate_nominal=oggpack_read1(opb,32);
  vi->bitrate_lower=oggpack_read1(opb,32);

  ci->blocksizes[0]=1<<oggpack_read1(opb,4);
  ci->blocksizes[1]=1<<oggpack_read1(opb,4);
  


    
 /* vi->channels=oggpack_read(opb,8);
  vi->rate=oggpack_read(opb,32);

  vi->bitrate_upper=oggpack_read(opb,32);
  vi->bitrate_nominal=oggpack_read(opb,32);
  vi->bitrate_lower=oggpack_read(opb,32);

  ci->blocksizes[0]=1<<oggpack_read(opb,4);
  ci->blocksizes[1]=1<<oggpack_read(opb,4);
  */

  if(vi->rate<1)goto err_out;
  if(vi->channels<1)goto err_out;
  if(ci->blocksizes[0]<64)goto err_out; 
  if(ci->blocksizes[1]<ci->blocksizes[0])goto err_out;
  if(ci->blocksizes[1]>8192)goto err_out;
  
  if(oggpack_read1(opb,1)!=1)goto err_out; /* EOP check */
  //if(oggpack_read(opb,1)!=1)goto err_out; /* EOP check */

  return(0);
 err_out:
  vorbis_info_clear(vi);
  return(OV_EBADHEADER);
}

static int _vorbis_unpack_comment(vorbis_comment *vc,oggpack_buffer *opb){
  int i;
  int vendorlen=oggpack_read1(opb,32);
    
  //int vendorlen=oggpack_read(opb,32);

  if(vendorlen<0)goto err_out;
  vc->vendor=(char *)calloc(vendorlen+1,1);
  _v_readstring(opb,vc->vendor,vendorlen);
  vc->comments=oggpack_read1(opb,32);    
  //vc->comments=oggpack_read(opb,32);

  if(vc->comments<0)goto err_out;
  vc->user_comments=(char **)calloc(vc->comments+1,sizeof(*vc->user_comments));
  vc->comment_lengths=(int *)calloc(vc->comments+1, sizeof(*vc->comment_lengths));
	    
  for(i=0;i<vc->comments;i++){
    int len=oggpack_read1(opb,32);         
     //int len=oggpack_read(opb,32);

    if(len<0)goto err_out;
	vc->comment_lengths[i]=len;
    vc->user_comments[i]=(char *)calloc(len+1,1);
    _v_readstring(opb,vc->user_comments[i],len);
  }	  
  if(oggpack_read1(opb,1)!=1)goto err_out; /* EOP check */
  //if(oggpack_read(opb,1)!=1)goto err_out; /* EOP check */

  return(0);
 err_out:
  vorbis_comment_clear(vc);
  return(OV_EBADHEADER);
}
int vorbis_staticbook_unpack(oggpack_buffer *opb,static_codebook *s){
  int i,j,tmp;
  memset(s,0,sizeof(*s));

  /* make sure alignment is correct */
  if(oggpack_read1(opb,24)!=0x564342)goto _eofout;
  // if(oggpack_read(opb,24)!=0x564342)goto _eofout;

  /* first the basic parameters */
  s->dim=oggpack_read1(opb,16);
   //s->dim=oggpack_read(opb,16);

  s->entries=oggpack_read1(opb,24);
    //s->entries=oggpack_read(opb,24);

  if(s->entries==-1)goto _eofout;

  /* codeword ordering.... length ordered or unordered? */
  switch((int)oggpack_read1(opb,1)){
    //switch((int)oggpack_read(opb,1)){

  case 0:
    /* unordered */
    s->lengthlist=(int *)malloc(sizeof(*s->lengthlist)*s->entries);

    /* allocated but unused entries? */
    if(oggpack_read1(opb,1)){
    //if(oggpack_read(opb,1)){
      /* yes, unused entries */

      for(i=0;i<s->entries;i++){
	if(oggpack_read1(opb,1)){
   //if(oggpack_read(opb,1)){
	  long num=oggpack_read1(opb,5);
      //long num=oggpack_read(opb,5);
	  if(num==-1)goto _eofout;
	  s->lengthlist[i]=num+1;
	}else
	  s->lengthlist[i]=0;
      }
    }else{
      /* all entries used; no tagging */
      for(i=0;i<s->entries;i++){
	long num=oggpack_read1(opb,5);
   //  long num=oggpack_read(opb,5);
	if(num==-1)goto _eofout;
	s->lengthlist[i]=num+1;
      }
    }
    
    break;
  case 1:
    /* ordered */
    {
      long length=oggpack_read1(opb,5)+1;
       //long length=oggpack_read(opb,5)+1;
      s->lengthlist=(int *)malloc(sizeof(*s->lengthlist)*s->entries);

      for(i=0;i<s->entries;){
	long num=oggpack_read1(opb,vorbisilog(s->entries-i));
     //long num=oggpack_read(opb,vorbisilog(s->entries-i));
	if(num==-1)goto _eofout;
	for(j=0;j<num && i<s->entries;j++,i++)
	  s->lengthlist[i]=length;
	length++;
      }
    }
    break;
  default:
    /* EOF */
    return(-1);
  }
  
  /* Do we have a mapping to unpack? */
  switch((s->maptype=oggpack_read1(opb,4))){
    //switch((s->maptype=oggpack_read(opb,4))){
  case 0:
    /* no mapping */
    break;
  case 1: case 2:
    /* implicitly populated value mapping */
    /* explicitly populated value mapping */

    s->q_min=oggpack_read1(opb,32);
    s->q_delta=oggpack_read1(opb,32);
    tmp = oggpack_read1(opb,5);
    s->q_quant = (tmp & 0xf) + 1;
    s->q_sequencep = ((tmp & 0x10)>>4);
    //s->q_quant=oggpack_read(opb,4)+1;
    //s->q_sequencep=oggpack_read(opb,1);
   
    {
      int quantvals=0;
      switch(s->maptype){
      case 1:
	quantvals=_book_maptype1_quantvals(s);
	break;
      case 2:
	quantvals=s->entries*s->dim;
	break;
      }
      
      /* quantized values */
      s->quantlist=(long *)malloc(sizeof(*s->quantlist)*quantvals);
      for(i=0;i<quantvals;i++)
	s->quantlist[i]=oggpack_read1(opb,s->q_quant);
   //   s->quantlist[i]=oggpack_read(opb,s->q_quant);
      if(quantvals&&s->quantlist[quantvals-1]==-1)goto _eofout;
    }
    break;
  default:
    goto _errout;
  }

  /* all set */
  return(0);
  
 _errout:
 _eofout:
  vorbis_staticbook_clear(s);
  return(-1); 
}
void floor0_free_info(vorbis_info_floor *i){
  vorbis_info_floor0 *info=(vorbis_info_floor0 *)i;
  if(info){
    memset(info,0,sizeof(*info));
    free(info);
  }
}
void floor1_free_info(vorbis_info_floor *i){
  vorbis_info_floor1 *info=(vorbis_info_floor1 *)i;
  if(info){
    memset(info,0,sizeof(*info));
    free(info);
  }
}
void mapping0_free_info(vorbis_info_mapping *i){
  vorbis_info_mapping0 *info=(vorbis_info_mapping0 *)i;
  if(info){
    memset(info,0,sizeof(*info));
    free(info);
  }
}
vorbis_info_floor *floor0_unpack (vorbis_info *vi,oggpack_buffer *opb){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  int j,tmp;

  vorbis_info_floor0 *info=(vorbis_info_floor0 *)malloc(sizeof(*info));
 /* info->order=oggpack_read(opb,8);
  info->rate=oggpack_read(opb,16);
  info->barkmap=oggpack_read(opb,16);
  info->ampbits=oggpack_read(opb,6);
  info->ampdB=oggpack_read(opb,8);
  info->numbooks=oggpack_read(opb,4)+1;
  */

  tmp = oggpack_read1(opb,24);
  info->order=tmp & 0xff;
  info->rate=(tmp & 0xffff00)>>8;
  tmp = oggpack_read1(opb,22);
  info->barkmap=tmp & 0xffff;
  info->ampbits=(tmp&0x3f0000)>>16;
  tmp = oggpack_read1(opb,12);
  info->ampdB=tmp & 0xff;
  info->numbooks=((tmp&0xf00)>>8) + 1;

  if(info->order<1)goto err_out;
  if(info->rate<1)goto err_out;
  if(info->barkmap<1)goto err_out;
  if(info->numbooks<1)goto err_out;
    
  for(j=0;j<info->numbooks;j++){
    info->books[j]=oggpack_read1(opb,8);
    // info->books[j]=oggpack_read(opb,8);
    if(info->books[j]<0 || info->books[j]>=ci->books)goto err_out;
  }
  return(info);

 err_out:
  floor0_free_info(info);
  return(0);
}
static const ogg_uint32_t crc_lookup[256]={
  0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,
  0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
  0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,
  0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
  0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,
  0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
  0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,
  0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
  0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,
  0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
  0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,
  0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
  0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,
  0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
  0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,
  0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
  0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,
  0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
  0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,
  0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
  0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,
  0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
  0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,
  0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
  0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,
  0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
  0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,
  0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
  0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,
  0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
  0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,
  0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
  0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,
  0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
  0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,
  0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
  0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,
  0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
  0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,
  0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
  0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,
  0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
  0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,
  0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
  0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,
  0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
  0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,
  0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
  0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,
  0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
  0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,
  0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
  0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,
  0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
  0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
  0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
  0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
  0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
  0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
  0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
  0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
  0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
  0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,
  0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4};

void ogg_page_checksum_set(ogg_page *og){
  if(og){
    ogg_uint32_t crc_reg=0;
    int i;

    /* safety; needed for API behavior, but not framing code */
    og->header[22]=0;
    og->header[23]=0;
    og->header[24]=0;
    og->header[25]=0;
    
    for(i=0;i<og->header_len;i++)
      crc_reg=(crc_reg<<8)^crc_lookup[((crc_reg >> 24)&0xff)^og->header[i]];
    for(i=0;i<og->body_len;i++)
      crc_reg=(crc_reg<<8)^crc_lookup[((crc_reg >> 24)&0xff)^og->body[i]];
    
    og->header[22]=(unsigned char)(crc_reg&0xff);
    og->header[23]=(unsigned char)((crc_reg>>8)&0xff);
    og->header[24]=(unsigned char)((crc_reg>>16)&0xff);
    og->header[25]=(unsigned char)((crc_reg>>24)&0xff);
  }
}
int ogg_page_version(ogg_page *og){
  return((int)(og->header[4]));
}

int ogg_page_continued(ogg_page *og){
  return((int)(og->header[5]&0x01));
}

int ogg_page_bos(ogg_page *og){
  return((int)(og->header[5]&0x02));
}

int ogg_page_eos(ogg_page *og){
  return((int)(og->header[5]&0x04));
}
long ogg_page_pageno(ogg_page *og){
  return(og->header[18] |
	 (og->header[19]<<8) |
	 (og->header[20]<<16) |
	 (og->header[21]<<24));
}

ogg_int64_t ogg_page_granulepos(ogg_page *og){
  unsigned char *page=og->header;
  ogg_int64_t granulepos=page[13]&(0xff);
  granulepos= (granulepos<<8)|(page[12]&0xff);
  granulepos= (granulepos<<8)|(page[11]&0xff);
  granulepos= (granulepos<<8)|(page[10]&0xff);
  granulepos= (granulepos<<8)|(page[9]&0xff);
  granulepos= (granulepos<<8)|(page[8]&0xff);
  granulepos= (granulepos<<8)|(page[7]&0xff);
  granulepos= (granulepos<<8)|(page[6]&0xff);
  return(granulepos);
}
static void _os_body_expand(ogg_stream_state *os,int needed){
  if(os->body_storage<=os->body_fill+needed){
    os->body_storage+=(needed+1024);
    os->body_data=realloc(os->body_data,os->body_storage*sizeof(*os->body_data));
  }
}

static void _os_lacing_expand(ogg_stream_state *os,int needed){
  if(os->lacing_storage<=os->lacing_fill+needed){
    os->lacing_storage+=(needed+32);
    os->lacing_vals=realloc(os->lacing_vals,os->lacing_storage*sizeof(*os->lacing_vals));
    os->granule_vals=realloc(os->granule_vals,os->lacing_storage*sizeof(*os->granule_vals));
  }
}
int ogg_stream_pagein(ogg_stream_state *os, ogg_page *og){
  unsigned char *header=og->header;
  unsigned char *body=og->body;
  long           bodysize=og->body_len;
  int            segptr=0;

  int version=ogg_page_version(og);
  int continued=ogg_page_continued(og);
  int bos=ogg_page_bos(og);
  int eos=ogg_page_eos(og);
  ogg_int64_t granulepos=ogg_page_granulepos(og);
  int serialno=ogg_page_serialno(og);
  long pageno=ogg_page_pageno(og);
  int segments=header[26];
  
  /* clean up 'returned data' */
  {
    long lr=os->lacing_returned;
    long br=os->body_returned;

    /* body data */
    if(br){
      os->body_fill-=br;
      if(os->body_fill)
	memmove(os->body_data,os->body_data+br,os->body_fill);
      os->body_returned=0;
    }

    if(lr){
      /* segment table */
      if(os->lacing_fill-lr){
	memmove(os->lacing_vals,os->lacing_vals+lr,
		(os->lacing_fill-lr)*sizeof(*os->lacing_vals));
	memmove(os->granule_vals,os->granule_vals+lr,
		(os->lacing_fill-lr)*sizeof(*os->granule_vals));
      }
      os->lacing_fill-=lr;
      os->lacing_packet-=lr;
      os->lacing_returned=0;
    }
  }

  /* check the serial number */
  if(serialno!=os->serialno)return(-1);
  if(version>0)return(-1);

  _os_lacing_expand(os,segments+1);

  /* are we in sequence? */
  if(pageno!=os->pageno){
    int i;

    /* unroll previous partial packet (if any) */
    for(i=os->lacing_packet;i<os->lacing_fill;i++)
      os->body_fill-=os->lacing_vals[i]&0xff;
    os->lacing_fill=os->lacing_packet;

    /* make a note of dropped data in segment table */
    if(os->pageno!=-1){
      os->lacing_vals[os->lacing_fill++]=0x400;
      os->lacing_packet++;
    }
  }

  /* are we a 'continued packet' page?  If so, we may need to skip
     some segments */
  if(continued){
    if(os->lacing_fill<1 || 
       os->lacing_vals[os->lacing_fill-1]==0x400){
      bos=0;
      for(;segptr<segments;segptr++){
	int val=header[27+segptr];
	body+=val;
	bodysize-=val;
	if(val<255){
	  segptr++;
	  break;
	}
      }
    }
  }
  
  if(bodysize){
    _os_body_expand(os,bodysize);
    memcpy(os->body_data+os->body_fill,body,bodysize);
    os->body_fill+=bodysize;
  }

  {
    int saved=-1;
    while(segptr<segments){
      int val=header[27+segptr];
      os->lacing_vals[os->lacing_fill]=val;
      os->granule_vals[os->lacing_fill]=-1;
      
      if(bos){
	os->lacing_vals[os->lacing_fill]|=0x100;
	bos=0;
      }
      
      if(val<255)saved=os->lacing_fill;
      
      os->lacing_fill++;
      segptr++;
      
      if(val<255)os->lacing_packet=os->lacing_fill;
    }
  
    /* set the granulepos on the last granuleval of the last full packet */
    if(saved!=-1){
      os->granule_vals[saved]=granulepos;
    }

  }

  if(eos){
    os->e_o_s=1;
    if(os->lacing_fill>0)
      os->lacing_vals[os->lacing_fill-1]|=0x200;
  }

  os->pageno=pageno+1;

  return(0);
}
static int _packetout(ogg_stream_state *os,ogg_packet *op,int adv){

  /* The last part of decode. We have the stream broken into packet
     segments.  Now we need to group them into packets (or return the
     out of sync markers) */

  int ptr=os->lacing_returned;

  if(os->lacing_packet<=ptr)return(0);

  if(os->lacing_vals[ptr]&0x400){
    /* we need to tell the codec there's a gap; it might need to
       handle previous packet dependencies. */
    os->lacing_returned++;
    os->packetno++;
    return(-1);
  }

  if(!op && !adv)return(1); /* just using peek as an inexpensive way
                               to ask if there's a whole packet
                               waiting */

  /* Gather the whole packet. We'll have no holes or a partial packet */
  {
    int size=os->lacing_vals[ptr]&0xff;
    int bytes=size;
    int eos=os->lacing_vals[ptr]&0x200; /* last packet of the stream? */
    int bos=os->lacing_vals[ptr]&0x100; /* first packet of the stream? */

    while(size==255){
      int val=os->lacing_vals[++ptr];
      size=val&0xff;
      if(val&0x200)eos=0x200;
      bytes+=size;
    }

    if(op){
      op->e_o_s=eos;
      op->b_o_s=bos;
      op->packet=os->body_data+os->body_returned;
      op->packetno=os->packetno;
      op->granulepos=os->granule_vals[ptr];
      op->bytes=bytes;
    }

    if(adv){
      os->body_returned+=bytes;
      os->lacing_returned=ptr+1;
      os->packetno++;
    }
  }
  return(1);
}
char *ogg_sync_buffer(ogg_sync_state *oy, long size){

  /* first, clear out any space that has been previously returned */
  if(oy->returned){
    oy->fill-=oy->returned;
    if(oy->fill>0)
      memmove(oy->data,oy->data+oy->returned,oy->fill);
    oy->returned=0;
  }

  if(size>oy->storage-oy->fill){
    /* We need to extend the internal buffer */
    long newsize=size+oy->fill+4096; /* an extra page to be nice */

    if(oy->data)
      oy->data=realloc(oy->data,newsize);
    else
      oy->data=malloc(newsize);
    oy->storage=newsize;
  }

  /* expose a segment at least as large as requested at the fill mark */
  return((char *)oy->data+oy->fill);
}

int ogg_sync_wrote(ogg_sync_state *oy, long bytes){
  if(oy->fill+bytes>oy->storage)return(-1);
  oy->fill+=bytes;
  return(0);
}
int ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op){
  return _packetout(os,op,1);
}
int ogg_stream_init(ogg_stream_state *os,int serialno){
  if(os){
    memset(os,0,sizeof(*os));
    os->body_storage=16*1024;
    os->body_data=malloc(os->body_storage*sizeof(*os->body_data));

    os->lacing_storage=1024;
    os->lacing_vals=malloc(os->lacing_storage*sizeof(*os->lacing_vals));
    os->granule_vals=malloc(os->lacing_storage*sizeof(*os->granule_vals));

    os->serialno=serialno;

    return(0);
  }
  return(-1);
} 
vorbis_info_floor *floor1_unpack (vorbis_info *vi,oggpack_buffer *opb){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  int j,k,count=0,maxclass=-1,rangebits;

  vorbis_info_floor1 *info=(vorbis_info_floor1 *)calloc(1,sizeof(*info));
  /* read partitions */
  info->partitions=oggpack_read1(opb,5); /* only 0 to 31 legal */
   // info->partitions=oggpack_read(opb,5); /* only 0 to 31 legal */

  for(j=0;j<info->partitions;j++){
    info->partitionclass[j]=oggpack_read1(opb,4); /* only 0 to 15 legal */
     //info->partitionclass[j]=oggpack_read(opb,4); /* only 0 to 15 legal */
    if(maxclass<info->partitionclass[j])maxclass=info->partitionclass[j];
  }

  /* read partition classes */
  for(j=0;j<maxclass+1;j++){
    info->class_dim[j]=oggpack_read1(opb,3)+1; /* 1 to 8 */
    info->class_subs[j]=oggpack_read1(opb,2); /* 0,1,2,3 bits */
         
   // info->class_dim[j]=oggpack_read(opb,3)+1; /* 1 to 8 */
   // info->class_subs[j]=oggpack_read(opb,2); /* 0,1,2,3 bits */

    if(info->class_subs[j]<0)
      goto err_out;
    if(info->class_subs[j])info->class_book[j]=oggpack_read1(opb,8);
        
   // if(info->class_subs[j])info->class_book[j]=oggpack_read(opb,8);

    if(info->class_book[j]<0 || info->class_book[j]>=ci->books)
      goto err_out;
    for(k=0;k<(1<<info->class_subs[j]);k++){
      info->class_subbook[j][k]=oggpack_read1(opb,8)-1;             
     //  info->class_subbook[j][k]=oggpack_read(opb,8)-1;

      if(info->class_subbook[j][k]<-1 || info->class_subbook[j][k]>=ci->books)
	goto err_out;
    }
  }

  /* read the post list */
  info->mult=oggpack_read1(opb,2)+1;     /* only 1,2,3,4 legal now */ 
  rangebits=oggpack_read1(opb,4);
  //info->mult=oggpack_read(opb,2)+1;     /* only 1,2,3,4 legal now */ 
  //rangebits=oggpack_read(opb,4);

  for(j=0,k=0;j<info->partitions;j++){
    count+=info->class_dim[info->partitionclass[j]]; 
    for(;k<count;k++){
      int t=info->postlist[k+2]=oggpack_read1(opb,rangebits);             
    //   int t=info->postlist[k+2]=oggpack_read(opb,rangebits);

      if(t<0 || t>=(1<<rangebits))
	goto err_out;
    }
  }
  info->postlist[0]=0;
  info->postlist[1]=1<<rangebits;

  return(info);
  
 err_out:
  floor1_free_info(info);
  return(0);
}
vorbis_info_mapping *mapping0_unpack(vorbis_info *vi,oggpack_buffer *opb){
  int i,tmp,rtmp;
  vorbis_info_mapping0 *info=(vorbis_info_mapping0 *)calloc(1,sizeof(*info));
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  memset(info,0,sizeof(*info));
  if(oggpack_read1(opb,1))
    info->submaps=oggpack_read1(opb,4)+1;

  //if(oggpack_read(opb,1))
   // info->submaps=oggpack_read(opb,4)+1;
  else
    info->submaps=1;
  if(oggpack_read1(opb,1)){

  //if(oggpack_read(opb,1)){
    info->coupling_steps=oggpack_read1(opb,8)+1;
    //info->coupling_steps=oggpack_read(opb,8)+1;
    //modify by archin
    tmp = ilog(vi->channels)<<1;
    
    for(i=0;i<info->coupling_steps;i++){
      //int testM = info->coupling_mag[i]=oggpack_read(opb,ilog(vi->channels));
      //int testA = info->coupling_ang[i]=oggpack_read(opb,ilog(vi->channels));       

      int testM;//=info->coupling_mag[i]=Ogg_Bit_Read(opb,ilog(vi->channels));
      int testA;//=info->coupling_ang[i]=Ogg_Bit_Read(opb,ilog(vi->channels));       
      rtmp = oggpack_read1(opb,tmp);
      testM= info->coupling_mag[i] = (rtmp & ((1<<(tmp>>1)) - 1));
      testA = info->coupling_ang[i]= ((rtmp & ((1<<tmp) - 1))>>(tmp>>1));
      if(testM<0 || 
	      testA<0 || 
	      testM==testA || 
	      testM>=vi->channels ||
	      testA>=vi->channels) goto err_out;
    }

  }
  if(oggpack_read1(opb,2)>0)goto err_out; /* 2,3:reserved */

  //if(oggpack_read(opb,2)>0)goto err_out; /* 2,3:reserved */
    
  if(info->submaps>1){
    for(i=0;i<vi->channels;i++){
      info->chmuxlist[i]=oggpack_read1(opb,4);
     // info->chmuxlist[i]=oggpack_read(opb,4);
      if(info->chmuxlist[i]>=info->submaps)goto err_out;
    }
  }
  for(i=0;i<info->submaps;i++){
    int temp=oggpack_read1(opb,8);
    if(temp>=ci->times)goto err_out;
    info->floorsubmap[i]=oggpack_read1(opb,8);
    if(info->floorsubmap[i]>=ci->floors)goto err_out;
     info->residuesubmap[i]=oggpack_read1(opb,8);
    if(info->residuesubmap[i]>=ci->residues)goto err_out;
  /*  int temp=oggpack_read(opb,24);
    if((temp&0xff)>=ci->times)goto err_out;
     info->floorsubmap[i]=(temp&0xff00)>>8;
    if(info->floorsubmap[i]>=ci->floors)goto err_out;
    info->residuesubmap[i]=(temp&0xff0000)>>16;
    if(info->residuesubmap[i]>=ci->residues)goto err_out;*/
      
  }

  return info;

 err_out:
  mapping0_free_info(info);
  return(0);
}
void res0_free_info(vorbis_info_residue *i){
  vorbis_info_residue0 *info=(vorbis_info_residue0 *)i;
  if(info){
    memset(info,0,sizeof(*info));
    free(info);
  }
}
vorbis_info_residue *res0_unpack(vorbis_info *vi,oggpack_buffer *opb){
  int j,acc=0;
  int acc4,acc1,tmp,cascade; //by archin
  vorbis_info_residue0 *info=(vorbis_info_residue0 *)calloc(1,sizeof(*info));
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  
  info->begin=oggpack_read1(opb,24);
  info->end=oggpack_read1(opb,24);
  info->grouping=oggpack_read1(opb,24)+1;
  info->partitions=oggpack_read1(opb,6)+1;
  info->groupbook=oggpack_read1(opb,8);
  
  /*info->begin=oggpack_read(opb,24);
  info->end=oggpack_read(opb,24);
  info->grouping=oggpack_read(opb,24)+1;
  info->partitions=oggpack_read(opb,6)+1;
  info->groupbook=oggpack_read(opb,8);
*/
  for(j=0;j<info->partitions;j++){
    
     /*int cascade=oggpack_read(opb,3);
    if(oggpack_read(opb,1))
      cascade|=(oggpack_read(opb,5)<<3);
   */
    tmp =  oggpack_read1(opb,4);
    cascade = tmp & 7;
    if(tmp & 8)
      cascade|=(oggpack_read1(opb,5)<<3);
    info->secondstages[j]=cascade;

    acc+=vorbisicount(cascade);
  }
 
  acc4 = acc>>2;
  acc1 = acc - (acc4<<2);
  /*for(j=0;j<acc;j++)         
    info->booklist[j]=oggpack_read(opb,8);
  */
  for(j = 0;j< acc4;j++)
  {
     tmp = oggpack_read1(opb,32);
     info->booklist[4*j]=tmp&0xff;
     info->booklist[4*j + 1]=(tmp&0xff00)>>8;
     info->booklist[4*j + 2]=(tmp&0xff0000)>>16;
     info->booklist[4*j + 3]=(tmp&0xff000000)>>24;

  }

  for(j=0;j<acc1;j++)         
    info->booklist[4*acc4 + j]=oggpack_read1(opb,8);

  if(info->groupbook>=ci->books)goto errout;
  for(j=0;j<acc;j++)
    if(info->booklist[j]>=ci->books)goto errout;

  return(info);
 errout:
  res0_free_info(info);
  return(0);
}
/* all of the real encoding details are here.  The modes, books,
   everything */
static int _vorbis_unpack_books(vorbis_info *vi,oggpack_buffer *opb){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  int i,tmp;
  if(!ci)return(OV_EFAULT);

  /* codebooks */
  ci->books=oggpack_read1(opb,8)+1;
    //ci->books=oggpack_read(opb,8)+1;

  /*ci->book_param=calloc(ci->books,sizeof(*ci->book_param));*/
  for(i=0;i<ci->books;i++){
    ci->book_param[i]=(static_codebook *)calloc(1,sizeof(*ci->book_param[i]));
    if(vorbis_staticbook_unpack(opb,ci->book_param[i]))goto err_out;
  }

  /* time backend settings */
  ci->times=oggpack_read1(opb,6)+1;
    //ci->times=oggpack_read(opb,6)+1;

  /*ci->time_type=_ogg_malloc(ci->times*sizeof(*ci->time_type));*/
  /*ci->time_param=calloc(ci->times,sizeof(void *));*/
  for(i=0;i<ci->times;i++){
    ci->time_type[i]=oggpack_read1(opb,16);         
     //ci->time_type[i]=oggpack_read(opb,16);

    if(ci->time_type[i]<0 || ci->time_type[i]>=VI_TIMEB)goto err_out;
    /* ci->time_param[i]=_time_P[ci->time_type[i]]->unpack(vi,opb);
       Vorbis I has no time backend */
    /*if(!ci->time_param[i])goto err_out;*/
  }

  /* floor backend settings */
  ci->floors=oggpack_read1(opb,6)+1;
    //ci->floors=oggpack_read(opb,6)+1;

  /*ci->floor_type=_ogg_malloc(ci->floors*sizeof(*ci->floor_type));*/
  /*ci->floor_param=calloc(ci->floors,sizeof(void *));*/
  for(i=0;i<ci->floors;i++){
    ci->floor_type[i]=oggpack_read1(opb,16);        
     //ci->floor_type[i]=oggpack_read(opb,16);     
    if(ci->floor_type[i]<0 || ci->floor_type[i]>=VI_FLOORB)goto err_out;
	if(ci->floor_type[i]==0)
		ci->floor_param[i]=floor0_unpack(vi,opb);
	else
		ci->floor_param[i]=floor1_unpack(vi,opb);
	if(!ci->floor_param[i])goto err_out;
  }

  /* residue backend settings */
  ci->residues=oggpack_read1(opb,6)+1;
  //ci->residues=oggpack_read(opb,6)+1;

  /*ci->residue_type=_ogg_malloc(ci->residues*sizeof(*ci->residue_type));*/
  /*ci->residue_param=calloc(ci->residues,sizeof(void *));*/
  for(i=0;i<ci->residues;i++){
    ci->residue_type[i]=oggpack_read1(opb,16);        
    //ci->residue_type[i]=oggpack_read(opb,16);

    if(ci->residue_type[i]<0 || ci->residue_type[i]>=VI_RESB)goto err_out;
	
	ci->residue_param[i]=res0_unpack(vi,opb);
	
    if(!ci->residue_param[i])goto err_out;
  }

  /* map backend settings */
  ci->maps=oggpack_read1(opb,6)+1;
  //ci->maps=oggpack_read(opb,6)+1;

  /*ci->map_type=_ogg_malloc(ci->maps*sizeof(*ci->map_type));*/
  /*ci->map_param=calloc(ci->maps,sizeof(void *));*/
  for(i=0;i<ci->maps;i++){         
     ci->map_type[i]=oggpack_read1(opb,16);
    //ci->map_type[i]=oggpack_read(opb,16);
    if(ci->map_type[i]<0 || ci->map_type[i]>=VI_MAPB)goto err_out;
    ci->map_param[i]=mapping0_unpack(vi,opb);
    if(!ci->map_param[i])goto err_out;
  }
  
  /* mode settings */
  ci->modes=oggpack_read1(opb,6)+1;
    //ci->modes=oggpack_read(opb,6)+1;

  /*vi->mode_param=calloc(vi->modes,sizeof(void *));*/
  for(i=0;i<ci->modes;i++){
    ci->mode_param[i]=(vorbis_info_mode *)calloc(1,sizeof(*ci->mode_param[i]));
  /*  ci->mode_param[i]->blockflag=oggpack_read(opb,1);
    ci->mode_param[i]->windowtype=oggpack_read(opb,16);
    ci->mode_param[i]->transformtype=oggpack_read(opb,16);
    ci->mode_param[i]->mapping=oggpack_read(opb,8); 
  */
    tmp = oggpack_read1(opb,17);
    ci->mode_param[i]->blockflag=tmp&0x1;
    ci->mode_param[i]->windowtype=(tmp&0x1fffe)>>1;
  
    tmp = oggpack_read1(opb,24);
    ci->mode_param[i]->transformtype=tmp&0xffff;
    ci->mode_param[i]->mapping=(tmp&0xff0000)>>16;
   
    if(ci->mode_param[i]->windowtype>=VI_WINDOWB)goto err_out;
    if(ci->mode_param[i]->transformtype>=VI_WINDOWB)goto err_out;
    if(ci->mode_param[i]->mapping>=ci->maps)goto err_out;
  }
  
  if(oggpack_read1(opb,1)!=1)goto err_out; /* top level EOP check */
  //if(oggpack_read(opb,1)!=1)goto err_out; /* top level EOP check */
  return(0);
 err_out:
  vorbis_info_clear(vi);
  return(OV_EBADHEADER);
}
int vorbis_synthesis_headerin(vorbis_info *vi,vorbis_comment *vc,ogg_packet *op){
  oggpack_buffer opb;
  
  if(op){

    oggpack_readinit1(&opb,op->packet,op->bytes);
   //  Ogg_Bit_Init(&opb,op->packet,op->bytes);
    /* Which of the three types of header is this? */
    /* Also verify header-ness, vorbis */
    {
      char buffer[6];
      int packtype=oggpack_read1(&opb,8);

     // int packtype=oggpack_read(&opb,8);
      memset(buffer,0,6);
      _v_readstring(&opb,buffer,6);
      if(memcmp(buffer,"vorbis",6)){
	/* not a vorbis header */
	return(OV_ENOTVORBIS);
      }
      switch(packtype){
      case 0x01: /* least significant *bit* is read first */
	if(!op->b_o_s){
	  /* Not the initial packet */
	  return(OV_EBADHEADER);
	}
	if(vi->rate!=0){
	  /* previously initialized info header */
	  return(OV_EBADHEADER);
	}

	return(_vorbis_unpack_info(vi,&opb));

      case 0x03: /* least significant *bit* is read first */
	if(vi->rate==0){
	  /* um... we didn't get the initial header */
	  return(OV_EBADHEADER);
	}

	return(_vorbis_unpack_comment(vc,&opb));

      case 0x05: /* least significant *bit* is read first */
	if(vi->rate==0 || vc->vendor==0){
	  /* um... we didn;t get the initial header or comments yet */
	  return(OV_EBADHEADER);
	}

	return(_vorbis_unpack_books(vi,&opb));

      default:
	/* Not a valid vorbis header type */
	return(OV_EBADHEADER);
	break;
      }
    }
  }
  return(OV_EBADHEADER);
}
void vorbis_staticbook_clear(static_codebook *b){
  if(b->quantlist)free(b->quantlist);
  if(b->lengthlist)free(b->lengthlist);
  memset(b,0,sizeof(*b));

}

void vorbis_staticbook_destroy(static_codebook *b){
  vorbis_staticbook_clear(b);
  free(b);
}
void vorbis_book_clear(codebook *b){
  /* static book is not cleared; we're likely called on the lookup and
     the static codebook belongs to the info struct */
  if(b->valuelist)free(b->valuelist);
  if(b->codelist)free(b->codelist);

  if(b->dec_index)free(b->dec_index);
  if(b->dec_codelengths)free(b->dec_codelengths);
  if(b->dec_firsttable)free(b->dec_firsttable);

  memset(b,0,sizeof(*b));
}
void vorbis_info_clear(vorbis_info *vi){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  int i;

  if(ci){

    for(i=0;i<ci->modes;i++)
      if(ci->mode_param[i])free(ci->mode_param[i]);

    for(i=0;i<ci->maps;i++) /* unpack does the range checking */
      mapping0_free_info(ci->map_param[i]);

    for(i=0;i<ci->floors;i++) /* unpack does the range checking */
	{
		if(ci->floor_type[i]==0)
			floor0_free_info(ci->floor_param[i]);
		else
			floor1_free_info(ci->floor_param[i]);
	}
    
    for(i=0;i<ci->residues;i++) /* unpack does the range checking */
      res0_free_info(ci->residue_param[i]);

    for(i=0;i<ci->books;i++){
      if(ci->book_param[i]){
	/* knows if the book was not alloced */
	vorbis_staticbook_destroy(ci->book_param[i]);
      }
      if(ci->fullbooks)
	vorbis_book_clear(ci->fullbooks+i);
    }
    if(ci->fullbooks)
	free(ci->fullbooks);
    
    free(ci);
  }

  memset(vi,0,sizeof(*vi));
}
int ogg_sync_reset(ogg_sync_state *oy){
  oy->fill=0;
  oy->returned=0;
  oy->unsynced=0;
  oy->headerbytes=0;
  oy->bodybytes=0;
  return(0);
}

int ogg_stream_reset(ogg_stream_state *os){
  os->body_fill=0;
  os->body_returned=0;

  os->lacing_fill=0;
  os->lacing_packet=0;
  os->lacing_returned=0;

  os->header_fill=0;

  os->e_o_s=0;
  os->b_o_s=0;
  os->pageno=-1;
  os->packetno=0;
  os->granulepos=0;

  return(0);
}

int ogg_sync_init(ogg_sync_state *oy){
  if(oy){
    memset(oy,0,sizeof(*oy));
  }
  return(0);
}
void vorbis_comment_init(vorbis_comment *vc){
  memset(vc,0,sizeof(*vc));
}
void vorbis_comment_clear(vorbis_comment *vc){
  if(vc){
    long i;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    for(i=0;i<vc->comments;i++)
      if(vc->user_comments[i])free(vc->user_comments[i]);
    if(vc->user_comments)free(vc->user_comments);
	if(vc->comment_lengths)free(vc->comment_lengths);
    if(vc->vendor)free(vc->vendor);
  }
  memset(vc,0,sizeof(*vc));
}
void vorbis_info_init(vorbis_info *vi){
  memset(vi,0,sizeof(*vi));
  vi->codec_setup=(codec_setup_info *)calloc(1,sizeof(codec_setup_info));
}



long vorbis_packet_blocksize(vorbis_info *vi,ogg_packet *op){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  oggpack_buffer       opb;
  int                  mode;
 
  //Ogg_Bit_Init(&opb,op->packet,op->bytes);
  oggpack_readinit1(&opb,op->packet,op->bytes);

  /* Check the packet type */
  if(oggpack_read1(&opb,1)!=0){
    /* Oops.  This is not an audio data packet */
    return(OV_ENOTAUDIO);
  }

  {
    int modebits=0;
    int v=ci->modes;
    while(v>1){
      modebits++;
      v>>=1;
    }

    /* read our mode and pre/post windowsize */
    mode=oggpack_read1(&opb,modebits);
  }
  if(mode==-1)return(OV_EBADPACKET);
  return(ci->blocksizes[ci->mode_param[mode]->blockflag]);
}
#endif
//#include "skogg.h"
#ifndef  EngineMode   

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>    
#include "skipc.h"   

#include "config.h"
#include "ad_internal.h"




//extern int strncasecmp(const char *s1, const char *s2, size_t n); 

static ad_info_t info = 
{
	"Skyviia Vorbis audio decoder",
	"saogg",
	"Archin",
	"Archin",
	"Skyviia copyright"
};
LIBAD_EXTERN(saogg)


#else

;//#include "mp3_mplayer.h"

#endif

#include "audio_server.h"
#define MP3_IN_SIZE 8192
extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;




static int preinit(sh_audio_t *sh)
{
	aserver_pid = 0;
#ifndef  EngineMode 
	union smc *sc;
	long curr_codec;
#endif
	  in_buf_len = 0;
   in_buf_size = MP3_IN_SIZE;
 // sh->audio_out_minsize=1024*4; // 1024 samples/frame
 sh->audio_out_minsize=2048*4; // 1024 samples/frame  archin for debug
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
	

	id = get_semaphore_id();
	
	if (!reset_semvalue(id, 0)){
		return 0;
 	}

	sc = (union smc *)sm_com;

	sc->magic.magic_num = PLAYER_MAGIC;
	curr_codec = OGG_ID;

	sc->magic.codec_id = curr_codec;
	if (!semaphore_v(id, 1))
		return 0;	

	if (!wait_server(sc)){
		deallocate_share_mem(sm_com);	
#ifndef AD_SHM_REWRITE		
		shmctl(sm_com_id, IPC_RMID, 0);
#endif		
		del_semvalue(id, 0);
		printf("Audio server crash \n");
		return 0;
	}	

	if (sc->magic.codec_id != curr_codec){
		printf("Unsuppoted codec\n");
		return 0;		
	}
	
  // sh->audio_out_minsize=2*4608;

#endif 

	aserver_pid = sc->pinit.pid;

  return 1;
}
static int OGG_frame_init(sh_audio_t *sh, aduio_info *ai)
{
   int ret;
   int init = 1;
   unsigned int offset, i, length;
   unsigned char* extradata;
   union smc *sc = (union smc *)sm_com;
   play_init *pi = &sc->pinit;
   serv_init *si = &sc->sinit;
   extradata = (char*)(sh->wf+1);


  offset = 1;
  for (i=0; i < 2; i++) {
    length = 0;
    while ((extradata[offset] == (unsigned char) 0xFF) && length < sh->wf->cbSize) {
      length += 255;
      offset++;
    } 
/*    if(offset >= (sh->wf->cbSize - 1)) {
       ;
      //mp_msg (MSGT_DEMUX, MSGL_WARN, "ad_vorbis: Vorbis track does not contain valid headers.\n");
    //  ERROR();
    }*/
    length += extradata[offset];
    offset++;
//    mp_msg (MSGT_DEMUX, MSGL_V, "ad_vorbis, offset: %u, length: %u\n", offset, length);
    pi->OGGinfo.Ogghsizes[i] = length;
  }
	   
 // pi->OGGinfo.Oggheaders[0] = &extradata[offset];
 // pi->OGGinfo.Oggheaders[1] = &extradata[offset + pi->OGGinfo.Ogghsizes[0]];
 // pi->OGGinfo.Oggheaders[2] = &extradata[offset + pi->OGGinfo.Ogghsizes[0] + pi->OGGinfo.Ogghsizes[1]];
  pi->OGGinfo.Ogghsizes[2] = sh->wf->cbSize - offset - pi->OGGinfo.Ogghsizes[0] - pi->OGGinfo.Ogghsizes[1];
 
  memcpy(sm_in,&offset,  2); 
		offset = offset + pi->OGGinfo.Ogghsizes[0];
		    
		   
		memcpy(sm_in+2,&offset,  2);  
		offset =offset + pi->OGGinfo.Ogghsizes[1];
		  
		memcpy(sm_in+4,&offset,  2); 
		memcpy(sm_in+6,sh->wf+1,  sh->wf->cbSize); 
  if(!ipc_init(sc, OGG_ID, in_buf_len, id, sm_data_id, MP3_IN_SIZE))			
	   return -1;		//IPC Error 
   if (si->codec_id == OGG_ID)
            ret = 0;
         else
            ret = si->codec_id;   //error code

         init = 0;
     
			*ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif
   return ret;   //end of mp3
}

static int init(sh_audio_t *sh)
{
	      
  int ret;	
  //void *headers[3];
 
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
	sm_data_id = get_shared_mem_id(KEY_SM_IN,320 );
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL)
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif	
#endif
  

  ret = OGG_frame_init(sh,ai);
   if (ret==-1)
      return ret ; 

//Barry 2010-12-24
if (ai->ch > 2)
{
	printf("[SAOGG] channels %d > 2, not support!\n", ai->ch);
	not_supported_profile_level = 2;
	return 0;
}

 sh->channels = ai->ch;
  sh->samplerate=ai->samplerate;
  sh->i_bps=ai->bitrate/8;
  sh->samplesize=2;
  //  sh->context = ai->AAC_MODE;
  return 1;
}

static void uninit(sh_audio_t *sh)
{

 union smc *sc = (union smc *)sm_com;
      ipc_uninit(sc, AMRNB_ID, id);

      //server
      #ifndef  EngineMode
	   deallocate_share_mem(sm_in);
	   deallocate_share_mem(sm_out);	
#ifndef AD_SHM_REWRITE	   
	   shmctl(sm_data_id, IPC_RMID, 0);
#endif	   
	   deallocate_share_mem(sm_com);
      #endif

}



static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
  int len = 0,buflen= 0;
    int samples = 0;
    long **pcm;    
	 int i,j;
	 int clipflag=0;


  // struct ov_struct_st *ov = sh->context;
        
   union smc *sc = (union smc *)sm_com;     //  shared memory

   samples=0;
 
	while(len < minlen) {
				  
		unsigned char* bufptr=NULL;
	  while(samples<=0){

	    double pts;
	   // memset(&op,0,sizeof(op)); //op.b_o_s = op.e_o_s = 0;
	    buflen = ds_get_packet_pts(sh->ds,&bufptr, &pts);
	    if(buflen<=0) break;
	    if (pts != MP_NOPTS_VALUE) {
		sh->pts = pts;
		sh->pts_bytes = 0;
	    }
			
		memcpy(sm_in,bufptr, buflen); 
#if 1
		int ret;
		ret = ipc_decode(sc, OGG_ID, buflen, id);
		if(!ret)
			return 0;
		else if(ret < 0)
			return -1;
#else
       if (!ipc_decode(sc, OGG_ID, buflen, id))
	      return 0 ;
#endif



	   samples = sc->sctrl.data_len;
		if(samples>0)
		{
			memcpy(buf+len,sm_out, 2*sh->channels*samples); 
	    //if(clipflag)
	     // mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"Clipping in frame %ld\n",(long)(ov->vd.sequence));
	
			len+=2*sh->channels*samples;
			sh->pts_bytes += 2*sh->channels*samples;
		}//if(samples>0)

	  }
	  if(samples<=0) break; // error/EOF
    else 
  	   samples = 0;
	}

  


return len;
}

#ifndef  EngineMode

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    switch(cmd)
    {
#if 0      
      case ADCTRL_RESYNC_STREAM:
	  return CONTROL_TRUE;
      case ADCTRL_SKIP_FRAME:
	  return CONTROL_TRUE;
#endif
    }
  return CONTROL_UNKNOWN;
}

#else

static int control(sh_audio_t *sh,int cmd,void* arg, ...){

   return 0;
}

ad_functions_t mp_skogg = {
  // &info,
	preinit,
	init,
   uninit,
	control,
	decode_audio
};
#endif

