/*
 * Copyright (C) 2002 Samuel Hocevar <sam@zoy.org>,
 *                    Håkan Hjort <d95hjort@dtek.chalmers.se>
 *
 * This file is part of libdvdread.
 *
 * libdvdread is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libdvdread is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with libdvdread; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

#include "config.h"
#include "dvdread/dvd_reader.h"
#include "dvd_input.h"
#include "pthread-macro.h"

//20110527 Robert back to use 256KB
#define DVD_BUFFER_SIZE DVD_VIDEO_LB_LEN*128
//20110526 Robert reduce DVD buffer size, some HDD need fast response time
//#define DVD_BUFFER_SIZE DVD_VIDEO_LB_LEN*32
//static int dvd_last_seek_block=0;
//static int dvd_total_blocks=0;
//static int dvd_seek_flag=0;
static unsigned char *dvd_local_buf=0;
static unsigned char *dvd_local_buf1=0;
static unsigned char *dvd_local_buf2=0;
// read thread
static int dvdnav_thread_running = 0;
static pthread_t dvdnav_read_thread;
static pthread_mutex_t dvdnav_mutex;
static pthread_cond_t read_cond;
static pthread_cond_t write_cond;
static dvd_input_t thread_dev = NULL;
static file_open_cnt = 0;
//static off_t dvd_local_len=0, dvd_local_offset=0;
//static off_t dvd_local_range_start_addr=0, dvd_local_range_end_addr=0;
//static int dvd_last_seek_blocks=0;
/* The function pointers that is the exported interface of this file. */
dvd_input_t (*dvdinput_open)  (const char *);
int         (*dvdinput_close) (dvd_input_t);
int         (*dvdinput_seek)  (dvd_input_t, int);
int         (*dvdinput_title) (dvd_input_t, int);
int         (*dvdinput_read)  (dvd_input_t, void *, int, int);
char *      (*dvdinput_error) (dvd_input_t);

#ifdef HAVE_DVDCSS_DVDCSS_H
/* linking to libdvdcss */
#include <dvdcss/dvdcss.h>
#define DVDcss_open(a) dvdcss_open((char*)(a))
#define DVDcss_close   dvdcss_close
#define DVDcss_seek    dvdcss_seek
#define DVDcss_title   dvdcss_title
#define DVDcss_read    dvdcss_read
#define DVDcss_error   dvdcss_error
#else

/* dlopening libdvdcss */
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#else
/* Only needed on MINGW at the moment */
#include "../../msvc/contrib/dlfcn.c"
#endif

typedef struct dvdcss_s *dvdcss_handle;
static dvdcss_handle (*DVDcss_open)  (const char *);
static int           (*DVDcss_close) (dvdcss_handle);
static int           (*DVDcss_seek)  (dvdcss_handle, int, int);
static int           (*DVDcss_title) (dvdcss_handle, int);
static int           (*DVDcss_read)  (dvdcss_handle, void *, int, int);
static char *        (*DVDcss_error) (dvdcss_handle);
#endif

/* The DVDinput handle, add stuff here for new input methods. */
struct dvd_input_s {
    /* libdvdcss handle */
    dvdcss_handle dvdcss;

    /* dummy file input */
    int fd;

    int dvd_seek_flag;
    int dvd_local_len;
    int dvd_cache_len;
    int dvd_local_offset;
    off_t dvd_local_range_start_addr;
    off_t dvd_local_range_end_addr;
    int dvd_local_range_end_blocks;
    int dvd_last_seek_blocks;
    int dvd_last_seek_block;
    int dvd_total_blocks;
    int is_vob;
};

#ifdef STREAM_DIO
extern int dio;
#endif
extern int vol_blksz;
#include "stream/stream_dvdnav.h"
extern DvdnavState   dvdnavstate;
extern int is_pause;

/**
 * initialize and open a DVD device or file.
 */
static dvd_input_t css_open(const char *target)
{
    dvd_input_t dev;

    /* Allocate the handle structure */
    dev = (dvd_input_t) malloc(sizeof(*dev));
    if(dev == NULL) {
        fprintf(stderr, "libdvdread: Could not allocate memory.\n");
        return NULL;
    }

    /* Really open it with libdvdcss */
    dev->dvdcss = DVDcss_open(target);
    if(dev->dvdcss == 0) {
        fprintf(stderr, "libdvdread: Could not open %s with libdvdcss.\n", target);
        free(dev);
        return NULL;
    }

    return dev;
}

