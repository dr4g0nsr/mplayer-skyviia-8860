#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "stream.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

#define USE_UDC		// use unix domain socket

#ifdef USE_UDC
#define SOCK_NAME	" skynet"	// compatible with java code
#else
#define SERV_HOST	"127.0.0.1"
#define SERV_PORT	8251
#endif

#define SKYNET_CLI_MAGIC	0xCAFE0000
#define SKYNET_SER_MAGIC	0xF00D0000

#define SKYNET_RESP_OK		(SKYNET_SER_MAGIC | 0x0)
#define SKYNET_RESP_ERROR	(SKYNET_SER_MAGIC | 0x1)
#define SKYNET_RESP_UNKNOWN	(SKYNET_SER_MAGIC | 0x2)

#define SKYNET_CMD_INIT		(SKYNET_CLI_MAGIC | 0x0)
#define SKYNET_CMD_GETDATA	(SKYNET_CLI_MAGIC | 0x1)
#define SKYNET_CMD_SETURL	(SKYNET_CLI_MAGIC | 0x2)
#define SKYNET_CMD_GETSIZE	(SKYNET_CLI_MAGIC | 0x3)
#define SKYNET_CMD_GETSEEKABLE  (SKYNET_CLI_MAGIC | 0x4)
#define SKYNET_CMD_SETPOSITION  (SKYNET_CLI_MAGIC | 0x5)
#define SKYNET_CMD_STARTSTREAM  (SKYNET_CLI_MAGIC | 0x6)
#define SKYNET_CMD_STOPSTREAM   (SKYNET_CLI_MAGIC | 0x7)
#define SKYNET_CMD_GETSTREAMTYPE	(SKYNET_CLI_MAGIC | 0x8)
#define SKYNET_CMD_SEEKTOTIME	(SKYNET_CLI_MAGIC | 0x9)

#define SKYNET_STAT_INIT	0
#define SKYNET_STAT_READY	2
#define SKYNET_STAT_STREAMING	3
#define SKYNET_STAT_UNKNOWN	4

#define SKYNET_CMD_LEN		512	//command
#define SKYNET_RESP_LEN		512	//response
#define SKYNET_DATA_LEN		(8*1024)

#define SKYNET_CACHE_SIZE_MIN	320	//320kbytes

enum {
	SKYNET_STREAM_RMVB,
	SKYNET_STREAM_WMV,
	SKYNET_STREAM_FLV
};

