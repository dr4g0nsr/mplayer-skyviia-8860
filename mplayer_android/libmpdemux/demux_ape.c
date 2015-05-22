

#include <stdio.h>
#include <stdlib.h>

#include "mp_msg.h"
#include "help_mp.h"

#ifndef EngineMode
#include "stream/stream.h"
#include "./libavformat/avformat.h" //for ape
#include "./libavformat/apetag.h"   //for ape
#else
#include "stream.h"
#define SEEK_ABSOLUTE (1 << 0)
#endif

#include "demuxer.h"
#include "stheader.h"


#ifndef WIN32
#define __int64 long long 
#endif

typedef __int64 int64_t;

#ifndef UINT_MAX
#define UINT_MAX 0xFFFFFFFFUL
#endif

#define ENABLE_DEBUG 0

/* The earliest and latest file formats supported by this library */
#define APE_MIN_VERSION 3950
#define APE_MAX_VERSION 3990

#define MAC_FORMAT_FLAG_8_BIT                 1 // is 8-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_CRC                   2 // uses the new CRC32 error detection [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_PEAK_LEVEL        4 // uint32 nPeakLevel after the header [OBSOLETE]
#define MAC_FORMAT_FLAG_24_BIT                8 // is 24-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS    16 // has the number of seek elements after the peak level
#define MAC_FORMAT_FLAG_CREATE_WAV_HEADER    32 // create the wave header on decompression (not stored)

#define MAC_SUBFRAME_SIZE 4608

#define APE_EXTRADATA_SIZE 6

#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))

typedef struct {
    int64_t pos;
    int nblocks;
    int size;
    int skip;
    int64_t pts;
} APEFrame;

typedef struct {
    /* Derived fields */
    unsigned long junklength;
    unsigned long firstframe;
    unsigned long totalsamples;
    int currentframe;
    APEFrame *frames;

    /* Info from Descriptor Block */
    char magic[4];
    short fileversion;
    short padding1;
    unsigned long descriptorlength;
    unsigned long headerlength;
    unsigned long seektablelength;
    unsigned long wavheaderlength;
    unsigned long audiodatalength;
    unsigned long audiodatalength_high;
    unsigned long wavtaillength;
    unsigned char md5[16];

    /* Info from Header Block */
    unsigned short compressiontype;
    unsigned short formatflags;
    unsigned long blocksperframe;
    unsigned long finalframeblocks;
    unsigned long totalframes;
    unsigned short bps;
    unsigned short channels;
    unsigned long samplerate;

    /* Seektable */
    unsigned long *seektable;
   //time stamp
   float time;
   float last_pts;

} APECONTEXT;

static unsigned long get_ape32(demuxer_t *demuxer)
{
   unsigned long r;
   stream_read(demuxer->stream, (char*)&r, 4);
   return r;
}

static unsigned short get_ape16(demuxer_t *demuxer)
{
   unsigned short r;
   stream_read(demuxer->stream, (char*)&r, 2);
   return r;
}





void ape_dumpinfo( APECONTEXT* ape_ctx)
{
  int i;

    printf("Descriptor Block:\n\n");
    printf("magic                = \"%c%c%c%c\"\n",
            ape_ctx->magic[0],ape_ctx->magic[1],
            ape_ctx->magic[2],ape_ctx->magic[3]);
    printf("fileversion          = %d\n",ape_ctx->fileversion);
    printf("descriptorlength     = %ld\n",ape_ctx->descriptorlength);
    printf("headerlength         = %ld\n",ape_ctx->headerlength);
    printf("seektablelength      = %ld\n",ape_ctx->seektablelength);
    printf("wavheaderlength      = %ld\n",ape_ctx->wavheaderlength);
    printf("audiodatalength      = %ld\n",ape_ctx->audiodatalength);
    printf("audiodatalength_high = %ld\n",ape_ctx->audiodatalength_high);
    printf("wavtaillength        = %ld\n",ape_ctx->wavtaillength);
    printf("md5                  = ");
    for (i = 0; i < 16; i++)
        printf("%02x",ape_ctx->md5[i]);
    printf("\n");

    printf("\nHeader Block:\n\n");

    printf("compressiontype      = %d\n",ape_ctx->compressiontype);
    printf("formatflags          = %d\n",ape_ctx->formatflags);
    printf("blocksperframe       = %ld\n",ape_ctx->blocksperframe);
    printf("finalframeblocks     = %ld\n",ape_ctx->finalframeblocks);
    printf("totalframes          = %ld\n",ape_ctx->totalframes);
    printf("bps                  = %d\n",ape_ctx->bps);
    printf("channels             = %d\n",ape_ctx->channels);
    printf("samplerate           = %ld\n",ape_ctx->samplerate);

    printf("\nSeektable\n\n");
    if ((ape_ctx->seektablelength / sizeof(uint32_t)) != ape_ctx->totalframes)
    {
        printf("No seektable\n");
    }
    else
    {
        for ( i = 0; i < ape_ctx->seektablelength / sizeof(uint32_t) ; i++)
        {
            if (i < ape_ctx->totalframes-1) {
                printf("%8d   %ld (%ld bytes)\n",i,ape_ctx->seektable[i],ape_ctx->seektable[i+1]-ape_ctx->seektable[i]);
            } else {
                printf("%8d   %ld\n",i,ape_ctx->seektable[i]);
            }
        }
    }
    printf("\nCalculated information:\n\n");
    printf("junklength           = %ld\n",ape_ctx->junklength);
    printf("firstframe           = %ld\n",ape_ctx->firstframe);
    printf("totalsamples         = %ld\n",ape_ctx->totalsamples);
}