/**
 * return the last error message
 */
static char *css_error(dvd_input_t dev)
{
    return DVDcss_error(dev->dvdcss);
}

/**
 * seek into the device.
 */
static int css_seek(dvd_input_t dev, int blocks)
{
    /* DVDINPUT_NOFLAGS should match the DVDCSS_NOFLAGS value. */
    return DVDcss_seek(dev->dvdcss, blocks, DVDINPUT_NOFLAGS);
}

/**
 * set the block for the beginning of a new title (key).
 */
static int css_title(dvd_input_t dev, int block)
{
    return DVDcss_title(dev->dvdcss, block);
}

/**
 * read data from the device.
 */
static int css_read(dvd_input_t dev, void *buffer, int blocks, int flags)
{
    return DVDcss_read(dev->dvdcss, buffer, blocks, flags);
}

/**
 * close the DVD device and clean up the library.
 */
static int css_close(dvd_input_t dev)
{
    int ret;

    ret = DVDcss_close(dev->dvdcss);

    if(ret < 0)
        return ret;

    free(dev);

    return 0;
}

/**
 * initialize and open a DVD device or file.
 */
static dvd_input_t file_open(const char *target)
{
    dvd_input_t dev;
    char local_name[256];

    /* Allocate the library structure */
    dev = (dvd_input_t) malloc(sizeof(*dev));
    if(dev == NULL) {
        fprintf(stderr, "libdvdread: Could not allocate memory.\n");
        return NULL;
    }


    dev->dvd_seek_flag = 0;
    dev->dvd_local_len=0;
    dev->dvd_cache_len=0;
    dev->dvd_local_offset=0;
    dev->dvd_local_range_start_addr=0;
    dev->dvd_local_range_end_addr=0;
    dev->dvd_local_range_end_blocks = 0;
    dev->dvd_last_seek_blocks=0;
    dev->dvd_last_seek_block=0;

    sprintf(local_name, "%s", target);

    /* Open the device */
#if !defined(WIN32) && !defined(__OS2__)
    dev->fd = open(target, O_RDONLY);
#else
    dev->fd = open(target, O_RDONLY | O_BINARY);
#endif
    if(dev->fd < 0) {
        perror("libdvdread: Could not open input");
        free(dev);
        return NULL;
    }

//#ifdef STREAM_DIO
//	if(dio){
//		int flags = fcntl(dev->fd, F_GETFL) | O_DIRECT;
//		if(fcntl(dev->fd, F_SETFL, flags) == -1){
//			printf("%s: failed to enable direct I/O\n", __FILE__);
//			dio = 0;
//		}else{
//			printf("%s: enable dio\n", __FILE__);
//		}
//	}
//#endif
    if (strncasecmp(&local_name[strlen(local_name)-3], "VOB", 3) == 0 || strncasecmp(&local_name[strlen(local_name)-3], "iso", 3) == 0)
    {
        int flags;
//        printf(" 01 local_name[strlen(local_name)-3]=%s!!\n", &local_name[strlen(local_name)-3]);
//        printf(" 01 local_name=%s!!  len=%d\n", local_name, strlen(local_name));

	if (dvd_local_buf == NULL)
	{
		dvd_local_buf1 = memalign(vol_blksz, DVD_BUFFER_SIZE);
		dvd_local_buf2 = memalign(vol_blksz, DVD_BUFFER_SIZE);
		dvd_local_buf = dvd_local_buf1;
		if ((dvd_local_buf1 == NULL) || (dvd_local_buf2 == NULL)) {
			//printf("malloc dvdnav cache buffer failed !!!\n");
		} else {
			printf("malloc dvdnav cache buffer!!!\n");
		}
	}

	if (dvd_local_buf)
	{
		dev->is_vob = 1;

		flags = fcntl(dev->fd, F_GETFL) | O_DIRECT;
		if ( fcntl(dev->fd, F_SETFL, flags) == -1)
		{
			printf("Fail to enable dio\n");
		}
		else
		{
			printf("Enable DIO !\n");
		}
	}
    }
    else
    {
        //printf("local_name=%s!\n", local_name);
        //printf(" 02 local_name[strlen(local_name)-3]=%s!!\n", &local_name[strlen(local_name)-3]);
        //printf(" 02 local_name=%s!!  len=%d\n", local_name, strlen(local_name));
        dev->is_vob = 0;

    }

    off_t pos;
    pos = lseek(dev->fd, 0, SEEK_END);
    dev->dvd_total_blocks = (int)(pos/DVD_VIDEO_LB_LEN);
    lseek(dev->fd, 0, SEEK_SET);
    file_open_cnt++;

//printf("dev->fd[%d],dvd_local_len[%d],target[%s]\n",dev->fd,dev->dvd_local_len,target);
//printf("dvd_total_blocks[%d]\n",dev->dvd_total_blocks);
    return dev;
}

