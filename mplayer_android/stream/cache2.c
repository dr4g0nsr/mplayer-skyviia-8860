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

// Initial draft of my new cache system...
// Note it runs in 2 processes (using fork()), but doesn't requires locking!!
// TODO: seeking, data consistency checking

#define READ_USLEEP_TIME 10000
#define FILL_USLEEP_TIME 50000
#define PREFILL_SLEEP_TIME 200
#define CONTROL_SLEEP_TIME 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "osdep/shmem.h"
#include "osdep/timer.h"
#if defined(__MINGW32__)
#include <windows.h>
static void cache_thread( void *s );
#elif defined(__OS2__)
#define INCL_DOS
#include <os2.h>
static void cache_thread( void *s );
#elif defined(PTHREAD_CACHE)
#include "pthread-macro.h"
static void *cache_thread(void *s);
#else
#include <sys/wait.h>
#endif

#include <time.h>

#include "mp_msg.h"
#include "help_mp.h"

#include "stream.h"
#include "cache2.h"
extern int use_gui;

typedef struct {
  // constats:
  unsigned char *buffer;      // base pointer of the alllocated buffer memory
  int buffer_size; // size of the alllocated buffer memory
  int sector_size; // size of a single sector (2048/2324)
  int back_size;   // we should keep back_size amount of old bytes for backward seek
  int fill_limit;  // we should fill buffer only if space>=fill_limit
  int seek_limit;  // keep filling cache if distanse is less that seek limit
#ifdef FILE_CACHE
  int mmap_cache;  // use memory-mapped buffer as cache space
#endif
  // filler's pointers:
  int eof;
  off_t min_filepos; // buffer contain only a part of the file, from min-max pos
  off_t max_filepos;
  off_t offset;      // filepos <-> bufferpos  offset value (filepos of the buffer's first byte)
  // reader's pointers:
  off_t read_filepos;
  // commands/locking:
//  int seek_lock;   // 1 if we will seek/reset buffer, 2 if we are ready for cmd
//  int fifo_flag;  // 1 if we should use FIFO to notice cache about buffer reads.
  // callback
  stream_t* stream;
  volatile int control;
  volatile unsigned control_uint_arg;
  volatile double control_double_arg;
  volatile int control_res;
  volatile off_t control_new_pos;
  volatile double stream_time_length;
} cache_vars_t;

extern int is_skynet;
extern int vol_blksz;
static int min_fill=0;
#ifdef YOUTUBE_RECONNECT
extern int youtube;
#endif

int cache_type = 2; // 0:malloc / 1:file cache / 2:vmem cache
int cache_fill_status=0;

//charleslin 20100610
#ifdef PTHREAD_CACHE
static pthread_t cachetid;
static pthread_cond_t readcond;
static pthread_mutex_t readmtx;
static pthread_cond_t cachecond;
static pthread_mutex_t cachemtx;
#endif

#ifdef FILE_CACHE
#if 1
int ncache_size = 0;	// in kbytes
char *ncache_fn = NULL;
#else
int ncache_size = 15*1024;	// in kbytes
char *ncache_fn = "/data/xbfe1846.tmp";
#endif
#endif

//#define DUMP_RECVDATA
#ifdef DUMP_RECVDATA
//#define DUMP_FILENAME "/mnt/usb81/dump/dumpXXXXXX"
//static char dumpfn[] = DUMP_FILENAME;
#define DUMP_FILENAME "/mnt/usb81/dump/%Y%m%d-%H%M%S.dump"
static char dumpfn[128];
static int dumpfd = -1;
#endif

static inline void update_cache_fill_status(cache_vars_t *s)
{
	int change = 0;
	if(s->eof){
		if(cache_fill_status != 100) change = 1;
		//printf("end of file, force cache_fill_status to 100%\n");
		cache_fill_status = 100; // used for ipc_callback_buffering() 
	}else{
		int percent = (s->max_filepos - s->read_filepos) / (s->buffer_size / 100);
		if(percent < 0){
			printf("%s:%d invalid cache_fill_status:%d read_filepos:%llx max_filepos:%llx\n", __func__, __LINE__, percent, s->read_filepos, s->max_filepos);
			//cache_fill_status = -1;	//don't report negative value or SkyPlayerService will stop ipc_callback_buffering()
			cache_fill_status = 0;		//when seek happens, set cache_fill_status to 0 to trigger VideoPlayer buffering mechanism
		}else{
			if(cache_fill_status != percent) change = 1;
			cache_fill_status = percent;
		}
	}
	//if(change) printf("cache %d%%\n", cache_fill_status);
}

