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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "mp_msg.h"
#include "osdep/timer.h"
#include "input/input.h"
#include "stream.h"
#include "libmpdemux/demuxer.h"
#include <dvdnav/dvdnav.h>
#include "stream_dvdnav.h"
#include "libvo/video_out.h"
#include "libavutil/common.h"
#include "spudec.h"
#include "m_option.h"
#include "m_struct.h"
#include "help_mp.h"
#include "stream_dvd_common.h"

int seek_chapter = 0;
extern int dvdnav_clear_subcc_buf;
extern int menu_rsm_cmd;

/* state flags */
typedef enum {
  NAV_FLAG_EOF                  = 1 << 0,  /* end of stream has been reached */
  NAV_FLAG_WAIT                 = 1 << 1,  /* wait event */
  NAV_FLAG_WAIT_SKIP            = 1 << 2,  /* wait skip disable */
  NAV_FLAG_CELL_CHANGE          = 1 << 3,  /* cell change event */
  NAV_FLAG_WAIT_READ_AUTO       = 1 << 4,  /* wait read auto mode */
  NAV_FLAG_WAIT_READ            = 1 << 5,  /* suspend read from stream */
  NAV_FLAG_VTS_DOMAIN           = 1 << 6,  /* vts domain */
  NAV_FLAG_SPU_SET              = 1 << 7,  /* spu_clut is valid */
  NAV_FLAG_STREAM_CHANGE        = 1 << 8,  /* title, chapter, audio or SPU */
  NAV_FLAG_AUDIO_CHANGE         = 1 << 9,  /* audio stream change event */
  NAV_FLAG_SPU_CHANGE           = 1 << 10, /* spu stream change event */
  NAV_FLAG_CELL_CHANGE_NOW      = 1 << 11, /* receive input command need change cell now */
  NAV_FLAG_VTS_CHANGE           = 1 << 12, /* vts change event */
  NAV_FLAG_STOP                 = 1 << 13, /* stop */
  NAV_FLAG_STILL_FRAME          = 1 << 14, /* still frame */
} dvdnav_state_t;

//+Skyviia_Vincent07202010
typedef struct {
  int title;
  int mp_aid;
  int nav_aid;
  int mp_sid;
  int nav_sid;
} title_prop_t;
//Skyviia_Vincent07202010+

typedef struct {
  dvdnav_t *       dvdnav;              /* handle to libdvdnav stuff */
  char *           filename;            /* path */
  unsigned int     duration;            /* in milliseconds */
  int              mousex, mousey;
  int              title;
  unsigned int     spu_clut[16];
  dvdnav_highlight_event_t hlev;
  int              still_length;        /* still frame duration */
  unsigned int     state;

//+Skyviia_Vincent07202010
  int		        part;		        /* current chapter */
  int		        titles_nr;		    /* all titles numbers */
  int		        chapters_nr;		/* all chapert numbers in current title */
  int		        nr_of_channels;	    /* audio channels numbers */
  stream_language_t audio_streams[32];	/* audio channels propreties */
  int		        nr_of_subtitles;	/* spu numbers */
  stream_language_t subtitles[32];	    /* spu properties */
  int               title_prop_num;
  title_prop_t*     title_prop;
//+Skyviia_Vincent07202010+
} dvdnav_priv_t;

