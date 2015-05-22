/*
 * based on libmpeg2/header.c by Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mpeg_hdr.h"

#include "mp_msg.h"
//Robert 20100604
int h264_frame_mbs_only = 1;
int h264_frame_field = 1; //Polun 2011-12-09
int maxFrameNum = 1; //Polun 2011-12-13 for H264 get frame num. 
int log2_max_frame_num = 0;
//Barry 2010-06-17
int num_ref_frames = 0;
int h264_reorder_num = 0;
//Fuchun 2010.07.01
int pic_struct_present_flag = 0;
int nal_hrd_parameters_present_flag = 0;
int vcl_hrd_parameters_present_flag = 0;
int cpb_removal_delay_length = 0;
int dpb_output_delay_length = 0;
//int interlace_mbaff = 0; //Polun 2011-07-01 +
int mb_adaptive_frame_field_flag = 0;  //Polun 2012-01-02  it is MBAFF flag.

extern unsigned int vd_dpb_size;
extern unsigned int multibuf_op_mode;

//Robert 2010/12/31 add one more H264 JitterBuffer
int StandaloneH264JitterBufferNum = 4+1;	//Barry 2010-10-18

static float frameratecode2framerate[16] = {
  0,
  // Official mpeg1/2 framerates: (1-8)
  24000.0/1001, 24,25,
  30000.0/1001, 30,50,
  60000.0/1001, 60,
  // Xing's 15fps: (9)
  15,
  // libmpeg3's "Unofficial economy rates": (10-13)
  5,10,12,15,
  // some invalid ones: (14-15)
  0,0
};

TSVERSION ts_version;   //SkyMedi_Vincent06032009
extern int sky_repeatfield_cnt;
extern int sky_repeatfield_flag;

int mp_header_process_sequence_header (mp_mpeg_header_t * picture, const unsigned char * buffer)
{
    int width, height;

    if ((buffer[6] & 0x20) != 0x20){
	fprintf(stderr, "missing marker bit!\n");
	return 1;	/* missing marker_bit */
    }

    height = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];

    picture->display_picture_width = (height >> 12);
    picture->display_picture_height = (height & 0xfff);

    width = ((height >> 12) + 15) & ~15;
    height = ((height & 0xfff) + 15) & ~15;

    picture->aspect_ratio_information = buffer[3] >> 4;
    picture->frame_rate_code = buffer[3] & 15;
    picture->fps=frameratecode2framerate[picture->frame_rate_code];
    picture->bitrate = (buffer[4]<<10)|(buffer[5]<<2)|(buffer[6]>>6);
    picture->mpeg1 = 1;
    picture->picture_structure = 3; //FRAME_PICTURE;
    picture->display_time=100;
    picture->frame_rate_extension_n = 1;
    picture->frame_rate_extension_d = 1;
    return 0;
}

static int header_process_sequence_extension (mp_mpeg_header_t * picture,
					      unsigned char * buffer)
{
    /* check chroma format, size extensions, marker bit */

    if ( ((buffer[1] & 0x06) == 0x00) ||
         ((buffer[1] & 0x01) != 0x00) || (buffer[2] & 0xe0) ||
         ((buffer[3] & 0x01) != 0x01) )
	return 1;

    picture->progressive_sequence = (buffer[1] >> 3) & 1;
    picture->frame_rate_extension_n = ((buffer[5] >> 5) & 3) + 1;
    picture->frame_rate_extension_d = (buffer[5] & 0x1f) + 1;

    picture->mpeg1 = 0;
    return 0;
}


#ifdef CMV_WORKAROUND //JF 1031
     extern int  picture_coding_type; 
     extern int  mpeg_f_code_0_0;
     extern int  mpeg_f_code_0_1;
     extern int  c_m_v;
     extern int  picture_structure;
     extern int  frame_pred_frame_dct;
     extern int  q_scale_type;
     extern int  intra_vlc_format;
#endif