static void cache_stats(cache_vars_t *s)
{
  int newb=s->max_filepos-s->read_filepos; // new bytes in the buffer
  mp_msg(MSGT_CACHE,MSGL_INFO,"0x%06X  [0x%06X]  0x%06X   ",(int)s->min_filepos,(int)s->read_filepos,(int)s->max_filepos);
  mp_msg(MSGT_CACHE,MSGL_INFO,"%3d %%  (%3d%%)\n",100*newb/s->buffer_size,100*min_fill/s->buffer_size);
}

static int cache_read(cache_vars_t *s, unsigned char *buf, int size)
{
  int total=0;
  while(size>0){
    int pos,newb,len;

  //printf("%s:%d 0x%llx <= 0x%llx <= 0x%llx\n", __func__, __LINE__, s->min_filepos, s->read_filepos, s->max_filepos);

    if(s->read_filepos>=s->max_filepos || s->read_filepos<s->min_filepos){
	// eof?
	if(s->eof) break;
	// waiting for buffer fill...
#ifdef PTHREAD_CACHE
	//charleslin 20100610
	//printf("cache empty %d\n", cache_fill_status);
	if(pwait_event_timeout(&readcond, &readmtx, 1) == ETIMEDOUT){
		//printf("wait cache not empty timedout\n");
	}
#else
	usec_sleep(READ_USLEEP_TIME); // 10ms
#endif
	continue; // try again...
    }

    newb=s->max_filepos-s->read_filepos; // new bytes in the buffer
    if(newb<min_fill) min_fill=newb; // statistics...

//    printf("*** newb: %d bytes ***\n",newb);

    pos=s->read_filepos - s->offset;
    if(pos<0) pos+=s->buffer_size; else
    if(pos>=s->buffer_size) pos-=s->buffer_size;

    if(newb>s->buffer_size-pos) newb=s->buffer_size-pos; // handle wrap...
    if(newb>size) newb=size;

    // check:
    if(s->read_filepos<s->min_filepos) mp_msg(MSGT_CACHE,MSGL_ERR,"Ehh. s->read_filepos<s->min_filepos !!! Report bug...\n");

    // len=write(mem,newb)
    //printf("Buffer read: %d bytes\n",newb);
    memcpy(buf,&s->buffer[pos],newb);
    buf+=newb;
    len=newb;
    // ...

    s->read_filepos+=len;
    size-=len;
    total+=len;

  }

  update_cache_fill_status(s);
  
//charleslin 20100610
#ifdef PTHREAD_CACHE
#if 1
  //printf("eof %d end_pos %llx max_filepos %llx\n", s->stream->eof, s->stream->end_pos, s->max_filepos);
  // not EOF and cache not finished
  if(s->stream->eof == 0 &&
     (s->stream->end_pos == 0 || s->max_filepos < s->stream->end_pos)){
    int back, newb, space;

    // calc number of back-bytes:
    back=s->read_filepos - s->min_filepos;
    if(back<0) back=0; // strange...
    if(back>s->back_size) back=s->back_size;

    // calc number of new bytes:
    newb=s->max_filepos - s->read_filepos;
    if(newb<0) newb=0; // strange...

    space=s->buffer_size - (newb+back);
    //if(space >= s->fill_limit)
    if(space)
    {
  	//printf("space %d\n", space);
  	pwake_up(&cachecond, &cachemtx);
    }
  }
#else
  pwake_up(&cachecond, &cachemtx);
#endif
#endif

  return total;
}