static int demux_probe_ape(demuxer_t *demuxer)
{
    APECONTEXT *ape = 0;
    unsigned long tag;
    int i;    
	
    if (! (ape = calloc(1, sizeof(APECONTEXT))))
      goto fail;

    /* TODO: Skip any leading junk such as id3v2 tags */
    ape->junklength = 0;

    tag = get_ape32(demuxer);
    if (tag != MKTAG('M', 'A', 'C', ' '))
      goto fail;

    ape->fileversion = get_ape16(demuxer);

    if (ape->fileversion < APE_MIN_VERSION || ape->fileversion > APE_MAX_VERSION) {
        printf("Unsupported file version - %d.%02d\n", ape->fileversion / 1000, (ape->fileversion % 1000) / 10);
        goto fail;
    }

    if (ape->fileversion >= 3980) {
        ape->padding1             = get_ape16(demuxer);
        ape->descriptorlength     = get_ape32(demuxer);
        ape->headerlength         = get_ape32(demuxer);
        ape->seektablelength      = get_ape32(demuxer);
        ape->wavheaderlength      = get_ape32(demuxer);
        ape->audiodatalength      = get_ape32(demuxer);
        ape->audiodatalength_high = get_ape32(demuxer);
        ape->wavtaillength        = get_ape32(demuxer);
        stream_read(demuxer->stream, ape->md5, 16);

        /* Skip any unknown bytes at the end of the descriptor.
           This is for future compatibility */
        if (ape->descriptorlength > 52)
            stream_skip(demuxer->stream, ape->descriptorlength - 52);

        /* Read header data */
        ape->compressiontype      = get_ape16(demuxer);
        ape->formatflags          = get_ape16(demuxer);
        ape->blocksperframe       = get_ape32(demuxer);
        ape->finalframeblocks     = get_ape32(demuxer);
        ape->totalframes          = get_ape32(demuxer);
        ape->bps                  = get_ape16(demuxer);
        ape->channels             = get_ape16(demuxer);
        ape->samplerate           = get_ape32(demuxer);
    } else {
        ape->descriptorlength = 0;
        ape->headerlength = 32;

        ape->compressiontype      = get_ape16(demuxer);
        ape->formatflags          = get_ape16(demuxer);
        ape->channels             = get_ape16(demuxer);
        ape->samplerate           = get_ape32(demuxer);
        ape->wavheaderlength      = get_ape32(demuxer);
        ape->wavtaillength        = get_ape32(demuxer);
        ape->totalframes          = get_ape32(demuxer);
        ape->finalframeblocks     = get_ape32(demuxer);

        if (ape->formatflags & MAC_FORMAT_FLAG_HAS_PEAK_LEVEL) {
            stream_skip(demuxer->stream, 4); /* Skip the peak level */
            ape->headerlength += 4;
        }

        if (ape->formatflags & MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS) {
            ape->seektablelength = get_ape32(demuxer);
            ape->headerlength += 4;
            ape->seektablelength *= sizeof(long);
        } else
            ape->seektablelength = ape->totalframes * sizeof(long);

        if (ape->formatflags & MAC_FORMAT_FLAG_8_BIT)
            ape->bps = 8;
        else if (ape->formatflags & MAC_FORMAT_FLAG_24_BIT)
            ape->bps = 24;
        else
            ape->bps = 16;

        if (ape->fileversion >= 3950)
            ape->blocksperframe = 73728 * 4;
        else if (ape->fileversion >= 3900 || (ape->fileversion >= 3800  && ape->compressiontype >= 4000))
            ape->blocksperframe = 73728;
        else
            ape->blocksperframe = 9216;

        /* Skip any stored wav header */
        if (!(ape->formatflags & MAC_FORMAT_FLAG_CREATE_WAV_HEADER))
            stream_skip(demuxer->stream, ape->wavheaderlength);
    }

    #ifdef WIN32
    printf("\n");
    printf("** fileversion : %d\t\t", ape->fileversion );
    printf("compression level : %d\n", ape->compressiontype );
    #endif

    //limit
    if(ape->compressiontype >4000) //not supported for insane mode 
    {
    printf("not supported APE format!");
    goto fail;
    }
    if(ape->totalframes > UINT_MAX / sizeof(APEFrame)){
        printf("Too many frames: %ld\n", ape->totalframes);
        goto fail;
    }
    ape->frames  = malloc(ape->totalframes * sizeof(APEFrame));
    if(!ape->frames)
        goto fail;
    ape->firstframe   = ape->junklength + ape->descriptorlength + ape->headerlength + ape->seektablelength + ape->wavheaderlength;
    ape->currentframe = 0;


    ape->totalsamples = ape->finalframeblocks;
    if (ape->totalframes > 1)
        ape->totalsamples += ape->blocksperframe * (ape->totalframes - 1);

    if (ape->seektablelength > 0) {
        ape->seektable = malloc(ape->seektablelength);
        for (i = 0; (unsigned)i < ape->seektablelength / sizeof(unsigned long); i++)
            ape->seektable[i] = get_ape32(demuxer);
    }

    ape->frames[0].pos     = ape->firstframe;
    ape->frames[0].nblocks = ape->blocksperframe;
    ape->frames[0].skip    = 0;
    for (i = 1; (unsigned)i < ape->totalframes; i++) {
        ape->frames[i].pos      = ape->seektable[i]; //ape->frames[i-1].pos + ape->blocksperframe;
        ape->frames[i].nblocks  = ape->blocksperframe;
        ape->frames[i - 1].size = (int)(ape->frames[i].pos - ape->frames[i - 1].pos);
        ape->frames[i].skip     = (int)((ape->frames[i].pos - ape->frames[0].pos) & 3);
    }


   //---------begin  8/24-bit mode ,larry
    //最後一個frame size 
    if (ape->bps==24)
       ape->frames[ape->totalframes - 1].size    = ape->finalframeblocks * 6; 
    else if (ape->bps==16)
       ape->frames[ape->totalframes - 1].size    = ape->finalframeblocks * 4;
    if (ape->bps==8)
       ape->frames[ape->totalframes - 1].size    = ape->finalframeblocks * 2;
     
    //----------end

 

    ape->frames[ape->totalframes - 1].nblocks = ape->finalframeblocks;

    for (i = 0; (unsigned)i < ape->totalframes; i++) {
        if(ape->frames[i].skip){
            ape->frames[i].pos  -= ape->frames[i].skip;
            ape->frames[i].size += ape->frames[i].skip;
        }
        ape->frames[i].size = (ape->frames[i].size + 3) & ~3;
    }

   if (ape->fileversion < 3980) 
   {
	   ape->audiodatalength = ape->frames[ape->totalframes-1].pos +ape->finalframeblocks;

   }
    /* try to read APE tags */
    //TODO : support APE tags
/*    if (!url_is_streamed(pb)) {
        ff_ape_parse_tag(s);
        url_fseek(pb, 0, SEEK_SET);
    }  */

    
    
#if 0
    pts = 0;
    for (i = 0; i < ape->totalframes; i++) {
        ape->frames[i].pts = pts;
        pts += ape->blocksperframe / MAC_SUBFRAME_SIZE;
    }
#endif
   //ape_dumpinfo(ape);
    demuxer->priv = ape;

    return DEMUXER_TYPE_APE;
fail:
   if (ape->frames)
      free(ape->frames);
   if (ape)
      free(ape);  
   return 0;
}

