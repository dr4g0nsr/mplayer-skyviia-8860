/*
 * ASF file parser for DEMUXER v0.3
 * copyright (c) 2001 A'rpi/ESP-team
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
#include <limits.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "asf.h"
#include "asfheader.h"
#include "demuxer.h"
#include "libmpcodecs/dec_audio.h"
#include "libvo/fastmemcpy.h"
#include "libavutil/intreadwrite.h"

typedef struct //added by ethan 090521
{
	int timeStamp;
	struct ASFTime* next;
	
}ASFTime;

#if 1	// Raymond 2009/05/31
ASFTime* asfTimeHead = NULL;//ethan 090521

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
	( (long)(unsigned char)(ch0) | ( (long)(unsigned char)(ch1) << 8 ) |		\
	( (long)(unsigned char)(ch2) << 16 ) | ( (long)(unsigned char)(ch3) << 24 ) )
#endif

extern unsigned int fourCC;
extern unsigned long long playPreRoll;
extern unsigned char* asfHeader;
extern unsigned int asfHeaderSize;
extern unsigned int GotASFHeader;	// Raymond 2010/05/26
extern unsigned long long playDuration;
extern unsigned long long avgFrTime;
extern int asf_exter_data ;//Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC.
int asf_first_start_code = 1; //Polun 2011-08-11 add asf_first_start_code for mantis 5680
int asf_FR_to_end = 0 ;
int VC1_has_framePrefix = 0;	//Barry 2011-12-29
    
//Fuchun 2010.03.05
off_t FB_last_pos;		
int FB_read_key;
int FB_read_nonkey;
unsigned int keyframe_objnum;
#endif

// based on asf file-format doc by Eugene [http://divx.euro.ru]

/**
 * \brief reads int stored in number of bytes given by len
 * \param ptr pointer to read from, is incremented appropriately
 * \param len lowest 2 bits indicate number of bytes to read
 * \param def default value to return if len is invalid
 */
static inline unsigned read_varlen(uint8_t **ptr, int len, int def) {
    const uint8_t *p = *ptr;
    len &= 3;
    switch (len) {
      case 1: *ptr += 1; return *p;
      case 2: *ptr += 2; return AV_RL16(p);
      case 3: *ptr += 4; return AV_RL32(p);
    }
    return def;
}

/**
 * \brief checks if there is enough data to read the bytes given by len
 * \param ptr pointer to read from
 * \param endptr pointer to the end of the buffer
 * \param len lowest 2 bits indicate number of bytes to read
 */
static inline int check_varlen(uint8_t *ptr, uint8_t *endptr, int len) {
    return len&3 ? ptr + (1<<((len&3) - 1)) <= endptr : 1;
}

static void asf_descrambling(unsigned char **src,unsigned len, struct asf_priv* asf){
  unsigned char *dst;
  unsigned char *s2=*src;
  unsigned i=0,x,y;
  if (len > UINT_MAX - MP_INPUT_BUFFER_PADDING_SIZE)
	return;
  dst = malloc(len + MP_INPUT_BUFFER_PADDING_SIZE);
  while(len>=asf->scrambling_h*asf->scrambling_w*asf->scrambling_b+i){
//    mp_msg(MSGT_DEMUX,MSGL_DBG4,"descrambling! (w=%d  b=%d)\n",w,asf_scrambling_b);
	//i+=asf_scrambling_h*asf_scrambling_w;
	for(x=0;x<asf->scrambling_w;x++)
	  for(y=0;y<asf->scrambling_h;y++){
	    fast_memcpy(dst+i,s2+(y*asf->scrambling_w+x)*asf->scrambling_b,asf->scrambling_b);
		i+=asf->scrambling_b;
	  }
	s2+=asf->scrambling_h*asf->scrambling_w*asf->scrambling_b;
  }
  //if(i<len) fast_memcpy(dst+i,src+i,len-i);
  free(*src);
  *src = dst;
}

/*****************************************************************
 * \brief initializes asf private data
 *
 */
static void init_priv (struct asf_priv* asf){
  asf->last_vid_seq=-1;
  asf->vid_ext_timing_index=-1;
  asf->aud_ext_timing_index=-1;
  asf->vid_ext_frame_index=-1;
}

static void demux_asf_append_to_packet(demux_packet_t* dp,unsigned char *data,int len,int offs)
{
  if(dp->len!=offs && offs!=-1) mp_msg(MSGT_DEMUX,MSGL_V,"warning! fragment.len=%d BUT next fragment offset=%d  \n",dp->len,offs);
  dp->buffer=realloc(dp->buffer,dp->len+len+MP_INPUT_BUFFER_PADDING_SIZE);
  fast_memcpy(dp->buffer+dp->len,data,len);
  memset(dp->buffer+dp->len+len, 0, MP_INPUT_BUFFER_PADDING_SIZE);
  mp_dbg(MSGT_DEMUX,MSGL_DBG4,"data appended! %d+%d\n",dp->len,len);
  dp->len+=len;
}

