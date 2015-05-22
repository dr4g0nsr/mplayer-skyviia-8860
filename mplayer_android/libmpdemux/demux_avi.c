/*
 * AVI file parser for DEMUXER v2.9
 * Copyright (c) 2001 A'rpi/ESP-team
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"


#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "demux_ogg.h"
#include "aviheader.h"

#ifdef SUPPORT_DIVX_DRM
#include "drm_mplayer.h"
extern int DRM_Open;
extern DRM_chunk *video_dd_chunk;
extern DRM_chunk *audio_dd_chunk;
static inline int DRM_KEY_Setup(demuxer_t *demux, char type);
static char do_video_decryption = 0;
static char do_audio_decryption = 0;
#endif // end of SUPPORT_DIVX_DRM

extern const demuxer_desc_t demuxer_desc_avi_ni;
extern const demuxer_desc_t demuxer_desc_avi_nini;

// PTS:  0=interleaved  1=BPS-based
int pts_from_bps=1;

//Fuchun 2009.12.08
static int packet_cnt;

//extern unsigned char* videobuffer;	// Raymond 2009/08/03
int check_h264_sps = 0;	//Barry 2010-06-01
int avi_got_asf_header = 0;//Barry 2010-09-08

//Barry 2010-10-18
int check_xvid_first_frame = 1;
extern int check_avc1_sps_bank0(unsigned char *in_buf, int len);
extern int check_mp4_header_vol(unsigned char * buf, int buf_size);

//Barry 2010-10-22
unsigned char* avi_asfHeader = NULL;
unsigned int avi_asfHeaderSize=0;
extern unsigned char* asfHeader;
extern unsigned int asfHeaderSize;

int mp4s_hdr_len = 0;	//Barry 2011-01-24

int no_need_asf_header = 0;	//Barry 2011-08-02

// Select ds from ID
static demux_stream_t *demux_avi_select_stream(demuxer_t *demux,
                                               unsigned int id)
{
  int stream_id=avi_stream_id(id);


  if(demux->video->id==-1)
    if(demux->v_streams[stream_id])
        demux->video->id=stream_id;

  if(demux->audio->id==-1)
    if(demux->a_streams[stream_id])
        demux->audio->id=stream_id;

#ifdef AVI_SUBTITLES
	avi_priv_t* priv=demux->priv;
	if(demux->sub->id == -1 && (stream_id-((int)priv->vid+(int)priv->aid)) >= 0)
		if(demux->s_streams[(stream_id-(priv->vid+priv->aid))])
			demux->sub->id = (stream_id-(priv->vid+priv->aid));
#endif

  if(stream_id==demux->audio->id){
      if(!demux->audio->sh){
        sh_audio_t* sh;
	avi_priv_t *priv=demux->priv;
        sh=demux->audio->sh=demux->a_streams[stream_id];
        //Barry 2010-12-28
        if (!sh->samplerate)
			sh->samplerate = sh->wf->nSamplesPerSec;
        if (!sh->channels)
			sh->channels = sh->wf->nChannels;

        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected AVI audio ID = %d\n",demux->audio->id);
	if(sh->wf){
	  priv->audio_block_size=sh->wf->nBlockAlign;
	  if(!priv->audio_block_size){
	    // for PCM audio we can calculate the blocksize:
	    if(sh->format==1)
		priv->audio_block_size=sh->wf->nChannels*(sh->wf->wBitsPerSample/8);
	    else
		priv->audio_block_size=1; // hope the best...
	  } else {
	    // workaround old mencoder's bug:
	    if(sh->audio.dwSampleSize==1 && sh->audio.dwScale==1 &&
	       (sh->wf->nBlockAlign==1152 || sh->wf->nBlockAlign==576)){
		mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_WorkAroundBlockAlignHeaderBug);
		priv->audio_block_size=1;
	    }
		/*
		 * Mantis 5968: audio pts wrong, because audio_block_size is 1 and dwScale is not equal to 1.
		 * According to AVI File format, the audio will be problem when dwScale set to  1152.
		 */
		if (sh->wf->nBlockAlign == 1 && sh->audio.dwScale == 1152)
		{
			priv->audio_block_size = 0;
			printf("In [%s][%d] set audio_block_size to zero. MP3 header problem$$$$$$$\n", __func__, __LINE__);
		}

	  }
	} else {
	  priv->audio_block_size=sh->audio.dwSampleSize;
	}
      }
      return demux->audio;
  }
  if(stream_id==demux->video->id){
	  //printf("$$$$$ In [%s][%d] stream_id is [%d] demux->video->id is [%d] $$$$\n", __func__, __LINE__, stream_id, demux->video->id);
      if(!demux->video->sh){
        demux->video->sh=demux->v_streams[stream_id];
        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected AVI video ID = %d\n",demux->video->id);
      }
      return demux->video;
  }
  
#ifdef AVI_SUBTITLES
	if((stream_id-((int)priv->vid+(int)priv->aid)) >= 0 && (stream_id-(priv->vid+priv->aid)) == demux->sub->id)
	{
		if(!demux->sub->sh)
		{
			demux->sub->sh = demux->s_streams[stream_id];
			mp_msg(MSGT_DEMUX, MSGL_V, "Auto-selected AVI subtitle ID = %d\n", demux->sub->id);
		}
		return demux->sub;
	}
#endif

  if(id!=mmioFOURCC('J','U','N','K')){
     // unknown
     mp_msg(MSGT_DEMUX,MSGL_DBG2,"Unknown chunk: %.4s (%X)\n",(char *) &id,id);
     //abort();
  }
  return NULL;
}

static int valid_fourcc(unsigned int id){
    static const char valid[] = "0123456789abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    unsigned char* fcc=(unsigned char*)(&id);
    return strchr(valid, fcc[0]) && strchr(valid, fcc[1]) &&
           strchr(valid, fcc[2]) && strchr(valid, fcc[3]);
}

static int valid_stream_id(unsigned int id) {
    unsigned char* fcc=(unsigned char*)(&id);
    return fcc[0] >= '0' && fcc[0] <= '9' && fcc[1] >= '0' && fcc[1] <= '9' &&
           ((fcc[2] == 'w' && fcc[3] == 'b') || (fcc[2] == 'd' && fcc[3] == 'c') || (fcc[2] == 's' && fcc[3] == 'b'));
}

static int choose_chunk_len(unsigned int len1,unsigned int len2){
    // len1 has a bit more priority than len2. len1!=len2
    // Note: this is a first-idea-logic, may be wrong. comments welcomed.

    // prefer small frames rather than 0
    if(!len1) return (len2>0x80000) ? len1 : len2;
    if(!len2) return (len1>0x100000) ? len2 : len1;

    // choose the smaller value:
    return (len1<len2)? len1 : len2;
}

