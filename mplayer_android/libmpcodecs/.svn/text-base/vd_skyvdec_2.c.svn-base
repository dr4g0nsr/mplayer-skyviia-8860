
/*****************************************************************************
 * Includes
 ****************************************************************************/



#include <stdio.h>
#include <stdlib.h>


#include "config.h"
#include "mp_msg.h"

#include "vd_internal.h"
#include "m_option.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "sky_vdec_2.h"
#include "libmpdemux/salloc.h"
#include <linux/ioctl.h>    /* needed for the _IOW etc stuff used later */
#include "libvo/sky_api.h"
#include "libmpdemux/demuxer.h"

//FILE * dumpfile=NULL;

//JF 1031
#ifdef CMV_WORKAROUND
extern void cmv_workaround_uninit(void);
#endif

/*****************************************************************************
 * Module structure definition
 ****************************************************************************/

static vd_info_t info =
{
    "Skyviia video decoder",
    "skyvdec",
    "Raymond Chen",
    "Raymond Chen",
    "Multi-format video decoder"
};

LIBVD_EXTERN(skyvdec)

#define VDEC_DEBUG
//20110615 Robert fine tune AVsync for DVD
int last_pulldown32_vft_diff = 0;
unsigned int sky_disp_rate = 0;
static double last_pulldown32_pts=0;
static int last_pulldown32_ptsidx = 0;
static unsigned int NextVideoBufIdx=0;
extern vdec_shm_t *svsd_state;
extern int hwtsdemux;
extern int cvq_threshold;
extern double force_fps;
extern int sky_telecine;
extern int sky_repeatfield_cnt;
extern int sky_repeatfield_flag;
vdec_init_t vdec_init_info;
vdec_init2_t vdec_init2_info;	//for kernel display workaround flag
vdec_dec_t vdec_dec_info;
static BufParams vd_dpb_parm;
extern vdec_shm_t *svsd_state;
extern int skyfb_fd;

int id = 0;
int skyfd = -1;
static int vd_memfd = -1;
#ifdef SUPPORT_DIVX_DRM
int is_ICT = 0;
#endif /* end of SUPPORT_DIVX_DRM */

unsigned int mem_addr = 0;
unsigned int mem_size = 0;

unsigned int mmap_physAddr=0;	/*johnnyke 20100903*/
void *mmap_virtAddr = NULL; /*johnnyke 20100903*/
unsigned int mmap_size = 0; /*johnnyke 20100903*/

unsigned int mmap_size_dvdnav = 0; //Skyviia_Vincent10222010

unsigned int SallocBufferSize = (4 << 20);
void *SallocVirtualBase = NULL;

void *SallocPhysicalBase = NULL;
//unsigned int SallocPhysicalBase = 0; /*johnnyke 20100803*/
static volatile void *sky_regbase = 0;
static volatile unsigned int *sky_clkreg = 0;

extern unsigned char *VideoBuffer[3];
//static unsigned int VideoPhysAddr[3] = {0,0,0}; /*johnnyke 20100803*/
//static unsigned char *vbuf = NULL;
//static unsigned int phyAddr = 0;	/*johnnyke 20100803*/
unsigned int buf_idx = 0;

void *PFMVirtualBase = NULL;
void *PFMPhysicalBase = NULL;

int file_format = -1;
unsigned int standard = 0;

unsigned int fcnt = 0;	// Raymond 2009/07/09

unsigned int stthumb_mode = 0;
static int stthumb_got_begin = 0;
unsigned int thumbnail_mode = 0;	// Raymond 2009/09/25

unsigned int th_width = 0;	// Raymond 2009/09/25
unsigned int th_height = 0;	// Raymond 2009/09/25


//Robert 20100712 Video Frame Time history
static double sky_vft_history[MAX_SKY_VFT_BUFFER];
double sky_vpts_history[MAX_SKY_VFT_BUFFER];
int sky_vpts_total_cnts=0;
int sky_vft_idx=0;
extern unsigned int VideoBufferIdx;
extern int use_PP_buf;
extern unsigned int sky_vd_reserved_size;
extern int sky_vdec_vdref_num;
extern unsigned int no_osd;
extern int quiet;

extern int num_reorder_frames; //Fuchun 2010.05.17
extern int h264_frame_mbs_only;	//Robert 20100604
extern int wait_video_or_audio_sync;
extern int seek_sync_flag, dvbt_wait_sync;
int lock_flag = 1;
//Barry 2010-08-11
extern int ts_avc1_pts_queue_cnt;
extern float *ts_avc1_pts_queue;
extern int num_ref_frames;
extern int h264_reorder_num;
extern int check_fast_ts_startcode;
extern int init_disp2_param(void);
extern int  vo_need_dvdnav_menu;
#ifdef DVBT_USING_NORMAL_METHOD
int start_to_display = 0;
#endif // end of DVBT_USING_NORMAL_METHOD

int first_height=0;
#include "stream/stream_dvdnav.h"
extern DvdnavState   dvdnavstate;
extern unsigned int dvdnav_btn_virtaddr[2];
extern unsigned int dvdnav_btn_phyaddr[2];
extern unsigned int dvd_btn_addr_idx;
extern int is_dvdnav;
extern int dvdnav_should_do_alpha;
extern int dvdnav_btn_change;

extern int slave_mode;  //SkyViia_Vincent03232010
extern int slave_mode_quit; //SkyViia_Vincent03232010

// Raymond 2010/04/14
extern unsigned char* asfHeader;
extern unsigned int asfHeaderSize;

unsigned int operation_mode=STANDALONE_MODE;	/*johnnyke 20100412*/
unsigned int standalone_op_mode=1;					/*johnnyke 20100412*/
unsigned int pingpong_op_mode=0;					/*johnnyke 20100412*/
unsigned int multibuf_op_mode=0;					/*johnnyke 20100412*/
unsigned int disable_disp_deint = 0;				//Fuchun 2010.11.17

int		    rotation_degree=0;					/*johnnyke 20100412*/
float		    zoom_ratio=1.0;						/*johnnyke 20100412*/
unsigned int scaling=0;							/*johnnyke 20100412*/
unsigned int deinterlacing=0;						/*johnnyke 20100412*/
unsigned int rgb_converstion=0;					/*johnnyke 20100412*/
unsigned int dithering_en=0;						/*johnnyke 20100412*/
unsigned int pulldown32=0;						/*johnnyke 20100430*/
unsigned int buf_num=3;						/*johnnyke 20100503*/
unsigned int alpha_blend=0;
unsigned int yuv422_outFmt=0;
//extern int speed_mult;						/*johnnyke 20100524*/

unsigned int non_bframe_cnt = 0;		// Raymond 2010/06/02
unsigned int resync_video = 0;		// Raymond 2010/06/03

unsigned int isRV8 = 0;				// Raymond 2010/06/10

vdec_th_res_t thumbnail_res; /*johnnyke 20100929*/
extern unsigned int disp2_addr_phy; /*johnnyke 20101020*/
extern int sky_aspect;
extern int fd;
int set_video_ratio_byuser = 0;

int sky_need_drop_b = 0;
extern unsigned int ts_h264_field_num;
extern unsigned int ts_h264_field_offset[8];
int sky_is_output_interlace = 0;
/*
unsigned int get_thumbnail(char *callback_str)
{
	return 1;
}
*/

// Raymond 2009/09/28
char *dump_yuv_file_name = NULL;
FILE *dump_yuv_file = NULL;

#undef DUMP_ES_STREAM
#ifdef DUMP_ES_STREAM
char *bitstream_name = "/data/stream.es";
FILE *bitstream_file = NULL;
#endif

/*void DumpYUV(void)
{

}//*/

// Raymond 2009/11/27
extern unsigned int skydroid, skyqt;
static int last_svsd_recv_count = 0;
char *parser_table_path = NULL;
int video_height = 0;
int video_width = 0;

// end Raymond 2009/11/27

#if 1	// Raymond 2009/06/24
int check_dma = 0;
char *check_file_name = NULL;
#endif
unsigned int vd_dpb_size = 0;	//Barry 2010-10-06
extern int StandaloneH264JitterBufferNum;	//Barry 2010-10-26
extern float video_aspect_ratio;

//Barry 2011-10-05
extern int svsd_ipc_callback(vdec_dec_t *pvdec);
extern int svsd_ipc_init_callback(vdec_init_t *pvdec);

//=============== sky ioctl ==============
void sky_set_trick_mode(int trick_mode)
{
//	ioctl(skyfd, SKY_VDEC_SET_FF_MODE, &trick_mode);
    printf("MPlayer: sky_set_trick_mode not yet implemented... %s:%d\n", __FILE__, __LINE__);
}

double sky_get_vft_total(int skip, int num)
{
    int start_vft_idx = sky_vft_idx;
    double total = 0;

    while(skip-- > 0)
    {
        if (start_vft_idx == 0)
            start_vft_idx = MAX_SKY_VFT_BUFFER - 1;
        else
            start_vft_idx--;
    }

    while(num--)
    {
        total += sky_vft_history[start_vft_idx];
        if (start_vft_idx == 0)
            start_vft_idx = MAX_SKY_VFT_BUFFER - 1;
        else
            start_vft_idx--;
    }
//	return ((double)total/90000.0);
    return total;
}

void sky_get_next_pts(double cur_pts, double *value)
{
    int start_vpts_idx = sky_vft_idx, next_vpts_idx, prev_vpts_idx;
    int num = 32;
    double ret_pts = 0.0;

    while(num)
    {

        if (start_vpts_idx == 0)
            prev_vpts_idx = MAX_SKY_VFT_BUFFER - 1;
        else
            prev_vpts_idx = start_vpts_idx - 1;

        if (sky_vpts_history[start_vpts_idx] == cur_pts)
        {

            if (start_vpts_idx == MAX_SKY_VFT_BUFFER - 1)
                next_vpts_idx = 0;
            else
                next_vpts_idx = start_vpts_idx + 1;
            if(sky_vpts_history[next_vpts_idx] == 0.0)
            {
                start_vpts_idx = prev_vpts_idx;
                continue;
            }
            ret_pts = sky_vpts_history[next_vpts_idx];
            break;
        }

        num--;
        start_vpts_idx = prev_vpts_idx;
    }
//printf("=== num %d   ret_pts %f ===\n", num, ret_pts);
    if(num == 0 && ret_pts == 0.0)
    {
        if(sky_vft_history[sky_vft_idx] > 0.0)
            ret_pts = cur_pts + sky_vft_history[sky_vft_idx];
        else
            ret_pts = cur_pts + 0.033;
    }

    *value = ret_pts;
}

void sky_get_pts_by_idx(int idx_offset, double *value)
{
    int start_vpts_idx = sky_vft_idx, vpts_idx_tmp;//next_vpts_idx, prev_vpts_idx;
    int num = 32;
    double ret_pts = 0.0;

    vpts_idx_tmp = (start_vpts_idx + idx_offset + MAX_SKY_VFT_BUFFER)%MAX_SKY_VFT_BUFFER;
    *value = sky_vpts_history[vpts_idx_tmp];
}

//SKY_VDEC_SET_VSYNC_STATUS
void sky_set_vsync_status(int vsync_status)
{
//printf("\n===> sky_set_vsync_status (%d)\n", vsync_status);
    ioctl(skyfd, SKY_VDEC_SET_VSYNC_STATUS, &vsync_status);
}

//Robert reserved function, try this later
void sky_set_display_lock(int lock_stat)
{
//printf("!! sky_set_display_lock(%d)  lock_flag=%d\n", lock_stat, lock_flag);
    int need_drop_cnts;
    if (lock_flag == 1 && lock_stat == 1)	//do lock display
    {
        lock_flag = 0;
//printf("=== try to lock display ===\n");
        //printf("== decode flags=%d\n", flags);
//		need_drop_cnts = -1;
//		ioctl(skyfd, SKY_VDEC_DROP_FRAME, &need_drop_cnts);
        need_drop_cnts = 30*60;
        ioctl(skyfd, SKY_VDEC_DROP_FRAME, &need_drop_cnts);
    }
    else if (lock_flag == 0 && lock_stat == 0)
    {
        lock_flag = 1;
//printf("=== try to unlock display ===\n");
        need_drop_cnts = 0;	//unlock
        ioctl(skyfd, SKY_VDEC_DROP_FRAME, &need_drop_cnts);
    }
}

void sky_set_display_drop_cnt(int drop_cnt)
{
//printf("sky_set_display_drop_cnt!! drop_cnt=%d\n", drop_cnt);
    ioctl(skyfd, SKY_VDEC_DROP_FRAME, &drop_cnt);
}

void sky_set_reset_display_queue(int flag)
{
//printf("sky_set_reset_display_queue1\n");
    int need_drop_cnts;
    if(flag == 0)
        need_drop_cnts = -1;
    else if(flag == 1) 	//for uninit
        need_drop_cnts = -2;
//printf("sky_set_reset_display_queue  need_drop_cnts = %d\n", need_drop_cnts);
    ioctl(skyfd, SKY_VDEC_DROP_FRAME, &need_drop_cnts);
    lock_flag = 1;
}

void sky_set_sync_frame_ridx(void)
{
//printf("\n===> !!! sky_set_sync_frame_ridx\n");
    int flag = 0, ret=0;
    ret = ioctl(skyfd, SKY_VDEC_SYNC_FRMAE_RIDX, &flag);
}

void sky_get_svread(vdec_svread_buf_t *nrb)
{
    ioctl(skyfd, SKY_VDEC_GET_SVREAD, nrb);
}

int sky_get_decode_done(void)
{
    int decode_done=0;
//	ioctl(skyfd, SKY_VDEC_GET_DECODE_DONE, &decode_done);
#if 0
    printf("MPlayer: %s not yet implemented... %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
    decode_done = 1;
#else
	int svsd_video_cnts = svsd_state->queue_length;
	svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);
	vdec_svread_buf_t nrb;
	svread(0, &nrb, 0);
	if(svsd_video_cnts > 0 || nrb.qlen[2] > 0)
		decode_done = 0;
	else
		decode_done = 1;
#endif
    return decode_done;
}

int sky_get_decode_num_frames(void)
{
    unsigned int qlen[4];
    ioctl(skyfd, SKY_VDEC_GET_DQUEUE_LEN, qlen);
//	printf("get queue :%d  %d  %d  %d\n", qlen[0], qlen[1], qlen[2], qlen[3]);
    return  qlen[0];
}

