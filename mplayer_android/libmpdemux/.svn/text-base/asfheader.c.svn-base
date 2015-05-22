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

// .asf fileformat docs from http://divx.euro.ru

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "libavutil/common.h"
#include "libavutil/intreadwrite.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "aviprint.h"
#include "demuxer.h"
#include "stheader.h"

#include "asf.h"
#include "asfguid.h"
#include "asfheader.h"

#if 1	// Raymond 2009/05/31
//SkyMedi_Ethan 090518
unsigned long long avgFrTime=0;
unsigned int fourCC=0;
unsigned long long playPreRoll=0;
unsigned char* asfHeader = NULL;
unsigned int asfHeaderSize;
unsigned int GotASFHeader = 0;	// Raymond 2010/05/26
extern double correct_sub_pts;
unsigned long long playDuration = 0;
unsigned int asf_exter_data = 0;//Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC.
#endif
/* Add on 2011-09-22 for checking upnp stream by carlostai */
extern int upnp;

typedef struct {
  // must be 0 for metadata record, might be non-zero for metadata lib record
  uint16_t lang_list_index;
  uint16_t stream_num;
  uint16_t name_length;
  uint16_t data_type;
  uint32_t data_length;
  uint16_t* name;
  void* data;
} ASF_meta_record_t;

static char* get_ucs2str(const uint16_t* inbuf, uint16_t inlen)
{
  char* outbuf = calloc(inlen, 2);
  char* q;
  int i;

  if (!outbuf) {
    mp_msg(MSGT_HEADER, MSGL_ERR, MSGTR_MemAllocFailed);
    return NULL;
  }
  q = outbuf;
  for (i = 0; i < inlen / 2; i++) {
    uint8_t tmp;
    PUT_UTF8(AV_RL16(&inbuf[i]), tmp, *q++ = tmp;)
  }
  return outbuf;
}

static const char* asf_chunk_type(unsigned char* guid) {
  static char tmp[60];
  char *p;
  int i;

  switch(ASF_LOAD_GUID_PREFIX(guid)){
    case ASF_GUID_PREFIX_audio_stream:
      return "guid_audio_stream";
    case ASF_GUID_PREFIX_ext_audio_stream:
      return "guid_ext_audio_stream";
    case ASF_GUID_PREFIX_ext_stream_embed_stream_header:
      return "guid_ext_stream_embed_stream_header";
    case ASF_GUID_PREFIX_video_stream:
      return "guid_video_stream";
    case ASF_GUID_PREFIX_audio_conceal_none:
      return "guid_audio_conceal_none";
    case ASF_GUID_PREFIX_audio_conceal_interleave:
      return "guid_audio_conceal_interleave";
    case ASF_GUID_PREFIX_header:
      return "guid_header";
    case ASF_GUID_PREFIX_data_chunk:
      return "guid_data_chunk";
    case ASF_GUID_PREFIX_index_chunk:
      return "guid_index_chunk";
    case ASF_GUID_PREFIX_stream_header:
      return "guid_stream_header";
    case ASF_GUID_PREFIX_header_2_0:
      return "guid_header_2_0";
    case ASF_GUID_PREFIX_file_header:
      return "guid_file_header";
    case ASF_GUID_PREFIX_content_desc:
      return "guid_content_desc";
    case ASF_GUID_PREFIX_dvr_ms_timing_rep_data:
      return "guid_dvr_ms_timing_rep_data";
    case ASF_GUID_PREFIX_dvr_ms_vid_frame_rep_data:
      return "guid_dvr_ms_vid_frame_rep_data";
    default:
      strcpy(tmp, "unknown guid ");
      p = tmp + strlen(tmp);
      for (i = 0; i < 16; i++) {
	if ((1 << i) & ((1<<4) | (1<<6) | (1<<8))) *p++ = '-';
	sprintf(p, "%02x", guid[i]);
	p += 2;
      }
      return tmp;
  }
}

char *fAsfhdr_data = 0;
int fAsfhdr_size = 0;
int fAsfhdr_packetsize = 0;

int asf_check_header(demuxer_t *demuxer){
  unsigned char asfhdrguid[16]={0x30,0x26,0xB2,0x75,0x8E,0x66,0xCF,0x11,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C};
  struct asf_priv* asf = calloc(1,sizeof(*asf));
  asf->scrambling_h=asf->scrambling_w=asf->scrambling_b=1;
//printf("=== enter asf_check_header ...\n");
  if (demuxer->stream->type == STREAMTYPE_DS)
  {
//printf("demuxer->desc->type=%d\n", demuxer->desc->type);
//printf("demuxer->stream->type=%d\n", demuxer->stream->type);
    stream_t *ss = demuxer->stream;
    //printf("0000 demuxer->stream buf_len=%d buf_pos=%d ss->pos=%d\n", ss->buf_len, ss->buf_pos, ss->pos);
    memcpy(&ss->buffer[ss->buf_pos], fAsfhdr_data, fAsfhdr_size);
    ss->buf_len += fAsfhdr_size;
ss->pos = fAsfhdr_size;//+s->buf_pos-s->buf_len

//printf("==  demuxer->movi_start=%d buf_len=%d buf_pos=%d pos=%d\n", (int)demuxer->movi_start, (int)ss->buf_len, (int)ss->buf_pos, (int)ss->pos);

  }
  stream_read(demuxer->stream,(char*) &asf->header,sizeof(asf->header)); // header obj
  le2me_ASF_header_t(&asf->header);			// swap to machine endian
  int i;
  //for(i=0;i<16;i++) printf(" %02X",temp[i]);printf("\n");
  //for(i=0;i<16;i++) printf(" %02X",asfhdrguid[i]);printf("\n");
  //for(i=0;i<16;i++) printf(" %02X",asf->header.objh.guid[i]);printf("\n");
  if(memcmp(asfhdrguid,asf->header.objh.guid,16)){
    mp_msg(MSGT_HEADER,MSGL_V,"ASF_check: not ASF guid!\n");
    free(asf);
    return 0; // not ASF guid
  }
  if(asf->header.cno>256){
    mp_msg(MSGT_HEADER,MSGL_V,"ASF_check: invalid subchunks_no %d\n",(int) asf->header.cno);
    free(asf);
    return 0; // invalid header???
  }
  demuxer->priv = asf;
//printf("@@@@@@@ in asfheader,  demuxer=%p demuxer->priv=asf=%p\n", demuxer, demuxer->priv);
  return DEMUXER_TYPE_ASF;
}

