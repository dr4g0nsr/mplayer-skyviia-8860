/*
 * FLV demuxer
 * Copyright (c) 2003 The FFmpeg Project
 *
 * This demuxer will generate a 1 byte extradata for VP6F content.
 * It is composed of:
 *  - upper 4bits: difference between encoded width and visible width
 *  - lower 4bits: difference between encoded height and visible height
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/avstring.h"
#include "libavcodec/bytestream.h"
#include "libavcodec/mpeg4audio.h"
#include "avformat.h"
#include "flv.h"

#include "libavcodec/get_bits.h"
#include "libavcodec/golomb.h"

//Fuchun 2010.03.11
static int backward_keyframe_idx=-1;		
int64_t last_timestamp;
extern int FR_to_end;
extern int num_reorder_frames;
extern int queue_frames;
extern int first_start_code;
extern unsigned int vd_dpb_size;
extern unsigned int multibuf_op_mode;
extern int StandaloneH264JitterBufferNum;	//Barry 2010-10-18
extern int h264_frame_mbs_only;

//Barry 2010-10-18
int find_flv_res=0, flv_width=0, flv_height=0;
static int garbage_byte_cnt = 0;	//Barry 2010-11-22

typedef struct {
    int wrong_dts; ///< wrong dts due to negative cts
} FLVContext;

int decode_264_sps_flv(const uint8_t *buf, int buf_size)
{
	GetBitContext gb;
	const uint8_t *ptr;
	int i, v, k;
	int buf_index = 2;
	int bit_length;
	int dst_length;
	int length = buf_size - buf_index;
	int reorder_frames = 0;
	uint8_t *dst = NULL;
	int frame_mbs_only = 0, mbh = 0;
	unsigned int frame_size = 0;
	int num_ref_frames = 0;

	//h264_decode_nal
	ptr = buf + buf_index;
	ptr++;
	length--;
	for(i=0; i+1<length; i+=2){
       	if(ptr[i]) continue;
		if(i>0 && ptr[i-1] == 0) i--;

		if(i+2<length && ptr[i+1]==0 && ptr[i+2]<=3){
			if(ptr[i+2]!=3){
				/* startcode, so we must be past the end */
				length=i;
			}
			break;
		}
	}
	
	if(i >= length - 1)
	{
		dst_length= length;
	}
	else
	{
		dst = av_malloc(length + FF_INPUT_BUFFER_PADDING_SIZE);
		int si, di;
		memcpy(dst, ptr, i);
		si=di=i;

		while(si+2<length){
			//remove escapes (very rare 1:2^22)
			if(ptr[si+2]>3){
				dst[di++]= ptr[si++];
				dst[di++]= ptr[si++];
			}else if(ptr[si]==0 && ptr[si+1]==0){
				if(ptr[si+2]==3){ //escape
					dst[di++]= 0;
					dst[di++]= 0;
					si+=3;
					continue;
				}else //next start code
					goto nsc;
			}

			dst[di++]= ptr[si++];
		}
    		while(si<length)
			dst[di++]= ptr[si++];
nsc:
		memset(dst+di, 0, FF_INPUT_BUFFER_PADDING_SIZE);
		dst_length= di;
		ptr = dst;
	}


	v = *(ptr + dst_length - 1);
	int r;
	for(r = 1; r < 9; r++)
	{
		if(v & 1) break;
		v >>= 1;
	}
	if(r == 9) r = 0;
	bit_length= !dst_length ? 0 : (8*dst_length - r);


	//init_get_bits
	int buffer_size = (bit_length+7) >> 3;
	if(buffer_size < 0 || bit_length < 0)
	{
		buffer_size = bit_length = 0;
		ptr = NULL;
	}
	gb.buffer = ptr;
    	gb.size_in_bits = bit_length;
    	gb.buffer_end = ptr + buffer_size;
	gb.index = 0;	//Fix me


	//h264_decode_seq_parameter_set
	if(get_bits(&gb, 8) >= 100)		//profile_idc
	{
		get_bits(&gb, 16);
		get_ue_golomb_31(&gb);	//sps_id
		if(get_ue_golomb_31(&gb) == 3)	//chroma_format_idc
			get_bits1(&gb);		//residual_color_transform_flag
		get_ue_golomb(&gb);
		get_ue_golomb(&gb);
		get_bits1(&gb);		//transform_bypass
		
		if(get_bits1(&gb))	//decode_scaling_matrices
		{
			for(i = 0; i < 8; i++)
			{
				if(get_bits1(&gb))
				{
					v = 8;
					for(k = (i < 6 ? 16 : 64); k && v; k--)
						v = (v + get_ue_golomb(&gb)) & 255;
				}
			}
		}
	}
	else
	{
		get_bits(&gb, 16);
		get_ue_golomb_31(&gb);
	}

	get_ue_golomb(&gb);
	v = get_ue_golomb_31(&gb);	//poc_type
	if(v == 0)
		get_ue_golomb(&gb);
	else if(v == 1)
	{
		get_bits1(&gb);
		get_ue_golomb(&gb);
		get_ue_golomb(&gb);
		v = get_ue_golomb(&gb);
		for(i = 0; i < v; i++)
			get_ue_golomb(&gb);
	}
	num_ref_frames = get_ue_golomb_31(&gb);	//ref_frame_count
	get_bits1(&gb);			//gaps_in_frame_num_allowed_flag
