/*
 * Matroska demuxer
 * Copyright (C) 2004 Aurelien Jacobs <aurel@gnuage.org>
 * Based on the one written by Ronald Bultje for gstreamer
 * and on demux_mkv.cpp from Moritz Bunkus.
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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "ebml.h"
#include "matroska.h"
#include "demux_real.h"

#include "mp_msg.h"
#include "help_mp.h"

#include "vobsub.h"
#include "subreader.h"
#include "libvo/sub.h"

#include "libass/ass_mp.h"

#include "libavutil/common.h"

#ifdef CONFIG_QTX_CODECS
#include "loader/qtx/qtxsdk/components.h"
#endif

#if CONFIG_ZLIB
#include <zlib.h>
#endif

#include "libavutil/lzo.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/avstring.h"

#ifdef SUPPORT_DIVX_DRM_MKV
#include "drm_mplayer.h"
extern int DRM_Open;
extern DRM_chunk *video_dd_chunk;
extern DRM_chunk *audio_dd_chunk;
#endif /* end of SUPPORT_DIVX_DRM_MKV */

// Raymond 2009/05/11
unsigned char *avc1_header = NULL;
int avc1_header_size = 0;
int avc1_nal_bytes = 0;	//Barry 2010-07-09
int add_avc1_extradata = 0;	//Barry 2010-08-24
int add_mpeg2_extradata = 0;	//Barry 2010-09-28
//Barry 2011-06-18
int add_wvc1_extradata = 0;
extern unsigned int no_osd;

int mkv_indexnull_flag = 0;

//extern unsigned char* videobuffer;	// Raymond 2009/06/01

#if 1	// Raymond 2009/06/01
//+SkyMedi_Vincent05182009
int rv40_header_size = 0;

uint16_t    rv9_num_seq=0;
uint16_t    rv9_flags=0;
uint32_t    rv9_last_packet=0;

//SkyMedi_Vincent05182009+
static int backward_keyframe_idx=-1;		//Fuchun 2010.03.09
#endif

//Barry 2010-07-21
unsigned char* mkv_asfHeader = NULL;
unsigned int mkv_asfHeaderSize=0;
int isVC1 = 0;// 1:WMV3   2:WVC1
int Mincache=0, Maxcache=0;	//Barry 2010-12-30

#ifdef _SKY_VDEC_V2	/*johnnyke 20100727*/
extern unsigned char* asfHeader;
extern unsigned int asfHeaderSize;
#endif

static const unsigned char sipr_swaps[38][2]={
    {0,63},{1,22},{2,44},{3,90},{5,81},{7,31},{8,86},{9,58},{10,36},{12,68},
    {13,39},{14,73},{15,53},{16,69},{17,57},{19,88},{20,34},{21,71},{24,46},
    {25,94},{26,54},{28,75},{29,50},{32,70},{33,92},{35,74},{38,85},{40,56},
    {42,87},{43,65},{45,59},{48,79},{49,93},{51,89},{55,95},{61,76},{67,83},
    {77,80} };

// Map flavour to bytes per second
#define SIPR_FLAVORS 4
#define ATRC_FLAVORS 8
#define COOK_FLAVORS 34
static const int sipr_fl2bps[SIPR_FLAVORS] = {813, 1062, 625, 2000};
static const int atrc_fl2bps[ATRC_FLAVORS] =
    {8269, 11714, 13092, 16538, 18260, 22050, 33075, 44100};
static const int cook_fl2bps[COOK_FLAVORS] =
    { 1000,  1378,  2024,  2584, 4005,  5513, 8010, 4005,   750, 2498,
      4048,  5513,  8010, 11973, 8010,  2584, 4005, 2067,  2584, 2584,
      4005,  4005,  5513,  5513, 8010, 12059, 1550, 8010, 12059, 5513,
     12016, 16408, 22911, 33506};

typedef struct
{
  uint32_t order, type, scope;
  uint32_t comp_algo;
  uint8_t *comp_settings;
  int comp_settings_len;
} mkv_content_encoding_t;

typedef struct mkv_track
{
  int tnum;
  char *name;

  char *codec_id;
  int ms_compat;
  char *language;

  int type;

  uint32_t v_width, v_height, v_dwidth, v_dheight;
  float v_frate;

  uint32_t a_formattag;
  uint32_t a_channels, a_bps;
  float a_sfreq;

  float default_duration;

  int default_track;

  void *private_data;
  unsigned int private_size;

  /* stuff for realmedia */
  int realmedia;
  int64_t rv_kf_base;
  int rv_kf_pts;
  float rv_pts;  /* previous video timestamp */
  float ra_pts;  /* previous audio timestamp */

  /** realaudio descrambling */
  int sub_packet_size; ///< sub packet size, per stream
  int sub_packet_h; ///< number of coded frames per block
  int coded_framesize; ///< coded frame size, per stream
  int audiopk_size; ///< audio packet size
  unsigned char *audio_buf; ///< place to store reordered audio data
  float *audio_timestamp; ///< timestamp for each audio packet
  int sub_packet_cnt; ///< number of subpacket already received
  int audio_filepos; ///< file position of first audio packet in block

  /* stuff for quicktime */
  int fix_i_bps;
  float qt_last_a_pts;

  int subtitle_type;

  /* The timecodes of video frames might have to be reordered if they're
     in display order (the timecodes, not the frames themselves!). In this
     case demux packets have to be cached with the help of these variables. */
  int reorder_timecodes;
  demux_packet_t **cached_dps;
  int num_cached_dps, num_allocated_dps;
  float max_pts;

  /* generic content encoding support */
  mkv_content_encoding_t *encodings;
  int num_encodings;

  /* For VobSubs and SSA/ASS */
  sh_sub_t *sh_sub;
} mkv_track_t;

typedef struct mkv_index
{
  int tnum;
  uint64_t timecode, filepos;
} mkv_index_t;

typedef struct mkv_demuxer
{
  off_t segment_start;

  float duration, last_pts;
  uint64_t last_filepos;

  mkv_track_t **tracks;
  int num_tracks;

  uint64_t tc_scale, cluster_tc, first_tc;
  int has_first_tc;

  uint64_t cluster_size;
  uint64_t blockgroup_size;

  mkv_index_t *indexes;
  int num_indexes;

  off_t *parsed_cues;
  int parsed_cues_num;
  off_t *parsed_seekhead;
  int parsed_seekhead_num;

  uint64_t *cluster_positions;
  int num_cluster_pos;

  int64_t skip_to_timecode;
  int v_skip_to_keyframe, a_skip_to_keyframe;

  int64_t stop_timecode;

  int last_aid;
  int audio_tracks[MAX_A_STREAMS];
} mkv_demuxer_t;

#define REALHEADER_SIZE    16
#define RVPROPERTIES_SIZE  34
#define RAPROPERTIES4_SIZE 56
#define RAPROPERTIES5_SIZE 70

/* for e.g. "-slang ger" */
extern char *dvdsub_lang;
extern char *audio_lang;
extern int dvdsub_id;

#define get_b_frames		//Fuchun 2010.05.26

#include "mpeg_hdr.h"

extern int num_reorder_frames;
extern int queue_frames;
extern unsigned int dts_packet;

#define getbits mp_getbits
#define min(a, b) ((a) <= (b) ? (a) : (b))

unsigned int read_golomb(unsigned char *buffer, unsigned int *init)
{
	unsigned int x, v = 0, v2 = 0, m, len = 0, n = *init;

	while(getbits(buffer, n++, 1) == 0)
		len++;

	x = len + n;
	while(n < x)
	{
		m = min(x - n, 8);
		v |= getbits(buffer, n, m);
		n += m;
		if(x - n > 8)
			v <<= 8;
	}

	v2 = 1;
	for(n = 0; n < len; n++)
		v2 <<= 1;
	v2 = (v2 - 1) + v;

	*init = x;
	return v2;
}

inline int read_golomb_s(unsigned char *buffer, unsigned int *init)
{
	unsigned int v = read_golomb(buffer, init);
	return (v & 1) ? ((v + 1) >> 1) : -(v >> 1);
}

int mp_unescape03(unsigned char *buf, int len)
{
	unsigned char *dest;
	int i, j, skip;

	dest = malloc(len);
	if(! dest)
		return 0;

	j = i = skip = 0;
	while(i <= len-3)
	{
		if(buf[i] == 0 && buf[i+1] == 0 && buf[i+2] == 3)
		{
			dest[j] = dest[j+1] = 0;
			j += 2;
			i += 3;
			skip++;
		}
		else
		{
			dest[j] = buf[i];
			j++;
			i++;
		}
	}
	dest[j] = buf[len-2];
	dest[j+1] = buf[len-1];
	len -= skip;
	memcpy(buf, dest, len);
	free(dest);

	return len;
}

#ifdef get_b_frames
extern int h264_frame_mbs_only;
extern int pic_struct_present_flag;
extern int nal_hrd_parameters_present_flag;
extern int vcl_hrd_parameters_present_flag;
extern int cpb_removal_delay_length;
extern int dpb_output_delay_length;
int decode_264_sps_mkv(unsigned char *src_buf, int len)
{
	unsigned int n = 0, v, i, k, reorder_frames = 0;
	unsigned char *buf = malloc(len);
	memcpy(buf, src_buf, len);
	len = mp_unescape03(buf, len);
	unsigned int num_ref_frames = 0;

	//Fuchun 2010.07.01
  	nal_hrd_parameters_present_flag = 0;
	vcl_hrd_parameters_present_flag = 0;
	cpb_removal_delay_length = 0;
	dpb_output_delay_length = 0;

	n = 24;
	read_golomb(buf, &n);
	if(buf[0] >= 100)
	{
		if(read_golomb(buf, &n) == 3)
			n++;
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		n++;
		if(getbits(buf, n++, 1))
		{
			for(i = 0; i < 8; i++)
			{
				if(getbits(buf, n++, 1))
				{
					v = 8;
					for(k = (i < 6 ? 16 : 64); k && v; k--)
						v = (v + read_golomb_s(buf, &n)) & 255;
				}
			}
		}
	}

	read_golomb(buf, &n);
	v = read_golomb(buf, &n);
	if(v == 0)
		read_golomb(buf, &n);
	else if(v == 1)
	{
		getbits(buf, n++, 1);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		v = read_golomb(buf, &n);
		for(i = 0; i < v; i++)
			read_golomb(buf, &n);
	}

	num_ref_frames = read_golomb(buf, &n);
	getbits(buf, n++, 1);
	read_golomb(buf, &n);
	read_golomb(buf, &n);
	if(!getbits(buf, n++, 1))
		getbits(buf, n++, 1);
	getbits(buf, n++, 1);

	if(getbits(buf, n++, 1))
	{
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
	}

	if(getbits(buf, n++, 1))
	{
		if(getbits(buf, n++, 1))	//aspect_ratio_information
		{
			if(getbits(buf, n, 8) == 255)
				n += 40;
			else
				n += 8;
		}

		if(getbits(buf, n++, 1))	//overscan
			n++;

		if(getbits(buf, n++, 1))	//vsp_color
		{
			n += 4;
			if(getbits(buf, n++, 1))
				n += 24;
		}

		if(getbits(buf, n++, 1))	//chroma
		{
			read_golomb(buf, &n);
			read_golomb(buf, &n);
		}

		if(getbits(buf, n++, 1))	//timing
		{
			n += 64;
			getbits(buf, n++, 1);	//fixed_fps
		}

		nal_hrd_parameters_present_flag = getbits(buf, n++, 1);
		if(nal_hrd_parameters_present_flag)
		{
			v = read_golomb(buf, &n) + 1;
			n += 8;
			for(i = 0; i < v; i++)
			{
				read_golomb(buf, &n);
				read_golomb(buf, &n);
				n++;
			}
			n += 5;
			cpb_removal_delay_length = getbits(buf, n, 5)+1;
			n += 5;
			dpb_output_delay_length = getbits(buf, n, 5)+ 1;
			n += 10;
		}
		vcl_hrd_parameters_present_flag = getbits(buf, n++, 1);
		if(vcl_hrd_parameters_present_flag)
		{
			v = read_golomb(buf, &n) + 1;
			n += 8;
			for(i = 0; i < v; i++)
			{
				read_golomb(buf, &n);
				read_golomb(buf, &n);
				n++;
			}
			n += 5;
			cpb_removal_delay_length = getbits(buf, n, 5)+1;
			n += 5;
			dpb_output_delay_length = getbits(buf, n, 5)+ 1;
			n += 10;
		}
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
			n++;
		pic_struct_present_flag = getbits(buf, n++, 1);
		
		if(getbits(buf, n++, 1))	//bitstream_restriction_flag
		{
			getbits(buf, n++, 1);
			read_golomb(buf, &n);
			read_golomb(buf, &n);
			read_golomb(buf, &n);
			read_golomb(buf, &n);
			reorder_frames = read_golomb(buf, &n);
			read_golomb(buf, &n);
		}
	}
	if(buf)
		free(buf);
	printf("nal_hrd_parameters_present_flag:%d\n", nal_hrd_parameters_present_flag);
	printf("vcl_hrd_parameters_present_flag:%d\n", vcl_hrd_parameters_present_flag);
	printf("cpb_removal_delay_length:%d\n", cpb_removal_delay_length);
	printf("dpb_output_delay_length:%d\n", dpb_output_delay_length);
	printf("pic_struct_present_flag:%d\n", pic_struct_present_flag);

	return (reorder_frames ? reorder_frames : num_ref_frames);	//Fuchun 20110907 use num_ref_frames instead of reorder_frames while reorder_frames == 0
}

int h264_parse_sei_mkv(unsigned char *buf, int len)	//Fuchun 20110915
{
	int n = 0;
	int sei_pic_struct;
	unsigned char *temp_buf = malloc(len);
	memcpy(temp_buf, buf, len);
	len = mp_unescape03(&temp_buf[n], len);
	while(n + 16 < len*8)
	{
		int size, type;

		type = 0;
		do
		{
			type += getbits(temp_buf, n, 8);
			n += 8;
		}while(getbits(temp_buf, n-8, 8) == 255);

		size = 0;
		do
		{
			size += getbits(temp_buf, n, 8);
			n += 8;
		}while(getbits(temp_buf, n-8, 8) == 255);

		switch(type)
		{
			case 1:		//SEI_TYPE_PIC_TIMING
				if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
					n+=(cpb_removal_delay_length + dpb_output_delay_length);
				sei_pic_struct = getbits(temp_buf, n, 4);
				printf("=== H264 sei_pic_struct = %d ===\n", sei_pic_struct);
				if(sei_pic_struct == 1 || sei_pic_struct == 2) 
				{
					h264_frame_mbs_only = 0;	//if sei_pic_struct = SEI_PIC_STRUCT_TOP_FIELD or SEI_PIC_STRUCT_BOTTOM_FIELD

					if(temp_buf)
						free(temp_buf);
					return 2;
				}

				if(temp_buf)
					free(temp_buf);
				return 1;
			default:
				n += 8*size;
				break;
		}
	}

	if(temp_buf)
		free(temp_buf);

	return 0;
}
#endif

/**
 * \brief ensures there is space for at least one additional element
 * \param arrayp array to grow
 * \param nelem current number of elements in array
 * \param elsize size of one array element
 */
static void av_noinline grow_array(void *arrayp, int nelem, size_t elsize) {
  void **array = arrayp;
  void *oldp = *array;
  if (nelem & 31)
    return;
  if (nelem > UINT_MAX / elsize - 32)
    *array = NULL;
  else
    *array = realloc(*array, (nelem + 32) * elsize);
  if (!*array)
    free(oldp);
}

static mkv_track_t *
demux_mkv_find_track_by_num (mkv_demuxer_t *d, int n, int type)
{
  int i, id;

  for (i=0, id=0; i < d->num_tracks; i++)
    if (d->tracks[i] != NULL && d->tracks[i]->type == type)
      if (id++ == n)
        return d->tracks[i];

  return NULL;
}

static void
add_cluster_position (mkv_demuxer_t *mkv_d, uint64_t position)
{
  int i = mkv_d->num_cluster_pos;

  while (i--)
    if (mkv_d->cluster_positions[i] == position)
      return;

  grow_array(&mkv_d->cluster_positions, mkv_d->num_cluster_pos,
             sizeof(uint64_t));
  if (!mkv_d->cluster_positions) {
    mkv_d->num_cluster_pos = 0;
    return;
  }
  mkv_d->cluster_positions[mkv_d->num_cluster_pos++] = position;
}


#define AAC_SYNC_EXTENSION_TYPE 0x02b7
static int
aac_get_sample_rate_index (uint32_t sample_rate)
{
  static const int srates[] = {92017, 75132, 55426, 46009, 37566, 27713, 23004, 18783, 13856, 11502, 9391, 0};
  int i = 0;
  while (sample_rate < srates[i]) i++;
  return i;
}

/** \brief Free cached demux packets
 *
 * Reordering the timecodes requires caching of demux packets. This function
 * frees all these cached packets and the memory for the cached pointers
 * itself.
 *
 * \param demuxer The demuxer for which the cache is to be freed.
 */
static void
free_cached_dps (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  mkv_track_t *track;
  int i, k;

  for (k = 0; k < mkv_d->num_tracks; k++)
    {
      track = mkv_d->tracks[k];
      for (i = 0; i < track->num_cached_dps; i++)
        free_demux_packet (track->cached_dps[i]);
      free(track->cached_dps);
      track->cached_dps = NULL;
      track->num_cached_dps = 0;
      track->num_allocated_dps = 0;
      track->max_pts = 0;
    }
}