static int demux_avi_read_packet(demuxer_t *demux,demux_stream_t *ds,unsigned int id,unsigned int len,int idxpos,int flags){
  avi_priv_t *priv=demux->priv;
  int skip;
  float pts=0;

  mp_dbg(MSGT_DEMUX,MSGL_DBG3,"demux_avi.read_packet: %X\n",id);

  if(ds==demux->audio){
      if(priv->pts_corrected==0){
          if(priv->pts_has_video){
	      // we have video pts now
	      float delay=0;
	      if(((sh_audio_t*)(ds->sh))->wf->nAvgBytesPerSec)
	          delay=(float)priv->pts_corr_bytes/((sh_audio_t*)(ds->sh))->wf->nAvgBytesPerSec;
	      mp_msg(MSGT_DEMUX,MSGL_V,"XXX initial  v_pts=%5.3f  a_pos=%d (%5.3f) \n",priv->avi_audio_pts,priv->pts_corr_bytes,delay);
	      //priv->pts_correction=-priv->avi_audio_pts+delay;
	      priv->pts_correction=delay-priv->avi_audio_pts;
	      priv->avi_audio_pts+=priv->pts_correction;
	      priv->pts_corrected=1;
	  } else
	      priv->pts_corr_bytes+=len;
      }
#if 0	//Fuchun 2010.08.24
    if(demux->type == DEMUXER_TYPE_AVI_NINI)	
    {
	if(pts_from_bps){
	  pts = priv->audio_block_no *
	    (float)((sh_audio_t*)demux->audio->sh)->audio.dwScale /
	    (float)((sh_audio_t*)demux->audio->sh)->audio.dwRate;
	  priv->avi_audio_pts = pts;
      } else
          pts=priv->avi_audio_pts; //+priv->pts_correction;
    }
    else
#endif
    {
      if(pts_from_bps){
	  pts = priv->audio_block_no *
	    (float)((sh_audio_t*)demux->audio->sh)->audio.dwScale /
	    (float)((sh_audio_t*)demux->audio->sh)->audio.dwRate;
      } else
          pts=priv->avi_audio_pts; //+priv->pts_correction;
      priv->avi_audio_pts=0;
    }
      // update blockcount:
      priv->audio_block_no+=priv->audio_block_size ?
	((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
  } else
  if(ds==demux->video){
     // video
     if(priv->skip_video_frames>0){
       // drop frame (seeking)
       --priv->skip_video_frames;
#ifdef SUPPORT_DIVX_DRM
	   do_video_decryption = 0;
#endif
       ds=NULL;
     }

#if 1	//Fuchun 2009.12.02
	if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2))
	{
		int i;
#ifdef SUPPORT_DIVX_DRM	
		int j = 0;
		if(speed_mult >= 2)
		{
			for(i = priv->last_idx_pos; i < (int)ds->pos; i++)
			{
#ifdef AVI_NOT_SORT
				int id;
				if(priv->not_sort)
					id = (&((AVIINDEXENTRY *)priv->idx_v)[i])->ckid;
				else
					id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#else
				int id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#endif
//				printf("@@@@ In [%s][%d] speed_mult[%d] i [%d] dp->pos [%lld] avi_stream_id [%d] ds->id[%d] id [%x] priv->suidx_size[%x] @@@@\n", __func__, __LINE__, speed_mult, i, ds->pos, avi_stream_id(id), ds->id, id >> 16, priv->suidx_size);
				if(avi_stream_id(id) == ds->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
				//if(avi_stream_id(id) == ds->id)
					++ds->pack_no;
			}
			priv->last_idx_pos = ds->pos+1;
		}
		else
		{
			for(i = priv->last_idx_pos; i > (int)ds->pos; i--)
			{
#ifdef AVI_NOT_SORT
				int id;
				if(priv->not_sort)
					id = (&((AVIINDEXENTRY *)priv->idx_v)[i])->ckid;
				else
					id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#else
				int id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#endif
//				printf("@@@@ In [%s][%d] speed_mult[%d] i [%d] dp->pos [%lld] avi_stream_id [%d] ds->id[%d] id [%x] priv->suidx_size[%x] @@@@\n", __func__, __LINE__, speed_mult, i, ds->pos, avi_stream_id(id), ds->id, id >> 16, priv->suidx_size);
				if(avi_stream_id(id) == ds->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
				//if(avi_stream_id(id) == ds->id )
				{
					j++;
					--ds->pack_no;
				}
			}
			if(j == 0)
				--ds->pack_no;
			--ds->pack_no;
			priv->last_idx_pos = ds->pos-1;
		}
#else /* else of SUPPORT_DIVX_DRM */
		ds->pack_no = 0;
		for(i = 0; i < ds->pos; i++)
		{
#ifdef AVI_NOT_SORT
			int id;
			if(priv->not_sort)
				id = (&((AVIINDEXENTRY *)priv->idx_v)[i])->ckid;
			else
				id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#else
			int id = (&((AVIINDEXENTRY *)priv->idx)[i])->ckid;
#endif
			if(avi_stream_id(id) == ds->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
			//if(avi_stream_id(id) == ds->id)
				++ds->pack_no;
		}
#endif /* end of SUPPORT_DIVX_DRM */
		//printf("$$$$$$$ In [%s][%d] pack_no[%d] dwScale[%f] dwRate[%f] pts[%f] $$$$$$\n", __func__, __LINE__, ds->pack_no, (float)((sh_video_t*)demux->video->sh)->video.dwScale, (float)((sh_video_t*)demux->video->sh)->video.dwRate, priv->video_pack_no *(float)((sh_video_t*)demux->video->sh)->video.dwScale /(float)((sh_video_t*)demux->video->sh)->video.dwRate);
		priv->video_pack_no = ds->pack_no;
	}
#endif
     pts = priv->avi_video_pts = priv->video_pack_no *
         (float)((sh_video_t*)demux->video->sh)->video.dwScale /
	 (float)((sh_video_t*)demux->video->sh)->video.dwRate;


   if(!pts_from_bps)	//Fuchun 2010.08.24
     priv->avi_audio_pts=priv->avi_video_pts+priv->pts_correction;
     priv->pts_has_video=1;

     if(ds) ++priv->video_pack_no;

  }

	skip=(len+1)&(~1); // total bytes in this chunk

	if(ds)
	{
		mp_dbg(MSGT_DEMUX,MSGL_DBG2,"DEMUX_AVI: Read %d data bytes from packet %04X\n",len,id);
		//Fuchun 2009.12.08
		if(!FR_to_end && speed_mult != 0 && is_mjpeg == 1)
		{
			if(ds == demux->video)
			{
				packet_cnt++;

				if(packet_cnt % mjpeg_skip2 < mjpeg_skip1)
				{
					demux_packet_t* dp;
#if 1
					dp=new_demux_packet(len);
#else
					if(videobuffer != NULL)
						dp=new_video_packet(len, videobuffer);
					else
						dp=new_demux_video_packet(len);
#endif			
					len = stream_read(demux->stream,dp->buffer,len);
#ifdef SUPPORT_DIVX_DRM                 
					//printf("### In [%s][%d] video_dd_chunk [%p] do_video_decryption [%d] EncryptionLength[%d] ###\n", __func__, __LINE__, video_dd_chunk, do_video_decryption, video_dd_chunk->EncryptionLength);
					if (video_dd_chunk && do_video_decryption && video_dd_chunk->EncryptionLength)
					{
						if (video_dd_chunk->EncryptionLength > 0)
							DRMDecode(len, dp->buffer, video_dd_chunk, DIVX_DECODE_VIDEO);
						do_video_decryption = 0;
					}
#endif /* end of SUPPORT_DIVX_DRM */
					dp->pts=pts; //(float)pts/90000.0f;
					dp->pos=idxpos;
					dp->flags=flags;
					// append packet to DS stream:
					ds_add_packet(ds,dp);
				}
				else
					stream_skip(demux->stream, len);
			}
			else
				ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
		}
#ifdef SUPPORT_DIVX_DRM
		else if ( DRM_Open > 0 && ds == demux->video && do_video_decryption)
		{
			//ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
			demux_packet_t *dp = new_demux_packet(len);
			len = stream_read(demux->stream, dp->buffer, len);
			//printf("### In [%s][%d] video_dd_chunk [%p] do_video_decryption [%d] EncryptionLength[%d] ###\n", __func__, __LINE__, video_dd_chunk, do_video_decryption, video_dd_chunk->EncryptionLength);
			if (video_dd_chunk && video_dd_chunk->EncryptionLength)
			{
				if (video_dd_chunk->EncryptionLength > 0)
					DRMDecode(len, dp->buffer, video_dd_chunk, DIVX_DECODE_VIDEO);
				do_video_decryption = 0;
			}
			resize_demux_packet(dp, len);
			dp->pts = pts;
			dp->pos = idxpos;
			dp->flags = flags;
			// append packet to DS stream:
			ds_add_packet(ds, dp);

		}
#endif /* end of SUPPORT_DIVX_DRM */
		else if(ds == demux->video && (((sh_video_t*)demux->video->sh)->format == mmioFOURCC('M','4','S','2')	//Fuchun 2010.04.21
				|| ((sh_video_t*)demux->video->sh)->format == mmioFOURCC('M','P','4','S')) )	//Barry 2011-01-24
		{
			if(((sh_video_t*)demux->video->sh)->mpeg4_header_len != 0)
			{
				demux_packet_t* dp;
				int header_len = ((sh_video_t*)demux->video->sh)->mpeg4_header_len;
				unsigned char *p = ((sh_video_t*)demux->video->sh)->mpeg4_header;
					
				dp=new_demux_packet(len+header_len);
				memcpy(dp->buffer, p, header_len);
				stream_read(demux->stream,dp->buffer+header_len,len);
				dp->pts=pts; //(float)pts/90000.0f;
				dp->pos=idxpos;
				dp->flags=flags;
				// append packet to DS stream:
				ds_add_packet(ds,dp);

				len += header_len;
				((sh_video_t*)demux->video->sh)->mpeg4_header_len = 0;
				skip=(len+1)&(~1);
			}
			else
				ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
		}
		else if ( !no_need_asf_header && ds == demux->video && (((sh_video_t*)demux->video->sh)->bih->biCompression == mmioFOURCC('W','V','C','1')) )
		{
			demux_packet_t* dp;
			int header_len = 0, i;
			if (!avi_got_asf_header)
			{
				header_len = ((sh_video_t*)demux->video->sh)->bih->biSize - sizeof(BITMAPINFOHEADER) - 1;
				unsigned char *p = ((uint8_t*)(((sh_video_t*)demux->video->sh)->bih)) + sizeof(BITMAPINFOHEADER) + 1;	
				printf("AVI: asfHeader[%d] =", header_len);
				for (i=0;i<header_len;i++)
					printf("%.2X ", p[i]);
				printf("\n");

				dp = new_demux_packet(len+header_len+4);
				memcpy(dp->buffer, p, header_len);
				avi_got_asf_header++;
			}
			else
				dp = new_demux_packet(len+header_len+4);

			dp->buffer[header_len] = 0x00;
			dp->buffer[header_len+1] = 0x00;
			dp->buffer[header_len+2] = 0x01;
			dp->buffer[header_len+3] = 0x0D;
			stream_read(demux->stream,dp->buffer+header_len+4,len);
			#if 1	//Barry 2011-08-02
			if ( (header_len != 0) && !strncmp(dp->buffer, dp->buffer+header_len+4, header_len+4) )
			{
				//printf("\n\n########  [%s - %d]  dp->len = %d     len = %d  ############\n\n", __func__, __LINE__, dp->len, len);
				memcpy(dp->buffer, dp->buffer+header_len+4, len);
				dp->len = len;
				no_need_asf_header = 1;
			}
			#endif

			dp->pts=pts; //(float)pts/90000.0f;
			dp->pos=idxpos;
			dp->flags=flags;
			// append packet to DS stream:
			ds_add_packet(ds,dp);
		}

#if 1	//Barry 2010-10-22
		else if ( ds == demux->video && (((sh_video_t*)demux->video->sh)->bih->biCompression == mmioFOURCC('W','M','V','3')) )
		{
			demux_packet_t* dp;
			dp = new_demux_packet(len+8);

			dp->buffer[0] = (len & 0xFF);
			dp->buffer[1] = (len >> 8) & 0xFF;
			dp->buffer[2] = (len >> 16) & 0xFF;

			//check keyframe
#ifdef AVI_NOT_SORT
			uint32_t id;
			if(priv->not_sort)
				id = (&((AVIINDEXENTRY *)priv->idx_v)[idxpos])->dwFlags;
			else
				id = (&((AVIINDEXENTRY *)priv->idx)[idxpos])->dwFlags;
#else
			uint32_t id = (&((AVIINDEXENTRY *)priv->idx)[idxpos])->dwFlags;
#endif
			if(id & AVIIF_KEYFRAME)
				dp->buffer[3] = 0x80;
			else
				dp->buffer[3] = 0;

			//Add timeStamp
			unsigned int timeStamp = (unsigned int)((unsigned int)(pts * 1000));
			PUT32_L(timeStamp, (unsigned char*)(dp->buffer+4));

			stream_read(demux->stream,dp->buffer+8,len);
			dp->pts=pts; //(float)pts/90000.0f;
			dp->pos=idxpos;
			dp->flags=flags;

			// append packet to DS stream:
			ds_add_packet(ds,dp);
		}
#endif
#if 1	//Barry 2010-10-18
		else if ( ds == demux->video && (((sh_video_t*)demux->video->sh)->bih->biCompression == mmioFOURCC('X','V','I','D')) )
		{
			if (check_xvid_first_frame)
			{
				int i = 0, skip_byte = 0, pre_start_code_check = 0, read_len = 0;
				unsigned char skip_value = 0;
				demux_packet_t* dp;
				dp = new_demux_packet(len);
				if (len < 128)
					read_len = stream_read(demux->stream, dp->buffer, len);
				else
					read_len = stream_read(demux->stream, dp->buffer, 128);
				for (i=0; i<read_len-4-skip_byte; i++)
				{
					if (dp->buffer[i]==0 && dp->buffer[i+1]==0 && dp->buffer[i+2]==1)
					{
						if (dp->buffer[i+3]==0xB0)
						{
							if (dp->buffer[i+5] != 0 && dp->buffer[i+6]==0 && dp->buffer[i+7]==0 && dp->buffer[i+8]==1)
							{
								skip_value = dp->buffer[i+5];
								memcpy(dp->buffer+i+5, dp->buffer+i+6, 128-(i+6));
								skip_byte++;
							}
							else
								check_xvid_first_frame = 0;
						}

						if (pre_start_code_check)	// start code <= 0xB2
						{
							if (dp->buffer[i-1] == skip_value && skip_byte)
							{
								memcpy(dp->buffer+i-1, dp->buffer+i, 128-i);
								skip_byte++;
								i--;
								pre_start_code_check = 0;
							}
						}

						if ((dp->buffer[i+3] <= 0xB2) && (dp->buffer[i+3] != 0xB0))
							pre_start_code_check = 1;
						else
							pre_start_code_check = 0;
						if (dp->buffer[i+3]==0xB6)
							break;
					}
				}

				if ( (len - 128) > 0 )
					stream_read(demux->stream, dp->buffer+128-skip_byte, len-128);
				dp->pts=pts; //(float)pts/90000.0f;
				dp->pos=idxpos;
				dp->flags=flags;
				dp->len -= skip_byte;
				// append packet to DS stream:
				ds_add_packet(ds,dp);
			}
			else
				ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
		}
#endif
		else
		{
#if 0	// Raymond 2009/08/03
			if(ds == demux->video)
			{
				demux_packet_t* dp;
				if(videobuffer != NULL)
					dp=new_video_packet(len, videobuffer);
				else
					dp=new_demux_video_packet(len);
				
				len = stream_read(demux->stream,dp->buffer,len);
				dp->pts=pts; //(float)pts/90000.0f;
				dp->pos=idxpos;
				dp->flags=flags;
				// append packet to DS stream:
				ds_add_packet(ds,dp);
			}
			else
#endif	
#ifdef SUPPORT_DIVX_DRM
				if(ds == demux->audio && audio_dd_chunk)
				{
					//ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
					//printf("#### Carlos in [%s][%d] will do audio decryption offset[%x] size[%x] audio len[%x]####\n", __func__, __LINE__, audio_dd_chunk->EncryptionOffset, audio_dd_chunk->EncryptionLength, len);

					if (len > (audio_dd_chunk->EncryptionOffset + audio_dd_chunk->EncryptionLength))
					{
						demux_packet_t *dp = new_demux_packet(len);
						len = stream_read(demux->stream, dp->buffer, len);
						//printf("### In [%s][%d] video_dd_chunk [%p] do_video_decryption [%d] EncryptionLength[%d] ###\n", __func__, __LINE__, video_dd_chunk, do_video_decryption, video_dd_chunk->EncryptionLength);
						//printf("@@@ In [%s][%d] really audio len is [%x] @@@\n", __func__, __LINE__, len);
						DRMDecode(len, dp->buffer, audio_dd_chunk, DIVX_DECODE_AUDIO);
						resize_demux_packet(dp, len);
						dp->pts = pts;
						dp->pos = idxpos;
						dp->flags = flags;
						// append packet to DS stream:
						ds_add_packet(ds, dp);
					}
					else
						ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
				}
				else
#endif /* end of SUPPORT_DIVX_DRM */               
	    		ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
		}
		skip-=len;
	}
	
	skip = FFMAX(skip, 0);
  if (avi_stream_id(id) > 99 && id != mmioFOURCC('J','U','N','K'))
    skip = FFMIN(skip, 65536);
  if(skip){
    mp_dbg(MSGT_DEMUX,MSGL_DBG2,"DEMUX_AVI: Skipping %d bytes from packet %04X\n",skip,id);
    stream_skip(demux->stream,skip);
  }
  return ds?1:0;
}

extern int zero_data_cnt;
static uint32_t avi_find_id(stream_t *stream) {
  uint32_t id = stream_read_dword_le(stream);
  if (!id) {
    mp_msg(MSGT_DEMUX, MSGL_WARN, "Incomplete stream? Trying resync.\n");
    do {
      id = stream_read_dword_le(stream);
      //Barry 2011-05-19
      if (zero_data_cnt >= 20)
      {
      		stream->eof = 1;
      		return 0;
      }

      if (stream_eof(stream)) return 0;
    } while (avi_stream_id(id) > 99);
  }
  return id;
}

// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_avi_fill_buffer(demuxer_t *demux, demux_stream_t *dsds){
avi_priv_t *priv=demux->priv;
unsigned int id=0;
unsigned int len;
int ret=0;
demux_stream_t *ds;

//Fuchun 2009.11.27
demux_stream_t *ds1 = NULL;
demux_stream_t *ds2;
int keyframe_cnt = 0;/*, fast_offset = 0;
unsigned char *pkt_buf = NULL;*/
#ifdef SUPPORT_DIVX_DRM
char do_dencryption = 0;
unsigned int new_id = 0;
#endif /* end of SUPPORT_DIVX_DRM */

do{
  int flags=1;
  AVIINDEXENTRY *idx=NULL;
  if(priv->idx_size>0 && priv->idx_pos<priv->idx_size){
    off_t pos;

#if 1	//Fuchun 2009.12.02
	if(!FR_to_end && speed_mult < 0)
	{
		if(priv->idx_pos == 0)
		{
#if 0
			demux->stream->eof = 1;
			return 0;
#else
//			speed_mult = 0;
			FR_to_end = 1;
			idx = &((AVIINDEXENTRY *)priv->idx)[priv->idx_pos++];
#endif
		}
		else
		{
			idx = &((AVIINDEXENTRY *)priv->idx)[priv->idx_pos--];
			//printf("==== In [%s][%d]  idx->ckid is [%x]  priv->idx_pos[%d]===\n", __func__, __LINE__,  idx->ckid, priv->idx_pos);
			ds1 = demux_avi_select_stream(demux, idx->ckid);
		}
	}
	else if (speed_mult >= 2)
	{
		if(priv->idx_pos >= priv->idx_size-1)
		{
			demux->stream->eof = 1;
			return 0;
		}
		else
		{
			idx = &((AVIINDEXENTRY *)priv->idx)[priv->idx_pos++];
			//printf("==== In [%s][%d]  idx->ckid is [%x]  priv->idx_pos[%d]===\n", __func__, __LINE__,  idx->ckid, priv->idx_pos);
			ds1 = demux_avi_select_stream(demux, idx->ckid);
		}
	}
	else
		idx = &((AVIINDEXENTRY *)priv->idx)[priv->idx_pos++];

#if 1	//Fuchun 2011.03.03
	if(idx->dwChunkLength == 0 && idx->dwChunkOffset == 0 && priv->idx_pos < priv->idx_size)
	{
		extern int Can_FF_FB;
		printf("==== switch to AVI_NINI, just can FF to 2x and can't fast rewind !!! ====\n");
		demux->type = DEMUXER_TYPE_AVI_NINI;
		demux->desc=&demuxer_desc_avi_nini;
		priv->idx_pos=demux->filepos; // hack
//		printf("@@@ idx_pos[%"PRId64"] @@@\n", priv->idx_pos);
		priv->idx_pos_v=priv->idx_pos_a=priv->idx_pos;
		Can_FF_FB = 0;
		if(speed_mult != 0)
			FFFR_to_normalspeed(NULL);
				
		return -1;
	}
#endif
	if(!FR_to_end && speed_mult < 0)
	{
		if(!(ds1 == demux->video && (idx->dwFlags & AVIIF_KEYFRAME)))
			continue;

		int kk;
		for(kk = priv->idx_pos; kk > 0; kk--)
		{
			AVIINDEXENTRY *idx2 = NULL;
			idx2 = &((AVIINDEXENTRY *)priv->idx)[kk];
			ds2 = demux_avi_select_stream(demux, idx2->ckid);
#ifdef SUPPORT_DIVX_DRM
			if(ds2 != demux->video || (priv->suidx_size == 0 && ((idx2->ckid >> 16) == 0x6464)))
#else /* else of SUPPORT_DIVX_DRM */
			if(ds2 != demux->video)
#endif /* end of SUPPORT_DIVX_DRM */			
				continue;
			else
			{
				if(idx2->dwFlags & AVIIF_KEYFRAME)
				{
					idx = idx2;
					keyframe_cnt++;
					if(keyframe_cnt >= 15)
						break;
				}
				else
					break;
			}
		}
		if(keyframe_cnt != 0)
			priv->idx_pos = kk;
#ifdef SUPPORT_DIVX_DRM
       if (DRM_Open > 0 && priv->suidx_size == 0)
       {
	   	   int kk = 0;
           AVIINDEXENTRY *idx2 = NULL;
           for(; kk < priv->idx_size; kk++)
           {
               idx2 = &((AVIINDEXENTRY *)priv->idx)[kk];
               if(idx2 == idx)
               {
                   idx2 = &((AVIINDEXENTRY *)priv->idx)[--kk];
                   break;
               }
           }
           if(kk == priv->idx_size)
           {
               printf("@@@@ In[%s][%d] no keyframe be found ???? @@@@\n", __func__, __LINE__);
           }
           else
           {
               pos = (off_t)priv->idx_offset+AVI_IDX_OFFSET(idx2);
               stream_seek(demux->stream,pos);
               id=stream_read_dword_le(demux->stream);
               if (id >> 16 == 0x6464) //xxdd skip for DRM
               {
//                   printf("@@@@ In [%s][%d] call DRM_KEY_Setup len [%d], priv->suidx_size[%d] priv->suidx[%p]@@@@\n", __func__, __LINE__, len, priv->suidx_size, priv->suidx);
                   len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
                   do_video_decryption = 1;
               }
           }
       }
#endif /* end of SUPPORT_DIVX_DRM */
	}
	else if(speed_mult >= 2)
	{
		if(!(ds1 == demux->video && (idx->dwFlags & AVIIF_KEYFRAME)))
			continue;

		int kk = 0;
		for(kk = priv->idx_pos; kk < priv->idx_size; kk++)
		{
			AVIINDEXENTRY *idx2 = NULL;
			idx2 = &((AVIINDEXENTRY *)priv->idx)[kk];
			ds2 = demux_avi_select_stream(demux, idx2->ckid);
#ifdef SUPPORT_DIVX_DRM
			if(ds2 != demux->video || (priv->suidx_size == 0 && ((idx2->ckid >> 16) == 0x6464)))
#else /* else of SUPPORT_DIVX_DRM */
			if(ds2 != demux->video)
#endif /* end of SUPPORT_DIVX_DRM */
				continue;
			else
			{
				if(idx2->dwFlags & AVIIF_KEYFRAME)
				{
					idx = idx2;
					keyframe_cnt++;
					if(keyframe_cnt >= 15)
						break;
				}
				else
					break;
			}
		}
		if(keyframe_cnt != 0)
			priv->idx_pos = kk;
#ifdef SUPPORT_DIVX_DRM
       if (DRM_Open > 0 && priv->suidx_size == 0)
       {
           AVIINDEXENTRY *idx2 = NULL;
           for(; kk > 0; kk--)
           {
               idx2 = &((AVIINDEXENTRY *)priv->idx)[kk];
               if(idx2 == idx)
               {
                   idx2 = &((AVIINDEXENTRY *)priv->idx)[--kk];
                   break;
               }
           }
           if(kk == 0)
           {
               printf("@@@@ In[%s][%d] no keyframe be found ???? @@@@\n", __func__, __LINE__);
           }
           else
           {
               pos = (off_t)priv->idx_offset+AVI_IDX_OFFSET(idx2);
               stream_seek(demux->stream,pos);
               id=stream_read_dword_le(demux->stream);
               if (id >> 16 == 0x6464) //xxdd skip for DRM
               {
//                 printf("@@@@ In [%s][%d] call DRM_KEY_Setup @@@@\n", __func__, __LINE__);
                   len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
                   do_video_decryption = 1;
               }
           }
       }
#endif
	}
#else  
    idx=&((AVIINDEXENTRY *)priv->idx)[priv->idx_pos++];
#endif

    if(idx->dwFlags&AVIIF_LIST){
      if (!valid_stream_id(idx->ckid))
      // LIST
      continue;
      if (!priv->warned_unaligned)
        mp_msg(MSGT_DEMUX, MSGL_WARN, "Looks like unaligned chunk in index, broken AVI file!\n");
      priv->warned_unaligned = 1;
    }
    if(!demux_avi_select_stream(demux,idx->ckid)){
      mp_dbg(MSGT_DEMUX,MSGL_DBG3,"Skip chunk %.4s (0x%X)  \n",(char *)&idx->ckid,(unsigned int)idx->ckid);
      continue; // skip this chunk
    }

    pos = (off_t)priv->idx_offset+AVI_IDX_OFFSET(idx);
    if((pos<demux->movi_start || pos>=demux->movi_end) && (demux->movi_end>demux->movi_start) && (demux->stream->flags & MP_STREAM_SEEK)){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkOffset out of range!   idx=0x%"PRIX64"  \n",(int64_t)pos);
      continue;
    }
    stream_seek(demux->stream,pos);
    demux->filepos=stream_tell(demux->stream);
    id=stream_read_dword_le(demux->stream);
#ifdef SUPPORT_DIVX_DRM
	if (id >> 16 == 0x6464 && DRM_Open > 0 && (speed_mult == 0 || speed_mult == 1 || priv->suidx_size > 0)) //xxdd skip for DRM
	{
		len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
		//printf("@@@@ In [%s][%d] call DRM_KEY_Setup len [%d], priv->suidx_size[%d] priv->suidx[%p]@@@@\n", __func__, __LINE__, len, priv->suidx_size, priv->suidx);
		do_video_decryption = 1;
		if (priv->suidx_size > 0)
		{
			new_id=stream_read_dword_le(demux->stream);
			//printf("#### In [%s][%d] id [%x] id >> 16[%x]idx->ckid[%x]dwChunkLength[%x]####\n", __func__, __LINE__, id, id >> 16, idx->ckid, idx->dwChunkLength);
			//idx->ckid = id;
		}
		else
		{
			//printf("#### In [%s][%d] do continue id [%x] id >> 16[%x] suidx_sizei[%d]####\n", __func__, __LINE__, id, id >> 16, priv->suidx_size);
			continue;
		}
	}
#endif // end of SUPPORT_DIVX_DRM
    if(stream_eof(demux->stream)) return 0; // EOF!

    if(id!=idx->ckid){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkID mismatch! raw=%.4s idx=%.4s  \n",(char *)&id,(char *)&idx->ckid);
      if(valid_fourcc(idx->ckid))
          id=idx->ckid;	// use index if valid
      else
          if(!valid_fourcc(id)) continue; // drop chunk if both id and idx bad
    }
    len=stream_read_dword_le(demux->stream);
#if 1	//Fuchun 2009.12.02
	if(demux->filepos + 8 + len > demux->movi_end)
	{
		printf("######  [%s - %d]  len=0x%X    filepos=%lld    end_pos=%lld    movi_end=%lld   movi_start=%lld\n", __func__, __LINE__, len, demux->filepos, demux->stream->end_pos, demux->movi_end, demux->movi_start);
		demux->stream->eof = 1;
		printf("File truncated => EOF\n");
		return 0;
	}
#endif
#if 1
#ifdef SUPPORT_DIVX_DRM
	if (do_video_decryption && priv->suidx_size > 0 && (new_id >> 16 == 0x6364 || new_id >> 16 == 0x6264) )
	{
		idx->dwChunkLength = len;
	}
#endif /* end of SUPPORT_DIVX_DRM*/
#endif

    if((len!=idx->dwChunkLength)&&((len+1)!=idx->dwChunkLength)){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkSize mismatch! raw=%d idx=%d  \n",len,idx->dwChunkLength);
      if(len>0x200000 && idx->dwChunkLength>0x200000) continue; // both values bad :(
      len=choose_chunk_len(idx->dwChunkLength,len);
    }
    if(!(idx->dwFlags&AVIIF_KEYFRAME)) flags=0;
  } else {
    demux->filepos=stream_tell(demux->stream);
    if(demux->filepos>=demux->movi_end && demux->movi_end>demux->movi_start && (demux->stream->flags & MP_STREAM_SEEK)){
          demux->stream->eof=1;
          return 0;
    }
    id=avi_find_id(demux->stream);
#ifdef SUPPORT_DIVX_DRM
	if (id >> 16 == 0x6464) //xxdd skip for DRM
	{
		//printf("@@@@ In [%s][%d] call DRM_KEY_Setup @@@@\n", __func__, __LINE__);
		len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
		//printf("@@@@ In [%s][%d] call DRM_KEY_Setup len [%d], priv->suidx_size[%d] priv->suidx[%p]@@@@\n", __func__, __LINE__, len, priv->suidx_size, priv->suidx);
		do_video_decryption = 1;
		continue;
	}
	else
#endif /* end of SUPPORT_DIVX_DRM */
    len=stream_read_dword_le(demux->stream);
    if(stream_eof(demux->stream)) return 0; // EOF!

    if(id==mmioFOURCC('L','I','S','T') || id==mmioFOURCC('R', 'I', 'F', 'F')){
      id=stream_read_dword_le(demux->stream); // list or RIFF type
      continue;
    }
  }


  ds=demux_avi_select_stream(demux,id);
  if(ds)
    if(ds->packs+1>=MAX_PACKS || ds->bytes+len>=MAX_PACK_BYTES){
	// this packet will cause a buffer overflow, switch to -ni mode!!!
	mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_SwitchToNi);
	if(priv->idx_size>0){
	    // has index
	    demux->type=DEMUXER_TYPE_AVI_NI;
	    demux->desc=&demuxer_desc_avi_ni;
	    --priv->idx_pos; // hack
	} else {
	    // no index
	    demux->type=DEMUXER_TYPE_AVI_NINI;
	    demux->desc=&demuxer_desc_avi_nini;
	    priv->idx_pos=demux->filepos; // hack
	}
	priv->idx_pos_v=priv->idx_pos_a=priv->idx_pos;
	// quit now, we can't even (no enough buffer memory) read this packet :(
	return -1;
    }

  ret=demux_avi_read_packet(demux,ds,id,len,priv->idx_pos-1,flags);
#ifdef SUPPORT_DIVX_DRM   //Fuchun 2011.03.09
  if(dsds == demux->audio && priv->idx_size_a != -1 && (priv->idx_pos-1) > priv->idx_size_a)
  {
	  mp_msg(MSGT_DEMUX,MSGL_V,"[%s][%d]   idx_pos: %"PRId64" > idx_size_a: %d, detect audio eof\n", __func__, __LINE__, priv->idx_pos, priv->idx_size_a);
	  return 0;
  } 
#endif /* end of SUPPORT_DIVX_DRM */
} while(ret!=1);
  return 1;
}


// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_avi_fill_buffer_ni(demuxer_t *demux, demux_stream_t *ds)
{
avi_priv_t *priv=demux->priv;
unsigned int id=0;
unsigned int len;
int ret=0;

do{
  int flags=1;
  AVIINDEXENTRY *idx=NULL;
  int idx_pos=0;
  demux->filepos=stream_tell(demux->stream);

#if 1	//Fuchun 2009.12.03
	if(!FR_to_end && speed_mult < 0)
	{
		if(ds == demux->video) idx_pos = priv->idx_pos_v--;
		else if(ds == demux->audio) idx_pos = priv->idx_pos_a--;
#ifdef AVI_NOT_SORT
		else if(ds == demux->sub) idx_pos = priv->idx_pos_s--;
#endif
		else idx_pos = priv->idx_pos--;

		if(idx_pos < 0)
		{
#if 0
			demux->stream->eof = 1;
			return 0;
#else
//			speed_mult = 0;
			FR_to_end = 1;
			if(ds == demux->video) priv->idx_pos_v = 0;
			else if(ds == demux->audio) priv->idx_pos_a = 0;
#ifdef AVI_NOT_SORT
			else if(ds == demux->sub) priv->idx_pos_s = 0;
#endif
			else priv->idx_pos = 0;
			idx_pos = 0;
#endif
		}
	}
	else
	{
		if(ds == demux->video) idx_pos = priv->idx_pos_v++;
		else if(ds == demux->audio) idx_pos = priv->idx_pos_a++;
#ifdef AVI_NOT_SORT
		else if(ds == demux->sub) idx_pos = priv->idx_pos_s++;
#endif
		else idx_pos = priv->idx_pos++;
	}
#else 
  if(ds==demux->video) idx_pos=priv->idx_pos_v++; else
  if(ds==demux->audio) idx_pos=priv->idx_pos_a++; else
                       idx_pos=priv->idx_pos++;
#endif

#ifdef AVI_NOT_SORT
	if(priv->not_sort)
	{
		if((ds == demux->video && priv->suidx_size_v > 0 && idx_pos < priv->suidx_size_v)
			|| (ds == demux->audio && priv->suidx_size_a > 0 && idx_pos < priv->suidx_size_a)
			|| (ds == demux->sub && priv->suidx_size_s > 0 && idx_pos < priv->suidx_size_s))
		{
			off_t pos;
			if(ds == demux->video)
				idx=&((AVIINDEXENTRY *)priv->idx_v)[idx_pos];
			else if(ds == demux->audio)
				idx=&((AVIINDEXENTRY *)priv->idx_a)[idx_pos];
			else
				idx=&((AVIINDEXENTRY *)priv->idx_s)[idx_pos];

			if(idx->dwFlags&AVIIF_LIST)
			{
				if (!valid_stream_id(idx->ckid))
					// LIST
					continue;
				if (!priv->warned_unaligned)
					mp_msg(MSGT_DEMUX, MSGL_WARN, "Looks like unaligned chunk in index, broken AVI file!\n");
				priv->warned_unaligned = 1;
			}

			if(ds == demux->sub && priv->not_sort)
			{
				off_t v_offset, s_offset;
				v_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_v)[priv->idx_pos_v]);
				s_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_s)[idx_pos]);
				if(s_offset > v_offset)
				{
					priv->idx_pos_s--;
					return 0;
				}
			}

			if(ds && demux_avi_select_stream(demux,idx->ckid)!=ds)
			{
				mp_dbg(MSGT_DEMUX,MSGL_DBG3,"Skip chunk %.4s (0x%X)  \n",(char *)&idx->ckid,(unsigned int)idx->ckid);
				continue; // skip this chunk
			}

			pos = priv->idx_offset+AVI_IDX_OFFSET(idx);
			if((pos<demux->movi_start || pos>=demux->movi_end) && (demux->movi_end>demux->movi_start))
			{
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkOffset out of range!  current=0x%"PRIX64"  idx=0x%"PRIX64"  \n",(int64_t)demux->filepos,(int64_t)pos);
				continue;
			}

#if 1	//Fuchun 2009.12.03
			if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2))
			{
				if(!(idx->dwFlags & AVIIF_KEYFRAME))
					continue;
				else if(is_mjpeg == 0)
					mp_msg(MSGT_DEMUX,MSGL_V,"AVI_NI: speed_mult = %2d, idx_pos = %d\n", speed_mult, idx_pos);
			}