static int get_ext_stream_properties(char *buf, int buf_len, int stream_num, struct asf_priv* asf, int is_video)
{
  int pos=0;
  uint8_t *buffer = &buf[0];
  uint64_t avg_ft;
  unsigned bitrate;

  while ((pos = find_asf_guid(buf, asf_ext_stream_header, pos, buf_len)) >= 0) {
    int this_stream_num, stnamect, payct, i;
    int buf_max_index=pos+50;
    if (buf_max_index > buf_len) return 0;
    buffer = &buf[pos];

    // the following info is available
    // some of it may be useful but we're skipping it for now
    // starttime(8 bytes), endtime(8),
    // leak-datarate(4), bucket-datasize(4), init-bucket-fullness(4),
    // alt-leak-datarate(4), alt-bucket-datasize(4), alt-init-bucket-fullness(4),
    // max-object-size(4),
    // flags(4) (reliable,seekable,no_cleanpoints?,resend-live-cleanpoints, rest of bits reserved)

    buffer += 8+8;
    bitrate = AV_RL32(buffer);
    buffer += 8*4;
    this_stream_num=AV_RL16(buffer);buffer+=2;

    if (this_stream_num == stream_num) {
      buf_max_index+=14;
      if (buf_max_index > buf_len) return 0;
      buffer+=2; //skip stream-language-id-index
      avg_ft = AV_RL64(buffer); // provided in 100ns units
      avgFrTime = avg_ft;  //ethan 090521
      buffer+=8;
      asf->bps = bitrate / 8;

      // after this are values for stream-name-count and
      // payload-extension-system-count
      // followed by associated info for each
      stnamect = AV_RL16(buffer);buffer+=2;
      payct = AV_RL16(buffer);buffer+=2;

      // need to read stream names if present in order
      // to get lengths - values are ignored for now
      for (i=0; i<stnamect; i++) {
        int stream_name_len;
        buf_max_index+=4;
        if (buf_max_index > buf_len) return 0;
        buffer+=2; //language_id_index
        stream_name_len = AV_RL16(buffer);buffer+=2;
        buffer+=stream_name_len; //stream_name
        buf_max_index+=stream_name_len;
        if (buf_max_index > buf_len) return 0;
      }

      if (is_video) {
        asf->vid_repdata_count = payct;
        asf->vid_repdata_sizes = malloc(payct*sizeof(int));
      } else {
        asf->aud_repdata_count = payct;
        asf->aud_repdata_sizes = malloc(payct*sizeof(int));
      }

      for (i=0; i<payct; i++) {
        int payload_len;
        buf_max_index+=22;
        if (buf_max_index > buf_len) return 0;
        // Each payload extension definition starts with a GUID.
        // In dvr-ms files one of these indicates the presence an
        // extension that contains pts values and this is always present
        // in the video and audio streams.
        // Another GUID indicates the presence of an extension
        // that contains useful video frame demuxing information.
        // Note that the extension data in each packet does not contain
        // these GUIDs and that this header section defines the order the data
        // will appear in.
        if (memcmp(buffer, asf_dvr_ms_timing_rep_data, 16) == 0) {
          if (is_video)
            asf->vid_ext_timing_index = i;
          else
            asf->aud_ext_timing_index = i;
        } else if (is_video && memcmp(buffer, asf_dvr_ms_vid_frame_rep_data, 16) == 0)
          asf->vid_ext_frame_index = i;
        buffer+=16;

        payload_len = AV_RL16(buffer);buffer+=2;

        if (is_video)
          asf->vid_repdata_sizes[i] = payload_len;
        else
          asf->aud_repdata_sizes[i] = payload_len;
        buffer+=4;//sys_len
      }

      return 1;
    }
  }
  return 1;
}

#define CHECKDEC(l, n) if (((l) -= (n)) < 0) return 0
static char* read_meta_record(ASF_meta_record_t* dest, char* buf,
    int* buf_len)
{
  CHECKDEC(*buf_len, 2 + 2 + 2 + 2 + 4);
  dest->lang_list_index = AV_RL16(buf);
  dest->stream_num = AV_RL16(&buf[2]);
  dest->name_length = AV_RL16(&buf[4]);
  dest->data_type = AV_RL16(&buf[6]);
  dest->data_length = AV_RL32(&buf[8]);
  buf += 2 + 2 + 2 + 2 + 4;
  CHECKDEC(*buf_len, dest->name_length);
  dest->name = (uint16_t*)buf;
  buf += dest->name_length;
  CHECKDEC(*buf_len, dest->data_length);
  dest->data = buf;
  buf += dest->data_length;
  return buf;
}

static int get_meta(char *buf, int buf_len, int this_stream_num,
    float* asp_ratio)
{
  int pos = 0;
  uint16_t records_count;
  uint16_t x = 0, y = 0;

  if ((pos = find_asf_guid(buf, asf_metadata_header, pos, buf_len)) < 0)
    return 0;

  CHECKDEC(buf_len, pos);
  buf += pos;
  CHECKDEC(buf_len, 2);
  records_count = AV_RL16(buf);
  buf += 2;

  while (records_count--) {
    ASF_meta_record_t record_entry;
    char* name;

    if (!(buf = read_meta_record(&record_entry, buf, &buf_len)))
        return 0;
    /* reserved, must be zero */
    if (record_entry.lang_list_index)
      continue;
    /* match stream number: 0 to match all */
    if (record_entry.stream_num && record_entry.stream_num != this_stream_num)
      continue;
    if (!(name = get_ucs2str(record_entry.name, record_entry.name_length))) {
      mp_msg(MSGT_HEADER, MSGL_ERR, MSGTR_MemAllocFailed);
      continue;
    }
    if (strcmp(name, "AspectRatioX") == 0)
      x = AV_RL16(record_entry.data);
    else if (strcmp(name, "AspectRatioY") == 0)
      y = AV_RL16(record_entry.data);
    free(name);
  }
  if (x && y) {
    *asp_ratio = (float)x / (float)y;
    return 1;
  }
  return 0;
}

