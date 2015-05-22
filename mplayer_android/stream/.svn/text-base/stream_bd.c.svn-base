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

#include "udf.h"
#include "libbluray/bluray.h"
#include "libbluray/bdnav/mpls_parse.h"
#include "libmpdemux/demux_ts.h" /* for bdiso_debug message */

//#define SHOW_BD_INFO

//#define ENABLE_CHECK_BD_INFO // Automatic choosed bd title if user do not input title number
#ifdef ENABLE_CHECK_BD_INFO
//#define DEBUG_ENABLE_CHECK_BD_INFO
static MPLS_PL * bd_title_filter(BLURAY *bd, uint32_t count, BLURAY_TITLE_MPLS_ID *title, int is_dir, char *filepath, int *title_id, int *mpls_id);
static int bd_get_chapters(MPLS_PL *def_pl);
static uint32_t _pl_duration(MPLS_PL *pl);
#endif /* end of ENABLE_CHECK_BD_INFO */

#define ROUND_UP(x, y)		((x + y - 1) & ~(y - 1))	// y must be power of 2
#define ROUND_DOWN(x, y)	(x & ~(y - 1))
#define IS_MULT(x, y)		((x & (y - 1)) == 0)
#define REMAINDER(x, y)		(x & (y - 1))

#define IS_KERNADDR(addr)	((unsigned long)(addr) >= 0xC0000000)
#define KERN2VIRT(addr)		((unsigned long)(addr) - ((unsigned long)SallocPhysicalBase | (unsigned long)0xC0000000) + (unsigned long)SallocVirtualBase)

extern int vol_blksz;
int bluray_title = 0;
int bluray_m2ts = 0;
#ifdef BD_CLIP_BASE_PTS
double *list_base_time[3];
double bd_now_base_pts = 0.0;
#endif