/**
 * return the last error message
 */
static char *file_error(dvd_input_t dev)
{
    /* use strerror(errno)? */
    return (char *)"unknown error";
}

/**
 * seek into the device.
 */
static int file_seek(dvd_input_t dev, int blocks)
{
    extern int vol_blksz;
    off_t pos;
    int ali_blocks;
    static int tmp_fd1;
    
    if(dev->fd != tmp_fd1)
    {
	dev->dvd_cache_len = 0;
	dev->dvd_local_len = 0;
	dev->dvd_last_seek_blocks = 0;
	dev->dvd_last_seek_block = 0;
	dev->dvd_local_offset = (int)0;
    }
    tmp_fd1 = dev->fd;
    
//printf("==file_seek  fd=%d dvd_last_seek_blocks=%d cur=%d blocks=%d dvd_local_offset=%d\n", dev->fd, dev->dvd_last_seek_blocks, dev->dvd_last_seek_block, blocks, (int)dev->dvd_local_offset);
//printf("  ==> len=%d, dvd_local_range_end_blocks=%d\n", (int)dev->dvd_local_len, dev->dvd_local_range_end_blocks);
//printf("=seek,dev->fd[%d],blocks[%d]  is_vob=%d\n", dev->fd,blocks, dev->is_vob);
    if (dev->is_vob == 0)
    {
        pos = lseek(dev->fd, (off_t)blocks * (off_t)DVD_VIDEO_LB_LEN, SEEK_SET);
        if(pos < 0) {
            return pos;
        }
        return (int) (pos / DVD_VIDEO_LB_LEN);
    }

    pthread_mutex_lock(&dvdnav_mutex);
    if (dev->dvd_local_range_end_blocks != 0)
    {
/*
    if (blocks <= dev->dvd_local_range_end_blocks && blocks >= dev->dvd_last_seek_blocks)
    {
        dev->dvd_local_offset = blocks * DVD_VIDEO_LB_LEN - dev->dvd_local_range_start_addr;
        dev->dvd_local_len = dev->dvd_local_range_end_addr - blocks * DVD_VIDEO_LB_LEN;
//    dev->dvd_local_len += DVD_VIDEO_LB_LEN;
        dev->dvd_last_seek_block = blocks;
//printf("==  case 1,  new len=%d  len2=%d\n", (int)dev->dvd_local_len, (int)(dev->dvd_local_range_end_addr - dev->dvd_local_offset));
    pthread_mutex_unlock(&dvdnav_mutex);
        return blocks;
    }
    else */if ( (blocks >= dev->dvd_last_seek_blocks) &&  (blocks < dev->dvd_local_range_end_blocks))
    {
        if (dev->dvd_local_len > 0)
        {
//printf("!!! dev->dvd_local_len=%d blocks=%d dev->dvd_last_seek_blocks=%d\n", (int)dev->dvd_local_len, blocks, dev->dvd_last_seek_blocks);
//printf("!!! dev->dvd_local_len/DVD_VIDEO_LB_LEN=%d\n", (int)(dev->dvd_local_len/DVD_VIDEO_LB_LEN));
    dev->dvd_local_offset = (blocks - dev->dvd_last_seek_blocks) * DVD_VIDEO_LB_LEN;
            //dev->dvd_local_offset = blocks * DVD_VIDEO_LB_LEN - dev->dvd_local_range_start_addr;
            dev->dvd_local_len = dev->dvd_local_range_end_addr - blocks * DVD_VIDEO_LB_LEN;
            dev->dvd_last_seek_block = blocks;
//printf(" == case 2   new_len=%d\n", (int)dev->dvd_local_len);
    pthread_mutex_unlock(&dvdnav_mutex);
            return blocks;
        }
    } else if ((blocks >= dev->dvd_local_range_end_blocks) &&
		    (blocks < dev->dvd_local_range_end_blocks + dev->dvd_cache_len/DVD_VIDEO_LB_LEN)) {

//printf("blocks: %d, dvd_local_range_end_blocks: %d, dvd_cache_len: %d\n", 
//blocks, dev->dvd_local_range_end_blocks, dev->dvd_cache_len);
            dev->dvd_last_seek_block = blocks;
	    dev->dvd_local_range_start_addr += DVD_BUFFER_SIZE;
	    dev->dvd_local_len = 0;
//printf("end_addr: %lld, dvd_local_len: %d\n", dev->dvd_local_range_end_addr ,dev->dvd_local_len);
	    dev->dvd_local_range_end_addr += dev->dvd_cache_len;
	    off_t tmp_end = (off_t)dev->dvd_total_blocks * DVD_VIDEO_LB_LEN;
	    if (dev->dvd_local_range_end_addr > tmp_end)
		    dev->dvd_local_range_end_addr = tmp_end;
	    dev->dvd_local_range_end_blocks = dev->dvd_local_range_end_addr/DVD_VIDEO_LB_LEN;
	    dev->dvd_last_seek_blocks += DVD_BUFFER_SIZE/DVD_VIDEO_LB_LEN;
	    dev->dvd_local_offset = (blocks - dev->dvd_last_seek_blocks) * DVD_VIDEO_LB_LEN;
//printf("+++++++++++=dev->dvd_last_seek_blocks=%d, dvd_total_blocks=%d, dvd_local_range_end_blocks=%d\n",dev->dvd_last_seek_blocks, dev->dvd_total_blocks, dev->dvd_local_range_end_blocks);
    pthread_mutex_unlock(&dvdnav_mutex);
            return blocks;
    }
    }
    if (vol_blksz > DVD_VIDEO_LB_LEN)
    {
	if (vol_blksz % DVD_VIDEO_LB_LEN == 0)
	{
	    if (blocks % (vol_blksz / DVD_VIDEO_LB_LEN) != 0)
	    {
		//ali_blocks = ((((blocks * DVD_VIDEO_LB_LEN + vol_blksz) / vol_blksz) -1) * vol_blksz) / DVD_VIDEO_LB_LEN;
		ali_blocks = (((((off_t)blocks * (off_t)DVD_VIDEO_LB_LEN + (off_t)vol_blksz) / (off_t)vol_blksz) -1) * (off_t)vol_blksz) / (off_t)DVD_VIDEO_LB_LEN;
		dev->dvd_local_offset = (blocks - ali_blocks) * DVD_VIDEO_LB_LEN;
	    } else {
		dev->dvd_local_offset=0;
		ali_blocks = blocks;
	    }
	} else {
	    dev->dvd_local_offset=0;
	    ali_blocks = blocks;
	}
    } else {
	dev->dvd_local_offset=0;
	ali_blocks = blocks;
    }
    pos = lseek(dev->fd, (off_t)ali_blocks * (off_t)DVD_VIDEO_LB_LEN, SEEK_SET);
    if(pos < 0) {
    pthread_mutex_unlock(&dvdnav_mutex);
        return pos;
    }
//  dvd_seek_flag = 1;
//printf(" == seek to pos=%lld, start_pos=%lld, end_pos=%lld\n", pos, dev->dvd_local_range_start_addr, dev->dvd_local_range_end_addr);
    dev->dvd_local_len=0;
    dev->dvd_cache_len=0;
    dev->dvd_last_seek_blocks = ali_blocks;
    dev->dvd_last_seek_block = ali_blocks;
    dev->dvd_local_range_start_addr = pos;
    dev->dvd_local_range_end_addr = pos + DVD_BUFFER_SIZE;
    off_t tmp_end = (off_t)dev->dvd_total_blocks * DVD_VIDEO_LB_LEN;
    if (dev->dvd_local_range_end_addr > tmp_end)
        dev->dvd_local_range_end_addr = tmp_end;
    dev->dvd_local_range_end_blocks = dev->dvd_local_range_end_addr/DVD_VIDEO_LB_LEN;
//printf("+++++++++++=dev->dvd_last_seek_blocks=%d, dvd_total_blocks=%d, dvd_local_range_end_blocks=%d\n",dev->dvd_last_seek_blocks, dev->dvd_total_blocks, dev->dvd_local_range_end_blocks);
    /* assert pos % DVD_VIDEO_LB_LEN == 0 */
    pthread_mutex_unlock(&dvdnav_mutex);
    //return (int) (pos / DVD_VIDEO_LB_LEN);
    if ((pos / DVD_VIDEO_LB_LEN) != ali_blocks)
	return -1;
    return blocks;
}

