/*
 * routines (with C-linkage) that interface between MPlayer
 * and the "LIVE555 Streaming Media" libraries
 *
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

extern "C" {
// on MinGW, we must include windows.h before the things it conflicts
#ifdef __MINGW32__    // with.  they are each protected from
#include <windows.h>  // windows.h, but not the other way around.
#endif
#include <sys/time.h>

//#include "demuxer.h"
#include "demux_rtp.h"
#include "stheader.h"
#include "../libavutil/base64.h"
    int eof_retry_cnt=0;
    int check_is_asf = 0;
	int demux_open_idx=0;
	extern int network_timeout;
}
#include "demux_rtp_internal.h"

#include "BasicUsageEnvironment.hh"
#include "UsageEnvironment.hh"
#include "liveMedia.hh"
#include "GroupsockHelper.hh"
#include <unistd.h>

// A data structure representing input data for each stream:
class ReadBufferQueue {
public:
    ReadBufferQueue(MediaSubsession* subsession, demuxer_t* demuxer,
                    char const* tag);
    virtual ~ReadBufferQueue();

    FramedSource* readSource() const {
        return fReadSource;
    }
    RTPSource* rtpSource() const {
        return fRTPSource;
    }
    demuxer_t* ourDemuxer() const {
        return fOurDemuxer;
    }
    char const* tag() const {
        return fTag;
    }

    char blockingFlag; // used to implement synchronous reads

    // For A/V synchronization:
    Boolean prevPacketWasSynchronized;
    float prevPacketPTS;
    ReadBufferQueue** otherQueue;

    // The 'queue' actually consists of just a single "demux_packet_t"
    // (because the underlying OS does the actual queueing/buffering):
    demux_packet_t* dp;

    // However, we sometimes inspect buffers before delivering them.
    // For this, we maintain a queue of pending buffers:
    void savePendingBuffer(demux_packet_t* dp);
    demux_packet_t* getPendingBuffer();

    // For H264 over rtsp using AVParser, the next packet has to be saved
    demux_packet_t* nextpacket;

private:
    demux_packet_t* pendingDPHead;
    demux_packet_t* pendingDPTail;

    FramedSource* fReadSource;
    RTPSource* fRTPSource;
    demuxer_t* fOurDemuxer;
    char const* fTag; // used for debugging
};

// A structure of RTP-specific state, kept so that we can cleanly
// reclaim it:
struct RTPState {
    char const* sdpDescription;
    RTSPClient* rtspClient;
    SIPClient* sipClient;
    MediaSession* mediaSession;
    ReadBufferQueue* audioBufferQueue;
    ReadBufferQueue* videoBufferQueue;
    unsigned flags;
	unsigned sessionTimeout;
    struct timeval firstSyncTime;
};

extern "C" char* network_username;
extern "C" char* network_password;
static char* openURL_rtsp(RTSPClient* client, char const* url) {
    // If we were given a user name (and optional password), then use them:
    if (network_username != NULL) {
        char const* password = network_password == NULL ? "" : network_password;
        return client->describeWithPassword(url, network_username, password);
    } else {
        return client->describeURL(url);
    }
}

static char* openURL_sip(SIPClient* client, char const* url) {
    // If we were given a user name (and optional password), then use them:
    if (network_username != NULL) {
        char const* password = network_password == NULL ? "" : network_password;
        return client->inviteWithPassword(url, network_username, password);
    } else {
        return client->invite(url);
    }
}

#ifdef CONFIG_LIBNEMESI
extern int rtsp_transport_tcp;
extern int rtsp_transport_http;
#else
int rtsp_transport_tcp = 0;
int rtsp_transport_http = 0;
#endif

#ifdef NATIVE_RTSP
extern int rtsp_port;
#endif
#ifdef CONFIG_LIBAVCODEC
extern AVCodecContext *avcctx;
#endif

//extern "C" int av_base64_decode(uint8_t *out, const char *in, int out_size);

extern "C" int audio_id, video_id, dvdsub_id;

extern "C" int parseSDPLine(char const* inputLine,
                            char const*& nextLine) {
    // Begin by finding the start of the next line (if any):
    nextLine = NULL;
    for (char const* ptr = inputLine; *ptr != '\0'; ++ptr) {
        if (*ptr == '\r' || *ptr == '\n') {
            // We found the end of the line
            ++ptr;
            while (*ptr == '\r' || *ptr == '\n') ++ptr;
            nextLine = ptr;
            if (nextLine[0] == '\0') nextLine = NULL; // special case for end
            break;
        }
    }

    // Then, check that this line is a SDP line of the form <char>=<etc>
    // (However, we also accept blank lines in the input.)
    if (inputLine[0] == '\r' || inputLine[0] == '\n') return True;
    if (strlen(inputLine) < 2 || inputLine[1] != '='
            || inputLine[0] < 'a' || inputLine[0] > 'z') {
        return False;
    }

    return True;
}

extern char *fAsfhdr_data;
extern int fAsfhdr_size;
extern int fAsfhdr_packetsize;

extern "C" int parseSDPAttribute_pgmpu(char const* sdpLine) {
    char *pgmpu_str = strDupSize(sdpLine); // ensures we have enough space
    char *found_pgmpu_ptr = 0;
//  char *fAsfhdr_data = 0;
//  int fAsfhdr_size = 0;
    char const* nextSDPLine;
//  found_pgmpu_ptr = strstr(sdpLine, "a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;");

//printf("in parseSDPAttribute_pgmpu, found_pgmpu_ptr=%p\n", found_pgmpu_ptr);
    if (sscanf(sdpLine, "a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;base64,%s", pgmpu_str) == 1)
    {
        int ret=0, out_size=0;
//    unsigned asfhdr_size;
//printf("pgmpu_str len=%d\n", strlen(pgmpu_str));
//printf("pgmpu_str:%s!\n", pgmpu_str);
        if (fAsfhdr_data == 0)
        {
            fAsfhdr_data = (char *)malloc(strlen(pgmpu_str));
        }
//      fAsfhdr_data = av_base64_decode(asfhdr, pgmpu_str, fAsfhdr_size);
        ret = av_base64_decode((uint8_t *)fAsfhdr_data, pgmpu_str, strlen(pgmpu_str));
//       fAsfhdr_data = base64Decode(pgmpu_str, fAsfhdr_size);
        fAsfhdr_size = ret;
        printf("##~!!! got pgmpu, fAsfhdr_size=%d ret=%d\n", fAsfhdr_size, ret);
        int i;
        for (i=0; i<fAsfhdr_size && i < 16; i++)
        {
            printf("%.2x ", (unsigned char)fAsfhdr_data[i]);
        }
        printf("\n!!!!!\n");

//      free(fAsfhdr_data);
        free(pgmpu_str);
        return 1;
    }
    else
    {
        free(pgmpu_str);
//    free(fAsfhdr_data);
        return 0;
    }
}


extern "C" demuxer_t* demux_open_rtp(demuxer_t* demuxer) {
    int use_audio_ds = 0;
    Boolean success = False;
//  int check_is_asf = 0;
    check_is_asf = 0;
	demux_open_idx = 0;

    //printf("== demux_open_rtp  demuxer->desc->name=%s\n", demuxer->desc->name);
    do {
        //printf("dbg 01\n");
        TaskScheduler* scheduler = BasicTaskScheduler::createNew();
        //printf("dbg 02\n");
        if (scheduler == NULL) break;
        UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
        if (env == NULL) break;
        //printf("dbg 03\n");

        RTSPClient* rtspClient = NULL;
        SIPClient* sipClient = NULL;

        if (demuxer == NULL || demuxer->stream == NULL) break;  // shouldn't happen
        demuxer->stream->eof = 0; // just in case

        // Look at the stream's 'priv' field to see if we were initiated
        // via a SDP description:
        char* sdpDescription = (char*)(demuxer->stream->priv);
        if (sdpDescription == NULL) {
            // We weren't given a SDP description directly, so assume that
            // we were given a RTSP or SIP URL:
//            char const* protocol = demuxer->stream->streaming_ctrl->url->protocol;
//            char const* url = demuxer->stream->streaming_ctrl->url->url;
//			char *ptr = NULL;
            const char* protocol = demuxer->stream->streaming_ctrl->url->protocol;
            const char* url = demuxer->stream->streaming_ctrl->url->url;
            const char *ptr = NULL;
            extern int verbose;
            //printf("dbg 04  protocol=%s\n", protocol);

            if (strcmp(protocol, "rtsp") == 0 || strcmp(protocol, "mms") == 0) {
                if (rtsp_transport_http == 1) {
                    rtsp_transport_http = demuxer->stream->streaming_ctrl->url->port;
                    rtsp_transport_tcp = 1;
                }
                rtspClient = RTSPClient::createNew(*env, verbose, "MPlayer", rtsp_transport_http);
                //printf("dbg 05\n");

                if (rtspClient == NULL) {
                    fprintf(stderr, "Failed to create RTSP client: %s\n",
                            env->getResultMsg());
                    break;
                }
#if 1 // 20110407 charleslin - try rtsp protocol for mms URL
				if(strcmp(protocol, "mms") == 0){
					char tmpurl[1024];
					ptr = strchr(url, ':');
					snprintf(tmpurl, 1024, "%s%s", "rtsp", ptr);
					printf("%s:%d trying ms-rtsp protocol: %s\n", __func__, __LINE__, tmpurl);
                	sdpDescription = openURL_rtsp(rtspClient, tmpurl);
				}else
#endif
                	sdpDescription = openURL_rtsp(rtspClient, url);

                //printf("dbg 06\n");

            } else { // SIP
                unsigned char desiredAudioType = 0; // PCMU (use 3 for GSM)
                sipClient = SIPClient::createNew(*env, desiredAudioType, NULL,
                                                 verbose, "MPlayer");
                if (sipClient == NULL) {
                    fprintf(stderr, "Failed to create SIP client: %s\n",
                            env->getResultMsg());
                    break;
                }
                sipClient->setClientStartPortNum(8000);
                sdpDescription = openURL_sip(sipClient, url);
            }

            if (sdpDescription == NULL) {
                fprintf(stderr, "Failed to get a SDP description from URL \"%s\": %s\n",
                        url, env->getResultMsg());
                break;
            }
        }

        // Now that we have a SDP description, create a MediaSession from it:
//printf("MP= !!! sdpDescription=%s!\n", sdpDescription);
        MediaSession* mediaSession = MediaSession::createNew(*env, sdpDescription);
        if (mediaSession == NULL) break;
        printf("MP== !!! af MediaSession::createNew()\n");

        // Create a 'RTPState' structure containing the state that we just created,
        // and store it in the demuxer's 'priv' field, for future reference:
        RTPState* rtpState = new RTPState;
        rtpState->sdpDescription = sdpDescription;
        rtpState->rtspClient = rtspClient;
        rtpState->sipClient = sipClient;
        rtpState->mediaSession = mediaSession;
        rtpState->audioBufferQueue = rtpState->videoBufferQueue = NULL;
        rtpState->flags = 0;
        rtpState->firstSyncTime.tv_sec = rtpState->firstSyncTime.tv_usec = 0;
		rtpState->sessionTimeout = 60;
        demuxer->priv = rtpState;
        //printf("!!!!! demuxer=%p demuxer->priv=rtpState=%p\n", demuxer, demuxer->priv);
        int audiofound = 0, videofound = 0;
        // Create RTP receivers (sources) for each subsession:
        MediaSubsessionIterator iter(*mediaSession);
        MediaSubsession* subsession;
        unsigned desiredReceiveBufferSize;
        while ((subsession = iter.next()) != NULL) {
            // Ignore any subsession that's not audio or video:
            printf("== subsession->mediumName()=%s!\n", subsession->mediumName());
            if (strcmp(subsession->mediumName(), "audio") == 0) {
                if (audiofound) {
                    fprintf(stderr, "Additional subsession \"audio/%s\" skipped\n", subsession->codecName());
                    continue;
                }
                desiredReceiveBufferSize = 100000;
            } else if (strcmp(subsession->mediumName(), "video") == 0) {
                if (videofound) {
                    fprintf(stderr, "Additional subsession \"video/%s\" skipped\n", subsession->codecName());
                    continue;
                }
                desiredReceiveBufferSize = 2000000;
            } else {
                continue;
            }
#ifdef NATIVE_RTSP
            if (rtsp_port)
                subsession->setClientPortNum (rtsp_port);
#endif
            int rtpInit=0;
            printf("!!!! subsession->codecName()=%s!!\n", subsession->codecName());
	    if ( !strcmp(subsession->codecName(), "X-ASF-PF") )
	    {
		    char const* nextSDPLine;
		    char const* sdpLine = sdpDescription;
		    while(1)
		    {
			    if (!parseSDPLine(sdpLine, nextSDPLine)) break;

			    if (sdpLine[0] == 'm') break;
			    sdpLine = nextSDPLine;
			    if (sdpLine == NULL) break; // there are no m= lines at all
			    if (parseSDPAttribute_pgmpu(nextSDPLine))
				    break;
		    }

		    rtsp_transport_tcp = 1;
		    //      rtsp_transport_http = 1;
		    check_is_asf = 1;
		    demux_open_idx = DEMUXER_TYPE_ASF;
		    rtpInit = subsession->initiate(0);
	    }	/* leochang +*/
	    else if ( !strcmp(subsession->codecName(), "MP2T") )
	    {
		    //printf("\nMP2T\n");
		    rtsp_transport_tcp = 1;
		    check_is_asf       = 2; // may delete this variable
		    demux_open_idx     = DEMUXER_TYPE_MPEG_TS;
		    rtpInit = subsession->initiate();
	    }
	    else
	    {
		    //set demux_open_idx to 0, then demuxer type will be auto detected
		    //demux_open_idx = DEMUXER_TYPE_UNKNOWN;
		    //demux_open_idx = 0;
		    rtsp_transport_tcp = 1;
		    rtpInit = subsession->initiate();
	    }