#if 1	//Barry 2010-10-18
	flv_width = 16 * (get_ue_golomb(&gb) + 1);
	mbh = get_ue_golomb(&gb) + 1;
	frame_mbs_only = get_bits1(&gb);
	flv_height = 16 * (2 - frame_mbs_only) * mbh;

	if(!frame_mbs_only)		//frame_mbs_only_flag
		get_bits1(&gb);	//mb_aff

	frame_size = ( ((((flv_width)+15)>>4)<<4 ) * ( ((((2-h264_frame_mbs_only)*flv_height)+15)>>4)<<4) / (16*16) * 448 );
	if (multibuf_op_mode)
	{
		if ( (frame_size*(num_ref_frames+2)) > vd_dpb_size )
		{
			av_log(NULL, AV_LOG_ERROR, "Video memory not enough:\n");
			av_log(NULL, AV_LOG_ERROR, "WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", flv_width, flv_height, num_ref_frames
				, vd_dpb_size, frame_size*(num_ref_frames+2));
		}
	}
	else
	{
		//Barry 2010-10-26
		if ( (((flv_width) >= 1920) || ((flv_height) >= 1080)) && (num_ref_frames == 5) )
			StandaloneH264JitterBufferNum = 3;

#if 1	//Barry 2010-12-13
		if ( (frame_size*((num_ref_frames+1)*2+3)) > vd_dpb_size )	//at least need 3 jitter buffer
#else
		if ( (frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum)) > vd_dpb_size )
#endif
		{
			av_log(NULL, AV_LOG_ERROR, "Video memory not enough:\n");
			av_log(NULL, AV_LOG_ERROR, "WxH = %dx%d, num_ref_frames=%d   vd_dpb_size=%d     need_dpb_size=%d\n\n\n", flv_width, flv_height, num_ref_frames
				, vd_dpb_size, frame_size*((num_ref_frames+1)*2+StandaloneH264JitterBufferNum));
		}
		else
		{
#if 1	//Barry 2010-12-13
			int compute_bufnum = (vd_dpb_size - (frame_size*((num_ref_frames+1)*2))) / frame_size;
//			av_log(NULL, AV_LOG_ERROR, "01 dpb_size = %d, frame_size = %d, no jitter need size = %d, compute_bufnum = %d\n", vd_dpb_size, frame_size, frame_size*((num_ref_frames+1)*2), compute_bufnum);
			if (StandaloneH264JitterBufferNum > compute_bufnum)
				StandaloneH264JitterBufferNum = compute_bufnum;
			av_log(NULL, AV_LOG_ERROR, "[%s] Set StandaloneH264JitterBufferNum = %d\n", __func__, StandaloneH264JitterBufferNum);
#endif
		}
	}
#else
	get_ue_golomb(&gb);
	get_ue_golomb(&gb);

	if(!get_bits1(&gb))		//frame_mbs_only_flag
		get_bits1(&gb);	//mb_aff
#endif

	get_bits1(&gb);		//direct_8x8_inference_flag
	if(get_bits1(&gb))		//crop
	{
		get_ue_golomb(&gb);	//crop_left
		get_ue_golomb(&gb);	//crop_right
		get_ue_golomb(&gb);	//crop_top
		get_ue_golomb(&gb);	//crop_bottom
	}

	if(get_bits1(&gb))		//vui_parameters_present_flag
	{
		if(get_bits1(&gb))	//aspect_ratio_info_present_flag
		{
			if(get_bits(&gb, 8) == 255)	//aspect_ratio_idc
			{
				get_bits(&gb, 16);		//sar.num
				get_bits(&gb, 16);		//sar.den
			}
		}

		if(get_bits1(&gb))		/* overscan_info_present_flag */
		{
			get_bits1(&gb);	/* overscan_appropriate_flag */
		}

		if(get_bits1(&gb))		//video_signal_type_present_flag
		{
			get_bits(&gb, 3);	/* video_format */
			get_bits1(&gb);	/* video_full_range_flag */

			if(get_bits1(&gb))	//colour_description_present_flag
			{
				get_bits(&gb, 8);	/* colour_primaries */
				get_bits(&gb, 8);	/* transfer_characteristics */
				get_bits(&gb, 8);	/* matrix_coefficients */
			}
		}

		if(get_bits1(&gb))
		{
			get_ue_golomb(&gb);
			get_ue_golomb(&gb);	/* chroma_sample_location_type_bottom_field */
		}

		if(get_bits1(&gb))		//timing_info_present_flag
		{
			get_bits_long(&gb, 32);	//num_units_in_tick
			get_bits_long(&gb, 32);	//time_scale
			get_bits1(&gb);			//fixed_frame_rate_flag
		}

		int nal_hrd_parameters_present_flag = get_bits1(&gb);	//nal_hrd_parameters_present_flag
		int vcl_hrd_parameters_present_flag = get_bits1(&gb);	//vcl_hrd_parameters_present_flag
		if(nal_hrd_parameters_present_flag)
		{
			v = get_ue_golomb_31(&gb) + 1;
			get_bits(&gb, 8);

			for(i = 0; i < v; i++)
			{
				get_ue_golomb(&gb);
				get_ue_golomb(&gb);
				get_bits1(&gb);
			}
			get_bits(&gb, 20);
		}
		if(vcl_hrd_parameters_present_flag)
		{
			v = get_ue_golomb_31(&gb) + 1;
			get_bits(&gb, 8);

			for(i = 0; i < v; i++)
			{
				get_ue_golomb(&gb);
				get_ue_golomb(&gb);
				get_bits1(&gb);
			}
			get_bits(&gb, 20);
		}
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
			get_bits1(&gb);
		get_bits1(&gb);	//pic_struct_present_flag

		if(get_bits1(&gb))	//bitstream_restriction_flag
		{
			get_bits1(&gb);		/* motion_vectors_over_pic_boundaries_flag */
			get_ue_golomb(&gb);	/* max_bytes_per_pic_denom */
			get_ue_golomb(&gb);	/* max_bits_per_mb_denom */
			get_ue_golomb(&gb);	/* log2_max_mv_length_horizontal */
			get_ue_golomb(&gb);	/* log2_max_mv_length_vertical */
			reorder_frames = get_ue_golomb(&gb);	/*num_reorder_frames*/
			get_ue_golomb(&gb);	/*max_dec_frame_buffering*/
		}
	}
	if(dst) av_free(dst);
	return reorder_frames;
}