static int
demux_mkv_decode (mkv_track_t *track, uint8_t *src, uint8_t **dest,
                  uint32_t *size, uint32_t type)
{
 
  int i, result;
  int modified = 0;

  *dest = src;
  if (track->num_encodings <= 0)
    return 0;

  for (i=0; i<track->num_encodings; i++)
    {
      if (!(track->encodings[i].scope & type))
        continue;

#if CONFIG_ZLIB
      if (track->encodings[i].comp_algo == 0)
        {
          /* zlib encoded track */
          z_stream zstream;

          zstream.zalloc = (alloc_func) 0;
          zstream.zfree = (free_func) 0;
          zstream.opaque = (voidpf) 0;
          if (inflateInit (&zstream) != Z_OK)
            {
              mp_msg (MSGT_DEMUX, MSGL_WARN,
                      MSGTR_MPDEMUX_MKV_ZlibInitializationFailed);
              return modified;
            }
          zstream.next_in = (Bytef *) src;
          zstream.avail_in = *size;

          modified = 1;
          *dest = NULL;
          zstream.avail_out = *size;
          do {
            *size += 4000;
            *dest = realloc (*dest, *size);
            zstream.next_out = (Bytef *) (*dest + zstream.total_out);
            result = inflate (&zstream, Z_NO_FLUSH);
            if (result != Z_OK && result != Z_STREAM_END)
              {
                mp_msg (MSGT_DEMUX, MSGL_WARN,
                        MSGTR_MPDEMUX_MKV_ZlibDecompressionFailed);
                free(*dest);
                *dest = NULL;
                inflateEnd (&zstream);
                return modified;
              }
            zstream.avail_out += 4000;
          } while (zstream.avail_out == 4000 &&
                   zstream.avail_in != 0 && result != Z_STREAM_END);

          *size = zstream.total_out;
          inflateEnd (&zstream);
        }
#endif
      if (track->encodings[i].comp_algo == 2)
        {
          /* lzo encoded track */
          int dstlen = *size * 3;

          *dest = NULL;
          while (1)
            {
              int srclen = *size;
              if (dstlen > SIZE_MAX - AV_LZO_OUTPUT_PADDING) goto lzo_fail;
              *dest = realloc (*dest, dstlen + AV_LZO_OUTPUT_PADDING);
              result = av_lzo1x_decode (*dest, &dstlen, src, &srclen);
              if (result == 0)
                break;
              if (!(result & AV_LZO_OUTPUT_FULL))
                {
lzo_fail:
                  mp_msg (MSGT_DEMUX, MSGL_WARN,
                          MSGTR_MPDEMUX_MKV_LzoDecompressionFailed);
                  free(*dest);
                  *dest = NULL;
                  return modified;
                }
              mp_msg (MSGT_DEMUX, MSGL_DBG2,
                      "[mkv] lzo decompression buffer too small.\n");
              dstlen *= 2;
            }
          *size = dstlen;
        }
    }

  return modified;
}


static int
demux_mkv_read_info (demuxer_t *demuxer)
{

  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;
  uint64_t tc_scale = 1000000;
  long double duration = 0.;

  length = ebml_read_length (s, NULL);
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TIMECODESCALE:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 1;
            tc_scale = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + timecode scale: %"PRIu64"\n",
                    tc_scale);
            break;
          }

        case MATROSKA_ID_DURATION:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 1;
            duration = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + duration: %.3Lfs\n",
                    duration * tc_scale / 1000000000.0);
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }
      length -= l + il;
    }
  mkv_d->tc_scale = tc_scale;
  mkv_d->duration = duration * tc_scale / 1000000000.0;
  return 0;
}

/**
 * \brief free array of kv_content_encoding_t
 * \param encodings pointer to array
 * \param numencodings number of encodings in array
 */
static void
demux_mkv_free_encodings(mkv_content_encoding_t *encodings, int numencodings)
{
  while (numencodings-- > 0)
    free(encodings[numencodings].comp_settings);
  free(encodings);
}

static int
demux_mkv_read_trackencodings (demuxer_t *demuxer, mkv_track_t *track)
{
  stream_t *s = demuxer->stream;
  mkv_content_encoding_t *ce, e;
  uint64_t len, length, l;
  int il, n;

  ce = malloc (sizeof (*ce));
  n = 0;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_CONTENTENCODING:
          {
            uint64_t len;
            int i;

            memset (&e, 0, sizeof (e));
            e.scope = 1;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t num, l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CONTENTENCODINGORDER:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.order = num;
                    break;

                  case MATROSKA_ID_CONTENTENCODINGSCOPE:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.scope = num;
                    break;

                  case MATROSKA_ID_CONTENTENCODINGTYPE:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.type = num;
                    break;

                  case MATROSKA_ID_CONTENTCOMPRESSION:
                    {
                      uint64_t le;

                      le = ebml_read_length (s, &i);
                      l = le + i;

                      while (le > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CONTENTCOMPALGO:
                              num = ebml_read_uint (s, &l);
                              if (num == EBML_UINT_INVALID)
                                goto err_out;
                              e.comp_algo = num;
                              break;

                            case MATROSKA_ID_CONTENTCOMPSETTINGS:
                              l = ebml_read_length (s, &i);
                              e.comp_settings = malloc (l);
                              stream_read (s, e.comp_settings, l);
                              e.comp_settings_len = l;
                              l += i;
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          le -= l + il;
                        }

                      if (e.type == 1)
                        {
                          mp_msg(MSGT_DEMUX, MSGL_WARN,
                                 MSGTR_MPDEMUX_MKV_TrackEncrypted, track->tnum);
                        }
                      else if (e.type != 0)
                        {
                          mp_msg(MSGT_DEMUX, MSGL_WARN,
                                 MSGTR_MPDEMUX_MKV_UnknownContentEncoding, track->tnum);
                        }

                      if (e.comp_algo != 0 && e.comp_algo != 2)
                        {
                          mp_msg (MSGT_DEMUX, MSGL_WARN,
                                  MSGTR_MPDEMUX_MKV_UnknownCompression,
                                  track->tnum, e.comp_algo);
                        }
#if !CONFIG_ZLIB
                      else if (e.comp_algo == 0)
                        {
                          mp_msg (MSGT_DEMUX, MSGL_WARN,
                                  MSGTR_MPDEMUX_MKV_ZlibCompressionUnsupported,
                                  track->tnum);
                        }
#endif

                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            for (i=0; i<n; i++)
              if (e.order <= ce[i].order)
                break;
            ce = realloc (ce, (n+1) *sizeof (*ce));
            memmove (ce+i+1, ce+i, (n-i) * sizeof (*ce));
            memcpy (ce+i, &e, sizeof (e));
            n++;
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;
    }

  track->encodings = ce;
  track->num_encodings = n;
  return len;

err_out:
  demux_mkv_free_encodings(ce, n);
  return 0;
}

static int
demux_mkv_read_trackaudio (demuxer_t *demuxer, mkv_track_t *track)
{
  stream_t *s = demuxer->stream;
  uint64_t len, length, l;
  int il;

  track->a_sfreq = 8000.0;
  track->a_channels = 1;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_AUDIOSAMPLINGFREQ:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 0;
            track->a_sfreq = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Sampling frequency: %f\n",
                    track->a_sfreq);
            break;
          }

        case MATROSKA_ID_AUDIOBITDEPTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->a_bps = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Bit depth: %u\n",
                    track->a_bps);
            break;
          }

        case MATROSKA_ID_AUDIOCHANNELS:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->a_channels = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Channels: %u\n",
                    track->a_channels);
            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return len;
}

static int
demux_mkv_read_trackvideo (demuxer_t *demuxer, mkv_track_t *track)
{
 
  stream_t *s = demuxer->stream;
  uint64_t len, length, l;
  int il;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_VIDEOFRAMERATE:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 0;
            track->v_frate = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Frame rate: %f\n",
                    track->v_frate);
            if (track->v_frate > 0)
              track->default_duration = 1 / track->v_frate;
            break;
          }

        case MATROSKA_ID_VIDEODISPLAYWIDTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_dwidth = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Display width: %u\n",
                    track->v_dwidth);
            break;
          }

        case MATROSKA_ID_VIDEODISPLAYHEIGHT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_dheight = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Display height: %u\n",
                    track->v_dheight);
            break;
          }

        case MATROSKA_ID_VIDEOPIXELWIDTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_width = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Pixel width: %u\n",
                    track->v_width);
            break;
          }

        case MATROSKA_ID_VIDEOPIXELHEIGHT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_height = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Pixel height: %u\n",
                    track->v_height);
            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return len;
}

/**
 * \brief free any data associated with given track
 * \param track track of which to free data
 */
static void
demux_mkv_free_trackentry(mkv_track_t *track) {
#if 1	//Barry 2010-09-27
	if (track->name)
		free (track->name);
	if (track->codec_id)
		free (track->codec_id);
	if (track->language)
		free (track->language);
	if (track->private_data)
		free (track->private_data);
	if (track->audio_buf)
		free (track->audio_buf);
	if (track->audio_timestamp)
		free (track->audio_timestamp);
#else
  free (track->name);
  free (track->codec_id);
  free (track->language);
  free (track->private_data);
  free (track->audio_buf);
  free (track->audio_timestamp);
#endif
  demux_mkv_free_encodings(track->encodings, track->num_encodings);
  free(track);
}

static int
demux_mkv_read_trackentry (demuxer_t *demuxer)
{

  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  mkv_track_t *track;
  uint64_t len, length, l;
  int il;

  Mincache = Maxcache = 0;	//Barry 2010-12-30

  track = calloc (1, sizeof (*track));
  /* set default values */
  track->default_track = 1;
  track->name = 0;
  track->language = strdup("eng");

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TRACKNUMBER:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            track->tnum = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Track number: %u\n",
                    track->tnum);
            break;
          }

        case MATROSKA_ID_TRACKNAME:
          {
            track->name = ebml_read_utf8 (s, &l);
            if (track->name == NULL)
              goto err_out;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Name: %s\n",
                    track->name);
            break;
          }

        case MATROSKA_ID_TRACKTYPE:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->type = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Track type: ");
            switch (track->type)
              {
              case MATROSKA_TRACK_AUDIO:
                mp_msg (MSGT_DEMUX, MSGL_V, "Audio\n");
                break;
              case MATROSKA_TRACK_VIDEO:
                mp_msg (MSGT_DEMUX, MSGL_V, "Video\n");
                break;
              case MATROSKA_TRACK_SUBTITLE:
                mp_msg (MSGT_DEMUX, MSGL_V, "Subtitle\n");
                break;
              default:
                mp_msg (MSGT_DEMUX, MSGL_V, "unknown\n");
                break;
            }
            break;
          }

        case MATROSKA_ID_TRACKAUDIO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Audio track\n");
          l = demux_mkv_read_trackaudio (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

        case MATROSKA_ID_TRACKVIDEO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Video track\n");
          l = demux_mkv_read_trackvideo (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

        case MATROSKA_ID_CODECID:
          track->codec_id = ebml_read_ascii (s, &l);
          if (track->codec_id == NULL)
            goto err_out;
          if (!strcmp (track->codec_id, MKV_V_MSCOMP) ||
              !strcmp (track->codec_id, MKV_A_ACM))
            track->ms_compat = 1;
          else if (!strcmp (track->codec_id, MKV_S_VOBSUB))
            track->subtitle_type = MATROSKA_SUBTYPE_VOBSUB;
          else if (!strcmp (track->codec_id, MKV_S_TEXTSSA)
                   || !strcmp (track->codec_id, MKV_S_TEXTASS)
                   || !strcmp (track->codec_id, MKV_S_SSA)
                   || !strcmp (track->codec_id, MKV_S_ASS))
            {
              track->subtitle_type = MATROSKA_SUBTYPE_SSA;
            }
          else if (!strcmp (track->codec_id, MKV_S_TEXTASCII))
            track->subtitle_type = MATROSKA_SUBTYPE_TEXT;
          if (!strcmp (track->codec_id, MKV_S_TEXTUTF8))
            {
              track->subtitle_type = MATROSKA_SUBTYPE_TEXT;
            }
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Codec ID: %s\n",
                  track->codec_id);
          break;

        case MATROSKA_ID_CODECPRIVATE:
          {
            int x;
            uint64_t num = ebml_read_length (s, &x);
	    // audit: cheap guard against overflows later..
	    if (num > SIZE_MAX - 1000) return 0;
            l = x + num;
            track->private_data = malloc (num + AV_LZO_INPUT_PADDING);
            if (stream_read(s, track->private_data, num) != (int) num)
              goto err_out;
            track->private_size = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + CodecPrivate, length "
                    "%u\n", track->private_size);

		unsigned char*  ptr, *hdr;
		ptr = (unsigned char *)track->private_data;

		uint32_t tmp_fourcc;
		if (track->private_size > 20)
			tmp_fourcc = ptr[16] | (ptr[17]<<8) | (ptr[18]<<16) | (ptr[19]<<24);

#if 1	// Raymond 2009/05/11
		if( track->type == MATROSKA_TRACK_VIDEO &&
			(!strcmp(track->codec_id,MKV_V_MPEG4_AVC) ||
			( !strcmp(track->codec_id,MKV_V_MSCOMP) && tmp_fourcc ==mmioFOURCC('H','2','6','4') )	//Barry 2010-11-03
			)
		)
		{
			int write_len=0;

			//Barry 2010-07-09
			avc1_nal_bytes = (ptr[4] & 0x3)+1;
			printf("avc1_nal_bytes=%d\n", avc1_nal_bytes);
			if (avc1_nal_bytes == 0)
				printf("[mkv - avc1] NAL unit %d bytes, Fix me later!\n", avc1_nal_bytes);

			ptr = (unsigned char *)track->private_data + 7;	//skip 01 64 00 29 FF E1 00 and seek to first header len pos.

			avc1_header = malloc (track->private_size + AV_LZO_INPUT_PADDING);
			hdr = avc1_header;

			write_len = *ptr++; //get first header len

#ifdef get_b_frames	//Fuchun 2010.05.26
                     if(tmp_fourcc == mmioFOURCC('a','v','c','1') ) //Polun 2011-10-27 fixed mantis 6417 bSDF Macross - Dual - 01_MKV_H264_MP4a_DualAud_IntSSA.mkv can't play. 
                         num_reorder_frames = decode_264_sps_mkv(ptr+1, write_len-1);
			printf("num_reorder_frames %d\n", num_reorder_frames);
			if(num_reorder_frames != 0) queue_frames = num_reorder_frames;
#endif

			hdr[0] = hdr[1] = hdr[2] = 0;
			hdr[3] = (unsigned char)write_len;
			hdr += 4;
			
			memcpy(hdr, ptr, write_len);
			hdr += write_len;
			ptr += write_len;

			avc1_header_size += 4 + write_len;
                
			printf("1write_len[%d]\n",write_len);//SkyMedi_Vincent
                
			ptr += 2;   //skip 01 00 and seek to second header len pos.
			write_len = *ptr++; //get second header len

			hdr[0] = hdr[1] = hdr[2] = 0;
			hdr[3] = (unsigned char)write_len;
			hdr += 4;
			
			memcpy(hdr, ptr, write_len);

			avc1_header_size += 4 + write_len;
			
			printf("2write_len[%d]\n",write_len);//SkyMedi_Vincent
			printf("avc1_header_size = %d\n", avc1_header_size);
			{
				int i = 0;
				for( i = 0 ; i< avc1_header_size; i++)
					printf("%02X ", avc1_header[i]);	
				printf("\n");
			}

			//Barry 2011-03-25
			if (!(!strcmp(track->codec_id,MKV_V_MSCOMP) && (avc1_header_size-5) <= 3))
			{
				if (!check_avc1_sps_bank0(avc1_header+5, avc1_header_size-5))
					goto err_out;
			}
			else
				printf("******  MKV: SPS len [%d], not enough to analysis\n", avc1_header_size-5);

		}
#endif
#if 1	// Raymond 2009/06/01
		else if(track->type == MATROSKA_TRACK_VIDEO && 
                    !strcmp(track->codec_id,MKV_V_REALV40))
		{
			rv40_header_size = track->private_size;
			printf("rv40_header_size = %d\n", rv40_header_size);
		}
#endif					
            break;
          }

        case MATROSKA_ID_TRACKLANGUAGE:
          if (track->language)
		  	free(track->language);
          track->language = ebml_read_utf8 (s, &l);
          if (track->language == NULL)
            goto err_out;
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Language: %s\n",
                  track->language);
          break;

        case MATROSKA_ID_TRACKFLAGDEFAULT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            track->default_track = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default flag: %u\n",
                    track->default_track);
            break;
          }

        case MATROSKA_ID_TRACKDEFAULTDURATION:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            if (num == 0)
              mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default duration: 0");
            else
              {
                track->v_frate = 1000000000.0 / num;
                track->default_duration = num / 1000000000.0;
                mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default duration: "
                        "%.3fms ( = %.3f fps)\n",num/1000000.0,track->v_frate);
              }
            break;
          }

        case MATROSKA_ID_TRACKENCODINGS:
          l = demux_mkv_read_trackencodings (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

#if 1	//Barry 2010-12-30
        case MATROSKA_ID_TRACKMINCACHE:
        {
            int num = stream_read_char(s);
            if (num == 0x81)
            {
            		Mincache = stream_read_char(s);
			l = 2;
//			printf("Mincache= %d\n", Mincache);
            }
            else
			printf("MATROSKA_ID_TRACKMINCACHE  =======> size=%d\n", num);
            break;
        }

        case MATROSKA_ID_TRACKMAXCACHE:
        {
            int num = stream_read_char(s);
            if (num == 0x81)
            {
            		Maxcache = stream_read_char(s);
			l = 2;
//			printf("Maxcache= %d\n", Maxcache);
            }
            else
			printf("MATROSKA_ID_TRACKMAXCACHE  =======> size=%d\n", num);
            break;
        }
#endif

        default:
          ebml_read_skip (s, &l);
          break;
        }
      length -= l + il;
    }

  mkv_d->tracks[mkv_d->num_tracks++] = track;
  return len;

err_out:
  demux_mkv_free_trackentry(track);
  return 0;
}

static int
demux_mkv_read_tracks (demuxer_t *demuxer)
{
 
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;

  mkv_d->tracks = malloc (sizeof (*mkv_d->tracks));
  mkv_d->num_tracks = 0;

  length = ebml_read_length (s, NULL);
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TRACKENTRY:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + a track...\n");
          mkv_d->tracks = realloc (mkv_d->tracks,
                                   (mkv_d->num_tracks+1)
                                   *sizeof (*mkv_d->tracks));
          l = demux_mkv_read_trackentry (demuxer);
          if (l == 0)
            return 1;
          break;

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return 0;
}