#endif
#ifdef STREAM_NI
			demux_switch_stream(demux, ds);
#endif
			stream_seek(demux->stream,pos);

			id=stream_read_dword_le(demux->stream);

#ifdef SUPPORT_DIVX_DRM
			if (id >> 16 == 0x6464 && DRM_Open > 0 && (speed_mult == 0 || speed_mult == 1 || priv->suidx_size > 0)) //xxdd skip for DRM
			{
				len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
				//printf("@@@@ In [%s][%d] call DRM_KEY_Setup len [%d], priv->suidx_size[%d] priv->suidx[%p]@@@@\n", __func__, __LINE__, len, priv->suidx_size, priv->suidx);
				do_video_decryption = 1;
				if (priv->suidx_size > 0)
				{
					id=stream_read_dword_le(demux->stream);
					//printf("#### In [%s][%d] id [%x] id >> 16[%x]idx->ckid[%x]dwChunkLength[%x]####\n", __func__, __LINE__, id, id >> 16, idx->ckid, idx->dwChunkLength);
					idx->ckid = id;
				}
				else
				{
					//printf("#### In [%s][%d] do continue id [%x] id >> 16[%x] suidx_sizei[%d]####\n", __func__, __LINE__, id, id >> 16, priv->suidx_size);
					continue;
				}
			}