static int flv_probe(AVProbeData *p)
{
    const uint8_t *d;

    d = p->buf;
    if (d[0] == 'F' && d[1] == 'L' && d[2] == 'V' && d[3] < 5 && d[5]==0 && AV_RB32(d+5)>8) {
        return AVPROBE_SCORE_MAX;
    }
    return 0;
}

static void flv_set_audio_codec(AVFormatContext *s, AVStream *astream, int flv_codecid) {
    AVCodecContext *acodec = astream->codec;
    switch(flv_codecid) {
        //no distinction between S16 and S8 PCM codec flags
        case FLV_CODECID_PCM:
            acodec->codec_id = acodec->bits_per_coded_sample == 8 ? CODEC_ID_PCM_U8 :
#if HAVE_BIGENDIAN
                                CODEC_ID_PCM_S16BE;
#else
                                CODEC_ID_PCM_S16LE;
#endif
            break;
        case FLV_CODECID_PCM_LE:
            acodec->codec_id = acodec->bits_per_coded_sample == 8 ? CODEC_ID_PCM_U8 : CODEC_ID_PCM_S16LE; break;
        case FLV_CODECID_AAC  : acodec->codec_id = CODEC_ID_AAC;                                    break;
        case FLV_CODECID_ADPCM: acodec->codec_id = CODEC_ID_ADPCM_SWF;                              break;
        case FLV_CODECID_SPEEX:
            acodec->codec_id = CODEC_ID_SPEEX;
            acodec->sample_rate = 16000;
            break;
        case FLV_CODECID_MP3  : acodec->codec_id = CODEC_ID_MP3      ; astream->need_parsing = AVSTREAM_PARSE_FULL; break;
        case FLV_CODECID_NELLYMOSER_8KHZ_MONO:
            acodec->sample_rate = 8000; //in case metadata does not otherwise declare samplerate
        case FLV_CODECID_NELLYMOSER:
            acodec->codec_id = CODEC_ID_NELLYMOSER;
            break;
        default:
            av_log(s, AV_LOG_INFO, "Unsupported audio codec (%x)\n", flv_codecid >> FLV_AUDIO_CODECID_OFFSET);
            acodec->codec_tag = flv_codecid >> FLV_AUDIO_CODECID_OFFSET;
    }
}

static int flv_set_video_codec(AVFormatContext *s, AVStream *vstream, int flv_codecid) {
    AVCodecContext *vcodec = vstream->codec;

    switch(flv_codecid) {
        case FLV_CODECID_H263:
			vcodec->codec_id = CODEC_ID_FLV1;
#if 1	//Barry 2010-10-18
			if (flv_width && flv_height)
			{
				vcodec->width = flv_width;
				vcodec->height = flv_height;
				find_flv_res = 1;
			}

			//get FLV resolution
			if (find_flv_res == 0)
			{
				int format, hdr=0, read_tmp=0;
				off_t tmp_pos;

				tmp_pos = url_ftell(s->pb);
				hdr = get_be24(s->pb);
				//av_log(s, AV_LOG_ERROR, "\nhdr = %d       url_ftell(s->pb)=%d\n", hdr, url_ftell(s->pb));
				if (hdr != 0x84)
				{
					url_fseek(s->pb, tmp_pos, SEEK_SET);
					break;
				}
				else
				{
					read_tmp = get_be16(s->pb);
					format = (read_tmp>>7) & 0x7;
					//av_log(s, AV_LOG_ERROR, "\nformat = %d\n", format);
					switch (format)
					{
						case 0:
							read_tmp = (read_tmp<<8) | get_byte(s->pb);
							vcodec->width = (read_tmp >> 7) & 0xFF;
							read_tmp = ((read_tmp & 0x7F) << 8) | get_byte(s->pb);
							vcodec->height = (read_tmp >> 7) & 0xFF;
							break;
						case 1:
							read_tmp = (read_tmp<<16) | get_be16(s->pb);
							vcodec->width = (read_tmp >> 7) & 0xFFFF;
							read_tmp = ((read_tmp & 0x7F) << 16) | get_be16(s->pb);
							vcodec->height = (read_tmp >> 7) & 0xFFFF;
							break;
						case 2:
							vcodec->width = 352;
							vcodec->height = 288;
							break;
						case 3:
							vcodec->width = 176;
							vcodec->height = 144;
							break;
						case 4:
							vcodec->width = 128;
							vcodec->height = 96;
							break;
						case 5:
							vcodec->width = 320;
							vcodec->height = 240;
							break;
						case 6:
							vcodec->width = 160;
							vcodec->height = 120;
							break;
						default:
							vcodec->width = vcodec->height = 0;
							break;
					}
					find_flv_res++;
					url_fseek(s->pb, tmp_pos, SEEK_SET);
				}
			}
#endif
			break;
        case FLV_CODECID_SCREEN: vcodec->codec_id = CODEC_ID_FLASHSV; break;
        case FLV_CODECID_SCREEN2: vcodec->codec_id = CODEC_ID_FLASHSV2; break;
        case FLV_CODECID_VP6   : vcodec->codec_id = CODEC_ID_VP6F   ;
        case FLV_CODECID_VP6A  :
            if(flv_codecid == FLV_CODECID_VP6A)
                vcodec->codec_id = CODEC_ID_VP6A;
            if(vcodec->extradata_size != 1) {
                vcodec->extradata_size = 1;
                vcodec->extradata = av_malloc(1);
            }
            vcodec->extradata[0] = get_byte(s->pb);
            return 1; // 1 byte body size adjustment for flv_read_packet()
        case FLV_CODECID_H264:
            vcodec->codec_id = CODEC_ID_H264;
            return 3; // not 4, reading packet type will consume one byte
        default:
            av_log(s, AV_LOG_INFO, "Unsupported video codec (%x)\n", flv_codecid);
            vcodec->codec_tag = flv_codecid;
    }

    return 0;
}

