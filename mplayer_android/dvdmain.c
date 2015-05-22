#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "config.h"

#ifdef CONFIG_DVDNAV_MAIN2

#include "input/input.h"
#include "libvo/sub.h"
#include "libmpdemux/demuxer.h"
#include "libmpcodecs/sky_vdec_2.h"
#include "mp_core.h"
#include "help_mp.h"
#ifdef CONFIG_DVDREAD
#include "stream/stream_dvd.h"
#endif
#include "stream/stream_dvdnav.h"
#define ENABLE_KEY_EVENT_FUNCTION_MODE
#ifdef ENABLE_KEY_EVENT_FUNCTION_MODE
extern int parse_and_run_command(void);
#endif // end of ENABLE_KEY_EVENT_FUNCTION_MODE
extern int sky_vpts_total_cnts;
extern int is_first_loop;
extern int nosync;
extern int force_quit;
extern DvdnavState dvdnavstate;
extern int dvdnav_movie_sub_id, dvdnav_rel_seek, dvdnav_seek_result,dvdnav_movie_audio_id;
extern int dvdsub_id;
extern int dvdnav_sub_menu_select;
extern int dvdnav_audio_menu_select;
extern int vo_need_dvdnav_menu;
extern unsigned int read_frame_time, ipcdomain;
extern int rewind_mult;
extern int switch_audio_thread_status;
extern int audio_id;
extern int seek_sync_flag;
extern int set_pause_lock;
extern int last_audio_id;
extern double AV_delay;
extern int get_audio_pts_first_flag;
extern float audio_start_pts;
extern int audio_speed;
extern int my_current_pts;
extern int quiet;
extern vdec_shm_t *svsd_state;
extern vdec_svread_buf_t sky_nrb;
extern int audio_not_support;

extern double last_rewind_pts;
extern int duration_sec_cur;
extern int duration_sec_old;
extern int entry_while;
extern int audio_quality;
extern pthread_mutex_t mut;
extern int rel_audio_id;
extern mp_cmd_t* domainsocket_cmd;
extern int is_pause;
extern int rel_global_sub_pos;
extern int dvdnav_color_spu;
extern short edl_decision;
extern int decode_num_cnt, benchmark;
extern int check_framedrop_flag;
extern unsigned int avsync_timeout_num, avsync_timeout_cnt;
extern double sky_vdec_wait_sync_vpts, vout_time_usage;
extern double last_sub_pts;
extern int last_sub_lines;
extern int first_osd, no_osd;
extern int is_dvdnav;
extern float audio_delay;
extern int dvdnav_return_menu;
extern vdec_init2_t vdec_init2_info;
extern unsigned long long last_spu_pts;
extern char *current_module;
extern int seek_by_time_done;
extern int subcc_enabled;
extern int dvdnav_globle_num_subtitle;
extern int subcc_force_off;
extern int select_sub_id;
extern int dvdnav_first_menu;
extern int dvdnav_video_first;
extern unsigned int skydroid;
extern int subcc_reg;

static int loop_seek=0;
static int play_n_frames=-1;
static int play_n_frames_mf=-1;

extern void ipc_callback(char *datastr);
extern void sky_set_display_lock(int lock_stat);
extern int fill_audio_out_buffers_wrapper(void);
extern int sleep_until_update_original_wrapper(float *time_frame, float *aq_sleep_time);
extern int sleep_until_update_wrapper(float *time_frame, float *aq_sleep_time);
extern int sleep_until_update_dvdnav_wrapper(float *time_frame, float *aq_sleep_time);
extern void ipc_callback_seek_by_time_done(void);
extern void sky_set_vsync_status(int vsync_status);
extern int vd_get_is_deint(void);
extern int dvdnav_demuxer_set_sub_reg(demuxer_t *demuxer, int sub);
extern void ipc_callback_audio_sub(void);
extern double written_audio_pts_wrapper(sh_audio_t *sh_audio, demux_stream_t *d_audio);
extern void ipc_callback_audio_fast(void);
extern void print_status_wrapper(float a_pos, float a_v, float corr);
extern void update_subtitles(sh_video_t *sh_video, double refpts, demux_stream_t *d_dvdsub, int reset);
extern void adjust_sync_and_print_status_wrapper(int between_frames, float timing_error);
extern double update_video_wrapper(int *blit_frame);
extern int mp_dvdnav_audio_handle_wrapper(void);
extern void mp_dvdnav_spu_handle_wrapper(void);
extern void mp_dvdnav_highlight_handle_wrapper(int stream_is_change);
extern void pause_loop_wrapper(void);
extern void edl_update_wrapper(MPContext *mpctx);
extern void ipc_callback_fast(void);
extern int seek_wrapper(MPContext *mpctx, double amount, int style);
extern void FFFR_to_normalspeed(void *this);
extern void ipc_callback_duration(void);
extern void update_osd_msg_wrapper(void);
extern void ipc_callback_error(void);
extern int spudec_visible(void *this);
extern int demuxer_get_sub_reg(demuxer_t *demuxer);
extern int usec_sleep(int usec_delay);
extern int mp_dvdnav_is_stop (stream_t *stream);
extern char* mp_property_print(const char *name, void* ctx);
extern void sky_set_sync_frame_ridx(void);
extern unsigned long long spu_get_pts(void *this);
extern void clear_osd(void);
extern unsigned int GetTimer(void);
extern void uninit_video(sh_video_t *sh_video);

extern int dvdnav_toomanypkg;
extern int dvdnav_sub_reg;
extern int cvq_threshold;

int dvdloop_lock_display = 0;
char callback_names[1024];

