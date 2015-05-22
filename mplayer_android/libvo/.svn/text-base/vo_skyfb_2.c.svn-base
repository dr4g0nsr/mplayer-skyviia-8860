/*
 * Video driver for Skyviia Framebuffer device
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "video_out.h"
#include "video_out_internal.h"
#ifdef VO_SUB_THREAD
#include "../pthread-macro.h"
#endif
#ifdef CONFIG_DVDNAV
#include "../stream/stream_dvdnav.h"
#endif

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include "sky_api.h"	//Fuchun 2009.09.18
#include "../libmpcodecs/sky_vdec_2.h"

static vo_info_t info = {
	"Skyviia Framebuffer Device",
	"skyfb",
	"Raymond Chen",
	""
};


//RGB  255,255,0   Yellow (H:60  S:100%)
//RGB  255,255,112   Yellow (H:60  S:56%)
//RGB  255,255,196   Yellow (H:60  S:23%)

unsigned char sub_color[8] = {	//Fuchun 2009.12.18
	0xff,	//white
	0x80,	//black
	0xfc,	//yellow
	0xbb, //0xf3,	//blue
	0xe0,	//red
//	0xa1,	//purple
	0x9c,	//green
	0xda,	//lite white
	0xad	//gray
};

int color_num = 0;	//default white	//Fuchun 2009.12.18
extern char *dvd_device;
extern void* vo_spudec;
extern int vo_teletextsub;
extern vdec_shm_t *svsd_state;
extern unsigned int no_osd;
LIBVO_EXTERN(skyfb)

static uint32_t image_width, image_height;

//static int fd = -1;
int skyfb_fd = -1;

//Fuchun 2009.09.18
struct skyfb_api_display_info d_info;
struct skyfb_api_osd_common o_comm;
struct skyfb_api_osd_parm o_parm;
unsigned int real_display = 0;
unsigned int x_res_shift = 0;
unsigned int y_res_shift = 0;

void *memBase0 = NULL;
unsigned char *osd_addr = NULL;
unsigned int osd_addr_phy = 0;
unsigned int osd_size = 0;
extern int display_height;
extern int display_width;
extern int dvdsub_id;
extern int vobsub_id;
extern volatile void *regmap;
float transport_float = 0.0;

unsigned char color = 0;

unsigned int skydroid = 0;	// Raymond 2009/08/11
unsigned int skyqt = 0;		// Robert 2011/02/18
unsigned int forcelock = 0;	// Mark 2010/04/21
#ifdef HW_TS_DEMUX
int hwtsdemux = 0; // Carlos 2010/05/19
#ifdef SUPPORT_NETWORKING_HWTS
unsigned int hwtsnet = 0; /* Carlos add 2011-01-18 */
#endif /* end of SUPPORT_NETWORKING_HWTS */
#endif /* end of HW_TS_DEMUX */
#ifdef SUPPORT_TS_SELECT_LOW_AUDIO
unsigned int ts_easy_audio = 0; /* Carlos add 2011-01-18 */
#endif /* end of SUPPORT_TS_SELECT_LOW_AUDIO */
#ifdef TS_STREAM_CHECK_RANGE
unsigned int ts_check_cache = 0xFFFFFFFF;
#endif /* end of TS_STREAM_CHECK_RANGE */ // add by carlos, 2011-03-15
float video_aspect_ratio = 0.0;	//Barry 2010-06-21
int sky_aspect = -1;	//Barry 2010-10-06
#ifdef DEBUG_ADD_PACKET_PTS
unsigned int showvideo = 0;
unsigned int showaudio= 0;
unsigned int showsub= 0;
unsigned int showpts= 0;
#endif // end of DEBUG_ADD_PACKET_PTS

//+Skyviia_Vincent07202010
#ifdef CONFIG_DVDNAV
void *memBase1 = NULL;
struct skyfb_api_display_parm d_parm;
struct skyfb_api_display_status d_status;
unsigned int disp2_addr = 0;
unsigned int disp2_addr_phy = 0;
unsigned int disp2_size = 0;
extern int  vo_need_dvdnav_menu;
unsigned int dvdnav_btn_virtaddr[2];
unsigned int dvdnav_btn_phyaddr[2];
unsigned int dvd_btn_addr_idx=0;
extern DvdnavState dvdnavstate;
#endif /* CONFIG_DVDNAV */
//Skyviia_Vincent07202010+

extern int first_height;

#ifdef VO_SUB_THREAD
pthread_t thread_draw_osd;
volatile int draw_osd_cmdqueue=0;
static int draw_osd_thread_running=0;
static pthread_cond_t osd_cond;
static pthread_mutex_t osd_mtx, osd_idx_mutex;
#endif
//Polun 2011-05-20
int start_x = -1;
int start_y = -1;
int end_x = -1;
int end_y = -1;
extern int resampleao_fbflg;  //Polun 2011-06-13 modify resampleao_fbflg 
//Polun 2011-07-01 ++s
extern int video_1080_mbaff_flag;
unsigned int  ori_display_scale = 0xFFFFFFFF;
unsigned int  is_preinit = 0;
//Polun 2011-07-01 ++e