static int demux_asf_read_packet(demuxer_t *demux,unsigned char *data,int len,int id,int seq,uint64_t time,unsigned short dur,int offs,int keyframe){
  struct asf_priv* asf = demux->priv;
  demux_stream_t *ds=NULL;
  int close_seg=0;

  mp_dbg(MSGT_DEMUX,MSGL_DBG4,"demux_asf.read_packet: id=%d seq=%d len=%d\n",id,seq,len);
//printf("demux_asf.read_packet: id=%d seq=%d len=%d\n",id,seq,len);
  if(demux->video->id==-1)
    if(demux->v_streams[id])
        demux->video->id=id;

  if(demux->audio->id==-1)
    if(demux->a_streams[id])
        demux->audio->id=id;

  if(id==demux->audio->id){
      // audio
      ds=demux->audio;
      if(!ds->sh){
        ds->sh=demux->a_streams[id];
        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected ASF audio ID = %d\n",ds->id);
      }
  } else
  if(id==demux->video->id){
      // video
      ds=demux->video;
      if(!ds->sh){
        ds->sh=demux->v_streams[id];
        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected ASF video ID = %d\n",ds->id);
      }
  }

  if(ds){
    if(ds->asf_packet){
      demux_packet_t* dp=ds->asf_packet;

      if (ds==demux->video && asf->asf_is_dvr_ms) {
        if (asf->new_vid_frame_seg) {
          dp->pos=demux->filepos;
          close_seg = 1;
        } else seq = ds->asf_seq;
      } else close_seg = ds->asf_seq!=seq;

      if(close_seg){
        // closed segment, finalize packet:
		if(ds==demux->audio)
		  if(asf->scrambling_h>1 && asf->scrambling_w>1 && asf->scrambling_b>0)
		    asf_descrambling(&ds->asf_packet->buffer,ds->asf_packet->len,asf);
        ds_add_packet(ds,ds->asf_packet);
        ds->asf_packet=NULL;
      } else {
        // append data to it!
        demux_asf_append_to_packet(dp,data,len,offs);
        // we are ready now.
        return 1;
      }
    }
    // create new packet:
    { demux_packet_t* dp;
      if(offs>0){
        mp_msg(MSGT_DEMUX,MSGL_V,"warning!  broken fragment, %d bytes missing  \n",offs);
        return 0;
      }

#if 1	// Raymond 2009/05/31
if(id==demux->video->id)
{ 	
      //ethan 090525-------------------ADD FILE HEADER-----------------------------------------
	if(fourCC == mmioFOURCC('W','M','V','3'))
	{	
		{
			len+=8;
			dp=new_demux_packet(len);
			fast_memcpy((unsigned char*)(dp->buffer+8),data,len-8);

			// Raymond 2010/06/03
			if(keyframe == 1)
				dp->buffer[3] = 0x80;
			else
				dp->buffer[3] = 0;
		}
	}
	else if(fourCC ==mmioFOURCC('W','V','C','1'))
	{
#if 1	//Barry 2011-12-29
              //if(GotASFHeader==2) //Polun 2011-08-12 add seek check for mantis 5680
              if(GotASFHeader==2 || ( GotASFHeader==3 && keyframe == 1))
		{
			if (data[0]==0 && data[1]==0 && data[2]==1 && data[3]==0x0F)
			{
				int i;
				for (i=0; (i<256) && (i<(len-4)); i++)
				{
					if (data[i]==0 && data[i+1]==0 && data[i+2]==1 && data[i+3]==0x0D)
					{
						VC1_has_framePrefix = 1;
						GotASFHeader = 0;
						break;
					}
				}
			}
			if (VC1_has_framePrefix == 1)
			{
				dp=new_demux_packet(len);
				fast_memcpy((unsigned char*)(dp->buffer),data,len);
			}
			else
			{
				len = len + asfHeaderSize + 4; 
		  		dp=new_demux_packet(len);
				fast_memcpy((unsigned char*)(dp->buffer+asfHeaderSize+4),data,len-asfHeaderSize-4);
				fast_memcpy(dp->buffer, asfHeader, asfHeaderSize);

	                     if (GotASFHeader == 2)//Polun 2011-08-15 ++s add asf_first_start_code for mantis 5680 WVC1
				    GotASFHeader = 1;	// Raymond 2010/05/26
			}
		}
		else
		{
			if (VC1_has_framePrefix == 1)
			{
				dp=new_demux_packet(len);
				fast_memcpy((unsigned char*)(dp->buffer),data,len);
			}
			else
			{
				len+=4;
				dp=new_demux_packet(len);
				fast_memcpy((unsigned char*)(dp->buffer+4),data,len-4);
			}
		}
#else
              //if(GotASFHeader==2) //Polun 2011-08-12 add seek check for mantis 5680
              if(GotASFHeader==2 || ( GotASFHeader==3 && keyframe == 1))
		{
			len = len + asfHeaderSize + 4; 
	  		dp=new_demux_packet(len);
			fast_memcpy((unsigned char*)(dp->buffer+asfHeaderSize+4),data,len-asfHeaderSize-4);
			fast_memcpy(dp->buffer, asfHeader, asfHeaderSize);

                     if (GotASFHeader == 2)//Polun 2011-08-15 ++s add asf_first_start_code for mantis 5680 WVC1
			    GotASFHeader = 1;	// Raymond 2010/05/26
		}
		else
		{
			len+=4;
			dp=new_demux_packet(len);
			fast_memcpy((unsigned char*)(dp->buffer+4),data,len-4);
		}
#endif
	}  
	else
	//ethan 090525------------------------------------------------------------	

	//Fuchun 2010.04.22
	if(fourCC ==mmioFOURCC('M','4','S','2') || fourCC == mmioFOURCC('M','P','4','S'))
	{
		sh_video_t *sh_video = demux->video->sh;
        	//if(sh_video->mpeg4_header_len != 0)  //Polun 2011-08-11 add asf_first_start_code for mantis 5680
		if( asf_first_start_code && sh_video->mpeg4_header_len != 0)
		{
			len += sh_video->mpeg4_header_len;
			dp=new_demux_packet(len);
			fast_memcpy(dp->buffer, sh_video->mpeg4_header, sh_video->mpeg4_header_len);
			fast_memcpy((unsigned char*)(dp->buffer+sh_video->mpeg4_header_len), data, len-sh_video->mpeg4_header_len);
			//sh_video->mpeg4_header_len = 0;      //Polun 2011-08-11 remove for mantis 5680
		}
		else
		{
			dp=new_demux_packet(len);
	  		fast_memcpy((unsigned char*)(dp->buffer),data,len);
		}
	}
	else
#endif	
	{
		dp=new_demux_packet(len);
	  	fast_memcpy((unsigned char*)(dp->buffer),data,len);
	}
}
else
{
	dp=new_demux_packet(len);
	fast_memcpy((unsigned char*)(dp->buffer),data,len);
}

      if (asf->asf_is_dvr_ms)
        dp->pts=time*0.0000001;
      else
        dp->pts=time*0.001;
      dp->flags=keyframe;
//      if(ds==demux->video) printf("ASF time: %8d  dur: %5d  \n",time,dur);
      dp->pos=demux->filepos;
      ds->asf_packet=dp;
      ds->asf_seq=seq;
      // we are ready now.
      return 1;
    }
  }

  return 0;
}

/*****************************************************************
 * \brief read the replicated data associated with each segment
 * \parameter pp reference to replicated data
 * \parameter id stream number
 * \parameter seq media object number
 * \parameter keyframe key frame indicator - set to zero if keyframe, non-zero otherwise
 * \parameter seg_time set to payload time when valid, if audio or new video frame payload, zero otherwise
 *
 */
static void get_payload_extension_data(demuxer_t *demux, unsigned char** pp, unsigned char id, unsigned int seq, int *keyframe, uint64_t *seg_time){
    struct asf_priv* asf = demux->priv;
    uint64_t payload_time; //100ns units
    int i, ext_max, ext_timing_index;
    uint8_t *pi = *pp+4;

    if(demux->video->id==-1)
        if(demux->v_streams[id])
            demux->video->id=id;

    if(demux->audio->id==-1)
        if(demux->a_streams[id])
            demux->audio->id=id;

    if (id!=demux->video->id && id!=demux->audio->id) return;

    if (id==demux->video->id) {
      ext_max = asf->vid_repdata_count;
      ext_timing_index = asf->vid_ext_timing_index;
    } else {
      ext_max = asf->aud_repdata_count;
      ext_timing_index = asf->aud_ext_timing_index;
    }

    *seg_time=0.0;
    asf->new_vid_frame_seg = 0;

    for (i=0; i<ext_max; i++) {
        uint16_t payextsize;
        uint8_t segment_marker;

        if (id==demux->video->id)
            payextsize = asf->vid_repdata_sizes[i];
        else
            payextsize = asf->aud_repdata_sizes[i];

        if (payextsize == 65535) {
            payextsize = AV_RL16(pi);
            pi+=2;
        }

        // if this is the timing info extension then read the payload time
        if (i == ext_timing_index)
            payload_time = AV_RL64(pi+8);

        // if this is the video frame info extension then
        // set the keyframe indicator, the 'new frame segment' indicator
        // and (initially) the 'frame time'
        if (i == asf->vid_ext_frame_index && id==demux->video->id) {
            segment_marker = pi[0];
            // Known video stream segment_marker values that
            // contain useful information:
            //
            // NTSC/ATSC (29.97fps):        0X4A 01001010
            //                              0X4B 01001011
            //                              0X49 01001001
            //
            // PAL/ATSC (25fps):            0X3A 00111010
            //                              0X3B 00111011
            //                              0X39 00111001
            //
            // ATSC progressive (29.97fps): 0X7A 01111010
            //                              0X7B 01111011
            //                              0X79 01111001
            //   11111111
            //       ^    this is new video frame marker
            //
            //   ^^^^     these bits indicate the framerate
            //            0X4 is 29.97i, 0X3 is 25i, 0X7 is 29.97p, ???=25p
            //
            //        ^^^ these bits indicate the frame type:
            //              001 means I-frame
            //              010 and 011 probably mean P and B

            asf->new_vid_frame_seg = (0X08 & segment_marker) && seq != asf->last_vid_seq;

            if (asf->new_vid_frame_seg) asf->last_vid_seq = seq;

            if (asf->avg_vid_frame_time == 0) {
                // set the average frame time initially (in 100ns units).
                // This is based on what works for known samples.
                // It can be extended if more samples of different types can be obtained.
                if (((segment_marker & 0XF0) >> 4) == 4) {
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 30.0) * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 3) {
                    asf->avg_vid_frame_time = (uint64_t)(0.04 * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 6) {
                    asf->avg_vid_frame_time = (uint64_t)(0.02 * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 7) {
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 60.0) * 10000000.0);
                    asf->know_frame_time=1;
                } else {
                    // we dont know the frame time initially so
                    // make a guess and then recalculate as we go.
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 60.0) * 10000000.0);
                    asf->know_frame_time=0;
                }
            }
            *keyframe = (asf->new_vid_frame_seg && (segment_marker & 0X07) == 1);
        }
        pi +=payextsize;
    }

    if (id==demux->video->id && asf->new_vid_frame_seg) {
        asf->vid_frame_ct++;
        // Some samples only have timings on key frames and
        // the rest contain non-cronological timestamps. Interpolating
        // the values between key frames works for all samples.
        if (*keyframe) {
            asf->found_first_key_frame=1;
            if (!asf->know_frame_time && asf->last_key_payload_time > 0) {
                // We dont know average frametime so recalculate.
                // Giving precedence to the 'weight' of the existing
                // average limits damage done to new value when there is
                // a sudden time jump which happens occasionally.
                asf->avg_vid_frame_time =
                   (0.9 * asf->avg_vid_frame_time) +
                   (0.1 * ((payload_time - asf->last_key_payload_time) / asf->vid_frame_ct));
            }
            asf->last_key_payload_time = payload_time;
            asf->vid_frame_ct = 1;
            *seg_time = payload_time;
        } else
            *seg_time = (asf->last_key_payload_time  + (asf->avg_vid_frame_time * (asf->vid_frame_ct-1)));
    }

    if (id==demux->audio->id) {
        if (payload_time != -1)
            asf->last_aud_diff = payload_time - asf->last_aud_pts;
        asf->last_aud_pts += asf->last_aud_diff;
        *seg_time = asf->last_aud_pts;
   }
}
//static int num_elementary_packets100=0;
//static int num_elementary_packets101=0;

// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_asf_fill_buffer(demuxer_t *demux, demux_stream_t *ds){
  struct asf_priv* asf = demux->priv;
//printf("--- demux_asf_fill_buffer()\n");
  demux->filepos=stream_tell(demux->stream);
//printf(" demux->filepos =%d demux->movi_start = %d\n", (int)demux->filepos, (int)demux->movi_start);
  // Brodcast stream have movi_start==movi_end
  // Better test ?

  static int found_keyframe = 0;
  static int forward_keyframe_idx = 0;
  static int backward_keyframe_idx = -1;
  if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2) && asf_found_index_flag == 1 && read_nextframe == 1)
  {
	unsigned int cur_packet_num;
	off_t new_pos;
	int i;
	found_keyframe = 0;
	keyframe_objnum = 0;
	read_nextframe = 0;
	ds_free_packs(demux->video);

	if(speed_mult >= 2)
	{
		if(forward_keyframe_idx >= asf_index_num-1)
		{
			demux->stream->eof=1;
			return 0;
		}

		if(forward_keyframe_idx == 0)
		{
			cur_packet_num = (demux->filepos - demux->movi_start) / asf->packetsize;
			for(i = 0; i < asf_index_num; i++)
			{
				if(asf_index_packet_num[i] >= cur_packet_num)
				{
					forward_keyframe_idx = i+1;
					new_pos = demux->movi_start + (asf_index_packet_num[i] * asf->packetsize);
					stream_seek(demux->stream, new_pos);
					demux->filepos = stream_tell(demux->stream);
					break;
				}
			}
		}
		else
		{
			new_pos = demux->movi_start + (asf_index_packet_num[forward_keyframe_idx++] * asf->packetsize);
			stream_seek(demux->stream, new_pos);
			demux->filepos = stream_tell(demux->stream);
		}
	}
	else
		forward_keyframe_idx = 0;

	if(speed_mult < 0)
	{
		if(backward_keyframe_idx == 0)
		{
//			speed_mult = 0;
			FR_to_end = 1;
                     asf_FR_to_end = 1; //Polun 2011-09-16  for mantis 6101 FR_to_end can't player from first keyframe
		}

		if(backward_keyframe_idx == -1)
		{
			cur_packet_num = (demux->filepos - demux->movi_start) / asf->packetsize;
			for(i = (asf_index_num - 1); i >= 0; i--)
			{
				if(asf_index_packet_num[i] <= cur_packet_num)
				{
					backward_keyframe_idx = i-1;
					new_pos = demux->movi_start + (asf_index_packet_num[i] * asf->packetsize);
					stream_seek(demux->stream, new_pos);
					demux->filepos = stream_tell(demux->stream);
					if(backward_keyframe_idx < 0)
					{
						FR_to_end = 1;
                                          asf_FR_to_end = 1;//Polun 2011-09-16  for mantis 6101 FR_to_end can't player from first keyframe
					}
					break;
				}
			}
		}
		else
		{
			new_pos = demux->movi_start + (asf_index_packet_num[backward_keyframe_idx--] * asf->packetsize);
			stream_seek(demux->stream, new_pos);
			demux->filepos = stream_tell(demux->stream);
			if(backward_keyframe_idx < 0)
			{
				FR_to_end = 1;
                           asf_FR_to_end = 1;//Polun 2011-09-16  for mantis 6101 FR_to_end can't player from first keyframe
			}
		}
	}
	else
		backward_keyframe_idx = -1;
  }
  else if(read_nextframe == 1)
  {
	forward_keyframe_idx = 0;
	backward_keyframe_idx = -1;
  }

