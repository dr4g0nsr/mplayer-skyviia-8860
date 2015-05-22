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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <sys/mman.h>

#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

#ifdef STREAM_DIO
int dio = 0;
#endif
#ifdef STREAM_AIO
int aio = 0;
int nbuf = 1;
#endif

#ifdef STREAM_USE_DEVMEM
int nodevmem = 0;
int memfd = -1;
void *mem_ptr = NULL;
//extern void *SallocVirtualBase;
#endif

extern int vol_blksz;

static struct stream_priv_s {
  char* filename;
  char *filename2;
} stream_priv_dflts = {
  NULL, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
  {"string", ST_OFF(filename), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  {"filename", ST_OFF(filename2), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  { NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
  "file",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};

#ifdef STREAM_DIO_AUTO
void dio_enable(stream_t *s)
{
	int flags;

	if(dio == 1){
		printf("%s: dio has already been enabled\n", __func__);
		return;
	}
 
	printf("%s: enable dio\n", __func__);
	flags = fcntl(s->fd, F_GETFL) | O_DIRECT;
    
#ifdef STREAM_NI
	if(s->afd > 0 && s->vfd > 0){
		if(fcntl(s->afd, F_SETFL, flags) == -1 ||
			fcntl(s->vfd, F_SETFL, flags) == -1)
			goto err;
	}else
#endif
	if(s->fd > 0){
		if(fcntl(s->fd, F_SETFL, flags) == -1)
			goto err;
	}
	else
	{
		goto err;
	}

	dio = 1;
	return;
	
err:
	printf("%s: failed to enable direct I/O\n", __func__);

}
#endif

#ifdef STREAM_AIO

#ifdef STREAM_USE_DEVMEM
extern unsigned int mmap_physAddr;
extern void *mmap_virtAddr;
extern unsigned int mmap_size;

#define DEVMEM_NBUF	(sizeof(devbs)/sizeof(devbs[0]))
#define DEVMEM_BSZ	(1024*256)
#define DEVMEM_SIZE	(DEVMEM_BSZ*DEVMEM_NBUF)
#define DEVMEM_PHYS	(mmap_physAddr + mmap_size - DEVMEM_SIZE)
#define DEVMEM_MMAP	(mmap_virtAddr + mmap_size - DEVMEM_SIZE)
#define DEVMEM_KERN	(0xC0000000 | DEVMEM_PHYS)

int devbs[] = { DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ, DEVMEM_BSZ };
int devnbuf = sizeof(devbs)/sizeof(devbs[0]);
#endif

//int bs[] = { 128*1024, 256*1024, 512*1024, 1024*1024 };
//int bs[] = { 1024*1024, 1024*1024 };
int membs[] = { 256*1024, 256*1024, 256*1024, 256*1024, 256*1024, 256*1024, 256*1024, 256*1024 };
int memnbuf = sizeof(membs)/sizeof(membs[0]);

#ifdef STREAM_AIO_AUTO
void aio_enable(stream_t *s)
{
	int i, ret;
	int *bs;
	unsigned int totalsize = 0;
	unsigned char *nextbuf;

	if(aio == 1){
		printf("aio has already been enabled\n");
		return;
	}

	aio = 1;

#ifdef STREAM_USE_DEVMEM
	if(nodevmem == 0){
		nbuf = devnbuf;
		bs = devbs;

		for(i=0; i< nbuf; i++)
			totalsize += bs[i];

		printf("stream buffer phys 0x%08x kern 0x%08x mmap %p\n", DEVMEM_PHYS, DEVMEM_KERN, DEVMEM_MMAP);
		mem_ptr = DEVMEM_MMAP;
		nextbuf = (unsigned char *)DEVMEM_KERN;
	}
	else
#endif
	{
		nbuf = memnbuf;
		bs = membs;

		for(i=0; i< nbuf; i++)
			totalsize += bs[i];

		s->realbuf = memalign(vol_blksz, totalsize);
		if(s->realbuf == NULL){
		  printf("%s:%d allocate buffer memory failed\n", __func__, __LINE__);
		  return;
		}

		if(bs[0] < s->buf_len){
		  printf("%s:%d first buffer size %d smaller than buf_len %d\n", __func__, __LINE__, bs[0], s->buf_len);
		  return;
		}

		if(s->buf_pos < s->buf_len){
		  printf("%s:%d remaining %d bytes\n", __func__, __LINE__, s->buf_len - s->buf_pos);
		  memcpy(s->realbuf, s->buffer, s->buf_len); // copy data to new buffer
		}

		free(s->buffer);
		s->buffer = s->realbuf;

		nextbuf = s->realbuf;
	}

	printf("%s: enable aio nbuf %d total %d\n", __func__, nbuf, totalsize);

	// initialize once
	s->aiopos = s->pos;
#ifdef CONFIG_BLURAY
	extern char *bluray_device;
	if(bluray_device){
		off_t bd_get_current_file_position(stream_t *s);
		s->aiopos = bd_get_current_file_position(s);
	}
#endif

	s->aiolst = calloc(nbuf, sizeof(struct aiocb));
	if(s->aiolst == NULL){
		printf("%s:%d allocate memory for aiolst failed\n", __func__, __LINE__);
		return;
	}

	for(i=0; i<nbuf; i++){
		s->aiolst[i].aio_fildes = s->fd;
		s->aiolst[i].aio_buf = nextbuf;
		s->aiolst[i].aio_nbytes = bs[i];
		if(i != 0){ // 1st buffer has already been filled, skip it
			s->aiolst[i].aio_offset = s->aiopos;
			s->aiopos += bs[i];
			//printf("%s:%d starts new aio idx %d buf %p ofs 0x%llx size 0x%x\n", __func__, __LINE__, i, s->aiolst[i].aio_buf, s->aiolst[i].aio_offset, s->aiolst[i].aio_nbytes);
			ret = aio_read(&s->aiolst[i]);
			if(ret < 0) printf("aio_read to buffer %d error: %s\n", i, strerror(errno));
		}
		nextbuf += bs[i];
	}

	s->aioidx = 0;
}
#endif

static void dump_buf(off_t ofs, char *buf, int size)
{
	int i;

	for(i=0; i<size; i++){
		if((ofs + i) % 16 == 0)
			printf("\n%0llx: ", ofs + i);
		printf("%02x ", *(buf + i));
	}
	printf("\n");
}

ssize_t aio_fill_buffer(stream_t *s)
{
	int i, ret, size;
	int *bs;
	unsigned char *nextbuf;

	//printf("%s:%d pos 0x%llx buf_pos 0x%x buf_len 0x%x\n", __func__, __LINE__, s->pos, s->buf_pos, s->buf_len);

	//buffer not empty
	if(s->buf_pos != s->buf_len){
		printf("%s:%d buffer not empty yet. pos 0x%x len 0x%x\n", __func__, __LINE__, s->buf_pos, s->buf_len);
		return 0;
	}

	if(s->buf_len == 0){
		//printf("%s: aio reinit\n", __func__);
		// cancel all asynchronous I/O
		if(aio_cancel(s->fd, NULL) == AIO_NOTCANCELED){
			for(i=0; i<nbuf; i++){
				while(aio_error(&s->aiolst[i]) == EINPROGRESS){
					//printf("%s:%d waiting for aio cancel finished idx %d\n", __func__, __LINE__, i);
					struct aiocb *cblist[] = { &s->aiolst[i] };
					ret = aio_suspend(cblist, 1, NULL);
					if(ret != 0)
						perror("aio_suspend");
					else
						break;
				}
			}
		}

		// re-initialize while seeking
		s->aiopos = s->pos;
#ifdef CONFIG_BLURAY
		extern char *bluray_device;
		if(bluray_device){
			off_t bd_get_current_file_position(stream_t *s);
			s->aiopos = bd_get_current_file_position(s);
		}
#endif

#ifdef STREAM_USE_DEVMEM
		if(nodevmem == 0){
			bs = devbs;
			nextbuf = (unsigned char *)DEVMEM_KERN;
		}
		else
#endif
		{
			bs = membs;
			nextbuf = s->realbuf;
		}

		for(i=0; i<nbuf; i++){
			s->aiolst[i].aio_fildes = s->fd;
			s->aiolst[i].aio_buf = nextbuf;
			s->aiolst[i].aio_nbytes = bs[i];
			s->aiolst[i].aio_offset = s->aiopos;
			s->aiopos += bs[i];
			nextbuf += bs[i];
			//printf("%s:%d starts new aio idx %d buf %p ofs 0x%llx size 0x%x\n", __func__, __LINE__, i, s->aiolst[i].aio_buf, s->aiolst[i].aio_offset, s->aiolst[i].aio_nbytes);
			ret = aio_read(&s->aiolst[i]);
			if(ret < 0) perror("aio_read");
		}
		s->aioidx = 0;
	}else{
		//start new aio
		//printf("%s:%d starts new aio idx %d buf %p ofs 0x%llx size 0x%x\n", __func__, __LINE__, s->aioidx, s->aiolst[i].aio_buf, s->aiolst[i].aio_offset, s->aiolst[i].aio_nbytes);
		s->aiolst[s->aioidx].aio_offset = s->aiopos;
		ret = aio_read(&s->aiolst[s->aioidx]);
		if(ret < 0) perror("aio_read");
		s->aiopos += s->aiolst[s->aioidx].aio_nbytes;
		s->aioidx = (s->aioidx + 1) % nbuf;
	}

	//wait buffer finish
	while(aio_error(&s->aiolst[s->aioidx]) == EINPROGRESS){
		//printf("%s:%d waiting for aio read finished idx %d\n", __func__, __LINE__, s->aioidx);
		struct aiocb *cblist[] = { &s->aiolst[s->aioidx] };
		ret = aio_suspend(cblist, 1, NULL);
		if(ret != 0)
			perror("aio_suspend");
		else
			break;
	}

	//check size
	size = aio_return(&s->aiolst[s->aioidx]);
	if(size <= 0){
		off_t endpos = s->end_pos;
#ifdef CONFIG_BLURAY
		extern char *bluray_device;
		if(bluray_device){
			struct stat fst;
			if(fstat(s->fd, &fst) == 0){
				endpos = fst.st_size;
			}
		}
#endif
		if(s->aiolst[s->aioidx].aio_offset < endpos)
			printf("%s:%d aio read error idx %d buf %p ofs 0x%llx ret %d %s\n", __func__, __LINE__, s->aioidx, s->aiolst[s->aioidx].aio_buf, s->aiolst[s->aioidx].aio_offset, size, size ? strerror(errno) : "");
		//else
		//	printf("aio_offset (0x%llx) >= endpos (0x%llx)\n", s->aiolst[s->aioidx].aio_offset, endpos);
		return size;
	}

	//change buffer
	//printf("%s:%d aio read finished idx %d buf %p ofs 0x%llx size 0x%x\n", __func__, __LINE__, s->aioidx, s->aiolst[s->aioidx].aio_buf, s->aiolst[s->aioidx].aio_offset, size);
#ifdef STREAM_USE_DEVMEM
	if(nodevmem == 0){
		s->buffer = mem_ptr + (DEVMEM_BSZ * s->aioidx);
		//printf("s->buffer=0x%x\n", s->buffer);
	}
	else
#endif
	{
		s->buffer = s->aiolst[s->aioidx].aio_buf;
	}

	//if(s->buf_len == 0) dump_buf(s->pos, s->buffer, 1024);

	return size;
}
#endif

static int fill_buffer(stream_t *s, char* buffer, int max_len){
  int r;

#ifdef STREAM_AIO
  if(aio && s->aiolst)
    r = aio_fill_buffer(s);
  else
#endif
    r = read(s->fd,buffer,max_len);

  return (r <= 0) ? -1 : r;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
  int r = write(s->fd,buffer,len);
  return (r <= 0) ? -1 : r;
}

static int seek(stream_t *s,off_t newpos) {
  s->pos = newpos;
  s->buf_pos=s->buf_len=0; // reset to make sure buffer is cleared
  if(lseek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
  return 1;
}

static int seek_forward(stream_t *s,off_t newpos) {
  if(newpos<s->pos){
    mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
    return 0;
  }
  while(s->pos<newpos){
    int len=s->fill_buffer(s,s->buffer,STREAM_BUFFER_SIZE);
    if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; break; } // EOF
    s->buf_pos=0;
    s->buf_len=len;
    s->pos+=len;
  }
  return 1;
}

static int control(stream_t *s, int cmd, void *arg) {
  switch(cmd) {
    case STREAM_CTRL_GET_SIZE: {
      off_t size;

      size = lseek(s->fd, 0, SEEK_END);
      lseek(s->fd, s->pos, SEEK_SET);
      if(size != (off_t)-1) {
        *((off_t*)arg) = size;
        return 1;
      }
    }
  }
  return STREAM_UNSUPPORTED;
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {
  int f, f2;
  mode_t m = 0;
  off_t len;
  unsigned char *filename;
  struct stream_priv_s* p = (struct stream_priv_s*)opts;
  if(mode == STREAM_READ){
    m = O_RDONLY;
  }else if(mode == STREAM_WRITE){
    m = O_RDWR|O_CREAT|O_TRUNC;
  }else {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] Unknown open mode %d\n",mode);
    m_struct_free(&stream_opts,opts);
    return STREAM_UNSUPPORTED;
  }

  if(p->filename)
    filename = p->filename;
  else if(p->filename2)
    filename = p->filename2;
  else
    filename = NULL;
  if(!filename) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] No filename\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

#if HAVE_DOS_PATHS
  // extract '/' from '/x:/path'
  if( filename[ 0 ] == '/' && filename[ 1 ] && filename[ 2 ] == ':' )
    filename++;
#endif

  m |= O_BINARY;

  if(!strcmp(filename,"-")){
    if(mode == STREAM_READ) {
      // read from stdin
      mp_msg(MSGT_OPEN,MSGL_INFO,MSGTR_ReadSTDIN);
      f=0; // 0=stdin
#if HAVE_SETMODE
      setmode(fileno(stdin),O_BINARY);
#endif
    } else {
      mp_msg(MSGT_OPEN,MSGL_INFO,"Writing to stdout\n");
      f=1;
#if HAVE_SETMODE
      setmode(fileno(stdout),O_BINARY);
#endif
    }
  } else {
      mode_t openmode = S_IRUSR|S_IWUSR;
#ifndef __MINGW32__
      openmode |= S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
#endif

      f=open(filename,m, openmode);
      if(f<0) {
        mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
        m_struct_free(&stream_opts,opts);
        return STREAM_ERROR;
      }
#ifdef STREAM_NI
      f2=open(filename, m, openmode);
      if(f2<0) {
        mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
        m_struct_free(&stream_opts,opts);
        return STREAM_ERROR;
      }
#endif
#ifdef STREAM_DIO
      if(dio){
	int flags = fcntl(f, F_GETFL) | O_DIRECT;
#ifdef STREAM_NI
	if(fcntl(f, F_SETFL, flags) == -1 || fcntl(f2, F_SETFL, flags) == -1)
#else
	if(fcntl(f, F_SETFL, flags) == -1)
#endif
	{
	  printf("%s: failed to enable direct I/O\n", __func__);
	  dio = 0;
	}else{
	  printf("%s: enable dio\n", __func__);
	}
      }
#endif
  }

  len=lseek(f,0,SEEK_END); lseek(f,0,SEEK_SET);
#ifdef __MINGW32__
  if(f==0 || len == -1) {
#else
  if(len == -1) {
#endif
    if(mode == STREAM_READ) stream->seek = seek_forward;
    stream->type = STREAMTYPE_STREAM; // Must be move to STREAMTYPE_FILE
    stream->flags |= MP_STREAM_SEEK_FW;
  } else if(len >= 0) {
    stream->seek = seek;
    stream->end_pos = len;
    stream->type = STREAMTYPE_FILE;
  }

  mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %"PRId64" bytes\n", (int64_t)len);

  stream->fd = f;
#ifdef STREAM_NI
  stream->afd = f;
  stream->abuf = stream->realbuf;

  stream->vfd = f2;
  stream->vbuf = stream->realbuf + STREAM_BUFFER_SIZE;
#endif
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = write_buffer;
  stream->control = control;

  m_struct_free(&stream_opts,opts);
  return STREAM_OK;
}

const stream_info_t stream_info_file = {
  "File",
  "file",
  "Albeu",
  "based on the code from ??? (probably Arpi)",
  open_f,
  { "file", "", NULL },
  &stream_opts,
  1 // Urls are an option string
};