int do_dvdloop(MPContext *mpctx, int *frame_cnt, float *c_total, double seek_to_sec, m_time_size_t *end_at, unsigned int *initialized_flags)
{
    char callback_str[42];
    int blit_frame=0;
    int frame_time_remaining=0;
int dvdloop_lock_display_timeout_cnt = 0;

printf("!!!!!!!!!! enter do_dvdloop !!!!!!!!!!\n");
//printf("dvdnavstate.dvdnav_title_state == %d\n", dvdnavstate.dvdnav_title_state);

    if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
    {
        sky_set_vsync_status(1);
        dvdloop_lock_display = 0;
    }
    else
    {
        sky_set_vsync_status(-1);
        dvdloop_lock_display = 1;
        dvdloop_lock_display_timeout_cnt = 31;
    }

    if(select_sub_id == DVDNAV_SUBCC_FLG)
    {
	subcc_enabled = 1;
    }
    else if(select_sub_id == -1 && seek_to_sec)
    {
    	subcc_force_off = 1;
    }

    if (vd_get_is_deint() == 0)
    {
        cvq_threshold = 4;//2;//3;
    }
    else
    {
//20110609 Robert use cvq_threshold = 4 for DVD, also use cur_pts for AV_delay
        cvq_threshold = 4;//2;//4;
    }
        is_first_loop = 1;
        /*
        MAIN_while________________()
        */
        while(!mpctx->eof) {
            float aq_sleep_time=0;
//printf("dvdnavstate.dvdnav_title_state[%d]\n",dvdnavstate.dvdnav_title_state);
//printf("\n==00 = seek_sync_flag = %d, wait=%d\n", seek_sync_flag, wait_video_or_audio_sync);
//printf("\n=> debug ... speed_mult:%d  seek_sync_flag:%d wait:%d AV_delay:%f\n", speed_mult, seek_sync_flag, wait_video_or_audio_sync, AV_delay);

            //printf("### Carlos in [%s][%d], vq :[%d] aq[%d] ###\n", __func__, __LINE__, mpctx->d_video->packs, mpctx->d_audio->packs);
//printf("audio_id[%d], mpctx->demuxer->audio->id[%d]\n",audio_id, mpctx->demuxer->audio->id);
//printf("mpctx->d_sub->id[%d],dvdnav_movie_sub_id[%d],dvdsub_id[%d],subcc_enabled[%d]##\n", mpctx->d_sub->id,dvdnav_movie_sub_id,dvdsub_id,subcc_enabled);
#if 0
//if (mpctx->delay < 0)
            {
                static float last_delay=1.0;
                vdec_svread_buf_t nrb;
//printf("bf sv_read...\n");
                svread(0, &nrb, 0);
                if (nrb.qlen[2] == 0)
                    printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d [2]:[%d]r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.qlen[2], nrb.ft_ridx, nrb.ft_widx);
                /*
                printf("apts2 = %f next_pts = %f ", playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out), sky_get_next_pts(nrb.cur_pts));
                if (last_delay < 0.0 && mpctx->delay > 0.0)
                {
                printf("!!! === a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
                }
                else
                {
                printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
                }
                */
                last_delay = mpctx->delay;
//printf("=== a_pts:%f  v_pts:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
//printf("=== a_pts:%f v_pts:%f delay:%f  cur_pts:%f vsync_num:%d r:%d w:%d\n", mpctx->d_audio->pts, mpctx->d_video->pts, mpctx->delay, nrb.cur_pts, nrb.qlen[3], nrb.ft_ridx, nrb.ft_widx);
            }
#endif

            if(force_quit)
            {
                break;
            }

#ifdef DEMUX_THREAD //charleslin 20100603
            //demux_check(mpctx->demuxer);
#endif

if(seek_by_time_done && (wait_video_or_audio_sync & AVSYNC_NORMAL) && seek_sync_flag==0)
	ipc_callback_seek_by_time_done();

            if(dvd_last_chapter>0) {
                int cur_chapter = demuxer_get_current_chapter(mpctx->demuxer);
                if(cur_chapter!=-1 && cur_chapter+1>dvd_last_chapter)
                {
                    //goto goto_next_file;
                    return 3;
                }
            }
//printf("mpctx->demuxer->sub->id[%d],mpctx->d_sub->id[%d],dvdsub_id[%d]\n",mpctx->demuxer->sub->id,mpctx->d_sub->id,dvdsub_id);
			if (mpctx->stream->type == STREAMTYPE_DVDNAV)
			{
				if(dvdnavstate.dvdnav_state_change == 1) // for call back status menu or movie
				{
printf("dvdnav_button_on[%d]\n",dvdnavstate.dvdnav_button_on);
					clear_osd();
					if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
					{
						//right now do nothing
						if(dvdnav_first_menu == 0)
						{
							extern int dvdnav_resume; //Mantis:6150
							if(!dvdnav_resume)
							{
							printf("-----clear subtitle register -----\n");
							dvdnav_demuxer_set_sub_reg(mpctx->demuxer, -3);
							}

							if(mpctx->audio_out && vd_get_is_deint())
							{
								//Mantis:5596 i-mode menu can't hold
								printf("----- audio out reset -----\n");
								mpctx->audio_out->reset();
							}
						}
							
						dvdnav_first_menu = 1;
//20110706 Robert fix mantis 5342
						wait_video_or_audio_sync = AVSYNC_NORMAL;

                                                if(dvdnav_toomanypkg == 1)
                                                {
                                                	//Dirty prety things.iso skip menu
                                                	change_flag = 1;
                                                	dvdnav_toomanypkg = 0;
                                        	}
					}
					else if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
					{
						*frame_cnt = 0;

						
						dvdnav_sub_reg = demuxer_get_sub_reg(mpctx->demuxer);

mplayer_dvdnav_debug("#1# DVDNAV In [%s][%d],mpctx->d_sub->id[%d],dvdnav_movie_sub_id[%d],dvdsub_id[%d],dvdnav_sub_reg[%d]##\n", __func__, __LINE__,mpctx->d_sub->id,dvdnav_movie_sub_id,dvdsub_id,dvdnav_sub_reg);
						//if(skydroid)
						if(1) //Mantis:6066
						{
							int x,y=0;
							for (x = 0; x < 32; x++)
							{
								if(dvdnavstate.sid[x] == dvdnav_sub_reg)
									dvdsub_id = y;
								else if(dvdnavstate.sid[x] != -1)
									y++;
							}
						}

						if(dvdnav_sub_menu_select == 2)
						{
							//if(skydroid)
							if(1) //Mantis:6066
								mpctx->d_sub->id = dvdnav_movie_sub_id;
							else
								dvdsub_id = mpctx->d_sub->id = dvdnav_movie_sub_id;
							
							if(mpctx->d_sub->id == -2 && subcc_reg)
								subcc_enabled = 1;
								
						}
						if(dvdnav_audio_menu_select == 2)
							mpctx->demuxer->audio->id = dvdnav_movie_audio_id;
						if(dvdnavstate.dvdnav_button_on) //for dolby
							mpctx->d_sub->id = 0;

mplayer_dvdnav_debug("#2# DVDNAV In [%s][%d],mpctx->d_sub->id[%d],dvdnav_movie_sub_id[%d],dvdsub_id[%d]##\n", __func__, __LINE__,mpctx->d_sub->id,dvdnav_movie_sub_id,dvdsub_id);
					}
					dvdnavstate.dvdnav_state_change = 0;
				}
			}

            //if(!mpctx->sh_audio && mpctx->d_audio->sh) {
            if(!mpctx->sh_audio && mpctx->d_audio->sh && !switch_audio_thread_status && !audio_not_support) {
                mpctx->sh_audio = mpctx->d_audio->sh;
                mpctx->sh_audio->ds = mpctx->d_audio;
                reinit_audio_chain();

				if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE){
		    if ( ((not_supported_profile_level == 2) || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
		    {
                        if (not_supported_profile_level == 3)
                        	strcpy(callback_str,"status: playing 3\0");
                        else
                        {
                        	if (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001))
					strcpy(callback_str,"status: playing 2\0");
				else
					strcpy(callback_str,"status: playing 4\0");
				sprintf(callback_names, "AUDIO: No Audio can switch\0");
				ipc_callback(callback_names);
                        }
		    }
		    else if (not_supported_profile_level == 1)
		    {
			strcpy(callback_str,"status: playing 1\0");
			ipc_callback(callback_str);
			free_demuxer(mpctx->demuxer);
			mpctx->demuxer = NULL;
//			goto goto_next_file;
                        return 3;

		    }
		    else
			strcpy(callback_str,"status: playing 0\0");
		    ipc_callback(callback_str);
		    
		    ipc_callback_audio_sub();
		}

                if(speed_mult == 0 && mpctx->sh_audio && mpctx->sh_video && nosync==0 && dvdnavstate.dvdnav_title_state != DVDNAV_TITLE_STATE_MENU && (is_first_loop == 1) ) 	//Fuchun 2010.07.20
                {
#ifdef DVBT_USING_NORMAL_METHOD
// TODO, Need FuChun add new A-V sync method, for dvb-T
                    if (mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
                        seek_sync_flag = 2;
                    else
                        seek_sync_flag = 3;
#else // else of DVBT_USING_NORMAL_METHOD
                    seek_sync_flag = 2;
#endif
                }
            }

            /*========================== PLAY AUDIO ============================*/


//if (mpctx->sh_audio)
//if (mpctx->sh_audio && audio_id != -2)	//Fuchun 2009.12.02
//Robert 20101130 PAUSE command lock/unlock flag
            //if ((set_pause_lock==0) && mpctx->sh_audio && audio_id != -2)	//Fuchun 2009.12.02
            if ((set_pause_lock==0) && mpctx->sh_audio && audio_id != -2 && !switch_audio_thread_status)	//Fuchun 2009.12.02
            {
            	  last_audio_id = audio_id;
//printf("==  wait_video=%d  AV_delay=%f seek_sync_flag=%d\n", wait_video_or_audio_sync, (float)AV_delay, seek_sync_flag);
                if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                {
                }
                else
                {
//Robert 20100712 keep audio decoding in STREAMing mode, fix me later...
//                    if (wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
//                    {
////printf("\n==> wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO AV_delay=%f\n", AV_delay);
//                        fill_audio_out_buffers_wrapper();
//                    }

#if 1
                    if(AV_delay > 1.0 && mpctx->stream->type == STREAMTYPE_STREAM)
                    {
                    }
                    else if((dvdloop_lock_display==0) && (wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO) && AV_delay <= ( - 0.02))//0.1)	//Fuchun 2010.07.05
                    {

#ifdef  jfueng_2011_0311

        if (audio_long_period == 0)
        	{

		printf("===== video catch up with audio!!  AV_delay[%f]\n", AV_delay);
		sky_set_display_lock(0);
		mpctx->delay = 0;
		wait_video_or_audio_sync = AVSYNC_NORMAL;


        	}
	else
		{


		    if (mpctx->d_audio->packs  > 0)
	            {   //JF 
 		          printf("\nJF:===== video catch up with audio!!  AV_delay[%f]\n", AV_delay);
		
		          sky_set_display_lock(0);
		
		          mpctx->delay = mpctx->audio_out->get_delay();
		          wait_video_or_audio_sync = AVSYNC_NORMAL;

	             }				

		}


#else
                        printf("===== video catch up with audio!!  AV_delay[%f] apts:%f\n", AV_delay, (float)mpctx->d_audio->pts);
                        sky_set_display_lock(0);
                        //Barry 2011-01-13
                        mpctx->delay = 0;
                        wait_video_or_audio_sync = AVSYNC_NORMAL;

#endif //JF

                    }
                    else if(((dvdloop_lock_display==0)) && (wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO) && AV_delay >= (-0.1))	//Fuchun 2010.07.20
                    {
                        printf("===== audio catch up with video!!  AV_delay[%f]\n", AV_delay);
                        if(AV_delay >= 0.1)
                            wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
                        else
                        {
                            sky_set_display_lock(0);
                            mpctx->delay = 0;
                            wait_video_or_audio_sync = AVSYNC_NORMAL;
                        }
                    }
					/* add check  mpctx->sh_audio && mpctx->audio_out on 2011-05-13 for mantis 4626 by carlos*/
					else if((!(wait_video_or_audio_sync&AVSYNC_VIDEO_CATCH_AUDIO)) &&  mpctx->sh_audio && mpctx->audio_out && !dvdnav_video_first) 
#endif
                    {
                        if (!fill_audio_out_buffers_wrapper())
                        {
                            // at eof, all audio at least written to ao
                            if (!mpctx->sh_video)
                                mpctx->eof = PT_NEXT_ENTRY;
                        }
                        else
                        {
                            if(!mpctx->sh_video && get_audio_pts_first_flag == 1 && written_audio_pts_wrapper(mpctx->sh_audio, mpctx->d_audio) > 0.01)
                            {
                                get_audio_pts_first_flag = 0;
                                audio_start_pts = written_audio_pts_wrapper(mpctx->sh_audio, mpctx->d_audio);
                                printf("@@@ audio_start_pts[%f] @@@\n", audio_start_pts);
                            }

                            if(audio_speed >= 3 || audio_speed < 0)
                                usleep(100000);

                            if(audio_speed < 0 && (written_audio_pts_wrapper(mpctx->sh_audio, mpctx->d_audio) - audio_start_pts + audio_speed) < 0)
                            {
                                audio_speed = 0;
                                abs_seek_pos = 1;
                                rel_seek_secs = 0.0;
                                ipc_callback_audio_fast();
                            }
                        }
                    }
                    else if(dvdnav_video_first == 1)
                    {
				static int timeout_cnt = 0;
				int timeout_threshold = 10;
				timeout_cnt++;
				if(timeout_cnt > timeout_threshold)
				{
					printf("=== In[%s][%d]  timeout ===\n", __func__, __LINE__);
					timeout_cnt = 0;
					dvdnav_video_first = 0;
				}
                    }
                }
            }

            if(!mpctx->sh_video) {
                // handle audio-only case:
                double a_pos=0;
                // sh_audio can be NULL due to video stream switching
                // TODO: handle this better
                //if(!quiet || end_at.type == END_AT_TIME ) //SkyMedi_Vincent09072009 mark: always evaluate current position of a_pos
                a_pos = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);

//                my_current_pts = a_pos;
                dvdnav_video_first = 0;

                if(!quiet)
                {
//Robert 20101217 reduce print_status cpu loading
                    static double last_nov_apts=0.0;
                    if (mpctx->d_audio && (last_nov_apts != a_pos))
                    {
                        print_status_wrapper(a_pos, 0, 0);
                        last_nov_apts = a_pos;
                    }
                }

                if(end_at->type == END_AT_TIME && end_at->pos < a_pos)
                    mpctx->eof = PT_NEXT_ENTRY;
                update_subtitles(NULL, a_pos, mpctx->d_sub, 0);
                update_osd_msg_wrapper();

//Robert 20100329 handle audio only case, prevent cpu busy
//  if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                {
//printf("Usleep:%s:%d\n", __FUNCTION__, __LINE__);
//    usec_sleep(1000);
                    usec_sleep(1001);
                }

            } else {
                /*========================== PLAY VIDEO ============================*/
                vo_pts=mpctx->sh_video->timer*90000.0;
                vo_fps=mpctx->sh_video->fps;

//Robert 20101130 PAUSE command lock/unlock flag
                if (set_pause_lock == 1)
                {
					/* add check mpctx->audio_out on 2011-05-13 for mantis 4626 by carlos*/
					if (mpctx->sh_audio && audio_id != -2 && ((mpctx->audio_out && (mpctx->audio_out->get_delay() <= 0.03)) || !mpctx->audio_out))
                        set_pause_lock = 2;
                }

//                my_current_pts = mpctx->sh_video->pts;
//printf("mpctx->sh_video->pts[%f]\n",mpctx->sh_video->pts);
#if 1
                int svsd_video_cnts = svsd_state->queue_length;
//Robert 20101004 need checking mplayer side counter
                svsd_video_cnts += (svsd_state->send_count - svsd_state->recv_count);

                if (nosync == 0)
        		adjust_sync_and_print_status_wrapper(frame_time_remaining, mpctx->time_frame);

                int check_lock_by_audio_pkts = 0;
/*                
                if (mpctx->sh_audio && mpctx->d_audio->packs > 2 )
                {
                    if (mpctx->audio_out->get_delay() > 0.08)
                        check_lock_by_audio_pkts = 1;
                }
#ifdef CONFIG_DEBUG_AVSYNC
                else 
                {
                    if (seek_sync_flag > 0)
                    {
extern last_audio_pts;
            double a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
printf("\n===BF DECODE CHECK a_pts:%f SH_pts:%f  diff:%f\n", last_audio_pts, mpctx->sh_video->pts, last_audio_pts - mpctx->sh_video->pts);
//printf(" cvq:%d\n", svsd_state->queue_length);
printf(" ridx:%d  widx:%d   vft diff:%f\n", sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_get_vft_total(0, (sky_nrb.ft_widx-sky_nrb.ft_ridx+128)%128));
printf("\n#### seek flag : %d,   cur_pts=%f  sh vpts:%f  a_pts:%f\n", seek_sync_flag, sky_nrb.cur_pts, mpctx->sh_video->pts, a_pts);

                    }
                }
#endif
*/
                if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU ||
                    speed_mult != 0 || (mpctx->d_video->packs < 1))
                {
                    check_lock_by_audio_pkts = 0;
                    if (dvdloop_lock_display_timeout_cnt == 0)
                    {
                        dvdloop_lock_display = 0;
                        sky_set_vsync_status(1);
                    }
//printf("### seek flag:%d WAIT:%d ridx:%d  widx:%d   vft diff:%f\n", seek_sync_flag, wait_video_or_audio_sync, sky_nrb.ft_ridx, sky_nrb.ft_widx, 0);
                }