//Fuchun 2010.03.05
while(1)
{
	int asf_read_continue = 1;
	if(!FR_to_end && speed_mult != 0 && demux->filepos <= 0)
	{
#if 0
		demux->stream->eof = 1;
		return 0;
#else
		demux->filepos = demux->movi_start;
		stream_seek(demux->stream, demux->filepos);
//		speed_mult = 0;
		FR_to_end = 1;
              asf_FR_to_end = 1;  //Polun 2011-09-16  for mantis 6101 FR_to_end can't player from first keyframe
#endif
	}


  if((demux->movi_start < demux->movi_end) && (demux->filepos>=demux->movi_end)){
          demux->stream->eof=1;
          return 0;
  }
//printf("!!!! bf stream_read asf->packetsize=%d %s:%s:%d\n", (int)asf->packetsize, __FILE__, __FUNCTION__, __LINE__);
    stream_read(demux->stream,asf->packet,asf->packetsize);
//printf("!!!! af stream_read\n");
    if(demux->stream->eof) return 0; // EOF
    if(asf->packetsize < 2) return 0; // Packet too short

//    {	//disable Fuchun 2010.03.04
	    unsigned char* p=asf->packet;
            unsigned char* p_end=asf->packet+asf->packetsize;
            unsigned char flags=p[0];
            unsigned char segtype=p[1];
            unsigned padding;
            unsigned plen;
	    unsigned sequence;
            unsigned long time=0;
            unsigned short duration=0;

            int segs=1;
            unsigned char segsizetype=0x80;
            int seg=-1;

	#ifdef CONFIG_DRM_ENABLE
	// WT, 100628, DRM payload process api
	/* Discretix integration Start */
	/*before handling the packet decrypt it if file is DRM protected*/
    /* move decryption to here (packet based) added by hamer@20111202 */
	if (demux->fileIsDrmProtected == DX_TRUE)
	{
		unsigned long output = asf->packetsize;
		if (demux->drmFileOpened != DX_TRUE)
		{
			printf("DX: ERROR content is encrypted but DRM stream is not opened\n");
			return 0;
		}		
		if (DxDrmStream_ProcessPacket(demux->drmStream,1 ,p, asf->packetsize, p, &output) != DX_DRM_SUCCESS)
		{
			printf("DX: ERROR - packet decryption failed\n");
			return 0;
		}
	}
	/* Discretix integration End */
	#endif
            if( mp_msg_test(MSGT_DEMUX,MSGL_DBG2) ){
                int i;
                for(i=0;i<FFMIN(16, asf->packetsize);i++) printf(" %02X",asf->packet[i]);
                printf("\n");
            }

	    // skip ECC data if present by testing bit 7 of flags
	    // 1xxxbbbb -> ecc data present, skip bbbb byte(s)
	    // 0xxxxxxx -> payload parsing info starts
	    if (flags & 0x80)
	    {
		p += (flags & 0x0f)+1;
		if (p+1 >= p_end) return 0; // Packet too short
		flags = p[0];
		segtype = p[1];
	    }

            //if(segtype!=0x5d) printf("Warning! packet[4] != 0x5d  \n");

	    p+=2; // skip flags & segtype

            // Read packet size (plen):
	    if(!check_varlen(p, p_end, flags>> 5)) return 0; // Not enough data
	    plen = read_varlen(&p, flags >> 5, 0);

            // Read sequence:
	    if(!check_varlen(p, p_end, flags>> 1)) return 0; // Not enough data
	    sequence = read_varlen(&p, flags >> 1, 0);

            // Read padding size (padding):
	    if(!check_varlen(p, p_end, flags>> 3)) return 0; // Not enough data
	    padding = read_varlen(&p, flags >> 3, 0);

	    if(((flags>>5)&3)!=0){
              // Explicit (absoulte) packet size
              mp_dbg(MSGT_DEMUX,MSGL_DBG2,"Explicit packet size specified: %d  \n",plen);
              if(plen>asf->packetsize) mp_msg(MSGT_DEMUX,MSGL_V,"Warning! plen>packetsize! (%d>%d)  \n",plen,asf->packetsize);
	    } else {
              // Padding (relative) size
              plen=asf->packetsize-padding;
	    }

	    // Read time & duration:
	    if (p+5 >= p_end) return 0; // Packet too short
	    time = AV_RL32(p); p+=4;
	    duration = AV_RL16(p); p+=2;

	    // Read payload flags:
            if(flags&1){
	      // multiple sub-packets
              if (p >= p_end) return 0; // Packet too short
              segsizetype=p[0]>>6;
              segs=p[0] & 0x3F;
              ++p;
            }
            mp_dbg(MSGT_DEMUX,MSGL_DBG4,"%08"PRIu64":  flag=%02X  segs=%d  seq=%u  plen=%u  pad=%u  time=%ld  dur=%d\n",
              (uint64_t)demux->filepos,flags,segs,sequence,plen,padding,time,duration);

            for(seg=0;seg<segs;seg++){
              //ASF_segmhdr_t* sh;
              unsigned char streamno;
              unsigned int seq;
              unsigned int x;	// offset or timestamp
	      unsigned int rlen;
	      //
              int len;
              uint64_t time2=0;
	      int keyframe=0;

              if(p>=p_end) {
                mp_msg(MSGT_DEMUX,MSGL_V,"Warning! invalid packet 1, aborting parsing...\n");
                break;
              }

              if( mp_msg_test(MSGT_DEMUX,MSGL_DBG2) ){
                int i;
                printf("seg %d:",seg);
                for(i=0;i<FFMIN(16, p_end - p);i++) printf(" %02X",p[i]);
                printf("\n");
              }

              streamno=p[0]&0x7F;
	      if(p[0]&0x80) keyframe=1;
	      p++;

              // Read media object number (seq):
	      if(!check_varlen(p, p_end, segtype >> 4)) break; // Not enough data
	      seq = read_varlen(&p, segtype >> 4, 0);

              // Read offset or timestamp:
	      if(!check_varlen(p, p_end, segtype >> 2)) break; // Not enough data
	      x = read_varlen(&p, segtype >> 2, 0);

              // Read replic.data len:
	      if(!check_varlen(p, p_end, segtype)) break; // Not enough data
	      rlen = read_varlen(&p, segtype, 0);

		//Fuchun 2010.03.05
		if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2) && asf_found_index_flag)
		{
			if(keyframe == 1 && streamno == demux->video->id && (keyframe_objnum == 0 || keyframe_objnum == seq))
			{
				keyframe_objnum = seq;
				found_keyframe = 1;
			}
			else if(streamno == demux->audio->id || !found_keyframe)
			{
				switch(rlen){
	             		case 0x01: // 1 = special, means grouping
	             			++p; // skip PTS delta
	             			break;
	             		default:
		    			p+=rlen;
	             		}

	             		if(flags&1){
		             		 // multiple segments
					if(!check_varlen(p, p_end, segsizetype)) break; // Not enough data
					len = read_varlen(&p, segsizetype, plen-(p-asf->packet));
	             		} else {
	               		// single segment
	             			len=plen-(p-asf->packet);
	             		}
	             		if(len<0 || (p+len)>p_end){
	             			len = p_end - p;
	             		}

	             		switch(rlen){
	             		case 0x01:
					while(len>0){
		  				int len2=p[0];
		  				p++;
	             				if(len2 > len - 1 || len2 < 0) break; // Not enough data
	                  			len2 = FFMIN(len2, asf->packetsize);
	                  			p+=len2;
			  			len-=len2+1;
					}
	                		break;
	              	default:
	                		if (len <= 0) break;
	                		if (!asf->asf_is_dvr_ms || asf->found_first_key_frame) {
	                   	 		len = FFMIN(len, asf->packetsize);
	                		}
	                		p+=len;
	                		break;
		      		}
				continue;
			}
		}
		else if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2))
		{
			if(speed_mult >= 2)
			{
				if(keyframe == 1 && streamno == demux->video->id && 
					(keyframe_objnum == 0 || keyframe_objnum == seq || (seq - keyframe_objnum) > 25))
				{
					keyframe_objnum = seq;
					asf_read_continue = 0;
				}
				else
				{
					switch(rlen){
	              		case 0x01: // 1 = special, means grouping
	                			++p; // skip PTS delta
	                			break;
	              		default:
			    			p+=rlen;
	              		}

	              		if(flags&1){
	               		 // multiple segments
						if(!check_varlen(p, p_end, segsizetype)) break; // Not enough data
						len = read_varlen(&p, segsizetype, plen-(p-asf->packet));
	              		} else {
	                		// single segment
	                			len=plen-(p-asf->packet);
	              		}
	              		if(len<0 || (p+len)>p_end){
	                			len = p_end - p;
	              		}

	              		switch(rlen){
	              		case 0x01:
						while(len>0){
			  				int len2=p[0];
			  				p++;
	                  				if(len2 > len - 1 || len2 < 0) break; // Not enough data
	                  				len2 = FFMIN(len2, asf->packetsize);
	                  				p+=len2;
			  				len-=len2+1;
						}
	                			break;
	              		default:
	                			if (len <= 0) break;
	                			if (!asf->asf_is_dvr_ms || asf->found_first_key_frame) {
	                   		 		len = FFMIN(len, asf->packetsize);
	                			}
	                			p+=len;
	                			break;
		      			}
					continue;
				}
			}
			else if(speed_mult < 0)
			{
				if(keyframe == 1 && streamno == demux->video->id
					&& FB_read_nonkey == 0
					&& (keyframe_objnum == 0 || keyframe_objnum == seq || (keyframe_objnum - seq) > 25))
				{
					FB_read_key = 1;
					keyframe_objnum = seq;
					break;
				}
				else if(//keyframe == 0 && 				//one packet may have more key frame, so disable this
					streamno == demux->video->id
					&& FB_read_key == 1
					&& keyframe_objnum != seq)
				{
					FB_last_pos = demux->filepos;
					FB_read_nonkey = 1;

					switch(rlen){
	              		case 0x01: // 1 = special, means grouping
	                			++p; // skip PTS delta
	                			break;
	              		default:
			    			p+=rlen;
	              		}

	              		if(flags&1){
	               		 // multiple segments
						if(!check_varlen(p, p_end, segsizetype)) break; // Not enough data
						len = read_varlen(&p, segsizetype, plen-(p-asf->packet));
	              		} else {
	                		// single segment
	                			len=plen-(p-asf->packet);
	              		}
	              		if(len<0 || (p+len)>p_end){
	                			len = p_end - p;
	              		}

	              		switch(rlen){
	              		case 0x01:
						while(len>0){
			  				int len2=p[0];
			  				p++;
	                  				if(len2 > len - 1 || len2 < 0) break; // Not enough data
	                  				len2 = FFMIN(len2, asf->packetsize);
	                  				p+=len2;
			  				len-=len2+1;
						}
	                			break;
	              		default:
	                			if (len <= 0) break;
	                			if (!asf->asf_is_dvr_ms || asf->found_first_key_frame) {
	                   		 		len = FFMIN(len, asf->packetsize);
	                			}
	                			p+=len;
	                			break;
		      			}
					continue;
				}
				else if(keyframe == 1 && streamno == demux->video->id
					&& FB_read_nonkey == 1
					&& keyframe_objnum == seq)
				{
					asf_read_continue = 0;
					FB_read_key = 0;
				}
				else if(FB_read_key == 0)
				{
					if(streamno == demux->video->id)
					{
						FB_read_nonkey = 0;
						break;
					}

					switch(rlen){
	              		case 0x01: // 1 = special, means grouping
	                			++p; // skip PTS delta
	                			break;
	              		default:
			    			p+=rlen;
	              		}

	              		if(flags&1){
	               		 // multiple segments
						if(!check_varlen(p, p_end, segsizetype)) break; // Not enough data
						len = read_varlen(&p, segsizetype, plen-(p-asf->packet));
	              		} else {
	                		// single segment
	                			len=plen-(p-asf->packet);
	              		}
	              		if(len<0 || (p+len)>p_end){
	                			len = p_end - p;
	              		}

	              		switch(rlen){
	              		case 0x01:
						while(len>0){
			  				int len2=p[0];
			  				p++;
	                  				if(len2 > len - 1 || len2 < 0) break; // Not enough data
	                  				len2 = FFMIN(len2, asf->packetsize);
	                  				p+=len2;
			  				len-=len2+1;
						}
	                			break;
	              		default:
	                			if (len <= 0) break;
	                			if (!asf->asf_is_dvr_ms || asf->found_first_key_frame) {
	                   		 		len = FFMIN(len, asf->packetsize);
	                			}
	                			p+=len;
	                			break;
		      			}
					continue;
				}
			}
		}