static struct stream_priv_s {
  int track;
  char* device;
} stream_priv_dflts = {
  0,
  NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
  {"filename", 	ST_OFF(device), CONF_TYPE_STRING, 0, 0, 0, NULL },
  {"hostname", 	ST_OFF(track),  CONF_TYPE_INT, M_OPT_RANGE, 1, 99, NULL},
  { NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
  "dvd",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};

extern void ipc_callback(char *datastr);
extern int dvdnav_sub_menu_select;
extern int dvdnav_audio_menu_select;
int menu_to_movie = 0;
extern int dvdnav_rel_seek;
int dvdnav_error_call_menu = 0;

static int seek(stream_t *s, off_t newpos);
static void show_audio_subs_languages(dvdnav_t *nav, int show);

extern DvdnavState   dvdnavstate;
int dvdnav_stream_err=0;
int dvdnav_globle_num_subtitle=0;
extern int dvdnav_auto_action;
int seek_to_end=0;	//Skyviia_Vincent04252011 seek 20 mins
unsigned int last_sector = 0;
int last_vobustart = 0;	//Fuchun 2011.05.13
int raise_cnt = 0;		//Fuchun 2011.05.13
extern int dvdnav_error_file;
extern int subcc_enabled;
extern int Dvdnav_cmd_exit = 0; //Polun 2012-02-03 fixed mantis6790 playback to eof signal 11 issue  

static dvdnav_priv_t * new_dvdnav_stream(char * filename) {
  const char * title_str;
  dvdnav_priv_t *priv;

  if (!filename)
    return NULL;

  if (!(priv=calloc(1,sizeof(dvdnav_priv_t))))
    return NULL;

  if (!(priv->filename=strdup(filename))) {
    free(priv);
    return NULL;
  }

  dvd_set_speed(priv->filename, dvd_speed);

  if(dvdnav_open(&(priv->dvdnav),priv->filename)!=DVDNAV_STATUS_OK)
  {
    free(priv->filename);
    free(priv);
    return NULL;
  }

  if (!priv->dvdnav) {
    free(priv);
    return NULL;
  }

  if(1)	//from vlc: if not used dvdnav from cvs will fail
  {
    int len, event;
    char buf[2048];

    dvdnav_get_next_block(priv->dvdnav,buf,&event,&len);
    dvdnav_sector_search(priv->dvdnav, 0, SEEK_SET);
  }

  /* turn off dvdnav caching */
  dvdnav_set_readahead_flag(priv->dvdnav, 0);
  if(dvdnav_set_PGC_positioning_flag(priv->dvdnav, 1) != DVDNAV_STATUS_OK)
    mp_msg(MSGT_OPEN,MSGL_ERR,"stream_dvdnav, failed to set PGC positioning\n");
  /* report the title?! */
  if (dvdnav_get_title_string(priv->dvdnav,&title_str)==DVDNAV_STATUS_OK) {
    mp_msg(MSGT_IDENTIFY, MSGL_INFO,"Title: '%s'\n",title_str);
  }

  //dvdnav_event_clear(priv);
  last_sector = 0;
  last_vobustart = 0;
  raise_cnt = 0;

  return priv;
}

static void dvdnav_get_highlight (dvdnav_priv_t *priv, int display_mode) {
  pci_t *pnavpci = NULL;
  dvdnav_highlight_event_t *hlev = &(priv->hlev);
  int btnum;
//  static int button_type=2; // 0:hide 1:show

  if (!priv || !priv->dvdnav)
    return;

  pnavpci = dvdnav_get_current_nav_pci (priv->dvdnav);
  if (!pnavpci)
    return;

  dvdnav_get_current_highlight (priv->dvdnav, &(hlev->buttonN));
  hlev->display = display_mode; /* show */
//printf("pnavpci->hli.hl_gi.btn_ns[%d],hlev->buttonN[%d]\n",pnavpci->hli.hl_gi.btn_ns,hlev->buttonN);
  if ( (pnavpci->hli.hl_gi.btn_ns > 0) && (hlev->buttonN > pnavpci->hli.hl_gi.btn_ns) )
  {
      // if button out of range, set it to default
      if (dvdnav_button_select(priv->dvdnav, pnavpci, 1) == DVDNAV_STATUS_OK)
      {
	  hlev->buttonN = 1;
	  puts("reset button index to 1\n");
      }
  }

//printf("--------hlev->buttonN[%x],hlev->display[%d]\n",hlev->buttonN,hlev->display);
  if (hlev->buttonN > 0 && pnavpci->hli.hl_gi.btn_ns > 0 && hlev->display) {
#if 0
///////////////////////////////////////////////////////////////////
printf("pnavpci->hli.hl_gi.btn_ns[%d],hlev->buttonN[%d]\n",pnavpci->hli.hl_gi.btn_ns,hlev->buttonN);
  printf("libdvdnav: hl_gi:\n");
  printf("libdvdnav: hli_ss        0x%01x\n", pnavpci->hli.hl_gi.hli_ss & 0x03);
  printf("libdvdnav: hli_s_ptm     0x%08x\n", pnavpci->hli.hl_gi.hli_s_ptm);
  printf("libdvdnav: hli_e_ptm     0x%08x\n", pnavpci->hli.hl_gi.hli_e_ptm);
  printf("libdvdnav: btn_se_e_ptm  0x%08x\n", pnavpci->hli.hl_gi.btn_se_e_ptm);

  printf("libdvdnav: btngr_ns      %d\n",  pnavpci->hli.hl_gi.btngr_ns);
  printf("libdvdnav: btngr%d_dsp_ty    0x%02x\n", 1, pnavpci->hli.hl_gi.btngr1_dsp_ty);
  printf("libdvdnav: btngr%d_dsp_ty    0x%02x\n", 2, pnavpci->hli.hl_gi.btngr2_dsp_ty);
  printf("libdvdnav: btngr%d_dsp_ty    0x%02x\n", 3, pnavpci->hli.hl_gi.btngr3_dsp_ty);

  printf("libdvdnav: btn_ofn       %d\n", pnavpci->hli.hl_gi.btn_ofn);
  printf("libdvdnav: btn_ns        %d\n", pnavpci->hli.hl_gi.btn_ns);
  printf("libdvdnav: nsl_btn_ns    %d\n", pnavpci->hli.hl_gi.nsl_btn_ns);
  printf("libdvdnav: fosl_btnn     %d\n", pnavpci->hli.hl_gi.fosl_btnn);
  printf("libdvdnav: foac_btnn     %d\n", pnavpci->hli.hl_gi.foac_btnn);
///////////////////////////////////////////////////////////////////
#endif
	extern int dvdnav_menupage_chg;
	if(dvdnav_menupage_chg == 1)
	{
//printf("hlev->buttonN[%d],fosl_btnn[%x]\n",hlev->buttonN,pnavpci->hli.hl_gi.fosl_btnn);
//printf("btn_ns[%d]\n",pnavpci->hli.hl_gi.btn_ns);
//printf("auto_action_mode[%d]\n",pnavpci->hli.btnit[pnavpci->hli.hl_gi.fosl_btnn].auto_action_mode);
//printf("auto_action_mode1[%d]\n",pnavpci->hli.btnit[1].auto_action_mode);
      		if(pnavpci->hli.hl_gi.fosl_btnn == 0)
      			hlev->buttonN = 1;
      		else
      		{
      			if (dvdnav_button_select(priv->dvdnav, pnavpci, pnavpci->hli.hl_gi.fosl_btnn) == DVDNAV_STATUS_OK)
      				hlev->buttonN = pnavpci->hli.hl_gi.fosl_btnn;
      		}
		dvdnav_menupage_chg = 0;
	}
	else
	{
    		for (btnum = 0; btnum < pnavpci->hli.hl_gi.btn_ns; btnum++) {
      			btni_t *btni = &(pnavpci->hli.btnit[btnum]);

      			if (hlev->buttonN == btnum + 1) {
        			hlev->sx = FFMIN (btni->x_start, btni->x_end);
        			hlev->ex = FFMAX (btni->x_start, btni->x_end);
        			hlev->sy = FFMIN (btni->y_start, btni->y_end);
        			hlev->ey = FFMAX (btni->y_start, btni->y_end);

//printf("--show\n");
//	  if(button_type == 0 || button_type == 2)
//	  {
//	  	button_type = 1;
//	  	dvdnavstate.dvdnav_button_on = 1;
//	  	dvdnavstate.dvdnav_state_change = 1;
//	  }

        			hlev->palette = (btni->btn_coln == 0) ? 0 :
          			pnavpci->hli.btn_colit.btn_coli[btni->btn_coln - 1][0];
        			break;
      			}
    		}
	}
  } else { /* hide button or no button */
//printf("--hide\n");
//	if(button_type == 1 || button_type == 2)
//	{
//	  button_type = 0;
//	  dvdnavstate.dvdnav_button_on = 0;
//	  dvdnavstate.dvdnav_state_change = 1;
//	}

    hlev->sx = hlev->ex = 0;
    hlev->sy = hlev->ey = 0;
    hlev->palette = hlev->buttonN = 0;
  }
}

static inline int dvdnav_get_duration (int length) {
  return (length == 255) ? 0 : length * 1000;
}

static int dvdnav_stream_read(dvdnav_priv_t * priv, unsigned char *buf, int *len) {
  int event = DVDNAV_NOP;
  dvdnav_status_t status;

  *len=-1;
  status = dvdnav_get_next_block(priv->dvdnav,buf,&event,len);
  if (status!=DVDNAV_STATUS_OK) {
    mp_msg(MSGT_OPEN,MSGL_V, "Error getting next block from DVD %d (%s)\n",event, dvdnav_err_to_string(priv->dvdnav) );
    *len=-1;
    if(status == DVDNAV_ERROR_FILE_END)
    	return DVDNAV_ERROR_FILE_END;
  }
  else if (event!=DVDNAV_BLOCK_OK) {
    // need to handle certain events internally (like skipping stills)
mplayer_dvdnav_debug("## DVDNAV In [%s][%d], event[%d],len[%d]##\n", __func__, __LINE__,event,*len);

    switch (event) {
      case DVDNAV_NAV_PACKET:
        return event;
      case DVDNAV_STILL_FRAME: {
        dvdnav_still_event_t *still_event = (dvdnav_still_event_t *) buf;
        priv->still_length = still_event->length;
        /* set still frame duration */
        priv->duration = dvdnav_get_duration (priv->still_length);
        if (priv->still_length <= 1) {
          pci_t *pnavpci = dvdnav_get_current_nav_pci (priv->dvdnav);
          priv->duration = mp_dvdtimetomsec (&pnavpci->pci_gi.e_eltm);
        }
        break;
      }
      case DVDNAV_HIGHLIGHT: {
        dvdnav_get_highlight (priv, 1);
        break;
      }
      case DVDNAV_CELL_CHANGE: {
        dvdnav_cell_change_event_t *ev =  (dvdnav_cell_change_event_t*)buf;
        uint32_t nextstill;
        priv->state &= ~NAV_FLAG_WAIT_SKIP;
        priv->state |= NAV_FLAG_STREAM_CHANGE;
        if(ev->pgc_length)
          priv->duration = ev->pgc_length/90;

extern int dvdnav_is_delay_cell;
if(dvdnav_is_delay_cell)
	dvdnav_is_delay_cell++;
#if 0
///////////////////////////////////////////////
        printf("-cellN=%d", ev->cellN );
        printf(" -pgN=%d", ev->pgN );
        printf(" -cell_length=%"PRId64, ev->cell_length );
        printf(" -pg_length=%"PRId64, ev->pg_length );
        printf(" -pgc_length=%"PRId64, ev->pgc_length );
        printf(" -cell_start=%"PRId64, ev->cell_start );
        printf(" -pg_start=%"PRId64, ev->pg_start );
        printf(" -duration=%d\n",ev->pgc_length / 90);
        printf("\n");
///////////////////////////////////////////////
#endif

        if (dvdnav_is_domain_vts(priv->dvdnav)) {
	   if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)		//Fuchun 2011.01.14
			{
				menu_to_movie = 1;
				dvdnavstate.dvdnav_state_change = 1;
			}
	   
          dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MOVIE;
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "DVDNAV_TITLE_IS_MOVIE\n");
          priv->state &= ~NAV_FLAG_VTS_DOMAIN;
        } else if(dvdnav_is_domain_vtsm(priv->dvdnav)){
			if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)		//Fuchun 2011.01.14
				dvdnavstate.dvdnav_state_change = 1;

          dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MENU;
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "DVDNAV_TITLE_IS_MENU\n");
          priv->state |= NAV_FLAG_VTS_DOMAIN;
        }
        else if(dvdnav_is_domain_fp(priv->dvdnav))
        	printf("**First Play domain**\n");
        else if(dvdnav_is_domain_vmgm(priv->dvdnav))
		{
		    if ((dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_IDLE)
			    || (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE))
		    {
			dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MENU;
			dvdnavstate.dvdnav_state_change = 1;
			printf("**Video management Menu domain**\n");
		    }
		}
        else
        	printf("**can't recognize dvdnav domain**\n");

        nextstill = dvdnav_get_next_still_flag (priv->dvdnav);
        if (nextstill) {
          priv->duration = dvdnav_get_duration (nextstill);
          priv->still_length = nextstill;
          if (priv->still_length <= 1) {
            pci_t *pnavpci = dvdnav_get_current_nav_pci (priv->dvdnav);
            priv->duration = mp_dvdtimetomsec (&pnavpci->pci_gi.e_eltm);
          }
        }

        break;
      }
      case DVDNAV_SPU_CLUT_CHANGE: {
        memcpy(priv->spu_clut, buf, 16*sizeof(unsigned int));
        priv->state |= NAV_FLAG_SPU_SET;
        break;
      }
      case DVDNAV_WAIT: {
        if ((priv->state & NAV_FLAG_WAIT_SKIP) &&
            !(priv->state & NAV_FLAG_WAIT))
          dvdnav_wait_skip (priv->dvdnav);
        else
          priv->state |= NAV_FLAG_WAIT;
        break;
      }
      case DVDNAV_VTS_CHANGE: {
        char callback_str[42];
        priv->state &= ~NAV_FLAG_WAIT_SKIP;
        priv->state |= NAV_FLAG_STREAM_CHANGE;
        if (dvdnav_is_domain_vts(priv->dvdnav)) {
	   		if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)		//Fuchun 2011.01.14
	   		{
 	   				menu_to_movie = 1;
	   		}

	   		dvdnavstate.dvdnav_state_change = 1;	   

          dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MOVIE;
        } else if(dvdnav_is_domain_vtsm(priv->dvdnav)){
			if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE)		//Fuchun 2011.01.14
				dvdnavstate.dvdnav_state_change = 1;

          dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MENU;
	      FFFR_to_normalspeed(NULL);
        }
        else if(dvdnav_is_domain_fp(priv->dvdnav))
        	printf("**First Play domain**\n");
        else if(dvdnav_is_domain_vmgm(priv->dvdnav))
		{
		    if ((dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_IDLE)
			    || (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE))
		    {
		    	//Barry 2011-07-13
		    	if (speed_mult != 0)
		    		FFFR_to_normalspeed(NULL);

			dvdnavstate.dvdnav_title_state = DVDNAV_TITLE_STATE_MENU;
			dvdnavstate.dvdnav_state_change = 1;
			printf("**Video management Menu domain**\n");
		    }
		}
        else
        	printf("**can't recognize dvdnav domain**\n");
        	

        break;
      }
      case DVDNAV_SPU_STREAM_CHANGE: {
        priv->state |= NAV_FLAG_STREAM_CHANGE;
        break;
      }
    }

    *len=0;
  }
  return event;
}

