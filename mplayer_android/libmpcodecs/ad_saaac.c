
/*
//------------------------------------------------------------------------------------------------------
//   skyviia HEAAC V2  decoder
//
//------------------------------------------------------------------------------------------------------
*/



#include <stdio.h>
#include <stdlib.h>

#ifndef  EngineMode
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "skipc.h"
#include "config.h"
#include "ad_internal.h"
#include "libmpcodecs/sky_vdec_2.h"
static ad_info_t info =
{
    "Skyviia HEAAC V2 decoder",
    "saaac",
    "Honda",
    "Honda",
    "Skyviia copyright"
};

LIBAD_EXTERN(saaac)
#else
#include "mp_msg.h"
#include "help_mp.h"
#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "aac_mplayer.h"

#endif



#include "audio_server.h"


extern unsigned char* sm_com;
extern unsigned char* sm_in;
extern unsigned char* sm_out;

static int sm_com_id=-1, sm_data_id=-1, id=-1;
static int in_buf_len;
static int in_buf_size;
//--------------------------------------------------------------------
// 20111031 charleslin increasing sh->pts by decoded pcm data
//#define SAAAC_DECODE_PTS
#ifdef SAAAC_DECODE_PTS
#define MAX_NOPTS_PKTS	20
double startpts;
unsigned long long decsum;
#endif

#define AAC_2CH_ONLY

#ifdef   AAC_2CH_ONLY
#define AAC_MAX_CHANNELS 2
#else
#define AAC_MAX_CHANNELS 6
#endif

#define USE_NEW_AAC_METHOD 1

#ifdef USE_NEW_AAC_METHOD	//=========
#define AAC_BUFFLEN (768*6)
static int use_aac_bufflen = AAC_BUFFLEN;

#ifdef  EngineMode  //驗證用only
extern  PCM32S PCM32_OUT ;
#endif



static int ADIF_ON;     //Honda : for ADIF
static int AAC_MODE;    //0: mp4, 1: LATM, 2: ADIF, 3: ADTS
extern int audio_output_channels;



static unsigned int fillin_ptr = 0;
static unsigned int consumed_ptr = 0 ;
static unsigned int residue_num = 0 ;
#define AAC_SM_BLOCK      3
//20101227 Robert fix buffer overflow for ADIF case
#define MAX_AAC_LOCALBUF (1024*32)
static unsigned char *aac_local_buf=NULL;
static int local_offset=0;
static int need_update_pts_flag=0;
extern float rel_seek_secs;
static int need_resync=0;
static int aac_sync(sh_audio_t *sh);

int aac_resync_stream(sh_audio_t *sh)
{
#ifdef SAAAC_DECODE_PTS
	decsum = 0;
	startpts = MP_NOPTS_VALUE;
	sh->pts = MP_NOPTS_VALUE;
#endif
	local_offset = 0;
	consumed_ptr = 0;
	sh->a_in_buffer_len = residue_num = 0;
	return aac_sync(sh);
}

int aac_demux_read_data(sh_audio_t *sh, demux_stream_t *ds, unsigned char *mem, int len)
{
    int ret=0, getlen=0, total_getlen=0;
    double pts, nextpts, tmppts, lastpts;
#if 1
    if (!sh->codecdata_len)
    {
        unsigned char *buf2;
        ret = local_offset;
        while((ret < len || ret < AAC_BUFFLEN) && ret <= AAC_BUFFLEN*2)
        {
#ifdef SAAAC_DECODE_PTS
	    int i;
	    for(i=0; i < MAX_NOPTS_PKTS; i++){
		    getlen = ds_get_packet_pts(sh->ds, &buf2, &pts);
		    if(sh->ds->eof || getlen <= 0 || startpts != MP_NOPTS_VALUE){
			    //printf("getlen:%d ds->eof:%d startpts:%f\n", getlen, sh->ds->eof, startpts);
			    break;
		    }else if(pts != MP_NOPTS_VALUE){	// found valid pts, assign to startpts
			    startpts = pts; //set first valid pts as start pts
			    printf("%s:%d set startpts to %f\n", __func__, __LINE__, startpts);
			    break;
		    }else{				// skip audio packet without pts
			    //printf("%s:%d skip audio packets without pts\n", __func__, __LINE__);
		    }
	    }

	    if(i >= MAX_NOPTS_PKTS)
		    printf("%s:%d can't find valid pts for startpts\n", __func__, __LINE__);

            nextpts = ds_get_next_pts(sh->ds);
            if (getlen > 0)
                total_getlen += getlen;

            if (getlen > 0 && pts != MP_NOPTS_VALUE && nextpts != MP_NOPTS_VALUE){
		double duration = nextpts - pts;

                if(nextpts < sh->pts){ //pattern gen loop? resync later...
		    printf("%s:%d need to resync stream nextpts:%f < sh_pts:%f\n", __func__, __LINE__, nextpts, sh->pts);
		    need_resync = 1;
		    break;
		}

                if (duration < 0 || duration > 5.0){
                    sh->pts = pts;
                    local_offset = 0;
                    ret = 0;
                    sh->a_in_buffer_len = 0;
                }
                sh->pts_bytes = 0;
                need_update_pts_flag = 0;
	    }
#else
            lastpts = sh->ds->pts;
            getlen = ds_get_packet_pts(sh->ds, &buf2, &pts);
//printf("**** getlen=%d pts=0x%x\n", getlen, pts);
            if (getlen > 0)
                total_getlen += getlen;
//            if (need_update_pts_flag==1 && pts != MP_NOPTS_VALUE && getlen>0) 
//            if (pts != MP_NOPTS_VALUE && getlen>0) 
//printf("001 residue_num+local_offset=%d   fi-co+lo=%d\n", residue_num+local_offset, fillin_ptr- consumed_ptr+local_offset);

            if (lastpts != MP_NOPTS_VALUE && getlen>0) {
                /*
                			if (use_aac_bufflen > getlen*3)
                			{
                				use_aac_bufflen = getlen*3;
                			}
                */
//                nextpts = ds_get_next_pts(sh->ds);
//printf("== getlen=%d  old pts_bytes=%d  local_offset=%d a_in=%d \npts=%f nextpts=%f\n", getlen, sh->pts_bytes, local_offset, sh->a_in_buffer_len, (float)pts, (float)nextpts);
//                if (nextpts - pts > 0 && nextpts - pts <= 5.0)
                if (pts - lastpts >= 0 && pts - lastpts <= 5.0)
                {
//                    tmppts = pts - ((double)(local_offset + sh->a_in_buffer_len)/(double)getlen)*(nextpts - pts);
#if 0
                    if (sh->a_in_buffer_len > AAC_BUFFLEN)
                    {
                    tmppts = pts - ((double)(local_offset + sh->a_in_buffer_len - AAC_BUFFLEN)/(double)getlen)*(pts - lastpts);
                    }
                    else
                    {
                    tmppts = pts - ((double)(local_offset + sh->a_in_buffer_len)/(double)getlen)*(pts - lastpts);
                    }
#else
                    tmppts = pts - ((double)(local_offset + fillin_ptr - consumed_ptr)/(double)getlen)*(pts - lastpts);

#endif
//printf("@@ origpts=%f lo=%d ain=%d tmppts=%f  pts=%f lastpts=%f\n", sh->pts, local_offset, sh->a_in_buffer_len, tmppts, pts, lastpts);
//printf("residue_num %d -> fillin_ptr - consumed_ptr =%d ain=%d\n", residue_num, fillin_ptr - consumed_ptr, sh->a_in_buffer_len);
//printf("residue_num+local_offset=%d   fi-co+lo=%d\n", residue_num+local_offset, fillin_ptr- consumed_ptr+local_offset);
//		        	sh->pts = pts - ((double)(local_offset + sh->a_in_buffer_len)/(double)getlen)*(nextpts - pts);
			if (tmppts > 0)
			{
	                    if (sh->pts > 0 && (sh->pts - tmppts > 2 || tmppts - sh->pts > 2))
	                    {
	//                        rel_seek_secs += 0.1;
	                        sh->pts = tmppts;
	//printf("@@@@@@ got case 1\n");
	                    }
	                    else
	                    {
	                        sh->pts = tmppts;
	/*
	                        if (tmppts >= sh->pts)
	                        {
	                            sh->pts = tmppts;
	//printf("@@@@@@ case 2\n");
	                        }
	                        else
	                        {
	//printf("@@@@@@ case 3\n");

	                        }
	*/
	                    }
	//printf("== getlen=%d  old pts_bytes=%d  local_offset=%d a_in=%d \npts=%f nextpts=%f  sh->pts=%f (tmppts=%f) offset=%f\n", getlen, sh->pts_bytes, local_offset, sh->a_in_buffer_len, (float)pts, (float)nextpts, (float)sh->pts, (float)tmppts, (float)(sh->pts- pts));
			}
                }
                else
                {
//printf("@@@!!!!!!!  nextpts=%f  pts=%f\n", (float)nextpts, (float)pts);
                    sh->pts = pts;
                    local_offset = 0;
                    ret = 0;
                    sh->a_in_buffer_len = 0;
                }
                sh->pts_bytes = 0;
                need_update_pts_flag = 0;
            }
#endif
            if (getlen <= 0)
                break;

            ret += getlen;

            fast_memcpy(&aac_local_buf[local_offset], buf2, getlen);
            local_offset += getlen;
        }
//printf("local_offset = %d\n", local_offset);
//20101227 Robert fix buffer overflow for ADIF case
/*
        if (ret <= AAC_BUFFLEN*2)
        {
            if (ret < AAC_BUFFLEN)
            {
                getlen = ds_get_packet_pts(sh->ds, &buf2, &pts);
                if (getlen > 0)
                {
                    ret += getlen;
                    fast_memcpy(&aac_local_buf[local_offset], buf2, getlen);
                    local_offset += getlen;
                }
            }
        }
*/
/*
        if (total_getlen <= 0)
        {
            ret = 0;
        }
        else
*/
        {
            if (ret >= len)
            {
                fast_memcpy(mem, &aac_local_buf[0], len);

                if (local_offset-len>0)
                {
                    memmove(aac_local_buf, &aac_local_buf[len], local_offset - len);
                }
                local_offset -= len;
            }
            else if (ret > 0)
            {
                fast_memcpy(mem, &aac_local_buf[0], ret);
                local_offset = 0;
            }

            if (local_offset < 0)
                local_offset = 0;
            if (ret >= len)
                ret = len;
        }
    }
    else
#endif
    {
        ret= demux_read_data(ds, mem, len);
    }
    return ret;
}