//	      printf("### rlen=%d   \n",rlen);

              switch(rlen){
              case 0x01: // 1 = special, means grouping
	        //printf("grouping: %02X  \n",p[0]);
                ++p; // skip PTS delta
                break;
              default:
	        if(rlen>=8){
            	    p+=4;	// skip object size
            	    if (p+3 >= p_end) break; // Packet too short
            	    time2=AV_RL32(p); // read PTS
            	    if (asf->asf_is_dvr_ms)
            	        get_payload_extension_data(demux, &p, streamno, seq, &keyframe, &time2);
		    p+=rlen-4;
#if 0	// Raymond 2009/05/31		    
		//ethan 090521   get timestamp
				if(streamno == demux->video->id|| ((time2-playPreRoll) == 0))  //ethan 090605  video stream
				{
					if(asfTimeHead==NULL)
					{
						asfTimeHead = malloc(sizeof(ASFTime));
						asfTimeHead->timeStamp = time2 -playPreRoll;
						asfTimeHead->next = NULL;
					}
					else
					{
						ASFTime* findLast = asfTimeHead;

						while(findLast->next != NULL)
						{
							findLast = findLast->next;
						}

						if(findLast->timeStamp != (time2-playPreRoll))
						{
							findLast->next = malloc(sizeof(ASFTime));
							findLast = findLast->next;
							findLast->timeStamp = time2 -playPreRoll;
							findLast->next=NULL;
						}
					}
				}
#endif				
		} else {
            	    mp_msg(MSGT_DEMUX,MSGL_V,"unknown segment type (rlen): 0x%02X  \n",rlen);
		    time2=0; // unknown
		    p+=rlen;
		}
              }

              if(flags&1){
                // multiple segments
		if(!check_varlen(p, p_end, segsizetype)) break; // Not enough data
		len = read_varlen(&p, segsizetype, plen-(p-asf->packet));
              } else {
                // single segment
                len=plen-(p-asf->packet);
              }
              if(len<0 || (p+len)>p_end){
                mp_msg(MSGT_DEMUX,MSGL_V,"ASF_parser: warning! segment len=%d\n",len);
                len = p_end - p;
              }
              mp_dbg(MSGT_DEMUX,MSGL_DBG4,"  seg #%d: streamno=%d  seq=%d  type=%02X  len=%d\n",seg,streamno,seq,rlen,len);

    #if 0 // decryption unit is by payload for wmdrm, but for playready, it shall be by packet.
    //#ifdef CONFIG_DRM_ENABLE
	// WT, 100628, DRM payload process api
	/* Discretix integration Start */
	/*before handling the packet decrypt it if file is DRM protected*/
	if (demux->fileIsDrmProtected == DX_TRUE)
	{
		unsigned long output = len;
		if (demux->drmFileOpened != DX_TRUE)
		{
			printf("DX: ERROR content is encrypted but DRM stream is not opened\n");
			return 0;
		}		
		if (DxDrmStream_ProcessPacket(demux->drmStream,1 ,p, len, p, &output) != DX_DRM_SUCCESS)
		{
			printf("DX: ERROR - packet decryption failed\n");
			return 0;
		}
	}
	/* Discretix integration End */
	#endif
	
              switch(rlen){
              case 0x01:
                // GROUPING:
                //printf("ASF_parser: warning! grouping (flag=1) not yet supported!\n",len);
                //printf("  total: %d  \n",len);
		while(len>0){
		  int len2=p[0];
		  p++;
                  //printf("  group part: %d bytes\n",len2);
                  if(len2 > len - 1 || len2 < 0) break; // Not enough data
                  len2 = FFMIN(len2, asf->packetsize);
                  demux_asf_read_packet(demux,p,len2,streamno,seq,x,duration,-1,keyframe);
                  p+=len2;
		  len-=len2+1;
		  ++seq;
		}
                if(len!=0){
                  mp_msg(MSGT_DEMUX,MSGL_V,"ASF_parser: warning! groups total != len\n");
                }
                break;
              default:
                // NO GROUPING:
                //printf("fragment offset: %d  \n",sh->x);
                if (len <= 0) break;
                if (!asf->asf_is_dvr_ms || asf->found_first_key_frame) {
                    len = FFMIN(len, asf->packetsize);
                    //Polun 2011-08-11 for seek must be keyframe to read packet.
                    //if(speed_mult == 0 || speed_mult == 1 || keyframe == 1)
                    if(speed_mult == 0 || speed_mult == 1 || keyframe == 1 || FR_to_end) //Polun 2011-09-16 for mantis 6101 FR_to_end can't player from first keyframe
                    demux_asf_read_packet(demux,p,len,streamno,seq,time2,duration,x,keyframe);
                }
                p+=len;
                break;
	      }

            } // for segs

	//Fuchun 2010.03.05
	if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2) && asf_found_index_flag)
	{
		break;
	}
	else if(!FR_to_end && (speed_mult < 0 || speed_mult >= 2))
	{
		if(asf_read_continue && speed_mult >= 2)
		{
			demux->filepos += asf->packetsize;
			stream_seek(demux->stream, demux->filepos);
			FB_last_pos = demux->movi_end;		//Fuchun 2010.05.03 for speed_mult > 0 change to speed_mult < 0 use
		}
		else if(asf_read_continue && speed_mult < 0)
		{
			if(FB_read_nonkey == 0)
			{
				if(demux->filepos > FB_last_pos)
				{
					demux->filepos = FB_last_pos - asf->packetsize;
					FB_last_pos = demux->movi_end;
					FB_read_key = 0;
				}
				else
					demux->filepos -= asf->packetsize;
			}
			else if(FB_read_nonkey == 1)
				demux->filepos += asf->packetsize;
            
                     if(demux->filepos < 0) // Polun 2011-09-13 mantis6904 FR x2 jump to next file.
                        demux->filepos = 0;
			
			stream_seek(demux->stream, demux->filepos);
		}
		else
			break;
	}
	else
		break;
	
}

            return 1; // success
//    }	//disable Fuchun 2010.03.04

    mp_msg(MSGT_DEMUX,MSGL_V,"%08"PRIX64":  UNKNOWN TYPE  %02X %02X %02X %02X %02X...\n",(int64_t)demux->filepos,asf->packet[0],asf->packet[1],asf->packet[2],asf->packet[3],asf->packet[4]);
    return 0;
}