unsigned int get_thumbnail(char *callback_str)
{
    printf("MPlayer: %s not yet implemented... %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
    return 1;
}

void sky_get_thumnail_res()
{
    memset(&thumbnail_res, 0, sizeof(vdec_th_res_t));

    if(ioctl(skyfd, SKY_VDEC_GET_THUMBNAIL_RES,&thumbnail_res) == -1)
    {
        printf("SKY_VDEC_GET_THUMBNAIL_RES ioctl failed\n");

        return;
    }
}

void sky_get_thumnail_res2(unsigned int width, unsigned int height)
{
    int fd = -1;
    unsigned int res;
    memset(&thumbnail_res, 0, sizeof(vdec_th_res_t));

    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
        return;
    }

    res = (width << 16) | height;
    if(ioctl(skyfb_fd, SKYFB_SET_VIDEO_RES, &res) == -1)
    {
        printf("SKYFB_SET_VIDEO_RES ioctl failed\n");
        return;
    }
    thumbnail_res.th_width = res >> 16;
    thumbnail_res.th_height = res & 0xffff;

    close(fd);
}

#ifdef SUPPORT_DIVX_DRM
unsigned int check_HD_output(void)
{
   int fd = -1;
   unsigned int output_mode;
   int ret = 0;
    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
    }
   else
   {
       if (ioctl(fd, SKYFB_GET_MODE, &output_mode) == -1)
           fprintf(stderr,"SKYFB_GET_MODE ioctl failed  errno[%d] str [%s]in [%s][%d]\n", errno, strerror(errno), __func__, __LINE__);
       else
       {
           output_mode = output_mode >> 16;
           /* 
            * Divx : B01 ~ B05 playback on 576P/I or 480P/I 
            * if output is not equal 1080p/i, return 1 
            * DISP_1080I_30FPS, // 0x04
            * DISP_1080I_25FPS, // 0x05
            * DISP_1080P_60HZ, // 0x0a
            * DISP_1080P_50HZ, // 0x0b
            * DISP_1080P_30HZ, // 0x0d
            * DISP_1080P_24HZ, // 0x0e
            */
           if (output_mode == 0x04 || output_mode == 0x05 || output_mode == 0x0a || output_mode == 0x0b || output_mode == 0x0d || output_mode == 0x0e)
           {
               ret = 0;
//             printf("@@@ In [%s][%d] we found 1080p/i [%.2x], don't need slow down the speed of CPU @@@\n", __func__, __LINE__, output_mode);
               
           }
           else
           {
               ret = 1;
               printf("@@@ In [%s][%d] we found other output [%.2x], need slow down the speed of CPU @@@\n", __func__, __LINE__, output_mode);
           }
       }
       close(fd);
   }
   return ret;
}
#endif /* end of SUPPORT_DIVX_DRM */


//unsigned int is_double_deint()	//Fuchun 2010.11.25
unsigned int is_double_deint(sh_video_t *sh) //Polun 2012-01-16 fixed DLNA test file O-MP2TS_SN_I-1.mpg playback not smooth.
{
    int fd = -1;
    unsigned int double_deint_flag;
    unsigned int output_mode;
    unsigned int disp_rate=0;
    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
        return 0;
    }

    ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
    if(ioctl(skyfb_fd, SKYFB_GET_DISP_FPS, &disp_rate) == -1)
    {
        fprintf(stderr,"SKYFB_GET_DISP_FPS ioctl failed\n");
        disp_rate = 0;
    }
    sky_disp_rate = disp_rate;
//	printf("##### display rate = %d #####\n", disp_rate);
    /*
    input : uint32_t
    output : mode << 16 | format

    enum DISP_MODE{
    //-- Interlace
    DISP_480I, // 0x00
    DISP_W480I, // 0x01
    DISP_576I, // 0x02
    DISP_W576I, // 0x03
    DISP_1080I_30FPS, // 0x04
    DISP_1080I_25FPS, // 0x05
    DISP_INTERLACE_TV = DISP_1080I_25FPS,

    //-- Progress
    DISP_480P, // 0x06
    DISP_W480P, // 0x07
    DISP_576P, // 0x08
    DISP_W576P, // 0x09
    DISP_1080P_60HZ, // 0x0a
    DISP_1080P_50HZ, // 0x0b
    DISP_720P_60HZ, // 0x0c
    DISP_1080P_30HZ, // 0x0d
    DISP_1080P_24HZ, // 0x0e
    DISP_TV_MAX = DISP_1080P_24HZ,

    //-- LCD
    DISP_800x600_LCD, // 0x0f
    DISP_800x600_TCON, // 0x10
    DISP_800x480_LCD, // 0x11
    DISP_1024x600_LCD, // 0x12
    DISP_800x600_AUO_LCD, // 0x13
    DISP_640x480_DSUB_60HZ, // 0x14
    DISP_800x600_DSUB_60HZ, // 0x15
    DISP_1024x768_DSUB_60HZ, // 0x16
    DISP_1366x768_DSUB_60HZ, // 0x17
    DISP_1280x1024_DSUB_60HZ, // 0x18
    DISP_1600x1200_DSUB_60HZ, // 0x19
    DISP_1920x1080_DSUB_60HZ, // 0x1a

    DISP_800x480_TCON, // 0x1b
    DISP_720P_50HZ, // 0x1c

    DISP_USER_DEFINED_PARAM,
    DISP_MAX,
    DISP_USER_DEFINED = 0xff,
    };
    */
    close(fd);

    output_mode &= 0x00ffffff;	//discard bit 24~31
    output_mode = output_mode >> 16;
    sky_is_output_interlace = 0;

    if(output_mode >= 0 && output_mode <= 5)
    {
        double_deint_flag = 0;	//i mode
        sky_is_output_interlace = 1;
    }
    else if(disp_rate <= 30)
    {
        double_deint_flag = 2;	//once deint
        printf("@@@@@ On2's once de-deinterlace @@@@@\n");
    }
    else
    {
        #if 0  //Polun 2012-01-16 fixed DLNA test file O-MP2TS_SN_I-1.mpg playback not smooth.
        double_deint_flag = 1;	//double deint
        printf("@@@@@ On2's double de-deinterlace @@@@@\n");
        #else
        if (sh->fps < 50.0)
        {
            double_deint_flag = 1;	//double deint
            printf("@@@@@ On2's double de-deinterlace @@@@@\n");
        }
        else
        {
            double_deint_flag = 2;	//once deint
            printf("@@@@@ On2's once de-deinterlace @@@@@\n");
        }  
        #endif
    }

    return double_deint_flag;
}

unsigned int is_pulldown32(sh_video_t *sh)
{
    int fd = -1;
    unsigned int output_mode;
    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
        return 0;
    }

    ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
    close(fd);

    output_mode &= 0x00ffffff;	//discard bit 24~31
    output_mode = output_mode >> 16;
//    if(output_mode >= 0 && output_mode <= 3)
//20110620 Robert only 480i need to set pulldown32
    if(output_mode >= 0 && output_mode <= 1)
    {
//		if(((output_mode == 0 || output_mode == 1) && sh->disp_h <= 480) || ((output_mode == 2 || output_mode == 3) && sh->disp_h <= 576))	//Fuchun disable 2010.12.31
        {
            printf("@@@@@ On2's pulldown32 @@@@@\n");
            return 1;
        }
    }
    return 0;
}

unsigned int is_pulldown32_2(void)
{
    int fd = -1;
    unsigned int output_mode;
    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
        return 0;
    }

    ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
    close(fd);

    output_mode &= 0x00ffffff;	//discard bit 24~31
    output_mode = output_mode >> 16;
//    if(output_mode == 4 || output_mode == 5)
//20110620 Robert only 1080 60I need to set pulldown32_2
    if(output_mode == 4)
    {
        printf("@@@@@ On2's pulldown32_2 @@@@@\n");
        return 1;
    }
    return 0;
}

//20110615 Robert fine tune AVsync for DVD
void is_output_interlace()
{
    int fd = -1;
    unsigned int output_mode;
    unsigned int disp_rate=0;
    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }
    if (fd == -1)
    {
        printf("Can't open fd devices %d!!!\n", skydroid);
        return 0;
    }

    ioctl(skyfb_fd, SKYFB_GET_MODE, &output_mode) ;
    if(ioctl(skyfb_fd, SKYFB_GET_DISP_FPS, &disp_rate) == -1)
    {
        fprintf(stderr,"SKYFB_GET_DISP_FPS ioctl failed\n");
        disp_rate = 0;
    }
    sky_disp_rate = disp_rate;
    close(fd);

    output_mode &= 0x00ffffff;	//discard bit 24~31
    output_mode = output_mode >> 16;
    sky_is_output_interlace = 0;

    if(output_mode >= 0 && output_mode <= 5)
    {
        //i mode
        sky_is_output_interlace = 1;
    }
}



//Robert 20100712 use dynamic frame rate
//Robert 20100601 use SKY_VDEC_FRAMERATE_SET change display fps
//frate = mpctx->sh_video->fps*100
void sky_set_framerate(int frate)
{
    int framerate = frate;
    printf("## set display framerate to %d (frate=%d)##\n", framerate, frate);

    if(ioctl(skyfd, SKY_VDEC_FRAMERATE_SET, &framerate) == -1)
    {
        printf("SKY_VDEC_FRAMERATE_SET ioctl failed\n");
        return;
    }
}

//=============== sky ioctl ==============

int init_sky(void)
{
//	printf("init_sky = %d\n", skyfd);

    //+SkyViia_Vincent03232010
    if(slave_mode && skyfd > 0)  //in slave mode, no need to reopen skyvdec
        return skyfd;
    //SkyViia_Vincent03232010+


#if 1	/*johnnyke 20100903*/
    skyfd = open("/dev/skyvdec", O_RDWR);

    if ( skyfd== -1 )
    {
        printf("Unable to open /dev/skyvdec\n");
        goto error;
    }

    unsigned int dpb_id=DEFAULT_DPB_ID;
    BufParams dpb_parm;

    dpb_parm.id = dpb_id;

    if (ioctl(skyfd, SKY_VDEC_IOC_GET_MEM_ADDRESS, &dpb_parm) == -1)
    {
        fprintf(stderr,"SKY_VDEC_GET_MEM_ADDRESS ioctl failed\n");
        goto error;
    }

    memcpy(&vd_dpb_parm, &dpb_parm, sizeof(dpb_parm));
    mem_addr = dpb_parm.busAddress;

    dpb_parm.id = dpb_id;

    if (ioctl(skyfd, SKY_VDEC_IOC_GET_MEM_SIZE, &dpb_parm) == -1)
    {
        fprintf(stderr,"SKY_VDEC_IOC_GET_MEM_SIZE ioctl failed\n");
        goto error;
    }
    mem_size = dpb_parm.size;
    vd_dpb_parm.size = mem_size;	//Barry 2011-10-05 vd_store_thumbnail() use this parameter
    vd_memfd = open("/dev/mem", O_RDWR);

    if ( vd_memfd== -1 )
    {
        printf("Unable to open /dev/mem\n");
        goto error;
    }


#if 1 /*johnnyke 20100921*/

    if(is_dvdnav)
        mmap_size_dvdnav = VMEM_DVDNAV_SIZE; // add 3MB for dvdnav menu button
    else
        mmap_size_dvdnav = 0;

    mmap_size = VMEM_STREAM_SIZE+ mmap_size_dvdnav;
    vd_dpb_size = mem_size - mmap_size;
    mmap_physAddr = mem_addr + vd_dpb_size;

    //mmap_physAddr= mem_addr + (((dpb_parm.size>>20)-6) <<20);
    //vd_dpb_size = (((dpb_parm.size>>20)-6) <<20);	//Barry 2010-10-06

//	mmap_physAddr= mem_addr + (27 <<20);
//	mmap_physAddr= mem_addr + ((52-6) <<20);
    //mmap_size = /*mem_size*/(6<<20) ;
#endif
//Robert 20110225 map register area
    if (sky_regbase == 0)
    {
        sky_regbase = mmap(0, 0x2000, PROT_READ | PROT_WRITE, MAP_SHARED, vd_memfd, 0xc0fc8000);
    }
    sky_clkreg = (volatile unsigned int *)((unsigned int)sky_regbase + 0x1120);
    mmap_virtAddr = mmap(0, /*SallocBufferSize*/mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vd_memfd, mmap_physAddr);
    if (mmap_virtAddr == 0)
    {
        printf("mplayer: === bad mmap mmap_physAddr=0x%x  size=0x%x(%d)  virtAddr=0x%x\n", mem_addr, mem_size, mem_size, mmap_virtAddr);
    }

//	SallocPhysicalBase = (void *)mmap_physAddr;//(void *)vdec_init_info.streamBuf.phyAddr;
//	SallocVirtualBase =  (void *)mmap_virtAddr;// (void *)(vdec_init_info.streamBuf.phyAddr + ((unsigned int)mmap_virtAddr - mmap_physAddr));

    SallocPhysicalBase = (void *)mmap_physAddr +mmap_size_dvdnav;//(void *)vdec_init_info.streamBuf.phyAddr;
    SallocVirtualBase =  (void *)mmap_virtAddr +mmap_size_dvdnav;// (void *)(vdec_init_info.streamBuf.phyAddr + ((unsigned int)mmap_virtAddr - mmap_physAddr));

///////////////////////////////////////////////////
    if(is_dvdnav)
    {
        dvdnav_btn_phyaddr[0]= mmap_physAddr;
        dvdnav_btn_phyaddr[1]= mmap_physAddr + DVDNAV_BUTTON_SIZE + 1024;
        dvdnav_btn_virtaddr[0]= mmap_virtAddr;
        dvdnav_btn_virtaddr[1]= mmap_virtAddr + DVDNAV_BUTTON_SIZE + 1024;

        memset(mmap_virtAddr, 0x00, mmap_size);
    }

///////////////////////////////////////////////////

//	printf("vdec_alloc_info.virtAddr :%p, vdec_alloc_info.phyAddr:0x%08x, virtAddr-phyAddr:  0x%08x\n",vdec_init_info.streamBuf.virtAddr,vdec_init_info.streamBuf.phyAddr, ((unsigned int)vdec_init_info.streamBuf.virtAddr) -vdec_init_info.streamBuf.phyAddr);
    printf("SallocVirtualBase :%p, SallocPhysicalBase:0x%08x\n",SallocVirtualBase, SallocPhysicalBase);

    buf_idx = 0;
    VideoBuffer[0] = (unsigned char *) SallocVirtualBase;
    VideoBuffer[1] = (unsigned char *) SallocVirtualBase + (1 << 20);
    VideoBuffer[2] = (unsigned char *) SallocVirtualBase + (2 << 20);

    buf_idx = 0;
    VideoBuffer[0] = (unsigned char *) SallocVirtualBase;
    VideoBuffer[1] = (unsigned char *) SallocVirtualBase + (1 << 20);
    VideoBuffer[2] = (unsigned char *) SallocVirtualBase + (2 << 20);

    return skyfd;

error:
    SallocVirtualBase = NULL;
    mmap_virtAddr =NULL;
    if (skyfd >= 0)
        close(skyfd);
    if(vd_memfd >=0)
        close(vd_memfd);

    skyfd= -1;
    vd_memfd=-1;
    return -1;
#else


    return 1;
#endif
}