static int is_drm(char* buf, int buf_len)
{
  uint32_t data_len, type_len, key_len, url_len;
  int pos = find_asf_guid(buf, asf_content_encryption, 0, buf_len);

  if (pos < 0)
    return 0;

  CHECKDEC(buf_len, pos + 4);
  buf += pos;
  data_len = AV_RL32(buf);
  buf += 4;
  CHECKDEC(buf_len, data_len);
  buf += data_len;
  type_len = AV_RL32(buf);
  if (type_len < 4)
    return 0;
  CHECKDEC(buf_len, 4 + type_len + 4);
  buf += 4;

  if (buf[0] != 'D' || buf[1] != 'R' || buf[2] != 'M' || buf[3] != '\0')
    return 0;

  buf += type_len;
  key_len = AV_RL32(buf);
  CHECKDEC(buf_len, key_len + 4);
  buf += 4;

  buf[key_len - 1] = '\0';
  mp_msg(MSGT_HEADER, MSGL_V, "DRM Key ID: %s\n", buf);

  buf += key_len;
  url_len = AV_RL32(buf);
  CHECKDEC(buf_len, url_len);
  buf += 4;

  buf[url_len - 1] = '\0';
  mp_msg(MSGT_HEADER, MSGL_INFO, MSGTR_MPDEMUX_ASFHDR_DRMLicenseURL, buf);
  return 1;
}

#ifdef CONFIG_DRM_ENABLE
// WT, 100624, drm open stream APIs
static int DxDrmOpenStream(demuxer_t *demux, char *data, int size)
{
  EDxDrmStatus result = DX_FALSE;

  demux->drmFileOpened = DX_FALSE;

  printf("DX: Opening drm stream ( DxDrmOpenStream)\n");
  
  if(demux->clientInitialized != DX_TRUE)
  {
    printf("DX: ERROR - Discretix DRM client not initialized\n");
    goto error;
  }

  result = DxDrmClient_OpenDrmStreamFromData(&(demux->drmStream),data ,size );	
  if(result != DX_SUCCESS)
  {
	printf("DX: ERROR (%d) - error while opening stream\n", result);
    goto error;
  }
  else
  {
  	printf("DX: DRM stream opened successfully\n");
  }

  //GST_INFO_OBJECT (demux, "DX: stream opened successfully");
  
  /* Hamer, 20100802  start */
  /* check the rights for the file */
  /* It would check if there is the valid right for playback */
  DxUint32 activeFlag = 0;
  result = DxDrmStream_GetFlags(demux->drmStream, DX_FLAG_HAS_VALID_RIGHTS|DX_FLAG_CAN_PLAY,
                &activeFlag, DX_PERMISSION_TO_PLAY, DX_ACTIVE_CONTENT);
  if (result != DX_SUCCESS)
  {
    printf("DX: ERROR(0x%08X) - Fail to get flags to check the rights\n", result);
    goto error;
  }
  if ((activeFlag&DX_FLAG_HAS_VALID_RIGHTS) && (activeFlag&DX_FLAG_CAN_PLAY))
    printf("DX: Have valid rights to play\n");
  else
  {
    printf("DX: DONOT have playback rights - flags (0x%08X)\n", activeFlag);
    goto error;
  }
  /* Hamer, 20100802  end */
  
  /*setting use intent for the drm file*/
  /*this operation checks if we can use the file according to the intent specified*/
  /*It will check for valid license*/  
  result = DxDrmStream_SetIntent(demux->drmStream, DX_INTENT_AUTO_PLAY, DX_AUTO_NO_UI);

  if(result != DX_SUCCESS)
  {
   	printf("DX: ERROR(%d) - opening stream failed because there are no rights (license) to play the content ", result);
    	goto error;
  }
  else
  {
  	printf("DX: playback rights found\n");
  }


  /*starting consumption of the file - notifying the drm that the file is being used*/
  result = DxDrmFile_HandleConsumptionEvent(demux->drmStream, DX_EVENT_START);
  if(result != DX_SUCCESS)
  {
    printf("DX:ERROR(%d): Content consumption failed", result);
    goto error;
  }
  printf("DX: Stream was opened and is ready for playback\n");
  demux->drmFileOpened = DX_TRUE;
  return 0;
	
error:
  demux->drmFileOpened = DX_FALSE;
  DxDrmStream_Close(&(demux->drmStream));
  return -1;

}
#endif


static int asf_init_audio_stream(demuxer_t *demuxer,struct asf_priv* asf, sh_audio_t* sh_audio, ASF_stream_header_t *streamh, int *ppos, uint8_t** buf, char *hdr, unsigned int hdr_len)
{
  uint8_t *buffer = *buf;
  int pos = *ppos;

  sh_audio->wf=calloc((streamh->type_size<sizeof(WAVEFORMATEX))?sizeof(WAVEFORMATEX):streamh->type_size,1);
  memcpy(sh_audio->wf,buffer,streamh->type_size);
  le2me_WAVEFORMATEX(sh_audio->wf);
  if( mp_msg_test(MSGT_HEADER,MSGL_V) ) print_wave_header(sh_audio->wf,MSGL_V);
  if(ASF_LOAD_GUID_PREFIX(streamh->concealment)==ASF_GUID_PREFIX_audio_conceal_interleave){
    buffer = &hdr[pos];
    pos += streamh->stream_size;
    if (pos > hdr_len) return 0;
    asf->scrambling_h=buffer[0];
    asf->scrambling_w=(buffer[2]<<8)|buffer[1];
    asf->scrambling_b=(buffer[4]<<8)|buffer[3];
    if(asf->scrambling_b>0){
      asf->scrambling_w/=asf->scrambling_b;
    }
  } else {
    asf->scrambling_b=asf->scrambling_h=asf->scrambling_w=1;
  }
  mp_msg(MSGT_HEADER,MSGL_V,"ASF: audio scrambling: %d x %d x %d\n",asf->scrambling_h,asf->scrambling_w,asf->scrambling_b);
  return 1;
}