#endif /* end of SUPPORT_DIVX_DRM */

			if(stream_eof(demux->stream)) return 0;

			if(id!=idx->ckid)
			{
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkID mismatch! raw=%.4s idx=%.4s  \n",(char *)&id,(char *)&idx->ckid);
				if(valid_fourcc(idx->ckid))
					id=idx->ckid;	// use index if valid
				else
				if(!valid_fourcc(id)) continue; // drop chunk if both id and idx bad
			}
			len=stream_read_dword_le(demux->stream);
#ifdef SUPPORT_DIVX_DRM
			if (do_video_decryption && priv->suidx_size > 0 && (id >> 16 ==0x6364 || id >> 16 == 0x6264) )
			{
				idx->dwChunkLength = len;
			}
#endif /* end of SUPPORT_DIVX_DRM*/
			if((len!=idx->dwChunkLength)&&((len+1)!=idx->dwChunkLength))
			{
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkSize mismatch! raw=%d idx=%d  \n",len,idx->dwChunkLength);
				if(len>0x200000 && idx->dwChunkLength>0x200000) continue; // both values bad :(
					len=choose_chunk_len(idx->dwChunkLength,len);
			}
			if(!(idx->dwFlags&AVIIF_KEYFRAME)) flags=0;
		}
		else
			return 0;
	}
	else
#endif
  if(priv->idx_size>0 && idx_pos<priv->idx_size){
    off_t pos;
    idx=&((AVIINDEXENTRY *)priv->idx)[idx_pos];

    if(idx->dwFlags&AVIIF_LIST){
      if (!valid_stream_id(idx->ckid))
      // LIST
      continue;
      if (!priv->warned_unaligned)
        mp_msg(MSGT_DEMUX, MSGL_WARN, "Looks like unaligned chunk in index, broken AVI file!\n");
      priv->warned_unaligned = 1;
    }
	
#ifdef AVI_NOT_SORT
	if(ds == demux->sub && priv->not_sort)
	{
		off_t v_offset, s_offset;
		v_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[priv->idx_pos_v]);
		s_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[idx_pos]);
		if(s_offset > v_offset)
		{
			priv->idx_pos_s--;
			return 0;
		}
	}
#endif

    if(ds && demux_avi_select_stream(demux,idx->ckid)!=ds){
      mp_dbg(MSGT_DEMUX,MSGL_DBG3,"Skip chunk %.4s (0x%X)  \n",(char *)&idx->ckid,(unsigned int)idx->ckid);
      continue; // skip this chunk
    }

    pos = priv->idx_offset+AVI_IDX_OFFSET(idx);
    if((pos<demux->movi_start || pos>=demux->movi_end) && (demux->movi_end>demux->movi_start)){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkOffset out of range!  current=0x%"PRIX64"  idx=0x%"PRIX64"  \n",(int64_t)demux->filepos,(int64_t)pos);
      continue;
    }
	
#if 1	//Fuchun 2009.12.03
	if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2))
	{
		if(!(idx->dwFlags & AVIIF_KEYFRAME))
			continue;
		else if(is_mjpeg == 0)
			mp_msg(MSGT_DEMUX,MSGL_V,"AVI_NI: speed_mult = %2d, idx_pos = %d\n", speed_mult, idx_pos);
	}
#endif
#ifdef STREAM_NI
    demux_switch_stream(demux, ds);
#endif
    stream_seek(demux->stream,pos);

    id=stream_read_dword_le(demux->stream);

#ifdef SUPPORT_DIVX_DRM
	if (id >> 16 == 0x6464 && DRM_Open > 0 && (speed_mult == 0 || speed_mult == 1 || priv->suidx_size > 0)) //xxdd skip for DRM
	{
		len = DRM_KEY_Setup(demux, DIVX_DECODE_VIDEO);
		//printf("@@@@ In [%s][%d] call DRM_KEY_Setup len [%d], priv->suidx_size[%d] priv->suidx[%p]@@@@\n", __func__, __LINE__, len, priv->suidx_size, priv->suidx);
		do_video_decryption = 1;
		if (priv->suidx_size > 0)
		{
			id=stream_read_dword_le(demux->stream);
			//printf("#### In [%s][%d] id [%x] id >> 16[%x]idx->ckid[%x]dwChunkLength[%x]####\n", __func__, __LINE__, id, id >> 16, idx->ckid, idx->dwChunkLength);
			idx->ckid = id;
		}
		else
		{
			//printf("#### In [%s][%d] do continue id [%x] id >> 16[%x] suidx_sizei[%d]####\n", __func__, __LINE__, id, id >> 16, priv->suidx_size);
			continue;
		}
	}
#endif /* end of SUPPORT_DIVX_DRM */

    if(stream_eof(demux->stream)) return 0;

    if(id!=idx->ckid){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkID mismatch! raw=%.4s idx=%.4s  \n",(char *)&id,(char *)&idx->ckid);
      if(valid_fourcc(idx->ckid))
          id=idx->ckid;	// use index if valid
      else
          if(!valid_fourcc(id)) continue; // drop chunk if both id and idx bad
    }
    len=stream_read_dword_le(demux->stream);
#ifdef SUPPORT_DIVX_DRM
	if (do_video_decryption && priv->suidx_size > 0 && (id >> 16 ==0x6364) )
	{
		idx->dwChunkLength = len;
	}
#endif /* end of SUPPORT_DIVX_DRM*/
    if((len!=idx->dwChunkLength)&&((len+1)!=idx->dwChunkLength)){
      mp_msg(MSGT_DEMUX,MSGL_V,"ChunkSize mismatch! raw=%d idx=%d  \n",len,idx->dwChunkLength);
      if(len>0x200000 && idx->dwChunkLength>0x200000) continue; // both values bad :(
      len=choose_chunk_len(idx->dwChunkLength,len);
    }
    if(!(idx->dwFlags&AVIIF_KEYFRAME)) flags=0;
  } else return 0;
  ret=demux_avi_read_packet(demux,demux_avi_select_stream(demux,id),id,len,idx_pos,flags);
} while(ret!=1);
  return 1;
}


// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_avi_fill_buffer_nini(demuxer_t *demux, demux_stream_t *ds)
{
avi_priv_t *priv=demux->priv;
unsigned int id=0;
unsigned int len;
int ret=0;
off_t *fpos=NULL;

  if(ds==demux->video) fpos=&priv->idx_pos_v; else
  if(ds==demux->audio) fpos=&priv->idx_pos_a; else
  return 0;

  stream_seek(demux->stream,fpos[0]);

do{

  demux->filepos=stream_tell(demux->stream);
  if(demux->filepos>=demux->movi_end && (demux->movi_end>demux->movi_start)){
	  ds->eof=1;
          return 0;
  }

  id=avi_find_id(demux->stream);
  len=stream_read_dword_le(demux->stream);

  if(stream_eof(demux->stream)) return 0;

  if(id==mmioFOURCC('L','I','S','T')){
      id=stream_read_dword_le(demux->stream);      // list type
      continue;
  }

  if(id==mmioFOURCC('R','I','F','F')){
      mp_msg(MSGT_DEMUX,MSGL_V,"additional RIFF header...\n");
      id=stream_read_dword_le(demux->stream);      // "AVIX"
      continue;
  }

  if(ds==demux_avi_select_stream(demux,id)){
    // read it!
    ret=demux_avi_read_packet(demux,ds,id,len,priv->idx_pos-1,0);
  } else {
    // skip it!
    int skip=(len+1)&(~1); // total bytes in this chunk
    stream_skip(demux->stream,skip);
  }

} while(ret!=1);
  fpos[0]=stream_tell(demux->stream);
  return 1;
}

// AVI demuxer parameters:
int index_mode=-1;  // -1=untouched  0=don't use index  1=use (generate) index
char *index_file_save = NULL, *index_file_load = NULL;
//int force_ni=1;     // force non-interleaved AVI parsing
int force_ni=0;	//Barry 2010-10-14  // force non-interleaved AVI parsing

static demuxer_t* demux_open_avi(demuxer_t* demuxer){
    demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=NULL;
    sh_video_t *sh_video=NULL;
    avi_priv_t* priv=calloc(1, sizeof(avi_priv_t));

  demuxer->priv=(void*)priv;

#ifdef AVI_NOT_SORT
	priv->not_sort = 0;
	demuxer->sub->non_interleaved = 0;
#endif

  if(packet_cnt) packet_cnt = 0;	//reset parameter Fuchun 2010.02.24
  avi_got_asf_header = 0;
  mjpeg_skip1 = 1;
  mjpeg_skip2 = 2;

  //Barry 2010-10-22
  avi_asfHeader = NULL;
  avi_asfHeaderSize = 0;
  no_need_asf_header = 0;

  //---- AVI header:
  read_avi_header(demuxer,(demuxer->stream->flags & MP_STREAM_SEEK_BW)?index_mode:-2);

#ifdef SUPPORT_DIVX_DRM
  if (demuxer->divx_drm_notsupport)
	  return demuxer;
  else
  {
	  //DRM_debug Barry 2010-11-21
	  if (DRM_Open > 0)
	  {
		  video_dd_chunk = malloc(sizeof(DRM_chunk));
		  if (video_dd_chunk)
			  memset(video_dd_chunk, 0, sizeof(DRM_chunk));
		  else
			  printf("####malloc video_dd_chunk failed ===\n");
	  }
  }
#endif /* end of SUPPORT_DIVX_DRM */


  if(demuxer->audio->id>=0 && !demuxer->a_streams[demuxer->audio->id]){
      mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_InvalidAudioStreamNosound,demuxer->audio->id);
      demuxer->audio->id=-2; // disabled
  }
  if(demuxer->video->id>=0 && !demuxer->v_streams[demuxer->video->id]){
      mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_InvalidAudioStreamUsingDefault,demuxer->video->id);
      demuxer->video->id=-1; // autodetect
  }
  
#ifdef AVI_SUBTITLES
   if(demuxer->sub->id>=0 && !demuxer->s_streams[demuxer->sub->id]){
      mp_msg(MSGT_DEMUX,MSGL_WARN,"@@@ Auto-detect subtitle id[%d] @@@\n",demuxer->sub->id);
      demuxer->sub->id=-1; // autodetect
  }
#endif

  stream_reset(demuxer->stream);
  stream_seek(demuxer->stream,demuxer->movi_start);
#ifdef AVI_NOT_SORT
	if(priv->not_sort)
	{

	}
	else
#endif
  if(priv->idx_size>1){
    // decide index format:
#if 1
    if((AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[0])<demuxer->movi_start ||
        AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[1])<demuxer->movi_start )&& !priv->isodml)
      priv->idx_offset=demuxer->movi_start-4;
#else
    if(AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[0])<demuxer->movi_start)
      priv->idx_offset=demuxer->movi_start-4;
