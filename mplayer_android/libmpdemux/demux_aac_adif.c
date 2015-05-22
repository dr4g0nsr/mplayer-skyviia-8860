
// AAC (ADIF) demuxer

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

#include "ms_hdr.h"



#define READLEN 8192

static unsigned int sample_rates[] = {
  96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000
};
#define NUM_SAMPLE_RATES 12

// Header program config element - see http://www.audiocoding.com/modules/wiki/?page=program_config_element&PHPSESSID=75da1dfd89b43de591eca2bd9904ca50
typedef struct {
  char front_element_is_cpei;
  char front_element_tag_selecti;
} pce_element_info;

typedef struct {
  char element_instance_tag;
  char object_type;
  unsigned char sampling_frequency_index;
  char num_front_channel_elements;
  char num_side_channel_elements;
  char num_back_channel_elements;
  char num_lfe_channel_elements;
  char num_assoc_data_elements;
  char num_valid_cc_elements;
  char mono_mixdown_present;
  char mono_mixdown_element_number;
  char stereo_mixdown_present;
  char stereo_mixdown_element_number;
  char matrix_mixdown_idx_present;
  char matrix_mixdown_idx;
  char pseudo_surround_enable;
  pce_element_info* front_elements;
  pce_element_info* side_elements;
  pce_element_info* back_elements;
  char* lfe_elt_tag_selecti;
  char* assoc_data_tag_selecti;
  pce_element_info* cc_elements;
  //  char byte_alignment;
  char comment_field_bytes;
  char* comment_field_data; 	
} aac_adif_prog_config_elt;

// Header contents - see http://www.audiocoding.com/modules/wiki/?page=ADIF&PHPSESSID=75da1dfd89b43de591eca2bd9904ca50
typedef struct {
  char copyright_id_present;
  char copyright_id[9];
  char original_copy;
  char home;
  char bitstream_type; // 0 - CBR; 1 - VBR
  unsigned int bitrate;
  char num_pces;
  unsigned int* buffer_fullness;
  aac_adif_prog_config_elt *pces;
} aac_adif_hdr_contents;


typedef struct {
  uint8_t *buf;
  uint64_t size;
  float time;
  float last_pts;
  int bitrate;
  aac_adif_hdr_contents* hdr;
  int srate;
  int hdrlen;
} aac_adif_priv_t;

static int bitno = 9;
static char input_byte;

static int demux_aac_adif_init(demuxer_t *demuxer)
{
  aac_adif_priv_t *priv;
	
  if (! (priv = calloc(1, sizeof(aac_adif_priv_t))))
    return 0;
  if (! (priv->buf = (uint8_t*) malloc(8)))
  {
    free(priv);
    return 0;
  }
  if (! (priv->hdr = calloc(1, sizeof(aac_adif_hdr_contents))))
  {
    free(priv->buf);
    free(priv);
    return 0;
  }

  demuxer->priv = priv;
  priv->size = 0;
  priv->time = 0;
  priv->last_pts = 0;
  priv->bitrate = 0;
  priv->srate = 0;

  demuxer->seekable = 0; // Input frames are variable length, so not possible to accurately find a position in the stream.

  return 1;
}

static void demux_close_aac_adif(demuxer_t *demuxer)
{
  aac_adif_priv_t *priv = (aac_adif_priv_t *) demuxer->priv;
	
  if(!priv)
    return;

  if(priv->buf)
    free(priv->buf);

  if (priv->hdr)
    free(priv->hdr);

  free(priv);
  demuxer->priv = 0;

  return;
}


static void resync_adif_bit_reader()
{
  bitno = 8;
}

static unsigned int get_adif_onebit(demuxer_t *demuxer)
{
  unsigned int res;

  if (bitno > 7)
  {
    input_byte = stream_read_char(demuxer->stream);
    bitno = 0;
  }
  res = (input_byte & 0x80) ? 1 : 0;
  input_byte <<= 1;
  bitno++;
  return res;
}