/**
 * set the block for the beginning of a new title (key).
 */
static int file_title(dvd_input_t dev, int block)
{
    return -1;
}

static int dvdnav_read_buffer(void)
{
	int nRet = 0;
	unsigned char *dvd_cache_buf;
	int timewait_ret=0; //prevent deadlock
	struct timeval tp;
	struct timespec ts;
	
	while (1)
	{
		pthread_mutex_lock(&dvdnav_mutex);
//printf("-----1-dvd_local_len[%d] dvd_cache_len[%d],fd[%d]\n", thread_dev->dvd_local_len, (int)thread_dev->dvd_cache_len,thread_dev->fd);
//printf("--dvd_total_blocks[%d],dvd_last_seek_block[%d]\n",thread_dev->dvd_total_blocks, thread_dev->dvd_last_seek_block);
		if ((thread_dev->is_vob == 1) && (thread_dev->dvd_local_len < DVD_BUFFER_SIZE * 0.8) &&
				(thread_dev->dvd_cache_len == 0) &&
				(thread_dev->dvd_total_blocks != thread_dev->dvd_last_seek_block) ||
				timewait_ret != 0)
		{
			//printf("--thread fd: %d\n", thread_dev->fd);
			//printf("--@@@ dvd local len=%d\n", (int)thread_dev->dvd_local_len);
			timewait_ret = 0;
			if (dvd_local_buf == dvd_local_buf1)
			{
				dvd_cache_buf = dvd_local_buf2;
				//printf("cache buf2\n");
			} else  {
				dvd_cache_buf = dvd_local_buf1;
				//printf("cache buf1\n");
			}

			thread_dev->dvd_cache_len = read(thread_dev->fd, dvd_cache_buf, DVD_BUFFER_SIZE);
			//printf("--#### read ret = %d\n", (int)thread_dev->dvd_cache_len);
			if (thread_dev->dvd_cache_len < 0)
			{
				perror("read error: ");
			}
			pthread_cond_signal(&read_cond);
		}
//printf("wait------write-------->\n");
//		printf("-----2-dvd_local_len[%d] dvd_cache_len[%d]\n", thread_dev->dvd_local_len, thread_dev->dvd_cache_len);
//printf("--dvd_total_blocks[%d],dvd_last_seek_block[%d]\n",thread_dev->dvd_total_blocks, thread_dev->dvd_last_seek_block);

		if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU || is_pause)
			pthread_cond_wait(&write_cond, &dvdnav_mutex);
		else
		{
			do
			{
			    gettimeofday(&tp, NULL);
			    ts.tv_sec  = tp.tv_sec;
			    ts.tv_nsec = tp.tv_usec * 1000;
			    ts.tv_sec += 10;

			    timewait_ret = pthread_cond_timedwait(&write_cond, &dvdnav_mutex, &ts);

                if(timewait_ret)
                {
                    printf("### DVDNAV: In [%s][%d] cond wait time out!! is_pause[%d]###\n", __func__, __LINE__,is_pause);
                }
			}while(is_pause);
		}
//printf("wait<------write--------\n");
		pthread_mutex_unlock(&dvdnav_mutex);
	}
	return nRet;
}