static int
demux_mkv_read_cues (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l, time, track, pos;
  off_t off;
  int i, il;

  if (index_mode == 0) {
    ebml_read_skip (s, NULL);
    return 0;
  }
  off = stream_tell (s);
  for (i=0; i<mkv_d->parsed_cues_num; i++)
    if (mkv_d->parsed_cues[i] == off)
      {
        ebml_read_skip (s, NULL);
        return 0;
      }
  mkv_d->parsed_cues = realloc (mkv_d->parsed_cues,
                                (mkv_d->parsed_cues_num+1)
                                * sizeof (off_t));
  mkv_d->parsed_cues[mkv_d->parsed_cues_num++] = off;

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing cues ] -----------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      time = track = pos = EBML_UINT_INVALID;

      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_POINTENTRY:
          {
            uint64_t len;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CUETIME:
                    time = ebml_read_uint (s, &l);
                    break;

                  case MATROSKA_ID_CUETRACKPOSITION:
                    {
                      uint64_t le;

                      le = ebml_read_length (s, &i);
                      l = le + i;

                      while (le > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CUETRACK:
                              track = ebml_read_uint (s, &l);
                              break;

                            case MATROSKA_ID_CUECLUSTERPOSITION:
                              pos = ebml_read_uint (s, &l);
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          le -= l + il;
                        }
                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;

      if (time != EBML_UINT_INVALID && track != EBML_UINT_INVALID
          && pos != EBML_UINT_INVALID)
        {
          grow_array(&mkv_d->indexes, mkv_d->num_indexes, sizeof(mkv_index_t));
          if (!mkv_d->indexes) {
            mkv_d->num_indexes = 0;
            break;
          }
          mkv_d->indexes[mkv_d->num_indexes].tnum = track;
          mkv_d->indexes[mkv_d->num_indexes].timecode = time;
          mkv_d->indexes[mkv_d->num_indexes].filepos =mkv_d->segment_start+pos;
          mp_msg (MSGT_DEMUX, MSGL_DBG2, "[mkv] |+ found cue point "
                  "for track %"PRIu64": timecode %"PRIu64", filepos: %"PRIu64"\n",
                  track, time, mkv_d->segment_start + pos);
          mkv_d->num_indexes++;
        }
    }

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing cues ] -----------\n");
  return 0;
}

static int
demux_mkv_read_chapters (demuxer_t *demuxer)
{

  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il, jj=0;

  if (demuxer->chapters)
    {
      ebml_read_skip (s, NULL);
      return 0;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing chapters ] ---------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_EDITIONENTRY:
          {
            uint64_t len;
            int i;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CHAPTERATOM:
                    {
                      uint64_t len, start=0, end=0;
                      char* name = 0;
                      int i;
                      int cid;

                      len = ebml_read_length (s, &i);
                      l = len + i;

                      while (len > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CHAPTERTIMESTART:
                              start = ebml_read_uint (s, &l) / 1000000;
                              break;

                            case MATROSKA_ID_CHAPTERTIMEEND:
                              end = ebml_read_uint (s, &l) / 1000000;
                              break;

                            case MATROSKA_ID_CHAPTERDISPLAY:
                              {
                                uint64_t len;
                                int i;

                                len = ebml_read_length (s, &i);
                                l = len + i;
                                while (len > 0)
                                  {
                                    uint64_t l;
                                    int il;

                                    switch (ebml_read_id (s, &il))
                                      {
                                        case MATROSKA_ID_CHAPSTRING:
                                          name = ebml_read_utf8 (s, &l);
                                          break;
                                        default:
                                          ebml_read_skip (s, &l);
                                          break;
                                      }
                                    len -= l + il;
                                  }
                              }
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          len -= l + il;
                        }

                      if (!name)
                        name = strdup("(unnamed)");

#if 1	//Barry 2011-09-15 fix mantis: 6119
			for (jj=0;jj<demuxer->num_chapters;jj++)
			{
				if (start == demuxer->chapters[jj].start && end == demuxer->chapters[jj].end && !strcmp(name, demuxer->chapters[jj].name))
					break;
			}
			if (jj == demuxer->num_chapters)
			{
				cid = demuxer_add_chapter(demuxer, name, start, end);
				//printf("#### [%s - %d]  jj=%d  cid = %d\n", __func__, __LINE__, jj, cid);
			}
#else
			cid = demuxer_add_chapter(demuxer, name, start, end);
#endif

                      mp_msg(MSGT_DEMUX, MSGL_V,
                             "[mkv] Chapter %u from %02d:%02d:%02d."
                             "%03d to %02d:%02d:%02d.%03d, %s\n",
                             cid,
                             (int) (start / 60 / 60 / 1000),
                             (int) ((start / 60 / 1000) % 60),
                             (int) ((start / 1000) % 60),
                             (int) (start % 1000),
                             (int) (end / 60 / 60 / 1000),
                             (int) ((end / 60 / 1000) % 60),
                             (int) ((end / 1000) % 60),
                             (int) (end % 1000), name);

                      free(name);
                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing chapters ] ---------\n");
  return 0;
}

static int
demux_mkv_read_tags (demuxer_t *demuxer)
{

  ebml_read_skip (demuxer->stream, NULL);
  return 0;
}

static int
demux_mkv_read_attachments (demuxer_t *demuxer)
{
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing attachments ] ---------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
          case MATROSKA_ID_ATTACHEDFILE:
            {
              uint64_t len;
              int i;
              char* name = NULL;
              char* mime = NULL;
              char* data = NULL;
              int data_size = 0;

              len = ebml_read_length (s, &i);
              l = len + i;

              mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + an attachment...\n");

              while (len > 0)
                {
                  uint64_t l;
                  int il;

                  switch (ebml_read_id (s, &il))
                    {
                    case MATROSKA_ID_FILENAME:
                      name = ebml_read_utf8 (s, &l);
                      if (name == NULL)
                        return 0;
                      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileName: %s\n",
                        name);
                      break;

                    case MATROSKA_ID_FILEMIMETYPE:
                      mime = ebml_read_ascii (s, &l);
                      if (mime == NULL)
                        return 0;
                      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileMimeType: %s\n",
                        mime);
                      break;

#if 0	//Barry 2010-06-08	don't loading true type font
                    case MATROSKA_ID_FILEDATA:
                      {
                        int x;
                        uint64_t num = ebml_read_length (s, &x);
                        l = x + num;
                        free(data);
                        data = malloc (num);
                        if (stream_read(s, data, num) != (int) num)
                        {
                          free(data);
                          return 0;
                        }
                        data_size = num;
                        mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileData, length "
                                "%u\n", data_size);
                        break;
                      }
#endif

                    default:
                      ebml_read_skip (s, &l);
                      break;
                    }
                  len -= l + il;
                }

              demuxer_add_attachment(demuxer, name, mime, data, data_size);
              mp_msg(MSGT_DEMUX, MSGL_V,
                     "[mkv] Attachment: %s, %s, %u bytes\n",
                     name, mime, data_size);
              break;
            }

          default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing attachments ] ---------\n");
  return 0;
}

