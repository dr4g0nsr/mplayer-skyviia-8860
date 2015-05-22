
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libmpdemux/demuxer.h"
#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"
#include "btype.h"
#include "c2ms.h"


typedef struct {
#ifdef _DEBUG
  int fd;
#else
  void * mgmt;
  void * sess;
#endif
  int method;
  FILE *log;
  char info[512];
} stream_priv_s;


static int fill_buffer(stream_t *s, char* buffer, int max_len){
  stream_priv_s *priv = s->priv;
  int ret;
  int prevratio=0,currratio=0;
readbuffer:
  	ret = c2ms_sess_read(priv->sess, buffer, max_len,1);
	if( ret < 0 ) return ret;
	if(( ret == 0) && ( s->pos == s->end_pos))
		return ret;
	if( ret == 0){
		goto readbuffer;
	}
	return ret;
}

static int seek_vosp(stream_t *s,off_t newpos) {
  stream_priv_s *priv = s->priv;
  s->pos = newpos;
  int pos = 0;
  pos = c2ms_sess_seek(priv->sess, newpos);
  sleep(1);
  if(pos<0) {
	  s->eof=1;
	  return 0;
  }
  return 1;
}

static void close_vosp(stream_t *s) {
  stream_priv_s *priv = s->priv;
  if (priv->sess) c2ms_sess_close(priv->sess);
  if (priv->mgmt) c2ms_mgmt_clean(priv->mgmt);
  sprintf(priv->info,"close_vosp\n");
  fwrite(priv->info,1,strlen(priv->info),priv->log);
  fflush(priv->log);
  free(priv);
}

static int open_vosp(stream_t *stream,int mode, void* opts, int* file_format) {
  int f;
  mode_t m = 0;
  off_t len;
  stream_priv_s *priv = malloc(sizeof(stream_priv_s));
  priv->method = 0;
  FILE *ini = fopen("./vosp.ini","r");
  if(ini)
  {
	  priv->method = fgetc(ini);
	  fclose(ini);
  }
  if(priv->method == '1')
  {
	  stream->type = STREAMTYPE_FILE;	  
//	  *file_format = DEMUXER_TYPE_VOTV;	  
	  return STREAM_OK;
  }

  char    *filename =  stream->url;
  
  priv->log = fopen("./log.txt","w");

  if(mode == STREAM_READ)
    m = O_RDONLY;
  else if(mode == STREAM_WRITE)
    m = O_RDWR|O_CREAT|O_TRUNC;
  else {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] Unknown open mode %d\n",mode);
    free(priv);
    return STREAM_UNSUPPORTED;
  }

  if(!filename) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] No filename\n");
    free(priv);
    return STREAM_ERROR;
  }
  int exitcode = 0;
  uint8      fid[33];
  uint8      mname[48];
  uint64     msize = 0;
  uint8      mmime = 0;
  uint64     seekpos = 0;
  priv->mgmt = c2ms_mgmt_init(NULL, 1000, NULL, 16*1024*1024);
  if(!priv->mgmt)
  {
	  printf("stream_vosp.c c2ms_mgmt_init failed\n");
	  free(priv);
	  return STREAM_ERROR;
  }
  
  priv->sess = c2ms_sess_open(priv->mgmt, filename, &exitcode);
  if(!priv->sess)
  {
	  if (priv->sess) c2ms_sess_close(priv->sess);
	  if (priv->mgmt) c2ms_mgmt_clean(priv->mgmt);
	  printf("stream_vosp.c c2ms_sess_open failed, exitcode: %d, %s\n", exitcode, c2ms_sess_error(exitcode));
	  free(priv);
	  return STREAM_ERROR;
  }
  
  memset(fid, 0, sizeof(fid));
  memset(mname, 0, sizeof(mname));
  c2ms_sess_attr(priv->sess, fid, mname, &msize, &mmime);
  printf("stream_vosp.c c2ms session open successfull sdscd   sd for %s\n", filename);
  printf("      fid: %s\n", fid);
  printf("    mname: %s\n", mname);
  printf("    msize: %llu\n", msize);
  printf("    mmime: %u\n", mmime);
  
  len = msize;
	c2ms_sess_cache_init(priv->sess, 16*1024);
	c2ms_sess_buffering(priv->sess, 3*1024*1024);
	c2ms_sess_set_media(priv->sess, 0, 0, msize);
//  c2ms_sess_range(priv->sess, 0, msize);
  
  printf("stream_vosp.c Now seek to offset %llu\n", seekpos);
  seekpos = 0;
  c2ms_sess_seek(priv->sess, seekpos);
  
  if(len >= 0) {
    stream->seek = seek_vosp;
    stream->end_pos = len;
    stream->type = STREAMTYPE_FILE;
  }

  mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %"PRId64" bytes\n", (int64_t)len);

  stream->priv = priv;
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = NULL;
  stream->control = NULL;
  stream->close = close_vosp;

  sprintf(priv->info,"open_vosp successfully\n");
  fwrite(priv->info,1,strlen(priv->info),priv->log);
  fflush(priv->log);

  *file_format = DEMUXER_TYPE_MOV;
  
  return STREAM_OK;
}

stream_info_t stream_info_vosp = {
  "VOSP",
  "vosp",
  "chenzhong",
  "",
  open_vosp,
  { "vosp", "", NULL },
  NULL,
  0 // Urls are an option string
};