//      if (!subsession->initiate()) {
            if (!rtpInit) {
                fprintf(stderr, "Failed to initiate \"%s/%s\" RTP subsession: %s\n", subsession->mediumName(), subsession->codecName(), env->getResultMsg());
            } else {
                fprintf(stderr, "Initiated \"%s/%s\" RTP subsession on port %d\n", subsession->mediumName(), subsession->codecName(), subsession->clientPortNum());

                // Set the OS's socket receive buffer sufficiently large to avoid
                // incoming packets getting dropped between successive reads from this
                // subsession's demuxer.  Depending on the bitrate(s) that you expect,
                // you may wish to tweak the "desiredReceiveBufferSize" values above.
                int rtpSocketNum = subsession->rtpSource()->RTPgs()->socketNum();
                int receiveBufferSize
                = increaseReceiveBufferTo(*env, rtpSocketNum,
                                          desiredReceiveBufferSize);
                printf("receiveBufferSize=%d\n", receiveBufferSize);
                if (verbose > 0) {
                    fprintf(stderr, "Increased %s socket receive buffer to %d bytes \n",
                            subsession->mediumName(), receiveBufferSize);
                }

                if (rtspClient != NULL) {
                    // Issue a RTSP "SETUP" command on the chosen subsession:
                    if (!rtspClient->setupMediaSubsession(*subsession, False,
                                                          rtsp_transport_tcp)) break;
                    if (!strcmp(subsession->mediumName(), "audio")){
                        audiofound = 1;
						if(subsession->rtpChannelId == 0)
							use_audio_ds = 1;
                    }
                    if (!strcmp(subsession->mediumName(), "video")){
                        videofound = 1;
						if(subsession->rtpChannelId == 0)
							use_audio_ds = 0;
                    }
                }
            }
        }

        if (rtspClient != NULL) {
			unsigned to;
            // Issue a RTSP aggregate "PLAY" command on the whole session:
            if (!rtspClient->playMediaSession(*mediaSession)) break;

			to = rtpState->rtspClient->sessionTimeoutParameter();
			if(to > 0){
				rtpState->sessionTimeout = to;
				//printf("set session_timeout to %u\n", rtpState->sessionTimeout);
			}
        } else if (sipClient != NULL) {
            sipClient->sendACK(); // to start the stream flowing
        }

        // Now that the session is ready to be read, do additional
        // MPlayer codec-specific initialization on each subsession:
        iter.reset();
        while ((subsession = iter.next()) != NULL) {
            if (subsession->readSource() == NULL) continue; // not reading this

            unsigned flags = 0;
            if (strcmp(subsession->mediumName(), "audio") == 0) {
                rtpState->audioBufferQueue
                = new ReadBufferQueue(subsession, demuxer, "audio");
                rtpState->audioBufferQueue->otherQueue = &(rtpState->videoBufferQueue);
                rtpCodecInitialize_audio(demuxer, subsession, flags);
            } else if (strcmp(subsession->mediumName(), "video") == 0) {
                rtpState->videoBufferQueue
                = new ReadBufferQueue(subsession, demuxer, "video");
                rtpState->videoBufferQueue->otherQueue = &(rtpState->audioBufferQueue);
                rtpCodecInitialize_video(demuxer, subsession, flags);
            }
            rtpState->flags |= flags;
        }
        success = True;
    } while (0);
    if (!success) return NULL; // an error occurred

    // Hack: If audio and video are demuxed together on a single RTP stream,
    // then create a new "demuxer_t" structure to allow the higher-level
    // code to recognize this:
    //printf("== demux_open_rtp  demuxer->desc->name=%s\n", demuxer->desc->name);

    if (demux_is_multiplexed_rtp_stream(demuxer)) {
        stream_t* s = NULL;
#if 1 // 20110407 charleslin
        //printf("%s:%d use %s demux stream for reading asf stream data\n", __func__, __LINE__, use_audio_ds ? "audio" : "video");
        if(use_audio_ds)
            s = new_ds_stream(demuxer->audio);
        else
#endif
            s = new_ds_stream(demuxer->video);


		//demux_open_idx
		/*leochang 20110519*/
		demuxer_t* od = demux_open(s, demux_open_idx,																 
			audio_id, video_id, dvdsub_id, NULL);
		demuxer = new_demuxers_demuxer(od, od, od);
#if 0
        //printf("check_is_asf=%d!!\n", check_is_asf);
        if ( check_is_asf == 1 )
        {
            printf("DEMUXER_TYPE_ASF=%d! aid=%d vid=%d\n", DEMUXER_TYPE_ASF, audio_id, video_id);
            demuxer_t* od = demux_open(s, DEMUXER_TYPE_ASF,
                audio_id, video_id, dvdsub_id, NULL);
//printf("===> after demux_open!\n");
            demuxer = new_demuxers_demuxer(od, od, od);
//printf("===> after new_demuxers_demuxer!!\n");
        }/* leochang +*/
        else if(check_is_asf == 2 )
        {
			printf("check_is_asf == 2\n");
            demuxer_t* od = demux_open(s, DEMUXER_TYPE_MPEG_TS,
                audio_id, video_id, dvdsub_id, NULL);
            demuxer = new_demuxers_demuxer(od, od, od);
			printf("===> after new_demuxers_demuxer!!\n");
        }
        else
        {
            demuxer_t* od = demux_open(s, DEMUXER_TYPE_UNKNOWN,
            	audio_id, video_id, dvdsub_id, NULL);
            demuxer = new_demuxers_demuxer(od, od, od);
        }
#endif
				
    }

    return demuxer;
}