static int init_osd_param(void)
{
	unsigned int ul = 0;

	if (ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info) == -1) 
	{
		fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
		return -1;
	}

	osd_size = d_info.width * d_info.height;	
	osd_addr_phy = d_info.fb_base_addr + d_info.osd_offset;
/*
	printf("d_info.fb_size = %d\n", d_info.fb_size);
	printf("d_info.width = %d\n", d_info.width);
	printf("d_info.height = %d\n", d_info.height);
	printf("osd_size = %d\n", osd_size);
	printf("d_info.osd_offset = %08X\n", d_info.osd_offset);
	printf("osd_addr_phy = %08X\n", osd_addr_phy);
*/
	memBase0 = mmap(0, d_info.fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, skyfb_fd, 0);

	if((long)memBase0 == -1) 
	{
		fprintf(stderr, "Couldn't map memBase0\n");
		goto error;
	}

	//TODO: need to add the offset of FC's osd.
	if(d_info.width < 720)
		osd_addr = (unsigned char *)memBase0 + d_info.osd_offset + (d_info.width * 310);	//Barry 2010-10-05
	else
		osd_addr = (unsigned char *)memBase0 + d_info.osd_offset  + (720 * 310);	//Barry 2010-10-05

	usleep(500);

	o_comm.addr = osd_addr_phy;
	o_comm.size = osd_size;
	o_comm.palette_change = SKYFB_FALSE;
	ioctl(skyfb_fd, SKYFB_SET_OSD_COMM, &o_comm);
	
	o_parm.index = SKYFB_OSD1;
	o_parm.alpha = 15;	// 0~15;
	o_parm.status = SKYFB_ON;
	o_parm.start_x = 0; 	//set wanted position;
	o_parm.start_y = 0;	//set wanted position;
	o_parm.width = d_info.width;
	o_parm.height = d_info.height - o_parm.start_y;	//Fuchun 2009.12.14
	ioctl(skyfb_fd, SKYFB_SET_OSD_PARM, &o_parm);
					
	ul = SKYFB_ON;
	ioctl(skyfb_fd, SKYFB_SET_OSD_STATUS, &ul);

	//clean osd
	memset(osd_addr, 0, o_parm.width*o_parm.height);

	return 0;

error:
	memBase0 = NULL;
	if (skyfb_fd >= 0)
	    close(skyfb_fd);
	skyfb_fd = -1;
	return -1;
}

//Fuchun 2009.10.08
void uninit_osd(void)
{
	//memset(osd_addr, 0, o_parm.width*o_parm.height);
	struct skyfb_api_osd osddata;
	osddata.block = 3;
	osddata.alpha = 15;
	osddata.x = 0;
	osddata.y = 0;
	osddata.width = d_info.width;
	osddata.height = d_info.height;
	ioctl(skyfb_fd, SKYFB_OSD_ERASE, &osddata);
}

//+Skyviia_Vincent07202010
#ifdef CONFIG_DVDNAV
//static int init_disp2_param(void)
int init_disp2_param(void)
{
printf("--init disp2 for dvdnav--\n");

	if (ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info) == -1) 
	{
		fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
		return -1;
	}

//	if(0 == first_height)
//	{
//printf("first_height = 0\n");
//	    first_height = d_info.height;
//	    return 0;
//	}
//printf("d_info.width[%d],d_info.height[%d],d_info.rgb1_offset[%x]\n",d_info.width,d_info.height,d_info.rgb1_offset);
#if 0
	if(memBase1) 
	{
        printf("init_disp2_param():munmap-----\n");
		munmap(memBase1, d_info.fb_size);
		memBase1 = NULL;
	}
    d_info.width = 720;
    d_info.height = 480;

	memBase1 = mmap(0, d_info.fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    disp2_size = d_info.width * d_info.height * 4;

	if((long)memBase1 == -1) 
	{
		fprintf(stderr, "Couldn't map memBase1\n");
		goto error;
	}

	//disp2_addr = (unsigned char *)memBase1 + d_info.rgb1_offset;
	//disp2_addr_phy = d_info.fb_base_addr + d_info.rgb1_offset;
	disp2_addr = (unsigned char *)memBase1 + d_info.osd_offset;
	disp2_addr_phy = d_info.fb_base_addr+d_info.osd_offset;//johnnyke 20101021
	usleep(500);
#endif

#if 0
    d_parm.display = SKYFB_DISP2;
	d_parm.input_format = INPUT_FORMAT_ARGB;

    d_parm.start_x = 0;
	d_parm.start_y = 0;	//set wanted position;
    d_parm.width_in = d_info.width;
    d_parm.height_in = d_info.height;
    d_parm.stride = d_info.width;
    d_parm.alpha = 0x0;//0xf9;
    d_parm.y_addr = disp2_addr_phy;
    d_parm.u_addr = 0;
    d_parm.v_addr = 0;
    if (ioctl(skyfb_fd, SKYFB_SET_DISPLAY_PARM, &d_parm) == -1) {
        printf("SKYFB_SET_DISPLAY_PARM ioctl failed\n");
        return -1;
    }
    
    d_status.display = SKYFB_DISP2;
    d_status.status = SKYFB_ON;
    if (ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &d_status) == -1) {
        printf("SKYFB_SET_DISPLAY_STATUS ioctl failed\n");
        return -1;
    }