static void update_title_len(stream_t *stream) {
  dvdnav_priv_t *priv = stream->priv;
  dvdnav_status_t status;
  uint32_t pos = 0, len = 0;

  status = dvdnav_get_position(priv->dvdnav, &pos, &len);
  if(status == DVDNAV_STATUS_OK && len) {
    stream->end_pos = (off_t) len * 2048;
    stream->seek = seek;
  } else {
    stream->seek = NULL;
    stream->end_pos = 0;
  }
}


static int seek(stream_t *s, off_t newpos) {
  uint32_t sector = 0;
  dvdnav_priv_t *priv = s->priv;
  extern int FFFR_to_normal;
  int result;
  //Polun 2011-07-20 ++s mantis 5502  
  extern int duration_sec_cur;
  extern int my_current_pts;
  //Polun 2011-07-20 ++e mantis 5502  

  if(s->end_pos && newpos > s->end_pos)
     newpos = s->end_pos;

  if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
  {
  	if(speed_mult != 0)
  	{
		FFFR_to_normalspeed(NULL);
		return 1;
	}
    sector = newpos / 2048ULL;
    if(dvdnav_sector_search(priv->dvdnav, (uint64_t) sector, SEEK_SET) != DVDNAV_STATUS_OK)
    {
    	if(FFFR_to_normal)
	{
		dvdnav_rel_seek = 1;
		FFFR_to_normal = 0;
		last_sector = 0;
		last_vobustart = 0;
		raise_cnt = 0;
	}
        goto fail;
    }
  }
  else
  {
	extern int seek_sync_flag;
//printf("seek_sync_flag[%d]\n",seek_sync_flag);

	if(FFFR_to_normal)	//mantis:3714. FF-2x to normal speed will seek to movie begin
		dvdnav_rel_seek = 0;

	unsigned int new_sector = (newpos / 2048ULL);
    if(!dvdnav_rel_seek)
    {
	//if(last_sector == 0 || ((new_sector - last_sector) < 512))
	if(last_sector == 0 || ((int)(new_sector - last_sector) < 512))
		sector = 512;
	else
		sector = (newpos / 2048ULL) - last_sector;
	last_sector = newpos / 2048ULL;
    }
    else // relative seek 
    {
    	if(seek_sync_flag == 1)
    		sector = newpos / 2048ULL;
    }

    result = dvdnav_sector_search(priv->dvdnav, (uint64_t) sector, SEEK_CUR);
    if(result != DVDNAV_STATUS_OK)
    {
    	if(FFFR_to_normal)
    	{
    		dvdnav_rel_seek = 1;
    		FFFR_to_normal = 0;
		last_sector = 0;
		last_vobustart = 0;
		raise_cnt = 0;
    	}
    	
    	if(result == DVDNAV_STATUS_EOF)
    	{
			newpos = s->end_pos;

    		sector = newpos / 2048ULL ;
    		if(dvdnav_sector_search(priv->dvdnav, (uint64_t) sector, SEEK_END) != DVDNAV_STATUS_OK)
			{
    			printf("## DVDNAV [%s][%d] seek to END fail ##\n", __func__, __LINE__);
			}
			else
			{
				printf("## DVDNAV [%s][%d] seek to END ok ##\n", __func__, __LINE__);
				seek_to_end = 5;//workaround for after dvdnav_stream_read 5 times than call menu
				s->pos = newpos;
            	return 1;
			}
    	}
    	
        goto fail;
    }

    if(FFFR_to_normal)
    {
    	dvdnav_rel_seek = 1;
    	FFFR_to_normal = 0;
	last_sector = 0;
	last_vobustart = 0;
	raise_cnt = 0;
    }
  }

  s->pos = newpos;

  return 1;

fail:
  //Polun 2011-07-20 ++s mantis 5502  
  if((duration_sec_cur == my_current_pts) && (speed_mult != 0))
 {
    s->eof = 1 ; 
  }
  mp_msg(MSGT_STREAM,MSGL_INFO,"dvdnav_stream, seeking to %"PRIu64" failed: %s\n", newpos, dvdnav_err_to_string(priv->dvdnav));
  //Polun 2011-07-20 ++e mantis 5502  
  mp_msg(MSGT_STREAM,MSGL_INFO,"dvdnav_stream, seeking to %"PRIu64" failed: %s\n", newpos, dvdnav_err_to_string(priv->dvdnav));

  return 1;
}

static void stream_dvdnav_close(stream_t *s) {
  dvdnav_priv_t *priv = s->priv;
  dvdnav_close(priv->dvdnav);
  priv->dvdnav = NULL;
  dvd_set_speed(priv->filename, -1);
  free(priv);
}