#define demux_read_data(a, b, c, d) aac_demux_read_data(a, b, c, d)

/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
//
//------------------------------------------------------------------------------------------------------
*/
static int preinit(sh_audio_t *sh)
{
    aserver_pid = 0;

#ifndef  EngineMode
    union smc *sc;
    long curr_codec;
#endif
    {
        sh->audio_out_minsize=8192*AAC_MAX_CHANNELS;
        sh->audio_in_minsize=AAC_BUFFLEN;//0; //not used
        fillin_ptr = consumed_ptr = fillin_ptr = 0 ;//initialize it everytime
#ifdef SAAAC_DECODE_PTS
	printf("%s:%d reset startpts to MP_NOPTS_VALUE\n", __func__, __LINE__);
	decsum = 0;
	startpts = MP_NOPTS_VALUE;
#endif
    }

#ifndef  EngineMode
#ifdef AD_SHM_REWRITE
	sm_com_id = get_first_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);
	if(sm_com_id < 0)
		sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if(sm_com == NULL)
	{
		if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command )
			return 0;
	}
#else
	sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command )
		return 0;
#endif

    
    id = get_semaphore_id();                                 //get semaphore ID

    if (!reset_semvalue(id, 0)) {                            //reset semaphore
        return 0;
    }

    sc = (union smc *)sm_com;

    sc->magic.magic_num = PLAYER_MAGIC;
    curr_codec = AAC_ID;                                     //inform server what kind of coded to be used.

    sc->magic.codec_id = curr_codec;
    if (!semaphore_v(id, 1))                                 //wakeup server!
        return 0;

    if (!wait_server(sc)) {                                   //wait for server's response
        deallocate_share_mem(sm_com);
#ifndef AD_SHM_REWRITE
        shmctl(sm_com_id, IPC_RMID, 0);
#endif
        del_semvalue(id, 0);
        printf("Audio server crash \n");
        return 0;
    }

    if (sc->magic.codec_id != curr_codec) {                  //check codec
        printf("Unsuppoted codec\n");
        return 0;
    }


#endif

    aserver_pid = sc->pinit.pid;

    return 1;
}