static int header_process_picture_coding_extension (mp_mpeg_header_t * picture, unsigned char * buffer)
{
    picture->picture_structure = buffer[2] & 3;
    picture->top_field_first = buffer[3] >> 7;
    picture->repeat_first_field = (buffer[3] >> 1) & 1;
    picture->progressive_frame = buffer[4] >> 7;

#ifdef CMV_WORKAROUND //JF 1031

    c_m_v =   buffer[3] & 32  ;
    if( c_m_v  &&   (picture_coding_type == 2) )
    	{
    	     picture_structure = picture->picture_structure;
    	     mpeg_f_code_0_0 = buffer[0] & 0x0f;
	     mpeg_f_code_0_1 = (buffer[1] & 0xf0) >> 4;
	     frame_pred_frame_dct =  (buffer[3] & 64)  >> 6 ;
	     q_scale_type  =    (buffer[3] & 16 )  >> 4 ;
	     intra_vlc_format =   (buffer[3] & 8 )  >> 3 ;	 
    	}		
	
#endif

    // repeat_first implementation by A'rpi/ESP-team, based on libmpeg3:
    picture->display_time=100;
    if(picture->repeat_first_field){
        if(picture->progressive_sequence){
            if(picture->top_field_first)
                picture->display_time+=200;
            else
                picture->display_time+=100;
        } else
        if(picture->progressive_frame){
                picture->display_time+=50;
                sky_repeatfield_cnt++;
                sky_repeatfield_flag = 1;
        }
    }
    //temopral hack. We calc time on every field, so if we have 2 fields
    // interlaced we'll end with double time for 1 frame
    if( picture->picture_structure!=3 ) picture->display_time/=2;
    return 0;
}

int mp_header_process_extension (mp_mpeg_header_t * picture, unsigned char * buffer)
{
    switch (buffer[0] & 0xf0) {
    case 0x10:	/* sequence extension */
	return header_process_sequence_extension (picture, buffer);
    case 0x80:	/* picture coding extension */
	return header_process_picture_coding_extension (picture, buffer);
    }
    return 0;
}

float mpeg12_aspect_info(mp_mpeg_header_t *picture)
{
    float aspect = 0.0;

    switch(picture->aspect_ratio_information) {
      case 2:  // PAL/NTSC SVCD/DVD 4:3
      case 8:  // PAL VCD 4:3
      case 12: // NTSC VCD 4:3
        aspect=4.0/3.0;
        break;
      case 3:  // PAL/NTSC Widescreen SVCD/DVD 16:9
      case 6:  // (PAL?)/NTSC Widescreen SVCD 16:9
        aspect=16.0/9.0;
        break;
      case 4:  // according to ISO-138182-2 Table 6.3
        aspect=2.21;
        break;
      case 1:  // VGA 1:1 - do not prescale
      case 9: // Movie Type ??? / 640x480
        aspect=0.0;
        break;
      default:
        mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Detected unknown aspect_ratio_information in mpeg sequence header.\n"
               "Please report the aspect value (%i) along with the movie type (VGA,PAL,NTSC,"
               "SECAM) and the movie resolution (720x576,352x240,480x480,...) to the MPlayer"
               " developers, so that we can add support for it!\nAssuming 1:1 aspect for now.\n",
               picture->aspect_ratio_information);
    }

    return aspect;
}

//MPEG4 HEADERS
unsigned char mp_getbits(unsigned char *buffer, unsigned int from, unsigned char len)
{
    unsigned int n;
    unsigned char m, u, l, y;

    n = from / 8;
    m = from % 8;
    u = 8 - m;
    l = (len > u ? len - u : 0);

    y = (buffer[n] << m);
    if(8 > len)
    	y  >>= (8-len);
    if(l)
    	y |= (buffer[n+1] >> (8-l));

    //fprintf(stderr, "GETBITS(%d -> %d): bytes=0x%x 0x%x, n=%d, m=%d, l=%d, u=%d, Y=%d\n",
    //	from, (int) len, (int) buffer[n],(int) buffer[n+1], n, (int) m, (int) l, (int) u, (int) y);
    return  y;
}

static inline unsigned int mp_getbits16(unsigned char *buffer, unsigned int from, unsigned char len)
{
    if(len > 8)
        return (mp_getbits(buffer, from, len - 8) << 8) | mp_getbits(buffer, from + len - 8, 8);
    else
        return mp_getbits(buffer, from, len);
}

#define getbits mp_getbits
#define getbits16 mp_getbits16

static int read_timeinc(mp_mpeg_header_t * picture, unsigned char * buffer, int n)
{
    if(picture->timeinc_bits > 8) {
      picture->timeinc_unit = getbits(buffer, n, picture->timeinc_bits - 8) << 8;
      n += picture->timeinc_bits - 8;
      picture->timeinc_unit |= getbits(buffer, n, 8);
      n += 8;
    } else {
      picture->timeinc_unit = getbits(buffer, n, picture->timeinc_bits);
      n += picture->timeinc_bits;
    }
    //fprintf(stderr, "TIMEINC2: %d, bits: %d\n", picture->timeinc_unit, picture->timeinc_bits);
    return n;
}