#endif
    
    unsigned int video_format = INPUT_FORMAT_422;
    
    /* set display output to argb */
    if (ioctl(skyfb_fd, SKYFB_SET_VIDEO_ARGB_FMT, &video_format) == -1) {
        printf("SKYFB_SET_VIDEO_ARGB_FMT ioctl failed\n");
        return -1;
    }
    
    /* clear disp2 */
//    memset(disp2_addr, 0x00, (d_info.width * d_info.height*4));

	return 0;

error:
	memBase1 = NULL;
	if (skyfb_fd >= 0)
	    close(skyfb_fd);
	skyfb_fd = -1;
	return -1;
}

void uninit_disp2(void)
{
//	memset(disp2_addr, 0, disp2_size);
}
#endif /* CONFIG_DVDNAV */
//Skyviia_Vincent07202010+

static int preinit(const char *arg)
{
//	printf("skyfb: preinit()\n");

	// Raymond 2009/08/11
    if (skyfb_fd == -1)
    {
	if( skydroid == 0 )
	{	
		skyfb_fd = open("/dev/fb0", O_RDWR);
		printf("open /dev/fb\n");
	}
	else
	{
		skyfb_fd = open("/dev/graphics/fb0", O_RDWR);
		printf("open /dev/graphics/fb\n");
	}
    }
    resampleao_fbflg = 0; //Polun 2011-06-13 modify resampleao_fbflg 
	if( skyfb_fd < 0 )
	{
		fprintf(stderr, "Unable to init skyfb\n");
		return -1; 
	}

	// Mark 2010/04/21
	// For testing force lock display!
	if (forcelock == 1) {
            if (ioctl(skyfb_fd, 0xffff100d/*SKYFB_LOCK_DISPLAY*/) == -1) {
                fprintf(stderr, "lock display ioctrl is failed!\n");
            }
	}

	if(arg) 
	{
		return ENOSYS;
	}

//	if(vo_need_osd)	//Fuchun 2009.12.15
	{
#if 1
#if 0
		if (ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info) == -1) 
		{
			fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
			return -1;
		}
		display_height = d_info.height;
		display_width = d_info.width;
#else
		unsigned int display_scale = 0;
		unsigned int orig_display_height = 0;
		unsigned int orig_display_width = 0;
		if(ioctl(skyfb_fd, SKYFB_GET_REAL_DISPLAY, &real_display) == -1)
		{
			fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
			return -1;
		}
		orig_display_height = real_display & 0x0000ffff;
		orig_display_width = (real_display >> 16) & 0x0000ffff;
		
		if(ioctl(skyfb_fd, SKYFB_GET_SCALE_ERS, &display_scale) == -1)
		{
			fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
			return -1;
		}
		display_scale = display_scale >> 16;
		display_height = orig_display_height/* - (orig_display_height*display_scale/100)*/;
		display_width = orig_display_width/* - (orig_display_width*display_scale/100)*/;
		x_res_shift = (orig_display_width - display_width)/2;
		y_res_shift = (orig_display_height - display_height)/2;
//		printf("@@@ display_scale[%d]  display_width[%d]  display_height[%d] @@@\n", display_scale, display_width, display_height);
#endif
//		color_num = 0;
		//ioctl(skyfb_fd, SKYFB_OSD_INIT);
#else
		if(init_osd_param() == -1)
		{
			fprintf(stderr, "init osd is failed!\n");
			return -1;
		}
#endif
	}


#ifdef CONFIG_DVDNAV
	if(vo_need_dvdnav_menu)
	    if(init_disp2_param() == -1)
		    return -1;