static struct stream_priv_s {
	char *hostname;
	char *filename;
} stream_priv_dflts = {
	0, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
	{"hostname", ST_OFF(hostname), CONF_TYPE_STRING, 0, 0 , 0, NULL},
	{"filename", ST_OFF(filename), CONF_TYPE_STRING, 0, 0 ,0, NULL},
	{ NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
	"bd",
	sizeof(struct stream_priv_s),
	&stream_priv_dflts,
	stream_opts_fields
};

struct skynet_data_t
{
	unsigned int state;
	int streamtype;
	int seekable;
	char *sendbuf;
	char *recvbuf;
};

extern int stream_cache_size;
extern int network_timeout;
#ifdef REAL_NO_INDEX_SEEK
extern int is_skynet;
#endif

static void dump_buf(off_t ofs, unsigned char *buf, int size)
{
#if 1
	int i;
	for(i=0; i<size; i++){
		if((ofs + i) % 16 == 0)
			printf("\n%0llx: ", ofs + i);
		printf("%02x ", *(buf + i));
	}
	printf("\n");
#else
	static int cnt = 0;
	int dumpfd;
	char dumpfn[128];

	sprintf(dumpfn, "/mnt/usb81/dump/buf-%03d-%llx", ++cnt, ofs);
	printf("ofs %llx fn %s\n", ofs, dumpfn);
	dumpfd = open(dumpfn, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(dumpfd >= 0 && size > 0){
	      write(dumpfd, buf, size);
	      close(dumpfd);
	}
#endif
}

static int skynet_send_command(stream_t *stream, char *sendbuf, int cmdsize, char *recvbuf, int reqsize, int chk_ok)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)sendbuf;
	uint32_t *recvval = (uint32_t *)recvbuf;

	printf("%s: send command %x\n", __func__, sendval[0]);

	ret = send(stream->fd, sendbuf, cmdsize, 0);
	if(ret == -1){
		printf("%s: send command fail %s\n", __func__, strerror(errno));
		return -1;
	}else if(ret != cmdsize){
		printf("%s: send command size mismatch, expect:%d actual:%d\n", __func__, cmdsize, ret);
		return -1;
	}

	if(reqsize == 0)
		return 0;
	
	ret = recv(stream->fd, recvbuf, reqsize, 0);
	if(ret == 0){
		printf("%s: connection closed by peer\n", __func__);
		return -1;
	}else if(ret == -1){
		printf("%s: receive response failed %s\n", __func__, strerror(errno));
		return -1;
	}else if(ret != reqsize){
		printf("%s: receive response length mismatch expect:%d actual:%d\n", __func__, reqsize, ret);
	}

	if(chk_ok && recvval[0] != SKYNET_RESP_OK){
		if(recvval[0] == SKYNET_RESP_ERROR){
			printf("%s: check response: error\n", __func__);
		}else if(recvval[0] == SKYNET_RESP_UNKNOWN){
			printf("%s: check response: unknown\n", __func__);
		}else{
			printf("%s: check response: unknown response %x\n", __func__, recvval[0]);
		}
		skynet_data->state = SKYNET_STAT_UNKNOWN;
		return -1;
	}

	return ret;
}

static int skynet_start_streaming(stream_t *stream)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;

	sendval[0] = SKYNET_CMD_STARTSTREAM;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t), 1);
	if(ret == -1){
		printf("%s: failed to restart streaming\n", __func__);
		return 1;
	}

	skynet_data->state = SKYNET_STAT_STREAMING;

	return 0;
}

static int skynet_stop_streaming(stream_t *stream)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;
	char buf[2048];

#if 0
	while(1){
		//send stop streaming command, receive data, but don't check response because there's still data in buffer.
		//printf("%s: send stop command\n", __func__);
		sendval[0] = SKYNET_CMD_STOPSTREAM;
		ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t), 0);

		//wait until response ok
		if(ret == sizeof(uint32_t) && recvval[0] == SKYNET_RESP_OK){
			//printf("%s: stream stopped\n", __func__);
			break;
		}
		
		// read out all unread data
		while(1){
			ret = recv(stream->fd, buf, 2048, MSG_DONTWAIT);
			if(ret == -1){
				if(errno == EAGAIN)	// no more data available
					break;
				printf("error receiving data %d(%s)\n", errno, strerror(errno));
				return 1;
			}else if(ret == 0){
				printf("connection closed by peer\n");
				return 1;
			}
		}
	}
#else
	//send stop streaming command, receive data, but don't check response because there's still data in buffer.
	//printf("%s: send stop command\n", __func__);
	sendval[0] = SKYNET_CMD_STOPSTREAM;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), NULL, 0, 0);
	while(1){
		ret = recv(stream->fd, buf, 2048, 0);
		//dump_buf(0, buf, ret);
		if(ret >= sizeof(uint32_t)){
			if(*(uint32_t *)(&buf[ret - sizeof(uint32_t)]) == SKYNET_RESP_OK)
				break;
		}else if(ret == -1){
			if(errno == EAGAIN)	// no more data available
				break;
			printf("error receiving data %d(%s)\n", errno, strerror(errno));
			return 1;
		}else if(ret == 0){
			printf("connection closed by peer\n");
			return 1;
		}
	}
#endif
	skynet_data->state = SKYNET_STAT_READY;
	
	return 0;
}