static int cache_fill(cache_vars_t *s)
{
  int back,back2,newb,space,len,pos,limit;
  off_t read=s->read_filepos;

  if(read<s->min_filepos || read>s->max_filepos){
      // seek...
      mp_msg(MSGT_CACHE,MSGL_DBG2,"Out of boundaries... seeking to 0x%"PRIX64"  \n",(int64_t)read);
      //printf("read_filepos %llx min_filepos %llx max_filepos %llx seek_limit %x\n", read, s->min_filepos, s->max_filepos, s->seek_limit);
      // streaming: drop cache contents only if seeking backward or too much fwd:
      if(s->stream->type!=STREAMTYPE_STREAM ||
	  (is_skynet && s->stream->seek) ||
          read<s->min_filepos || read>=s->max_filepos+s->seek_limit)
      {
        s->offset= // FIXME!?
        s->min_filepos=s->max_filepos=read; // drop cache content :(
        if(s->stream->eof) stream_reset(s->stream);
        stream_seek(s->stream,read);
        mp_msg(MSGT_CACHE,MSGL_DBG2,"Seek done. new pos: 0x%"PRIX64"  \n",(int64_t)stream_tell(s->stream));
        //printf("Seek done. new pos: 0x%"PRIX64"  \n",(int64_t)stream_tell(s->stream));
      }
  }

  // calc number of back-bytes:
  back=read - s->min_filepos;
  if(back<0) back=0; // strange...
  if(back>s->back_size) back=s->back_size;

  // calc number of new bytes:
  newb=s->max_filepos - read;
  if(newb<0) newb=0; // strange...

  // calc free buffer space:
  space=s->buffer_size - (newb+back);

  //printf("min_filepos %llx max_filepos %llx\n", s->min_filepos, s->max_filepos);
  //printf("### read=0x%llx  back=%d  newb=%d  space=%d  pos=%d\n",read,back,newb,space,pos);

  // WORKAROUND
  // Space may keep 0 if back is smaller than back_size, which is buffer_size/2 by default.
  // Once space is 0, cache may stop receiving data from network, then youtube disconnects.
  // So we set back_size to current back to avoid this issue.
#ifdef YOUTUBE_RECONNECT
  if((space == 0) && (back > 0) && (youtube)){
	  //printf("%s:%d back_size = %d; back = %d; change = %d\n", __func__, __LINE__, s->back_size, back, back - s->back_size);
	  s->back_size = back;
  }
#endif
  // calc bufferpos:
  pos=s->max_filepos - s->offset;
  if(pos>=s->buffer_size) pos-=s->buffer_size; // wrap-around

#if 0
  //if(space<s->fill_limit)
  if(space <= 0)
   {
     //printf("Buffer is full (%d bytes free, limit: %d)\n",space,s->fill_limit);
     return 0; // no fill...
   }
#else
#ifdef YOUTUBE_RECONNECT
  if(youtube)
    limit = 0;
  else
#endif
    limit = s->fill_limit;

  if(space<=limit)
  {
    //printf("%s:%d Buffer is full (%d bytes free, back_size = %d)\n", __func__, __LINE__, space, s->back_size);
    return 0; // no fill...
  }
#endif

  // reduce space if needed:
  //printf("s->buffer_size %d - pos %d = %d space %d\n", s->buffer_size, pos, s->buffer_size-pos, space);
  if(space > (s->buffer_size-pos)) space=s->buffer_size-pos;
  //printf("%d space %d\n", __LINE__, space);

//  if(space>32768) space=32768; // limit one-time block size
  if(space > (4*s->sector_size)) space=4*s->sector_size;
  //printf("%d space %d\n", __LINE__, space);

//  if(s->seek_lock) return 0; // FIXME

#if 1
  // back+newb+space <= buffer_size
  back2=s->buffer_size-(space+newb); // max back size
  if(s->min_filepos<(read-back2)) s->min_filepos=read-back2;
#else
  s->min_filepos=read-back; // avoid seeking-back to temp area...
#endif

  // ....
  //printf("Buffer fill: %d bytes of %d\n",space,s->buffer_size);
  //len=stream_fill_buffer(s->stream);
  //memcpy(&s->buffer[pos],s->stream->buffer,len); // avoid this extra copy!
  // ....
  len=stream_read(s->stream,&s->buffer[pos],space);
#ifdef DUMP_RECVDATA
  if(dumpfd >= 0){
	  if(len > 0){
		write(dumpfd, &s->buffer[pos], len);
	  }else{
	  	printf("%s: get zero length, stop dumping file\n", __func__);
		close(dumpfd);
		dumpfd = -1;
	  }
  }
  
#endif

  if(!len){
    //printf("%s:%d cache eof\n", __func__, __LINE__);
    s->eof=1;
  }

  s->max_filepos+=len;
  if(pos+len>=s->buffer_size){
      // wrap...
      s->offset+=s->buffer_size;
  }

  update_cache_fill_status(s);

  //printf("read %x max_filepos %llx\n", len, s->max_filepos);
  return len;

}

