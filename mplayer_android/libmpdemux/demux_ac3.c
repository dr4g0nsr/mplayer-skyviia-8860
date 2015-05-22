#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "parse_es.h"
#include "stheader.h"
#include "aac_hdr.h"
#include "ms_hdr.h"

#define READLEN 4096
#define MAX_SEARCH 2048

typedef struct {
  uint64_t size;
  float time;
  float last_pts;
  int bitrate;
  int srate;
  int endian;     //big: 0x770b  little: 0x0b77
} ac3_priv_t;

static unsigned short rate[] = { 32,  40,  48,  56,  64,  80,  96, 112,
			               128, 160, 192, 224, 256, 320, 384, 448,
	      		         512, 576, 640};

static unsigned char halfrate[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3};

int ac3_bitrate=0;

static int ac3_syncinfo(unsigned char* buf, int* sample_rate, int* bit_rate)
{
  
   int frmsizecod;
   int bitrate;
   int half;
   int bsid;

   if ((buf[0] != 0x0b) || (buf[1] != 0x77))	/* syncword */
	   return 0;

   bsid = buf[5] >> 3;	   
   if (bsid > 16)		
	   return 0;      //illegal bsid   
   if (bsid > 10){
      if (buf[2] >= 0xc0)
         return 0;   // Not E-AC-3 format
   }      	   

   half = halfrate[buf[5] >> 3];

   frmsizecod = buf[4] & 63;
   if (frmsizecod >= 38)
	   return 0;
   bitrate = rate [frmsizecod >> 1];
   *bit_rate = (bitrate * 1000) >> half;

   switch (buf[4] & 0xc0) 
   {
      case 0:
	      *sample_rate = 48000 >> half;
	      return 4 * bitrate;
      case 0x40:
	      *sample_rate = 44100 >> half;
	      return 2 * (320 * bitrate / 147 + (frmsizecod & 1));
      case 0x80:
	      *sample_rate = 32000 >> half;
	      return 6 * bitrate;
      default:
	      return 0;
   }
}

inline static int stream_read_char_ac3(stream_t *s){
  return (s->buf_pos<s->buf_len)?s->buffer[s->buf_pos++]:
    (cache_stream_fill_buffer(s)?s->buffer[s->buf_pos++]:-256);
}      

static int ac3_sync(demuxer_t *demuxer, ac3_priv_t *priv)
{
   int read;
   unsigned char buf[8];
   stream_read(demuxer->stream, buf, 8); 
   read = 8;
   do{
      unsigned char swbuf[8];
      int len;
      int nc;   
      int sync = buf[0] + (buf[1]<<8);
      if (sync == 0x770b){
         len = ac3_syncinfo(buf, &priv->srate, &priv->bitrate);   
      }        
      else if (sync == 0x0b77){ 
         memcpy(swbuf, buf, 8);
         swab(swbuf, swbuf, 8);
         len = ac3_syncinfo(swbuf, &priv->srate, &priv->bitrate);   
      }    
      else
         goto nextbyte;     
      if(len>=7 && len<=3840){
         off_t current = stream_tell(demuxer->stream);  
         int ns = 0;
         stream_skip(demuxer->stream, len-8);
         stream_read(demuxer->stream, (unsigned char*)&ns, 2);
         if (ns == sync){
            priv->endian = sync;   //get AC3 file
            return current-8;         
         }       
         stream_seek(demuxer->stream, current);                         
      }
nextbyte:      
      memmove(buf, buf+1, 7);
      nc = stream_read_char_ac3(demuxer->stream);
      buf[7] = (unsigned char)nc;
      read++;
      if (read>MAX_SEARCH)
         return -1;  
      if(nc <= -256)
         return -1;  
   }while(1);
}   