#endif /* CONFIG_DVDNAV */

	if (sky_aspect == -1)
	{
		//Barry 2010-06-21
		unsigned int ratio_value = RATIO_ORIGINAL;
		//printf("video_aspect_ratio=%f\n\n", video_aspect_ratio);
		if (1.76 <= video_aspect_ratio && video_aspect_ratio <= 1.79)
			ratio_value = RATIO_16_9;
		else if (1.32 <= video_aspect_ratio && video_aspect_ratio <= 1.34)
			ratio_value = RATIO_4_3;
		ioctl(skyfb_fd, SKYFB_SET_VIDEO_RATIO, &ratio_value);

	}
	else		//Barry 2010-10-06
	{
		unsigned int ratio_value = (unsigned int)sky_aspect;
		//printf("video_aspect_ratio=%f\n\n", video_aspect_ratio);
		ioctl(skyfb_fd, SKYFB_SET_VIDEO_RATIO, &ratio_value);
	}
    //Polun 2011-05-20
    //{status; left_x;right_x;top_y;bottom_y;}
     if((start_x >= 0) && (start_y >= 0) && (end_x <=  (((real_display >> 16) & 0x0000ffff)-1 ) )&& (end_y <= ((real_display & 0x0000ffff)-1) ))
     {
            unsigned int skyfb_fixed_video_pos[5] = {SKYFB_TRUE,start_x,end_x,start_y,end_y};  
            unsigned int ratio_value = RATIO_FIT;
            ioctl(skyfb_fd, SKYFB_SET_VIDEO_RATIO, &ratio_value);
            ioctl(skyfb_fd, SKYFB_SET_FIXED_VIDEO_POSITION, &skyfb_fixed_video_pos[0]);
      }
      //Polun 2011-07-01 ++s
      unsigned int output_mode;
      ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
      output_mode = output_mode >> 16;
      unsigned int  display_scale;
      if(is_preinit == 0)
      {
            if(ioctl(skyfb_fd, SKYFB_GET_SCALE_ERS, &display_scale) == -1)
            {
                 fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
                 return -1;
            }
            ori_display_scale = display_scale;
      }  
      if((output_mode == 0x04) && (video_1080_mbaff_flag == 1) && (!no_osd))
      {
                printf("===video is 1920x1080 MBAFF  and on 1080i 60HZ need rescale===\n");
                display_scale = 0;
                if(ioctl(skyfb_fd, SKYFB_SET_SCALE_ERS, &display_scale) == -1)
                {
                        fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
                        return -1;
                }
      }
      is_preinit = 1;
      //Polun 2011-07-01 ++e
	return 0;
}

static int
config(uint32_t width, uint32_t height, uint32_t d_width, uint32_t d_height, uint32_t flags, char *title, uint32_t format)
{
//	printf("skyfb: config() = %4d x %4d\n", width, height);

	image_width = width;
	image_height = height;
	return 0;
}

static int control(uint32_t request, void *data, ...)
{
//	printf("skyfb: control() = %X\n", request);

	switch (request) 
	{
	case VOCTRL_QUERY_FORMAT:
		return query_format(*((uint32_t*)data));
	}
	return VO_NOTIMPL;
}
//Polun 2011-07-01 ++s
void skyfb_reset_display_scale()
{
	printf("skyfb: skyfb_reset_display_scale()\n");
      unsigned int output_mode;
      ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
      output_mode = output_mode >> 16;
      if(output_mode == 0x04)  //1080i 60HZ
      {
             unsigned int  display_scale;
             if(ioctl(skyfb_fd, SKYFB_GET_SCALE_ERS, &display_scale) == -1)
                {
                        fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
                        return -1;
                }
                printf("ori_display_scale =%d display_scale = %d",ori_display_scale,display_scale);
                if((ori_display_scale != 0xFFFFFFFF) && (ori_display_scale != display_scale))
                {
                    display_scale = ori_display_scale;
                    printf("==========reset display scale ==========\n");
                    if(ioctl(skyfb_fd, SKYFB_SET_SCALE_ERS, &display_scale) == -1)
                    {
                        printf("==========reset display scale fail==========\n");
                        fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
                        return -1;
                    }
                    usleep(1000);
                } 
      }
      is_preinit = 0;

}
//Polun 2011-07-01 ++e

static void
uninit(void)
{
	printf("skyfb: uninit()\n");

//	if(vo_need_osd)
//		uninit_osd();		//Fuchun 2009.10.08
printf("draw_osd_thread_running=%d draw_osd_cmdqueue=%d\n", draw_osd_thread_running, draw_osd_cmdqueue);
        if (draw_osd_thread_running)
        {
            int vo_text_timeout_cnts = 1200, timeout_cnts = 50;
            draw_osd_thread_running = 2;
//printf("osd_mtx.__data.__count=%d\n", osd_mtx.__data.__count);
//printf("osd_mtx.__data.__lock=%d\n", osd_mtx.__data.__lock);
            while((draw_osd_cmdqueue > 0) && (vo_text_timeout_cnts-- > 0))
            {
                usleep(1000);
            }
//printf("new draw_osd_cmdqueue = %d   vo_text_timeout_cnts=%d\n", draw_osd_cmdqueue, vo_text_timeout_cnts);
            if (draw_osd_cmdqueue == 0)
                pwake_up(&osd_cond, &osd_mtx);
                
            while((draw_osd_thread_running == 2) && (timeout_cnts-- > 0))
            {
                usleep(1000);
            }
            if (draw_osd_thread_running == 3)
            {
printf("draw_osd_thread_running change to %d\n", draw_osd_thread_running);
                pthread_join(thread_draw_osd, 0);
            }
            draw_osd_thread_running = 0;
        }

//	if(vo_need_osd)
		uninit_osd();		//Fuchun 2009.10.08

#ifdef CONFIG_DVDNAV
	if(vo_need_dvdnav_menu) uninit_disp2();
	else
#endif
	
	if(skyfb_fd != -1) 
	{
		close(skyfb_fd);
		skyfb_fd = -1;
	}
}