#endif
    mp_msg(MSGT_DEMUX,MSGL_V,"AVI index offset: 0x%X (movi=0x%X idx0=0x%X idx1=0x%X)\n",
	    (int)priv->idx_offset,(int)demuxer->movi_start,
	    (int)((AVIINDEXENTRY *)priv->idx)[0].dwChunkOffset,
	    (int)((AVIINDEXENTRY *)priv->idx)[1].dwChunkOffset);
  }
#if 0	//Barry 2011-02-10 disable
  else	//Barry 2010-10-14
  	force_ni = 1;
#endif

#ifdef AVI_NOT_SORT
	if(priv->not_sort && (priv->suidx_size_v > 0 || priv->suidx_size_a > 0))
	{
		// check that file is non-interleaved:
		int i;
		off_t a_pos=-1;
		off_t v_pos=-1;
		for(i=0;i<priv->suidx_size_v;i++)
		{
			AVIINDEXENTRY* idx=&((AVIINDEXENTRY *)priv->idx_v)[i];
			demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
			off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
			if(v_pos==-1 && ds==demuxer->video)
			{
				v_pos=pos;
				break;
			}
		}
		
		for(i=0;i<priv->suidx_size_a;i++)
		{
			AVIINDEXENTRY* idx=&((AVIINDEXENTRY *)priv->idx_a)[i];
			demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
			off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
			if(a_pos==-1 && ds==demuxer->audio)
			{
				a_pos=pos;
				break;
			}
		}
#ifdef AVI_SUBTITLES
		off_t s_pos = -1;
		for (i = 0; i < MAX_S_STREAMS; i++)
		{
			if (demuxer->s_streams[i])
			{
				for(i=0;i<priv->suidx_size_s;i++)
				{
					AVIINDEXENTRY* idx=&((AVIINDEXENTRY *)priv->idx_s)[i];
					demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
					off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
					if(ds == demuxer->sub)
					{
						s_pos = pos;
						break;
					}
				}
				if(s_pos != -1)
				break;
			}
		}
#endif

		if(v_pos==-1)
		{
			mp_msg(MSGT_DEMUX,MSGL_ERR,"AVI_NI: " MSGTR_MissingVideoStream);
			return NULL;
		}
		if(a_pos==-1)
		{
			d_audio->sh=sh_audio=NULL;
		} 
		else 
		{
			if(force_ni || abs(a_pos-v_pos)>0x100000)	// distance > 1MB
			{  
				mp_msg(MSGT_DEMUX,MSGL_INFO,MSGTR_NI_Message,force_ni?MSGTR_NI_Forced:MSGTR_NI_Detected);
				demuxer->type=DEMUXER_TYPE_AVI_NI; // HACK!!!!
				demuxer->desc=&demuxer_desc_avi_ni; // HACK!!!!
				pts_from_bps=1; // force BPS sync!
			}
		}
	}
	else
#endif
  if(priv->idx_size>0){
      // check that file is non-interleaved:
      int i;
      off_t a_pos=-1;
      off_t v_pos=-1;
      for(i=0;i<priv->idx_size;i++){
        AVIINDEXENTRY* idx=&((AVIINDEXENTRY *)priv->idx)[i];
        demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
        off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
        if(a_pos==-1 && ds==demuxer->audio){
          a_pos=pos;
          if(v_pos!=-1) break;
        }
        if(v_pos==-1 && ds==demuxer->video){
          v_pos=pos;
          if(a_pos!=-1) break;
        }
      }

#ifdef AVI_SUBTITLES
	off_t s_pos = -1;
	for (i = 0; i < MAX_S_STREAMS; i++)
	{
		if (demuxer->s_streams[i])
		{
			for(i=0;i<priv->idx_size;i++)
			{
				AVIINDEXENTRY* idx=&((AVIINDEXENTRY *)priv->idx)[i];
				demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
				off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
				if(ds == demuxer->sub)
				{
					s_pos = pos;
					break;
				}
			}
			if(s_pos != -1)
				break;
		}
	}
#endif

      if(v_pos==-1){
        mp_msg(MSGT_DEMUX,MSGL_ERR,"AVI_NI: " MSGTR_MissingVideoStream);
	return NULL;
      }
      if(a_pos==-1){
        d_audio->sh=sh_audio=NULL;
      } else {
        if(force_ni || abs(a_pos-v_pos)>0x100000){  // distance > 1MB
          mp_msg(MSGT_DEMUX,MSGL_INFO,MSGTR_NI_Message,force_ni?MSGTR_NI_Forced:MSGTR_NI_Detected);
          demuxer->type=DEMUXER_TYPE_AVI_NI; // HACK!!!!
          demuxer->desc=&demuxer_desc_avi_ni; // HACK!!!!
	  pts_from_bps=1; // force BPS sync!
        }
      }
  } else {
      // no index
      if(force_ni){
          mp_msg(MSGT_DEMUX,MSGL_INFO,MSGTR_UsingNINI);
          demuxer->type=DEMUXER_TYPE_AVI_NINI; // HACK!!!!
          demuxer->desc=&demuxer_desc_avi_nini; // HACK!!!!
	  priv->idx_pos_a=
	  priv->idx_pos_v=demuxer->movi_start;
	  pts_from_bps=1; // force BPS sync!
      }
      demuxer->seekable=0;
  }
  if(!ds_fill_buffer(d_video)){
    mp_msg(MSGT_DEMUX,MSGL_ERR,"AVI: " MSGTR_MissingVideoStreamBug);
    return NULL;
  }
  sh_video=d_video->sh;sh_video->ds=d_video;
  if(d_audio->id!=-2){
    mp_msg(MSGT_DEMUX,MSGL_V,"AVI: Searching for audio stream (id:%d)\n",d_audio->id);
    if(!priv->audio_streams || !ds_fill_buffer(d_audio)){
      mp_msg(MSGT_DEMUX,MSGL_INFO,"AVI: " MSGTR_MissingAudioStream);
      d_audio->sh=sh_audio=NULL;
    } else {
      sh_audio=d_audio->sh;sh_audio->ds=d_audio;
    }
  }
#ifdef AVI_NOT_SORT
	if(priv->not_sort && (priv->suidx_size_v > 0 || priv->suidx_size_a > 0))
	{
		// we have index, let's count 'em!
		AVIINDEXENTRY *idx = priv->idx_v;
		int64_t vsize=0;
		int64_t asize=0;
		size_t vsamples=0;
		size_t asamples=0;
		int i;
		for(i=0;i<priv->suidx_size_v;i++)
		{
			int id=avi_stream_id(idx[i].ckid);
			unsigned len=idx[i].dwChunkLength;
			if(sh_video->ds->id == id) 
			{

				if ( !check_h264_sps && len && sh_video->bih && sh_video->bih->biCompression == mmioFOURCC('H', '2', '6', '4') )
				{
					off_t v_pos = priv->idx_offset + AVI_IDX_OFFSET(idx+i);
					char *check_h264_sps_buf = malloc(1024);
					int y=0;
					stream_seek(demuxer->stream, v_pos);
					stream_read(demuxer->stream, check_h264_sps_buf, 1024);
					for (y=0;y<1024;y++)
					{
						//if (check_h264_sps_buf[y]==0 && check_h264_sps_buf[y+1]==0 && check_h264_sps_buf[y+2]==0 && check_h264_sps_buf[y+3]==1 && check_h264_sps_buf[y+4]==0x67 )
						if (check_h264_sps_buf[y]==0 && check_h264_sps_buf[y+1]==0 && check_h264_sps_buf[y+2]==0 && check_h264_sps_buf[y+3]==1 && ((check_h264_sps_buf[y+4]&0x1F)==7) )	//Barry 2010-10-06
						{
							if (!check_avc1_sps_bank0(check_h264_sps_buf+(y+5), 1024-(y+5)))
							{
								not_supported_profile_level = 1;
								printf("AVI: H.264 Not supported, Profile=%d, Level=%d.%d\n", check_h264_sps_buf[y+5], check_h264_sps_buf[y+7]/10, check_h264_sps_buf[y+7]%10);
							}
							check_h264_sps++;
							break;
						}
					}
					free(check_h264_sps_buf);
					stream_reset(demuxer->stream);
					stream_seek(demuxer->stream,demuxer->movi_start);
				}
				else		//Barry 2010-06-10
				{
					//if (!vsamples && !is_mjpeg)
					if (!vsamples && !is_mjpeg && sh_video->bih &&
					(sh_video->bih->biCompression == mmioFOURCC('X', 'V', 'I', 'D')
					||sh_video->bih->biCompression == mmioFOURCC('x', 'v', 'i', 'd')
					||sh_video->bih->biCompression == mmioFOURCC('D', 'I', 'V', 'X')
					||sh_video->bih->biCompression == mmioFOURCC('d', 'i', 'v', 'x')
					||sh_video->bih->biCompression == mmioFOURCC('D', 'X', '5', '0')
					)
					)
					{
						off_t v_pos = priv->idx_offset + AVI_IDX_OFFSET(idx+i);
						char *check_mp4_vol_buf = malloc(1024);
						stream_seek(demuxer->stream, v_pos);
						stream_read(demuxer->stream, check_mp4_vol_buf, 1024);
						if ( !check_mp4_header_vol(check_mp4_vol_buf, 1024) )
						{
							not_supported_profile_level = 1;
							printf("AVI: GMC AND STATIC SPRITE CODING not supported\n");
						}
						free(check_mp4_vol_buf);
						stream_reset(demuxer->stream);
						stream_seek(demuxer->stream,demuxer->movi_start);
					}
				}

				vsize+=len;
				++vsamples;
			}
		}
		idx = priv->idx_a;
		for(i=0;i<priv->suidx_size_a;i++)
		{
			int id=avi_stream_id(idx[i].ckid);
			unsigned len=idx[i].dwChunkLength;
			if(sh_audio && sh_audio->ds->id == id) 
			{
				asize+=len;
				asamples+=(len+priv->audio_block_size-1)/priv->audio_block_size;
			}
		}
		mp_msg(MSGT_DEMUX,MSGL_V,"AVI video size=%"PRId64" (%u) audio size=%"PRId64" (%u)\n",vsize,vsamples,asize,asamples);
		priv->numberofframes=vsamples;
		sh_video->i_bps=((float)vsize/(float)vsamples)*(float)sh_video->video.dwRate/(float)sh_video->video.dwScale;
		if(sh_audio) sh_audio->i_bps=((float)asize/(float)asamples)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
	}
	else