static unsigned int get_adif_bits(demuxer_t *demuxer, int numbits)
{
  unsigned int res;
  int i;

  res = 0;
  for (i = 0; i < numbits; i++)
  {
    res <<= 1;

    if (bitno > 7)
    {
      input_byte = stream_read_char(demuxer->stream);
      bitno = 0;
    }
    res |= (input_byte & 0x80) ? 1 : 0;
    input_byte <<= 1;
    bitno++;
  }
  return res;
}

static void read_adif_pce_element_info(demuxer_t *demuxer, pce_element_info *info)
{
  info->front_element_is_cpei = (char) get_adif_bits(demuxer, 1);
  info->front_element_tag_selecti = (char) get_adif_bits(demuxer, 4);
}

static void read_adif_pce(demuxer_t *demuxer, aac_adif_prog_config_elt *elt)
{
  int i;
  aac_adif_priv_t *priv = (aac_adif_priv_t *) demuxer->priv;

  elt->element_instance_tag = (char) get_adif_bits(demuxer, 4);
  elt->object_type = (char) get_adif_bits(demuxer, 2);
  elt->sampling_frequency_index = (char) get_adif_bits(demuxer, 4);

  if ((priv->srate == 0) && (elt->sampling_frequency_index >= 0) && (elt->sampling_frequency_index < NUM_SAMPLE_RATES))
    priv->srate = sample_rates[elt->sampling_frequency_index];

  elt->num_front_channel_elements = (char) get_adif_bits(demuxer, 4);
  elt->num_side_channel_elements = (char) get_adif_bits(demuxer, 4);
  elt->num_back_channel_elements = (char) get_adif_bits(demuxer, 4);
  elt->num_lfe_channel_elements = (char) get_adif_bits(demuxer, 2);
  elt->num_assoc_data_elements = (char) get_adif_bits(demuxer, 3);
  elt->num_valid_cc_elements = (char) get_adif_bits(demuxer, 4);
  elt->mono_mixdown_present = (char) get_adif_bits(demuxer, 1);
  if (elt->mono_mixdown_present)
      elt->mono_mixdown_element_number = (char) get_adif_bits(demuxer, 4);
  elt->stereo_mixdown_present = (char) get_adif_bits(demuxer, 1);
  if (elt->stereo_mixdown_present)
      elt->stereo_mixdown_element_number = (char) get_adif_bits(demuxer, 4);
  elt->matrix_mixdown_idx_present = (char) get_adif_bits(demuxer, 1);
  if (elt->matrix_mixdown_idx_present){
      elt->matrix_mixdown_idx = (char) get_adif_bits(demuxer, 2);
      elt->pseudo_surround_enable = (char) get_adif_bits(demuxer, 1);
  }


  if (elt->num_front_channel_elements)
  {
    elt->front_elements = (pce_element_info*) calloc(elt->num_front_channel_elements, sizeof(pce_element_info));
    for (i = 0; i < elt->num_front_channel_elements; i++)
      read_adif_pce_element_info(demuxer, &elt->front_elements[i]);
  }
  else 
    elt->front_elements = 0;

  if (elt->num_side_channel_elements)
  {
    elt->side_elements = (pce_element_info*) calloc(elt->num_side_channel_elements, sizeof(pce_element_info));
    for (i = 0; i < elt->num_side_channel_elements; i++)
      read_adif_pce_element_info(demuxer, &elt->side_elements[i]);
  }
  else 
    elt->side_elements = 0;

  if (elt->num_back_channel_elements)
  {
    elt->back_elements = (pce_element_info*) calloc(elt->num_back_channel_elements, sizeof(pce_element_info));
    for (i = 0; i < elt->num_back_channel_elements; i++)
      read_adif_pce_element_info(demuxer, &elt->back_elements[i]);
  }
  else 
    elt->back_elements = 0;

  if (elt->num_lfe_channel_elements)
  {
    elt->lfe_elt_tag_selecti = (char*) malloc(elt->num_lfe_channel_elements);
    for (i = 0; i < elt->num_lfe_channel_elements; i++)
      elt->lfe_elt_tag_selecti[i] = (char) get_adif_bits(demuxer, 4);
  }
  else
    elt->lfe_elt_tag_selecti = 0;

  if (elt->num_assoc_data_elements)
  {
    elt->assoc_data_tag_selecti = (char*) malloc(elt->num_assoc_data_elements);
    for (i = 0; i < elt->num_assoc_data_elements; i++)
      elt->assoc_data_tag_selecti[i] = (char) get_adif_bits(demuxer, 4);
  }
  else
    elt->assoc_data_tag_selecti = 0;

  if (elt->num_valid_cc_elements)
  {
    elt->cc_elements = (pce_element_info*) calloc(elt->num_valid_cc_elements, sizeof(pce_element_info));;
    for (i = 0; i < elt->num_back_channel_elements; i++)
      read_adif_pce_element_info(demuxer, &elt->cc_elements[i]);
  }
  else
    elt->cc_elements = 0;

  resync_adif_bit_reader();

  if (elt->comment_field_bytes = stream_read_char(demuxer->stream))
  {
    elt->comment_field_data = (char*) malloc(elt->comment_field_bytes);
    for (i = 0; i < elt->comment_field_bytes; i++)
      elt->comment_field_data[i] =  stream_read_char(demuxer->stream); 	
  }
  else
    elt->comment_field_data = 0;
}