static int draw_slice(uint8_t *image[], int stride[], int w,int h,int x,int y)
{
	return 0;
}

//Fuchun 2009.09.29
/***********************************************************************************/

static void draw_alpha(int x,int y, int w,int h, unsigned char* src, unsigned char *srca, int stride){
	int i,j;
	extern int sub_on;
#ifdef CONFIG_DVDNAV
	if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)	//Fuchun 2011.01.14 avoid draw on menu
		return;
#endif
#if 1
	struct skyfb_api_osd osddata;
	unsigned char *tmp_buf = (unsigned char *)calloc(1, w * h);
	
	if (tmp_buf == NULL)
	{
		printf("malloc memory error!!!\n");
		return;
	}
	if(vo_spudec == NULL || (dvdsub_id < 0 && vobsub_id < 0))
	{
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				if(vo_teletextsub == 1)
				{
					if ((y + i) >= 0 && srca[i*stride+j] > 0x10) 
						tmp_buf[j + i*w] = sub_color[1];
					else if((y + i) >= 0 && srca[i*stride+j] > 0x00 && srca[i*stride+j] <= 0x10)
						tmp_buf[j + i*w] = sub_color[color_num];
				}
				else
				{
					if ((y + i) >= 0 && srca[i*stride+j] > 0x00) 
					{
						if(src[i*stride+j] > 0x60)
							tmp_buf[j + i*w] = sub_color[color_num];
						else
							tmp_buf[j + i*w] = sub_color[1];
					}
				}

				if ((vo_teletextsub == 1) && (y + i) >= 0 && src[i*stride+j] > 0x10) 
				{
					if((y+i) == 0 || (y+i) == display_height -1 || (x+j) == 0 || (x+j) == display_width -1)
						tmp_buf[j + i*w] = sub_color[1];
					else if(src[(i-1)*stride+j] <= 0x20 || src[(i+1)*stride+j] <= 0x20 || src[i*stride+(j-1)] <= 0x20 || src[i*stride+(j+1)] <= 0x20)
						tmp_buf[j + i*w] = sub_color[1];
					else if(src[(i-1)*stride+j] <= 0x40 || src[(i+1)*stride+j] <= 0x40 || src[i*stride+(j-1)] <= 0x40 || src[i*stride+(j+1)] <= 0x40)
						tmp_buf[j + i*w] = sub_color[color_num]; //tmp_buf[j + i*w] = sub_color[5];
					else
						tmp_buf[j + i*w] = sub_color[color_num];
				}
			}
		}
	}
	else
	{
		extern int is_dvdnav;
		unsigned int is_decoding = spu_get_decodeing(vo_spudec);
		if(is_decoding || is_dvdnav)
		{
#ifdef FIX_SUBTITLE_FLICKING
			int cnt = 0;
#endif
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
	//Robert 20101103 use white background with black bolder -- by FuChun
#if 0
					if ((y + i) >= 0 && src[i*stride+j] >= 0x10) 
					{
						if(src[i*stride+j] < 0x78)
							tmp_buf[j + i*w] = sub_color[color_num];
						else if(src[i*stride+j] < 0x82)
							tmp_buf[j + i*w] = sub_color[5];
						else if(src[i*stride+j] < 0xB0)
							tmp_buf[j + i*w] = sub_color[7];
						else
							tmp_buf[j + i*w] = sub_color[1];
					}
#else
					if(is_dvdnav)
					{
#if 0
						if ((y + i) >= 0 && src[i*stride+j] > 0x00) 
						{
							if(src[i*stride+j] < 0x78)
								tmp_buf[j + i*w] = sub_color[1];
							else
								tmp_buf[j + i*w] = sub_color[color_num];
						}
#else
						if((y + i) >= 0 && src[i*stride+j] > 0x00)
							tmp_buf[j + i*w] = src[i*stride+j];
#endif
					}
					else if(is_decoding == 1)
					{
#ifdef DVB_TRUE_COLOR
						if ((y + i) >= 0 && src[i*stride+j] > 0x00) 
							tmp_buf[j + i*w] = src[i*stride+j];
#else
						if ((y + i) >= 0 && src[i*stride+j] >= 0x10) 
						{
							if(src[i*stride+j] < 0x78)
								tmp_buf[j + i*w] = sub_color[1];
							else if(src[i*stride+j] < 0x82)
								tmp_buf[j + i*w] = sub_color[color_num];
							else if(src[i*stride+j] < 0xB0)
								tmp_buf[j + i*w] = sub_color[color_num];
							else
								tmp_buf[j + i*w] = sub_color[color_num];
#ifdef FIX_SUBTITLE_FLICKING
							if(cnt == 0) usleep(1);
							cnt = (cnt + 1) & 0x3FF;
#endif
						}
#endif
					}
					else if(is_decoding == 2)
					{
						if(y+h > display_height)
							y = display_height-h-1;
						x = x + (display_width/2 - (x + w/2));
						if ((y + i) >= 0 && src[i*stride+j] > 0x00) 
							tmp_buf[j + i*w] = src[i*stride+j];
					}
					else
					{
						if ((y + i) >= 0 && src[i*stride+j] > 0x00) 
							tmp_buf[j + i*w] = src[i*stride+j];
					}
#endif
				}
			}
		}
		else
		{
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					if ((y + i) >= 0 && src[i*stride+j] > 0x40) 
					{
						if(src[i*stride+j] < 0x80)
							tmp_buf[j + i*w] = sub_color[1];
						else if(src[i*stride+j] < 0xB0)
							tmp_buf[j + i*w] = sub_color[7];
						else
							tmp_buf[j + i*w] = sub_color[0];
					}
				}
			}
		}
	}
	if(!sub_on) return;
	osddata.block = 3;
	if(transport_float)
	{
		printf("### transport_float[%f] ###\n", transport_float);
		osddata.alpha = 15*transport_float;
		transport_float = 0.0;
	}
	else
		osddata.alpha = 15;
	osddata.x = x+x_res_shift;
	osddata.y = y+y_res_shift;
	osddata.width = w;
	osddata.height = h;
	osddata.data_addr = (uint32_t)&(tmp_buf[0]);
	ioctl(skyfb_fd, SKYFB_OSD_FILL, &osddata);
	free((void *)tmp_buf);