#endif
  // calculating audio/video bitrate:
  if(priv->idx_size>0){
    // we have index, let's count 'em!
    AVIINDEXENTRY *idx = priv->idx;
    int64_t vsize=0;
    int64_t asize=0;
    size_t vsamples=0;
    size_t asamples=0;
    int i;
    for(i=0;i<priv->idx_size;i++){
      int id=avi_stream_id(idx[i].ckid);
      unsigned len=idx[i].dwChunkLength;
      if(sh_video->ds->id == id) {

	if ( !check_h264_sps && len && sh_video->bih && sh_video->bih->biCompression == mmioFOURCC('H', '2', '6', '4') )
	{
		off_t v_pos = priv->idx_offset + AVI_IDX_OFFSET(idx+i);
		char *check_h264_sps_buf = malloc(1024);
		int y=0;
		stream_seek(demuxer->stream, v_pos);
		stream_read(demuxer->stream, check_h264_sps_buf, 1024);
		for (y=0;y<1024;y++)
		{
			//if (check_h264_sps_buf[y]==0 && check_h264_sps_buf[y+1]==0 && check_h264_sps_buf[y+2]==0 && check_h264_sps_buf[y+3]==1 && check_h264_sps_buf[y+4]==0x67 )
			if (check_h264_sps_buf[y]==0 && check_h264_sps_buf[y+1]==0 && check_h264_sps_buf[y+2]==0 && check_h264_sps_buf[y+3]==1 && ((check_h264_sps_buf[y+4]&0x1F)==7) )	//Barry 2010-10-06
			{
				if (!check_avc1_sps_bank0(check_h264_sps_buf+(y+5), 1024-(y+5)))
				{
					not_supported_profile_level = 1;
					printf("AVI: H.264 Not supported, Profile=%d, Level=%d.%d\n", check_h264_sps_buf[y+5], check_h264_sps_buf[y+7]/10, check_h264_sps_buf[y+7]%10);
				}
				check_h264_sps++;
				break;
			}
		}
		free(check_h264_sps_buf);
		stream_reset(demuxer->stream);
		stream_seek(demuxer->stream,demuxer->movi_start);
	}
	else		//Barry 2010-06-10
	{
		//if (!vsamples && !is_mjpeg)
		if (!vsamples && !is_mjpeg && sh_video->bih &&
			(sh_video->bih->biCompression == mmioFOURCC('X', 'V', 'I', 'D')
			||sh_video->bih->biCompression == mmioFOURCC('x', 'v', 'i', 'd')
			||sh_video->bih->biCompression == mmioFOURCC('D', 'I', 'V', 'X')
			||sh_video->bih->biCompression == mmioFOURCC('d', 'i', 'v', 'x')
			||sh_video->bih->biCompression == mmioFOURCC('D', 'X', '5', '0')
			)
		)
		{
			off_t v_pos = priv->idx_offset + AVI_IDX_OFFSET(idx+i);
			char *check_mp4_vol_buf = malloc(1024);
			stream_seek(demuxer->stream, v_pos);
			stream_read(demuxer->stream, check_mp4_vol_buf, 1024);
			if ( !check_mp4_header_vol(check_mp4_vol_buf, 1024) )
			{
				not_supported_profile_level = 1;
				printf("AVI: GMC AND STATIC SPRITE CODING not supported\n");
			}
			free(check_mp4_vol_buf);
			stream_reset(demuxer->stream);
			stream_seek(demuxer->stream,demuxer->movi_start);
		}
	}

        vsize+=len;
        ++vsamples;
      }
      else if(sh_audio && sh_audio->ds->id == id) {
        asize+=len;
	asamples+=(len+priv->audio_block_size-1)/priv->audio_block_size;
      }
    }
    mp_msg(MSGT_DEMUX,MSGL_V,"AVI video size=%"PRId64" (%u) audio size=%"PRId64" (%u)\n",vsize,vsamples,asize,asamples);
    priv->numberofframes=vsamples;
    sh_video->i_bps=((float)vsize/(float)vsamples)*(float)sh_video->video.dwRate/(float)sh_video->video.dwScale;
    if(sh_audio) sh_audio->i_bps=((float)asize/(float)asamples)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
  } else {
    // guessing, results may be inaccurate:
    int64_t vsize;
    int64_t asize=0;
    off_t ori_pos;

	//Barry 2010-10-11 fix cats.avi
	if (!is_mjpeg && sh_video->bih &&
		(sh_video->bih->biCompression == mmioFOURCC('X', 'V', 'I', 'D')
		||sh_video->bih->biCompression == mmioFOURCC('x', 'v', 'i', 'd')
		||sh_video->bih->biCompression == mmioFOURCC('D', 'I', 'V', 'X')
		||sh_video->bih->biCompression == mmioFOURCC('d', 'i', 'v', 'x')
		||sh_video->bih->biCompression == mmioFOURCC('D', 'X', '5', '0')
		)
	)
	{
		ori_pos = stream_tell(demuxer->stream);
		char *check_mp4_vol_buf = malloc(1024);
		stream_seek(demuxer->stream, demuxer->movi_start);
		off_t y=0;
		int is_check = 1;

		while(1)	//Barry 2010-11-11
		{
			stream_read(demuxer->stream, check_mp4_vol_buf, 8);
			
			while(check_mp4_vol_buf[0] == 0x00)	//Fuchun 2011.03.29 skip garbage byte
			{
				stream_skip(demuxer->stream, 1-8);
				stream_read(demuxer->stream, check_mp4_vol_buf, 8);
			}
			
			if ( (check_mp4_vol_buf[2] == 0x64 && check_mp4_vol_buf[3] == 0x63) ||
				(check_mp4_vol_buf[2] == 0x64 && check_mp4_vol_buf[3] == 0x62)
			   )
			{
				stream_read(demuxer->stream, check_mp4_vol_buf+8, 1024-8);
				break;
			}
			else
			{
				y = (check_mp4_vol_buf[4] | (check_mp4_vol_buf[5]<<8) | (check_mp4_vol_buf[6]<<16) | (check_mp4_vol_buf[7]<<24));

				//Fuchun 2011.03.29 check the skip length
				if((y + stream_tell(demuxer->stream)) > demuxer->stream->end_pos || (y + stream_tell(demuxer->stream)) < demuxer->stream->start_pos)
				{
					printf("AVI: skip length[%"PRIx64"] is wrong, break and not check GMC\n", y);
					is_check = 0;
					break;
				}
				
				stream_skip(demuxer->stream, y);
			}
		}

		if (is_check == 1 && !check_mp4_header_vol(check_mp4_vol_buf, 1024) )
		{
			not_supported_profile_level = 1;
			printf("AVI: GMC AND STATIC SPRITE CODING not supported\n");
		}
		free(check_mp4_vol_buf);
		stream_reset(demuxer->stream);
		stream_seek(demuxer->stream, ori_pos);
	}
#if 1	//Barry 2010-11-09
	if ( !check_h264_sps && sh_video->bih && sh_video->bih->biCompression == mmioFOURCC('H', '2', '6', '4') )
	{
		ori_pos = stream_tell(demuxer->stream);
		char *check_h264_sps_buf = malloc(1024);
		int y=0;

		while(1)
		{
			stream_read(demuxer->stream, check_h264_sps_buf, 8);
			if ( (check_h264_sps_buf[2] == 0x64 && check_h264_sps_buf[3] == 0x63) ||
				(check_h264_sps_buf[3] == 0x64 && check_h264_sps_buf[4] == 0x63)
			   )
			{
				stream_read(demuxer->stream, check_h264_sps_buf+8, 1024-8);
				break;
			}//Polun 2011-08-19 ++s fixed mantis 5886
                     else if(check_h264_sps_buf[3] == 0x77 && check_h264_sps_buf[4] == 0x62)
			{
			       stream_read(demuxer->stream, check_h264_sps_buf +8, 1);
				y = (check_h264_sps_buf[5] | (check_h264_sps_buf[6]<<8) | (check_h264_sps_buf[7]<<16) | (check_h264_sps_buf[8]<<24));
                            stream_skip(demuxer->stream, y);
			}//Polun 2011-08-19 ++e 
			else 
			{
				y = (check_h264_sps_buf[4] | (check_h264_sps_buf[5]<<8) | (check_h264_sps_buf[6]<<16) | (check_h264_sps_buf[7]<<24));
                            stream_skip(demuxer->stream, y);
			}
		}

		for (y=0;y<1024;y++)
		{
			if (check_h264_sps_buf[y]==0 && check_h264_sps_buf[y+1]==0 && check_h264_sps_buf[y+2]==0 && check_h264_sps_buf[y+3]==1 && ((check_h264_sps_buf[y+4]&0x1F)==7) )
			{
				if (!check_avc1_sps_bank0(check_h264_sps_buf+(y+5), 1024-(y+5)))
				{
					not_supported_profile_level = 1;
					printf("AVI: H.264 Not supported, Profile=%d, Level=%d.%d\n", check_h264_sps_buf[y+5], check_h264_sps_buf[y+7]/10, check_h264_sps_buf[y+7]%10);
				}
				check_h264_sps++;
				break;
			}
		}
		free(check_h264_sps_buf);
		stream_reset(demuxer->stream);
		stream_seek(demuxer->stream, ori_pos);
	}
#endif	//Barry 2010-11-09 add

    if((priv->numberofframes=sh_video->video.dwLength)<=1)
      // bad video header, try to get number of frames from audio
      if(sh_audio && sh_audio->wf->nAvgBytesPerSec) priv->numberofframes=sh_video->fps*sh_audio->audio.dwLength/sh_audio->audio.dwRate*sh_audio->audio.dwScale;
    if(priv->numberofframes<=1){
      mp_msg(MSGT_SEEK,MSGL_WARN,MSGTR_CouldntDetFNo);
      priv->numberofframes=0;
    }

    if(sh_audio){
      if(sh_audio->wf->nAvgBytesPerSec && sh_audio->audio.dwSampleSize!=1){
        asize=(float)sh_audio->wf->nAvgBytesPerSec*sh_audio->audio.dwLength*sh_audio->audio.dwScale/sh_audio->audio.dwRate;
      } else {
        asize=sh_audio->audio.dwLength;
        sh_audio->i_bps=(float)asize/(sh_video->frametime*priv->numberofframes);
      }
    }
    vsize=demuxer->movi_end-demuxer->movi_start-asize-8*priv->numberofframes;
    mp_msg(MSGT_DEMUX,MSGL_V,"AVI video size=%"PRId64" (%u)  audio size=%"PRId64"\n",vsize,priv->numberofframes,asize);
    sh_video->i_bps=(float)vsize/(sh_video->frametime*priv->numberofframes);
  }

#if 1	//Barry 2010-10-22
	if ( !is_mjpeg && sh_video->bih && (sh_video->bih->biCompression == mmioFOURCC('W', 'M', 'V', '3')) )
	{
		unsigned char* wmv3StrC;
		avi_asfHeaderSize = 36;
		avi_asfHeader = malloc(avi_asfHeaderSize);
		wmv3StrC = ((uint8_t*)sh_video->bih) + sizeof(BITMAPINFOHEADER);

		PUT32_L(0x00000004, (unsigned char*)(avi_asfHeader+4));

		memcpy((unsigned char*)(avi_asfHeader+8), wmv3StrC, 4);

		PUT32_L(sh_video->bih->biHeight, (unsigned char*)(avi_asfHeader+12));
		PUT32_L(sh_video->bih->biWidth, (unsigned char*)(avi_asfHeader+16));
		PUT32_L(0x0000000C, (unsigned char*)(avi_asfHeader+20));
		PUT32_L(0x00000000, (unsigned char*)(avi_asfHeader+24));
		PUT32_L(0x00000000, (unsigned char*)(avi_asfHeader+28));
		PUT32_L(0x00000000, (unsigned char*)(avi_asfHeader+32));


		asfHeaderSize = avi_asfHeaderSize;
		asfHeader = avi_asfHeader; 
		*((int*)(asfHeader)) = 0xC5<<24 | 1;
	}
#endif
#ifdef SUPPORT_DIVX_DRM
	/* carlos 2011-03-17 add for divx certification, only set audio_output_channels to large than 2 when nChannels is large than 2 channels */
	{
		extern int audio_output_channels;
		extern unsigned int sky_hwac3;
#if 0
		printf("=-==========In [%s][%d] nChannels is [%d] audio_output_channels[%d]========\n", __func__, __LINE__, sh_audio->wf->nChannels, audio_output_channels);
		if (sh_audio && sh_audio->wf)
		{
			printf("=== In [%s][%d] wFormatTag [%d] nChannels [%d] nSamplesPerSec[%d] nAvgBytesPerSec[%d] nBlockAlign[%d] wBitsPerSample[%d] cbSize[%d] ==\n", __func__, __LINE__, sh_audio->wf->wFormatTag, sh_audio->wf->nChannels, sh_audio->wf->nSamplesPerSec, sh_audio->wf->nAvgBytesPerSec, sh_audio->wf->nBlockAlign, sh_audio->wf->wBitsPerSample, sh_audio->wf->cbSize);
		}
#endif
		if (sh_audio && sh_audio->wf && ((sh_audio->wf->nChannels == 2 ) || !sky_hwac3))
			audio_output_channels = 2;
	}
#endif /* end of SUPPORT_DIVX_DRM */
  return demuxer;

}