static int fill_buffer(stream_t *s, char *but, int len)
{
    int event;

    dvdnav_priv_t* priv=s->priv;
    if (priv->state & NAV_FLAG_WAIT_READ) /* read is suspended */
      return -1;
    len=0;
    if(!s->end_pos)
      update_title_len(s);
    while(!len) /* grab all event until DVDNAV_BLOCK_OK (len=2048), DVDNAV_STOP or DVDNAV_STILL_FRAME */
    {
        //Polun 2012-02-03 ++s fixed mantis6790 playback to eof signal 11 issue  
        if(Dvdnav_cmd_exit ) 
        {
            s->eof = 1;
            priv->state |= NAV_FLAG_EOF;
            return 0;
        }
        //Polun 2012-02-03 ++e
        event=dvdnav_stream_read(priv, s->buffer, &len);
        
    	if( seek_to_end > 0)
    	{
		if(seek_to_end == 1)
		{
			dvdnav_t *nav = priv->dvdnav;
			dvdnav_status_t status=DVDNAV_STATUS_ERR;
			seek_to_end = 0;            
			if(dvdnav_menu_call(nav, DVD_MENU_Root) == DVDNAV_STATUS_OK)
			{
				return 1;
			}
			else
			{
				dvdnav_menu_call(priv->dvdnav, DVD_MENU_Title);
				return 1;
			}
		}
		seek_to_end--;
    	}
    	
    	if(event == DVDNAV_ERROR_FILE_END)
    	{
    		dvdnav_t *nav = priv->dvdnav;
    		dvdnav_error_file = 0;

		if(dvdnav_menu_call(nav, DVD_MENU_Root) == DVDNAV_STATUS_OK)
		{
			return 1;
		}
		else
		{
			dvdnav_menu_call(priv->dvdnav, DVD_MENU_Title);
			return 1;
		}
    	}

      if(event==-1 || len==-1)
      {
        mp_msg(MSGT_CPLAYER,MSGL_ERR, "DVDNAV stream read error!\n");
        dvdnav_stream_err++;
        if(dvdnav_stream_err>0)
        {
            //for error handle
            dvdnav_t *nav = priv->dvdnav;
            dvdnav_status_t status=DVDNAV_STATUS_ERR;
            
            
            if(dvdnav_menu_call(nav, DVD_MENU_Root) == DVDNAV_STATUS_OK)
	    {
		dvdnav_error_call_menu = 1;
		menu_to_movie = 0;//WINNIE The POOH can't playback
            	return 1;
	    }

            dvdnav_stream_err = 0;
        }
            
        return 0;
      }
      if (event != DVDNAV_BLOCK_OK)
        dvdnav_get_highlight (priv, 1);
      switch (event) {
        case DVDNAV_STOP: {
          priv->state |= NAV_FLAG_STOP;
          return len;
        }
        case DVDNAV_BLOCK_OK:
        case DVDNAV_NAV_PACKET:
        case DVDNAV_STILL_FRAME:
          if(event == DVDNAV_STILL_FRAME)
          	priv->state |= NAV_FLAG_STILL_FRAME;
          return len;
        case DVDNAV_WAIT: {
          if (priv->state & NAV_FLAG_WAIT)
            return len;
          break;
        }
        case DVDNAV_VTS_CHANGE: {
//printf("case DVDNAV_VTS_CHANGE\n");
          int tit = 0, part = 0;
          dvdnav_vts_change_event_t *vts_event = (dvdnav_vts_change_event_t *)s->buffer;
          mp_msg(MSGT_CPLAYER,MSGL_INFO, "DVDNAV, switched to title: %d\r\n", vts_event->new_vtsN);
          priv->state |= NAV_FLAG_CELL_CHANGE;
          priv->state |= NAV_FLAG_AUDIO_CHANGE;
          priv->state |= NAV_FLAG_SPU_CHANGE;
	  priv->state |= NAV_FLAG_VTS_CHANGE;
          priv->state &= ~NAV_FLAG_WAIT_SKIP;
          priv->state &= ~NAV_FLAG_WAIT;
          s->end_pos = 0;
          update_title_len(s);
          show_audio_subs_languages(priv->dvdnav, 0);
          if (priv->state & NAV_FLAG_WAIT_READ_AUTO)
            priv->state |= NAV_FLAG_WAIT_READ;
          if(dvdnav_current_title_info(priv->dvdnav, &tit, &part) == DVDNAV_STATUS_OK) {
            mp_msg(MSGT_CPLAYER,MSGL_V, "\r\nDVDNAV, NEW TITLE %d\r\n", tit);
            dvdnav_get_highlight (priv, 0);
            if(priv->title > 0 && tit != priv->title) {
              priv->state |= NAV_FLAG_EOF;
              return 0;
            }
          }
          break;
        }
        case DVDNAV_CELL_CHANGE: {
//printf("case DVDNAV_CELL_CHANGE\n");
	extern int dvdnav_pgcN_change;
//printf("______________dvdnav_pgcN_change[%d]\n",dvdnav_pgcN_change);
	if(dvdnav_pgcN_change)
	{
		dvdnav_pgcN_change = 0;
		show_audio_subs_languages(priv->dvdnav, 1);
	}

	  if(menu_to_movie == 0)
          	priv->state |= NAV_FLAG_CELL_CHANGE;

          priv->state |= NAV_FLAG_AUDIO_CHANGE;
          priv->state |= NAV_FLAG_SPU_CHANGE;
          priv->state &= ~NAV_FLAG_WAIT_SKIP;
          priv->state &= ~NAV_FLAG_WAIT;
          if (priv->state & NAV_FLAG_WAIT_READ_AUTO)
            priv->state |= NAV_FLAG_WAIT_READ;
          if(priv->title > 0 && dvd_last_chapter > 0) {
            int tit=0, part=0;
            if(dvdnav_current_title_info(priv->dvdnav, &tit, &part) == DVDNAV_STATUS_OK && part > dvd_last_chapter) {
              priv->state |= NAV_FLAG_EOF;
              return 0;
            }
          }
          dvdnav_get_highlight (priv, 1);
        }
        break;
        case DVDNAV_AUDIO_STREAM_CHANGE:
        {
		if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && speed_mult == 0)	//Fuchun 2010.12.27
		{
			extern int seek_sync_flag;
			seek_sync_flag = 6;//5;
		}

		
		if (dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE || dvdnav_audio_menu_select != 2)
		{
			static uint32_t tmp_audio=-1;
			uint32_t audio_curr, audio_num;

			if(dvdnav_get_audio_info(priv->dvdnav, &audio_curr, &audio_num) != DVDNAV_STATUS_OK)
			{
				break;
			}
//printf("audio_curr[%d], audio_num[%d],dvdnav_audio_menu_select[%d]\n",audio_curr, audio_num,dvdnav_audio_menu_select);
			if(tmp_audio != audio_curr)
			{
				tmp_audio = audio_curr;
				dvdnav_audio_menu_select = 1;
			}
        	}
                priv->state |= NAV_FLAG_AUDIO_CHANGE;
                
                break;

#if 0
	      dvdnav_audio_stream_change_event_t *astream_event = 
	        (dvdnav_audio_stream_change_event_t *)(s->buffer);
          dvdnavstate.new_aid_map = astream_event->physical;
          priv->state |= NAV_FLAG_AUDIO_CHANGE;
        break;
#endif
        }
        case DVDNAV_SPU_STREAM_CHANGE:
        {
		uint32_t spu_curr, spu_num;

		if(dvdnav_get_spu_info(priv->dvdnav, &spu_curr, &spu_num) != DVDNAV_STATUS_OK)
		{
			break;
		}
//printf("spu_curr[%d], spu_num[%d],dvdnav_sub_menu_select[%d]\n",spu_curr, spu_num,dvdnav_sub_menu_select);
		if(dvdnav_sub_menu_select !=2)
		{
			if(dvdnavstate.new_sid_map != spu_curr)
			{
				dvdnav_sub_menu_select = 1;
				dvdnavstate.new_sid_map = spu_curr;
				priv->state |= NAV_FLAG_SPU_CHANGE;
				break;
			}
		}

                priv->state &= ~NAV_FLAG_SPU_CHANGE;
                break;

#if 0
          dvdnav_spu_stream_change_event_t *sstream_event = 
            (dvdnav_spu_stream_change_event_t*)(s->buffer);
          if(!sstream_event->physical_pan_scan)
          {
            if(dvdnavstate.new_sid_map != sstream_event->physical_wide)
            {
                dvdnav_sub_menu_select = 1;
                dvdnavstate.new_sid_map = sstream_event->physical_wide;
            }
          }
    	  else if(sstream_event->physical_wide > 64)
            dvdnavstate.new_sid_map = -1;

          priv->state |= NAV_FLAG_SPU_CHANGE;
          break;
#endif
        }
      }
  }
  mp_msg(MSGT_STREAM,MSGL_DBG2,"DVDNAV fill_buffer len: %d\n",len);
  return len;
}