static int amf_get_string(ByteIOContext *ioc, char *buffer, int buffsize) {
    int length = get_be16(ioc);
    if(length >= buffsize) {
        url_fskip(ioc, length);
        return -1;
    }

    get_buffer(ioc, buffer, length);

    buffer[length] = '\0';

    return length;
}

static int amf_parse_object(AVFormatContext *s, AVStream *astream, AVStream *vstream, const char *key, int64_t max_pos, int depth) {
    AVCodecContext *acodec, *vcodec;
    ByteIOContext *ioc;
    AMFDataType amf_type;
    char str_val[256];
    double num_val;

    num_val = 0;
    ioc = s->pb;

    amf_type = get_byte(ioc);

    switch(amf_type) {
        case AMF_DATA_TYPE_NUMBER:
            num_val = av_int2dbl(get_be64(ioc)); break;
        case AMF_DATA_TYPE_BOOL:
            num_val = get_byte(ioc); break;
        case AMF_DATA_TYPE_STRING:
            if(amf_get_string(ioc, str_val, sizeof(str_val)) < 0)
                return -1;
            break;
        case AMF_DATA_TYPE_OBJECT: {
            unsigned int keylen;

            while(url_ftell(ioc) < max_pos - 2 && (keylen = get_be16(ioc))) {
                url_fskip(ioc, keylen); //skip key string
                if(amf_parse_object(s, NULL, NULL, NULL, max_pos, depth + 1) < 0)
                    return -1; //if we couldn't skip, bomb out.
            }
            if(get_byte(ioc) != AMF_END_OF_OBJECT)
                return -1;
        }
            break;
        case AMF_DATA_TYPE_NULL:
        case AMF_DATA_TYPE_UNDEFINED:
        case AMF_DATA_TYPE_UNSUPPORTED:
            break; //these take up no additional space
        case AMF_DATA_TYPE_MIXEDARRAY:
            url_fskip(ioc, 4); //skip 32-bit max array index
            while(url_ftell(ioc) < max_pos - 2 && amf_get_string(ioc, str_val, sizeof(str_val)) > 0) {
                //this is the only case in which we would want a nested parse to not skip over the object
                if(amf_parse_object(s, astream, vstream, str_val, max_pos, depth + 1) < 0)
                    return -1;
            }
            if(get_byte(ioc) != AMF_END_OF_OBJECT)
                return -1;
            break;
        case AMF_DATA_TYPE_ARRAY: {
            unsigned int arraylen, i;

            arraylen = get_be32(ioc);
            for(i = 0; i < arraylen && url_ftell(ioc) < max_pos - 1; i++) {
                if(amf_parse_object(s, NULL, NULL, NULL, max_pos, depth + 1) < 0)
                    return -1; //if we couldn't skip, bomb out.
            }
        }
            break;
        case AMF_DATA_TYPE_DATE:
            url_fskip(ioc, 8 + 2); //timestamp (double) and UTC offset (int16)
            break;
        default: //unsupported type, we couldn't skip
            return -1;
    }

    if(depth == 1 && key) { //only look for metadata values when we are not nested and key != NULL
        acodec = astream ? astream->codec : NULL;
        vcodec = vstream ? vstream->codec : NULL;

#if 1	//Barry 2010-10-18
		//printf("key=%s   num_val=%f\n", key, num_val);
	if(!strcmp(key, "width"))
		flv_width = (int)num_val;
	if(!strcmp(key, "height"))
		flv_height = (int)num_val;
	//av_log(s, AV_LOG_ERROR, "\n\namf_parse_object %d x %d\n\n", flv_width, flv_height);
#endif

        if(amf_type == AMF_DATA_TYPE_BOOL) {
            av_strlcpy(str_val, num_val > 0 ? "true" : "false", sizeof(str_val));
            av_metadata_set(&s->metadata, key, str_val);
        } else if(amf_type == AMF_DATA_TYPE_NUMBER) {
            snprintf(str_val, sizeof(str_val), "%.f", num_val);
            av_metadata_set(&s->metadata, key, str_val);
            if(!strcmp(key, "duration")) s->duration = num_val * AV_TIME_BASE;
            else if(!strcmp(key, "videodatarate") && vcodec && 0 <= (int)(num_val * 1024.0))
                vcodec->bit_rate = num_val * 1024.0;
            //Polun 2011-09-08 for flv file meta data have bitrate but  vcodec->bit_rate = 0
            else if(!strcmp(key, "bitrate") &&  vcodec->bit_rate == 0)
                vcodec->bit_rate = num_val;
        } else if (amf_type == AMF_DATA_TYPE_STRING)
          av_metadata_set(&s->metadata, key, str_val);
    }

    return 0;
}