extern "C" int demux_is_mpeg_rtp_stream(demuxer_t* demuxer) {
    // Get the RTP state that was stored in the demuxer's 'priv' field:
    RTPState* rtpState = (RTPState*)(demuxer->priv);

    return (rtpState->flags&RTPSTATE_IS_MPEG12_VIDEO) != 0;
}

extern "C" int demux_is_multiplexed_rtp_stream(demuxer_t* demuxer) {
    // Get the RTP state that was stored in the demuxer's 'priv' field:
    RTPState* rtpState = (RTPState*)(demuxer->priv);

    return (rtpState->flags&RTPSTATE_IS_MULTIPLEXED) != 0;
}

static demux_packet_t* getBuffer(demuxer_t* demuxer, demux_stream_t* ds,
                                 Boolean mustGetNewData,
                                 float& ptsBehind); // forward

extern "C" int demux_rtp_fill_buffer(demuxer_t* demuxer, demux_stream_t* ds) {
    // Get a filled-in "demux_packet" from the RTP source, and deliver it.
    // Note that this is called as a synchronous read operation, so it needs
    // to block in the (hopefully infrequent) case where no packet is
    // immediately available.
//printf("==> in demux_rtp_fill_buffer()\n");
	//printf("%s:%d fill %s buffer\n", __func__, __LINE__, (ds == demuxer->audio) ? "audio" : "video");
#if 1 //FIXME: if video is paused, connection will be timedout because keepalive is stopped, use a timer instead
	if(check_is_asf == 1){ // 20111103 charleslin Server: PVSS response "500 Internal Server Error" when receiving GET_PARAMETER command
		static struct timeval tv = { 0, 0 };
		struct timeval curtv;
		RTPState* rtpState = (RTPState*)demuxer->priv;

		gettimeofday(&curtv, NULL);
		if(tv.tv_sec == 0 || curtv.tv_sec - tv.tv_sec >= rtpState->sessionTimeout){
			gettimeofday(&tv, NULL);
			printf("%s:%d send GET_PARAMETER command (keepalive)\n", __func__, __LINE__);
			rtpState->rtspClient->sendGetParameterCommand(*rtpState->mediaSession, NULL, NULL, NULL);
		}
	}
#else
	static int cnt = 500;
	if(--cnt == 0){
		cnt = 500;
		printf("%s:%d count down to zero, send GET_PARAMETER command\n", __func__, __LINE__);
		rtpState->rtspClient->sendGetParameterCommand(*rtpState->mediaSession, NULL, NULL, NULL);
	}
#endif
    while (1) {
        float ptsBehind;
        demux_packet_t* dp = getBuffer(demuxer, ds, False, ptsBehind); // blocking
//printf("==> in demux_rtp_fill_buffer  dp=%p\n", dp);
        if (dp == NULL) return 0;

        if (demuxer->stream->eof) return 0; // source stream has closed down

        // Before using this packet, check to make sure that its presentation
        // time is not far behind the other stream (if any).  If it is,
        // then we discard this packet, and get another instead.  (The rest of
        // MPlayer doesn't always do a good job of synchronizing when the
        // audio and video streams get this far apart.)
        // (We don't do this when streaming over TCP, because then the audio and
        // video streams are interleaved.)
        // (Also, if the stream is *excessively* far behind, then we allow
        // the packet, because in this case it probably means that there was
        // an error in the source's timestamp synchronization.)
        const float ptsBehindThreshold = 1.0; // seconds
        const float ptsBehindLimit = 60.0; // seconds
//printf("==> in demux_rtp_fill_buffer  ptsBehind=%f ptsBehindThreshold=%f ptsBehindLimit=%f rtsp_transport_tcp=%d\n", (float)ptsBehind, (float)ptsBehindThreshold, (float)ptsBehindLimit,  rtsp_transport_tcp);
        if (ptsBehind < ptsBehindThreshold ||
                ptsBehind > ptsBehindLimit ||
                rtsp_transport_tcp) { // packet's OK
//printf(" ==> bf ds_add_packet()\n");
            ds_add_packet(ds, dp);
            break;
        }

//#ifdef DEBUG_PRINT_DISCARDED_PACKETS
#if 1
        RTPState* rtpState = (RTPState*)(demuxer->priv);
        ReadBufferQueue* bufferQueue = ds == demuxer->video ? rtpState->videoBufferQueue : rtpState->audioBufferQueue;
        fprintf(stderr, "Discarding %s packet (%fs behind)\n", bufferQueue->tag(), ptsBehind);
#endif
        free_demux_packet(dp); // give back this packet, and get another one
    }

    return 1;
}