/*
{
vdec_svread_buf_t nrb;
svread(0, &nrb, 0);
if (nrb.qlen[2] == 0)
printf("=== [2]:[%d] r:%d w:%d svsd cnt:%d num_buffered_frames=%d buf=%d\n", nrb.qlen[2], nrb.ft_ridx, nrb.ft_widx, svsd_video_cnts, mpctx->num_buffered_frames, chk_bufspace(mpctx->demuxer));
}
*/
                if (dvdloop_lock_display == 1)
                {
                    if (dvdloop_lock_display_timeout_cnt > 0)
                        dvdloop_lock_display_timeout_cnt--;
                    else
                    {
                        dvdloop_lock_display = 0;
                        sky_set_vsync_status(1);
                    }
                }
//printf("check_lock_by_audio_pkts=%d mpctx->num_buffered_frames:%d svsd_video_cnts:%d\n", check_lock_by_audio_pkts, mpctx->num_buffered_frames, svsd_video_cnts);

                if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS && mpctx->d_video->packs == 0 )
                {
                }
                else
#ifdef BITSTREAM_BUFFER_CONTROL
                    if (check_lock_by_audio_pkts==0 && !mpctx->num_buffered_frames && /*!(wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO) &&*/ chk_bufspace(mpctx->demuxer) && svsd_video_cnts <= cvq_threshold)