static int control(stream_t *stream, int cmd, void* arg) {
  dvdnav_priv_t* priv=stream->priv;
  int tit, part, num_prog;

  switch(cmd)
  {
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    {
	  //dvdnav_next_pg_search(priv->dvdnav); //xine using this for seek chapter, maybe we can reference this

      int chap = *((unsigned int *)arg)+1;

      if(chap < 1 || dvdnav_current_title_info(priv->dvdnav, &tit, &part) != DVDNAV_STATUS_OK)
        break;
      seek_chapter = 1;
      dvdnav_clear_subcc_buf = 1;

      dvdnav_get_number_of_program(priv->dvdnav,&num_prog);

      if(dvdnav_get_number_of_parts(priv->dvdnav, tit, &part) != DVDNAV_STATUS_OK)
      {
	part = 0;
      }
      
//printf("part[%d], num_prog[%d]\n",part,num_prog);
      if(num_prog == 1 && num_prog != part)
      	dvdnav_next_pg_search(priv->dvdnav);
      else
      {
      if(dvdnav_part_play(priv->dvdnav, tit, chap) != DVDNAV_STATUS_OK)
        break;
      }
      return 1;
    }
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    {
      if(dvdnav_current_title_info(priv->dvdnav, &tit, &part) != DVDNAV_STATUS_OK)
      {
        *((unsigned int *)arg) = 0;
        break;
      }
      if(dvdnav_get_number_of_parts(priv->dvdnav, tit, &part) != DVDNAV_STATUS_OK)
      {
	*((unsigned int *)arg) = 0;
        break;
      }
      if(!part)
        break;
      *((unsigned int *)arg) = part;
      return 1;
    }
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    {
      if(dvdnav_current_title_info(priv->dvdnav, &tit, &part) != DVDNAV_STATUS_OK)
      {      	
        *((unsigned int *)arg) = 0;
        break;
      }
      *((unsigned int *)arg) = part - 1;
      return 1;
    }
    case STREAM_CTRL_GET_CURRENT_TITLE:
    {
      if(dvdnav_current_title_info(priv->dvdnav, &tit, &part) != DVDNAV_STATUS_OK)
      {      	
        *((unsigned int *)arg) = 0;
        break;
      }
      *((unsigned int *)arg) = tit;
      return 1;
    }
    case STREAM_CTRL_GET_TIME_LENGTH:
    {
      if(priv->duration || priv->still_length)
      {
        *((double *)arg) = (double)priv->duration / 1000.0;
        return 1;
      }
      break;
    }
    case STREAM_CTRL_GET_ASPECT_RATIO:
    {
      uint8_t ar = dvdnav_get_video_aspect(priv->dvdnav);
      *((double *)arg) = !ar ? 4.0/3.0 : 16.0/9.0;
      return 1;
    }
    case STREAM_CTRL_GET_CURRENT_TIME:
    {
      double tm;
      tm = dvdnav_get_current_time(priv->dvdnav)/90000.0f;
      if(tm != -1)
      {
        *((double *)arg) = tm;
        return 1;
      }
      break;
    }
    case STREAM_CTRL_SEEK_TO_TIME:
    {
      uint64_t tm = (uint64_t) (*((double*)arg) * 90000);
      if(dvdnav_time_search(priv->dvdnav, tm) == DVDNAV_STATUS_OK)
        return 1;
      break;
    }
    case STREAM_CTRL_GET_NUM_ANGLES:
    {
        uint32_t curr, angles;
        if(dvdnav_get_angle_info(priv->dvdnav, &curr, &angles) != DVDNAV_STATUS_OK)
          break;
        *((int *)arg) = angles;
        return 1;
    }
    case STREAM_CTRL_GET_ANGLE:
    {
        uint32_t curr, angles;
        if(dvdnav_get_angle_info(priv->dvdnav, &curr, &angles) != DVDNAV_STATUS_OK)
          break;
        *((int *)arg) = curr;
        return 1;
    }
    
    case STREAM_CTRL_GET_SUB:
    {
		uint32_t spu_curr, spu_num;

		if(dvdnav_get_spu_info(priv->dvdnav, &spu_curr, &spu_num) != DVDNAV_STATUS_OK)
		{
			break;
		}
//printf("**************spu_curr[%x]\n",spu_curr);
        *((int *)arg) = spu_curr;
        return 1;
    }
    
    case STREAM_CTRL_SET_ANGLE:
    {
        uint32_t curr, angles;
        int new_angle = *((int *)arg);
        if(dvdnav_get_angle_info(priv->dvdnav, &curr, &angles) != DVDNAV_STATUS_OK)
          break;
        if(new_angle>angles || new_angle<1)
            break;
        if(dvdnav_angle_change(priv->dvdnav, new_angle) != DVDNAV_STATUS_OK)
            break;
        return 1;
    }
    
    case STREAM_CTRL_SET_SUB:
    {
        //maybe use it 
        int new_sub = *((int *)arg);

        if(dvdnav_sub_change(priv->dvdnav, new_sub) != DVDNAV_STATUS_OK)
            break;
        return 1;
    }
    
    case STREAM_CTRL_SET_AUDIO:
    {
        int new_auido = *((int *)arg);

        if(dvdnav_audio_change(priv->dvdnav, new_auido) != DVDNAV_STATUS_OK)
            break;
        return 1;
    }
  }

  return STREAM_UNSUPPORTED;
}

static void identify_chapters(dvdnav_t *nav, uint32_t title)
{
  uint64_t *parts=NULL, duration=0;
  uint32_t n, i, t;
  n = dvdnav_describe_title_chapters(nav, title, &parts, &duration);
  if(parts) {
    t = duration / 90;
    mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_DVD_TITLE_%d_LENGTH=%d.%03d\n", title, t / 1000, t % 1000);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "TITLE %u, CHAPTERS: ", title);
    for(i=0; i<n; i++) {
      t = parts[i] /  90000;
      mp_msg(MSGT_IDENTIFY, MSGL_INFO, "%02d:%02d:%02d,", t/3600, (t/60)%60, t%60);
    }
    free(parts);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "\n");
  }
}

static void identify(dvdnav_priv_t *priv, struct stream_priv_s *p)
{
  uint32_t titles=0, i;
  if(p->track <= 0) {
    dvdnav_get_number_of_titles(priv->dvdnav, &titles);
    for(i=0; i<titles; i++)
      identify_chapters(priv->dvdnav, i);
  }
  else
    identify_chapters(priv->dvdnav, p->track);
}

static void show_audio_subs_languages(dvdnav_t *nav, int show)
{
  uint8_t lg;
  uint16_t i, lang, format, id, channels, stream_aid;
  int base[7] = {128, 0, 0, 0, 160, 136, 0};
  for(i=0; i<8; i++)
  {
    char tmp[] = "unknown";
    lg = dvdnav_get_audio_logical_stream(nav, i);
    if(lg == 0xff)
    {
    	 dvdnavstate.aid[i] = -1;
        dvdnavstate.stream_aid[i] = -1;
    	 continue;
    }
    channels = dvdnav_audio_stream_channels(nav, lg);
    if((channels == 0xFFFF) || (channels > 6))
      channels = 2; //unknown
    else
      channels--;
    lang = dvdnav_audio_stream_to_lang(nav, lg);
    if(lang != 0xFFFF)
    {
      tmp[0] = lang >> 8;
      tmp[1] = lang & 0xFF;
      tmp[2] = 0;
    }
    format = dvdnav_audio_stream_format(nav, lg);
    if(format == 0xFFFF || format > 6)
      format = 1; //unknown
    id = i + base[format];
    stream_aid = lg + base[format];

    dvdnavstate.aid[i] = id; //Skyviia_Vincent12142010
    dvdnavstate.stream_aid[i] = stream_aid;

    if(show)
    mp_msg(MSGT_OPEN,MSGL_STATUS,MSGTR_DVDaudioStreamInfo, i,
           dvd_audio_stream_types[format], dvd_audio_stream_channels[channels], tmp, id);
    if (lang != 0xFFFF && lang && tmp[0])
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_LANG=%s\n", id, tmp);
  }

  dvdnav_globle_num_subtitle = 0;
  for(i=0; i<32; i++)
  {
    char tmp[] = "unknown";
    lg = dvdnav_get_spu_logical_stream(nav, i);
    if(lg == 0xff)
    {
    	 dvdnavstate.sid[i] = -1;
    	 continue;
    }
    
    dvdnavstate.sid[i] = lg;//Skyviia_Vincent12142010

    dvdnav_globle_num_subtitle++;
    
    lang = dvdnav_spu_stream_to_lang(nav, i);
    if(lang != 0xFFFF)
    {
      tmp[0] = lang >> 8;
      tmp[1] = lang & 0xFF;
      tmp[2] = 0;
    }

    if(show)
    mp_msg(MSGT_OPEN,MSGL_STATUS,MSGTR_DVDsubtitleLanguage, lg, tmp);
    if (lang != 0xFFFF && lang && tmp[0])
        mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_LANG=%s\n", lg, tmp);
  }
  
  if(dvdnav_globle_num_subtitle)
  	subcc_enabled = 0;
}

static int open_s(stream_t *stream,int mode, void* opts, int* file_format) {
  struct stream_priv_s* p = (struct stream_priv_s*)opts;
  char *filename;
  dvdnav_priv_t *priv;

  if(p->device) filename = p->device;
  else if(dvd_device) filename= dvd_device;
  else filename = DEFAULT_DVD_DEVICE;
  if(!(priv=new_dvdnav_stream(filename))) {
    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_CantOpenDVD,filename, strerror(errno));
    return STREAM_UNSUPPORTED;
  }

  if(p->track > 0) {
    priv->title = p->track;
    if(dvdnav_title_play(priv->dvdnav, p->track) != DVDNAV_STATUS_OK) {
      mp_msg(MSGT_OPEN,MSGL_FATAL,"dvdnav_stream, couldn't select title %d, error '%s'\n", p->track, dvdnav_err_to_string(priv->dvdnav));
      return STREAM_UNSUPPORTED;
    }
  }
#if 0
   else if (p->track == 0) {
	mplayer_dvdnav_debug("+++++++++++++++dvdnav_menu_call++++++++++++++++++\n");
    if(dvdnav_menu_call(priv->dvdnav, DVD_MENU_Root) != DVDNAV_STATUS_OK)
      ;//dvdnav_menu_call(priv->dvdnav, DVD_MENU_Title);
  }
