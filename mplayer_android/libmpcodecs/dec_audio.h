/*
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

#ifndef MPLAYER_DEC_AUDIO_H
#define MPLAYER_DEC_AUDIO_H

#include "libmpdemux/stheader.h"

// dec_audio.c:
void afm_help(void);
int init_best_audio_codec(sh_audio_t *sh_audio, char** audio_codec_list, char** audio_fm_list);
int decode_audio(sh_audio_t *sh_audio, int minlen);
void resync_audio_stream(sh_audio_t *sh_audio);
void skip_audio_frame(sh_audio_t *sh_audio);
void uninit_audio(sh_audio_t *sh_audio);

int init_audio_filters(sh_audio_t *sh_audio, int in_samplerate,
                       int *out_samplerate, int *out_channels, int *out_format);

#ifdef PARSE_REAL_CHANNELS
int get_audio_channels(sh_audio_t *sh);	//Fuchun 2011.05.17
#define oggpack_read_dword(ptr)	( (int)( ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0] ) )
enum{ AAC_ADIF, AAC_ADTS };

typedef struct 
{
    int num_ele;
    int ele_is_cpe[16];
    int ele_tag[16];
} EleList;

typedef struct 
{
    int present;
    int ele_tag;
    int pseudo_enab;
} MIXdown;

typedef struct 
{
    int profile;
    int sampling_rate_idx;
    int nChannels;
//	char comments[(1<<8)+1];
    long buffer_fullness;	// put this transport level info here 
	EleList front;
    EleList side;
    EleList back;
    EleList lfe;
    EleList data;
    EleList coupling;
    MIXdown mono_mix;
    MIXdown stereo_mix;
    MIXdown matrix_mix;
    
} ProgConfig;

// audio data transport stream frame format header
typedef struct
{
	int	syncword;
	int	id;
	int	layer;
	int	protection_abs;
	int profile;
	int	sampling_freq_idx;
	int	private_bit;
	int	channel_config;
	int	original_copy;
	int	home;
	int	copyright_id_bit;
	int	copyright_id_start;
	int	frame_length;
	int	adts_buffer_fullness;
	int	num_of_rdb;
	int	crc_check;
} ADTSHeader;

// audio data interchange format header
typedef struct 
{
    char    adif_id[5];
    int	    copy_id_present;
    char    copy_id[10];
    int	    original_copy;
    int	    home;
    int	    bitstream_type;
    long    bitrate;		// bps
    int	    num_pce;
//    int	    prog_tags[16];	//(1<<4)

	ProgConfig prog_config;

} ADIFHeader;

typedef struct
{
	ADTSHeader adtsHeader;
	ADIFHeader adifHeader;
	
	int nAACFormat;
	int	nADIFHeaderLength;
	int nChannels;
	int nSamplingFreq;
	int nBitRate;			// kbps
	
} AAC_INFO;

typedef struct 
{
	unsigned char* pSource;
	int		nSourceCount;
//	int		nMaxNumSourceBytes;

	int		nbits;
	long	cword;
	
} BitData;
#endif

#endif /* MPLAYER_DEC_AUDIO_H */