#else

	if(vo_spudec == NULL)
	{
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				if ((y + i) >= 0 && src[i*stride+j] > 0x40) 
				{
					if((y+i) == 0 || (y+i) == o_parm.height -1 || (x+j) == 0 || (x+j) == o_parm.width -1)
						osd_addr[x + j + (y+i)*display_width] = sub_color[1];
					else if(src[(i-1)*stride+j] <= 0x40 || src[(i+1)*stride+j] <= 0x40 || src[i*stride+(j-1)] <= 0x40 || src[i*stride+(j+1)] <= 0x40)
						osd_addr[x + j + (y+i)*display_width] = sub_color[1];
					else
						osd_addr[x + j + (y+i)*display_width] = sub_color[color_num];
				}
			}
		}
	}
	else
	{
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				if ((y + i) >= 0 && src[i*stride+j] > 0x40) 
				{
					if(src[i*stride+j] < 0xB0)
						osd_addr[x + j + (y+i)*display_width] = sub_color[1];
					else
						osd_addr[x + j + (y+i)*display_width] = sub_color[color_num];
				}
			}
		}
	}
#endif

//TODO: wait for the way that how to set display mode, this is for I-mode.
#if 0
	printf("x %d	y %d	w %d	h %d\n", x, y, w, h);
	if(dvd_device == NULL)
	{
		for (i = 0; i < h; i++)
		{
			if(i % 2 == 0)
			{
				for (j = 0; j < w; j++)
				{
					if ((y+i/2+h/2-o_parm.height/2-1) >= 0 && src[i*stride+j] > 0x40) 
					{
						if((y+i/2+h/2-o_parm.height/2-1) == 0 || (y+i/2+h/2-o_parm.height/2-1) == o_parm.height -1 || (x+j) == 0 || (x+j) == o_parm.width -1)
							osd_addr[x + j + (y+i/2+h/2-o_parm.height/2-1)*display_width] = sub_color[1];
						else if(src[(i-1)*stride+j] <= 0x40 || src[(i+1)*stride+j] <= 0x40 || src[i*stride+(j-1)] <= 0x40 || src[i*stride+(j+1)] <= 0x40)
							osd_addr[x + j + (y+i/2+h/2-o_parm.height/2-1)*display_width] = sub_color[1];
						else
							osd_addr[x + j + (y+i/2+h/2-o_parm.height/2-1)*display_width] = sub_color[color_num];
					}
				}
			}
			else
			{
				for (j = 0; j < w; j++)
				{
					if ((y+i/2+h/2) >= 0 && src[i*stride+j] > 0x40) 
					{
						if((y+i/2+h/2) == 0 || (y+i/2+h/2) == o_parm.height -1 || (x+j) == 0 || (x+j) == o_parm.width -1)
							osd_addr[x + j + (y+i/2+h/2)*display_width] = sub_color[1];
						else if(src[(i-1)*stride+j] <= 0x40 || src[(i+1)*stride+j] <= 0x40 || src[i*stride+(j-1)] <= 0x40 || src[i*stride+(j+1)] <= 0x40)
							osd_addr[x + j + (y+i/2+h/2)*display_width] = sub_color[1];
						else
							osd_addr[x + j + (y+i/2+h/2)*display_width] = sub_color[color_num];
					}
				}
			}
		}
	}
	else
	{
		for (i = 0; i < h; i++)
		{
			if(i % 2 == 0)
			{
				for (j = 0; j < w; j++)
				{
					if ((y+i/2+h/2-o_parm.height/2-1) >= 0 && src[i*stride+j] > 0x40) 
					{
						if(src[i*stride+j] < 0xB0)
							osd_addr[x + j + (y+i/2+h/2-o_parm.height/2-1)*display_width] = sub_color[1];
						else
							osd_addr[x + j + (y+i/2+h/2-o_parm.height/2-1)*display_width] = sub_color[color_num];
					}
				}
			}
			else
			{
				for (j = 0; j < w; j++)
				{
					if ((y+i/2+h/2) >= 0 && src[i*stride+j] > 0x40) 
					{
						if(src[i*stride+j] < 0xB0)
							osd_addr[x + j + (y+i/2+h/2)*display_width] = sub_color[1];
						else
							osd_addr[x + j + (y+i/2+h/2)*display_width] = sub_color[color_num];
					}
				}
			}
		}
	}