static int aac_probe(unsigned char *buffer, int len, int mode)
{
    int i = 0, pos = 0;
    mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: %d bytes\n", len);
//printf("\ntry to AAC_PROBE: %d bytes\n", len);
    while(i <= len-4) {
#if 1
        int t, t2;
        if ((buffer[i] == 0x56) && (mode==0 || mode==1)) {
            if ((buffer[i+1]&0xE0)==0xE0) {
                t = ((buffer[i+1]&0x1F)<<8)+buffer[i+2];
                if ((i+4+t)<len) {
                    t2 = (buffer[i+3+t]<<8)+(buffer[i+4+t]);
                    if ((t2>>5)==0x2b7) {
                        pos = i;
                        break;
                    }
                }
            }
        }
        if ((buffer[i] == 0xff) && (mode==0 || mode==3)) {
            if ((buffer[i+1] & 0xf6) == 0xf0) {
                t = ((buffer[i+3]&0x3)<<11)+(buffer[i+4]<<3)+(buffer[i+5]>>5);
                if ((i+1+t)<len) {
                    t2 = (buffer[i+t]<<8)+(buffer[i+1+t]);
                    if ((t2&0xfff6)==0xfff0) {
                        pos = i;
                        break;
                    }
                }
            }
        }
        if ((mode==0 || mode==2) && (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')) {
            pos = i;
            break;
        }
#else
        if(
            ((buffer[i] == 0xff) && ((buffer[i+1] & 0xf6) == 0xf0)) ||
            (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')
        ) {
            pos = i;
            break;
        }
#endif

        i++;
    }
    mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: ret %d\n", pos);
//printf("AAC_PROBE:  ret=%d  i=%d  len=%d\n", pos, i, len);
//    return pos;
    return i;
}





int AAC_frame_init(sh_audio_t *sh, aduio_info *ai) {



    int init = 1 , ret;
    int buflen ;
    union smc *sc = (union smc *)sm_com;
    play_init *pi = &sc->pinit;
    serv_init *si = &sc->sinit;



    //send to server
    buflen = sh->codecdata_len;
    if(!buflen)
    {
        buflen =sh->a_in_buffer_len  ;
        //memcpy(sm_in,sh->a_in_buffer,   sh->a_in_buffer_len); // pes bistream  to sm_in
    }
    else
        memcpy(sm_in,sh->codecdata,  buflen);   //MP4 bistream  to sm_in

    //some parameters to server for ini
    pi->AACinfo.sh_codecdata_len =  sh->codecdata_len;
    pi->AACinfo.sh_samplerate = sh->samplerate ;
    pi->AACinfo.sh_samplesize = sh->samplesize ;
    pi->paremeter1 = init;
    pi->paremeter3 = audio_output_channels ;           //send channel number to server

    if(!ipc_init(sc, AAC_ID, buflen, id, sm_data_id, 0)) //init server
        return 0;//break;		//IPC Error
    if (si->codec_id == AAC_ID)
        ret = 0;
    else
        ret = si->codec_id;   //error code

    init = 0;

    *ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

    return ret; // OK!!!


}



/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory,initialize audio server
//
//------------------------------------------------------------------------------------------------------
*/
static int init(sh_audio_t *sh)
{

    int aac_init, pos = 0;
    int output_channels;
    int ret_init=0;

    //for audio server
    aduio_info gai;
    aduio_info *ai = &gai;

    int aac_retry_cnts = 6;
    while(aac_retry_cnts-->0)
    {
        ret_init=0;
        pos = 0;
        ai = &gai;

        use_aac_bufflen = AAC_BUFFLEN;

#ifndef  EngineMode
#ifdef AD_SHM_REWRITE
	sm_data_id = get_first_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_data_id < 0)
		sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_in == NULL)
	{
		if ((sm_in = allocate_share_mem(sm_data_id))==NULL) 
		{
			printf("data share memory allocate fail\n");
			return 0;
		}
	}
#else
	sm_data_id = get_shared_mem_id(KEY_SM_IN, AAC_BUFFLEN*AAC_SM_BLOCK);//expansion 3
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL) 
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif
#endif


//20101227 Robert fix buffer overflow for ADIF case
        if (aac_local_buf == NULL)
        {
            aac_local_buf = malloc(MAX_AAC_LOCALBUF);
        }
        memset(aac_local_buf, 0x00, MAX_AAC_LOCALBUF);

        local_offset = 0;
        need_update_pts_flag = 1;

#ifdef   AAC_2CH_ONLY
        output_channels = 2;
#else
        output_channels = audio_output_channels;
#endif
        AAC_MODE = 0;
        // If we don't get the ES descriptor, try manual config
        //player
        if(!sh->codecdata_len && sh->wf) {
            sh->codecdata_len = sh->wf->cbSize;
//      sh->codecdata = (unsigned char*)(sh->wf+1);		//Barry 2010-05-20 disable	Fuchun 2010.04.20 enable
            sh->codecdata = malloc(sh->codecdata_len);		//Barry 2010-05-20 enable	Fuchun 2010.04.20 disable
            memcpy(sh->codecdata, sh->wf+1, sh->codecdata_len);
            mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: codecdata extracted from WAVEFORMATEX\n");
        }


        //player
        if(!sh->codecdata_len)
        {
            ///sh->a_in_buffer_len = demux_read_data(sh, sh->ds, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_size);
            sh->a_in_buffer_len = demux_read_data(sh, sh->ds, sm_in, use_aac_bufflen);
            if (sh->ds->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
                if (sh->a_in_buffer_len == 0)
                {
                    int retry_timeout_cnt = 100;
                printf("######################\n##################\n");
                printf("AAC init  == demux_read_data = 0\n");
                    while(sh->a_in_buffer_len == 0 && retry_timeout_cnt-- > 0)
                    {
                        sh->a_in_buffer_len = demux_read_data(sh, sh->ds, sm_in, use_aac_bufflen);
                        usleep(1000);
                    }
                }
                
                if (sh->a_in_buffer_len == 0)
                {
                    usleep(1000);
                    continue;
                }
            }
            pos = aac_probe( sm_in, sh->a_in_buffer_len, AAC_MODE);
            if(pos) {
                sh->a_in_buffer_len -= pos;
                memmove( sm_in,  sm_in +pos, sh->a_in_buffer_len);
                sh->a_in_buffer_len +=
                    demux_read_data(sh, sh->ds, (sm_in + sh->a_in_buffer_len), sh->a_in_buffer_size - sh->a_in_buffer_len);

                pos = 0;
            }
            //ini
            fillin_ptr = sh->a_in_buffer_len ;
            residue_num = fillin_ptr - consumed_ptr ;

        }


        if(!sh->samplesize) sh->samplesize=2;

        ret_init = AAC_frame_init(sh,ai) ;  //ai:audio info
        aac_init = ai->skaac_ini ;

        if(aac_init < 0|| ret_init!=0)
        {
//printf("AAC init fail, retry_cnts=%d ret_init=%d aac_init=%d\n", aac_retry_cnts, ret_init, aac_init);
            uninit(sh);
            preinit(sh);
        }
        else
        {
//printf("AAC init ok  ret_init=%d aac_init=%d\n", ret_init, aac_init);
            break;
        }
    }

    sh->i_bps = ai->sh_i_bps/8 ; //from server
    AAC_MODE = ai->AAC_MODE ;  //from server
    aac_init = ai->skaac_ini ;


