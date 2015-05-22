/* sky_vdec_2.h                                                          
 *                                                                     
 * Copyright (C) 2010 Skyviia, Inc.                                    
 *                                                                     
 * This software is licensed under the terms of the GNU General Public 
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.         
 *                                                                     
 * This program is distributed in the hope that it will be useful,     
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       
 * GNU General Public License for more details.                        
 *                                                                     
 */                                                                    
                                                                       
#ifndef __SKY_VDEC_H__
#define __SKY_VDEC_H__

#include <linux/ioctl.h>    /* needed for the _IOW etc stuff used later */

#include "../libvo/sky_api.h"

#define MAX_SVSD_VDEC_BUFFER 256
#define SKY_STORE_THUMB_BEGIN 0x01
#define SKY_STORE_THUMB_END 0x02


// Standard
enum VideoStandard
{
	VCODEC_JPEG 	= 0,		// M-JPEG
	VCODEC_MP2 	= 1,		// MPEG-1/2
	VCODEC_MP4 	= 2,		// MPEG-4 / H.263
	VCODEC_DIVX3 	= 3,		// DivX 3
	VCODEC_DIVX4 	= 4,		// DivX 4
	VCODEC_SOR 	= 5,		// Sorenson
	VCODEC_MP4S	= 6,		// MP4S	
	VCODEC_H264 	= 7,		// H.264
	VCODEC_WMV3	= 8,		// VC-1 Simple / Main profile (*.rcv)
	VCODEC_WVC1 	= 9,		// VC-1 Advanced profile (*.vc1)
	VCODEC_REAL 	= 10,		// Real video
	VCODEC_VP6 	= 11,	// VP6
	VCODEC_VP6A	= 12,	// VP6 Alpha
};


enum OutputMode
{
	OUTPUT_DISPLAY 	= 0,		// Output to display
	OUTPUT_FILE 	= 1,		// Output to YUV file
};
                                                                       
enum OperationMode
{
	STANDALONE_MODE	=0,
	COMBINED_PINGPONG_MODE =1,
	COMBINED_MULTIBUFFER_MODE =2

};




typedef struct _pp_cfg_init
{
	
	unsigned int 	operation_mode;		/*0: STANDALONE_MODE, 1: COMBINED_PINGPONG_MODE, 2: COMBINED_MULTIBUFFER_MODE*/
	int 			rotation;
	float			zoom_ratio;
	unsigned int 	scaling_en;
	unsigned int 	deinterlacing_en;
	unsigned int 	RGBconversion;
	unsigned int    yuv422Conv;
	unsigned int	dithering_en;
	unsigned int 	ppInW;		/*added by jonnyke 20100419*/
	unsigned int 	ppInH;		/*added by jonnyke 20100419*/
}pp_cfg_init;
																	   
// Structure

#define HEADER_DATA_SIZE	512

typedef struct _vdec_alloc_info
{
	unsigned int len;
	void *virtAddr;
	unsigned int phyAddr; /*johnnyke 20100803*/
}vdec_alloc_t;

typedef struct _vdec_thumbnail_resolution 
{
	unsigned int th_width;		// thumbnail width
	unsigned int th_height;	// thumbnail height
	
}vdec_th_res_t;

typedef struct _vdec_alpha_blend 
{
	unsigned int blend_enable;
	unsigned int cur_blend_enable;
	unsigned int blend_sx;
	unsigned int blend_sy;
	unsigned int blend_w;
	unsigned int blend_h;
	unsigned int blend_ba;
	
	
}vdec_alpha_blend_t;


typedef struct _vdec_init_info
{
	unsigned int standard;
	unsigned int width;			// for DivX 3
	unsigned int height;			// for DivX 3
	unsigned int output_mode;
	unsigned int IntraFreezeEnable; 
	unsigned int h264_jitterbufnum;
	pp_cfg_init  ppCfgInit;
	unsigned char HeaderData[HEADER_DATA_SIZE];	// for WMV3 (*.rcv)
	unsigned int pulldown32;
	vdec_alloc_t streamBuf;
	unsigned int thumbnail_mode;		// enable thumbnail mode
	vdec_th_res_t thumbnail_res;		
	unsigned int double_deint;
	unsigned int pulldown32_2;
	unsigned int aspect_ratio;
	
}vdec_init_t;