//Barry 2010-09-21 disable	#ifdef _SKY_VDEC_V1	//Barry 2010-06-10
int check_mp4_header_vol(unsigned char * buf, int buf_size)
{
	unsigned int n=0, aspect=0, aspectw=0, aspecth=0, v, x=0,  vol_shape=0, vol_verid=0, sprite_enable=0, vop_time_increment_resolution=0, visual_object_verid = 1, find_start_code = 0;
	unsigned char* buffer = malloc(buf_size);
	//mp_mpeg_header_t picture1;
	memcpy(buffer, buf, buf_size);

	for (x=0;x<buf_size-3;x++)
	{
		if (buffer[x] == 0 && buffer[x+1] == 0 && buffer[x+2] == 1)
		{
			if (buffer[x+3] == 0xB5)	//visual_object_start_code
			{
				n = ((x+4)<<3);
				if (getbits(buffer, n, 1))
				{
					n++;
					visual_object_verid = getbits(buffer, n, 4);
				}
			}
			if ((buffer[x+3]>>4) == 2)
			{
				n = ((x+4)<<3);
				find_start_code = 1;
				break;
			}
		}
	}

	//Barry 2010-06-14
	if (!find_start_code)
	{
		free(buffer);
		return 1;
	}

	//begins with 0x0000012x
	n += 9;
	if(getbits(buffer, n, 1))
	{
		n++;
		vol_verid = getbits(buffer, n, 4);
		n += 7;
	}
	else
	{
		n++;
		vol_verid = visual_object_verid;
	}

	aspect=getbits(buffer, n, 4);
	n += 4;
	if(aspect == 0x0f)
	{
		aspectw = getbits(buffer, n, 8);
		n += 8;
		aspecth = getbits(buffer, n, 8);
		n += 8;
	}

	if(getbits(buffer, n, 1))
	{
		n += 4;
		if(getbits(buffer, n, 1))
			n += 79;
		n++;
	}
	else
		n++;

	vol_shape = getbits(buffer, n, 2);
	//printf("\nvol_shape=%d\n", vol_shape);
	n += 3;

	vop_time_increment_resolution = getbits16(buffer, n, 16);
	//printf("vop_time_resolution=%X\n", vop_time_increment_resolution);
	n += 16;	//timeinc_resolution

	v = vop_time_increment_resolution - 1;

	//keep shifting number by 1 bit position to the right, till it becomes zero
	for (x=1; x<=16; x++ )
	{
		v >>= 1;
		if (0 == v)
			break;
	}
	n++; //marker bit

	if(getbits(buffer, n, 1))
	{	//fixed_vop_timeinc
		n++;
		n+=x;
	}
	else
		n++;

	n += 29;
	n++;	//interlaced
	n++;	//obmc_disable
	
	//printf("\nvol_verid=%d\n", vol_verid);
	if (0x1 == vol_verid)
	{
		sprite_enable = getbits(buffer, n, 1);
		n++;
	}
	else
	{
		sprite_enable = getbits(buffer, n, 2);
		n+=2;
	}
	//printf("\nsprite_enable=%d\n", sprite_enable);

	free(buffer);
	if ((0x1 == sprite_enable) || (0x2 == sprite_enable))
		return 0;
	else
		return 1;
}
//#endif