static int find_backwards_asf_guid(char *buf, const char *guid, int cur_pos)
{
  int i;
  for (i=cur_pos-16; i>0; i--) {
    if (memcmp(&buf[i], guid, 16) == 0)
      return i + 16 + 8; // point after guid + length
  }
  return -1;
}

int read_asf_header(demuxer_t *demuxer,struct asf_priv* asf){
  int hdr_len = asf->header.objh.size - sizeof(asf->header);
  int hdr_skip = 0;
  char *hdr = NULL;
  char guid_buffer[16];
  int pos, start = stream_tell(demuxer->stream);
  uint32_t* streams = NULL;
  int audio_streams=0;
  int video_streams=0;
  uint16_t stream_count=0;
  int best_video = -1;
  int best_audio = -1;
  uint64_t data_len;
  ASF_stream_header_t *streamh;
  uint8_t *buffer;
  int audio_pos=0;
  //ethan----------------------
  unsigned int frameWidth, frameHeight;
  unsigned char *wmv3StrC = NULL;
  uint32_t max_packet_num = 0;

  if(hdr_len < 0) {
    mp_msg(MSGT_HEADER, MSGL_FATAL, "Header size is too small.\n");
    return 0;
  }

  if (hdr_len > 1024 * 1024) {
    mp_msg(MSGT_HEADER, MSGL_ERR, MSGTR_MPDEMUX_ASFHDR_HeaderSizeOver1MB,
			hdr_len);
    hdr_skip = hdr_len - 1024 * 1024;
    hdr_len = 1024 * 1024;
  }
  hdr = malloc(hdr_len);
  if (!hdr) {
    mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_HeaderMallocFailed,
            hdr_len);
    return 0;
  }
	// WT, 100701, seek to asf header to open DRM stream
#ifdef CONFIG_DRM_ENABLE
	char *drmhdr = NULL;			
	drmhdr = malloc(hdr_len + start);
	stream_seek(demuxer->stream, 0);				
	stream_read(demuxer->stream, drmhdr, hdr_len + start);	
	int by;
	printf("first guid is ");
	for (by = 0; by < 16; by ++)
	{
		printf(" 0x%x ", drmhdr[by]);
	}
	printf("\n");
	stream_seek(demuxer->stream, start);				
	stream_read(demuxer->stream, hdr, hdr_len);	
	printf("second guid is ");
	for (by = 0; by < 16; by ++)
	{
		printf(" 0x%x ", hdr[by]);
	}
	printf("\n");

	if (hdr_skip)
		stream_skip(demuxer->stream, hdr_skip);
	if (stream_eof(demuxer->stream)) 
	{
		mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_EOFWhileReadingHeader);
		goto err_out;
	}
	// open drm stream with header data 
	if (DxDrmOpenStream( demuxer,drmhdr,hdr_len + start) != DX_SUCCESS)
	{
		printf("DX: ERROR - DxDrmOpenStream failed\n");
	}
	else {
		mp_msg(MSGT_HEADER,MSGL_V,"ASF: DxDrmOpenStream OK, fileIsDrmProtected: %d\n", demuxer->fileIsDrmProtected);		
	}
        free(drmhdr);
	drmhdr = NULL;    
#else
  stream_read(demuxer->stream, hdr, hdr_len);
  if (hdr_skip)
    stream_skip(demuxer->stream, hdr_skip);
  if (stream_eof(demuxer->stream)) {
    mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_EOFWhileReadingHeader);
    goto err_out;
  }
#endif

  if (is_drm(hdr, hdr_len))
  {
    mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_DRMProtected);

	// WT, 100701, set DRM-protected
#ifdef CONFIG_DRM_ENABLE
	demuxer->fileIsDrmProtected = DX_TRUE;
	mp_msg(MSGT_HEADER,MSGL_V,"ASF: fileIsDrmProtected: %d\n", demuxer->fileIsDrmProtected);
    if (demuxer->drmFileOpened == DX_FALSE)
    {
        if (hdr) free(hdr);
        if (streams) free(streams);    
        return 1;
    }