typedef struct _vdec_init2_info
{
        unsigned int workaround_flag;			// workaround for video decode timeout or bitstream error
	 unsigned int disable_deinterlace;		// 1: turn off deinterlace(display)
	 unsigned int double_deint;				// for On2's de-interlace
	 unsigned int pulldown32;				// for On2's 3:2pulldown
}vdec_init2_t;

typedef struct _vdec_dec_info
{
	unsigned int standard;
	unsigned int buf_size;
	unsigned char * buf_addr;
	unsigned int 	buf_busaddr;	/*johnnyke 20100803*/
	unsigned int dec_flags;   // 0 : normal , 1 : usePeekOutput , 2 : resync_video

	//int speed_mult;	/*johnnyke 20100524*/
	//int resync_video;	/*johnnyke 20100609*/

        unsigned int vft;
        double pts;
    int dvdnav;
	
	vdec_alpha_blend_t alpha_blend;
	unsigned int int_pts;

	unsigned char try_drop_b;

	//Barry 2011-05-25
	unsigned int h264_workaround_field_num;
	unsigned int h264_workaround_offset[8];
	unsigned int dvdmenu;
}vdec_dec_t;

#define HAS_DEC_CAPABILITY
#ifdef HAS_DEC_CAPABILITY
struct dec_cap_t {
	unsigned char dolby;
	unsigned char dts;
	unsigned char divx;
	unsigned char rmvb;
	unsigned char drm;
};
#endif

typedef struct _vdec_shm
{
	volatile unsigned int pid;
	volatile unsigned int queue_length;
	volatile unsigned int send_count;
	volatile unsigned int recv_count;
	volatile unsigned int decode_addr;
	volatile unsigned int decode_length;
#ifdef NEW_SVSD_CMD
	volatile unsigned int send_cmd_id;
	volatile unsigned int recv_cmd_id;
#endif
        struct skyfb_api_display_parm d_parm;
        int dwl_skip_enqueue;
        struct skyfb_api_display_parm d_parm_list[10];
#ifdef HAS_DEC_CAPABILITY
	struct dec_cap_t dec_cap;
#endif
        int isprogressive;
        int tune_on_h264_workaround;	//Barry 2011-05-25
        int dwl_enqueue_cnts;	//svsd enqueue counter
        int h264_dpb_numOut;
}vdec_shm_t;

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#ifdef HAVE_SOUNDCARD_H
#include <soundcard.h>
#endif
#endif

typedef struct _vdec_svread_buf
{
	double cur_pts;
        int qlen[4];
        int ft_ridx, ft_widx;
        int decode_done;
	audio_buf_info abinfo;
        unsigned int timeout_current, timeout_total;   

}vdec_svread_buf_t;

typedef struct {
    unsigned busAddress;
    unsigned size;
    int id;
} BufParams;

#define VMEM_CACHE_SIZE		(20 << 20)	// 20MB
#define VMEM_DVDNAV_SIZE	(4 << 20)	// 4MB
#define VMEM_STREAM_SIZE	(6 << 20)	// 6MB
#define DEFAULT_DPB_ID	0

#define MAX_SKY_VFT_BUFFER 128




int sky_video_init(vdec_init_t *init_info); 
int sky_video_uninit(unsigned int standard); 
int sky_video_decode(vdec_dec_t *dec_info);

#if 1	/*johnnyke 20100907*/
void sky_alloc(vdec_alloc_t * alloc_info);
#else
void *sky_alloc(int len);
#endif


void sky_free(void *addr);

#ifdef BITSTREAM_BUFFER_CONTROL
int chk_bufspace(struct demuxer *demuxer);
#endif


/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define HX170DEC_IOC_MAGIC  'k'
/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */

#define HX170DEC_PP_INSTANCE       _IO(HX170DEC_IOC_MAGIC, 1)   /* the client is pp instance */
#define HX170DEC_HW_PERFORMANCE    _IO(HX170DEC_IOC_MAGIC, 2)   /* decode/pp time for HW performance */
#define HX170DEC_IOCGHWOFFSET      _IOR(HX170DEC_IOC_MAGIC,  3, unsigned long *)
#define HX170DEC_IOCGHWIOSIZE      _IOR(HX170DEC_IOC_MAGIC,  4, unsigned int *)