#define SCALE_SHIFT 16
#define ScaleColorY(col0, col1, part, all, div)	\
	(((  (col1-col0)*part + col0*all)*div) >> SCALE_SHIFT) 
#define ScaleColorUV(col0, col1, part, all, div)	\
	(col0==col1) ? (col0) : ((((  (col1-col0)*part + (col0-128)*all)*div) >> SCALE_SHIFT)+128) 

static unsigned int *ScaleLineCol0=0, *ScaleLineCol1=0, *ScaleLinePartx=0;

static void ScaleLine(unsigned char *srcline, unsigned char *dstline, unsigned short srcWidth, unsigned short dstWidth, unsigned short xdiv, int blank)
{
    unsigned int i;

//    if (blank == 0x10)	//Y
    if (blank == 0x0)	//Y
    {
      for (i=0; i<dstWidth; i++)
      {
        dstline[i] = ScaleColorY(srcline[ScaleLineCol0[i]], srcline[ScaleLineCol1[i]], ScaleLinePartx[i], dstWidth, xdiv);
      }
    }
    else
    {
      for (i=0; i<dstWidth*2; i+=2)
      {
        dstline[i] = ScaleColorUV(srcline[ScaleLineCol0[i/2]*2], srcline[ScaleLineCol0[i/2]*2+2], ScaleLinePartx[i/2], dstWidth, xdiv);
        dstline[i+1] = ScaleColorUV(srcline[ScaleLineCol0[i/2]*2+1], srcline[ScaleLineCol0[i/2]*2+3], ScaleLinePartx[i/2], dstWidth, xdiv);
      }
    }
}

void vd_store_thumbnail(void)
{
        struct skyfb_api_display_parm *d_ptr = &svsd_state->d_parm;
        void *mmap_virtrawAddr = NULL;
        char *bufname = 0;//malloc(128);
        unsigned char *y_start_ptr, *uv_start_ptr;
        unsigned int v_stride = d_ptr->stride;
        FILE *buffn = NULL;
        int dispx=160, dispy = 160;
        int input_format = INPUT_FORMAT_YCC420;//INPUT_FORMAT_422,

        if (skyfd < 0 || vd_memfd < 0)
                return;

        if (stthumb_mode & SKY_STORE_THUMB_END)
        {
//check need got startup thumbnail
            if (stthumb_mode & SKY_STORE_THUMB_BEGIN && stthumb_got_begin == 1)
            {
                if (last_svsd_recv_count < 1)
                    return ;
            }
        }

        if (is_dvdnav == 1)
        {
            if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
            {
                if (d_ptr->input_format == INPUT_FORMAT_422)
                {
                    input_format = INPUT_FORMAT_422;
                }
            }
        }

        if (thumbnail_res.th_width != 0 && thumbnail_res.th_height != 0)
        {
            dispx = thumbnail_res.th_width;
            dispy = thumbnail_res.th_height;
        }
        bufname = malloc(128);
        sprintf(bufname , "/tmp/vthumb.bgr");
/*
        printf("dump video frame to /tmp/vthumb.bgr\n");
        printf("Raw data in w/h:%d/%d out w/h:%d/%d stride:%d y_addr:0x%x u_addr:0x%x\n", 
            d_ptr->width_in, d_ptr->height_in, d_ptr->width_out, d_ptr->height_out,
            d_ptr->stride, d_ptr->y_addr, d_ptr->u_addr);
        printf("d_ptr->input_format=%d\n", d_ptr->input_format);
*/
        if (d_ptr->y_addr != 0 && d_ptr->u_addr != 0 && d_ptr->stride > 0 && d_ptr->stride <= 2000)
        {
            buffn = fopen(bufname, "w");
            if (buffn)
            {
                mmap_virtrawAddr = mmap(0, vd_dpb_parm.size, PROT_READ | PROT_WRITE, MAP_SHARED, vd_memfd, vd_dpb_parm.busAddress);
//                printf("vd_dpb_parm.busAddress=0x%x  vd_dpb_parm.size=%d\n", vd_dpb_parm.busAddress, vd_dpb_parm.size);
//Robert 20110301 sw scaling for thumbnail
//input_format == INPUT_FORMAT_YCC420
                if (mmap_virtrawAddr)// && input_format != INPUT_FORMAT_422)	//scaling for YUV420 semi-planar
                {
                    int i=0, j=0, partx=0, party=0, sidx=0;
                    int descale_factor = 1, smooth_div_factor = 1;
                    unsigned char *Yline0=malloc(dispx), *Yline1=malloc(dispx), *UVline=malloc(dispx*2);
                    unsigned char *Ytmp0=malloc(dispx), *Ytmp1=malloc(dispx);
                    unsigned char *outbuf = malloc(dispx*(dispy+1)*3);

                    unsigned char *y422 = malloc(d_ptr->width_out);
                    unsigned char *uv422 = malloc(d_ptr->width_out);
                    int rr, gg, bb , YY, Cb, Cr;
                    unsigned int xdiv = (1<<SCALE_SHIFT)/dispx;
                    unsigned int uvdiv = (1<<SCALE_SHIFT)/(dispx/2);
                    int new_j, uv_offset;
                    y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress);
                    uv_start_ptr = mmap_virtrawAddr + (d_ptr->u_addr - vd_dpb_parm.busAddress);

                    if (d_ptr->width_out <= dispx)
                    {
                        descale_factor = 1;
                    }
                    else
                    {
                        descale_factor = d_ptr->width_out / dispx;
                    }

                    partx = 0;
                    sidx = 0;
                    ScaleLineCol0 = malloc(sizeof(int) * (dispx+10));
                    ScaleLineCol1 = malloc(sizeof(int) * (dispx+10));
                    ScaleLinePartx = malloc(sizeof(int) * (dispx+10));

                    for (i=0; i< dispx; i++)
                    {
                        while(partx >= dispx)
                        {
                            sidx++;
                            partx -= dispx;
                        }
                        ScaleLineCol0[i] = sidx;
                        ScaleLineCol1[i] = sidx + 1;
                        ScaleLinePartx[i] = partx>>descale_factor;
                        //DBG("Partx[%d]=%d sidx=%d partx=%d dispx=%d origsrcWidth/4=%d\n", i, ScaleLin
//printf("Partx[%d]=%d sidx=%d partx=%d dispx=%d\n", i, ScaleLinePartx[i], sidx, partx, dispx);
                        partx += d_ptr->width_out/smooth_div_factor;
                    }

                    party = sidx = 0;
                                                                                                                                            
                    for (j=0; j<dispy; j++)
                    {
                        while (party >= dispy)
                        {
                            sidx ++;
                            party -= dispy;
                        }
                        party += d_ptr->height_out * 2;
                        
                        new_j = sidx;
						if (input_format == INPUT_FORMAT_422)
						{
							y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*new_j*2);
							for (i = 0; i <d_ptr->width_out/2; i++)
							{
								uv422[i*2]  = *y_start_ptr;
								y_start_ptr++;
								y422[i*2] = *y_start_ptr;
								y_start_ptr++;
								uv422[(i*2)+1]  = *y_start_ptr;
								y_start_ptr++;
								y422[(i*2)+1] = *y_start_ptr;
								y_start_ptr++;
							}
							ScaleLine(uv422, UVline, d_ptr->width_out, dispx/2, uvdiv, 1);
							ScaleLine(y422, Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
						}
						else
						{
                        ScaleLine(uv_start_ptr + (((new_j/2)) * v_stride), UVline, d_ptr->width_out, dispx/2, uvdiv, 1);
							ScaleLine(y_start_ptr + (new_j * v_stride), Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
						}

                        if (j == (dispy - 1))
                        {
                            memcpy(Ytmp1, Ytmp0, dispx);
                        }
                        else
                        {
							if (input_format == INPUT_FORMAT_422)
							{
								y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*(new_j+1)*2);
								for (i = 0; i <d_ptr->width_out/2; i++)
								{
									//uv422[i*2]  = *y_start_ptr;
									y_start_ptr++;
									y422[i*2] = *y_start_ptr;
									y_start_ptr++;
									//uv422[(i*2)+1]  = *y_start_ptr;
									y_start_ptr++;
									y422[(i*2)+1] = *y_start_ptr;
									y_start_ptr++;
								}
								ScaleLine(y422, Ytmp1, d_ptr->width_out, dispx, xdiv, 0);
							}
							else
                            ScaleLine(y_start_ptr + ((new_j+1) * v_stride), Ytmp1, d_ptr->width_out, dispx, xdiv, 0);
                        }

                        for (i=0; i<dispx; i++)
                        {
                            Yline0[i] = (Ytmp0[i] + Ytmp1[i])/2;
                        }
                        j++;
                        new_j++;

                        if (j == (dispy - 1))
                        {
							if (input_format == INPUT_FORMAT_422)
							{
								y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*(new_j)*2);
								for (i = 0; i <d_ptr->width_out/2; i++)
								{
									//uv422[i*2]  = *y_start_ptr;
									y_start_ptr++;
									y422[i*2] = *y_start_ptr;
									y_start_ptr++;
									//uv422[(i*2)+1]  = *y_start_ptr;
									y_start_ptr++;
									y422[(i*2)+1] = *y_start_ptr;
									y_start_ptr++;
								}
								ScaleLine(y422, Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
							}
							else
                            ScaleLine(y_start_ptr + ((new_j) * v_stride), Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
                            memcpy(Ytmp1, Ytmp0, dispx);
                        }
                        else
                        {
							if (input_format == INPUT_FORMAT_422)
							{
								y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*(new_j)*2);
								for (i = 0; i <d_ptr->width_out/2; i++)
								{
									//uv422[i*2]  = *y_start_ptr;
									y_start_ptr++;
									y422[i*2] = *y_start_ptr;
									y_start_ptr++;
									//uv422[(i*2)+1]  = *y_start_ptr;
									y_start_ptr++;
									y422[(i*2)+1] = *y_start_ptr;
									y_start_ptr++;
								}
								ScaleLine(y422, Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
								y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*(new_j+1)*2);
								for (i = 0; i <d_ptr->width_out/2; i++)
								{
									//uv422[i*2]  = *y_start_ptr;
									y_start_ptr++;
									y422[i*2] = *y_start_ptr;
									y_start_ptr++;
									//uv422[(i*2)+1]  = *y_start_ptr;
									y_start_ptr++;
									y422[(i*2)+1] = *y_start_ptr;
									y_start_ptr++;
								}
								ScaleLine(y422, Ytmp1, d_ptr->width_out, dispx, xdiv, 0);
							}
							else
							{
                            ScaleLine(y_start_ptr + ((new_j) * v_stride), Ytmp0, d_ptr->width_out, dispx, xdiv, 0);
                            ScaleLine(y_start_ptr + ((new_j+1) * v_stride), Ytmp1, d_ptr->width_out, dispx, xdiv, 0);
                        }
						}

                        for (i=0; i<dispx; i++)
                        {
                            Yline1[i] = (Ytmp0[i] + Ytmp1[i])/2;
                        }
//R    =  Y +            +   1.402*Cr   ==> 64*Y +         + 89*Cr
//G    =  Y - 0.34568*Cb - 0.71448*Cr   ==> 64*Y -  22*Cb  -  45*Cr
//B    =  Y +   1.771*Cb                ==> 64*Y +  113*Cb

                        uv_offset = 0;
                        for (i=0; i<dispx; i++)
                        {
                            YY = Yline0[i];
                            Cb = UVline[uv_offset] - 128;
                            Cr = UVline[uv_offset+1] - 128;
                            if (i%2==1)
                                uv_offset+=2;
                            //R
                            rr = (64*YY + 89*(Cr))/64;
                            if (rr >= 255)
                                rr = 255;
                            else if (rr < 0)
                                rr = 0;
                            outbuf[(j-1)*dispx*3+i*3+2] = rr;

                            //G
                            gg = (64*YY - 22*(Cb) - 45*(Cr))/64;
                            if (gg >= 255)
                                gg = 255;
                            else if (gg < 0)
                                gg = 0;
                            outbuf[(j-1)*dispx*3+i*3+1] = gg;

                            //B
                            bb = (64*YY + 113*(Cb))/64;
                            if (bb >= 255)
                                bb = 255;
                            else if (bb < 0)
                                bb = 0;
                            outbuf[(j-1)*dispx*3+i*3] = bb;

                            YY = Yline1[i];
                            //R
                            rr = (64*YY + 89*(Cr))/64;
                            if (rr >= 255)
                                rr = 255;
                            else if (rr < 0)
                                rr = 0;
                            outbuf[(j)*dispx*3+i*3+2] = rr;
                                
                            //G
                            gg = (64*YY - 22*(Cb) - 45*(Cr))/64;
                            if (gg >= 255)
                                gg = 255;
                            else if (gg < 0)
                                gg = 0;
                            outbuf[(j)*dispx*3+i*3+1] = gg;

                            //B
                            bb = (64*YY + 113*(Cb))/64;
                            if (bb >= 255)
                                bb = 255;
                            else if (bb < 0)
                                bb = 0;
                            outbuf[(j)*dispx*3+i*3] = bb;
                        }
                    }
                    fwrite(outbuf, dispx*dispy*3, 1, buffn);
                    
                    free(ScaleLineCol0);
                    free(ScaleLineCol1);
                    free(ScaleLinePartx);
                    free(Yline0);
                    free(Yline1);
                    free(UVline);
                    free(Ytmp0);
                    free(Ytmp1);
					free(y422);
					free(uv422);

                    free(outbuf);

                    munmap(mmap_virtrawAddr, vd_dpb_parm.size);
                }
				/*else if (mmap_virtrawAddr && input_format == INPUT_FORMAT_422)
				{
					int i = 0,j=0;
					unsigned char *outbuftemp = malloc(d_ptr->width_out*d_ptr->height_out*3);
					unsigned char *y422_1 = malloc(d_ptr->width_out/2);
					unsigned char *y422_2 = malloc(d_ptr->width_out/2);
					unsigned char *u422 = malloc(d_ptr->width_out/2);
					unsigned char *v422 = malloc(d_ptr->width_out/2);
					int r,g,b,u,v;
					//y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress);
					for (j = 0; j < d_ptr->height_out; j++)
					{
						y_start_ptr = mmap_virtrawAddr + (d_ptr->y_addr - vd_dpb_parm.busAddress) + (v_stride*j*2);
						for (i = 0; i <d_ptr->width_out/2; i++)
						{
							u422[i]  = *y_start_ptr;
							y_start_ptr++;
							y422_1[i] = *y_start_ptr;
							y_start_ptr++;
							v422[i]  = *y_start_ptr;
							y_start_ptr++;
							y422_2[i] = *y_start_ptr;
							y_start_ptr++;
						}

						for (i = 0; i <d_ptr->width_out/2; i++)
						{
							u = u422[i] - 128;
							v = v422[i] - 128;

							int rdif = v + ((v * 103) >> 8);
							int invgdif = ((u * 88) >> 8) +((v * 183) >> 8);
							int bdif = u +( (u*198) >> 8);
							r = y422_1[i] + rdif;
							g = y422_1[i] - invgdif;
							b = y422_1[i] + bdif;
							if (r >= 255)
								r = 255;
							else if (r < 0)
								r = 0;
							if (g >= 255)
								g = 255;
							else if (g < 0)
								g = 0;
							if (b >= 255)
								b = 255;
							else if (b < 0)
								b = 0;
							outbuftemp[j*d_ptr->height_out*3+i*3] = b;
							outbuftemp[j*d_ptr->height_out*3+i*3+1] = g;
							outbuftemp[j*d_ptr->height_out*3+i*3+2] = r;

							r = y422_2[i] + rdif;
							g = y422_2[i] - invgdif;
							b = y422_2[i] + bdif;
							if (r >= 255)
								r = 255;
							else if (r < 0)
								r = 0;
							if (g >= 255)
								g = 255;
							else if (g < 0)
								g = 0;
							if (b >= 255)
								b = 255;
							else if (b < 0)
								b = 0;
							outbuftemp[j*d_ptr->height_out*3+i*3+3] = b;
							outbuftemp[j*d_ptr->height_out*3+i*3+4] = g;
							outbuftemp[j*d_ptr->height_out*3+i*3+5] = r;
						}

					}

					FILE*re = fopen("/skydata/test.bgr","w");
					fwrite(outbuftemp, d_ptr->width_out*d_ptr->height_out*3, 1, re);
					fclose(re);

					free(y422_1);
					free(y422_2);
					free(u422);
					free(v422);
					free(outbuftemp);

					munmap(mmap_virtrawAddr, vd_dpb_parm.size);
				}*/
                fclose(buffn);
                if (stthumb_mode & SKY_STORE_THUMB_BEGIN)
                {
                    stthumb_got_begin = 1;
                }
                printf("MPlayer: dump thumbnail done\n");
                fflush(stdout);
            }
            free(bufname);
        }
}