static int flv_read_metabody(AVFormatContext *s, int64_t next_pos) {
    AMFDataType type;
    AVStream *stream, *astream, *vstream;
    ByteIOContext *ioc;
    int i;
    char buffer[11]; //only needs to hold the string "onMetaData". Anything longer is something we don't want.

    astream = NULL;
    vstream = NULL;
    ioc = s->pb;

    //first object needs to be "onMetaData" string
    type = get_byte(ioc);
    if(type != AMF_DATA_TYPE_STRING || amf_get_string(ioc, buffer, sizeof(buffer)) < 0 || strcmp(buffer, "onMetaData"))
        return -1;

    //find the streams now so that amf_parse_object doesn't need to do the lookup every time it is called.
    for(i = 0; i < s->nb_streams; i++) {
        stream = s->streams[i];
        if     (stream->codec->codec_type == CODEC_TYPE_AUDIO) astream = stream;
        else if(stream->codec->codec_type == CODEC_TYPE_VIDEO) vstream = stream;
    }

    //parse the second object (we want a mixed array)
    if(amf_parse_object(s, astream, vstream, buffer, next_pos, 0) < 0)
        return -1;

    return 0;
}

static AVStream *create_stream(AVFormatContext *s, int is_audio){
    AVStream *st = av_new_stream(s, is_audio);
    if (!st)
        return NULL;
    st->codec->codec_type = is_audio ? CODEC_TYPE_AUDIO : CODEC_TYPE_VIDEO;
    av_set_pts_info(st, 32, 1, 1000); /* 32 bit pts in ms */
    return st;
}

static int flv_read_header(AVFormatContext *s,
                           AVFormatParameters *ap)
{
    int offset, flags;

	//Fuchun 2010.08.11 reset global variable
	backward_keyframe_idx=-1;
	find_flv_res=0, flv_width=0, flv_height=0;	//Barry 2010-11-05 reset global variable
	garbage_byte_cnt = 0;	//Barry 2010-11-22

    url_fskip(s->pb, 4);
    flags = get_byte(s->pb);
    /* old flvtool cleared this field */
    /* FIXME: better fix needed */
    if (!flags) {
        flags = FLV_HEADER_FLAG_HASVIDEO | FLV_HEADER_FLAG_HASAUDIO;
        av_log(s, AV_LOG_WARNING, "Broken FLV file, which says no streams present, this might fail\n");
    }

    if((flags & (FLV_HEADER_FLAG_HASVIDEO|FLV_HEADER_FLAG_HASAUDIO))
             != (FLV_HEADER_FLAG_HASVIDEO|FLV_HEADER_FLAG_HASAUDIO))
        s->ctx_flags |= AVFMTCTX_NOHEADER;

    if(flags & FLV_HEADER_FLAG_HASVIDEO){
        if(!create_stream(s, 0))
            return AVERROR(ENOMEM);
    }
    if(flags & FLV_HEADER_FLAG_HASAUDIO){
        if(!create_stream(s, 1))
            return AVERROR(ENOMEM);
    }

    offset = get_be32(s->pb);
    url_fseek(s->pb, offset, SEEK_SET);

    s->start_time = 0;

    return 0;
}

static int flv_get_extradata(AVFormatContext *s, AVStream *st, int size)
{
    av_free(st->codec->extradata);
    st->codec->extradata = av_mallocz(size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!st->codec->extradata)
        return AVERROR(ENOMEM);
    st->codec->extradata_size = size;
    get_buffer(s->pb, st->codec->extradata, st->codec->extradata_size);
    return 0;
}

static int flv_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    FLVContext *flv = s->priv_data;
    int ret, i, type, size, flags, is_audio;
    int64_t next, pos;
    int64_t dts, pts = AV_NOPTS_VALUE;
    AVStream *st = NULL;