static int cache_execute_control(cache_vars_t *s) {
  static unsigned last;
  int quit = s->control == -2;
  if (quit || !s->stream->control) {
    s->stream_time_length = 0;
    s->control_new_pos = 0;
    s->control_res = STREAM_UNSUPPORTED;
    s->control = -1;
    return !quit;
  }
  if (GetTimerMS() - last > 99) {
    double len;
    if (s->stream->control(s->stream, STREAM_CTRL_GET_TIME_LENGTH, &len) == STREAM_OK)
      s->stream_time_length = len;
    else
      s->stream_time_length = 0;
    last = GetTimerMS();
  }
  if (s->control == -1) return 1;
  switch (s->control) {
    case STREAM_CTRL_GET_CURRENT_TIME:
    case STREAM_CTRL_SEEK_TO_TIME:
    case STREAM_CTRL_GET_ASPECT_RATIO:
      s->control_res = s->stream->control(s->stream, s->control, &s->control_double_arg);
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
    case STREAM_CTRL_SET_ANGLE:
      s->control_res = s->stream->control(s->stream, s->control, &s->control_uint_arg);
      break;
    default:
      s->control_res = STREAM_UNSUPPORTED;
      break;
  }
  s->control_new_pos = s->stream->pos;
  s->control = -1;
  return 1;
}

#ifdef VMEM_CACHE
#include "libmpcodecs/sky_vdec_2.h"
extern int is_dvdnav;
static int allocate_vmem_cache(cache_vars_t *s, int sector)
{
	BufParams dpb_parm;    
	int skyfd, memfd;
	unsigned int video_addr;
	unsigned int video_size;
	unsigned int mmap_addr;

	s->buffer = NULL;
	
	skyfd = open("/dev/skyvdec", O_RDWR);
	if ( skyfd== -1 )
	{
		printf("Unable to open /dev/skyvdec\n");
		return 1;
	}

	dpb_parm.id = DEFAULT_DPB_ID;
	if (ioctl(skyfd, SKY_VDEC_IOC_GET_MEM_ADDRESS, &dpb_parm) == -1)
	{
		printf("SKY_VDEC_GET_MEM_ADDRESS ioctl failed\n");
		return 1;
	}
	video_addr = dpb_parm.busAddress;

	dpb_parm.id = DEFAULT_DPB_ID;
	if (ioctl(skyfd, SKY_VDEC_IOC_GET_MEM_SIZE, &dpb_parm) == -1)
	{
		printf("SKY_VDEC_IOC_GET_MEM_SIZE ioctl failed\n");
		return 1;
	}
	video_size = dpb_parm.size;

	memfd = open("/dev/mem", O_RDWR);
	if ( memfd== -1 )
	{
		printf("Unable to open /dev/mem\n");
		return 1;
	}

	if(s->buffer_size > VMEM_CACHE_SIZE)
		s->buffer_size = VMEM_CACHE_SIZE;
	
	mmap_addr = video_addr + video_size;
	mmap_addr -= VMEM_STREAM_SIZE;
	if(is_dvdnav)
		mmap_addr -= VMEM_DVDNAV_SIZE;
	mmap_addr -= s->buffer_size;

	s->buffer = mmap(0, s->buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, mmap_addr);
	close(memfd);
	if (s->buffer == MAP_FAILED)
	{
		printf("%s: mmap failed phys=0x%x  size=0x%x\n", __func__, mmap_addr, s->buffer_size);
		s->buffer = NULL;
		return 1;
	}

	s->mmap_cache = 1;
	s->fill_limit = 8*sector;
	s->back_size = s->buffer_size/2;
	//printf("%s: phys %x virt %x size %x\n", __func__, mmap_addr, s->buffer, s->buffer_size);

	//printf("%s:%d mmap file %s size %d addr %p\n", __func__, __LINE__, tmpfn, s->buffer_size, s->buffer);
	return 0;
}
#endif