// Raymond 2009/06/10
void uninit_sky(void)
{

    printf("uninit_sky(), skyfd=%d\n", skyfd);
//Robert 20110301 dump and scaling video frame to RGB thumbnail
    if (stthumb_mode & SKY_STORE_THUMB_END)
    {
        vd_store_thumbnail();
    }

    //+SkyViia_Vincent03232010
    if(slave_mode && !slave_mode_quit)//in slave mode, no need to close skyvdec, except quit cmd
        return;
    //SkyViia_Vincent03232010+
#if 1 /*johnnyke 20100903*/
    if(mmap_virtAddr)
    {
        /*johnnyke this munmap() will cause long time delay on FPGA!!!, why ?!, comment it temporally*/
        /*munmap(mmap_virtAddr, mmap_size);*/
        mmap_virtAddr= NULL;
    }
#endif

    SallocVirtualBase = NULL;
    mmap_virtAddr =NULL;
    if (skyfd >= 0) {
        ioctl(skyfd, SKY_VDEC_SET_UNUSED, SKY_VDEC_VIDEO);
        close(skyfd);
    }

    if(vd_memfd >=0)
        close(vd_memfd);

    skyfd= -1;
    vd_memfd=-1;
}

/*****************************************************************************
 * Video decoder API function definitions
 ****************************************************************************/

/*============================================================================
 * control - to set/get/query special features/parameters
 *==========================================================================*/

static int control(sh_video_t *sh,int cmd,void* arg,...)
{

    // Raymond 2009/07/08
    switch(cmd)
    {
    case VDCTRL_RESYNC_STREAM:
        fcnt = 0;
        non_bframe_cnt = 0;	// Raymond 2010/06/02
        resync_video = 1;		// Raymond 2010/06/03
        printf("\n===== resync_video = 1 =====\n\n");
        return CONTROL_TRUE;

        // Raymond 2010/03/18
    case VDCTRL_QUERY_UNSEEN_FRAMES:
        return num_reorder_frames + 10;		//Fuchun 2010.05.17
//		return 10;	// FIX ME!!
//		return avctx->has_b_frames + 10;

//Robert 20100526 add video queue delay calculate, fix A/V sync issue
    case VOCTRL_GET_QUEUE_LEN:
    {
        unsigned int qlen[4];

        vdec_svread_buf_t nrb;
        svread(0, &nrb, 0);
        memcpy(qlen, nrb.qlen, sizeof(qlen));

        sky_vdec_vdref_num=qlen[0];
//		printf("get queue :%d  %d  %d  %d\n", qlen[0], qlen[1], qlen[2], qlen[3]);
        return  qlen[0]+qlen[2];
    }
    break;

//Robert 20100617 get current display queue buffers
    case VOCTRL_GET_VQ_BUFFERS:
    {
        unsigned int qlen[4];

        vdec_svread_buf_t nrb;
        svread(0, &nrb, 0);
        memcpy(qlen, nrb.qlen, sizeof(qlen));

//		printf("get queue :%d  %d  %d  %d\n", qlen[0], qlen[1], qlen[2], qlen[3]);
        memcpy(&arg[0], qlen, sizeof(qlen));
        return  qlen[2];
    }
    break;

    default:
//		printf("skyvdec: control() - cmd = %X\n", cmd);
        break;
    }

    return(CONTROL_UNKNOWN);
}

/*============================================================================
 * init - initialize the codec
 *==========================================================================*/

static int init(sh_video_t *sh)
{
    int nRet = 0;

//	printf("skyvdec: init() - format = %X\n", sh->format);
#ifdef SUPPORT_DIVX_DRM
	unsigned int is_divx_codec = 0;
#endif /* end of SUPPORT_DIVX_DRM */	
    last_svsd_recv_count = 0;

    // decide standard
    switch (sh->format)
    {
// MJPEG
    case mmioFOURCC('A','V','R','n'):	// AVID
    case mmioFOURCC('A','V','D','J'):	// AVID
    case mmioFOURCC('M','J','P','G'):
    case mmioFOURCC('m','j','p','g'):
    case mmioFOURCC('j','p','e','g'):	// MOV Photo-JPEG
    case mmioFOURCC('m','j','p','a'):	// Apple MJPEG-A (Quicktime)
    case mmioFOURCC('d','m','b','1'):	// MJPEG by Matrox Rainbow Runner
        standard = VCODEC_JPEG;
        break;

// MPEG-1/2
    case 0x10000001:  // MPEG-1
    case 0x10000002:  // MPEG-2
    case mmioFOURCC('m','p','g','1'):
    case mmioFOURCC('m','p','g','2'):
    case mmioFOURCC('M','P','G','2'):
    case mmioFOURCC('M','P','E','G'):
        standard = VCODEC_MP2;
        break;

// MPEG-4 / H.263
    case mmioFOURCC('H','2','6','3'):	// H.263
    case mmioFOURCC('h','2','6','3'):
    case mmioFOURCC('s','2','6','3'):
    case mmioFOURCC('D','X','5','0'):
    case mmioFOURCC('d','x','5','0'):
    case mmioFOURCC('X','V','I','D'):	// XviD
    case mmioFOURCC('x','v','i','d'):
    case mmioFOURCC('X','v','i','D'):
    case mmioFOURCC('M','4','S','2'):
    case mmioFOURCC('m','4','s','2'):
    case mmioFOURCC('M','P','4','V'):
    case mmioFOURCC('m','p','4','v'):
    case mmioFOURCC('F','M','P','4'):
    case mmioFOURCC('f','m','p','4'):
    case 0x4:
    case 0x10000004: 				// mpeg 4 es
        standard = VCODEC_MP4;
        break;
    case mmioFOURCC('M','P','4','S'):	// ISO MPEG-4 Video V1
    case mmioFOURCC('m','p','4','s'):	// ISO MPEG-4 Video V1
        standard = VCODEC_MP4S;
        break;

// DivX 3
    case mmioFOURCC('D','I','V','3'):
    case mmioFOURCC('d','i','v','3'):
    case mmioFOURCC('M','P','4','3'):	//MSMPEG-4 v3
    case mmioFOURCC('m','p','4','3'):
        standard = VCODEC_DIVX3;
        break;

// DivX 4
    case mmioFOURCC('D','I','V','X'):
    case mmioFOURCC('d','i','v','x'):
        standard = VCODEC_DIVX4;
        break;

// Sorenson Spark
    case mmioFOURCC('F','L','V','1'):
        standard = VCODEC_SOR;
        break;

// H.264
    case mmioFOURCC('H','2','6','4'):
    case mmioFOURCC('h','2','6','4'):
    case mmioFOURCC('X','2','6','4'):
    case mmioFOURCC('x','2','6','4'):
    case mmioFOURCC('A','V','C','1'):
    case mmioFOURCC('a','v','c','1'):
    case mmioFOURCC('D','A','V','C'):
    case mmioFOURCC('d','a','v','c'):
    case 0x10000005:
        standard = VCODEC_H264;
        break;

// VC-1
    case mmioFOURCC('W','M','V','3'):
    case mmioFOURCC('w','m','v','3'):
        standard = VCODEC_WMV3;
        break;

    case mmioFOURCC('W','V','C','1'):
    case mmioFOURCC('w','v','c','1'):
    case mmioFOURCC('W','M','V','A'):
    case mmioFOURCC('W','M','V','P'):
        standard = VCODEC_WVC1;
        break;

// Real Video 8/9/10
    case mmioFOURCC('R', 'V', '3', '0'):
        standard = VCODEC_REAL;
        isRV8 = 1;				// Raymond 2010/06/10
        break;
    case mmioFOURCC('R', 'V', '4', '0'):
        standard = VCODEC_REAL;
        isRV8 = 0;				// Raymond 2010/06/10
        break;

// VP6
    case mmioFOURCC('V','P','6','F'):
        standard = VCODEC_VP6;
        break;
    case mmioFOURCC('V','P','6','A'):
        standard = VCODEC_VP6A;
        break;

    default:
        printf("skyvdec: init() - Unsupported video format\n");
        return 0;
    }

#ifdef SUPPORT_DIVX_DRM
	if(standard == VCODEC_MP4 || standard == VCODEC_DIVX3 || standard == VCODEC_DIVX4)
		is_divx_codec = 1;
#endif /* end of SUPPORT_DIVX_DRM */		

#ifdef DVBT_USING_NORMAL_METHOD
    start_to_display = 0;
#endif // end of DVBT_USING_NORMAL_METHOD
    // Init video decoder
    memset(&vdec_init_info, 0, sizeof(vdec_init_info));
    memset(&vdec_dec_info, 0, sizeof(vdec_dec_info));

    vdec_init_info.standard = standard;

    // Raymond 2010/04/09
    if( standard == VCODEC_DIVX3 || (sh->format == mmioFOURCC('M','P','4','S')) || (sh->format == mmioFOURCC('m','p','4','s')) || (sh->format == mmioFOURCC('s','2','6','3')) )
    {
        vdec_init_info.width = sh->disp_w;
        vdec_init_info.height = sh->disp_h;
        printf("VCODEC_DIVX3 : %4d x %4d\n", vdec_init_info.width, vdec_init_info.height);
    }
    else if (standard == VCODEC_WMV3)
    {
	if(asfHeaderSize > 0)
		memcpy(vdec_init_info.HeaderData, asfHeader, asfHeaderSize);


    }
    else if (standard == VCODEC_REAL)
    {
#ifdef FIX_RMVB_FLICKING
        extern int cvq_threshold;
        cvq_threshold = 2;
        printf("%s:%d set cvq to %d for VCODEC_REAL\n", __FILE__, __LINE__, cvq_threshold);
#endif
        memcpy(vdec_init_info.HeaderData, sh->rmvb_sequence_header, sh->rmvb_sequence_header_len);
    }

    /*pp cfg init*/	/*johnnyke 20100412*/

    if(pingpong_op_mode)
        vdec_init_info.ppCfgInit.operation_mode= COMBINED_PINGPONG_MODE;
    else if(multibuf_op_mode)
        vdec_init_info.ppCfgInit.operation_mode= COMBINED_MULTIBUFFER_MODE;
    else
        vdec_init_info.ppCfgInit.operation_mode= STANDALONE_MODE;


    vdec_init_info.ppCfgInit.rotation= rotation_degree;
    vdec_init_info.ppCfgInit.zoom_ratio= zoom_ratio;
    vdec_init_info.ppCfgInit.scaling_en= scaling;
    vdec_init_info.ppCfgInit.deinterlacing_en= deinterlacing;
    vdec_init_info.ppCfgInit.RGBconversion= rgb_converstion;
    vdec_init_info.ppCfgInit.dithering_en= dithering_en;
    vdec_init_info.ppCfgInit.yuv422Conv = yuv422_outFmt;
    vdec_init_info.pulldown32 = pulldown32;
    last_pulldown32_pts = 0.0;
    last_pulldown32_ptsidx = 0;
    last_pulldown32_vft_diff = 0;

    buf_num = sky_vd_reserved_size;
    if (standard == VCODEC_H264 && !multibuf_op_mode)	//Barry 2010-10-26
    {
        vdec_init_info.h264_jitterbufnum = StandaloneH264JitterBufferNum;
        //printf("vdec_init_info.h264_jitterbufnum = %d\n\n", vdec_init_info.h264_jitterbufnum);
    }

//Robert 20100712 reset Video Frame Time index
    sky_vft_idx = 0;


#if 1  /*johnnyke just for thumnail test*/

    if(!no_osd) {

//		skyfb_set_display_status(0);	//Fuchun 2010.08.18
        ioctl(skyfd, SKY_VDEC_VIDEO_MODE, NULL);

        int i;
        for(i = 0; i < MAX_SKY_VFT_BUFFER; i++)
        {
            sky_vpts_history[i] = 0.0;
            sky_vft_history[i] = 0.0;
        }
    }
    else { /*johnnyke 20100929*/

#if 0 /*this line just for mplayer testing, should be set by UI, and should be removed in the future!*/
        skyfb_set_thumnail_mode_enable();
#endif
        if (thumbnail_mode)
        {
            skyfb_set_thumnail_mode_enable();
        }
        skyfb_set_display_status(1);

        int retry_cnts = 3;
//Robert 20101111 check thumbail_res2 value
retry_get_thumb:
        sky_get_thumnail_res2(sh->disp_w, sh->disp_h);
        printf("MPlayer: sky_get_thumnail_res2 thumbnail_res %d/%d retry:%d\n", thumbnail_res.th_width, thumbnail_res.th_height, retry_cnts);

        if(thumbnail_res.th_width > 0  && thumbnail_res.th_height > 0) {
            vdec_init_info.thumbnail_mode = 1;
            vdec_init_info.thumbnail_res.th_width = thumbnail_res.th_width;
            vdec_init_info.thumbnail_res.th_height= thumbnail_res.th_height;
        } else {
            if (retry_cnts > 0)
            {
                sleep(1);
                retry_cnts--;
                goto retry_get_thumb;
            }
            else
            {
                return 0;
//			vdec_init_info.thumbnail_mode = 0;
//			vdec_init_info.thumbnail_res.th_width = 0;
//			vdec_init_info.thumbnail_res.th_height= 0;
            }
        }

    }

#else
    if(!no_osd)
        skyfb_set_display_status(0);	//Fuchun 2010.08.18

#endif

    video_height = sh->disp_h;
    video_width = sh->disp_w;


#ifdef SUPPORT_DIVX_DRM // johnnyke 20110408 only for divx certification 
	if(is_divx_codec)
	{
		struct skyfb_api_display_info d_info;

		if (ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info) == -1)
		{
			fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
			return -1;
		}

		unsigned int wh, scalar_w, scalar_h;
		wh = (((unsigned int)(sh->disp_w << 16)) | ((unsigned int)sh->disp_h));

		if (ioctl(skyfd, SKY_VDEC_GET_VIDEO_ORIG_SCALAR_INFO, &wh) == -1)
		{
			fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
			return -1;
		}
		scalar_w = (wh >> 16);
		scalar_h = (wh & 0xffff);
		//printf("####  scalar_w [%d] scalar_h[%d] sh->disp_w[%d] sh->disp_h [%d] [%s][%d] #####\n", scalar_w, scalar_h, sh->disp_w, sh->disp_h, __func__, __LINE__);
		if( (scalar_w > sh->disp_w  && scalar_h < sh->disp_h) || (scalar_w < sh->disp_w  && scalar_h > sh->disp_h)){
			vdec_init_info.thumbnail_mode = 1;
			vdec_init_info.thumbnail_res.th_width = scalar_w;
			vdec_init_info.thumbnail_res.th_height = sh->disp_h;
		}else{
			//printf("####  d_info.width: %d, d_info.height:%d\n",d_info.width, d_info.height);
			if(d_info.width> sh->disp_w *4 || d_info.height > sh->disp_h*4){ // limitation of display scaling
				int offset = 0;
				// do on2' scaling first
				vdec_init_info.thumbnail_mode = 1;
				if(d_info.width > sh->disp_w *4)	
				{
					offset = (sh->disp_w & 0xf);
					if (offset & 0x10)
						offset = 0;
					else
						offset = 0x10 - offset;
					//vdec_init_info.thumbnail_res.th_width = ((d_info.width /4)+7)&~7;
					vdec_init_info.thumbnail_res.th_width = (int) ((double)(scalar_w * (offset + sh->disp_w)/ 4) / (double)(sh->disp_w)) + 2;
					//printf("@@@@ In [%s][%d] offset is [%x] sh->disp_w [%d] new_source [%d]$$$$\n", __func__, __LINE__, offset, sh->disp_w, sh->disp_w + offset);
				}
				else
					vdec_init_info.thumbnail_res.th_width = sh->disp_w;
				if(d_info.height >= sh->disp_h*4)
				{
					offset = (sh->disp_h & 0xf);
					if (offset & 0x10)
						offset = 0;
					else
						offset = 0x10 - offset;
					//printf("@@@@ In [%s][%d] offset is [%x] sh->disp_h [%d] new_source [%d]$$$$\n", __func__, __LINE__, offset, sh->disp_h, sh->disp_h + offset);
					vdec_init_info.thumbnail_res.th_height = (int) ((double)(scalar_h * (offset + sh->disp_h)/ 4) / (double)(sh->disp_h)) + 2 ;
				}
				else
					vdec_init_info.thumbnail_res.th_height = sh->disp_h;

				//printf("$$$ In [%s][%d] d_info.width[%d] sh->disp_w *4[%d] d_info.height[%d] sh->disp_h*4[%d] th_width[%d] th_height[%d] $$$$$\n", __func__, __LINE__, d_info.width, sh->disp_w *4, d_info.height, sh->disp_h*4, vdec_init_info.thumbnail_res.th_width, vdec_init_info.thumbnail_res.th_height);
			}
		}
	}