//player
    if(!sh->codecdata_len ) {

        sh->a_in_buffer_len -= (aac_init > 0)?aac_init:0; // how many bytes init consumed
        if (aac_init>0) {                 //Honda : for ADIF
            memmove(sm_in,sm_in+aac_init,sh->a_in_buffer_len);
            ADIF_ON = 1;
            fillin_ptr = sh->a_in_buffer_len ;
            residue_num =  sh->a_in_buffer_len ;
        }

    }




    if(aac_init < 0|| ret_init!=0) {
        mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: Failed to initialize the decoder!\n"); // XXX: deal with cleanup!

        // XXX: free a_in_buffer here or in uninit?
        return 0;
    }
    else {

        sh->channels = ai->ch;;//get from server;
        sh->samplerate = ai->samplerate;//get from server ;
        sh->samplesize=2;
        if(!sh->i_bps) {
#if 0	//Barry 2010-11-10	avoid a/v async
            mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: compressed input bitrate missing, assuming 0 kbit/s!\n");
            sh->i_bps = 0; // XXX: HACK!!! ::atmos
#else
            mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: compressed input bitrate missing, assuming 128kbit/s!\n");
            sh->i_bps = 128*1000/8; // XXX: HACK!!! ::atmos
#endif
        } else
            mp_msg(MSGT_DECAUDIO,MSGL_V,"SAAAC: got %dkbit/s bitrate from MP4 header!\n",sh->i_bps*8/1000);
        mp_msg(MSGT_DECAUDIO,MSGL_V,"USING %s\n", "SKYVIIA HEAAC V2 LIBRARY");
    }


    return 1;
}







/*
//-----------------------------------------------------------------------------------------
//    free AAC decoder
//
//-----------------------------------------------------------------------------------------
*/
static void uninit(sh_audio_t *sh)
{
    union smc *sc = (union smc *)sm_com;
    ipc_uninit(sc, AAC_ID, id);
    //server
#ifndef  EngineMode
    deallocate_share_mem(sm_in);
    deallocate_share_mem(sm_out);
#ifndef AD_SHM_REWRITE
    shmctl(sm_data_id, IPC_RMID, 0);
#endif
    deallocate_share_mem(sm_com);
#endif
    printf("=== aac uninit ===\n");
}


/*
//-----------------------------------------------------------------------------------------
//    bistream sync
//
//-----------------------------------------------------------------------------------------
*/
static int aac_sync(sh_audio_t *sh)
{
    int pos = 0;
    int read_length ;

    if(!sh->codecdata_len) {
        need_update_pts_flag = 1;
//        local_offset = 0;
        //ADTS/LATM
        residue_num = fillin_ptr = sh->a_in_buffer_len ;
//        memcpy(sm_in,sh->a_in_buffer, sh->a_in_buffer_len);
//        consumed_ptr = 0;

        if(residue_num < AAC_BUFFLEN) { //若小於guard ,就再read one blcok .

            if( fillin_ptr>AAC_BUFFLEN*(AAC_SM_BLOCK-1) )
            {
                read_length = demux_read_data(sh, sh->ds,(sm_in + fillin_ptr), AAC_BUFFLEN);
                fillin_ptr = residue_num + read_length ;
                //consumed_ptr = 0 ;

            } else {
                //read one block defaultly (maybe read_length<=AAC_BUFFLEN)
                read_length = demux_read_data(sh, sh->ds,sm_in+fillin_ptr, AAC_BUFFLEN);
                fillin_ptr+=read_length;

            }
            sh->a_in_buffer_len = fillin_ptr;
            residue_num = fillin_ptr;
        }


        pos = aac_probe(sm_in, sh->a_in_buffer_len, AAC_MODE); //find aac mode,and raw data position
//printf("aac_sync :  aac_probe ret=%d\n", pos);
        if(pos) {
            sh->a_in_buffer_len -= pos;
            memmove(sm_in,( sm_in+pos), sh->a_in_buffer_len);   //move to sm_in
            mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC SYNC AFTER %d bytes\n", pos);
        }

        consumed_ptr = 0 ;
        fillin_ptr = sh->a_in_buffer_len ;
        residue_num = fillin_ptr = sh->a_in_buffer_len ;
//printf("new residue_num = %d\n", residue_num);
    }



    return pos;
}




/*
//------------------------------------------------------------------------------------------------------
//     AAC decoder
//
//------------------------------------------------------------------------------------------------------
*/