Boolean awaitRTPPacket(demuxer_t* demuxer, demux_stream_t* ds,
                       unsigned char*& packetData, unsigned& packetDataLen,
                       float& pts) {
    // Similar to "demux_rtp_fill_buffer()", except that the "demux_packet"
    // is not delivered to the "demux_stream".
    float ptsBehind;
printf("!!!! in awaitRTPPacket()\n");
    demux_packet_t* dp = getBuffer(demuxer, ds, True, ptsBehind); // blocking
    if (dp == NULL) return False;

    packetData = dp->buffer;
    packetDataLen = dp->len;
    pts = dp->pts;

    return True;
}

static void teardownRTSPorSIPSession(RTPState* rtpState); // forward

extern "C" void demux_close_rtp(demuxer_t* demuxer) {
    // Reclaim all RTP-related state:

    // Get the RTP state that was stored in the demuxer's 'priv' field:
    RTPState* rtpState = (RTPState*)(demuxer->priv);
    if (rtpState == NULL) return;

    teardownRTSPorSIPSession(rtpState);

    UsageEnvironment* env = NULL;
    TaskScheduler* scheduler = NULL;
    if (rtpState->mediaSession != NULL) {
        env = &(rtpState->mediaSession->envir());
        scheduler = &(env->taskScheduler());
    }
    Medium::close(rtpState->mediaSession);
    Medium::close(rtpState->rtspClient);
    Medium::close(rtpState->sipClient);
    delete rtpState->audioBufferQueue;
    delete rtpState->videoBufferQueue;
    delete[] rtpState->sdpDescription;
    delete rtpState;
#ifdef CONFIG_LIBAVCODEC
    av_freep(&avcctx);
#endif

    env->reclaim();
    delete scheduler;
}