/**
 * read data from the device.
 */
static int file_read(dvd_input_t dev, void *buffer, int blocks, int flags)
{
    size_t len;
    ssize_t ret;
    static int tmpfd;

    len = (size_t)blocks * DVD_VIDEO_LB_LEN;
//printf("dvd_local_len=%d blocks=%d,blocks=%d,len=%d\n", dev->dvd_local_len, blocks,len,len);
//printf("dvd_local_len=%d blocks=%d\n", dev->dvd_local_len, (size_t)blocks);
//printf("len=%d-",len);
//printf("=file_read()=tmpfd[%d],dev->fd[%d]\n",tmpfd, dev->fd);

    if (dev->is_vob == 0)
    {
        while(len > 0) {

            ret = read(dev->fd, buffer, len);

            if(ret < 0) {
                /* One of the reads failed, too bad.  We won't even bother
                       * returning the reads that went OK, and as in the POSIX spec
                       * the file position is left unspecified after a failure. */
                return ret;
            }

            if(ret == 0) {
                /* Nothing more to read.  Return all of the whole blocks, if any.
                 * Adjust the file position back to the previous block boundary. */
                size_t bytes = (size_t)blocks * DVD_VIDEO_LB_LEN - len;
                off_t over_read = -(bytes % DVD_VIDEO_LB_LEN);
                /*off_t pos =*/
                lseek(dev->fd, over_read, SEEK_CUR);
                /* should have pos % 2048 == 0 */
                return (int) (bytes / DVD_VIDEO_LB_LEN);
            }

            len -= ret;
        }

        return blocks;
//========
    }
//printf(" -- read dev->dvd_local_len=%d\n", dev->dvd_local_len);

    thread_dev = dev;
    if (dvdnav_thread_running == 0)
    {
	    //printf("create pthread !!!\n");
	    //printf("fd: %d\n", thread_dev->fd);
	    pthread_mutex_init(&dvdnav_mutex,NULL);
	    pthread_cond_init(&read_cond,NULL);
	    pthread_cond_init(&write_cond,NULL);
	    pthread_create(&dvdnav_read_thread, NULL, dvdnav_read_buffer, NULL);
	    dvdnav_thread_running = 1;
    }

    while(len > 0) {
	pthread_mutex_lock(&dvdnav_mutex);
//printf("blocks: %d %d len: %d %d\n", dev->dvd_total_blocks, dev->dvd_last_seek_block, (int)dev->dvd_local_len, (int)dev->dvd_cache_len);
	if ((dev->dvd_local_len < DVD_BUFFER_SIZE * 0.8) &&
				(dev->dvd_cache_len == 0) &&
				(dev->dvd_total_blocks != thread_dev->dvd_last_seek_block))
	{
		pthread_cond_signal(&write_cond);
	}
        if ( ((dev->dvd_local_len == 0) && ( dev->dvd_cache_len == 0)) || (tmpfd !=dev->fd) )
	{
//printf("=tmpfd[%d],dev->fd[%d]\n",tmpfd, dev->fd);
		/*
		if (tmpfd !=dev->fd) {
			printf("change to fd %d\n", dev->fd);
		} else {
			printf("@@@ cache empty !!! %d, local_len: %d, cache_len: %d,tmpfd[%d]\n", dev->fd,
					thread_dev->dvd_local_len, thread_dev->dvd_cache_len,tmpfd);
		}
		*/
//printf("wait-------read------->\n");
//printf("===3-dvd_local_len[%d] dvd_cache_len[%d]\n", thread_dev->dvd_local_len, thread_dev->dvd_cache_len);
//printf("dvd_total_blocks[%d],dvd_last_seek_block[%d]\n",thread_dev->dvd_total_blocks, thread_dev->dvd_last_seek_block);
		pthread_cond_signal(&write_cond);
		pthread_cond_wait(&read_cond, &dvdnav_mutex);
//printf("wait<------read--------\n");
	}
	pthread_mutex_unlock(&dvdnav_mutex);

        if (dev->dvd_local_len < len || tmpfd !=dev->fd )
        {
            int last_len = dev->dvd_local_len;
            if (dev->dvd_local_len > 0 && tmpfd == dev->fd)
            {
                memcpy(buffer, &dvd_local_buf[dev->dvd_local_offset], dev->dvd_local_len);
		dev->dvd_local_offset = 0;
            }
            tmpfd = dev->fd;

            //dev->dvd_local_len = read(dev->fd, dvd_local_buf, DVD_BUFFER_SIZE);
	    pthread_mutex_lock(&dvdnav_mutex);
            dev->dvd_local_len = dev->dvd_cache_len;
	    if (dvd_local_buf == dvd_local_buf1)
		{
		dvd_local_buf = dvd_local_buf2;
//printf("switch to buf2\n");
	    } else {
		dvd_local_buf = dvd_local_buf1;
//printf("switch to buf1\n");
		}
	    dev->dvd_cache_len = 0;
//printf("dvd_local_range_end_blocks: %d\n", dev->dvd_local_range_end_blocks);

	    pthread_mutex_unlock(&dvdnav_mutex);
//printf("#### len ret = %d %d\n", dev->dvd_local_len, dev->dvd_cache_len);
            if (dev->dvd_local_len <= 0)
            {
                ret = dev->dvd_local_len;
            }
            else
            {
		if (dev->dvd_local_len != DVD_BUFFER_SIZE)
		{
			//printf("cache read not enough only %d, shoult be %d!!!\n", dev->dvd_local_len, DVD_BUFFER_SIZE);
			dev->dvd_local_range_end_blocks = dev->dvd_last_seek_blocks + (dev->dvd_local_len/DVD_VIDEO_LB_LEN);
			//printf("Update dvd_local_range_end_blocks: %d\n", dev->dvd_local_range_end_blocks);
		}
                if (dev->dvd_local_len > (len - last_len))
                {
                    memcpy(&buffer[last_len], &dvd_local_buf[dev->dvd_local_offset], len - last_len);
//            memcpy(buffer, &dvd_local_buf[0], len - last_len);
                    dev->dvd_local_offset = len - last_len;
                    dev->dvd_local_len -= len - last_len;
                }
                ret = len;
            }
//printf("@@@ got new dvd_local_offset=%d dvd_local_len=%d ret=%d\n", dev->dvd_local_offset, dev->dvd_local_len,  ret);
        }
        else
        {
//printf("@@@ len=%d %d %d\n", len, dev->dvd_local_len, thread_dev->dvd_local_len);
//printf("offset : %d\n", dev->dvd_local_offset);
            tmpfd = dev->fd;
            memcpy(buffer, &dvd_local_buf[dev->dvd_local_offset], len);
            dev->dvd_local_len -= len;
            dev->dvd_local_offset += len;
            ret = len;
//printf("******   len=%d  new _len=%d\n", len, dev->dvd_local_len);
            dev->dvd_last_seek_block += len/DVD_VIDEO_LB_LEN;

        }
//    ret = read(dev->fd, buffer, len);

        if(ret < 0) {
            /* One of the reads failed, too bad.  We won't even bother
             * returning the reads that went OK, and as in the POSIX spec
             * the file position is left unspecified after a failure. */
            return ret;
        }

        if(ret == 0) {
            /* Nothing more to read.  Return all of the whole blocks, if any.
             * Adjust the file position back to the previous block boundary. */
            size_t bytes = (size_t)blocks * DVD_VIDEO_LB_LEN - len;
            off_t over_read = -(bytes % DVD_VIDEO_LB_LEN);
            /*off_t pos =*/
            lseek(dev->fd, over_read, SEEK_CUR);
            /* should have pos % 2048 == 0 */
            return (int) (bytes / DVD_VIDEO_LB_LEN);
        }

        len -= ret;
    }

    return blocks;
}