#define MAX_ERRORS 10
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
    int len = 0, last_dec_len = 1, errors = 0;
    int   nsamples = 0,buflen = 0;
    union smc *sc = (union smc *)sm_com;     //  shared memory
    unsigned char* bufptr=NULL;
    double pts;
    int read_length = 0;

    need_update_pts_flag = 1;
    while(len < minlen && last_dec_len > 0 && errors < MAX_ERRORS && len != -1) {

        //[PES]
        /* update buffer for raw aac streams: */
        if(!sh->codecdata_len)
		{

			//printf(" *** residue_num=%d sh->a_in_buffer_len=%d\n", residue_num, sh->a_in_buffer_len);
			//printf("consumed_ptr=%d\n", consumed_ptr);
			//printf(" ~~~~ residue_num=%d fillin_ptr(%d) - consumed_ptr(%d)=%d\n", residue_num, fillin_ptr, consumed_ptr, fillin_ptr - consumed_ptr);
			//residue_num = fillin_ptr - consumed_ptr;
			if(residue_num < use_aac_bufflen) { //若小於guard ,就再read one blcok .

				if( fillin_ptr>use_aac_bufflen*(AAC_SM_BLOCK-1) )
				{

					read_length = aac_demux_read_data(sh, sh->ds,(sm_in + residue_num), use_aac_bufflen);
					fillin_ptr = residue_num + read_length ;
					//                    consumed_ptr = 0 ;
					sh->a_in_buffer_len = fillin_ptr;
				}
				else {
					//read one block defaultly (maybe read_length<=use_aac_bufflen)
					read_length = aac_demux_read_data(sh, sh->ds,sm_in+fillin_ptr, use_aac_bufflen);
					fillin_ptr+=read_length;
					sh->a_in_buffer_len = fillin_ptr;
					//printf(" ---> new read_length=%d fillin_ptr=%d\n", read_length, fillin_ptr);
				}
				//printf("==> new fillin_ptr=%d  read_length=%d\n", fillin_ptr, read_length);
#ifdef SAAAC_DECODE_PTS
				if(need_resync){
					need_resync = 0;
					printf("%s:%d AAC resync stream\n", __func__, __LINE__);
					aac_resync_stream(sh);
					break;
				}
#endif
			}
		}


        if(!sh->codecdata_len) 
		{
            residue_num = fillin_ptr - consumed_ptr ;
//printf("new residue_num=%d\n", residue_num);
            if (sh->ds->demuxer->type == DEMUXER_TYPE_SKYMPEG_TS)
            {
                if (residue_num < 1024 && read_length == 0)
                {
//printf(" SKYMPEG_TS  residue_num=%d read_length=%d len=%d\n", residue_num, read_length, len);
                    return len;
                }
            }

            //[PES case]
            // raw aac stream:
            do {
                if (residue_num== 0) //if residue(avialable) byte = 0 when end of file --> exit and no more decode.
                    return len;

                //faac_sample_buffer = AACDecDecode(faac_hdec, &faac_finfo, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_len);
                //  bistream to sm_in
                sc->pdecod.paremeter1 = consumed_ptr; //send to server
                buflen = residue_num;//available ;
                sc->sctrl.finfo.bytesconsumed = 0;
//printf("== pre decode buflen=%d err:%d ", buflen, sc->sctrl.finfo.error);
#if 1
                int ret;
                ret = ipc_decode(sc, AAC_ID, buflen, id);
                if(!ret)
                    break;
                else if(ret < 0)
                    return -1;
#else
                if (!ipc_decode(sc, AAC_ID, buflen, id))
                    break;		      //IPC Error
#endif
                if (sc->sctrl.codec_id == AAC_ID) //decode success!
                    nsamples = sc->sctrl.data_len;
//printf("  bytesconsumed=%d id=%x %x ", sc->sctrl.finfo.bytesconsumed, sc->sctrl.codec_id, AAC_ID);
                consumed_ptr += sc->sctrl.finfo.bytesconsumed ;
                residue_num = fillin_ptr - consumed_ptr ;
//printf(" new res=%d\n", residue_num);

                /* update buffer index after AACDecDecode */
                //if( sc->sctrl.finfo.bytesconsumed  >= (unsigned int)sh->a_in_buffer_len)
/*
                if( sc->sctrl.finfo.bytesconsumed  >= residue_num) {
printf("!!!!! sc->sctrl.finfo.bytesconsumed=%d >= residue_num=%d sh->a_in_buffer_len=%d consumed_ptr=%d\n", sc->sctrl.finfo.bytesconsumed, residue_num, sh->a_in_buffer_len, consumed_ptr);
                    residue_num=0;    // to be end..
                    consumed_ptr=0;
                    sh->a_in_buffer_len = 0;
                    
                }
                else 
*/
                if (sc->sctrl.finfo.bytesconsumed > 0)
                {
                    sh->a_in_buffer_len -= sc->sctrl.finfo.bytesconsumed;
                    // copy residue data to start point

                    if(residue_num < use_aac_bufflen) {

                        if(fillin_ptr>use_aac_bufflen*(AAC_SM_BLOCK-1) )
                        {
                            memmove(sm_in,sm_in+consumed_ptr,residue_num); // copy residue data to start point
//printf(" --> reset consumed_ptr = 0;\n");
                            consumed_ptr = 0;
                            sh->a_in_buffer_len = residue_num;
                            fillin_ptr = residue_num;
                        }
                    }
                }




                if(sc->sctrl.finfo.error> 0) {
                    //sh->a_in_buffer_len = residue_num;
                    mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: error: %d, trying to resync! residue_num=%d\n", sc->sctrl.finfo.error, residue_num);
//printf("SAAAC: error: %d, trying to resync!\n", sc->sctrl.finfo.error);
//printf(" sh->a_in_buffer_len=%d sh->a_in_buffer=%x\n", sh->a_in_buffer_len, sh->a_in_buffer);
                    if ((sh->a_in_buffer_len <= 0)||ADIF_ON || (sh->a_in_buffer == NULL)) {      ////Honda : for ADIF
                        /* Carlos add check sh->a_in_buffer avoid call aac_sync caused mplayer crash, 2010-11-10 */
                        errors = MAX_ERRORS;
                        if (ADIF_ON) {
                            sh->a_in_buffer_len = 0;
                            aac_demux_read_data(sh, sh->ds,sm_in, sh->a_in_buffer_size);
                            sh->ds->eof = 1;
                        }
                        break;
                    }

//                    sh->a_in_buffer_len--;
                    if (consumed_ptr > 0 && residue_num > 0)
                    {
                        memmove(sm_in,sm_in+consumed_ptr, residue_num);
                    }
                    consumed_ptr = 0;
                    if (residue_num > 0)
                    {
                        residue_num--;
                        sh->a_in_buffer_len = residue_num;
                        fillin_ptr = residue_num;
                    
                        memmove(sm_in,sm_in+1,sh->a_in_buffer_len);
                    }
                    else
                    {
                        sh->a_in_buffer_len = residue_num;
                        fillin_ptr = residue_num;
                    }

                    if (sc->sctrl.finfo.error >= MAX_ERRORS)
                    {
                        sc->sctrl.finfo.error = 0;
//                        rel_seek_secs += 0.1;
//                        len = -1;
                                                
//                        break;
                    }
#ifdef SAAAC_DECODE_PTS
		    printf("%s:%d AAC resync stream\n", __func__, __LINE__);
		    aac_resync_stream(sh);
		    break;
#else
                    aac_sync(sh);
                    errors++;
#endif
                } else
                    break;
            } while(errors < MAX_ERRORS);
        }
        else {
            //[MP4 case]
            // packetized (.mp4) aac stream:
            buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
            if(buflen<=0)
                break;
            if (pts != MP_NOPTS_VALUE) {
                sh->pts = pts;
                sh->pts_bytes = 0;
            }

            sc->pdecod.paremeter1 = 0; //send to server
            memcpy(sm_in,bufptr, buflen); //  bistream  to sm_in
#if 1
	    int ret;
	    ret = ipc_decode(sc, AAC_ID, buflen, id);
	    if(!ret)
		    break;
	    else if(ret < 0)
		    return -1;
#else
	    if (!ipc_decode(sc, AAC_ID, buflen, id))
		    break;		//IPC Error
#endif
            if (sc->sctrl.codec_id == AAC_ID) { //decode success!
                nsamples = sc->sctrl.data_len;     //get from server


            }


        }

#ifdef  EngineMode  //驗證用only
        sh->pcm32.data =  PCM32_OUT.data ;
        sh->pcm32.ch = PCM32_OUT.ch ;
        sh->pcm32.len = PCM32_OUT.len ;
#endif

#if 1
	//20111117 charleslin, reinit audio when aac audio can't be recovered
	if (errors >= MAX_ERRORS)
	{
		extern unsigned int aserver_crash;
		printf("%s:%d errors:%d, need to reinit audio server\n", __func__, __LINE__, errors);
		aserver_crash = 1;
	}
#endif

        if(sc->sctrl.finfo.error > 0) {  //get from server
            mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: Failed to decode frame: %d  len=%d\n",
                   sc->sctrl.finfo.error, len);
            len = -1;
            //Barry 2010-11-19
            if (errors >= MAX_ERRORS)
            {
                sh->ds->eof = 1;
                return -1;
            }
        }
        else if (nsamples == 0) {    //不輸出
            mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: Decoded zero samples!\n");
        }
        else {   //audio out
            /* XXX: samples already multiplied by channels! */
            mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: Successfully decoded frame (%ld Bytes)!\n",sh->samplesize*nsamples);
            memcpy(buf+len,sm_out, sh->samplesize*nsamples); // sm_out -->  buf(synthesis output buffer)
            last_dec_len = sh->samplesize*nsamples;
            len += last_dec_len;
#ifdef SAAAC_DECODE_PTS
	    if(!sh->codecdata_len){ //[PES]
		if(startpts == MP_NOPTS_VALUE){
			printf("%s:%d startpts is MP_NOPTS_VALUE\n", __func__, __LINE__);
		}else{
			decsum += last_dec_len;
			sh->pts = startpts + (double)decsum / (double)sh->o_bps;
			//printf("decsum:%llu o_bps:%d sh_pts:%f\n", decsum, sh->o_bps, sh->pts);
		}
	    }else //[MP4]
#endif
            sh->pts_bytes += last_dec_len;
        }



    }

    return len;
}