int mp4_header_process_vol(mp_mpeg_header_t * picture, unsigned char * buffer)
{
    unsigned int n, aspect=0, aspectw=0, aspecth=0,  x=1, v, tmp_res = 0;

    //begins with 0x0000012x
    picture->fps = 0;
    picture->timeinc_bits = picture->timeinc_resolution = picture->timeinc_unit = 0;
    n = 9;
    if(getbits(buffer, n, 1))
      n += 7;
    n++;
    aspect=getbits(buffer, n, 4);
    n += 4;
    if(aspect == 0x0f) {
      aspectw = getbits(buffer, n, 8);
      n += 8;
      aspecth = getbits(buffer, n, 8);
      n += 8;
    }

    if(getbits(buffer, n, 1)) {
      n += 4;
      if(getbits(buffer, n, 1))
        n += 79;
      n++;
    } else n++;

    n+=3;

    picture->timeinc_resolution = getbits(buffer, n, 8) << 8;
    n += 8;
    picture->timeinc_resolution |= getbits(buffer, n, 8);
    n += 8;

    picture->timeinc_bits = 0;
    v = picture->timeinc_resolution - 1;
    while(v && (x<16)) {
      v>>=1;
      picture->timeinc_bits++;
    }
    picture->timeinc_bits = (picture->timeinc_bits > 1 ? picture->timeinc_bits : 1);

    n++; //marker bit
    if(getbits(buffer, n, 1)) {	//fixed_vop_timeinc
      n++;
      n = read_timeinc(picture, buffer, n);

      if(picture->timeinc_unit)
        picture->fps = (float) picture->timeinc_resolution / (float) picture->timeinc_unit;
    }
#if 1	//Barry 2010-11-18
    else
    	n++;

    n++; //marker bit
    tmp_res = getbits(buffer, n, 8);
    n+=8;
    tmp_res = (tmp_res<<5) | (getbits(buffer, n, 5));
    n+=5;
    picture->display_picture_width = tmp_res;
    n++; //marker bit

    tmp_res = getbits(buffer, n, 8);
    n+=8;
    tmp_res = (tmp_res<<5) | (getbits(buffer, n, 5));
    n+=5;
    picture->display_picture_height = tmp_res;
//	printf("\n\nW x H   =   %d   x   %d\n\n", picture->display_picture_width, picture->display_picture_height);
#endif

    //fprintf(stderr, "ASPECT: %d, PARW=%d, PARH=%d, TIMEINCRESOLUTION: %d, FIXED_TIMEINC: %d (number of bits: %d), FPS: %u\n",
    //	aspect, aspectw, aspecth, picture->timeinc_resolution, picture->timeinc_unit, picture->timeinc_bits, picture->fps);

    return 0;
}

void mp4_header_process_vop(mp_mpeg_header_t * picture, unsigned char * buffer)
{
  int n;
  n = 0;
  picture->picture_type = getbits(buffer, n, 2);
  n += 2;
  while(getbits(buffer, n, 1))
    n++;
  n++;
  getbits(buffer, n, 1);
  n++;
  n = read_timeinc(picture, buffer, n);
}

#define min(a, b) ((a) <= (b) ? (a) : (b))

static unsigned int read_golomb(unsigned char *buffer, unsigned int *init)
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

  //fprintf(stderr, "READ_GOLOMB(%u), V=2^%u + %u-1 = %u\n", *init, len, v, v2);
  *init = x;
  return v2;
}

inline static int read_golomb_s(unsigned char *buffer, unsigned int *init)
{
  unsigned int v = read_golomb(buffer, init);
  return (v & 1) ? ((v + 1) >> 1) : -(v >> 1);
}

static int h264_parse_vui(mp_mpeg_header_t * picture, unsigned char * buf, unsigned int n)
{
  unsigned int overscan, vsp_color, chroma, timing, fixed_fps, tmp_w, tmp_h;

  if(getbits(buf, n++, 1))
  {
    picture->aspect_ratio_information = getbits(buf, n, 8);
    n += 8;
    if(picture->aspect_ratio_information == 255)
    {
      tmp_w = (getbits(buf, n, 8) << 8) | getbits(buf, n + 8, 8);
      n += 16;
      tmp_h = (getbits(buf, n, 8) << 8) | getbits(buf, n + 8, 8);
      n += 16;
    }
  }

  if((overscan=getbits(buf, n++, 1)))
    n++;
  if((vsp_color=getbits(buf, n++, 1)))
  {
    n += 4;
    if(getbits(buf, n++, 1))
      n += 24;
  }
  if((chroma=getbits(buf, n++, 1)))
  {
    read_golomb(buf, &n);
    read_golomb(buf, &n);
  }
  if((timing=getbits(buf, n++, 1)))
  {
    picture->timeinc_unit = (getbits(buf, n, 8) << 24) | (getbits(buf, n+8, 8) << 16) | (getbits(buf, n+16, 8) << 8) | getbits(buf, n+24, 8);
    n += 32;

    picture->timeinc_resolution = (getbits(buf, n, 8) << 24) | (getbits(buf, n+8, 8) << 16) | (getbits(buf, n+16, 8) << 8) | getbits(buf, n+24, 8);
    n += 32;

    fixed_fps = getbits(buf, n++, 1);

    if(picture->timeinc_unit > 0 && picture->timeinc_resolution > 0)
      picture->fps = (float) picture->timeinc_resolution / (float) picture->timeinc_unit;
    if(fixed_fps)
      picture->fps /= 2;
  }

  {	
  	unsigned int i, v;
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
		cpb_removal_delay_length = getbits(buf, n, 5)+ 1;
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
		cpb_removal_delay_length = getbits(buf, n, 5)+ 1;
		n += 5;
		dpb_output_delay_length = getbits(buf, n, 5)+ 1;
		n += 10;
	}
	if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
		n++;
	pic_struct_present_flag = getbits(buf, n++, 1);

	if(getbits(buf, n++, 1))
	{
		getbits(buf, n++, 1);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		read_golomb(buf, &n);
		h264_reorder_num = read_golomb(buf, &n);
		printf("@@@@@ h264_reorder_num:%d @@@@@\n", h264_reorder_num);
		read_golomb(buf, &n);
	}
  }

  //fprintf(stderr, "H264_PARSE_VUI, OVESCAN=%u, VSP_COLOR=%u, CHROMA=%u, TIMING=%u, DISPW=%u, DISPH=%u, TIMERES=%u, TIMEINC=%u, FIXED_FPS=%u\n", overscan, vsp_color, chroma, timing, picture->display_picture_width, picture->display_picture_height,
  //	picture->timeinc_resolution, picture->timeinc_unit, picture->timeinc_unit, fixed_fps);

  return n;
}