/**
 * close the DVD device and clean up.
 */
static int file_close(dvd_input_t dev)
{
    int ret;

    //printf("close fd: %d\n", dev->fd);
    ret = close(dev->fd);
    dev->fd = -1;

    if(ret < 0)
        return ret;
    file_open_cnt--;
    //printf("finish close file, remain: %d !!!\n", file_open_cnt);
    if (file_open_cnt == 0)
    {
	    if (dvdnav_thread_running == 1)
	    {
		    pthread_cancel(dvdnav_read_thread);
		    //printf("join pthread begian !!!\n");
		    if (pthread_join(dvdnav_read_thread, NULL))
			    printf("error join dvdnav thread\n");
		    else { 
			    dvdnav_thread_running = 0;
			    pthread_cond_destroy(&read_cond);
			    pthread_cond_destroy(&write_cond);
			    pthread_mutex_destroy(&dvdnav_mutex);
			    //printf("join pthread finish !!!\n");
		    }
	    }

	    if (dvd_local_buf1)
		    free(dvd_local_buf1);
	    if (dvd_local_buf2)
		    free(dvd_local_buf2);
	    dvd_local_buf = NULL;
	    //printf("Close read thread buffer!!!\n");
    }

    free(dev);

    return 0;
}


/**
 * Setup read functions with either libdvdcss or minimal DVD access.
 */