#ifdef FILE_CACHE
static int allocate_file_cache(cache_vars_t *s, int sector)
{
	struct stat fst;
	int cache_size;
	int fd;

	s->buffer = NULL;
	
	if(ncache_size == 0 || ncache_fn == NULL)
		return 1;
	
	cache_size = ncache_size * 1024;
	fd = open(ncache_fn, O_CREAT | O_RDWR, S_IRWXU);
	if(fd == -1){
		//printf("%s:%d open file %s failed: %d(%s)\n", __func__, __LINE__, tmpfn, errno, strerror(errno));
		printf("%s:%d error %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
		return 1;
	}

	if(fstat(fd, &fst) == -1){
		//printf("%s:%d get file status failed: %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
		printf("%s:%d error %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
		close(fd);
		return 1;
	}

	if(fst.st_size < cache_size){
		int i, sz;
		char buf[1024];
		for(i=0; i < cache_size; i+=1024){
			sz = write(fd, buf, 1024);
			if(sz != 1024){
				//printf("%s:%d enlarge temp file to %d bytes failed: sz %d errno %d (%s)\n", __func__, __LINE__, cache_size, sz, errno, strerror(errno));
				printf("%s:%d error %d %d (%s)\n", __func__, __LINE__, sz, errno, strerror(errno));
				close(fd);
				return 1;
			}
		}
	}

	s->buffer = mmap(NULL, cache_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);
	if(s->buffer == MAP_FAILED){
		//printf("%s:%d mmap failed: %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
		printf("%s:%d error %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
		s->buffer = NULL;
		return 1;
	}

	s->mmap_cache = 1;
	s->fill_limit = 8*sector;
	s->buffer_size = cache_size;
	s->back_size = s->buffer_size/2;

	//printf("%s:%d mmap file %s size %d addr %p\n", __func__, __LINE__, tmpfn, s->buffer_size, s->buffer);
	return 0;
}
#endif

static cache_vars_t* cache_init(int size,int sector){
  int num;
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__)
  cache_vars_t* s=shmem_alloc(sizeof(cache_vars_t));
#else
  cache_vars_t* s=malloc(sizeof(cache_vars_t));
#endif
  if(s==NULL) return NULL;

  memset(s,0,sizeof(cache_vars_t));
  num=size/sector;
  if(num < 16){
     num = 16;
  }//32kb min_size
  s->buffer_size=num*sector;
  s->sector_size=sector;
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__)
  s->buffer=shmem_alloc(s->buffer_size);
#else

  switch(cache_type){
  case 0:
    //printf("%s: uses malloc\n", __func__);
    s->buffer=malloc(s->buffer_size);
    break;
#ifdef FILE_CACHE // charleslin 20101130
  case 1:
    //printf("%s: uses file cache\n", __func__);
    allocate_file_cache(s, sector);
    break;
#endif
#ifdef VMEM_CACHE // charleslin 20110217
  case 2:
  default:
    //printf("%s: uses vmem cache\n", __func__);
    allocate_vmem_cache(s, sector);
    break;
#endif
  } //end of switch
#endif

  if(s->buffer == NULL){
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__)
    shmem_free(s,sizeof(cache_vars_t));
#else
    free(s);
#endif
    return NULL;
  }

  printf("%s: current cache type:%d size:%dKB\n", __func__, cache_type, s->buffer_size/1024);

  s->fill_limit=8*sector;
  s->back_size=s->buffer_size/2;
  return s;
}