static int mp_unescape03(unsigned char *buf, int len);

int h264_parse_sps(mp_mpeg_header_t * picture, unsigned char * buf, int len)
{
  unsigned int n = 0, v, i, k, mbh;
  int frame_mbs_only;

  //Fuchun 2010.07.01
  nal_hrd_parameters_present_flag = 0;
  vcl_hrd_parameters_present_flag = 0;
  cpb_removal_delay_length = 0;
  dpb_output_delay_length = 0;
  h264_reorder_num = 0;

  len = mp_unescape03(buf, len);

  picture->fps = picture->timeinc_unit = picture->timeinc_resolution = 0;
  n = 24;
  read_golomb(buf, &n);
  if(buf[0] >= 100){
    if(read_golomb(buf, &n) == 3)
      n++;
    read_golomb(buf, &n);
    read_golomb(buf, &n);
    n++;
    if(getbits(buf, n++, 1)){
      for(i = 0; i < 8; i++)
      {  // scaling list is skipped for now
        if(getbits(buf, n++, 1))
        {
          v = 8;
          for(k = (i < 6 ? 16 : 64); k && v; k--)
            v = (v + read_golomb_s(buf, &n)) & 255;
        }
      }
    }
  }
  #if 0 //Polun 2011-12-13 for H264 get frame num. 
  read_golomb(buf, &n);
  #else
  log2_max_frame_num= read_golomb(buf, &n) +4;
  while(log2_max_frame_num)
  {
      maxFrameNum = maxFrameNum*2;
      log2_max_frame_num -=1;
  }
  #endif
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
  num_ref_frames = read_golomb(buf, &n);		//Barry 2010-06-17
  getbits(buf, n++, 1);
  picture->display_picture_width = 16 *(read_golomb(buf, &n)+1);
  mbh = read_golomb(buf, &n)+1;
  frame_mbs_only = getbits(buf, n++, 1);
  picture->display_picture_height = 16 * (2 - frame_mbs_only) * mbh;
//Robert 20100604
//  h264_frame_mbs_only = frame_mbs_only;	//Fuchun 2010.07.01 disable
  if(!frame_mbs_only)
  {  
       #if 0  //Polun 2012-01-02  it is to get correcte MBAFF flag.
       getbits(buf, n++, 1);
       #else
       mb_adaptive_frame_field_flag = getbits(buf, n++, 1);
       //printf("############## mb_adaptive_frame_field_flag = %d $$$$$$$$$$$$\n",mb_adaptive_frame_field_flag);
       if(mb_adaptive_frame_field_flag == 1)
       {
           printf("###### scan type is MBAFF ####\n");
       }
       #endif
  } 
  getbits(buf, n++, 1);
  if(getbits(buf, n++, 1))
  {
    read_golomb(buf, &n);
    read_golomb(buf, &n);
    read_golomb(buf, &n);
    read_golomb(buf, &n);
  }
  if(getbits(buf, n++, 1))
    n = h264_parse_vui(picture, buf, n);

  //Barry 2010-05-12	If have 2 fields interlaced, set double fps for decoder
  //Fuchun 2010.07.01 disable
//  if (!frame_mbs_only)
//  	picture->fps *= 2;

//+SkyMedi_Vincent08102009
    if(ts_version == TS_2 && picture->fps == 0)
        picture->fps = 30.0;
//SkyMedi_Vincent08102009+ 

	if(h264_reorder_num == 0)	//Fuchun 20110922
		h264_reorder_num = num_ref_frames;

  return n;
}