////////// Extra routines that help implement the above interface functions:

//#define MAX_RTP_FRAME_SIZE 5000000
#define MAX_RTP_FRAME_SIZE (1024*256)
// >= the largest conceivable frame composed from one or more RTP packets

static void afterReading(void* clientData, unsigned frameSize,
                         unsigned /*numTruncatedBytes*/,
                         struct timeval presentationTime,
                         unsigned /*durationInMicroseconds*/) {
    int headersize = 0;
	//printf("%s:%d frameSize %u\n", __func__, __LINE__, frameSize);
    if (frameSize >= MAX_RTP_FRAME_SIZE) {
        fprintf(stderr, "Saw an input frame too large (>=%d).  Increase MAX_RTP_FRAME_SIZE in \"demux_rtp.cpp\".\n",
                MAX_RTP_FRAME_SIZE);
    }
    ReadBufferQueue* bufferQueue = (ReadBufferQueue*)clientData;
    demuxer_t* demuxer = bufferQueue->ourDemuxer();
    RTPState* rtpState = (RTPState*)(demuxer->priv);

    if (frameSize > 0) demuxer->stream->eof = 0;

    demux_packet_t* dp = bufferQueue->dp;

    if (bufferQueue->readSource()->isAMRAudioSource())
        headersize = 1;
    else if (bufferQueue == rtpState->videoBufferQueue &&
             ((sh_video_t*)demuxer->video->sh)->format == mmioFOURCC('H','2','6','4')) {
        dp->buffer[0]=0x00;
        dp->buffer[1]=0x00;
        dp->buffer[2]=0x01;
        headersize = 3;
    }
//printf("afterReading ==> frameSize=%d headersize=%d\n",frameSize, headersize);
    resize_demux_packet(dp, frameSize + headersize);

    // Set the packet's presentation time stamp, depending on whether or
    // not our RTP source's timestamps have been synchronized yet:
    Boolean hasBeenSynchronized
    = bufferQueue->rtpSource()->hasBeenSynchronizedUsingRTCP();
    if (hasBeenSynchronized) {
        if (verbose > 0 && !bufferQueue->prevPacketWasSynchronized) {
            fprintf(stderr, "%s stream has been synchronized using RTCP \n",
                    bufferQueue->tag());
        }

        struct timeval* fst = &(rtpState->firstSyncTime); // abbrev
        if (fst->tv_sec == 0 && fst->tv_usec == 0) {
            *fst = presentationTime;
        }

        // For the "pts" field, use the time differential from the first
        // synchronized time, rather than absolute time, in order to avoid
        // round-off errors when converting to a float:
        dp->pts = presentationTime.tv_sec - fst->tv_sec
                  + (presentationTime.tv_usec - fst->tv_usec)/1000000.0;
        bufferQueue->prevPacketPTS = dp->pts;
    } else {
        if (verbose > 0 && bufferQueue->prevPacketWasSynchronized) {
            fprintf(stderr, "%s stream is no longer RTCP-synchronized \n",
                    bufferQueue->tag());
        }

        // use the previous packet's "pts" once again:
        dp->pts = bufferQueue->prevPacketPTS;
    }
    bufferQueue->prevPacketWasSynchronized = hasBeenSynchronized;

    dp->pos = demuxer->filepos;
    demuxer->filepos += frameSize + headersize;

    // Signal any pending 'doEventLoop()' call on this queue:
    bufferQueue->blockingFlag = ~0;
}