static void demux_seek_avi(demuxer_t *demuxer, float rel_seek_secs,
                           float audio_delay, int flags)
{
	avi_priv_t *priv=demuxer->priv;
	demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
#ifdef AVI_NOT_SORT
	demux_stream_t *d_sub=demuxer->sub;
	sh_sub_t *sh_sub=d_sub->sh;
#endif
	float skip_audio_secs=0;

	//FIXME: OFF_T - Didn't check AVI case yet (avi files can't be >2G anyway?)
	//================= seek in AVI ==========================
	int rel_seek_frames=rel_seek_secs*sh_video->fps;
	int video_chunk_pos=d_video->pos;
	int i;

	avi_got_asf_header = 0;	//Barry 2010-09-08

	if(flags&SEEK_ABSOLUTE)	// seek absolute
	{
		//Barry 2011-01-24
		if (sh_video && sh_video->format == mmioFOURCC('M','P','4','S') && !sh_video->mpeg4_header_len)
			sh_video->mpeg4_header_len = mp4s_hdr_len;

		video_chunk_pos=0;
	}
	if(flags&SEEK_FACTOR)
		rel_seek_frames=rel_seek_secs*priv->numberofframes;

	priv->skip_video_frames=0;
	priv->avi_audio_pts=0;

	// ------------ STEP 1: find nearest video keyframe chunk ------------
	// find nearest video keyframe chunk pos:
	if(rel_seek_frames>0)
	{
		// seek forward
#ifdef AVI_NOT_SORT
		if(priv->not_sort)
		{
			while(video_chunk_pos<priv->suidx_size_v-1)
			{
				int id=((AVIINDEXENTRY *)priv->idx_v)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
				if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
				if(avi_stream_id(id)==d_video->id)
#endif
				{  // video frame
					if((--rel_seek_frames)<0 && ((AVIINDEXENTRY *)priv->idx_v)[video_chunk_pos].dwFlags & AVIIF_KEYFRAME)
						break;
				}
				++video_chunk_pos;
			}
			//Barry 2010-10-01
			if ( video_chunk_pos == (priv->suidx_size_v-1) )
			{
				priv->idx_pos = video_chunk_pos;
				priv->idx_pos_v = video_chunk_pos;
				demuxer->stream->eof = 1;
				return;
			}
		}
		else
		{
			while(video_chunk_pos<priv->idx_size-1)
			{
				int id=((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
				if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
				if(avi_stream_id(id)==d_video->id)
#endif
				{  // video frame
					if((--rel_seek_frames)<0 && ((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].dwFlags & AVIIF_KEYFRAME)
						break;
				}
				++video_chunk_pos;
			}
			//Barry 2010-10-01
			if ( video_chunk_pos == (priv->idx_size-1) )
			{
				priv->idx_pos = video_chunk_pos;
				priv->idx_pos_v = video_chunk_pos;
				demuxer->stream->eof = 1;
				return;
			}
		}
#else
		while(video_chunk_pos<priv->idx_size-1)
		{
			int id=((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
			if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
			if(avi_stream_id(id)==d_video->id)
#endif /* end of SUPPORT_DIVX_DRM */			
			{  // video frame
				if((--rel_seek_frames)<0 && ((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].dwFlags & AVIIF_KEYFRAME)
					break;
			}
			++video_chunk_pos;
		}
		//Barry 2010-10-01
		//if ( video_chunk_pos == (priv->idx_size-1) )
		extern int no_osd;  //Polun 2011-07-19  for Cars_FullBitRate1080P_10min.avi can't preview
		if ( video_chunk_pos == (priv->idx_size-1) && !no_osd)
		{
			priv->idx_pos = video_chunk_pos;
			priv->idx_pos_v = video_chunk_pos;
			demuxer->stream->eof = 1;
			return;
		}
#endif
	}
	else
	{
		// seek backward
#ifdef AVI_NOT_SORT
		if(priv->not_sort)
		{
			while(video_chunk_pos>0)
			{
				int id=((AVIINDEXENTRY *)priv->idx_v)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
				if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
				if(avi_stream_id(id)==d_video->id)
#endif
				{  // video frame
					if((++rel_seek_frames)>0 && ((AVIINDEXENTRY *)priv->idx_v)[video_chunk_pos].dwFlags&AVIIF_KEYFRAME)
						break;
				}
				--video_chunk_pos;
			}
		}
		else
		{
			while(video_chunk_pos>0)
			{
				int id=((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
				if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
				if(avi_stream_id(id)==d_video->id)
#endif
				{  // video frame
					if((++rel_seek_frames)>0 && ((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].dwFlags&AVIIF_KEYFRAME)
						break;
				}
				--video_chunk_pos;
			}
		}
#else
		while(video_chunk_pos>0)
		{
			int id=((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].ckid;
#ifdef SUPPORT_DIVX_DRM
			if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0))
#else /* else of SUPPORT_DIVX_DRM */
			if(avi_stream_id(id)==d_video->id)
#endif /* end of SUPPORT_DIVX_DRM */			
			{  // video frame
				if((++rel_seek_frames)>0 && ((AVIINDEXENTRY *)priv->idx)[video_chunk_pos].dwFlags&AVIIF_KEYFRAME)
					break;
			}
			--video_chunk_pos;
		}
#endif
	}
#ifdef AVI_NOT_SORT
	if(priv->not_sort)
		priv->idx_pos_v=priv->idx_pos=video_chunk_pos;
	else
		priv->idx_pos_a=priv->idx_pos_v=priv->idx_pos_s=priv->idx_pos=video_chunk_pos;
	if(sh_sub && demuxer->type == DEMUXER_TYPE_AVI_NI && speed_mult == 0)
	{
		off_t v_offset, s_offset;
		int id;
		AVIINDEXENTRY *idx = NULL;
		if(priv->not_sort)
		{
			v_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_v)[priv->idx_pos_v]);
			
			
			for(i = 0; i < priv->suidx_size_s; i++)
			{
				s_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_s)[i]);
				if(s_offset > v_offset)
					break;
			}
			if(i < priv->suidx_size_s)
				priv->idx_pos_s = i;
		}
		else
		{
			v_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[priv->idx_pos_v]);
			for(i = 0; i < priv->idx_size; i++)
			{
				idx = &((AVIINDEXENTRY *)priv->idx)[i];
				id = avi_stream_id(idx->ckid);
				if((id -((int)priv->vid+(int)priv->aid)) >= 0 && (id -(priv->vid+priv->aid)) == d_sub->id)
				{
					s_offset = AVI_IDX_OFFSET(idx);
					if(s_offset > v_offset)
						break;
				}
			}
			if(i < priv->idx_size)
				priv->idx_pos_s = i;
				
		}
	}
#else
	priv->idx_pos_a=priv->idx_pos_v=priv->idx_pos=video_chunk_pos;
#endif

#ifdef SUPPORT_DIVX_DRM
	if (DRM_Open > 0 && priv->suidx_size == 0)
		video_chunk_pos--;
#endif /* end of SUPPORT_DIVX_DRM */

	// re-calc video pts:
	d_video->pack_no=0;
	for(i=0;i<video_chunk_pos;i++)
	{
#ifdef AVI_NOT_SORT
		int id;
		if(priv->not_sort)
			id = ((AVIINDEXENTRY *)priv->idx_v)[i].ckid;
		else
			id = ((AVIINDEXENTRY *)priv->idx)[i].ckid;
#else
		int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
#endif
		//printf("$$$$$ In [%s][%d] d_video->id [%d] avi_stream_id(id)[%d] id >> 16[%x] suidx_size[%d] i[%d] video_chunk_pos[%d]$$$$\n", __func__, __LINE__,d_video->id, avi_stream_id(id),  id >> 16, priv->suidx_size, i, video_chunk_pos);
#ifdef SUPPORT_DIVX_DRM		
		if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0)) 
#else /* else of SUPPORT_DIVX_DRM */		
		if(avi_stream_id(id)==d_video->id)
#endif /* end of SUPPORT_DIVX_DRM */		
			++d_video->pack_no;
	}
	priv->video_pack_no=sh_video->num_frames=sh_video->num_frames_decoded=d_video->pack_no;
	priv->avi_video_pts=d_video->pack_no*(float)sh_video->video.dwScale/(float)sh_video->video.dwRate;
	d_video->pos=video_chunk_pos;

	mp_msg(MSGT_SEEK,MSGL_DBG2,"V_SEEK:  pack=%d  pts=%5.3f  chunk=%d  \n",d_video->pack_no,priv->avi_video_pts,video_chunk_pos);

	// ------------ STEP 2: seek audio, find the right chunk & pos ------------
	d_audio->pack_no=0;
	priv->audio_block_no=0;
	d_audio->dpos=0;

#ifdef SUPPORT_DIVX_DRM		
	if(sh_audio && speed_mult == 0 && (priv->not_sort || (priv->idx_size_a != -1 && priv->idx_pos <= priv->idx_size_a)))
#else /* else of SUPPORT_DIVX_DRM */
	if(sh_audio && speed_mult == 0)
#endif /* end of SUPPORT_DIVX_DRM */
	{
		int i;
		int len=0;
		int skip_audio_bytes=0;
		int curr_audio_pos=-1;
		int audio_chunk_pos=-1;
#ifdef AVI_NOT_SORT
		int chunk_max=(demuxer->type==DEMUXER_TYPE_AVI)?video_chunk_pos:(priv->not_sort?priv->suidx_size_a:priv->idx_size);
#else
		int chunk_max=(demuxer->type==DEMUXER_TYPE_AVI)?video_chunk_pos:priv->idx_size;
#endif

		if(sh_audio->audio.dwSampleSize)
		{
			// constant rate audio stream
			/* immediate seeking to audio position, including when streams are delayed */
			curr_audio_pos=(priv->avi_video_pts + audio_delay)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
			curr_audio_pos*=sh_audio->audio.dwSampleSize;

			// find audio chunk pos:
			for(i=0;i<chunk_max;i++)
			{
#ifdef AVI_NOT_SORT
				int id;
				if(priv->not_sort)
					id=((AVIINDEXENTRY *)priv->idx_a)[i].ckid;
				else
					id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
#else
				int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
#endif
				if(avi_stream_id(id)==d_audio->id)
				{
#ifdef AVI_NOT_SORT
					if(priv->not_sort)
						len=((AVIINDEXENTRY *)priv->idx_a)[i].dwChunkLength;
					else
						len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
#else
					len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
#endif
					if(d_audio->dpos<=curr_audio_pos && curr_audio_pos<(d_audio->dpos+len))
						break;

					++d_audio->pack_no;
					priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
					d_audio->dpos+=len;
				}
			}
			audio_chunk_pos=i;
			skip_audio_bytes=curr_audio_pos-d_audio->dpos;

			mp_msg(MSGT_SEEK,MSGL_V,"SEEK: i=%d (max:%d) dpos=%d (wanted:%d)  \n", i,chunk_max,(int)d_audio->dpos,curr_audio_pos);

#if 1
			if(sh_audio->format == 0x55 && i == chunk_max && sh_audio->audio.dwSampleSize != 1)
			{
				printf("######## switch to VBR process ########\n");
				skip_audio_bytes = 0;
				curr_audio_pos=-1;
				
				d_audio->pack_no=0;
				priv->audio_block_no=0;
				d_audio->dpos=0;
				// VBR audio
				/* immediate seeking to audio position, including when streams are delayed */
				int chunks=(priv->avi_video_pts + audio_delay)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
				audio_chunk_pos=0;

				// find audio chunk pos:
#ifdef AVI_NOT_SORT
				if(priv->not_sort)
				{
					for(i=0;i<priv->suidx_size_a && chunks>0;i++)
					{
						int id=((AVIINDEXENTRY *)priv->idx_a)[i].ckid;
						if(avi_stream_id(id)==d_audio->id)
						{
							len=((AVIINDEXENTRY *)priv->idx_a)[i].dwChunkLength;
							if(i>chunk_max)
								skip_audio_bytes+=len;
							else
							{
								++d_audio->pack_no;
								priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
								d_audio->dpos+=len;
								audio_chunk_pos=i;
							}
							if(priv->audio_block_size)
								chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
						}
					}
				}
				else
				{
					for(i=0;i<priv->idx_size && chunks>0;i++)
					{
						int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
						if(avi_stream_id(id)==d_audio->id)
						{
							len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
							if(i>chunk_max)
								skip_audio_bytes+=len;
							else
							{
								++d_audio->pack_no;
								priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
								d_audio->dpos+=len;
								audio_chunk_pos=i;
							}
							if(priv->audio_block_size)
								chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
						}
					}
				}
#else
				for(i=0;i<priv->idx_size && chunks>0;i++)
				{
					int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
					if(avi_stream_id(id)==d_audio->id)
					{
						len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
						if(i>chunk_max)
							skip_audio_bytes+=len;
						else
						{
							++d_audio->pack_no;
							priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
							d_audio->dpos+=len;
							audio_chunk_pos=i;
						}
						if(priv->audio_block_size)
							chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
					}
				}
#endif
				sh_audio->audio.dwSampleSize = 0;
			}
#endif
		}
		else
		{
			// VBR audio
			/* immediate seeking to audio position, including when streams are delayed */
			int chunks=(priv->avi_video_pts + audio_delay)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
			audio_chunk_pos=0;

			// find audio chunk pos:
#ifdef AVI_NOT_SORT
			if(priv->not_sort)
			{
				for(i=0;i<priv->suidx_size_a && chunks>0;i++)
				{
					int id=((AVIINDEXENTRY *)priv->idx_a)[i].ckid;
					if(avi_stream_id(id)==d_audio->id)
					{
						len=((AVIINDEXENTRY *)priv->idx_a)[i].dwChunkLength;
						if(i>chunk_max)
							skip_audio_bytes+=len;
						else
						{
							++d_audio->pack_no;
							priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
							d_audio->dpos+=len;
							audio_chunk_pos=i;
						}
						if(priv->audio_block_size)
							chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
					}
				}
			}
			else
			{
				for(i=0;i<priv->idx_size && chunks>0;i++)
				{
					int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
					if(avi_stream_id(id)==d_audio->id)
					{
						len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
						if(i>chunk_max)
							skip_audio_bytes+=len;
						else
						{
							++d_audio->pack_no;
							priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
							d_audio->dpos+=len;
							audio_chunk_pos=i;
						}
						if(priv->audio_block_size)
							chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
					}
				}
			}
#else
			for(i=0;i<priv->idx_size && chunks>0;i++)
			{
				int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
				if(avi_stream_id(id)==d_audio->id)
				{
					len=((AVIINDEXENTRY *)priv->idx)[i].dwChunkLength;
					if(i>chunk_max)
						skip_audio_bytes+=len;
					else
					{
						++d_audio->pack_no;
						priv->audio_block_no+=priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
						d_audio->dpos+=len;
						audio_chunk_pos=i;
					}
					/* However, we need decrease chunks, even audio_block_size is zero */
					chunks-= priv->audio_block_size ? ((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
				}
			}
#endif
		}
		// Now we have:
		//      audio_chunk_pos = chunk no in index table (it's <=chunk_max)
		//      skip_audio_bytes = bytes to be skipped after chunk seek
		//      d-audio->pack_no = chunk_no in stream at audio_chunk_pos
		//      d_audio->dpos = bytepos in stream at audio_chunk_pos
		// let's seek!

		// update stream position:
		d_audio->pos=audio_chunk_pos;

		if(demuxer->type==DEMUXER_TYPE_AVI)
		{
			// interleaved stream:
			if(audio_chunk_pos<video_chunk_pos)
			{
				// calc priv->skip_video_frames & adjust video pts counter:
				for(i=audio_chunk_pos;i<video_chunk_pos;i++)
				{
					int id=((AVIINDEXENTRY *)priv->idx)[i].ckid;
#ifdef SUPPORT_DIVX_DRM
					if(avi_stream_id(id)==d_video->id && (((id >> 16) != 0x6464) || priv->suidx_size > 0)) 
#else /* else of SUPPORT_DIVX_DRM */
					if(avi_stream_id(id)==d_video->id)
#endif /* end of SUPPORT_DIVX_DRM */					
						++priv->skip_video_frames;
				}
				// requires for correct audio pts calculation (demuxer):
				priv->avi_video_pts-=priv->skip_video_frames*(float)sh_video->video.dwScale/(float)sh_video->video.dwRate;
				priv->avi_audio_pts=priv->avi_video_pts;
				// set index position:
				priv->idx_pos_a=priv->idx_pos_v=priv->idx_pos=audio_chunk_pos;
			}
		}
		else
		{
			// non-interleaved stream:
			priv->idx_pos_a=audio_chunk_pos;
			priv->idx_pos_v=video_chunk_pos;
			priv->idx_pos=(audio_chunk_pos<video_chunk_pos)?audio_chunk_pos:video_chunk_pos;
		}
		mp_msg(MSGT_SEEK,MSGL_V,"SEEK: idx=%d  (a:%d v:%d)  v.skip=%d  a.skip=%d/%4.3f  \n",
		(int)priv->idx_pos,audio_chunk_pos,video_chunk_pos,
		(int)priv->skip_video_frames,skip_audio_bytes,skip_audio_secs);

		if(skip_audio_bytes)
			demux_read_data(d_audio,NULL,skip_audio_bytes);
	}
	d_video->pts=priv->avi_video_pts; // OSD
}


static void demux_close_avi(demuxer_t *demuxer)
{
  avi_priv_t* priv=demuxer->priv;

#ifdef SUPPORT_DIVX_DRM
  if (video_dd_chunk)
  {
	  free(video_dd_chunk);
	  video_dd_chunk = NULL;
  }
#endif // end of SUPPORT_DIVX_DRM

  if(!priv)
    return;

  //Barry 2010-10-22
  if (avi_asfHeaderSize != 0 && avi_asfHeader)
  {
  	avi_asfHeaderSize = 0;
	free(avi_asfHeader);
  }

#ifdef AVI_NOT_SORT
	if(priv->not_sort)
	{
		avisuperindex_chunk *cx;
		int j;
		free(priv->suidx_id);
		free(priv->suidx_num);

		cx = &priv->suidx[0];
		do 
		{
			for (j=0;j<cx->nEntriesInUse;j++)
			if (cx->stdidx[j].nEntriesInUse) free(cx->stdidx[j].aIndex);
			free(cx->stdidx);
		} while (cx++ != &priv->suidx[priv->suidx_size-1]);
		free(priv->suidx);
	}
#endif

  if(priv->idx_size > 0)
    free(priv->idx);
#ifdef AVI_NOT_SORT
	if(priv->suidx_size_v)
		free(priv->idx_v);
	if(priv->suidx_size_a)
		free(priv->idx_a);
	if(priv->suidx_size_s)
		free(priv->idx_s);
#endif
  free(priv);
}


static int demux_avi_control(demuxer_t *demuxer,int cmd, void *arg){
    avi_priv_t *priv=demuxer->priv;
    demux_stream_t *d_video=demuxer->video;
    sh_video_t *sh_video=d_video->sh;

    switch(cmd) {
	case DEMUXER_CTRL_GET_TIME_LENGTH:
    	    if (!priv->numberofframes || !sh_video) return DEMUXER_CTRL_DONTKNOW;
	    *((double *)arg)=(double)priv->numberofframes/sh_video->fps;
	    if (sh_video->video.dwLength<=1) return DEMUXER_CTRL_GUESS;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_GET_PERCENT_POS:
    	    if (!priv->numberofframes || !sh_video) {
              return DEMUXER_CTRL_DONTKNOW;
	    }
	    *((int *)arg)=(int)(priv->video_pack_no*100/priv->numberofframes);
	    if (sh_video->video.dwLength<=1) return DEMUXER_CTRL_GUESS;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_SWITCH_AUDIO:
	case DEMUXER_CTRL_SWITCH_VIDEO: {
	    int audio = (cmd == DEMUXER_CTRL_SWITCH_AUDIO);
	    demux_stream_t *ds = audio ? demuxer->audio : demuxer->video;
	    void **streams = audio ? demuxer->a_streams : demuxer->v_streams;
	    int maxid = FFMIN(100, audio ? MAX_A_STREAMS : MAX_V_STREAMS);
	    int chunkid;

		//Fuchun 2011.02.28 reset audio_block_no
		uint32_t last_audio_scale;
		uint32_t last_audio_rate;
		float a_pts;
		if(audio && demuxer->audio->sh)
		{
			last_audio_scale = ((sh_audio_t*)demuxer->audio->sh)->audio.dwScale;
			last_audio_rate = ((sh_audio_t*)demuxer->audio->sh)->audio.dwRate;
			if (last_audio_rate)
				a_pts = priv->audio_block_no * (float)last_audio_scale / (float)last_audio_rate;
		}
		
	    if (ds->id < -1)
	      return DEMUXER_CTRL_NOTIMPL;

	    if (*(int *)arg >= 0)
	      ds->id = *(int *)arg;
	    else {
	      int i;
	      for (i = 0; i < maxid; i++) {
	        if (++ds->id >= maxid) ds->id = 0;
	        if (streams[ds->id]) break;
	      }
	    }

	    chunkid = (ds->id / 10 + '0') | (ds->id % 10 + '0') << 8;
	    ds->sh = NULL;
	    if (!streams[ds->id]) // stream not available
	      ds->id = -1;
	    else
	      demux_avi_select_stream(demuxer, chunkid);

		//Fuchun 2011.02.28 reset audio_block_no
		if(audio && ds->id != -1 && demuxer->audio->sh)
		{
			if (((sh_audio_t*)demuxer->audio->sh)->audio.dwScale)
				priv->audio_block_no = (off_t)(a_pts * (float)((sh_audio_t*)demuxer->audio->sh)->audio.dwRate / (float)((sh_audio_t*)demuxer->audio->sh)->audio.dwScale);
		}

#if 0
		if(priv->not_sort && ds->id != -1)
		{
			int id, i, j, num_index;
			off_t start_pos, end_pos, cur_pos, last_pos; 
			off_t last_offset, current_offset;
			uint32_t last_length, cur_length;
			int last_block_num, cur_block_num;
			off_t consume_size;
			last_pos = (audio ? priv->idx_pos_a : priv->idx_pos_v);
			for(i = 0; i < priv->suidx_size; i++)
			{
				start_pos = (i != 0 ? priv->suidx_num[i-1] : 0);
				end_pos = priv->suidx_num[i];
				if(last_pos >= start_pos && end_pos > last_pos)
				{
					num_index = i;
					break;
				}
			}
			
			for(i = 0; i < priv->suidx_size; i++)
			{
				id = avi_stream_id(priv->suidx_id[i]);
				if(id == ds->id)
					break;
			}

			if(i < priv->suidx_size)
			{
				AVIINDEXENTRY *idx = NULL;
				start_pos = (i != 0 ? priv->suidx_num[i-1] : 0);
				end_pos = priv->suidx_num[i];

				idx = &((AVIINDEXENTRY *)priv->idx)[last_pos];
				last_length = idx->dwChunkLength;
				idx = &((AVIINDEXENTRY *)priv->idx)[((end_pos-start_pos)/2+start_pos)];
				cur_length = idx->dwChunkLength;

				last_block_num = (int)(last_pos - (num_index != 0 ? priv->suidx_num[num_index-1] : 0));
				consume_size = (off_t)(last_block_num*last_length);

//printf("@@@@ last_block_num[%d]   last_pos[%"PRId64"]   num_index[%d]   suidx_num[%"PRId64"]   consume_size[0x%"PRIx64"]   last_length[%d] @@@@\n", last_block_num, last_pos, num_index, priv->suidx_num[i-1], consume_size, last_length);

				cur_block_num = (int)(consume_size/cur_length);
				cur_pos = start_pos+cur_block_num;
				if(cur_pos > end_pos)
					cur_pos = end_pos-1;

//printf("###### cur_pos[%"PRId64"]   start_pos[%"PRId64"]   cur_block_num[%d]   consume_size[%"PRId64"]   cur_length[%d] ####\n", cur_pos, start_pos, cur_block_num, consume_size, cur_length);

				last_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[last_pos]);
				current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[cur_pos]);

//printf("### cur_offset[%"PRIx64"]  last_offset[%"PRIx64"]    cur_pos[%"PRId64"]  last_pos[%"PRId64"]   start_pos[%"PRId64"]   end_pos[%"PRId64"] ###\n", current_offset, last_offset, cur_pos, last_pos, start_pos, end_pos);

				if(current_offset > last_offset)
				{
					for(j = cur_pos; j >= start_pos; j--)
					{
						current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[j]);
						if(current_offset < last_offset)
						{
							j++;
							break;
						}
					}
				}
				else
				{
					for(j = cur_pos; j < end_pos; j++)
					{
						current_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx)[j]);
						if(current_offset > last_offset)
						{
							break;
						}
					}
				}

				if(j < end_pos && j >= start_pos)
				{
					if(audio)
						priv->idx_pos_a = j;
					else
						priv->idx_pos_v = j;
				}
			}
		}
#endif
		
	    *(int *)arg = ds->id;
	    return DEMUXER_CTRL_OK;
	}