static int
demux_mkv_read_seekhead (demuxer_t *demuxer)
{

  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l, seek_pos, saved_pos, num;
  uint32_t seek_id;
  int i, il, res = 0;
  off_t off;

  off = stream_tell (s);
  for (i=0; i<mkv_d->parsed_seekhead_num; i++)
    if (mkv_d->parsed_seekhead[i] == off)
      {
        ebml_read_skip (s, NULL);
        return 0;
      }
  mkv_d->parsed_seekhead = realloc (mkv_d->parsed_seekhead,
                                    (mkv_d->parsed_seekhead_num+1)
                                    * sizeof (off_t));
  mkv_d->parsed_seekhead[mkv_d->parsed_seekhead_num++] = off;

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing seek head ] ---------\n");
  length = ebml_read_length (s, NULL);
  /* off now holds the position of the next element after the seek head. */
  off = stream_tell (s) + length;
  while (length > 0 && !res)
    {

      seek_id = 0;
      seek_pos = EBML_UINT_INVALID;

      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_SEEKENTRY:
          {
            uint64_t len;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_SEEKID:
                    num = ebml_read_uint (s, &l);
                    if (num != EBML_UINT_INVALID)
                      seek_id = num;
                    break;

                  case MATROSKA_ID_SEEKPOSITION:
                    seek_pos = ebml_read_uint (s, &l);
                    break;

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }

            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;

      if (seek_id == 0 || seek_id == MATROSKA_ID_CLUSTER
          || seek_pos == EBML_UINT_INVALID ||
          ((mkv_d->segment_start + seek_pos) >= (uint64_t)demuxer->movi_end))
        continue;

      saved_pos = stream_tell (s);
      if (!stream_seek (s, mkv_d->segment_start + seek_pos))
        res = 1;
      else
        {
          if (ebml_read_id (s, &il) != seek_id)
            res = 1;
          else
            switch (seek_id)
              {
              case MATROSKA_ID_CUES:
                if (demux_mkv_read_cues (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_TAGS:
                if (demux_mkv_read_tags (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_SEEKHEAD:
                if (demux_mkv_read_seekhead (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_CHAPTERS:
                if (demux_mkv_read_chapters (demuxer))
                  res = 1;
                break;
              }
        }

      if(s->eof)		//Fuchun 2010.12.27
	  	s->eof = 0;
	  
      stream_seek (s, saved_pos);
    }
  if (res)
    {
      /* If there was an error then try to skip this seek head. */
      if (stream_seek (s, off))
        res = 0;
    }
  else
  if (length > 0)
     stream_seek (s, stream_tell (s) + length);
  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing seek head ] ---------\n");
  return res;
}

static int
demux_mkv_open_video (demuxer_t *demuxer, mkv_track_t *track, int vid);
static int
demux_mkv_open_audio (demuxer_t *demuxer, mkv_track_t *track, int aid);
static int
demux_mkv_open_sub (demuxer_t *demuxer, mkv_track_t *track, int sid);

static void
display_create_tracks (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *)demuxer->priv;
  int i, vid=0, aid=0, sid=0;

  for (i=0; i<mkv_d->num_tracks; i++)
    {
      char *type = "unknown", str[32];
      *str = '\0';
      switch (mkv_d->tracks[i]->type)
        {
        case MATROSKA_TRACK_VIDEO:
          type = "video";
          demux_mkv_open_video(demuxer, mkv_d->tracks[i], vid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VID_%d_NAME=%s\n", vid, mkv_d->tracks[i]->name);
          sprintf (str, "-vid %u", vid++);
          break;
        case MATROSKA_TRACK_AUDIO:
          type = "audio";
          demux_mkv_open_audio(demuxer, mkv_d->tracks[i], aid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_NAME=%s\n", aid, mkv_d->tracks[i]->name);
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_LANG=%s\n", aid, mkv_d->tracks[i]->language);
          sprintf (str, "-aid %u, -alang %.5s",aid++,mkv_d->tracks[i]->language);
          break;
        case MATROSKA_TRACK_SUBTITLE:
          type = "subtitles";
          demux_mkv_open_sub(demuxer, mkv_d->tracks[i], sid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_NAME=%s\n", sid, mkv_d->tracks[i]->name);
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_LANG=%s\n", sid, mkv_d->tracks[i]->language);
          sprintf (str, "-sid %u, -slang %.5s",sid++,mkv_d->tracks[i]->language);
          break;
        }
      if (mkv_d->tracks[i]->name)
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_TrackIDName,
             mkv_d->tracks[i]->tnum, type, mkv_d->tracks[i]->codec_id, mkv_d->tracks[i]->name, str);
      else
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_TrackID,
             mkv_d->tracks[i]->tnum, type, mkv_d->tracks[i]->codec_id, str);
    }
}

typedef struct {
  char *id;
  int fourcc;
  int extradata;
} videocodec_info_t;

static const videocodec_info_t vinfo[] = {
  { MKV_V_MPEG1,     mmioFOURCC('m', 'p', 'g', '1'), 0 },
  { MKV_V_MPEG2,     mmioFOURCC('m', 'p', 'g', '2'), 0 },
  { MKV_V_MPEG4_SP,  mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_ASP, mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_AP,  mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_AVC, mmioFOURCC('a', 'v', 'c', '1'), 1 },
  { MKV_V_THEORA,    mmioFOURCC('t', 'h', 'e', 'o'), 1 },
  { NULL, 0, 0 }
};

static int
demux_mkv_open_video (demuxer_t *demuxer, mkv_track_t *track, int vid)
{
  BITMAPINFOHEADER *bih;
  void *ImageDesc = NULL;
  sh_video_t *sh_v;

  if (track->ms_compat)  /* MS compatibility mode */
    {
      BITMAPINFOHEADER *src;

      if (track->private_data == NULL
          || track->private_size < sizeof (BITMAPINFOHEADER))
        return 1;

      src = (BITMAPINFOHEADER *) track->private_data;
      bih = calloc (1, track->private_size);
      bih->biSize = le2me_32 (src->biSize);
      bih->biWidth = le2me_32 (src->biWidth);
      bih->biHeight = le2me_32 (src->biHeight);
      bih->biPlanes = le2me_16 (src->biPlanes);
      bih->biBitCount = le2me_16 (src->biBitCount);
      bih->biCompression = le2me_32 (src->biCompression);
      bih->biSizeImage = le2me_32 (src->biSizeImage);
      bih->biXPelsPerMeter = le2me_32 (src->biXPelsPerMeter);
      bih->biYPelsPerMeter = le2me_32 (src->biYPelsPerMeter);
      bih->biClrUsed = le2me_32 (src->biClrUsed);
      bih->biClrImportant = le2me_32 (src->biClrImportant);
      memcpy((char *) bih + sizeof (BITMAPINFOHEADER),
             (char *) src + sizeof (BITMAPINFOHEADER),
             track->private_size - sizeof (BITMAPINFOHEADER));

      if (track->v_width == 0)
        track->v_width = bih->biWidth;
      if (track->v_height == 0)
        track->v_height = bih->biHeight;

#ifdef _SKY_VDEC_V2	/*johnnyke 20100727*/
	if (bih->biCompression == mmioFOURCC('W','M','V','3'))
		isVC1= 1;
#if 1	//Barry 2011-06-18
	else if (bih->biCompression == mmioFOURCC('W','V','C','1'))
		isVC1= 2;
#endif
	else
		printf("[mkv] MS compatibility mode, bih->biCompression=%.4s\n", (char*)&bih->biCompression);

	if (isVC1 == 1)
	{		unsigned char* wmv3StrC;
			mkv_asfHeaderSize = 36;
			mkv_asfHeader = malloc(mkv_asfHeaderSize);
    			wmv3StrC = ((uint8_t*)bih) + sizeof(BITMAPINFOHEADER);

			PUT32_L(0x00000004, (unsigned char*)(mkv_asfHeader+4));

			memcpy((unsigned char*)(mkv_asfHeader+8), wmv3StrC, 4);

			PUT32_L(track->v_height, (unsigned char*)(mkv_asfHeader+12));
			PUT32_L(track->v_width, (unsigned char*)(mkv_asfHeader+16));
			PUT32_L(0x0000000C, (unsigned char*)(mkv_asfHeader+20));
			PUT32_L(0x00000000, (unsigned char*)(mkv_asfHeader+24));
			PUT32_L(0x00000000, (unsigned char*)(mkv_asfHeader+28));
			PUT32_L(0x00000000, (unsigned char*)(mkv_asfHeader+32));

			asfHeaderSize = mkv_asfHeaderSize;
			asfHeader = mkv_asfHeader; 
			*((int*)(asfHeader)) = 0xC5<<24 | 1;
	}
#endif
#if 1	//Barry 2011-06-18
	else if (isVC1 == 2)
	{
		int i;
		mkv_asfHeaderSize = bih->biSize - sizeof(BITMAPINFOHEADER) - 1;
		mkv_asfHeader = malloc(mkv_asfHeaderSize);
		unsigned char *p = ((uint8_t*)(bih)) + sizeof(BITMAPINFOHEADER) + 1;	
		printf("[mkv]: mkv_asfHeader[%d] =", mkv_asfHeaderSize);
		memcpy(mkv_asfHeader, p, mkv_asfHeaderSize);
		for (i=0;i<mkv_asfHeaderSize;i++)
			printf("%.2X ", mkv_asfHeader[i]);
		printf("\n");
	}
#endif
    }
  else
    {
      bih = calloc (1, sizeof (BITMAPINFOHEADER));
      bih->biSize = sizeof (BITMAPINFOHEADER);
      bih->biWidth = track->v_width;
      bih->biHeight = track->v_height;
      bih->biBitCount = 24;
      bih->biSizeImage = bih->biWidth * bih->biHeight * bih->biBitCount/8;

      if (track->private_size >= RVPROPERTIES_SIZE
          && (!strcmp (track->codec_id, MKV_V_REALV10)
              || !strcmp (track->codec_id, MKV_V_REALV20)
              || !strcmp (track->codec_id, MKV_V_REALV30)
              || !strcmp (track->codec_id, MKV_V_REALV40)))
        {
          unsigned char *dst, *src;
          uint32_t type2;
          unsigned int cnt;

          src = (uint8_t *)track->private_data + RVPROPERTIES_SIZE;

          cnt = track->private_size - RVPROPERTIES_SIZE;
          bih = realloc(bih, sizeof (BITMAPINFOHEADER)+8+cnt);
          bih->biSize = 48+cnt;
          bih->biPlanes = 1;
          type2 = AV_RB32(src - 4);
          if (type2 == 0x10003000 || type2 == 0x10003001)
            bih->biCompression=mmioFOURCC('R','V','1','3');
          else
            bih->biCompression=mmioFOURCC('R','V',track->codec_id[9],'0');
          dst = (unsigned char *) (bih + 1);
          // copy type1 and type2 info from rv properties
          memcpy(dst, src - 8, 8);
          stream_read(demuxer->stream, dst+8, cnt);
          track->realmedia = 1;

#ifdef CONFIG_QTX_CODECS
        }
      else if (track->private_size >= sizeof (ImageDescription)
               && !strcmp(track->codec_id, MKV_V_QUICKTIME))
        {
          ImageDescriptionPtr idesc;

          idesc = (ImageDescriptionPtr) track->private_data;
          idesc->idSize = be2me_32 (idesc->idSize);
          idesc->cType = be2me_32 (idesc->cType);
          idesc->version = be2me_16 (idesc->version);
          idesc->revisionLevel = be2me_16 (idesc->revisionLevel);
          idesc->vendor = be2me_32 (idesc->vendor);
          idesc->temporalQuality = be2me_32 (idesc->temporalQuality);
          idesc->spatialQuality = be2me_32 (idesc->spatialQuality);
          idesc->width = be2me_16 (idesc->width);
          idesc->height = be2me_16 (idesc->height);
          idesc->hRes = be2me_32 (idesc->hRes);
          idesc->vRes = be2me_32 (idesc->vRes);
          idesc->dataSize = be2me_32 (idesc->dataSize);
          idesc->frameCount = be2me_16 (idesc->frameCount);
          idesc->depth = be2me_16 (idesc->depth);
          idesc->clutID = be2me_16 (idesc->clutID);
          bih->biPlanes = 1;
          bih->biCompression = idesc->cType;
          ImageDesc = idesc;
#endif /* CONFIG_QTX_CODECS */

        }
      else
        {
          const videocodec_info_t *vi = vinfo;
          while (vi->id && strcmp(vi->id, track->codec_id)) vi++;
          bih->biCompression = vi->fourcc;
          if (vi->extradata && track->private_data && (track->private_size > 0))
            {
              bih->biSize += track->private_size;
              bih = realloc (bih, bih->biSize);
              memcpy (bih + 1, track->private_data, track->private_size);
            }
          //track->reorder_timecodes = user_correct_pts == 0;		//disable FUchun 2010.03.29
          track->reorder_timecodes = 1;
          if (!vi->id) {
              mp_msg (MSGT_DEMUX,MSGL_WARN, MSGTR_MPDEMUX_MKV_UnknownCodecID,
                      track->codec_id, track->tnum);
              free(bih);
              return 1;
          }
        }
    }

  sh_v = new_sh_video_vid (demuxer, track->tnum, vid);
#ifdef _SKY_VDEC_V2	//Barry 2010-09-29
      if (track->private_size > 0
          && (!strcmp (track->codec_id, MKV_V_REALV30)
              || !strcmp (track->codec_id, MKV_V_REALV40)))
      	{
      		sh_v->rmvb_sequence_header_len = track->private_size;
		//Barry 2010-09-30
		sh_v->rmvb_sequence_header = malloc(sh_v->rmvb_sequence_header_len);
		memcpy(sh_v->rmvb_sequence_header, (unsigned char*)track->private_data, sh_v->rmvb_sequence_header_len);
      	}
#endif
  sh_v->bih = bih;
  sh_v->format = sh_v->bih->biCompression;
  if (track->v_frate == 0.0)
    track->v_frate = 25.0;

  //Barry 2011-01-11
  if (track->v_frate > 10000.0)
  {
  	printf("Wrong FPS [%f], set FPS to 29.97\n", track->v_frate);
	track->v_frate = 29.97;
  }
  sh_v->fps = track->v_frate;
  sh_v->frametime = 1 / track->v_frate;
  sh_v->aspect = 0;
  if (!track->realmedia)
    {
      sh_v->disp_w = track->v_width;
      sh_v->disp_h = track->v_height;
      if (track->v_dheight)
      sh_v->aspect = (float)track->v_dwidth / (float)track->v_dheight;
    }
  else
    {
      // vd_realvid.c will set aspect to disp_w/disp_h and rederive
      // disp_w and disp_h from the RealVideo stream contents returned
      // by the Real DLLs. If DisplayWidth/DisplayHeight was not set in
      // the Matroska file then it has already been set to PixelWidth/Height
      // by check_track_information.
      sh_v->disp_w = track->v_dwidth;
      sh_v->disp_h = track->v_dheight;
    }
  sh_v->ImageDesc = ImageDesc;
  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] Aspect: %f\n", sh_v->aspect);

  sh_v->ds = demuxer->video;
  return 0;
}

static int
demux_mkv_open_audio (demuxer_t *demuxer, mkv_track_t *track, int aid)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  sh_audio_t *sh_a = new_sh_audio_aid(demuxer, track->tnum, aid);
//  demux_packet_t *dp;
  if(!sh_a) return 1;
  mkv_d->audio_tracks[mkv_d->last_aid] = track->tnum;

  if (track->language && (strcmp(track->language, "und") != 0))
    sh_a->lang = strdup(track->language);
  sh_a->default_track = track->default_track;
  sh_a->ds = demuxer->audio;
  sh_a->wf = malloc (sizeof (WAVEFORMATEX));
  if (track->ms_compat && (track->private_size >= sizeof(WAVEFORMATEX)))
    {
      WAVEFORMATEX *wf = (WAVEFORMATEX *)track->private_data;
      sh_a->wf = realloc(sh_a->wf, track->private_size);
      sh_a->wf->wFormatTag = le2me_16 (wf->wFormatTag);
      sh_a->wf->nChannels = le2me_16 (wf->nChannels);
      sh_a->wf->nSamplesPerSec = le2me_32 (wf->nSamplesPerSec);
      sh_a->wf->nAvgBytesPerSec = le2me_32 (wf->nAvgBytesPerSec);
      sh_a->wf->nBlockAlign = le2me_16 (wf->nBlockAlign);
      sh_a->wf->wBitsPerSample = le2me_16 (wf->wBitsPerSample);
      sh_a->wf->cbSize = track->private_size - sizeof(WAVEFORMATEX);
      memcpy(sh_a->wf + 1, wf + 1, track->private_size - sizeof(WAVEFORMATEX));
      if (track->a_sfreq == 0.0)
        track->a_sfreq = sh_a->wf->nSamplesPerSec;
      if (track->a_channels == 0)
        track->a_channels = sh_a->wf->nChannels;
      if (track->a_bps == 0)
        track->a_bps = sh_a->wf->wBitsPerSample;
      track->a_formattag = sh_a->wf->wFormatTag;
    }
  else
    {
      memset(sh_a->wf, 0, sizeof (WAVEFORMATEX));
      if (!strcmp(track->codec_id, MKV_A_MP3) ||
          !strcmp(track->codec_id, MKV_A_MP2))
        track->a_formattag = 0x0055;
      else if (!strncmp(track->codec_id, MKV_A_AC3, strlen(MKV_A_AC3)))
        track->a_formattag = 0x2000;
      else if (!strcmp(track->codec_id, MKV_A_DTS))
	  {
        track->a_formattag = 0x2001;
        dts_packet = 1;
	  }
      else if (!strcmp(track->codec_id, MKV_A_PCM) ||
               !strcmp(track->codec_id, MKV_A_PCM_BE))
        track->a_formattag = 0x0001;
      else if (!strcmp(track->codec_id, MKV_A_AAC_2MAIN) ||
               !strncmp(track->codec_id, MKV_A_AAC_2LC,
                        strlen(MKV_A_AAC_2LC)) ||
               !strcmp(track->codec_id, MKV_A_AAC_2SSR) ||
               !strcmp(track->codec_id, MKV_A_AAC_4MAIN) ||
               !strncmp(track->codec_id, MKV_A_AAC_4LC,
                        strlen(MKV_A_AAC_4LC)) ||
               !strcmp(track->codec_id, MKV_A_AAC_4SSR) ||
               !strcmp(track->codec_id, MKV_A_AAC_4LTP) ||
               !strcmp(track->codec_id, MKV_A_AAC))
        track->a_formattag = mmioFOURCC('M', 'P', '4', 'A');
      else if (!strcmp(track->codec_id, MKV_A_VORBIS))
        {
          if (track->private_data == NULL)
            return 1;
          track->a_formattag = mmioFOURCC('v', 'r', 'b', 's');
        }
      else if (!strcmp(track->codec_id, MKV_A_QDMC))
        track->a_formattag = mmioFOURCC('Q', 'D', 'M', 'C');
      else if (!strcmp(track->codec_id, MKV_A_QDMC2))
        track->a_formattag = mmioFOURCC('Q', 'D', 'M', '2');
      else if (!strcmp(track->codec_id, MKV_A_WAVPACK))
        track->a_formattag = mmioFOURCC('W', 'V', 'P', 'K');
      else if (!strcmp(track->codec_id, MKV_A_TRUEHD))
        track->a_formattag = mmioFOURCC('T', 'R', 'H', 'D');
      else if (!strcmp(track->codec_id, MKV_A_FLAC))
        {
          if (track->private_data == NULL || track->private_size == 0)
            {
              mp_msg (MSGT_DEMUX, MSGL_WARN,
                      MSGTR_MPDEMUX_MKV_FlacTrackDoesNotContainValidHeaders);
              return 1;
            }
          track->a_formattag = mmioFOURCC ('f', 'L', 'a', 'C');
        }
      else if (track->private_size >= RAPROPERTIES4_SIZE)
        {
          if (!strcmp(track->codec_id, MKV_A_REAL28))
            track->a_formattag = mmioFOURCC('2', '8', '_', '8');
          else if (!strcmp(track->codec_id, MKV_A_REALATRC))
            track->a_formattag = mmioFOURCC('a', 't', 'r', 'c');
          else if (!strcmp(track->codec_id, MKV_A_REALCOOK))
            track->a_formattag = mmioFOURCC('c', 'o', 'o', 'k');
          else if (!strcmp(track->codec_id, MKV_A_REALDNET))
            track->a_formattag = mmioFOURCC('d', 'n', 'e', 't');
          else if (!strcmp(track->codec_id, MKV_A_REALSIPR))
            track->a_formattag = mmioFOURCC('s', 'i', 'p', 'r');
        }
      else
        {
          mp_msg (MSGT_DEMUX, MSGL_WARN, MSGTR_MPDEMUX_MKV_UnknownAudioCodec,
                  track->codec_id, track->tnum);
          free_sh_audio(demuxer, track->tnum);
          //Barry 2011-10-17 marked, fix mantis: 6351
          //not_supported_profile_level = 2;	//Barry 2010-11-11
          return 1;
        }
    }

  sh_a->format = track->a_formattag;
  sh_a->wf->wFormatTag = track->a_formattag;
  sh_a->channels = track->a_channels;
  sh_a->wf->nChannels = track->a_channels;
  sh_a->samplerate = (uint32_t) track->a_sfreq;
  sh_a->wf->nSamplesPerSec = (uint32_t) track->a_sfreq;
  if (track->a_bps == 0)
    {
      sh_a->samplesize = 2;
      sh_a->wf->wBitsPerSample = 16;
    }
  else
    {
      sh_a->samplesize = track->a_bps / 8;
      sh_a->wf->wBitsPerSample = track->a_bps;
    }
  if (track->a_formattag == 0x0055)  /* MP3 || MP2 */
    {
      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1152;
    }
  else if (track->a_formattag == 0x2000 )/* AC3 */
    {
      free(sh_a->wf);
      sh_a->wf = NULL;
    }
  else if (track->a_formattag == 0x2001) /* DTS */
    {
      free(sh_a->wf);
      sh_a->wf = NULL;
      dts_packet = 1;
    }
  else if (track->a_formattag == 0x0001)  /* PCM || PCM_BE */
    {
      sh_a->wf->nAvgBytesPerSec = sh_a->channels * sh_a->samplerate*2;
      sh_a->wf->nBlockAlign = sh_a->wf->nAvgBytesPerSec;
      if (!strcmp(track->codec_id, MKV_A_PCM_BE))
        sh_a->format = mmioFOURCC('t', 'w', 'o', 's');
    }
  else if (!strcmp(track->codec_id, MKV_A_QDMC) ||
           !strcmp(track->codec_id, MKV_A_QDMC2))
    {
      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1486;
      track->fix_i_bps = 1;
      track->qt_last_a_pts = 0.0;
      if (track->private_data != NULL)
        {
          sh_a->codecdata=malloc(track->private_size);
          memcpy (sh_a->codecdata, track->private_data,
                  track->private_size);
          sh_a->codecdata_len = track->private_size;
        }
    }
  else if (track->a_formattag == mmioFOURCC('M', 'P', '4', 'A'))
    {
      int profile, srate_idx;

      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1024;

      if (!strcmp (track->codec_id, MKV_A_AAC) &&
          (NULL != track->private_data))
        {
          sh_a->codecdata=malloc(track->private_size);
          memcpy (sh_a->codecdata, track->private_data,
                  track->private_size);
		  
          sh_a->codecdata_len = track->private_size;

          //Barry 2011-05-24
          if (sh_a->codecdata_len == 2)
          {
          	if (((sh_a->codecdata[0]>>3) & 0x1F) == 1)	//aac main profile
          	{
          		free_sh_audio(demuxer, track->tnum);
			not_supported_profile_level = 2;
			return 1;
		}
          }
#ifdef SUPPORT_DIVX_DRM_MKV
			/* fixed HD plus VOD file as V15, V16, V17 can't placback issue */
		  if (track->private_size == 5) 
		  { 
			  /* According lavf sample codes */
			  if ((sh_a->codecdata[2] == 0x56) && (sh_a->codecdata[3] == 0xE5) && (sh_a->codecdata[4] == 0xe8))
			  {
				  //printf("@@@ In [%s][%d] set codecdata_len to [%d] sh_a->codecdata[%s]-[%x %x %x %x %x]@@@\n", __func__, __LINE__, sh_a->codecdata_len, sh_a->codecdata, sh_a->codecdata[0], sh_a->codecdata[1], sh_a->codecdata[2], sh_a->codecdata[3], sh_a->codecdata[4]);
				  //sh_a->samplerate *= 2;
				  sh_a->wf->nSamplesPerSec *= 2;
				  srate_idx = aac_get_sample_rate_index(sh_a->samplerate);
				  sh_a->codecdata[2] = AAC_SYNC_EXTENSION_TYPE >> 3;
				  sh_a->codecdata[3] = ((AAC_SYNC_EXTENSION_TYPE&0x07)<<5) | 5;
				  sh_a->codecdata[4] = (1 << 7) | (srate_idx << 3);
				  track->default_duration = 1024.0 / (sh_a->samplerate / 2);
			  }
			  else
			  {
				  /* debug used */
				  int ii = 0;
				  printf("== In [%s][%d] unknown codecdata is [", __func__, __LINE__);
				  while(ii < track->private_size)
				  {
					  printf("%x ", sh_a->codecdata[ii]);
					  ii++;
				  }
				  printf("]\n");
			  }
		  }
#endif /* end of SUPPORT_DIVX_DRM_MKV */

          return 0;
        }

      /* Recreate the 'private data' */
      /* which faad2 uses in its initialization */
      srate_idx = aac_get_sample_rate_index (sh_a->samplerate);
      if (!strncmp (&track->codec_id[12], "MAIN", 4))
      {
      		profile = 0;
		//Barry 2011-05-24
      		free_sh_audio(demuxer, track->tnum);
		not_supported_profile_level = 2;
		return 1;
      }
      else if (!strncmp (&track->codec_id[12], "LC", 2))
        profile = 1;
      else if (!strncmp (&track->codec_id[12], "SSR", 3))
        profile = 2;
      else
        profile = 3;
      sh_a->codecdata = malloc (5);
      sh_a->codecdata[0] = ((profile+1) << 3) | ((srate_idx&0xE) >> 1);
      sh_a->codecdata[1] = ((srate_idx&0x1)<<7)|(track->a_channels<<3);

      if (strstr(track->codec_id, "SBR") != NULL)
        {
          /* HE-AAC (aka SBR AAC) */
          sh_a->codecdata_len = 5;

          sh_a->samplerate *= 2;
          sh_a->wf->nSamplesPerSec *= 2;
          srate_idx = aac_get_sample_rate_index(sh_a->samplerate);
          sh_a->codecdata[2] = AAC_SYNC_EXTENSION_TYPE >> 3;
          sh_a->codecdata[3] = ((AAC_SYNC_EXTENSION_TYPE&0x07)<<5) | 5;
          sh_a->codecdata[4] = (1 << 7) | (srate_idx << 3);
          track->default_duration = 1024.0 / (sh_a->samplerate / 2);
        }
      else
        {
          sh_a->codecdata_len = 2;
          track->default_duration = 1024.0 / (float)sh_a->samplerate;
        }
    }
  else if (track->a_formattag == mmioFOURCC('v', 'r', 'b', 's'))  /* VORBIS */
    {
      sh_a->wf->cbSize = track->private_size;
      sh_a->wf = realloc(sh_a->wf, sizeof(WAVEFORMATEX) + sh_a->wf->cbSize);
      memcpy((unsigned char *) (sh_a->wf+1), track->private_data, sh_a->wf->cbSize);
    }
  else if (track->private_size >= RAPROPERTIES4_SIZE
           && !strncmp (track->codec_id, MKV_A_REALATRC, 7))
    {
      /* Common initialization for all RealAudio codecs */
      unsigned char *src = track->private_data;
      int codecdata_length, version;
      int flavor;

      sh_a->wf->nAvgBytesPerSec = 0;  /* FIXME !? */

      version = AV_RB16(src + 4);
      flavor = AV_RB16(src + 22);
      track->coded_framesize = AV_RB32(src + 24);
      track->sub_packet_h = AV_RB16(src + 40);
      sh_a->wf->nBlockAlign =
      track->audiopk_size = AV_RB16(src + 42);
      track->sub_packet_size = AV_RB16(src + 44);
      if (version == 4)
        {
          src += RAPROPERTIES4_SIZE;
          src += src[0] + 1;
          src += src[0] + 1;
        }
      else
        src += RAPROPERTIES5_SIZE;

      src += 3;
      if (version == 5)
        src++;
      codecdata_length = AV_RB32(src);
      src += 4;
      sh_a->wf->cbSize = codecdata_length;
      sh_a->wf = realloc (sh_a->wf,
                          sizeof (WAVEFORMATEX) +
                          sh_a->wf->cbSize);
      memcpy(((char *)(sh_a->wf + 1)), src, codecdata_length);

      switch (track->a_formattag) {
        case mmioFOURCC('a', 't', 'r', 'c'):
          sh_a->wf->nAvgBytesPerSec = atrc_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->sub_packet_size;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('c', 'o', 'o', 'k'):
          sh_a->wf->nAvgBytesPerSec = cook_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->sub_packet_size;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('s', 'i', 'p', 'r'):
          sh_a->wf->nAvgBytesPerSec = sipr_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->coded_framesize;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('2', '8', '_', '8'):
          sh_a->wf->nAvgBytesPerSec = 3600;
          sh_a->wf->nBlockAlign = track->coded_framesize;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
      }

      track->realmedia = 1;
    }
  else if (!strcmp(track->codec_id, MKV_A_FLAC) ||
           (track->a_formattag == 0xf1ac))
    {
      unsigned char *ptr;
      int size;
#if 1	//Fuchun 2010.06.23
	if(track->a_formattag == mmioFOURCC('f', 'L', 'a', 'C'))
	{
		ptr = (unsigned char *)track->private_data;
		size = track->private_size;
	}
	else
	{
		sh_a->format = mmioFOURCC('f', 'L', 'a', 'C');
		ptr = (unsigned char *) track->private_data + sizeof (WAVEFORMATEX);
		size = track->private_size - sizeof (WAVEFORMATEX);
	}
	if(size < 4 || ptr[0] != 'f' || ptr[1] != 'L' ||ptr[2] != 'a' || ptr[3] != 'C')
	{
		sh_a->wf->cbSize = 4;
		sh_a->wf = realloc(sh_a->wf, sizeof (WAVEFORMATEX) + sh_a->wf->cbSize);
		memcpy(((char *)(sh_a->wf + 1)), "fLaC", sh_a->wf->cbSize);
	}
	else
	{
		sh_a->wf->cbSize = size;
		sh_a->wf = realloc(sh_a->wf, sizeof (WAVEFORMATEX) + sh_a->wf->cbSize);
		memcpy(((char *)(sh_a->wf + 1)), ptr, sh_a->wf->cbSize);
	}
#else
      free(sh_a->wf);
      sh_a->wf = NULL;

      if (track->a_formattag == mmioFOURCC('f', 'L', 'a', 'C'))
        {
          ptr = (unsigned char *)track->private_data;
          size = track->private_size;
        }
      else
        {
          sh_a->format = mmioFOURCC('f', 'L', 'a', 'C');
          ptr = (unsigned char *) track->private_data
            + sizeof (WAVEFORMATEX);
          size = track->private_size - sizeof (WAVEFORMATEX);
        }
      if (size < 4 || ptr[0] != 'f' || ptr[1] != 'L' ||
          ptr[2] != 'a' || ptr[3] != 'C')
        {
          dp = new_demux_packet (4);
          memcpy (dp->buffer, "fLaC", 4);
        }
      else
        {
          dp = new_demux_packet (size);
          memcpy (dp->buffer, ptr, size);
        }
      dp->pts = 0;
      dp->flags = 0;
      ds_add_packet (demuxer->audio, dp);
#endif
    }
  else if (track->a_formattag == mmioFOURCC('W', 'V', 'P', 'K') ||
           track->a_formattag == mmioFOURCC('T', 'R', 'H', 'D'))
    {  /* do nothing, still works */  }
  else if (!track->ms_compat || (track->private_size < sizeof(WAVEFORMATEX)))
    {
      free_sh_audio(demuxer, track->tnum);
      return 1;
    }

  return 0;
}

static int
demux_mkv_open_sub (demuxer_t *demuxer, mkv_track_t *track, int sid)
{

  if (track->subtitle_type != MATROSKA_SUBTYPE_UNKNOWN)
    {
      int size, m;
      uint8_t *buffer;
      sh_sub_t *sh = new_sh_sub_sid(demuxer, track->tnum, sid);
      track->sh_sub = sh;
      sh->type = 't';
      if (track->subtitle_type == MATROSKA_SUBTYPE_VOBSUB)
        sh->type = 'v';
      if (track->subtitle_type == MATROSKA_SUBTYPE_SSA)
        sh->type = 'a';
      size = track->private_size;
      m = demux_mkv_decode (track,track->private_data,&buffer,&size,2);
      if (buffer && m)
        {
          if (track->private_data)
		  	free (track->private_data);
          track->private_data = buffer;
          track->private_size = size;
        }
      sh->extradata=malloc(track->private_size);
      memcpy (sh->extradata, track->private_data,
              track->private_size);
      sh->extradata_len = track->private_size;
      if (track->language && (strcmp(track->language, "und") != 0))
        sh->lang = strdup(track->language);
      sh->default_track = track->default_track;
    }
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_ERR, MSGTR_MPDEMUX_MKV_SubtitleTypeNotSupported,
              track->codec_id);
      return 1;
    }

  return 0;
}