static void onSourceClosure(void* clientData) {
    ReadBufferQueue* bufferQueue = (ReadBufferQueue*)clientData;
    demuxer_t* demuxer = bufferQueue->ourDemuxer();
#if 1
    demuxer->stream->eof = 1;
#else
    // 20111104 charleslin: using eof_retry_cnt has no help and will get signal 6 on retrying
    printf("MP== onSourceClosure ?? why?? eof_retry_cnt=%d\n", eof_retry_cnt);
    if (eof_retry_cnt == 0)
    {
        demuxer->stream->eof = 1;
    }
    else
    {
        eof_retry_cnt--;
    }
#endif
    // Signal any pending 'doEventLoop()' call on this queue:
    bufferQueue->blockingFlag = ~0;
}

#ifndef __MAX
#   define __MAX(a, b)   ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef __MIN
#   define __MIN(a, b)   ( ((a) < (b)) ? (a) : (b) )
#endif

static demux_packet_t* getBuffer(demuxer_t* demuxer, demux_stream_t* ds,
                                 Boolean mustGetNewData,
                                 float& ptsBehind) {
    // Begin by finding the buffer queue that we want to read from:
    // (Get this from the RTP state, which we stored in
    //  the demuxer's 'priv' field)
    RTPState* rtpState = (RTPState*)(demuxer->priv);
    ReadBufferQueue* bufferQueue = NULL;
    int headersize = 0;
    TaskToken task;
    eof_retry_cnt = 2;
FramedSource* fsrc = bufferQueue->readSource();
//printf("==> rtp getBuffer demuxer=%p demuxer->priv=%p\n", demuxer, demuxer->priv);
    if (demuxer->stream->eof) return NULL;

    if (ds == demuxer->video) {
        bufferQueue = rtpState->videoBufferQueue;
        if (((sh_video_t*)ds->sh)->format == mmioFOURCC('H','2','6','4'))
            headersize = 3;
    } else if (ds == demuxer->audio) {
        bufferQueue = rtpState->audioBufferQueue;
        if (bufferQueue->readSource()->isAMRAudioSource())
            headersize = 1;
    } else {
        fprintf(stderr, "(demux_rtp)getBuffer: internal error: unknown stream\n");
        return NULL;
    }

    if (bufferQueue == NULL || bufferQueue->readSource() == NULL) {
        fprintf(stderr, "(demux_rtp)getBuffer failed: no appropriate RTP subsession has been set up\n");
        return NULL;
    }

    demux_packet_t* dp = NULL;
    if (!mustGetNewData) {
        // Check whether we have a previously-saved buffer that we can use:
        dp = bufferQueue->getPendingBuffer();
//printf("== mustGetNewData=%d  bufferQueue->getPendingBuffer()=%p\n", dp);
        if (dp != NULL) {
            ptsBehind = 0.0; // so that we always accept this data
            return dp;
        }
    }

    // Allocate a new packet buffer, and arrange to read into it:
//printf("== dbg00 bufferQueue->nextpacket=%p\n", bufferQueue->nextpacket);
    if (!bufferQueue->nextpacket) {
        dp = new_demux_packet(MAX_RTP_FRAME_SIZE);
        bufferQueue->dp = dp;
        if (dp == NULL) return NULL;
    }

#ifdef CONFIG_LIBAVCODEC
    extern AVCodecParserContext * h264parserctx;
    int consumed, poutbuf_size = 1;
    const uint8_t *poutbuf = NULL;
    float lastpts = 0.0;
    char *psz_bye = NULL;
    char *psz_name = NULL;

//  bufferQueue->blockingFlag = 0;
//rtpState->rtspClient->getMediaSessionParameter(*rtpState->mediaSession, (char const *)NULL, psz_bye);
    do {
//printf("MP== bufferQueue->nextpacket=%p\n", bufferQueue->nextpacket);
        if (!bufferQueue->nextpacket) {
#endif
            // Schedule the read operation:
            bufferQueue->blockingFlag = 0;
			//fprintf(stderr, "%s:%d dp->len %d\n", __func__, __LINE__, dp->len);
            //printf("MP== bufferQueue->readSource()->getNextFrame headersize=%d\n", headersize);
            bufferQueue->readSource()->getNextFrame(&dp->buffer[headersize], MAX_RTP_FRAME_SIZE - headersize,
                                                    afterReading, bufferQueue,
                                                    onSourceClosure, bufferQueue);
            // Block ourselves until data becomes available:
            TaskScheduler& scheduler
            = bufferQueue->readSource()->envir().taskScheduler();
            int delay = network_timeout * 1000000; // in microseconds
            if (bufferQueue->prevPacketPTS * 1.05 > rtpState->mediaSession->playEndTime())
                delay /= 10;
            //printf("MP== delay=%d! bufferQueue->prevPacketPTS=%f playEndTIme=%f\n", delay, (float)bufferQueue->prevPacketPTS, (float)rtpState->mediaSession->playEndTime());
            task = scheduler.scheduleDelayedTask(delay, onSourceClosure, bufferQueue);
            scheduler.doEventLoop(&bufferQueue->blockingFlag);
            scheduler.unscheduleDelayedTask(task);

            //printf("MP== getNextFrame done\n");
			//fprintf(stderr, "%s:%d dp->len %d\n", __func__, __LINE__, dp->len);
//printf("!!bufferQueue->readSource()->fIsCurrentlyAwaitingData = %d\n", bufferQueue->readSource()->isCurrentlyAwaitingData());
if (demuxer->stream->eof)
    printf("MP== demuxer->stream->eof=%d!\n", demuxer->stream->eof);
            if (demuxer->stream->eof) {
                free_demux_packet(dp);
                return NULL;
            }

            if (headersize == 1) // amr
                dp->buffer[0] =
                    ((AMRAudioSource*)bufferQueue->readSource())->lastFrameHeader();
#ifdef CONFIG_LIBAVCODEC
        } else {
            bufferQueue->dp = dp = bufferQueue->nextpacket;
            bufferQueue->nextpacket = NULL;
//printf("!! bufferQueue->nextpacket=%p\n", bufferQueue->nextpacket);
        }
        if (headersize == 3 && h264parserctx) { // h264
            consumed = h264parserctx->parser->parser_parse(h264parserctx,
                       avcctx,
                       &poutbuf, &poutbuf_size,
                       dp->buffer, dp->len);

            if (!consumed && !poutbuf_size)
                return NULL;

            if (!poutbuf_size) {
                lastpts=dp->pts;
                free_demux_packet(dp);
                bufferQueue->dp = dp = new_demux_packet(MAX_RTP_FRAME_SIZE);
            } else {
                bufferQueue->nextpacket = dp;
                bufferQueue->dp = dp = new_demux_packet(poutbuf_size);
                memcpy(dp->buffer, poutbuf, poutbuf_size);
                dp->pts=lastpts;
            }
        }

        if (check_is_asf == 1 && bufferQueue->readSource()->isCurrentlyAwaitingData() == 0)
        {
            unsigned i_size;
            int b_marker = bufferQueue->rtpSource()->curPacketMarkerBit();
            const unsigned i_packet_size = dp->len;
            unsigned char *p_data = dp->buffer;
            unsigned i_header_size = 4;
            int packetSize = dp->len;
            i_size = packetSize;
//            printf("== start b_marker=%d.... dp->len=%d\n", b_marker, dp->len);
            int i;
/*
            for (i=0; i<64; i++)
            {
                printf("%.2x ", p_data[i]);
            }
*/
	//fprintf(stderr, "%s:%d dp->len %d\n", __func__, __LINE__, dp->len);
			//printf("packetSize %d\n", packetSize);
            while( i_size >= 4 )
            {
                unsigned i_flags = p_data[0];
                unsigned i_length_offset = (p_data[1] << 16) |
                                           (p_data[2] <<  8) |
                                           (p_data[3]      );
                bool b_key = i_flags & 0x80;
                bool b_length = i_flags & 0x40;
                bool b_relative_ts = i_flags & 0x20;
                bool b_duration = i_flags & 0x10;
                bool b_location_id = i_flags & 0x08;

				//printf("i_size %u\n", i_size);
                //printf(" .. i_flags = p_data[0]=0x%x\n", i_flags);
				//if(b_key) printf("    is key frame\n");
				//printf("    %s: %d\n", b_length ? "length" : "offset", i_length_offset);
                //printf("ASF: marker=%d size=%d : %c i_len_off=%d\n", b_marker, i_size, b_length ? 'L' : 'O', i_length_offset );
                //msg_Dbg( p_demux, "ASF: marker=%d size=%d : %c=%d id=%d",
                //         b_marker, i_size, b_length ? 'L' : 'O', i_length_offset );
                // unsigned i_header_size = 4;
                i_header_size = 4;
                if( b_relative_ts ){
					unsigned val = p_data[i_header_size + 0] << 24 | p_data[i_header_size + 1] << 16 | p_data[i_header_size + 2] << 8 | p_data[i_header_size + 3];
					//printf("    has relative timestamp: %u\n", val);
                    i_header_size += 4;
                }
                if( b_duration ){
					unsigned val = p_data[i_header_size + 0] << 24 | p_data[i_header_size + 1] << 16 | p_data[i_header_size + 2] << 8 | p_data[i_header_size + 3];
					//printf("    has duration: %u\n", val);
                    i_header_size += 4;
                }
                if( b_location_id ){
					unsigned val = p_data[i_header_size + 0] << 24 | p_data[i_header_size + 1] << 16 | p_data[i_header_size + 2] << 8 | p_data[i_header_size + 3];
					//printf("    has location id: %u\n", val);
                    i_header_size += 4;
                }
                if( i_header_size > i_size )
                {
                    //msg_Warn( p_demux, "Invalid header size\n" );
                    //printf("Invalid header size\n" );
                    break;
                }

                /* XXX
                 * When b_length is true, the streams I found do not seems to respect
                 * the documentation.
                 * From them, I have failed to find which choice between '__MIN()' or
                 * 'i_length_offset - i_header_size' is the right one.
                 */
                unsigned i_payload;
                if( b_length )
                    i_payload = __MIN( i_length_offset, i_size - i_header_size);
                else
                    i_payload = i_size - i_header_size;

//        if( !tk->p_asf_block )
                {
//            tk->p_asf_block = block_New( p_demux, i_packet_size );
//            if( !tk->p_asf_block )
//                break;
//            tk->p_asf_block->i_buffer = 0;
                }
                unsigned i_offset  = b_length ? 0 : i_length_offset;
//                printf("===> i_packet_size=%d i_offset=%d i_payload=%d \n", i_packet_size, i_offset, i_payload);
#if 0
                if( i_offset == tk->p_asf_block->i_buffer && i_offset + i_payload <= i_packet_size )
                {
                    memcpy( &tk->p_asf_block->p_buffer[i_offset], &p_data[i_header_size], i_payload );
                    tk->p_asf_block->i_buffer += i_payload;
                    if( b_marker )
                    {
                        /* We have a complete packet */
                        tk->p_asf_block->i_buffer = i_packet_size;
                        block_ChainAppend( &p_list, tk->p_asf_block );
                        tk->p_asf_block = NULL;
                    }
                }
                else
                {
                    /* Reset on broken stream */
                    printf( "Broken packet detected (%d vs %d or %d + %d vs %d)",
                            i_offset, tk->p_asf_block->i_buffer, i_offset, i_payload, i_packet_size);
                    tk->p_asf_block->i_buffer = 0;
                }
#endif
                /* */
                p_data += i_header_size + i_payload;
                i_size -= i_header_size + i_payload;
            }
#if 0            
            memmove(dp->buffer, &dp->buffer[4], dp->len-4);
            dp->len -= 4;
#else
            memmove(dp->buffer, &dp->buffer[i_header_size], dp->len-i_header_size);
            dp->len -= i_header_size;
#endif
            if (dp->len != fAsfhdr_packetsize)
            {
                //printf("\n============== dp->len = %d != %d\n", dp->len, fAsfhdr_packetsize);
                dp->len = fAsfhdr_packetsize;
            }
            /*
                  {
            static FILE *fn=NULL;
            if (fn == NULL)
              fn = fopen("/tmp/test3.asf", "w");
              fwrite(dp->buffer, dp->len, 1, fn);
                  }
            */
        }

//        printf("MP== poutbuf_size=%d dp->len=%d\n", poutbuf_size, dp->len);
    } while (!poutbuf_size);
#endif

    // Set the "ptsBehind" result parameter:
    if (bufferQueue->prevPacketPTS != 0.0
            && bufferQueue->prevPacketWasSynchronized
            && *(bufferQueue->otherQueue) != NULL
            && (*(bufferQueue->otherQueue))->prevPacketPTS != 0.0
            && (*(bufferQueue->otherQueue))->prevPacketWasSynchronized) {
        ptsBehind = (*(bufferQueue->otherQueue))->prevPacketPTS
                    - bufferQueue->prevPacketPTS;
    } else {
        ptsBehind = 0.0;
    }

    if (mustGetNewData) {
        // Save this buffer for future reads:
        bufferQueue->savePendingBuffer(dp);
    }

    return dp;
}