static int skynet_set_position(stream_t *stream, off_t newpos)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;

	sendval[0] = SKYNET_CMD_SETPOSITION;
	sendval[1] = newpos & 0xFFFFFFFF;
	sendval[2] = (newpos >> 32) & 0xFFFFFFFF;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t)*3, recvval, sizeof(uint32_t), 1);
	if(ret == -1){
		printf("%s: failed to set position %llx\n", __func__, newpos);
		return 1;
	}

	printf("%s: set position %llx ok\n", __func__, newpos);
	return 0;
}

static int skynet_seek_to_time(stream_t *stream, double newtime, off_t *newpos)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;
	uint64_t t = newtime;

	sendval[0] = SKYNET_CMD_SEEKTOTIME;
	sendval[1] = t & 0xFFFFFFFF;
	sendval[2] = (t >> 32) & 0xFFFFFFFF;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t)*3, recvval, sizeof(uint32_t)*3, 1);
	if(ret == -1){
		printf("%s: failed to seel to time %f\n", __func__, newtime);
		return 1;
	}

	*newpos = ((off_t)recvval[2] << 32) | recvval[1];
	printf("%s: seek to time %f ok. %lld => %llx\n", __func__, newtime, t, *newpos);
	return 0;
}

static int skynet_control(stream_t *stream, int cmd, void* arg)
{
	int ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;
	off_t newpos = 0;

	switch(cmd){
	case STREAM_CTRL_SEEK_TO_TIME:
		// only PPStream FLV supports seek to time function
		if(skynet_data->streamtype != SKYNET_STREAM_FLV)
			break;

		if(skynet_stop_streaming(stream) != 0)
			return 0;

		if(skynet_seek_to_time(stream, *((double *)arg), &newpos) != 0)
			return 0;
#if 1
		ret = skynet_start_streaming(stream);
		if(ret != 0){
			return 0;
		}
#endif
		stream->pos = newpos;
		stream->buf_pos = stream->buf_len = 0;
		return 1;
	default:
		break;
	}
	return -1;
}

static int skynet_seek(stream_t *stream, off_t newpos)
{
	int ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
	uint32_t *sendval = (uint32_t *)skynet_data->sendbuf;
	uint32_t *recvval = (uint32_t *)skynet_data->recvbuf;

	if(skynet_stop_streaming(stream) != 0)
		return 0;
	
	if(skynet_set_position(stream, newpos) != 0)
		return 0;
#if 1
	ret = skynet_start_streaming(stream);
	if(ret != 0){
		return 0;
	}
#endif
	stream->pos = newpos;
	stream->buf_pos = stream->buf_len = 0;
	return 1;
}

static int skynet_fill_buffer(stream_t *stream, char* buffer, int max_len)
{
	ssize_t ret;
	struct skynet_data_t *skynet_data = (struct skynet_data_t *)stream->priv;
#if 0
	if(stream->eof == 1){
		printf("%s: already end of file, start next video\n", __func__);
		if(skynet_start_next_video(stream) != 0){
			return 0;
		}
	}
#endif
	if(skynet_data->state == SKYNET_STAT_READY){
		printf("%s: warning! streaming not started\n", __func__);
		if(skynet_start_streaming(stream) != 0){
			return 0;
		}
	}
	if(skynet_data->state != SKYNET_STAT_STREAMING){
		printf("%s: streaming not started\n", __func__);
		return 0;
	}
	
	if(stream->end_pos > 0 && stream->pos >= stream->end_pos){
		printf("%s: reach end of file, stop streaming... pos %llx\n", __func__, stream->pos);
		skynet_stop_streaming(stream);
		return 0;
	}

	ret = recv(stream->fd, buffer, max_len, 0);
	if(ret == 0){
		printf("%s: connection closed by peer\n", __func__);
		return 0;
	}else if(ret == -1){
		printf("%s: receive response failed %s\n", __func__, strerror(errno));
		return 0;
	}

	//printf("%s: receive length %d\n", __func__, ret);
	//dump_buf(stream->pos, buffer, 10240);
	return ret;
}