#endif
  if(mp_msg_test(MSGT_IDENTIFY, MSGL_INFO))
    identify(priv, p);
  if(p->track > 0)
    show_audio_subs_languages(priv->dvdnav, 1);
  if(dvd_angle > 1)
    dvdnav_angle_change(priv->dvdnav, dvd_angle);

  stream->sector_size = 2048;
  stream->flags = STREAM_READ | MP_STREAM_SEEK;
  stream->fill_buffer = fill_buffer;
  stream->seek = seek;
  stream->control = control;
  stream->close = stream_dvdnav_close;
  stream->type = STREAMTYPE_DVDNAV;
  stream->priv=(void*)priv;
  *file_format = DEMUXER_TYPE_MPEG_PS;

  update_title_len(stream);
  if(!stream->pos && p->track > 0)
    mp_msg(MSGT_OPEN,MSGL_ERR, "INIT ERROR: couldn't get init pos %s\r\n", dvdnav_err_to_string(priv->dvdnav));

  mp_msg(MSGT_OPEN,MSGL_INFO, "Remember to disable MPlayer's cache when playing dvdnav:// streams (adding -nocache to your command line)\r\n");
  menu_to_movie = 0;

  return STREAM_OK;
}


void mp_dvdnav_handle_input(stream_t *stream, int cmd, int *button) {
  dvdnav_priv_t * priv = stream->priv;
  dvdnav_t *nav = priv->dvdnav;
  dvdnav_status_t status=DVDNAV_STATUS_ERR;
  pci_t *pci = dvdnav_get_current_nav_pci(nav);

  if(cmd != MP_CMD_DVDNAV_SELECT && !pci)
    return;

  switch(cmd) {
    case MP_CMD_DVDNAV_UP:
      status = dvdnav_upper_button_select(nav, pci);
      break;
    case MP_CMD_DVDNAV_DOWN:
      status = dvdnav_lower_button_select(nav, pci);
      break;
    case MP_CMD_DVDNAV_LEFT:
      status = dvdnav_left_button_select(nav, pci);
      break;
    case MP_CMD_DVDNAV_RIGHT:
      status = dvdnav_right_button_select(nav, pci);
      break;
    case MP_CMD_DVDNAV_AUTOACTION:
      status = dvdnav_autoaction_button_select(nav, pci);
      break;
    case MP_CMD_DVDNAV_MENU:
      menu_rsm_cmd = 1;
      status = dvdnav_menu_call(nav,DVD_MENU_Root);

      //Mantis:5880 Harry Potter mosaic
      extern unsigned int resync_video;
      resync_video = 2;

      break;
    case MP_CMD_DVDNAV_PREVMENU: {
      int title=0, part=0;

      dvdnav_current_title_info(nav, &title, &part);
      if(title) {
        if((status=dvdnav_menu_call(nav, DVD_MENU_Part)) == DVDNAV_STATUS_OK)
          break;
      }
      if((status=dvdnav_menu_call(nav, DVD_MENU_Title)) == DVDNAV_STATUS_OK)
        break;
      status=dvdnav_menu_call(nav, DVD_MENU_Root);
      }
      break;
    case MP_CMD_DVDNAV_SELECT:
      status = dvdnav_button_activate(nav, pci);

      extern unsigned int dvdnav_btn_virtaddr[2];
      memset(dvdnav_btn_virtaddr[0], 0x00, DVDNAV_BUTTON_SIZE);
      memset(dvdnav_btn_virtaddr[1], 0x00, DVDNAV_BUTTON_SIZE);
      clear_osd();

      //Mantis:6193 Monty Python, Last_Crusade, Chroncles of Narnia menu to movie occurr with green screen.
      //if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU)
      if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MENU && mp_dvdnav_is_stillframe(stream)==0)//Mantis:6415
      {
      	extern unsigned int resync_video;
      	resync_video = 2;
      	change_flag = 2;
      }

      break;
    case MP_CMD_DVDNAV_MOUSECLICK:
      /*
        this is a workaround: in theory the simple dvdnav_lower_button_select()+dvdnav_button_activate()
        should be enough (and generally it is), but there are cases when the calls to dvdnav_lower_button_select()
        and friends fail! Hence we have to call dvdnav_mouse_activate(priv->mousex, priv->mousey) with
        the coodinates saved by mp_dvdnav_update_mouse_pos().
        This last call always works well
      */
      status = dvdnav_mouse_activate(nav, pci, priv->mousex, priv->mousey);
      break;
    default:
      mp_msg(MSGT_CPLAYER, MSGL_V, "Unknown DVDNAV cmd %d\n", cmd);
      break;
  }

  if(status == DVDNAV_STATUS_OK)
  {
    dvdnav_get_current_highlight(nav, button);
    switch(cmd) {
       case MP_CMD_DVDNAV_MENU:
       case MP_CMD_DVDNAV_PREVMENU: 
       case MP_CMD_DVDNAV_SELECT:
       case MP_CMD_DVDNAV_MOUSECLICK:
           dvdnav_error_file = 0;
           priv->state |= NAV_FLAG_CELL_CHANGE_NOW;
           break;
       case MP_CMD_DVDNAV_UP:
       case MP_CMD_DVDNAV_DOWN:
       case MP_CMD_DVDNAV_LEFT:
       case MP_CMD_DVDNAV_RIGHT:
		   if(dvdnav_auto_action)
	           priv->state |= NAV_FLAG_CELL_CHANGE_NOW;
		   dvdnav_auto_action = 0;
       default:
           break;
   }
  }
}

void mp_dvdnav_update_mouse_pos(stream_t *stream, int32_t x, int32_t y, int* button) {
  dvdnav_priv_t * priv = stream->priv;
  dvdnav_t *nav = priv->dvdnav;
  dvdnav_status_t status;
  pci_t *pci = dvdnav_get_current_nav_pci(nav);

  if(!pci) return;

  status = dvdnav_mouse_select(nav, pci, x, y);
  if(status == DVDNAV_STATUS_OK) dvdnav_get_current_highlight(nav, button);
  else *button = -1;
  priv->mousex = x;
  priv->mousey = y;
}

static int mp_dvdnav_get_aid_from_format (stream_t *stream, int index, uint8_t lg) {
  dvdnav_priv_t * priv = stream->priv;
  uint8_t format;

  format = dvdnav_audio_stream_format(priv->dvdnav, lg);
  switch(format) {
  case DVDNAV_FORMAT_AC3:
    return index + 128;
  case DVDNAV_FORMAT_DTS:
    return index + 136;
  case DVDNAV_FORMAT_LPCM:
    return index + 160;
  case DVDNAV_FORMAT_MPEGAUDIO:
    return index;
  default:
    return -1;
  }

  return -1;
}

/**
 * \brief mp_dvdnav_aid_from_lang() returns the audio id corresponding to the language code 'lang'
 * \param stream: - stream pointer
 * \param lang: 2-characters language code[s], eventually separated by spaces of commas
 * \return -1 on error, current subtitle id if successful
 */
int mp_dvdnav_aid_from_lang(stream_t *stream, unsigned char *language) {
  dvdnav_priv_t * priv = stream->priv;
  int k;
  uint8_t lg;
  uint16_t lang, lcode;

  while(language && strlen(language)>=2) {
    lcode = (language[0] << 8) | (language[1]);
    for(k=0; k<32; k++) {
      lg = dvdnav_get_audio_logical_stream(priv->dvdnav, k);
      if(lg == 0xff) continue;
      lang = dvdnav_audio_stream_to_lang(priv->dvdnav, lg);
      if(lang != 0xFFFF && lang == lcode)
        return mp_dvdnav_get_aid_from_format (stream, k, lg);
    }
    language += 2;
    while(language[0]==',' || language[0]==' ') ++language;
  }
  return -1;
}

/**
 * \brief mp_dvdnav_aid_from_audio_num() returns the audio id corresponding to the logical number
 * \param stream: - stream pointer
 * \param audio_num: logical number
 * \return -1 on error, current subtitle id if successful
 */