#if 1	//Fuchun 2010.03.11
	if(!FR_to_end && speed_mult < 0)
	{
		pos = url_ftell(s->pb);
		for(i = 0; i < s->nb_streams; i++)
		{
			st = s->streams[i];
			if(st->codec->codec_type == CODEC_TYPE_VIDEO)
				break;
		}

		if(backward_keyframe_idx == 0)
		{
#if 0
			return AVERROR(EIO);
#else
//			speed_mult = 0;
			FR_to_end = 1;
#endif
		}

		if(backward_keyframe_idx == -1)
		{
			for(i=(st->nb_index_entries-1); i >= 0; i--)
			{
				if(st->index_entries[i].pos <= pos)
				{
					url_fseek(s->pb, st->index_entries[i].pos, SEEK_SET);
					last_timestamp = st->index_entries[i].timestamp;
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
			while((last_timestamp - st->index_entries[backward_keyframe_idx].timestamp) < (int64_t)(st->time_base.den/st->time_base.num) 	//let over 1 second between two keyframes
				&& backward_keyframe_idx > 0)	
				backward_keyframe_idx--;
			url_fseek(s->pb, st->index_entries[backward_keyframe_idx].pos, SEEK_SET);
			last_timestamp = st->index_entries[backward_keyframe_idx].timestamp;
			backward_keyframe_idx--;
			if(backward_keyframe_idx < 0)
			{
//				speed_mult = 0;
				FR_to_end = 1;
			}
		}
	}
	else
		backward_keyframe_idx = -1;
#endif

 for(;;){
    pos = url_ftell(s->pb);
    url_fskip(s->pb, 4); /* size of previous packet */
    type = get_byte(s->pb);
    size = get_be24(s->pb);
    dts = get_be24(s->pb);
    dts |= get_byte(s->pb) << 24;
//	av_log(s, AV_LOG_ERROR, "pos:%d\n", pos);
//    av_log(s, AV_LOG_ERROR, "type:%d, size:%d, dts:%d\n", type, size, dts);
	
    if (url_feof(s->pb))
        return AVERROR_EOF;
    url_fskip(s->pb, 3); /* stream id, always 0 */
    flags = 0;

    if(size == 0)
        continue;

    next= size + url_ftell(s->pb);

    if (type == FLV_TAG_TYPE_AUDIO) {
        is_audio=1;
        flags = get_byte(s->pb);
        size--;
    } else if (type == FLV_TAG_TYPE_VIDEO) {
        is_audio=0;
        flags = get_byte(s->pb);
        size--;
        if ((flags & 0xf0) == 0x50) /* video info / command frame */
            goto skip;
    } else {
        if (type == FLV_TAG_TYPE_META && size > 13+1+4)
            flv_read_metabody(s, next);
        else /* skip packet */
            av_log(s, AV_LOG_DEBUG, "skipping flv packet: type %d, size %d, flags %d\n", type, size, flags);
    skip:
        url_fseek(s->pb, next, SEEK_SET);
        continue;
    }

    /* skip empty data packets */
    if (!size)
        continue;

    /* now find stream */
    for(i=0;i<s->nb_streams;i++) {
        st = s->streams[i];
        if (st->id == is_audio)
            break;
    }
    if(i == s->nb_streams){
        av_log(s, AV_LOG_ERROR, "invalid stream\n");
        st= create_stream(s, is_audio);
        s->ctx_flags &= ~AVFMTCTX_NOHEADER;
    }
//    av_log(s, AV_LOG_DEBUG, "%d %X %d \n", is_audio, flags, st->discard);
    if(  (st->discard >= AVDISCARD_NONKEY && !((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY ||         is_audio))
       ||(st->discard >= AVDISCARD_BIDIR  &&  ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_DISP_INTER && !is_audio))
       || st->discard >= AVDISCARD_ALL
       ){
        url_fseek(s->pb, next, SEEK_SET);
        continue;
    }
    if ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY)
        av_add_index_entry(st, pos, dts, size, 0, AVINDEX_KEYFRAME);
    break;
 }

    // if not streamed and no duration from metadata then seek to end to find the duration from the timestamps
    if(!url_is_streamed(s->pb) && (!s->duration || s->duration==AV_NOPTS_VALUE)){
        int size;
        const int64_t pos= url_ftell(s->pb);
        const int64_t fsize= url_fsize(s->pb);
        url_fseek(s->pb, fsize-4, SEEK_SET);
        size= get_be32(s->pb);
        url_fseek(s->pb, fsize-3-size, SEEK_SET);
        if(size == get_be24(s->pb) + 11){
            uint32_t ts = get_be24(s->pb);
            ts |= get_byte(s->pb) << 24;
            s->duration = ts * (int64_t)AV_TIME_BASE / 1000;
        }
        url_fseek(s->pb, pos, SEEK_SET);
    }

    if(is_audio){
        if(!st->codec->channels || !st->codec->sample_rate || !st->codec->bits_per_coded_sample) {
            st->codec->channels = (flags & FLV_AUDIO_CHANNEL_MASK) == FLV_STEREO ? 2 : 1;
            st->codec->sample_rate = (44100 << ((flags & FLV_AUDIO_SAMPLERATE_MASK) >> FLV_AUDIO_SAMPLERATE_OFFSET) >> 3);
            st->codec->bits_per_coded_sample = (flags & FLV_AUDIO_SAMPLESIZE_MASK) ? 16 : 8;
        }
        if(!st->codec->codec_id){
            flv_set_audio_codec(s, st, flags & FLV_AUDIO_CODECID_MASK);
        }
    }else{
        size -= flv_set_video_codec(s, st, flags & FLV_VIDEO_CODECID_MASK);
    }

    if (st->codec->codec_id == CODEC_ID_AAC ||
        st->codec->codec_id == CODEC_ID_H264) {
        int type = get_byte(s->pb);
        size--;
        if (st->codec->codec_id == CODEC_ID_H264) {
            int32_t cts = (get_be24(s->pb)+0xff800000)^0xff800000; // sign extension
            pts = dts + cts;
            if (cts < 0) { // dts are wrong
                flv->wrong_dts = 1;
                av_log(s, AV_LOG_WARNING, "negative cts, previous timestamps might be wrong\n");
            }
            if (flv->wrong_dts)
                dts = AV_NOPTS_VALUE;
        }
        if (type == 0) {
            if ((ret = flv_get_extradata(s, st, size)) < 0)
                return ret;
            if (st->codec->codec_id == CODEC_ID_AAC) {
                MPEG4AudioConfig cfg;
                ff_mpeg4audio_get_config(&cfg, st->codec->extradata,
                                         st->codec->extradata_size);
                st->codec->channels = cfg.channels;
                st->codec->sample_rate = cfg.sample_rate;
                dprintf(s, "mp4a config channels %d sample rate %d\n",
                        st->codec->channels, st->codec->sample_rate);
            }

            return AVERROR(EAGAIN);
        }
    }

    /* skip empty data packets */
    if (!size)
        return AVERROR(EAGAIN);

#if 1	//Fuchun 2010.01.11
	if(st->codec->codec_id == CODEC_ID_H264) 
	{
		static int is_FD = 0;
//		if(st->codec->extradata_size != 0)
		if(first_start_code == 1 && st->codec->extradata_size != 0)
		{
			first_start_code = 0;
			unsigned char *p = st->codec->extradata;
			unsigned char *q = NULL;
        		unsigned int cnt = 0, nalsize = 0, hdr_size = 0;
			if(p[4] == 0xFD) is_FD = 1;

			// Decode sps from avcC
			cnt = *(p+5) & 0x1f; // Number of sps
			av_log(s, AV_LOG_ERROR, "sps cnt %d\n", cnt);        
        		p += 6;
        		for (i = 0; i < cnt; i++) 
			{
            			nalsize = AV_RB16(p) + 2;
				av_log(s, AV_LOG_ERROR, "sps[%d] nalsize %d\n", i, nalsize);
				num_reorder_frames = decode_264_sps_flv(p, nalsize);

				//Barry 2010-10-18
				st->codec->width = flv_width;
				st->codec->height = flv_height;

            			p += nalsize;
				hdr_size += nalsize + 2;		
        		}

			if(num_reorder_frames != 0) queue_frames = num_reorder_frames;
			av_log(s, AV_LOG_ERROR, "number of reorder_b_frame %d\n", num_reorder_frames);

			// Decode pps from avcC
        		cnt = *(p++); // Number of pps
			av_log(s, AV_LOG_ERROR, "pps cnt %d\n", cnt);           
        		for (i = 0; i < cnt; i++) 
			{
            			nalsize = AV_RB16(p) + 2;
				av_log(s, AV_LOG_ERROR, "pps[%d] nalsize %d\n", i, nalsize); 			
            			p += nalsize;
				hdr_size += nalsize + 2;		
			}

			av_log(s, AV_LOG_ERROR, "hdr_size %d\n", hdr_size);	

			if(is_FD)
			{
				ret= av_new_packet(pkt, size + hdr_size + 2);
				pkt->pos= url_ftell(s->pb);
				ret= get_buffer(s->pb, pkt->data + hdr_size + 2, size);
				ret += 2;
				pkt->data[hdr_size] = pkt->data[hdr_size+1] = 0;
			}
			else
			{
				ret= av_new_packet(pkt, size + hdr_size);
    				pkt->pos= url_ftell(s->pb);
				ret= get_buffer(s->pb, pkt->data + hdr_size, size);
			}

			// Fill SPS and PPS of hdr_size
			p = st->codec->extradata;
			q = pkt->data;
			// Decode sps from avcC
			cnt = *(p+5) & 0x1f; // Number of sps
			p += 6;
        		for (i = 0; i < cnt; i++) 
			{
				q[0] = q[1] = q[2] = 0;
				q[3] = 1;
            			nalsize = AV_RB16(p);
				p += 2;
				q += 4;		
				memcpy(q, p, nalsize);	
				p += nalsize;
				q += nalsize;
        		}

			// Decode pps from avcC
        		cnt = *(p++); // Number of pps
			for (i = 0; i < cnt; i++) 
			{
				q[0] = q[1] = q[2] = 0;
				q[3] = 1;
            			nalsize = AV_RB16(p);
				p += 2;
				q += 4;		
				memcpy(q, p, nalsize);	
				p += nalsize;
				q += nalsize;
			}

			ret += hdr_size;

			av_log(s, AV_LOG_ERROR, "ret %d\n", ret);	

			int k = 0;
			int len = 0;
			unsigned char *ps = pkt->data;

			if(is_FD)
			{
				k = hdr_size;
				len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
				ps[k] = ps[k+1] = ps[k+2] = 0;
				ps[k+3] = 0x1;
				k += (len + 4);

				while(k < ret)
				{
				       #if 0 //Polun 2011-11-21 fixed mantis 6413
					unsigned char *pss = av_malloc(ret-k);
					memcpy(pss, ps + k, ret - k);
					memcpy(ps + k + 2, pss, ret - k);
                                   #else
					unsigned char *pss;
					pss = av_malloc(ret-k);
					memcpy(pss, ps + k, ret - k);
					memcpy(ps + k + 2, pss, ret - k);
                                   if (ps[k-1] == 0x80)
                                   {
					       garbage_byte_cnt = 1;
						ps[k-1] = 0;
                                   }
                                   else
						garbage_byte_cnt = 0;
                                   #endif
					ps[k] = ps[k+1] = 0;
					len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3];
					ps[k+2] = 0;
					ps[k+3] = 0x01;
					ret += 2;
					k += (len + 4);
					//Barry 2010-10-20
					if (pss)
						av_free(pss);
				}
			}
			else
			{
				for( k = hdr_size ; k < ret ; k += 4)
				{
					len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
					ps[k] = ps[k+1] = ps[k+2] = 0;
					ps[k+3] = 0x1;
					k += len;
				}
			}

//			st->codec->extradata_size = 0;
		}
		else
		{
			if(is_FD)
			{
				ret= av_new_packet(pkt, size + 2);
				pkt->pos= url_ftell(s->pb);
				ret= get_buffer(s->pb, pkt->data + 2, size);
				ret += 2;
				pkt->data[0] = pkt->data[1] = 0;
			}
			else
				
			ret= av_get_packet(s->pb, pkt, size);	

			//Fuchun 2010.01.29
			int k = 0;
			int len = 0;
			unsigned char *ps = pkt->data;

			if(is_FD)
			{
				len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
				ps[k] = ps[k+1] = ps[k+2] = 0;
				ps[k+3] = 0x1;
				k += (len + 4);

				while(k < ret)
				{
#if 1	//Barry 2010-11-22
					unsigned char *pss;
                                   #if 0 //Polun 2011-11-21 fixed mantis 6413
					if (garbage_byte_cnt >= 10)
					{
						pss = av_malloc(ret-k);
						memcpy(pss, ps + k, ret - k);
						memcpy(ps + k + 2, pss, ret - k);
						if (ps[k-1] == 0x80)
							ps[k-1] = 0;
					}
					else
					{
						pss = av_malloc(ret-k);
						memcpy(pss, ps + k, ret - k);
						memcpy(ps + k + 2, pss, ret - k);
						//av_log(s, AV_LOG_ERROR, "k-1: ps[%d] = %.2X, 00 00 00 01 ps[%d] = %.2X\n", k-1, ps[k-1], k-len, ps[k-len]);
						if (ps[k-1] == 0x80)
							garbage_byte_cnt++;
						else
							garbage_byte_cnt = 0;
					}
                                   #else
					if ((flv_width * flv_height) <= (320*240) && garbage_byte_cnt)
					{
						pss = av_malloc(ret-k);
						memcpy(pss, ps + k, ret - k);
						memcpy(ps + k + 2, pss, ret - k);
						if (ps[k-1] == 0x80)
							ps[k-1] = 0;
                                   }
                                   else
                                   {
						pss = av_malloc(ret-k);
						memcpy(pss, ps + k, ret - k);
						memcpy(ps + k + 2, pss, ret - k);
                                   }
                                   #endif
					ps[k] = ps[k+1] = 0;
					len = ((int)ps[k+2] << 8) + ps[k+3];
					ps[k+2] = 0;
					ps[k+3] = 0x01;
					ret += 2;
					k += (len + 4);
					//Barry 2010-10-20
					if (pss)
						av_free(pss);
#else
					unsigned char *pss = av_malloc(ret-k);
					memcpy(pss, ps + k, ret - k);
					memcpy(ps + k + 2, pss, ret - k);
					ps[k] = ps[k+1] = 0;
					len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3];
					ps[k+2] = 0;
					ps[k+3] = 0x01;
					ret += 2;
					k += (len + 4);
					//Barry 2010-10-20
					if (pss)
						av_free(pss);
#endif
				}
			}
			else
			{
				for( k = 0 ; k < ret ; k += 4)
				{
					len = ((int)ps[k] << 24) + ((int)ps[k+1] << 16) + ((int)ps[k+2] << 8) + ps[k+3] ;
					ps[k] = ps[k+1] = ps[k+2] = 0;
					ps[k+3] = 0x1;
					k += len;
				}
			}
			
//			pkt->data[0] = pkt->data[1] = pkt->data[2] = 0;
//			pkt->data[3] = 1;
		}

	}
	else
#endif

    ret= av_get_packet(s->pb, pkt, size);
    if (ret < 0) {
        return AVERROR(EIO);
    }
    /* note: we need to modify the packet size here to handle the last
       packet */
    pkt->size = ret;
    pkt->dts = dts;
    pkt->pts = pts == AV_NOPTS_VALUE ? dts : pts;
    pkt->stream_index = st->index;

    if (is_audio || ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY))
        pkt->flags |= PKT_FLAG_KEY;

    return ret;
}

AVInputFormat flv_demuxer = {
    "flv",
    NULL_IF_CONFIG_SMALL("FLV format"),
    sizeof(FLVContext),
    flv_probe,
    flv_read_header,
    flv_read_packet,
    .extensions = "flv",
    .value = CODEC_ID_FLV1,
};