static void teardownRTSPorSIPSession(RTPState* rtpState) {
    MediaSession* mediaSession = rtpState->mediaSession;
    if (mediaSession == NULL) return;
    if (rtpState->rtspClient != NULL) {
        rtpState->rtspClient->teardownMediaSession(*mediaSession);
    } else if (rtpState->sipClient != NULL) {
        rtpState->sipClient->sendBYE();
    }
}

////////// "ReadBuffer" and "ReadBufferQueue" implementation:

ReadBufferQueue::ReadBufferQueue(MediaSubsession* subsession,
                                 demuxer_t* demuxer, char const* tag)
    : prevPacketWasSynchronized(False), prevPacketPTS(0.0), otherQueue(NULL),
    dp(NULL), nextpacket(NULL),
    pendingDPHead(NULL), pendingDPTail(NULL),
    fReadSource(subsession == NULL ? NULL : subsession->readSource()),
    fRTPSource(subsession == NULL ? NULL : subsession->rtpSource()),
    fOurDemuxer(demuxer), fTag(strdup(tag)) {
}

ReadBufferQueue::~ReadBufferQueue() {
    free((void *)fTag);

    // Free any pending buffers (that never got delivered):
    demux_packet_t* dp = pendingDPHead;
    while (dp != NULL) {
        demux_packet_t* dpNext = dp->next;
        dp->next = NULL;
        free_demux_packet(dp);
        dp = dpNext;
    }
}