//--------------------------------------------------------------------------

#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    // union smc *sc = (union smc *)sm_com;
    switch(cmd)
    {
    case ADCTRL_RESYNC_STREAM:
        // ipc_ctrl(sc, AAC_ID, 1, id);
//printf("!!!!! hay in control aac_sync\n");
        aac_resync_stream(sh);
        return CONTROL_TRUE;
#if 0
    case ADCTRL_SKIP_FRAME:
        return CONTROL_TRUE;
#endif
    }
    return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...) {

    return 0;
}

ad_functions_t mp_skaac = {
    preinit,
    init,
    uninit,
    control,
    decode_audio
};

#endif


#else //===============

#define AAC_BUFFLEN (768*6)



#ifdef  EngineMode  //驗證用only
extern  PCM32S PCM32_OUT ;
#endif



static int ADIF_ON;     //Honda : for ADIF
static int AAC_MODE;    //0: mp4, 1: LATM, 2: ADIF, 3: ADTS
extern int audio_output_channels;



static unsigned int fillin_ptr = 0;
static unsigned int consumed_ptr = 0 ;
static unsigned int residue_num = 0 ;
#define AAC_SM_BLOCK      3
/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory ,set semaphore,wakeup audio server
//
//------------------------------------------------------------------------------------------------------
*/
static int preinit(sh_audio_t *sh)
{
    aserver_pid = 0;
#ifndef  EngineMode
    union smc *sc;
    long curr_codec;
#endif

    {
        sh->audio_out_minsize=8192*AAC_MAX_CHANNELS;
        sh->audio_in_minsize=0; //not used
        fillin_ptr = consumed_ptr = fillin_ptr = 0 ;//initialize it everytime
    }

#ifndef  EngineMode
#ifdef AD_SHM_REWRITE
	sm_com_id = get_first_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);
	if(sm_com_id < 0)
		sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if(sm_com == NULL)
	{
		if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command )
			return 0;
	}
#else
	sm_com_id = get_shared_mem_id(KEY_SM_COM, SM_COM_SIZE);  //get shared memory ID
	if ((sm_com = allocate_share_mem(sm_com_id))==NULL) 	   //allocate shared memory ( command )
		return 0;
#endif

    id = get_semaphore_id();                                 //get semaphore ID

    if (!reset_semvalue(id, 0)) {                            //reset semaphore
        return 0;
    }

    sc = (union smc *)sm_com;

    sc->magic.magic_num = PLAYER_MAGIC;
    curr_codec = AAC_ID;                                     //inform server what kind of coded to be used.

    sc->magic.codec_id = curr_codec;
    if (!semaphore_v(id, 1))                                 //wakeup server!
        return 0;

    aserver_pid = sc->pinit.pid;
    if (!wait_server(sc)) {                                   //wait for server's response
        deallocate_share_mem(sm_com);
#ifndef AD_SHM_REWRITE
        shmctl(sm_com_id, IPC_RMID, 0);
#endif
        del_semvalue(id, 0);
        printf("Audio server crash \n");
        return 0;
    }

    if (sc->magic.codec_id != curr_codec) {                  //check codec
        printf("Unsuppoted codec\n");
        return 0;
    }


#endif

    aserver_pid = sc->pinit.pid;

    return 1;
}