#include "stheader.h"

static void demux_seek_asf(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags){
    struct asf_priv* asf = demuxer->priv;
    demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=d_audio->sh;
//    sh_video_t *sh_video=d_video->sh;

  //FIXME: OFF_T - didn't test ASF case yet (don't have a large asf...)
  //FIXME: reports good or bad to steve@daviesfam.org please

  //================= seek in ASF ==========================
    float p_rate=asf->packetrate; // packets / sec
    
    if(d_video->id < 0)	//Fuchun 2010.11.08 for only audio seek
		p_rate = sh_audio->i_bps/(double)asf->packetsize;

    off_t rel_seek_packs=(flags&SEEK_FACTOR)?	 // FIXME: int may be enough?
	(rel_seek_secs*(demuxer->movi_end-demuxer->movi_start)/asf->packetsize):
	(rel_seek_secs*p_rate);
    off_t rel_seek_bytes=rel_seek_packs*asf->packetsize;
    off_t newpos = 0;
	char normal_case = 1;

	asfseek_debug("### Carlos in [%s][%d] asf_index_num is [%d] total size is [%lld] rel_seek_bytes[%f]###\n", __func__, __LINE__, asf_index_num, demuxer->movi_end, rel_seek_bytes);
    //printf("ASF: packs: %d  duration: %d  \n",(int)fileh.packets,*((int*)&fileh.duration));
    asfseek_debug("ASF: packs: %d  packrate: %f  movielength:%f play_duration[%lld]\n",asf->packetsize, asf->packetrate, asf->movielength, asf->play_duration);
    //printf("ASF_seek: %f secs -> %"PRId64" packs -> %"PRId64" bytes  \n",
       //rel_seek_secs,rel_seek_packs,rel_seek_bytes);

	//reset to avoid fault Fuchun 2010.03.08
	FB_read_key = 0;
	FB_read_nonkey = 0;
	FB_last_pos = demuxer->movi_end;
	keyframe_objnum = 0;

	asfseek_debug("@@@@ In [%s][%d] movi_start[%lld] filepos[%lld] rel_seek_bytes[%lld] newpos[%lld] @@@@\n", __func__, __LINE__, demuxer->movi_start, demuxer->filepos, rel_seek_bytes, newpos);
	if (flags&SEEK_ABSOLUTE)
	{
		double per_sec_keyframe = 0.0;
		int real_seek_num = 0;
	    double check_pts;
	    char *new_start;
		int retry_count = 50;
		double diff_time = 0.0;
		int max_num = 0;
		int min_num = 0;
		int check_case = 1;
              #if 0 //Polun 2011-12-07 fixed mantis6294 2004_Pass_the_Flame.wma seek fail root cause: the file is audio file but d_video is not NULL and has indextable asf_index_num is 1.    
		if (asf_index_packet_num)
              #else
		if (asf_index_packet_num && asf_index_num > 1)
              #endif
		{
			normal_case = 0;
			per_sec_keyframe = (double)asf_index_num / asf->movielength;
			real_seek_num = (int)(per_sec_keyframe * rel_seek_secs);
			asfseek_debug("@@@ Carlos in [%s][%d] per_sec_keyframe is [%f] asf_index_num is [%d] count[%d]@@@\n", __func__, __LINE__, per_sec_keyframe, asf_index_num, real_seek_num);
			rel_seek_bytes = asf_index_packet_num[real_seek_num] * asf->packetsize;
			asfseek_debug("@@@@ In [%s][%d] asf_index_packet_num[%d] is [%d] rel_seek_bytes is [%lld] @@@@\n", __func__, __LINE__, real_seek_num, asf_index_packet_num[real_seek_num], rel_seek_bytes);
			newpos = demuxer->movi_start + rel_seek_bytes;
			stream_seek(demuxer->stream, newpos);
			demuxer->filepos = stream_tell(demuxer->stream);
			min_num = 0;
			max_num = asf_index_num;
			do
			{
				ds_get_packet_pts(d_video, &new_start, &check_pts);
				asfseek_debug("@@@ In [%s][%d] vpts=%f  check_pts=%f count [%d]\n", __func__, __LINE__, (double)d_video->pts, (double)check_pts);
				diff_time = d_video->pts-rel_seek_secs;
				if (abs(diff_time) < 3.0)
				{
					ds_free_packs(d_video);
					break;
				}
				else
				{
					if (diff_time > 3.0)
					{
						if (real_seek_num < asf_index_num)
							max_num = real_seek_num; //update previous time
						else
							printf("!!!! In [%s][%d] real_seek_num [%d] over asf_index_num [%d] !!!!\n", __func__, __LINE__, real_seek_num, asf_index_num);
					}
					else /* diff_time < 5.0 */
					{
						if (real_seek_num > min_num)
							min_num = real_seek_num; //update previous time
						else
							printf("!!!! In [%s][%d] real_seek_num [%d] samll than min_num [%d] !!!!\n", __func__, __LINE__, real_seek_num, min_num);
					}

					real_seek_num = (int)((rel_seek_secs / d_video->pts) * (double)real_seek_num);
					if (real_seek_num >= max_num || real_seek_num <= min_num)
					{
						check_case = 1;
						asfseek_debug("#### In [%s][%d] case [%.3d] min_num[%d] max_num[%d] real_seek_num[%d] new_seek_num[%d]####\n", __func__, __LINE__, check_case, min_num, max_num, real_seek_num, (min_num + max_num) / 2);
						real_seek_num = (min_num + max_num) / 2;
					}
					else
					{
						check_case = 2;
						asfseek_debug("#### In [%s][%d] case [%.3d] min_num[%d] max_num[%d] real_seek_num[%d] ####\n", __func__, __LINE__, check_case, min_num, max_num, real_seek_num);
					}
					asfseek_debug("$$$$ In [%s][%d] diff_time is [%f] real_seek_num is [%d] min[%d] max[%d]$$$$\n", __func__, __LINE__, diff_time, real_seek_num, min_num, max_num);
				}
				ds_free_packs(d_video);
				rel_seek_bytes = asf_index_packet_num[real_seek_num] * asf->packetsize;
				asfseek_debug("@@@@ In [%s][%d] asf_index_packet_num[%d] is [%d] rel_seek_bytes is [%lld] @@@@", __func__, __LINE__, real_seek_num, asf_index_packet_num[real_seek_num], rel_seek_bytes);
				newpos = demuxer->movi_start + rel_seek_bytes;
				asfseek_debug("newpos is [%lld] ###\n", newpos);

				stream_seek(demuxer->stream, newpos);
				demuxer->filepos = stream_tell(demuxer->stream);
				retry_count--;

			} while(retry_count > 0);
		}
              //Polun 2011-11-16 ++s fixed mantis 6294 wma audio file no index table 
              else
              {
                   normal_case = 0;
		     newpos = demuxer->movi_start + rel_seek_bytes;
                   if(d_video)
                       ds_free_packs(d_video);
                   if(d_audio)
                       ds_free_packs(d_audio);
		     stream_seek(demuxer->stream, newpos);
                   demuxer->filepos = stream_tell(demuxer->stream);
              }
              //Polun 2011-11-16 ++e
		/* TODO, need do upnp case, upnp no simple index table */
	}
	if (normal_case)
	{
              //Polun 2011-09-16 ++s for mantis 6101 FR_to_end can't player from first keyframe
              double seek_pts = 0.0;
              seek_pts = d_video->pts+rel_seek_secs;
              //Polun 2011-09-16 ++e
              
		newpos = demuxer->filepos + rel_seek_bytes;
              //Polun 2011-10-11 ++s fixed ¥Û»R¥x.asf press right key jump to next file.
              if( asf_found_index_flag == 1)
              {
                   double check_pts;
	            char *new_start;
                   double cur_packet_pts;
                   double new_packet_pts;
                   printf("####ASF: Use index table to seek #### \n"); 
                   ds_get_packet_pts(d_video, &new_start, &check_pts);
                   cur_packet_pts = d_video->pts;
	            unsigned int cur_packet_num;
                   int indexcunt_j;
                   int indexcunt_k;
	            cur_packet_num = (demuxer->filepos - demuxer->movi_start) / asf->packetsize;
                   if(rel_seek_secs > 0)
                   {
			  for(indexcunt_j = 0; indexcunt_j < asf_index_num; indexcunt_j += 6)
			  {
		              if(asf_index_packet_num[indexcunt_j] > cur_packet_num)
				{
			            newpos = demuxer->movi_start + (asf_index_packet_num[indexcunt_j] * asf->packetsize);
		                   stream_seek(demuxer->stream,newpos);
			            demuxer->filepos = stream_tell(demuxer->stream);
                                 ds_get_packet_pts(d_video, &new_start, &check_pts);
                                 new_packet_pts = d_video->pts;

                                 if((new_packet_pts -cur_packet_pts) >= (rel_seek_secs - 20))
                                 {
			                 for(indexcunt_k = indexcunt_j; indexcunt_k <= (indexcunt_j + 6); indexcunt_k++)
			                 {
			                          newpos = demuxer->movi_start + (asf_index_packet_num[indexcunt_k] * asf->packetsize);
		                                 stream_seek(demuxer->stream,newpos);
			                          demuxer->filepos = stream_tell(demuxer->stream);
                                               ds_get_packet_pts(d_video, &new_start, &check_pts);
                                               new_packet_pts = d_video->pts;

                                              if((new_packet_pts -cur_packet_pts) >= (rel_seek_secs))
                                              {
                                                   break;
                                               }
				            }
                                        break;
                                  }
				}
			    }
                     }
                     else
                     {
			     for(indexcunt_j = (asf_index_num - 1); indexcunt_j >= 0; indexcunt_j -= 6)
			     {
			          if ((cur_packet_pts + rel_seek_secs) <= 0)
                               {
                                   newpos=demuxer->movi_start;
                                   break;
                               }

				   if(asf_index_packet_num[indexcunt_j] <= cur_packet_num)
				   {
					newpos = demuxer->movi_start + (asf_index_packet_num[indexcunt_j] * asf->packetsize);
					stream_seek(demuxer->stream, newpos);
					demuxer->filepos = stream_tell(demuxer->stream);
                                   ds_get_packet_pts(d_video, &new_start, &check_pts);
                                   new_packet_pts = d_video->pts;
                                   
                                   if((new_packet_pts - rel_seek_secs - 20) <= cur_packet_pts )
                                  {
			                   for(indexcunt_k = (indexcunt_j + 6); indexcunt_k <= indexcunt_j ; indexcunt_k--)
			                   { 
			                          newpos = demuxer->movi_start + (asf_index_packet_num[indexcunt_k] * asf->packetsize);
		                                 stream_seek(demuxer->stream,newpos);
			                          demuxer->filepos = stream_tell(demuxer->stream);
                                               ds_get_packet_pts(d_video, &new_start, &check_pts);
                                               new_packet_pts = d_video->pts;

                                              if((new_packet_pts -cur_packet_pts) >= (rel_seek_secs))
                                              {
                                                   break;
                                               }
				            }
				            break;
                                   }
				    }
			      }
                     }
                     ds_free_packs(d_video);
                     ds_free_packs(d_audio);
              }
               //Polun 2011-10-11 ++e
		if(newpos<0 || newpos<demuxer->movi_start) newpos=demuxer->movi_start;
		printf("\r -- asf: newpos=%"PRId64" -- \n",newpos);
		stream_seek(demuxer->stream,newpos);

              //Polun 2011-09-16 ++s for mantis 6101 FR_to_end can't player from first keyframe
              if((rel_seek_secs == -0.1f && asf_FR_to_end == 1) ||(seek_pts < 0) && (fourCC == mmioFOURCC('W','M','V','3') ||fourCC == mmioFOURCC('M','P','4','S')))
              {
                   asf_FR_to_end = 0;
		     ds_free_packs(d_video);
                   printf("==== ASF: FR_to_end or FR seek to end ====\n");
                   return;
              } 
              asf_FR_to_end = 0;
              //Polun 2011-09-16 ++e
	}
     //Polun 2011-08-11 ++s mantis 5680
    if(flags & SEEK_ABSOLUTE)
    {
       if(fourCC ==mmioFOURCC('M','4','S','2') || fourCC == mmioFOURCC('M','P','4','S'))
           asf_first_start_code = 1;
       else if(fourCC ==mmioFOURCC('W','V','C','1'))//Polun 2011-08-15  add asf_first_start_code for mantis 5680 WVC1
           GotASFHeader = 3;

     }   
     //Polun 2011-08-11 ++e mantis 5680
    if (asf->asf_is_dvr_ms) asf->dvr_last_vid_pts = 0.0f;

    if (d_video->id >= 0)
    ds_fill_buffer(d_video);
    if(sh_audio){
      ds_fill_buffer(d_audio);
    }

    //printf("apts=%f vpts=%f\n", (float)d_audio->pts, (float)d_video->pts);
    if (d_video->id >= 0 && sh_audio)
    {
	    double check_pts;
	    char *new_start;
	    while (d_video->pts > 0 && d_audio->pts > d_video->pts)
	    {
		ds_get_packet_pts(d_video, &new_start, &check_pts);
		//printf("@@@ apts=%f vpts=%f  check_pts=%f\n", (float)d_audio->pts, (float)d_video->pts, (float)check_pts);

	    }
	    d_video->flags &= ~1;
    }

    if (d_video->id >= 0)
    {
      while(1){
	if(sh_audio && !d_audio->eof){
#if 0
	  float a_pts=d_audio->pts;
          a_pts+=(ds_tell_pts(d_audio)-sh_audio->a_in_buffer_len)/(float)sh_audio->i_bps;
#else
	  float a_pts=d_audio->pts;
#endif
	  // sync audio:
          if (d_video->pts > a_pts){
	      skip_audio_frame(sh_audio);
//	      if(!ds_fill_buffer(d_audio)) sh_audio=NULL; // skip audio. EOF?
	      continue;
	  }
	}
	if(d_video->flags&1)
       {
            if(fourCC ==mmioFOURCC('W','V','C','1'))//Polun 2011-08-15  add asf_first_start_code for mantis 5680 WVC1
            {
                if(GotASFHeader ==3)
                    GotASFHeader = 1;
            }    
            break; // found a keyframe!
       }
	if(!ds_fill_buffer(d_video)) break; // skip frame.  EOF?
      }
    }

}