void ReadBufferQueue::savePendingBuffer(demux_packet_t* dp) {
    // Keep this buffer around, until MPlayer asks for it later:
    if (pendingDPTail == NULL) {
        pendingDPHead = pendingDPTail = dp;
    } else {
        pendingDPTail->next = dp;
        pendingDPTail = dp;
    }
    dp->next = NULL;
}

demux_packet_t* ReadBufferQueue::getPendingBuffer() {
    demux_packet_t* dp = pendingDPHead;
    if (dp != NULL) {
        pendingDPHead = dp->next;
        if (pendingDPHead == NULL) pendingDPTail = NULL;

        dp->next = NULL;
    }

    return dp;
}

static int demux_rtp_control(struct demuxer *demuxer, int cmd, void *arg) {
    double endpts = ((RTPState*)demuxer->priv)->mediaSession->playEndTime();

    switch(cmd) {
    case DEMUXER_CTRL_GET_TIME_LENGTH:
        if (endpts <= 0)
            return DEMUXER_CTRL_DONTKNOW;
        *((double *)arg) = endpts;
        return DEMUXER_CTRL_OK;

    case DEMUXER_CTRL_GET_PERCENT_POS:
        if (endpts <= 0)
            return DEMUXER_CTRL_DONTKNOW;
        *((int *)arg) = (int)(((RTPState*)demuxer->priv)->videoBufferQueue->prevPacketPTS*100/endpts);
        return DEMUXER_CTRL_OK;

    default:
        return DEMUXER_CTRL_NOTIMPL;
    }
}

demuxer_desc_t demuxer_desc_rtp = {
    "LIVE555 RTP demuxer",
    "live555",
    "",
    "Ross Finlayson",
    "requires LIVE555 Streaming Media library",
    DEMUXER_TYPE_RTP,
    0, // no autodetect
    NULL,
    demux_rtp_fill_buffer,
    demux_open_rtp,
    demux_close_rtp,
    NULL,
    demux_rtp_control
};