int dvdinput_setup(void)
{
    void *dvdcss_library = NULL;
    char **dvdcss_version = NULL;

#ifdef HAVE_DVDCSS_DVDCSS_H
    /* linking to libdvdcss */
    dvdcss_library = &dvdcss_library;  /* Give it some value != NULL */
    /* the DVDcss_* functions have been #defined at the top */
    dvdcss_version = &dvdcss_interface_2;

#else
    /* dlopening libdvdcss */

#ifdef __APPLE__
#define CSS_LIB "libdvdcss.2.dylib"
#elif defined(WIN32)
#define CSS_LIB "libdvdcss.dll"
#elif defined(__OS2__)
#define CSS_LIB "dvdcss.dll"
#else
#define CSS_LIB "libdvdcss.so.2"
#endif
    dvdcss_library = dlopen(CSS_LIB, RTLD_LAZY);

    if(dvdcss_library != NULL) {
#if defined(__OpenBSD__) && !defined(__ELF__) || defined(__OS2__)
#define U_S "_"
#else
#define U_S
#endif
        DVDcss_open = (dvdcss_handle (*)(const char*))
                      dlsym(dvdcss_library, U_S "dvdcss_open");
        DVDcss_close = (int (*)(dvdcss_handle))
                       dlsym(dvdcss_library, U_S "dvdcss_close");
        DVDcss_title = (int (*)(dvdcss_handle, int))
                       dlsym(dvdcss_library, U_S "dvdcss_title");
        DVDcss_seek = (int (*)(dvdcss_handle, int, int))
                      dlsym(dvdcss_library, U_S "dvdcss_seek");
        DVDcss_read = (int (*)(dvdcss_handle, void*, int, int))
                      dlsym(dvdcss_library, U_S "dvdcss_read");
        DVDcss_error = (char* (*)(dvdcss_handle))
                       dlsym(dvdcss_library, U_S "dvdcss_error");

        dvdcss_version = (char **)dlsym(dvdcss_library, U_S "dvdcss_interface_2");

        if(dlsym(dvdcss_library, U_S "dvdcss_crack")) {
            fprintf(stderr,
                    "libdvdread: Old (pre-0.0.2) version of libdvdcss found.\n"
                    "libdvdread: You should get the latest version from "
                    "http://www.videolan.org/\n" );
            dlclose(dvdcss_library);
            dvdcss_library = NULL;
        } else if(!DVDcss_open  || !DVDcss_close || !DVDcss_title || !DVDcss_seek
                  || !DVDcss_read || !DVDcss_error || !dvdcss_version) {
            fprintf(stderr,  "libdvdread: Missing symbols in %s, "
                    "this shouldn't happen !\n", CSS_LIB);
            dlclose(dvdcss_library);
        }
    }
#endif /* HAVE_DVDCSS_DVDCSS_H */

    if(dvdcss_library != NULL) {
        /*
        char *psz_method = getenv( "DVDCSS_METHOD" );
        char *psz_verbose = getenv( "DVDCSS_VERBOSE" );
        fprintf(stderr, "DVDCSS_METHOD %s\n", psz_method);
        fprintf(stderr, "DVDCSS_VERBOSE %s\n", psz_verbose);
        */
        fprintf(stderr, "libdvdread: Using libdvdcss version %s for DVD access\n",
                dvdcss_version ? *dvdcss_version : "");

        /* libdvdcss wrapper functions */
        dvdinput_open  = css_open;
        dvdinput_close = css_close;
        dvdinput_seek  = css_seek;
        dvdinput_title = css_title;
        dvdinput_read  = css_read;
        dvdinput_error = css_error;
        return 1;

    } else {
        fprintf(stderr, "libdvdread: Encrypted DVD support unavailable.\n");

        /* libdvdcss replacement functions */
        dvdinput_open  = file_open;
        dvdinput_close = file_close;
        dvdinput_seek  = file_seek;
        dvdinput_title = file_title;
        dvdinput_read  = file_read;
        dvdinput_error = file_error;
        return 0;
    }
}