#else
                    if (svsd_video_cnts <= cvq_threshold && !mpctx->num_buffered_frames /*&& !(wait_video_or_audio_sync&AVSYNC_AUDIO_CATCH_VIDEO)*/)
#endif
#else
                if (!mpctx->num_buffered_frames && wait_video_or_audio_sync != 2)
#endif
                    {
                        if(dvdnav_video_first == 1)	//Fuchun 20110721 video need read data first when audio_stream_change
				dvdnav_video_first = 0;
                        double frame_time = update_video_wrapper(&blit_frame);
//Robert 20100324 benchmark

				if(change_flag == 2)	//Fuchun 2011.04.21 fix a/v not sync when switch between menu and movie repeatedly or seek chapter
				{
					change_flag = 0;
					mpctx->startup_decode_retry = 6;
				}
				
                        if((speed_mult < 0 || speed_mult >= 2) && decode_num_cnt == 0)
                        {
                            unsigned int sleep_time = 0;
                            unsigned int total_sleep_time = 0;
                            total_sleep_time = 400000/(1 << (abs(speed_mult)-1));

                            if(total_sleep_time > read_frame_time)
                            {
                                sleep_time = total_sleep_time - read_frame_time;
                                usleep(sleep_time);
                            }
                        }

                        if(decode_num_cnt) decode_num_cnt--;
//printf("     ##3  blit_frame=%d  startup=%d\n", blit_frame, mpctx->startup_decode_retry);
//      while (!blit_frame && mpctx->startup_decode_retry > 0)
                        if (mpctx->startup_decode_retry > 0)
                        {
                            //double need_to_catch_pts = mpctx->d_video->pts;
                            double need_to_catch_pts = mpctx->sh_video->pts;	//modify by Robert

//      int decoder_need_frame = -1, need_check_decoder_ref = 1, decoder_ref_cnts;
//Robert 20101230 don't need to check decoder ref
                            //int decoder_need_frame = -1, need_check_decoder_ref = 0, decoder_ref_cnts=0;	//Barry 2011-01-11 disable, use in A2
                            int decoder_ref_cnts=0;
                            int ft_idx_cnts;

#if 1
                            svread(0, &sky_nrb, 0);
//      printf("!!!!! decoder_need_frame = %d\n", decoder_need_frame);
                            ft_idx_cnts = (sky_nrb.ft_widx - sky_nrb.ft_ridx + 128)%128;
				int idx_count = (vdec_init2_info.double_deint == 1 ? 4 : 2);

                            while(ft_idx_cnts > idx_count)
                            {
                                sky_set_sync_frame_ridx();
                                svread(0, &sky_nrb, 0);
                                ft_idx_cnts = (sky_nrb.ft_widx - sky_nrb.ft_ridx + 128)%128;
                            }
#endif
//printf("!!! #@@@@@# pts:%f r:%d w:%d qlen[2]=%d need_to_catch_pts=%f AV=%f \n", sky_nrb.cur_pts, sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_nrb.qlen[2], (float)need_to_catch_pts, (float)AV_delay);

                            while (mpctx->startup_decode_retry > 0) {
                                double delay = mpctx->delay;
                                if (need_to_catch_pts == MP_NOPTS_VALUE)
                                {
                                    need_to_catch_pts = 0.0;
                                }
                                else if (need_to_catch_pts == 0.0 && mpctx->sh_video->pts != MP_NOPTS_VALUE)
                                {
                                    if ((mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS) || (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS && mpctx->sh_video->pts != 0.0))
                                        need_to_catch_pts = mpctx->sh_video->pts;
                                }

                                // these initial decode failures are probably due to codec delay,
                                // ignore them and also their probably nonsense durations
#if 1
                                svsd_video_cnts = svsd_state->queue_length;
                                int time_cnt=0;
#ifdef BITSTREAM_BUFFER_CONTROL
                                while (chk_bufspace(mpctx->demuxer) == 0)
#else
                                while (svsd_video_cnts > cvq_threshold)
#endif
                                {
                                    usleep(1000);
                                    svsd_video_cnts = svsd_state->queue_length;

                                    if(time_cnt++ > 100)
                                    {
                                        printf("-- svsd_video_cnts no change --\n");
                                        mpctx->startup_decode_retry = 0;
                                        break;
                                    }
                                }
#endif
                                {
                                    if (nosync == 0)
                                        adjust_sync_and_print_status_wrapper(frame_time_remaining, mpctx->time_frame);
                                    update_video_wrapper(&blit_frame);

                                    mpctx->delay = delay;
                                    mpctx->startup_decode_retry--;

#if 1

//printf(" #@@@@@# pts:%f r:%d w:%d qlen[2]=%d need_to_catch_pts=%f AV=%f \n", sky_nrb.cur_pts, sky_nrb.ft_ridx, sky_nrb.ft_widx, sky_nrb.qlen[2], (float)need_to_catch_pts, (float)AV_delay);
//printf("seek_sync_flag=%d\n", seek_sync_flag);
//check again
                                    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                                    {
                                    }
                                    else
                                    {
                                        if ((seek_sync_flag == 1 ? sky_nrb.cur_pts != 0.0 : 1) && sky_nrb.cur_pts >= need_to_catch_pts && ((sky_nrb.cur_pts - need_to_catch_pts <= 0.06)) )
                                        {
                                            mpctx->startup_decode_retry = 0;
                                        }
                                        else if ((seek_sync_flag == 1 ? sky_nrb.cur_pts != 0.0 : 1) && need_to_catch_pts >= sky_nrb.cur_pts && (need_to_catch_pts - sky_nrb.cur_pts <= 0.06))
                                        {
                                            mpctx->startup_decode_retry = 0;
                                        }
                                    }


//		  if (mpctx->startup_decode_retry == 0)
                                    if (mpctx->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
                                    {
                                    }
                                    else
                                    {
                                        if(mpctx->startup_decode_retry == 0 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && seek_sync_flag == 0)	//Fuchun 2010.08.19
                                        {
//			if (is_first_loop == 0)
                                            sky_set_display_lock(0);
                                        }
                                    }
#endif
                                }
				svread(0, &sky_nrb, 0);
                            }
                        }

                        if(dvdnav_return_menu)
                        {
                            int button = -1;
                            mp_dvdnav_handle_input(mpctx->stream,MP_CMD_DVDNAV_MENU,&button);
                            dvdnav_return_menu = 0;
                        }

                        mpctx->startup_decode_retry = 0;
                        mp_dbg(MSGT_AVSYNC,MSGL_DBG2,"*** ftime=%5.3f ***\n",frame_time);
                        if (mpctx->sh_video->vf_initialized < 0) {
                            mp_msg(MSGT_CPLAYER,MSGL_FATAL, MSGTR_NotInitializeVOPorVO);
                            mpctx->eof = 1;
                            if(ipcdomain) //using domain socket option
                                ipc_callback_error();
//                            goto goto_next_file;
                            return 3;

                        }
                        if (frame_time < 0)
                        {
                            mpctx->eof = 1;
                        }
                        else
                        {
                            // might return with !eof && !blit_frame if !correct_pts
                            if (mpctx->demuxer->type != DEMUXER_TYPE_SKYMPEG_TS)
                            {
                                mpctx->num_buffered_frames += blit_frame;
                            }
                            mpctx->time_frame += frame_time / playback_speed;  // for nosound
                        }

                    }
                    else
                    {
//	if (speed_mult > 0 || (speed_mult == 0 && audio_id < 0))
                        if (speed_mult > 0 || (speed_mult == 0 && !mpctx->sh_audio))
                        {
//printf("Usleep:%s:%d\n", __FUNCTION__, __LINE__);
//  		usec_sleep(1000);	//1ms, avoid system too busy at FF 2x or 4x
                            usec_sleep(1002);	//1ms, avoid system too busy at FF 2x or 4x
                        }

//      printf("@@ no update_video svsd_cnts=%d num_buf_frames=%d wait_va_sync=%d\n",svsd_video_cnts, mpctx->num_buffered_frames, wait_video_or_audio_sync);
                    }
// ==========================================================================


                current_module="draw_osd";
                int draw_by_ass = 0;
                if (ass_enabled)
                {
                    sh_sub_t* sh = mpctx->d_sub->sh;
                    if (sh && (sh->type == 'a'))
                    {
                        draw_by_ass = 1;
                    } else {
                        draw_by_ass = 0;
                    }
                }

//    if(vo_config_count) mpctx->video_out->draw_osd();

#ifdef CONFIG_DVDNAV
		if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
		{
#endif /* CONFIG_DVDNAV */
                    if(speed_mult == 0 && FR_to_end == 0 && vo_need_osd == 1 && (draw_by_ass == 0))
                    {
                        //Fuchun 2010.03.25
                        if(mpctx->global_sub_size > 0 && mpctx->set_of_sub_pos >= 0)		//for external subtitle file
                        {
                            if(sub_visible)
                            {
                                mpctx->video_out->draw_osd();
                                sub_visible = 0;
                            }
                        }
                        else if(vo_spudec != NULL && subcc_enabled == 0) //dvdnav pgs
                        {
                            if(spudec_visible(vo_spudec) && spu_get_pts(vo_spudec) != last_spu_pts)
                            {
                                mpctx->video_out->draw_osd();
                                last_spu_pts = spu_get_pts(vo_spudec);
                            }
                            else if(!spudec_visible(vo_spudec) && spu_get_pts(vo_spudec) == last_spu_pts)
                            {
//				mpctx->video_out->draw_osd();
                                clear_osd();
                                last_spu_pts = -1;
                            }                            
                        }
                        else if(vo_sub != NULL && ((vo_sub->endpts[vo_sub->lines] != last_sub_pts) ||		//for the subtitle is a part of media file
                                                   (vo_sub->lines != last_sub_lines) || sub_visible == 1))
                        {
                            if (vo_sub->lines == 0)
                                clear_osd();
                            else
                                mpctx->video_out->draw_osd();
                            last_sub_pts = vo_sub->endpts[vo_sub->lines];
                            last_sub_lines = vo_sub->lines;
                            sub_visible = 0;
                        }

                        //Fuchun 2010.03.24  need once draw osd
                        if(vo_need_osd == 1 && first_osd)
                        {
                            first_osd = 0;
                            mpctx->video_out->draw_osd();
                        }
                    }
#if 1	//Fuchun 20110823 disable
                    else if(!skydroid && speed_mult == 0 && dvdnav_globle_num_subtitle==0 && vo_sub!=NULL && subcc_enabled==1 && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)
                    {
			//for subcc resume -1
			mpctx->video_out->draw_osd();
                    }
#ifdef CONFIG_DVDNAV
		}
#endif /* CONFIG_DVDNAV */
#endif

                current_module="vo_check_events";
                if (vo_config_count) mpctx->video_out->check_events();

//Robert 20100324 benchmark
                if(no_osd)
                {
                    frame_time_remaining = sleep_until_update_original_wrapper(&mpctx->time_frame, &aq_sleep_time);
                }
                //else if(((speed_mult == 0 && FR_to_end == 0) || is_mjpeg == 1) && (wait_video_or_audio_sync&AVSYNC_NORMAL) && check_framedrop_flag == 0)	//Fuchun 2010.05.03
                else if(((speed_mult == 0 && FR_to_end == 0) || is_mjpeg == 1) && (wait_video_or_audio_sync&AVSYNC_NORMAL) && check_framedrop_flag == 0 && !switch_audio_thread_status)	//Fuchun 2010.05.03
                {
//                    frame_time_remaining = sleep_until_update_wrapper(&mpctx->time_frame, &aq_sleep_time);
                    frame_time_remaining = sleep_until_update_dvdnav_wrapper(&mpctx->time_frame, &aq_sleep_time);

                }
                else
                {
                    if(speed_mult != 0 && (seek_sync_flag != 0 || wait_video_or_audio_sync != AVSYNC_NORMAL))	//Fuchun 20110719 avoid video freeze
                    {
				seek_sync_flag = 0;
				wait_video_or_audio_sync = AVSYNC_NORMAL;
                    }
                    frame_time_remaining = 0;
                }
//====================== FLIP PAGE (VIDEO BLT): =========================
                if (benchmark)
                {
                    current_module="flip_page";
#if 1
#ifdef BITSTREAM_BUFFER_CONTROL
//                    if (!frame_time_remaining && blit_frame && chk_bufspace(mpctx->demuxer))
                    if ((mpctx->num_buffered_frames > 0) && !frame_time_remaining && blit_frame)
#else
                    if (!frame_time_remaining && blit_frame && (svsd_video_cnts <= cvq_threshold))
#endif
#else
                    if (!frame_time_remaining && blit_frame)
#endif
                    {
                        unsigned int t2=GetTimer();

                        if(vo_config_count) mpctx->video_out->flip_page();
                        mpctx->num_buffered_frames--;

                        vout_time_usage += (GetTimer() - t2) * 0.000001;
                    }
                }
                else
                {
#ifdef BITSTREAM_BUFFER_CONTROL
//                    if (!frame_time_remaining && blit_frame && chk_bufspace(mpctx->demuxer))
                    if ((mpctx->num_buffered_frames > 0) && !frame_time_remaining && blit_frame)
#else
                    if (!frame_time_remaining && blit_frame && (svsd_video_cnts <= cvq_threshold))
#endif
                    {
                        mpctx->num_buffered_frames--;
                    }
                }

                if (mpctx->num_buffered_frames < 0)
                    mpctx->num_buffered_frames = 0;
//====================== A-V TIMESTAMP CORRECTION: =========================

		if (nosync == 0)
			adjust_sync_and_print_status_wrapper(frame_time_remaining, mpctx->time_frame);
		else
		{
			if(!quiet)
			{
				if(mpctx->sh_audio && !switch_audio_thread_status)
				{
					//Robert 20101217 reduce print_status cpu loading
					static double last_vpts=0.0, last_apts=0.0;
					if (mpctx->d_video && (last_vpts != mpctx->d_video->pts || last_apts != mpctx->d_audio->pts))
					{
						float a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
						float v_pts;
						svread(0, &sky_nrb, 0);
						v_pts = sky_nrb.cur_pts;
						
#ifdef SUPPORT_QT_BD_ISO_ENHANCE
						if ( (vdec_init2_info.pulldown32 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && !seek_sync_flag)
							|| (quick_bd && mpctx->sh_video->format != 0x10000005)	//Barry 2011-01-14
							|| (is_dvdnav))
#else
						if(vdec_init2_info.pulldown32 && (wait_video_or_audio_sync&AVSYNC_NORMAL) && !seek_sync_flag)
#endif
							AV_delay = a_pts - audio_delay - mpctx->sh_video->pts;
						else if(sky_vdec_wait_sync_vpts != 0.0)
							AV_delay = a_pts - audio_delay - sky_vdec_wait_sync_vpts;
						else
							AV_delay = a_pts - audio_delay - v_pts;
						
						print_status_wrapper(a_pts - audio_delay, AV_delay, *c_total);
						last_vpts = mpctx->d_video->pts;
						last_apts = mpctx->d_audio->pts;
					}
				}
				else
				{
					//Robert 20101217 reduce print_status cpu loading
					static double last_noa_vpts=0.0;
					if (mpctx->d_video && last_noa_vpts != mpctx->d_video->pts )
					{
						print_status_wrapper(0, 0, 0);
						last_noa_vpts = mpctx->d_video->pts;
					}
				}
			}
		}

                if (dvdloop_lock_display == 1)
                {
//printf("\n====> dvdloop_lock_display = %d,  AV_delay = %f  sky_vpts_total_cnts:%d\n", dvdloop_lock_display, AV_delay, sky_vpts_total_cnts);
//printf("d_audio->pts:%f\n", mpctx->d_audio->pts);
                    if (dvdloop_lock_display_timeout_cnt > 0)
                    {
                        if (mpctx->sh_audio && audio_id != -2)
                        {
                            if (mpctx->d_audio->pts > 0 && AV_delay > -0.06 && AV_delay < 0.06 && sky_vpts_total_cnts > 6)
                            {
                                sky_set_vsync_status(1);
#ifdef CONFIG_DEBUG_AVSYNC
printf("\n====>001 dvdloop_lock_display = %d,  AV_delay = %f  sky_vpts_total_cnts:%d\n", dvdloop_lock_display, AV_delay, sky_vpts_total_cnts);
printf("d_audio->pts:%f  timeout_cnt:%d\n", mpctx->d_audio->pts, dvdloop_lock_display_timeout_cnt);
#endif
                                dvdloop_lock_display = 0;
                                dvdloop_lock_display_timeout_cnt = 0;
                            }
                            else
                            {
#ifdef CONFIG_DEBUG_AVSYNC
printf("\n====> --- dvdloop_lock_display = %d,  AV_delay = %f  sky_vpts_total_cnts:%d\n", dvdloop_lock_display, AV_delay, sky_vpts_total_cnts);
printf("d_audio->pts:%f  timeout_cnt:%d\n", mpctx->d_audio->pts, dvdloop_lock_display_timeout_cnt);
#endif
                            }

                        }
                        else
                        {
                            sky_set_vsync_status(1);
#ifdef CONFIG_DEBUG_AVSYNC
printf("\n====>002 dvdloop_lock_display = %d,  AV_delay = %f  sky_vpts_total_cnts:%d\n", dvdloop_lock_display, AV_delay, sky_vpts_total_cnts);
printf("d_audio->pts:%f  timeout_cnt:%d\n", mpctx->d_audio->pts, dvdloop_lock_display_timeout_cnt);
#endif
                            dvdloop_lock_display = 0;
                            dvdloop_lock_display_timeout_cnt = 0;
                        }
                    }
                    else
                    {
#ifdef CONFIG_DEBUG_AVSYNC
printf("\n====>003 dvdloop_lock_display = %d,  AV_delay = %f  sky_vpts_total_cnts:%d\n", dvdloop_lock_display, AV_delay, sky_vpts_total_cnts);
printf("d_audio->pts:%f  timeout_cnt:%d\n", mpctx->d_audio->pts, dvdloop_lock_display_timeout_cnt);
#endif
                        dvdloop_lock_display_timeout_cnt = 0;
                        sky_set_vsync_status(1);
                        dvdloop_lock_display = 0;
                    }
                }
		

		//Fuchun 2011.01.07 add timeout for av-sync
		if(seek_sync_flag == 1 || seek_sync_flag == 2)
		{
			if(avsync_timeout_cnt > avsync_timeout_num)
			{
				seek_sync_flag = 0;
				avsync_timeout_cnt = 0;
				if(AV_delay > 0)
					wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
			}
			if(!mpctx->num_buffered_frames && (AV_delay < -10 || AV_delay > 10))
				avsync_timeout_cnt++;
		}
		else if(avsync_timeout_cnt)
			avsync_timeout_cnt = 0;
//============================ Auto QUALITY ============================
#if 0
                /*Output quality adjustments:*/
                if(auto_quality>0) {
                    current_module="autoq";
//  float total=0.000001f * (GetTimer()-aq_total_time);
//  if(output_quality<auto_quality && aq_sleep_time>0.05f*total)
                    if(output_quality<auto_quality && aq_sleep_time>0)
                        ++output_quality;
                    else
//  if(output_quality>0 && aq_sleep_time<-0.05f*total)
                        if(output_quality>1 && aq_sleep_time<0)
                            --output_quality;
                        else if(output_quality>0 && aq_sleep_time<-0.050f) // 50ms
                            output_quality=0;
//  printf("total: %8.6f  sleep: %8.6f  q: %d\n",(0.000001f*aq_total_time),aq_sleep_time,output_quality);
                    set_video_quality(mpctx->sh_video,output_quality);
                }
#endif
                if (play_n_frames >= 0 && !frame_time_remaining && blit_frame) {
                    --play_n_frames;
                    if (play_n_frames <= 0) mpctx->eof = PT_NEXT_ENTRY;
                }


// FIXME: add size based support for -endpos
                if (end_at->type == END_AT_TIME &&
                        !frame_time_remaining && end_at->pos <= mpctx->sh_video->pts)
                    mpctx->eof = PT_NEXT_ENTRY;

#if 1	//Fuchun 2010.05.19
                if(mpctx->eof == 1 && speed_mult == 0 && mpctx->sh_audio)
                {
                    if(mpctx->d_audio->eof == 0 && mpctx->d_video->eof == 1)
                    {
                        mpctx->eof = 0;

                        {
                            unsigned int tmp_initialized_flags = *initialized_flags;
                            tmp_initialized_flags&=~INITIALIZED_VCODEC;
                            *initialized_flags = tmp_initialized_flags;
                            seek_sync_flag = 0;
                            wait_video_or_audio_sync = AVSYNC_NORMAL;
                            current_module="uninit_vcodec";
                            if(mpctx->sh_video) uninit_video(mpctx->sh_video);
                            mpctx->sh_video=NULL;
#ifdef CONFIG_MENU
                            vf_menu=NULL;
#endif
                        }
                    }
                }
#endif
            } // end if(mpctx->sh_video)