static void demux_mkv_seek (demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags);

static int
demux_mkv_open (demuxer_t *demuxer)
{
  stream_t *s = demuxer->stream;
  mkv_demuxer_t *mkv_d;
  mkv_track_t *track;
  int i, version, cont = 0;
  char *str;

  //reset parameter Fuchun 2010.03.02
  avc1_header = NULL;
  avc1_header_size = 0;
  rv40_header_size = 0;
  rv9_num_seq=0;
  rv9_flags=0;
  rv9_last_packet=0;
  //Barry 2010-07-21
  mkv_asfHeader = NULL;
  mkv_asfHeaderSize = 0;
  add_avc1_extradata = 0;
  add_mpeg2_extradata = 0;
  add_wvc1_extradata = 0;
  isVC1 = 0;
   mkv_indexnull_flag = 0;

  stream_seek(s, s->start_pos);
  str = ebml_read_header (s, &version);
  if (str == NULL || strcmp (str, "matroska") || version > 2)
    {
      mp_msg (MSGT_DEMUX, MSGL_DBG2, "[mkv] no head found\n");
      return 0;
    }
  free (str);

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] Found the head...\n");

  if (ebml_read_id (s, NULL) != MATROSKA_ID_SEGMENT)
    {
      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] but no segment :(\n");
      return 0;
    }
  ebml_read_length (s, NULL);  /* return bytes number until EOF */

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] + a segment...\n");

  mkv_d = calloc (1, sizeof (mkv_demuxer_t));
  demuxer->priv = mkv_d;
  mkv_d->tc_scale = 1000000;
  mkv_d->segment_start = stream_tell (s);
  mkv_d->parsed_cues = malloc (sizeof (off_t));
  mkv_d->parsed_seekhead = malloc (sizeof (off_t));

  while (!cont)
    {
      switch (ebml_read_id (s, NULL))
        {
        case MATROSKA_ID_INFO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ segment information...\n");
          cont = demux_mkv_read_info (demuxer);
          break;

        case MATROSKA_ID_TRACKS:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ segment tracks...\n");
          cont = demux_mkv_read_tracks (demuxer);
          break;

        case MATROSKA_ID_CUES:
          cont = demux_mkv_read_cues (demuxer);
          break;

        case MATROSKA_ID_TAGS:
          cont = demux_mkv_read_tags (demuxer);
          break;

        case MATROSKA_ID_SEEKHEAD:
          cont = demux_mkv_read_seekhead (demuxer);
          break;

        case MATROSKA_ID_CHAPTERS:
          cont = demux_mkv_read_chapters (demuxer);
          break;

        case MATROSKA_ID_ATTACHMENTS:
          cont = demux_mkv_read_attachments (demuxer);
          break;

        case MATROSKA_ID_CLUSTER:
          {
            int p, l;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ found cluster, headers are "
                    "parsed completely :)\n");
            /* get the first cluster timecode */
            p = stream_tell(s);
            l = ebml_read_length (s, NULL);
            while (ebml_read_id (s, NULL) != MATROSKA_ID_CLUSTERTIMECODE)
              {
                ebml_read_skip (s, NULL);
                if (stream_tell (s) >= p + l)
                  break;
              }
            if (stream_tell (s) < p + l)
              {
                uint64_t num = ebml_read_uint (s, NULL);
                if (num == EBML_UINT_INVALID)
                  return 0;
                mkv_d->first_tc = num * mkv_d->tc_scale / 1000000.0;
                mkv_d->has_first_tc = 1;
                //Polun 2011-05-24 ++s
                if(mkv_d->first_tc > 18000000000000)
                {
                    printf("!!!mkv_d->first_tc error  [%s - %d]   mkv_d->first_tc = %lld\n", __func__, __LINE__, mkv_d->first_tc);
                    printf("!!![%s - %d]  adjust  mkv_d->first_tc to 0 \n", __func__, __LINE__ );
                    mkv_d->first_tc = 0 ;
                 }
                //Polun 2011-05-24 ++e
              }
            stream_seek (s, p - 4);
            cont = 1;
            break;
          }

        default:
          cont = 1;
        case EBML_ID_VOID:
          ebml_read_skip (s, NULL);
          break;
        }
    }

  display_create_tracks (demuxer);

  /* select video track */
  track = NULL;
  if (demuxer->video->id == -1)  /* automatically select a video track */
    {
      /* search for a video track that has the 'default' flag set */
      for (i=0; i<mkv_d->num_tracks; i++)
        if (mkv_d->tracks[i]->type == MATROSKA_TRACK_VIDEO
            && mkv_d->tracks[i]->default_track)
          {
            track = mkv_d->tracks[i];
            break;
          }

      if (track == NULL)
        /* no track has the 'default' flag set */
        /* let's take the first video track */
        for (i=0; i<mkv_d->num_tracks; i++)
          if (mkv_d->tracks[i]->type == MATROSKA_TRACK_VIDEO)
            {
              track = mkv_d->tracks[i];
              break;
            }
    }
  else if (demuxer->video->id != -2)  /* -2 = no video at all */
    track = demux_mkv_find_track_by_num (mkv_d, demuxer->video->id,
                                         MATROSKA_TRACK_VIDEO);

  if (track && demuxer->v_streams[track->tnum])
              {
                mp_msg (MSGT_DEMUX, MSGL_INFO,
                        MSGTR_MPDEMUX_MKV_WillPlayVideoTrack, track->tnum);
                demuxer->video->id = track->tnum;
                demuxer->video->sh = demuxer->v_streams[track->tnum];
              }
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_NoVideoTrackFound);
      demuxer->video->id = -2;
    }

#if 1	//Barry 2010-12-14  //Polun 2011-06-29 add (mkv_d->num_indexes != 0) 
	if( (demuxer->video->id >= 0) && !strcmp (track->codec_id, MKV_V_MSCOMP) && (mkv_d->num_indexes != 0) &&
		(((sh_video_t*)demuxer->video->sh)->bih->biCompression == mmioFOURCC('X', 'V', 'I', 'D')
		||((sh_video_t*)demuxer->video->sh)->bih->biCompression == mmioFOURCC('x', 'v', 'i', 'd')
		||((sh_video_t*)demuxer->video->sh)->bih->biCompression == mmioFOURCC('D', 'I', 'V', 'X')
		||((sh_video_t*)demuxer->video->sh)->bih->biCompression == mmioFOURCC('d', 'i', 'v', 'x')
		||((sh_video_t*)demuxer->video->sh)->bih->biCompression == mmioFOURCC('D', 'X', '5', '0')
		)
	)
	{
		mkv_index_t *index = NULL;
		off_t ori_pos = stream_tell(s);
		uint8_t *check_mp4_vol_buf = malloc(512);

		for (i=0; i < mkv_d->num_indexes; i++)
		{
			if (mkv_d->indexes[i].tnum == demuxer->video->id)
			{
				index = &mkv_d->indexes[i];
//				printf("[%d]   index->filepos = %ld\n", i, index->filepos);
				break;
			}
		}
		stream_seek (s, index->filepos);
		stream_read (s, check_mp4_vol_buf, 512);

		if ( !check_mp4_header_vol(check_mp4_vol_buf, 512) )
		{
			not_supported_profile_level = 1;
			printf("\nMKV: GMC AND STATIC SPRITE CODING not supported\n\n");
		}
		free(check_mp4_vol_buf);
		stream_seek (s, ori_pos);
	}
#endif

  /* select audio track */
  track = NULL;
  int track_num = 0;  //Polun 2011-06-15 +
  if (track == NULL)
    /* search for an audio track that has the 'default' flag set */
    for (i=0; i < mkv_d->num_tracks; i++)
      if (mkv_d->tracks[i]->type == MATROSKA_TRACK_AUDIO
          && mkv_d->tracks[i]->default_track)
        {
          track = mkv_d->tracks[i];
          track_num = i;  //Polun 2011-06-15 +
          break;
        }

  if (track == NULL)
    /* no track has the 'default' flag set */
    /* let's take the first audio track */
    for (i=0; i < mkv_d->num_tracks; i++)
      if (mkv_d->tracks[i]->type == MATROSKA_TRACK_AUDIO)
        {
          track = mkv_d->tracks[i];
          break;
        }
  int track_cunt = mkv_d->num_tracks - track_num; 
  if (track && demuxer->a_streams[track->tnum])
    {
      demuxer->audio->id = track->tnum;
      demuxer->audio->sh = demuxer->a_streams[track->tnum];
    }
  //Polun 2011-06-15 ++s  if the audio track is not support
  else if((track_num+1) < mkv_d->num_tracks)
  //else if(track_num < mkv_d->num_tracks && demuxer->a_streams[track->tnum])
  {
        track_cunt = track_cunt -1; //Polun 2011-06-24 start audio track is audio track 2
        while(track_cunt)
        {
        	if (mkv_d->tracks[track_num + 1])
            		track = mkv_d->tracks[track_num + 1] ;
		else
			break;
		if (track && demuxer->a_streams[track->tnum])
		{
			demuxer->audio->id = track->tnum;
			demuxer->audio->sh = demuxer->a_streams[track->tnum];
			not_supported_profile_level = 0;
			break;
		}
		track_num +=1;
		track_cunt -=1;
        }
        if (!(track && demuxer->a_streams[track->tnum]))
        {
             mp_msg (MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_NoAudioTrackFound);
             demuxer->audio->id = -2;
        }
    }
  //Polun 2011-06-15 ++e
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_NoAudioTrackFound);
      demuxer->audio->id = -2;
    }


  if(demuxer->audio->id != -2)
  for (i=0; i < mkv_d->num_tracks; i++)
    {
      if(mkv_d->tracks[i]->type != MATROSKA_TRACK_AUDIO)
          continue;
      if(demuxer->a_streams[track->tnum])
        {
          mkv_d->last_aid++;
          if(mkv_d->last_aid == MAX_A_STREAMS)
            break;
        }
    }
  
  if (demuxer->chapters)
    {
      for (i=0; i < (int)demuxer->num_chapters; i++)
        {
          demuxer->chapters[i].start -= mkv_d->first_tc;
          demuxer->chapters[i].end -= mkv_d->first_tc;
        }
      if (dvd_last_chapter > 0 && dvd_last_chapter <= demuxer->num_chapters)
        {
          if (demuxer->chapters[dvd_last_chapter-1].end != 0)
            mkv_d->stop_timecode = demuxer->chapters[dvd_last_chapter-1].end;
          else if (dvd_last_chapter + 1 <= demuxer->num_chapters)
            mkv_d->stop_timecode = demuxer->chapters[dvd_last_chapter].start;
        }
    }
  //Polun 2011-0-21 for Prestige.mkv seek long time cause UI timeout
  if (s->end_pos == 0 || (mkv_d->indexes == NULL && index_mode < 0))
  {
      printf("====MKV: This file is no index can FF/FR but can't seek.====\n");
      mkv_indexnull_flag = 1;
  }
  //if (s->end_pos == 0 || (mkv_d->indexes == NULL && index_mode < 0))
  if ( s->end_pos == 0 ||
  	/*(mkv_d->indexes == NULL && index_mode < 0) ||*/	//Fuchun 20110801 disable, demux_mkv still can seek if indexes == NULL
  	(Mincache==1 && Maxcache==1 && !strcmp(track->codec_id, MKV_A_AAC))	//Barry 2010-12-30
  )
    demuxer->seekable = 0;
  else
    {
      demuxer->movi_start = s->start_pos;
      demuxer->movi_end = s->end_pos;
      demuxer->seekable = 1;
    }

#if 1	//Fuchun 2011.03.29 seek position to first index if the file has index table adn current position is wrong
	if(mkv_d->indexes != NULL)
	{
		uint64_t cur_pos = stream_tell(s);
		if(cur_pos != mkv_d->indexes[0].filepos && mkv_d->indexes[0].filepos > s->start_pos && mkv_d->indexes[0].filepos < s->end_pos)
		{
			stream_seek(s, mkv_d->indexes[0].filepos);
			if(avc1_header_size != 0 && demuxer->video->id >= 0)	//Fuchun 20110915 it's H264 type
			{
				int ii = 0;
				int isIframe = 0;
				int check_byte = 4;
				if(avc1_nal_bytes == 3)
					check_byte= 3;
				add_avc1_extradata = -1;
				char *new_buff = NULL;
				while(demuxer->video->packs < 1 && ii < 20)
				{
					ii++;
					demuxer->desc->fill_buffer(demuxer, demuxer->video);
				}
				if(demuxer->video->packs > 0)
				{
					demux_packet_t *p = demuxer->video->first;
					new_buff = calloc(p->len, sizeof(unsigned char));
					memcpy(new_buff, p->buffer, p->len);
					if(((new_buff[check_byte] & ~0x60) == 0x05 || (new_buff[check_byte] & ~0x60) == 0x07))
						isIframe = 1;
					else
						isIframe = 0;
					free(new_buff);
					new_buff = NULL;
				}
				ds_free_packs(demuxer->video);
				ds_free_packs(demuxer->audio);
				ds_free_packs(demuxer->sub);
				free_cached_dps (demuxer);
				if(isIframe == 1)
					stream_seek(s, mkv_d->indexes[0].filepos);
				else
					stream_seek(s, cur_pos);
				mkv_d->cluster_size = mkv_d->blockgroup_size = 0;

#ifdef get_b_frames	//Fuchun 20110915
				if(pic_struct_present_flag == 0)
				{
					ii = 0;
					int check_length = 0;
					while(demuxer->video->packs < 2 && ii < 20)
					{
						ii++;
						demuxer->desc->fill_buffer(demuxer, demuxer->video);
					}
					if(demuxer->video->packs >= 2)	//read two frame to parse sei
					{
						demux_packet_t *p = demuxer->video->first;
						if(new_buff)
							free(new_buff);
						new_buff = calloc(demuxer->video->bytes, sizeof(unsigned char));
						for(ii = 0; ii < demuxer->video->packs; ii++)
						{
							if(p)
							{
								memcpy(new_buff+check_length, p->buffer, p->len);
								check_length += p->len;
								p = p->next;
							}
						}
					}

					if((new_buff[check_byte] & ~0x60) == 0x05 || (new_buff[check_byte] & ~0x60) == 0x07)
					{
						int sps_len = 0;
						int k, length = 0;
						if(avc1_nal_bytes == 3)
							sps_len = ((int)new_buff[0] << 16) + ((int)new_buff[1] << 8) + ((int)new_buff[2]);
						else
							sps_len = ((int)new_buff[0] << 24) + ((int)new_buff[1] << 16) + ((int)new_buff[2] << 8) + ((int)new_buff[3]);

						num_reorder_frames = decode_264_sps_mkv(new_buff+check_byte+1, sps_len-1);
						if(num_reorder_frames != 0) queue_frames = num_reorder_frames;
						for(k = 0; k < check_length && pic_struct_present_flag == 1; k+=check_byte)
						{
							if(avc1_nal_bytes == 3)
								length = ((int)new_buff[k] << 16) + ((int)new_buff[k+1] << 8) + ((int)new_buff[k+2]);
							else
								length = ((int)new_buff[k] << 24) + ((int)new_buff[k+1] << 16) + ((int)new_buff[k+2] << 8) + ((int)new_buff[k+3]);

							if(((new_buff[k+check_byte]&~0x60) == 0x06))
							{
								int interlace_flag;
								if(interlace_flag = h264_parse_sei_mkv(&new_buff[k+check_byte+1], length-1))
								{
									pic_struct_present_flag = 0;
								}
							}
							k += length;
						}
					}
					if(new_buff)
						free(new_buff);

					ds_free_packs(demuxer->video);
					ds_free_packs(demuxer->audio);
					ds_free_packs(demuxer->sub);
					free_cached_dps (demuxer);
					if(isIframe == 1)
						stream_seek(s, mkv_d->indexes[0].filepos);
					else
						stream_seek(s, cur_pos);
					mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
				}
#endif
				add_avc1_extradata = 0;
			}
		}
	}