#endif /* end of SUPPORT_DIVX_DRM */

    //Fuchun 2010.11.25
	if(!no_osd)
	{
		if(standard == VCODEC_MP2 && sh->disp_h <= 600)
		{
			if(sh->disp_h >= 480)   //Fuchun 2010.12.29 if height < 480, don't 3:2pulldown now. by Johnny
			{
				if(pulldown32)
					vdec_init_info.pulldown32 = pulldown32;
				else
					vdec_init_info.pulldown32 = is_pulldown32(sh);
				vdec_init_info.pulldown32_2 = is_pulldown32_2();
			}
			else
			{
				vdec_init_info.pulldown32 = pulldown32;
				vdec_init_info.pulldown32_2 = 0;
			}
                     #if 0 //Polun 2012-01-16 fixed DLNA test file O-MP2TS_SN_I-1.mpg playback not smooth.
                     vdec_init_info.double_deint = is_double_deint(sh);
                     #else
			vdec_init_info.double_deint = is_double_deint(sh);
                     #endif
		}
		else
		{
			vdec_init_info.pulldown32 = pulldown32;
			vdec_init_info.double_deint = 0;
			vdec_init_info.pulldown32_2 = 0;
is_output_interlace();
		}

		vdec_init2_info.workaround_flag = 0;	//Barry 2010-08-05
		if(vdec_init_info.double_deint == 1)
			vdec_init2_info.double_deint = 1;
		else
			vdec_init2_info.double_deint = 0;

#if 0	//Fuchun 20110902 not need enable vdec_init2_info.pulldown32
		if(vdec_init_info.pulldown32 == 1 || vdec_init_info.pulldown32_2 == 1)
		{
		    if (vdec_init_info.pulldown32 == 1)
		    {
		        if (sh->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
		        {
		            vdec_init2_info.pulldown32 = 1;
		        }
		        else
		        {
		            vdec_init2_info.pulldown32 = 0;//1;
                        }
                    }

//Robert 20110615 should be 1080i output, disable display pulldown32_2.
                    if (vdec_init_info.pulldown32_2 == 1)
                    {
		        if (sh->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
		        {
		            vdec_init2_info.pulldown32 = 1;
		        }
		        else
		        {
			    vdec_init2_info.pulldown32 = 0;//1;
                        }
                    }
                }
		else
#endif
			vdec_init2_info.pulldown32 = 0;

//	printf("##### MPlayer: init.double_deint[%d], init2.double_deint[%d], init.pulldown32[%d] #####\n", vdec_init_info.double_deint, vdec_init2_info.double_deint, vdec_init_info.pulldown32);
		if(disable_disp_deint)
			vdec_init2_info.disable_deinterlace = 1;
		else
			vdec_init2_info.disable_deinterlace = 0;

#ifdef SUPPORT_DIVX_DRM    //Just for divx certified, don't commit this to trunk !!!!!!!!!!
		if(is_divx_codec)
		{
			vdec_init2_info.disable_deinterlace = 1;
			vdec_init_info.ppCfgInit.operation_mode= COMBINED_MULTIBUFFER_MODE;
			vdec_init_info.ppCfgInit.deinterlacing_en = 1;
		}
#endif /* end of SUPPORT_DIVX_DRM */
	}

    if(standard == VCODEC_H264 && sh->disp_h <= 600)
    {
        vdec_init_info.ppCfgInit.operation_mode= COMBINED_MULTIBUFFER_MODE;
        vdec_init_info.ppCfgInit.deinterlacing_en = 1;
    }

    if (ioctl(skyfd, SKY_VDEC_INIT, &vdec_init2_info) == -1)
    {
        fprintf(stderr,"SKY_VDEC_INIT ioctl failed\n");
        return 0;
    }

#ifdef SUPPORT_DIVX_DRM
	//Fuchun 2011.03.11 for ICT files
	if(!no_osd && is_ICT)
	{
		int sky_fd = -1;
		unsigned int output_device;
		unsigned int orig_display_height = 0;
		unsigned int orig_display_width = 0;
		unsigned int real_display = 0;
		if( skydroid == 0 )
		{
			sky_fd = open("/dev/fb0", O_RDWR);
		}
		else
		{
			sky_fd = open("/dev/graphics/fb0", O_RDWR);
		}
		if (sky_fd== -1)
		{
			printf("Can't open fd devices %d!!!\n", skydroid);
		}
		else
		{
			if(ioctl(sky_fd, SKYFB_GET_REAL_DISPLAY, &real_display) == -1)
			{
				fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
				return 0;
			}
			orig_display_height = real_display & 0x0000ffff;
			orig_display_width = (real_display >> 16) & 0x0000ffff;
			ioctl(sky_fd, SKYFB_GET_OUTPUT_DEVICE, &output_device) ;
			close(sky_fd);
		}
		/*
		   enum {
		   SKYFB_HDMI = 0,
		   SKYFB_YPBPR,
		   SKYFB_CVBS_SVIDEO,
		   SKYFB_DSUB,
		   SKYFB_LCD,
		   SKYFB_DVI,
		   };
		 */
		if(sky_fd != -1 && (output_device == 1 || output_device == 2) && (sh->disp_w/2) < orig_display_width && (sh->disp_h/2) < orig_display_height)
		{
			unsigned int set_res = 0;
			vdec_init2_info.disable_deinterlace = 1;
			set_res = ((sh->disp_w/2) << 16) | (sh->disp_h/2);
			printf("Video is ICT and output is %s(%dx%d), so set real output be %d x %d\n"
					,(output_device == 1 ? "YPbPr" : "CVBS"), orig_display_width, orig_display_height, sh->disp_w/2, sh->disp_h/2);
			ioctl(skyfd, SKY_VDEC_SET_REALOUT_RES, &set_res);
		}
		is_ICT = 0;
	}

#endif /* end of SUPPORT_DIVX_DRM */

    sky_set_vsync_status(0);
    sky_set_vsync_status(1);

    sky_vdec_vdref_num = 0;

    //Fuchun 2010.07.15 set display_frametime_idx_step = 2
    if(h264_frame_mbs_only == 0)
    {
        int flag = 1;
        ioctl(skyfd, SKY_VDEC_FIELD_TYPE_FLAG, &flag);
    }


    /*printf("\n\n#####sh->vfilter->priv->interlaced %d\n",sh->vfilter->priv->interlaced);*/

#if 1 /*johnnyke 20100902*/
    vdec_init_info.streamBuf.len = (sky_vd_reserved_size<<20);
    vdec_init_info.streamBuf.virtAddr = NULL;
    vdec_init_info.streamBuf.phyAddr = 0;
    nRet = svsd_ipc_init_callback(&vdec_init_info);
#endif


    if(nRet == 0)
    {
        printf("sky_video_init failed : %d\n", vdec_init_info.standard);
        return 1;
    }


    // Raymond 2009/04/08
    vdec_dec_info.standard = standard;

    // Raymond 2009/04/24
    file_format = sh->ds->demuxer->file_format;

//	vbuf = VideoBuffer[buf_idx];
//	phyAddr = VideoPhysAddr[0]; /*johnnyke 20100805*/

    // Raymond 2009/09/28
    if(dump_yuv_file_name != NULL)
    {
        if((dump_yuv_file = fopen(dump_yuv_file_name, "wb")) == NULL)
        {
            fprintf(stderr, "Unable to open %s\n", dump_yuv_file_name);
        }
    }

#ifdef DUMP_ES_STREAM
    if(bitstream_name != NULL)
    {
        if((bitstream_file = fopen(bitstream_name, "wb")) == NULL)
        {
            fprintf(stderr, "Unable to open %s\n", bitstream_name);
        }
    }
#endif

	if(vdec_init_info.pulldown32_2 == 1)
	{
		ioctl(skyfb_fd, SKYFB_IGNORE_VIDEO_US , NULL);
        }

    return(1);
}

//Barry 2010-08-05
int Vd_Reinit(sh_video_t *sh)
{
    printf("skyvdec: Vd_Reinit() - format = %X\n\n", sh->format);

    int uninit_timeout = 20;
    //Robert 20100712 reset Video Frame Time index
    sky_vft_idx = 0;

    //Barry 2010-08-07
    while(sky_get_decode_done() == 0)
    {
        if (--uninit_timeout <= 0)
        {
            break;
        }
        usleep(1);
        printf("wait get_decode_done ...\n");
    }
    if (sh) // for DVB-T, don't unlock display
    {
        sky_set_reset_display_queue(0);
    }

    //Robert 20100712 add force VDEC_UNINIT before INIT
    id = 0;
//	ioctl(skyfd, SKY_VDEC_UNINIT, &id);
    vdec_init2_info.workaround_flag = 1;
//	skyfb_set_display_status(1);

    if (ioctl(skyfd, SKY_VDEC_INIT, &vdec_init2_info) == -1)
    {
        fprintf(stderr,"SKY_VDEC_INIT ioctl failed\n");
        return 0;
    }

    sky_vdec_vdref_num = 0;

    //Barry 2010-08-11
    if (ts_avc1_pts_queue_cnt)
    {
        ts_avc1_pts_queue_cnt = 0;
        memset(ts_avc1_pts_queue, 0, (h264_reorder_num*(2-h264_frame_mbs_only)+1)*sizeof(float));
    }
    if (check_fast_ts_startcode)
        check_fast_ts_startcode = 0;

    //Fuchun 2010.07.15 set display_frametime_idx_step = 2
    if(h264_frame_mbs_only == 0)
    {
        int flag = 1;
        ioctl(skyfd, SKY_VDEC_FIELD_TYPE_FLAG, &flag);
    }

    return(1);
}


/*============================================================================
 * uninit - close the codec
 *==========================================================================*/

static void uninit(sh_video_t *sh)
{
    int uninit_timeout = 20;
//	printf("skyvdec: uninit()\n");
    printf("\n!!!!!!!!!!!!!!!!!!1skyvdec: uninit()\n");

//JF 1031 
#ifdef CMV_WORKAROUND
    cmv_workaround_uninit();
#endif

    last_svsd_recv_count = svsd_state->recv_count;

#if 1/*johnnyke 20100902 svsd*/
//    svsd_ipc_drop_video_cnts();

    /*svsd_ipc_free_callback(SallocVirtualBase);*/


    printf("mplayer: got svsd back\n");

    while(sky_get_decode_done() == 0)
    {
        if (--uninit_timeout <= 0)
        {
            break;
        }
//        usleep(1000);
        usleep(33000);
        printf("wait get_decode_done ...\n");
    }

    svsd_ipc_drop_video_cnts();
    svsd_ipc_uninit_callback(standard);
//Robert 20101028 do svsd reset in svsd side
//	svsd_ipc_reset();
#endif

    sky_set_reset_display_queue(1);
    sky_set_display_lock(0);		//Fuchun 2010.07.05
    //skyfb_set_display_status(1); // Carlos move this function to mplayer.c main function


    // Raymond 2009/09/28
    if(dump_yuv_file != NULL)
    {
        fclose(dump_yuv_file);
        dump_yuv_file = NULL;
    }

#ifdef DUMP_ES_STREAM
    if(bitstream_file != NULL)
    {
        fclose(bitstream_file);
        bitstream_file = NULL;
    }
#endif

//20100722 Robert fix VC1 corrupt data while video thumbnail
    VideoBufferIdx = 0;

//	printf("skyvdec: uninit() end\n");
}

/*============================================================================
 * functions to check buffer overlapping issue
 *==========================================================================*/
#ifdef BITSTREAM_BUFFER_CONTROL
/*
  * 0: has enough tail room
  * 1: no enough tail room
  */
int chk_tailroom(struct demuxer *demuxer, unsigned int start, unsigned int len)
{
    unsigned int maxlen = sky_vd_reserved_size << 20;
    unsigned int bsstart = (unsigned int)SallocPhysicalBase;

    if(aio == 0) {
        maxlen += (2 << 20);
    }

#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))
    if(demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux) {
        maxlen -= (1 << 20);
        bsstart += (1 << 20);
    }
#endif

    //printf("%s: new(%x+%x=%x) max(%x+%x=%x)\n", __func__, start, len, start+len, bsstart, maxlen, bsstart + maxlen);
    if((start + len) > (bsstart + maxlen)) {
//printf("%s: new(%x+%x=%x) max(%x+%x=%x)\n", __func__, start, len, start+len, bsstart, maxlen, bsstart + maxlen);
        //printf("buffer overwrite 0x%08x > 0x%08x\n", start + len, bsstart + maxlen);
//printf("buffer overwrite 0x%08x > 0x%08x\n", start + len, bsstart + maxlen);
        return 1;
    }
    return 0;
}

/*
  * 0: no enough buffer space before decode frame
  * 1: has enough buffer space before decode frame
  */
int chk_bufspace(struct demuxer *demuxer)
{
    void *nextaddr, *next2addr, *first_boundaryaddr;

    // queue full
    //if(svsd_state->queue_length >= MAX_SVSD_VDEC_BUFFER - 1)
    if(svsd_state->queue_length > cvq_threshold)
        return 0;

    // no check is required
    if(svsd_state->decode_addr == 0)
        return 1;

    // At least MAX_FRAME_SIZE space before 'decode_addr'
    if(demuxer->need_double_copy)
    {
        nextaddr = VIRTUAL_TO_UCM(SallocVirtualBase + NextVideoBufIdx);
        first_boundaryaddr = VIRTUAL_TO_UCM(SallocVirtualBase + MAX_FRAME_SIZE);
    }
#if (defined(HW_TS_DEMUX) &&  defined(HWTS_USED_FRAMEBUFFER))
    else if(demuxer->type == DEMUXER_TYPE_MPEG_TS && hwtsdemux)
    {
        nextaddr = VIRTUAL_TO_UCM(SallocVirtualBase + (1 << 20) + VideoBufferIdx);
        first_boundaryaddr = VIRTUAL_TO_UCM(SallocVirtualBase + (1<<20) + MAX_FRAME_SIZE);
    }
#endif
    else
    {
        nextaddr = VIRTUAL_TO_UCM(SallocVirtualBase + VideoBufferIdx);
        first_boundaryaddr = VIRTUAL_TO_UCM(SallocVirtualBase + MAX_FRAME_SIZE);
    }
    next2addr = VIRTUAL_TO_UCM(SallocVirtualBase + SallocBufferSize - MAX_FRAME_SIZE);

	//printf("%s: next_addr %x decode_addr %x\n", __func__, nextaddr, svsd_state->decode_addr);
	//printf("%s: next_addr %x decode_addr %x f2:%x n:%x n2:%x\n", __func__, nextaddr, svsd_state->decode_addr, first_boundaryaddr, nextaddr, next2addr);
    if(((nextaddr <= svsd_state->decode_addr) &&
            (nextaddr + MAX_FRAME_SIZE >= svsd_state->decode_addr))
            ||
            ((nextaddr > svsd_state->decode_addr) && (nextaddr > next2addr) && (first_boundaryaddr > svsd_state->decode_addr))
      )
    {
        //printf("%s: No enough space before decode_addr\n", __func__);
        return 0;
    }

    return 1;
}

/*
  * 0: new frame and decode frame are not overlapped
  * 1: new frame and decode frame are overlapped
  */
static inline int chk_overlap(unsigned int astart, unsigned int alen, unsigned int bstart, unsigned int blen)
{
    unsigned int aend = astart + alen - 1;
    unsigned int bend = bstart + blen - 1;

    if(alen == 0 || blen == 0)
        return 0;

    //printf("%s: new(%x~%x) decode(%x~%x)\n", __func__, astart, aend, bstart, bend);
    if((bstart >= astart && bstart <= aend) ||
            (astart >= bstart && astart <= bend)) {
        //printf("%s: buffer overlapped: new(%x~%x) decode(%x~%x)\n", __func__, astart, aend, bstart, bend);
        return 1;
    }

    return 0;
}

int wait_buffer(unsigned int start, unsigned int len)
{
    int cnt = 0;
    int maxcnt = 500;

    if (svsd_state->decode_addr == 0 && svsd_state->decode_length == 0)
    {
        return 0;
    }

    while(chk_overlap(start, len, svsd_state->decode_addr, svsd_state->decode_length) == 1) {
//		usleep(10000);
        usleep(502);
//printf("start=%p len=%d dec_addr=%p dec_len=%d svsd_state->queue_length=%d\n", start, len, svsd_state->decode_addr, svsd_state->decode_length, svsd_state->queue_length);
        cnt++;
        if(cnt > maxcnt) break;
    }

    //if(cnt) printf("New frame overlapped decode frame, waiting %d times\n", cnt);

    if(cnt > maxcnt) {
        printf("New frame overlapped decode frame, waiting timeout\n");
        return 1;
    }

    return 0;
}
#endif

/*============================================================================
 * decode - decode a frame from stream
 *==========================================================================*/
static mp_image_t* decode(sh_video_t *sh, void* data, int len, int flags)
{
    mp_image_t* mpi = NULL;
    unsigned char * hdr = (unsigned char *)data;
//	unsigned int pic_type = 0;
//	int ret = 0, i = 0;
    int hdr_offset=0;
    static int show_cnt = 0;
    static int drop_cnt = 0;
    int ft_idx_cnts;

    if (stthumb_mode & SKY_STORE_THUMB_BEGIN)
    {
        if (stthumb_got_begin == 0)
        {
            vd_store_thumbnail();
        }
    }


//Robert 20100712 use quick frame time instead of drop frame, fix me later..
//Robert 20100608 framedrop ioctl
#if 1
    if (flags > 0)
    {
        int need_drop_cnts=1;
//printf("== decode flags=%d\n", flags);
        ioctl(skyfd, SKY_VDEC_DROP_FRAME, &need_drop_cnts);
    }

#endif

    // Raymond 2009/04/24
    if( len == 0 || data == NULL )
        return mpi;

#ifdef VDEC_DEBUG
//	printf("\n[%d] decode - len = %6d, addr = %08X - %02X %02X %02X %02X %02X %02X\n", fcnt, len, (unsigned int)data, hdr[0], hdr[1], hdr[2], hdr[3], hdr[4], hdr[5]);
#endif


#if 1/*johnnyke 20101021*/

    if(alpha_blend /*&& vo_need_dvdnav_menu*/) {

        vdec_dec_info.alpha_blend.blend_enable = 1;
        vdec_dec_info.alpha_blend.blend_sx = 0;
        vdec_dec_info.alpha_blend.blend_sy = 0;
        vdec_dec_info.alpha_blend.blend_w = sh->disp_w;
        vdec_dec_info.alpha_blend.blend_h = sh->disp_h;
//		vdec_dec_info.alpha_blend.blend_ba = disp2_addr_phy;/*0x087e9000*/;//disp2_addr;
        vdec_dec_info.alpha_blend.blend_ba = dvdnav_btn_phyaddr[dvd_btn_addr_idx];/*0x087e9000*/;//disp2_addr;
//printf("alpha---idx[%d]\n",dvd_btn_addr_idx);


        //if(vo_need_dvdnav_menu)
//        extern int dvdnav_button_on;
        if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
	    vdec_dec_info.dvdmenu =1;
	else
	    vdec_dec_info.dvdmenu =0;
//printf("dvdnavstate.dvdnav_button_on[%d]\n",dvdnavstate.dvdnav_button_on);
        if(dvdnavstate.dvdnav_button_on==1)
//        if(dvdnav_button_on)
            vdec_dec_info.alpha_blend.cur_blend_enable =1;
        else
            vdec_dec_info.alpha_blend.cur_blend_enable =0;
        //printf("## disp2_addr: 0x%08x\n",disp2_addr);

    } else {

        vdec_dec_info.alpha_blend.blend_enable = 0;
        vdec_dec_info.alpha_blend.cur_blend_enable =0;
	 vdec_dec_info.dvdmenu =0;
    }
    /*
    	if(dynamic_outFmt){

    //if()
    vdec_dec_info.cur_outFmt = 0;

    	}else{

    vdec_dec_info.cur_outFmt = 0;
    	}*/

#endif





#if 1		// Raymond 2010/06/02 - workaround for FF/FB mode	


    //vdec_dec_info.resync_video= resync_video; /*johnnyke 20100609*/
//	if(speed_mult != 0)
    if(speed_mult < 0 || speed_mult >= 2)
        vdec_dec_info.dec_flags = 1;        // usePeekOutput
    else if (resync_video == 1)
        vdec_dec_info.dec_flags = 2;        // resync_video
    else if(speed_mult == 1)
        vdec_dec_info.dec_flags = 111;	   // for display use
    else
        vdec_dec_info.dec_flags = 0;

    if(resync_video == 2)		//Fuchun 2011.01.05
    {
        if(hdr[3] == 0xB3)
        {
            int i;
            for( i = 0 ; i < len -6 ; i++ )
            {
                if(hdr[i] == 0 && hdr[i+1] == 0 && hdr[i+2] == 1 && hdr[i+3] == 0)
                    break;
            }
            if(((((unsigned char)hdr[i+5]) >> 3)&7) == 1) //found I
            {
                vdec_dec_info.dec_flags = 2;
                resync_video = 0;
//printf("[%d][%d] [%d] decode - len = %6d, addr = %08X - %02X %02X %02X %02X %02X %02X\n", vdec_dec_info.dec_flags, vdec_dec_info.alpha_blend.cur_blend_enable, fcnt, len, (unsigned int)data, hdr[0], hdr[1], hdr[2], hdr[3], hdr[4], hdr[5]);
            }
            else
                return mpi;
        }
        else
            return mpi;
    }
//printf("\n[%d] [%d] decode - len = %6d, addr = %08X - %02X %02X %02X %02X %02X %02X\n", vdec_dec_info.dec_flags, fcnt, len, (unsigned int)data, hdr[0], hdr[1], hdr[2], hdr[3], hdr[4], hdr[5]);



    if(resync_video == 1)
    {
        resync_video = 0;
    }


    if(standard == VCODEC_REAL)
    {
        unsigned int numSegments, hdr_len;

        numSegments = (hdr[16] << 24) | (hdr[17] << 16) | (hdr[18] << 8) | hdr[19];
        hdr_len = 20 + (numSegments << 3);		// * 8
//20100924 Robert RMVB need 8 bytes alignment
//FIXME: 20101213 Charles actually should be: hdr_offset = 8 - (hdr_len % 8)
        hdr_offset = hdr_len%8;
    }


#endif


//printf("use_PP_buf=%d\n", use_PP_buf);
    if( use_PP_buf == 0 )	// Raymond 2009/04/27
    {
        //Barry 2010-04-07
        demuxer_t *demux = sh->ds->demuxer;
//printf("  need_double_copy=%d\n", demux->need_double_copy);

//		if( demux->file_format != DEMUXER_TYPE_MPEG_PS && demux->file_format != DEMUXER_TYPE_MPEG_TS)
        if (demux->need_double_copy)
        {
#ifdef BITSTREAM_BUFFER_CONTROL
            if(chk_tailroom(sh->ds->demuxer, VIRTUAL_TO_UCM(SallocVirtualBase + NextVideoBufIdx), len) == 1) {
                //printf("Frame too large, reset to head\n");
                NextVideoBufIdx = 0;
            }
            NextVideoBufIdx += hdr_offset;
            videobuffer = SallocVirtualBase + NextVideoBufIdx;
            //printf("%s:%d buf_busaddr %p - %p\n", __func__, __LINE__, VIRTUAL_TO_UCM(videobuffer), VIRTUAL_TO_UCM(videobuffer + len));

            wait_buffer(VIRTUAL_TO_UCM(videobuffer), len);

            memcpy(videobuffer, data, len);
//Robert 20110117, add msync and fine tune ROUND_UP to 1024*32, try to prevent H.264 broken issue
            msync(videobuffer, len, MS_SYNC);
            vdec_dec_info.buf_busaddr = VIRTUAL_TO_UCM(videobuffer);

//			NextVideoBufIdx = ROUND_UP(NextVideoBufIdx + len, 8192);
            NextVideoBufIdx = ROUND_UP(NextVideoBufIdx + len, 1024*32);
            //printf("%s:%d next buf_busaddr %x\n", __func__, __LINE__, VIRTUAL_TO_UCM(SallocVirtualBase + NextVideoBufIdx));
#else
            if (ROUND_UP(NextVideoBufIdx + len, 8192) > (sky_vd_reserved_size<<20))
            {
                NextVideoBufIdx = 0;
            }
            videobuffer = hdr_offset + SallocVirtualBase + NextVideoBufIdx;

            //Robert 20101111 add boundary check
            if (((hdr_offset + NextVideoBufIdx + len )) >= (sky_vd_reserved_size<<20))
            {
                printf("WARNING... videobuffer=0x%x  len=%d hdr_offset=%d NextVideoBufIdx=%d (0x%x)\n", videobuffer, len, hdr_offset, NextVideoBufIdx+len, NextVideoBufIdx+len + hdr_offset);
            }

            NextVideoBufIdx = ROUND_UP(NextVideoBufIdx + len, 8192);
            memcpy(videobuffer, data, len);
            vdec_dec_info.buf_busaddr = VIRTUAL_TO_UCM(videobuffer);
#endif
        }
        else
        {
            vdec_dec_info.buf_busaddr = VIRTUAL_TO_UCM(videobuffer);
        }
    }
    else if (use_PP_buf == 3)
    {
        vdec_dec_info.buf_busaddr = VIRTUAL_TO_UCM(data);
//		vdec_dec_info.buf_busaddr = data;
//printf("=== use_PP_buf = 3  addr:%x  data=%x\n", vdec_dec_info.buf_busaddr, data);
    }

    vdec_dec_info.buf_size = len;
    //printf("%s: new frame addr (%x~%x) len %x\n", __func__, vdec_dec_info.buf_busaddr, vdec_dec_info.buf_busaddr + vdec_dec_info.buf_size, vdec_dec_info.buf_size);
#ifdef BITSTREAM_BUFFER_CONTROL
    if(chk_tailroom(sh->ds->demuxer, vdec_dec_info.buf_busaddr, vdec_dec_info.buf_size))
        printf("%s: new frame addr (%x~%x) len %x exceeds tail\n", __func__, vdec_dec_info.buf_busaddr, vdec_dec_info.buf_busaddr + vdec_dec_info.buf_size, vdec_dec_info.buf_size);

//chk_overlap_retry:
int retry_cnts=0;    
//    if(chk_overlap(vdec_dec_info.buf_busaddr, vdec_dec_info.buf_size, svsd_state->decode_addr, svsd_state->decode_length))
    while(chk_overlap(vdec_dec_info.buf_busaddr, vdec_dec_info.buf_size, svsd_state->decode_addr, svsd_state->decode_length))
    {
retry_cnts++;
//        printf("%s: new frame addr (%x~%x) len %x overlapped decode frame\n", __func__, vdec_dec_info.buf_busaddr, vdec_dec_info.buf_busaddr + vdec_dec_info.buf_size, vdec_dec_info.buf_size);
        printf("%s: retry:%d new frame addr (%x~%x) len %x overlapped decode frame\n", __func__, retry_cnts, vdec_dec_info.buf_busaddr, vdec_dec_info.buf_busaddr + vdec_dec_info.buf_size, vdec_dec_info.buf_size);
        printf("  ==> svsd_state->decode_addr=%p len=%d\n", svsd_state->decode_addr, svsd_state->decode_length);
        usleep(1000*10);
    }
#endif

    //vdec_dec_info.speed_mult = speed_mult; /*johnnyke 20100524*/
//20100811 Robert prevent too large packet size cause video buffer overflow
    //if (vdec_dec_info.buf_size > 1024*1024*1.3)
    if (vdec_dec_info.buf_size > 1024*1024*1.2) /* carlos chagne from 1.3 to 1.2, for Pepsi Demo\HD-DVD_Shinobi_clip0*.ts , 2010-11-24*/
    {
        extern int cvq_threshold;
        if (cvq_threshold > 0)
        {
//			cvq_threshold--;
            cvq_threshold = 0;
            printf("SKY_VDEC buf_size too large (%d) new cvq_threashold = %d\n", vdec_dec_info.buf_size, cvq_threshold);
        }
    }

//printf("--- next_frame_time=%f  last_pts=%f\n", sh->next_frame_time, sh->last_pts);
    if(speed_mult != 0)
    {
        sky_repeatfield_cnt = 0;
        sky_repeatfield_flag = 0;
        if(is_mjpeg == 1 || speed_mult >= 2 || speed_mult < 0)
        {
            vdec_dec_info.vft = (int)((90000.0)/(60));

            show_cnt = 0;
            drop_cnt = 0;
            sky_set_display_lock(0);
        }
        else
        {
//20100728 Robert use new Video Frame Time method, let display driver check it
            vdec_dec_info.vft = (int)((90000.0)/(1.0/sh->next_frame_time*2));
            sky_set_display_lock(0); //Polun 2011-09-29 fixed start playing fast press FF X2 no unlockdisplay cause black screen
        }
    }
    else
    {
        if (sh->next_frame_time > 0)
        {
//20100728 Robert use new Video Frame Time method, let display driver check it
#if 1	//Robert reserved, open this later
            if (force_fps > 0)
            {
                if (h264_frame_mbs_only==0)
                {
                    vdec_dec_info.vft = (int)((90000.0)/(force_fps/2))/2;
                }
                else
                {
                    //Barry 2010-12-30
                    if (sh->fps > 59.0)
                        vdec_dec_info.vft = (int)((90000.0)/(force_fps/2))/2;
                    else
                        vdec_dec_info.vft = (int)((90000.0)/((force_fps)));
                }
            }
            else
#endif
            {
                if (h264_frame_mbs_only==0)
                {
                    vdec_dec_info.vft = (int)((90000.0)/(sh->fps/2))/2;
                }
                else
                {
                    if ((sh->fps > 59.0 && h264_frame_mbs_only == 0) || (sh->ds->demuxer->audio->id < 0))	//Barry 2011-01-20
                        vdec_dec_info.vft = (int)((90000.0)/(sh->fps/2))/2;
                    else
                    {
                        vdec_dec_info.vft = (int)((90000.0)/(1.0/(sh->next_frame_time)));
//printf("@@@@2  sh->next_frame_time=%f   vft=%d\n", sh->next_frame_time, vdec_dec_info.vft);
                    }
                }
            }

//Robert 20100712 keep original frame time info
//		sky_vft_history[sky_vft_idx] = vdec_dec_info.vft;
            sky_vft_history[sky_vft_idx] = sh->next_frame_time;
            sky_vpts_history[sky_vft_idx] = (double)sh->pts;
            sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;

//Robert 20100712 use quick frame time instead of drop frame, fix me later..
            if (flags > 0)
            {
//vdec_dec_info.vft = vdec_dec_info.vft/2;
//			vdec_dec_info.vft = (int)((90000.0)/(60));
            }
        }
        else
        {
            if (!quiet)
            {
                printf("--- Check again next_frame_time=%f  last_pts=%f\n", sh->next_frame_time, sh->last_pts);
            }
//		vdec_dec_info.vft = 0;
            //Fuchun 2010.08.05 if next_frame_time = 0, need to add 0.033 to vft_history too.
            if(sh->fps != 1000 && sh->fps > 1)
            {
                vdec_dec_info.vft = (int)((90000.0)/(sh->fps));
                sky_vft_history[sky_vft_idx] = 1/sh->fps;
            }
            else
            {
                vdec_dec_info.vft = (int)((90000.0)/(30));
                sky_vft_history[sky_vft_idx] = 0.033;
            }

            sky_vpts_history[sky_vft_idx] = (double)sh->pts;
            sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
        }
        show_cnt = 0;
        drop_cnt = 0;
    }

    if(speed_mult == 0)
    {
#ifdef QT_SUPPORT_DVBT
        if (sh->ds->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
        {
            if((dvbt_wait_sync == 2) || (wait_video_or_audio_sync&AVSYNC_NOVIDEO) || (seek_sync_flag != 0 && seek_sync_flag != 1 && seek_sync_flag != 4) || !start_to_display)
            {
#ifdef DVBT_USING_NORMAL_METHOD
                if (no_osd)
                {
                    start_to_display = 1;
                }
                else
                {
                    vdec_svread_buf_t nrb;
                    svread(0, &nrb, 0);
                    if (nrb.ft_ridx != 0)
                        start_to_display = 1;
                }
#endif // end of DVBT_USING_NORMAL_METHOD
                sky_set_display_lock(1);
            }
            else
            {
                sky_set_display_lock(0);
            }
        }
        else
#endif /* end of QT_SUPPORT_DVBT */		
        {
#ifdef QT_SUPPORT_DVBT		
            if((wait_video_or_audio_sync&AVSYNC_NOVIDEO) || (seek_sync_flag != 0 && seek_sync_flag != 1 && seek_sync_flag != 4) || !start_to_display)
#else /* else of QT_SUPPORT_DVBT */			
            if((wait_video_or_audio_sync&AVSYNC_NOVIDEO) || (seek_sync_flag != 0 && seek_sync_flag != 1 && seek_sync_flag != 4) )
#endif /* end of QT_SUPPORT_DVBT */			
            {
#ifdef DVBT_USING_NORMAL_METHOD
                if (no_osd)
                {
                    start_to_display = 1;
                }
                else
                {
                    vdec_svread_buf_t nrb;
                    svread(0, &nrb, 0);
                    if (nrb.ft_ridx != 0)
                        start_to_display = 1;
                }
#endif // end of DVBT_USING_NORMAL_METHOD
                if ((is_dvdnav && (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)) || sh->ds->demuxer->type == DEMUXER_TYPE_ASF)
                {
                    ;
                    //don't do display_lock(1)
                }
                else
                {
//printf("\n\n@@@@@@@@@@ got sky_set_display_lock(1) seek_sync_flag = %d\n", sky_set_display_lock);
                sky_set_display_lock(1);
                }
            }
            else
            {
                sky_set_display_lock(0);
            }
        }
    }
//   else
//   	skyfb_set_display_status(1);
#if 0
    vdec_dec_info.pts = (double)sh->pts;
    vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
//printf("     @@@@@   pts=%f  last_pts=%f  vdec_dec_info.vft=%d fps:%f\n", vdec_dec_info.pts, sh->last_pts, vdec_dec_info.vft, sh->fps);
#else
    double next_vft;
    double next_frame_time;
    if (standard == VCODEC_MP2)
    {
    	if (sh->ds->demuxer->type == DEMUXER_TYPE_MPEG_TS)
    	{
    		//printf("sky_telecine=%d vdec_init_info.pulldown32=%d 32_2=%d\n", sky_telecine, vdec_init_info.pulldown32, vdec_init_info.pulldown32_2);
    		if ((vdec_init_info.pulldown32_2 || vdec_init_info.pulldown32) && sky_repeatfield_cnt == 2 /*&& sky_telecine */)
			//&& (!is_dvdnav || (is_dvdnav && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)))
		{
			if(vdec_dec_info.vft > 9000)
				vdec_dec_info.vft = 3003;
			else
				vdec_dec_info.vft = vdec_dec_info.vft / 1.5;
			if ((int)vdec_dec_info.vft+last_pulldown32_vft_diff > 0)
				next_vft = vdec_dec_info.vft+last_pulldown32_vft_diff;
			else
				next_vft = vdec_dec_info.vft;

			next_frame_time = ((next_vft)/90000.0);

			//if (last_pulldown32_ptsidx == 0)	//Fuchun 20110902 disable
			{
				sky_repeatfield_cnt = 0;
				sky_repeatfield_flag = 0;
				vdec_dec_info.pts = (double)sh->pts;
				vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
				//printf(" 01 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);

				last_pulldown32_pts = vdec_dec_info.pts;
				last_pulldown32_ptsidx++;
				vdec_dec_info.vft = (int)(next_vft);
				//send one fake DECODE info
				ioctl(skyfd, SKY_VDEC_DECODE, &vdec_dec_info);
				sky_vpts_total_cnts++;
				//update vft & pts history
				sky_vft_idx = (sky_vft_idx + MAX_SKY_VFT_BUFFER - 1)%MAX_SKY_VFT_BUFFER;
				sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
				sky_vpts_history[sky_vft_idx] = (double)sh->pts;
				sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;

				last_pulldown32_pts += next_frame_time;//0.0333667;
				vdec_dec_info.pts = (double)(last_pulldown32_pts);
				vdec_dec_info.int_pts = (unsigned int)(last_pulldown32_pts*90000.0);
				//printf(" 02 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);
				last_pulldown32_ptsidx++;
				vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
				sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
				sky_vpts_history[sky_vft_idx] = (double)last_pulldown32_pts;
				sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
			}
#if 0	//Fuchun 20110902 disable
			else
			{
			        last_pulldown32_pts += next_frame_time;//0.0333667;
			        vdec_dec_info.pts = (double)(last_pulldown32_pts);
			        vdec_dec_info.int_pts = (unsigned int)(last_pulldown32_pts*90000.0);
			//printf(" 03 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);

			        last_pulldown32_ptsidx++;
			        vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
			        sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
			        sky_vpts_history[sky_vft_idx] = (double)last_pulldown32_pts;
			        sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;

			        if (last_pulldown32_ptsidx >= 5)
			        {
			                last_pulldown32_ptsidx = 0;
			        }
			}
#endif
		}
		else
		{
			if((vdec_init_info.pulldown32_2 || vdec_init_info.pulldown32) && sky_repeatfield_flag == 1)
			{
				sky_repeatfield_flag = 0;
				vdec_dec_info.vft = vdec_dec_info.vft / 1.5;
			}
			if ((int)(vdec_dec_info.vft + last_pulldown32_vft_diff) > 0)
				next_vft = vdec_dec_info.vft + last_pulldown32_vft_diff;
			else
				next_vft = vdec_dec_info.vft;
			next_frame_time = ((next_vft)/90000.0);
			vdec_dec_info.pts = (double)sh->pts;
			vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
			vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
			sky_vft_idx = (sky_vft_idx + MAX_SKY_VFT_BUFFER - 1)%MAX_SKY_VFT_BUFFER;
			sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
			sky_vpts_history[sky_vft_idx] = (double)sh->pts;
			sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
			//printf(" P  == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)vdec_dec_info.vft, vdec_dec_info.int_pts);
		}
    	}
	else	//Barry 2011-09-15 fix mantis: 5644, 6106
	{
		//printf("sky_telecine=%d vdec_init_info.pulldown32=%d 32_2=%d\n", sky_telecine, vdec_init_info.pulldown32, vdec_init_info.pulldown32_2);
		if ((vdec_init_info.pulldown32_2 || vdec_init_info.pulldown32) && sky_telecine 
			&& (!is_dvdnav || (is_dvdnav && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)))
		{
			if (3003+last_pulldown32_vft_diff > 0)
				next_vft = 3003+last_pulldown32_vft_diff;
			else
				next_vft = 3003;
			next_frame_time = ((next_vft)/90000.0);

			if (last_pulldown32_ptsidx == 0)
			{
				vdec_dec_info.pts = (double)sh->pts;
				vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
				//printf(" 01 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);

				last_pulldown32_pts = vdec_dec_info.pts;
				last_pulldown32_ptsidx++;
				vdec_dec_info.vft = (int)(next_vft);
				//send one fake DECODE info
				ioctl(skyfd, SKY_VDEC_DECODE, &vdec_dec_info);
				sky_vpts_total_cnts++;
				//update vft & pts history
				sky_vft_idx = (sky_vft_idx + MAX_SKY_VFT_BUFFER - 1)%MAX_SKY_VFT_BUFFER;
				sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
				sky_vpts_history[sky_vft_idx] = (double)sh->pts;
				sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;

				last_pulldown32_pts += next_frame_time;//0.0333667;
				vdec_dec_info.pts = (double)(last_pulldown32_pts);
				vdec_dec_info.int_pts = (unsigned int)(last_pulldown32_pts*90000.0);
				//printf(" 02 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);
				last_pulldown32_ptsidx++;
				vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
				sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
				sky_vpts_history[sky_vft_idx] = (double)last_pulldown32_pts;
				sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
			}
			else
			{
				last_pulldown32_pts += next_frame_time;//0.0333667;
				vdec_dec_info.pts = (double)(last_pulldown32_pts);
				vdec_dec_info.int_pts = (unsigned int)(last_pulldown32_pts*90000.0);
				//printf(" 03 == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)next_vft, vdec_dec_info.int_pts);

				last_pulldown32_ptsidx++;
				vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
				sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
				sky_vpts_history[sky_vft_idx] = (double)last_pulldown32_pts;
				sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;

				if (last_pulldown32_ptsidx >= 5)
				{
					last_pulldown32_ptsidx = 0;
				}
			}
		}
		else
		{
			if ((int)(vdec_dec_info.vft + last_pulldown32_vft_diff) > 0)
				next_vft = vdec_dec_info.vft + last_pulldown32_vft_diff;
			else
				next_vft = vdec_dec_info.vft;
			next_frame_time = ((next_vft)/90000.0);
			vdec_dec_info.pts = (double)sh->pts;
			vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
			vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
			sky_vft_idx = (sky_vft_idx + MAX_SKY_VFT_BUFFER - 1)%MAX_SKY_VFT_BUFFER;
			sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
			sky_vpts_history[sky_vft_idx] = (double)sh->pts;
			sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
			//printf(" P  == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)vdec_dec_info.vft, vdec_dec_info.int_pts);
		}
	}
    }
    else
    {
#if 0
        vdec_dec_info.pts = (double)sh->pts;
        vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
#else
                next_vft = (int)(vdec_dec_info.vft + last_pulldown32_vft_diff);
                if (next_vft <= 0)
                {
                    next_vft = (int)(vdec_dec_info.vft + last_pulldown32_vft_diff/2);
                    if (next_vft <= 0)
                        next_vft = vdec_dec_info.vft/2;
                }
                next_frame_time = ((next_vft)/90000.0);
                vdec_dec_info.pts = (double)sh->pts;
                vdec_dec_info.int_pts = (unsigned int)(sh->pts*90000.0);
                vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
                sky_vft_idx = (sky_vft_idx + MAX_SKY_VFT_BUFFER - 1)%MAX_SKY_VFT_BUFFER;
                sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
                sky_vpts_history[sky_vft_idx] = (double)sh->pts;
                sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
#endif
    }
#endif

//20110621 Robert add vft range checking for DVD
    if (is_dvdnav && vdec_dec_info.vft > 6000)
    {
//Robert 20110618 Debug while VFT too large
        if (!quiet)
            printf("\n P  == %d  pts:%f vft:%d int_pts :%d\n", last_pulldown32_ptsidx, (float)vdec_dec_info.pts, (int)vdec_dec_info.vft, vdec_dec_info.int_pts);

        if (speed_mult == 1)
            vdec_dec_info.vft = 1499;
        else
            vdec_dec_info.vft = 2997;
    }
//if (sky_need_drop_b==1)
//printf("     @@@@@   pts=%f  last_pts=%f  vdec_dec_info.vft=%d fps:%f need_dro:%d\n", vdec_dec_info.pts, sh->last_pts, vdec_dec_info.vft, sh->fps, sky_need_drop_b);
#ifdef VDEC_DEBUG
//	printf("Addr = %08X, size = %6d\n", vdec_dec_info.buf_addr, vdec_dec_info.buf_size);
#endif

#ifdef CONFIG_DVDNAV
    if(dvdnav_btn_change == 1)
    {
//        vdec_dec_info.dec_flags = 3;	//Fuchun 2010.11.24 disable

        if(dvdnav_should_do_alpha)
        {
            dvd_btn_addr_idx = !dvd_btn_addr_idx;
            dvdnav_btn_change =0;
        }
    }
#endif

        if (sky_need_drop_b == 1)
        {
                vdec_dec_info.try_drop_b = 1;
                sky_need_drop_b = 0;
        }
        else
        {
                vdec_dec_info.try_drop_b = 0;
        }
    //Barry 2011-05-25
    demuxer_t *demux = sh->ds->demuxer;
    if (demux->file_format == DEMUXER_TYPE_MPEG_TS && ts_h264_field_num && svsd_state->tune_on_h264_workaround)
    {
	vdec_dec_info.h264_workaround_field_num = ts_h264_field_num;
	memcpy(&vdec_dec_info.h264_workaround_offset[0], &ts_h264_field_offset[0], 8*sizeof(unsigned int));
    }
//static int cur_vft_total=0, cur_vft_diff=0;

    if (vdec_dec_info.dec_flags == 2)
    {
        sky_vpts_total_cnts = 0;
//cur_vft_total =0;
//cur_vft_diff=0;
    }
    sky_vpts_total_cnts++;
#ifdef DUMP_ES_STREAM
	fwrite(data, len, 1, bitstream_file);
#endif
//cur_vft_total += vdec_dec_info.vft;
//cur_vft_diff += last_pulldown32_vft_diff;    
//printf("\ntotal:%.8d    diff:%d\n", cur_vft_total, cur_vft_diff);
    ioctl(skyfd, SKY_VDEC_DECODE, &vdec_dec_info);
    
    if (standard == 2 && (sh->ds->demuxer->type >= DEMUXER_TYPE_AVI && sh->ds->demuxer->type <= DEMUXER_TYPE_AVI_NINI))
    {
//printf("standard=%d  dwl_enqueue_cnts=%d  total:%d \n", standard, svsd_state->dwl_enqueue_cnts, sky_vpts_total_cnts);
        extern vdec_svread_buf_t sky_nrb;
        int decode_enqueue_diff;
        if (h264_frame_mbs_only==0)
        {
            decode_enqueue_diff = sky_vpts_total_cnts/2 - svsd_state->dwl_enqueue_cnts;
        }
        else
        {
            decode_enqueue_diff = sky_vpts_total_cnts - svsd_state->dwl_enqueue_cnts;
        }
        ft_idx_cnts = (sky_nrb.ft_widx - sky_nrb.ft_ridx + 128)%128;
        if (ft_idx_cnts < (5+sky_nrb.qlen[2]) && (decode_enqueue_diff+sky_nrb.qlen[2]) > ft_idx_cnts)
        {
            ioctl(skyfd, SKY_VDEC_DECODE, &vdec_dec_info);
        }
    }

    //if(dumpfile){
    //	fwrite(data, len , 1 ,dumpfile);
    //}

//printf("dvdnavstate.dvdnav_stream[%d]\n",dvdnavstate.dvdnav_stream);
#if 1/*johnnyke 20100902 svsd*/
    if(dvdnavstate.dvdnav_stream == 1)
        vdec_dec_info.dvdnav = 1;
    svsd_ipc_callback(&vdec_dec_info);
#else
    sky_video_decode(&vdec_dec_info);
#endif

    /*	// Raymond 2009/09/28
    	if(dump_yuv_file != NULL)
    	{
    		DumpYUV();
    	}//*/

    fcnt++;

    return mpi;
}

int check_display_change()
{
//static int first_height=0;
    struct skyfb_api_display_info d_info;
    int fd = -1;
    int display_change=0;

    if( skydroid == 0 )
    {
        fd = open("/dev/fb0", O_RDWR);
    }
    else
    {
        fd = open("/dev/graphics/fb0", O_RDWR);
    }

    if (ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info) == -1)
    {
        fprintf(stderr,"SKYFB_GET_DISPLAY_INFO ioctl failed\n");
        return -1;
    }
    printf("d_info.height[%d],first_height[%d]\n",d_info.height,first_height);
//	if(!first_height)
//	    first_height = d_info.height;

    if(first_height != d_info.height)
    {
////        init_disp2_param();
        display_change = 1;
    }

    if(fd != -1)
    {
        close(fd);
        fd = -1;
    }

    return display_change;
}

//Robert 2011/02/18
void skyfb_trick_play_workaround(int status)
{
//Robert 2011/02/18 1730 still buggy while using HWTS, fixed this later...
//Robert 2011/02/25 use switch fifo depth 1<->8 to workaround
#ifdef HW_TS_DEMUX
    volatile unsigned int clk_value = 0;
//    struct skyfb_api_display_parm trick_d_parm;
//    struct skyfb_api_display_status trick_d_status;

    if (vo_need_dvdnav_menu || skydroid == 0 || skyqt == 1 || no_osd == 1)
    {
		printf(" == In [%s][%d] can't set f0fc8190  bit 15 to 0 vo_need_dvdnav_menu [%d] no_osd[%d]\n", __func__, __LINE__, vo_need_dvdnav_menu, no_osd);
        return;
    }

    if (status == 1)
    {
#if 1
        clk_value = (volatile unsigned int)*sky_clkreg;
//printf(" == status set 1, orig clk_value = 0x%x\n", clk_value);
        if (clk_value & (1<<15))
        {
            unsigned int tmpmask = ~(1<<15);
            *sky_clkreg = clk_value & tmpmask;
//printf(" == status set 1, new *sky_clkreg = 0x%x\n", (volatile )*sky_clkreg);
        }
#else
            if (d_info.fb_base_addr == 0)
                ioctl(skyfb_fd, SKYFB_GET_DISPLAY_INFO, &d_info);
	    trick_d_parm.display = SKYFB_DISP2;
	    trick_d_parm.input_format = INPUT_FORMAT_YCC420;//INPUT_FORMAT_ARGB;
	    trick_d_parm.start_x = 0;
	    trick_d_parm.start_y = 0;	//set wanted position;
	    trick_d_parm.width_in = d_info.width;
	    trick_d_parm.height_in = d_info.height;
	    trick_d_parm.stride = d_info.width;
	    trick_d_parm.alpha = 0x0;
	    trick_d_parm.y_addr = svsd_state->d_parm.y_addr;//d_info.video_offset;//osd_addr_phy;//disp2_addr_phy;
	    trick_d_parm.u_addr = svsd_state->d_parm.u_addr;//d_info.video_offset+d_info.width*d_info.height*2;
	    trick_d_parm.v_addr = svsd_state->d_parm.v_addr;//trick_d_parm.u_addr;
	    if (ioctl(skyfb_fd, SKYFB_SET_DISPLAY_PARM, &trick_d_parm) == -1) {
	   	 printf("SKYFB_SET_DISPLAY_PARM ioctl failed\n");
	   	 return ;
	    }

    	    trick_d_status.display = SKYFB_DISP2;
    	    trick_d_status.status = SKYFB_ON;
#endif
    }
    else
    {
        clk_value = (volatile )*sky_clkreg;
//printf(" == status set 0, orig clk_value = 0x%x\n", clk_value);
        if ((clk_value & (1<<15)) == 0)
        {
            *sky_clkreg = clk_value | (1<<15);
//printf(" == status set 0, new *sky_clkreg = 0x%x\n", (volatile )*sky_clkreg);
        }
#if 0
    	    trick_d_status.display = SKYFB_DISP2;
    	    trick_d_status.status = SKYFB_OFF;
#endif
    }
#if 0
    if (ioctl(skyfb_fd, SKYFB_SET_DISPLAY_STATUS, &trick_d_status) == -1) {
        printf("SKYFB_SET_DISPLAY_STATUS ioctl failed\n");
        return ;
    }
#endif
#else
    return;
#endif
}

void svsd_flush_video_frame()
{
	int uninit_timeout = 20;
	vdec_dec_info.dec_flags = 4;
	vdec_dec_info.buf_size = 0;
	int svsd_video_cnts = svsd_state->queue_length;
	svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);
	while(svsd_video_cnts != 0)
	{
		if (--uninit_timeout <= 0)
		{
			break;
		}
		svsd_video_cnts = svsd_state->queue_length;
		svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);
		usleep(33000);
		printf("wait get_decode_done ...\n");
	}
	svsd_ipc_callback(&vdec_dec_info);
	sky_set_display_lock(1);

	vdec_svread_buf_t nrb;
	svread(0, &nrb, 0);
	uninit_timeout = 20;
	while(nrb.qlen[2]!= 0)
	{
		if (--uninit_timeout <= 0)
		{
			break;
		}
		svread(0, &nrb, 0);
		usleep(33000);
		printf("wait get_decode_done ...\n");
	}
	svsd_video_cnts = svsd_state->queue_length;
	svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);
	svread(0, &nrb, 0);
	printf("##### svsd_q[%d]   display_q[%d] ######\n", svsd_video_cnts, nrb.qlen[2]);
	sky_set_display_lock(0);
}