#ifdef CONFIG_DVDNAV
            if (mpctx->stream->type == STREAMTYPE_DVDNAV) {

                if(dvdnav_color_spu==0) {
                    nav_highlight_t hl;
                    mp_dvdnav_get_highlight (mpctx->stream, &hl);
                    osd_set_nav_box (hl.sx, hl.sy, hl.ex, hl.ey);
                    vo_osd_changed (OSDTYPE_DVDNAV);
                }
#if 0
                else
                {
                    if (mp_dvdnav_is_stream_change()) // is cell change (title, part, audio, subtitle)
                    {

                        double ar=-1.0;
                        if(stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
                            mpctx->sh_video->stream_aspect = ar;
                        mp_dvdnav_audio_handle_wrapper();	// call audio process
                        mp_dvdnav_spu_handle_wrapper();		// call subtitle process
#ifdef HAVE_NEW_GUI
                        if ( use_gui ) guiGetEvent( guiSetStream,(char *)mpctx->stream );	// update gui dvdnav stream info
#endif
                        mp_dvdnav_highlight_handle_wrapper(1);	// update dvdnav menu //¡·¡·note¡·¡·:switch menu page. stream_change=1
                    } else
                        mp_dvdnav_highlight_handle_wrapper(0);
                }
#endif //#if 0

                if (mp_dvdnav_stream_has_changed(mpctx->stream))
                {
                    double ar = -1.0;
                    if (stream_control (mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
                    {
                        mpctx->sh_video->stream_aspect = ar;
                    }

                    //if(dvdnav_color_spu==1 && (dvdnavstate.new_aid_map >= 0))
                    if(dvdnav_color_spu==1)
                    {
                        mp_dvdnav_audio_handle_wrapper();	// call audio process
//                        if(dvdnavstate.aid[dvdnavstate.new_aid_map] != mpctx->demuxer->audio->id)
//                            mpctx->demuxer->audio->id = dvdnavstate.aid[dvdnavstate.new_aid_map];

                        mp_dvdnav_spu_handle_wrapper();    // call subtitle process
                        mp_dvdnav_highlight_handle_wrapper(1);
                    }
                }
                else if(dvdnav_color_spu==1)
                    mp_dvdnav_highlight_handle_wrapper(0);
            }
#endif

//============================ Handle PAUSE ===============================

            current_module="pause";

//  if (mpctx->osd_function == OSD_PAUSE) {
//Robert 20101130 PAUSE command lock/unlock flag
            if (((set_pause_lock == 0) && mpctx->osd_function == OSD_PAUSE) || (set_pause_lock == 2)) {
                mpctx->was_paused = 1;
                set_pause_lock = 0;
// WT, 100714, DRM pause event
                pause_loop_wrapper();
//+SkyMedi_Vincent03232010
//	if(ipcdomain && !thumbnail_mode) //using domain socket option
                if(ipcdomain && !(domainsocket_cmd && domainsocket_cmd->id == MP_CMD_FRAME_STEP)) //using domain socket option
                {
                    char callback_str[42];
                    is_pause = 0;

                    if (!mpctx->eof)
                    {
                        //Polun 2011-09-01 for mantis 5994   
                        if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE){
	                    //Barry 2010-11-11
	                    if ( (not_supported_profile_level == 2 || (mpctx->sh_video && !mpctx->sh_audio)) && (audio_id != -2) )
	                    {
	                        if (not_supported_profile_level == 3)
	                        	strcpy(callback_str,"status: playing 3\0");
	                        else
	                        {
	                        	if (mpctx->sh_audio && (mpctx->sh_audio->format == 0x2001))
						strcpy(callback_str,"status: playing 2\0");
					else
						strcpy(callback_str,"status: playing 4\0");
					sprintf(callback_names, "AUDIO: No Audio can switch\0");
					ipc_callback(callback_names);
	                        }
	                    }
	                    else if (not_supported_profile_level == 1)
	                    {
	                        strcpy(callback_str,"status: playing 1\0");
	                        ipc_callback(callback_str);
	                        free_demuxer(mpctx->demuxer);
	                        mpctx->demuxer = NULL;
//	                        goto goto_next_file;
                                return 3;

	                    }
	                    else
	                        strcpy(callback_str,"status: playing 0\0");
	                    ipc_callback(callback_str);
	                    
	                    ipc_callback_audio_sub();
                          }
                    }
                }
//20110620 Robert don't lock display here, prevent lock dvd menu(frame_step -> dvdnav menu)
                sky_set_vsync_status(1);
                dvdloop_lock_display = 0;
                dvdloop_lock_display_timeout_cnt = 0;

//SkyMedi_Vincent03232010+
            }

// handle -sstep
/*
            if(step_sec>0) {
                mpctx->osd_function=OSD_FFW;
                rel_seek_secs+=step_sec;
            }
*/
            edl_update_wrapper(mpctx);


//================= Keyboard events, SEEKing ====================
#ifdef ENABLE_KEY_EVENT_FUNCTION_MODE
            if (parse_and_run_command() == 2)
            {
//                goto goto_enable_cache;
                return 2;
            }
#endif // end of ENABLE_KEY_EVENT_FUNCTION_MODE

//Robert 20101130 PAUSE command lock/unlock flag
            if (mpctx->osd_function == OSD_PAUSE)
            {
                if (mpctx->sh_video && mpctx->sh_audio)
                {
                    unsigned char ao_buf_256[256];
                    memset(ao_buf_256, 0, 256);
                    set_pause_lock = 1;
                    if (mpctx->audio_out && mpctx->audio_out->play)
                    {
                        mpctx->audio_out->play(ao_buf_256, 256, 0);
                    }
                }
            }

            if(FR_to_end == 1)
            {
                speed_mult = 0;
                FR_to_end = 0;
                last_rewind_pts = 0.0;

                audio_id = rel_audio_id;
                rel_audio_id = -5;
                rel_seek_secs -= 0.1;

                if(mpctx->global_sub_size > 0)
                {
                    rel_global_sub_pos = -5;

                    if(vo_spudec != NULL)
                    {
                        last_spu_pts = 0;
                    }
                }

                ipc_callback_fast();
				
			if(mpctx->audio_out)
                		mpctx->audio_out->reset();
            }

            pthread_mutex_lock(&mut); //for waiting domain socket to switch audio language
            pthread_mutex_unlock(&mut);

            //Fuchun 2010.04.30
            if (speed_mult != 0 && mpeg_fast== 1)
            {
                if (mpctx->demuxer->type == DEMUXER_TYPE_MPEG_PS) {
                    if (speed_mult < 0)
                        rel_seek_secs += -2.5*rewind_mult;
                    else if (speed_mult >= 2)
                        rel_seek_secs += 1.0;
                }

                if (mpctx->stream->type == STREAMTYPE_DVDNAV) {
                    if (speed_mult < 0)
                        rel_seek_secs = 1; //special case, don't let newpos become to negative
                    else if (speed_mult >= 2)
                        rel_seek_secs += 1.0;
                }

                mpeg_fast = 0;
            }

            if(audio_speed != 0)
            {
                float time_interval = 1.0;
                if(audio_speed >= 3)
                {
                    if(audio_speed == 3)		//4x
                        time_interval = 4.0;
                    else if(audio_speed == 4)		//8x
                        time_interval = 8.0;
                    else if(audio_speed == 5)		//16x
                        time_interval = 16.0;
                    else if(audio_speed == 6)		//32x
                        time_interval = 32.0;
                    rel_seek_secs += (float)(time_interval);
                }
                else if(audio_speed < 0)
                {
                    if(audio_speed == -1)	//-1.5x
                        time_interval = -2.0;
                    else if(audio_speed == -2)	//-2x
                        time_interval = -3.0;
                    else if(audio_speed == -3)	//-4x
                        time_interval = -4.0;
                    else if(audio_speed == -4)	//-8x
                        time_interval = -8.0;
                    else if(audio_speed == -5)	//-16x
                        time_interval = -16.0;
                    else if(audio_speed == -6)	//-32x
                        time_interval = -32.0;
                    rel_seek_secs += (float)(time_interval);
                }
            }

            mpctx->was_paused = 0;

            /* Looping. */
            if(mpctx->eof==1 && mpctx->loop_times>=0) {
                mp_msg(MSGT_CPLAYER,MSGL_V,"loop_times = %d, eof = %d\n", mpctx->loop_times,mpctx->eof);

                if(mpctx->loop_times>1) mpctx->loop_times--;
                else if(mpctx->loop_times==1) mpctx->loop_times=-1;
                play_n_frames= -1;//play_n_frames_mf;
                mpctx->eof=0;
                abs_seek_pos=SEEK_ABSOLUTE;
                rel_seek_secs=seek_to_sec;
                loop_seek = 1;
            }

            if(rel_seek_secs || abs_seek_pos) {
                if (seek_wrapper(mpctx, rel_seek_secs, abs_seek_pos) >= 0) {
                    // Set OSD:
                    if(!loop_seek) {
                        if( !edl_decision )
                            set_osd_bar(0,"Position",0,100,demuxer_get_percent_pos(mpctx->demuxer));
                    }
                    if(is_dvdnav)
                    {
                    	if(dvdnav_seek_result == 1)
                    	{
                    		//Skyviia_Vincent02222011:usavich_season1.iso seek to end can't return to menu
                    		FFFR_to_normalspeed(NULL);
                    		dvdnav_seek_result = 0;
                            //Polun 2011-07-20 ++s mantis 5502  
                            if (mpctx->stream->type == STREAMTYPE_DVDNAV && mp_dvdnav_is_stop(mpctx->stream))
                            {
    	                           mpctx->eof = 1;
	                           return 0;
                            }
                            //Polun 2011-07-20 ++e mantis 5502  
                    	}
                    }
                }

                if (speed_mult == 0)
                {
//printf("\n ==== after seek, lock display\n");
//seek_sync_flag = 0;
//wait_video_or_audio_sync = AVSYNC_NORMAL;
//wait_video_or_audio_sync = AVSYNC_VIDEO_CATCH_AUDIO;
//printf("\n=== seek_sync_flag = %d, wait=%d\n", seek_sync_flag, wait_video_or_audio_sync);
                    sky_set_vsync_status(-1);
                    dvdloop_lock_display = 1;
                    dvdloop_lock_display_timeout_cnt = 31;
                }

                rel_seek_secs=0;
                abs_seek_pos=0;
                loop_seek=0;
                edl_decision = 0;
            }

            //Polun 2011-07-20 ++s mantis 5502  
            //printf("duration_sec_cur = %d my_current_pts = %d speed_mult = %d\n",duration_sec_cur,my_current_pts,speed_mult);
            if (mpctx->stream->type == STREAMTYPE_DVDNAV && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && mp_dvdnav_is_stop(mpctx->stream) && (duration_sec_cur <= (my_current_pts +1 )))
            {
    	             mpctx->eof = 1;
	             return 0;
             }
             //Polun 2011-07-20 ++e mantis 5502  

            if(speed_mult != 0) mpctx->num_buffered_frames = 0;	//Fuchun 2010.04.30
            if(check_framedrop_flag == 1)
            {
                mpctx->delay = mpctx->audio_out->get_delay();
                check_framedrop_flag = 0;
            }

            if(ipcdomain)
            {
		if(is_first_loop) //Mantis:6001 time bar wrong
		{
			duration_sec_cur = demuxer_get_time_length(mpctx->demuxer);
			ipc_callback_duration();
		}

                if(duration_sec_old != duration_sec_cur)
                {
                    duration_sec_old = duration_sec_cur;

                    if(skydroid && dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && duration_sec_cur > 0)
                    {
                    	char callback_str[1024];
                    	extern char subtitle_names[];
			extern int sub_change_success;

                    	mp_property_print("sub", mpctx);
                    	if(sub_change_success == 1)
				sprintf(callback_str, "SUBTITLE: full %s\0", subtitle_names);
			else if(sub_change_success == 2)
				sprintf(callback_str, "SUBTITLE: partial %s\0", subtitle_names);
			else
				sprintf(callback_str, "SUBTITLE: %s\0", subtitle_names);
			sub_change_success = 0;
                    	ipc_callback(callback_str);
                    }

                    //Polun 2011-07-21 ++s for ALVIN AND THE CHIPMUNKS 4X3 FF to end
                    if(is_dvdnav && duration_sec_old ==0 && speed_mult > 0)
                    {
                        FFFR_to_normalspeed(NULL);
                    }
                    //Polun 2011-07-21 ++e for ALVIN AND THE CHIPMUNKS 4X3 FF to end
                }
            }
	    if (is_first_loop == 1)
		is_first_loop = 0;
        } // while(!mpctx->eof)

        entry_while = 0;
        printf("mpctx->eof[%d]\n",mpctx->eof);
        return 0;
}


#endif