void cache_uninit(stream_t *s) {
  cache_vars_t* c = s->cache_data;
  if(s->cache_pid) {
#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
    cache_do_control(s, -2, NULL);
    pwake_up(&cachecond, &cachemtx);
    pthread_join(cachetid, NULL);
#else
    kill(s->cache_pid,SIGKILL);
    waitpid(s->cache_pid,NULL,0);
#endif
    s->cache_pid = 0;
  }
  if(!c) return;
#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
#ifdef FILE_CACHE
  if(c->mmap_cache)
    munmap(c->buffer, c->buffer_size);
  else
#endif
    free(c->buffer);
  c->buffer = NULL;
#ifdef STREAM_AIO
  if(c->stream->aiolst) free(c->stream->aiolst);
#endif
#ifdef STREAM_BUF_PTR
  if(c->stream->realbuf) free(c->stream->realbuf);
#endif
  if(c->stream) free(c->stream);
  c->stream = NULL;
  free(s->cache_data);
#else
  shmem_free(c->buffer,c->buffer_size);
  c->buffer = NULL;
  shmem_free(s->cache_data,sizeof(cache_vars_t));
#endif
  s->cache_data = NULL;
#ifdef DUMP_RECVDATA
  if(dumpfd >= 0) close(dumpfd);
#endif
}

static void exit_sighandler(int x){
  // close stream
  exit(0);
}

/**
 * \return 1 on success, 0 if the function was interrupted and -1 on error
 */