int mp_dvdnav_aid_from_audio_num(stream_t *stream, int audio_num) {
  dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
  int k;
  uint8_t format, lg;

#ifdef DVDNAV_FORMAT_AC3
  //this macro is defined only in libdvdnav-cvs
  for(k=0; k<32; k++) {
    lg = dvdnav_get_audio_logical_stream(priv->dvdnav, k);
    if(lg == 0xff) continue;
    if(lg != audio_num) continue;
    format = dvdnav_audio_stream_format(priv->dvdnav, lg);
    switch(format) {
      case DVDNAV_FORMAT_AC3:
        return k+128;
      case DVDNAV_FORMAT_DTS:
        return k+136;
      case DVDNAV_FORMAT_LPCM:
        return k+160;
      case DVDNAV_FORMAT_MPEGAUDIO:
        return k;
      default:
        return -1;
    }
  }
#endif
  return -1;
}

/**
 * \brief mp_dvdnav_lang_from_aid() assigns to buf the language corresponding to audio id 'aid'
 * \param stream: - stream pointer
 * \param sid: physical subtitle id
 * \param buf: buffer to contain the 2-chars language string
 * \return 0 on error, 1 if successful
 */
int mp_dvdnav_lang_from_aid(stream_t *stream, int aid, unsigned char *buf) {
  uint8_t lg;
  uint16_t lang;
  dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;

  if(aid < 0)
    return 0;
  lg = dvdnav_get_audio_logical_stream(priv->dvdnav, aid & 0x7);
  if(lg == 0xff) return 0;
  lang = dvdnav_audio_stream_to_lang(priv->dvdnav, lg);
  if(lang == 0xffff) return 0;
  buf[0] = lang >> 8;
  buf[1] = lang & 0xFF;
  buf[2] = 0;
  return 1;
}


/**
 * \brief mp_dvdnav_sid_from_lang() returns the subtitle id corresponding to the language code 'lang'
 * \param stream: - stream pointer
 * \param lang: 2-characters language code[s], eventually separated by spaces of commas
 * \return -1 on error, current subtitle id if successful
 */
int mp_dvdnav_sid_from_lang(stream_t *stream, unsigned char *language) {
  dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
  uint8_t lg, k;
  uint16_t lang, lcode;

  while(language && strlen(language)>=2) {
    lcode = (language[0] << 8) | (language[1]);
    for(k=0; k<32; k++) {
      lg = dvdnav_get_spu_logical_stream(priv->dvdnav, k);
      if(lg == 0xff) continue;
      lang = dvdnav_spu_stream_to_lang(priv->dvdnav, lg);
      if(lang != 0xFFFF && lang == lcode) {
        return k;
      }
    }
    language += 2;
    while(language[0]==',' || language[0]==' ') ++language;
  }
  return -1;
}

/**
 * \brief mp_dvdnav_lang_from_sid() assigns to buf the language corresponding to subtitle id 'sid'
 * \param stream: - stream pointer
 * \param sid: physical subtitle id
 * \param buf: buffer to contain the 2-chars language string
 * \return 0 on error, 1 if successful
 */
int mp_dvdnav_lang_from_sid(stream_t *stream, int sid, unsigned char *buf) {
    uint8_t k;
    uint16_t lang;
    dvdnav_priv_t *priv=(dvdnav_priv_t*)stream->priv;
    if(sid < 0) return 0;
    for (k=0; k<32; k++)
        if (dvdnav_get_spu_logical_stream(priv->dvdnav, k) == sid)
            break;
    if (k == 32)
        return 0;
    lang = dvdnav_spu_stream_to_lang(priv->dvdnav, k);
    if(lang == 0xffff) return 0;
    buf[0] = lang >> 8;
    buf[1] = lang & 0xFF;
    buf[2] = 0;
    return 1;
}

int mp_dvdnav_number_of_audios(stream_t *stream) {
  dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
  uint8_t lg, k, n=0;

  for(k=0; k<8; k++) {
    lg = dvdnav_get_audio_logical_stream(priv->dvdnav, k);
    if(lg == 0xff) continue;
    n++;
  }
  return n;
}

/**
 * \brief mp_dvdnav_number_of_subs() returns the count of available subtitles
 * \param stream: - stream pointer
 * \return 0 on error, something meaningful otherwise
 */
int mp_dvdnav_number_of_subs(stream_t *stream) {
  dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
  uint8_t lg, k, n=0;

  if (priv->state & NAV_FLAG_VTS_DOMAIN) return 0;
  for(k=0; k<32; k++) {
    lg = dvdnav_get_spu_logical_stream(priv->dvdnav, k);
    if(lg == 0xff) continue;
//    if(lg >= n) n = lg + 1;
    n++;
  }
  return n;
}

/**
 * \brief mp_dvdnav_get_spu_clut() returns the spu clut
 * \param stream: - stream pointer
 * \return spu clut pointer
 */
unsigned int *mp_dvdnav_get_spu_clut(stream_t *stream) {
    dvdnav_priv_t *priv=(dvdnav_priv_t*)stream->priv;
    return (priv->state & NAV_FLAG_SPU_SET) ? priv->spu_clut : NULL;
}

/**
 * \brief mp_dvdnav_get_highlight() get dvdnav highlight struct
 * \param stream: - stream pointer
 * \param hl    : - highlight struct pointer
 */
void mp_dvdnav_get_highlight (stream_t *stream, nav_highlight_t *hl) {
  dvdnav_priv_t *priv = (dvdnav_priv_t *) stream->priv;
  dvdnav_highlight_event_t hlev = priv->hlev;

  dvdnav_get_highlight(priv, 1);
  hl->sx = hlev.sx;
  hl->sy = hlev.sy;
  hl->ex = hlev.ex;
  hl->ey = hlev.ey;
  hl->palette = hlev.palette;
}

/**
 * \brief Check if end of stream has been reached
 * \param stream: - stream pointer
 * \return 1 on really eof
 */
int mp_dvdnav_is_eof (stream_t *stream) {
  return ((dvdnav_priv_t *) stream->priv)->state & NAV_FLAG_EOF;
}

int mp_dvdnav_is_stop (stream_t *stream) {
  return ((dvdnav_priv_t *) stream->priv)->state & NAV_FLAG_STOP;
}

int mp_dvdnav_is_stillframe (stream_t *stream) {
  return ((dvdnav_priv_t *) stream->priv)->state & NAV_FLAG_STILL_FRAME;
}

/**
 * \brief Skip still frame
 * \param stream: - stream pointer
 * \return 0 on success
 */
int mp_dvdnav_skip_still (stream_t *stream) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (priv->still_length == 0xff)
    return 1;
  priv->state &= ~NAV_FLAG_STILL_FRAME;
  dvdnav_still_skip(priv->dvdnav);
  return 0;
}

/**
 * \brief Skip wait event
 * \param stream: - stream pointer
 * \return 0 on success
 */
int mp_dvdnav_skip_wait (stream_t *stream) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (!(priv->state & NAV_FLAG_WAIT))
    return 1;
  priv->state &= ~NAV_FLAG_WAIT;
  dvdnav_wait_skip(priv->dvdnav);
  return 0;
}

/**
 * \brief Set wait mode
 * \param stream  : - stream pointer
 * \param mode    : - if true, then suspend block read
 * \param automode: - if true, then VTS or cell change set wait mode
 */
void mp_dvdnav_read_wait (stream_t *stream, int mode, int automode) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (mode == 0)
    priv->state &= ~NAV_FLAG_WAIT_READ;
  if (mode > 0)
    priv->state |= NAV_FLAG_WAIT_READ;
  if (automode == 0)
    priv->state &= ~NAV_FLAG_WAIT_READ_AUTO;
  if (automode > 0)
    priv->state |= NAV_FLAG_WAIT_READ_AUTO;
}

/**
 * \brief Check if cell need to changed now
 * \param stream: - stream pointer
 * \param clear : - if true, then clear cell change flag
 * \return 1 if cell has changed
 */
int mp_dvdnav_cell_has_changed_now (stream_t *stream, int clear) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (!(priv->state & NAV_FLAG_CELL_CHANGE_NOW))
    return 0;

  if (clear) {
    priv->state &= ~NAV_FLAG_CELL_CHANGE_NOW;
  }
  return 1;
}

int mp_dvdnav_vts_has_changed(stream_t *stream, int clear) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (!(priv->state & NAV_FLAG_VTS_CHANGE))
    return 0;

  if (clear) {
    priv->state &= ~NAV_FLAG_VTS_CHANGE;
  }
  return 1;
}

/**
 * \brief Check if cell has changed
 * \param stream: - stream pointer
 * \param clear : - if true, then clear cell change flag
 * \return 1 if cell has changed
 */
int mp_dvdnav_cell_has_changed (stream_t *stream, int clear) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (!(priv->state & NAV_FLAG_CELL_CHANGE))
    return 0;

  if (clear) {
    priv->state &= ~NAV_FLAG_CELL_CHANGE;
    priv->state |= NAV_FLAG_STREAM_CHANGE;
  }
  return 1;
}