#endif

#ifdef SUPPORT_DIVX_DRM_MKV
  //DRM_debug Barry 2010-11-21
  if (DRM_Open > 0 && !video_dd_chunk)
  {               
	  video_dd_chunk = malloc(sizeof(DRM_chunk));
	  if (video_dd_chunk)
		  memset(video_dd_chunk, 0, sizeof(DRM_chunk));
	  else        
		  printf("####malloc video_dd_chunk failed ===\n");
  }               
#endif /* end of SUPPORT_DIVX_DRM_MKV */

  return DEMUXER_TYPE_MATROSKA;
}

static void
demux_close_mkv (demuxer_t *demuxer)
{

  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;

#ifdef SUPPORT_DIVX_DRM_MKV       
  if (video_dd_chunk)
  {
	  free(video_dd_chunk);
	  video_dd_chunk = NULL;
  }
#endif /* end of SUPPORT_DIVX_DRM_MKV */

  if (mkv_d)
    {
      int i;
      free_cached_dps (demuxer);
      if (mkv_d->tracks)
        {
          for (i=0; i<mkv_d->num_tracks; i++)
            demux_mkv_free_trackentry(mkv_d->tracks[i]);
          free (mkv_d->tracks);
        }
#if 1	//Barry 2010-09-27
      if (mkv_d->indexes)
	  	free(mkv_d->indexes);
      if (mkv_d->cluster_positions)
	  	free(mkv_d->cluster_positions);
      if (mkv_d->parsed_cues)
	  	free(mkv_d->parsed_cues);
      if (mkv_d->parsed_seekhead)
	  	free(mkv_d->parsed_seekhead);
#else
      free (mkv_d->indexes);
      free (mkv_d->cluster_positions);
      free (mkv_d->parsed_cues);
      free (mkv_d->parsed_seekhead);
#endif
      free (mkv_d);
      //Barry 2010-08-24
      if(avc1_header_size != 0 && add_avc1_extradata)
      {
      		if (avc1_header)
      			free(avc1_header);
		avc1_header = NULL;
		avc1_header_size = 0;
		add_avc1_extradata = 0;
      }
      add_mpeg2_extradata = 0;	//Barry 2010-09-28
      add_wvc1_extradata = 0;

      //Barry 2010-10-26
      if (mkv_asfHeaderSize != 0 && mkv_asfHeader)
      {
      		free(mkv_asfHeader);
		mkv_asfHeader = NULL;
		mkv_asfHeaderSize = 0;
      }
    }
}

static int
demux_mkv_read_block_lacing (uint8_t *buffer, uint64_t *size,
                             uint8_t *laces, uint32_t **all_lace_sizes)
{

  uint32_t total = 0, *lace_size;
  uint8_t flags;
  int i;

  *all_lace_sizes = NULL;
  lace_size = NULL;
  /* lacing flags */
  flags = *buffer++;
  (*size)--;

  switch ((flags & 0x06) >> 1)
    {
    case 0:  /* no lacing */
      *laces = 1;
      lace_size = calloc(*laces, sizeof(uint32_t));
      lace_size[0] = *size;
      break;

    case 1:  /* xiph lacing */
    case 2:  /* fixed-size lacing */
    case 3:  /* EBML lacing */
      *laces = *buffer++;
      (*size)--;
      (*laces)++;
      lace_size = calloc(*laces, sizeof(uint32_t));

      switch ((flags & 0x06) >> 1)
        {
        case 1:  /* xiph lacing */
          for (i=0; i < *laces-1; i++)
            {
              lace_size[i] = 0;
              do
                {
                  lace_size[i] += *buffer;
                  (*size)--;
                } while (*buffer++ == 0xFF);
              total += lace_size[i];
            }
          lace_size[i] = *size - total;
          break;

        case 2:  /* fixed-size lacing */
          for (i=0; i < *laces; i++)
            lace_size[i] = *size / *laces;
          break;

        case 3:  /* EBML lacing */
          {
            int l;
            uint64_t num = ebml_read_vlen_uint (buffer, &l);
            if (num == EBML_UINT_INVALID) {
              if (lace_size)
			free(lace_size);
              return 1;
            }
            buffer += l;
            *size -= l;

            total = lace_size[0] = num;
            for (i=1; i < *laces-1; i++)
              {
                int64_t snum;
                snum = ebml_read_vlen_int (buffer, &l);
                if (snum == EBML_INT_INVALID) {
                  if (lace_size)
                  	free(lace_size);
                  return 1;
                }
                buffer += l;
                *size -= l;
                lace_size[i] = lace_size[i-1] + snum;
                total += lace_size[i];
              }
            lace_size[i] = *size - total;
            break;
          }
        }
      break;
    }
  *all_lace_sizes = lace_size;
  return 0;
}

static void
handle_subtitles(demuxer_t *demuxer, mkv_track_t *track, char *block,
                 int64_t size, uint64_t block_duration, uint64_t timecode)
{
  demux_packet_t *dp;

  if (block_duration == 0)
  {
      mp_msg(MSGT_DEMUX, MSGL_WARN, MSGTR_MPDEMUX_MKV_NoBlockDurationForSubtitleTrackFound);
      return;
  }

  if(size <= 0)
  {
     mp_msg(MSGT_DEMUX, MSGL_WARN, "%s:%d Invalid size:%lld\n", __func__, __LINE__, size);
     return;
  }

  sub_utf8 = 1;

  dp = new_demux_packet(size);
  if(dp == NULL || dp->buffer == NULL || dp->len == 0)
  {
    return;
  }

  memset(dp->buffer+size, 0, 8);	//Fuchun 2010.10.25
  memcpy(dp->buffer, block, size);
  dp->pts = timecode / 1000.0f;
  dp->endpts = (timecode + block_duration) / 1000.0f;
  ds_add_packet(demuxer->sub, dp);
}

#define SKIP_BITS(n) buffer<<=n
#define SHOW_BITS(n) ((buffer)>>(32-(n)))

static double real_fix_timestamp1(mkv_track_t *track, unsigned char *s,
                                int timestamp) {
  float v_pts;
  uint32_t buffer = (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
  int kf = timestamp;
  int pict_type;
  int orig_kf;

  if (!strcmp(track->codec_id, MKV_V_REALV30) ||
      !strcmp(track->codec_id, MKV_V_REALV40)) {

    if (!strcmp(track->codec_id, MKV_V_REALV30)) {
      SKIP_BITS(3);
      pict_type = SHOW_BITS(2);
      SKIP_BITS(2 + 7);
    }else{
      SKIP_BITS(1);
      pict_type = SHOW_BITS(2);
      SKIP_BITS(2 + 7 + 3);
    }
    kf = SHOW_BITS(13);         // kf= 2*SHOW_BITS(12);
    orig_kf = kf;
    if (pict_type <= 1) {
      // I frame, sync timestamps:
      track->rv_kf_base = (int64_t)timestamp - kf;
      mp_msg(MSGT_DEMUX, MSGL_DBG2, "\nTS: base=%08X\n", track->rv_kf_base);
      kf = timestamp;
    } else {
      // P/B frame, merge timestamps:
      unsigned int tmp = (int64_t)timestamp - track->rv_kf_base;
      kf |= tmp & (~0x1fff);    // combine with packet timestamp
      if (kf < (tmp - 4096))    // workaround wrap-around problems
        kf += 8192;
      else if (kf > (tmp + 4096))
        kf -= 8192;
      kf += track->rv_kf_base;
    }
    if (pict_type != 3) {       // P || I  frame -> swap timestamps
      int tmp = kf;
      kf = track->rv_kf_pts;
      track->rv_kf_pts = tmp;
    }
    mp_msg(MSGT_DEMUX, MSGL_DBG2, "\nTS: %08X -> %08X (%04X) %d %02X %02X %02X "
           "%02X %5d\n", timestamp, kf, orig_kf, pict_type, s[0], s[1], s[2],
           s[3], kf - (int)(1000.0 * track->rv_pts));
  }
  v_pts=(double)(kf*0.001f);
  track->rv_pts = v_pts;

  return v_pts;
}

static void
handle_realvideo (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                  uint32_t size, int block_bref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  demux_packet_t *dp;
  uint32_t timestamp = mkv_d->last_pts * 1000.0;
  uint8_t *hbuf, *hbuf1;
  uint8_t chunks;
  int isize;

#if 1	// Raymond 2009/06/01
	//+SkyMedi_Vincent05182009 
	if(!strcmp(track->codec_id, MKV_V_REALV40))
	{
//		uint64_t  *slice_data;
		unsigned int i,slice_data_len,tmp_num_slices;
		float tmp_timestamp;
		//printf("**************************R V 40 *************\n");//SkyMedi_Vincent
        
		//reduce floating calculate error
		timestamp =(uint32_t)((float)(mkv_d->last_pts * 1000.0f));
		tmp_timestamp = (float)(mkv_d->last_pts * 1000.0f);
        
		tmp_timestamp = tmp_timestamp - timestamp;
		if(tmp_timestamp >= 0.9)
		{
			timestamp = timestamp + 1;
		}
    
		chunks = *buffer++;
		isize = --size - (chunks+1)*8;
        
		if(chunks>1)
			slice_data_len = (chunks+1)*8;
		else
			slice_data_len = 16;
        
		size -= slice_data_len; // subtract slice data length. some file has over than 2 slices.
		{
			// Raymond 2009/06/01
//			dp = new_video_packet ( slice_data_len + 20 + size, videobuffer); //rv9 picture header len = 20 bytes + slice num*8
			dp = new_demux_packet ( slice_data_len + 20 + size); //rv9 picture header len = 20 bytes + slice num*8
			memcpy (dp->buffer + (slice_data_len + 20), buffer + slice_data_len, isize);
			hbuf = dp->buffer;
		}
    		
		if(rv9_num_seq == 0)
			rv9_flags = 2;
		else
			rv9_flags = 0;

		PUT32(isize, hbuf);                   			// length of actual data
		PUT32(timestamp, hbuf + 4);                	// timestamp from packet header
		PUT16(rv9_num_seq, hbuf + 8);   		// num of sequence
		PUT16(rv9_flags, hbuf + 10);   			// and flags
		PUT32(rv9_last_packet, hbuf + 12);       // last_packet
		PUT32(chunks+1, hbuf + 16);                  	// num of slices
		
		rv9_num_seq++;

		if (mkv_d->v_skip_to_keyframe)
		{
			dp->pts = mkv_d->last_pts;
			track->rv_kf_base = 0;
			track->rv_kf_pts = timestamp;
		}
		else
			dp->pts = real_fix_timestamp1 (track, hbuf + REALHEADER_SIZE,
                                      timestamp);

		if(chunks+1 > 2)
			tmp_num_slices = chunks+1;
		else
			tmp_num_slices = 2; //The pic_header of slice_data must be at least 16 bytes.

		hbuf1 = buffer;
		hbuf = hbuf + 20;
		
		for(i=0;i<tmp_num_slices;i++)
		{
			if(i<=chunks)
			{
				hbuf[0] = hbuf1[3];
				hbuf[1] = hbuf1[2];
				hbuf[2] = hbuf1[1];
				hbuf[3] = hbuf1[0];

				hbuf += 4;
				hbuf1 += 4;
			
				hbuf[0] = hbuf1[3];
				hbuf[1] = hbuf1[2];
				hbuf[2] = hbuf1[1];
				hbuf[3] = hbuf1[0];

				hbuf += 4;
				hbuf1 += 4;
			}
			else
			{
				memcpy(hbuf, hbuf1, 8);
				hbuf += 8;
				hbuf1 += 8;
			}
		}	
	}
	else if(!strcmp(track->codec_id, MKV_V_REALV30))
	{
		printf("**************************R V 30 *************\n");//SkyMedi_Vincent
	}
	//SkyMedi_Vincent05182009+
	else //original code
#endif	
	{  //original code
		dp = new_demux_packet (size);
		memcpy (dp->buffer, buffer, size);

		if (mkv_d->v_skip_to_keyframe)
		{
			dp->pts = mkv_d->last_pts;
			track->rv_kf_base = 0;
			track->rv_kf_pts = timestamp;
		}
		else
			dp->pts = real_fix_timestamp1 (track, hbuf + REALHEADER_SIZE,
                                      timestamp);
//			dp->pts = real_fix_timestamp (dp->buffer, timestamp,
//                               ((sh_video_t*)demuxer->video->sh)->bih->biCompression,
//                                  &track->rv_kf_base, &track->rv_kf_pts, NULL);
	}  //original code

	dp->pos = demuxer->filepos;
	dp->flags = block_bref ? 0 : 0x10;

#ifdef  SUPPORT_DIVX_DRM_MKV
	//printf("### In [%s][%d] will do drm decyption EncryptionLength[%d]DRM_Open[%d] video_dd_chunk[%p] block[%p] len [%d]###\n", __func__, __LINE__, video_dd_chunk->EncryptionLength, DRM_Open, video_dd_chunk, dp->buffer, dp->len);
	if (DRM_Open > 0 && video_dd_chunk && dp->buffer)
	{                        
		if (video_dd_chunk->EncryptionLength > 0)
		{
			DRMDecode(dp->len, dp->buffer, video_dd_chunk, DIVX_DECODE_VIDEO);
		} 
	} 
#endif /* end of SUPPORT_DIVX_DRM_MKV */

	ds_add_packet(demuxer->video, dp);
}

static void
handle_realaudio (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                  uint32_t size, int block_bref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  int sps = track->sub_packet_size;
  int sph = track->sub_packet_h;
  int cfs = track->coded_framesize;
  int w = track->audiopk_size;
  int spc = track->sub_packet_cnt;
  demux_packet_t *dp;
  int x;

  if ((track->a_formattag == mmioFOURCC('2', '8', '_', '8')) ||
      (track->a_formattag == mmioFOURCC('c', 'o', 'o', 'k')) ||
      (track->a_formattag == mmioFOURCC('a', 't', 'r', 'c')) ||
      (track->a_formattag == mmioFOURCC('s', 'i', 'p', 'r')))
    {
//      if(!block_bref)
//        spc = track->sub_packet_cnt = 0;
      switch (track->a_formattag) {
        case mmioFOURCC('2', '8', '_', '8'):
          for (x = 0; x < sph / 2; x++)
            memcpy(track->audio_buf + x * 2 * w + spc * cfs, buffer + cfs * x, cfs);
          break;
        case mmioFOURCC('c', 'o', 'o', 'k'):
        case mmioFOURCC('a', 't', 'r', 'c'):
          for (x = 0; x < w / sps; x++)
            memcpy(track->audio_buf + sps * (sph * x + ((sph + 1) / 2) * (spc & 1) + (spc >> 1)), buffer + sps * x, sps);
          break;
        case mmioFOURCC('s', 'i', 'p', 'r'):
          memcpy(track->audio_buf + spc * w, buffer, w);
          if (spc == sph - 1)
            {
              int n;
              int bs = sph * w * 2 / 96;  // nibbles per subpacket
              // Perform reordering
              for(n=0; n < 38; n++)
                {
                  int j;
                  int i = bs * sipr_swaps[n][0];
                  int o = bs * sipr_swaps[n][1];
                  // swap nibbles of block 'i' with 'o'      TODO: optimize
                  for(j = 0;j < bs; j++)
                    {
                      int x = (i & 1) ? (track->audio_buf[i >> 1] >> 4) : (track->audio_buf[i >> 1] & 0x0F);
                      int y = (o & 1) ? (track->audio_buf[o >> 1] >> 4) : (track->audio_buf[o >> 1] & 0x0F);
                      if(o & 1)
                        track->audio_buf[o >> 1] = (track->audio_buf[o >> 1] & 0x0F) | (x << 4);
                      else
                        track->audio_buf[o >> 1] = (track->audio_buf[o >> 1] & 0xF0) | x;
                      if(i & 1)
                        track->audio_buf[i >> 1] = (track->audio_buf[i >> 1] & 0x0F) | (y << 4);
                      else
                        track->audio_buf[i >> 1] = (track->audio_buf[i >> 1] & 0xF0) | y;
                      ++i; ++o;
                    }
                }
            }
          break;
      }
      track->audio_timestamp[track->sub_packet_cnt] = (track->ra_pts == mkv_d->last_pts) ? 0 : (mkv_d->last_pts);
      track->ra_pts = mkv_d->last_pts;
      if (track->sub_packet_cnt == 0)
        track->audio_filepos = demuxer->filepos;
      if (++(track->sub_packet_cnt) == sph)
        {
           int apk_usize = ((WAVEFORMATEX*)((sh_audio_t*)demuxer->audio->sh)->wf)->nBlockAlign;
           track->sub_packet_cnt = 0;
           // Release all the audio packets
           for (x = 0; x < sph*w/apk_usize; x++)
             {
               dp = new_demux_packet(apk_usize);
               memcpy(dp->buffer, track->audio_buf + x * apk_usize, apk_usize);
               /* Put timestamp only on packets that correspond to original audio packets in file */
               dp->pts = (x * apk_usize % w) ? 0 : track->audio_timestamp[x * apk_usize / w];
               dp->pos = track->audio_filepos; // all equal
               dp->flags = x ? 0 : 0x10; // Mark first packet as keyframe
               ds_add_packet(demuxer->audio, dp);
             }
        }
   } else { // Not a codec that require reordering
  dp = new_demux_packet (size);
  memcpy(dp->buffer, buffer, size);
  if (track->ra_pts == mkv_d->last_pts && !mkv_d->a_skip_to_keyframe)
    dp->pts = 0;
  else
    dp->pts = mkv_d->last_pts;
  track->ra_pts = mkv_d->last_pts;

  dp->pos = demuxer->filepos;
  dp->flags = block_bref ? 0 : 0x10;
  ds_add_packet (demuxer->audio, dp);
  }
}

/** Reorder timecodes and add cached demux packets to the queues.
 *
 * Timecode reordering is needed if a video track contains B frames that
 * are timestamped in display order (e.g. MPEG-1, MPEG-2 or "native" MPEG-4).
 * MPlayer doesn't like timestamps in display order. This function adjusts
 * the timestamp of cached frames (which are exactly one I/P frame followed
 * by one or more B frames) so that they are in coding order again.
 *
 * Example: The track with 25 FPS contains four frames with the timecodes
 * I at 0ms, P at 120ms, B at 40ms and B at 80ms. As soon as the next I
 * or P frame arrives these timecodes can be changed to I at 0ms, P at 40ms,
 * B at 80ms and B at 120ms.
 *
 * This works for simple H.264 B-frame pyramids, but not for arbitrary orders.
 *
 * \param demuxer The Matroska demuxer struct for this instance.
 * \param track The track structure whose cache should be handled.
 */
static void
flush_cached_dps (demuxer_t *demuxer, mkv_track_t *track)
{
  int i, ok;

  if (track->num_cached_dps == 0)
    return;

  do {
    ok = 1;
    for (i = 1; i < track->num_cached_dps; i++)
      if (track->cached_dps[i - 1]->pts > track->cached_dps[i]->pts) {
        float tmp_pts = track->cached_dps[i - 1]->pts;
        track->cached_dps[i - 1]->pts = track->cached_dps[i]->pts;
        track->cached_dps[i]->pts = tmp_pts;
        ok = 0;
      }
  } while (!ok);

  for (i = 0; i < track->num_cached_dps; i++)
    ds_add_packet (demuxer->video, track->cached_dps[i]);
  track->num_cached_dps = 0;
}

/** Cache video frames if timecodes have to be reordered.
 *
 * Timecode reordering is needed if a video track contains B frames that
 * are timestamped in display order (e.g. MPEG-1, MPEG-2 or "native" MPEG-4).
 * This function takes in a Matroska block read from the file, allocates a
 * demux packet for it, fills in its values, allocates space for storing
 * pointers to the cached demux packets and adds the packet to it. If
 * the packet contains an I or a P frame then ::flush_cached_dps is called
 * in order to send the old cached frames downstream.
 *
 * \param demuxer The Matroska demuxer struct for this instance.
 * \param track The packet is meant for this track.
 * \param buffer The actual frame contents.
 * \param size The frame size in bytes.
 * \param block_bref A relative timecode (backward reference). If it is \c 0
 *   then the frame is an I frame.
 * \param block_fref A relative timecode (forward reference). If it is \c 0
 *   then the frame is either an I frame or a P frame depending on the value
 *   of \a block_bref. Otherwise it's a B frame.
 */
static void
handle_video_bframes (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                      uint32_t size, int block_bref, int block_fref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  demux_packet_t *dp;
#ifdef  SUPPORT_DIVX_DRM_MKV
  char add_drm_header_len = 0;
#endif /* end of SUPPORT_DIVX_DRM_MKV */

  #if 1	// Raymond 2009/05/11
#ifdef SUPPORT_DIVX_DRM_MKV
  /* Carlos add on 2011-03-03 fixed i-frames include aviheader, avoid add duplicated aviheader data */
  if (avc1_header_size && !add_avc1_extradata)
  {
	  if(memcmp(avc1_header, buffer, avc1_header_size) == 0)
	  {
		  printf("$$Video data include avi_header already$$\n");
		  add_avc1_extradata++;
	  }
	  else
	  {
		  add_drm_header_len = 1;
	  }
  }
#endif /* end of SUPPORT_DIVX_DRM_MKV */
  
	if(avc1_header_size != 0 && !add_avc1_extradata)
	{
		int check_byte = (avc1_nal_bytes == 3 ? 3 : 4);
#if 1	//Barry 2011-03-21
		//printf("[%s - %d]  comp_algo=%d       comp_settings_len=%d     comp_settings[0] = %.2X\n", __func__, __LINE__, track->encodings->comp_algo, track->encodings->comp_settings_len, track->encodings->comp_settings[0]);
		if (track->encodings && track->encodings->comp_algo == 3)
		{
			dp = new_demux_packet (size + avc1_header_size + track->encodings->comp_settings_len);
			memcpy(dp->buffer, avc1_header, avc1_header_size);
			memcpy(dp->buffer + avc1_header_size, track->encodings->comp_settings, track->encodings->comp_settings_len);
			memcpy(dp->buffer + avc1_header_size + track->encodings->comp_settings_len, buffer, size);
		}
		else
#endif
		{
			dp = new_demux_packet (size + avc1_header_size);
			memcpy(dp->buffer, avc1_header, avc1_header_size);
			memcpy(dp->buffer + avc1_header_size, buffer, size);
		}
		add_avc1_extradata++;
	}
	//Barry 2010-09-28
	else if ((!strcmp (track->codec_id, MKV_V_MPEG2) ) && !add_mpeg2_extradata)
	{
		unsigned char *buf = (unsigned char *)track->private_data;
		if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1 && buf[3] == 0xB3)
		{
			dp = new_demux_packet (size + track->private_size);
			memcpy(dp->buffer, track->private_data, track->private_size);
			memcpy(dp->buffer + track->private_size, buffer, size);
		}
		add_mpeg2_extradata++;
	}
	else
	{
#if 1	//Barry 2011-03-21
		if (track->encodings && track->encodings->comp_algo == 3)
		{
			dp = new_demux_packet (size + track->encodings->comp_settings_len);
			memcpy(dp->buffer, track->encodings->comp_settings, track->encodings->comp_settings_len);
			memcpy(dp->buffer + track->encodings->comp_settings_len, buffer, size);
		}
		else
#endif
		{
			dp = new_demux_packet (size);
			memcpy(dp->buffer, buffer, size);
		}
	}
#else
  dp = new_demux_packet (size);
  memcpy(dp->buffer, buffer, size);
#endif  

#ifdef  SUPPORT_DIVX_DRM_MKV
  if (DRM_Open > 0 && video_dd_chunk && dp->buffer)
  {
	  if (video_dd_chunk->EncryptionLength > 0)
	  {
		  //printf("### In [%s][%d] will do drm decyption EncryptionLength[%d]  offset [%d] DRM_Open[%d] video_dd_chunk[%p] block[%p] len [%d] size[%d]###\n", __func__, __LINE__, video_dd_chunk->EncryptionLength, video_dd_chunk->EncryptionOffset, DRM_Open, video_dd_chunk, dp->buffer, dp->len, size);
		  if (add_drm_header_len)
		  {
			  video_dd_chunk->EncryptionOffset += avc1_header_size;
			  add_drm_header_len = 0;
		  }
		  DRMDecode(size, dp->buffer, video_dd_chunk, DIVX_DECODE_VIDEO);
	  }
  }
#endif /* end of SUPPORT_DIVX_DRM_MKV */

  dp->pos = demuxer->filepos;
  dp->pts = mkv_d->last_pts;
  if ((track->num_cached_dps > 0) && (dp->pts < track->max_pts))
    block_fref = 1;
  if (block_fref == 0)          /* I or P frame */
    flush_cached_dps (demuxer, track);
  if (block_bref != 0)          /* I frame, don't cache it */
    dp->flags = 0x10;
  if ((track->num_cached_dps + 1) > track->num_allocated_dps)
    {
      track->cached_dps = (demux_packet_t **)
        realloc(track->cached_dps, (track->num_cached_dps + 10) *
                sizeof(demux_packet_t *));
      track->num_allocated_dps += 10;
    }
  track->cached_dps[track->num_cached_dps] = dp;
  track->num_cached_dps++;
  if (dp->pts > track->max_pts)
    track->max_pts = dp->pts;
}