#endif
  	}

  if ((pos = find_asf_guid(hdr, asf_ext_stream_audio, 0, hdr_len)) >= 0)
  {
    // Special case: found GUID for dvr-ms audio.
    // Now skip back to associated stream header.
    int sh_pos=0;

    sh_pos = find_backwards_asf_guid(hdr, asf_stream_header_guid, pos);

    if (sh_pos > 0) {
      sh_audio_t *sh_audio;

       mp_msg(MSGT_HEADER, MSGL_V, "read_asf_header found dvr-ms audio stream header pos=%d\n", sh_pos);
      // found audio stream header - following code reads header and
      // initializes audio stream.
      audio_pos = pos - 16 - 8;
      streamh = (ASF_stream_header_t *)&hdr[sh_pos];
      le2me_ASF_stream_header_t(streamh);
      audio_pos += 64; //16+16+4+4+4+16+4;
      buffer = &hdr[audio_pos];
      sh_audio=new_sh_audio(demuxer,streamh->stream_no & 0x7F);
      sh_audio->needs_parsing = 1;
      mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_AudioID, "asfheader", streamh->stream_no & 0x7F);
      ++audio_streams;
      if (!asf_init_audio_stream(demuxer, asf, sh_audio, streamh, &audio_pos, &buffer, hdr, hdr_len))
        goto len_err_out;
      if (!get_ext_stream_properties(hdr, hdr_len, streamh->stream_no, asf, 0))
        goto len_err_out;
    }
  }
  // find stream headers
  // only reset pos if we didnt find dvr_ms audio stream
  // if we did find it then we want to avoid reading its header twice
  if (audio_pos == 0)
    pos = 0;

  while ((pos = find_asf_guid(hdr, asf_stream_header_guid, pos, hdr_len)) >= 0)
  {
#ifdef CONFIG_DRM_ENABLE	//Barry 2011-12-01
    uint64_t guid_len;
    int guid_pos = pos - 16 - 8;
    if ((pos-8) >= 0)
    {
    	memcpy((char *)&guid_len, &hdr[pos-8], 8);
	guid_len = le2me_64(guid_len);
	//printf("########  [%s - %d]   guid_len = %d\n", __func__, __LINE__, guid_len);
    }
#endif
    streamh = (ASF_stream_header_t *)&hdr[pos];
    pos += sizeof(ASF_stream_header_t);
    if (pos > hdr_len) goto len_err_out;
    le2me_ASF_stream_header_t(streamh);
    mp_msg(MSGT_HEADER, MSGL_V, "stream type: %s\n",
            asf_chunk_type(streamh->type));
    mp_msg(MSGT_HEADER, MSGL_V, "stream concealment: %s\n",
            asf_chunk_type(streamh->concealment));
    mp_msg(MSGT_HEADER, MSGL_V, "type: %d bytes,  stream: %d bytes  ID: %d\n",
            (int)streamh->type_size, (int)streamh->stream_size,
            (int)streamh->stream_no);
    mp_msg(MSGT_HEADER, MSGL_V, "unk1: %lX  unk2: %X\n",
            (unsigned long)streamh->unk1, (unsigned int)streamh->unk2);
    mp_msg(MSGT_HEADER, MSGL_V, "FILEPOS=0x%X  pos=0x%x(%d) start=0x%x(%d)\n", pos + start, pos, pos, start, start);
    // type-specific data:
    buffer = &hdr[pos];
    pos += streamh->type_size;
    if (pos > hdr_len) goto len_err_out;
    switch(ASF_LOAD_GUID_PREFIX(streamh->type)){
      case ASF_GUID_PREFIX_audio_stream: {
        sh_audio_t* sh_audio=new_sh_audio(demuxer,streamh->stream_no & 0x7F);
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_AudioID, "asfheader", streamh->stream_no & 0x7F);
        ++audio_streams;
        if (!asf_init_audio_stream(demuxer, asf, sh_audio, streamh, &pos, &buffer, hdr, hdr_len))
          goto len_err_out;
	//if(demuxer->audio->id==-1) demuxer->audio->id=streamh.stream_no & 0x7F;
#ifdef CONFIG_DRM_ENABLE
        /* added on 2011.11.07 based on Discretix's gstreamer sample code 
         * It is for PlayReady/WMDRM encrypted asf file. 
         */
        if (0x5052 == sh_audio->wf->wFormatTag) {
                printf("Change PlayReady Audio\n");
                sh_audio->wf->wFormatTag = 0x0161;
                demuxer->fileIsDrmProtected = DX_TRUE;
        }
#endif /* CONFIG_DRM_ENABLE */    
        break;
        }
      case ASF_GUID_PREFIX_video_stream: {
        unsigned int len;
        float asp_ratio;
        sh_video_t* sh_video=new_sh_video(demuxer,streamh->stream_no & 0x7F);
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_VideoID, "asfheader", streamh->stream_no & 0x7F);
        len=streamh->type_size-(4+4+1+2);
	++video_streams;
//        sh_video->bih=malloc(chunksize); memset(sh_video->bih,0,chunksize);
        sh_video->bih=calloc((len<sizeof(BITMAPINFOHEADER))?sizeof(BITMAPINFOHEADER):len,1);
        memcpy(sh_video->bih,&buffer[4+4+1+2],len);
	le2me_BITMAPINFOHEADER(sh_video->bih);

#ifdef CONFIG_DRM_ENABLE
        /* added on 2011.11.07 based on Discretix's gstreamer sample code 
         * It is for PlayReady/WMDRM encrypted asf file. 
         */
        printf("original video fourCC = %p (%s)\n", sh_video->bih->biCompression, (char *)&sh_video->bih->biCompression);
        if (mmioFOURCC('P','R','D','Y') == sh_video->bih->biCompression)
	 {
	 	//Barry 2011-12-01
		if (guid_len && (guid_pos >= 0) && ((guid_pos+guid_len) < hdr_len))
		{
			if (hdr[guid_pos+guid_len-4] == 0x57 && hdr[guid_pos+guid_len-3] == 0x4D && hdr[guid_pos+guid_len-2] == 0x56 && hdr[guid_pos+guid_len-1] == 0x33)
			{
				sh_video->bih->biCompression = mmioFOURCC('W','M','V','3');
				demuxer->fileIsDrmProtected = DX_TRUE;
			}
			else if (hdr[guid_pos+guid_len-4] == 0x57 && hdr[guid_pos+guid_len-3] == 0x56 && hdr[guid_pos+guid_len-2] == 0x43 && hdr[guid_pos+guid_len-1] == 0x31)
			{
				sh_video->bih->biCompression = mmioFOURCC('W','V','C','1');
				demuxer->fileIsDrmProtected = DX_TRUE;
			}
		}
		printf("new video fourCC = %p (%s)\n", sh_video->bih->biCompression, (char *)&sh_video->bih->biCompression);
        }
#endif /* CONFIG_DRM_ENABLE */

#if 1
	//20110719 charleslin mantis 5443 - filter complex profile
	//if(len > sizeof(BITMAPINFOHEADER))
	if ( ((sh_video->bih->biCompression == mmioFOURCC('W','M','V','3')) || (sh_video->bih->biCompression == mmioFOURCC('W','V','C','1'))) &&
		(len > sizeof(BITMAPINFOHEADER))
	   )
	   {
		int i;
		unsigned char *ptr = (unsigned char *)sh_video->bih + sizeof(BITMAPINFOHEADER);
		extern int not_supported_profile_level;
#if 0
		printf("%s:%d biSize:%d len:%d > %d\n", __func__, __LINE__, sh_video->bih->biSize, len, sizeof(BITMAPINFOHEADER));
		printf("extra data:");
		for(i=0; i < len - sizeof(BITMAPINFOHEADER); i++){
			printf(" %02x", ptr[i]);
		}
		printf("\n");
#endif
		if((ptr[0] & 0xC0) == 0x80){
			/*
			 * for local file, media parser will block complex profile video,
			 * so we don't block complex profile here to support mantis 6130.
			 * mantis 5443 is 0x8c => unsupported
			 * mantis 6130 is 0x8e => supported
			 */
			//not_supported_profile_level = 1; // video format not support
			printf("%s:%d found complex profile:%02x, this stream may be unsupported!\n", __func__, __LINE__, ptr[0]);
		}
	   }
#endif

#if 1	//Fuchun 2011.04.18 move to demux_open_asf
    if(asf_exter_data == 0)//Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC.
    {
	// Raymond 2009/06/02
	frameWidth = sh_video->bih->biWidth;
	frameHeight = sh_video->bih->biHeight;
	fourCC = sh_video->bih->biCompression;

	// Raymond 2009/06/05
	if ( sh_video->bih->biSize > sizeof(BITMAPINFOHEADER) )
  	{  	
  		if(fourCC == mmioFOURCC('W','M','V','3'))
  		{
  			GotASFHeader = 2;	// Raymond 2010/05/26

#ifdef _SKY_VDEC_V2
			asfHeaderSize = 36;
			asfHeader = malloc(asfHeaderSize);
    			wmv3StrC = ((uint8_t*)sh_video->bih) + sizeof(BITMAPINFOHEADER);

			PUT32_L(0x00000004, (unsigned char*)(asfHeader+4));

			memcpy((unsigned char*)(asfHeader+8), wmv3StrC, 4);

			PUT32_L(frameHeight, (unsigned char*)(asfHeader+12));
			PUT32_L(frameWidth, (unsigned char*)(asfHeader+16));
			PUT32_L(0x0000000C, (unsigned char*)(asfHeader+20));
			PUT32_L(0x00000000, (unsigned char*)(asfHeader+24));
			PUT32_L(0x00000000, (unsigned char*)(asfHeader+28));
			PUT32_L(0x00000000, (unsigned char*)(asfHeader+32));
#endif
  		}
		else if(fourCC == mmioFOURCC('W','V','C','1'))
		{
			GotASFHeader = 2;	// Raymond 2010/05/26
			
			asfHeaderSize = sh_video->bih->biSize - sizeof(BITMAPINFOHEADER) -1;	
			asfHeader = ((uint8_t*)sh_video->bih) + sizeof(BITMAPINFOHEADER) + 1;		
		}
		else if(fourCC == mmioFOURCC('M','4','S','2') || fourCC == mmioFOURCC('M','P','4','S'))		//Fuchun 2010.04.22
		{
			sh_video->mpeg4_header_len = sh_video->bih->biSize - sizeof(BITMAPINFOHEADER);
			sh_video->mpeg4_header = sh_video->bih + 1;
		}
  	}
    }
    else
        printf("###ASF : mutile video stream on file. \n");
#endif

	if (sh_video->bih->biSize > len && sh_video->bih->biSize > sizeof(BITMAPINFOHEADER))
		sh_video->bih->biSize = len;
        if (sh_video->bih->biCompression == mmioFOURCC('D', 'V', 'R', ' ')) {
          //mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_MPDEMUX_ASFHDR_DVRWantsLibavformat);
          //sh_video->fps=(float)sh_video->video.dwRate/(float)sh_video->video.dwScale;
          //sh_video->frametime=(float)sh_video->video.dwScale/(float)sh_video->video.dwRate;
          asf->asf_frame_state=-1;
          asf->asf_frame_start_found=0;
          asf->asf_is_dvr_ms=1;
          asf->dvr_last_vid_pts=0.0;
        } else asf->asf_is_dvr_ms=0;
        if (!get_ext_stream_properties(hdr, hdr_len, streamh->stream_no, asf, 1))
            goto len_err_out;
        if (get_meta(hdr, hdr_len, streamh->stream_no, &asp_ratio)) {
          sh_video->aspect = asp_ratio * sh_video->bih->biWidth /
            sh_video->bih->biHeight;
        }
        sh_video->i_bps = asf->bps;

        if( mp_msg_test(MSGT_DEMUX,MSGL_V) ) print_video_header(sh_video->bih, MSGL_V);
        //asf_video_id=streamh.stream_no & 0x7F;
	//if(demuxer->video->id==-1) demuxer->video->id=streamh.stream_no & 0x7F;
        break;
        }
      }
      // stream-specific data:
      // stream_read(demuxer->stream,(char*) buffer,streamh.stream_size);
  }

  // find file header
  pos = find_asf_guid(hdr, asf_file_header_guid, 0, hdr_len);
  if (pos >= 0) {
      ASF_file_header_t *fileh = (ASF_file_header_t *)&hdr[pos];

#if 1	// Raymond 2009/05/31	      
	//added by ethan 090521------------------

	playDuration = fileh->play_duration;
	playPreRoll = fileh->preroll;
	correct_sub_pts = playPreRoll/1000;

	printf("playDuration = %lld\n", playDuration);
    	printf("avgFrTime = %lld\n", avgFrTime);
	printf("playPreRoll = %lld\n",playPreRoll);
	playDuration -= playPreRoll*10000;
       if(avgFrTime != 0)
	    printf("frame count :%lld\n",( playDuration/avgFrTime));

#if 1	//Fuchun 2011.04.18 move to demux_open_asf
    if(asf_exter_data == 0) //Polun 2011-10-24 fixed StarTrekXICorpHQ2009_WM_2500k_Seagate2_5pt8-900secBuffer_Full_NoBurnIn.wmv can't AVSYNC.
    {
	if(fourCC == mmioFOURCC('W','M','V','3'))
	{
		if(demuxer->stream->type == STREAMTYPE_STREAM)		//Fuchun 2010.05.25
			*((int*)(asfHeader)) = 0xC5<<24 | 1;
		else
		*((int*)(asfHeader)) = 0xC5<<24 | (playDuration/avgFrTime + 1);		
	}	  

	//added by ethan 090521------------------

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
       asf_exter_data = 1 ;
    }
#endif
#endif

      pos += sizeof(ASF_file_header_t);
      if (pos > hdr_len) goto len_err_out;
      le2me_ASF_file_header_t(fileh);
      mp_msg(MSGT_HEADER, MSGL_V, "ASF: packets: %d  flags: %d  "
              "max_packet_size: %d  min_packet_size: %d  max_bitrate: %d  "
              "preroll: %d\n",
              (int)fileh->num_packets, (int)fileh->flags,
              (int)fileh->min_packet_size, (int)fileh->max_packet_size,
              (int)fileh->max_bitrate, (int)fileh->preroll);
      max_packet_num = (int)fileh->num_packets;	//Fuchun 2010.08.27
      asf->packetsize=fileh->max_packet_size;
fAsfhdr_packetsize = asf->packetsize;
      asf->packet=malloc(asf->packetsize); // !!!
      asf->packetrate=fileh->max_bitrate/8.0/(double)asf->packetsize;
      asf->movielength=(fileh->play_duration-10000*fileh->preroll)/10000000.0;
//printf("asf->movielength=%d fileh->play_duration=%d fileh->preroll=%d\n", (int)asf->movielength, (int)fileh->play_duration, (int)fileh->preroll);
  }

  // find content header
  pos = find_asf_guid(hdr, asf_content_desc_guid, 0, hdr_len);
  if (pos >= 0) {
        ASF_content_description_t *contenth = (ASF_content_description_t *)&hdr[pos];
        char *string=NULL;
        uint16_t* wstring = NULL;
        uint16_t len;
        pos += sizeof(ASF_content_description_t);
        if (pos > hdr_len) goto len_err_out;
	le2me_ASF_content_description_t(contenth);
	mp_msg(MSGT_HEADER,MSGL_V,"\n");
        // extract the title
        if((len = contenth->title_size) != 0) {
          wstring = (uint16_t*)&hdr[pos];
          pos += len;
          if (pos > hdr_len) goto len_err_out;
          if ((string = get_ucs2str(wstring, len))) {
            mp_msg(MSGT_HEADER,MSGL_V," Title: %s\n", string);
//            demux_info_add(demuxer, "title", string);
            free(string);
          }
        }
        // extract the author
        if((len = contenth->author_size) != 0) {
          wstring = (uint16_t*)&hdr[pos];
          pos += len;
          if (pos > hdr_len) goto len_err_out;
          if ((string = get_ucs2str(wstring, len))) {
            mp_msg(MSGT_HEADER,MSGL_V," Author: %s\n", string);
//            demux_info_add(demuxer, "author", string);
            free(string);
          }
        }
        // extract the copyright
        if((len = contenth->copyright_size) != 0) {
          wstring = (uint16_t*)&hdr[pos];
          pos += len;
          if (pos > hdr_len) goto len_err_out;
          if ((string = get_ucs2str(wstring, len))) {
            mp_msg(MSGT_HEADER,MSGL_V," Copyright: %s\n", string);
//            demux_info_add(demuxer, "copyright", string);
            free(string);
          }
        }
        // extract the comment
        if((len = contenth->comment_size) != 0) {
          wstring = (uint16_t*)&hdr[pos];
          pos += len;
          if (pos > hdr_len) goto len_err_out;
          if ((string = get_ucs2str(wstring, len))) {
            mp_msg(MSGT_HEADER,MSGL_V," Comment: %s\n", string);
//            demux_info_add(demuxer, "comments", string);
            free(string);
          }
        }
        // extract the rating
        if((len = contenth->rating_size) != 0) {
          wstring = (uint16_t*)&hdr[pos];
          pos += len;
          if (pos > hdr_len) goto len_err_out;
          if ((string = get_ucs2str(wstring, len))) {
            mp_msg(MSGT_HEADER,MSGL_V," Rating: %s\n", string);
            free(string);
          }
        }
	mp_msg(MSGT_HEADER,MSGL_V,"\n");
  }

  // find content header
  pos = find_asf_guid(hdr, asf_stream_group_guid, 0, hdr_len);
  if (pos >= 0) {
        int max_streams = (hdr_len - pos - 2) / 6;
        uint16_t stream_id, i;
        uint32_t max_bitrate;
        char *ptr = &hdr[pos];
        mp_msg(MSGT_HEADER,MSGL_V,"============ ASF Stream group == START ===\n");
        if(max_streams <= 0) goto len_err_out;
        stream_count = AV_RL16(ptr);
        ptr += sizeof(uint16_t);
        if(stream_count > max_streams) stream_count = max_streams;
        if(stream_count > 0)
              streams = malloc(2*stream_count*sizeof(uint32_t));
        mp_msg(MSGT_HEADER,MSGL_V," stream count=[0x%x][%u]\n", stream_count, stream_count );
        for( i=0 ; i<stream_count ; i++ ) {
          stream_id = AV_RL16(ptr);
          ptr += sizeof(uint16_t);
          memcpy(&max_bitrate, ptr, sizeof(uint32_t));// workaround unaligment bug on sparc
          max_bitrate = le2me_32(max_bitrate);
          ptr += sizeof(uint32_t);
          mp_msg(MSGT_HEADER,MSGL_V,"   stream id=[0x%x][%u]\n", stream_id, stream_id );
          mp_msg(MSGT_HEADER,MSGL_V,"   max bitrate=[0x%x][%u]\n", max_bitrate, max_bitrate );
          streams[2*i] = stream_id;
          streams[2*i+1] = max_bitrate;
        }
        mp_msg(MSGT_HEADER,MSGL_V,"============ ASF Stream group == END ===\n");
  }
  free(hdr);
  hdr = NULL;
  start = stream_tell(demuxer->stream); // start of first data chunk
  stream_read(demuxer->stream, guid_buffer, 16);
  if (memcmp(guid_buffer, asf_data_chunk_guid, 16) != 0) {
    mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_NoDataChunkAfterHeader);
    free(streams);
    streams = NULL;
    return 0;
  }

  // read length of chunk
  stream_read(demuxer->stream, (char *)&data_len, sizeof(data_len));
  data_len = le2me_64(data_len);
  demuxer->movi_start = stream_tell(demuxer->stream) + 26;
  demuxer->movi_end = start + data_len;
  mp_msg(MSGT_HEADER, MSGL_V, "Found movie at 0x%X - 0x%X\n",
          (int)demuxer->movi_start, (int)demuxer->movi_end);