static void skynet_stream_cache(stream_t *stream)
{
	if(stream_cache_size<0){
		// cache option not set, will use our computed value.
		// buffer in KBytes, *5 because the prefill is 20% of the buffer.
		//stream_cache_size = (stream->streaming_ctrl->prebuffer_size/1024)*5;
		//printf("prebuffer_size %d, stream_cache_size %d\n", stream->streaming_ctrl->prebuffer_
		//if( stream_cache_size<64 ) stream_cache_size = 64;        // 16KBytes min buffer
		stream_cache_size = SKYNET_CACHE_SIZE_MIN;
	}
	printf("%s: set cache size to %d kbytes\n", __func__, stream_cache_size);
}

static int skynet_connect(stream_t *stream)
{
	int max_retry = 5;
	int i, ret, addr_len;
	int sockfd;
#ifdef USE_UDC
	struct sockaddr_un serv_addr;
#else
	struct sockaddr_in serv_addr;
#endif
	struct timeval to = {
		.tv_sec = network_timeout,
		.tv_usec = 0
	};

	for(i=0; i<max_retry; i++){
#ifdef USE_UDC
		sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
#else
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
		if(sockfd != -1) break;
		usleep(5000);
	}

	if(sockfd == -1){
		printf("%s - Cannot create socket: %s\n", __func__, strerror(errno));
		return 1;
	}	

#ifdef USE_UDC
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, SOCK_NAME);
	addr_len = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);
	serv_addr.sun_path[0] = 0;   //Use abstract name as domain socket
#else
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST);
	serv_addr.sin_port = htons(SERV_PORT);
	addr_len = sizeof(serv_addr);
#endif

	//fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK );
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));

	for(i=0; i<max_retry; i++){
		ret = connect(sockfd, (struct sockaddr *)&serv_addr, addr_len);
		if(ret != -1) break;
		usleep(5000);
	}

	if(ret == -1){
		printf("%s - Cannot connect socket: %s\n", __func__, strerror(errno));
		close(sockfd);
		return 1;
	}

	stream->fd = sockfd;

	return 0;
}