int vd_get_is_deint(void)
{
        return sky_is_output_interlace;
}

int vd_force_enqueue(void)
{
    double next_vft = 3003+last_pulldown32_vft_diff;
    double next_frame_time = ((next_vft)/90000.0);
    vdec_dec_info.pts = (double)(last_pulldown32_pts);
    vdec_dec_info.int_pts = (unsigned int)(last_pulldown32_pts*90000.0);
    vdec_dec_info.vft = (int)(next_vft);//(int)((90000.0)/(29.97)) + last_pulldown32_vft_diff;
    sky_vft_history[sky_vft_idx] = next_frame_time;//0.0333667;
    sky_vpts_history[sky_vft_idx] = (double)last_pulldown32_pts;
    sky_vft_idx = (sky_vft_idx + 1)%MAX_SKY_VFT_BUFFER;
    ioctl(skyfd, SKY_VDEC_DECODE, &vdec_dec_info);
sky_vpts_total_cnts++;
}

//Fuchun 20110808 1:disable room key, 0:enable room key
unsigned int sky_room_disable(sh_video_t *shv)
{
	unsigned int ret = 0;
	if(vdec_init_info.pulldown32_2 || vdec_init_info.pulldown32)
		ret = 1;
	if(!svsd_state->isprogressive && !sky_is_output_interlace && (sky_disp_rate == 50 || sky_disp_rate == 60))
		ret = 1;

	return ret;
}