#endif
}

static void clear_alpha(int x0,int y0, int w,int h) {
	int l;

#if 1
	struct skyfb_api_osd osddata;
	osddata.block = 3;
	osddata.x = x0+x_res_shift;
	osddata.y = y0+y_res_shift;
	osddata.width = w;
	osddata.height = h;
	ioctl(skyfb_fd, SKYFB_OSD_ERASE, &osddata);
#else 
	for(l = 0 ; l < h ; l++) 
	{
		if(y0 + l >= 0) memset(osd_addr + (y0 + l) * display_width + x0, 0x00,w);	//Fuchun 2009.12.14
	}
#endif
	
//TODO: wait for the way that how to set display mode, this is for I-mode.
#if 0
	for(l = 0 ; l < h ; l++) 
	{
		if(l % 2 == 0)
		{
			if(y0+l/2+h/2-o_parm.height/2 >= 0)
				memset(osd_addr + (y0+l/2+h/2-o_parm.height/2-1) * display_width + x0, 0x00,w);
		}
		else
		{
			if(y0+l/2+h/2 >= 0) 
				memset(osd_addr + (y0+l/2+h/2) * display_width + x0, 0x00,w);
		}
	}
#endif
}

int vo_get_osd_thread_status(void)
{
    return draw_osd_thread_running;
}