// returns DEMUXER_TYPE_AAC_ADIF if this is an ADIF file, 0 otherwise.
static int demux_probe_aac_adif(demuxer_t *demuxer)
{
  char c;
  int i;
  int len, srate, num;
  off_t init, probed;
  aac_adif_priv_t *priv;
  aac_adif_hdr_contents* hdr;

  if(! demux_aac_adif_init(demuxer))
  {
    mp_msg(MSGT_DEMUX, MSGL_ERR, "Couldn't init ADIF demuxer, exit\n");
    return 0;
  }
	
  priv = (aac_adif_priv_t *) demuxer->priv;
  hdr = priv->hdr;

  stream_seek(demuxer->stream, 0);
  c = stream_read_char(demuxer->stream);
  if (c != 'A') goto fail;
  c = stream_read_char(demuxer->stream);
  if (c != 'D') goto fail;
  c = stream_read_char(demuxer->stream);
  if (c != 'I') goto fail;
  c = stream_read_char(demuxer->stream);
  if (c != 'F') goto fail;

  resync_adif_bit_reader();

  if (hdr->copyright_id_present = get_adif_onebit(demuxer))
  {
    for (i = 0; i < 9; i++)
      hdr->copyright_id[i] = get_adif_bits(demuxer,8);
  }

  hdr->original_copy = get_adif_onebit(demuxer);
  hdr->home = get_adif_onebit(demuxer);
  hdr->bitstream_type = get_adif_onebit(demuxer);
  hdr->bitrate =  get_adif_bits(demuxer,23);

  priv->bitrate = hdr->bitrate;

  if (hdr->num_pces = (get_adif_bits(demuxer,4) + 1))
  {
    if (hdr->bitstream_type == 0)
    {
      if (! (hdr->buffer_fullness = (unsigned int*) calloc(hdr->num_pces, 20)))
      {
	mp_msg(MSGT_DEMUX, MSGL_ERR, "Could\'t initialise the ADIF buffer_fullness memory, exit\n");
	return 0;
      }
    }
    else
      hdr->buffer_fullness = 0;

    if (! (hdr->pces = (aac_adif_prog_config_elt*) calloc(hdr->num_pces, sizeof(aac_adif_prog_config_elt))))
    {
      mp_msg(MSGT_DEMUX, MSGL_ERR, "Could\'t initialise the ADIF program config element memory, exit\n");
      return 0;
    }

    for (i = 0; i < hdr->num_pces; i++)
    {
      if ((hdr->bitstream_type == 0) && (hdr->buffer_fullness))
	hdr->buffer_fullness[i] = get_adif_bits(demuxer,20);

      if (hdr->pces)
	read_adif_pce(demuxer, &hdr->pces[i]);
    }
  }
  else
  {
    hdr->buffer_fullness = 0;
    hdr->pces = 0;
  }

  init = probed = stream_tell(demuxer->stream);

  priv->hdrlen = stream_tell(demuxer->stream);

  mp_msg(MSGT_DEMUX, MSGL_V, "demux_aac_adif_probe, INIT: %"PRIu64", PROBED: %"PRIu64"\n", init, probed);
  return DEMUXER_TYPE_AAC_ADIF;

fail:
  mp_msg(MSGT_DEMUX, MSGL_V, "demux_aac_adif_probe, failed to detect an AAC_ADIF stream\n");
  return 0;
}