static int
handle_block (demuxer_t *demuxer, uint8_t *block, uint64_t length,
              uint64_t block_duration, int64_t block_bref, int64_t block_fref, uint8_t simpleblock)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  mkv_track_t *track = NULL;
  demux_stream_t *ds = NULL;
  uint64_t old_length;
  int64_t tc;
  uint32_t *lace_size;
  uint8_t laces, flags;
  int i, num, tmp, use_this_block = 1;
  float current_pts;
  int16_t time;




  /* first byte(s): track num */
  num = ebml_read_vlen_uint (block, &tmp);
  block += tmp;
  /* time (relative to cluster time) */
  time = block[0] << 8 | block[1];
  block += 2;
  length -= tmp + 2;
  old_length = length;
  flags = block[0];
  if (demux_mkv_read_block_lacing (block, &length, &laces, &lace_size))
    return 0;
  block += old_length - length;

//  tc = ((time*mkv_d->tc_scale+mkv_d->cluster_tc) /1000000.0 - mkv_d->first_tc);
//	tc = ((time*mkv_d->tc_scale+mkv_d->cluster_tc) /(int64_t)1000000.0 - mkv_d->first_tc);    //SkyMedi_Vincent05182009 mplayer bug: add (int64_t)
  tc = ((int64_t)(((uint64_t)time)*mkv_d->tc_scale+mkv_d->cluster_tc) /(int64_t)1000000 - mkv_d->first_tc);    //Barry 2010-09-28
  if (tc < 0)
    tc = 0;
  if (mkv_d->stop_timecode > 0 && tc > mkv_d->stop_timecode) {
    if (lace_size)
    	free(lace_size);
    return -1;
  }
  current_pts = tc / 1000.0;

  for (i=0; i<mkv_d->num_tracks; i++)
    if (mkv_d->tracks[i]->tnum == num) {
      track = mkv_d->tracks[i];
      break;
    }
  if (track == NULL)
    {
      if (lace_size)
      	free(lace_size);
      return 1;
    }
  if (num == demuxer->audio->id)
    {
      ds = demuxer->audio;

      if (mkv_d->a_skip_to_keyframe)
        {
          if (simpleblock)
            {
               if (!(flags&0x80))   /*current frame isn't a keyframe*/
                 use_this_block = 0;
            }
          else if (block_bref != 0)
            use_this_block = 0;
        }
      else if (mkv_d->v_skip_to_keyframe)
        use_this_block = 0;

      if (track->fix_i_bps && use_this_block)
        {
          sh_audio_t *sh = (sh_audio_t *) ds->sh;

          if (block_duration != 0)
            {
              sh->i_bps = length * 1000 / block_duration;
              track->fix_i_bps = 0;
            }
          else if (track->qt_last_a_pts == 0.0)
            track->qt_last_a_pts = current_pts;
          else if(track->qt_last_a_pts != current_pts)
            {
              sh->i_bps = length / (current_pts - track->qt_last_a_pts);
              track->fix_i_bps = 0;
            }
        }
    }
  else if (tc < mkv_d->skip_to_timecode)
    use_this_block = 0;
  else if (num == demuxer->video->id)
    {
      ds = demuxer->video;
      if (mkv_d->v_skip_to_keyframe)
        {
          if (simpleblock)
            {
              if (!(flags&0x80))   /*current frame isn't a keyframe*/
                use_this_block = 0;
            }
          else if (block_bref != 0 || block_fref != 0)
            use_this_block = 0;
        }
    }
  else if (num == demuxer->sub->id)
    {
      ds = demuxer->sub;
      if (track->subtitle_type != MATROSKA_SUBTYPE_VOBSUB)
        {
          if (!mkv_d->v_skip_to_keyframe)
            handle_subtitles (demuxer, track, block, length,
                              block_duration, tc);
          use_this_block = 0;
        }
    }
  else
    use_this_block = 0;

  if (use_this_block)
    {
      mkv_d->last_pts = current_pts;
      mkv_d->last_filepos = demuxer->filepos;

      for (i=0; i < laces; i++)
        {
          if (ds == demuxer->video && track->realmedia)
            handle_realvideo (demuxer, track, block, lace_size[i], block_bref);
          else if (ds == demuxer->audio && track->realmedia)
            handle_realaudio (demuxer, track, block, lace_size[i], block_bref);
          else if (ds == demuxer->video && track->reorder_timecodes)
            handle_video_bframes (demuxer, track, block, lace_size[i],
                                  block_bref, block_fref);
          else
            {
              int modified, size = lace_size[i];
              demux_packet_t *dp;
              uint8_t *buffer;
              modified = demux_mkv_decode (track, block, &buffer, &size, 1);
              if (buffer)
                {
			// Raymond 2009/06/01
			if (num == demuxer->video->id)
			{
				if(!strcmp (track->codec_id, MKV_V_MPEG4_AVC) )
				{
//				printf("size = %d\n", size + avc1_header_size);
					if(avc1_header_size != 0 && !add_avc1_extradata)
					{
						dp = new_demux_packet (size + avc1_header_size);
//						dp = new_video_packet (size + avc1_header_size, videobuffer);
						memcpy(dp->buffer, avc1_header, avc1_header_size);
						memcpy(dp->buffer + avc1_header_size, buffer, size);
						//Barry 2010-08-24 marked, move to demux_close_mkv
						//free(avc1_header);
						//avc1_header_size = 0;
						add_avc1_extradata++;
					}
					else
					{
						dp = new_demux_packet (size);
//						dp = new_video_packet (size, videobuffer);
						memcpy(dp->buffer, buffer, size);
					}
				}

#ifdef _SKY_VDEC_V2	//johnnyke 20100727
				else if ( (!strcmp (track->codec_id, MKV_V_MSCOMP)) && (isVC1==1) )
				{
						unsigned int framePrefix[2];
						unsigned int timeStamp = (unsigned int)(current_pts * 1000);
						framePrefix[1] = timeStamp;
						
						dp = new_demux_packet (size + 8);
						if(!(block[0] & 0x80))
							framePrefix[0] = ((size - 8)&0x00FFFFFF) | 0x00000000;
						else
							framePrefix[0] = ((size - 8)&0x00FFFFFF) | 0x80000000;	// always keyframe ??!!
						PUT32_L(framePrefix[0], (unsigned char*)(dp->buffer));
						PUT32_L(framePrefix[1], (unsigned char*)(dp->buffer+4));
						memcpy(dp->buffer+8, buffer, size);
				}
#endif
#if 1	//Barry 2011-06-18   //Polun 2011-06-20 modify
				else if ( (!strcmp (track->codec_id, MKV_V_MSCOMP)) && (isVC1 == 2) )
				{
                                  int mkv_asfHeader_cunt;
                                  int wvc1_embade_header = 1;
                                    if (!add_wvc1_extradata)
					{
                                       for(mkv_asfHeader_cunt = 0; mkv_asfHeader_cunt < mkv_asfHeaderSize ;mkv_asfHeader_cunt++)
                                        {
                                             if(buffer[mkv_asfHeader_cunt] == mkv_asfHeader[mkv_asfHeader_cunt])
                                             {
                                                 wvc1_embade_header = 1;
                                              }
                                             else
                                             {
                                                 wvc1_embade_header = 0;
                                                 printf("!!!!!!!!no embade header \n");
                                                 break;
                                             }
                                        }

                                       if(wvc1_embade_header == 1 )
                                        {  
                                            dp = new_demux_packet(size);
                                            memcpy(dp->buffer, buffer, size);
                                        }
                                        else
                                        {
                                          dp = new_demux_packet(size + mkv_asfHeaderSize + 4);
						memcpy(dp->buffer, mkv_asfHeader, mkv_asfHeaderSize);
						dp->buffer[mkv_asfHeaderSize] = 0x00;
						dp->buffer[mkv_asfHeaderSize+1] = 0x00;
						dp->buffer[mkv_asfHeaderSize+2] = 0x01;
						dp->buffer[mkv_asfHeaderSize+3] = 0x0D;
						memcpy(dp->buffer+mkv_asfHeaderSize+4, buffer, size);
                                         }
                                        add_wvc1_extradata++;
					}
					else
					{
                                       if((buffer[0] == 0)&&(buffer[1] == 0) &&(buffer[2] == 0x01) &&(buffer[3] == 0x0d) )
                                        {
                                                dp = new_demux_packet(size);                                        
						      memcpy(dp->buffer, buffer, size);
                                       }
                                        else
                                        {
                                                if((buffer[mkv_asfHeaderSize]== 0)&&(buffer[mkv_asfHeaderSize +1] == 0) &&
                                                    (buffer[mkv_asfHeaderSize +2 ] == 0x01) &&(buffer[mkv_asfHeaderSize +3] == 0x0d) )
                                                {
                                                    wvc1_embade_header = 1;
                                                }
                                                else
                                                {
                                                    wvc1_embade_header = 0;
                                                }

                                                if(wvc1_embade_header == 1 )
                                                {  
                                                    dp = new_demux_packet(size - mkv_asfHeaderSize) ;
                                                    memcpy(dp->buffer, (buffer + mkv_asfHeaderSize ) , size -mkv_asfHeaderSize );
                                                }
                                                else
                                                {
                                                    dp = new_demux_packet(size + 4);
						          dp->buffer[0] = 0x00;
						          dp->buffer[1] = 0x00;
						          dp->buffer[2] = 0x01;
						          dp->buffer[3] = 0x0D;
						          memcpy(dp->buffer+4, buffer, size);
                                                }
                                        }
					}
                            }
#endif
				else	
				{
					dp = new_demux_packet (size);
//					dp = new_video_packet (size, videobuffer);
					memcpy (dp->buffer, buffer, size);
				}

                      }
			else
			//
			{
#if 1	//Barry 2011-03-21
				//printf("[%s - %d]  comp_algo=%d       comp_settings_len=%d     comp_settings[0] = %.2X\n", __func__, __LINE__, track->encodings->comp_algo, track->encodings->comp_settings_len, track->encodings->comp_settings[0]);
				if (num == demuxer->audio->id && track->encodings && track->encodings->comp_algo == 3)
				{
					dp = new_demux_packet (size + track->encodings->comp_settings_len);
					memcpy(dp->buffer, track->encodings->comp_settings, track->encodings->comp_settings_len);
					memcpy(dp->buffer + track->encodings->comp_settings_len, buffer, size);
				}
				else
#endif
				{
					dp = new_demux_packet (size);
					memcpy(dp->buffer, buffer, size);
				}
			}
#ifdef SUPPORT_DIVX_DRM_MKV             
			if (ds == demuxer->audio && audio_dd_chunk)
			{
				//printf("@@@@ In [%s][%d] this is audio @@@@\n", __func__, __LINE__);
				DRMDecode(size, dp->buffer, audio_dd_chunk, DIVX_DECODE_AUDIO);
			}
#endif /* end of SUPPORT_DIVX_DRM_MKV */
                  if (modified && buffer)
                    free (buffer);
                  dp->flags = (block_bref == 0 && block_fref == 0) ? 0x10 : 0;
                  /* If default_duration is 0, assume no pts value is known
                   * for packets after the first one (rather than all pts
                   * values being the same) */
                  if (i == 0 || track->default_duration)
                  dp->pts = mkv_d->last_pts + i * track->default_duration;
                  ds_add_packet (ds, dp);
                }

          }
          block += lace_size[i];
        }

      if (ds == demuxer->video)
        {
          mkv_d->v_skip_to_keyframe = 0;
          mkv_d->skip_to_timecode = 0;
        }
      else if (ds == demuxer->audio)
        mkv_d->a_skip_to_keyframe = 0;

      if (lace_size)
	  free(lace_size);
      return 1;
    }

  if (lace_size)
  	free(lace_size);
  return 0;
}