if(streams) {
  // stream selection is done in the network code, it shouldn't be done here
  // as the servers often do not care about what we requested.
#if 0
  uint32_t vr = 0, ar = 0,i;
#ifdef CONFIG_NETWORK
  if( demuxer->stream->streaming_ctrl!=NULL ) {
	  if( demuxer->stream->streaming_ctrl->bandwidth!=0 && demuxer->stream->streaming_ctrl->data!=NULL ) {
		  best_audio = ((asf_http_streaming_ctrl_t*)demuxer->stream->streaming_ctrl->data)->audio_id;
		  best_video = ((asf_http_streaming_ctrl_t*)demuxer->stream->streaming_ctrl->data)->video_id;
	  }
  } else
#endif
  for(i = 0; i < stream_count; i++) {
    uint32_t id = streams[2*i];
    uint32_t rate = streams[2*i+1];
    if(demuxer->v_streams[id] && rate > vr) {
      vr = rate;
      best_video = id;
    } else if(demuxer->a_streams[id] && rate > ar) {
      ar = rate;
      best_audio = id;
    }
  }
#endif
  free(streams);
  streams = NULL;
}

mp_msg(MSGT_HEADER,MSGL_V,"ASF: %d audio and %d video streams found\n",audio_streams,video_streams);
if(!audio_streams) demuxer->audio->id=-2;  // nosound
else if(best_audio > 0 && demuxer->audio->id == -1) demuxer->audio->id=best_audio;
if(!video_streams){
    if(!audio_streams){
	mp_msg(MSGT_HEADER,MSGL_ERR,MSGTR_MPDEMUX_ASFHDR_AudioVideoHeaderNotFound);
	return 0;
    }
    demuxer->video->id=-2; // audio-only
} else if (best_video > 0 && demuxer->video->id == -1) demuxer->video->id = best_video;