static demuxer_t* demux_open_aac_adif(demuxer_t *demuxer)
{
  sh_audio_t *sh;

  sh = new_sh_audio(demuxer, 0);
  sh->ds = demuxer->audio;
  sh->format = mmioFOURCC('M', 'P', '4', 'A');
  demuxer->audio->id = 0;           //2010-07-16 : HONDA_ADIF add for new mplayer
  demuxer->audio->sh = sh;

  stream_seek(demuxer->stream, 0);
  demuxer->filepos = 0;

  return demuxer;
}

static int demux_aac_adif_fill_buffer(demuxer_t *demuxer, demux_stream_t *ds)
{
  aac_adif_priv_t *priv = (aac_adif_priv_t *) demuxer->priv;
  demux_packet_t *dp;
  int c1, c2, len, srate, num;
  float tm = 0;

  if(demuxer->stream->eof || (demuxer->movi_end && stream_tell(demuxer->stream) >= demuxer->movi_end))
    return 0;

  len = READLEN;

  dp = new_demux_packet(len);
  if(! dp)
  {
    fprintf(stderr,"fill_buffer,NEW ADIF PACKET(%d)FAILED\n", len);
    mp_msg(MSGT_DEMUX, MSGL_ERR, "fill_buffer, NEW ADIF PACKET(%d)FAILED\n", len);
    return 0;
  }
					
  len = stream_read(demuxer->stream, dp->buffer, READLEN);

  if (len != READLEN)
    len = len;


  if(priv->bitrate)
  {
    tm = (len * 8);
    tm /= (priv->bitrate); // FIXME assumes CBR
  }

  priv->last_pts += tm;
  dp->len = len;
  dp->pts = priv->last_pts;
  //  fprintf(stderr, "\nPTS: %.3f\n", dp->pts);
  ds_add_packet(demuxer->audio, dp);
  priv->size += len;
  priv->time += tm;
	
  demuxer->filepos = stream_tell(demuxer->stream);
	
  return len;
}

static void demux_seek_aac_adif(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
  aac_adif_priv_t *priv = (aac_adif_priv_t *) demuxer->priv;
  demux_stream_t *d_audio=demuxer->audio;
  sh_audio_t *sh_audio=d_audio->sh;
  float time;
  unsigned int frame_len;

  //  frame_len = (1024 / priv->srate) * (priv->bitrate/8);
  //  frame_len = ((1024 * priv->bitrate * 4) / (8 * 44100));
  frame_len = ((1024.0 * 16.0) / (float) priv->srate) * ((float) priv->bitrate/8.0);

  ds_free_packs(d_audio);

  time = (flags & 1) ? rel_seek_secs :  priv->last_pts + rel_seek_secs;
  if(time < 0) 
  {
    stream_seek(demuxer->stream, demuxer->movi_start);
    time = priv->last_pts + time;
    priv->last_pts = 0;
  }
  if(time > 0)
  {
    float fltpos;
    int pos;
    fltpos = time * (priv->bitrate/8);
    pos = (int) fltpos;
    pos /= frame_len;
    pos *= frame_len;
    pos += priv->hdrlen;

    if (pos > demuxer->movi_end) pos = demuxer->movi_end;
    stream_seek(demuxer->stream, pos);
  }

  //  sh_audio->delay = priv->last_pts - (ds_tell_pts(demuxer->audio)-sh_audio->a_in_buffer_len)/(float)priv->bitrate; // FIXME - ????
}
 
demuxer_desc_t demuxer_desc_aac_adif = {
  "AAC-ADIF demuxer",
  "aac_adif",
  "AAC-ADIF",
  "Honda",
  "ADIF AAC files ",
  DEMUXER_TYPE_AAC_ADIF,
  0, // unsafe autodetect
  demux_probe_aac_adif,
  demux_aac_adif_fill_buffer,
  demux_open_aac_adif,
  demux_close_aac_adif,
  demux_seek_aac_adif,
  NULL
};