//Barry 2010-09-03
float h264_aspect_info(mp_mpeg_header_t *picture)
{
	float aspect = 0.0;
	switch(picture->aspect_ratio_information)
	{
		case 1:  // 1:1
		{
			aspect = ((float)(picture->display_picture_width)) / ((float)(picture->display_picture_height));
			break;
		}
		default:
		{
			//Barry 2010-11-09
			if (picture->display_picture_width == 1440 && picture->display_picture_height == 1088)
			{
				aspect = 1.7778;	//16:9
				break;
			}

			printf("Bug, fix me later! h264: picture->aspect_ratio_information=%d\n", picture->aspect_ratio_information);
			aspect = ((float)(picture->display_picture_width)) / ((float)(picture->display_picture_height));
			break;
		}
	}
	return aspect;
}


//Fuchun 2010.07.01
int h264_parse_sei(unsigned char *buf, int len)
{
	int n = 0;
	int temp_byte;
	int sei_pic_struct;
	unsigned char *temp_buf = malloc(len);
	memcpy(temp_buf, buf, len);
	len = mp_unescape03(&temp_buf[n], len);
	do
	{
		int size, type;

		type = 0;
		temp_byte = getbits(temp_buf, n, 8);
		while(temp_byte == 0xff)
		{
			type += 255;
			n += 8;
			temp_byte = getbits(temp_buf, n, 8);
		}
		type += temp_byte;
		n += 8;

		size = 0;
		temp_byte = getbits(temp_buf, n, 8);
		while(temp_byte == 0xff)
		{
			size += 255;
			n += 8;
			temp_byte = getbits(temp_buf, n, 8);
		}
		size += temp_byte;
		n += 8;

		switch(type)
		{
			case 1:		//SEI_TYPE_PIC_TIMING
				if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
					n+=(cpb_removal_delay_length + dpb_output_delay_length);
				sei_pic_struct = getbits(temp_buf, n, 4);
                             //Polun 2011-07-01 ++s
                            if( sei_pic_struct == 3)
                            {
                                // interlace_mbaff = 1 ;
				    //printf("=== scan type is MBAFF ===\n");
				    printf("=== H264 sei_pic_struct = 3 ===\n");
                            }
                            else
                            {
                                // interlace_mbaff = 0 ; 
                            }
                            //Polun 2011-07-01 ++e
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
		temp_byte = getbits(temp_buf, n, 8);
	}while(temp_byte != 0x80);

    if(temp_buf)
        free(temp_buf);

	return 0;
}

//Polun 2011-12-09 add  h264_video_sei to fixed MBAFF file merge two field to decoder .
int h264_video_sei(unsigned char *buf, int len)
{
	int n = 0;
	int temp_byte;
	int sei_pic_struct;
	unsigned char *temp_buf = malloc(len);
	memcpy(temp_buf, buf, len);
	len = mp_unescape03(&temp_buf[n], len);
	do
	{
		int size, type;

		type = 0;
		temp_byte = getbits(temp_buf, n, 8);
		while(temp_byte == 0xff)
		{
			type += 255;
			n += 8;
			temp_byte = getbits(temp_buf, n, 8);
		}
		type += temp_byte;
		n += 8;

		size = 0;
		temp_byte = getbits(temp_buf, n, 8);
		while(temp_byte == 0xff)
		{
			size += 255;
			n += 8;
			temp_byte = getbits(temp_buf, n, 8);
		}
		size += temp_byte;
		n += 8;

		switch(type)
		{
			case 1:		//SEI_TYPE_PIC_TIMING
				if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
					n+=(cpb_removal_delay_length + dpb_output_delay_length);
				sei_pic_struct = getbits(temp_buf, n, 4);
                            //Barry 2011-12-09
                            if( sei_pic_struct == 3)
                            {
                                 //interlace_mbaff = 1 ;
				    //printf("=== scan type is MBAFF ===\n");
				    printf("=== H264 sei_pic_struct = 3 ===\n");
                            }
				//printf("=== H264 sei_pic_struct = %d ===\n", sei_pic_struct);
				if(sei_pic_struct == 1 || sei_pic_struct == 2) 
				{
				       //if(h264_frame_field == 1)
				            //printf("h264_frame_field    1  ==> 0\n");
					h264_frame_field = 0;	//if sei_pic_struct = SEI_PIC_STRUCT_TOP_FIELD or SEI_PIC_STRUCT_BOTTOM_FIELD
                                   
					if(temp_buf)
						free(temp_buf);
					return 2;
				}
                            else
                            {
                                   //if(h264_frame_field == 0)
				            //printf("h264_frame_field    0  ==> 1\n");
                                   h264_frame_field = 1;
                            }
                            if(temp_buf)
					free(temp_buf);
				return 1;
			default:
				n += 8*size;
				break;
		}
		temp_byte = getbits(temp_buf, n, 8);
	}while(temp_byte != 0x80);

	if(temp_buf)
		free(temp_buf);

	return 0;
}

int check_h264_sps_bank_size(mp_mpeg_header_t * picture)
{
	unsigned int frame_size;
	frame_size =  ( (((picture->display_picture_width+15)>>4)<<4 ) * ( ((picture->display_picture_height+15)>>4)<<4) / (16*16) * 448 );

	if (multibuf_op_mode)
	{
		if ( (frame_size*(num_ref_frames+2)) > vd_dpb_size )
		{
			printf("[TS-avc1] video memory not enough:\n");
			printf("WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", picture->display_picture_width, picture->display_picture_height, num_ref_frames, vd_dpb_size, frame_size*(num_ref_frames+2));
			return 0;
		}
		else
			return 1;
	}
	else	//standalone mode
	{
		//Barry 2010-10-26  //Polun 2011-06-23 modify to num_ref_frames >= 5
		if ( ((picture->display_picture_width >= 1920) || (picture->display_picture_height >= 1080)) && (num_ref_frames >= 5) )
			StandaloneH264JitterBufferNum = 3;

#if 1	//Barry 2010-12-13
		if ( (frame_size*((num_ref_frames+1)*2+3)) > vd_dpb_size )	//at least need 3 jitter buffer
#else
		if ( (frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum)) > vd_dpb_size )
#endif
		{
			printf("[TS-avc1] video memory not enough:\n");
			printf("WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", picture->display_picture_width, picture->display_picture_height, num_ref_frames, vd_dpb_size, frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum));
			return 0;
		}
		else
		{
#if 1	//Barry 2010-12-13
			int compute_bufnum = (vd_dpb_size - (frame_size*((num_ref_frames+1)*2))) / frame_size;
//			printf("01 dpb_size = %d, frame_size = %d, no jitter need size = %d, compute_bufnum = %d\n", vd_dpb_size, frame_size, frame_size*((num_ref_frames+1)*2), compute_bufnum);
			if (StandaloneH264JitterBufferNum > compute_bufnum)
				StandaloneH264JitterBufferNum = compute_bufnum;
			printf("[%s] Set StandaloneH264JitterBufferNum = %d\n", __func__, StandaloneH264JitterBufferNum);
#endif
			return 1;
		}
	}
}