int stream_enable_cache(stream_t *stream,int size,int min,int seek_limit){
  //int ss = stream->sector_size ? stream->sector_size : STREAM_BUFFER_SIZE;
  int ss = stream->sector_size ? stream->sector_size : 2048;
  int res = -1;
  cache_vars_t* s;

  if (stream->flags & STREAM_NON_CACHEABLE) {
    mp_msg(MSGT_CACHE,MSGL_STATUS,"\rThis stream is non-cacheable\n");
    return 1;
  }

  s=cache_init(size,ss);
  if(s == NULL) return -1;

  stream->cache_data=s;
  s->stream=stream; // callback

  /* 
   * for some non-interleaved containers (mov/mp4/flv), the audio/video distance
   * is more than 1MB, we need to set seek_limit > 1MB, otherwise stream will
   * seek between audio/video and reconnect continuously.
   */
#if 1 // 20111226 charleslin - seek_limit = cache 20MB x 50% = 10MB is too large. set max value
  if(seek_limit > 2*1024*1024)
	s->seek_limit = 2*1024*1024;
  else
#endif
	s->seek_limit = seek_limit;

  //make sure that we won't wait from cache_fill
  //more data than it is alowed to fill
  if (s->seek_limit > s->buffer_size - s->fill_limit ){
     s->seek_limit = s->buffer_size - s->fill_limit;
  }
  if (min > s->buffer_size - s->fill_limit) {
     min = s->buffer_size - s->fill_limit;
  }

  //printf("buf %p size %d sect %d back %d fill %d seek %d\n", s->buffer, s->buffer_size, s->sector_size, s->back_size, s->fill_limit, s->seek_limit);

#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__)
  if((stream->cache_pid=fork())){
    if ((pid_t)stream->cache_pid == -1)
      stream->cache_pid = 0;
#else
  {
    stream_t* stream2=malloc(sizeof(stream_t));
    memcpy(stream2,s->stream,sizeof(stream_t));
#ifdef STREAM_BUF_PTR
    stream2->buffer = stream2->realbuf = memalign(vol_blksz, STREAM_BUFFER_SIZE);	// aligned to 512bytes for direct I/O
#endif
    s->stream=stream2;
#if defined(__MINGW32__)
    stream->cache_pid = _beginthread( cache_thread, 0, s );
#elif defined(__OS2__)
    stream->cache_pid = _beginthread( cache_thread, NULL, 256 * 1024, s );
#else
    //charleslin 20100610
    pthread_mutex_init(&readmtx, NULL);
    pthread_cond_init(&readcond, NULL);
    pthread_mutex_init(&cachemtx, NULL);
    pthread_cond_init(&cachecond, NULL);

    pthread_create(&cachetid, NULL, cache_thread, s);
    stream->cache_pid = 1;
#endif
#endif
    if (!stream->cache_pid) {
        mp_msg(MSGT_CACHE, MSGL_ERR,
               "Starting cache process/thread failed: %s.\n", strerror(errno));
        goto err_out;
    }

#ifdef DUMP_RECVDATA
    //strcpy(dumpfn, DUMP_FILENAME);
    //mktemp(dumpfn);
    time_t tt = time(NULL);
    strftime(dumpfn, 128, DUMP_FILENAME, localtime(&tt));
    dumpfd = open(dumpfn, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(dumpfd <= 0)
    	printf("%s: Open dump file %s failed (%s)\n", __func__, dumpfn, strerror(errno));
    else
    	printf("%s: Open dump file %s\n", __func__, dumpfn);
#endif

    // wait until cache is filled at least prefill_init %
    mp_msg(MSGT_CACHE,MSGL_V,"CACHE_PRE_INIT: %"PRId64" [%"PRId64"] %"PRId64"  pre:%d  eof:%d  \n",
	(int64_t)s->min_filepos,(int64_t)s->read_filepos,(int64_t)s->max_filepos,min,s->eof);
    while(s->read_filepos<s->min_filepos || s->max_filepos-s->read_filepos<min){
    	/* In prefill stage, cache to min is 100% */
	//cache_fill_status = (s->max_filepos-s->read_filepos) * 100 / s->buffer_size;
	cache_fill_status = (s->max_filepos-s->read_filepos) * 100 / min;

	mp_msg(MSGT_CACHE,MSGL_STATUS,MSGTR_CacheFill,
	    100.0*(float)(s->max_filepos-s->read_filepos)/(float)(s->buffer_size),
	    (int64_t)s->max_filepos-s->read_filepos
	);
	if(s->eof) break; // file is smaller than prefill size
	if(stream_check_interrupt(PREFILL_SLEEP_TIME)) {
	  res = 0;
	  goto err_out;
        }
    }
    mp_msg(MSGT_CACHE,MSGL_STATUS,"\n");
    return 1; // parent exits

err_out:
    cache_uninit(stream);
    return res;
  }

#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
}
#ifdef PTHREAD_CACHE
static void *cache_thread( void *s ){
#else
static void cache_thread( void *s ){
#endif
#endif
  unsigned int len;

#ifdef CONFIG_GUI
  use_gui = 0; // mp_msg may not use gui stuff in forked code
#endif
// cache thread mainloop:
  //printf("%s renice to %d\n", __func__, nice(-19));
  printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
  signal(SIGTERM, exit_sighandler); // kill
  do {
    len = cache_fill(s);
    if(!len){
#ifdef PTHREAD_CACHE
	//charleslin 20100610
	//printf("cache full %d\n", cache_fill_status);
	if(pwait_event_timeout(&cachecond, &cachemtx, 1) == ETIMEDOUT){
		//printf("wait cache not full timedout\n");
	}
#else
	 usec_sleep(FILL_USLEEP_TIME); // idle
#endif
    }
#ifdef PTHREAD_CACHE
    else{
	//charleslin 20100610
	//printf("cache some\n");
	pwake_up(&readcond, &readmtx);
    }
#endif
  } while (cache_execute_control(s));
  printf("cache thread/proc endend\n");
#if defined(__MINGW32__) || defined(__OS2__)
  _endthread();
#elif defined(PTHREAD_CACHE)
  return NULL;
#else
  // make sure forked code never leaves this function
  exit(0);
#endif
}

int cache_stream_fill_buffer(stream_t *s){
  int len;
  if(s->eof){ s->buf_pos=s->buf_len=0; return 0; }
  if(!s->cache_pid) return stream_fill_buffer(s);

//  cache_stats(s->cache_data);

  if(s->pos!=((cache_vars_t*)s->cache_data)->read_filepos) mp_msg(MSGT_CACHE,MSGL_ERR,"!!! read_filepos differs!!! report this bug...\n");

  len=cache_read(s->cache_data,s->buffer, ((cache_vars_t*)s->cache_data)->sector_size);
  //printf("cache_stream_fill_buffer->read -> %d\n",len);

  if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; return 0; }
  s->buf_pos=0;
  s->buf_len=len;
  s->pos+=len;
//  printf("[%d]",len);fflush(stdout);
  return len;

}

int cache_stream_seek_long(stream_t *stream,off_t pos){
  cache_vars_t* s;
  off_t newpos;
  if(!stream->cache_pid) return stream_seek_long(stream,pos);

  s=stream->cache_data;
//  s->seek_lock=1;

  mp_msg(MSGT_CACHE,MSGL_DBG2,"CACHE2_SEEK: 0x%"PRIX64" <= 0x%"PRIX64" (0x%"PRIX64") <= 0x%"PRIX64"  \n",s->min_filepos,pos,s->read_filepos,s->max_filepos);
  //printf("CACHE2_SEEK: 0x%"PRIX64" <= 0x%"PRIX64" (0x%"PRIX64") <= 0x%"PRIX64"  \n",s->min_filepos,pos,s->read_filepos,s->max_filepos);

  newpos=pos/s->sector_size; newpos*=s->sector_size; // align
  //printf("%s:%d set read_filepos %llx => %llx(%llx)\n", __func__, __LINE__, s->read_filepos, newpos, pos);
  stream->pos=s->read_filepos=newpos;
  s->eof=0; // !!!!!!!
#if 0
  if(s->read_filepos < s->min_filepos){
	  printf("pos = %d ; s->read_filepos = %d ; diff = %d \n",(int)pos,(int)s->read_filepos,(int)s->read_filepos-(int)pos);
	  printf("s->min_filepos = %d ,s->max_filepos = %d\n",(int)s->min_filepos,(int)s->max_filepos);
	  printf("s->read_filepos - s->min_filepos = %d \n",(int)s->read_filepos-(int)s->min_filepos);
	  printf("s->sector_size = %d ; s->fill_limit = %d ; stream->buf_pos = %d ; stream->buf_len = %d \n",s->sector_size,s->fill_limit,stream->buf_pos,stream->buf_len);
  }
#endif
  cache_stream_fill_buffer(stream);

  pos-=newpos;
  if(pos>=0 && pos<=stream->buf_len){
    stream->buf_pos=pos; // byte position in sector
    return 1;
  }

//  stream->buf_pos=stream->buf_len=0;
//  return 1;

  mp_msg(MSGT_CACHE,MSGL_V,"cache_stream_seek: WARNING! Can't seek to 0x%"PRIX64" !\n",(int64_t)(pos+newpos));
  return 0;
}

int cache_do_control(stream_t *stream, int cmd, void *arg) {
  cache_vars_t* s = stream->cache_data;
  switch (cmd) {
    case STREAM_CTRL_SEEK_TO_TIME:
      s->control_double_arg = *(double *)arg;
      s->control = cmd;
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_SET_ANGLE:
      s->control_uint_arg = *(unsigned *)arg;
      s->control = cmd;
      break;
// the core might call these every frame, they are too slow for this...
    case STREAM_CTRL_GET_TIME_LENGTH:
//    case STREAM_CTRL_GET_CURRENT_TIME:
      *(double *)arg = s->stream_time_length;
      return s->stream_time_length ? STREAM_OK : STREAM_UNSUPPORTED;
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_ASPECT_RATIO:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
    case -2:
      s->control = cmd;
      break;
    default:
      return STREAM_UNSUPPORTED;
  }
  while (s->control != -1)
    usec_sleep(CONTROL_SLEEP_TIME);
  switch (cmd) {
    case STREAM_CTRL_GET_TIME_LENGTH:
    case STREAM_CTRL_GET_CURRENT_TIME:
    case STREAM_CTRL_GET_ASPECT_RATIO:
      *(double *)arg = s->control_double_arg;
      break;
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
      *(unsigned *)arg = s->control_uint_arg;
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_SEEK_TO_TIME:
    case STREAM_CTRL_SET_ANGLE:
      stream->pos = s->read_filepos = s->control_new_pos;
      break;
  }
  return s->control_res;
}

#ifdef STREAM_AIO_AUTO
void cache_aio_enable(cache_vars_t *data)
{
	stream_t *s = data->stream;

	//printf("stream %p buffer %p buf_pos %d buf_len %d pos %lld\n", s, s->buffer, s->buf_pos, s->buf_len, s->pos);
	aio_enable(s);
}
#endif