#define HX170DEC_IOC_CLI           _IO(HX170DEC_IOC_MAGIC,  5)
#define HX170DEC_IOC_STI           _IO(HX170DEC_IOC_MAGIC,  6)

#define HX170DEC_IOC_GET_MEM_ADDRESS    _IOWR(HX170DEC_IOC_MAGIC,  7, unsigned int *)
#define HX170DEC_IOC_GET_MEM_SIZE       _IOWR(HX170DEC_IOC_MAGIC,  8, unsigned int *)
#define SKY_VDEC_IOC_GET_MEM_ADDRESS    _IOWR(HX170DEC_IOC_MAGIC,  7, unsigned int *)
#define SKY_VDEC_IOC_GET_MEM_SIZE       _IOWR(HX170DEC_IOC_MAGIC,  8, unsigned int *)

#define SKY_VDEC_INIT			_IO(HX170DEC_IOC_MAGIC, 9)
#define SKY_VDEC_DECODE			_IOW(HX170DEC_IOC_MAGIC,  10, vdec_dec_t *)
#define SKY_VDEC_FRAMERATE_SET		_IOW(HX170DEC_IOC_MAGIC,  11, unsigned int *)
#define SKY_VDEC_GET_DQUEUE_LEN		_IOR(HX170DEC_IOC_MAGIC, 12, unsigned int *)	// get queue length for A/V sync
#define SKY_VDEC_DROP_FRAME		_IOW(HX170DEC_IOC_MAGIC, 13, unsigned int *)
#define SKY_VDEC_FIELD_TYPE_FLAG	_IOW(HX170DEC_IOC_MAGIC, 14, unsigned int *)
#define SKY_VDEC_SYNC_FRMAE_RIDX	_IOW(HX170DEC_IOC_MAGIC, 15, unsigned int *)
#define SKY_VDEC_GET_SVREAD		_IOR(HX170DEC_IOC_MAGIC, 16, unsigned int *)	// get SVREAD ioctl
#define SKY_VDEC_SET_VSYNC_STATUS	_IOW(HX170DEC_IOC_MAGIC, 17, unsigned int *)
#define SKY_VDEC_ENQUEUE		_IOW(HX170DEC_IOC_MAGIC, 18, unsigned int *)
#define SKY_VDEC_WAIT			_IOR(HX170DEC_IOC_MAGIC, 19, unsigned int *)
#define SKY_VDEC_SET_THUMBNAIL_RES		_IOW(HX170DEC_IOC_MAGIC, 20, vdec_th_res_t *)	// Set thumbnail resolution
#define SKY_VDEC_GET_THUMBNAIL_RES		_IOR(HX170DEC_IOC_MAGIC, 21, vdec_th_res_t *)	// Get thumbnail info
#define SKY_VDEC_DO_ALPHABLEND		_IOW(HX170DEC_IOC_MAGIC, 23, unsigned int *)

#define SKY_VDEC_SET_INUSE		_IO(HX170DEC_IOC_MAGIC, 24)
#define SKY_VDEC_SET_UNUSED		_IO(HX170DEC_IOC_MAGIC, 25)
#define SKY_VDEC_SET_USE_TICK		_IO(HX170DEC_IOC_MAGIC, 26)
#define SKY_VDEC_GET_USE_STATUS		_IO(HX170DEC_IOC_MAGIC, 27)
#define SKY_VDEC_VIDEO_MODE			_IO(HX170DEC_IOC_MAGIC, 28)

#ifdef SUPPORT_DIVX_DRM
#define SKY_VDEC_SET_REALOUT_RES       _IOW(HX170DEC_IOC_MAGIC, 31, unsigned int *)
#define SKY_VDEC_GET_VIDEO_ORIG_SCALAR_INFO    _IOR(HX170DEC_IOC_MAGIC, 32, unsigned int *)
#define HX170DEC_IOC_MAXNR 35
#else /* else of SUPPORT_DIVX_DRM */
#define HX170DEC_IOC_MAXNR 30
#endif /* end of SUPPORT_DIVX_DRM */

enum sky_vdec_user {
        SKY_VDEC_VIDEO,
        SKY_VDEC_PHOTO,
        SKY_VDEC_USER_MAX
};

#endif /* __SKY_VDEC_H__ */
                                                                       