#if 0
if( mp_msg_test(MSGT_HEADER,MSGL_V) ){
    printf("ASF duration: %d\n",(int)fileh.duration);
    printf("ASF start pts: %d\n",(int)fileh.start_timestamp);
    printf("ASF end pts: %d\n",(int)fileh.end_timestamp);
}
#endif

  //Fuchun 2010.08.27
//printf("!!!!! demuxer->stream->type=%d STREAMTYPE_DS=%d\n", demuxer->stream->type, STREAMTYPE_DS);
//#ifdef CONFIG_LIVE555
#if 1
  if (demuxer->stream->type == STREAMTYPE_DS)
  {
	off_t cur_pos = stream_tell(demuxer->stream);
//printf("!!! ==> cur_pos=%d demuxer->movi_end:%d\n", (int)cur_pos, (int)demuxer->movi_end);
//printf("!!! for LIVE555, fix me later %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
//	  stream_skip(demuxer->stream, 1024);
//	stream_seek(demuxer->stream, cur_pos);

          asf_found_index_flag = 0;
  }
  else 
#endif
  /* 2011-09-22 if upnp is enable, we also check simple index table , mantis : 5957, upnp -ss is not correct */
  if (demuxer->stream->type != STREAMTYPE_STREAM || upnp )
  {
	off_t cur_pos = stream_tell(demuxer->stream);
	off_t skip_size = demuxer->movi_end - cur_pos;
//printf("==> cur_pos=%d demuxer->movi_end:%d - cur_pos:%d = %d\n", (int)cur_pos, (int)demuxer->movi_end, (int)cur_pos, (int)skip_size);
	int guid_size = 16, i, j = 0;
	uint64_t object_size;
	uint32_t max_packet_cnt;
	uint32_t index_size;
	uint32_t packet_num;
	char *temp_guid = malloc(guid_size);
	char *index_data = NULL;
	while(!asf_found_index_flag)
	{
		stream_skip(demuxer->stream, skip_size);
//printf("asf_found_index_flag=%d skip_size=%d\n", asf_found_index_flag, (int)skip_size);
		if(stream_read(demuxer->stream, temp_guid, guid_size) != guid_size)	//suppose the file doesn't has simple index object
			break;

		stream_read(demuxer->stream, (char *)&object_size, 8);
		object_size = le2me_64(object_size);

		//Barry 2010-09-06
		if (object_size <= 0)
			break;

		if(memcmp(temp_guid, asf_simple_index_object_guid, 16) != 0)
		{
			skip_size = (object_size - 16 - 8);
			continue;
		}
		else		//find simple index object
		{
			asf_found_index_flag = 1;
			printf("=== FIND THE SIMPLE INDEX OBJECT FOR ASF!!!===\n");
			
			stream_skip(demuxer->stream, 24);
			stream_read(demuxer->stream, (char *)&max_packet_cnt, 4);
			max_packet_cnt = le2me_32(max_packet_cnt);
			
			stream_read(demuxer->stream, (char *)&index_size, 4);
			index_size = le2me_32(index_size);

			asf_index_packet_num = malloc(index_size*sizeof(int));
			memset(asf_index_packet_num, max_packet_num, index_size);
//			printf("=== skip_size %"PRIx64"	object_size %"PRId64"	max_packet_cnt %d	index_size %d\n"
//				, skip_size, object_size, max_packet_cnt, index_size);
			
			index_data = malloc(index_size*6);
			stream_read(demuxer->stream, index_data, (index_size*6));

			for(i = 0; i < index_size; i++)
			{
				memcpy((char *)&packet_num, (index_data+i*6), 4);
				packet_num = le2me_32(packet_num);

				if(i > 0)
				{
					if(packet_num == asf_index_packet_num[j-1])
						continue;
				}
				asf_index_packet_num[j++] = packet_num;
			}

			asf_index_num = j;
//			printf("=== asf_index_num %d\n", asf_index_num);
			free(index_data);
		}
	}
	if(asf_found_index_flag)
	{
		if(asf_index_num < 10)
		{
			asf_found_index_flag = 0;
			printf("@@@ asf_index_num = %d < 10, use old way!!! @@@\n", asf_index_num);
		}
	}
	free(temp_guid);
	stream_seek(demuxer->stream, cur_pos);
  }
//printf("read_asf_header done!\n");

return 1;

len_err_out:
  mp_msg(MSGT_HEADER, MSGL_FATAL, MSGTR_MPDEMUX_ASFHDR_InvalidLengthInASFHeader);
err_out:
  if (hdr) free(hdr);
  if (streams) free(streams);
  return 0;
}