static int aac_probe(unsigned char *buffer, int len, int mode)
{
    int i = 0, pos = 0;
    mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: %d bytes\n", len);
    while(i <= len-4) {
#if 1
        int t, t2;
        if ((buffer[i] == 0x56) && (mode==0 || mode==1)) {
            if ((buffer[i+1]&0xE0)==0xE0) {
                t = ((buffer[i+1]&0x1F)<<8)+buffer[i+2];
                if ((i+4+t)<len) {
                    t2 = (buffer[i+3+t]<<8)+(buffer[i+4+t]);
                    if ((t2>>5)==0x2b7) {
                        pos = i;
                        break;
                    }
                }
            }
        }
        if ((buffer[i] == 0xff) && (mode==0 || mode==3)) {
            if ((buffer[i+1] & 0xf6) == 0xf0) {
                t = ((buffer[i+3]&0x3)<<11)+(buffer[i+4]<<3)+(buffer[i+5]>>5);
                if ((i+1+t)<len) {
                    t2 = (buffer[i+t]<<8)+(buffer[i+1+t]);
                    if ((t2&0xfff6)==0xfff0) {
                        pos = i;
                        break;
                    }
                }
            }
        }
        if ((mode==0 || mode==2) && (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')) {
            pos = i;
            break;
        }
#else
if(
    ((buffer[i] == 0xff) && ((buffer[i+1] & 0xf6) == 0xf0)) ||
    (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')
) {
    pos = i;
    break;
}
#endif

        i++;
    }
    mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: ret %d\n", pos);
    return pos;
}





int AAC_frame_init(sh_audio_t *sh, aduio_info *ai) {



    int init = 1 , ret;
    int buflen ;
    union smc *sc = (union smc *)sm_com;
    play_init *pi = &sc->pinit;
    serv_init *si = &sc->sinit;



    //send to server
    buflen = sh->codecdata_len;
    if(!buflen)
    {
        buflen =sh->a_in_buffer_len  ;
        //memcpy(sm_in,sh->a_in_buffer,   sh->a_in_buffer_len); // pes bistream  to sm_in
    }
    else
        memcpy(sm_in,sh->codecdata,  buflen);   //MP4 bistream  to sm_in

    //some parameters to server for ini
    pi->AACinfo.sh_codecdata_len =  sh->codecdata_len;
    pi->AACinfo.sh_samplerate = sh->samplerate ;
    pi->AACinfo.sh_samplesize = sh->samplesize ;
    pi->paremeter1 = init;
    pi->paremeter3 = audio_output_channels ;           //send channel number to server

    if(!ipc_init(sc, AAC_ID, buflen, id, sm_data_id, 0)) //init server
        return 0;//break;		//IPC Error
    if (si->codec_id == AAC_ID)
        ret = 0;
    else
        ret = si->codec_id;   //error code

    init = 0;

    *ai = si->ai;
#ifndef  EngineMode
	if(sm_out == NULL || sm_out == (void *)-1)
		sm_out = (void*)shmat(sc->sinit.shmid, (void *)0, 0);		//只有initial成功才會allocate sm_out
#endif

    return ret; // OK!!!


}



/*
//------------------------------------------------------------------------------------------------------
//     Allocate shraed memory,initialize audio server
//
//------------------------------------------------------------------------------------------------------
*/
static int init(sh_audio_t *sh)
{

    int aac_init, pos = 0;
    int output_channels;
    int ret_init=0;

    //for audio server
    aduio_info gai;
    aduio_info *ai = &gai;
#ifndef  EngineMode
#ifdef AD_SHM_REWRITE
	sm_data_id = get_first_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_data_id < 0)
		sm_data_id = get_shared_mem_id(KEY_SM_IN, SM_DATA_IN);
	if(sm_in == NULL)
	{
		if ((sm_in = allocate_share_mem(sm_data_id))==NULL) 
		{
			printf("data share memory allocate fail\n");
			return 0;
		}
	}
#else
	sm_data_id = get_shared_mem_id(KEY_SM_IN, AAC_BUFFLEN*AAC_SM_BLOCK);//expansion 3
	if ((sm_in = allocate_share_mem(sm_data_id))==NULL) 
	{
		printf("data share memory allocate fail\n");
		return 0;
	}
#endif
#endif




#ifdef   AAC_2CH_ONLY
    output_channels = 2;
#else
output_channels = audio_output_channels;
#endif
    AAC_MODE = 0;
    // If we don't get the ES descriptor, try manual config
    //player
    if(!sh->codecdata_len && sh->wf) {
        sh->codecdata_len = sh->wf->cbSize;
//      sh->codecdata = (unsigned char*)(sh->wf+1);		//Barry 2010-05-20 disable	Fuchun 2010.04.20 enable
        sh->codecdata = malloc(sh->codecdata_len);		//Barry 2010-05-20 enable	Fuchun 2010.04.20 disable
        memcpy(sh->codecdata, sh->wf+1, sh->codecdata_len);
        mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: codecdata extracted from WAVEFORMATEX\n");
    }


    //player
    if(!sh->codecdata_len)
    {

        ///sh->a_in_buffer_len = demux_read_data(sh->ds, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_size);
        sh->a_in_buffer_len = demux_read_data(sh->ds, sm_in, AAC_BUFFLEN);
        pos = aac_probe( sm_in, sh->a_in_buffer_len, AAC_MODE);
        if(pos) {
            sh->a_in_buffer_len -= pos;
            memmove( sm_in,  sm_in +pos, sh->a_in_buffer_len);
            sh->a_in_buffer_len +=
                demux_read_data(sh->ds, (sm_in + sh->a_in_buffer_len), sh->a_in_buffer_size - sh->a_in_buffer_len);
            pos = 0;
        }
        //ini
        fillin_ptr = sh->a_in_buffer_len ;
        residue_num = fillin_ptr - consumed_ptr ;

    }


    if(!sh->samplesize) sh->samplesize=2;


    ret_init = AAC_frame_init(sh,ai) ;  //ai:audio info


    sh->i_bps = ai->sh_i_bps/8 ; //from server
    AAC_MODE = ai->AAC_MODE ;  //from server
    aac_init = ai->skaac_ini ;


//player
    if(!sh->codecdata_len ) {

        sh->a_in_buffer_len -= (aac_init > 0)?aac_init:0; // how many bytes init consumed
        if (aac_init>0) {                 //Honda : for ADIF
            memmove(sm_in,sm_in+aac_init,sh->a_in_buffer_len);
            ADIF_ON = 1;
            fillin_ptr = sh->a_in_buffer_len ;
            residue_num =  sh->a_in_buffer_len ;
        }

    }




    if(aac_init < 0|| ret_init!=0) {
        mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: Failed to initialize the decoder!\n"); // XXX: deal with cleanup!

        // XXX: free a_in_buffer here or in uninit?
        return 0;
    }
    else {

        sh->channels = ai->ch;;//get from server;
        sh->samplerate = ai->samplerate;//get from server ;
        sh->samplesize=2;
        if(!sh->i_bps) {
#if 0	//Barry 2010-11-10	avoid a/v async
            mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: compressed input bitrate missing, assuming 0 kbit/s!\n");
            sh->i_bps = 0; // XXX: HACK!!! ::atmos
#else
mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: compressed input bitrate missing, assuming 128kbit/s!\n");
sh->i_bps = 128*1000/8; // XXX: HACK!!! ::atmos
#endif
        } else
            mp_msg(MSGT_DECAUDIO,MSGL_V,"SAAAC: got %dkbit/s bitrate from MP4 header!\n",sh->i_bps*8/1000);
        mp_msg(MSGT_DECAUDIO,MSGL_V,"USING %s\n", "SKYVIIA HEAAC V2 LIBRARY");
    }


    return 1;
}







/*
//-----------------------------------------------------------------------------------------
//    free AAC decoder
//
//-----------------------------------------------------------------------------------------
*/
static void uninit(sh_audio_t *sh)
{
    union smc *sc = (union smc *)sm_com;
    ipc_uninit(sc, AAC_ID, id);
    //server
#ifndef  EngineMode
    deallocate_share_mem(sm_in);
    deallocate_share_mem(sm_out);
#ifndef AD_SHM_REWRITE
    shmctl(sm_data_id, IPC_RMID, 0);
#endif
    deallocate_share_mem(sm_com);
#endif
}


/*
//-----------------------------------------------------------------------------------------
//    bistream sync
//
//-----------------------------------------------------------------------------------------
*/
static int aac_sync(sh_audio_t *sh)
{
    int pos = 0;
    int read_length ;

    if(!sh->codecdata_len) {
        //ADTS/LATM
        residue_num = fillin_ptr = sh->a_in_buffer_len ;
        memcpy(sm_in,sh->a_in_buffer, sh->a_in_buffer_len);

        if(residue_num < AAC_BUFFLEN) { //若小於guard ,就再read one blcok .

            if( fillin_ptr>AAC_BUFFLEN*(AAC_SM_BLOCK-1) )
            {
                read_length = demux_read_data(sh->ds,(sm_in + fillin_ptr), AAC_BUFFLEN);
                fillin_ptr = residue_num + read_length ;
                //consumed_ptr = 0 ;

            } else {
                //read one block defaultly (maybe read_length<=AAC_BUFFLEN)
                read_length = demux_read_data(sh->ds,sm_in+fillin_ptr, AAC_BUFFLEN);
                fillin_ptr+=read_length;

            }
            sh->a_in_buffer_len = fillin_ptr;
        }


        pos = aac_probe(sm_in, sh->a_in_buffer_len, AAC_MODE); //find aac mode,and raw data position
        if(pos) {
            sh->a_in_buffer_len -= pos;
            memmove(sm_in,( sm_in+pos), sh->a_in_buffer_len);   //move to sm_in
            mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC SYNC AFTER %d bytes\n", pos);
        }

        consumed_ptr = 0 ;
        fillin_ptr = sh->a_in_buffer_len ;
    }



    return pos;
}




/*
//------------------------------------------------------------------------------------------------------
//     AAC decoder
//
//------------------------------------------------------------------------------------------------------
*/

#define MAX_ERRORS 10
static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
    int len = 0, last_dec_len = 1, errors = 0;
    int   nsamples,buflen;
    union smc *sc = (union smc *)sm_com;     //  shared memory
    unsigned char* bufptr=NULL;
    double pts;
    int read_length = 0;


    while(len < minlen && last_dec_len > 0 && errors < MAX_ERRORS) {

        //[PES]
        /* update buffer for raw aac streams: */
        if(!sh->codecdata_len)


            if(residue_num < AAC_BUFFLEN) { //若小於guard ,就再read one blcok .

                if( fillin_ptr>AAC_BUFFLEN*(AAC_SM_BLOCK-1) )
                {

                    read_length = demux_read_data(sh->ds,(sm_in + residue_num), AAC_BUFFLEN);
                    fillin_ptr = residue_num + read_length ;

                    consumed_ptr = 0 ;
                    sh->a_in_buffer_len = fillin_ptr;
                } 
                else {
                    //read one block defaultly (maybe read_length<=AAC_BUFFLEN)
                    read_length = demux_read_data(sh->ds,sm_in+fillin_ptr, AAC_BUFFLEN);
                    fillin_ptr+=read_length;
                    sh->a_in_buffer_len = fillin_ptr;
                }


            }


        if(!sh->codecdata_len) {
            residue_num = fillin_ptr - consumed_ptr ;
            //[PES case]
            // raw aac stream:
            do {
                if (residue_num== 0) //若檔案結束時 residue(avialable) byte =0 -->就離開 ,不再decode.
                    return len;

                //faac_sample_buffer = AACDecDecode(faac_hdec, &faac_finfo, (unsigned char*)sh->a_in_buffer, sh->a_in_buffer_len);
                //  bistream to sm_in
                sc->pdecod.paremeter1 = consumed_ptr; //send to server
                buflen = residue_num;//available ;
#if 1
                int ret;
                ret = ipc_decode(sc, AAC_ID, buflen, id);
                if(!ret)
                    break;
                else if(ret < 0)
                    return -1;
#else
if (!ipc_decode(sc, AAC_ID, buflen, id))
    break;		      //IPC Error
#endif
                if (sc->sctrl.codec_id == AAC_ID) //decode success!
                    nsamples = sc->sctrl.data_len;

                consumed_ptr += sc->sctrl.finfo.bytesconsumed ;
                residue_num = fillin_ptr - consumed_ptr ;


                /* update buffer index after AACDecDecode */
                //if( sc->sctrl.finfo.bytesconsumed  >= (unsigned int)sh->a_in_buffer_len)
                if( sc->sctrl.finfo.bytesconsumed  >= residue_num) {
                    residue_num=0;    // to be end..
                }
                else {
                    sh->a_in_buffer_len -= sc->sctrl.finfo.bytesconsumed;
                    // copy residue data to start point

                    if(residue_num < AAC_BUFFLEN) {

                        if(fillin_ptr>AAC_BUFFLEN*(AAC_SM_BLOCK-1) )
                            memmove(sm_in,sm_in+consumed_ptr,residue_num); // copy residue data to start point

                    }

                }




                if(sc->sctrl.finfo.error> 0) {
                    sh->a_in_buffer_len = residue_num;
                    mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: error: %d, trying to resync!\n", sc->sctrl.finfo.error);
                    if ((sh->a_in_buffer_len <= 0)||ADIF_ON || (sh->a_in_buffer == NULL)) {      ////Honda : for ADIF
                        /* Carlos add check sh->a_in_buffer avoid call aac_sync caused mplayer crash, 2010-11-10 */
                        errors = MAX_ERRORS;
                        if (ADIF_ON) {
                            sh->a_in_buffer_len = 0;
                            demux_read_data(sh->ds,sm_in, sh->a_in_buffer_size);
                            sh->ds->eof = 1;
                        }
                        break;
                    }
                    sh->a_in_buffer_len--;
                    memmove(sm_in,sm_in+1,sh->a_in_buffer_len);
                    aac_sync(sh);
                    errors++;
                } else
                    break;
            } while(errors < MAX_ERRORS);
        }
        else {
            //[MP4 case]
            // packetized (.mp4) aac stream:
            buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
            if(buflen<=0)
                break;
            if (pts != MP_NOPTS_VALUE) {
                sh->pts = pts;
                sh->pts_bytes = 0;
            }

            sc->pdecod.paremeter1 = 0; //send to server
            memcpy(sm_in,bufptr, buflen); //  bistream  to sm_in
#if 1
	    int ret;
	    ret = ipc_decode(sc, AAC_ID, buflen, id);
	    if(!ret)
		    break;
	    else if(ret < 0)
		    return -1;
#else
	    if (!ipc_decode(sc, AAC_ID, buflen, id))
		    break;		//IPC Error
#endif
            if (sc->sctrl.codec_id == AAC_ID) { //decode success!
                nsamples = sc->sctrl.data_len;     //get from server


            }


        }




#ifdef  EngineMode  //驗證用only
        sh->pcm32.data =  PCM32_OUT.data ;
        sh->pcm32.ch = PCM32_OUT.ch ;
        sh->pcm32.len = PCM32_OUT.len ;
#endif


        if(sc->sctrl.finfo.error > 0) {  //get from server
            mp_msg(MSGT_DECAUDIO,MSGL_WARN,"SAAAC: Failed to decode frame: %d \n",
                   sc->sctrl.finfo.error);
            //Barry 2010-11-19
            if (errors >= MAX_ERRORS)
            {
                sh->ds->eof = 1;
                return -1;
            }
        }
        else if (nsamples == 0) {    //不輸出
            mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: Decoded zero samples!\n");
        }
        else {   //audio out
            /* XXX: samples already multiplied by channels! */
            mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"SAAAC: Successfully decoded frame (%ld Bytes)!\n",sh->samplesize*nsamples);
            memcpy(buf+len,sm_out, sh->samplesize*nsamples); // sm_out -->  buf(synthesis output buffer)
            last_dec_len = sh->samplesize*nsamples;
            len += last_dec_len;
            sh->pts_bytes += last_dec_len;
        }



    }

    return len;
}



//--------------------------------------------------------------------------

#ifndef  EngineMode
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    // union smc *sc = (union smc *)sm_com;
    switch(cmd)
    {
    case ADCTRL_RESYNC_STREAM:
        // ipc_ctrl(sc, AAC_ID, 1, id);
        aac_sync(sh);
        return CONTROL_TRUE;
#if 0
    case ADCTRL_SKIP_FRAME:
        return CONTROL_TRUE;
#endif
    }
    return CONTROL_UNKNOWN;
}
#else
static int control(sh_audio_t *sh,int cmd,void* arg, ...) {

    return 0;
}

ad_functions_t mp_skaac = {
    preinit,
    init,
    uninit,
    control,
    decode_audio
};

#endif

#endif //== old AAC METHOD