static int demux_asf_control(demuxer_t *demuxer,int cmd, void *arg){
    struct asf_priv* asf = demuxer->priv;
/*  demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=d_audio->sh;
    sh_video_t *sh_video=d_video->sh;
*/
    switch(cmd) {
	case DEMUXER_CTRL_GET_TIME_LENGTH:
	    *((double *)arg)=asf->movielength;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_GET_PERCENT_POS:
		return DEMUXER_CTRL_DONTKNOW;

	default:
	    return DEMUXER_CTRL_NOTIMPL;
    }
}


static demuxer_t* demux_open_asf(demuxer_t* demuxer)
{
    struct asf_priv* asf = demuxer->priv;
    sh_audio_t *sh_audio=NULL;
    sh_video_t *sh_video=NULL;

	asf_first_start_code = 1; //Polun 2011-08-11 mantis 5680
    //reset parameter Fuchun 2010.03.08
	asfTimeHead = NULL;		
	keyframe_objnum = 0;
	asf_found_index_flag = 0;
	asf_index_packet_num = NULL;
	asf_index_num = 0;
       asf_FR_to_end = 0;
       asf_exter_data = 0 ;//Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC.
       VC1_has_framePrefix = 0;
    //---- ASF header:
    if(!asf) return NULL;
    init_priv(asf);
    if (!read_asf_header(demuxer,asf))
        return NULL;
//printf("== read_asf_header ok!\n");
    stream_reset(demuxer->stream);
//printf("== try to stream_seek demuxer->movi_start=%d buf_len=%d buf_pos=%d pos=%d\n", demuxer->movi_start, demuxer->stream->buf_len, demuxer->stream->buf_pos, demuxer->stream->pos);

    if (demuxer->stream->type == STREAMTYPE_DS)
    {
        off_t cmp_movi_start = demuxer->movi_start, cmp_buf_pos = (off_t)demuxer->stream->buf_pos;

        if (cmp_movi_start != cmp_buf_pos)
        {
          stream_seek(demuxer->stream,demuxer->movi_start);
//printf("== af stream_seek demuxer->movi_start=%d buf_len=%d buf_pos=%d pos=%d\n", (int)demuxer->movi_start, (int)demuxer->stream->buf_len, (int)demuxer->stream->buf_pos, (int)demuxer->stream->pos);
        }
if(!demuxer->stream->seek)
    demuxer->seekable=0;
//printf("==> dbg 0000 demuxer->stream->seek=%d demuxer->seekable=%d\n", demuxer->stream->seek, demuxer->seekable);

    }
    else
    {
//printf("== try to stream_seek demuxer->movi_start=%d buf_len=%d pos=%d\n", demuxer->movi_start, demuxer->stream->buf_len, demuxer->stream->buf_pos);
      stream_seek(demuxer->stream,demuxer->movi_start);
//printf("== stream_seek done ! demuxer->movi_start=%d buf_len=%d pos=%d\n", demuxer->movi_start, demuxer->stream->buf_len, demuxer->stream->buf_pos);
    }

#if 0 //Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC move back to asfheader.c.
	if(demuxer->video->id != -2)
	{
		unsigned int frameWidth, frameHeight;
//		unsigned long long playDuration;
		unsigned char *wmv3StrC = NULL;
		sh_video_t *sh = demuxer->video->sh;
		if(!ds_fill_buffer(demuxer->video))
		{
			mp_msg(MSGT_DEMUXER,MSGL_WARN,"ASF: " MSGTR_MissingVideoStream);
			demuxer->video->sh=NULL;
		//printf("ASF: missing video stream!? contact the author, it may be a bug :(\n");
		} 
		else 
		{
			sh = demuxer->video->sh;
			ds_free_packs(demuxer->video);
			stream_reset(demuxer->stream);
			stream_seek(demuxer->stream,demuxer->movi_start);
			// Raymond 2009/06/02
			frameWidth = sh->bih->biWidth;
			frameHeight = sh->bih->biHeight;
			fourCC = sh->bih->biCompression;

			// Raymond 2009/06/05
			if ( sh->bih->biSize > sizeof(BITMAPINFOHEADER) )
		  	{  	
		  		if(fourCC == mmioFOURCC('W','M','V','3'))
		  		{
		  			GotASFHeader = 2;	// Raymond 2010/05/26

#ifdef _SKY_VDEC_V2
					asfHeaderSize = 36;
					asfHeader = malloc(asfHeaderSize);
		    			wmv3StrC = ((uint8_t*)sh->bih) + sizeof(BITMAPINFOHEADER);

					PUT32_L(0x00000004, (unsigned char*)(asfHeader+4));

					memcpy((unsigned char*)(asfHeader+8), wmv3StrC, 4);

					PUT32_L(frameHeight, (unsigned char*)(asfHeader+12));
					PUT32_L(frameWidth, (unsigned char*)(asfHeader+16));
					PUT32_L(0x0000000C, (unsigned char*)(asfHeader+20));
					PUT32_L(0x00000000, (unsigned char*)(asfHeader+24));
					PUT32_L(0x00000000, (unsigned char*)(asfHeader+28));
					PUT32_L(0x00000000, (unsigned char*)(asfHeader+32));

				
					if(demuxer->stream->type == STREAMTYPE_STREAM)		//Fuchun 2010.05.25
						*((int*)(asfHeader)) = 0xC5<<24 | 1;
					else
						*((int*)(asfHeader)) = 0xC5<<24 | (playDuration/avgFrTime + 1);		

#endif
		  		}
				else if(fourCC == mmioFOURCC('W','V','C','1'))
				{
					GotASFHeader = 2;	// Raymond 2010/05/26
					
					asfHeaderSize = sh->bih->biSize - sizeof(BITMAPINFOHEADER) -1;	
					asfHeader = ((uint8_t*)sh->bih) + sizeof(BITMAPINFOHEADER) + 1;		
				}
				else if(fourCC == mmioFOURCC('M','4','S','2') || fourCC == mmioFOURCC('M','P','4','S'))		//Fuchun 2010.04.22
				{
					sh->mpeg4_header_len = sh->bih->biSize - sizeof(BITMAPINFOHEADER);
					sh->mpeg4_header = sh->bih + 1;
				}
		  	}
		}
		
		{
			int i = 0;
			printf("asfHeader[%d] = ", asfHeaderSize);
			for( i = 0 ; i < asfHeaderSize ; i++ )
			{
				if( i % 16 == 0 )
					printf("\n");
				printf("%02X ", asfHeader[i]);			
			}
			printf("\n");
		}
	}
#endif

//    demuxer->idx_pos=0;
//    demuxer->endpos=avi_header.movi_end;
    if(demuxer->video->id != -2) {
//printf("==> dbg 01\n");
        if(!ds_fill_buffer(demuxer->video)){
            mp_msg(MSGT_DEMUXER,MSGL_WARN,"ASF: " MSGTR_MissingVideoStream);
            demuxer->video->sh=NULL;
            //printf("ASF: missing video stream!? contact the author, it may be a bug :(\n");
        } else {
//printf("==> dbg 02\n");

            sh_video=demuxer->video->sh;sh_video->ds=demuxer->video;
            sh_video->fps=1000.0f; sh_video->frametime=0.001f;

            if (asf->asf_is_dvr_ms) {
                sh_video->bih->biWidth = 0;
                sh_video->bih->biHeight = 0;
            }
        }
    }
//printf("==> dbg 03\n");

    if(demuxer->audio->id!=-2){
        mp_msg(MSGT_DEMUXER,MSGL_V,MSGTR_ASFSearchingForAudioStream,demuxer->audio->id);
        if(!ds_fill_buffer(demuxer->audio)){
            mp_msg(MSGT_DEMUXER,MSGL_INFO,"ASF: " MSGTR_MissingAudioStream);
            demuxer->audio->sh=NULL;
        } else {
            sh_audio=demuxer->audio->sh;sh_audio->ds=demuxer->audio;
            sh_audio->format=sh_audio->wf->wFormatTag;
        }
//printf("==> dbg 04\n");

    }
    if(!demuxer->stream->seek)
        demuxer->seekable=0;
//printf("==> dbg 05 demuxer->stream->seek=%d demuxer->seekable=%d\n", demuxer->stream->seek, demuxer->seekable);

	//reset parameter Fuchun  2010.03.05
	FB_last_pos = demuxer->movi_end;		
	FB_read_key= 0;
	FB_read_nonkey= 0;

    return demuxer;
}


static void demux_close_asf(demuxer_t *demuxer) {
    struct asf_priv* asf = demuxer->priv;
    sh_video_t *sh_video = demuxer->video->sh;

    if (!asf) return;

    if (asf->aud_repdata_sizes)
      free(asf->aud_repdata_sizes);

    if (asf->vid_repdata_sizes)
      free(asf->vid_repdata_sizes);

	if (asfHeader && sh_video->bih->biCompression == mmioFOURCC('W','M','V','3'))
	{
		free(asfHeader);
		asfHeader = NULL;
	}

	if(asf_index_packet_num)
		free(asf_index_packet_num);

    free(asf);
}

const demuxer_desc_t demuxer_desc_asf = {
  "ASF demuxer",
  "asf",
  "ASF",
  "A'rpi",
  "ASF, WMV, WMA",
  DEMUXER_TYPE_ASF,
  1, // safe autodetect
  asf_check_header,
  demux_asf_fill_buffer,
  demux_open_asf,
  demux_close_asf,
  demux_seek_asf,
  demux_asf_control
};