static demuxer_t* demux_open_ape(demuxer_t *demuxer)
{	
   APECONTEXT *ape = demuxer->priv;
   sh_audio_t *sh;
   int total_blocks;
   double byterate;
   sh = new_sh_audio(demuxer, 0);
   sh->ds = demuxer->audio;
   sh->format = 0x20455041;          //fourcc "APE "
   demuxer->audio->id = 0;           //add for new mplayer
   demuxer->audio->sh = sh;
   stream_seek(demuxer->stream, 0);
   demuxer->filepos = 0;
   demuxer->seekable = 1;            //TODO : support seek later  ,changed to "1" by larry
   
   sh->channels        = ape->channels;
   sh->samplerate     = ape->samplerate;
   sh->samplesize    = ape->bps>>3; //add 8/24-bit mode ,larry
   if (sh->samplesize==3) sh->samplesize = 4 ; //24 bit packed as 32 bit 

   sh->codecdata = malloc(6);  
   sh->codecdata_len = 6;

   memcpy(sh->codecdata + 0, &ape->fileversion, 2);
   memcpy(sh->codecdata + 2, &ape->compressiontype, 2);
   memcpy(sh->codecdata + 4, &ape->formatflags, 2);

   total_blocks = (ape->totalframes == 0) ? 0 : ((ape->totalframes - 1) * ape->blocksperframe) + ape->finalframeblocks;
   byterate = (double)ape->audiodatalength / ((double)total_blocks/ape->samplerate);
   sh->i_bps = (int)byterate;
   return demuxer;
}