#ifdef AVI_NOT_SORT
	case DEMUXER_CTRL_SWITCH_SUB:
	{
		demux_stream_t *ds = demuxer->sub;
		if(priv->not_sort && ds->id != -1)
		{
			int id, i, j, ckid, idx_id;
			avisuperindex_chunk *cx;
			off_t v_offset, s_offset;
			AVIINDEXENTRY *idx = NULL;
			v_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_v)[priv->idx_pos_v]);
			for (cx = priv->suidx; cx != &priv->suidx[priv->suidx_size]; cx++)
			{
				ckid = ((cx->dwChunkId[3] << 24) | (cx->dwChunkId[2] << 16) | (cx->dwChunkId[1] << 8) | cx->dwChunkId[0]);
				idx_id = ckid >> 16;
				id = avi_stream_id(ckid);
				if((id -((int)priv->vid+(int)priv->aid)) >= 0 && (id -(priv->vid+priv->aid)) == ds->id && (idx_id == 0x6273 || idx_id == 0x7874))
				{
					if(priv->suidx_size_s > 0)
					{
						free(priv->idx_s);
						priv->idx_s = NULL;
						priv->suidx_size_s = 0;
					}
					avistdindex_chunk *sic;
					for (sic = cx->stdidx; sic != &cx->stdidx[cx->nEntriesInUse]; sic++) 
					{
						avistdindex_entry *sie;

						priv->idx_s = realloc(priv->idx_s, (priv->suidx_size_s+sic->nEntriesInUse)*sizeof (AVIINDEXENTRY));
						idx = priv->idx_s + (priv->suidx_size_s*sizeof (AVIINDEXENTRY));
						priv->suidx_size_s += sic->nEntriesInUse;
						
						for (sie = sic->aIndex, i=0; sie != &sic->aIndex[sic->nEntriesInUse]; sie++, i++) 
						{
							uint64_t off = sic->qwBaseOffset + sie->dwOffset - 8;
							memcpy(&idx->ckid, sic->dwChunkId, 4);
							idx->dwChunkOffset = off;
							idx->dwFlags = (off >> 32) << 16;
							idx->dwChunkLength = sie->dwSize & 0x7fffffff;
							idx->dwFlags |= (sie->dwSize&0x80000000)?0x0:AVIIF_KEYFRAME; // bit 31 denotes !keyframe
							idx++;
						}
					}
				}
			}
			
			for(i = 0; i < priv->suidx_size_s; i++)
			{
				s_offset = AVI_IDX_OFFSET(&((AVIINDEXENTRY *)priv->idx_s)[i]);
				if(s_offset > v_offset)
					break;
			}
			if(i < priv->suidx_size_s)
				priv->idx_pos_s = i;
		}

		return DEMUXER_CTRL_OK;
	}
#endif

	default:
	    return DEMUXER_CTRL_NOTIMPL;
    }
}


static int avi_check_file(demuxer_t *demuxer)
{
  int id=stream_read_dword_le(demuxer->stream); // "RIFF"

  if((id==mmioFOURCC('R','I','F','F')) || (id==mmioFOURCC('O','N','2',' '))) {
    stream_read_dword_le(demuxer->stream); //filesize
    id=stream_read_dword_le(demuxer->stream); // "AVI "
    if(id==formtypeAVI)
      return DEMUXER_TYPE_AVI;
    // "Samsung Digimax i6 PMP" crap according to bug 742
    if(id==mmioFOURCC('A','V','I',0x19))
      return DEMUXER_TYPE_AVI;
    if(id==mmioFOURCC('O','N','2','f')){
      mp_msg(MSGT_DEMUXER,MSGL_INFO,MSGTR_ON2AviFormat);
      return DEMUXER_TYPE_AVI;
    }
  }

  return 0;
}


static demuxer_t* demux_open_hack_avi(demuxer_t *demuxer)
{
   sh_audio_t* sh_a;

   demuxer = demux_open_avi(demuxer);
   if(!demuxer) return NULL; // failed to open
   sh_a = demuxer->audio->sh;
   if(demuxer->audio->id != -2 && sh_a) {
#ifdef CONFIG_OGGVORBIS
    // support for Ogg-in-AVI:
    if(sh_a->format == 0xFFFE)
    {
           printf("sh_a->format == 0xFFFE is PCM formate not ogg\n");    
          //demuxer = init_avi_with_ogg(demuxer);
    }
    else if(sh_a->format == 0x674F) {
      stream_t* s;
      demuxer_t  *od;
      s = new_ds_stream(demuxer->audio);
      od = new_demuxer(s,DEMUXER_TYPE_OGG,-1,-2,-2,NULL);
      if(!demux_ogg_open(od)) {
        mp_msg( MSGT_DEMUXER,MSGL_ERR,MSGTR_ErrorOpeningOGGDemuxer);
        free_stream(s);
        demuxer->audio->id = -2;
      } else
        demuxer = new_demuxers_demuxer(demuxer,od,demuxer);
   }
#endif
   }

   return demuxer;
}


const demuxer_desc_t demuxer_desc_avi = {
  "AVI demuxer",
  "avi",
  "AVI",
  "Arpi?",
  "AVI files, including non interleaved files",
  DEMUXER_TYPE_AVI,
  1, // safe autodetect
  avi_check_file,
  demux_avi_fill_buffer,
  demux_open_hack_avi,
  demux_close_avi,
  demux_seek_avi,
  demux_avi_control
};

const demuxer_desc_t demuxer_desc_avi_ni = {
  "AVI demuxer, non-interleaved",
  "avini",
  "AVI",
  "Arpi?",
  "AVI files, including non interleaved files",
  DEMUXER_TYPE_AVI,
  1, // safe autodetect
  avi_check_file,
  demux_avi_fill_buffer_ni,
  demux_open_hack_avi,
  demux_close_avi,
  demux_seek_avi,
  demux_avi_control
};

const demuxer_desc_t demuxer_desc_avi_nini = {
  "AVI demuxer, non-interleaved and no index",
  "avinini",
  "AVI",
  "Arpi?",
  "AVI files, including non interleaved files",
  DEMUXER_TYPE_AVI,
  1, // safe autodetect
  avi_check_file,
  demux_avi_fill_buffer_nini,
  demux_open_hack_avi,
  demux_close_avi,
  demux_seek_avi,
  demux_avi_control
};

#ifdef SUPPORT_DIVX_DRM
static inline int DRM_KEY_Setup(demuxer_t *demux, char type)
{
   int len = 0;
   DRM_chunk *dd_chunk = NULL;
   len = stream_read_dword_le(demux->stream);

   if (type == DIVX_DECODE_VIDEO)
       dd_chunk = video_dd_chunk;
   else
       dd_chunk = audio_dd_chunk;

   if (dd_chunk && len == 10)
   {
       dd_chunk->KeyIdx = stream_read_word_le(demux->stream);
       dd_chunk->EncryptionOffset = stream_read_dword_le(demux->stream);
       dd_chunk->EncryptionLength = stream_read_dword_le(demux->stream);
       //mplayer_debug("========== In [%s][%d] set dd_chunk, KeyIdx [%d] EncryptionOffset[%d] EncryptionLength[%d] ======\n", __func__, __LINE__, dd_chunk->KeyIdx, dd_chunk->EncryptionOffset, dd_chunk->EncryptionLength);
   }
   else
   {
       mplayer_debug("### In [%s][%d] dd_chunk is NULL type is [%d] len [%x]####\n", __func__, __LINE__, type, len);
   }
   return len;
}
#endif // end of SUPPORT_DIVX_DRM