int mp_dvdnav_is_wait (stream_t *stream) {
  dvdnav_priv_t *priv = (dvdnav_priv_t*)stream->priv;
  if (!(priv->state & NAV_FLAG_WAIT))
    return 0;

  return 1;
}

// get current audio channel id
int mp_dvdnav_get_audio(stream_t *stream) {
    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
    return dvdnav_get_active_audio_stream(priv->dvdnav);
}

// get current spu
int mp_dvdnav_get_spu(stream_t *stream) {
    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
    return dvdnav_get_active_spu_stream(priv->dvdnav);
}

// get all audio info
int mp_dvdnav_get_all_audio_info(stream_t *stream) {
    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;

    show_audio_subs_languages(priv->dvdnav, 1);
    return 0;
}

/**
 * \brief Check if audio channel has changed
 * \param stream: - stream pointer
 * \param clear : - if true, then clear audio change flag
 * \return 1 if audio has changed
 */
int mp_dvdnav_audio_has_changed (stream_t *stream) {
    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
    if(!priv->state & NAV_FLAG_AUDIO_CHANGE) return 0;
    priv->state &= ~NAV_FLAG_AUDIO_CHANGE;
  return 1;
}

/**
 * \brief Check if SPU has changed
 * \param stream: - stream pointer
 * \param clear : - if true, then clear spu change flag
 * \return 1 if spu has changed
 */
int mp_dvdnav_spu_has_changed (stream_t *stream) {
    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
    if(!(priv->state & NAV_FLAG_SPU_CHANGE)) return 0;
    priv->state &= ~NAV_FLAG_SPU_CHANGE;

  return 1;
}

/* Notify if something has changed in stream
 * Can be related to title, chapter, audio or SPU
 * is change title, part, audio or spu
 */
int mp_dvdnav_stream_has_changed (stream_t *stream) {
    int tit = 0, part = 0, tit_nr = 0, part_nr = 0;
    audio_attr_t audio_attr;
    int i,sub_id,audio_id;
    uint16_t language;

    dvdnav_priv_t * priv=(dvdnav_priv_t*)stream->priv;
    if(!(priv->state & NAV_FLAG_STREAM_CHANGE)) return 0;	// no change
//dvdnavstate.dvdnav_cell_change = 1;
//Fuchun 2011.01.05
extern unsigned int resync_video;
if(dvdnavstate.dvdnav_title_state == DVDNAV_TITLE_STATE_MOVIE && speed_mult == 0 && resync_video == 0 && menu_to_movie == 1)
{
	menu_to_movie = 0;
	extern int change_flag;
	resync_video = 2;
	change_flag = 1;
}

  priv->state &= ~NAV_FLAG_STREAM_CHANGE;
    dvdnav_current_title_info(priv->dvdnav, &tit, &part);	// get curren title and part
    dvdnav_get_number_of_titles(priv->dvdnav,&tit_nr);
    dvdnav_get_number_of_parts(priv->dvdnav,tit,&part_nr);
    priv->titles_nr = tit_nr;		// update titles numbers
    priv->chapters_nr = part_nr;	// update chapters numbers
    priv->title=tit;			// update current title
    priv->part=part;			// update current chapter
    priv->nr_of_subtitles=0;
    priv->nr_of_channels=0;
    if (priv->state & NAV_FLAG_VTS_DOMAIN) return 1;	// if no vts then no set audio and spu
    for(i=0;i<32;i++) {			// updates spu's
	    priv->subtitles[priv->nr_of_subtitles].id=-1;
        if (0>(sub_id=dvdnav_get_spu_logical_stream(priv->dvdnav,i))) continue;
	    language=dvdnav_spu_stream_to_lang(priv->dvdnav, sub_id);
	    priv->subtitles[priv->nr_of_subtitles].language=language;
	    priv->subtitles[priv->nr_of_subtitles].id=sub_id;
	    priv->nr_of_subtitles++;
	}
    for(i=0;i<32;i++) {			// updates audio channels
	    priv->audio_streams[priv->nr_of_channels].id=-1;
        if (0>(audio_id=dvdnav_get_audio_logical_stream(priv->dvdnav,i))) continue;
	    language=dvdnav_audio_stream_to_lang(priv->dvdnav, audio_id);
        audio_id = mp_dvdnav_aid_from_audio_num(stream, audio_id);
        memset(&audio_attr,0,sizeof(audio_attr_t));
        dvdnav_get_audio_attr(priv->dvdnav, i, &audio_attr);
	    priv->audio_streams[priv->nr_of_channels].language=language;
	    priv->audio_streams[priv->nr_of_channels].id=audio_id;
	    priv->audio_streams[priv->nr_of_channels].type=audio_attr.audio_format;
	    priv->audio_streams[priv->nr_of_channels].channels=audio_attr.channels;
	    priv->nr_of_channels++;
	}
  return 1;
}

void mp_dvdnav_switch_title (stream_t *stream, int title) {
  dvdnav_priv_t *priv = stream->priv;
  uint32_t titles;
//printf("title_prop_num[%d], titles_nr[%d], title_prop->title[%d]\n",priv->title_prop_num,priv->titles_nr,priv->title_prop->title);
//printf("priv->part[%d], priv->title[%d]\n",priv->part,priv->title);
  dvdnav_get_number_of_titles (priv->dvdnav, &titles);
  if (title > 0 && title <= titles)
    dvdnav_title_play (priv->dvdnav, title);
}

title_prop_t* seek_title_prop(dvdnav_priv_t * priv, int title) {
    int i;
    
    if(!priv->title_prop)
        return NULL;
    if (priv->state & NAV_FLAG_VTS_DOMAIN)
        return NULL;
    for(i=0;i<priv->title_prop_num;i++)
        if(priv->title_prop[i].title==title)
	    return &priv->title_prop[i];
    return NULL;
}

title_prop_t* add_title_prop(dvdnav_priv_t * priv, int title) {
    title_prop_t* title_prop;
    priv->title_prop_num++;
    if(priv->title_prop)
        priv->title_prop=realloc(priv->title_prop,priv->title_prop_num*sizeof(title_prop_t));
    else
        priv->title_prop=malloc(priv->title_prop_num*sizeof(title_prop_t));
    title_prop=&priv->title_prop[priv->title_prop_num-1];
    title_prop->title=title;
    title_prop->mp_aid=-1;
    title_prop->nav_aid=-1;
    title_prop->mp_sid=-1;
    title_prop->nav_sid=-1;
    return title_prop;
}

int mp_dvdnav_reg_nav_aid(stream_t *stream, int audio_id) {
    dvdnav_priv_t* priv=(dvdnav_priv_t*)stream->priv;
    title_prop_t* title_prop = seek_title_prop(priv,priv->title);
    if (priv->state & NAV_FLAG_VTS_DOMAIN)
        return audio_id;
    if(!title_prop)
        title_prop=add_title_prop(priv,priv->title);
    if(title_prop->mp_aid==-1)
        return title_prop->nav_aid=audio_id;
    if(title_prop->nav_aid==-1)
        title_prop->nav_aid=audio_id;
    if(title_prop->nav_aid==audio_id)
        return title_prop->mp_aid;
    title_prop->nav_aid=audio_id;
    return audio_id;
}

int dvdnav_reg_nav_sid(stream_t *stream, int dvdsub_id) {
    dvdnav_priv_t* priv=(dvdnav_priv_t*)stream->priv;
    title_prop_t* title_prop = seek_title_prop(priv,priv->title);
    if (priv->state & NAV_FLAG_VTS_DOMAIN)
        return dvdsub_id;
    if(!title_prop)
        title_prop=add_title_prop(priv,priv->title);
    if(title_prop->mp_sid==-1)
        return title_prop->nav_sid=dvdsub_id;
    if(title_prop->nav_sid==-1)
        title_prop->nav_sid=dvdsub_id;
    if(title_prop->nav_sid==dvdsub_id)
        return title_prop->mp_sid;
    title_prop->nav_sid=dvdsub_id;
    return dvdsub_id;
}

int mp_dvdnav_get_cur_title (stream_t *stream, int *cur_title) {
  dvdnav_priv_t *priv = (dvdnav_priv_t *) stream->priv;
  uint32_t titles;

  dvdnav_get_number_of_titles (priv->dvdnav, cur_title);
  
  return priv->title;
}

const stream_info_t stream_info_dvdnav = {
  "DVDNAV stream",
  "null",
  "",
  "",
  open_s,
  { "dvdnav", NULL },
  &stream_opts,
  1 // Urls are an option string
};