static int skynet_init(stream_t *stream, unsigned char* filename)
{
	int ret;
	uint32_t *sendval, *recvval;
	struct skynet_data_t *skynet_data = malloc(sizeof(struct skynet_data_t));
	
	stream->priv = skynet_data;

	if(skynet_data == NULL){
		printf("%s: failed to allocate memory for private data\n", __func__);
		return -1;
	}
	
	skynet_data->sendbuf = malloc(SKYNET_CMD_LEN);
	if(skynet_data->sendbuf == NULL){
		printf("%s: failed to allocate memory for send buffer\n", __func__);
		free(skynet_data);
		return -1;
	}

	skynet_data->recvbuf = malloc(SKYNET_RESP_LEN);
	if(skynet_data->recvbuf == NULL){
		printf("%s: failed to allocate memory for recv buffer\n", __func__);
		free(skynet_data->sendbuf);
		free(skynet_data);
		return -1;
	}

	sendval = (uint32_t *)skynet_data->sendbuf;
	recvval = (uint32_t *)skynet_data->recvbuf;

	skynet_data->state = SKYNET_STAT_UNKNOWN;
	skynet_data->streamtype = -1;

	// Init
	sendval[0] = SKYNET_CMD_INIT;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t), 1);
	if(ret == -1){
		printf("%s: failed to init\n", __func__);
		return -1;
	}

	//printf("%s: init ok\n", __func__);
	skynet_data->state = SKYNET_STAT_INIT;

	// Request url
	if(filename){
		int fnlen = strlen(filename);
		
		//printf("%s: filename %s len %d\n", __func__, filename, fnlen);
		if(fnlen + 1 > SKYNET_CMD_LEN - sizeof(uint32_t)){
			printf("%s: Warning: filename exceeds command buffer %d > %d\n", __func__, fnlen, SKYNET_CMD_LEN - sizeof(uint32_t));
			return -1;
		}
		
		sendval[0] = SKYNET_CMD_SETURL;
		ret = snprintf(&sendval[1], SKYNET_CMD_LEN - sizeof(uint32_t), filename);
		//printf("%s: copy length max:%d actual:%d '%s'\n", __func__, SKYNET_CMD_LEN - sizeof(uint32_t), ret, &sendval[1]);
		skynet_data->sendbuf[SKYNET_CMD_LEN - 1] = '\0';
		ret = skynet_send_command(stream, sendval, SKYNET_CMD_LEN, recvval, sizeof(uint32_t), 1);
		if(ret == -1){
			printf("%s: failed to set url %s\n", __func__, filename);
			return -1;
		}
		printf("%s: set url ok %s\n", __func__, filename);
	}

	// Get stream length
	sendval[0] = SKYNET_CMD_GETSIZE;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t)*3, 1);
	if(ret == -1){
		printf("%s: failed to get size\n", __func__);
		return -1;
	}
	stream->end_pos = ((off_t)recvval[2] << 32) | recvval[1];
	printf("%s: get size ok %llx\n", __func__, stream->end_pos);

	// Check seekable
	sendval[0] = SKYNET_CMD_GETSEEKABLE;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t)*2, 1);
	if(ret == -1){
		printf("%s: failed to check seekable\n", __func__);
		return -1;
	}
	skynet_data->seekable = recvval[1];
	printf("%s: this stream is %sseekable\n", __func__, skynet_data->seekable ? "" : "not ");

	// Get stream type
	sendval[0] = SKYNET_CMD_GETSTREAMTYPE;
	ret = skynet_send_command(stream, sendval, sizeof(uint32_t), recvval, sizeof(uint32_t)*2, 1);
	if(ret == -1){
		printf("%s: server doesn't support GETSTREAMTYPE command\n", __func__);
	}else{
		skynet_data->streamtype = recvval[1];
	}

	skynet_data->state = SKYNET_STAT_READY;

	return 0;
}

static void skynet_close(stream_t *stream)
{
	if(stream->priv) free(stream->priv);
}

static int skynet_open(stream_t *stream, int mode, void *opts, int *file_format)
{
	int ret;
	int seekable = 0;
	unsigned char *filename;
	struct stream_priv_s* p = (struct stream_priv_s*)opts;
	struct skynet_data_t *skynet_data = NULL;
	//URL_t *url = url_new(stream->url);

	if(p->hostname)
		filename = p->hostname;
	else if(p->filename)
		filename = p->filename;
	else
		filename = NULL;

	//printf("%s: open file name %s\n", __func__, filename);
	ret = skynet_connect(stream);
	if(ret != 0){
		return STREAM_ERROR;
	}

	ret = skynet_init(stream, filename);
	if(ret != 0){
		return STREAM_ERROR;
	}
	
	skynet_data = (struct skynet_data_t *)stream->priv;
	if(skynet_data->seekable){
		stream->flags |= MP_STREAM_SEEK;
		stream->seek = skynet_seek;
	}
	stream->control = skynet_control;
	stream->type = STREAMTYPE_STREAM;
	stream->fill_buffer = skynet_fill_buffer;
	stream->close = skynet_close;
#ifdef REAL_NO_INDEX_SEEK
	is_skynet = 1;
#endif
#if 0
	// start streaming here may detect wrong demuxer ex. real => lavf, delay until skynet_seek()
	// start streaming
	ret = skynet_start_streaming(stream);
	if(ret != 0){
		return STREAM_ERROR;
	}
#endif
	//skynet_stream_cache(stream);

	return STREAM_OK;
}

const stream_info_t stream_info_skynet =
{
	"Skyviia network streaming",
	"skynet",
	"Charles Lin",
	"provide an interface to playback network bitstreams",
	skynet_open,
	{ "skynet", "pps", NULL },
	&stream_opts,
	1 // Urls are an option string
};