static int
demux_mkv_fill_buffer (demuxer_t *demuxer, demux_stream_t *ds)
{
#if 1	//Fuchun 2010.03.09
mkv_demuxer_t *mkv_d;
stream_t *s = demuxer->stream;;
uint64_t l;
int il, tmp;
static int64_t last_target_timecode = 0;
static unsigned int FR_cnt = 1;

//Barry 2010-12-29
if (s->eof)
{
	demuxer->audio->eof = 1;
	demuxer->video->eof = 1;
	return 0;
}

if(!FR_to_end && speed_mult < 0 && read_nextframe == 1)
{
	free_cached_dps (demuxer);
	mkv_d = (mkv_demuxer_t *) demuxer->priv;
	
	off_t current_pos = stream_tell(s);
	int i;
	if(mkv_d->indexes != NULL)
	{
		mkv_index_t *index = NULL;

		if(backward_keyframe_idx == 0)
		{
#if 0
			demuxer->stream->eof = 1;
			return 0;
#else
//			speed_mult = 0;
			FR_to_end = 1;
			
#endif
		}

		if(backward_keyframe_idx == -1)
		{
			for(i=(mkv_d->num_indexes-1); i >= 0; i--)
			{
				if(mkv_d->indexes[i].filepos <= current_pos && mkv_d->indexes[i].tnum == demuxer->video->id)
				{
					index = mkv_d->indexes + i;
					backward_keyframe_idx = i-1;
					if(backward_keyframe_idx < 0)
					{
						FR_to_end = 1;
					}
					break;
				}
			}
		}
		else
		{
			while((mkv_d->indexes[backward_keyframe_idx+1].filepos== mkv_d->indexes[backward_keyframe_idx].filepos
				|| mkv_d->indexes[backward_keyframe_idx].tnum != demuxer->video->id) && backward_keyframe_idx > 0)
				backward_keyframe_idx--;
			index = mkv_d->indexes + backward_keyframe_idx;
			backward_keyframe_idx--;
			if(backward_keyframe_idx < 0)
			{
//				speed_mult = 0;
				FR_to_end = 1;
			}
		}

          	if (index)
          	{
			mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              	stream_seek (s, index->filepos);
		}
	}
	else
	{
		int64_t target_timecode = 0, diff, min_diff=0xFFFFFFFFFFFFFFFLL;
		uint64_t target_filepos, cluster_pos;

        	target_timecode = (int64_t) (mkv_d->last_pts * 1000.0);
		if(target_timecode != last_target_timecode)	//Fuchun 20110801 avoid video lock at FR mode
		{
			last_target_timecode = target_timecode;
			FR_cnt = 1;
		}
		else
			FR_cnt++;
      		target_timecode += (int64_t)(-2.5 * FR_cnt * 1000.0);
      		if (target_timecode < 0)
      		{
        		target_timecode = 0;
			FR_to_end = 1;
      		}

          	target_filepos = (uint64_t) (target_timecode * mkv_d->last_filepos
                                       	/ (mkv_d->last_pts * 1000.0));

		cluster_pos = mkv_d->cluster_positions[0];
              /* Let's find the nearest cluster */
		for (i=0; i < mkv_d->num_cluster_pos; i++)
		{
			diff = mkv_d->cluster_positions[i] - target_filepos;
			if (diff < 0 && -diff < min_diff)
			{
				cluster_pos = mkv_d->cluster_positions[i];
				min_diff = -diff;
			}
		}
              mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              stream_seek (s, cluster_pos);
	}
	if (demuxer->video->id >= 0)
       	mkv_d->v_skip_to_keyframe = 1;
	
	read_nextframe = 0;
}
else if(read_nextframe == 1)
{
	backward_keyframe_idx=-1;
	last_target_timecode = 0;
	FR_cnt = 1;
	mkv_d = (mkv_demuxer_t *) demuxer->priv;
}
else
{
	mkv_d = (mkv_demuxer_t *) demuxer->priv;
}
#else
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t l;
  int il, tmp;
#endif

  while (1)
    {
      while (mkv_d->cluster_size > 0)
        {
          uint64_t block_duration = 0,  block_length = 0;
          int64_t block_bref = 0, block_fref = 0;
          uint8_t *block = NULL;

          while (mkv_d->blockgroup_size > 0)
            {
              switch (ebml_read_id (s, &il))
                {
                case MATROSKA_ID_BLOCKDURATION:
                  {
                    block_duration = ebml_read_uint (s, &l);
                    if (block_duration == EBML_UINT_INVALID) {
                      if(block)
				free(block);
                      return 0;
                    }
                    block_duration *= mkv_d->tc_scale / 1000000.0;
                    break;
                  }
#ifdef SUPPORT_DIVX_DRM_MKV             
				case MATROSKA_ID_DRMINFO: // 0xdd 
				  {
					  block_length = ebml_read_length (s, &l);
					  if (block_length == 10)
					  {
						  video_dd_chunk->KeyIdx = stream_read_word_le(demuxer->stream);
						  video_dd_chunk->EncryptionOffset = stream_read_dword_le(demuxer->stream);
						  video_dd_chunk->EncryptionLength = stream_read_dword_le(demuxer->stream);
						  l += block_length;

					  }
					  else
					  {
						  printf("$$$ In [%s][%d] block_length [%d] != 10 $$$\n", __func__, __LINE__, block_length);
					  }
					  /* next time, will do MATROSKA_ID_BLOCK */
					  break;
				  }
#endif /* end of SUPPORT_DIVX_DRM_MKV */
                case MATROSKA_ID_BLOCK:
                  block_length = ebml_read_length (s, &tmp);
                  if(block)
                  	free(block);
                  if (block_length > SIZE_MAX - AV_LZO_INPUT_PADDING) return 0;
                  block = malloc (block_length + AV_LZO_INPUT_PADDING);
                  if (!block)
			return 0;
                  demuxer->filepos = stream_tell (s);
                  if (stream_read (s,block,block_length) != (int) block_length)
                  {
                    if(block)
			free(block);
                    return 0;
                  }
                  l = tmp + block_length;
                  break;

                case MATROSKA_ID_REFERENCEBLOCK:
                  {
                    int64_t num = ebml_read_int (s, &l);
                    if (num == EBML_INT_INVALID) {
                      if(block)
				free(block);
                      return 0;
                    }
                    if (num <= 0)
                      block_bref = num;
                    else
                      block_fref = num;
                    break;
                  }

                case EBML_ID_INVALID:
                  if(block)
			free(block);
                  return 0;

                default:
                  ebml_read_skip (s, &l);
                  break;
                }
              mkv_d->blockgroup_size -= l + il;
              mkv_d->cluster_size -= l + il;
            }

          if (block)
            {
              int res = handle_block (demuxer, block, block_length,
                                      block_duration, block_bref, block_fref, 0);
              free (block);
              if (res < 0)
                return 0;
              if (res)
                return 1;
            }

          if (mkv_d->cluster_size > 0)
            {
              switch (ebml_read_id (s, &il))
                {
                case MATROSKA_ID_CLUSTERTIMECODE:
                  {
                    uint64_t num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      return 0;
                    if (!mkv_d->has_first_tc)
                      {
                        mkv_d->first_tc = num * mkv_d->tc_scale / 1000000.0;
                        mkv_d->has_first_tc = 1;
                      }
                    mkv_d->cluster_tc = num * mkv_d->tc_scale;
                    break;
                  }

                case MATROSKA_ID_BLOCKGROUP:
                  mkv_d->blockgroup_size = ebml_read_length (s, &tmp);
                  l = tmp;
                  break;

                case MATROSKA_ID_SIMPLEBLOCK:
                  {
                    int res;
                    block_length = ebml_read_length (s, &tmp);
                    block = malloc (block_length);
                    if (!block)
				return 0;
                    demuxer->filepos = stream_tell (s);
                    if (stream_read (s,block,block_length) != (int) block_length)
                    {
                      if(block)
				free(block);
                      return 0;
                    }
                    l = tmp + block_length;
                    res = handle_block (demuxer, block, block_length,
                                        block_duration, block_bref, block_fref, 1);
                    if(block)
				free(block);
                    mkv_d->cluster_size -= l + il;
                    if (res < 0)
                      return 0;
                    else if (res)
                      return 1;
                    else mkv_d->cluster_size += l + il;
                    break;
                  }
                case EBML_ID_INVALID:
                  return 0;

                default:
                  ebml_read_skip (s, &l);
                  break;
                }
              mkv_d->cluster_size -= l + il;
            }
        }
      
      #if 1   //Polun 2011-07-07 ++s
      while (ebml_read_id(s, &il) != MATROSKA_ID_CLUSTER) { 
             ebml_read_skip(s, NULL); 
            if (s->eof) 
                 return 0; 
            } 
      #else  //Polun 2011-07-07 ++e  
      if (ebml_read_id (s, &il) != MATROSKA_ID_CLUSTER)
         return 0;
      #endif
      add_cluster_position(mkv_d, stream_tell(s)-il);
      mkv_d->cluster_size = ebml_read_length (s, NULL);
    }

  return 0;
}

static void
demux_mkv_seek (demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
	if(h264_frame_mbs_only)	//Fuchun 20110915 don't need to add extradata for two field type
		add_avc1_extradata = 0;	//Barry 2010-08-24
	add_mpeg2_extradata = 0;	//Barry 2010-09-28
	add_wvc1_extradata = 0;

  free_cached_dps (demuxer);
  if (!(flags & SEEK_FACTOR))  /* time in secs */
    {
      mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
      stream_t *s = demuxer->stream;
      int64_t target_timecode = 0, diff, min_diff=0xFFFFFFFFFFFFFFFLL;
      int i;

      if (!(flags & SEEK_ABSOLUTE))  /* relative seek */
        target_timecode = (int64_t) (mkv_d->last_pts * 1000.0);
      target_timecode += (int64_t)(rel_seek_secs * 1000.0);
      if (target_timecode < 0)
        target_timecode = 0;

      if (mkv_d->indexes == NULL)  /* no index was found */
        {
          uint64_t target_filepos, cluster_pos, max_pos;

          target_filepos = (uint64_t) (target_timecode * mkv_d->last_filepos
                                       / (mkv_d->last_pts * 1000.0));

          max_pos = mkv_d->num_cluster_pos ? mkv_d->cluster_positions[mkv_d->num_cluster_pos-1] : 0;
          if (target_filepos > max_pos)
            {
              if ((off_t) max_pos > stream_tell (s))
                stream_seek (s, max_pos);
              else
                stream_seek (s, stream_tell (s) + mkv_d->cluster_size);
              /* parse all the clusters upto target_filepos */
              while (!s->eof && stream_tell(s) < (off_t) target_filepos)
                {
                  switch (ebml_read_id (s, &i))
                    {
                    case MATROSKA_ID_CLUSTER:
                      add_cluster_position(mkv_d, (uint64_t) stream_tell(s)-i);
                      break;

                    case MATROSKA_ID_CUES:
                      demux_mkv_read_cues (demuxer);
                      break;
                    }
                  ebml_read_skip (s, NULL);
                }
              if (s->eof)
                stream_reset(s);
            }

          if (mkv_d->indexes == NULL)
            {
               //Polun 2011-08-23 for mantis 5902
               #if 1
               if(!mkv_d->cluster_positions)
               {
                  printf("mkv_d->cluster_positions is null\n");
                  return;
               }
              else 
              #endif  
                  cluster_pos = mkv_d->cluster_positions[0];
              /* Let's find the nearest cluster */
              for (i=0; i < mkv_d->num_cluster_pos; i++)
                {
                  diff = mkv_d->cluster_positions[i] - target_filepos;
                  if (rel_seek_secs < 0 && diff < 0 && -diff < min_diff)
                    {
                      cluster_pos = mkv_d->cluster_positions[i];
                      min_diff = -diff;
                    }
                  else if (rel_seek_secs > 0
                           && (diff < 0 ? -1 * diff : diff) < min_diff)
                    {
                      cluster_pos = mkv_d->cluster_positions[i];
                      min_diff = diff < 0 ? -1 * diff : diff;
                    }
                }
              mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              stream_seek (s, cluster_pos);
            }
        }
      else
        {
          mkv_index_t *index = NULL;
          int seek_id = (demuxer->video->id < 0) ? demuxer->audio->id : demuxer->video->id;

          /* let's find the entry in the indexes with the smallest */
          /* difference to the wanted timecode. */
          for (i=0; i < mkv_d->num_indexes; i++)
            if (mkv_d->indexes[i].tnum == seek_id)
              {
                diff = target_timecode + mkv_d->first_tc -
                       (int64_t) mkv_d->indexes[i].timecode * mkv_d->tc_scale / 1000000.0;

                if ((flags & SEEK_ABSOLUTE || target_timecode <= mkv_d->last_pts*1000)) {
                    // Absolute seek or seek backward: find the last index
                    // position before target time
                    if (diff < 0 || diff >= min_diff)
                        continue;
                }
                else {
                    // Relative seek forward: find the first index position
                    // after target time. If no such index exists, find last
                    // position between current position and target time.
                    if (diff <= 0) {
                        if (min_diff <= 0 && diff <= min_diff)
                            continue;
                    }
                    else if (diff >= FFMIN(target_timecode - mkv_d->last_pts,
                                           min_diff))
                        continue;
                }
                min_diff = diff;
                index = mkv_d->indexes + i;
              }

          if (index)  /* We've found an entry. */
            {
              mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              stream_seek (s, index->filepos);
            }
            //Polun 2011-06-21 ++s  if FR, index is null and target_timecode = 0, reseek to file start.  
            else if((target_timecode == 0) && (rel_seek_secs < 0.0))
            {
                index = mkv_d->indexes;
                mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
                stream_seek (s, index->filepos);
                printf("==== FR to start=== \n");
            }
            //Polun 2011-06-21 ++e
        }

      if (demuxer->video->id >= 0)
        mkv_d->v_skip_to_keyframe = 1;
      if (rel_seek_secs > 0.0)
        mkv_d->skip_to_timecode = target_timecode;
      mkv_d->a_skip_to_keyframe = 1;

      demux_mkv_fill_buffer(demuxer, NULL);
    }
  else if ((demuxer->movi_end <= 0) || !(flags & SEEK_ABSOLUTE))
    mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] seek unsupported flags\n");
  else
    {
      mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
      stream_t *s = demuxer->stream;
      uint64_t target_filepos;
      mkv_index_t *index = NULL;
      int i;

      if (mkv_d->indexes == NULL)  /* no index was found */
        {                       /* I'm lazy... */
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] seek unsupported flags\n");
          return;
        }

      target_filepos = (uint64_t)(demuxer->movi_end * rel_seek_secs);
      for (i=0; i < mkv_d->num_indexes; i++)
        if (mkv_d->indexes[i].tnum == demuxer->video->id)
          if ((index == NULL) ||
              ((mkv_d->indexes[i].filepos >= target_filepos) &&
               ((index->filepos < target_filepos) ||
                (mkv_d->indexes[i].filepos < index->filepos))))
            index = &mkv_d->indexes[i];

      if (!index)
        return;

      mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
      stream_seek (s, index->filepos);

      if (demuxer->video->id >= 0)
        mkv_d->v_skip_to_keyframe = 1;
      mkv_d->skip_to_timecode = index->timecode;
      mkv_d->a_skip_to_keyframe = 1;

      demux_mkv_fill_buffer(demuxer, NULL);
    }
}

static int
demux_mkv_control (demuxer_t *demuxer, int cmd, void *arg)
{


  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;

  switch (cmd)
    {
    case DEMUXER_CTRL_CORRECT_PTS:
      return DEMUXER_CTRL_OK;
    case DEMUXER_CTRL_GET_TIME_LENGTH:
      if (mkv_d->duration == 0)
        return DEMUXER_CTRL_DONTKNOW;

      *((double *)arg) = (double)mkv_d->duration;
      return DEMUXER_CTRL_OK;

    case DEMUXER_CTRL_GET_PERCENT_POS:
      if (mkv_d->duration == 0)
        {
            return DEMUXER_CTRL_DONTKNOW;
        }

      *((int *) arg) = (int) (100 * mkv_d->last_pts / mkv_d->duration);
      return DEMUXER_CTRL_OK;

    case DEMUXER_CTRL_SWITCH_AUDIO:
      if (demuxer->audio && demuxer->audio->sh) {
        int aid = *(int*)arg;
#if 1	//Barry 2011-06-18
        if (aid == -2 && no_osd)
        {
      		*(int*)arg = -2;
		return DEMUXER_CTRL_OK;
        }
#endif
        sh_audio_t *sh = demuxer->a_streams[demuxer->audio->id];
        if (aid < 0)
          aid = (sh->aid + 1) % mkv_d->last_aid;
        if (aid != sh->aid) {
          mkv_track_t *track = demux_mkv_find_track_by_num (mkv_d, aid, MATROSKA_TRACK_AUDIO);
          //Polun 2011-06-15 ++s  if the audio track is not support
          if (!(track && demuxer->a_streams[track->tnum]))
          {
                track = NULL;
          }
           //Polun 2011-06-15 ++e
          if (track) {
            demuxer->audio->id = track->tnum;
            sh = demuxer->a_streams[demuxer->audio->id];
            ds_free_packs(demuxer->audio);
          }
        }
        *(int*)arg = sh->aid;
      } else
        *(int*)arg = -2;
      return DEMUXER_CTRL_OK;

    default:
      return DEMUXER_CTRL_NOTIMPL;
    }
}

const demuxer_desc_t demuxer_desc_matroska = {
  "Matroska demuxer",
  "mkv",
  "Matroska",
  "Aurelien Jacobs",
  "",
  DEMUXER_TYPE_MATROSKA,
  1, // safe autodetect
  demux_mkv_open,
  demux_mkv_fill_buffer,
  NULL,
  demux_close_mkv,
  demux_mkv_seek,
  demux_mkv_control
};