int check_avc1_sps_bank0(unsigned char *in_buf, int len)
{
	unsigned int n = 0, v, i, k, w, h, frame_size;
	unsigned char frame_field_flag = 0, frame_mbs_only_flag = 0;
	
	unsigned char *buf;
	buf = malloc(len);
	memcpy(buf, in_buf, len);
	len = mp_unescape03(buf, len);

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
	w = read_golomb(buf, &n) + 1;
	h = read_golomb(buf, &n) + 1;
#if 1	
	frame_mbs_only_flag = getbits(buf, n++, 1);	//Fuchun 2010.07.14 disable ==> MOV need check frame or field, carlos enable 2011-12-23, for \\172.26.40.14\data3\customer\asicen\SANY0245.MP4
	if (!frame_mbs_only_flag)
	{
		frame_field_flag = getbits(buf, n++, 1);
              mb_adaptive_frame_field_flag = frame_field_flag; //Polun 2012-01-02 for MBAFF flag
		if (!frame_field_flag)
		{
			h264_frame_mbs_only = 0;
			printf("$$ In [%s][%d] set h264_frame_mbs_only to 0 for field file $$\n", __func__, __LINE__);
		}
		else
			h264_frame_mbs_only = 1;
	}
#else
	if(!getbits(buf, n++, 1))
		getbits(buf, n++, 1);
#endif		
	getbits(buf, n++, 1);

	frame_size = ( ((((w<<4)+15)>>4)<<4 ) * ( ((((2-h264_frame_mbs_only)*h<<4)+15)>>4)<<4) / (16*16) * 448 );
	free(buf);

	if (multibuf_op_mode)
	{
		if ( (frame_size*(num_ref_frames+2)) > vd_dpb_size )
		{
			printf("Video memory not enough:\n");
			printf("WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", w<<4, h<<4, num_ref_frames, vd_dpb_size, frame_size*(num_ref_frames+2));
			return 0;
		}
		else
			return 1;
	}
	else	//standalone mode
	{
		//Barry 2010-10-26   //Polun 2011-06-23 modify to num_ref_frames >= 5
		if ( (((w<<4) >= 1920) || ((h<<4) >= 1080)) && (num_ref_frames >= 5) )
			StandaloneH264JitterBufferNum = 3;

#if 1	//Barry 2010-12-13
		if ( (frame_size*((num_ref_frames+1)*2+3)) > vd_dpb_size )	//at least need 3 jitter buffer
#else
		if ( (frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum)) > vd_dpb_size )
#endif
		{
			printf("Video memory not enough:\n");
			printf("WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", w<<4, h<<4, num_ref_frames, vd_dpb_size, frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum));
			return 0;
		}
		else
		{
#if 1	//Barry 2010-12-13
			int compute_bufnum = (vd_dpb_size - (frame_size*((num_ref_frames+1)*2))) / frame_size;
//			printf("02 dpb_size = %d, frame_size = %d, no jitter need size = %d, compute_bufnum = %d\n", vd_dpb_size, frame_size, frame_size*((num_ref_frames+1)*2), compute_bufnum);
			if (StandaloneH264JitterBufferNum > compute_bufnum)
				StandaloneH264JitterBufferNum = compute_bufnum;
			printf("[%s] Set StandaloneH264JitterBufferNum = %d\n", __func__, StandaloneH264JitterBufferNum);
#endif
			return 1;
		}
	}
}