void clear_osd(void);
#ifndef VO_SUB_THREAD
static void draw_osd(void)
{
	extern int sub_on;
	if(!sub_on) return;
//	vo_remove_text(display_width, display_height, clear_alpha);
	clear_osd();
	vo_draw_text(display_width, display_height, draw_alpha);
}
#else
void do_osd_pthread(void)
{
	printf("%s thread started, tid %d\n", __func__, syscall(SYS_gettid));
	extern int speed_mult;
	while(1)
	{
//	vo_remove_text(display_width, display_height, clear_alpha);
//printf("@@@ osd thread draw_osd_thread_running=%d 01\n", draw_osd_thread_running);
		if (draw_osd_thread_running == 1)
		{
		    if (draw_osd_cmdqueue > 0)
		    {
//printf("@@@ osd thread draw_osd_thread_running=%d 01-2\n", draw_osd_thread_running);

//			clear_osd();	//Fuchun 2010.11.08 disable, do it on different types
			vo_draw_text(display_width, display_height, draw_alpha);
//printf("@@@ osd thread draw_osd_thread_running=%d 01-3\n", draw_osd_thread_running);
//			draw_osd_cmdqueue--;

#ifdef CONFIG_DVDNAV
			if(speed_mult != 0 || dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
				clear_osd();
#else
			if(speed_mult != 0)
				clear_osd();
#endif

			draw_osd_cmdqueue = 0;
//printf("@@@ osd thread draw_osd_thread_running=%d 02\n", draw_osd_thread_running);

                    }
                    else
                    {
//printf("@@@ osd thread draw_osd_thread_running=%d 03\n", draw_osd_thread_running);

			pwait_event(&osd_cond, &osd_mtx);
//printf("@@@ osd thread draw_osd_thread_running=%d 04\n", draw_osd_thread_running);

                    }
                }
                else if (draw_osd_thread_running == 2)
		{
		    printf("== %s thread Quit , tid %d ==\n", __func__, syscall(SYS_gettid));
		    draw_osd_thread_running = 3;
		    break;
		}
	}
}

static void draw_osd(void)
{
	extern int sub_on;
	if(!sub_on) return;

	if (draw_osd_thread_running == 0)
	{
		pthread_mutex_init(&osd_mtx, NULL);
		pthread_cond_init(&osd_cond, NULL);
		draw_osd_cmdqueue = 1;
		pthread_create( &thread_draw_osd, NULL, (void*)do_osd_pthread, NULL);

		draw_osd_thread_running = 1;
		
	}
	else
	{
//		draw_osd_cmdqueue++;
		draw_osd_cmdqueue = 1;
		pwake_up(&osd_cond, &osd_mtx);
	}
}
#endif

/************************************************************************************/

static void
flip_page(void)
{
}

static int
draw_frame(uint8_t *src[])
{
	return 0;
}

static int
query_format(uint32_t format)
{
	printf("skyfb: query_format() = %X\n", format);
	
	return VFCAP_CSP_SUPPORTED;
}

static void check_events(void)
{
}

void skyfb_set_output_mode(int mode, int deint_flag)
{
	unsigned int flag = SKYFB_ON;
	int ret;

	if (mode == 0)
	{
		flag = SKYFB_OFF;
	}
        else
	{
		if (deint_flag > 0)
		{
			ret = ioctl(skyfb_fd, SKYFB_SET_DEINTERLACE_STATUS, &flag);
		}
		ret = ioctl(skyfb_fd, SKYFB_SET_DEDICATED_MODE, &mode);
//printf("skyfb_set_output_mode ret=%d\n", ret);
	}
}

//Fuchun 2010.08.18
void skyfb_set_display_status(int flag)
{
	struct skyfb_api_display_status display_status;
	static int lock_flag = 0;
	int ret;
#if 1
	display_status.display = SKYFB_DISP1;
	if(!flag)
	{
		lock_flag = 1;
		display_status.status = SKYFB_OFF;
		ret = ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &display_status);
		printf("skyfb_set_display_status: SKYFB_OFF\n");
	}
	else
	{
		lock_flag = 0;
		display_status.status = SKYFB_ON;
		ret = ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &display_status);
		printf("skyfb_set_display_status: SKYFB_ON\n");
	}
#else
	if(!flag)
	{
		display_status.display = SKYFB_DISP1;
		lock_flag = 1;
		display_status.status = SKYFB_OFF;
		ret = ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &display_status);
		printf("skyfb_set_display_status: SKYFB_OFF\n");
	}
#endif
}

#if 1
void skyfb_set_thumnail_mode_enable()
{
	struct skyfb_api_video_thumbnail v_thumb;

        ioctl(skyfb_fd, SKYFB_UNLOCK_DISPLAY);

	v_thumb.flag =1;
#if 1
	v_thumb.start_x= 100;
	v_thumb.start_y= 100;
	v_thumb.width = 400;//640;
	v_thumb.height = 300;//480;
	v_thumb.mode =0; /*scale mode*/
#endif
	//v_thumb.realw;		//UI don't care this
	//v_thumb.realh;		//UI don't care this
	
	ioctl(skyfb_fd, SKYFB_VIDEO_THUMBNAIL, &v_thumb);
    d_status.display = SKYFB_DISP2;
    d_status.status = SKYFB_ON;
    if (ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &d_status) == -1) {
        printf("SKYFB_SET_DISPLAY_STATUS ioctl failed\n");
        return -1;
    }

	return;
}
#endif

void skyfb_set_thumnail_mode_disable()
{
	struct skyfb_api_video_thumbnail v_thumb;

	v_thumb.flag =0;
	
	
	ioctl(skyfb_fd, SKYFB_VIDEO_THUMBNAIL, &v_thumb);
	return;
}

void draw_ass_osd(char *tmp_buf, int x, int y, int w, int h)
{
	extern int sub_on;
	if(!sub_on) return;
	struct skyfb_api_osd osddata;
	osddata.block = 3;
	osddata.alpha = 15;
	osddata.x = x;
	osddata.y = y;
	osddata.width = w;
	osddata.height = h;
	osddata.data_addr = (uint32_t)&(tmp_buf[0]);
	ioctl(skyfb_fd, SKYFB_OSD_FILL, &osddata);
}

void clear_osd(void)
{
	struct skyfb_api_osd osddata;
	unsigned char tmp_buf;
	osddata.block = 3;
	osddata.x = 0;
	osddata.y = 0;
	osddata.width = 1;
	osddata.height = 1;
	osddata.data_addr = (uint32_t)&(tmp_buf);
	ioctl(skyfb_fd, SKYFB_OSD_FILL, &osddata);
}