static struct stream_priv_s {
	int m2ts;
	char *device;
} stream_priv_dflts = {
	0, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
	{"hostname", ST_OFF(m2ts), CONF_TYPE_INT, M_OPT_RANGE, 0 , 99999, NULL},
	{"filename", ST_OFF(device), CONF_TYPE_STRING, 0, 0 ,0, NULL},
	{ NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
	"bd",
	sizeof(struct stream_priv_s),
	&stream_priv_dflts,
	stream_opts_fields
};

void print_segments(UDF_FILE_BLOCK *segments)
{
	int i;
	printf("segment count:%d total_length:%llx\n", segments->count, segments->total_length);
	for(i=0; i < segments->count; i++){
		UDF_BLOCK *block = &segments->block[i];
		printf("  segment[%d] len:%llx lbs:%x ofs:%llx\n", i, block->Length, block->Location, (uint64_t)block->Location * DVD_VIDEO_LB_LEN);
	}
}

void print_title_info(BLURAY_TITLE_INFO *ti)
{
	int i;
	printf("title idx %d playlist %d duration %lld clip %d angle %d chapter %d\n", ti->idx, ti->playlist, ti->duration/90000, ti->clip_count, ti->angle_count, ti->chapter_count);
	for(i=0; i < ti->clip_count; i++){
		BLURAY_CLIP_INFO *clip = &ti->clips[i];
		printf("clip[%d] video %d audio %d pg %d ig %d sec_audio %d sec_video %d\n", i, clip->video_stream_count, clip->audio_stream_count, clip->pg_stream_count, clip->ig_stream_count, clip->sec_audio_stream_count, clip->sec_video_stream_count);
	}
	for(i=0; i < ti->chapter_count; i++){
		BLURAY_TITLE_CHAPTER *chap = &ti->chapters[i];
		printf("chapter[%d] idx %d start %llx offset %llx duration %lld\n", i, chap->idx, chap->start, chap->offset, chap->duration/90000);
	}
}
off_t bd_get_current_file_position(stream_t *s)
{
	struct bd_priv_t *bdpriv = s->priv;
	//printf("%s: %llx\n", __func__, s->bd_ofs + bdpriv->clip_pos);
	return s->bd_ofs + bdpriv->clip_pos;
}

static int bd_get_chapter_idx(stream_t *s, off_t pos)
{
	int i;
	struct bd_priv_t *bdpriv = s->priv;
	BLURAY_TITLE_INFO *ti = bdpriv->title_info;

	if(pos >= s->end_pos){
		printf("%s:%d Error: pos %llx exceeds end of title %llx\n", __func__, __LINE__, pos, s->end_pos);
		return -1;
	}
	
	for(i=0; i < ti->chapter_count; i++){
		BLURAY_TITLE_CHAPTER *chap = &ti->chapters[i];
		if(pos <= chap->offset) break;
	}

	//printf("%s:%d pos %llx located in chapter %d\n", __func__, __LINE__, pos, i-1);
	return i - 1;
}

static off_t bd_get_chapter_offset(stream_t *s, int idx)
{
	struct bd_priv_t *bdpriv = s->priv;
	BLURAY_TITLE_INFO *ti = bdpriv->title_info;

	if(idx >= ti->chapter_count){
		printf("%s:%d Error: chapter index %d exceeds chapter count %d\n", __func__, __LINE__, idx, ti->chapter_count);
		return -1;
	}

	//printf("%s:%d chapter %d offset %llx\n", __func__, __LINE__, idx, ti->chapters[idx].offset);
	return ti->chapters[idx].offset;
}

static int bd_get_clip_idx(stream_t *s, off_t *pos)
{
	int i;
	off_t title_pos = *pos;
	struct bd_priv_t *bdpriv = s->priv;
	MPLS_PL *pl = bdpriv->play_list;

	if(*pos >= s->end_pos){
		printf("%s:%d Error: pos %llx exceeds end of title %llx\n", __func__, __LINE__, *pos, s->end_pos);
		return -1;
	}
	
	for(i=0; i < pl->list_count; i++){
		if(*pos < bdpriv->clip_list[i]) break;
		*pos -= bdpriv->clip_list[i];
	}

	if(i >= pl->list_count){
		printf("%s:%d Error: can't find position %llx in title\n", __func__, __LINE__, title_pos);
		return -1;
	}
	//printf("%s:%d title pos %llx located in clip[%d] offset %llx\n", __func__, __LINE__, title_pos, i, *pos);
	return i;
}

static int bd_get_block_idx(stream_t *s, off_t *pos)
{
	int i;
	off_t clip_pos = *pos;
	struct bd_priv_t *bdpriv = s->priv;
	UDF_FILE_BLOCK *segments = bdpriv->segments;

	if(bdpriv->iso == 0){
		printf("%s:%d Error: not iso image\n", __func__, __LINE__);
		return -1;
	}
	
	if(segments == NULL){
		printf("%s:%d Error: no blocks info\n", __func__, __LINE__);
		return -1;
	}

	if(*pos >= segments->total_length){
		printf("%s:%d Error: pos %llx exceeds end of clip %llx\n", __func__, __LINE__, *pos, segments->total_length);
		return -1;
	}

	for(i=0; i < segments->count; i++){
		if(*pos < segments->block[i].Length) break;
		*pos -= segments->block[i].Length;
	}

	if(i >= segments->count){
		printf("%s:%d Error: pos %llx not in segments\n", __func__, __LINE__, clip_pos);
		return -1;
	}
	
	//printf("%s:%d clip pos %llx located in blks[%d] offset %llx\n", __func__, __LINE__, clip_pos, i, *pos);
	return i;
}

static int bd_segment_switch(stream_t *s, off_t *pos)
{
	int i;
	off_t clip_pos = *pos;
	struct bd_priv_t *bdpriv = s->priv;
	UDF_FILE_BLOCK *segments = bdpriv->segments;

	if(bdpriv->iso == 0){
		printf("%s:%d Error: not iso image\n", __func__, __LINE__);
		return -1;
	}
	
	if(segments == NULL){
		printf("%s:%d Error: no blocks info\n", __func__, __LINE__);
		return -1;
	}

	if(*pos >= segments->total_length){
		printf("%s:%d Error: pos %llx exceeds end of clip %llx\n", __func__, __LINE__, *pos, segments->total_length);
		return -1;
	}

	for(i=0; i < segments->count; i++){
		if(*pos < segments->block[i].Length) break;
		*pos -= segments->block[i].Length;
	}

	if(i >= segments->count){
		printf("%s:%d Error: pos %llx not in segments\n", __func__, __LINE__, clip_pos);
		return -1;
	}
	
	bdpriv->segment_idx = i;
	bdpriv->segment_pos = *pos;
	bdpriv->segment_len = segments->block[i].Length;
	s->bd_ofs = (off_t)segments->block[i].Location * DVD_VIDEO_LB_LEN;
	//printf("%s:%d clip pos %llx is %llxth byte in segments[%d] len:%llx lbs:%x ofs:%llx\n", __func__, __LINE__, clip_pos, bdpriv->segment_pos, i, bdpriv->segment_len, segments->block[i].Location, s->bd_ofs);
	return 0;
}

static int open_file_dio(char *fn)
{
	int f;

	f = open(fn, O_RDONLY);
	if(f<0){
		printf("%s:%d failed to open file %s (%s)\n", __func__, __LINE__, fn, strerror(errno));
		return f;
	}

#ifdef STREAM_DIO
	if(dio){
		int flags = fcntl(f, F_GETFL) | O_DIRECT;
		if(fcntl(f, F_SETFL, flags) == -1){
			printf("%s:%d failed to enable direct I/O\n", __func__, __LINE__);
			dio = 0;
		}else{
			printf("%s:%d enable dio\n", __func__, __LINE__);
		}
	}
#endif

	return f;
}

static int file_set_dio(int f)
{
	int flags;
	
	if(!dio) return 0;

	flags = fcntl(f, F_GETFL) | O_DIRECT;
	
	if(fcntl(f, F_SETFL, flags) == -1){
		printf("%s:%d failed to set direct I/O\n", __func__, __LINE__);
		return 1;
	}

	return 0;
}

static int file_clr_dio(int f)
{
	int flags;
	
	if(!dio) return 0;

	flags = fcntl(f, F_GETFL) & ~O_DIRECT;
	
	if(fcntl(f, F_SETFL, flags) == -1){
		printf("%s:%d failed to clear direct I/O\n", __func__, __LINE__);
		return 1;
	}

	return 0;
}

static int has_dio(int f)
{
	return fcntl(f, F_GETFL) & O_DIRECT;
}

int bd_m2ts_open(stream_t *s, int clip_idx)
{
	//int i;
	uint64_t len;
	unsigned char fstream[1024];
	struct bd_priv_t *bdpriv = (struct bd_priv_t *)s->priv;
	MPLS_PL *pl = bdpriv->play_list;
	UDF_DATA udf_data = { s->fd, 0, NULL };
	UDF_FILE udf_info = { &udf_data, 0, 0, 0, 0 };

	if(bdpriv->iso){
		snprintf(fstream, 1024, "/BDMV/STREAM/%5s.m2ts", pl->play_item[clip_idx].clip->clip_id);
		if(bdpriv->segments)
			UDFFreeFileBlock(bdpriv->segments);
		bdiso_debug("@@@@@ In [%s][%d] switch_file [%s] @@@@@\n", __func__, __LINE__, fstream);

		if(file_clr_dio(s->fd) != 0) return 1;
		bdpriv->segments = BDUDFFindFileBlock(&udf_info, fstream);
		if(file_set_dio(s->fd) != 0) return 1;
		if(bdpriv->segments == NULL){
			printf("%s:%d failed to find %s in iso image\n", __func__, __LINE__, fstream);
			return 1;
		}

		//print_segments(bdpriv->segments);

		len = bdpriv->segments->total_length;
		bdpriv->segment_idx = 0;
		bdpriv->segment_pos = 0;
		bdpriv->segment_len = bdpriv->segments->block[0].Length;
		s->bd_ofs = (off_t)bdpriv->segments->block[0].Location * DVD_VIDEO_LB_LEN;
		lseek(s->fd, s->bd_ofs, SEEK_SET);	// seek to start of xxxxx.m2ts
		if(!IS_MULT(s->bd_ofs, vol_blksz))
			printf("%s:%d Warning: bd_ofs %llx is not multiple of vol_blksz %x\n", __func__, __LINE__, s->bd_ofs, vol_blksz);
		//printf("%s:%d open clip[%d] in iso fn:%s lbs:%x ofs:%llx len:%llx segments:%d\n", __func__, __LINE__, clip_idx, fstream, bdpriv->segments->block[0].Location, s->bd_ofs, len, bdpriv->segments->count);
	}else{
		struct stat fst;
		if(s->fd >= 0) close(s->fd);
		snprintf(fstream, 1024, "%s/BDMV/STREAM/%5s.m2ts", bdpriv->device, pl->play_item[clip_idx].clip->clip_id);
		s->fd = open_file_dio(fstream);
		bdiso_debug("@@@@@ In [%s][%d] switch_file [%s] @@@@@\n", __func__, __LINE__, fstream);
		if(s->fd < 0){
			//printf("%s: failed to open file %s\n", __func__, fstream);
			return 1;
		}

		if(fstat(s->fd, &fst) == -1){
			printf("%s:%d error on getting file status %s (%s)\n", __func__, __LINE__, fstream, strerror(errno));
			return 1;
		}
		len = fst.st_size;
	}

	bdpriv->clip_idx = clip_idx;
	bdpriv->clip_len = len;
	bdpriv->clip_pos = 0;

	//printf("%s:%d clip[%d] len %llx\n", __func__, __LINE__, bdpriv->clip_idx, bdpriv->clip_len);
	return 0;
}

static int fill_buffer(stream_t *s, char* buffer, int max_len)
{
	int r;
	int read_size = max_len;
	off_t remain;
	//off_t pos = s->pos;
	struct bd_priv_t *bdpriv = s->priv;
	MPLS_PL *pl = bdpriv->play_list;
	//int idx = bdpriv->clip_idx;

	/* get remaining size in the clip */
	remain = bdpriv->clip_len - bdpriv->clip_pos;
	//printf("%s:%d clip_len %llx clip_pos %llx remain %llx\n", __func__, __LINE__, bdpriv->clip_len, bdpriv->clip_pos, remain);
	if(remain <= 0){
		bdpriv->clip_idx++;

		/* no more clips */
		if(bdpriv->clip_idx >= pl->list_count)
		{
			printf("@@@@@ In [%s][%d] bdpriv->clip_idx[%d] pl->list_count [%d] @@@@@\n", __func__, __LINE__, bdpriv->clip_idx, pl->list_count);
			return 0;
		}
		
		/* open next clip */
		if(bd_m2ts_open(s, bdpriv->clip_idx) != 0)
		{
			return 0;
		}

#ifdef STREAM_AIO
		if(aio) s->buf_len = s->buf_pos = 0; // reset buf_len for aio reinit
#endif
		remain = bdpriv->clip_list[bdpriv->clip_idx];

		bdiso_debug("$$$$ In [%s][%d] bdpriv->clip_idx is [%d] bd_now_base_pts is [%f] $$$$\n", __func__, __LINE__, bdpriv->clip_idx, bd_now_base_pts, bdpriv->clip_idx > 0 ? list_base_time[bdpriv->clip_idx-1]: list_base_time[0]);
	}

	if(bdpriv->iso){	
		remain = bdpriv->segment_len - bdpriv->segment_pos;
		if(remain <= 0){
			bdpriv->segment_idx++;

			/* no more continuous blocks */
			if(bdpriv->segment_idx >= bdpriv->segments->count)
			{
				printf("@@@@@ In [%s][%d] bdpriv->segment_idx[%d] bdpriv->segments->count[%d] @@@@@\n", __func__, __LINE__, bdpriv->segment_idx, bdpriv->segments->count);
				return 0;
			}

			bdpriv->segment_pos = 0;
			bdpriv->segment_len = bdpriv->segments->block[bdpriv->segment_idx].Length;
			s->bd_ofs = (off_t)bdpriv->segments->block[bdpriv->segment_idx].Location * DVD_VIDEO_LB_LEN;
			lseek(s->fd, s->bd_ofs, SEEK_SET);	// seek to start of xxxxx.m2ts
			if(!IS_MULT(s->bd_ofs, vol_blksz))
				printf("%s:%d Warning: bd_ofs %llx is not multiple of vol_blksz %x\n", __func__, __LINE__, s->bd_ofs, vol_blksz);
			remain = bdpriv->segment_len;
			//printf("%s:%d change to next continuous segment[%d] len:%llx ofs:%llx\n", __func__, __LINE__, bdpriv->segment_idx, bdpriv->segment_len, s->bd_ofs);
#ifdef STREAM_AIO
			if(aio) s->buf_len = s->buf_pos = 0; // reset buf_len for aio reinit
#endif
		}
	}
#ifdef BD_CLIP_BASE_PTS
	if(bdpriv->clip_idx == 0 || (bdpriv->timebar_type == BDISO_TIMEBRR_TYPE_CON_TIME))
		bd_now_base_pts = 0.0;
	else
		bd_now_base_pts = list_base_time[BD_ISO_IN_TIME][bdpriv->clip_idx-1];
#endif

	//printf("%s:%d read_size %x remain %llx\n", __func__, __LINE__, read_size, remain);
	if(read_size > remain)
		read_size = remain;
	
#ifdef STREAM_AIO
	if(aio)
		r = aio_fill_buffer(s);
	else
#endif
	{
#if 0
		off_t curpos = lseek(s->fd, 0, SEEK_CUR);
		off_t v = REMAINDER(curpos, vol_blksz);
		if(v != 0){
			printf("current position:%llx is not aligned to vol_blksz:%x\n", curpos, vol_blksz);
			if(file_clr_dio(s->fd) != 0) return 0;
			if((unsigned long)buffer & 0xC0000000) // kernel addr
				r = read(s->fd, buffer - ((unsigned long)SallocPhysicalBase | (unsigned long)0xC0000000) + (unsigned long)SallocVirtualBase, vol_blksz-v);
			else
				r = read(s->fd, buffer, vol_blksz-v);
			if(file_set_dio(s->fd) != 0) return 0;
			if(r <= 0)
				printf("nodio read error! curpos:%llx buffer:%x size:%llx r:%d errno:%d\n", curpos, buffer, (off_t)vol_blksz-v, r, errno);
		}else{
			if(!IS_MULT(read_size, vol_blksz)) printf("read_size:%x is not multiple of volblksz:%x\n", read_size, vol_blksz);
			r = read(s->fd, buffer, ROUND_UP(read_size, vol_blksz));
			if(r > read_size) r = read_size;
			if(r <= 0)
				printf("read error! curpos:%llx buffer:%x size:%x r:%d errno:%d\n", curpos, buffer, ROUND_UP(read_size, vol_blksz), r, errno);
		}
#else
		off_t curpos = lseek(s->fd, 0, SEEK_CUR);
		off_t alignpos = ROUND_DOWN(curpos, vol_blksz);
		if(!IS_MULT(curpos, vol_blksz)){
			unsigned int back = REMAINDER(curpos, vol_blksz);
			unsigned int need = vol_blksz - back;
			if(need < read_size){
				//printf("curpos:%llx alignpos:%llx need:%x read_size:%x\n", curpos, alignpos, need, read_size);
				read_size = need;
			}
			lseek(s->fd, ROUND_DOWN(curpos, vol_blksz), SEEK_SET);

			if(!IS_MULT(read_size, vol_blksz)) printf("%s:%d read_size:%x is not multiple of volblksz:%x\n", __func__, __LINE__, read_size, vol_blksz);
			r = read(s->fd, buffer, ROUND_UP(read_size, vol_blksz));
			if(r <= 0){
				printf("read error! curpos:%llx buffer:%x size:%x r:%d errno:%d\n", curpos, buffer, ROUND_UP(read_size, vol_blksz), r, errno);
			}else{
				//printf("using %s addr:%x\n", IS_KERNADDR(buffer) ? "kernel" : "virtual", buffer);
				if(IS_KERNADDR(buffer)){ // kernel addr
					//printf("move %x bytes from %x=>%x to %x=>%x\n", read_size, buffer + back, KERN2VIRT(buffer + back), buffer, KERN2VIRT(buffer));
					memmove(KERN2VIRT(buffer), KERN2VIRT(buffer + back), read_size);
				}else{
					//printf("move %x bytes from %x to %x\n", read_size, buffer + back, buffer);
					memmove(buffer, buffer + back, read_size);
				}

				r -= back;
			}
		}else{
			if(!IS_MULT(read_size, vol_blksz)) printf("%s:%d read_size:%x is not multiple of volblksz:%x\n", __func__, __LINE__, read_size, vol_blksz);
			r = read(s->fd, buffer, ROUND_UP(read_size, vol_blksz));
			if(r > read_size){
				r = read_size; // only read_size bytes are required
			}else if(r <= 0){
				printf("read error! curpos:%llx buffer:%x size:%x r:%d errno:%d\n", curpos, buffer, ROUND_UP(read_size, vol_blksz), r, errno);
			}
		}
#endif
	}

	if(r <= 0) return 0;
	bdpriv->clip_pos += r;
	bdpriv->segment_pos += r;
	
	return r;
}

static int seek(stream_t *s, off_t newpos)
{
	int idx;
	off_t back;
	off_t oripos = s->pos;
	off_t pos = newpos;
	struct bd_priv_t *bdpriv = s->priv;
	MPLS_PL *pl = NULL;

	if(bdpriv == NULL){
		printf("%s:%d bdpriv is null\n", __FILE__, __LINE__);
		return 0;
	}

	pl = bdpriv->play_list;

	if(newpos >= s->end_pos){
		printf("%s:%d newpos %llx >= end_pos %llx\n", __FILE__, __LINE__, newpos, s->end_pos);
		s->eof = 1;
		return 0;
	}

	idx = bd_get_clip_idx(s, &pos);
	if(idx < 0){
		printf("%s:%d incorrect clip offset for newpos %llx\n", __FILE__, __LINE__, newpos);
		return 0;
	}

	//printf("%s:%d new title pos %llx is %llxth byte in clip[%d]=%5s\n", __func__, __LINE__, newpos, pos, idx, pl->play_item[idx].clip->clip_id);
	if(idx != bdpriv->clip_idx){ // need to open another clip
		//printf("clip_idx changed %d => %d\n", bdpriv->clip_idx, idx);
		if(bd_m2ts_open(s, idx) != 0)
			return 0;
	}

	bdpriv->clip_pos = pos;
	
	if(bdpriv->iso){
		if(bd_segment_switch(s, &pos) != 0){
			return 0;
		}
		pos += s->bd_ofs;
	}

	s->pos = newpos;

	/*
	 * pos may not be aligned to vol_blksz, which will cause read operation fail.
	 * adjusted pos won't be exactly the same to newpos.
	 */
	if(!IS_MULT(pos, vol_blksz))
		printf("%s:%d Warning: pos %llx is not multiple of vol_blksz %x\n", __func__, __LINE__, pos, vol_blksz);

	if(lseek(s->fd, ROUND_DOWN(pos, vol_blksz), SEEK_SET) < 0){
		printf("%s:%d seek failed new pos %llx, ofs %llx, adj pos %llx\n", __FILE__, __LINE__, newpos, s->bd_ofs, pos);
		s->eof = 1;
		return 0;
	}
	
	back = REMAINDER(pos, vol_blksz);

	if((bdpriv->segment_pos < back) || (bdpriv->clip_pos < back) || (s->pos < back))
		printf("Error!!!!! Can't back %x bytes. segment_pos:%llx clip_pos:%llx pos:%llx\n", back, bdpriv->segment_pos, bdpriv->clip_pos, s->pos);

	bdpriv->segment_pos -= REMAINDER(pos, vol_blksz);
	bdpriv->clip_pos -= REMAINDER(pos, vol_blksz);
	s->pos -= REMAINDER(pos, vol_blksz);

	s->buf_pos=s->buf_len=0; // reset to make sure buffer is cleared
#ifdef STREAM_AIO
	if(aio) s->buf_len = s->buf_pos = 0; // reset buf_len for aio reinit
#endif
	//printf("%s:%d pos ori %llx new %llx, ofs %llx, adj pos %llx\n", __func__, __LINE__, oripos, newpos, s->bd_ofs, pos);
	return 1;
}

static int control(stream_t *s, int cmd, void *arg)
{
	struct bd_priv_t *bdpriv = s->priv;
	BLURAY_TITLE_INFO *ti = bdpriv->title_info;

	switch(cmd) {
		case STREAM_CTRL_GET_TIME_LENGTH:
			*((double*)arg) = (double)ti->duration/90000.0;
			//printf("%s:%d STREAM_CTRL_GET_TIME_LENGTH %f\n", __FILE__, __LINE__, *((double*)arg));
			return 1;
		case STREAM_CTRL_GET_SIZE:
			*((off_t*)arg) = s->end_pos;
			//printf("%s:%d STREAM_CTRL_GET_SIZE %llx\n", __FILE__, __LINE__, s->end_pos);
			return 1;

		case STREAM_CTRL_GET_NUM_CHAPTERS:
			*((unsigned int *) arg) = ti->chapter_count;
			//printf("%s:%d STREAM_CTRL_GET_NUM_CHAPTERS %d\n", __FILE__, __LINE__, ti->chapter_count);
			return 1;

		case STREAM_CTRL_GET_CURRENT_CHAPTER:
			bdpriv->chapter = bd_get_chapter_idx(s, s->pos);
			*((unsigned int *) arg) = bdpriv->chapter;
			//printf("%d:%d STREAM_CTRL_GET_CURRENT_CHAPTER %d\n", __FILE__, __LINE__, bdpriv->chapter);
			return 1;

		case STREAM_CTRL_SEEK_TO_CHAPTER:
		{
			int r;
			int64_t pos;
			int chapter = *((unsigned int *) arg);

			if (chapter < 0 || chapter > ti->chapter_count) {
				return STREAM_UNSUPPORTED;
			}

			pos = bd_get_chapter_offset(s, chapter);
			pos = ROUND_DOWN(pos, (int64_t)vol_blksz); // align to vol_blksz for dio
			r = s->seek(s, pos);
			bdpriv->chapter = chapter;
			//printf("%s:%d STREAM_CTRL_SEEK_TO_CHAPTER chap %d pos %llx\n", __FILE__, __LINE__, chapter, pos);
			return r ? 1 : STREAM_UNSUPPORTED;
		}
	}
	return STREAM_UNSUPPORTED;
}

static void stream_close(stream_t *s)
{
	struct bd_priv_t *bdpriv = s->priv;

	if(bdpriv->segments)
		UDFFreeFileBlock(bdpriv->segments);

	if(bdpriv->clip_list)
		free(bdpriv->clip_list);

	if(bdpriv->title_info)
		bd_free_title_info(bdpriv->title_info);

	if(bdpriv->play_list)
		mpls_free(bdpriv->play_list);

	if(bdpriv->bd)
		bd_close(bdpriv->bd);

	if(bdpriv->device)
		free(bdpriv->device);

	if(s->priv){
		free(s->priv);
		s->priv = NULL;
	}

#ifdef BD_CLIP_BASE_PTS
	if (list_base_time[BD_ISO_IN_TIME])
	{
		free(list_base_time[BD_ISO_IN_TIME]);
		list_base_time[BD_ISO_IN_TIME] = NULL;
	}
	if (list_base_time[BD_ISO_OUT_TIME])
	{
		free(list_base_time[BD_ISO_OUT_TIME]);
		list_base_time[BD_ISO_OUT_TIME] = NULL;
	}
	if (list_base_time[BD_ISO_BASE_TIME])
	{
		free(list_base_time[BD_ISO_BASE_TIME]);
		list_base_time[BD_ISO_BASE_TIME] = NULL;
	}
#endif
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format)
{
	int i;
	int f = -1;
	int ret = STREAM_OK;
	uint64_t len;
	uint64_t title_size = 0;
	uint64_t *clip_list;
	//int64_t chapter_pos = 0;
	int title_count;
	int iso = 0;
	//off_t bd_m2ts_offset = 0;
	off_t clip_ofs = 0;
	int bd_m2ts_current = 0;
	int bd_title_current = 0;
	int bd_chapter_current = 0;
	char *bd_device_current = NULL;
	struct stat fstat;
	struct stream_priv_s* p = (struct stream_priv_s*)opts;
	struct bd_priv_t *bdpriv = NULL;
	unsigned char fstream[1024];
	BLURAY *bd = NULL;
	BLURAY_TITLE_INFO *ti = NULL;
	MPLS_PL *pl = NULL;
	UDF_DATA udf_data = { -1, 0, NULL };
	UDF_FILE udf_info = { &udf_data, 0, 0, 0, 0 };
#ifdef BD_CLIP_BASE_PTS
	double last_in_pts = 0.0;
#endif /* end of BD_CLIP_BASE_PTS */

	//printf("%s:%d title %05d device %s bd_device %s default %s\n", __FILE__, __LINE__, p->title, p->device, bd_device, DEFAULT_DVD_DEVICE);
	if(p->device)
		bd_device_current = p->device;
	else if(bluray_device)
		bd_device_current = bluray_device;
	else
		bd_device_current = DEFAULT_DVD_DEVICE;

	if(p->m2ts)
		bd_m2ts_current = p->m2ts;
	else if(bluray_m2ts)
		bd_m2ts_current = bluray_m2ts;

	if(bluray_title)
		bd_title_current = bluray_title;

	if(bluray_chapter)
		bd_chapter_current = bluray_chapter;

	//printf("%s:%d current device %s title %d m2ts %d\n", __FILE__, __LINE__, bd_device_current, bd_title_current, bd_m2ts_current);

	if(bd_device_current == NULL || bd_m2ts_current < 0 || bd_m2ts_current > 99999){
		printf("%s:%d Invalid device (%s) or title (%d)\n", __FILE__, __LINE__, bd_device_current, bd_m2ts_current);
		ret = STREAM_UNSUPPORTED;
		goto err_opts;
	}

	if(stat(bd_device_current, &fstat) == -1){
		printf("%s:%d Unable to get file status of %s: %s\n", __FILE__, __LINE__, bd_device_current, strerror(errno));
		ret = STREAM_ERROR;
		goto err_opts;
	}

	/* Blu-ray Block Device */
	if(S_ISBLK(fstat.st_mode)){
		printf("%s:%d Block device not supported: %s\n", __FILE__, __LINE__, bd_device_current);
		ret = STREAM_UNSUPPORTED;
		goto err_opts;

		/* Blu-ray Directory */
	}else if(S_ISDIR(fstat.st_mode)){
		iso = 0;

		/* Blu-ray ISO Image */
	}else if(S_ISREG(fstat.st_mode)){
		iso = 1;

		f = open_file_dio(bd_device_current);
		if(f<0) {
			ret = STREAM_ERROR;
			goto err_opts;
		}

		udf_data.fd = f;

		/* Unknown device */
	}else{
		printf("%s:%d Unsupported bluray device: %s\n", __FILE__, __LINE__, bd_device_current);
		ret = STREAM_UNSUPPORTED;
		goto err_opts;
	}

	/* process title */
	bd = bd_open(bd_device_current, NULL);
	if(bd == NULL){
		printf("%s:%d bd_open failed\n", __FILE__, __LINE__);
		ret = STREAM_ERROR;
		goto err_iso;
	}

	//title_count = bd_get_titles(bd, TITLES_RELEVANT);
	title_count = bd_get_titles(bd, TITLES_FILTER_DUP_CLIP);
#ifdef ENABLE_CHECK_BD_INFO
	/* Automatic search correct title */
	{
		BLURAY_TITLE_MAX_DUR *title_max = NULL;
		MPLS_PL * pl = NULL, * pl2 = NULL, * def_pl;
		int title_guess;
		int title_id[2] = {-1};
		int mpls_id[2] = {-1};
		int nRet = 0;
		title_max = bd_get_title_max2_dur(bd);
		if (title_max != NULL)
		{
			pl = bd_title_filter(bd, title_max->dur_first_count, title_max->dur_first, !iso, bd_device_current, &title_id[0], &mpls_id[0]);
			if (title_max->dur_second_count > 0)
			{
				pl2 = bd_title_filter(bd, title_max->dur_second_count, title_max->dur_second, !iso, bd_device_current, &title_id[1], &mpls_id[1]);
			}
			bdiso_debug("In [%s][%d] title_id: %d %d, mpls_id: %d %d\n", __func__, __LINE__, title_id[0], title_id[1], mpls_id[0], mpls_id[1]);
			title_guess = title_id[0];
			bd_free_title_max2_dur(title_max);
		}

		def_pl = pl;
		if (pl2 != NULL && pl != NULL)
		{
			MPLS_PI *pi, *pi2;
			pi = &pl->play_item[0];
			pi2 = &pl2->play_item[0];
			printf("In [%s][%d] playlist 1: c: %d, v: %d, a: %d, s: %d, d: %d\n", __func__, __LINE__, bd_get_chapters(pl), pi->stn.num_video, pi->stn.num_audio, pi->stn.num_pg, _pl_duration(pl)/45000);
			printf("In [%s][%d] playlist 2: c: %d, v: %d, a: %d, s: %d, d: %d\n", __func__, __LINE__, bd_get_chapters(pl2),	pi2->stn.num_video, pi2->stn.num_audio, pi2->stn.num_pg, _pl_duration(pl2)/45000);
			if ((bd_get_chapters(pl) < bd_get_chapters(pl2)) &&
					(pi->stn.num_audio <= pi2->stn.num_audio) )
			{
				// playlist 2 have more chapters and audio use it.
				def_pl = pl2;
				title_guess = title_id[1];
			} else {
				if ((pl->list_count > 0) && (pl2->list_count > 0))
				{
					if ( (pi2->stn.num_audio >= pi->stn.num_audio) &&
							(pi2->stn.num_pg > pi->stn.num_pg) &&
							(bd_get_chapters(pl2) >= bd_get_chapters(pl)) )	//Barry 2011-07-29 fix mantis: 5605
					{
						def_pl = pl2;
						title_guess = title_id[1];
					}
#if 1	//Barry 2011-07-13 fix mantis: 5192
					if ((_pl_duration(pl)/45000) > 60000)
					{
						def_pl = pl2;
						title_guess = title_id[1];
					}
#endif
				}
			}
		}
		else if (pl2 != NULL && pl == NULL)
		{
			// playlist 2 have more chapters and audio use it.
			def_pl = pl2;
			title_guess = title_id[1];
			bdiso_debug("***********  Switch to playlist 2 **********\n");
		}

		{
			MPLS_PI *pi = NULL;
			int ii, jj;
			for(ii = 0; ii < 1; ii++)
			{
				pi = &def_pl->play_item[ii];
				// video 
				bdiso_debug("@@@ In [%s][%d] num_video is [%d] @@@\n", __func__, __LINE__, pi->stn.num_video);
				for(jj = 0 ; jj < pi->stn.num_video ; jj++)
				{
					bdiso_debug("@@ In [%s][%d] Video Stream[%d] pid [%d] type[%x] in_time[%x] out_time[%x]@@\n", __func__, __LINE__, jj, pi->stn.video[jj].pid, pi->stn.video[jj].coding_type, pi->in_time, pi->out_time);
				}
				// audio
				bdiso_debug("@@@ In [%s][%d] num_audio is [%d] @@@\n", __func__, __LINE__, pi->stn.num_audio);
				for(jj = 0 ; jj < pi->stn.num_audio; jj++)
				{
					bdiso_debug("@@ In [%s][%d] Audio Stream[%d] pid [%d] type[%x] format[%x] in_time[%x] out_time[%x]@@\n", __func__, __LINE__, jj, pi->stn.audio[jj].pid, pi->stn.audio[jj].coding_type, pi->stn.audio[jj].format, pi->in_time, pi->out_time);
				}
				// subtitle
				bdiso_debug("@@@ In [%s][%d] num_sub is [%d] @@@\n", __func__, __LINE__, pi->stn.num_pg);
				for(jj = 0 ; jj < pi->stn.num_pg; jj++)
				{
					bdiso_debug("@@ In [%s][%d] Subtitle Stream[%d] pid [%d] type[%x] lang[%s] in_time[%x] out_time[%x]@@\n", __func__, __LINE__, jj, pi->stn.pg[jj].pid, pi->stn.pg[jj].coding_type, pi->stn.pg[jj].lang, pi->in_time, pi->out_time);
				}
			}
			pi = &def_pl->play_item[0];
			printf("### In [%s][%d] title_guess is [%d] m2ts [%d] bd_title_current[%d]###\n", __func__, __LINE__, title_guess, pi? atoi(pi->clip[0].clip_id): -1, bd_title_current);
		}

		if (pl != NULL) mpls_free(pl);
		if (pl2 != NULL) mpls_free(pl2);
		bdiso_debug("$$$ In [%s][%d] bd_title_current is [%d] $$$\n", __func__, __LINE__, bd_title_current);
		if (bd_title_current == 0)
			bd_title_current = title_guess;
	}
#endif /* end of ENABLE_CHECK_BD_INFO */
	bd_select_title(bd, bd_title_current);

	title_size = bd_get_title_size(bd);
	//mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %llx bytes\n", title_size);
	printf("%s:%d title size %llx\n", __FILE__, __LINE__, title_size);

	ti = bd_get_title_info(bd, bd_title_current);
	if (!ti){
		printf("%s: get bd title info failed\n", __FILE__);
		ret = STREAM_ERROR;
		goto err_bd;
	}
#if 1	//Barry 2011-09-08	chapters[jj].offset information are wrong
	int jj;
	for (jj=0;jj<ti->chapter_count-1;jj++)
	{
		if (ti->chapters[jj].offset == ti->chapters[jj+1].offset)
			break;
	}
	if ( (jj == 0) && (ti->chapters[ti->chapter_count-1].offset < ti->chapters[0].offset) && (ti->chapter_count > 1))
	{
		ti->chapter_count = 1;
		printf("####  [%s - %d]   ti->chapter_count[%d] value is wrong, set ti->chapter_count == 1\n", __func__, __LINE__, ti->chapter_count);
	}
#endif

#ifdef SHOW_BD_INFO
	print_title_info(ti);
#endif
	/* process playlist */
	if(iso)
		snprintf(fstream, 1024, "/BDMV/PLAYLIST/%05d.mpls", ti->playlist);
	else
		snprintf(fstream, 1024, "%s/BDMV/PLAYLIST/%05d.mpls", bd_device_current, ti->playlist);

	bdiso_debug("## In [%s][%d] mpls is [%s] ###\n", __func__, __LINE__, fstream);

	if(file_clr_dio(f) != 0) goto err_bd;
	pl = mpls_parse(fstream, 0, &udf_data);
	if(file_set_dio(f) != 0) goto err_bd;
#ifdef SHOW_BD_INFO
	printf("playlist count %d\n", pl->list_count);
#endif

#ifdef BD_CLIP_BASE_PTS
	if (pl->list_count)
	{
		list_base_time[BD_ISO_IN_TIME] = calloc(pl->list_count, sizeof(double));
		list_base_time[BD_ISO_OUT_TIME] = calloc(pl->list_count, sizeof(double));
		list_base_time[BD_ISO_BASE_TIME] = calloc(pl->list_count, sizeof(double));
	}
#endif
	clip_list = calloc(pl->list_count, sizeof(uint64_t));
	for(i=0; i < pl->list_count; i++){
		MPLS_PI *pi = &pl->play_item[i];
		uint32_t lbs;

		if(iso){
			snprintf(fstream, 1024, "/BDMV/STREAM/%5s.m2ts", pi->clip->clip_id);
			if(file_clr_dio(f) != 0) goto err_bd;
			lbs = BDUDFFindFile(&udf_info, fstream, &len);
			if(file_set_dio(f) != 0) goto err_bd;
#ifdef SHOW_BD_INFO
			printf("play_item[%d] clip_id %s lbs %x ofs %llx len %llx\n", i, pi->clip->clip_id, lbs, clip_ofs, len);
#endif
		}else{
			snprintf(fstream, 1024, "%s/BDMV/STREAM/%5s.m2ts", bd_device_current, pi->clip->clip_id);
			if(stat(fstream, &fstat) == -1){
				printf("%s:%d error to get file status %s (%s)\n", __FILE__, __LINE__, fstream, strerror(errno));
				goto err_bd;
			}
			len = fstat.st_size;
#ifdef SHOW_BD_INFO
			printf("play_item[%d] clip_id %s len %llx\n", i, pi->clip->clip_id, len);
#endif
		}
		bdiso_debug("$$$ In [%s][%d] file is [%s] clip_id [%s] len [%llx] pi->in_time[%x] pi->out_time[%x]$$$\n", __func__, __LINE__, fstream, pi->clip->clip_id, len, pi->in_time, pi->out_time);
#ifdef BD_CLIP_BASE_PTS
		list_base_time[BD_ISO_IN_TIME][i] = (double)pi->in_time/45000.0; // In Time
		list_base_time[BD_ISO_OUT_TIME][i] = (double)pi->out_time/45000.0; // Out Time
		if (i > 0)
			list_base_time[BD_ISO_BASE_TIME][i] = (list_base_time[BD_ISO_OUT_TIME][i] - list_base_time[BD_ISO_IN_TIME][i]) + list_base_time[BD_ISO_BASE_TIME][i-1]; // Base Time
		else
			list_base_time[BD_ISO_BASE_TIME][i] = list_base_time[BD_ISO_OUT_TIME][i];	// Base Time
		bdiso_debug("### In [%s][%d], list_base_time[%d] in_time[%f] out_time[%f] base_time[%f]###\n", __func__, __LINE__, i, list_base_time[BD_ISO_IN_TIME][i], list_base_time[BD_ISO_OUT_TIME][i], list_base_time[BD_ISO_BASE_TIME][i]);

#endif

		clip_list[i] = len;
		clip_ofs += len;
	}

	bdpriv = calloc(1, sizeof(struct bd_priv_t));
	if(bdpriv == NULL){
		printf("%s:%d Failed to allocate memory\n", __FILE__, __LINE__);
		ret = STREAM_ERROR;
		goto err_bd;
	}

	bdpriv->device = strdup(bd_device_current);
	bdpriv->iso = iso;
	bdpriv->title = bd_title_current;
	bdpriv->chapter = bd_chapter_current;
	bdpriv->m2ts = bd_m2ts_current;
	bdpriv->title_info = ti;
	bdpriv->play_list = pl;
	bdpriv->clip_list = clip_list;

#ifdef BD_CLIP_BASE_PTS
	if (ti->clip_count > 1)
	{
		if (list_base_time[BD_ISO_IN_TIME][0] == list_base_time[BD_ISO_IN_TIME][1])
			bdpriv->timebar_type = BDISO_TIMEBRR_TYPE_FIXED_INTIME;
		else if (abs(list_base_time[BD_ISO_OUT_TIME][0] -list_base_time[BD_ISO_IN_TIME][1]) < 1.0)
			bdpriv->timebar_type = BDISO_TIMEBRR_TYPE_CON_TIME;
		else
			bdpriv->timebar_type = BDISO_TIMEBRR_TYPE_RANDOM_TIME;
	}
	else
		bdpriv->timebar_type = BDISO_TIMEBRR_TYPE_CON_TIME;
	printf("@@@ In [%s][%d] timebar_type is [%d]@@@\n", __func__, __LINE__, bdpriv->timebar_type);
#endif /* end of BD_CLIP_BASE_PTS */

	stream->priv = bdpriv;
	//stream->start_pos = chapter_pos;	// we can't seek to position smaller than start_pos, don't modify it
	if (title_size < clip_ofs)
	{
		title_size = clip_ofs;
		printf("@@@ In [%s][%d] change title_size to [%llx] @@@\n", __func__, __LINE__, title_size);
	}
	stream->end_pos = title_size;
	stream->type = STREAMTYPE_FILE;
	stream->fill_buffer = fill_buffer;
	stream->seek = seek;
	stream->control = control;
	stream->close = stream_close;
	if(iso) stream->fd = f;

	/* open first clip */
	if(bd_m2ts_open(stream, 0) != 0){
		ret = STREAM_ERROR;
		goto err_priv;
	}
	goto err_bd;
	
err_priv:
	if(bdpriv) free(bdpriv);
err_bd:
	bd_close(bd);
err_iso:
err_opts:
err_none:
	m_struct_free(&stream_opts,opts);
	return ret;
}

#ifdef ENABLE_CHECK_BD_INFO
static uint32_t _pl_duration(MPLS_PL *pl)
{
	int ii;
	uint32_t duration = 0;
	MPLS_PI *pi;

	for (ii = 0; ii < pl->list_count; ii++) {
		pi = &pl->play_item[ii];
		duration += pi->out_time - pi->in_time;
	}
	return duration;
}

static int bd_get_chapters(MPLS_PL *def_pl)
{
	int ii;
	MPLS_PI *pi;
	MPLS_PLM *plm;
	int chapters = 0;
	for (ii = 0; ii < def_pl->mark_count; ii++) {
		plm = &def_pl->play_mark[ii];
		pi = &def_pl->play_item[plm->play_item_ref];
		if (plm->mark_type == BD_MARK_ENTRY) {
			if(pi->out_time < plm->time)
				continue;
			if (((pi->out_time - plm->time) / 45000) > 0)
			{
				chapters++;
			}
		}
	}
	return chapters;
}
static MPLS_PL * bd_title_filter(BLURAY *bd, uint32_t count, BLURAY_TITLE_MPLS_ID *title, int is_dir, char *filepath, int *title_id, int *mpls_id)
{
	MPLS_PL *pl = NULL, *pl_tmp = NULL;
	MPLS_PI *pi = NULL, *pi_tmp = NULL;
	int chapter_count, chapter_count_tmp;
	char *path = NULL;
	int ii;

	for (ii = 0; ii < count; ii++)
	{
		if (is_dir == 1) {
			path = str_printf("%s/BDMV/PLAYLIST/%05d.mpls", filepath, title[ii].mpls_id);
		} else {
			path = str_printf("/BDMV/PLAYLIST/%05d.mpls", title[ii].mpls_id);
		}
		if (pl == NULL)
		{
			pl = mpls_parse(path, 0, bd_get_udfdata(bd));
			if (pl != NULL)
			{
				pi = &pl->play_item[0];
				*title_id = (int)title[ii].title_id;
				*mpls_id = (int)title[ii].mpls_id;
				chapter_count = bd_get_chapters(pl);
			}
		} else {
			pl_tmp = mpls_parse(path, 0, bd_get_udfdata(bd));
			if (pl_tmp != NULL)
			{
				pi_tmp = &pl_tmp->play_item[0];
				chapter_count_tmp = bd_get_chapters(pl_tmp); 
				bdiso_debug(" ## In [%s][%d] filter playlist 1: c: %d, v: %d, a: %d, s: %d, d: %d\n", __func__, __LINE__, chapter_count, pi->stn.num_video, pi->stn.num_audio, pi->stn.num_pg, _pl_duration(pl)/45000);
				bdiso_debug(" ## In [%s][%d] filter playlist 2: c: %d, v: %d, a: %d, s: %d, d: %d\n", __func__, __LINE__, chapter_count_tmp,	pi_tmp->stn.num_video, pi_tmp->stn.num_audio, pi_tmp->stn.num_pg, _pl_duration(pl_tmp)/45000);
				if ( ((pi_tmp->stn.num_pg > 0) && (pi->stn.num_pg > pi_tmp->stn.num_pg)) && \
						((pi_tmp->stn.num_audio > 0) && (pi_tmp->stn.num_audio >= pi->stn.num_audio - 1)) && \
						(chapter_count_tmp >= chapter_count)
						)
				{
					mpls_free(pl);
					pl = pl_tmp;
					pi = &pl->play_item[0];
					chapter_count = chapter_count_tmp;
					*title_id = (int)title[ii].title_id;
					*mpls_id = (int)title[ii].mpls_id;
				} else {
#if 1	//Barry 2011-07-13 fix mantis: 5192
					if ((_pl_duration(pl)/45000) > 60000)
					{
						mpls_free(pl);
						pl = pl_tmp;
						pi = &pl->play_item[0];
						chapter_count = chapter_count_tmp;
						*title_id = (int)title[ii].title_id;
						*mpls_id = (int)title[ii].mpls_id;
					}
					else
#endif
					mpls_free(pl_tmp);
				}
			}
		}
		if (path != NULL) free(path);
	}

	return pl;
}

#endif /* end of ENABLE_CHECK_BD_INFO */

int mp_bd_lang_from_sid(stream_t *stream, int sid, unsigned char *lang) {
    struct bd_priv_t *bdpriv = stream->priv;
    MPLS_PL *pl = bdpriv->play_list;
    MPLS_PI *pi = NULL;

    pi=&pl->play_item[0];
    if(sid < 0) return 0;
    
    strcpy(lang,pi->stn.pg[sid].lang);

    return 1;
}

const stream_info_t stream_info_bd = {
	"Blu-ray Disc",
	"bd",
	"Charles Lin",
	"open bluray directory or iso image",
	open_f,
	{ "bd", NULL },
	&stream_opts,
	1 // Urls are an option string
};

