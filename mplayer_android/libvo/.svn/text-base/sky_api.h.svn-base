/* linux/drivers/video/sky_api.h
 *
 * Copyright (C) 2008 Skyviia, Inc.
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

#ifndef __SKY_API_H__
#define __SKY_API_H__

#define SKYFB_GET_DISPLAY_INFO		0xffff1000
#define	SKYFB_SET_DISPLAY_PARM		0xffff1001
#define	SKYFB_SET_DISPLAY_STATUS		0xffff1002
#define	SKYFB_SET_DISPLAY_ADDR		0xffff1003
#define	SKYFB_SET_SCALAR_PARM		0xffff1004
#define	SKYFB_SET_OSD_COMM			0xffff1005
#define	SKYFB_SET_OSD_PARM			0xffff1006
#define	SKYFB_SET_OSD_STATUS		0xffff1007
#define	SKYFB_FORMAT_TRANSFORM		0xffff1008
#define	SKYFB_WAIT_VSYNC              0xffff1009
#define	SKYFB_GET_DISPLAY_ADDR		0xffff100A
#define	SKYFB_SET_BRIGHTNESS		0xffff100B
#define	SKYFB_SET_CONTRAST			0xffff100C
#define	SKYFB_LOCK_DISPLAY			0xffff100D
#define	SKYFB_UNLOCK_DISPLAY		0xffff100E
#define	SKYFB_SET_SATURATION_HUE		0xffff100F
#define	SKYFB_GET_IMAGE_PARM		0xffff1010
#define	SKYFB_SET_SCALE_ERS			0xffff1011
#define	SKYFB_GET_SCALE_ERS			0xffff1012
#define	SKYFB_RECOVER_DISPLAY		0xffff1013
#define 	SKYFB_VIDEO_THUMBNAIL		0xffff1019
#define 	SKYFB_GET_VIDEO_SETMODE_STATUS  0xffff1020
#define	SKYFB_SET_VIDEO_RATIO	0xffff1021
#define 	SKYFB_SET_DEDICATED_MODE	0xffff1022
#define 	SKYFB_SET_DEINTERLACE_STATUS	0xffff1023
#define		SKYFB_SET_YPBPR_MAX		0xffff1024
#define		SKYFB_GET_SUPPORT_MODES		0xffff1025
#define		SKYFB_SET_VIDEO_RES		0xffff1026
#define		SKYFB_SET_VIDEO_ARGB_FMT	0xffff1029
#define SKYFB_IGNORE_VIDEO_US		0xffff1031
#define   SKYFB_SET_FIXED_VIDEO_POSITION        0xffff1034


#define	SKYFB_2D_BITBLT			0xffff2000
#define	SKYFB_2D_LINE_DRAW			0xffff2001
#define	SKYFB_2D_RECTANGLE_FILL		0xffff2002
#define	SKYFB_2D_PATTERN_FILL		0xffff2003
#define	SKYFB_2D_ROP				0xffff2004
#define	SKYFB_2D_ROTATION			0xffff2005

#define SKYFB_OSD_INIT 			0xffff5000
#define SKYFB_OSD_FILL 			0xffff5001
#define SKYFB_OSD_ERASE			0xffff5002

#define	SKYFB_SET_VMADDR			0xffffa001
#define	SKYFB_GET_VMADDR			0xffffa002
#define	SKYFB_ALLOC_MEM			0xffffa003
#define	SKYFB_REALLOC_MEM			0xffffa004
#define	SKYFB_FREE_MEM				0xffffa005

#define	SKYFB_GET_MODE			0xffff101F
#define SKYFB_GET_OUTPUT_DEVICE 0xffff1014
#define	SKYFB_GET_DISP_FPS		0xffff102C
#define	SKYFB_GET_REAL_DISPLAY	0xffff102F
#define   SKYFB_HDMI_GET_AUDIO_INFO     0xffff7000


enum {
	SKYFB_FALSE,
	SKYFB_TRUE,
};

enum {
	SKYFB_OFF,
	SKYFB_ON,
};

enum {
	SKYFB_DISP1,
	SKYFB_DISP2,
};

enum {
	SKYFB_BKG,
	SKYFB_OSD,
};

enum {
	SKYFB_SCALAR_DISPLAY,
	SKYFB_SCALAR_MEMORY,
};

enum {
	SKYFB_NONE_SET,
	SKYFB_SET,
};

enum {
	SKYFB_SCALAR_NONE,
	SKYFB_SCALAR_UP,
	SKYFB_SCALAR_DOWN,
};

enum {	
	INPUT_FORMAT_422,
	INPUT_FORMAT_420,
	INPUT_FORMAT_RGB888,
	INPUT_FORMAT_ARGB,
	INPUT_FORMAT_YCC420,
	INPUT_FORMAT_MAX,
};

//Barry 2010-06-21
enum {
	RATIO_ORIGINAL = 0,
	RATIO_FIT,
	RATIO_1_1,
	RATIO_4_3,
	RATIO_16_9,
	RATIO_USER = 0xff,
};

enum {
	SKYFB_OSD1,
	SKYFB_OSD2,
	SKYFB_OSD3,
	SKYFB_OSD4,
	SKYFB_OSD5,
};

enum {
	BB_DIR_START_UL,	//up left
	BB_DIR_START_UR,	//up right
	BB_DIR_START_DL,	//down left
	BB_DIR_START_DR,	//down right
};

enum {
	AB_FROM_REG,		//alpha blending value from register
	AB_FROM_SRC,		//alpha blending value from source
	AB_FROM_DST,		//alpha blending value from destination 
};

enum {
	AV_FROM_REG,		//alpha value from register
	AV_FROM_SRC,		//alpha value from source
	AV_FROM_PAT,		//alpha value from pattern
	AV_FROM_DST,		//alpha value from destination
	AV_FROM_G2D,		//alpha value depend on 2d command
};

enum {
	YUV_TO_ARGB,
	ARGB_TO_YUV,
	ARGB_TO_HSV,
	HSV_TO_ARGB,
	YUV_TO_HSV,
	HSV_TO_YUV,
};

enum {
	YUV_FMT_IN_YCC,
	YUV_FMT_IN_YUV,
	YUV_FMT_IN_YUV422H,
};

enum {
	YUV_FMT_OUT_YUV,
	YUV_FMT_OUT_YUV422H,
};

enum {
        MODE_1080P_60HZ = 1,
        MODE_1080P_50HZ,
        MODE_1080P_30HZ,
        MODE_1080P_24HZ,
};


typedef	unsigned int	uint32_t;

struct skyfb_api_display_info {
	uint32_t fb_base_addr;
	uint32_t fb_size;
	uint32_t video_offset;
	uint32_t video_size;
	uint32_t uncached_offset;
	uint32_t uncached_size;
	uint32_t width;		//screen width
	uint32_t height;		//screen height
	uint32_t rgb0_offset;	//offset is based on fb_base_addr
	uint32_t rgb1_offset;	//offset is based on fb_base_addr
	uint32_t osd_offset;	//offset is based on fb_base_addr
	uint32_t y_offset;		//offset is based on fb_base_addr
	uint32_t u_offset;		//offset is based on fb_base_addr
	uint32_t v_offset;		//offset is based on fb_base_addr
};

struct skyfb_api_display_parm {
	uint32_t display;		//display1 or display2
	uint32_t input_format;	//ARGB, YCC420
	uint32_t start_x;		//display 2 only
	uint32_t start_y;		//display 2 only
	uint32_t width_in;		//if no scalar, width_in = width_out
	uint32_t height_in;		//if no scalar, height_in = height_out
	uint32_t width_out;		//display 1 only
	uint32_t height_out;	//display 1 only
	uint32_t stride;
	uint32_t alpha;		//YCC420 global ahpha, display 2 only
	uint32_t y_addr;		//YCC420, ARGB
	uint32_t u_addr;		//YCC420
	uint32_t v_addr;		//none use in ARGB and YCC420 mode
};

struct skyfb_api_display_status {
	uint32_t display;		//display1 or display2
	uint32_t status;		//on/off
};

struct skyfb_api_display_addr {
	uint32_t display;		//display1 or display2
	uint32_t y_addr;		//YCC420, ARGB
	uint32_t u_addr;		//YCC420
	uint32_t v_addr;		//none use in ARGB and YCC420 mode
};

struct skyfb_api_scalar_parm {
	uint32_t width_in;
	uint32_t height_in;
	uint32_t width_out;
	uint32_t height_out;
	uint32_t stride_in;		//scalar to memory only
	uint32_t stride_out;	//scalar to memory only
	uint32_t y_addr_in;		//scalar to memory only
	uint32_t u_addr_in;		//scalar to memory only
	uint32_t y_addr_out;	//scalar to memory only
	uint32_t u_addr_out;	//scalar to memory only
	uint32_t pseudo;		//if true, just get result width and height
	uint32_t scale_to;		//display or memory
	uint32_t scale_mode;	//original ratio or fit in resolution
};

struct skyfb_api_osd_common {
	uint32_t addr;
	uint32_t size;
	uint32_t palette_change;	//if true then replace palette
	uint32_t red;			//r3<<24 | r2<<16 | r1<<8 | r0
	uint32_t green1;		//g3<<24 | g2<<16 | g1<<8 | g0
	uint32_t green2;		//g7<<24 | g6<<16 | g5<<8 | g4
	uint32_t blue;			//b3<<24 | b2<<16 | b1<<8 | b0
};

struct skyfb_api_osd_parm {
	uint32_t index;		//select osd 1~5;
	uint32_t alpha;		//alpha 0~15
	uint32_t status;		//select osd on/off
	uint32_t start_x;
	uint32_t start_y;
	uint32_t width;
	uint32_t height;
};

struct skyfb_api_brightness_parm {	
	uint32_t display;		//display1 or display2
	uint32_t brightness;	//0~255
};

struct skyfb_api_contrast_parm {
	uint32_t display;		//display1 or display2
	uint32_t contrast;		//0~255
};

struct skyfb_api_saturation_hue_parm {	
	uint32_t display;		//display1 or display2
	uint32_t saturation;	//0~127
	uint32_t hue;			//0~100
};

struct skyfb_api_image_parm {	
	uint32_t display;		//display1 or display2
	uint32_t brightness;	//0~255
	uint32_t contrast;		//0~255
	uint32_t saturation;	//0~127
	uint32_t hue;			//0~100
};

#ifdef CONFIG_FPGA
struct skyfb_api_ft_parm {
	uint32_t src_y_addr;
	uint32_t src_u_addr;
	uint32_t src_v_addr;
	uint32_t dst_y_addr;
	uint32_t dst_u_addr;
	uint32_t dst_v_addr;
	uint32_t src_stride;
	uint32_t dst_stride;
	uint32_t ft_mode;
	uint32_t yuv_in_mode;	//yuv to xxx only
	uint32_t yuv_out_mode;	//xxx to yuv only
	uint32_t width;
	uint32_t height;
	uint32_t alpha;		//optional, set to output alpha, ARGB only
};
#else
struct skyfb_api_ft_parm {	//format transform, only valid for YCC420
	uint32_t src_y_addr;
	uint32_t src_u_addr;
	uint32_t dst_y_addr;	//ARGB output address
	uint32_t src_stride;
	uint32_t dst_stride;
	uint32_t width;
	uint32_t height;
	uint32_t alpha;		//optional, set to output alpha
};
#endif
//2D API structure
struct skyfb_api_bitblt {
	uint32_t src_addr;
	uint32_t dst_addr;
	uint32_t width;
	uint32_t height;
	uint32_t src_stride;
	uint32_t dst_stride;
	uint32_t direction;			//0~3, need auto detection???
	uint32_t alpha_value_from;
	uint32_t alpha_value;
	uint32_t alpha_blend_status;
	uint32_t alpha_blend_from;
	uint32_t alpha_blend_value;
};

struct skyfb_api_line_draw {
	uint32_t dst_addr;
	uint32_t dst_stride;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t end_x;
	uint32_t end_y;
	uint32_t color;			//[31:0]ARGB
	uint32_t alpha_value_from;
	uint32_t alpha_value;	
};

struct skyfb_api_rectangle_fill {
	uint32_t dst_addr;
	uint32_t dst_stride;
	uint32_t width;
	uint32_t height;
	uint32_t color;			//[31:0]ARGB
	uint32_t alpha_value_from;
	uint32_t alpha_value;
	uint32_t alpha_blend_status;
	uint32_t alpha_blend_from;
	uint32_t alpha_blend_value;
};

struct skyfb_api_pattern_fill {
	uint32_t dst_addr;
	uint32_t dst_stride;
	uint32_t width;
	uint32_t height;
	uint32_t fg_color;			//[31:0]ARGB
	uint32_t bg_color;			//[31:0]ARGB
	uint32_t value1;			//[31:0]line4~line1, each [7:0] pixel 8~1
	uint32_t value2;			//[31:0]line8~line5, each [7:0] pixel 8~1
	uint32_t alpha_value_from;
	uint32_t alpha_value;
	uint32_t alpha_blend_status;
	uint32_t alpha_blend_from;
	uint32_t alpha_blend_value;
};
#ifdef SGL
struct skyfb_map {
	uint32_t start_addr;
	uint32_t size;
	uint32_t isfree;
	struct skyfb_map *pre;	
	struct skyfb_map *next;
};

struct skyfb_malloc {
	uint32_t offset;
	uint32_t size;
};
#endif

struct skyfb_api_video_thumbnail {
	uint32_t flag;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t width;
	uint32_t height;
	uint32_t mode;
	uint32_t realw;		//UI don't care this
	uint32_t realh;		//UI don't care this
};

struct skyfb_api_osd {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t data_addr;
	uint32_t block;
	uint32_t alpha;
};

struct hdmiAudioInfo{
  uint8_t bType;
  uint8_t bSRate;
  uint8_t bRes;
};

struct hdmiSupportAudioInfo_t{
  uint8_t bNum;
  struct hdmiAudioInfo bAudInfo[15];
}__attribute__((packed));



void api_get_display_info(struct skyfb_api_display_info *d_info);
int api_set_display_parm(struct skyfb_api_display_parm *d_parm);
int api_set_display_addr(struct skyfb_api_display_addr *d_addr);
uint32_t api_set_scalar_parm(struct skyfb_api_scalar_parm *s_parm);
void api_format_transform(struct skyfb_api_ft_parm *ft_parm);
void set_display_status(int dev, int status);
void api_2d_bitblt(struct skyfb_api_bitblt *bb);

#endif	/* __SKY_API_H__ */