static int demux_probe_ac3(demuxer_t *demuxer)
{
   ac3_priv_t *priv;
	
   if (! (priv = calloc(1, sizeof(ac3_priv_t))))
      goto fail;
   
   demuxer->priv = priv;
   priv->size = 0;
   priv->time = 0;
   priv->last_pts = 0;
   priv->endian = 0x770b;

   if (ac3_sync(demuxer, priv)<0)
      goto fail;     

   ac3_bitrate = priv->bitrate;
   mp_msg(MSGT_DEMUX, MSGL_V, "demux_ac3_probe, probe correct\n");
   return DEMUXER_TYPE_AC3;
fail:
   mp_msg(MSGT_DEMUX, MSGL_V, "demux_ac3_probe, failed to detect an AAC_ADIF stream\n");
   return 0;
}


static demuxer_t* demux_open_ac3(demuxer_t *demuxer)
{
  sh_audio_t *sh;
  sh = new_sh_audio(demuxer, 0);
  sh->ds = demuxer->audio;
  sh->format = 0x2000;
  demuxer->audio->id = 0;           //2010-07-16 : HONDA_ADIF add for new mplayer
  demuxer->audio->sh = sh;
  stream_seek(demuxer->stream, 0);
  demuxer->filepos = 0;
  //demuxer->seekable = 0;            //2010-10-2 : Not support seek this version
  return demuxer;
}

static int demux_fill_buffer_ac3(demuxer_t *demuxer, demux_stream_t *ds)
{
   ac3_priv_t *priv = (ac3_priv_t *) demuxer->priv;
   demux_packet_t *dp;
   int len;
   float tm = 0;

   if(demuxer->stream->eof || (demuxer->movi_end && stream_tell(demuxer->stream) >= demuxer->movi_end))
      return 0;

   len = READLEN;
   dp = new_demux_packet(len);
   if(! dp)
   {
      //fprintf(stderr,"fill_buffer,NEW AC3 PACKET(%d)FAILED\n", len);
      mp_msg(MSGT_DEMUX, MSGL_ERR, "fill_buffer, NEW AC3 PACKET(%d)FAILED\n", len);
      return 0;
   }
					
   len = stream_read(demuxer->stream, dp->buffer, READLEN);
   if (len != READLEN)
      len = len;
      
   if (priv->endian == 0x0b77)      
      swab(dp->buffer, dp->buffer, len);   

   if(priv->bitrate)
   {
      tm = (len * 8);
      tm /= (priv->bitrate); // FIXME assumes CBR
   }

   priv->last_pts += tm;
   dp->len = len;
   dp->pts = priv->last_pts;
   ds_add_packet(demuxer->audio, dp);
   priv->size += len;
   priv->time += tm;	
   demuxer->filepos = stream_tell(demuxer->stream);	
   return len;
}

static void demux_seek_ac3(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
   //TODO: implement later 
	ac3_priv_t *priv = (ac3_priv_t *) demuxer->priv;
	demux_stream_t *d_audio=demuxer->audio;
	sh_audio_t *sh_audio=d_audio->sh;
	float time;
	int curp;

	ds_free_packs(d_audio);

	time = (flags & SEEK_ABSOLUTE) ? rel_seek_secs - priv->last_pts : rel_seek_secs;
	if(time < 0)
	{
		stream_seek(demuxer->stream, demuxer->movi_start);
		time = priv->last_pts + time;
		priv->last_pts = 0;
	}

	if(time > 0)
	{
	   float lenf = (priv->bitrate>>3)*time;
	   int len = (int)lenf;
	   stream_skip(demuxer->stream, len); 
	   priv->last_pts += time; 
	}   
	curp = ac3_sync(demuxer, priv);
	stream_seek(demuxer->stream, curp);
	priv->size = curp;
}   

static void demux_close_ac3(demuxer_t *demuxer)
{
  ac3_priv_t *priv = (ac3_priv_t *) demuxer->priv;
	
  if(!priv)
    return;

  free(priv);
  demuxer->priv = 0;
  return;
}


demuxer_desc_t demuxer_desc_ac3 = {
  "AC3 demuxer",
  "ac3",
  "AC3",
  "Honda",
  "AC3 files ",
  DEMUXER_TYPE_AC3,
  0, 
  demux_probe_ac3,
  demux_fill_buffer_ac3,
  demux_open_ac3,
  demux_close_ac3,
  demux_seek_ac3,
  NULL
};