static int mp_unescape03(unsigned char *buf, int len)
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

int mp_vc1_decode_sequence_header(mp_mpeg_header_t * picture, unsigned char * buf, int len)
{
  int n, x;

  len = mp_unescape03(buf, len);

  picture->display_picture_width = picture->display_picture_height = 0;
  picture->fps = 0;
  n = 0;
  x = getbits(buf, n, 2);
  n += 2;
  if(x != 3) //not advanced profile
    return 0;

  getbits16(buf, n, 14);
  n += 14;
  picture->display_picture_width = getbits16(buf, n, 12) * 2 + 2;
  n += 12;
  picture->display_picture_height = getbits16(buf, n, 12) * 2 + 2;
  n += 12;
  getbits(buf, n, 6);
  n += 6;
  x = getbits(buf, n, 1);
  n += 1;
  if(x) //display info
  {
    getbits16(buf, n, 14);
    n += 14;
    getbits16(buf, n, 14);
    n += 14;
    if(getbits(buf, n++, 1)) //aspect ratio
    {
      x = getbits(buf, n, 4);
      n += 4;
      if(x == 15)
      {
        getbits16(buf, n, 16);
        n += 16;
      }
    }

    if(getbits(buf, n++, 1)) //framerates
    {
      int frexp=0, frnum=0, frden=0;

      if(getbits(buf, n++, 1))
      {
        frexp = getbits16(buf, n, 16);
        n += 16;
        picture->fps = (double) (frexp+1) / 32.0;
      }
      else
      {
        float frates[] = {0, 24000, 25000, 30000, 50000, 60000, 48000, 72000, 0};
        float frdivs[] = {0, 1000, 1001, 0};

        frnum = getbits(buf, n, 8);
        n += 8;
        frden = getbits(buf, n, 4);
        n += 4;
        if((frden == 1 || frden == 2) && (frnum < 8))
            picture->fps = frates[frnum] / frdivs[frden];
      }
    }
  }

  //free(dest);
  return 1;
}