static int demux_fill_buffer_ape(demuxer_t *demuxer, demux_stream_t *ds)
{

   APECONTEXT *ape = demuxer->priv;
   demux_packet_t *dp;
   int len;
   float tm = 0;
   int nblocks;
   unsigned long extra_size = 8;
   int blksize;

   if(demuxer->stream->eof ||(unsigned) (demuxer->movi_end && stream_tell(demuxer->stream) >= (unsigned)demuxer->movi_end))
      return 0;

   if ((unsigned)ape->currentframe > ape->totalframes)
      return 0;

    stream_seek(demuxer->stream, ape->frames[ape->currentframe].pos);
    blksize = ape->frames[ape->currentframe].size;

    /* Calculate how many blocks there are in this frame */
    if (ape->currentframe == (ape->totalframes - 1))
        nblocks = ape->finalframeblocks;
    else
        nblocks = ape->blocksperframe;

   dp = new_demux_packet(blksize+extra_size);
   if(! dp)
   {
      mp_msg(MSGT_DEMUX, MSGL_ERR, "fill_buffer, NEW APE PACKET(%d)FAILED\n", blksize);
      return 0;
   }  

   memcpy(dp->buffer, &nblocks, 4);
   memcpy(dp->buffer+4, &ape->frames[ape->currentframe].skip, 4);

   len = stream_read(demuxer->stream, dp->buffer+extra_size, blksize);
   if (len != blksize)
      len = len;
   
   dp->pts = ape->last_pts;
   dp->len = len+extra_size;
   ds_add_packet(demuxer->audio, dp);

   tm = (float)ape->blocksperframe/ape->samplerate;

   ape->last_pts += tm;
   ape->time += tm;	
   demuxer->filepos = stream_tell(demuxer->stream);	
   ape->currentframe++;
   return len;
}

static void demux_close_ape(demuxer_t *demuxer)
{
   APECONTEXT *ape = demuxer->priv;
   if (!ape)
      return;

   if (ape->seektable)
       free(ape->seektable);
   if (ape->frames)
      free(ape->frames);
   free(ape);   
   demuxer->priv = 0;
}

static void demux_seek_ape(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
//TODO : need to implement 
    //2010.12.30 larry added 
   /*
    printf("\nseek  ape \n");
    printf("rel_seek_secs=%f\n",rel_seek_secs);
  	 printf("audio_delay=%f\n",audio_delay);
    printf("flags=%d\n",flags);	
    */

    
    
    
  
    int total_blocks;
    double totaltime_secs ;     
    float increment  ; 
    int secs ;
    float time;  
    APECONTEXT *ape = demuxer->priv;
    //printf("\nape->last_pts=%f\n",ape->last_pts);
    
    
	  time = (flags & SEEK_ABSOLUTE) ? rel_seek_secs - ape->last_pts : rel_seek_secs; //絕對,相對
	  //printf("\ntime=%f\n",time);
    
           
    total_blocks = (ape->totalframes == 0) ? 0 : ((ape->totalframes - 1) * ape->blocksperframe) + ape->finalframeblocks;
    totaltime_secs = ((double)total_blocks/ape->samplerate); //file length (seconds)
    //printf("total seconds =%f\n\n",totaltime_secs);	//總長度秒數
  
    //printf("ape->currentframe=%d\n\n",ape->currentframe);	
    //printf("incerment=%f\n\n",ape->totalframes/totaltime_secs*time );	
    increment =   ape->totalframes/totaltime_secs*time ;
    secs = ( increment < 0 ) ? increment - 0.5-2 : increment + 0.5;
    //printf("secs=%d\n\n",secs );
    ape->currentframe += secs;
    
    if(ape->currentframe <0) ape->currentframe =1;                  //lower bound
    if(ape->currentframe >ape->totalframes) 	ape->currentframe =ape->totalframes ; //upper bound
   
#if 1	//Barry 2011-06-07
    ape->last_pts = ((float)ape->currentframe/(float)ape->totalframes) * totaltime_secs;
#else
    ape->last_pts +=	time ;
#endif
    if (ape->last_pts<=0.0)		ape->last_pts = 0.0;

}

demuxer_desc_t demuxer_desc_ape = {
  "APE demuxer",
  "ape",
  "APE",
  "Honda",
  "APE files ",
  DEMUXER_TYPE_APE,
  1, 
  demux_probe_ape,
  demux_fill_buffer_ape,
  demux_open_ape,
  demux_close_ape,
  demux_seek_ape,
  NULL
};